LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

CORE_DIR     := ../source
LIBRETRO_DIR := ..
HAVE_GRIFFIN := 1

LOCAL_MODULE    := retro

ifeq ($(TARGET_ARCH),arm)
LOCAL_CFLAGS += -DANDROID_ARM
LOCAL_ARM_MODE := arm
endif

ifeq ($(TARGET_ARCH),x86)
LOCAL_CFLAGS +=  -DANDROID_X86
endif

ifeq ($(TARGET_ARCH),mips)
LOCAL_CFLAGS += -DANDROID_MIPS -D__mips__ -D__MIPSEL__
endif

include ../Makefile.common

LOCAL_SRC_FILES    += $(SOURCES_C)
LOCAL_CFLAGS += -O2 -std=gnu99 -ffast-math -DINLINE=inline -DPERF_TEST -D__LIBRETRO__ -DFRONTEND_SUPPORTS_RGB565 -DNDEBUG=1 $(INCFLAGS)

include $(BUILD_SHARED_LIBRARY)
