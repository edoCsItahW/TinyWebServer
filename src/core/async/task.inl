// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file task.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/17 17:25
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#ifndef TINY_WEB_SERVER_TASK_INL
#define TINY_WEB_SERVER_TASK_INL
#pragma once

#include <functional>

namespace tiny_web_server::async {

    template<typename T>
    Task<T> Task<T>::PromiseType::getReturnObject() noexcept {
        return Task{std::coroutine_handle<PromiseType>::from_promise(*this)};
    }

    template<typename T>
    std::suspend_always Task<T>::PromiseType::initialSuspend() noexcept {
        return {};
    }

    template<typename T>
    std::suspend_always Task<T>::PromiseType::finalSuspend() noexcept {
        return {};
    }

    template<typename T>
    void Task<T>::PromiseType::returnValue(T vale) noexcept {
        value_ = std::move(vale);
    }

    template<typename T>
    void Task<T>::PromiseType::unhandledException() noexcept {
        std::terminate();
    }

    template<typename T>
    T Task<T>::PromiseType::result() {
        if (exception_)
            std::rethrow_exception(exception_);

        return std::move(value_);
    }

    template<typename T>
    Task<T>::Task(std::coroutine_handle<PromiseType> handle) noexcept : coroutine(handle) {}

    template<typename T>
    Task<T>::Task(Task &&other) noexcept : coroutine(std::exchange(other.coroutine, nullptr)) {}

    template<typename T>
    Task<T> &Task<T>::operator=(Task &&other) noexcept {
        if (this != &other) {
            if (coroutine)
                coroutine.destroy();

            coroutine = std::exchange(other.coroutine, nullptr);
        }

        return *this;
    }

    template<typename T>
    Task<T>::~Task() {
        if (coroutine)
            coroutine.destroy();
    }

    template<typename T>
    void Task<T>::resume() const {
        if (coroutine && !coroutine.done())
            coroutine.resume();
    }

    template<typename T>
    Task<T>::operator bool() const noexcept {
        return coroutine && !coroutine.done();
    }

} // namespace tiny_web_server::async

#endif // TINY_WEB_SERVER_TASK_INL
