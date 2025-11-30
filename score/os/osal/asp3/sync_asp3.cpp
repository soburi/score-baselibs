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

#include <array>

#include "score/os/osal/asp3/config_asp3.hpp"

namespace score::os {
namespace {
std::array<bool, asp3::kMaxMutexes> g_mutex_state{};

ID mutex_id_for_index(std::size_t index) {
    return static_cast<ID>(asp3::kMutexIdBase + static_cast<ID>(index));
}
}  // namespace

class Mutex::Impl {
public:
    std::size_t index{asp3::kMaxMutexes};
};

Mutex::Mutex() : impl_{new Impl()} {
    for (std::size_t i = 0; i < g_mutex_state.size(); ++i) {
        if (g_mutex_state[i]) {
            continue;
        }
        const ID mtxid = mutex_id_for_index(i);
        T_CMTX config{};
        config.mtxatr = asp3::kMutexAttributes;
        config.ceilpri = asp3::kDefaultPriority;
        if (cre_mtx(mtxid, &config) == asp3::kOk) {
            g_mutex_state[i] = true;
            impl_->index = i;
            break;
        }
    }
}

Mutex::Mutex(Mutex&& other) noexcept : impl_{other.impl_} { other.impl_ = nullptr; }

Mutex& Mutex::operator=(Mutex&& other) noexcept {
    if (this != &other) {
        delete impl_;
        impl_ = other.impl_;
        other.impl_ = nullptr;
    }
    return *this;
}

Mutex::~Mutex() {
    if (impl_ != nullptr && impl_->index < g_mutex_state.size()) {
        const ID mtxid = mutex_id_for_index(impl_->index);
        del_mtx(mtxid);
        g_mutex_state[impl_->index] = false;
    }
    delete impl_;
}

OsError Mutex::lock() {
    if (impl_ == nullptr || impl_->index >= g_mutex_state.size()) {
        return OsError::kInvalidArgument;
    }
    const ID mtxid = mutex_id_for_index(impl_->index);
    return loc_mtx(mtxid) == asp3::kOk ? OsError::kOk : OsError::kUnknown;
}

OsError Mutex::unlock() {
    if (impl_ == nullptr || impl_->index >= g_mutex_state.size()) {
        return OsError::kInvalidArgument;
    }
    const ID mtxid = mutex_id_for_index(impl_->index);
    return unl_mtx(mtxid) == asp3::kOk ? OsError::kOk : OsError::kUnknown;
}

LockGuard::LockGuard(Mutex& mutex) : mutex_{mutex} { mutex_.lock(); }

LockGuard::~LockGuard() { mutex_.unlock(); }

}  // namespace score::os
