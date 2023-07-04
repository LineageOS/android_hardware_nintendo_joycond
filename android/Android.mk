#
# Copyright (C) 2019-2021 Billy Laws
# Copyright (C) 2021 Daniel Ogorchok
# Copyright (C) 2022-2023 Thomas Makin
# Copyright (C) 2023 The LineageOS Project
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

# Disabled idc for left JoyCon
include $(CLEAR_VARS)
LOCAL_MODULE := Vendor_057e_Product_2006.idc
LOCAL_SRC_FILES := android/disabled.idc
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/usr/idc
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := nintendo
include $(BUILD_PREBUILT)

# Disabled idc for right JoyCon
include $(CLEAR_VARS)
LOCAL_MODULE := Vendor_057e_Product_2007.idc
LOCAL_SRC_FILES := android/disabled.idc
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/usr/idc
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := nintendo
include $(BUILD_PREBUILT)

# External idc for combined JoyCons
include $(CLEAR_VARS)
LOCAL_MODULE := Vendor_057e_Product_2008.idc
LOCAL_SRC_FILES := android/external.idc
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/usr/idc
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := nintendo
include $(BUILD_PREBUILT)

# Disabled idc for ProCon
include $(CLEAR_VARS)
LOCAL_MODULE := Vendor_057e_Product_2009_Version_8001.idc
LOCAL_SRC_FILES := android/disabled.idc
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/usr/idc
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := nintendo
include $(BUILD_PREBUILT)

# Disabled idc for Sio
include $(CLEAR_VARS)
LOCAL_MODULE := Vendor_057e_Product_f123.idc
LOCAL_SRC_FILES := android/disabled.idc
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/usr/idc
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := nintendo
include $(BUILD_PREBUILT)

# Left Single JoyCon
include $(CLEAR_VARS)
LOCAL_MODULE := Vendor_057e_Product_3006.kl
LOCAL_SRC_FILES := android/Vendor_057e_Product_3006.kl
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/usr/keylayout
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := nintendo
include $(BUILD_PREBUILT)

# Right Single JoyCon
include $(CLEAR_VARS)
LOCAL_MODULE := Vendor_057e_Product_3007.kl
LOCAL_SRC_FILES := android/Vendor_057e_Product_3007.kl
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/usr/keylayout
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := nintendo
include $(BUILD_PREBUILT)

# Combined JoyCons, Nintendo layout, digital triggers
include $(CLEAR_VARS)
LOCAL_MODULE := Vendor_057e_Product_2008.kl
LOCAL_SRC_FILES := android/Vendor_057e_Product_2008.kl
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/usr/keylayout
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := nintendo
include $(BUILD_PREBUILT)

# Combined JoyCons, Xbox layout, digital triggers
include $(CLEAR_VARS)
LOCAL_MODULE := Vendor_057e_Product_2018.kl
LOCAL_SRC_FILES := android/Vendor_057e_Product_2018.kl
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/usr/keylayout
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := nintendo
include $(BUILD_PREBUILT)

# Combined JoyCons, Nintendo layout, emulated analog triggers
include $(CLEAR_VARS)
LOCAL_MODULE := Vendor_057e_Product_2108.kl
LOCAL_SRC_FILES := android/Vendor_057e_Product_2108.kl
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/usr/keylayout
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := nintendo
include $(BUILD_PREBUILT)

# Combined JoyCons, Xbox layout, emulated analog triggers
include $(CLEAR_VARS)
LOCAL_MODULE := Vendor_057e_Product_2118.kl
LOCAL_SRC_FILES := android/Vendor_057e_Product_2118.kl
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/usr/keylayout
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := nintendo
include $(BUILD_PREBUILT)
