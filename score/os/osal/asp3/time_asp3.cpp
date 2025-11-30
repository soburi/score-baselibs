/*******************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
 *
 * See the NOTICE file(s) distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#include "score/os/time.hpp"

#include <chrono>

#include "score/os/osal/asp3/config_asp3.hpp"

namespace score::os {

TimePoint now() {
    SYSUTM timestamp{0};
    if (get_utm(&timestamp) != asp3::kOk) {
        return std::chrono::steady_clock::now();
    }
    return TimePoint(Duration(std::chrono::nanoseconds(timestamp)));
}

OsError sleep_for(Duration duration) {
    const auto micro = std::chrono::duration_cast<std::chrono::microseconds>(duration);
    const auto ticks = static_cast<RELTIM>(micro.count());
    if (dly_tsk(ticks) != asp3::kOk) {
        return OsError::kUnknown;
    }
    return OsError::kOk;
}

}  // namespace score::os
