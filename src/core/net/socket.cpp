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

#include <string>


namespace tiny_web_server::net {

    // SocketError

    SocketError::SocketError(int errorCode, std::string_view operation) :
        std::system_error(errorCode, std::system_category(),
                          std::format("SocketError: Network operation '{}' failed", operation)) {}

    SocketError::SocketError(std::string_view message) : std::system_error(0, std::system_category(), message.data()) {}

    namespace detail {

        void checkError(int result, std::string_view operation) {
            if (result < 0)
                throw SocketError(NET_ERROR, operation);
        }

    } // namespace detail

    // SocketInitializer

    SocketInitializer::SocketInitializer() {
#if WEB_SERVER_WINDOWS
        WSADATA wsaData;

        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
            throw SocketError(NET_ERROR, "Failed to initialize Winsock");

#endif
    }

    SocketInitializer::~SocketInitializer() {
#if WEB_SERVER_WINDOWS

        WSACleanup();

#endif
    }

    // IpAddress

    std::optional<IpAddress> IpAddress::fromString(std::string_view str) {
        // IPv6
        if (str.find(":") != std::string::npos) {
            if (in6_addr addr{}; inet_pton(AF_INET6, str.data(), &addr) == 1)
                return IpAddress{addr};
        }

        // IPv4
        else if (in_addr addr{}; inet_pton(AF_INET, str.data(), &addr) == 1)
            return IpAddress{addr};

        return std::nullopt;
    }

    IpAddress IpAddress::fromBytes(std::span<const std::byte> bytes, bool ipv6) {
        // IPv6
        if (ipv6) {
            if (bytes.size() < 16) // IPv6最小字节数为16
                throw SocketError("Insufficient bytes for IPv6 address");

            in6_addr addr{};
            std::memcpy(&addr, bytes.data(), 16);
            return IpAddress{addr};
        }

        // IPv4
        if (bytes.size() < 4) // IPv4最小字节数为4
            throw SocketError("Insufficient bytes for IPv4 address");

        in_addr addr{};
        std::memcpy(&addr, bytes.data(), 4);
        return IpAddress{addr};
    }

    std::string IpAddress::toString() const {
        char buffer[INET6_ADDRSTRLEN]{};

        // IPv6
        if (std::holds_alternative<in6_addr>(addr_))
            inet_ntop(AF_INET6, &std::get<in6_addr>(addr_), buffer, sizeof(buffer));

        // IPv4
        else
            inet_ntop(AF_INET, &std::get<in_addr>(addr_), buffer, sizeof(buffer));

        return buffer;
    }

    std::vector<std::byte> IpAddress::toBytes() const {
        std::vector<std::byte> bytes;

        // IPv4
        if (std::holds_alternative<in_addr>(addr_)) {
            const auto *data = reinterpret_cast<const std::byte *>(&std::get<in_addr>(addr_));

            bytes.assign(data, data + 4);
        }

        // IPv6
        else {
            const auto *data = reinterpret_cast<const std::byte *>(&std::get<in6_addr>(addr_));

            bytes.assign(data, data + 16);
        }

        return bytes;
    }

    bool IpAddress::isIpv4() const noexcept { return std::holds_alternative<in_addr>(addr_); }

    bool IpAddress::isIpv6() const noexcept { return std::holds_alternative<in6_addr>(addr_); }

    IpAddress IpAddress::any(bool ipv6) {
        // IPv6
        if (ipv6)
            return IpAddress{in6addr_any};

        // IPv4
        in_addr addr{};
        addr.s_addr = 0x00000000;
        return IpAddress{addr};
    }

    IpAddress IpAddress::loopback(bool ipv6) {
        // IPv6
        if (ipv6)
            return IpAddress{in6addr_loopback};

        // IPv4
        in_addr addr{};
        addr.s_addr = 0x0100007F;
        return IpAddress{addr};
    }

    IpAddress::IpAddress(in6_addr addr) : addr_{addr} {}

    IpAddress::IpAddress(in_addr addr) : addr_{addr} {}

    // Endpoint

    Endpoint::Endpoint(IpAddress addr, std::uint16_t port) : addr_{addr}, port_{port} {}

    std::expected<Endpoint, std::string> Endpoint::fromString(std::string_view str) {
        auto colonPos = str.find_last_of(':');
        if (colonPos == std::string_view::npos)
            return std::unexpected("Invalid endpoint format, expected 'host:port'");

        auto host = str.substr(0, colonPos);
        auto portStr = str.substr(colonPos + 1);

        auto addr = IpAddress::fromString(host);
        if (!addr)
            return std::unexpected("Invalid IP address");

        try {
            auto port = std::stoi(std::string(portStr));

            if (port < 0 || port > 65535)
                return std::unexpected("Port number out of range");

            return Endpoint{*addr, static_cast<std::uint16_t>(port)};

        } catch (...) {
            return std::unexpected("Invalid port number");
        }
    }

    const IpAddress &Endpoint::address() const noexcept { return addr_; }

    std::uint16_t Endpoint::port() const noexcept { return port_; }

    std::string Endpoint::toString() const { return std::format("{}:{}", addr_.toString(), port_); }

    // SocketHandle

    SocketHandle::SocketHandle(AddressFamily family, SocketType type, Protocol proto) {
        int sockType = 0;

        switch (type) {
            using enum SocketType;
            case STREAM:
                sockType = SOCK_STREAM;
                break;
            case DATAGRAM:
                sockType = SOCK_DGRAM;
                break;
            case RAW:
                sockType = SOCK_RAW;
                break;
            case SEQPACKET:
                sockType = SOCK_SEQPACKET;
                break;
        }

        handle_ = ::socket(static_cast<int>(family), sockType, static_cast<int>(proto));
        if (handle_ == invalidSocket)
            throw SocketError("Failed to create socket");
    }

