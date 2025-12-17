// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file enums.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/17 17:39
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#ifndef TINY_WEB_SERVER_ASYNC_ENUMS_HPP
#define TINY_WEB_SERVER_ASYNC_ENUMS_HPP
#pragma once

#include <cstdint>

namespace tiny_web_server::async {

    enum class EventType : uint32_t {
        READ = 0x01,
        WRITE = 0x02,
        ACCEPT = 0x04,
        CONNECT = 0x08,
        DISCONNECT = 0x10,
        CLOSE = 0x20,
        TIMEOUT = 0x40,
        ERROR = 0x80,
        ALL = 0xFF
    };

    constexpr EventType operator|(EventType a, EventType b) noexcept {
        return static_cast<EventType>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    constexpr EventType operator&(EventType a, EventType b) noexcept {
        return static_cast<EventType>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }

    constexpr EventType operator~(EventType a) noexcept { return static_cast<EventType>(~static_cast<uint32_t>(a)); }

} // namespace tiny_web_server::async

#endif // TINY_WEB_SERVER_ASYNC_ENUMS_HPP
