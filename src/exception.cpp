// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file exception.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/17 17:10
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#include "exception.hpp"
#include <format>
#include "platform.hpp"

namespace tiny_web_server::exception {

    SocketError::SocketError(int errorCode, std::string_view operation) :
        std::system_error(errorCode, std::system_category(),
                          std::format("SocketError: Network operation '{}' failed", operation)) {}

    SocketError::SocketError(std::string_view message) : std::system_error(0, std::system_category(), message.data()) {}

    namespace detail {

        void checkError(int result, std::string_view operation) {
            if (result < 0)
                throw SocketError(NET_ERROR, operation);
        }

    } // namespace detail

} // namespace tiny_web_server::exception
