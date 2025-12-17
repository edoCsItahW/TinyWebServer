// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file socket_initializer.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/17 17:13
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#ifndef TINY_WEB_SERVER_SOCKET_INITIALIZER_HPP
#define TINY_WEB_SERVER_SOCKET_INITIALIZER_HPP
#pragma once

namespace tiny_web_server::net {

    /** @class SocketInitializer
     *
     * @if zh
     * @brief 统一网络初始化器
     * @details 网络初始化器，用于统一网络初始化，包括初始化套接字库等。
     *
     * @else
     * @brief Unified network initializer
     * @details Network initializer, used to unify network initialization, including initializing socket library.
     *
     * @endif
     */
    class SocketInitializer {
    public:
        SocketInitializer();

        ~SocketInitializer();

        /**
         * @if zh
         * 禁止拷贝
         *
         * @else
         * Prohibit copy
         *
         * @endif
         */
        SocketInitializer(const SocketInitializer &) = delete;
        SocketInitializer &operator=(const SocketInitializer &) = delete;
    };

} // namespace tiny_web_server::net

#endif // TINY_WEB_SERVER_SOCKET_INITIALIZER_HPP
