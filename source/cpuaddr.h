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

EXTERN_C long OpAddress;

typedef enum {
    NONE = 0,
    READ = 1,
    WRITE = 2,
    MODIFY = 3,
    JUMP = 4
} AccessMode;

void Immediate8 (AccessMode a)
{
    OpAddress = ICPU.ShiftedPB + CPU.PC - CPU.PCBase;
    CPU.PC++;
}

void Immediate16 (AccessMode a)
{
    OpAddress = ICPU.ShiftedPB + CPU.PC - CPU.PCBase;
    CPU.PC += 2;
}

void Relative (AccessMode a)
{
    Int8 = *CPU.PC++;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif    
    OpAddress = ((int) (CPU.PC - CPU.PCBase) + Int8) & 0xffff;
}

void RelativeLong (AccessMode a)
{
#ifdef FAST_LSB_WORD_ACCESS
    OpAddress = *(uint16 *) CPU.PC;
#else
    OpAddress = *CPU.PC + (*(CPU.PC + 1) << 8);
#endif
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + ONE_CYCLE;
#endif
    CPU.PC += 2;
    OpAddress += (CPU.PC - CPU.PCBase);
    OpAddress &= 0xffff;
}

void AbsoluteIndexedIndirect (AccessMode a)
{
#ifdef FAST_LSB_WORD_ACCESS
    OpAddress = (Registers.X.W + *(uint16 *) CPU.PC) & 0xffff;
#else
    OpAddress = (Registers.X.W + *CPU.PC + (*(CPU.PC + 1) << 8)) & 0xffff;
#endif
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
#ifndef NO_OPEN_BUS
    OpenBus = *(CPU.PC + 1);
#endif
    CPU.PC += 2;
    OpAddress = S9xGetWord (ICPU.ShiftedPB + OpAddress);
#ifndef NO_OPEN_BUS
    if(a&READ) OpenBus = (uint8)(OpAddress>>8);
#endif
}

void AbsoluteIndirectLong (AccessMode a)
{
#ifdef FAST_LSB_WORD_ACCESS
    OpAddress = *(uint16 *) CPU.PC;
#else
    OpAddress = *CPU.PC + (*(CPU.PC + 1) << 8);
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
	OpAddress = S9xGetWord (OpAddress) | ((OpenBus=S9xGetByte (OpAddress + 2)) << 16);
    } else {
#endif
    OpAddress = S9xGetWord (OpAddress) | (S9xGetByte (OpAddress + 2) << 16);
#ifndef NO_OPEN_BUS
    }
#endif
}

void AbsoluteIndirect (AccessMode a)
{
#ifdef FAST_LSB_WORD_ACCESS
    OpAddress = *(uint16 *) CPU.PC;
#else
    OpAddress = *CPU.PC + (*(CPU.PC + 1) << 8);
#endif

#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
#ifndef NO_OPEN_BUS
    OpenBus = *(CPU.PC + 1);
#endif
    CPU.PC += 2;
    OpAddress = S9xGetWord (OpAddress);
#ifndef NO_OPEN_BUS
    if(a&READ) OpenBus = (uint8)(OpAddress>>8);
#endif
    OpAddress += ICPU.ShiftedPB;
}

void Absolute (AccessMode a)
{
#ifdef FAST_LSB_WORD_ACCESS
    OpAddress = *(uint16 *) CPU.PC + ICPU.ShiftedDB;
#else
    OpAddress = *CPU.PC + (*(CPU.PC + 1) << 8) + ICPU.ShiftedDB;
#endif
#ifndef NO_OPEN_BUS
    if(a&READ) OpenBus = *(CPU.PC+1);
#endif
    CPU.PC += 2;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2;
#endif
}

void AbsoluteLong (AccessMode a)
{
#ifdef FAST_LSB_WORD_ACCESS
    OpAddress = (*(uint32 *) CPU.PC) & 0xffffff;
#else
    OpAddress = *CPU.PC + (*(CPU.PC + 1) << 8) + (*(CPU.PC + 2) << 16);
#endif
#ifndef NO_OPEN_BUS
    if(a&READ) OpenBus = *(CPU.PC+2);
#endif
    CPU.PC += 3;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + CPU.MemSpeed;
#endif
}

void Direct(AccessMode a)
{
#ifndef NO_OPEN_BUS
    if(a&READ) OpenBus = *CPU.PC;
#endif
    OpAddress = (*CPU.PC++ + Registers.D.W) & 0xffff;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
//    if (Registers.DL != 0) CPU.Cycles += ONE_CYCLE;
}

void DirectIndirectIndexed (AccessMode a)
{
#ifndef NO_OPEN_BUS
    OpenBus = *CPU.PC;
#endif
    OpAddress = (*CPU.PC++ + Registers.D.W) & 0xffff;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif

    OpAddress = S9xGetWord (OpAddress);
#ifndef NO_OPEN_BUS
    if(a&READ) OpenBus = (uint8)(OpAddress>>8);
#endif
    OpAddress += ICPU.ShiftedDB + Registers.Y.W;

//    if (Registers.DL != 0) CPU.Cycles += ONE_CYCLE;
    // XXX: always add one if STA
    // XXX: else Add one cycle if crosses page boundary
}

void DirectIndirectIndexedLong (AccessMode a)
{
#ifndef NO_OPEN_BUS
    OpenBus = *CPU.PC;
#endif
    OpAddress = (*CPU.PC++ + Registers.D.W) & 0xffff;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif

#ifndef NO_OPEN_BUS
    if(a&READ){
	OpAddress = S9xGetWord (OpAddress) + ((OpenBus = S9xGetByte (OpAddress + 2)) << 16) + Registers.Y.W;
    } else {
#endif
	OpAddress = S9xGetWord (OpAddress) + (S9xGetByte (OpAddress + 2) << 16) + Registers.Y.W;
#ifndef NO_OPEN_BUS
    }
#endif
//    if (Registers.DL != 0) CPU.Cycles += ONE_CYCLE;
}

