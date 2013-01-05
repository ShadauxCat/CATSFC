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

#ifndef _CPUMACRO_H_
#define _CPUMACRO_H_

static void SetZN16 (uint16 Work)
{
    ICPU._Zero = Work != 0;
    ICPU._Negative = (uint8) (Work >> 8);
}

static void SetZN8 (uint8 Work)
{
    ICPU._Zero = Work;
    ICPU._Negative = Work;
}

static void ADC8 (long Addr)
{
    uint8 Work8 = S9xGetByte (Addr);
    
    if (CheckDecimal ())
    {
	uint8 A1 = (ICPU.Registers.A.W) & 0xF;
	uint8 A2 = (ICPU.Registers.A.W >> 4) & 0xF;
	uint8 W1 = Work8 & 0xF;
	uint8 W2 = (Work8 >> 4) & 0xF;

	A1 += W1 + CheckCarry();
	if (A1 > 9)
	{
	    A1 -= 10;
	    A1 &= 0xF;
	    A2++;
	}

	A2 += W2;
	if (A2 > 9)
	{
	    A2 -= 10;
	    A2 &= 0xF;
	    SetCarry ();
	}
	else
	{
	    ClearCarry ();
	}

	int8 Ans8 = (A2 << 4) | A1;
	if (~(ICPU.Registers.AL ^ Work8) &
	    (Work8 ^ Ans8) & 0x80)
	    SetOverflow();
	else
	    ClearOverflow();
	ICPU.Registers.AL = Ans8;
    }
    else
    {
	int16 Ans16 = ICPU.Registers.AL + Work8 + CheckCarry();

	ICPU._Carry = Ans16 >= 0x100;

	if (~(ICPU.Registers.AL ^ Work8) & 
	     (Work8 ^ (uint8) Ans16) & 0x80)
	    SetOverflow();
	else
	    ClearOverflow();
	ICPU.Registers.AL = (uint8) Ans16;
    }
    SetZN8 (ICPU.Registers.AL);
}

static void ADC16 (long Addr)
{
    uint16 Work16 = S9xGetWord (Addr);

    if (CheckDecimal ())
    {
	uint8 A1 = (ICPU.Registers.A.W) & 0xF;
	uint8 A2 = (ICPU.Registers.A.W >> 4) & 0xF;
	uint8 A3 = (ICPU.Registers.A.W >> 8) & 0xF;
	uint8 A4 = (ICPU.Registers.A.W >> 12) & 0xF;
	uint8 W1 = Work16 & 0xF;
	uint8 W2 = (Work16 >> 4) & 0xF;
	uint8 W3 = (Work16 >> 8) & 0xF;
	uint8 W4 = (Work16 >> 12) & 0xF;

	A1 += W1 + CheckCarry ();
	if (A1 > 9)
	{
	    A1 -= 10;
	    A1 &= 0xF;
	    A2++;
	}

	A2 += W2;
	if (A2 > 9)
	{
	    A2 -= 10;
	    A2 &= 0xF;
	    A3++;
	}

	A3 += W3;
	if (A3 > 9)
	{
	    A3 -= 10;
	    A3 &= 0xF;
	    A4++;
	}

	A4 += W4;
	if (A4 > 9)
	{
	    A4 -= 10;
	    A4 &= 0xF;
	    SetCarry ();
	}
	else
	{
	    ClearCarry ();
	}

	uint16 Ans16 = (A4 << 12) | (A3 << 8) | (A2 << 4) | (A1);
	if (~(ICPU.Registers.A.W ^ Work16) &
	    (Work16 ^ Ans16) & 0x8000)
	    SetOverflow();
	else
	    ClearOverflow();
	ICPU.Registers.A.W = Ans16;
    }
    else
    {
	uint32 Ans32 = ICPU.Registers.A.W + Work16 + CheckCarry();

	ICPU._Carry = Ans32 >= 0x10000;

	if (~(ICPU.Registers.A.W ^ Work16) &
	    (Work16 ^ (uint16) Ans32) & 0x8000)
	    SetOverflow();
	else
	    ClearOverflow();
	ICPU.Registers.A.W = (uint16) Ans32;
    }
    SetZN16 (ICPU.Registers.A.W);
}

static void AND16 (long Addr)
{
    ICPU.Registers.A.W &= S9xGetWord (Addr);
    SetZN16 (ICPU.Registers.A.W);
}

