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

#include <array>
#include <utility>

#include "score/os/osal/asp3/config_asp3.hpp"

namespace score::os {
namespace {
std::array<Thread::Entry, asp3::kMaxTasks> g_entries{};
std::array<bool, asp3::kMaxTasks> g_completed{};

extern "C" void score_os_thread_entry(std::intptr_t exinf) {
    const std::size_t slot = static_cast<std::size_t>(exinf);
    if (slot < g_entries.size() && g_entries[slot]) {
        g_entries[slot]();
        g_entries[slot] = nullptr;
    }
    if (slot < g_completed.size()) {
        g_completed[slot] = true;
    }
    ext_tsk();
}

std::size_t acquire_slot() {
    for (std::size_t i = 0; i < g_entries.size(); ++i) {
        if (!g_entries[i]) {
            return i;
        }
    }
    return g_entries.size();
}

OsError wait_until_finished(std::size_t slot) {
    if (slot >= g_completed.size()) {
        return OsError::kInvalidArgument;
    }
    while (!g_completed[slot]) {
        if (dly_tsk(1U) != asp3::kOk) {
            return OsError::kUnknown;
        }
    }
    return OsError::kOk;
}

}  // namespace

class Thread::Impl {
public:
    Impl() = default;
    explicit Impl(Entry entry) { start(std::move(entry)); }
    ~Impl() = default;

    bool joinable() const { return slot_ < asp3::kMaxTasks && !g_completed[slot_]; }

    OsError join() {
        if (!joinable()) {
            return OsError::kInvalidArgument;
        }
        return wait_until_finished(slot_);
    }

private:
    void start(Entry entry) {
        slot_ = acquire_slot();
        if (slot_ >= asp3::kMaxTasks) {
            slot_ = asp3::kMaxTasks;
            return;
        }
        g_entries[slot_] = std::move(entry);
        g_completed[slot_] = false;

        const ID tskid = static_cast<ID>(asp3::kTaskIdBase + slot_);
        T_CTSK config{};
        config.tskatr = asp3::kTaskAttributes;
        config.itskpri = asp3::kDefaultPriority;
        config.stksz = asp3::kDefaultStackSize;
        config.stk = nullptr;
        config.tskadr = score_os_thread_entry;
        config.exinf = static_cast<std::intptr_t>(slot_);

        if (cre_tsk(tskid, &config) != asp3::kOk) {
            slot_ = asp3::kMaxTasks;
            return;
        }
        act_tsk(tskid);
    }

    std::size_t slot_{asp3::kMaxTasks};
};

Thread::Thread() = default;

Thread::Thread(Entry entry) : impl_{std::make_unique<Impl>(std::move(entry))} {}

Thread::Thread(Thread&& other) noexcept = default;
Thread& Thread::operator=(Thread&& other) noexcept = default;
Thread::~Thread() = default;

bool Thread::joinable() const { return impl_ && impl_->joinable(); }

OsError Thread::join() { return impl_ ? impl_->join() : OsError::kInvalidArgument; }

}  // namespace score::os
