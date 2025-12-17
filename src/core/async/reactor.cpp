// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file reactor.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/16 22:25
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#include "reactor.hpp"

#if WEB_SERVER_LINUX

#include <utility>

#endif


namespace tiny_web_server::async {

    // EventData

    EventData::operator bool() const noexcept { return bytesTransferred > 0 || eventType == EventType::DISCONNECT; }

    // Task

    Task Task::PromiseType::getReturnObject() noexcept {
        return Task{std::coroutine_handle<PromiseType>::from_promise(*this)};
    }

    std::suspend_always Task::PromiseType::initialSuspend() noexcept { return {}; }

    std::suspend_always Task::PromiseType::finalSuspend() noexcept { return {}; }

    void Task::PromiseType::returnVoid() noexcept {}

    void Task::PromiseType::unhandledException() noexcept { std::terminate(); }

    Task::Task(std::coroutine_handle<PromiseType> handle) noexcept : coroutine(handle) {}

    Task::Task(Task &&other) noexcept : coroutine(std::exchange(other.coroutine, nullptr)) {}

    Task &Task::operator=(Task &&other) noexcept {
        if (this != &other) {
            if (coroutine)
                coroutine.destroy();

            coroutine = std::exchange(other.coroutine, nullptr);
        }

        return *this;
    }

    Task::~Task() {
        if (coroutine)
            coroutine.destroy();
    }

    void Task::resume() const {
        if (coroutine && !coroutine.done())
            coroutine.resume();
    }

    Task::operator bool() const noexcept { return coroutine && !coroutine.done(); }

    namespace detail {

#if WEB_SERVER_WINDOWS

        IoCompletionPort::IoCompletionPort(std::size_t threadCount) {
            iocp_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, static_cast<DWORD>(threadCount));

            if (!iocp_)
                throw net::SocketError(GetLastError(), "CreateIoCompletionPort failed");
        }

        IoCompletionPort::~IoCompletionPort() {
            if (iocp_)
                CloseHandle(iocp_);
        }

        HANDLE IoCompletionPort::handle() const noexcept { return iocp_; }

        bool IoCompletionPort::associate(SOCKET socket, ULONG_PTR key) const noexcept {
            return CreateIoCompletionPort(reinterpret_cast<HANDLE>(socket), iocp_, key, 0) != nullptr;
        }

        bool IoCompletionPort::getQueuedCompletionStatus(LPDWORD bytesTransferred, PULONG_PTR completionKey,
                                                         LPOVERLAPPED *overlapped, DWORD timeout) const noexcept {
            return GetQueuedCompletionStatus(iocp_, bytesTransferred, completionKey, overlapped, timeout);
        }

        bool IoCompletionPort::postQueuedCompletionStatus(DWORD bytesTransferred, ULONG_PTR completionKey,
                                                          LPOVERLAPPED overlapped) const noexcept {
            return PostQueuedCompletionStatus(iocp_, bytesTransferred, completionKey, overlapped);
        }

        IoCompletionPort::IoCompletionPort(IoCompletionPort &&other) noexcept :
            iocp_(std::exchange(other.iocp_, nullptr)) {}

        IoCompletionPort &IoCompletionPort::operator=(IoCompletionPort &&other) noexcept {
            if (this != &other) {
                if (iocp_)
                    CloseHandle(iocp_);

                iocp_ = std::exchange(other.iocp_, nullptr);
            }

            return *this;
        }

        IoOperation::IoOperation() noexcept { std::memset(this, 0, sizeof(OVERLAPPED)); }

#else

        IoUring::IoUring(std::size_t queueDepth, unsigned int flags) {
            if (io_uring_queue_init(static_cast<unsigned>(queueDepth), &ring_, flags) < 0)
                throw net::SocketError(errno, "io_uring_queue_init failed");
        }

        IoUring::~IoUring() { io_uring_queue_exit(&ring_); }

        io_uring *IoUring::get() noexcept { return &ring_; }

        const io_uring *IoUring::get() const noexcept { return &ring_; }

        io_uring_sqe *IoUring::getSqe() noexcept { return io_uring_get_sqe(&ring_); }

        void IoUring::submit() {
            if (io_uring_submit(&ring_) < 0)
                throw net::SocketError(errno, "io_uring_submit failed");
        }

        bool IoUring::waitCqeTimeout(io_uring_cqe **cqe, std::chrono::nanoseconds timeout) {
            __kernel_timespec ts{.tv_sec = timeout.count() / 1000, .tv_nsec = timeout.count() % 1000 * 1000000};

            return io_uring_wait_cqe_timeout(&ring_, cqe, &ts) == 0;
        }

        io_uring_cqe *IoUring::peekCqe() noexcept {
            io_uring_cqe *cqe = nullptr;

            io_uring_peek_cqe(&ring_, &cqe);

            return cqe;
        }

        void IoUring::cqeSeen(io_uring_cqe *cqe) noexcept { io_uring_cqe_seen(&ring_, cqe); }

        IoUring::IoUring(IoUring &&other) noexcept : ring_(std::exchange(other.ring_, {})) {}

        IoUring &IoUring::operator=(IoUring &&other) noexcept {
            if (this != &other) {
                io_uring_queue_exit(&ring_);

                ring_ = std::exchange(other.ring_, {});
            }

            return *this;
        }

#endif

    } // namespace detail

} // namespace tiny_web_server::async
