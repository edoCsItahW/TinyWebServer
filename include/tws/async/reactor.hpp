// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for
// commercial purposes is prohibited without the author's permission. If you have any
// questions or require permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file reactor.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/18 21:45
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#ifndef TEST_TINY_WEB_SERVER_REACTOR_HPP
#define TEST_TINY_WEB_SERVER_REACTOR_HPP
#pragma once

#include "../net/socket.hpp"

#include "task.hpp"
#include <memory>

namespace tiny_web_server::async {

    struct Reactor {
    private:
        struct Impl;

        std::unique_ptr<Impl> impl_;

    public:
        Reactor();

        ~Reactor();

        void run();

        void stop();

        Task<net::Socket> asyncAccept(net::Socket& listener);

        Task<std::vector<std::byte>> asyncRecv(net::Socket& socket, std::size_t size);

        Task<std::size_t> asyncSend(net::Socket& socket, std::span<const std::byte> data);

        Task<void> asyncConnect(net::Socket& socket, const net::Endpoint& endpoint);

        friend struct AcceptOperation;
    };

}  // namespace tiny_web_server::async

#endif  // TEST_TINY_WEB_SERVER_REACTOR_HPP