static void AND8 (long Addr)
{
    ICPU.Registers.AL &= S9xGetByte (Addr);
    SetZN8 (ICPU.Registers.AL);
}

static inline void A_ASL16 ()
{
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
    ICPU._Carry = (ICPU.Registers.AH & 0x80) != 0;
    ICPU.Registers.A.W <<= 1;
    SetZN16 (ICPU.Registers.A.W);
}

static inline void A_ASL8 ()
{
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
    ICPU._Carry = (ICPU.Registers.AL & 0x80) != 0;
    ICPU.Registers.AL <<= 1;
    SetZN8 (ICPU.Registers.AL);
}

static void ASL16 (long Addr)
{
    uint16 Work16 = S9xGetWord (Addr);
    ICPU._Carry = (Work16 & 0x8000) != 0;
    Work16 <<= 1;
    //S9xSetWord (Work16, Addr);
	S9xSetByte(Work16>>8, Addr+1);
	S9xSetByte(Work16&0xFF, Addr);
    SetZN16 (Work16);
}

static void ASL8 (long Addr)
{
    uint8 Work8 = S9xGetByte (Addr);
    ICPU._Carry = (Work8 & 0x80) != 0;
    Work8 <<= 1;
    S9xSetByte (Work8, Addr);
    SetZN8 (Work8);
}

static void BIT16 (long Addr)
{
    uint16 Work16 = S9xGetWord (Addr);
    ICPU._Overflow = (Work16 & 0x4000) != 0;
    ICPU._Negative = (uint8) (Work16 >> 8);
    ICPU._Zero = (Work16 & ICPU.Registers.A.W) != 0;
}

static void BIT8 (long Addr)
{
    uint8 Work8 = S9xGetByte (Addr);
    ICPU._Overflow = (Work8 & 0x40) != 0;
    ICPU._Negative = Work8;
    ICPU._Zero = Work8 & ICPU.Registers.AL;
}

static void CMP16 (long Addr)
{
    int32 Int32 = (long) ICPU.Registers.A.W -
	    (long) S9xGetWord (Addr);
    ICPU._Carry = Int32 >= 0;
    SetZN16 ((uint16) Int32);
}

static void CMP8 (long Addr)
{
    int16 Int16 = (short) ICPU.Registers.AL -
	    (short) S9xGetByte (Addr);
    ICPU._Carry = Int16 >= 0;
    SetZN8 ((uint8) Int16);
}

static void CMX16 (long Addr)
{
    int32 Int32 = (long) ICPU.Registers.X.W -
	    (long) S9xGetWord (Addr);
    ICPU._Carry = Int32 >= 0;
    SetZN16 ((uint16) Int32);
}

static void CMX8 (long Addr)
{
    int16 Int16 = (short) ICPU.Registers.XL -
	    (short) S9xGetByte (Addr);
    ICPU._Carry = Int16 >= 0;
    SetZN8 ((uint8) Int16);
}

static void CMY16 (long Addr)
{
    int32 Int32 = (long) ICPU.Registers.Y.W -
	    (long) S9xGetWord (Addr);
    ICPU._Carry = Int32 >= 0;
    SetZN16 ((uint16) Int32);
}

static void CMY8 (long Addr)
{
    int16 Int16 = (short) ICPU.Registers.YL -
	    (short) S9xGetByte (Addr);
    ICPU._Carry = Int16 >= 0;
    SetZN8 ((uint8) Int16);
}

static inline void A_DEC16 ()
{
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
#ifdef CPU_SHUTDOWN
    CPU.WaitAddress = NULL;
#endif

    ICPU.Registers.A.W--;
    SetZN16 (ICPU.Registers.A.W);
}

static inline void A_DEC8 ()
{
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
#ifdef CPU_SHUTDOWN
    CPU.WaitAddress = NULL;
#endif

    ICPU.Registers.AL--;
    SetZN8 (ICPU.Registers.AL);
}

static void DEC16 (long Addr)
{
#ifdef CPU_SHUTDOWN
    CPU.WaitAddress = NULL;
#endif

    uint16 Work16 = S9xGetWord (Addr) - 1;
    //S9xSetWord (Work16, Addr);
    S9xSetByte (Work16>>8, Addr+1);
	S9xSetByte (Work16&0xFF, Addr);
	SetZN16 (Work16);
}

