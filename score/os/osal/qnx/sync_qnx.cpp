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

#include "score/os/sync.hpp"

#include <mutex>

namespace score::os {

class Mutex::Impl {
public:
    std::mutex native_{};
};

Mutex::Mutex() : impl_{new Impl()} {}

Mutex::Mutex(Mutex&& other) noexcept : impl_{other.impl_} { other.impl_ = nullptr; }

Mutex& Mutex::operator=(Mutex&& other) noexcept {
    if (this != &other) {
        delete impl_;
        impl_ = other.impl_;
        other.impl_ = nullptr;
    }
    return *this;
}

Mutex::~Mutex() { delete impl_; }

OsError Mutex::lock() {
    if (impl_ == nullptr) {
        return OsError::kInvalidArgument;
    }
    impl_->native_.lock();
    return OsError::kOk;
}

OsError Mutex::unlock() {
    if (impl_ == nullptr) {
        return OsError::kInvalidArgument;
    }
    impl_->native_.unlock();
    return OsError::kOk;
}

LockGuard::LockGuard(Mutex& mutex) : mutex_{mutex} { mutex_.lock(); }

LockGuard::~LockGuard() { mutex_.unlock(); }

}  // namespace score::os
