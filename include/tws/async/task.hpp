// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for
// commercial purposes is prohibited without the author's permission. If you have any
// questions or require permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file task.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/18 21:23
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#ifndef TEST_TINY_WEB_SERVER_TASK_HPP
#define TEST_TINY_WEB_SERVER_TASK_HPP
#pragma once
#include "promise.hpp"

namespace tiny_web_server::async {

    template<typename T>
    struct Task {
        struct promise_type;

    private:
        std::coroutine_handle<promise_type> handle_;

    public:
        struct Awaiter;

        explicit Task(std::coroutine_handle<promise_type> handle) noexcept;

        ~Task();

        Task(Task&& other) noexcept;

        Awaiter operator co_await() const noexcept;
    };

    template<typename T>
    struct Task<T>::promise_type : Promise<T> {
        Task get_return_object() noexcept;

        void return_value(T value) noexcept;
    };

    template<typename T>
    struct Task<T>::Awaiter {
        std::coroutine_handle<promise_type> handle;

        bool await_ready() const noexcept;

        std::coroutine_handle<promise_type> await_suspend(
            std::coroutine_handle<> awaiting
        ) noexcept;

        T await_resume();
    };

    template<>
    struct Task<void> {
        struct promise_type;

    private:
        std::coroutine_handle<promise_type> handle_;

    public:
        struct Awaiter;

        explicit Task(std::coroutine_handle<promise_type> handle) noexcept;

        ~Task();

        Task(Task&& other) noexcept;

        Awaiter operator co_await() const noexcept;
    };

    struct Task<void>::promise_type : Promise<void> {
        Task get_return_object() noexcept;

        void return_void() noexcept;
    };

    struct Task<void>::Awaiter {
        std::coroutine_handle<promise_type> handle;

        [[nodiscard]] bool await_ready() const noexcept;

        std::coroutine_handle<promise_type> await_suspend(
            std::coroutine_handle<> awaiting
        ) noexcept;

        void await_resume();
    };

}  // namespace tiny_web_server::async

#endif  // TEST_TINY_WEB_SERVER_TASK_HPP