static void DEC8 (long Addr)
{
#ifdef CPU_SHUTDOWN
    CPU.WaitAddress = NULL;
#endif

    uint8 Work8 = S9xGetByte (Addr) - 1;
    S9xSetByte (Work8, Addr);
    SetZN8 (Work8);
}

static void EOR16 (long Addr)
{
    ICPU.Registers.A.W ^= S9xGetWord (Addr);
    SetZN16 (ICPU.Registers.A.W);
}

static void EOR8 (long Addr)
{
    ICPU.Registers.AL ^= S9xGetByte (Addr);
    SetZN8 (ICPU.Registers.AL);
}

static inline void A_INC16 ()
{
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
#ifdef CPU_SHUTDOWN
    CPU.WaitAddress = NULL;
#endif

    ICPU.Registers.A.W++;
    SetZN16 (ICPU.Registers.A.W);
}

static inline void A_INC8 ()
{
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
#ifdef CPU_SHUTDOWN
    CPU.WaitAddress = NULL;
#endif

    ICPU.Registers.AL++;
    SetZN8 (ICPU.Registers.AL);
}

static void INC16 (long Addr)
{
#ifdef CPU_SHUTDOWN
    CPU.WaitAddress = NULL;
#endif

    uint16 Work16 = S9xGetWord (Addr) + 1;
    //S9xSetWord (Work16, Addr);
	S9xSetByte (Work16>>8, Addr+1);
	S9xSetByte (Work16&0xFF, Addr);
    SetZN16 (Work16);
}

static void INC8 (long Addr)
{
#ifdef CPU_SHUTDOWN
    CPU.WaitAddress = NULL;
#endif

    uint8 Work8 = S9xGetByte (Addr) + 1;
    S9xSetByte (Work8, Addr);
    SetZN8 (Work8);
}

static void LDA16 (long Addr)
{
    ICPU.Registers.A.W = S9xGetWord (Addr);
    SetZN16 (ICPU.Registers.A.W);
}

static void LDA8 (long Addr)
{
    ICPU.Registers.AL = S9xGetByte (Addr);
    SetZN8 (ICPU.Registers.AL);
}

static void LDX16 (long Addr)
{
    ICPU.Registers.X.W = S9xGetWord (Addr);
    SetZN16 (ICPU.Registers.X.W);
}

static void LDX8 (long Addr)
{
    ICPU.Registers.XL = S9xGetByte (Addr);
    SetZN8 (ICPU.Registers.XL);
}

static void LDY16 (long Addr)
{
    ICPU.Registers.Y.W = S9xGetWord (Addr);
    SetZN16 (ICPU.Registers.Y.W);
}

static void LDY8 (long Addr)
{
    ICPU.Registers.YL = S9xGetByte (Addr);
    SetZN8 (ICPU.Registers.YL);
}

static inline void A_LSR16 ()
{
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
    ICPU._Carry = ICPU.Registers.AL & 1;
    ICPU.Registers.A.W >>= 1;
    SetZN16 (ICPU.Registers.A.W);
}

static inline void A_LSR8 ()
{
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
    ICPU._Carry = ICPU.Registers.AL & 1;
    ICPU.Registers.AL >>= 1;
    SetZN8 (ICPU.Registers.AL);
}

static void LSR16 (long Addr)
{
    uint16 Work16 = S9xGetWord (Addr);
    ICPU._Carry = Work16 & 1;
    Work16 >>= 1;
    //S9xSetWord (Work16, Addr);
	S9xSetByte (Work16>>8, Addr+1);
	S9xSetByte (Work16&0xFF, Addr);
    SetZN16 (Work16);
}

static void LSR8 (long Addr)
{
    uint8 Work8 = S9xGetByte (Addr);
    ICPU._Carry = Work8 & 1;
    Work8 >>= 1;
    S9xSetByte (Work8, Addr);
    SetZN8 (Work8);
}

static void ORA16 (long Addr)
{
    ICPU.Registers.A.W |= S9xGetWord (Addr);
    SetZN16 (ICPU.Registers.A.W);
}

static void ORA8 (long Addr)
{
    ICPU.Registers.AL |= S9xGetByte (Addr);
    SetZN8 (ICPU.Registers.AL);
}

static inline void A_ROL16 ()
{
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
    uint32 Work32 = (ICPU.Registers.A.W << 1) | CheckCarry();
    ICPU._Carry = Work32 >= 0x10000;
    ICPU.Registers.A.W = (uint16) Work32;
    SetZN16 ((uint16) Work32);
}

