// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file socket.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/16 19:50
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#ifndef TINY_WEB_SERVER_SOCKET_HPP
#define TINY_WEB_SERVER_SOCKET_HPP
#pragma once

#include <chrono>
#include "endpoint.hpp"
#include "enums.hpp"
#include "socket_handle.hpp"


namespace tiny_web_server::net {

    struct SocketOptions {
        bool reuse_address = false;
        bool keep_alive = false;
        bool no_delay = false;
        int receive_timeout_ms = 0;
        int send_timeout_ms = 0;
        int receive_buffer_size = 0;
        int send_buffer_size = 0;
    };

    class Socket {
    public:
        Socket() = default;

        Socket(AddressFamily family, SocketType type, Protocol proto = Protocol::TCP);

        void bind(const Endpoint &endpoint);

        void listen(int backlog = SOMAXCONN) const;

        [[nodiscard]] Socket accept() const;

        void connect(const Endpoint &endpoint) const;

        std::size_t send(std::span<const std::byte> data, int flags = 0) const;

        std::size_t receive(std::span<std::byte> buffer, int flags = 0) const;

        std::optional<std::size_t> sendWithTimeout(std::span<const std::byte> data,
                                                   std::chrono::milliseconds timeout) const;

        void setOptions(const SocketOptions &opts) const;

        void setNonBlocking(bool nonBlocking = true);

        void close();

        [[nodiscard]] bool isValid() const noexcept;

    private:
        SocketHandle handle_;

        friend class SocketHandle;
    };

} // namespace tiny_web_server::net

namespace std {

    template<>
    struct hash<tiny_web_server::net::IpAddress> {
        std::size_t operator()(const tiny_web_server::net::IpAddress &addr) const noexcept { return addr.hash(); }
    };

} // namespace std

#endif // TINY_WEB_SERVER_SOCKET_HPP
