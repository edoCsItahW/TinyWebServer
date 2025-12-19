// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for
// commercial purposes is prohibited without the author's permission. If you have any
// questions or require permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file reactor_iocp.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/18 21:53
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#include "tws/async/reactor_iocp.hpp"

#if WEB_SERVER_WINDOWS

namespace tiny_web_server::async {

    Reactor::Impl::Impl() {
        iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);

        if (!iocpHandle)
            throw std::system_error(
                GetLastError(), std::system_category(), "CreateIoCompletionPort failed"
            );

        workerThread = std::thread([this] { runIOCP(); });
    }

    Reactor::Impl::~Impl() {
        running = false;

        PostQueuedCompletionStatus(iocpHandle, 0, 0, nullptr);

        if (workerThread.joinable()) workerThread.join();

        CloseHandle(iocpHandle);
    }

    void Reactor::Impl::runIOCP() {
        while (running) {
            DWORD bytesTransferred  = 0;
            ULONG_PTR completionKey = 0;
            LPOVERLAPPED overlapped = nullptr;

            BOOL result = GetQueuedCompletionStatus(
                iocpHandle, &bytesTransferred, &completionKey, &overlapped, INFINITE
            );

            if (!result) break;

            if (overlapped) {
                auto* op = CONTAINING_RECORD(overlapped, Operation, overlapped);

                op->bytesTransferred = bytesTransferred;

                if (op->handle) op->handle.resume();

                if (op->cleanup) op->cleanup();
            }
        }
    }

    void Reactor::Impl::destroyOperation(Operation* operation) {
        operation->~Operation();

        memoryPool.deallocate(operation, sizeof(*operation), alignof(*operation));
    }

    AcceptOperation::AcceptOperation(net::Socket* listener, net::Socket* acceptedSocket)
        : listener(listener)
        , acceptedSocket(acceptedSocket) {
        std::memset(&overlapped, 0, sizeof(overlapped));
    }

}  // namespace tiny_web_server::async

#endif
