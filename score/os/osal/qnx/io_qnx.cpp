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

#include "score/os/io.hpp"

#include <unistd.h>

namespace score::os {

OsError write(const char* buffer, std::size_t length, std::size_t& written) {
    if (buffer == nullptr) {
        return OsError::kInvalidArgument;
    }
    ssize_t result = ::write(STDOUT_FILENO, buffer, length);
    if (result < 0) {
        written = 0U;
        return OsError::kUnknown;
    }
    written = static_cast<std::size_t>(result);
    return OsError::kOk;
}

}  // namespace score::os
