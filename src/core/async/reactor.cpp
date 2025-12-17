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

    Reactor::Reactor(ReactorConfig config) : config_(std::move(config)) { initialize(); }

    Reactor::~Reactor() { stop(); }

    Reactor::Reactor(Reactor &&other) noexcept :
        config_(std::move(other.config_)), stopSource_(std::move(other.stopSource_)) {
#if WEB_SERVER_WINDOWS
        iocp_ = std::move(other.iocp_);

#else
        uring_ = std::move(other.uring_);

#endif

        operations_ = std::move(other.operations_);
    }

    Reactor &Reactor::operator=(Reactor &&other) noexcept {
        if (this != &other) {
            stop();

            config_ = std::move(other.config_);

            stopSource_ = std::move(other.stopSource_);

#if WEB_SERVER_WINDOWS
            iocp_ = std::move(other.iocp_);

#else
            uring_ = std::move(other.uring_);

#endif
            operations_ = std::move(other.operations_);
        }

        return *this;
    }

    void Reactor::unregisterSocket(socket_t socket) noexcept {
        handlers_.erase(socket);

#if WEB_SERVER_WINDOWS
        CancelIoEx(reinterpret_cast<HANDLE>(socket), nullptr);

#endif
    }

    void Reactor::run() {
        running_.store(true, std::memory_order_release);

        workers_.reserve(config_.thread_count);

        for (std::size_t i{0}; i < config_.thread_count; ++i)
            workers_.emplace_back([this, stopToken = stopSource_.get_token()] { eventLoop(stopToken); });
    }

    void Reactor::stop() noexcept {
        if (!running_.exchange(false, std::memory_order_acq_rel))
            return;

        stopSource_.request_stop();

#if WEB_SERVER_WINDOWS
        for (std::size_t i{0}; i < workers_.size(); ++i)
            iocp_.postQueuedCompletionStatus(0, 0, nullptr);
#endif

        for (auto &worker: workers_)
            if (worker.joinable())
                worker.join();

        workers_.clear();
        operations_.clear();
        handlers_.clear();
    }

    bool Reactor::isRunning() const noexcept { return running_.load(std::memory_order_acquire); }

    void Reactor::initialize() {
#if WEB_SERVER_WINDOWS
        iocp_ = detail::IoCompletionPort(config_.thread_count);

#else
        unsigned flags = config_.usePolling ? IORING_SETUP_SQPOLL : 0;

        uring_ = detail::IoUring(config_.io_queue_depth, flags);

#endif
    }

    void Reactor::eventLoop(std::stop_token stopToken) {
        while (!stopToken.stop_requested() && running_.load(std::memory_order_acquire))
#if WEB_SERVER_WINDOWS
            processIocpEvents();

#else
            processUringEvents();

#endif
    }

#if WEB_SERVER_WINDOWS
    void Reactor::submitIocpRead(socket_t socket, std::span<std::byte> buffer, std::uintptr_t key) {
        auto *operation = reinterpret_cast<detail::IoOperation *>(key);

        WSABUF wsaBuf{.len = static_cast<ULONG>(buffer.size()), .buf = reinterpret_cast<char *>(buffer.data())};

        DWORD flags = 0;
        DWORD bytesReceived = 0;

        if (WSARecv(socket, &wsaBuf, 1, &bytesReceived, &flags, reinterpret_cast<LPOVERLAPPED>(operation), nullptr) ==
            SOCKET_ERROR)
            if (auto error = WSAGetLastError(); error != WSA_IO_PENDING) {
                operations_.erase(key);

                throw net::SocketError(error, "WSARecv failed");
            }
    }

    void Reactor::submitIocpWrite(socket_t socket, std::span<const std::byte> data, std::uintptr_t key) {
        auto *operation = reinterpret_cast<detail::IoOperation *>(key);

        WSABUF wsaBuf{.len = static_cast<ULONG>(data.size()),
                      .buf = const_cast<char *>(reinterpret_cast<const char *>(data.data()))};

        DWORD bytesSent = 0;

        if (WSASend(socket, &wsaBuf, 1, &bytesSent, 0, reinterpret_cast<LPOVERLAPPED>(operation), nullptr) ==
            SOCKET_ERROR)
            if (auto error = WSAGetLastError(); error != WSA_IO_PENDING) {
                operations_.erase(key);

                throw net::SocketError(error, "WSASend failed");
            }
    }

    void Reactor::processIocpEvents() {
        DWORD bytesTransferred = 0;
        ULONG_PTR completionKey = 0;
        LPOVERLAPPED overlapped = nullptr;

        if (iocp_.getQueuedCompletionStatus(&bytesTransferred, &completionKey, &overlapped,
                                            static_cast<DWORD>(config_.timeout.count())))
            if (overlapped) {
                auto *operation = reinterpret_cast<detail::IoOperation *>(overlapped);

                handleCompletion(operation, bytesTransferred);
            }
    }

#else
    void Reactor::registerLinuxEvents(socket_t socket, EventType events, auto &&handler) {

    }

    void Reactor::submitUringRead(socket_t socket, std::span<std::byte> buffer, std::uintptr_t key) {
        auto* sqe = uring_.getSqe();

        if (!sqe) {
            uring_.submit();
            sqe = uring_.getSqe();
        }

        auto* operation = reinterpret_cast<detail::IoOperation*>(key);

        io_uring_prep_read(sqe, socket, buffer.data(), buffer.size(), 0);
        io_uring_sqe_set_data(sqe, operation);
    }

    void Reactor::submitUringWrite(socket_t socket, std::span<const std::byte> data, std::uintptr_t key) {
        auto* sqe = uring_.getSqe();

        if (!sqe) {
            uring_.submit();
            sqe = uring_.getSqe();
        }

        auto* operation = reinterpret_cast<detail::IoOperation*>(key);
        operation->buffer.assign(data.begin(), data.end());

        io_uring_prep_write(sqe, socket, operation->buffer.data(), data.size(), 0);
        io_uring_sqe_set_data(sqe, operation);
    }

    void Reactor::processUringEvents() {
        io_uring_cqe* cqe = nullptr;

        if (uring_.waitCqeTimeout(&cqe, config_.timeout) && cqe) {
            if (auto *operation = static_cast<detail::IoOperation *>(io_uring_cqe_get_data(cqe))) {
                operation->res = cqe->res;

                handleCompletion(operation, static_cast<std::size_t>(cqe->res > 0 ? cqe->res : 0));
            }

            uring_.cqeSeen(cqe);
        }
    }

#endif

    void Reactor::handleCompletion(detail::IoOperation *operation, std::size_t bytesTransferred) {
        EventData data{
            .data = reinterpret_cast<void *>(operation->socket),
            .bytesTransferred = bytesTransferred,
            .errorCode = 0,
            .eventType = operation->eventType
        };

        if (operation->callback)
            operation->callback(data);

        auto key = reinterpret_cast<std::uintptr_t>(operation);

        operations_.erase(key);
    }

} // namespace tiny_web_server::async
