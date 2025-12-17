// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file reactor.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/17 14:18
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#ifndef TINY_WEB_SERVER_REACTOR_INL
#define TINY_WEB_SERVER_REACTOR_INL
#pragma once

#include "task.hpp"

#include "../../exception.hpp"

#if WEB_SERVER_WINDOWS

    #include "io_completion_port.hpp"

#else

    #include "io_uring.hpp"

#endif

namespace tiny_web_server::async {

    template<EventHandler Handler>
    void Reactor::registerSocket(socket_t socket, EventType events, Handler &&handler) {
        auto key = reinterpret_cast<std::uintptr_t>(&handler);

#if WEB_SERVER_WINDOWS
        if (!iocp_.associate(socket, key))
            throw exception::SocketError(GetLastError(), "IOCP associate failed");

        if (static_cast<bool>(events & EventType::READ)) {
            auto operation = std::make_unique<detail::IoOperation>();

            if (!operation)
                throw exception::SocketError("Failed to allocate IoOperation");

            submitIocpRead(socket, {}, reinterpret_cast<std::uintptr_t>(operation.get()));

            operations_[key] = std::move(operation);
        }

#else
        registerLinuxEvents(socket, events, std::forward<Handler>(handler));

#endif
        handlers_.emplace(
                socket, std::unique_ptr<void, void (*)(void *)>(new Handler(std::forward<Handler>(handler)),
                                                                [](void *ptr) { delete static_cast<Handler *>(ptr); }));
    }

    template<EventHandler Handler>
    void Reactor::submitRead(socket_t socket, Handler &&handler) {
        auto operation = std::make_unique<detail::IoOperation>();

        operation->socket = socket;
        operation->eventType = EventType::READ;
        operation->callback = [this, handler = std::forward<Handler>(handler)](
                                      std::expected<EventData, std::error_code> result) mutable {
            if (result)
                handler.onEvent(result->data ? *static_cast<socket_t *>(result->data) : NET_INVALID_SOCKET, *result);

            else
                handler.onError(NET_INVALID_SOCKET, result.error().value());
        };

        auto key = reinterpret_cast<std::uintptr_t>(operation.get());

        operations_.emplace(key, std::move(operation));

#if WEB_SERVER_WINDOWS
        submitIocpRead(socket, {}, key);

#else
        submitUringRead(socket, {}, key);

#endif
    }

    template<EventHandler Handler>
    Task Reactor::asyncRead(socket_t socket, std::span<std::byte> buffer, Handler &&handler) {
        auto operation = std::make_unique<detail::IoOperation>();

        operation->socket = socket;
        operation->eventType = EventType::READ;
        operation->callback = [this, handler = std::forward<Handler>(handler)](
                                      std::expected<EventData, std::error_code> result) mutable {
            if (result)
                handler.onEvent(result->data ? *static_cast<socket_t *>(result->data) : NET_INVALID_SOCKET, *result);

            else
                handler.onError(NET_INVALID_SOCKET, result.error().value());
        };

        auto key = reinterpret_cast<std::uintptr_t>(operation.get());

        operations_.emplace(key, std::move(operation));

#if WEB_SERVER_WINDOWS
        submitIocpRead(socket, buffer, key);

#else
        submitUringRead(socket, buffer, key);

#endif

        co_await std::suspend_always{};
    }

    template<EventHandler Handler>
    Task Reactor::asyncWrite(socket_t socket, std::span<const std::byte> data, Handler &&handler) {
        auto operation = std::make_unique<detail::IoOperation>();

        operation->socket = socket;
        operation->eventType = EventType::WRITE;
        operation->buffer.assign(data.data(), data.data() + data.size());
        operation->callback = [this, handler = std::forward<Handler>(handler)](
                                      std::expected<EventData, std::error_code> result) mutable {
            if (result)
                handler.onEvent(result->data ? *static_cast<socket_t *>(result->data) : NET_INVALID_SOCKET, *result);

            else
                handler.onError(NET_INVALID_SOCKET, result.error().value());
        };

        auto key = reinterpret_cast<std::uintptr_t>(operation.get());

        operations_.emplace(key, std::move(operation));

#if WEB_SERVER_WINDOWS
        submitIocpWrite(socket, data, key);

#else
        submitUringWrite(socket, data, key);

#endif

        co_await std::suspend_always{};
    }

    template<std::size_t I>
    auto Reactor::get() const {
        if constexpr (I == 0)
            return config_;

        else if constexpr (I == 1)
            return isRunning();

        else
            static_assert(I < 2, "Index out of range");
    }

} // namespace tiny_web_server::async

#endif // TINY_WEB_SERVER_REACTOR_INL
