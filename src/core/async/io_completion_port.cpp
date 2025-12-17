// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file io_completion_port.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/17 17:29
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#include "io_completion_port.hpp"
#include "../../exception.hpp"

namespace tiny_web_server::async::detail {

#if WEB_SERVER_WINDOWS

    IoCompletionPort::IoCompletionPort(std::size_t threadCount) {
        iocp_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, static_cast<DWORD>(threadCount));

        if (!iocp_)
            throw exception::SocketError(GetLastError(), "CreateIoCompletionPort failed");
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

#endif

} // namespace tiny_web_server::async::detail
