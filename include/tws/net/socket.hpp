// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for
// commercial purposes is prohibited without the author's permission. If you have any
// questions or require permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file socket.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/18 12:43
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#ifndef TINY_WEB_SERVER_SOCKET_HPP
#define TINY_WEB_SERVER_SOCKET_HPP
#pragma once

#include "../platform.hpp"
#include "endpoint.hpp"
#include "enums.hpp"

namespace tiny_web_server::net {

    struct Socket {
    private:
        socket_t handle_ = NET_INVALID_SOCKET;

        static int count;

        struct Options {
            bool reuse_address      = false;
            bool no_delay           = false;
            bool keep_alive         = false;
            int receive_timeout_ms  = 0;
            int send_timeout_ms     = 0;
            int receive_buffer_size = 0;
            int send_buffer_size    = 0;
        };

    public:
        Socket() = default;

        Socket(AddressFamily family, SocketType type, Protocol protocol = Protocol::TCP);

        ~Socket();

        Socket(Socket &&other) noexcept;
        Socket(const Socket &) = delete;

        Socket &operator=(Socket &&other) noexcept;
        Socket &operator=(const Socket &) = delete;

        void bind(const Endpoint &endpoint) const;

        void listen(int backlog = SOMAXCONN) const;

        [[nodiscard]] Socket accept() const;

        void connect(const Endpoint &endpoint) const;

        [[nodiscard]] std::size_t recv(std::span<std::byte> buffer, int flags = 0) const;

        [[nodiscard]] std::size_t send(std::span<const std::byte> data, int flags = 0) const;

        void setOptions(const Options &opts) const;

        void setNonBlocking(bool nonBlocking = true) const;

        void close();

        [[nodiscard]] bool isValid() const;

        [[nodiscard]] socket_t nativeHandle() const noexcept;

    private:
        Socket(socket_t&& handle);

        static void initialize();

    };

}  // namespace tiny_web_server::net

#endif  // TINY_WEB_SERVER_SOCKET_HPP
