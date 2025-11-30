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

#include <cstddef>
#include <cstdint>

#if __has_include(<kernel.h>)
#include <kernel.h>
#else
extern "C" {
using ER = int32_t;
using ID = int32_t;
using ATR = uint32_t;
using PRI = int32_t;
using SIZE = std::size_t;
using RELTIM = uint32_t;
using SYSUTM = uint64_t;
using EXINF = std::intptr_t;

using TASK = void (*)(EXINF exinf);

struct T_CTSK {
    ATR tskatr;
    PRI itskpri;
    SIZE stksz;
    void* stk;
    TASK tskadr;
    EXINF exinf;
};

struct T_CMTX {
    ATR mtxatr;
    PRI ceilpri;
};

ER cre_tsk(ID tskid, const T_CTSK* pk_ctsk);
ER act_tsk(ID tskid);
ER ext_tsk();
ER dly_tsk(RELTIM dlytim);
ER get_utm(SYSUTM* pk_utm);
ER cre_mtx(ID mtxid, const T_CMTX* pk_cmtx);
ER del_mtx(ID mtxid);
ER loc_mtx(ID mtxid);
ER unl_mtx(ID mtxid);

// Optional serial output used for debugging.
ER serial_wri_dat(ID portid, const char* buffer, std::size_t length);
}
#endif

namespace score::os::asp3 {

constexpr ID kTaskIdBase = 1;
constexpr int kMaxTasks = 4;
constexpr ID kMutexIdBase = 64;
constexpr int kMaxMutexes = 4;
constexpr ATR kTaskAttributes = 0x02U;  // Placeholder for TA_ACT
constexpr PRI kDefaultPriority = 5;
constexpr SIZE kDefaultStackSize = 1024U;
constexpr ATR kMutexAttributes = 0x00U;  // TA_TPRI
constexpr ID kDefaultSerialPort = 1;
#ifdef E_OK
constexpr ER kOk = E_OK;
#else
constexpr ER kOk = 0;
#endif

}  // namespace score::os::asp3
