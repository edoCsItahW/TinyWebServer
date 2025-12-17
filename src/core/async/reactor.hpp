// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file reactor.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/16 22:25
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#ifndef TINY_WEB_SERVER_REACTOR_HPP
#define TINY_WEB_SERVER_REACTOR_HPP
#pragma once

#include <thread>
#include <functional>

#include "../net/socket.hpp"
#include "enums.hpp"
#include "types.hpp"

#undef ERROR


namespace tiny_web_server::async {

    struct ReactorConfig {
        std::size_t max_events{1024};
        std::size_t io_queue_depth{4096};
        std::size_t thread_count{std::thread::hardware_concurrency()};
        bool usePolling{false};
        std::chrono::milliseconds timeout{1000};

        auto operator<=>(const ReactorConfig &) const = default;
    };

    class Reactor {
    public:
        explicit Reactor(ReactorConfig config = {});

        ~Reactor();

        Reactor(const Reactor &) = delete;

        Reactor &operator=(const Reactor &) = delete;

        Reactor(Reactor &&other) noexcept;

        Reactor &operator=(Reactor &&other) noexcept;

        template<EventHandler Handler>
        void registerSocket(socket_t socket, EventType events, Handler &&handler);

        void unregisterSocket(socket_t socket) noexcept;

        template<EventHandler Handler>
        Task asyncRead(socket_t socket, std::span<std::byte> buffer, Handler &&handler);

        template<EventHandler Handler>
        Task asyncWrite(socket_t socket, std::span<const std::byte> data, Handler &&handler);

        template<EventHandler Handler>
        void submitRead(socket_t socket, Handler &&handler);

        void run();

        void stop() noexcept;

        [[nodiscard]] bool isRunning() const noexcept;

        template<std::size_t I>
        auto get() const;

    private:
        void initialize();

        void eventLoop(std::stop_token stopToken);

#if WEB_SERVER_WINDOWS
        void submitIocpRead(socket_t socket, std::span<std::byte> buffer, std::uintptr_t key);

        void submitIocpWrite(socket_t socket, std::span<const std::byte> data, std::uintptr_t key);

        void processIocpEvents();

#elif WEB_SERVER_LINUX

        void registerLinuxEvents(socket_t socket, EventType events, auto &&handler);

        void submitUringRead(socket_t socket, std::span<std::byte> buffer, std::uintptr_t key);

        void submitUringWrite(socket_t socket, std::span<const std::byte> data, std::uintptr_t key);

        void processUringEvents();

#endif

        void handleCompletion(detail::IoOperation *operation, std::size_t bytesTransferred);

        ReactorConfig config_;

        std::atomic<bool> running_{false};

        std::stop_source stopSource_;

        std::vector<std::jthread> workers_;

#if WEB_SERVER_WINDOWS
        detail::IoCompletionPort iocp_;
#elif WEB_SERVER_LINUX
        detail::IoUring uring_;
#endif

        std::unordered_map<std::uintptr_t, std::unique_ptr<detail::IoOperation>> operations_;

        std::unordered_map<socket_t, std::unique_ptr<void, void (*)(void *)>> handlers_;
    };

} // namespace tiny_web_server::async

namespace std {
    template<>
    struct tuple_size<tiny_web_server::async::Reactor> : integral_constant<std::size_t, 2> {};

    template<>
    struct tuple_element<0, tiny_web_server::async::Reactor> {
        using type = tiny_web_server::async::ReactorConfig;
    };

    template<>
    struct tuple_element<1, tiny_web_server::async::Reactor> {
        using type = bool;
    };

} // namespace std

#include "reactor.inl"

#endif // TINY_WEB_SERVER_REACTOR_HPP
