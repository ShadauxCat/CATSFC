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
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "snes9x.h"
#include "cheats.h"
#include "memmap.h"

static bool8 S9xAllHex (const char *code, int len)
{
    for (int i = 0; i < len; i++)
        if ((code [i] < '0' || code [i] > '9') &&
            (code [i] < 'a' || code [i] > 'f') &&
            (code [i] < 'A' || code [i] > 'F'))
            return (FALSE);

    return (TRUE);
}

const char *S9xProActionReplayToRaw (const char *code, uint32 *address, uint8 *byte)
{
    uint32 data = 0;
    if (strlen (code) != 8 || !S9xAllHex (code, 8) ||
        sscanf (code, "%x", &data) != 1)
	return ("Invalid Pro Action Replay code - should be 8 hex digits in length.");

    *address = data >> 8;
    *byte = (uint8) data;
    return (NULL);
}

const char *S9xGoldFingerToRaw (const char *code, uint32 *address, bool8 *sram,
			        uint8 *num_bytes, uint8 bytes[3])
{
    char tmp [15];
    if (strlen (code) != 14)
	return ("Invalid Gold Finger code should be 14 hex digits in length.");

    strncpy (tmp, code, 5);
    tmp [5] = 0;
    if (sscanf (tmp, "%x", address) != 1)
	return ("Invalid Gold Finger code.");

    int i;
    for (i = 0; i < 3; i++)
    {
	strncpy (tmp, code + 5 + i * 2, 2);
	tmp [2] = 0;
	int byte;
	if (sscanf (tmp, "%x", &byte) != 1)
	    break;
	bytes [i] = (uint8) byte;
    }
    *num_bytes = i;
    *sram = code [13] == '1';
    return (NULL);
}

const char *S9xGameGenieToRaw (const char *code, uint32 *address, uint8 *byte)
{
    char new_code [12];
    
    if (strlen (code) != 9 || *(code + 4) != '-' || !S9xAllHex (code, 4) ||
        !S9xAllHex (code + 5, 4))
	return ("Invalid Game Genie(tm) code - should be 'xxxx-xxxx'.");

    strcpy (new_code, "0x");
    strncpy (new_code + 2, code, 4);
    strcpy (new_code + 6, code + 5);

    static char *real_hex  = "0123456789ABCDEF";
    static char *genie_hex = "DF4709156BC8A23E";
    
    for (int i = 2; i < 10; i++)
    {
	if (islower (new_code [i]))
	    new_code [i] = toupper (new_code [i]);
	int j;
	for (j = 0; j < 16; j++)
	{
	    if (new_code [i] == genie_hex [j])
	    {
		new_code [i] = real_hex [j];
		break;
	    }
	}
	if (j == 16)
	    return ("Invalid hex-character in Game Genie(tm) code");
    }
    uint32 data = 0;
    sscanf (new_code, "%x", &data);
    *byte = (uint8)(data >> 24);
    *address = ((data & 0x003c00) << 10) +
	      ((data & 0x00003c) << 14) +
	      ((data & 0xf00000) >>  8) +
	      ((data & 0x000003) << 10) +
	      ((data & 0x00c000) >>  6) +
	      ((data & 0x0f0000) >> 12) +
	      ((data & 0x0003c0) >>  6);

    return (NULL);
}

void S9xStartCheatSearch (SCheatData *d)
{
    memmove (d->CWRAM, d->RAM, 0x20000);
    memmove (d->CSRAM, d->SRAM, 0x10000);
    memmove (d->CIRAM, &d->FillRAM [0x3000], 0x2000);
    memset ((char *) d->WRAM_BITS, 0xff, 0x20000 >> 3);
    memset ((char *) d->SRAM_BITS, 0xff, 0x10000 >> 3);
    memset ((char *) d->IRAM_BITS, 0xff, 0x2000 >> 3);
}

#define BIT_CLEAR(a,v) \
(a)[(v) >> 5] &= ~(1 << ((v) & 31))

#define BIT_SET(a,v) \
(a)[(v) >> 5] |= 1 << ((v) & 31)

#define TEST_BIT(a,v) \
((a)[(v) >> 5] & (1 << ((v) & 31)))

