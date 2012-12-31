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

#ifndef _CPUADDR_H_
#define _CPUADDR_H_

typedef enum {
    NONE = 0,
    READ = 1,
    WRITE = 2,
    MODIFY = 3,
    JUMP = 4
} AccessMode;

EXTERN_C long OpAddress;

// The type for a function that can run after the addressing mode is resolved:
// void NAME (long Addr) {...}
typedef void (*InternalOp) (long);

/*
 * The addressing modes in this file do not update the OpAddress variable.
 * Rather, they pass the address they calculate to the operation that needs to
 * be done with it. If you need the calculated value, set a passthrough
 * that gets the calculated address from the internal op and then updates the
 * OpAddress variable.
 *
 * Not updating the OpAddress variable saves a few memory storage instructions
 * per SNES instruction.
 * Calling the operation at the end of the addressing mode calculation saves
 * one return instruction per SNES instruction, because the code can just
 * jump from one to the other.
 */

static void Immediate8 (AccessMode a, InternalOp op)
{
    long Addr = ICPU.ShiftedPB + CPU.PC - CPU.PCBase;
    CPU.PC++;
    (*op)(Addr);
}

static void Immediate16 (AccessMode a, InternalOp op)
{
    long Addr = ICPU.ShiftedPB + CPU.PC - CPU.PCBase;
    CPU.PC += 2;
    (*op)(Addr);
}

static void Relative (AccessMode a, InternalOp op)
{
    int8 Int8 = *CPU.PC++;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif    
    long Addr = ((int) (CPU.PC - CPU.PCBase) + Int8) & 0xffff;
    (*op)(Addr);
}

static void RelativeLong (AccessMode a, InternalOp op)
{
    long Addr;
#ifdef FAST_LSB_WORD_ACCESS
    Addr = *(uint16 *) CPU.PC;
#else
    Addr = *CPU.PC + (*(CPU.PC + 1) << 8);
#endif
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + ONE_CYCLE;
#endif
    CPU.PC += 2;
    Addr += (CPU.PC - CPU.PCBase);
    Addr &= 0xffff;
    (*op)(Addr);
}

static void AbsoluteIndexedIndirect (AccessMode a, InternalOp op)
{
    long Addr;
#ifdef FAST_LSB_WORD_ACCESS
    Addr = (ICPU.Registers.X.W + *(uint16 *) CPU.PC) & 0xffff;
#else
    Addr = (ICPU.Registers.X.W + *CPU.PC + (*(CPU.PC + 1) << 8)) & 0xffff;
#endif
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
#ifndef NO_OPEN_BUS
    OpenBus = *(CPU.PC + 1);
#endif
    CPU.PC += 2;
    Addr = S9xGetWord (ICPU.ShiftedPB + Addr);
#ifndef NO_OPEN_BUS
    if(a&READ) OpenBus = (uint8)(Addr>>8);
#endif
    (*op)(Addr);
}

static void AbsoluteIndirectLong (AccessMode a, InternalOp op)
{
    long Addr;
#ifdef FAST_LSB_WORD_ACCESS
    Addr = *(uint16 *) CPU.PC;
#else
    Addr = *CPU.PC + (*(CPU.PC + 1) << 8);
#endif

#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
#ifndef NO_OPEN_BUS
    OpenBus = *(CPU.PC + 1);
#endif
    CPU.PC += 2;
#ifndef NO_OPEN_BUS
    if(a&READ) {
	Addr = S9xGetWord (Addr) | ((OpenBus=S9xGetByte (Addr + 2)) << 16);
    } else {
#endif
    Addr = S9xGetWord (Addr) | (S9xGetByte (Addr + 2) << 16);
#ifndef NO_OPEN_BUS
    }
#endif
    (*op)(Addr);
}

static void AbsoluteIndirect (AccessMode a, InternalOp op)
{
    long Addr;
#ifdef FAST_LSB_WORD_ACCESS
    Addr = *(uint16 *) CPU.PC;
#else
    Addr = *CPU.PC + (*(CPU.PC + 1) << 8);
#endif

#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
#ifndef NO_OPEN_BUS
    OpenBus = *(CPU.PC + 1);
#endif
    CPU.PC += 2;
    Addr = S9xGetWord (Addr);
#ifndef NO_OPEN_BUS
    if(a&READ) OpenBus = (uint8)(Addr>>8);
#endif
    Addr += ICPU.ShiftedPB;
    (*op)(Addr);
}

static void Absolute (AccessMode a, InternalOp op)
{
    long Addr;
#ifdef FAST_LSB_WORD_ACCESS
    Addr = *(uint16 *) CPU.PC + ICPU.ShiftedDB;
#else
    Addr = *CPU.PC + (*(CPU.PC + 1) << 8) + ICPU.ShiftedDB;
#endif
#ifndef NO_OPEN_BUS
    if(a&READ) OpenBus = *(CPU.PC+1);
#endif
    CPU.PC += 2;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
    (*op)(Addr);
}

