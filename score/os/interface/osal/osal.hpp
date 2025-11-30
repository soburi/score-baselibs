#pragma once

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

#include <cstdint>

namespace score::os {

enum class OsError {
    kOk = 0,
    kUnavailable,
    kNoResources,
    kTimeout,
    kInvalidArgument,
    kUnknown,
};

/// Initialize the underlying OS abstraction layer.
void os_init();

}  // namespace score::os
