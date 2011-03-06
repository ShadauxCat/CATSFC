#1-0----------------------------------------------------------------------------

DS2SDKPATH :=/opt/ds2sdk

#The name final target
TARGET := CATSFC
#The directory where object files & intermediate files will be placed
BUILD := build

#1-1----------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#-------------------------------------------------------------------------------

export OUTPUT := $(CURDIR)/$(TARGET)

.PHONY: $(BUILD) clean

#---------------------------------------------------------------------------------
$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@make --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile
 
#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(OUTPUT).plg
 
#---------------------------------------------------------------------------------

#1-1----------------------------------------------------------------------------
else
#-------------------------------------------------------------------------------

# CROSS :=#
CROSS := /opt/mipsel-4.1.2-nopic/bin/

CC = $(CROSS)mipsel-linux-gcc
AR = $(CROSS)mipsel-linux-ar rcsv
LD	= $(CROSS)mipsel-linux-ld
OBJCOPY	= $(CROSS)mipsel-linux-objcopy
NM	= $(CROSS)mipsel-linux-nm
OBJDUMP	= $(CROSS)mipsel-linux-objdump

TOPDIR = ./..
SFCDIR = $(TOPDIR)/source

FS_DIR = $(DS2SDKPATH)/libsrc/fs
CONSOLE_DIR = $(DS2SDKPATH)/libsrc/console
KEY_DIR = $(DS2SDKPATH)/libsrc/key
ZLIB_DIR = $(DS2SDKPATH)/libsrc/zlib

SRC := 


SSRC :=

LIBS := $(DS2SDKPATH)/lib/libds2b.a -lc -lm -lgcc
EXTLIBS := $(DS2SDKPATH)/lib/libds2a.a

INC := -I$(DS2SDKPATH)/include -I$(FS_DIR) -I$(CONSOLE_DIR) -I$(KEY_DIR) -I$(ZLIB_DIR)

CFLAGS := -mips32 -O3 -mno-abicalls -fno-pic -fno-builtin \
	   -fno-exceptions -ffunction-sections -mlong-calls\
	   -fomit-frame-pointer -msoft-float -G 4



LINKS := $(DS2SDKPATH)/specs/link.xn
STARTS := $(DS2SDKPATH)/specs/start.S
STARTO := start.o

include $(SFCDIR)/sfc.mk

OBJS	:= $(addsuffix .o , $(basename $(notdir $(SRC))))
SOBJS	:= $(addsuffix .o , $(basename $(notdir $(SSRC))))

# OBJS	:= $(SRC:.c=.o)
# SOBJS	:= $(SSRC:.S=.o)


APP	:= sfc.elf


all: $(APP)
	@echo $(INC)
	$(OBJCOPY) -O binary $(APP) sfc.dat
	$(OBJDUMP) -d $(APP) > sfc.dump
	$(NM) $(APP) | sort > sfc.sym
	$(OBJDUMP) -h $(APP) > sfc.map
	$(DS2SDKPATH)/tools/makeplug sfc.dat $(OUTPUT).plg

$(APP): depend $(SOBJS) $(OBJS) $(STARTO) $(LINKS) $(EXTLIBS)
	$(CC) -nostdlib -static -T $(LINKS) -o $@ $(STARTO) $(SOBJS) $(OBJS) $(EXTLIBS) $(LIBS)

$(EXTLIBS): 
	make -C $(DS2SDKPATH)/source/

$(STARTO):
	$(CC) $(CFLAGS) $(INC) -o $@ -c $(STARTS)

.c.o:
	$(CC) $(CFLAGS) $(INC) -o $@ -c $<
.cpp.o:
	$(CC) $(CFLAGS) $(INC) -fno-rtti -o $@ -c $<
.S.o:
	$(CC) $(CFLAGS) $(INC) -D_ASSEMBLER_ -D__ASSEMBLY__ -o $@ -c $<

clean:
	rm -fr *.o $(OBJS) $(OTHER) *.bin *.sym *.map *.dump *.lib
	rm depend

# depend:	Makefile $(OBJS:.o=.c) $(SOBJS:.o=.S)

depend:	Makefile
	$(CC) -MM $(CFLAGS) $(INC) $(SSRC) $(SRC) > $@

sinclude depend

#1-1----------------------------------------------------------------------------
endif
#-------------------------------------------------------------------------------
