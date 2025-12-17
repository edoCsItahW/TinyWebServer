/**
 * @file types.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/17 18:21
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#ifndef TINY_WEB_SERVER_TYPES_HPP
#define TINY_WEB_SERVER_TYPES_HPP
#pragma once

#include "event_data.hpp"
#include "../../platform.hpp"

#include <functional>
#include <expected>
#include <system_error>

namespace tiny_web_server::async {

    template<typename T>
    concept EventHandler = requires(T handler, socket_t fd, EventType data) {
        { handler.onEvent(fd, data) } -> std::same_as<void>;
        { handler.onError(fd, data) } -> std::same_as<void>;
    };

    using TaskHandler = std::move_only_function<void(std::expected<EventData, std::error_code>)>;

} // namespace tiny_web_server::async

#endif // TINY_WEB_SERVER_TYPES_HPP
