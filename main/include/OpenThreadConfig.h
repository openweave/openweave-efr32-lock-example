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

/**
 *    @file
 *      Overrides to default OpenThread configuration.
 *
 */

#ifndef OPENTHREAD_PLATFORM_CONFIG_H
#define OPENTHREAD_PLATFORM_CONFIG_H

#include <em_device.h>

// Disable the SiLabs-supplied OpenThread logging facilities and use
// the facilities provided by the OpenWeave Device Layer (see
// openweave/src/adaptations/device-layer/EFR32/Logging.cpp).
#define OPENTHREAD_CONFIG_LOG_OUTPUT OPENTHREAD_CONFIG_LOG_OUTPUT_APP

// Turn on a moderate level of logging in OpenThread
#define OPENTHREAD_CONFIG_LOG_LEVEL OT_LOG_LEVEL_NOTE

// Enable use of external heap allocator (calloc/free) for OpenThread.
#define OPENTHREAD_CONFIG_HEAP_EXTERNAL_ENABLE 1

// EFR32MG21A020F1024IM32 has 96k of RAM. Reduce the number of buffers to
// conserve RAM for this Series 2 part.
#if defined(EFR32MG21A020F1024IM32)
#define OPENTHREAD_CONFIG_NUM_MESSAGE_BUFFERS 22
#endif

// Use the SiLabs-supplied default platform configuration for remainder
// of OpenThread config options.
//
// NB: This file gets included during the build of OpenThread.  Hence
// it cannot use "openthread" in the path to the included file.
//
#include "openthread-core-efr32-config.h"

#endif // OPENTHREAD_PLATFORM_CONFIG_H
