// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file scheduler.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/18 21:49
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#include "tws/async/scheduler.hpp"

namespace tiny_web_server::async {

    void Scheduler::schedule(Task<void> task) {
        std::lock_guard lock(taskQueue_.mutex);

        taskQueue_.tasks.emplace([task = std::move(task)] mutable {
            auto t = std::move(task);
        });

        taskQueue_.cv.notify_one();
    }

    void Scheduler::run() {
        running_ = true;

        auto numWorkers = std::thread::hardware_concurrency();

        for (std::size_t i{0}; i < numWorkers; ++i)
            workers_.emplace_back([this] {

                while (running_) {
                    std::function<void()> task;

                    {
                        std::unique_lock lock(taskQueue_.mutex);

                        taskQueue_.cv.wait(lock, [this] {
                            return !taskQueue_.tasks.empty() || !running_;
                        });

                        if (!running_ && taskQueue_.tasks.empty())
                            return;

                        task = std::move(taskQueue_.tasks.front());
                        taskQueue_.tasks.pop();
                    }

                    task();
                }

            });
    }

    void Scheduler::stop() {
        running_ = false;

        taskQueue_.cv.notify_all();

        for (auto& worker : workers_)
            if (worker.joinable())
                worker.join();
    }

}  // namespace tiny_web_server::async
