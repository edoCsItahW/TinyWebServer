// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for
// commercial purposes is prohibited without the author's permission. If you have any
// questions or require permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file reactor_iocp.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/18 21:53
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#ifndef TEST_TINY_WEB_SERVER_REACTOR_IOCP_HPP
#define TEST_TINY_WEB_SERVER_REACTOR_IOCP_HPP
#pragma once

#include "../platform.hpp"

#if WEB_SERVER_WINDOWS

    #include "reactor.hpp"
    #include <functional>
    #include <memory_resource>
    #include <thread>

namespace tiny_web_server::async {

    struct Reactor::Impl {
        HANDLE iocpHandle;

        std::atomic<bool> running{true};

        std::thread workerThread;

        struct Operation {
            OVERLAPPED overlapped;
            std::coroutine_handle<> handle;
            DWORD bytesTransferred;
            std::function<void()> cleanup;
        };

        std::pmr::synchronized_pool_resource memoryPool;

        Impl();

        ~Impl();

        void runIOCP();

        template<typename OP, typename... Args>
        Operation* createOperation(Args&&... args);

        void destroyOperation(Operation* operation);
    };

    struct AcceptOperation : Reactor::Impl::Operation {

        net::Socket* listener;

        net::Socket* acceptedSocket;

        sockaddr_in clientAddress;

        int addrLen = sizeof(clientAddress);

        AcceptOperation(net::Socket* listener, net::Socket* acceptedSocket);

    };

}  // namespace tiny_web_server::async

#endif  // WEB_SERVER_WINDOWS

#endif  // TEST_TINY_WEB_SERVER_REACTOR_IOCP_HPP
