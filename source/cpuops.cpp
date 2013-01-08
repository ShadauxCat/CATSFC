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

/*****************************************************************************/
/* CPU-S9xOpcodes.CPP                                                                            */
/* This file contains all the opcodes                                                         */
/*****************************************************************************/

#include "snes9x.h"
#include "memmap.h"
#include "debug.h"
#include "missing.h"
#include "apu.h"
#include "sa1.h"
#include "spc7110.h"

#include "cpuexec.h"
#include "cpuaddr.h"
#include "cpuops.h"
#include "cpumacro.h"
#include "apu.h"

EXTERN_C long OpAddress;

// For use with the opcodes whose functions here examine the OpAddress.
static void OpAddressPassthrough (long Addr)
{
    OpAddress = Addr;
}

/* ADC *************************************************************************************** */
static void Op69M1 (void)
{
    Immediate8 (READ, ADC8);
}

static void Op69M0 (void)
{
    Immediate16 (READ, ADC16);
}

static void Op65M1 (void)
{
    Direct (READ, ADC8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op65M0 (void)
{
    Direct (READ, ADC16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op75M1 (void)
{
    DirectIndexedX (READ, ADC8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
//    if (ICPU.Registers.DL != 0)
//	CPU.Cycles += TWO_CYCLES;
//    else
//	CPU.Cycles += ONE_CYCLE;
#endif
}

static void Op75M0 (void)
{
    DirectIndexedX (READ, ADC16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
//    if (ICPU.Registers.DL != 0)
//	CPU.Cycles += TWO_CYCLES;
//    else
//	CPU.Cycles += ONE_CYCLE;
#endif
}

static void Op72M1 (void)
{
    DirectIndirect (READ, ADC8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op72M0 (void)
{
    DirectIndirect (READ, ADC16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op61M1 (void)
{
    DirectIndexedIndirect (READ, ADC8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op61M0 (void)
{
    DirectIndexedIndirect (READ, ADC16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op71M1 (void)
{
    DirectIndirectIndexed (READ, ADC8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op71M0 (void)
{
    DirectIndirectIndexed (READ, ADC16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op67M1 (void)
{
    DirectIndirectLong (READ, ADC8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op67M0 (void)
{
    DirectIndirectLong (READ, ADC16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op77M1 (void)
{
    DirectIndirectIndexedLong (READ, ADC8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op77M0 (void)
{
    DirectIndirectIndexedLong (READ, ADC16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op6DM1 (void)
{
    Absolute (READ, ADC8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void Op6DM0 (void)
{
    Absolute (READ, ADC16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void Op7DM1 (void)
{
    AbsoluteIndexedX (READ, ADC8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void Op7DM0 (void)
{
    AbsoluteIndexedX (READ, ADC16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void Op79M1 (void)
{
    AbsoluteIndexedY (READ, ADC8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void Op79M0 (void)
{
    AbsoluteIndexedY (READ, ADC16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void Op6FM1 (void)
{
    AbsoluteLong (READ, ADC8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + CPU.MemSpeed;
#endif
}

static void Op6FM0 (void)
{
    AbsoluteLong (READ, ADC16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + CPU.MemSpeed;
#endif
}

static void Op7FM1 (void)
{
    AbsoluteLongIndexedX (READ, ADC8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + CPU.MemSpeed;
#endif
}

static void Op7FM0 (void)
{
    AbsoluteLongIndexedX (READ, ADC16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + CPU.MemSpeed;
#endif
}

static void Op63M1 (void)
{
    StackRelative (READ, ADC8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
#endif
}

static void Op63M0 (void)
{
    StackRelative (READ, ADC16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
#endif
}

static void Op73M1 (void)
{
    StackRelativeIndirectIndexed (READ, ADC8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + TWO_CYCLES;
#endif
}

static void Op73M0 (void)
{
    StackRelativeIndirectIndexed (READ, ADC16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + TWO_CYCLES;
#endif
}

/**********************************************************************************************/

/* AND *************************************************************************************** */
static void Op29M1 (void)
{
    ICPU.Registers.AL &= *CPU.PC++;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
    SetZN8 (ICPU.Registers.AL);
}

static void Op29M0 (void)
{
#ifdef FAST_LSB_WORD_ACCESS
    ICPU.Registers.A.W &= *(uint16 *) CPU.PC;
#else
    ICPU.Registers.A.W &= *CPU.PC + (*(CPU.PC + 1) << 8);
#endif
    CPU.PC += 2;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
    SetZN16 (ICPU.Registers.A.W);
}

static void Op25M1 (void)
{
    Direct (READ, AND8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op25M0 (void)
{
    Direct (READ, AND16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op35M1 (void)
{
    DirectIndexedX (READ, AND8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
//    if (ICPU.Registers.DL != 0)
//	CPU.Cycles += TWO_CYCLES;
//    else
//	CPU.Cycles += ONE_CYCLE;
#endif
}

static void Op35M0 (void)
{
    DirectIndexedX (READ, AND16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
//    if (ICPU.Registers.DL != 0)
//	CPU.Cycles += TWO_CYCLES;
//    else
//	CPU.Cycles += ONE_CYCLE;
#endif
}

static void Op32M1 (void)
{
    DirectIndirect (READ, AND8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op32M0 (void)
{
    DirectIndirect (READ, AND16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op21M1 (void)
{
    DirectIndexedIndirect (READ, AND8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op21M0 (void)
{
    DirectIndexedIndirect (READ, AND16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op31M1 (void)
{
    DirectIndirectIndexed (READ, AND8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op31M0 (void)
{
    DirectIndirectIndexed (READ, AND16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op27M1 (void)
{
    DirectIndirectLong (READ, AND8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op27M0 (void)
{
    DirectIndirectLong (READ, AND16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op37M1 (void)
{
    DirectIndirectIndexedLong (READ, AND8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op37M0 (void)
{
    DirectIndirectIndexedLong (READ, AND16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op2DM1 (void)
{
    Absolute (READ, AND8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void Op2DM0 (void)
{
    Absolute (READ, AND16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void Op3DM1 (void)
{
    AbsoluteIndexedX (READ, AND8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void Op3DM0 (void)
{
    AbsoluteIndexedX (READ, AND16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void Op39M1 (void)
{
    AbsoluteIndexedY (READ, AND8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void Op39M0 (void)
{
    AbsoluteIndexedY (READ, AND16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void Op2FM1 (void)
{
    AbsoluteLong (READ, AND8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + CPU.MemSpeed;
#endif
}

static void Op2FM0 (void)
{
    AbsoluteLong (READ, AND16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + CPU.MemSpeed;
#endif
}

static void Op3FM1 (void)
{
    AbsoluteLongIndexedX (READ, AND8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + CPU.MemSpeed;
#endif
}

static void Op3FM0 (void)
{
    AbsoluteLongIndexedX (READ, AND16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + CPU.MemSpeed;
#endif
}

static void Op23M1 (void)
{
    StackRelative (READ, AND8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
#endif
}

static void Op23M0 (void)
{
    StackRelative (READ, AND16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
#endif
}

static void Op33M1 (void)
{
    StackRelativeIndirectIndexed (READ, AND8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + TWO_CYCLES;
#endif
}

static void Op33M0 (void)
{
    StackRelativeIndirectIndexed (READ, AND16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + TWO_CYCLES;
#endif
}
/**********************************************************************************************/

/* ASL *************************************************************************************** */
static void Op0AM1 (void)
{
    A_ASL8 ();
}

static void Op0AM0 (void)
{
    A_ASL16 ();
}

static void Op06M1 (void)
{
    Direct (MODIFY, ASL8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed /* memory */ + ONE_CYCLE /* opcode */;
#endif
}

static void Op06M0 (void)
{
    Direct (MODIFY, ASL16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed /* memory */ + ONE_CYCLE /* opcode */;
#endif
}

static void Op16M1 (void)
{
    DirectIndexedX (MODIFY, ASL8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE /* memory */ + ONE_CYCLE /* opcode */;
#endif
}

static void Op16M0 (void)
{
    DirectIndexedX (MODIFY, ASL16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE /* memory */ + ONE_CYCLE /* opcode */;
#endif
}

static void Op0EM1 (void)
{
    Absolute (MODIFY, ASL8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 /* memory */ + ONE_CYCLE /* opcode */;
#endif
}

static void Op0EM0 (void)
{
    Absolute (MODIFY, ASL16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 /* memory */ + ONE_CYCLE /* opcode */;
#endif
}

static void Op1EM1 (void)
{
    AbsoluteIndexedX (MODIFY, ASL8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 /* memory */ + ONE_CYCLE /* opcode */;
#endif
}

static void Op1EM0 (void)
{
    AbsoluteIndexedX (MODIFY, ASL16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 /* memory */ + ONE_CYCLE /* opcode */;
#endif
}
/**********************************************************************************************/

/* BIT *************************************************************************************** */
static void Op89M1 (void)
{
    ICPU._Zero = ICPU.Registers.AL & *CPU.PC++;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op89M0 (void)
{
#ifdef FAST_LSB_WORD_ACCESS
    ICPU._Zero = (ICPU.Registers.A.W & *(uint16 *) CPU.PC) != 0;
#else
    ICPU._Zero = (ICPU.Registers.A.W & (*CPU.PC + (*(CPU.PC + 1) << 8))) != 0;
#endif	
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
    CPU.PC += 2;
}

static void Op24M1 (void)
{
    Direct (READ, BIT8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op24M0 (void)
{
    Direct (READ, BIT16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op34M1 (void)
{
    DirectIndexedX (READ, BIT8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
//    if (ICPU.Registers.DL != 0)
//	CPU.Cycles += TWO_CYCLES;
//    else
//	CPU.Cycles += ONE_CYCLE;
#endif
}

static void Op34M0 (void)
{
    DirectIndexedX (READ, BIT16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
//    if (ICPU.Registers.DL != 0)
//	CPU.Cycles += TWO_CYCLES;
//    else
//	CPU.Cycles += ONE_CYCLE;
#endif
}

static void Op2CM1 (void)
{
    Absolute (READ, BIT8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void Op2CM0 (void)
{
    Absolute (READ, BIT16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void Op3CM1 (void)
{
    AbsoluteIndexedX (READ, BIT8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void Op3CM0 (void)
{
    AbsoluteIndexedX (READ, BIT16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}
/**********************************************************************************************/

/* CMP *************************************************************************************** */
static void OpC9M1 (void)
{
    int32 Int32 = (int) ICPU.Registers.AL - (int) *CPU.PC++;
    ICPU._Carry = Int32 >= 0;
    SetZN8 ((uint8) Int32);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpC9M0 (void)
{
    int32 Int32;
#ifdef FAST_LSB_WORD_ACCESS    
    Int32 = (long) ICPU.Registers.A.W - (long) *(uint16 *) CPU.PC;
#else
    Int32 = (long) ICPU.Registers.A.W -
	    (long) (*CPU.PC + (*(CPU.PC + 1) << 8));
#endif
    ICPU._Carry = Int32 >= 0;
    SetZN16 ((uint16) Int32);
    CPU.PC += 2;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void OpC5M1 (void)
{
    Direct (READ, CMP8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpC5M0 (void)
{
    Direct (READ, CMP16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpD5M1 (void)
{
    DirectIndexedX (READ, CMP8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
//    if (ICPU.Registers.DL != 0)
//	CPU.Cycles += TWO_CYCLES;
//    else
//	CPU.Cycles += ONE_CYCLE;
#endif
}

static void OpD5M0 (void)
{
    DirectIndexedX (READ, CMP16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
//    if (ICPU.Registers.DL != 0)
//	CPU.Cycles += TWO_CYCLES;
//    else
//	CPU.Cycles += ONE_CYCLE;
#endif
}

static void OpD2M1 (void)
{
    DirectIndirect (READ, CMP8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpD2M0 (void)
{
    DirectIndirect (READ, CMP16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpC1M1 (void)
{
    DirectIndexedIndirect (READ, CMP8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpC1M0 (void)
{
    DirectIndexedIndirect (READ, CMP16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpD1M1 (void)
{
    DirectIndirectIndexed (READ, CMP8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpD1M0 (void)
{
    DirectIndirectIndexed (READ, CMP16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpC7M1 (void)
{
    DirectIndirectLong (READ, CMP8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpC7M0 (void)
{
    DirectIndirectLong (READ, CMP16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpD7M1 (void)
{
    DirectIndirectIndexedLong (READ, CMP8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpD7M0 (void)
{
    DirectIndirectIndexedLong (READ, CMP16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpCDM1 (void)
{
    Absolute (READ, CMP8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void OpCDM0 (void)
{
    Absolute (READ, CMP16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void OpDDM1 (void)
{
    AbsoluteIndexedX (READ, CMP8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void OpDDM0 (void)
{
    AbsoluteIndexedX (READ, CMP16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void OpD9M1 (void)
{
    AbsoluteIndexedY (READ, CMP8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void OpD9M0 (void)
{
    AbsoluteIndexedY (READ, CMP16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void OpCFM1 (void)
{
    AbsoluteLong (READ, CMP8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + CPU.MemSpeed;
#endif
}

static void OpCFM0 (void)
{
    AbsoluteLong (READ, CMP16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + CPU.MemSpeed;
#endif
}

static void OpDFM1 (void)
{
    AbsoluteLongIndexedX (READ, CMP8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + CPU.MemSpeed;
#endif
}

static void OpDFM0 (void)
{
    AbsoluteLongIndexedX (READ, CMP16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + CPU.MemSpeed;
#endif
}

static void OpC3M1 (void)
{
    StackRelative (READ, CMP8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
#endif
}

static void OpC3M0 (void)
{
    StackRelative (READ, CMP16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
#endif
}

static void OpD3M1 (void)
{
    StackRelativeIndirectIndexed (READ, CMP8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + TWO_CYCLES;
#endif
}

static void OpD3M0 (void)
{
    StackRelativeIndirectIndexed (READ, CMP16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + TWO_CYCLES;
#endif
}

/**********************************************************************************************/

/* CMX *************************************************************************************** */
static void OpE0X1 (void)
{
    int32 Int32 = (int) ICPU.Registers.XL - (int) *CPU.PC++;
    ICPU._Carry = Int32 >= 0;
    SetZN8 ((uint8) Int32);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpE0X0 (void)
{
    int32 Int32;
#ifdef FAST_LSB_WORD_ACCESS    
    Int32 = (long) ICPU.Registers.X.W - (long) *(uint16 *) CPU.PC;
#else
    Int32 = (long) ICPU.Registers.X.W -
	    (long) (*CPU.PC + (*(CPU.PC + 1) << 8));
#endif
    ICPU._Carry = Int32 >= 0;
    SetZN16 ((uint16) Int32);
    CPU.PC += 2;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void OpE4X1 (void)
{
    Direct (READ, CMX8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpE4X0 (void)
{
    Direct (READ, CMX16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpECX1 (void)
{
    Absolute (READ, CMX8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void OpECX0 (void)
{
    Absolute (READ, CMX16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

/**********************************************************************************************/

/* CMY *************************************************************************************** */
static void OpC0X1 (void)
{
    int32 Int32 = (int) ICPU.Registers.YL - (int) *CPU.PC++;
    ICPU._Carry = Int32 >= 0;
    SetZN8 ((uint8) Int32);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpC0X0 (void)
{
    int32 Int32;
#ifdef FAST_LSB_WORD_ACCESS    
    Int32 = (long) ICPU.Registers.Y.W - (long) *(uint16 *) CPU.PC;
#else
    Int32 = (long) ICPU.Registers.Y.W -
	    (long) (*CPU.PC + (*(CPU.PC + 1) << 8));
#endif
    ICPU._Carry = Int32 >= 0;
    SetZN16 ((uint16) Int32);
    CPU.PC += 2;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void OpC4X1 (void)
{
    Direct (READ, CMY8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpC4X0 (void)
{
    Direct (READ, CMY16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpCCX1 (void)
{
    Absolute (READ, CMY8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void OpCCX0 (void)
{
    Absolute (READ, CMY16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

/**********************************************************************************************/

/* DEC *************************************************************************************** */
static void Op3AM1 (void)
{
    A_DEC8 ();
}

static void Op3AM0 (void)
{
    A_DEC16 ();
}

static void OpC6M1 (void)
{
    Direct (MODIFY, DEC8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed /* memory */ + ONE_CYCLE /* opcode */;
#endif
}

static void OpC6M0 (void)
{
    Direct (MODIFY, DEC16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed /* memory */ + ONE_CYCLE /* opcode */;
#endif
}

static void OpD6M1 (void)
{
    DirectIndexedX (MODIFY, DEC8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE /* memory */ + ONE_CYCLE /* opcode */;
//    if (ICPU.Registers.DL != 0)
//	CPU.Cycles += TWO_CYCLES;
//    else
//	CPU.Cycles += ONE_CYCLE;
#endif
}

static void OpD6M0 (void)
{
    DirectIndexedX (MODIFY, DEC16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE /* memory */ + ONE_CYCLE /* opcode */;
//    if (ICPU.Registers.DL != 0)
//	CPU.Cycles += TWO_CYCLES;
//    else
//	CPU.Cycles += ONE_CYCLE;
#endif
}

static void OpCEM1 (void)
{
    Absolute (MODIFY, DEC8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 /* memory */ + ONE_CYCLE /* opcode */;
#endif
}

static void OpCEM0 (void)
{
    Absolute (MODIFY, DEC16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 /* memory */ + ONE_CYCLE /* opcode */;
#endif
}

static void OpDEM1 (void)
{
    AbsoluteIndexedX (MODIFY, DEC8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 /* memory */ + ONE_CYCLE /* opcode */;
#endif
}

static void OpDEM0 (void)
{
    AbsoluteIndexedX (MODIFY, DEC16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 /* memory */ + ONE_CYCLE /* opcode */;
#endif
}

/**********************************************************************************************/

/* EOR *************************************************************************************** */
static void Op49M1 (void)
{
    ICPU.Registers.AL ^= *CPU.PC++;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
    SetZN8 (ICPU.Registers.AL);
}

static void Op49M0 (void)
{
#ifdef FAST_LSB_WORD_ACCESS
    ICPU.Registers.A.W ^= *(uint16 *) CPU.PC;
#else
    ICPU.Registers.A.W ^= *CPU.PC + (*(CPU.PC + 1) << 8);
#endif
    CPU.PC += 2;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
    SetZN16 (ICPU.Registers.A.W);
}

static void Op45M1 (void)
{
    Direct (READ, EOR8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op45M0 (void)
{
    Direct (READ, EOR16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op55M1 (void)
{
    DirectIndexedX (READ, EOR8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
//    if (ICPU.Registers.DL != 0)
//	CPU.Cycles += TWO_CYCLES;
//    else
//	CPU.Cycles += ONE_CYCLE;
#endif
}

static void Op55M0 (void)
{
    DirectIndexedX (READ, EOR16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
//    if (ICPU.Registers.DL != 0)
//	CPU.Cycles += TWO_CYCLES;
//    else
//	CPU.Cycles += ONE_CYCLE;
#endif
}

static void Op52M1 (void)
{
    DirectIndirect (READ, EOR8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op52M0 (void)
{
    DirectIndirect (READ, EOR16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op41M1 (void)
{
    DirectIndexedIndirect (READ, EOR8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op41M0 (void)
{
    DirectIndexedIndirect (READ, EOR16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op51M1 (void)
{
    DirectIndirectIndexed (READ, EOR8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op51M0 (void)
{
    DirectIndirectIndexed (READ, EOR16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op47M1 (void)
{
    DirectIndirectLong (READ, EOR8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op47M0 (void)
{
    DirectIndirectLong (READ, EOR16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op57M1 (void)
{
    DirectIndirectIndexedLong (READ, EOR8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op57M0 (void)
{
    DirectIndirectIndexedLong (READ, EOR16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op4DM1 (void)
{
    Absolute (READ, EOR8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void Op4DM0 (void)
{
    Absolute (READ, EOR16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void Op5DM1 (void)
{
    AbsoluteIndexedX (READ, EOR8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void Op5DM0 (void)
{
    AbsoluteIndexedX (READ, EOR16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void Op59M1 (void)
{
    AbsoluteIndexedY (READ, EOR8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void Op59M0 (void)
{
    AbsoluteIndexedY (READ, EOR16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void Op4FM1 (void)
{
    AbsoluteLong (READ, EOR8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + CPU.MemSpeed;
#endif
}

static void Op4FM0 (void)
{
    AbsoluteLong (READ, EOR16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + CPU.MemSpeed;
#endif
}

static void Op5FM1 (void)
{
    AbsoluteLongIndexedX (READ, EOR8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + CPU.MemSpeed;
#endif
}

static void Op5FM0 (void)
{
    AbsoluteLongIndexedX (READ, EOR16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + CPU.MemSpeed;
#endif
}

static void Op43M1 (void)
{
    StackRelative (READ, EOR8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
#endif
}

static void Op43M0 (void)
{
    StackRelative (READ, EOR16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
#endif
}

static void Op53M1 (void)
{
    StackRelativeIndirectIndexed (READ, EOR8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + TWO_CYCLES;
#endif
}

static void Op53M0 (void)
{
    StackRelativeIndirectIndexed (READ, EOR16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + TWO_CYCLES;
#endif
}

/**********************************************************************************************/

/* INC *************************************************************************************** */
static void Op1AM1 (void)
{
    A_INC8 ();
}

static void Op1AM0 (void)
{
    A_INC16 ();
}

static void OpE6M1 (void)
{
    Direct (MODIFY, INC8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed /* memory */ + ONE_CYCLE /* opcode */;
#endif
}

static void OpE6M0 (void)
{
    Direct (MODIFY, INC16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed /* memory */ + ONE_CYCLE /* opcode */;
#endif
}

static void OpF6M1 (void)
{
    DirectIndexedX (MODIFY, INC8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE /* memory */ + ONE_CYCLE /* opcode */;
//    if (ICPU.Registers.DL != 0)
//	CPU.Cycles += TWO_CYCLES;
//    else
//	CPU.Cycles += ONE_CYCLE;
#endif
}

static void OpF6M0 (void)
{
    DirectIndexedX (MODIFY, INC16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE /* memory */ + ONE_CYCLE /* opcode */;
//    if (ICPU.Registers.DL != 0)
//	CPU.Cycles += TWO_CYCLES;
//    else
//	CPU.Cycles += ONE_CYCLE;
#endif
}

static void OpEEM1 (void)
{
    Absolute (MODIFY, INC8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 /* memory */ + ONE_CYCLE /* opcode */;
#endif
}

static void OpEEM0 (void)
{
    Absolute (MODIFY, INC16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 /* memory */ + ONE_CYCLE /* opcode */;
#endif
}

static void OpFEM1 (void)
{
    AbsoluteIndexedX (MODIFY, INC8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 /* memory */ + ONE_CYCLE /* opcode */;
#endif
}

static void OpFEM0 (void)
{
    AbsoluteIndexedX (MODIFY, INC16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 /* memory */ + ONE_CYCLE /* opcode */;
#endif
}

/**********************************************************************************************/
/* LDA *************************************************************************************** */
static void OpA9M1 (void)
{
    ICPU.Registers.AL = *CPU.PC++;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
    SetZN8 (ICPU.Registers.AL);
}

static void OpA9M0 (void)
{
#ifdef FAST_LSB_WORD_ACCESS
    ICPU.Registers.A.W = *(uint16 *) CPU.PC;
#else
    ICPU.Registers.A.W = *CPU.PC + (*(CPU.PC + 1) << 8);
#endif

    CPU.PC += 2;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
    SetZN16 (ICPU.Registers.A.W);
}

static void OpA5M1 (void)
{
    Direct (READ, LDA8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpA5M0 (void)
{
    Direct (READ, LDA16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpB5M1 (void)
{
    DirectIndexedX (READ, LDA8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
//    if (ICPU.Registers.DL != 0)
//	CPU.Cycles += TWO_CYCLES;
//    else
//	CPU.Cycles += ONE_CYCLE;
#endif
}

static void OpB5M0 (void)
{
    DirectIndexedX (READ, LDA16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
//    if (ICPU.Registers.DL != 0)
//	CPU.Cycles += TWO_CYCLES;
//    else
//	CPU.Cycles += ONE_CYCLE;
#endif
}

static void OpB2M1 (void)
{
    DirectIndirect (READ, LDA8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpB2M0 (void)
{
    DirectIndirect (READ, LDA16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpA1M1 (void)
{
    DirectIndexedIndirect (READ, LDA8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpA1M0 (void)
{
    DirectIndexedIndirect (READ, LDA16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpB1M1 (void)
{
    DirectIndirectIndexed (READ, LDA8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpB1M0 (void)
{
    DirectIndirectIndexed (READ, LDA16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpA7M1 (void)
{
    DirectIndirectLong (READ, LDA8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpA7M0 (void)
{
    DirectIndirectLong (READ, LDA16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpB7M1 (void)
{
    DirectIndirectIndexedLong (READ, LDA8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpB7M0 (void)
{
    DirectIndirectIndexedLong (READ, LDA16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpADM1 (void)
{
    Absolute (READ, LDA8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void OpADM0 (void)
{
    Absolute (READ, LDA16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void OpBDM1 (void)
{
    AbsoluteIndexedX (READ, LDA8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void OpBDM0 (void)
{
    AbsoluteIndexedX (READ, LDA16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void OpB9M1 (void)
{
    AbsoluteIndexedY (READ, LDA8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void OpB9M0 (void)
{
    AbsoluteIndexedY (READ, LDA16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void OpAFM1 (void)
{
    AbsoluteLong (READ, LDA8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + CPU.MemSpeed;
#endif
}

static void OpAFM0 (void)
{
    AbsoluteLong (READ, LDA16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + CPU.MemSpeed;
#endif
}

static void OpBFM1 (void)
{
    AbsoluteLongIndexedX (READ, LDA8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + CPU.MemSpeed;
#endif
}

static void OpBFM0 (void)
{
    AbsoluteLongIndexedX (READ, LDA16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + CPU.MemSpeed;
#endif
}

static void OpA3M1 (void)
{
    StackRelative (READ, LDA8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
#endif
}

static void OpA3M0 (void)
{
    StackRelative (READ, LDA16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
#endif
}

static void OpB3M1 (void)
{
    StackRelativeIndirectIndexed (READ, LDA8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + TWO_CYCLES;
#endif
}

static void OpB3M0 (void)
{
    StackRelativeIndirectIndexed (READ, LDA16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + TWO_CYCLES;
#endif
}

/**********************************************************************************************/

/* LDX *************************************************************************************** */
static void OpA2X1 (void)
{
    ICPU.Registers.XL = *CPU.PC++;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
    SetZN8 (ICPU.Registers.XL);
}

static void OpA2X0 (void)
{
#ifdef FAST_LSB_WORD_ACCESS
    ICPU.Registers.X.W = *(uint16 *) CPU.PC;
#else
    ICPU.Registers.X.W = *CPU.PC + (*(CPU.PC + 1) << 8);
#endif
    CPU.PC += 2;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
    SetZN16 (ICPU.Registers.X.W);
}

static void OpA6X1 (void)
{
    Direct (READ, LDX8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpA6X0 (void)
{
    Direct (READ, LDX16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpB6X1 (void)
{
    DirectIndexedY (READ, LDX8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
//    if (ICPU.Registers.DL != 0)
//	CPU.Cycles += TWO_CYCLES;
//    else
//	CPU.Cycles += ONE_CYCLE;
#endif
}

static void OpB6X0 (void)
{
    DirectIndexedY (READ, LDX16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
//    if (ICPU.Registers.DL != 0)
//	CPU.Cycles += TWO_CYCLES;
//    else
//	CPU.Cycles += ONE_CYCLE;
#endif
}

static void OpAEX1 (void)
{
    Absolute (READ, LDX8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void OpAEX0 (void)
{
    Absolute (READ, LDX16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void OpBEX1 (void)
{
    AbsoluteIndexedY (READ, LDX8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void OpBEX0 (void)
{
    AbsoluteIndexedY (READ, LDX16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}
/**********************************************************************************************/

/* LDY *************************************************************************************** */
static void OpA0X1 (void)
{
    ICPU.Registers.YL = *CPU.PC++;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
    SetZN8 (ICPU.Registers.YL);
}

static void OpA0X0 (void)
{
#ifdef FAST_LSB_WORD_ACCESS
    ICPU.Registers.Y.W = *(uint16 *) CPU.PC;
#else
    ICPU.Registers.Y.W = *CPU.PC + (*(CPU.PC + 1) << 8);
#endif

    CPU.PC += 2;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
    SetZN16 (ICPU.Registers.Y.W);
}

static void OpA4X1 (void)
{
    Direct (READ, LDY8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpA4X0 (void)
{
    Direct (READ, LDY16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpB4X1 (void)
{
    DirectIndexedX (READ, LDY8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
//    if (ICPU.Registers.DL != 0)
//	CPU.Cycles += TWO_CYCLES;
//    else
//	CPU.Cycles += ONE_CYCLE;
#endif
}

static void OpB4X0 (void)
{
    DirectIndexedX (READ, LDY16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
//    if (ICPU.Registers.DL != 0)
//	CPU.Cycles += TWO_CYCLES;
//    else
//	CPU.Cycles += ONE_CYCLE;
#endif
}

static void OpACX1 (void)
{
    Absolute (READ, LDY8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void OpACX0 (void)
{
    Absolute (READ, LDY16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void OpBCX1 (void)
{
    AbsoluteIndexedX (READ, LDY8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void OpBCX0 (void)
{
    AbsoluteIndexedX (READ, LDY16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}
/**********************************************************************************************/

/* LSR *************************************************************************************** */
static void Op4AM1 (void)
{
    A_LSR8 ();
}

static void Op4AM0 (void)
{
    A_LSR16 ();
}

static void Op46M1 (void)
{
    Direct (MODIFY, LSR8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed /* memory */ + ONE_CYCLE /* opcode */;
#endif
}

static void Op46M0 (void)
{
    Direct (MODIFY, LSR16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed /* memory */ + ONE_CYCLE /* opcode */;
#endif
}

static void Op56M1 (void)
{
    DirectIndexedX (MODIFY, LSR8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE /* memory */ + ONE_CYCLE /* opcode */;
//    if (ICPU.Registers.DL != 0)
//	CPU.Cycles += TWO_CYCLES;
//    else
//	CPU.Cycles += ONE_CYCLE;
#endif
}

static void Op56M0 (void)
{
    DirectIndexedX (MODIFY, LSR16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE /* memory */ + ONE_CYCLE /* opcode */;
//    if (ICPU.Registers.DL != 0)
//	CPU.Cycles += TWO_CYCLES;
//    else
//	CPU.Cycles += ONE_CYCLE;
#endif
}

static void Op4EM1 (void)
{
    Absolute (MODIFY, LSR8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 /* memory */ + ONE_CYCLE /* opcode */;
#endif
}

static void Op4EM0 (void)
{
    Absolute (MODIFY, LSR16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 /* memory */ + ONE_CYCLE /* opcode */;
#endif
}

static void Op5EM1 (void)
{
    AbsoluteIndexedX (MODIFY, LSR8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 /* memory */ + ONE_CYCLE /* opcode */;
#endif
}

static void Op5EM0 (void)
{
    AbsoluteIndexedX (MODIFY, LSR16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 /* memory */ + ONE_CYCLE /* opcode */;
#endif
}

/**********************************************************************************************/

/* ORA *************************************************************************************** */
static void Op09M1 (void)
{
    ICPU.Registers.AL |= *CPU.PC++;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
    SetZN8 (ICPU.Registers.AL);
}

static void Op09M0 (void)
{
#ifdef FAST_LSB_WORD_ACCESS
    ICPU.Registers.A.W |= *(uint16 *) CPU.PC;
#else
    ICPU.Registers.A.W |= *CPU.PC + (*(CPU.PC + 1) << 8);
#endif
    CPU.PC += 2;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
    SetZN16 (ICPU.Registers.A.W);
}

static void Op05M1 (void)
{
    Direct (READ, ORA8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op05M0 (void)
{
    Direct (READ, ORA16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op15M1 (void)
{
    DirectIndexedX (READ, ORA8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
//    if (ICPU.Registers.DL != 0)
//	CPU.Cycles += TWO_CYCLES;
//    else
//	CPU.Cycles += ONE_CYCLE;
#endif
}

static void Op15M0 (void)
{
    DirectIndexedX (READ, ORA16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
//    if (ICPU.Registers.DL != 0)
//	CPU.Cycles += TWO_CYCLES;
//    else
//	CPU.Cycles += ONE_CYCLE;
#endif
}

static void Op12M1 (void)
{
    DirectIndirect (READ, ORA8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op12M0 (void)
{
    DirectIndirect (READ, ORA16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op01M1 (void)
{
    DirectIndexedIndirect (READ, ORA8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op01M0 (void)
{
    DirectIndexedIndirect (READ, ORA16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op11M1 (void)
{
    DirectIndirectIndexed (READ, ORA8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op11M0 (void)
{
    DirectIndirectIndexed (READ, ORA16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op07M1 (void)
{
    DirectIndirectLong (READ, ORA8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op07M0 (void)
{
    DirectIndirectLong (READ, ORA16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op17M1 (void)
{
    DirectIndirectIndexedLong (READ, ORA8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op17M0 (void)
{
    DirectIndirectIndexedLong (READ, ORA16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op0DM1 (void)
{
    Absolute (READ, ORA8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void Op0DM0 (void)
{
    Absolute (READ, ORA16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void Op1DM1 (void)
{
    AbsoluteIndexedX (READ, ORA8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void Op1DM0 (void)
{
    AbsoluteIndexedX (READ, ORA16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void Op19M1 (void)
{
    AbsoluteIndexedY (READ, ORA8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void Op19M0 (void)
{
    AbsoluteIndexedY (READ, ORA16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void Op0FM1 (void)
{
    AbsoluteLong (READ, ORA8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + CPU.MemSpeed;
#endif
}

static void Op0FM0 (void)
{
    AbsoluteLong (READ, ORA16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + CPU.MemSpeed;
#endif
}

static void Op1FM1 (void)
{
    AbsoluteLongIndexedX (READ, ORA8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + CPU.MemSpeed;
#endif
}

static void Op1FM0 (void)
{
    AbsoluteLongIndexedX (READ, ORA16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + CPU.MemSpeed;
#endif
}

static void Op03M1 (void)
{
    StackRelative (READ, ORA8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
#endif
}

static void Op03M0 (void)
{
    StackRelative (READ, ORA16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
#endif
}

static void Op13M1 (void)
{
    StackRelativeIndirectIndexed (READ, ORA8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + TWO_CYCLES;
#endif
}

static void Op13M0 (void)
{
    StackRelativeIndirectIndexed (READ, ORA16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + TWO_CYCLES;
#endif
}

/**********************************************************************************************/

/* ROL *************************************************************************************** */
static void Op2AM1 (void)
{
    A_ROL8 ();
}

static void Op2AM0 (void)
{
    A_ROL16 ();
}

static void Op26M1 (void)
{
    Direct (MODIFY, ROL8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed /* memory */ + ONE_CYCLE /* opcode */;
#endif
}

static void Op26M0 (void)
{
    Direct (MODIFY, ROL16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed /* memory */ + ONE_CYCLE /* opcode */;
#endif
}

static void Op36M1 (void)
{
    DirectIndexedX (MODIFY, ROL8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE /* memory */ + ONE_CYCLE /* opcode */;
//    if (ICPU.Registers.DL != 0)
//	CPU.Cycles += TWO_CYCLES;
//    else
//	CPU.Cycles += ONE_CYCLE;
#endif
}

static void Op36M0 (void)
{
    DirectIndexedX (MODIFY, ROL16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE /* memory */ + ONE_CYCLE /* opcode */;
//    if (ICPU.Registers.DL != 0)
//	CPU.Cycles += TWO_CYCLES;
//    else
//	CPU.Cycles += ONE_CYCLE;
#endif
}

static void Op2EM1 (void)
{
    Absolute (MODIFY, ROL8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 /* memory */ + ONE_CYCLE /* opcode */;
#endif
}

static void Op2EM0 (void)
{
    Absolute (MODIFY, ROL16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 /* memory */ + ONE_CYCLE /* opcode */;
#endif
}

static void Op3EM1 (void)
{
    AbsoluteIndexedX (MODIFY, ROL8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 /* memory */ + ONE_CYCLE /* opcode */;
#endif
}

static void Op3EM0 (void)
{
    AbsoluteIndexedX (MODIFY, ROL16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 /* memory */ + ONE_CYCLE /* opcode */;
#endif
}
/**********************************************************************************************/

/* ROR *************************************************************************************** */
static void Op6AM1 (void)
{
    A_ROR8 ();
}

static void Op6AM0 (void)
{
    A_ROR16 ();
}

static void Op66M1 (void)
{
    Direct (MODIFY, ROR8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed /* memory */ + ONE_CYCLE /* opcode */;
#endif
}

static void Op66M0 (void)
{
    Direct (MODIFY, ROR16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed /* memory */ + ONE_CYCLE /* opcode */;
#endif
}

static void Op76M1 (void)
{
    DirectIndexedX (MODIFY, ROR8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE /* memory */ + ONE_CYCLE /* opcode */;
//    if (ICPU.Registers.DL != 0)
//	CPU.Cycles += TWO_CYCLES;
//    else
//	CPU.Cycles += ONE_CYCLE;
#endif
}

static void Op76M0 (void)
{
    DirectIndexedX (MODIFY, ROR16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE /* memory */ + ONE_CYCLE /* opcode */;
//    if (ICPU.Registers.DL != 0)
//	CPU.Cycles += TWO_CYCLES;
//    else
//	CPU.Cycles += ONE_CYCLE;
#endif
}

static void Op6EM1 (void)
{
    Absolute (MODIFY, ROR8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 /* memory */ + ONE_CYCLE /* opcode */;
#endif
}

static void Op6EM0 (void)
{
    Absolute (MODIFY, ROR16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 /* memory */ + ONE_CYCLE /* opcode */;
#endif
}

static void Op7EM1 (void)
{
    AbsoluteIndexedX (MODIFY, ROR8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 /* memory */ + ONE_CYCLE /* opcode */;
#endif
}

static void Op7EM0 (void)
{
    AbsoluteIndexedX (MODIFY, ROR16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 /* memory */ + ONE_CYCLE /* opcode */;
#endif
}
/**********************************************************************************************/

/* SBC *************************************************************************************** */
static void OpE9M1 (void)
{
    Immediate8 (READ, SBC8);
}

static void OpE9M0 (void)
{
    Immediate16 (READ, SBC16);
}

static void OpE5M1 (void)
{
    Direct (READ, SBC8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpE5M0 (void)
{
    Direct (READ, SBC16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpF5M1 (void)
{
    DirectIndexedX (READ, SBC8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
//    if (ICPU.Registers.DL != 0)
//	CPU.Cycles += TWO_CYCLES;
//    else
//	CPU.Cycles += ONE_CYCLE;
#endif
}

static void OpF5M0 (void)
{
    DirectIndexedX (READ, SBC16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
//    if (ICPU.Registers.DL != 0)
//	CPU.Cycles += TWO_CYCLES;
//    else
//	CPU.Cycles += ONE_CYCLE;
#endif
}

static void OpF2M1 (void)
{
    DirectIndirect (READ, SBC8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpF2M0 (void)
{
    DirectIndirect (READ, SBC16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpE1M1 (void)
{
    DirectIndexedIndirect (READ, SBC8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpE1M0 (void)
{
    DirectIndexedIndirect (READ, SBC16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpF1M1 (void)
{
    DirectIndirectIndexed (READ, SBC8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpF1M0 (void)
{
    DirectIndirectIndexed (READ, SBC16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpE7M1 (void)
{
    DirectIndirectLong (READ, SBC8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpE7M0 (void)
{
    DirectIndirectLong (READ, SBC16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpF7M1 (void)
{
    DirectIndirectIndexedLong (READ, SBC8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpF7M0 (void)
{
    DirectIndirectIndexedLong (READ, SBC16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void OpEDM1 (void)
{
    Absolute (READ, SBC8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void OpEDM0 (void)
{
    Absolute (READ, SBC16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void OpFDM1 (void)
{
    AbsoluteIndexedX (READ, SBC8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void OpFDM0 (void)
{
    AbsoluteIndexedX (READ, SBC16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void OpF9M1 (void)
{
    AbsoluteIndexedY (READ, SBC8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void OpF9M0 (void)
{
    AbsoluteIndexedY (READ, SBC16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void OpEFM1 (void)
{
    AbsoluteLong (READ, SBC8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + CPU.MemSpeed;
#endif
}

static void OpEFM0 (void)
{
    AbsoluteLong (READ, SBC16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + CPU.MemSpeed;
#endif
}

static void OpFFM1 (void)
{
    AbsoluteLongIndexedX (READ, SBC8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + CPU.MemSpeed;
#endif
}

static void OpFFM0 (void)
{
    AbsoluteLongIndexedX (READ, SBC16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + CPU.MemSpeed;
#endif
}

static void OpE3M1 (void)
{
    StackRelative (READ, SBC8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
#endif
}

static void OpE3M0 (void)
{
    StackRelative (READ, SBC16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
#endif
}

static void OpF3M1 (void)
{
    StackRelativeIndirectIndexed (READ, SBC8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + TWO_CYCLES;
#endif
}

static void OpF3M0 (void)
{
    StackRelativeIndirectIndexed (READ, SBC16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + TWO_CYCLES;
#endif
}
/**********************************************************************************************/

/* STA *************************************************************************************** */
static void Op85M1 (void)
{
    Direct (WRITE, STA8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op85M0 (void)
{
    Direct (WRITE, STA16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op95M1 (void)
{
    DirectIndexedX (WRITE, STA8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
//    if (ICPU.Registers.DL != 0)
//	CPU.Cycles += TWO_CYCLES;
//    else
//	CPU.Cycles += ONE_CYCLE;
#endif
}

static void Op95M0 (void)
{
    DirectIndexedX (WRITE, STA16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
//    if (ICPU.Registers.DL != 0)
//	CPU.Cycles += TWO_CYCLES;
//    else
//	CPU.Cycles += ONE_CYCLE;
#endif
}

static void Op92M1 (void)
{
    DirectIndirect (WRITE, STA8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op92M0 (void)
{
    DirectIndirect (WRITE, STA16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op81M1 (void)
{
    DirectIndexedIndirect (WRITE, STA8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
#ifdef noVAR_CYCLES
    if (CheckIndex ())
	CPU.Cycles += ONE_CYCLE;
#endif
}

static void Op81M0 (void)
{
    DirectIndexedIndirect (WRITE, STA16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
#ifdef noVAR_CYCLES
    if (CheckIndex ())
	CPU.Cycles += ONE_CYCLE;
#endif
}

static void Op91M1 (void)
{
    DirectIndirectIndexed (WRITE, STA8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op91M0 (void)
{
    DirectIndirectIndexed (WRITE, STA16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op87M1 (void)
{
    DirectIndirectLong (WRITE, STA8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op87M0 (void)
{
    DirectIndirectLong (WRITE, STA16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op97M1 (void)
{
    DirectIndirectIndexedLong (WRITE, STA8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op97M0 (void)
{
    DirectIndirectIndexedLong (WRITE, STA16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op8DM1 (void)
{
    Absolute (WRITE, STA8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void Op8DM0 (void)
{
    Absolute (WRITE, STA16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void Op9DM1 (void)
{
    AbsoluteIndexedX (WRITE, STA8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void Op9DM0 (void)
{
    AbsoluteIndexedX (WRITE, STA16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void Op99M1 (void)
{
    AbsoluteIndexedY (WRITE, STA8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void Op99M0 (void)
{
    AbsoluteIndexedY (WRITE, STA16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void Op8FM1 (void)
{
    AbsoluteLong (WRITE, STA8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + CPU.MemSpeed;
#endif
}

static void Op8FM0 (void)
{
    AbsoluteLong (WRITE, STA16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + CPU.MemSpeed;
#endif
}

static void Op9FM1 (void)
{
    AbsoluteLongIndexedX (WRITE, STA8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + CPU.MemSpeed;
#endif
}

static void Op9FM0 (void)
{
    AbsoluteLongIndexedX (WRITE, STA16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + CPU.MemSpeed;
#endif
}

static void Op83M1 (void)
{
    StackRelative (WRITE, STA8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
#endif
}

static void Op83M0 (void)
{
    StackRelative (WRITE, STA16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
#endif
}

static void Op93M1 (void)
{
    StackRelativeIndirectIndexed (WRITE, STA8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + TWO_CYCLES;
#endif
}

static void Op93M0 (void)
{
    StackRelativeIndirectIndexed (WRITE, STA16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + TWO_CYCLES;
#endif
}
/**********************************************************************************************/

/* STX *************************************************************************************** */
static void Op86X1 (void)
{
    Direct (WRITE, STX8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op86X0 (void)
{
    Direct (WRITE, STX16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op96X1 (void)
{
    DirectIndexedY (WRITE, STX8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
//    if (ICPU.Registers.DL != 0)
//	CPU.Cycles += TWO_CYCLES;
//    else
//	CPU.Cycles += ONE_CYCLE;
#endif
}

static void Op96X0 (void)
{
    DirectIndexedY (WRITE, STX16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
//    if (ICPU.Registers.DL != 0)
//	CPU.Cycles += TWO_CYCLES;
//    else
//	CPU.Cycles += ONE_CYCLE;
#endif
}

static void Op8EX1 (void)
{
    Absolute (WRITE, STX8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void Op8EX0 (void)
{
    Absolute (WRITE, STX16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}
/**********************************************************************************************/

/* STY *************************************************************************************** */
static void Op84X1 (void)
{
    Direct (WRITE, STY8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op84X0 (void)
{
    Direct (WRITE, STY16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op94X1 (void)
{
    DirectIndexedX (WRITE, STY8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
//    if (ICPU.Registers.DL != 0)
//	CPU.Cycles += TWO_CYCLES;
//    else
//	CPU.Cycles += ONE_CYCLE;
#endif
}

static void Op94X0 (void)
{
    DirectIndexedX (WRITE, STY16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
//    if (ICPU.Registers.DL != 0)
//	CPU.Cycles += TWO_CYCLES;
//    else
//	CPU.Cycles += ONE_CYCLE;
#endif
}

static void Op8CX1 (void)
{
    Absolute (WRITE, STY8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void Op8CX0 (void)
{
    Absolute (WRITE, STY16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}
/**********************************************************************************************/

/* STZ *************************************************************************************** */
static void Op64M1 (void)
{
    Direct (WRITE, STZ8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op64M0 (void)
{
    Direct (WRITE, STZ16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
}

static void Op74M1 (void)
{
    DirectIndexedX (WRITE, STZ8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
//    if (ICPU.Registers.DL != 0)
//	CPU.Cycles += TWO_CYCLES;
//    else
//	CPU.Cycles += ONE_CYCLE;
#endif
}

static void Op74M0 (void)
{
    DirectIndexedX (WRITE, STZ16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
//    if (ICPU.Registers.DL != 0)
//	CPU.Cycles += TWO_CYCLES;
//    else
//	CPU.Cycles += ONE_CYCLE;
#endif
}

static void Op9CM1 (void)
{
    Absolute (WRITE, STZ8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void Op9CM0 (void)
{
    Absolute (WRITE, STZ16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void Op9EM1 (void)
{
    AbsoluteIndexedX (WRITE, STZ8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

static void Op9EM0 (void)
{
    AbsoluteIndexedX (WRITE, STZ16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

/**********************************************************************************************/

/* TRB *************************************************************************************** */
static void Op14M1 (void)
{
    Direct (MODIFY, TRB8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed /* memory */ + ONE_CYCLE /* opcode */;
#endif
}

static void Op14M0 (void)
{
    Direct (MODIFY, TRB16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed /* memory */ + ONE_CYCLE /* opcode */;
#endif
}

static void Op1CM1 (void)
{
    Absolute (MODIFY, TRB8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 /* memory */ + ONE_CYCLE /* opcode */;
#endif
}

static void Op1CM0 (void)
{
    Absolute (MODIFY, TRB16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 /* memory */ + ONE_CYCLE /* opcode */;
#endif
}
/**********************************************************************************************/

/* TSB *************************************************************************************** */
static void Op04M1 (void)
{
    Direct (MODIFY, TSB8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed /* memory */ + ONE_CYCLE /* opcode */;
#endif
}

static void Op04M0 (void)
{
    Direct (MODIFY, TSB16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed /* memory */ + ONE_CYCLE /* opcode */;
#endif
}

static void Op0CM1 (void)
{
    Absolute (MODIFY, TSB8);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 /* memory */ + ONE_CYCLE /* opcode */;
#endif
}

static void Op0CM0 (void)
{
    Absolute (MODIFY, TSB16);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 /* memory */ + ONE_CYCLE /* opcode */;
#endif
}

/**********************************************************************************************/

/* Branch Instructions *********************************************************************** */
#ifndef SA1_OPCODES
#define BranchCheck0()\
    if( CPU.BranchSkip)\
    {\
	CPU.BranchSkip = FALSE;\
	if (!Settings.SoundSkipMethod)\
	    if( CPU.PC - CPU.PCBase > OpAddress)\
	        return;\
    }

#define BranchCheck1()\
    if( CPU.BranchSkip)\
    {\
	CPU.BranchSkip = FALSE;\
	if (!Settings.SoundSkipMethod) {\
	    if( CPU.PC - CPU.PCBase > OpAddress)\
	        return;\
	} else \
	if (Settings.SoundSkipMethod == 1)\
	    return;\
	if (Settings.SoundSkipMethod == 3)\
	    if( CPU.PC - CPU.PCBase > OpAddress)\
	        return;\
	    else\
		CPU.PC = CPU.PCBase + OpAddress;\
    }

#define BranchCheck2()\
    if( CPU.BranchSkip)\
    {\
	CPU.BranchSkip = FALSE;\
	if (!Settings.SoundSkipMethod) {\
	    if( CPU.PC - CPU.PCBase > OpAddress)\
	        return;\
	} else \
	if (Settings.SoundSkipMethod == 1)\
	    CPU.PC = CPU.PCBase + OpAddress;\
	if (Settings.SoundSkipMethod == 3)\
	    if (CPU.PC - CPU.PCBase > OpAddress)\
	        return;\
	    else\
		CPU.PC = CPU.PCBase + OpAddress;\
    }
#else
#define BranchCheck0()
#define BranchCheck1()
#define BranchCheck2()
#endif

#ifdef CPU_SHUTDOWN
#ifndef SA1_OPCODES
inline void CPUShutdown()
{
    if (Settings.Shutdown && CPU.PC == CPU.WaitAddress)
    {
	// Don't skip cycles with a pending NMI or IRQ - could cause delayed
	// interrupt. Interrupts are delayed for a few cycles already, but
	// the delay could allow the shutdown code to cycle skip again.
	// Was causing screen flashing on Top Gear 3000.

	if (CPU.WaitCounter == 0 && 
	    !(CPU.Flags & (IRQ_PENDING_FLAG | NMI_FLAG)))
	{
	    CPU.WaitAddress = NULL;
	    if (Settings.SA1)
		S9xSA1ExecuteDuringSleep ();
	    CPU.Cycles = CPU.NextEvent;
	    if (IAPU.APUExecuting)
	    {
		ICPU.CPUExecuting = FALSE;
		do
		{
		    APU_EXECUTE1();
		} while (APU.Cycles < CPU.NextEvent);
		ICPU.CPUExecuting = TRUE;
	    }
	}
	else
	if (CPU.WaitCounter >= 2)
	    CPU.WaitCounter = 1;
	else
	    CPU.WaitCounter--;
    }
}
#else
inline void CPUShutdown()
{
    if (Settings.Shutdown && CPU.PC == CPU.WaitAddress)
    {
	if (CPU.WaitCounter >= 1)
	{
	    SA1.Executing = FALSE;
	    SA1.CPUExecuting = FALSE;
	}
	else
	    CPU.WaitCounter++;
    }
}
#endif
#else
#define CPUShutdown()
#endif

/* BCC */
static void Op90 (void)
{
    Relative (JUMP, OpAddressPassthrough);
    BranchCheck0 ();
    if (!CheckCarry ())
    {
	CPU.PC = CPU.PCBase + OpAddress;
#ifndef SA1_OPCODES
	CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
#endif
	CPUShutdown();
    }
#ifndef SA1_OPCODES
    else
    {
        CPU.Cycles += CPU.MemSpeed;
    }
#endif
}

/* BCS */
static void OpB0 (void)
{
    Relative (JUMP, OpAddressPassthrough);
    BranchCheck0 ();
    if (CheckCarry ())
    {
	CPU.PC = CPU.PCBase + OpAddress;
#ifndef SA1_OPCODES
	CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
#endif
	CPUShutdown ();
    }
#ifndef SA1_OPCODES
    else
    {
        CPU.Cycles += CPU.MemSpeed;
    }
#endif
}

/* BEQ */
static void OpF0 (void)
{
    Relative (JUMP, OpAddressPassthrough);
    BranchCheck2 ();
    if (CheckZero ())
    {
	CPU.PC = CPU.PCBase + OpAddress;
#ifndef SA1_OPCODES
	CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
#endif
	CPUShutdown ();
    }
#ifndef SA1_OPCODES
    else
    {
        CPU.Cycles += CPU.MemSpeed;
    }
#endif
}

/* BMI */
static void Op30 (void)
{
    Relative (JUMP, OpAddressPassthrough);
    BranchCheck1 ();
    if (CheckNegative ())
    {
	CPU.PC = CPU.PCBase + OpAddress;
#ifndef SA1_OPCODES
	CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
#endif
	CPUShutdown ();
    }
#ifndef SA1_OPCODES
    else
    {
        CPU.Cycles += CPU.MemSpeed;
    }
#endif
}

/* BNE */
static void OpD0 (void)
{
    Relative (JUMP, OpAddressPassthrough);
    BranchCheck1 ();
    if (!CheckZero ())
    {
	CPU.PC = CPU.PCBase + OpAddress;

#ifndef SA1_OPCODES
	CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
#endif
	CPUShutdown ();
    }
#ifndef SA1_OPCODES
    else
    {
        CPU.Cycles += CPU.MemSpeed;
    }
#endif
}

/* BPL */
static void Op10 (void)
{
    Relative (JUMP, OpAddressPassthrough);
    BranchCheck1 ();
    if (!CheckNegative ())
    {
	CPU.PC = CPU.PCBase + OpAddress;
#ifndef SA1_OPCODES
	CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
#endif
	CPUShutdown ();
    }
#ifndef SA1_OPCODES
    else
    {
        CPU.Cycles += CPU.MemSpeed;
    }
#endif
}

/* BRA */
static void Op80 (void)
{
    Relative (JUMP, OpAddressPassthrough);
    CPU.PC = CPU.PCBase + OpAddress;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
#endif
    CPUShutdown ();
}

/* BVC */
static void Op50 (void)
{
    Relative (JUMP, OpAddressPassthrough);
    BranchCheck0 ();
    if (!CheckOverflow ())
    {
	CPU.PC = CPU.PCBase + OpAddress;
#ifndef SA1_OPCODES
	CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
#endif
	CPUShutdown ();
    }
#ifndef SA1_OPCODES
    else
    {
        CPU.Cycles += CPU.MemSpeed;
    }
#endif
}

/* BVS */
static void Op70 (void)
{
    Relative (JUMP, OpAddressPassthrough);
    BranchCheck0 ();
    if (CheckOverflow ())
    {
	CPU.PC = CPU.PCBase + OpAddress;
#ifndef SA1_OPCODES
	CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
#endif
	CPUShutdown ();
    }
#ifndef SA1_OPCODES
    else
    {
        CPU.Cycles += CPU.MemSpeed;
    }
#endif
}
/**********************************************************************************************/

/* ClearFlag Instructions ******************************************************************** */
/* CLC */
static void Op18 (void)
{
    ClearCarry ();
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
}

/* CLD */
static void OpD8 (void)
{
    ClearDecimal ();
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
}

/* CLI */
static void Op58 (void)
{
    ClearIRQ ();
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
/*    CHECK_FOR_IRQ(); */
}

/* CLV */
static void OpB8 (void)
{
    ClearOverflow ();
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
}
/**********************************************************************************************/

/* DEX/DEY *********************************************************************************** */
static void OpCAX1 (void)
{
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
#ifdef CPU_SHUTDOWN
    CPU.WaitAddress = NULL;
#endif

    ICPU.Registers.XL--;
    SetZN8 (ICPU.Registers.XL);
}

static void OpCAX0 (void)
{
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
#ifdef CPU_SHUTDOWN
    CPU.WaitAddress = NULL;
#endif

    ICPU.Registers.X.W--;
    SetZN16 (ICPU.Registers.X.W);
}

static void Op88X1 (void)
{
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
#ifdef CPU_SHUTDOWN
    CPU.WaitAddress = NULL;
#endif

    ICPU.Registers.YL--;
    SetZN8 (ICPU.Registers.YL);
}

static void Op88X0 (void)
{
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
#ifdef CPU_SHUTDOWN
    CPU.WaitAddress = NULL;
#endif

    ICPU.Registers.Y.W--;
    SetZN16 (ICPU.Registers.Y.W);
}
/**********************************************************************************************/

/* INX/INY *********************************************************************************** */
static void OpE8X1 (void)
{
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
#ifdef CPU_SHUTDOWN
    CPU.WaitAddress = NULL;
#endif

    ICPU.Registers.XL++;
    SetZN8 (ICPU.Registers.XL);
}

static void OpE8X0 (void)
{
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
#ifdef CPU_SHUTDOWN
    CPU.WaitAddress = NULL;
#endif

    ICPU.Registers.X.W++;
    SetZN16 (ICPU.Registers.X.W);
}

static void OpC8X1 (void)
{
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
#ifdef CPU_SHUTDOWN
    CPU.WaitAddress = NULL;
#endif

    ICPU.Registers.YL++;
    SetZN8 (ICPU.Registers.YL);
}

static void OpC8X0 (void)
{
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
#ifdef CPU_SHUTDOWN
    CPU.WaitAddress = NULL;
#endif

    ICPU.Registers.Y.W++;
    SetZN16 (ICPU.Registers.Y.W);
}

/**********************************************************************************************/

/* NOP *************************************************************************************** */
static void OpEA (void)
{
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
}
/**********************************************************************************************/

/* PUSH Instructions ************************************************************************* */
/* #define PushW(w) \
 *    S9xSetWord (w, ICPU.Registers.S.W - 1);\
 *    ICPU.Registers.S.W -= 2;                 
 */
#define PushB(b)\
    S9xSetByte (b, ICPU.Registers.S.W--);

#define PushBE(b)\
	S9xSetByte (b, ICPU.Registers.S.W--);\
	ICPU.Registers.SH=0x01;


#define PushW(w) \
    S9xSetByte ((w)>>8, ICPU.Registers.S.W);\
	S9xSetByte ((w)&0xff, (ICPU.Registers.S.W - 1)&0xFFFF);\
    ICPU.Registers.S.W -= 2;

#define PushWE(w) \
    S9xSetByte ((w)>>8, ICPU.Registers.S.W--);\
	ICPU.Registers.SH=0x01;\
	S9xSetByte ((w)&0xff, (ICPU.Registers.S.W--)&0xFFFF);\
    ICPU.Registers.SH = 0x01;

#define PushWENew(w) \
    S9xSetByte ((w)>>8, ICPU.Registers.S.W--);\
	S9xSetByte ((w)&0xff, (ICPU.Registers.S.W--)&0xFFFF);\
    ICPU.Registers.SH = 0x01;

//PEA NL
static void OpF4E1 (void)
{
    Absolute (NONE, OpAddressPassthrough);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
    PushWENew ((unsigned short)OpAddress);
}

static void OpF4 (void)
{
    Absolute (NONE, OpAddressPassthrough);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
    PushW ((unsigned short)OpAddress);
}

//PEI NL
static void OpD4E1 (void)
{
    DirectIndirect (NONE, OpAddressPassthrough);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
    PushWENew ((unsigned short)OpAddress);
}

static void OpD4 (void)
{
    DirectIndirect (NONE, OpAddressPassthrough);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
    PushW ((unsigned short)OpAddress);
}

//PER NL
static void Op62E1 (void)
{
    RelativeLong (NONE, OpAddressPassthrough);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + ONE_CYCLE;
#endif
    PushWENew ((unsigned short)OpAddress);
}

static void Op62 (void)
{
    RelativeLong (NONE, OpAddressPassthrough);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + ONE_CYCLE;
#endif
    PushW ((unsigned short)OpAddress);
}


//PHA
static void Op48E1 (void)
{
    PushBE (ICPU.Registers.AL);
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
}

static void Op48M1 (void)
{
    PushB (ICPU.Registers.AL);
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
}

static void Op48M0 (void)
{
    PushW (ICPU.Registers.A.W);
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
}

//PHB
static void Op8BE1 (void)
{
    PushBE (ICPU.Registers.DB);
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
}
static void Op8B (void)
{
    PushB (ICPU.Registers.DB);
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
}

//PHD NL
static void Op0BE1 (void)
{
    PushWENew (ICPU.Registers.D.W);
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
}

static void Op0B (void)
{
    PushW (ICPU.Registers.D.W);
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
}

//PHK
static void Op4BE1 (void)
{
    PushBE (ICPU.Registers.PB);
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
}

static void Op4B (void)
{
    PushB (ICPU.Registers.PB);
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
}

//PHP
static void Op08E1 (void)
{
    S9xPackStatus ();
    PushBE (ICPU.Registers.PL);
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
}

static void Op08 (void)
{
    S9xPackStatus ();
    PushB (ICPU.Registers.PL);
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
}

//PHX
static void OpDAE1 (void)
{
    PushBE (ICPU.Registers.XL);
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
}

static void OpDAX1 (void)
{
    PushB (ICPU.Registers.XL);
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
}

static void OpDAX0 (void)
{
    PushW (ICPU.Registers.X.W);
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
}

//PHY
static void Op5AE1 (void)
{
    PushBE (ICPU.Registers.YL);
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
}

static void Op5AX1 (void)
{
    PushB (ICPU.Registers.YL);
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
}

static void Op5AX0 (void)
{
    PushW (ICPU.Registers.Y.W);
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
}
/**********************************************************************************************/

/* PULL Instructions ************************************************************************* */
#define PullW(w) \
	w = S9xGetByte (++ICPU.Registers.S.W); \
	w |= (S9xGetByte (++ICPU.Registers.S.W)<<8);

/*	w = S9xGetWord (ICPU.Registers.S.W + 1); \
	ICPU.Registers.S.W += 2;
*/

#define PullB(b)\
	b = S9xGetByte (++ICPU.Registers.S.W);

#define PullBE(b)\
	ICPU.Registers.S.W++;\
	ICPU.Registers.SH=0x01;\
	b = S9xGetByte (ICPU.Registers.S.W);

#define PullWE(w) \
	ICPU.Registers.S.W++;\
	ICPU.Registers.SH=0x01;\
	w = S9xGetByte (ICPU.Registers.S.W); \
	ICPU.Registers.S.W++; \
	ICPU.Registers.SH=0x01;\
	w |= (S9xGetByte (ICPU.Registers.S.W)<<8);

#define PullWENew(w) \
	PullW(w);\
	ICPU.Registers.SH=0x01;	

//PLA
static void Op68E1 (void)
{
#ifndef SA1_OPCODES
    CPU.Cycles += TWO_CYCLES;
#endif
    PullBE (ICPU.Registers.AL);
    SetZN8 (ICPU.Registers.AL);
}

static void Op68M1 (void)
{
#ifndef SA1_OPCODES
    CPU.Cycles += TWO_CYCLES;
#endif
    PullB (ICPU.Registers.AL);
    SetZN8 (ICPU.Registers.AL);
}

static void Op68M0 (void)
{
#ifndef SA1_OPCODES
    CPU.Cycles += TWO_CYCLES;
#endif
    PullW (ICPU.Registers.A.W);
    SetZN16 (ICPU.Registers.A.W);
}

//PLB
static void OpABE1 (void)
{
#ifndef SA1_OPCODES
    CPU.Cycles += TWO_CYCLES;
#endif
    PullBE (ICPU.Registers.DB);
    SetZN8 (ICPU.Registers.DB);
    ICPU.ShiftedDB = ICPU.Registers.DB << 16;
}

static void OpAB (void)
{
#ifndef SA1_OPCODES
    CPU.Cycles += TWO_CYCLES;
#endif
    PullB (ICPU.Registers.DB);
    SetZN8 (ICPU.Registers.DB);
    ICPU.ShiftedDB = ICPU.Registers.DB << 16;
}

/* PHP */
//PLD NL
static void Op2BE1 (void)
{
#ifndef SA1_OPCODES
    CPU.Cycles += TWO_CYCLES;
#endif
    PullWENew (ICPU.Registers.D.W);
    SetZN16 (ICPU.Registers.D.W);
}

static void Op2B (void)
{
#ifndef SA1_OPCODES
    CPU.Cycles += TWO_CYCLES;
#endif
    PullW (ICPU.Registers.D.W);
    SetZN16 (ICPU.Registers.D.W);
}

/* PLP */
static void Op28E1 (void)
{
#ifndef SA1_OPCODES
    CPU.Cycles += TWO_CYCLES;
#endif
    PullBE (ICPU.Registers.PL);
    S9xUnpackStatus ();

    if (CheckIndex ())
    {
	ICPU.Registers.XH = 0;
	ICPU.Registers.YH = 0;
    }
    S9xFixCycles();
/*     CHECK_FOR_IRQ();*/
}

static void Op28 (void)
{
#ifndef SA1_OPCODES
    CPU.Cycles += TWO_CYCLES;
#endif
    PullB (ICPU.Registers.PL);
    S9xUnpackStatus ();

    if (CheckIndex ())
    {
	ICPU.Registers.XH = 0;
	ICPU.Registers.YH = 0;
    }
    S9xFixCycles();
/*     CHECK_FOR_IRQ();*/
}

//PLX
static void OpFAE1 (void)
{
#ifndef SA1_OPCODES
    CPU.Cycles += TWO_CYCLES;
#endif
    PullBE (ICPU.Registers.XL);
    SetZN8 (ICPU.Registers.XL);
}

static void OpFAX1 (void)
{
#ifndef SA1_OPCODES
    CPU.Cycles += TWO_CYCLES;
#endif
    PullB (ICPU.Registers.XL);
    SetZN8 (ICPU.Registers.XL);
}

static void OpFAX0 (void)
{
#ifndef SA1_OPCODES
    CPU.Cycles += TWO_CYCLES;
#endif
    PullW (ICPU.Registers.X.W);
    SetZN16 (ICPU.Registers.X.W);
}

//PLY
static void Op7AE1 (void)
{
#ifndef SA1_OPCODES
    CPU.Cycles += TWO_CYCLES;
#endif
    PullBE (ICPU.Registers.YL);
    SetZN8 (ICPU.Registers.YL);
}

static void Op7AX1 (void)
{
#ifndef SA1_OPCODES
    CPU.Cycles += TWO_CYCLES;
#endif
    PullB (ICPU.Registers.YL);
    SetZN8 (ICPU.Registers.YL);
}

static void Op7AX0 (void)
{
#ifndef SA1_OPCODES
    CPU.Cycles += TWO_CYCLES;
#endif
    PullW (ICPU.Registers.Y.W);
    SetZN16 (ICPU.Registers.Y.W);
}

/**********************************************************************************************/

/* SetFlag Instructions ********************************************************************** */
/* SEC */
static void Op38 (void)
{
    SetCarry ();
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
}

/* SED */
static void OpF8 (void)
{
    SetDecimal ();
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
    missing.decimal_mode = 1;
}

/* SEI */
static void Op78 (void)
{
    SetIRQ ();
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
}
/**********************************************************************************************/

/* Transfer Instructions ********************************************************************* */
/* TAX8 */
static void OpAAX1 (void)
{
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
    ICPU.Registers.XL = ICPU.Registers.AL;
    SetZN8 (ICPU.Registers.XL);
}

/* TAX16 */
static void OpAAX0 (void)
{
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
    ICPU.Registers.X.W = ICPU.Registers.A.W;
    SetZN16 (ICPU.Registers.X.W);
}

/* TAY8 */
static void OpA8X1 (void)
{
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
    ICPU.Registers.YL = ICPU.Registers.AL;
    SetZN8 (ICPU.Registers.YL);
}

/* TAY16 */
static void OpA8X0 (void)
{
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
    ICPU.Registers.Y.W = ICPU.Registers.A.W;
    SetZN16 (ICPU.Registers.Y.W);
}

static void Op5B (void)
{
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
    ICPU.Registers.D.W = ICPU.Registers.A.W;
    SetZN16 (ICPU.Registers.D.W);
}

static void Op1B (void)
{
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
    ICPU.Registers.S.W = ICPU.Registers.A.W;
    if (CheckEmulation())
	ICPU.Registers.SH = 1;
}

static void Op7B (void)
{
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
    ICPU.Registers.A.W = ICPU.Registers.D.W;
    SetZN16 (ICPU.Registers.A.W);
}

static void Op3B (void)
{
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
    ICPU.Registers.A.W = ICPU.Registers.S.W;
    SetZN16 (ICPU.Registers.A.W);
}

static void OpBAX1 (void)
{
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
    ICPU.Registers.XL = ICPU.Registers.SL;
    SetZN8 (ICPU.Registers.XL);
}

static void OpBAX0 (void)
{
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
    ICPU.Registers.X.W = ICPU.Registers.S.W;
    SetZN16 (ICPU.Registers.X.W);
}

static void Op8AM1 (void)
{
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
    ICPU.Registers.AL = ICPU.Registers.XL;
    SetZN8 (ICPU.Registers.AL);
}

static void Op8AM0 (void)
{
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
    ICPU.Registers.A.W = ICPU.Registers.X.W;
    SetZN16 (ICPU.Registers.A.W);
}

static void Op9A (void)
{
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
    ICPU.Registers.S.W = ICPU.Registers.X.W;
    if (CheckEmulation())
	ICPU.Registers.SH = 1;
}

static void Op9BX1 (void)
{
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
    ICPU.Registers.YL = ICPU.Registers.XL;
    SetZN8 (ICPU.Registers.YL);
}

static void Op9BX0 (void)
{
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
    ICPU.Registers.Y.W = ICPU.Registers.X.W;
    SetZN16 (ICPU.Registers.Y.W);
}

static void Op98M1 (void)
{
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
    ICPU.Registers.AL = ICPU.Registers.YL;
    SetZN8 (ICPU.Registers.AL);
}

static void Op98M0 (void)
{
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
    ICPU.Registers.A.W = ICPU.Registers.Y.W;
    SetZN16 (ICPU.Registers.A.W);
}

static void OpBBX1 (void)
{
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
    ICPU.Registers.XL = ICPU.Registers.YL;
    SetZN8 (ICPU.Registers.XL);
}

static void OpBBX0 (void)
{
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
    ICPU.Registers.X.W = ICPU.Registers.Y.W;
    SetZN16 (ICPU.Registers.X.W);
}

/**********************************************************************************************/

/* XCE *************************************************************************************** */
static void OpFB (void)
{
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif

    uint8 A1 = ICPU._Carry;
    uint8 A2 = ICPU.Registers.PH;
    ICPU._Carry = A2 & 1;
    ICPU.Registers.PH = A1;

    if (CheckEmulation())
    {
	SetFlags (MemoryFlag | IndexFlag);
	ICPU.Registers.SH = 1;
	missing.emulate6502 = 1;
    }
    if (CheckIndex ())
    {
	ICPU.Registers.XH = 0;
	ICPU.Registers.YH = 0;
    }
    S9xFixCycles();
}
/**********************************************************************************************/

/* BRK *************************************************************************************** */
static void Op00 (void)
{
#ifdef DEBUGGER
    if (CPU.Flags & TRACE_FLAG)
	S9xTraceMessage ("*** BRK");
#endif

#ifndef SA1_OPCODES
    CPU.BRKTriggered = TRUE;
#endif

    if (!CheckEmulation())
    {
	PushB (ICPU.Registers.PB);
	PushW (CPU.PC - CPU.PCBase + 1);
	S9xPackStatus ();
	PushB (ICPU.Registers.PL);
	OpenBus = ICPU.Registers.PL;
	ClearDecimal ();
	SetIRQ ();

	ICPU.Registers.PB = 0;
	ICPU.ShiftedPB = 0;
	S9xSetPCBase (S9xGetWord (0xFFE6));
#ifndef SA1_OPCODES
        CPU.Cycles += TWO_CYCLES;
#endif
    }
    else
    {
	PushW (CPU.PC - CPU.PCBase);
	S9xPackStatus ();
	PushB (ICPU.Registers.PL);
	OpenBus = ICPU.Registers.PL;
	ClearDecimal ();
	SetIRQ ();

	ICPU.Registers.PB = 0;
	ICPU.ShiftedPB = 0;
	S9xSetPCBase (S9xGetWord (0xFFFE));
#ifndef SA1_OPCODES
	CPU.Cycles += ONE_CYCLE;
#endif
    }
}
/**********************************************************************************************/

/* BRL ************************************************************************************** */
static void Op82 (void)
{
    RelativeLong (JUMP, OpAddressPassthrough);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + ONE_CYCLE;
#endif
    S9xSetPCBase (ICPU.ShiftedPB + OpAddress);
}
/**********************************************************************************************/

/* IRQ *************************************************************************************** */
void S9xOpcode_IRQ (void)
{
#ifdef DEBUGGER
    if (CPU.Flags & TRACE_FLAG)
	S9xTraceMessage ("*** IRQ");
#endif
    if (!CheckEmulation())
    {
	PushB (ICPU.Registers.PB);
	PushW (CPU.PC - CPU.PCBase);
	S9xPackStatus ();
	PushB (ICPU.Registers.PL);
	OpenBus = ICPU.Registers.PL;
	ClearDecimal ();
	SetIRQ ();

	ICPU.Registers.PB = 0;
	ICPU.ShiftedPB = 0;
#ifdef SA1_OPCODES
	S9xSA1SetPCBase (Memory.FillRAM [0x2207] |
			 (Memory.FillRAM [0x2208] << 8));
#else
	if (Settings.SA1 && (Memory.FillRAM [0x2209] & 0x40))
	    S9xSetPCBase (Memory.FillRAM [0x220e] | 
			  (Memory.FillRAM [0x220f] << 8));
	else
	    S9xSetPCBase (S9xGetWord (0xFFEE));
        CPU.Cycles += TWO_CYCLES;
#endif
    }
    else
    {
	PushW (CPU.PC - CPU.PCBase);
	S9xPackStatus ();
	PushB (ICPU.Registers.PL);
	OpenBus = ICPU.Registers.PL;
	ClearDecimal ();
	SetIRQ ();

	ICPU.Registers.PB = 0;
	ICPU.ShiftedPB = 0;
#ifdef SA1_OPCODES
	S9xSA1SetPCBase (Memory.FillRAM [0x2207] |
			 (Memory.FillRAM [0x2208] << 8));
#else
	if (Settings.SA1 && (Memory.FillRAM [0x2209] & 0x40))
	    S9xSetPCBase (Memory.FillRAM [0x220e] | 
			  (Memory.FillRAM [0x220f] << 8));
	else
	    S9xSetPCBase (S9xGetWord (0xFFFE));
	CPU.Cycles += ONE_CYCLE;
#endif
    }
}

/**********************************************************************************************/

/* NMI *************************************************************************************** */
void S9xOpcode_NMI (void)
{
#ifdef DEBUGGER
    if (CPU.Flags & TRACE_FLAG)
	S9xTraceMessage ("*** NMI");
#endif
    if (!CheckEmulation())
    {
	PushB (ICPU.Registers.PB);
	PushW (CPU.PC - CPU.PCBase);
	S9xPackStatus ();
	PushB (ICPU.Registers.PL);
	OpenBus = ICPU.Registers.PL;
	ClearDecimal ();
	SetIRQ ();

	ICPU.Registers.PB = 0;
	ICPU.ShiftedPB = 0;
#ifdef SA1_OPCODES
	S9xSA1SetPCBase (Memory.FillRAM [0x2205] |
			 (Memory.FillRAM [0x2206] << 8));
#else
	if (Settings.SA1 && (Memory.FillRAM [0x2209] & 0x20))
	    S9xSetPCBase (Memory.FillRAM [0x220c] |
			  (Memory.FillRAM [0x220d] << 8));
	else
	    S9xSetPCBase (S9xGetWord (0xFFEA));
	CPU.Cycles += TWO_CYCLES;
#endif
    }
    else
    {
	PushW (CPU.PC - CPU.PCBase);
	S9xPackStatus ();
	PushB (ICPU.Registers.PL);
	OpenBus = ICPU.Registers.PL;
	ClearDecimal ();
	SetIRQ ();

	ICPU.Registers.PB = 0;
	ICPU.ShiftedPB = 0;
#ifdef SA1_OPCODES
	S9xSA1SetPCBase (Memory.FillRAM [0x2205] |
			 (Memory.FillRAM [0x2206] << 8));
#else
	if (Settings.SA1 && (Memory.FillRAM [0x2209] & 0x20))
	    S9xSetPCBase (Memory.FillRAM [0x220c] |
			  (Memory.FillRAM [0x220d] << 8));
	else
	    S9xSetPCBase (S9xGetWord (0xFFFA));
	CPU.Cycles += ONE_CYCLE;
#endif
    }
}
/**********************************************************************************************/

/* COP *************************************************************************************** */
static void Op02 (void)
{
#ifdef DEBUGGER
    if (CPU.Flags & TRACE_FLAG)
	S9xTraceMessage ("*** COP");
#endif	
    if (!CheckEmulation())
    {
	PushB (ICPU.Registers.PB);
	PushW (CPU.PC - CPU.PCBase + 1);
	S9xPackStatus ();
	PushB (ICPU.Registers.PL);
	OpenBus = ICPU.Registers.PL;
	ClearDecimal ();
	SetIRQ ();

	ICPU.Registers.PB = 0;
	ICPU.ShiftedPB = 0;
	S9xSetPCBase (S9xGetWord (0xFFE4));
#ifndef SA1_OPCODES
        CPU.Cycles += TWO_CYCLES;
#endif
    }
    else
    {
	PushW (CPU.PC - CPU.PCBase);
	S9xPackStatus ();
	PushB (ICPU.Registers.PL);
	OpenBus = ICPU.Registers.PL;
	ClearDecimal ();
	SetIRQ ();

	ICPU.Registers.PB = 0;
	ICPU.ShiftedPB = 0;
	S9xSetPCBase (S9xGetWord (0xFFF4));
#ifndef SA1_OPCODES
	CPU.Cycles += ONE_CYCLE;
#endif
    }
}
/**********************************************************************************************/

/* JML *************************************************************************************** */
static void OpDC (void)
{
    AbsoluteIndirectLong (JUMP, OpAddressPassthrough);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + TWO_CYCLES;
#endif
    ICPU.Registers.PB = (uint8) (OpAddress >> 16);
    ICPU.ShiftedPB = OpAddress & 0xff0000;
    S9xSetPCBase (OpAddress);
}

static void Op5C (void)
{
    AbsoluteLong (JUMP, OpAddressPassthrough);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + CPU.MemSpeed;
#endif
    ICPU.Registers.PB = (uint8) (OpAddress >> 16);
    ICPU.ShiftedPB = OpAddress & 0xff0000;
    S9xSetPCBase (OpAddress);
}
/**********************************************************************************************/

/* JMP *************************************************************************************** */
static void Op4C (void)
{
    Absolute (JUMP, OpAddressPassthrough);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
    S9xSetPCBase (ICPU.ShiftedPB + (OpAddress & 0xffff));
#if defined(CPU_SHUTDOWN) && defined(SA1_OPCODES)
    CPUShutdown ();
#endif
}

static void Op6C (void)
{
    AbsoluteIndirect (JUMP, OpAddressPassthrough);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
    S9xSetPCBase (ICPU.ShiftedPB + (OpAddress & 0xffff));
}

static void Op7C (void)
{
    AbsoluteIndexedIndirect (JUMP, OpAddressPassthrough);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + ONE_CYCLE;
#endif
    S9xSetPCBase (ICPU.ShiftedPB + OpAddress);
}
/**********************************************************************************************/

/* JSL/RTL *********************************************************************************** */
static void Op22E1 (void)
{
    AbsoluteLong (JUMP, OpAddressPassthrough);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + CPU.MemSpeed;
#endif
    PushB (ICPU.Registers.PB);
    PushWENew (CPU.PC - CPU.PCBase - 1);
    ICPU.Registers.PB = (uint8) (OpAddress >> 16);
    ICPU.ShiftedPB = OpAddress & 0xff0000;
    S9xSetPCBase (OpAddress);
}

static void Op22 (void)
{
    AbsoluteLong (JUMP, OpAddressPassthrough);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + CPU.MemSpeed;
#endif
    PushB (ICPU.Registers.PB);
    PushW (CPU.PC - CPU.PCBase - 1);
    ICPU.Registers.PB = (uint8) (OpAddress >> 16);
    ICPU.ShiftedPB = OpAddress & 0xff0000;
    S9xSetPCBase (OpAddress);
}

static void Op6BE1 (void)
{
    PullWENew (ICPU.Registers.PC);
    PullB (ICPU.Registers.PB);
    ICPU.ShiftedPB = ICPU.Registers.PB << 16;
    S9xSetPCBase (ICPU.ShiftedPB + ((ICPU.Registers.PC + 1) & 0xffff));
#ifndef SA1_OPCODES
    CPU.Cycles += TWO_CYCLES;
#endif
}

static void Op6B (void)
{
    PullW (ICPU.Registers.PC);
    PullB (ICPU.Registers.PB);
    ICPU.ShiftedPB = ICPU.Registers.PB << 16;
    S9xSetPCBase (ICPU.ShiftedPB + ((ICPU.Registers.PC + 1) & 0xffff));
#ifndef SA1_OPCODES
    CPU.Cycles += TWO_CYCLES;
#endif
}
/**********************************************************************************************/

/* JSR/RTS *********************************************************************************** */
static void Op20 (void)
{
    Absolute (JUMP, OpAddressPassthrough);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + ONE_CYCLE;
#endif
    PushW (CPU.PC - CPU.PCBase - 1);
    S9xSetPCBase (ICPU.ShiftedPB + (OpAddress & 0xffff));
}

//JSR a,x
static void OpFCE1 (void)
{
    AbsoluteIndexedIndirect (JUMP, OpAddressPassthrough);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + ONE_CYCLE;
#endif
    PushWENew (CPU.PC - CPU.PCBase - 1);
    S9xSetPCBase (ICPU.ShiftedPB + OpAddress);
}

static void OpFC (void)
{
    AbsoluteIndexedIndirect (JUMP, OpAddressPassthrough);
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + ONE_CYCLE;
#endif
    PushW (CPU.PC - CPU.PCBase - 1);
    S9xSetPCBase (ICPU.ShiftedPB + OpAddress);
}

static void Op60 (void)
{
    PullW (ICPU.Registers.PC);
    S9xSetPCBase (ICPU.ShiftedPB + ((ICPU.Registers.PC + 1) & 0xffff));
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE * 3;
#endif
}

/**********************************************************************************************/

/* MVN/MVP *********************************************************************************** */
static void Op54X1 (void)
{
    uint32 SrcBank;

#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + TWO_CYCLES;
#endif
    
    ICPU.Registers.DB = *CPU.PC++;
    ICPU.ShiftedDB = ICPU.Registers.DB << 16;
    OpenBus = SrcBank = *CPU.PC++;

    S9xSetByte (S9xGetByte ((SrcBank << 16) + ICPU.Registers.X.W), 
	     ICPU.ShiftedDB + ICPU.Registers.Y.W);

    ICPU.Registers.XL++;
    ICPU.Registers.YL++;
    ICPU.Registers.A.W--;
    if (ICPU.Registers.A.W != 0xffff)
	CPU.PC -= 3;
}

static void Op54X0 (void)
{
    uint32 SrcBank;

#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + TWO_CYCLES;
#endif
    
    ICPU.Registers.DB = *CPU.PC++;
    ICPU.ShiftedDB = ICPU.Registers.DB << 16;
    OpenBus = SrcBank = *CPU.PC++;

    S9xSetByte (S9xGetByte ((SrcBank << 16) + ICPU.Registers.X.W), 
	     ICPU.ShiftedDB + ICPU.Registers.Y.W);

    ICPU.Registers.X.W++;
    ICPU.Registers.Y.W++;
    ICPU.Registers.A.W--;
    if (ICPU.Registers.A.W != 0xffff)
	CPU.PC -= 3;
}

static void Op44X1 (void)
{
    uint32 SrcBank;

#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + TWO_CYCLES;
#endif    
    ICPU.Registers.DB = *CPU.PC++;
    ICPU.ShiftedDB = ICPU.Registers.DB << 16;
    OpenBus = SrcBank = *CPU.PC++;
    S9xSetByte (S9xGetByte ((SrcBank << 16) + ICPU.Registers.X.W), 
	     ICPU.ShiftedDB + ICPU.Registers.Y.W);

    ICPU.Registers.XL--;
    ICPU.Registers.YL--;
    ICPU.Registers.A.W--;
    if (ICPU.Registers.A.W != 0xffff)
	CPU.PC -= 3;
}

static void Op44X0 (void)
{
    uint32 SrcBank;

#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + TWO_CYCLES;
#endif    
    ICPU.Registers.DB = *CPU.PC++;
    ICPU.ShiftedDB = ICPU.Registers.DB << 16;
    OpenBus = SrcBank = *CPU.PC++;
    S9xSetByte (S9xGetByte ((SrcBank << 16) + ICPU.Registers.X.W), 
	     ICPU.ShiftedDB + ICPU.Registers.Y.W);

    ICPU.Registers.X.W--;
    ICPU.Registers.Y.W--;
    ICPU.Registers.A.W--;
    if (ICPU.Registers.A.W != 0xffff)
	CPU.PC -= 3;
}

/**********************************************************************************************/

/* REP/SEP *********************************************************************************** */
static void OpC2 (void)
{
    uint8 Work8 = ~*CPU.PC++;
    ICPU.Registers.PL &= Work8;
    ICPU._Carry &= Work8;
    ICPU._Overflow &= (Work8 >> 6);
    ICPU._Negative &= Work8;
    ICPU._Zero |= ~Work8 & Zero;

#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
#endif
    if (CheckEmulation())
    {
	SetFlags (MemoryFlag | IndexFlag);
	missing.emulate6502 = 1;
    }
    if (CheckIndex ())
    {
	ICPU.Registers.XH = 0;
	ICPU.Registers.YH = 0;
    }
    S9xFixCycles();
/*    CHECK_FOR_IRQ(); */
}

static void OpE2 (void)
{
    uint8 Work8 = *CPU.PC++;
    ICPU.Registers.PL |= Work8;
    ICPU._Carry |= Work8 & 1;
    ICPU._Overflow |= (Work8 >> 6) & 1;
    ICPU._Negative |= Work8;
    if (Work8 & Zero)
	ICPU._Zero = 0;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
#endif
    if (CheckEmulation())
    {
	SetFlags (MemoryFlag | IndexFlag);
	missing.emulate6502 = 1;
    }
    if (CheckIndex ())
    {
	ICPU.Registers.XH = 0;
	ICPU.Registers.YH = 0;
    }
    S9xFixCycles();
}
/**********************************************************************************************/

/* XBA *************************************************************************************** */
static void OpEB (void)
{
    uint8 Work8 = ICPU.Registers.AL;
    ICPU.Registers.AL = ICPU.Registers.AH;
    ICPU.Registers.AH = Work8;

    SetZN8 (ICPU.Registers.AL);
#ifndef SA1_OPCODES
    CPU.Cycles += TWO_CYCLES;
#endif
}
/**********************************************************************************************/

/* RTI *************************************************************************************** */
static void Op40 (void)
{
    PullB (ICPU.Registers.PL);
    S9xUnpackStatus ();
    PullW (ICPU.Registers.PC);
    if (!CheckEmulation())
    {
	PullB (ICPU.Registers.PB);
	ICPU.ShiftedPB = ICPU.Registers.PB << 16;
    }
    else
    {
	SetFlags (MemoryFlag | IndexFlag);
	missing.emulate6502 = 1;
    }
    S9xSetPCBase (ICPU.ShiftedPB + ICPU.Registers.PC);
    if (CheckIndex ())
    {
	ICPU.Registers.XH = 0;
	ICPU.Registers.YH = 0;
    }
#ifndef SA1_OPCODES
    CPU.Cycles += TWO_CYCLES;
#endif
    S9xFixCycles();
/*    CHECK_FOR_IRQ(); */
}

/**********************************************************************************************/

/* STP/WAI/DB ******************************************************************************** */
// WAI
static void OpCB (void)
{

// Ok, let's just C-ify the ASM versions separately.
#ifdef SA1_OPCODES
    SA1.WaitingForInterrupt = TRUE;
    SA1.PC--;
#if 0
// XXX: FIXME
    if(Settings.Shutdown){
        SA1.Cycles = SA1.NextEvent;
        if (IAPU.APUExecuting)
        {
            SA1.Executing = FALSE;
            do
            {
                APU_EXECUTE1 (,         } while (APU.Cycles < SA1.NextEvent,         SA1.Executing = TRUE;
        }
    }
#endif
#else // SA1_OPCODES
#if 0


    if (CPU.IRQActive)
    {
#ifndef SA1_OPCODES
	CPU.Cycles += TWO_CYCLES;
#endif
    }
    else
#endif
    {
	CPU.WaitingForInterrupt = TRUE;
	CPU.PC--;
#ifdef CPU_SHUTDOWN
	if (Settings.Shutdown)
	{
	    CPU.Cycles = CPU.NextEvent;
	    if (IAPU.APUExecuting)
	    {
		ICPU.CPUExecuting = FALSE;
		do
		{
		    APU_EXECUTE1 ();
		} while (APU.Cycles < CPU.NextEvent);
		ICPU.CPUExecuting = TRUE;
	    }
	}
	else
        {
#ifndef SA1_OPCODES
            CPU.Cycles += TWO_CYCLES;
#endif
    }
#endif
    }
#endif // SA1_OPCODES
}

// STP
static void OpDB (void)
{
    CPU.PC--;
    CPU.Flags |= DEBUG_MODE_FLAG;
}

// Reserved S9xOpcode
static void Op42 (void)
{
}

/*****************************************************************************/

/*****************************************************************************/
/* CPU-S9xOpcodes Definitions                                                                    */
/*****************************************************************************/
struct SOpcodes S9xOpcodesM1X1[256] =
{
    {Op00},	 {Op01M1},    {Op02},      {Op03M1},    {Op04M1},
    {Op05M1},    {Op06M1},    {Op07M1},    {Op08},      {Op09M1},
    {Op0AM1},    {Op0B},      {Op0CM1},    {Op0DM1},    {Op0EM1},
    {Op0FM1},    {Op10},      {Op11M1},    {Op12M1},    {Op13M1},
    {Op14M1},    {Op15M1},    {Op16M1},    {Op17M1},    {Op18},
    {Op19M1},    {Op1AM1},    {Op1B},      {Op1CM1},    {Op1DM1},
    {Op1EM1},    {Op1FM1},    {Op20},      {Op21M1},    {Op22},
    {Op23M1},    {Op24M1},    {Op25M1},    {Op26M1},    {Op27M1},
    {Op28},      {Op29M1},    {Op2AM1},    {Op2B},      {Op2CM1},
    {Op2DM1},    {Op2EM1},    {Op2FM1},    {Op30},      {Op31M1},
    {Op32M1},    {Op33M1},    {Op34M1},    {Op35M1},    {Op36M1},
    {Op37M1},    {Op38},      {Op39M1},    {Op3AM1},    {Op3B},
    {Op3CM1},    {Op3DM1},    {Op3EM1},    {Op3FM1},    {Op40},
    {Op41M1},    {Op42},      {Op43M1},    {Op44X1},    {Op45M1},
    {Op46M1},    {Op47M1},    {Op48M1},    {Op49M1},    {Op4AM1},
    {Op4B},      {Op4C},      {Op4DM1},    {Op4EM1},    {Op4FM1},
    {Op50},      {Op51M1},    {Op52M1},    {Op53M1},    {Op54X1},
    {Op55M1},    {Op56M1},    {Op57M1},    {Op58},      {Op59M1},
    {Op5AX1},    {Op5B},      {Op5C},      {Op5DM1},    {Op5EM1},
    {Op5FM1},    {Op60},      {Op61M1},    {Op62},      {Op63M1},
    {Op64M1},    {Op65M1},    {Op66M1},    {Op67M1},    {Op68M1},
    {Op69M1},    {Op6AM1},    {Op6B},      {Op6C},      {Op6DM1},
    {Op6EM1},    {Op6FM1},    {Op70},      {Op71M1},    {Op72M1},
    {Op73M1},    {Op74M1},    {Op75M1},    {Op76M1},    {Op77M1},
    {Op78},      {Op79M1},    {Op7AX1},    {Op7B},      {Op7C},
    {Op7DM1},    {Op7EM1},    {Op7FM1},    {Op80},      {Op81M1},
    {Op82},      {Op83M1},    {Op84X1},    {Op85M1},    {Op86X1},
    {Op87M1},    {Op88X1},    {Op89M1},    {Op8AM1},    {Op8B},
    {Op8CX1},    {Op8DM1},    {Op8EX1},    {Op8FM1},    {Op90},
    {Op91M1},    {Op92M1},    {Op93M1},    {Op94X1},    {Op95M1},
    {Op96X1},    {Op97M1},    {Op98M1},    {Op99M1},    {Op9A},
    {Op9BX1},    {Op9CM1},    {Op9DM1},    {Op9EM1},    {Op9FM1},
    {OpA0X1},    {OpA1M1},    {OpA2X1},    {OpA3M1},    {OpA4X1},
    {OpA5M1},    {OpA6X1},    {OpA7M1},    {OpA8X1},    {OpA9M1},
    {OpAAX1},    {OpAB},      {OpACX1},    {OpADM1},    {OpAEX1},
    {OpAFM1},    {OpB0},      {OpB1M1},    {OpB2M1},    {OpB3M1},
    {OpB4X1},    {OpB5M1},    {OpB6X1},    {OpB7M1},    {OpB8},
    {OpB9M1},    {OpBAX1},    {OpBBX1},    {OpBCX1},    {OpBDM1},
    {OpBEX1},    {OpBFM1},    {OpC0X1},    {OpC1M1},    {OpC2},
    {OpC3M1},    {OpC4X1},    {OpC5M1},    {OpC6M1},    {OpC7M1},
    {OpC8X1},    {OpC9M1},    {OpCAX1},    {OpCB},      {OpCCX1},
    {OpCDM1},    {OpCEM1},    {OpCFM1},    {OpD0},      {OpD1M1},
    {OpD2M1},    {OpD3M1},    {OpD4},      {OpD5M1},    {OpD6M1},
    {OpD7M1},    {OpD8},      {OpD9M1},    {OpDAX1},    {OpDB},
    {OpDC},      {OpDDM1},    {OpDEM1},    {OpDFM1},    {OpE0X1},
    {OpE1M1},    {OpE2},      {OpE3M1},    {OpE4X1},    {OpE5M1},
    {OpE6M1},    {OpE7M1},    {OpE8X1},    {OpE9M1},    {OpEA},
    {OpEB},      {OpECX1},    {OpEDM1},    {OpEEM1},    {OpEFM1},
    {OpF0},      {OpF1M1},    {OpF2M1},    {OpF3M1},    {OpF4},
    {OpF5M1},    {OpF6M1},    {OpF7M1},    {OpF8},      {OpF9M1},
    {OpFAX1},    {OpFB},      {OpFC},      {OpFDM1},    {OpFEM1},
    {OpFFM1}
};

struct SOpcodes S9xOpcodesE1[256] =
{
    {Op00},	 {Op01M1},    {Op02},      {Op03M1},    {Op04M1},
    {Op05M1},    {Op06M1},    {Op07M1},    {Op08E1},      {Op09M1},
    {Op0AM1},    {Op0BE1},      {Op0CM1},    {Op0DM1},    {Op0EM1},
    {Op0FM1},    {Op10},      {Op11M1},    {Op12M1},    {Op13M1},
    {Op14M1},    {Op15M1},    {Op16M1},    {Op17M1},    {Op18},
    {Op19M1},    {Op1AM1},    {Op1B},      {Op1CM1},    {Op1DM1},
    {Op1EM1},    {Op1FM1},    {Op20},      {Op21M1},    {Op22E1},
    {Op23M1},    {Op24M1},    {Op25M1},    {Op26M1},    {Op27M1},
    {Op28},      {Op29M1},    {Op2AM1},    {Op2BE1},      {Op2CM1},
    {Op2DM1},    {Op2EM1},    {Op2FM1},    {Op30},      {Op31M1},
    {Op32M1},    {Op33M1},    {Op34M1},    {Op35M1},    {Op36M1},
    {Op37M1},    {Op38},      {Op39M1},    {Op3AM1},    {Op3B},
    {Op3CM1},    {Op3DM1},    {Op3EM1},    {Op3FM1},    {Op40},
    {Op41M1},    {Op42},      {Op43M1},    {Op44X1},    {Op45M1},
    {Op46M1},    {Op47M1},    {Op48E1},    {Op49M1},    {Op4AM1},
    {Op4BE1},      {Op4C},      {Op4DM1},    {Op4EM1},    {Op4FM1},
    {Op50},      {Op51M1},    {Op52M1},    {Op53M1},    {Op54X1},
    {Op55M1},    {Op56M1},    {Op57M1},    {Op58},      {Op59M1},
    {Op5AE1},    {Op5B},      {Op5C},      {Op5DM1},    {Op5EM1},
    {Op5FM1},    {Op60},      {Op61M1},    {Op62E1},      {Op63M1},
    {Op64M1},    {Op65M1},    {Op66M1},    {Op67M1},    {Op68E1},
    {Op69M1},    {Op6AM1},    {Op6BE1},      {Op6C},      {Op6DM1},
    {Op6EM1},    {Op6FM1},    {Op70},      {Op71M1},    {Op72M1},
    {Op73M1},    {Op74M1},    {Op75M1},    {Op76M1},    {Op77M1},
    {Op78},      {Op79M1},    {Op7AE1},    {Op7B},      {Op7C},
    {Op7DM1},    {Op7EM1},    {Op7FM1},    {Op80},      {Op81M1},
    {Op82},      {Op83M1},    {Op84X1},    {Op85M1},    {Op86X1},
    {Op87M1},    {Op88X1},    {Op89M1},    {Op8AM1},    {Op8BE1},
    {Op8CX1},    {Op8DM1},    {Op8EX1},    {Op8FM1},    {Op90},
    {Op91M1},    {Op92M1},    {Op93M1},    {Op94X1},    {Op95M1},
    {Op96X1},    {Op97M1},    {Op98M1},    {Op99M1},    {Op9A},
    {Op9BX1},    {Op9CM1},    {Op9DM1},    {Op9EM1},    {Op9FM1},
    {OpA0X1},    {OpA1M1},    {OpA2X1},    {OpA3M1},    {OpA4X1},
    {OpA5M1},    {OpA6X1},    {OpA7M1},    {OpA8X1},    {OpA9M1},
    {OpAAX1},    {OpABE1},      {OpACX1},    {OpADM1},    {OpAEX1},
    {OpAFM1},    {OpB0},      {OpB1M1},    {OpB2M1},    {OpB3M1},
    {OpB4X1},    {OpB5M1},    {OpB6X1},    {OpB7M1},    {OpB8},
    {OpB9M1},    {OpBAX1},    {OpBBX1},    {OpBCX1},    {OpBDM1},
    {OpBEX1},    {OpBFM1},    {OpC0X1},    {OpC1M1},    {OpC2},
    {OpC3M1},    {OpC4X1},    {OpC5M1},    {OpC6M1},    {OpC7M1},
    {OpC8X1},    {OpC9M1},    {OpCAX1},    {OpCB},      {OpCCX1},
    {OpCDM1},    {OpCEM1},    {OpCFM1},    {OpD0},      {OpD1M1},
    {OpD2M1},    {OpD3M1},    {OpD4E1},      {OpD5M1},    {OpD6M1},
    {OpD7M1},    {OpD8},      {OpD9M1},    {OpDAE1},    {OpDB},
    {OpDC},      {OpDDM1},    {OpDEM1},    {OpDFM1},    {OpE0X1},
    {OpE1M1},    {OpE2},      {OpE3M1},    {OpE4X1},    {OpE5M1},
    {OpE6M1},    {OpE7M1},    {OpE8X1},    {OpE9M1},    {OpEA},
    {OpEB},      {OpECX1},    {OpEDM1},    {OpEEM1},    {OpEFM1},
    {OpF0},      {OpF1M1},    {OpF2M1},    {OpF3M1},    {OpF4E1},
    {OpF5M1},    {OpF6M1},    {OpF7M1},    {OpF8},      {OpF9M1},
    {OpFAE1},    {OpFB},      {OpFCE1},      {OpFDM1},    {OpFEM1},
    {OpFFM1}
};

struct SOpcodes S9xOpcodesM1X0[256] =
{
    {Op00},	 {Op01M1},    {Op02},      {Op03M1},    {Op04M1},
    {Op05M1},    {Op06M1},    {Op07M1},    {Op08},      {Op09M1},
    {Op0AM1},    {Op0B},      {Op0CM1},    {Op0DM1},    {Op0EM1},
    {Op0FM1},    {Op10},      {Op11M1},    {Op12M1},    {Op13M1},
    {Op14M1},    {Op15M1},    {Op16M1},    {Op17M1},    {Op18},
    {Op19M1},    {Op1AM1},    {Op1B},      {Op1CM1},    {Op1DM1},
    {Op1EM1},    {Op1FM1},    {Op20},      {Op21M1},    {Op22},
    {Op23M1},    {Op24M1},    {Op25M1},    {Op26M1},    {Op27M1},
    {Op28},      {Op29M1},    {Op2AM1},    {Op2B},      {Op2CM1},
    {Op2DM1},    {Op2EM1},    {Op2FM1},    {Op30},      {Op31M1},
    {Op32M1},    {Op33M1},    {Op34M1},    {Op35M1},    {Op36M1},
    {Op37M1},    {Op38},      {Op39M1},    {Op3AM1},    {Op3B},
    {Op3CM1},    {Op3DM1},    {Op3EM1},    {Op3FM1},    {Op40},
    {Op41M1},    {Op42},      {Op43M1},    {Op44X0},    {Op45M1},
    {Op46M1},    {Op47M1},    {Op48M1},    {Op49M1},    {Op4AM1},
    {Op4B},      {Op4C},      {Op4DM1},    {Op4EM1},    {Op4FM1},
    {Op50},      {Op51M1},    {Op52M1},    {Op53M1},    {Op54X0},
    {Op55M1},    {Op56M1},    {Op57M1},    {Op58},      {Op59M1},
    {Op5AX0},    {Op5B},      {Op5C},      {Op5DM1},    {Op5EM1},
    {Op5FM1},    {Op60},      {Op61M1},    {Op62},      {Op63M1},
    {Op64M1},    {Op65M1},    {Op66M1},    {Op67M1},    {Op68M1},
    {Op69M1},    {Op6AM1},    {Op6B},      {Op6C},      {Op6DM1},
    {Op6EM1},    {Op6FM1},    {Op70},      {Op71M1},    {Op72M1},
    {Op73M1},    {Op74M1},    {Op75M1},    {Op76M1},    {Op77M1},
    {Op78},      {Op79M1},    {Op7AX0},    {Op7B},      {Op7C},
    {Op7DM1},    {Op7EM1},    {Op7FM1},    {Op80},      {Op81M1},
    {Op82},      {Op83M1},    {Op84X0},    {Op85M1},    {Op86X0},
    {Op87M1},    {Op88X0},    {Op89M1},    {Op8AM1},    {Op8B},
    {Op8CX0},    {Op8DM1},    {Op8EX0},    {Op8FM1},    {Op90},
    {Op91M1},    {Op92M1},    {Op93M1},    {Op94X0},    {Op95M1},
    {Op96X0},    {Op97M1},    {Op98M1},    {Op99M1},    {Op9A},
    {Op9BX0},    {Op9CM1},    {Op9DM1},    {Op9EM1},    {Op9FM1},
    {OpA0X0},    {OpA1M1},    {OpA2X0},    {OpA3M1},    {OpA4X0},
    {OpA5M1},    {OpA6X0},    {OpA7M1},    {OpA8X0},    {OpA9M1},
    {OpAAX0},    {OpAB},      {OpACX0},    {OpADM1},    {OpAEX0},
    {OpAFM1},    {OpB0},      {OpB1M1},    {OpB2M1},    {OpB3M1},
    {OpB4X0},    {OpB5M1},    {OpB6X0},    {OpB7M1},    {OpB8},
    {OpB9M1},    {OpBAX0},    {OpBBX0},    {OpBCX0},    {OpBDM1},
    {OpBEX0},    {OpBFM1},    {OpC0X0},    {OpC1M1},    {OpC2},
    {OpC3M1},    {OpC4X0},    {OpC5M1},    {OpC6M1},    {OpC7M1},
    {OpC8X0},    {OpC9M1},    {OpCAX0},    {OpCB},      {OpCCX0},
    {OpCDM1},    {OpCEM1},    {OpCFM1},    {OpD0},      {OpD1M1},
    {OpD2M1},    {OpD3M1},    {OpD4},      {OpD5M1},    {OpD6M1},
    {OpD7M1},    {OpD8},      {OpD9M1},    {OpDAX0},    {OpDB},
    {OpDC},      {OpDDM1},    {OpDEM1},    {OpDFM1},    {OpE0X0},
    {OpE1M1},    {OpE2},      {OpE3M1},    {OpE4X0},    {OpE5M1},
    {OpE6M1},    {OpE7M1},    {OpE8X0},    {OpE9M1},    {OpEA},
    {OpEB},      {OpECX0},    {OpEDM1},    {OpEEM1},    {OpEFM1},
    {OpF0},      {OpF1M1},    {OpF2M1},    {OpF3M1},    {OpF4},
    {OpF5M1},    {OpF6M1},    {OpF7M1},    {OpF8},      {OpF9M1},
    {OpFAX0},    {OpFB},      {OpFC},      {OpFDM1},    {OpFEM1},
    {OpFFM1}
};

struct SOpcodes S9xOpcodesM0X0[256] =
{
    {Op00},	 {Op01M0},    {Op02},      {Op03M0},    {Op04M0},
    {Op05M0},    {Op06M0},    {Op07M0},    {Op08},      {Op09M0},
    {Op0AM0},    {Op0B},      {Op0CM0},    {Op0DM0},    {Op0EM0},
    {Op0FM0},    {Op10},      {Op11M0},    {Op12M0},    {Op13M0},
    {Op14M0},    {Op15M0},    {Op16M0},    {Op17M0},    {Op18},
    {Op19M0},    {Op1AM0},    {Op1B},      {Op1CM0},    {Op1DM0},
    {Op1EM0},    {Op1FM0},    {Op20},      {Op21M0},    {Op22},
    {Op23M0},    {Op24M0},    {Op25M0},    {Op26M0},    {Op27M0},
    {Op28},      {Op29M0},    {Op2AM0},    {Op2B},      {Op2CM0},
    {Op2DM0},    {Op2EM0},    {Op2FM0},    {Op30},      {Op31M0},
    {Op32M0},    {Op33M0},    {Op34M0},    {Op35M0},    {Op36M0},
    {Op37M0},    {Op38},      {Op39M0},    {Op3AM0},    {Op3B},
    {Op3CM0},    {Op3DM0},    {Op3EM0},    {Op3FM0},    {Op40},
    {Op41M0},    {Op42},      {Op43M0},    {Op44X0},    {Op45M0},
    {Op46M0},    {Op47M0},    {Op48M0},    {Op49M0},    {Op4AM0},
    {Op4B},      {Op4C},      {Op4DM0},    {Op4EM0},    {Op4FM0},
    {Op50},      {Op51M0},    {Op52M0},    {Op53M0},    {Op54X0},
    {Op55M0},    {Op56M0},    {Op57M0},    {Op58},      {Op59M0},
    {Op5AX0},    {Op5B},      {Op5C},      {Op5DM0},    {Op5EM0},
    {Op5FM0},    {Op60},      {Op61M0},    {Op62},      {Op63M0},
    {Op64M0},    {Op65M0},    {Op66M0},    {Op67M0},    {Op68M0},
    {Op69M0},    {Op6AM0},    {Op6B},      {Op6C},      {Op6DM0},
    {Op6EM0},    {Op6FM0},    {Op70},      {Op71M0},    {Op72M0},
    {Op73M0},    {Op74M0},    {Op75M0},    {Op76M0},    {Op77M0},
    {Op78},      {Op79M0},    {Op7AX0},    {Op7B},      {Op7C},
    {Op7DM0},    {Op7EM0},    {Op7FM0},    {Op80},      {Op81M0},
    {Op82},      {Op83M0},    {Op84X0},    {Op85M0},    {Op86X0},
    {Op87M0},    {Op88X0},    {Op89M0},    {Op8AM0},    {Op8B},
    {Op8CX0},    {Op8DM0},    {Op8EX0},    {Op8FM0},    {Op90},
    {Op91M0},    {Op92M0},    {Op93M0},    {Op94X0},    {Op95M0},
    {Op96X0},    {Op97M0},    {Op98M0},    {Op99M0},    {Op9A},
    {Op9BX0},    {Op9CM0},    {Op9DM0},    {Op9EM0},    {Op9FM0},
    {OpA0X0},    {OpA1M0},    {OpA2X0},    {OpA3M0},    {OpA4X0},
    {OpA5M0},    {OpA6X0},    {OpA7M0},    {OpA8X0},    {OpA9M0},
    {OpAAX0},    {OpAB},      {OpACX0},    {OpADM0},    {OpAEX0},
    {OpAFM0},    {OpB0},      {OpB1M0},    {OpB2M0},    {OpB3M0},
    {OpB4X0},    {OpB5M0},    {OpB6X0},    {OpB7M0},    {OpB8},
    {OpB9M0},    {OpBAX0},    {OpBBX0},    {OpBCX0},    {OpBDM0},
    {OpBEX0},    {OpBFM0},    {OpC0X0},    {OpC1M0},    {OpC2},
    {OpC3M0},    {OpC4X0},    {OpC5M0},    {OpC6M0},    {OpC7M0},
    {OpC8X0},    {OpC9M0},    {OpCAX0},    {OpCB},      {OpCCX0},
    {OpCDM0},    {OpCEM0},    {OpCFM0},    {OpD0},      {OpD1M0},
    {OpD2M0},    {OpD3M0},    {OpD4},      {OpD5M0},    {OpD6M0},
    {OpD7M0},    {OpD8},      {OpD9M0},    {OpDAX0},    {OpDB},
    {OpDC},      {OpDDM0},    {OpDEM0},    {OpDFM0},    {OpE0X0},
    {OpE1M0},    {OpE2},      {OpE3M0},    {OpE4X0},    {OpE5M0},
    {OpE6M0},    {OpE7M0},    {OpE8X0},    {OpE9M0},    {OpEA},
    {OpEB},      {OpECX0},    {OpEDM0},    {OpEEM0},    {OpEFM0},
    {OpF0},      {OpF1M0},    {OpF2M0},    {OpF3M0},    {OpF4},
    {OpF5M0},    {OpF6M0},    {OpF7M0},    {OpF8},      {OpF9M0},
    {OpFAX0},    {OpFB},      {OpFC},      {OpFDM0},    {OpFEM0},
    {OpFFM0}
};

struct SOpcodes S9xOpcodesM0X1[256] =
{
    {Op00},	 {Op01M0},    {Op02},      {Op03M0},    {Op04M0},
    {Op05M0},    {Op06M0},    {Op07M0},    {Op08},      {Op09M0},
    {Op0AM0},    {Op0B},      {Op0CM0},    {Op0DM0},    {Op0EM0},
    {Op0FM0},    {Op10},      {Op11M0},    {Op12M0},    {Op13M0},
    {Op14M0},    {Op15M0},    {Op16M0},    {Op17M0},    {Op18},
    {Op19M0},    {Op1AM0},    {Op1B},      {Op1CM0},    {Op1DM0},
    {Op1EM0},    {Op1FM0},    {Op20},      {Op21M0},    {Op22},
    {Op23M0},    {Op24M0},    {Op25M0},    {Op26M0},    {Op27M0},
    {Op28},      {Op29M0},    {Op2AM0},    {Op2B},      {Op2CM0},
    {Op2DM0},    {Op2EM0},    {Op2FM0},    {Op30},      {Op31M0},
    {Op32M0},    {Op33M0},    {Op34M0},    {Op35M0},    {Op36M0},
    {Op37M0},    {Op38},      {Op39M0},    {Op3AM0},    {Op3B},
    {Op3CM0},    {Op3DM0},    {Op3EM0},    {Op3FM0},    {Op40},
    {Op41M0},    {Op42},      {Op43M0},    {Op44X1},    {Op45M0},
    {Op46M0},    {Op47M0},    {Op48M0},    {Op49M0},    {Op4AM0},
    {Op4B},      {Op4C},      {Op4DM0},    {Op4EM0},    {Op4FM0},
    {Op50},      {Op51M0},    {Op52M0},    {Op53M0},    {Op54X1},
    {Op55M0},    {Op56M0},    {Op57M0},    {Op58},      {Op59M0},
    {Op5AX1},    {Op5B},      {Op5C},      {Op5DM0},    {Op5EM0},
    {Op5FM0},    {Op60},      {Op61M0},    {Op62},      {Op63M0},
    {Op64M0},    {Op65M0},    {Op66M0},    {Op67M0},    {Op68M0},
    {Op69M0},    {Op6AM0},    {Op6B},      {Op6C},      {Op6DM0},
    {Op6EM0},    {Op6FM0},    {Op70},      {Op71M0},    {Op72M0},
    {Op73M0},    {Op74M0},    {Op75M0},    {Op76M0},    {Op77M0},
    {Op78},      {Op79M0},    {Op7AX1},    {Op7B},      {Op7C},
    {Op7DM0},    {Op7EM0},    {Op7FM0},    {Op80},      {Op81M0},
    {Op82},      {Op83M0},    {Op84X1},    {Op85M0},    {Op86X1},
    {Op87M0},    {Op88X1},    {Op89M0},    {Op8AM0},    {Op8B},
    {Op8CX1},    {Op8DM0},    {Op8EX1},    {Op8FM0},    {Op90},
    {Op91M0},    {Op92M0},    {Op93M0},    {Op94X1},    {Op95M0},
    {Op96X1},    {Op97M0},    {Op98M0},    {Op99M0},    {Op9A},
    {Op9BX1},    {Op9CM0},    {Op9DM0},    {Op9EM0},    {Op9FM0},
    {OpA0X1},    {OpA1M0},    {OpA2X1},    {OpA3M0},    {OpA4X1},
    {OpA5M0},    {OpA6X1},    {OpA7M0},    {OpA8X1},    {OpA9M0},
    {OpAAX1},    {OpAB},      {OpACX1},    {OpADM0},    {OpAEX1},
    {OpAFM0},    {OpB0},      {OpB1M0},    {OpB2M0},    {OpB3M0},
    {OpB4X1},    {OpB5M0},    {OpB6X1},    {OpB7M0},    {OpB8},
    {OpB9M0},    {OpBAX1},    {OpBBX1},    {OpBCX1},    {OpBDM0},
    {OpBEX1},    {OpBFM0},    {OpC0X1},    {OpC1M0},    {OpC2},
    {OpC3M0},    {OpC4X1},    {OpC5M0},    {OpC6M0},    {OpC7M0},
    {OpC8X1},    {OpC9M0},    {OpCAX1},    {OpCB},      {OpCCX1},
    {OpCDM0},    {OpCEM0},    {OpCFM0},    {OpD0},      {OpD1M0},
    {OpD2M0},    {OpD3M0},    {OpD4},      {OpD5M0},    {OpD6M0},
    {OpD7M0},    {OpD8},      {OpD9M0},    {OpDAX1},    {OpDB},
    {OpDC},      {OpDDM0},    {OpDEM0},    {OpDFM0},    {OpE0X1},
    {OpE1M0},    {OpE2},      {OpE3M0},    {OpE4X1},    {OpE5M0},
    {OpE6M0},    {OpE7M0},    {OpE8X1},    {OpE9M0},    {OpEA},
    {OpEB},      {OpECX1},    {OpEDM0},    {OpEEM0},    {OpEFM0},
    {OpF0},      {OpF1M0},    {OpF2M0},    {OpF3M0},    {OpF4},
    {OpF5M0},    {OpF6M0},    {OpF7M0},    {OpF8},      {OpF9M0},
    {OpFAX1},    {OpFB},      {OpFC},      {OpFDM0},    {OpFEM0},
    {OpFFM0}
};

