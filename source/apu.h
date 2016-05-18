/*******************************************************************************
  Snes9x - Portable Super Nintendo Entertainment System (TM) emulator.

  (c) Copyright 1996 - 2002 Gary Henderson (gary.henderson@ntlworld.com) and
                            Jerremy Koot (jkoot@snes9x.com)

  (c) Copyright 2001 - 2004 John Weidman (jweidman@slip.net)

  (c) Copyright 2002 - 2004 Brad Jorsch (anomie@users.sourceforge.net),
                            funkyass (funkyass@spam.shaw.ca),
                            Joel Yliluoma (http://iki.fi/bisqwit/)
                            Kris Bleakley (codeviolation@hotmail.com),
                            Matthew Kendora,
                            Nach (n-a-c-h@users.sourceforge.net),
                            Peter Bortas (peter@bortas.org) and
                            zones (kasumitokoduck@yahoo.com)

  C4 x86 assembler and some C emulation code
  (c) Copyright 2000 - 2003 zsKnight (zsknight@zsnes.com),
                            _Demo_ (_demo_@zsnes.com), and Nach

  C4 C++ code
  (c) Copyright 2003 Brad Jorsch

  DSP-1 emulator code
  (c) Copyright 1998 - 2004 Ivar (ivar@snes9x.com), _Demo_, Gary Henderson,
                            John Weidman, neviksti (neviksti@hotmail.com),
                            Kris Bleakley, Andreas Naive

  DSP-2 emulator code
  (c) Copyright 2003 Kris Bleakley, John Weidman, neviksti, Matthew Kendora, and
                     Lord Nightmare (lord_nightmare@users.sourceforge.net

  OBC1 emulator code
  (c) Copyright 2001 - 2004 zsKnight, pagefault (pagefault@zsnes.com) and
                            Kris Bleakley
  Ported from x86 assembler to C by sanmaiwashi

  SPC7110 and RTC C++ emulator code
  (c) Copyright 2002 Matthew Kendora with research by
                     zsKnight, John Weidman, and Dark Force

  S-DD1 C emulator code
  (c) Copyright 2003 Brad Jorsch with research by
                     Andreas Naive and John Weidman

  S-RTC C emulator code
  (c) Copyright 2001 John Weidman

  ST010 C++ emulator code
  (c) Copyright 2003 Feather, Kris Bleakley, John Weidman and Matthew Kendora

  Super FX x86 assembler emulator code
  (c) Copyright 1998 - 2003 zsKnight, _Demo_, and pagefault

  Super FX C emulator code
  (c) Copyright 1997 - 1999 Ivar, Gary Henderson and John Weidman


  SH assembler code partly based on x86 assembler code
  (c) Copyright 2002 - 2004 Marcus Comstedt (marcus@mc.pp.se)


  Specific ports contains the works of other authors. See headers in
  individual files.

  Snes9x homepage: http://www.snes9x.com

  Permission to use, copy, modify and distribute Snes9x in both binary and
  source form, for non-commercial purposes, is hereby granted without fee,
  providing that this license information and copyright notice appear with
  all copies and any derived work.

  This software is provided 'as-is', without any express or implied
  warranty. In no event shall the authors be held liable for any damages
  arising from the use of this software.

  Snes9x is freeware for PERSONAL USE only. Commercial users should
  seek permission of the copyright holders first. Commercial use includes
  charging money for Snes9x or software derived from Snes9x.

  The copyright holders request that bug fixes and improvements to the code
  should be forwarded to them so everyone can benefit from the modifications
  in future versions.

  Super NES and Super Nintendo Entertainment System are trademarks of
  Nintendo Co., Limited and its subsidiary companies.
*******************************************************************************/

#ifndef USE_BLARGG_APU

#ifndef _apu_h_
#define _apu_h_

#include "port.h"
#include "spc700.h"

typedef struct
{
   uint8_t*  PC;
   SAPURegisters Registers;
   uint8_t*  RAM;
   uint8_t*  DirectPage;
   bool  APUExecuting;
   uint8_t  Bit;
   uint32_t Address;
   uint8_t*  WaitAddress1;
   uint8_t*  WaitAddress2;
   uint32_t WaitCounter;
   uint8_t  _Carry;
   uint8_t  _Zero;
   uint8_t  _Overflow;
   uint32_t TimerErrorCounter;
   uint32_t Scanline;
   int32_t  OneCycle;
   int32_t  TwoCycles;
} SIAPU;

