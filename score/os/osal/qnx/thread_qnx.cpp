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

#include "score/os/thread.hpp"

#include <thread>
#include <utility>

namespace score::os {

class Thread::Impl {
public:
    Impl() = default;
    explicit Impl(Entry entry) : thread_{std::move(entry)} {}
    ~Impl() {
        if (thread_.joinable()) {
            thread_.join();
        }
    }

    bool joinable() const { return thread_.joinable(); }
    OsError join() {
        if (!thread_.joinable()) {
            return OsError::kInvalidArgument;
        }
        thread_.join();
        return OsError::kOk;
    }

private:
    std::thread thread_{};
};

Thread::Thread() = default;

Thread::Thread(Entry entry) : impl_{std::make_unique<Impl>(std::move(entry))} {}

Thread::Thread(Thread&& other) noexcept = default;
Thread& Thread::operator=(Thread&& other) noexcept = default;
Thread::~Thread() = default;

bool Thread::joinable() const { return impl_ && impl_->joinable(); }

OsError Thread::join() { return impl_ ? impl_->join() : OsError::kInvalidArgument; }

}  // namespace score::os
