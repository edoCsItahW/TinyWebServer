// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file endpoint.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/17 16:30
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#ifndef TINY_WEB_SERVER_ENDPOINT_HPP
#define TINY_WEB_SERVER_ENDPOINT_HPP
#pragma once

#include <expected>
#include "ip_address.hpp"

namespace tiny_web_server::net {

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

} // namespace tiny_web_server::net

#endif // TINY_WEB_SERVER_ENDPOINT_HPP
