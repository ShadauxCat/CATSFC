# - - - Modifiable paths - - -
DS2SDKPATH  := /opt/ds2sdk
CROSS       := /opt/mipsel-4.1.2-nopic/bin/mipsel-linux-

# - - - Libraries and includes - - -
FS_DIR       = $(DS2SDKPATH)/libsrc/fs
CONSOLE_DIR  = $(DS2SDKPATH)/libsrc/console
KEY_DIR      = $(DS2SDKPATH)/libsrc/key
ZLIB_DIR     = $(DS2SDKPATH)/libsrc/zlib

LIBS        := $(DS2SDKPATH)/lib/libds2b.a -lc -lm -lgcc
EXTLIBS     := $(DS2SDKPATH)/lib/libds2a.a

INCLUDE     := -Isource -Isource/unzip -Isource/nds -I$(DS2SDKPATH)/include \
               -I$(FS_DIR) -I$(CONSOLE_DIR) -I$(KEY_DIR) -I$(ZLIB_DIR)

LINK_SPEC   := $(DS2SDKPATH)/specs/link.xn
START_ASM   := $(DS2SDKPATH)/specs/start.S
START_O     := start.o

# - - - Names - - -
OUTPUT      := catsfc
PLUGIN_DIR  := CATSFC

# - - - Tools - - -
CC           = $(CROSS)gcc
AR           = $(CROSS)ar rcsv
LD           = $(CROSS)ld
OBJCOPY      = $(CROSS)objcopy
NM           = $(CROSS)nm
OBJDUMP      = $(CROSS)objdump

# - - - Sources and objects - - -
C_SOURCES   = source/unicode.c \
              source/unzip/explode.c source/unzip/unreduce.c \
              source/unzip/unshrink.c source/unzip/unzip.c \
              source/nds/bdf_font.c source/nds/bitmap.c source/nds/charsets.c \
              source/nds/draw.c source/nds/ds2_main.c source/nds/gcheat.c \
              source/nds/gui.c
CPP_SOURCES = source/apu.cpp source/apudebug.cpp source/c4.cpp \
              source/c4emu.cpp source/cheats2.cpp source/cheats.cpp \
              source/clip.cpp source/cpu.cpp source/cpuexec.cpp \
              source/cpuops.cpp source/data.cpp source/debug.cpp \
              source/dma.cpp source/dsp1.cpp \
              source/fxdbg.cpp source/fxemu.cpp source/fxinst.cpp \
              source/gfx.cpp source/globals.cpp source/loadzip.cpp \
              source/memmap.cpp source/movie.cpp source/netplay.cpp \
              source/obc1.cpp source/ppu.cpp \
              source/sa1.cpp source/sa1cpu.cpp source/screenshot.cpp \
              source/sdd1.cpp source/sdd1emu.cpp source/server.cpp \
              source/seta010.cpp source/seta011.cpp source/seta018.cpp \
              source/seta.cpp source/snaporig.cpp source/snapshot.cpp \
              source/soundux.cpp \
              source/spc700.cpp source/spc7110.cpp \
              source/srtc.cpp \
              source/tile.cpp \
              source/nds/displaymodes.cpp source/nds/entry.cpp
SOURCES      = $(C_SOURCES) $(CPP_SOURCES)
C_OBJECTS    = $(C_SOURCES:.c=.o)
CPP_OBJECTS  = $(CPP_SOURCES:.cpp=.o)
OBJECTS      = $(C_OBJECTS) $(CPP_OBJECTS)

# - - - Compilation flags - - -
CFLAGS := -mips32 -mno-abicalls -fno-pic -fno-builtin \
	      -fno-exceptions -ffunction-sections -mno-long-calls \
	      -msoft-float -G 4 \
          -O3 -fomit-frame-pointer -fgcse-sm -fgcse-las -fgcse-after-reload \
          -fweb -funroll-loops

DEFS   := -DSPC700_C -DEXECUTE_SUPERFX_PER_LINE -DSDD1_DECOMP \
          -DVAR_CYCLES -DCPU_SHUTDOWN -DSPC700_SHUTDOWN \
          -DNO_INLINE_SET_GET -DNOASM -DHAVE_MKSTEMP '-DACCEPT_SIZE_T=size_t' \
          -DUNZIP_SUPPORT -DSYNC_JOYPAD_AT_HBLANK

.PHONY: clean makedirs
.SUFFIXES: .elf .dat .plg

all: $(OUTPUT).plg makedirs

release: all
	zip -r $(OUTPUT).zip $(PLUGIN_DIR) $(OUTPUT).plg $(OUTPUT).bmp $(OUTPUT).ini copyright installation.txt README.md source.txt version

# $< is the source (OUTPUT.dat); $@ is the target (OUTPUT.plg)
.dat.plg:
	$(DS2SDKPATH)/tools/makeplug $< $@

# $< is the source (OUTPUT.elf); $@ is the target (OUTPUT.dat)
.elf.dat:
	$(OBJCOPY) -x -O binary $< $@

$(OUTPUT).elf: Makefile $(OBJECTS) $(START_O) $(LINK_SPEC) $(EXTLIBS)
	$(CC) -nostdlib -static -T $(LINK_SPEC) -o $@ $(START_O) $(OBJECTS) $(EXTLIBS) $(LIBS)

$(EXTLIBS):
	$(MAKE) -C $(DS2SDKPATH)/source/

$(START_O): $(START_ASM)
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<

makedirs:
	-mkdir $(PLUGIN_DIR)/gamepak
	-mkdir $(PLUGIN_DIR)/gamecht
	-mkdir $(PLUGIN_DIR)/gamerts
	-mkdir $(PLUGIN_DIR)/gamepic

clean:
	-rm -rf $(OUTPUT).plg $(OUTPUT).dat $(OUTPUT).elf depend $(OBJECTS) $(START_O)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDE) $(DEFS) -o $@ -c $<
.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDE) $(DEFS) -fno-rtti -o $@ -c $<

Makefile: depend

depend: $(SOURCES)
	$(CC) -MM $(CFLAGS) $(INCLUDE) $(DEFS) $(SOURCES) > $@
	touch Makefile

-include depend
