// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file platform.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/16 10:31
 *
 * @if zh
 * @brief 平台检测头文件
 *
 * @else
 * @brief Platform detection header file
 *
 * @endif
 *
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#ifndef TINY_WEB_SERVER_PLATFORM_HPP
#define TINY_WEB_SERVER_PLATFORM_HPP
#pragma once

/// @addtogroup macro
/// @{

/**
 * @if zh
 * @brief 平台检测宏
 *
 * @else
 * @brief Platform detection macro
 *
 * @endif
 */

// Windows
#if defined(_WIN32) || defined(_WIN64)

    #define WEB_SERVER_WINDOWS 1
    #define WEB_SERVER_POSIX 0

    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif

    #ifndef _WINSOCKAPI_
        #define _WINSOCKAPI_
    #endif

    #include <windows.h>
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <mswsock.h>

    #ifdef __MINGW32__
        #ifdef ULONG
            using ULONG = unsigned long;
        #endif
        #ifndef ULONG_PTR
            using ULONG_PTR = std::uintptr_t;
        #endif
        #ifdef DWORD_PTR
            using DWORD_PTR = std::uintptr_t;
        #endif

    #endif

    #pragma comment(lib, "ws2_32.lib")
    #pragma comment(lib, "mswsock.lib")

// Linux
#elif defined(__unix__) || defined(__APPLE__) || defined(__unix__)

    #define WEB_SERVER_POSIX 1
    #define WEB_SERVER_WINDOWS 0

    #include <arpa/inet.h>
    #include <fcntl.h>
    #include <netinet/in.h>
    #include <sys/epoll.h>
    #include <sys/socket.h>
    #include <unistd.h>

    #ifdef __linux__
        #define WEB_SERVER_LINUX 1
        #include <liburing.h>

    #else
        #define WEB_SERVER_LINUX 0

    #endif

#else

    #error "Unsupported platform"

#endif

/**
 * @if zh
 * @brief 统一错误码处理
 *
 * @else
 * @brief Unified error code processing
 *
 * @endif
 */

#if WEB_SERVER_WINDOWS

    /** @def NET_ERROR
     *
     * @if zh
     * @brief 网络错误码
     * @details 使用 @c WSAGetLastError() 获取错误码
     *
     * @else
     * @brief Network error code
     * @details Use @c WSAGetLastError() to get the error code
     *
     * @endif
     */
    #define NET_ERROR WSAGetLastError()

    /** @def NET_CLOSE
     *
     * @if zh
     * @brief 关闭套接字
     * @details 使用 @c closesocket() 关闭套接字
     *
     * @else
     * @brief Close socket
     * @details Use @c closesocket() to close the socket
     *
     * @endif
     */
    #define NET_CLOSE closesocket

    /** @def NET_INVALID_SOCKET
     *
     * @if zh
     * @brief 无效套接字
     * @details 定义为 @c INVALID_SOCKET
     *
     * @else
     * @brief Invalid socket
     * @details Define as @c INVALID_SOCKET
     *
     * @endif
     */
    #define NET_INVALID_SOCKET INVALID_SOCKET

    /** @typedef socket_t
     *
     * @if zh
     * @brief 套接字类型
     * @details 统一使用 @c SOCKET 类型
     *
     * @else
     * @brief Socket type
     * @details Unified use of @c SOCKET type
     *
     * @endif
     */
    using socket_t = SOCKET;

/** @typedef ssize_t
 *
 * @if zh
 * @brief 统一使用 @c ssize_t 类型
 * @details 不同平台的 @c ssize_t 类型可能不同，统一使用 @c ssize_t 类型
 *
 * @else
 * @brief Unified use of @c ssize_t type
 * @details The type of @c ssize_t may be different on different platforms, unified use of @c ssize_t type
 *
 * @endif
 */
    #define ssize_t SSIZE_T

#else

    /** @def NET_ERROR
     *
     * @if zh
     * @brief 网络错误码
     * @details 使用 @c errno 获取错误码
     *
     * @else
     * @brief Network error code
     * @details Use @c errno to get the error code
     *
     * @endif
     */
    #define NET_ERROR errno

    /** @def NET_CLOSE
     *
     * @if zh
     * @brief 关闭套接字
     * @details 使用 @c close 关闭套接字
     *
     * @else
     * @brief Close socket
     * @details Use @c close to close the socket
     *
     * @endif
     */
    #define NET_CLOSE close

    /** @def NET_INVALID_SOCKET
     *
     * @if zh
     * @brief 无效套接字
     * @details 定义为 @c -1
     *
     * @else
     * @brief Invalid socket
     * @details Define as @c -1
     *
     * @endif
     */
    #define NET_INVALID_SOCKET (-1)

    /** @typedef socket_t
     *
     * @if zh
     * @brief 套接字类型
     * @details 统一使用 @c int 类型
     *
     * @else
     * @brief Socket type
     * @details Unified use of @c int type
     *
     * @endif
     */
    using socket_t = int;

#endif

/// @}

#endif // TINY_WEB_SERVER_PLATFORM_HPP
