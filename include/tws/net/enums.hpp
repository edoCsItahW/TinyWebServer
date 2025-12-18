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
 * @date 2025/12/18 12:19
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#ifndef TINY_WEB_SERVER_NET_ENUMS_HPP
#define TINY_WEB_SERVER_NET_ENUMS_HPP
#pragma once

#include "tws/platform.hpp"


namespace tiny_web_server::net {

    /** @enum AddressFamily
     *
     * @brief 地址族枚举
     * @details 地址族枚举，用于指定IP地址的类型
     */
    enum class AddressFamily {
        /// IPv4地址族，对应AF_INET
        IPv4 = AF_INET,
        /// IPv6地址族，对应AF_INET6
        IPv6 = AF_INET6,
        /// 红外地址族，对应AF_IRDA
        RDA = AF_IRDA,
        /// 蓝牙地址族，对应AF_BTH
        BTH = AF_BTH
    };

    /** @enum SocketType
     *
     * @brief 套接字类型枚举
     * @details 套接字类型枚举，用于指定套接字的类型
     */
    enum class SocketType {
        /**
         * @brief 字节流套接字，对应SOCK_STREAM
         * @details 一种套接字类型，提供带有OOB数据传输机制的顺序，可靠，双向，基于连接的字节流。 此套接字类型使用传输控制协议（TCP）作为Internet地址系列（AF_INET或AF_INET6）。
         */
        STREAM = SOCK_STREAM,
        /**
         * @brief 数据报套接字，对应SOCK_DGRAM
         * @details 一种支持数据报的套接字类型，它是固定（通常很小）最大长度的无连接，不可靠的缓冲区。 此套接字类型使用用户数据报协议（UDP）作为Internet地址系列（AF_INET或AF_INET6）。
         */
        DGRAM = SOCK_DGRAM,
        /**
         * @brief 原始套接字，对应SOCK_RAW
         * @details 一种套接字类型，提供允许应用程序操作下一个上层协议头的原始套接字。 要操作IPv4标头，必须在套接字上设置IP_HDRINCL套接字选项。
         * 要操作IPv6标头，必须在套接字上设置IPV6_HDRINCL套接字选项。
         */
        RAW = SOCK_RAW,
        /**
         * @brief 路由套接字，对应SOCK_RDM
         * @details 一种套接字类型，提供可靠的消息数据报。 这种类型的一个示例是Windows中的实用通用多播（PGM）多播协议实现，通常称为可靠多播节目。仅在安装了可靠多播协议时才支持此类型值。
         */
        RDM = SOCK_RDM,
        /**
         * @brief 序列包套接字，对应SOCK_SEQPACKET
         * @details 一种套接字类型，提供基于数据报的伪流数据包。
         */
        SEQPACKET = SOCK_SEQPACKET
    };

    /** @enum Protocol
     *
     * @brief 协议枚举
     * @details 协议枚举，用于指定套接字的协议
     */
    enum class Protocol {
        /**
         * @brief 传输控制协议，对应IPPROTO_TCP
         * @details 传输控制协议（TCP）。 当af参数为AF_INET或AF_INET6且类型参数为SOCK_STREAM时，这是一个可能的值。
         */
        TCP = IPPROTO_TCP,
        /**
         * @brief 用户数据报协议，对应IPPROTO_UDP
         * @details 用户数据报协议（UDP）。 当af参数为AF_INET或AF_INET6且类型参数为SOCK_DGRAM时，这是一个可能的值。
         */
        UDP = IPPROTO_UDP,
        /**
         * @brief Internet控制报文协议，对应IPPROTO_ICMP
         * @details Internet控制消息协议（ICMP）。 当af参数为AF_UNSPEC，AF_INET或AF_INET6且类型参数为SOCK_RAW或未指定时，这是一个可能的值。
         */
        ICMP = IPPROTO_ICMP,
        /**
         * @brief 网际组管理协议，对应IPPROTO_IGMP
         * @details Internet组管理协议（IGMP）。 当af参数为AF_UNSPEC，AF_INET或AF_INET6且类型参数为SOCK_RAW或未指定时，这是一个可能的值。
         */
        IGMP = IPPROTO_IGMP,
        /**
         * @brief PGM协议，对应IPPROTO_PGM
         * @details 用于可靠多播的PGM协议。 当af参数为AF_INET且类型参数为SOCK_RDM时，这是一个可能的值。 在针对Windows Vista及更高版本发布的Windows SDK上，此协议也称为IPPROTO_PGM。仅在安装了可靠多播协议时才支持此协议值。
         */
        PGM = IPPROTO_PGM,
        /**
         * @brief 不指定协议
         * @details 调用者不希望指定协议，服务提供商将选择要使用的协议。
         */
        NONE = 0
    };

} // namespace tiny_web_server::net

#endif // TINY_WEB_SERVER_NET_ENUMS_HPP
