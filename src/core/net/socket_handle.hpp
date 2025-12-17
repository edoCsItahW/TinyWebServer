// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file socket_handle.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/17 17:07
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#ifndef TINY_WEB_SERVER_SOCKET_HANDLE_HPP
#define TINY_WEB_SERVER_SOCKET_HANDLE_HPP
#pragma once

#include "enums.hpp"

namespace tiny_web_server::net {

    class SocketHandle {
    public:
        SocketHandle() noexcept = default;

        explicit SocketHandle(AddressFamily family, SocketType type, Protocol proto = Protocol::TCP);

        ~SocketHandle();

        SocketHandle(SocketHandle &&other) noexcept;

        SocketHandle &operator=(SocketHandle &&other) noexcept;

        SocketHandle(const SocketHandle &) = delete;

        SocketHandle &operator=(const SocketHandle &) = delete;

        [[nodiscard]] bool isValid() const noexcept;

        [[nodiscard]] socket_t nativeHandle() const noexcept;

        void reset() noexcept;

        void swap(SocketHandle &other) noexcept;

    private:
        static constexpr socket_t invalidSocket = NET_INVALID_SOCKET;

        socket_t handle_ = invalidSocket;
    };

} // namespace tiny_web_server::net

#endif // TINY_WEB_SERVER_SOCKET_HANDLE_HPP
