// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for
// commercial purposes is prohibited without the author's permission. If you have any
// questions or require permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file promise.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/18 21:16
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#ifndef TEST_TINY_WEB_SERVER_PROMISE_HPP
#define TEST_TINY_WEB_SERVER_PROMISE_HPP
#pragma once

#include <coroutine>
#include <variant>

namespace tiny_web_server::async {

    template<typename T>
    struct Task;

    template<typename T>
    struct Promise {
        virtual ~Promise() = default;

        std::coroutine_handle<T> continuation;
        std::variant<std::monostate, T, std::exception_ptr> result;

        auto initial_suspend() noexcept;

        auto final_suspend() noexcept;

        void unhandled_exception() noexcept;

        virtual Task<T> get_return_object() noexcept = 0;
    };

    template<>
    struct Promise<void> {
        std::coroutine_handle<> continuation;
        std::variant<std::monostate, std::exception_ptr> result;

        auto initial_suspend() noexcept;

        auto final_suspend() noexcept;

        void unhandled_exception() noexcept;
    };

}  // namespace tiny_web_server::async

// #include "promise.inl"

#endif  // TEST_TINY_WEB_SERVER_PROMISE_HPP
