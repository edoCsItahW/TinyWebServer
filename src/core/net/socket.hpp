// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file socket.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/16 19:50
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#ifndef TINY_WEB_SERVER_SOCKET_HPP
#define TINY_WEB_SERVER_SOCKET_HPP
#pragma once

#include <chrono>
#include <expected>
#include <format>
#include <optional>
#include <span>
#include <system_error>
#include <variant>
#include <vector>
#include "platform.hpp"


namespace tiny_web_server::net {

    class SocketError : public std::system_error {
    public:
        SocketError(int errorCode, std::string_view operation);

        explicit SocketError(std::string_view message);
    };

    namespace detail {

        inline void checkError(int result, std::string_view operation);

    }

    /** @class SocketInitializer
     *
     * @if zh
     * @brief 统一网络初始化器
     * @details 网络初始化器，用于统一网络初始化，包括初始化套接字库等。
     *
     * @else
     * @brief Unified network initializer
     * @details Network initializer, used to unify network initialization, including initializing socket library.
     *
     * @endif
     */
    class SocketInitializer {
    public:
        SocketInitializer();

        ~SocketInitializer();

        /**
         * @if zh
         * 禁止拷贝
         *
         * @else
         * Prohibit copy
         *
         * @endif
         */
        SocketInitializer(const SocketInitializer &) = delete;
        SocketInitializer &operator=(const SocketInitializer &) = delete;
    };

    /** @class IpAddress
     *
     * @if zh
     * @brief IP地址类
     * @details IP地址类，用于表示IPv4和IPv6地址。
     *
     * @else
     * @brief IP address class
     * @details IP address class, used to represent IPv4 and IPv6 addresses.
     *
     * @endif
     */
    class IpAddress {
    public:
        /**
         * @if zh
         * @brief 从字符串构造IP地址
         * @details 从字符串构造IP地址，支持IPv4和IPv6。
         *
         * @param str 字符串
         * @return 若字符串格式正确，则返回IP地址，否则返回空值
         *
         * @else
         * @brief Construct IP address from string
         * @details Construct IP address from string, support IPv4 and IPv6.
         *
         * @param str String
         * @return If the string format is correct, return the IP address, otherwise return an empty value
         *
         * @endif
         */
        static std::optional<IpAddress> fromString(std::string_view str);

        /**
         * @if zh
         * @brief 从字节数组构造IP地址
         * @details 从字节数组构造IP地址，支持IPv4和IPv6。
         *
         * @param bytes 字节数组
         * @param ipv6 是否为IPv6地址
         * @return 若字节数组格式正确，则返回IP地址，否则返回空值
         *
         * @else
         * @brief Construct IP address from byte array
         * @details Construct IP address from byte array, support IPv4 and IPv6.
         *
         * @param bytes Byte array
         * @param ipv6 Whether it is an IPv6 address
         * @return If the byte array format is correct, return the IP address, otherwise return an empty value
         *
         * @endif
         */
        static IpAddress fromBytes(std::span<const std::byte> bytes, bool ipv6 = false);

        /**
         * @if zh
         * @brief 获取为字符串
         * @details 获取IP地址的字符串表示形式。
         *
         * @return 字符串表示形式
         *
         * @else
         * @brief Get as string
         * @details Get the string representation of the IP address.
         *
         * @return String representation
         *
         * @endif
         */
        [[nodiscard]] std::string toString() const;

        /**
         * @if zh
         * @brief 获取为字节数组
         * @details 获取IP地址的字节数组表示形式。
         *
         * @return 字节数组表示形式
         *
         * @else
         * @brief Get as byte array
         * @details Get the byte array representation of the IP address.
         *
         * @return Byte array representation
         *
         * @endif
         */
        [[nodiscard]] std::vector<std::byte> toBytes() const;

        /**
         * @if zh
         * @brief 检查是否为IPv4地址
         * @details 检查是否为IPv4地址。
         *
         * @return 是否为IPv4地址
         *
         * @else
         * @brief Check if it is an IPv4 address
         * @details Check if it is an IPv4 address.
         *
         * @return Whether it is an IPv4 address
         *
         * @endif
         */
        [[nodiscard]] bool isIpv4() const noexcept;

        /**
         * @if zh
         * @brief 检查是否为IPv6地址
         * @details 检查是否为IPv6地址。
         *
         * @return 是否为IPv6地址
         *
         * @else
         * @brief Check if it is an IPv6 address
         * @details Check if it is an IPv6 address.
         *
         * @return Whether it is an IPv6 address
         *
         * @endif
         */
        [[nodiscard]] bool isIpv6() const noexcept;

