// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file socket_handle.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/17 17:07
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#include "../../exception.hpp"
#include "socket_handle.hpp"

namespace tiny_web_server::net {

    SocketHandle::SocketHandle(AddressFamily family, SocketType type, Protocol proto) {
        int sockType = 0;

        switch (type) {
            using enum SocketType;
            case STREAM:
                sockType = SOCK_STREAM;
                break;
            case DATAGRAM:
                sockType = SOCK_DGRAM;
                break;
            case RAW:
                sockType = SOCK_RAW;
                break;
            case SEQPACKET:
                sockType = SOCK_SEQPACKET;
                break;
        }

        handle_ = socket(static_cast<int>(family), sockType, static_cast<int>(proto));
        if (handle_ == invalidSocket)
            throw exception::SocketError("Failed to create socket");
    }

    SocketHandle::~SocketHandle() {
        if (isValid())
            NET_CLOSE(handle_);
    }

    SocketHandle::SocketHandle(SocketHandle &&other) noexcept : handle_{other.handle_} {
        other.handle_ = invalidSocket;
    }

    SocketHandle &SocketHandle::operator=(SocketHandle &&other) noexcept {
        if (this != &other) {
            if (isValid())
                NET_CLOSE(handle_);

            handle_ = other.handle_;
            other.handle_ = invalidSocket;
        }

        return *this;
    }

    bool SocketHandle::isValid() const noexcept { return handle_ != invalidSocket; }

    socket_t SocketHandle::nativeHandle() const noexcept { return handle_; }

    void SocketHandle::reset() noexcept {
        if (isValid()) {
            NET_CLOSE(handle_);

            handle_ = invalidSocket;
        }
    }

    void SocketHandle::swap(SocketHandle &other) noexcept { std::swap(handle_, other.handle_); }


} // namespace tiny_web_server::net