static inline void A_ROL8 ()
{
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
    uint16 Work16 = ICPU.Registers.AL;
    Work16 <<= 1;
    Work16 |= CheckCarry();
    ICPU._Carry = Work16 >= 0x100;
    ICPU.Registers.AL = (uint8) Work16;
    SetZN8 ((uint8) Work16);
}

static void ROL16 (long Addr)
{
    uint32 Work32 = S9xGetWord (Addr);
    Work32 <<= 1;
    Work32 |= CheckCarry();
    ICPU._Carry = Work32 >= 0x10000;
    //S9xSetWord ((uint16) Work32, Addr);
	S9xSetByte((Work32>>8)&0xFF, Addr+1);
	S9xSetByte(Work32&0xFF, Addr);
    SetZN16 ((uint16) Work32);
}

static void ROL8 (long Addr)
{
    uint16 Work16 = S9xGetByte (Addr);
    Work16 <<= 1;
    Work16 |= CheckCarry ();
    ICPU._Carry = Work16 >= 0x100;
    S9xSetByte ((uint8) Work16, Addr);
    SetZN8 ((uint8) Work16);
}

static inline void A_ROR16 ()
{
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
    uint32 Work32 = ICPU.Registers.A.W;
    Work32 |= (int) CheckCarry() << 16;
    ICPU._Carry = (uint8) (Work32 & 1);
    Work32 >>= 1;
    ICPU.Registers.A.W = (uint16) Work32;
    SetZN16 ((uint16) Work32);
}

static inline void A_ROR8 ()
{
#ifndef SA1_OPCODES
    CPU.Cycles += ONE_CYCLE;
#endif
    uint16 Work16 = ICPU.Registers.AL | ((uint16) CheckCarry() << 8);
    ICPU._Carry = (uint8) Work16 & 1;
    Work16 >>= 1;
    ICPU.Registers.AL = (uint8) Work16;
    SetZN8 ((uint8) Work16);
}

static void ROR16 (long Addr)
{
    uint32 Work32 = S9xGetWord (Addr);
    Work32 |= (int) CheckCarry() << 16;
    ICPU._Carry = (uint8) (Work32 & 1);
    Work32 >>= 1;
    //S9xSetWord ((uint16) Work32, Addr);
	S9xSetByte ( (Work32>>8)&0x00FF, Addr+1);
	S9xSetByte (Work32&0x00FF, Addr);
    SetZN16 ((uint16) Work32);
}

static void ROR8 (long Addr)
{
    uint16 Work16 = S9xGetByte (Addr);
    Work16 |= (int) CheckCarry () << 8;
    ICPU._Carry = (uint8) (Work16 & 1);
    Work16 >>= 1;
    S9xSetByte ((uint8) Work16, Addr);
    SetZN8 ((uint8) Work16);
}

static void SBC16 (long Addr)
{
    uint16 Work16 = S9xGetWord (Addr);

    if (CheckDecimal ())
    {
	uint8 A1 = (ICPU.Registers.A.W) & 0xF;
	uint8 A2 = (ICPU.Registers.A.W >> 4) & 0xF;
	uint8 A3 = (ICPU.Registers.A.W >> 8) & 0xF;
	uint8 A4 = (ICPU.Registers.A.W >> 12) & 0xF;
	uint8 W1 = Work16 & 0xF;
	uint8 W2 = (Work16 >> 4) & 0xF;
	uint8 W3 = (Work16 >> 8) & 0xF;
	uint8 W4 = (Work16 >> 12) & 0xF;

	A1 -= W1 + !CheckCarry ();
	A2 -= W2;
	A3 -= W3;
	A4 -= W4;
	if (A1 > 9)
	{
	    A1 += 10;
	    A2--;
	}
	if (A2 > 9)
	{
	    A2 += 10;
	    A3--;
	}
	if (A3 > 9)
	{
	    A3 += 10;
	    A4--;
	}
	if (A4 > 9)
	{
	    A4 += 10;
	    ClearCarry ();
	}
	else
	{
	    SetCarry ();
	}

	uint16 Ans16 = (A4 << 12) | (A3 << 8) | (A2 << 4) | (A1);
	if ((ICPU.Registers.A.W ^ Work16) &
	    (ICPU.Registers.A.W ^ Ans16) & 0x8000)
	    SetOverflow();
	else
	    ClearOverflow();
	ICPU.Registers.A.W = Ans16;
	SetZN16 (ICPU.Registers.A.W);
    }
    else
    {

	int32 Int32 = (long) ICPU.Registers.A.W - (long) Work16 + (long) CheckCarry() - 1;

	ICPU._Carry = Int32 >= 0;

	if ((ICPU.Registers.A.W ^ Work16) &
	    (ICPU.Registers.A.W ^ (uint16) Int32) & 0x8000)
	    SetOverflow();
	else
	    ClearOverflow ();
	ICPU.Registers.A.W = (uint16) Int32;
	SetZN16 (ICPU.Registers.A.W);
    }
}

