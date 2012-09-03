# Copyright (C) 2008 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

LOCAL_PATH := $(call my-dir)

# HAL module implemenation, not prelinked and stored in
# hw/<OVERLAY_HARDWARE_MODULE_ID>.<ro.product.board>.so
include $(CLEAR_VARS)

LOCAL_SHARED_LIBRARIES := liblog libdl

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS = optional eng

LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw

LOCAL_SRC_FILES := nfcc.c


LOCAL_C_INCLUDES += $(LOCAL_PATH)/interfaces	\
						  $(LOCAL_PATH)/porting	\
						  $(LOCAL_PATH)/driver	\
							$(ANDROID_BUILD_TOP)/packages/apps/OpenNFC/frameworks/libs/open_nfc/interfaces \
							$(ANDROID_BUILD_TOP)/packages/apps/OpenNFC/frameworks/libs/open_nfc/porting/common \
							$(ANDROID_BUILD_TOP)/packages/apps/OpenNFC/frameworks/libs/open_nfc/porting/server \
							$(LOCAL_PATH)/../../../include

LOCAL_MODULE := nfcc.default


include $(BUILD_SHARED_LIBRARY)
