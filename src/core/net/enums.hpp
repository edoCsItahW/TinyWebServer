// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file enums.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/17 17:14
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#ifndef TINY_WEB_SERVER_NET_ENUMS_HPP
#define TINY_WEB_SERVER_NET_ENUMS_HPP
#pragma once

#include "../../platform.hpp"

namespace tiny_web_server::net {

    enum class SocketType { STREAM, DATAGRAM, RAW, SEQPACKET };

    enum class Protocol { TCP = IPPROTO_TCP, UDP = IPPROTO_UDP, ICMP = IPPROTO_ICMP, ICMPV6 = IPPROTO_ICMPV6 };

    enum class AddressFamily { IPV4 = AF_INET, IPV6 = AF_INET6, UNSPECIFIED = AF_UNSPEC };

} // namespace tiny_web_server::net

#endif // TINY_WEB_SERVER_NET_ENUMS_HPP
