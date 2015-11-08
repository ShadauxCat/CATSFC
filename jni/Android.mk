LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

CORE_DIR     := ../source
LIBRETRO_DIR := ..

DEBUG                 = 0
PERF_TEST             = 0
HAVE_GRIFFIN          = 1
LOAD_FROM_MEMORY_TEST = 1
USE_BLARGG_APU        = 0


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

LOCAL_SRC_FILES += $(SOURCES_C)
LOCAL_CFLAGS    += -std=gnu99 -ffast-math -DINLINE=inline $(FLAGS)

include $(BUILD_SHARED_LIBRARY)
