// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file endpoint.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/17 16:30
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#include "endpoint.hpp"

#include <format>

namespace tiny_web_server::net {

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

} // namespace tiny_web_server::net
