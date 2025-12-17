// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file socket.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/16 19:50
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#include "socket.hpp"
#include "../../exception.hpp"


namespace tiny_web_server::net {

    Socket::Socket(AddressFamily family, SocketType type, Protocol proto) : handle_(family, type, proto) {}

    void Socket::bind(const Endpoint &endpoint) {
        // IPv6
        if (endpoint.address().isIpv6()) {
            sockaddr_in6 addr{};
            addr.sin6_family = AF_INET6;
            addr.sin6_port = htons(endpoint.port());

            std::memcpy(&addr.sin6_addr, endpoint.address().toBytes().data(), 16);

            exception::detail::checkError(
                    ::bind(handle_.nativeHandle(), reinterpret_cast<sockaddr *>(&addr), sizeof(addr)), "bind");
        }

        // IPv4
        else {
            sockaddr_in addr{};
            addr.sin_family = AF_INET;
            addr.sin_port = htons(endpoint.port());

            auto bytes = endpoint.address().toBytes();

            std::memcpy(&addr.sin_addr, bytes.data(), 4);

            exception::detail::checkError(
                    ::bind(handle_.nativeHandle(), reinterpret_cast<sockaddr *>(&addr), sizeof(addr)), "bind");
        }
    }

    void Socket::listen(int backlog) const {
        exception::detail::checkError(::listen(handle_.nativeHandle(), backlog), "listen");
    }

    Socket Socket::accept() const {
        sockaddr_storage addr{};
        socklen_t addrLen = sizeof(addr);

        auto clientHandle = ::accept(handle_.nativeHandle(), reinterpret_cast<sockaddr *>(&addr), &addrLen);

        exception::detail::checkError(static_cast<int>(clientHandle), "accept");

        Socket clientSocket;
        clientSocket.handle_ = SocketHandle();
        *reinterpret_cast<socket_t *>(&clientSocket.handle_) = clientHandle;

        return clientSocket;
    }

    void Socket::connect(const Endpoint &endpoint) const {
        // IPv6
        if (endpoint.address().isIpv6()) {
            sockaddr_in6 addr{};
            addr.sin6_family = AF_INET6;
            addr.sin6_port = htons(endpoint.port());

            std::memcpy(&addr.sin6_addr, endpoint.address().toBytes().data(), 16);

            exception::detail::checkError(
                    ::connect(handle_.nativeHandle(), reinterpret_cast<sockaddr *>(&addr), sizeof(addr)), "connect");
        }

        // IPv4
        else {
            sockaddr_in addr{};
            addr.sin_family = AF_INET;
            addr.sin_port = htons(endpoint.port());

            auto bytes = endpoint.address().toBytes();

            std::memcpy(&addr.sin_addr, bytes.data(), 4);

            exception::detail::checkError(
                    ::connect(handle_.nativeHandle(), reinterpret_cast<sockaddr *>(&addr), sizeof(addr)), "connect");
        }
    }

    std::size_t Socket::send(std::span<const std::byte> data, int flags) const {
        auto result = ::send(handle_.nativeHandle(), reinterpret_cast<const char *>(data.data()),
                             static_cast<int>(data.size()), flags);

        exception::detail::checkError(result, "send");

        return result;
    }

    std::size_t Socket::receive(std::span<std::byte> buffer, int flags) const {
        auto result = recv(handle_.nativeHandle(), reinterpret_cast<char *>(buffer.data()),
                           static_cast<int>(buffer.size()), flags);

        exception::detail::checkError(result, "recv");

        return static_cast<std::size_t>(result);
    }

    std::optional<std::size_t> Socket::sendWithTimeout(std::span<const std::byte> data,
                                                       std::chrono::milliseconds timeout) const {
        fd_set writeSet{};
        FD_ZERO(&writeSet);
        FD_SET(handle_.nativeHandle(), &writeSet);

        timeval timeoutVal{.tv_sec = static_cast<long>(timeout.count() / 1000),
                           .tv_usec = static_cast<long>((timeout.count() % 1000) * 1000)};

        int result = ::select(static_cast<int>(handle_.nativeHandle()) + 1, nullptr, &writeSet, nullptr, &timeoutVal);

        if (result > 0) {
            auto sent = ::send(handle_.nativeHandle(), reinterpret_cast<const char *>(data.data()),
                               static_cast<int>(data.size()), 0);

            if (sent > 0)
                return static_cast<size_t>(sent);
            if (sent == 0)
                return 0;

            if (errno == EAGAIN || errno == EWOULDBLOCK)
                return std::nullopt;
        }

        return std::nullopt;
    }

    void Socket::setOptions(const SocketOptions &opts) const {
        if (opts.reuse_address) {
            int enable = 1;

            if (setsockopt(handle_.nativeHandle(), SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char *>(&enable),
                           sizeof(enable)) == -1)
                throw exception::SocketError(NET_ERROR, "setsockopt SO_REUSEADDR failed");
        }

        if (opts.keep_alive) {
            int enable = 1;

            if (setsockopt(handle_.nativeHandle(), SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<const char *>(&enable),
                           sizeof(enable)))
                throw exception::SocketError(NET_ERROR, "setsockopt SO_KEEPALIVE failed");
        }

        if (opts.no_delay) {
            int enable = 1;

            if (setsockopt(handle_.nativeHandle(), IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<const char *>(&enable),
                           sizeof(enable)))
                throw exception::SocketError(NET_ERROR, "setsockopt TCP_NODELAY failed");
        }

        if (opts.receive_timeout_ms > 0) {
#if WEB_SERVER_WINDOWS
            DWORD timeout = opts.receive_timeout_ms;

#else
            struct timeval timeout{};
            timeout.tv_sec = opts.receive_timeout_ms / 1000;
            timeout.tv_usec = (opts.receive_timeout_ms % 1000) * 1000;

#endif

            if (setsockopt(handle_.nativeHandle(), SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char *>(&timeout),
                           sizeof(timeout)))
                throw exception::SocketError(NET_ERROR, "setsockopt SO_RCVTIMEO failed");
        }

        if (opts.send_timeout_ms > 0) {
#if WEB_SERVER_WINDOWS
            DWORD timeout = opts.receive_timeout_ms;

#else
            struct timeval timeout{};
            timeout.tv_sec = opts.receive_timeout_ms / 1000;
            timeout.tv_usec = (opts.receive_timeout_ms % 1000) * 1000;

#endif

            if (setsockopt(handle_.nativeHandle(), SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char *>(&timeout),
                           sizeof(timeout)))
                throw exception::SocketError(NET_ERROR, "setsockopt SO_SNDTIMEO failed");
        }
    }

    void Socket::setNonBlocking(bool nonBlocking) {
#if WEB_SERVER_WINDOWS
        u_long mode = nonBlocking ? 1 : 0;

        ioctlsocket(handle_.nativeHandle(), FIONBIO, &mode);

#else
        int flags = fcntl(handle_.nativeHandle(), F_GETFL, 0);

        if (nonBlocking)
            flags |= O_NONBLOCK;

        else
            flags &= ~O_NONBLOCK;

        fcntl(handle_.nativeHandle(), F_SETFL, flags);
#endif
    }

    void Socket::close() { handle_.reset(); }

    bool Socket::isValid() const noexcept { return handle_.isValid(); }

} // namespace tiny_web_server::net
