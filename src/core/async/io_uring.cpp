// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file io_uring.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/17 17:33
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#include "io_uring.hpp"
#include "../../exception.hpp"

#include <utility>

namespace tiny_web_server::async::detail {

    IoUring::IoUring(std::size_t queueDepth, unsigned int flags) {
        if (io_uring_queue_init(static_cast<unsigned>(queueDepth), &ring_, flags) < 0)
            throw exception::SocketError(errno, "io_uring_queue_init failed");
    }

    IoUring::~IoUring() { io_uring_queue_exit(&ring_); }

    io_uring *IoUring::get() noexcept { return &ring_; }

    const io_uring *IoUring::get() const noexcept { return &ring_; }

    io_uring_sqe *IoUring::getSqe() noexcept { return io_uring_get_sqe(&ring_); }

    void IoUring::submit() {
        if (io_uring_submit(&ring_) < 0)
            throw exception::SocketError(errno, "io_uring_submit failed");
    }

    bool IoUring::waitCqeTimeout(io_uring_cqe **cqe, std::chrono::nanoseconds timeout) {
        __kernel_timespec ts{.tv_sec = timeout.count() / 1000000000, .tv_nsec = timeout.count() % 1000000000};

        return io_uring_wait_cqe_timeout(&ring_, cqe, &ts) == 0;
    }

    io_uring_cqe *IoUring::peekCqe() noexcept {
        io_uring_cqe *cqe = nullptr;

        io_uring_peek_cqe(&ring_, &cqe);

        return cqe;
    }

    void IoUring::cqeSeen(io_uring_cqe *cqe) noexcept { io_uring_cqe_seen(&ring_, cqe); }

    IoUring::IoUring(IoUring &&other) noexcept : ring_(std::exchange(other.ring_, {})) {}

    IoUring &IoUring::operator=(IoUring &&other) noexcept {
        if (this != &other) {
            io_uring_queue_exit(&ring_);

            ring_ = std::exchange(other.ring_, {});
        }

        return *this;
    }

} // namespace tiny_web_server::async::detail
