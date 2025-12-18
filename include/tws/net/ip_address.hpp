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
 * @date 2025/12/18 12:55
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#ifndef TINY_WEB_SERVER_IP_ADDRESS_HPP
#define TINY_WEB_SERVER_IP_ADDRESS_HPP
#pragma once

#include <span>
#include <string>
#include <variant>
#include <vector>

#include "../platform.hpp"


namespace tiny_web_server::net {

    struct IpAddress {
    private:
        std::variant<in_addr, in6_addr> address_;

    public:
        IpAddress() = default;

        IpAddress(std::string_view str);

        IpAddress(std::span<const std::byte> bytes, bool ipv6 = false);

        [[nodiscard]] std::string toString() const;

        [[nodiscard]] std::vector<std::byte> toBytes() const;

        [[nodiscard]] bool isIPv4() const noexcept;

        [[nodiscard]] bool isIPv6() const noexcept;

        static IpAddress any(bool ipv6 = false);

        static IpAddress loopback(bool ipv6 = false);

        [[nodiscard]] std::variant<in_addr, in6_addr> getAddress() const;

        bool operator==(const IpAddress &other) const;

        bool operator!=(const IpAddress &other) const;

        std::size_t hash() const;

    private:
        IpAddress(in_addr addr);

        IpAddress(in6_addr addr);

    };

} // namespace tiny_web_server::net

#endif // TINY_WEB_SERVER_IP_ADDRESS_HPP
