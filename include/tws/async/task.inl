// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for
// commercial purposes is prohibited without the author's permission. If you have any
// questions or require permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file task.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/18 21:29
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#ifndef TEST_TINY_WEB_SERVER_TASK_INL
#define TEST_TINY_WEB_SERVER_TASK_INL
#pragma once

#include "task.hpp"

namespace tiny_web_server::async {

    template<typename T>
    Task<T> Task<T>::promise_type::get_return_object() noexcept {
        return {std::coroutine_handle<promise_type>::from_promise(*this)};
    }

    template<typename T>
    void Task<T>::promise_type::return_value(T value) noexcept {
        this->result.template emplace<1>(std::move(value));
    }

    template<typename T>
    bool Task<T>::Awaiter::await_ready() const noexcept {
        return false;
    }

    template<typename T>
    std::coroutine_handle<typename Task<T>::promise_type> Task<T>::Awaiter::await_suspend(
        std::coroutine_handle<> awaiting
    ) noexcept {
        handle.promise().continuation = awaiting;
        return handle;
    }

    template<typename T>
    T Task<T>::Awaiter::await_resume() {
        auto& promise = handle.promise();

        if (promise.result.index() == 2) std::rethrow_exception(std::get<2>(promise.result));

        return std::get<1>(promise.result);
    }

    template<typename T>
    Task<T>::Task(std::coroutine_handle<promise_type> handle) noexcept
        : handle_(handle) {}

    template<typename T>
    Task<T>::~Task() {
        if (handle_) handle_.destroy();
    }

    template<typename T>
    Task<T>::Task(Task&& other) noexcept
        : handle_(std::exchange(other.handle_, nullptr)) {}

    template<typename T>
    Task<T>::Awaiter Task<T>::operator co_await() const noexcept {
        return handle_;
    }

    inline Task<void> Task<void>::promise_type::get_return_object() noexcept {
        return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
    }

    inline void Task<void>::promise_type::return_void() noexcept {
        this->result.emplace<1>();
    }

    inline bool Task<void>::Awaiter::await_ready() const noexcept { return false; }

    inline std::coroutine_handle<Task<void>::promise_type> Task<void>::Awaiter::await_suspend(
        std::coroutine_handle<> awaiting
    ) noexcept {
        handle.promise().continuation = awaiting;
        return handle;
    }

    inline void Task<void>::Awaiter::await_resume() {
        auto& promise = handle.promise();

        if (promise.result.index() == 2) std::rethrow_exception(std::get<2>(promise.result));
    }

    inline Task<void>::Task(std::coroutine_handle<promise_type> handle) noexcept
        : handle_(handle) {}

    inline Task<void>::~Task() {
        if (handle_) handle_.destroy();
    }

    inline Task<void>::Task(Task&& other) noexcept
        : handle_(std::exchange(other.handle_, nullptr)) {}

    inline Task<void>::Awaiter Task<void>::operator co_await() const noexcept {
        return {handle_};
    }

}  // namespace tiny_web_server::async

#endif  // TEST_TINY_WEB_SERVER_TASK_INL
