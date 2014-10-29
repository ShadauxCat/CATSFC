TARGET_NAME   := catsfc

INCLUDE     := -Isource -Isource/unzip -Isource/nds
INCLUDE     += -I.

platform = unix

ifeq ($(platform), unix)
   TARGET := $(TARGET_NAME)_libretro.so
   fpic   := -fPIC
   SHARED := -shared -Wl,--version-script=link.T
   CROSS  :=

   CFLAGS := -fno-builtin \
            -fno-exceptions -ffunction-sections \
             -fomit-frame-pointer -fgcse-sm -fgcse-las -fgcse-after-reload \
             -fweb -fpeel-loops \
             -Wall -Wno-unused-function -Wno-unused-variable

   DEFS   :=
else
   TARGET := $(TARGET_NAME)_libretro_psp1.a
   CROSS  := psp-
   CFLAGS := -G0 -march=allegrex -mno-abicalls -fno-pic -fno-builtin \
            -fno-exceptions -ffunction-sections -mno-long-calls \
             -fomit-frame-pointer -fgcse-sm -fgcse-las -fgcse-after-reload \
             -fweb -fpeel-loops \
             -Wall -Wno-unused-function -Wno-unused-variable

#   CFLAGS   += -march=allegrex -mfp32 -mgp32 -mlong32 -mabi=eabi
#   CFLAGS   += -fomit-frame-pointer -fstrict-aliasing
#   CFLAGS   += -falign-functions=32 -falign-loops -falign-labels -falign-jumps
#   CFLAGS   += -Wall -Wundef -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Wsign-compare

   DEFS   :=  -DPSP -D_PSP_FW_VERSION=371

   INCLUDE     += -I$(shell psp-config --pspsdk-path)/include
#   INCLUDE     += -I$(shell psp-config --psp-prefix)/include
   STATIC_LINKING := 1
endif

# - - - Tools - - -
CC           = $(CROSS)gcc
AR           = $(CROSS)ar
LD           = $(CROSS)ld
OBJCOPY      = $(CROSS)objcopy
NM           = $(CROSS)nm
OBJDUMP      = $(CROSS)objdump

#C_SOURCES   = libretro.c
CPP_SOURCES = source/apu.cpp source/c4.cpp \
              source/c4emu.cpp source/cheats2.cpp source/cheats.cpp \
              source/clip.cpp source/cpu.cpp source/cpuexec.cpp \
              source/cpuops.cpp source/data.cpp\
              source/dma.cpp source/dsp1.cpp \
              source/fxdbg.cpp source/fxemu.cpp source/fxinst.cpp \
              source/gfx.cpp source/globals.cpp \
              source/memmap.cpp \
              source/obc1.cpp source/ppu.cpp \
              source/sa1.cpp source/sa1cpu.cpp source/screenshot.cpp \
              source/sdd1.cpp source/sdd1emu.cpp \
              source/seta010.cpp source/seta011.cpp source/seta018.cpp \
              source/seta.cpp source/snaporig.cpp source/snapshot.cpp \
              source/soundux.cpp \
              source/spc700.cpp source/spc7110.cpp \
              source/srtc.cpp \
              source/tile.cpp \
              libretro.cpp

SOURCES      = $(C_SOURCES) $(CPP_SOURCES)
C_OBJECTS    = $(C_SOURCES:.c=.o)
CPP_OBJECTS  = $(CPP_SOURCES:.cpp=.o)
OBJECTS      = $(C_OBJECTS) $(CPP_OBJECTS)

# - - - Compilation flags - - -

DEFS   += -DSPC700_C -DEXECUTE_SUPERFX_PER_LINE -DSDD1_DECOMP \
          -DVAR_CYCLES -DCPU_SHUTDOWN -DSPC700_SHUTDOWN \
          -DNO_INLINE_SET_GET -DNOASM -DHAVE_MKSTEMP '-DACCEPT_SIZE_T=size_t'

ifeq ($(DEBUG), 1)
OPTIMIZE	      := -O0 -g
OPTIMIZE_SAFE  := -O0 -g
else
OPTIMIZE	      := -O3
OPTIMIZE_SAFE  := -O2
endif


DEFS  += -D__LIBRETRO__

CFLAGS += $(fpic)

all: $(TARGET)

$(TARGET): $(OBJECTS)
ifeq ($(STATIC_LINKING), 1)
	$(AR) rcs $@ $(OBJECTS)
else
	$(CC) $(fpic) $(SHARED) $(INCLUDES) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBM)
endif

clean:
	rm -f $(OBJECTS)
	rm -f depend
	rm -f $(TARGET)

.c.o:
	$(CC) $(OPTIMIZE) $(CFLAGS) $(INCLUDE) $(DEFS) -o $@ -c $<
.cpp.o:
	$(CC) $(OPTIMIZE) $(CFLAGS) $(INCLUDE) $(DEFS) -fno-rtti -o $@ -c $<

Makefile: depend

depend: $(SOURCES)
	$(CC) -MM $(CFLAGS) $(INCLUDE) $(DEFS) $(SOURCES) > $@
	touch Makefile

.PHONY: clean

-include depend
