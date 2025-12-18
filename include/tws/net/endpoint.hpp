// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for
// commercial purposes is prohibited without the author's permission. If you have any
// questions or require permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file endpoint.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/18 12:53
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#ifndef TINY_WEB_SERVER_ENDPOINT_HPP
#define TINY_WEB_SERVER_ENDPOINT_HPP
#pragma once
#include "ip_address.hpp"

namespace tiny_web_server::net {

    struct Endpoint {
    private:
        IpAddress address_;

        std::uint16_t port_;

    public:
        Endpoint(const IpAddress& address, std::uint16_t port);

        Endpoint(std::string_view str);

        [[nodiscard]] const IpAddress& address() const noexcept;

        [[nodiscard]] std::uint16_t port() const noexcept;

        [[nodiscard]] std::string toString() const;
    };

}  // namespace tiny_web_server::net

#endif  // TINY_WEB_SERVER_ENDPOINT_HPP