typedef struct
{
   int32_t  Cycles;
   bool  ShowROM;
   uint8_t  Flags;
   uint8_t  KeyedChannels;
   uint8_t  OutPorts [4];
   uint8_t  DSP [0x80];
   uint8_t  ExtraRAM [64];
   uint16_t Timer [3];
   uint16_t TimerTarget [3];
   bool  TimerEnabled [3];
   bool  TimerValueWritten [3];
} SAPU;

SAPU APU;
SIAPU IAPU;

STATIC inline void S9xAPUUnpackStatus()
{
   IAPU._Zero = ((IAPU.Registers.P & Zero) == 0) | (IAPU.Registers.P & Negative);
   IAPU._Carry = (IAPU.Registers.P & Carry);
   IAPU._Overflow = (IAPU.Registers.P & Overflow) >> 6;
}

STATIC inline void S9xAPUPackStatus()
{
   IAPU.Registers.P &= ~(Zero | Negative | Carry | Overflow);
   IAPU.Registers.P |= IAPU._Carry | ((IAPU._Zero == 0) << 1) |
                       (IAPU._Zero & 0x80) | (IAPU._Overflow << 6);
}

void S9xResetAPU(void);
bool S9xInitAPU();
void S9xDeinitAPU();
void S9xDecacheSamples();
int S9xTraceAPU();
int S9xAPUOPrint(char* buffer, uint16_t Address);
void S9xSetAPUControl(uint8_t byte);
void S9xSetAPUDSP(uint8_t byte);
uint8_t S9xGetAPUDSP();
void S9xSetAPUTimer(uint16_t Address, uint8_t byte);
bool S9xInitSound(int quality, bool stereo, int buffer_size);
void S9xOpenCloseSoundTracingFile(bool);
void S9xPrintAPUState();
extern uint16_t S9xAPUCycles [256];   // Scaled cycle lengths
extern uint16_t S9xAPUCycleLengths [256];   // Raw data.
extern void (*S9xApuOpcodes [256])(void);


#define APU_VOL_LEFT 0x00
#define APU_VOL_RIGHT 0x01
#define APU_P_LOW 0x02
#define APU_P_HIGH 0x03
#define APU_SRCN 0x04
#define APU_ADSR1 0x05
#define APU_ADSR2 0x06
#define APU_GAIN 0x07
#define APU_ENVX 0x08
#define APU_OUTX 0x09

#define APU_MVOL_LEFT 0x0c
#define APU_MVOL_RIGHT 0x1c
#define APU_EVOL_LEFT 0x2c
#define APU_EVOL_RIGHT 0x3c
#define APU_KON 0x4c
#define APU_KOFF 0x5c
#define APU_FLG 0x6c
#define APU_ENDX 0x7c

#define APU_EFB 0x0d
#define APU_PMON 0x2d
#define APU_NON 0x3d
#define APU_EON 0x4d
#define APU_DIR 0x5d
#define APU_ESA 0x6d
#define APU_EDL 0x7d

#define APU_C0 0x0f
#define APU_C1 0x1f
#define APU_C2 0x2f
#define APU_C3 0x3f
#define APU_C4 0x4f
#define APU_C5 0x5f
#define APU_C6 0x6f
#define APU_C7 0x7f

#define APU_SOFT_RESET 0x80
#define APU_MUTE 0x40
#define APU_ECHO_DISABLED 0x20

#define FREQUENCY_MASK 0x3fff
#endif

#else
#include "apu_blargg.h"
#define ONE_APU_CYCLE 21
#define APU_EXECUTE1() do{}while(0)
#define APU_EXECUTE()  do{}while(0)

//typedef struct
//{
//   uint8_t*  PC;
//   SAPURegisters Registers;
//   uint8_t*  RAM;
//   uint8_t*  DirectPage;
//   bool  APUExecuting;
//   uint8_t  Bit;
//   uint32_t Address;
//   uint8_t*  WaitAddress1;
//   uint8_t*  WaitAddress2;
//   uint32_t WaitCounter;
//   uint8_t  _Carry;
//   uint8_t  _Zero;
//   uint8_t  _Overflow;
//   uint32_t TimerErrorCounter;
//   uint32_t Scanline;
//   int32_t  OneCycle;
//   int32_t  TwoCycles;
//} SIAPU;

//typedef struct
//{
//   int32_t  Cycles;
//   bool  ShowROM;
//   uint8_t  Flags;
//   uint8_t  KeyedChannels;
//   uint8_t  OutPorts [4];
//   uint8_t  DSP [0x80];
//   uint8_t  ExtraRAM [64];
//   uint16_t Timer [3];
//   uint16_t TimerTarget [3];
//   bool  TimerEnabled [3];
//   bool  TimerValueWritten [3];
//} SAPU;

//SAPU APU;
//SIAPU IAPU;

#endif

