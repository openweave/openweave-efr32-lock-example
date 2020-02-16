#
#
#   Copyright (c) 2019 Google LLC.
#   All rights reserved.
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#
# 
#   @file
#         Makefile for building the OpenWeave SiLabs EFR32 Lock Example Application.
#

#
# Differentiate between boards
#
# MG12
# - BRD4304A / SLWSTK6000B / MGM12P Module / 2.4GHz@19dBm
# - BRD4161A / SLWSTK6000B / Wireless Starter Kit / 2.4GHz@19dBm
# - BRD4166A / SLTB004A / Thunderboard Sense 2 / 2.4GHz@10dBm
# - BRD4170A / SLWSTK6000B / Multiband Wireless Starter Kit / 2.4GHz@19dBm, 915MHz@19dBm
#
# MG21
# - BRD4180A / SLWSTK6006A / Wireless Starter Kit / 2.4GHz@20dBm

PROJECT_ROOT := $(realpath .)

#
# Location of EFR32 SDK and tools
#
# The default values expect the OpenWeave application to be located in the
# app subdirectory of the Simplicity Studio v2.7 SDK.  E.g.:
#
#    SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.7/app
#
# Override these in the environment to allow to application to be located
# elsewhere.
#
EFR32_TOOLS_ROOT ?= $(PROJECT_ROOT)/../../../../../../
EFR32_SDK_ROOT   ?= $(PROJECT_ROOT)/../../

#
# Default locations of dependent projects
#
OPENWEAVE_ROOT ?= $(PROJECT_ROOT)/third_party/openweave-core
OPENTHREAD_ROOT ?= $(PROJECT_ROOT)/third_party/openthread
FREERTOS_ROOT ?= $(PROJECT_ROOT)/third_party/FreeRTOS/FreeRTOS

FREERTOSCONFIG_DIR := $(PROJECT_ROOT)/main/include

OPENTHREAD_SDK_SYMLINK := $(OPENTHREAD_ROOT)/third_party/silabs/gecko_sdk_suite
GECKO_SDK_SUITE_DIR    := $(EFR32_SDK_ROOT)/..

all : | $(OPENTHREAD_SDK_SYMLINK) 

$(OPENTHREAD_SDK_SYMLINK) :
	ln -s $(GECKO_SDK_SUITE_DIR) $(OPENTHREAD_SDK_SYMLINK)

BUILD_SUPPORT_DIR := $(OPENWEAVE_ROOT)/build/efr32

include $(BUILD_SUPPORT_DIR)/efr32-app.mk
include $(BUILD_SUPPORT_DIR)/efr32-openweave.mk
include $(BUILD_SUPPORT_DIR)/efr32-openthread.mk
include $(BUILD_SUPPORT_DIR)/efr32-freertos.mk

APP = openweave-efr32-lock-example

SRCS = \
    $(PROJECT_ROOT)/main/main.cpp \
    $(PROJECT_ROOT)/main/AppTask.cpp \
    $(PROJECT_ROOT)/main/LEDWidget.cpp \
    $(PROJECT_ROOT)/main/BoltLockManager.cpp \
    $(PROJECT_ROOT)/main/WDMFeature.cpp \
    $(PROJECT_ROOT)/main/ButtonHandler.cpp \
    $(PROJECT_ROOT)/main/traits/BoltLockTraitDataSource.cpp \
    $(PROJECT_ROOT)/main/traits/BoltLockSettingsTraitDataSink.cpp \
    $(PROJECT_ROOT)/main/traits/DeviceIdentityTraitDataSource.cpp \
    $(PROJECT_ROOT)/main/schema/BoltLockTrait.cpp \
    $(PROJECT_ROOT)/main/schema/BoltLockSettingsTrait.cpp \
    $(PROJECT_ROOT)/main/schema/DeviceIdentityTrait.cpp \
    $(PROJECT_ROOT)/main/support/CXXExceptionStubs.cpp \
    $(PROJECT_ROOT)/main/support/FreeRTOSNewlibLockSupport.c \
    $(PROJECT_ROOT)/main/support/AltPrintf.c \
    $(PROJECT_ROOT)/third_party/printf/printf.c \

INC_DIRS = \
    $(PROJECT_ROOT) \
    $(PROJECT_ROOT)/main \
    $(PROJECT_ROOT)/main/include \
    $(PROJECT_ROOT)/main/traits/include \
    $(PROJECT_ROOT)/main/schema/include \
    $(PROJECT_ROOT)/third_party/printf

ifeq ($(EFR32FAMILY), efr32mg12)
LINKER_SCRIPT=./main/ldscripts/$(APP)-MG12P.ld
else 
ifeq ($(EFR32FAMILY), efr32mg21)
LINKER_SCRIPT=./main/ldscripts/$(APP)-MG21.ld
endif
endif
    
DEFINES = \
  HARD_FAULT_LOG_ENABLE \
  RETARGET_VCOM \
  RETARGET_USART0 \
  PLATFORM_HEADER='<hal/micro/cortexm3/compiler/gcc.h>' \
  CORTEXM3_EFM32_MICRO \
  WEAVE_ERROR_LOGGING=1 \
  WEAVE_PROGRESS_LOGGING=1 \
  WEAVE_DETAIL_LOGGING=1  \
  THREAD_FULL_LOGS=1 \
  EFR32_LOG_ENABLED=1 \
  NVM3_DEFAULT_NVM_SIZE=40960
  
CFLAGS = \
    -specs=nano.specs

LDFLAGS = \
    -specs=nano.specs
    
ifdef DEVICE_FIRMWARE_REVISION
DEFINES += \
    WEAVE_DEVICE_CONFIG_DEVICE_FIRMWARE_REVISION=\"$(DEVICE_FIRMWARE_REVISION)\"
endif

OPENWEAVE_PROJECT_CONFIG = $(PROJECT_ROOT)/main/include/WeaveProjectConfig.h
OPENTHREAD_PROJECT_CONFIG = $(PROJECT_ROOT)/main/include/OpenThreadConfig.h

clean ::
	@echo "RM $(OPENTHREAD_SDK_SYMLINK) "
	$(NO_ECHO)rm -f $(OPENTHREAD_SDK_SYMLINK) 

$(call GenerateBuildRules)
