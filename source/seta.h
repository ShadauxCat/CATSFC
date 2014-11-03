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

#ifndef NO_SETA
#ifndef _seta_h
#define _seta_h

#include "port.h"

#define ST_010 0x01
#define ST_011 0x02
#define ST_018 0x03

uint8_t S9xGetSetaDSP(uint32_t Address);
void S9xSetSetaDSP(uint8_t byte, uint32_t Address);
uint8_t S9xGetST018(uint32_t Address);
void S9xSetST018(uint8_t Byte, uint32_t Address);

uint8_t S9xGetST010(uint32_t Address);
void S9xSetST010(uint32_t Address, uint8_t Byte);
uint8_t S9xGetST011(uint32_t Address);
void S9xSetST011(uint32_t Address, uint8_t Byte);

extern void (*SetSETA)(uint32_t, uint8_t);
extern uint8_t(*GetSETA)(uint32_t);

typedef struct SETA_ST010_STRUCT
{
   uint8_t input_params[16];
   uint8_t output_params[16];
   uint8_t op_reg;
   uint8_t execute;
   bool control_enable;
} ST010_Regs;

typedef struct SETA_ST011_STRUCT
{
   bool waiting4command;
   uint8_t status;
   uint8_t command;
   uint32_t in_count;
   uint32_t in_index;
   uint32_t out_count;
   uint32_t out_index;
   uint8_t parameters [512];
   uint8_t output [512];
} ST011_Regs;

typedef struct SETA_ST018_STRUCT
{
   bool waiting4command;
   uint8_t status;
   uint8_t part_command;
   uint8_t pass;
   uint32_t command;
   uint32_t in_count;
   uint32_t in_index;
   uint32_t out_count;
   uint32_t out_index;
   uint8_t parameters [512];
   uint8_t output [512];
} ST018_Regs;

#endif
#endif

