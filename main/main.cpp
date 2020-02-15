/*
 *
 *    Copyright (c) 2019 Google LLC.
 *    All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <bsp.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>

#include <openthread/instance.h>
#include <openthread/thread.h>
#include <openthread/tasklet.h>
#include <openthread/link.h>
#include <openthread/dataset.h>
#include <openthread/error.h>
#include <openthread/heap.h>
#include <openthread/logging.h>
#include <openthread/icmp6.h>
#include <openthread/platform/openthread-system.h>

#include <Weave/DeviceLayer/WeaveDeviceLayer.h>
#include <Weave/DeviceLayer/ThreadStackManager.h>
#include <Weave/DeviceLayer/EFR32/GroupKeyStoreImpl.h>

#include <AppTask.h>

#include "AppConfig.h"

#include <FreeRTOS.h>
#include <efr32-weave-mbedtls-config.h>
#include <mbedtls/threading.h>

using namespace ::nl;
using namespace ::nl::Inet;
using namespace ::nl::Weave;
using namespace ::nl::Weave::DeviceLayer;

#define UNUSED_PARAMETER(a) (a = a)

volatile int apperror_cnt;

// ================================================================================
// App Error
//=================================================================================
void appError(int err)
{
    EFR32_LOG("!!!!!!!!!!!! App Critical Error: %d !!!!!!!!!!!", err);
    portDISABLE_INTERRUPTS();
    while (1)
        ;
}

// ================================================================================
// FreeRTOS Callbacks
// ================================================================================
extern "C" void vApplicationIdleHook(void)
{
    // FreeRTOS Idle callback

    // Check Weave Config nvm3 and repack flash if necessary.
    Internal::EFR32Config::RepackNvm3Flash();
}

// ================================================================================
// Main Code
// ================================================================================
int main(void)
{
    int ret = WEAVE_ERROR_MAX;

    otSysInit(0, NULL);
    otHeapSetCAllocFree(calloc, free);

    // Initialize mbedtls threading support on EFR32
    THREADING_setup();

    BSP_LedsInit();
    BSP_LedSet(0);
    BSP_LedSet(1);

#if EFR32_LOG_ENABLED
    if (efr32LogInit() != 0)
    {
        appError(ret);
    }
#endif

    EFR32_LOG("==================================================");
    EFR32_LOG("openweave-efr32-lock-example starting");
    EFR32_LOG("==================================================");

    EFR32_LOG("Initializing Weave stack");
    ret = PlatformMgr().InitWeaveStack();
    if (ret != WEAVE_NO_ERROR)
    {
        EFR32_LOG("PlatformMgr().InitWeaveStack() failed");
        appError(ret);
    }

    EFR32_LOG("Initializing OpenThread stack");
    ret = ThreadStackMgr().InitThreadStack();
    if (ret != WEAVE_NO_ERROR)
    {
        EFR32_LOG("ThreadStackMgr().InitThreadStack() failed");
        appError(ret);
    }

    // Configure device to operate as a Thread sleepy end-device.
    ret = ConnectivityMgr().SetThreadDeviceType(ConnectivityManager::kThreadDeviceType_SleepyEndDevice);
    if (ret != WEAVE_NO_ERROR)
    {
        EFR32_LOG("ConnectivityMgr().SetThreadDeviceType() failed");
        appError(ret);
    }

    // Configure the Thread polling behavior for the device.
    {
        ConnectivityManager::ThreadPollingConfig pollingConfig;
        pollingConfig.Clear();
        pollingConfig.ActivePollingIntervalMS   = THREAD_ACTIVE_POLLING_INTERVAL_MS;
        pollingConfig.InactivePollingIntervalMS = THREAD_INACTIVE_POLLING_INTERVAL_MS;
        ret                                     = ConnectivityMgr().SetThreadPollingConfig(pollingConfig);
        if (ret != WEAVE_NO_ERROR)
        {
            EFR32_LOG("ConnectivityMgr().SetThreadPollingConfig() failed");
            appError(ret);
        }
    }

    EFR32_LOG("Starting Weave task");
    ret = PlatformMgr().StartEventLoopTask();
    if (ret != WEAVE_NO_ERROR)
    {
        EFR32_LOG("PlatformMgr().StartEventLoopTask() failed");
        appError(ret);
    }

    EFR32_LOG("Starting OpenThread task");
    ret = ThreadStackMgrImpl().StartThreadTask();
    if (ret != WEAVE_NO_ERROR)
    {
        EFR32_LOG("ThreadStackMgr().StartThreadTask() failed");
        appError(ret);
    }

    EFR32_LOG("Starting App task");
    ret = GetAppTask().StartAppTask();
    if (ret != WEAVE_NO_ERROR)
    {
        EFR32_LOG("GetAppTask().Init() failed");
        appError(ret);
    }

    EFR32_LOG("Starting FreeRTOS scheduler");
    vTaskStartScheduler();

    // Should never get here.
    EFR32_LOG("vTaskStartScheduler() failed");
    appError(ret);
}
