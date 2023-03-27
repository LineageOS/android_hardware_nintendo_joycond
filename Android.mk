#
# Copyright (C) 2019-2021 Billy Laws
# Copyright (C) 2021 Daniel Ogorchok
# Copyright (C) 2022-2023 Thomas Makin
# Copyright (C) 2022-2023 The LineageOS Project
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

LOCAL_PATH := $(call my-dir)

include $(call all-makefiles-under,$(LOCAL_PATH))

include $(CLEAR_VARS)
LOCAL_SRC_FILES := \
    src/ctlr_detector_android.cpp \
    src/ctlr_mgr.cpp \
    src/epoll_mgr.cpp \
    src/epoll_subscriber.cpp \
    src/phys_ctlr.cpp \
    src/virt_ctlr.cpp \
    src/virt_ctlr_combined.cpp \
    src/virt_ctlr_passthrough.cpp \
    src/virt_ctlr_pro.cpp \
    src/main.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libevdev \
    libnl \
    liblog

LOCAL_REQUIRED_MODULES := \
    Vendor_057e_Product_2006.idc \
    Vendor_057e_Product_2007.idc \
    Vendor_057e_Product_2008.idc \
	Vendor_057e_Product_2006.kl  \
	Vendor_057e_Product_2007.kl  \
    Vendor_057e_Product_2008.kl  \
    Vendor_057e_Product_2018.kl  \
    Vendor_057e_Product_2108.kl  \
    Vendor_057e_Product_2118.kl  \
    Vendor_057e_Product_2009_Version_8001.idc

LOCAL_MODULE := joycond
LOCAL_INIT_RC := android/joycond.rc
LOCAL_MODULE_TAGS := optional
LOCAL_CPPFLAGS := -std=c++17 -Wno-error -fexceptions
LOCAL_VENDOR_MODULE := true
LOCAL_MODULE_OWNER := nintendo
include $(BUILD_EXECUTABLE)
