# For putsurface
# =====================================================

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
  putsurface_android.cpp

LOCAL_CFLAGS += \
    -DANDROID

LOCAL_C_INCLUDES += \
  $(LOCAL_PATH)/../common

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := putsurface

LOCAL_SHARED_LIBRARIES := libva-android libva libdl libdrm libcutils libutils libgui

include $(BUILD_EXECUTABLE)

