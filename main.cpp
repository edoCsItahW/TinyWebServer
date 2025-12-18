// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file main.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/15 23:30
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */

#include <array>
#include <iostream>

#include "src/core/async/reactor.hpp"
#include "src/core/net/socket.hpp"
#include "src/core/net/socket_initializer.hpp"

using namespace tiny_web_server;

void test_tcp_server() {
    using namespace net;

    SocketInitializer init;

    Socket serverSocket(AddressFamily::IPV4, SocketType::STREAM);

    Endpoint endpoint(IpAddress::any(), 15234);

    serverSocket.bind(endpoint);
    serverSocket.listen();
    serverSocket.setNonBlocking(true);
}


struct TestHandler {
    void onEvent(socket_t socket, const async::EventData &data) {
        if (data.bytesTransferred > 0)
            std::cout << "Received data from client: " << data.bytesTransferred << std::endl;
    }

    void onError(socket_t socket, int error) { std::cout << "Error: " << error << std::endl; }
};


auto test_reactor() -> async::Task<> {
    using namespace net;
    using namespace async;

    try {
        SocketInitializer init;

        Reactor reactor;

        Socket serverSocket(AddressFamily::IPV4, SocketType::STREAM);

        Endpoint endpoint(IpAddress::any(), 8080);

        serverSocket.bind(endpoint);
        serverSocket.listen();

        auto clientSocket = serverSocket.accept();

        clientSocket.setNonBlocking(true);

        TestHandler handler;

        std::array<std::byte, 1024> buffer{};

        reactor.registerSocket(clientSocket, EventType::READ, TestHandler{});

        reactor.run();

        co_await reactor.asyncRead(clientSocket.getHandle().nativeHandle(), std::span<std::byte>{buffer}, handler);

        reactor.stop();

    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

int main() {
    auto task = test_reactor();

    task.resume();

    std::this_thread::sleep_for(std::chrono::seconds(10));

    return 0;
}
