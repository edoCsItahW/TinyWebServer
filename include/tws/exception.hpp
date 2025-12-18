// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for
// commercial purposes is prohibited without the author's permission. If you have any
// questions or require permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file exception.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/18 13:05
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#ifndef TINY_WEB_SERVER_EXCEPTION_HPP
#define TINY_WEB_SERVER_EXCEPTION_HPP
#pragma once

#include "tws/utils/fstr.h"
#include <format>
#include <system_error>

namespace tiny_web_server {

    template<auto...>
    struct SocketError;

    template<>
    struct SocketError<> : std::system_error {
        SocketError(int errc, std::string_view operation)
            : std::system_error(
                  errc, std::system_category(),
                  std::format("SocketError: Socket operation '{}' failed", operation)
              ) {}

        explicit SocketError(std::string_view message)
            : std::system_error(
                  0, std::system_category(), std::format("SocketError: {}", message)
              ) {}
    };

    template<int C, FStrChar O>
    struct SocketError<C, O> : std::system_error {
        SocketError()
            : std::system_error(
                  C, std::system_category(),
                  std::format("SocketError: Socket operation '{}' failed", O)
              ) {}
    };

    template<FStrChar M>
    struct SocketError<M> : std::system_error {
        SocketError()
            : std::system_error(
                  0, std::system_category(),
                  std::format("SocketError: {}", std::string{M.data.data()})
              ) {}
    };

}  // namespace tiny_web_server

#endif  // TINY_WEB_SERVER_EXCEPTION_HPP
