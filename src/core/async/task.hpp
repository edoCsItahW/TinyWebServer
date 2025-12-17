// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file task.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/17 17:25
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#ifndef TINY_WEB_SERVER_TASK_HPP
#define TINY_WEB_SERVER_TASK_HPP
#pragma once

#include <coroutine>
#include <exception>

namespace tiny_web_server::async {

    template<typename T = void>
    struct Task {

        struct PromiseType {
            T value_;
            std::exception_ptr exception_;

            Task getReturnObject();

            static std::suspend_always initialSuspend() noexcept;

            static std::suspend_always finalSuspend() noexcept;

            void returnValue(T value) noexcept;

            static void unhandledException() noexcept;

            T result();
        };

        explicit Task(std::coroutine_handle<PromiseType> handle = nullptr) noexcept;

        Task(Task &&other) noexcept;

        Task &operator=(Task &&other) noexcept;

        ~Task();

        void resume() const;

        explicit operator bool() const noexcept;

    private:
        std::coroutine_handle<PromiseType> coroutine;
    };

} // namespace tiny_web_server::async

#include "task.inl"

#endif // TINY_WEB_SERVER_TASK_HPP
