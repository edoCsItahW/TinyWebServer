// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for
// commercial purposes is prohibited without the author's permission. If you have any
// questions or require permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file reactor_io_uring.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/18 22:00
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#include "tws/async/reactor_io_uring.hpp"

#if WEB_SERVER_LINUX

namespace tiny_web_server::async {

    Reactor::Impl::Impl() {
        if (io_uring_queue_init(1024, &ring, 0) < 0)
            throw std::system_error(
                NET_ERROR, std::system_category(), "io_uring_queue_init failed"
            );

        workerThread = std::thread([this] { runIoUring(); });
    }

    Reactor::Impl::~Impl() {
        running = false;

        if (workerThread.joinable()) workerThread.join();

        io_uring_queue_exit(&ring);
    }

    void Reactor::Impl::runIoUring() {
        while (running) {
            io_uring_submit(&ring);

            io_uring_cqe* cqe = nullptr;

            if (io_uring_wait_cqe(&ring, &cqe) < 0) continue;

            if (cqe->user_data) {
                auto* op   = reinterpret_cast<Operation*>(cqe->user_data);
                op->result = cqe->res;

                if (op->handle) op->handle.resume();

                if (op->cleanup) op->cleanup();
            }

            io_uring_cqe_seen(&ring, cqe);
        }
    }

    void Reactor::Impl::destroyOperation(Operation* operation) {
        operation->~Operation();

        memoryPool.deallocate(operation, sizeof(*operation), alignof(*operation));
    }

    void Reactor::Impl::submitAccept(
        int sockfd, sockaddr* addr, socklen_t* addrlen, Operation* operation
    ) {
        io_uring_sqe* sqe = io_uring_get_sqe(&ring);

        io_uring_prep_accept(sqe, sockfd, addr, addrlen, 0);

        io_uring_sqe_set_data(sqe, operation);
    }

    void Reactor::Impl::submitRecv(int sockfd, void* buf, size_t len, Operation* operation) {
        io_uring_sqe* sqe = io_uring_get_sqe(&ring);

        io_uring_prep_recv(sqe, sockfd, buf, len, 0);

        io_uring_sqe_set_data(sqe, operation);
    }

    void Reactor::Impl::submitSend(
        int sockfd, const void* buf, size_t len, Operation* operation
    ) {
        io_uring_sqe* sqe = io_uring_get_sqe(&ring);

        io_uring_prep_send(sqe, sockfd, buf, len, 0);

        io_uring_sqe_set_data(sqe, operation);
    }

    void Reactor::Impl::submitConnect(
        int sockfd, const sockaddr* addr, socklen_t addrlen, Operation* operation
    ) {
        io_uring_sqe* sqe = io_uring_get_sqe(&ring);

        io_uring_prep_connect(sqe, sockfd, addr, addrlen);

        io_uring_sqe_set_data(sqe, operation);
    }

    AcceptOperation::AcceptOperation(net::Socket* listener)
        : listener(listener) {}

    RecvOperation::RecvOperation(std::size_t size)
        : buffer(size) {}

    SendOperation::SendOperation(std::span<const std::byte> data)
        : data(data.begin(), data.end()) {}

    ConnectOperation::ConnectOperation(const net::Endpoint& endpoint)
        : endpoint(endpoint) {}

}  // namespace tiny_web_server::async

#endif  // WEB_SERVER_LINUX