#define _C(c,a,b) \
((c) == S9X_LESS_THAN ? (a) < (b) : \
 (c) == S9X_GREATER_THAN ? (a) > (b) : \
 (c) == S9X_LESS_THAN_OR_EQUAL ? (a) <= (b) : \
 (c) == S9X_GREATER_THAN_OR_EQUAL ? (a) >= (b) : \
 (c) == S9X_EQUAL ? (a) == (b) : \
 (a) != (b))

#define _D(s,m,o) \
((s) == S9X_8_BITS ? (uint8) (*((m) + (o))) : \
 (s) == S9X_16_BITS ? ((uint16) (*((m) + (o)) + (*((m) + (o) + 1) << 8))) : \
 (s) == S9X_24_BITS ? ((uint32) (*((m) + (o)) + (*((m) + (o) + 1) << 8) + (*((m) + (o) + 2) << 16))) : \
((uint32)  (*((m) + (o)) + (*((m) + (o) + 1) << 8) + (*((m) + (o) + 2) << 16) + (*((m) + (o) + 3) << 24))))

#define _DS(s,m,o) \
((s) == S9X_8_BITS ? ((int8) *((m) + (o))) : \
 (s) == S9X_16_BITS ? ((int16) (*((m) + (o)) + (*((m) + (o) + 1) << 8))) : \
 (s) == S9X_24_BITS ? (((int32) ((*((m) + (o)) + (*((m) + (o) + 1) << 8) + (*((m) + (o) + 2) << 16)) << 8)) >> 8): \
 ((int32) (*((m) + (o)) + (*((m) + (o) + 1) << 8) + (*((m) + (o) + 2) << 16) + (*((m) + (o) + 3) << 24))))

void S9xSearchForChange (SCheatData *d, S9xCheatComparisonType cmp, 
                         S9xCheatDataSize size, bool8 is_signed, bool8 update)
{
    int l;

    switch (size)
    {
    case S9X_8_BITS: l = 0; break;
    case S9X_16_BITS: l = 1; break;
    case S9X_24_BITS: l = 2; break;
    default:
    case S9X_32_BITS: l = 3; break;
    }

    int i;
    if (is_signed)
    {
        for (i = 0; i < 0x20000 - l; i++)
        {
            if (TEST_BIT (d->WRAM_BITS, i) &&
                _C(cmp, _DS(size, d->RAM, i), _DS(size, d->CWRAM, i)))
            {
                if (update)
                    d->CWRAM [i] = d->RAM [i];
            }
            else
                BIT_CLEAR (d->WRAM_BITS, i);
        }
        
        for (i = 0; i < 0x10000 - l; i++)
        {
            if (TEST_BIT (d->SRAM_BITS, i) &&
                _C(cmp, _DS(size, d->SRAM, i), _DS(size, d->CSRAM, i)))
            {
                if (update)
                    d->CSRAM [i] = d->SRAM [i];
            }
            else
                BIT_CLEAR (d->SRAM_BITS, i);
        }
        
        for (i = 0; i < 0x2000 - l; i++)
        {
            if (TEST_BIT (d->IRAM_BITS, i) &&
                _C(cmp, _DS(size, d->FillRAM + 0x3000, i), _DS(size, d->CIRAM, i)))
            {
                if (update)
                    d->CIRAM [i] = d->FillRAM [i + 0x3000];
            }
            else
                BIT_CLEAR (d->IRAM_BITS, i);
        }
    }
    else
    {
        for (i = 0; i < 0x20000 - l; i++)
        {
            if (TEST_BIT (d->WRAM_BITS, i) &&
                _C(cmp, _D(size, d->RAM, i), _D(size, d->CWRAM, i)))
            {
                if (update)
                    d->CWRAM [i] = d->RAM [i];
            }
            else
                BIT_CLEAR (d->WRAM_BITS, i);
        }
        
        for (i = 0; i < 0x10000 - l; i++)
        {
            if (TEST_BIT (d->SRAM_BITS, i) &&
                _C(cmp, _D(size, d->SRAM, i), _D(size, d->CSRAM, i)))
            {
                if (update)
                    d->CSRAM [i] = d->SRAM [i];
            }
            else
                BIT_CLEAR (d->SRAM_BITS, i);
        }
        
        for (i = 0; i < 0x2000 - l; i++)
        {
            if (TEST_BIT (d->IRAM_BITS, i) &&
                _C(cmp, _D(size, d->FillRAM + 0x3000, i), _D(size, d->CIRAM, i)))
            {
                if (update)
                    d->CIRAM [i] = d->FillRAM [i + 0x3000];
            }
            else
                BIT_CLEAR (d->IRAM_BITS, i);
        }
    }
}

