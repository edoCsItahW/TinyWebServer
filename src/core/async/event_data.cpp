// Copyright (c) 2025. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file event_data.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2025/12/17 17:41
 * @brief
 * @copyright CC BY-NC-SA 2025. All rights reserved.
 * */
#include "event_data.hpp"

namespace tiny_web_server::async {

    EventData::operator bool() const noexcept { return bytesTransferred > 0 || eventType == EventType::DISCONNECT; }

} // namespace tiny_web_server::async
