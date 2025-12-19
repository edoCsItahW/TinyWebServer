// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for
// commercial purposes is prohibited without the author's permission. If you have any
// questions or require permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file reactor_io_uring.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/19 13:14
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#ifndef TEST_TINY_WEB_SERVER_REACTOR_IO_URING_INL
#define TEST_TINY_WEB_SERVER_REACTOR_IO_URING_INL
#pragma once


namespace tiny_web_server::async {

    template<typename OP, typename... Args>
    OP* Reactor::Impl::createOperation(Args&&... args) {
        void* mem = memoryPool.allocate(sizeof(OP), alignof(OP));

        return new (mem) OP(std::forward<Args>(args)...);
    }

}  // namespace tiny_web_server::async

#endif  // TEST_TINY_WEB_SERVER_REACTOR_IO_URING_INL