        /**
         * @if zh
         * @brief 任意地址
         * @details 任意地址，用于表示任意IP地址。
         *
         * @param ipv6 是否为IPv6地址
         * @return 任意IP地址
         *
         * @else
         * @brief Any address
         * @details Any address, used to represent any IP address.
         *
         * @param ipv6 Whether it is an IPv6 address
         * @return Any IP address
         *
         * @endif
         */
        static IpAddress any(bool ipv6 = false);

        /**
         * @if zh
         * @brief 环回地址
         * @details 环回地址，用于表示本机地址。
         *
         * @param ipv6 是否为IPv6地址
         * @return 环回IP地址
         *
         * @else
         * @brief Loopback address
         * @details Loopback address, used to represent the local address.
         *
         * @param ipv6 Whether it is an IPv6 address
         * @return Loopback IP address
         *
         * @endif
         */
        static IpAddress loopback(bool ipv6 = false);

    private:
        explicit IpAddress(in_addr addr);

        explicit IpAddress(in6_addr addr);

        /**
         * @if zh
         * @brief 地址
         * @details 地址，用于表示IPv4或IPv6地址。
         *
         * @else
         * @brief Address
         * @details Address, used to represent IPv4 or IPv6 address.
         *
         * @endif
         */
        std::variant<in_addr, in6_addr> addr_;
    };

    /** @class Endpoint
     *
     * @if zh
     * @brief 端点类
     * @details 端点类，用于表示网络连接的端点。
     *
     * @else
     * @brief Endpoint class
     * @details Endpoint class, used to represent the endpoint of a network connection.
     *
     * @endif
     */
    class Endpoint {
    public:
        Endpoint(IpAddress addr, std::uint16_t port);

        /**
         * @if zh
         * @brief 从字符串构造端点
         * @details 从字符串构造端点，格式为"ip:port"。
         *
         * @param str 字符串
         * @return 若字符串格式正确，则返回端点，否则返回空值
         *
         * @else
         * @brief Construct endpoint from string
         * @details Construct endpoint from string, format is "ip:port".
         *
         * @param str String
         * @return If the string format is correct, return the endpoint, otherwise return an empty value
         *
         * @endif
         */
        static std::expected<Endpoint, std::string> fromString(std::string_view str);

        [[nodiscard]] const IpAddress &address() const noexcept;

        [[nodiscard]] std::uint16_t port() const noexcept;

        [[nodiscard]] std::string toString() const;

    private:
        IpAddress addr_;

        std::uint16_t port_;
    };

    enum class SocketType { STREAM, DATAGRAM, RAW, SEQPACKET };

    enum class Protocol { TCP = IPPROTO_TCP, UDP = IPPROTO_UDP, ICMP = IPPROTO_ICMP, ICMPV6 = IPPROTO_ICMPV6 };

    enum class AddressFamily { IPV4 = AF_INET, IPV6 = AF_INET6, UNSPECIFIED = AF_UNSPEC };

    struct SocketOptions {
        bool reuse_address = false;
        bool keep_alive = false;
        bool no_delay = false;
        int receive_timeout_ms = 0;
        int send_timeout_ms = 0;
        int receive_buffer_size = 0;
        int send_buffer_size = 0;
    };

    class SocketHandle {
    public:
        SocketHandle() noexcept = default;

        explicit SocketHandle(AddressFamily family, SocketType type, Protocol proto = Protocol::TCP);

        ~SocketHandle();

        SocketHandle(SocketHandle &&other) noexcept;

        SocketHandle &operator=(SocketHandle &&other) noexcept;

        SocketHandle(const SocketHandle &) = delete;

        SocketHandle &operator=(const SocketHandle &) = delete;

        [[nodiscard]] bool isValid() const noexcept;

        [[nodiscard]] socket_t nativeHandle() const noexcept;

        void reset() noexcept;

        void swap(SocketHandle &other) noexcept;

    private:
        static constexpr socket_t invalidSocket = NET_INVALID_SOCKET;

        socket_t handle_ = invalidSocket;
    };

    class Socket {
    public:
        Socket() = default;

        Socket(AddressFamily family, SocketType type, Protocol proto = Protocol::TCP);

        void bind(const Endpoint &endpoint);

        void listen(int backlog = SOMAXCONN) const;

        [[nodiscard]] Socket accept() const;

        void connect(const Endpoint &endpoint) const;

        std::size_t send(std::span<const std::byte> data, int flags = 0) const;

        std::size_t receive(std::span<std::byte> buffer, int flags = 0) const;

        std::optional<std::size_t> sendWithTimeout(std::span<const std::byte> data, std::chrono::milliseconds timeout) const;

        void setOptions(const SocketOptions& opts) const;

        void setNonBlocking(bool nonBlocking = true);

        void close();

        [[nodiscard]] bool isValid() const noexcept;

    private:
        SocketHandle handle_;

        friend class SocketHandle;
    };

} // namespace tiny_web_server::net


#endif // TINY_WEB_SERVER_SOCKET_HPP
