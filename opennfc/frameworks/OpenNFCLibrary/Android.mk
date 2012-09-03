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

# This makefile shows how to build your own shared library that can be
# shipped on the system of a phone, and included additional examples of
# including JNI code with the library and writing client applications against it.

LOCAL_PATH := $(call my-dir)

# the library
# ============================================================
include $(CLEAR_VARS)

# This is the target being built.
LOCAL_MODULE:= org.opennfc_library

LOCAL_MODULE_TAGS := optional eng

LOCAL_SRC_FILES := $(call all-subdir-java-files) \
					../OpenNFCService/src/

# Also link against our own custom library.
LOCAL_JNI_SHARED_LIBRARIES := open_nfc_client_jni

# Manually copy the optional library XML files in the system image.

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/org.opennfc_library.xml:system/etc/permissions/org.opennfc_library.xml

include $(BUILD_JAVA_LIBRARY)

# the documentation
# ============================================================
include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(call all-subdir-java-files) $(call all-subdir-html-files)

LOCAL_MODULE:= opennfc_library

ifeq ("$(TARGET_PRODUCT)", "opennfc_core_addon")
   addon="yes"
endif

ifeq ("$(TARGET_PRODUCT)","opennfc_us_addon")
  addon :="yes"
endif

ifeq ("$(TARGET_PRODUCT)","opennfc_eu_addon")
  addon :="yes"
endif

ifeq ($(addon),"yes")

  LOCAL_DROIDDOC_USE_STANDARD_DOCLET := true
  LOCAL_DROIDDOC_OPTIONS:= org.opennfc

else

	opennfc_docs_LOCAL_DROIDDOC_OPTIONS := \
      -error 1 -error 2 -warning 3 -error 4 -error 6 -error 8

  LOCAL_DROIDDOC_HTML_DIR:= packages/apps/OpenNFC/frameworks/OpenNFCLibrary/docs/html
  LOCAL_DROIDDOC_USE_STANDARD_DOCLET := false
  LOCAL_DROIDDOC_CUSTOM_TEMPLATE_DIR:= build/tools/droiddoc/templates-opennfc
  LOCAL_DROIDDOC_CUSTOM_ASSET_DIR:= assets

  LOCAL_DROIDDOC_OPTIONS:= $(opennfc_docs_LOCAL_DROIDDOC_OPTIONS)  \
                           -offlinemode \
                            -title "Open NFC for Android SDK addon documentation" \
                           -hdf android.whichdoc offline

endif

LOCAL_MODULE_CLASS := JAVA_LIBRARIES

include $(BUILD_DROIDDOC)



