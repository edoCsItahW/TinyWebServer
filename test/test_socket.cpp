// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file test_socket.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/17 15:42
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#include <iostream>
#include "../src/core/net/socket.hpp"

using namespace tiny_web_server::net;

void test_tcp_server() {
    try {
        static SocketInitializer init;

        Endpoint endpoint(IpAddress::any(), 8080);



    } catch (const SocketError& e) {
        std::cerr << "Network error: " << e.what() << std::endl;
    }
}
