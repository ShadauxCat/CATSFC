DEBUG     = 0
PERF_TEST = 1
HAVE_GRIFFIN = 1
LOAD_FROM_MEMORY_TEST = 1

ifeq ($(platform),)
platform = unix
ifeq ($(shell uname -a),)
   platform = win
else ifneq ($(findstring Darwin,$(shell uname -a)),)
   platform = osx
	arch = intel
ifeq ($(shell uname -p),powerpc)
	arch = ppc
endif
else ifneq ($(findstring MINGW,$(shell uname -a)),)
   platform = win
endif
endif

# system platform
system_platform = unix
ifeq ($(shell uname -a),)
EXE_EXT = .exe
   system_platform = win
else ifneq ($(findstring Darwin,$(shell uname -a)),)
   system_platform = osx
	arch = intel
ifeq ($(shell uname -p),powerpc)
	arch = ppc
endif
else ifneq ($(findstring MINGW,$(shell uname -a)),)
   system_platform = win
endif

TARGET_NAME := catsfc
DEFS        :=
CFLAGS      :=

ifeq ($(platform), unix)
   TARGET := $(TARGET_NAME)_libretro.so
   fpic := -fPIC
   SHARED := -shared -Wl,--no-undefined -Wl,--version-script=link.T

   CFLAGS += -fno-builtin \
            -fno-exceptions -ffunction-sections \
             -fomit-frame-pointer -fgcse-sm -fgcse-las -fgcse-after-reload \
             -fweb -fpeel-loops
else ifeq ($(platform), osx)
   TARGET := $(TARGET_NAME)_libretro.dylib
   fpic := -fPIC
   SHARED := -dynamiclib

ifeq ($(arch),ppc)
	FLAGS += -DMSB_FIRST
	OLD_GCC = 1
endif
   OSXVER = `sw_vers -productVersion | cut -d. -f 2`
   OSX_LT_MAVERICKS = `(( $(OSXVER) <= 9)) && echo "YES"`
ifeq ($(OSX_LT_MAVERICKS),"YES")
   fpic += -mmacosx-version-min=10.5
endif
else ifeq ($(platform), ios)
   TARGET := $(TARGET_NAME)_libretro_ios.dylib
   fpic := -fPIC
   SHARED := -dynamiclib

ifeq ($(IOSSDK),)
   IOSSDK := $(shell xcrun -sdk iphoneos -show-sdk-path)
endif

   CC = clang -arch armv7 -isysroot $(IOSSDK)
   CXX = clang++ -arch armv7 -isysroot $(IOSSDK)
   OSXVER = `sw_vers -productVersion | cut -d. -f 2`
   OSX_LT_MAVERICKS = `(( $(OSXVER) <= 9)) && echo "YES"`
ifeq ($(OSX_LT_MAVERICKS),"YES")
   SHARED += -miphoneos-version-min=5.0
   CC +=  -miphoneos-version-min=5.0
   CXX +=  -miphoneos-version-min=5.0
endif
else ifeq ($(platform), qnx)
   TARGET := $(TARGET_NAME)_libretro_qnx.so
   fpic := -fPIC
   SHARED := -shared -Wl,--no-undefined -Wl,--version-script=link.T
	CC = qcc -Vgcc_ntoarmv7le
	CXX = QCC -Vgcc_ntoarmv7le_cpp
else ifeq ($(platform), ps3)
   TARGET := $(TARGET_NAME)_libretro_ps3.a
   CC = $(CELL_SDK)/host-win32/ppu/bin/ppu-lv2-gcc.exe
   CXX = $(CELL_SDK)/host-win32/ppu/bin/ppu-lv2-g++.exe
   AR = $(CELL_SDK)/host-win32/ppu/bin/ppu-lv2-ar.exe
   STATIC_LINKING = 1
	FLAGS += -DMSB_FIRST
	OLD_GCC = 1
else ifeq ($(platform), sncps3)
   TARGET := $(TARGET_NAME)_libretro_ps3.a
   CC = $(CELL_SDK)/host-win32/sn/bin/ps3ppusnc.exe
   CXX = $(CELL_SDK)/host-win32/sn/bin/ps3ppusnc.exe
   AR = $(CELL_SDK)/host-win32/sn/bin/ps3snarl.exe
   STATIC_LINKING = 1
	FLAGS += -DMSB_FIRST
	NO_GCC = 1
else ifeq ($(platform), psp1)
   TARGET := $(TARGET_NAME)_libretro_psp1.a
	CC = psp-gcc$(EXE_EXT)
	CXX = psp-g++$(EXE_EXT)
	AR = psp-ar$(EXE_EXT)
   STATIC_LINKING = 1
	FLAGS += -G0 -DLSB_FIRST
   CFLAGS += -march=allegrex -mno-abicalls -fno-pic -fno-builtin \
		-fno-exceptions -ffunction-sections -mno-long-calls \
		-fomit-frame-pointer -fgcse-sm -fgcse-las -fgcse-after-reload \
		-fweb -fpeel-loops
	DEFS   +=  -DPSP -D_PSP_FW_VERSION=371
   INCLUDE     += -I$(shell psp-config --pspsdk-path)/include
   STATIC_LINKING := 1
else
   TARGET := $(TARGET_NAME)_libretro.dll
   CC = gcc
   CXX = g++
   SHARED := -shared -Wl,--no-undefined -Wl,--version-script=link.T
   LDFLAGS += -static-libgcc -static-libstdc++ -lwinmm
endif

DEFS   += -DSPC700_C -DEXECUTE_SUPERFX_PER_LINE -DSDD1_DECOMP \
          -DVAR_CYCLES -DCPU_SHUTDOWN -DSPC700_SHUTDOWN \
          -DNO_INLINE_SET_GET -DNOASM -DHAVE_MKSTEMP '-DACCEPT_SIZE_T=size_t'

DEFS  += -D__LIBRETRO__

CORE_DIR     := ./source
LIBRETRO_DIR := .

include Makefile.common


OBJECTS := $(SOURCES_C:.c=.o)

all: $(TARGET)

ifeq ($(DEBUG),1)
FLAGS += -O0 -g
else
FLAGS += -O3 -DNDEBUG
endif

ifeq ($(PERF_TEST),1)
FLAGS += -DPERF_TEST
endif


ifeq ($(LOAD_FROM_MEMORY_TEST),1)
FLAGS += -DLOAD_FROM_MEMORY_TEST
endif

LDFLAGS += $(fpic) -lz $(SHARED)
FLAGS += $(fpic) 
FLAGS += $(INCFLAGS)


ifeq ($(OLD_GCC), 1)
WARNINGS := -Wall
else ifeq ($(NO_GCC), 1)
WARNINGS :=
else
WARNINGS := -Wall \
	-Wno-sign-compare \
	-Wno-unused-variable \
	-Wno-unused-function \
	-Wno-uninitialized \
	-Wno-strict-aliasing \
	-Wno-overflow \
	-fno-strict-overflow
endif

FLAGS += -D__LIBRETRO__ $(WARNINGS) $(INCLUDE) $(DEFS)

CXXFLAGS += $(FLAGS)
CFLAGS += $(FLAGS)

$(TARGET): $(OBJECTS)
ifeq ($(STATIC_LINKING), 1)
	$(AR) rcs $@ $(OBJECTS)
else
	$(CXX) -o $@ $^ $(LDFLAGS)
endif

%.o: %.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f $(TARGET) $(OBJECTS)

.PHONY: clean
