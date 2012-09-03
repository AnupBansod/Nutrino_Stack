#
# Copyright (C) 2010 Inside Contactless
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
#

LOCAL_PATH:= $(call my-dir)

#---------------------------------------------------------------------------------
# Open NFC client dynamic library
#---------------------------------------------------------------------------------

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional eng

# This is the target being built.
LOCAL_MODULE:= libopen_nfc_client_jni

# All of the source files that we will compile.
LOCAL_SRC_FILES:= \
	$(call all-c-files-under, sources) \
	$(call all-c-files-under, porting/common) \
	$(call all-c-files-under, porting/client)
#Additionnal headers

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/interfaces \
	$(LOCAL_PATH)/sources \
	$(LOCAL_PATH)/porting/common \
	$(LOCAL_PATH)/porting/client \
	$(JNI_H_INCLUDE)

LOCAL_CFLAGS += -D DO_NOT_USE_LWRAP_UNICODE=1

# All of the shared libraries we link against.
LOCAL_SHARED_LIBRARIES := \
	libandroid_runtime \
	libnativehelper \
	libcutils \
	libutils


# Don't prelink this library.  For more efficient code, you may want
# to add this library to the prelink map and set this to true.
LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)

#---------------------------------------------------------------------------------
# Open NFC server dynamic library
#---------------------------------------------------------------------------------

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional eng

# This is the target being built.
LOCAL_MODULE:= libopen_nfc_server_jni

# All of the source files that we will compile.
LOCAL_SRC_FILES:= \
	$(call all-c-files-under, sources) \
	$(call all-c-files-under, porting/common) \
	$(call all-c-files-under, porting/server)

#Additionnal headers
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/interfaces \
	$(LOCAL_PATH)/porting/common \
	$(LOCAL_PATH)/porting/server \
	$(ANDROID_BUILD_TOP)/packages/apps/OpenNFC/hardware/libhardware/include \
	$(ANDROID_BUILD_TOP)/packages/apps/OpenNFC/hardware/libhardware/include/hardware \
	$(ANDROID_BUILD_TOP)/packages/apps/OpenNFC/hardware/libhardware/modules/nfcc/porting \
        $(ANDROID_BUILD_TOP)/packages/apps/OpenNFC/hardware/libhardware/modules/nfcc/nfc_hal_microread/porting \
	$(JNI_H_INCLUDE)

LOCAL_CFLAGS += -D DO_NOT_USE_LWRAP_UNICODE=1

# All of the shared libraries we link against.
LOCAL_SHARED_LIBRARIES := \
	libhardware \
	libandroid_runtime \
	libnativehelper \
	libcutils \
	libutils

# Don't prelink this library.  For more efficient code, you may want
# to add this library to the prelink map and set this to true.
LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)
