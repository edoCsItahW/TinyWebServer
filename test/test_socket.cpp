// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for
// commercial purposes is prohibited without the author's permission. If you have any
// questions or require permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file test_socket.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/17 15:42
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#include "tws/net/socket.hpp"
#include <iostream>
#include <thread>
#include <array>


using namespace tiny_web_server;

void test_tcp_server() {
    using namespace net;

    try {
        Socket serverSocket(AddressFamily::IPv4, SocketType::STREAM);

        serverSocket.bind({IpAddress::any(), 8088});
        serverSocket.listen();

        Socket clientSocket = serverSocket.accept();

        while (true) {
            std::array<std::byte, 1024> buffer;

            auto result = clientSocket.recv(buffer);

            auto message = reinterpret_cast<char*>(buffer.data());
            message[result] = '\0';

            std::cout << "Received message: " << message << std::endl;

           std::this_thread::sleep_for(std::chrono::seconds(1));
        }

    } catch (const std::exception& e) { std::cerr << e.what() << '\n'; }
}

int main() {
    test_tcp_server();

    return 0;
}
