// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for
// commercial purposes is prohibited without the author's permission. If you have any
// questions or require permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file reactor.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/18 21:45
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#include "tws/async/reactor.hpp"
#include "tws/async/reactor_io_uring.hpp"
#include "tws/async/reactor_iocp.hpp"
#include <cstring>

namespace tiny_web_server::async {


#if WEB_SERVER_WINDOWS


    void Reactor::run() { impl_->workerThread.join(); }

    void Reactor::stop() {
        impl_->running = false;
        PostQueuedCompletionStatus(impl_->iocpHandle, 0, 0, nullptr);
    }

    Task<net::Socket> Reactor::asyncAccept(net::Socket& listener) {
        auto op = impl_->createOperation<AcceptOperation>(&listener, nullptr);

        op->handle = std::coroutine_handle<>::from_address(co_await std::suspend_always{});

        auto accepted = net::Socket(net::AddressFamily::IPv4, net::SocketType::STREAM);

        CreateIoCompletionPort(
            reinterpret_cast<HANDLE>(accepted.nativeHandle()), impl_->iocpHandle,
            reinterpret_cast<ULONG_PTR>(&listener), 0
        );

        DWORD bytes_received = 0;
        BOOL result          = AcceptEx(
            listener.nativeHandle(), accepted.nativeHandle(), &op->client_addr, 0,
            sizeof(op->client_addr) + 16, sizeof(op->client_addr) + 16, &bytes_received,
            &op->overlapped
        );

        if (!result && GetLastError() != ERROR_IO_PENDING) {
            impl_->destroyOperation(op);
            throw std::system_error(
                GetLastError(), std::system_category(), "AcceptEx failed"
            );
        }

        co_await std::suspend_always{};

        auto socket = std::move(accepted);
        impl_->destroyOperation(op);
        co_return socket;
    }

#elif WEB_SERVER_LINUX


    Reactor::Reactor()
        : impl_(std::make_unique<Impl>()) {}

    Reactor::~Reactor() = default;

    void Reactor::run() { impl_->workerThread.join(); }

    void Reactor::stop() { impl_->running = false; }

    Task<net::Socket> Reactor::asyncAccept(net::Socket& listener) {
        auto op = impl_->createOperation<AcceptOperation>(&listener);

        op->handle = std::coroutine_handle<>::from_address(co_await std::suspend_always{});

        sockaddr_in clientAddr{};
        socklen_t clientAddrLen = sizeof(clientAddr);

        impl_->submitAccept(listener.nativeHandle(), reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrLen, op);

        co_await std::suspend_always{};

        if (op->result < 0) {
            impl_->destroyOperation(op);

            throw std::system_error(-op->result, std::system_category(), "Accept failed");
        }

        net::Socket socket(static_cast<socket_t>(op->result));
        impl_->destroyOperation(op);

        co_await socket;
    }

    Task<std::vector<std::byte>> Reactor::asyncRecv(net::Socket& socket, std::size_t size) {
        auto op = impl_->createOperation<RecvOperation>(size);

        op->handle = std::coroutine_handle<>::from_address(co_await std::suspend_always{});

        impl_->submitRecv(socket.nativeHandle(), op->buffer.data(), op->buffer.size(), op);

        co_await std::suspend_always{};

        if (op->result < 0) {
            impl_->destroyOperation(op);

            throw std::system_error(-op->result, std::system_category(), "Recv failed");
        }

        op->buffer.resize(op->result);
        auto result = std::move(op->buffer);
        impl_->destroyOperation(op);

        co_await result;
    }

    Task<std::size_t> Reactor::asyncSend(
        net::Socket& socket, std::span<const std::byte> data
    ) {
        auto op = impl_->createOperation<SendOperation>(data);

        op->handle = std::coroutine_handle<>::from_address(co_await std::suspend_always{});

        impl_->submitSend(socket.nativeHandle(), op->data.data(), op->data.size(), op);

        co_await std::suspend_always{};

        if (op->result < 0) {
            impl_->destroyOperation(op);

            throw std::system_error(-op->result, std::system_category(), "Send failed");
        }

        auto bytesSent = static_cast<std::size_t>(op->result);
        impl_->destroyOperation(op);

        co_await bytesSent;
    }

    Task<void> Reactor::asyncConnect(net::Socket& socket, const net::Endpoint& endpoint) {
        auto op = impl_->createOperation<ConnectOperation>(endpoint);

        op->handle = std::coroutine_handle<>::from_address(co_await std::suspend_always{});

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(endpoint.port());

        auto addressBytes = endpoint.address().toBytes();
        std::memcpy(&addr.sin_addr, addressBytes.data(), 4);

        impl_->submitConnect(socket.nativeHandle(), reinterpret_cast<sockaddr*>(&addr), sizeof(addr), op);

        co_await std::suspend_always{};

        if (op->result < 0) {
            impl_->destroyOperation(op);

            throw std::system_error(-op->result, std::system_category(), "Connect failed");
        }

        impl_->destroyOperation(op);

        co_return;
    }

#else

    #error "Unsupported platform"

#endif


}  // namespace tiny_web_server::async