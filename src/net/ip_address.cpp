// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for
// commercial purposes is prohibited without the author's permission. If you have any
// questions or require permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file ip_address.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/18 12:55
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#include "tws/net/ip_address.hpp"
#include "tws/exception.hpp"

#if WEB_SERVER_LINUX
#include <cstring>
#endif

namespace tiny_web_server::net {

    IpAddress::IpAddress(const std::string_view str) {
        // IPv6
        if (str.contains(":")) {
            if (in6_addr addr{}; inet_pton(AF_INET6, str.data(), &addr) == 1)
                address_ = addr;
        }

        // IPv4
        else if (in_addr addr{}; inet_pton(AF_INET, str.data(), &addr) == 1)
            address_ = addr;

        throw SocketError<"Ip address parsing error"_s>();
    }

    IpAddress::IpAddress(const std::span<const std::byte> bytes, const bool ipv6) {
        // IPv6
        if (ipv6) {
            if (bytes.size() < 16)
                throw SocketError<"Insufficient bytes for IPv6 address"_s>();

            in6_addr addr{};
            std::memcpy(&addr, bytes.data(), 16);
            address_ = addr;
        }

        // IPv4
        else {
            if (bytes.size() < 4)
                throw SocketError<"Insufficient bytes for IPv4 address"_s>();

            in_addr addr{};
            std::memcpy(&addr, bytes.data(), 4);
            address_ = addr;
        }
    }

    std::string IpAddress::toString() const {
        char buffer[INET6_ADDRSTRLEN]{};

        // IPv6
        if (std::holds_alternative<in6_addr>(address_))
            inet_ntop(AF_INET6, &std::get<in6_addr>(address_), buffer, sizeof(buffer));

        // IPv4
        else if (std::holds_alternative<in_addr>(address_))
            inet_ntop(AF_INET, &std::get<in_addr>(address_), buffer, sizeof(buffer));

        return buffer;
    }

    std::vector<std::byte> IpAddress::toBytes() const {
        std::vector<std::byte> bytes;

        // IPv4
        if (std::holds_alternative<in_addr>(address_)) {
            const auto* data =
                reinterpret_cast<const std::byte*>(&std::get<in_addr>(address_));

            bytes.assign(data, data + 4);
        }

        // IPv6
        else if (std::holds_alternative<in6_addr>(address_)) {
            const auto* data =
                reinterpret_cast<const std::byte*>(&std::get<in6_addr>(address_));

            bytes.assign(data, data + 16);
        }

        return bytes;
    }

    bool IpAddress::isIPv4() const noexcept {
        return std::holds_alternative<in_addr>(address_);
    }

    bool IpAddress::isIPv6() const noexcept {
        return std::holds_alternative<in6_addr>(address_);
    }

    IpAddress IpAddress::any(const bool ipv6) {
        // IPv6
        if (ipv6) return in6addr_any;

        // IPv4
        in_addr addr{};
        addr.s_addr = INADDR_ANY;

        return addr;
    }

    IpAddress IpAddress::loopback(bool ipv6) {
        // IPv6
        if (ipv6) return in6addr_loopback;

        // IPv4
        in_addr addr{};
        addr.s_addr = INADDR_LOOPBACK;

        return addr;
    }

    std::variant<in_addr, in6_addr> IpAddress::getAddress() const {
        return address_;
    }

    std::size_t IpAddress::hash() const {
        // IPv4
        if (isIPv4()) return std::hash<uint32_t>()(std::get<in_addr>(address_).s_addr);

        // IPv6
        else {
            const auto& addr = std::get<in6_addr>(address_);
            std::size_t hash = 0;

            for (auto i = 0; i < 4; i++)
                hash ^= std::hash<uint32_t>{}(reinterpret_cast<const uint32_t*>(&addr)[i]);

            return hash;
        }
    }

    bool IpAddress::operator==(const IpAddress& other) const {
        if (address_.index() != other.address_.index()) return false;

        // IPv4
        if (isIPv4())
            return std::get<in_addr>(address_).s_addr
                == std::get<in_addr>(other.address_).s_addr;

        // IPv6
        return memcmp(
                   &std::get<in6_addr>(address_), &std::get<in6_addr>(other.address_),
                   sizeof(in6_addr)
               )
            == 0;
    }

    bool IpAddress::operator!=(const IpAddress& other) const { return !(*this == other); }

    IpAddress::IpAddress(in_addr addr)
        : address_(addr) {}

    IpAddress::IpAddress(in6_addr addr)
        : address_(addr) {}

}  // namespace tiny_web_server::net
