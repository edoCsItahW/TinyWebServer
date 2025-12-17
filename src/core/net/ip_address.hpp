// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file ip_address.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/17 16:28
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#ifndef TINY_WEB_SERVER_IP_ADDRESS_HPP
#define TINY_WEB_SERVER_IP_ADDRESS_HPP
#pragma once

#include <optional>
#include <span>
#include <string>
#include <variant>
#include <vector>
#include "../../platform.hpp"


namespace tiny_web_server::net {

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

        bool operator==(const IpAddress &other) const;

        bool operator!=(const IpAddress &other) const;

        std::size_t hash() const;

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

} // namespace tiny_web_server::net

#endif // TINY_WEB_SERVER_IP_ADDRESS_HPP
