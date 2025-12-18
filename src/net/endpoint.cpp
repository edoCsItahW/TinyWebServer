// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for
// commercial purposes is prohibited without the author's permission. If you have any
// questions or require permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file endpoint.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/18 12:53
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#include "../../include/tws/net/endpoint.hpp"
#include "tws/exception.hpp"

namespace tiny_web_server::net {

    Endpoint::Endpoint(const IpAddress& address, const std::uint16_t port)
        : address_(address)
        , port_(port) {}

    Endpoint::Endpoint(const std::string_view str) {
        auto pos = str.find_last_of(':');
        if (pos == std::string::npos)
            throw SocketError<"Invalid endpoint format, expected 'ip:port'"_s>();

        address_ = IpAddress{str.substr(0, pos)};
        port_    = std::stoi(std::string(str.substr(pos + 1)));

        if (port_ < 0 || port_ > 65535)
            throw SocketError<>(std::format("Invalid port number: {}", port_));
    }

    const IpAddress& Endpoint::address() const noexcept { return address_; }

    std::uint16_t Endpoint::port() const noexcept { return port_; }

    std::string Endpoint::toString() const {
        return std::format("{}:{}", address_.toString(), port_);
    }

}  // namespace tiny_web_server::net
