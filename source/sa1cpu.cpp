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

#include "snes9x.h"
#include "memmap.h"
#include "ppu.h"
#include "cpuexec.h"

#include "sa1.h"
#define CPU SA1
#define ICPU SA1
#define S9xGetByte S9xSA1GetByte
#define S9xGetWord S9xSA1GetWord
#define S9xSetByte S9xSA1SetByte
#define S9xSetWord S9xSA1SetWord
#define S9xSetPCBase S9xSA1SetPCBase
#define S9xOpcodesM1X1 S9xSA1OpcodesM1X1
#define S9xOpcodesM1X0 S9xSA1OpcodesM1X0
#define S9xOpcodesM0X1 S9xSA1OpcodesM0X1
#define S9xOpcodesM0X0 S9xSA1OpcodesM0X0
#define S9xOpcodesE1   S9xSA1OpcodesE1
#define S9xOpcode_IRQ S9xSA1Opcode_IRQ
#define S9xOpcode_NMI S9xSA1Opcode_NMI
#define S9xUnpackStatus S9xSA1UnpackStatus
#define S9xPackStatus S9xSA1PackStatus
#define S9xFixCycles S9xSA1FixCycles
#define Immediate8 SA1Immediate8
#define Immediate16 SA1Immediate16
#define Relative SA1Relative
#define RelativeLong SA1RelativeLong
#define AbsoluteIndexedIndirect SA1AbsoluteIndexedIndirect
#define AbsoluteIndirectLong SA1AbsoluteIndirectLong
#define AbsoluteIndirect SA1AbsoluteIndirect
#define Absolute SA1Absolute
#define AbsoluteLong SA1AbsoluteLong
#define Direct SA1Direct
#define DirectIndirectIndexed SA1DirectIndirectIndexed
#define DirectIndirectIndexedLong SA1DirectIndirectIndexedLong
#define DirectIndexedIndirect SA1DirectIndexedIndirect
#define DirectIndexedX SA1DirectIndexedX
#define DirectIndexedY SA1DirectIndexedY
#define AbsoluteIndexedX SA1AbsoluteIndexedX
#define AbsoluteIndexedY SA1AbsoluteIndexedY
#define AbsoluteLongIndexedX SA1AbsoluteLongIndexedX
#define DirectIndirect SA1DirectIndirect
#define DirectIndirectLong SA1DirectIndirectLong
#define StackRelative SA1StackRelative
#define StackRelativeIndirectIndexed SA1StackRelativeIndirectIndexed

#define SetZN16 SA1SetZN16
#define SetZN8 SA1SetZN8
#define ADC8 SA1ADC8
#define ADC16 SA1ADC16
#define AND16 SA1AND16
#define AND8 SA1AND8
#define A_ASL16 SA1A_ASL16
#define A_ASL8 SA1A_ASL8
#define ASL16 SA1ASL16
#define ASL8 SA1ASL8
#define BIT16 SA1BIT16
#define BIT8 SA1BIT8
#define CMP16 SA1CMP16
#define CMP8 SA1CMP8
#define CMX16 SA1CMX16
#define CMX8 SA1CMX8
#define CMY16 SA1CMY16
#define CMY8 SA1CMY8
#define A_DEC16 SA1A_DEC16
#define A_DEC8 SA1A_DEC8
#define DEC16 SA1DEC16
#define DEC8 SA1DEC8
#define EOR16 SA1EOR16
#define EOR8 SA1EOR8
#define A_INC16 SA1A_INC16
#define A_INC8 SA1A_INC8
#define INC16 SA1INC16
#define INC8 SA1INC8
#define LDA16 SA1LDA16
#define LDA8 SA1LDA8
#define LDX16 SA1LDX16
#define LDX8 SA1LDX8
#define LDY16 SA1LDY16
#define LDY8 SA1LDY8
#define A_LSR16 SA1A_LSR16
#define A_LSR8 SA1A_LSR8
#define LSR16 SA1LSR16
#define LSR8 SA1LSR8
#define ORA16 SA1ORA16
#define ORA8 SA1ORA8
#define A_ROL16 SA1A_ROL16
#define A_ROL8 SA1A_ROL8
#define ROL16 SA1ROL16
#define ROL8 SA1ROL8
#define A_ROR16 SA1A_ROR16
#define A_ROR8 SA1A_ROR8
#define ROR16 SA1ROR16
#define ROR8 SA1ROR8
#define SBC16 SA1SBC16
#define SBC8 SA1SBC8
#define STA16 SA1STA16
#define STA8 SA1STA8
#define STX16 SA1STX16
#define STX8 SA1STX8
#define STY16 SA1STY16
#define STY8 SA1STY8
#define STZ16 SA1STZ16
#define STZ8 SA1STZ8
#define TSB16 SA1TSB16
#define TSB8 SA1TSB8
#define TRB16 SA1TRB16
#define TRB8 SA1TRB8

//#undef CPU_SHUTDOWN
#undef VAR_CYCLES
#define SA1_OPCODES

#include "cpuops.cpp"

void S9xSA1MainLoop ()
{
    int i;

#if 0
    if (SA1.Flags & NMI_FLAG)
    {
	SA1.Flags &= ~NMI_FLAG;
	if (SA1.WaitingForInterrupt)
	{
	    SA1.WaitingForInterrupt = FALSE;
	    SA1.PC++;
	}
	S9xSA1Opcode_NMI ();
    }
#endif
    if (SA1.Flags & IRQ_PENDING_FLAG)
    {
	if (SA1.IRQActive)
	{
	    if (SA1.WaitingForInterrupt)
	    {
		SA1.WaitingForInterrupt = FALSE;
		SA1.PC++;
	    }
	    if (!SA1CheckFlag (IRQ))
		S9xSA1Opcode_IRQ ();
	}
	else
	    SA1.Flags &= ~IRQ_PENDING_FLAG;
    }
#ifdef DEBUGGER
    if (SA1.Flags & TRACE_FLAG)
    {
	for (i = 0; i < 3 && SA1.Executing; i++)
	{
	    S9xSA1Trace ();
#ifdef CPU_SHUTDOWN
	    SA1.PCAtOpcodeStart = SA1.PC;
#endif
	    (*SA1.S9xOpcodes [*SA1.PC++].S9xOpcode) ();
	}
    }
    else
#endif
    for (i = 0; i < 3 && SA1.Executing; i++)
    {
#ifdef CPU_SHUTDOWN
	SA1.PCAtOpcodeStart = SA1.PC;
#endif
	(*SA1.S9xOpcodes [*SA1.PC++].S9xOpcode) ();
    }
}