static void AbsoluteLong (AccessMode a, InternalOp op)
{
    long Addr;
#ifdef FAST_LSB_WORD_ACCESS
    Addr = (*(uint32 *) CPU.PC) & 0xffffff;
#elif defined FAST_ALIGNED_LSB_WORD_ACCESS
    if (((int) CPU.PC & 1) == 0)
        Addr = (*(uint16 *) CPU.PC) + (*(CPU.PC + 2) << 16);
    else
        Addr = *CPU.PC + ((*(uint16 *) (CPU.PC + 1)) << 8);
#else
    Addr = *CPU.PC + (*(CPU.PC + 1) << 8) + (*(CPU.PC + 2) << 16);
#endif
#ifndef NO_OPEN_BUS
    if(a&READ) OpenBus = *(CPU.PC+2);
#endif
    CPU.PC += 3;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + CPU.MemSpeed;
#endif
    (*op)(Addr);
}

static void Direct(AccessMode a, InternalOp op)
{
#ifndef NO_OPEN_BUS
    if(a&READ) OpenBus = *CPU.PC;
#endif
    long Addr = (*CPU.PC++ + ICPU.Registers.D.W) & 0xffff;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
//    if (ICPU.Registers.DL != 0) CPU.Cycles += ONE_CYCLE;
    (*op)(Addr);
}

static void DirectIndirectIndexed (AccessMode a, InternalOp op)
{
#ifndef NO_OPEN_BUS
    OpenBus = *CPU.PC;
#endif
    long Addr = (*CPU.PC++ + ICPU.Registers.D.W) & 0xffff;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif

    Addr = S9xGetWord (Addr);
#ifndef NO_OPEN_BUS
    if(a&READ) OpenBus = (uint8)(Addr>>8);
#endif
    Addr += ICPU.ShiftedDB + ICPU.Registers.Y.W;

//    if (ICPU.Registers.DL != 0) CPU.Cycles += ONE_CYCLE;
    // XXX: always add one if STA
    // XXX: else Add one cycle if crosses page boundary
    (*op)(Addr);
}

static void DirectIndirectIndexedLong (AccessMode a, InternalOp op)
{
#ifndef NO_OPEN_BUS
    OpenBus = *CPU.PC;
#endif
    long Addr = (*CPU.PC++ + ICPU.Registers.D.W) & 0xffff;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif

#ifndef NO_OPEN_BUS
    if(a&READ){
	Addr = S9xGetWord (Addr) + ((OpenBus = S9xGetByte (Addr + 2)) << 16) + ICPU.Registers.Y.W;
    } else {
#endif
	Addr = S9xGetWord (Addr) + (S9xGetByte (Addr + 2) << 16) + ICPU.Registers.Y.W;
#ifndef NO_OPEN_BUS
    }
#endif
//    if (ICPU.Registers.DL != 0) CPU.Cycles += ONE_CYCLE;
    (*op)(Addr);
}

static void DirectIndexedIndirect(AccessMode a, InternalOp op)
{
#ifndef NO_OPEN_BUS
    OpenBus = *CPU.PC;
#endif
    long Addr = (*CPU.PC++ + ICPU.Registers.D.W + ICPU.Registers.X.W) & 0xffff;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif

    Addr = S9xGetWord (Addr);
#ifndef NO_OPEN_BUS
    if(a&READ) OpenBus = (uint8)(Addr>>8);
#endif
    Addr += ICPU.ShiftedDB;

#ifndef SA1_OPCODES
//    if (ICPU.Registers.DL != 0)
//	CPU.Cycles += TWO_CYCLES;
//    else
	CPU.Cycles += ONE_CYCLE;
#endif
    (*op)(Addr);
}

static void DirectIndexedX (AccessMode a, InternalOp op)
{
#ifndef NO_OPEN_BUS
	if(a&READ) OpenBus = *CPU.PC;
#endif
    long Addr = (*CPU.PC++ + ICPU.Registers.D.W + ICPU.Registers.X.W);
    Addr &= CheckEmulation() ? 0xff : 0xffff;

#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
//    if (ICPU.Registers.DL != 0)
//	CPU.Cycles += TWO_CYCLES;
//    else
//	CPU.Cycles += ONE_CYCLE;
#endif
    (*op)(Addr);
}

static void DirectIndexedY (AccessMode a, InternalOp op)
{
#ifndef NO_OPEN_BUS
	if(a&READ) OpenBus = *CPU.PC;
#endif
    long Addr = (*CPU.PC++ + ICPU.Registers.D.W + ICPU.Registers.Y.W);
    Addr &= CheckEmulation() ? 0xff : 0xffff;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
//    if (ICPU.Registers.DL != 0)
//	CPU.Cycles += TWO_CYCLES;
//    else
//	CPU.Cycles += ONE_CYCLE;
#endif
    (*op)(Addr);
}

