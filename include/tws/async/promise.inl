// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for
// commercial purposes is prohibited without the author's permission. If you have any
// questions or require permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file promise.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/18 21:19
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#ifndef TEST_TINY_WEB_SERVER_PROMISE_INL
#define TEST_TINY_WEB_SERVER_PROMISE_INL
#pragma once
#include "promise.hpp"

namespace tiny_web_server::async {

    template<typename T>
    auto Promise<T>::initial_suspend() noexcept {
        return std::suspend_always{};
    }

    template<typename T>
    auto Promise<T>::final_suspend() noexcept {
        struct Awaiter {
            [[nodiscard]] bool await_ready() const noexcept { return false; }

            std::coroutine_handle<> await_suspend(
                std::coroutine_handle<Promise> coroutine
            ) noexcept {
                auto& promise = coroutine.promise();

                if (promise.continuation) return promise.continuation;

                return std::noop_coroutine();
            }

            void await_resume() const noexcept {}
        };

        return Awaiter{};
    }

    template<typename T>
    void Promise<T>::unhandled_exception() noexcept {
        result.template emplace<2>(std::current_exception());
    }

    inline auto Promise<void>::initial_suspend() noexcept { return std::suspend_always{}; }

    inline auto Promise<void>::final_suspend() noexcept {
        struct Awaiter {
            [[nodiscard]] bool await_ready() const noexcept { return false; }

            std::coroutine_handle<> await_suspend(
                std::coroutine_handle<Promise> coroutine
            ) noexcept {
                auto& promise = coroutine.promise();

                if (promise.continuation) return promise.continuation;

                return std::noop_coroutine();
            }

            void await_resume() const noexcept {}
        };

        return Awaiter{};
    }

    inline void Promise<void>::unhandled_exception() noexcept {
        result.emplace<1>(std::current_exception());
    }

}  // namespace tiny_web_server::async

#endif  // TEST_TINY_WEB_SERVER_PROMISE_INL