    SocketHandle::~SocketHandle() {
        if (isValid())
            NET_CLOSE(handle_);
    }

    SocketHandle::SocketHandle(SocketHandle &&other) noexcept : handle_{other.handle_} {
        other.handle_ = invalidSocket;
    }

    SocketHandle &SocketHandle::operator=(SocketHandle &&other) noexcept {
        if (this != &other) {
            if (isValid())
                NET_CLOSE(handle_);

            handle_ = other.handle_;
            other.handle_ = invalidSocket;
        }

        return *this;
    }

    bool SocketHandle::isValid() const noexcept { return handle_ != invalidSocket; }

    socket_t SocketHandle::nativeHandle() const noexcept { return handle_; }

    void SocketHandle::reset() noexcept {
        if (isValid()) {
            NET_CLOSE(handle_);

            handle_ = invalidSocket;
        }
    }

    void SocketHandle::swap(SocketHandle &other) noexcept { std::swap(handle_, other.handle_); }

    // Socket

    Socket::Socket(AddressFamily family, SocketType type, Protocol proto) : handle_(family, type, proto) {}

    void Socket::bind(const Endpoint &endpoint) {
        // IPv6
        if (endpoint.address().isIpv6()) {
            sockaddr_in6 addr{};
            addr.sin6_family = AF_INET6;
            addr.sin6_port = htons(endpoint.port());

            std::memcpy(&addr.sin6_addr, endpoint.address().toBytes().data(), 16);

            detail::checkError(::bind(handle_.nativeHandle(), reinterpret_cast<sockaddr *>(&addr), sizeof(addr)),
                               "bind");
        }

        // IPv4
        else {
            sockaddr_in addr{};
            addr.sin_family = AF_INET;
            addr.sin_port = htons(endpoint.port());

            auto bytes = endpoint.address().toBytes();

            std::memcpy(&addr.sin_addr, bytes.data(), 4);

            detail::checkError(::bind(handle_.nativeHandle(), reinterpret_cast<sockaddr *>(&addr), sizeof(addr)),
                               "bind");
        }
    }

    void Socket::listen(int backlog) const { detail::checkError(::listen(handle_.nativeHandle(), backlog), "listen"); }

    Socket Socket::accept() const {
        sockaddr_storage addr{};
        socklen_t addrLen = sizeof(addr);

        auto clientHandle = ::accept(handle_.nativeHandle(), reinterpret_cast<sockaddr *>(&addr), &addrLen);

        detail::checkError(static_cast<int>(clientHandle), "accept");

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

            detail::checkError(::connect(handle_.nativeHandle(), reinterpret_cast<sockaddr *>(&addr), sizeof(addr)),
                               "connect");
        }

        // IPv4
        else {
            sockaddr_in addr{};
            addr.sin_family = AF_INET;
            addr.sin_port = htons(endpoint.port());

            auto bytes = endpoint.address().toBytes();

            std::memcpy(&addr.sin_addr, bytes.data(), 4);

            detail::checkError(::connect(handle_.nativeHandle(), reinterpret_cast<sockaddr *>(&addr), sizeof(addr)),
                               "connect");
        }
    }

    std::size_t Socket::send(std::span<const std::byte> data, int flags) const {
        auto result = ::send(handle_.nativeHandle(), reinterpret_cast<const char *>(data.data()),
                             static_cast<int>(data.size()), flags);

        detail::checkError(result, "send");

        return result;
    }

    std::size_t Socket::receive(std::span<std::byte> buffer, int flags) const {
        auto result = recv(handle_.nativeHandle(), reinterpret_cast<char *>(buffer.data()),
                           static_cast<int>(buffer.size()), flags);

        detail::checkError(result, "recv");

        return static_cast<std::size_t>(result);
    }

    std::optional<std::size_t> Socket::sendWithTimeout(std::span<const std::byte> data,
                                                       std::chrono::milliseconds timeout) const {
        fd_set writeSet{};
        FD_ZERO(&writeSet);
        FD_SET(handle_.nativeHandle(), &writeSet);

        timeval timeoutVal{.tv_sec = static_cast<long>(timeout.count() / 1000),
                           .tv_usec = static_cast<long>(timeout.count() % 1000 * 1000)};

        auto result = ::select(static_cast<int>(handle_.nativeHandle()) + 1, nullptr, &writeSet, nullptr, &timeoutVal);

        if (result > 0)
            return send(data);

        return std::nullopt;
    }

    void Socket::setOptions(const SocketOptions &opts) const {
        if (opts.reuse_address) {
            int enable = 1;

            setsockopt(handle_.nativeHandle(), SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char *>(&enable),
                       sizeof(enable));
        }

        if (opts.keep_alive) {
            int enable = 1;

            setsockopt(handle_.nativeHandle(), SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<const char *>(&enable),
                       sizeof(enable));
        }

        if (opts.no_delay) {
            int enable = 1;

            setsockopt(handle_.nativeHandle(), IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<const char *>(&enable),
                       sizeof(enable));
        }

        if (opts.receive_timeout_ms > 0) {
            DWORD timeout = opts.receive_timeout_ms;

            setsockopt(handle_.nativeHandle(), SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char *>(&timeout),
                       sizeof(timeout));
        }

        if (opts.send_timeout_ms > 0) {
            DWORD timeout = opts.send_timeout_ms;

            setsockopt(handle_.nativeHandle(), SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char *>(&timeout),
                       sizeof(timeout));
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