static void SBC8 (long Addr)
{
    uint8 Work8 = S9xGetByte (Addr);
    if (CheckDecimal ())
    {
	uint8 A1 = (ICPU.Registers.A.W) & 0xF;
	uint8 A2 = (ICPU.Registers.A.W >> 4) & 0xF;
	uint8 W1 = Work8 & 0xF;
	uint8 W2 = (Work8 >> 4) & 0xF;

	A1 -= W1 + !CheckCarry ();
	A2 -= W2;
	if (A1 > 9)
	{
	    A1 += 10;
	    A2--;
	}
	if (A2 > 9)
	{
	    A2 += 10;
	    ClearCarry ();
	}
	else
	{
	    SetCarry ();
	}

	uint8 Ans8 = (A2 << 4) | A1;
	if ((ICPU.Registers.AL ^ Work8) &
	    (ICPU.Registers.AL ^ Ans8) & 0x80)
	    SetOverflow ();
	else
	    ClearOverflow ();
	ICPU.Registers.AL = Ans8;
	SetZN8 (ICPU.Registers.AL);
    }
    else
    {
	int16 Int16 = (short) ICPU.Registers.AL - (short) Work8 + (short) CheckCarry() - 1;

	ICPU._Carry = Int16 >= 0;
	if ((ICPU.Registers.AL ^ Work8) &
	    (ICPU.Registers.AL ^ (uint8) Int16) & 0x80)
	    SetOverflow ();
	else
	    ClearOverflow ();
	ICPU.Registers.AL = (uint8) Int16;
	SetZN8 (ICPU.Registers.AL);
    }
}

static void STA16 (long Addr)
{
    S9xSetWord (ICPU.Registers.A.W, Addr);
}

static void STA8 (long Addr)
{
    S9xSetByte (ICPU.Registers.AL, Addr);
}

static void STX16 (long Addr)
{
    S9xSetWord (ICPU.Registers.X.W, Addr);
}

static void STX8 (long Addr)
{
    S9xSetByte (ICPU.Registers.XL, Addr);
}

static void STY16 (long Addr)
{
    S9xSetWord (ICPU.Registers.Y.W, Addr);
}

static void STY8 (long Addr)
{
    S9xSetByte (ICPU.Registers.YL, Addr);
}

static void STZ16 (long Addr)
{
    S9xSetWord (0, Addr);
}

static void STZ8 (long Addr)
{
    S9xSetByte (0, Addr);
}

static void TSB16 (long Addr)
{
    uint16 Work16 = S9xGetWord (Addr);
    ICPU._Zero = (Work16 & ICPU.Registers.A.W) != 0;
    Work16 |= ICPU.Registers.A.W;
    //S9xSetWord (Work16, Addr);
	S9xSetByte (Work16>>8, Addr+1);
	S9xSetByte (Work16&0xFF, Addr);
}

static void TSB8 (long Addr)
{
    uint8 Work8 = S9xGetByte (Addr);
    ICPU._Zero = Work8 & ICPU.Registers.AL;
    Work8 |= ICPU.Registers.AL;
    S9xSetByte (Work8, Addr);
}

static void TRB16 (long Addr)
{
    uint16 Work16 = S9xGetWord (Addr);
    ICPU._Zero = (Work16 & ICPU.Registers.A.W) != 0;
    Work16 &= ~ICPU.Registers.A.W;
    //S9xSetWord (Work16, Addr);
	S9xSetByte (Work16>>8, Addr+1);
	S9xSetByte (Work16&0xFF, Addr);
}

static void TRB8 (long Addr)
{
    uint8 Work8 = S9xGetByte (Addr);
    ICPU._Zero = Work8 & ICPU.Registers.AL;
    Work8 &= ~ICPU.Registers.AL;
    S9xSetByte (Work8, Addr);
}
#endif