static void AbsoluteIndexedX (AccessMode a, InternalOp op)
{
    long Addr;
#ifdef FAST_LSB_WORD_ACCESS
    Addr = ICPU.ShiftedDB + *(uint16 *) CPU.PC + ICPU.Registers.X.W;
#else
    Addr = ICPU.ShiftedDB + *CPU.PC + (*(CPU.PC + 1) << 8) +
		ICPU.Registers.X.W;
#endif
#ifndef NO_OPEN_BUS
    if(a&READ) OpenBus = *(CPU.PC+1);
#endif
    CPU.PC += 2;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
    // XXX: always add one cycle for ROL, LSR, etc
    // XXX: else is cross page boundary add one cycle
    (*op)(Addr);
}

static void AbsoluteIndexedY (AccessMode a, InternalOp op)
{
    long Addr;
#ifdef FAST_LSB_WORD_ACCESS
    Addr = ICPU.ShiftedDB + *(uint16 *) CPU.PC + ICPU.Registers.Y.W;
#else
    Addr = ICPU.ShiftedDB + *CPU.PC + (*(CPU.PC + 1) << 8) +
		ICPU.Registers.Y.W;
#endif    
#ifndef NO_OPEN_BUS
    if(a&READ) OpenBus = *(CPU.PC+1);
#endif
    CPU.PC += 2;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
    // XXX: always add cycle for STA
    // XXX: else is cross page boundary add one cycle
    (*op)(Addr);
}

static void AbsoluteLongIndexedX (AccessMode a, InternalOp op)
{
    long Addr;
#ifdef FAST_LSB_WORD_ACCESS
    Addr = (*(uint32 *) CPU.PC + ICPU.Registers.X.W) & 0xffffff;
#elif defined FAST_ALIGNED_LSB_WORD_ACCESS
    if (((int) CPU.PC & 1) == 0)
        Addr = ((*(uint16 *) CPU.PC) + (*(CPU.PC + 2) << 16) + ICPU.Registers.X.W) & 0xFFFFFF;
    else
        Addr = (*CPU.PC + ((*(uint16 *) (CPU.PC + 1)) << 8) + ICPU.Registers.X.W) & 0xFFFFFF;
#else
    Addr = (*CPU.PC + (*(CPU.PC + 1) << 8) + (*(CPU.PC + 2) << 16) + ICPU.Registers.X.W) & 0xffffff;
#endif
#ifndef NO_OPEN_BUS
    if(a&READ) OpenBus = *(CPU.PC+2);
#endif
    CPU.PC += 3;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + CPU.MemSpeed;
#endif
    (*op)(Addr);
}

static void DirectIndirect (AccessMode a, InternalOp op)
{
#ifndef NO_OPEN_BUS
    OpenBus = *CPU.PC;
#endif
    long Addr = (*CPU.PC++ + ICPU.Registers.D.W) & 0xffff;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
    Addr = S9xGetWord (Addr);
#ifndef NO_OPEN_BUS
    if(a&READ) OpenBus = (uint8)(Addr>>8);
#endif
    Addr += ICPU.ShiftedDB;

//    if (ICPU.Registers.DL != 0) CPU.Cycles += ONE_CYCLE;
    (*op)(Addr);
}

static void DirectIndirectLong (AccessMode a, InternalOp op)
{
#ifndef NO_OPEN_BUS
    OpenBus = *CPU.PC;
#endif
    long Addr = (*CPU.PC++ + ICPU.Registers.D.W) & 0xffff;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
#ifndef NO_OPEN_BUS
    if(a&READ){
	Addr = S9xGetWord (Addr) + ((OpenBus=S9xGetByte (Addr + 2)) << 16);
    } else {
#endif
	Addr = S9xGetWord (Addr) + (S9xGetByte (Addr + 2) << 16);
#ifndef NO_OPEN_BUS
    }
#endif
//    if (ICPU.Registers.DL != 0) CPU.Cycles += ONE_CYCLE;
    (*op)(Addr);
}

static void StackRelative (AccessMode a, InternalOp op)
{
#ifndef NO_OPEN_BUS
    if(a&READ) OpenBus = *CPU.PC;
#endif
    long Addr = (*CPU.PC++ + ICPU.Registers.S.W) & 0xffff;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
#endif
    (*op)(Addr);
}

static void StackRelativeIndirectIndexed (AccessMode a, InternalOp op)
{
#ifndef NO_OPEN_BUS
    OpenBus = *CPU.PC;
#endif
    long Addr = (*CPU.PC++ + ICPU.Registers.S.W) & 0xffff;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + TWO_CYCLES;
#endif
    Addr = S9xGetWord (Addr);
#ifndef NO_OPEN_BUS
    if(a&READ) OpenBus = (uint8)(Addr>>8);
#endif
    Addr = (Addr + ICPU.ShiftedDB +
		 ICPU.Registers.Y.W) & 0xffffff;
    (*op)(Addr);
}
#endif

