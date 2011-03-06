# sfc.mk

SRC +=  $(SFCDIR)/c4.cpp	\
        $(SFCDIR)/c4emu.cpp	\
        $(SFCDIR)/cheats.cpp	\
        $(SFCDIR)/cheats2.cpp	\
        $(SFCDIR)/clip.cpp	\
        $(SFCDIR)/cpu.cpp	\
        $(SFCDIR)/cpuexec.cpp	\
        $(SFCDIR)/cpuops.cpp	\
		$(SFCDIR)/data.cpp	\
        $(SFCDIR)/dma.cpp	\
        $(SFCDIR)/dsp1.cpp	\
        $(SFCDIR)/fxdbg.cpp	\
        $(SFCDIR)/fxemu.cpp	\
        $(SFCDIR)/fxinst.cpp	\
        $(SFCDIR)/gfx.cpp	\
        $(SFCDIR)/globals.cpp	\
        $(SFCDIR)/memmap.cpp	\
        $(SFCDIR)/ppu.cpp	\
		$(SFCDIR)/sa1.cpp	\
		$(SFCDIR)/sa1cpu.cpp	\
		$(SFCDIR)/sdd1.cpp	\
		$(SFCDIR)/sdd1emu.cpp	\
		$(SFCDIR)/snapshot.cpp	\
		$(SFCDIR)/snes9x.cpp	\
		$(SFCDIR)/srtc.cpp	\
		$(SFCDIR)/tile.cpp	\
		$(SFCDIR)/apu.cpp	\
		$(SFCDIR)/spc700.cpp	\
		$(SFCDIR)/soundux.cpp	\
		$(SFCDIR)/apudebug.cpp	\
		$(SFCDIR)/debug.cpp	\
		$(SFCDIR)/snaporig.cpp	\
		$(SFCDIR)/movie.cpp	\
		$(SFCDIR)/screenshot.cpp \
		$(SFCDIR)/spc7110.cpp	\
		$(SFCDIR)/obc1.cpp	\
		$(SFCDIR)/seta.cpp	\
		$(SFCDIR)/seta010.cpp	\
		$(SFCDIR)/seta011.cpp	\
		$(SFCDIR)/seta018.cpp	\
		$(SFCDIR)/loadzip.cpp	\
		$(SFCDIR)/nds/charsets.c  \
		$(SFCDIR)/nds/entry.cpp  \
		$(SFCDIR)/nds/displaymodes.cpp\
		$(SFCDIR)/nds/bitmap.c	\
		$(SFCDIR)/nds/bdf_font.c  \
		$(SFCDIR)/nds/draw.c  \
		$(SFCDIR)/nds/gui.c  \
		$(SFCDIR)/nds/ds2_main.c \
		$(SFCDIR)/nds/gcheat.c \
		$(SFCDIR)/nds/cheats3.cpp \
		$(SFCDIR)/unzip/explode.c \
		$(SFCDIR)/unzip/unreduce.c \
		$(SFCDIR)/unzip/unshrink.c \
		$(SFCDIR)/unzip/unzip.c \
		


CFLAGS	+= -I$(SFCDIR) -I$(SFCDIR)/nds -I$(SFCDIR)/unzip\
		-DSPC700_C \
		-DEXECUTE_SUPERFX_PER_LINE \
		-DSDD1_DECOMP \
		-DVAR_CYCLES \
		-DCPU_SHUTDOWN \
		-DSPC700_SHUTDOWN \
		-DNO_INLINE_SET_GET \
		-DNOASM -DHAVE_MKSTEMP \
		'-DACCEPT_SIZE_T=size_t' \
		-DUNZIP_SUPPORT \

VPATH   += $(SFCDIR) $(SFCDIR)/nds $(SFCDIR)/unzip



# $(NETPLAYDEFINES) \
# $(UNZIPDEFINES) \





