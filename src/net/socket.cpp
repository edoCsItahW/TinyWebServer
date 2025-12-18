// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for
// commercial purposes is prohibited without the author's permission. If you have any
// questions or require permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file socket.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/18 12:43
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#include "tws/net/socket.hpp"
#include "tws/exception.hpp"

#if WEB_SERVER_LINUX
#include <cstring>
#endif

namespace tiny_web_server::net {

    int Socket::count = 0;

    Socket::Socket(AddressFamily family, SocketType type, Protocol protocol) {
        if (count == 0) initialize();

        handle_ = socket(
            static_cast<int>(family), static_cast<int>(type), static_cast<int>(protocol)
        );

        if (handle_ == NET_INVALID_SOCKET) throw SocketError<"Failed to create socket"_s>();
    }

    Socket::~Socket() { close(); }

    Socket::Socket(Socket&& other) noexcept
        : handle_(other.handle_) {
        other.handle_ = NET_INVALID_SOCKET;
    }

    Socket& Socket::operator=(Socket&& other) noexcept {
        if (this != &other) {
            if (isValid()) NET_CLOSE(handle_);

            handle_       = other.handle_;
            other.handle_ = NET_INVALID_SOCKET;
        }

        return *this;
    }

    void Socket::bind(const Endpoint& endpoint) const {
        // IPv6
        if (endpoint.address().isIPv6()) {
            sockaddr_in6 addr{};
            addr.sin6_family = AF_INET6;
            addr.sin6_port   = htons(endpoint.port());

            std::memcpy(&addr.sin6_addr, endpoint.address().toBytes().data(), 16);

            if (::bind(handle_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
                throw SocketError<"Failed to bind socket to endpoint"_s>();
        }

        // IPv4
        else {
            sockaddr_in addr{};
            addr.sin_family = AF_INET;
            addr.sin_port   = htons(endpoint.port());

            std::memcpy(&addr.sin_addr, endpoint.address().toBytes().data(), 4);

            if (::bind(handle_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
                throw SocketError<>(NET_ERROR, "Failed to bind socket to endpoint in IPv4");
        }
    }

    void Socket::listen(const int backlog) const {
        if (::listen(handle_, backlog) < 0)
            throw SocketError<"Failed to listen on socket"_s>();
    }

    Socket Socket::accept() const {
        sockaddr_storage addr{};
        socklen_t addrlen = sizeof(addr);

        auto clientHandler = ::accept(handle_, reinterpret_cast<sockaddr*>(&addr), &addrlen);
        if (static_cast<int>(clientHandler) < 0)
            throw SocketError<"Failed to accept connection from socket"_s>();

        return std::move(clientHandler);
    }

    void Socket::connect(const Endpoint& endpoint) const {
        // IPv6
        if (endpoint.address().isIPv6()) {
            sockaddr_in6 addr{};
            addr.sin6_family = AF_INET6;
            addr.sin6_port   = htons(endpoint.port());

            std::memcpy(
                &addr.sin6_addr, endpoint.address().toBytes().data(), sizeof(addr.sin6_addr)
            );

            if (::connect(handle_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
                throw SocketError<"Failed to connect to endpoint"_s>();
        }

        // IPv4
        else {
            sockaddr_in addr{};
            addr.sin_family = AF_INET;
            addr.sin_port   = htons(endpoint.port());

            std::memcpy(
                &addr.sin_addr, endpoint.address().toBytes().data(), sizeof(addr.sin_addr)
            );

            if (::connect(handle_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
                throw SocketError<"Failed to connect to endpoint"_s>();
        }
    }

    std::size_t Socket::recv(std::span<std::byte> buffer, int flags) const {
        auto received =
            ::recv(handle_, reinterpret_cast<char*>(buffer.data()), buffer.size(), flags);

        if (received < 0) throw SocketError<"Failed to receive from socket"_s>();

        return static_cast<std::size_t>(received);
    }

    std::size_t Socket::send(std::span<const std::byte> data, int flags) const {
        auto sent =
            ::send(handle_, reinterpret_cast<const char*>(data.data()), data.size(), flags);

        if (sent < 0) throw SocketError<"Failed to send to socket"_s>();

        return static_cast<std::size_t>(sent);
    }

    void Socket::setOptions(const Options& opts) const {
        if (opts.reuse_address) {
            if (char enable = 1;
                setsockopt(handle_, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)))
                throw SocketError<"Failed to set SO_REUSEADDR option on socket"_s>();
        }

        if (opts.keep_alive) {
            if (char enable = 1;
                setsockopt(handle_, SOL_SOCKET, SO_KEEPALIVE, &enable, sizeof(int)))
                throw SocketError<"Failed to set SO_KEEPALIVE option on socket"_s>();
        }

        if (opts.no_delay) {
            if (char enable = 1;
                setsockopt(handle_, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(int)))
                throw SocketError<"Failed to set TCP_NODELAY option on socket"_s>();
        }

        if (opts.receive_timeout_ms > 0) {
#if WEB_SERVER_WINDOWS
            DWORD timeout = opts.receive_timeout_ms;
#else
            struct timeval timeout{};
            timeout.tv_sec  = opts.receive_timeout_ms / 1000;
            timeout.tv_usec = (opts.receive_timeout_ms % 1000) * 1000;
#endif

            if (setsockopt(
                    handle_, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&timeout),
                    sizeof(timeout)
                ))
                throw SocketError<"Failed to set SO_RCVTIMEO option on socket"_s>();
        }

        if (opts.send_timeout_ms > 0) {
#if WEB_SERVER_WINDOWS
            DWORD timeout = opts.send_timeout_ms;
#else
            struct timeval timeout{};
            timeout.tv_sec  = opts.send_timeout_ms / 1000;
            timeout.tv_usec = (opts.send_timeout_ms % 1000) * 1000;
#endif

            if (setsockopt(
                    handle_, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<char*>(&timeout),
                    sizeof(timeout)
                ))
                throw SocketError<"Failed to set SO_SNDTIMEO option on socket"_s>();
        }
    }

    void Socket::setNonBlocking(bool nonBlocking) const {
#if WEB_SERVER_WINDOWS
        u_long mode = nonBlocking ? 1 : 0;

        if (ioctlsocket(handle_, FIONBIO, &mode) != 0)
            throw SocketError<"Failed to set non-blocking mode on socket"_s>();

#else
        int flags = fcntl(handle_, F_GETFL, 0);

        if (nonBlocking)
            flags |= O_NONBLOCK;
        else
            flags &= ~O_NONBLOCK;

        if (fcntl(handle_, F_SETFL, flags) != 0)
            throw SocketError<"Failed to set non-blocking mode on socket"_s>();
#endif
    }

    void Socket::close() {
        count--;

        if (isValid()) NET_CLOSE(handle_);

        handle_ = NET_INVALID_SOCKET;
    }

    bool Socket::isValid() const { return handle_ != NET_INVALID_SOCKET; }

    socket_t Socket::nativeHandle() const noexcept { return handle_; }

    void Socket::initialize() {
#if WEB_SERVER_WINDOWS
        WSADATA wsaData;

        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
            throw SocketError<>(NET_ERROR, "Failed to initialize Winsock");

#endif
    }

    Socket::Socket(socket_t&& handle)
        : handle_(handle) {}

}  // namespace tiny_web_server::net
