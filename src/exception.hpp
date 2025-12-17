// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file exception.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/17 17:10
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#ifndef TINY_WEB_SERVER_EXCEPTION_HPP
#define TINY_WEB_SERVER_EXCEPTION_HPP
#pragma once

#include <system_error>

namespace tiny_web_server::exception {

    class SocketError : public std::system_error {
    public:
        SocketError(int errorCode, std::string_view operation);

        explicit SocketError(std::string_view message);
    };

    namespace detail {

        inline void checkError(int result, std::string_view operation);

    }

} // namespace tiny_web_server::exception

#endif // TINY_WEB_SERVER_EXCEPTION_HPP