void S9xSearchForValue (SCheatData *d, S9xCheatComparisonType cmp, 
                        S9xCheatDataSize size, uint32 value, 
                        bool8 is_signed, bool8 update)
{
    int l;

    switch (size)
    {
    case S9X_8_BITS: l = 0; break;
    case S9X_16_BITS: l = 1; break;
    case S9X_24_BITS: l = 2; break;
    default:
    case S9X_32_BITS: l = 3; break;
    }

    int i;

    if (is_signed)
    {
        for (i = 0; i < 0x20000 - l; i++)
        {
            if (TEST_BIT (d->WRAM_BITS, i) &&
                _C(cmp, _DS(size, d->RAM, i), (int32) value))
            {
                if (update)
                    d->CWRAM [i] = d->RAM [i];
            }
            else
                BIT_CLEAR (d->WRAM_BITS, i);
        }
        
        for (i = 0; i < 0x10000 - l; i++)
        {
            if (TEST_BIT (d->SRAM_BITS, i) &&
                _C(cmp, _DS(size, d->SRAM, i), (int32) value))
            {
                if (update)
                    d->CSRAM [i] = d->SRAM [i];
            }
            else
                BIT_CLEAR (d->SRAM_BITS, i);
        }
        
        for (i = 0; i < 0x2000 - l; i++)
        {
            if (TEST_BIT (d->IRAM_BITS, i) &&
                _C(cmp, _DS(size, d->FillRAM + 0x3000, i), (int32) value))
            {
                if (update)
                    d->CIRAM [i] = d->FillRAM [i + 0x3000];
            }
            else
                BIT_CLEAR (d->IRAM_BITS, i);
        }
    }
    else
    {
        for (i = 0; i < 0x20000 - l; i++)
        {
            if (TEST_BIT (d->WRAM_BITS, i) &&
                _C(cmp, _D(size, d->RAM, i), value))
            {
                if (update)
                    d->CWRAM [i] = d->RAM [i];
            }
            else
                BIT_CLEAR (d->WRAM_BITS, i);
        }
        
        for (i = 0; i < 0x10000 - l; i++)
        {
            if (TEST_BIT (d->SRAM_BITS, i) &&
                _C(cmp, _D(size, d->SRAM, i), value))
            {
                if (update)
                    d->CSRAM [i] = d->SRAM [i];
            }
            else
                BIT_CLEAR (d->SRAM_BITS, i);
        }
        
        for (i = 0; i < 0x2000 - l; i++)
        {
            if (TEST_BIT (d->IRAM_BITS, i) &&
                _C(cmp, _D(size, d->FillRAM + 0x3000, i), value))
            {
                if (update)
                    d->CIRAM [i] = d->FillRAM [i + 0x3000];
            }
            else
                BIT_CLEAR (d->IRAM_BITS, i);
        }
    }
}

void S9xOutputCheatSearchResults (SCheatData *d)
{
    int i;
    for (i = 0; i < 0x20000; i++)
    {
	if (TEST_BIT (d->WRAM_BITS, i))
	    printf ("WRAM: %05x: %02x\n", i, d->RAM [i]);
    }

    for (i = 0; i < 0x10000; i++)
    {
	if (TEST_BIT (d->SRAM_BITS, i))
	    printf ("SRAM: %04x: %02x\n", i, d->SRAM [i]);
    }

    for (i = 0; i < 0x2000; i++)
    {
	if (TEST_BIT (d->IRAM_BITS, i))
	    printf ("IRAM: %05x: %02x\n", i, d->FillRAM [i + 0x3000]);
    }
}

