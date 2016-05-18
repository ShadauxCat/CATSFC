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
#ifndef _srtc_h_
#define _srtc_h_

#include <time.h>

#define MAX_RTC_INDEX       0xC

#define MODE_READ           0
#define MODE_LOAD_RTC       1
#define MODE_COMMAND        2
#define MODE_COMMAND_DONE   3

#define COMMAND_LOAD_RTC    0
#define COMMAND_CLEAR_RTC   4


/***   The format of the rtc_data structure is:

Index Description     Range (nibble)
----- --------------  ---------------------------------------

  0   Seconds low     0-9
  1   Seconds high    0-5

  2   Minutes low     0-9
  3   Minutes high    0-5

  4   Hour low        0-9
  5   Hour high       0-2

  6   Day low         0-9
  7   Day high        0-3

  8   Month           1-C (0xC is December, 12th month)

  9   Year ones       0-9
  A   Year tens       0-9
  B   Year High       9-B  (9=19xx, A=20xx, B=21xx)

  C   Day of week     0-6  (0=Sunday, 1=Monday,...,6=Saturday)

***/

typedef struct
{
   bool needs_init;
   bool count_enable; // Does RTC mark time or is it frozen
   uint8_t data [MAX_RTC_INDEX + 1];
   int8_t  index;
   uint8_t mode;

   time_t system_timestamp;  // Of latest RTC load time
   uint32_t pad;
} SRTC_DATA;

extern SRTC_DATA           rtc;

void    S9xUpdateSrtcTime();
void  S9xSetSRTC(uint8_t data, uint16_t Address);
uint8_t S9xGetSRTC(uint16_t Address);
void  S9xSRTCPreSaveState();
void  S9xSRTCPostLoadState();
void  S9xResetSRTC();
void  S9xHardResetSRTC();

#define SRTC_SRAM_PAD (4 + 8 + 1 + MAX_RTC_INDEX)

#endif   // _srtc_h

