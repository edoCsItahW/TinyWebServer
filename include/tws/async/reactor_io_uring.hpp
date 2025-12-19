// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for
// commercial purposes is prohibited without the author's permission. If you have any
// questions or require permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file reactor_io_uring.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/18 22:00
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#ifndef TEST_TINY_WEB_SERVER_REACTOR_IO_URING_HPP
#define TEST_TINY_WEB_SERVER_REACTOR_IO_URING_HPP
#pragma once

#include "../platform.hpp"

#if WEB_SERVER_LINUX

    #include "reactor.hpp"
    #include <functional>
    #include <memory_resource>
    #include <mutex>
    #include <queue>
    #include <thread>

namespace tiny_web_server::async {

    struct Reactor::Impl {
        io_uring ring;

        std::atomic<bool> running{true};

        std::thread workerThread;

        struct Operation {
            std::coroutine_handle<> handle;

            int result;

            std::function<void()> cleanup;
        };

        struct OperationQueue {
            std::mutex mutex;
            std::queue<Operation*> operations;
        };

        OperationQueue opQueue;

        std::pmr::synchronized_pool_resource memoryPool;

        Impl();

        ~Impl();

        void runIoUring();

        template<typename OP, typename... Args>
        OP* createOperation(Args&&... args);

        void destroyOperation(Operation* operation);

        void submitAccept(
            int sockfd, sockaddr* addr, socklen_t* addrlen, Operation* operation
        );

        void submitRecv(int sockfd, void* buf, size_t len, Operation* operation);

        void submitSend(int sockfd, const void* buf, size_t len, Operation* operation);

        void submitConnect(
            int sockfd, const sockaddr* addr, socklen_t addrlen, Operation* operation
        );
    };

}  // namespace tiny_web_server::async

#endif  // WEB_SERVER_LINUX

#endif  // TEST_TINY_WEB_SERVER_REACTOR_IO_URING_HPP
