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

#ifndef _sa1_h_
#define _sa1_h_

#include "memmap.h"
#include "cpuexec.h"

typedef struct
{
   uint8_t   PB;
   uint8_t   DB;
   pair    P;
   pair    A;
   pair    D;
   pair    S;
   pair    X;
   pair    Y;
   uint16_t  PC;
} SSA1Registers;

typedef struct
{
   SOpcodes* S9xOpcodes;
   uint8_t   _Carry;
   uint8_t   _Zero;
   uint8_t   _Negative;
   uint8_t   _Overflow;
   bool   CPUExecuting;
   uint32_t  ShiftedPB;
   uint32_t  ShiftedDB;
   uint32_t  Flags;
   bool   Executing;
   bool   NMIActive;
   bool   IRQActive;
   bool   WaitingForInterrupt;
   bool   Waiting;
   //    uint8_t   WhichEvent;
   uint8_t*   PC;
   uint8_t*   PCBase;
   uint8_t*   BWRAM;
   uint8_t*   PCAtOpcodeStart;
   uint8_t*   WaitAddress;
   uint32_t  WaitCounter;
   uint8_t*   WaitByteAddress1;
   uint8_t*   WaitByteAddress2;
   //    long    Cycles;
   //    long    NextEvent;
   //    long    V_Counter;
   uint8_t*   Map [MEMMAP_NUM_BLOCKS];
   uint8_t*   WriteMap [MEMMAP_NUM_BLOCKS];
   int16_t   op1;
   int16_t   op2;
   int     arithmetic_op;
   int64_t   sum;
   bool   overflow;
   uint8_t   VirtualBitmapFormat;
   bool   in_char_dma;
   uint8_t   variable_bit_pos;
   SSA1Registers Registers;
} SSA1;

#define SA1CheckZero() (SA1._Zero == 0)
#define SA1CheckCarry() (SA1._Carry)
#define SA1CheckIRQ() (SA1.Registers.PL & IRQ)
#define SA1CheckDecimal() (SA1.Registers.PL & Decimal)
#define SA1CheckIndex() (SA1.Registers.PL & IndexFlag)
#define SA1CheckMemory() (SA1.Registers.PL & MemoryFlag)
#define SA1CheckOverflow() (SA1._Overflow)
#define SA1CheckNegative() (SA1._Negative & 0x80)
#define SA1CheckEmulation() (SA1.Registers.P.W & Emulation)

#define SA1ClearFlags(f) (SA1.Registers.P.W &= ~(f))
#define SA1SetFlags(f)   (SA1.Registers.P.W |=  (f))
#define SA1CheckFlag(f)  (SA1.Registers.PL & (f))


uint8_t S9xSA1GetByte(uint32_t);
uint16_t S9xSA1GetWord(uint32_t);
void S9xSA1SetByte(uint8_t, uint32_t);
void S9xSA1SetWord(uint16_t, uint32_t);
void S9xSA1SetPCBase(uint32_t);
uint8_t S9xGetSA1(uint32_t);
void S9xSetSA1(uint8_t, uint32_t);

extern SOpcodes S9xSA1OpcodesM1X1 [256];
extern SOpcodes S9xSA1OpcodesM1X0 [256];
extern SOpcodes S9xSA1OpcodesM0X1 [256];
extern SOpcodes S9xSA1OpcodesM0X0 [256];
extern SSA1 SA1;

void S9xSA1MainLoop();
void S9xSA1Init();
void S9xFixSA1AfterSnapshotLoad();
void S9xSA1ExecuteDuringSleep();

#define SNES_IRQ_SOURCE     (1 << 7)
#define TIMER_IRQ_SOURCE    (1 << 6)
#define DMA_IRQ_SOURCE      (1 << 5)

STATIC inline void S9xSA1UnpackStatus()
{
   SA1._Zero = (SA1.Registers.PL & Zero) == 0;
   SA1._Negative = (SA1.Registers.PL & Negative);
   SA1._Carry = (SA1.Registers.PL & Carry);
   SA1._Overflow = (SA1.Registers.PL & Overflow) >> 6;
}

STATIC inline void S9xSA1PackStatus()
{
   SA1.Registers.PL &= ~(Zero | Negative | Carry | Overflow);
   SA1.Registers.PL |= SA1._Carry | ((SA1._Zero == 0) << 1) |
                       (SA1._Negative & 0x80) | (SA1._Overflow << 6);
}

STATIC inline void S9xSA1FixCycles()
{
   if (SA1CheckEmulation())
      SA1.S9xOpcodes = S9xSA1OpcodesM1X1;
   else if (SA1CheckMemory())
   {
      if (SA1CheckIndex())
         SA1.S9xOpcodes = S9xSA1OpcodesM1X1;
      else
         SA1.S9xOpcodes = S9xSA1OpcodesM1X0;
   }
   else
   {
      if (SA1CheckIndex())
         SA1.S9xOpcodes = S9xSA1OpcodesM0X1;
      else
         SA1.S9xOpcodes = S9xSA1OpcodesM0X0;
   }
}
#endif