void DirectIndexedIndirect(AccessMode a)
{
#ifndef NO_OPEN_BUS
    OpenBus = *CPU.PC;
#endif
    OpAddress = (*CPU.PC++ + Registers.D.W + Registers.X.W) & 0xffff;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif

    OpAddress = S9xGetWord (OpAddress);
#ifndef NO_OPEN_BUS
    if(a&READ) OpenBus = (uint8)(OpAddress>>8);
#endif
    OpAddress += ICPU.ShiftedDB;

#ifndef SA1_OPCODES
//    if (Registers.DL != 0)
//	CPU.Cycles += TWO_CYCLES;
//    else
	CPU.Cycles += ONE_CYCLE;
#endif
}

void DirectIndexedX (AccessMode a)
{
#ifndef NO_OPEN_BUS
	if(a&READ) OpenBus = *CPU.PC;
#endif
    OpAddress = (*CPU.PC++ + Registers.D.W + Registers.X.W);
    OpAddress &= CheckEmulation() ? 0xff : 0xffff;

#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
//    if (Registers.DL != 0)
//	CPU.Cycles += TWO_CYCLES;
//    else
//	CPU.Cycles += ONE_CYCLE;
#endif
}

void DirectIndexedY (AccessMode a)
{
#ifndef NO_OPEN_BUS
	if(a&READ) OpenBus = *CPU.PC;
#endif
    OpAddress = (*CPU.PC++ + Registers.D.W + Registers.Y.W);
    OpAddress &= CheckEmulation() ? 0xff : 0xffff;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
//    if (Registers.DL != 0)
//	CPU.Cycles += TWO_CYCLES;
//    else
//	CPU.Cycles += ONE_CYCLE;
#endif
}

void AbsoluteIndexedX (AccessMode a)
{
#ifdef FAST_LSB_WORD_ACCESS
    OpAddress = ICPU.ShiftedDB + *(uint16 *) CPU.PC + Registers.X.W;
#else
    OpAddress = ICPU.ShiftedDB + *CPU.PC + (*(CPU.PC + 1) << 8) +
		Registers.X.W;
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
}

void AbsoluteIndexedY (AccessMode a)
{
#ifdef FAST_LSB_WORD_ACCESS
    OpAddress = ICPU.ShiftedDB + *(uint16 *) CPU.PC + Registers.Y.W;
#else
    OpAddress = ICPU.ShiftedDB + *CPU.PC + (*(CPU.PC + 1) << 8) +
		Registers.Y.W;
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
}

void AbsoluteLongIndexedX (AccessMode a)
{
#ifdef FAST_LSB_WORD_ACCESS
    OpAddress = (*(uint32 *) CPU.PC + Registers.X.W) & 0xffffff;
#else
    OpAddress = (*CPU.PC + (*(CPU.PC + 1) << 8) + (*(CPU.PC + 2) << 16) + Registers.X.W) & 0xffffff;
#endif
#ifndef NO_OPEN_BUS
    if(a&READ) OpenBus = *(CPU.PC+2);
#endif
    CPU.PC += 3;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeedx2 + CPU.MemSpeed;
#endif
}

void DirectIndirect (AccessMode a)
{
#ifndef NO_OPEN_BUS
    OpenBus = *CPU.PC;
#endif
    OpAddress = (*CPU.PC++ + Registers.D.W) & 0xffff;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
    OpAddress = S9xGetWord (OpAddress);
#ifndef NO_OPEN_BUS
    if(a&READ) OpenBus = (uint8)(OpAddress>>8);
#endif
    OpAddress += ICPU.ShiftedDB;

//    if (Registers.DL != 0) CPU.Cycles += ONE_CYCLE;
}

void DirectIndirectLong (AccessMode a)
{
#ifndef NO_OPEN_BUS
    OpenBus = *CPU.PC;
#endif
    OpAddress = (*CPU.PC++ + Registers.D.W) & 0xffff;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed;
#endif
#ifndef NO_OPEN_BUS
    if(a&READ){
	OpAddress = S9xGetWord (OpAddress) + ((OpenBus=S9xGetByte (OpAddress + 2)) << 16);
    } else {
#endif
	OpAddress = S9xGetWord (OpAddress) + (S9xGetByte (OpAddress + 2) << 16);
#ifndef NO_OPEN_BUS
    }
#endif
//    if (Registers.DL != 0) CPU.Cycles += ONE_CYCLE;
}

void StackRelative (AccessMode a)
{
#ifndef NO_OPEN_BUS
    if(a&READ) OpenBus = *CPU.PC;
#endif
    OpAddress = (*CPU.PC++ + Registers.S.W) & 0xffff;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + ONE_CYCLE;
#endif
}

void StackRelativeIndirectIndexed (AccessMode a)
{
#ifndef NO_OPEN_BUS
    OpenBus = *CPU.PC;
#endif
    OpAddress = (*CPU.PC++ + Registers.S.W) & 0xffff;
#ifndef SA1_OPCODES
    CPU.Cycles += CPU.MemSpeed + TWO_CYCLES;
#endif
    OpAddress = S9xGetWord (OpAddress);
#ifndef NO_OPEN_BUS
    if(a&READ) OpenBus = (uint8)(OpAddress>>8);
#endif
    OpAddress = (OpAddress + ICPU.ShiftedDB +
		 Registers.Y.W) & 0xffffff;
}
#endif

