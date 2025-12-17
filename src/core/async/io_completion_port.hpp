// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file io_completion_port.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/17 17:29
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#ifndef TINY_WEB_SERVER_IO_COMPLETION_PORT_HPP
#define TINY_WEB_SERVER_IO_COMPLETION_PORT_HPP
#pragma once

#include "../net/socket.hpp"


namespace tiny_web_server::async::detail {

#if WEB_SERVER_WINDOWS

    class IoCompletionPort {
    public:
        explicit IoCompletionPort(std::size_t threadCount = 0);

        ~IoCompletionPort();

        [[nodiscard]] HANDLE handle() const noexcept;

        bool associate(SOCKET socket, ULONG_PTR key) const noexcept;

        bool getQueuedCompletionStatus(LPDWORD bytesTransferred, PULONG_PTR completionKey, LPOVERLAPPED *overlapped,
                                       DWORD timeout) const noexcept;

        bool postQueuedCompletionStatus(DWORD bytesTransferred, ULONG_PTR completionKey,
                                        LPOVERLAPPED overlapped) const noexcept;

        IoCompletionPort(IoCompletionPort &&other) noexcept;

        IoCompletionPort &operator=(IoCompletionPort &&other) noexcept;

    private:
        HANDLE iocp_{nullptr};
    };

    struct IoOperation : OVERLAPPED {
        TaskHandler callback;
        socket_t socket{NET_INVALID_SOCKET};
        EventType eventType{EventType::READ};
        std::vector<std::byte> buffer;

        IoOperation() noexcept;

        ~IoOperation() = default;
    };

#endif

} // namespace tiny_web_server::async::detail

#endif // TINY_WEB_SERVER_IO_COMPLETION_PORT_HPP
