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

#include <coroutine>
#include <cstdint>
#include <functional>
#include <thread>

#include "../net/platform.hpp"
#include "../net/socket.hpp"
#undef ERROR

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

    struct EventData {
        void *data{nullptr};

        std::size_t bytesTransferred{0};

        int errorCode{0};

        EventType eventType{EventType::READ};

        explicit operator bool() const noexcept;
    };

    template<typename T>
    concept EventHandler = requires(T handler, socket_t fd, EventType data) {
        { handler.onEvent(fd, data) } -> std::same_as<void>;
        { handler.onError(fd, data) } -> std::same_as<void>;
    };

    struct Task {

        struct PromiseType {

            Task getReturnObject() noexcept;

            static std::suspend_always initialSuspend() noexcept;

            static std::suspend_always finalSuspend() noexcept;

            static void returnVoid() noexcept;

            static void unhandledException() noexcept;
        };

        explicit Task(std::coroutine_handle<PromiseType> handle = nullptr) noexcept;

        Task(Task &&other) noexcept;

        Task &operator=(Task &&other) noexcept;

        ~Task();

        void resume() const;

        explicit operator bool() const noexcept;

    private:
        std::coroutine_handle<PromiseType> coroutine;
    };

    using TaskHandler = std::move_only_function<void(std::expected<EventData, std::error_code>)>;

    struct ReactorConfig {
        std::size_t max_events{1024};
        std::size_t io_queue_depth{4096};
        std::size_t thread_count{std::thread::hardware_concurrency()};
        bool usePolling{false};
        std::chrono::milliseconds timeout{1000};

        auto operator<=>(const ReactorConfig &) const = default;
    };

    namespace detail {

#if WEB_SERVER_WINDOWS

        class IoCompletionPort {
        public:
            explicit IoCompletionPort(std::size_t threadCount = 0);

            ~IoCompletionPort();

            [[nodiscard]] HANDLE handle() const noexcept;

            bool associate(SOCKET socket, ULONG_PTR key) const noexcept;

            bool getQueuedCompletionStatus(LPDWORD bytesTransferred, PULONG_PTR completionKey, LPOVERLAPPED *overlapped,
                                           DWORD timeout) const noexcept;

            bool postQueuedCompletionStatus(DWORD bytesTransferred, ULONG_PTR completionKey,
                                            LPOVERLAPPED overlapped) const noexcept;

            IoCompletionPort(IoCompletionPort &&other) noexcept;

            IoCompletionPort &operator=(IoCompletionPort &&other) noexcept;

        private:
            HANDLE iocp_{nullptr};
        };

        struct IoOperation : OVERLAPPED {
            TaskHandler callback;
            socket_t socket{NET_INVALID_SOCKET};
            EventType eventType{EventType::READ};
            std::vector<std::byte> buffer;

            IoOperation() noexcept;

            ~IoOperation() = default;
        };

#else

        class IoUring {
        public:
            explicit IoUring(std::size_t queueDepth = 4096, unsigned int flags = 0);

            ~IoUring();

            [[nodiscard]] io_uring *get() noexcept;

            [[nodiscard]] const io_uring *get() const noexcept;

            io_uring_sqe *getSqe() noexcept;

            void submit();

            bool waitCqeTimeout(io_uring_cqe **cqe, std::chrono::nanoseconds timeout);

            io_uring_cqe *peekCqe() noexcept;

            void cqeSeen(io_uring_cqe *cqe) noexcept;

            IoUring(IoUring &&other) noexcept;

            IoUring &operator=(IoUring &&other) noexcept;

        private:
            io_uring ring_{};
        };

        struct IoOperation {
            TaskHandler callback;
            socket_t socket{NET_INVALID_SOCKET};
            EventType eventType{EventType::READ};
            std::vector<std::byte> buffer;
            int res{0};
            unsigned flags{0};

            ~IoOperation() = default;
        };

#endif

    } // namespace detail

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
