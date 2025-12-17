// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file io_uring.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/17 17:33
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#ifndef TINY_WEB_SERVER_IO_URING_HPP
#define TINY_WEB_SERVER_IO_URING_HPP
#pragma once
#include "../../platform.hpp"


#if WEB_SERVER_LINUX
#include <liburing/io_uring.h>
#endif
#include <chrono>
#include "types.hpp"

namespace tiny_web_server::async::detail {

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

} // namespace tiny_web_server::async::detail

#endif // TINY_WEB_SERVER_IO_URING_HPP
