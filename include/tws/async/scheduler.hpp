// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for
// commercial purposes is prohibited without the author's permission. If you have any
// questions or require permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file scheduler.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/18 21:49
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#ifndef TEST_TINY_WEB_SERVER_SCHEDULER_HPP
#define TEST_TINY_WEB_SERVER_SCHEDULER_HPP
#pragma once

#include "task.hpp"
#include <condition_variable>
#include <functional>
#include <queue>

namespace tiny_web_server::async {

    struct Scheduler {
    private:
        struct TaskQueue {
            std::queue<std::function<void()>> tasks;
            std::mutex mutex;
            std::condition_variable cv;
        };

        std::vector<std::thread> workers_;

        std::atomic<bool> running_{false};

        TaskQueue taskQueue_;

    public:
        template<typename Awaitable>
        void spawn(Awaitable&& awaitable);

        void schedule(Task<void> task);

        void run();

        void stop();
    };

}  // namespace tiny_web_server::async

#endif  // TEST_TINY_WEB_SERVER_SCHEDULER_HPP
