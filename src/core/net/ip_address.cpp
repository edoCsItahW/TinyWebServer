// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file ip_address.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/17 16:28
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#include "../../exception.hpp"
#include "ip_address.hpp"

namespace tiny_web_server::net {

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
                throw exception::SocketError("Insufficient bytes for IPv6 address");

            in6_addr addr{};
            std::memcpy(&addr, bytes.data(), 16);
            return IpAddress{addr};
        }

        // IPv4
        if (bytes.size() < 4) // IPv4最小字节数为4
            throw exception::SocketError("Insufficient bytes for IPv4 address");

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

} // namespace tiny_web_server::net
