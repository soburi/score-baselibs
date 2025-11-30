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

#include <functional>
#include <memory>

#include "score/os/osal.hpp"

namespace score::os {

class Thread {
public:
    using Entry = std::function<void()>;

    Thread();
    explicit Thread(Entry entry);
    Thread(Thread&& other) noexcept;
    Thread& operator=(Thread&& other) noexcept;
    Thread(const Thread&) = delete;
    Thread& operator=(const Thread&) = delete;
    ~Thread();

    bool joinable() const;
    OsError join();

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace score::os
