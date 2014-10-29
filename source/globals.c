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
#include "dsp1.h"
#include "missing.h"
#include "cpuexec.h"
#include "apu.h"
#include "dma.h"
#include "fxemu.h"
#include "gfx.h"
#include "soundux.h"
#include "cheats.h"
#include "sa1.h"
#include "spc7110.h"

char String[513];

struct Missing missing;

SICPU ICPU;

SCPUState CPU;

SAPU APU;

SIAPU IAPU;

SSettings Settings;

SDSP1 DSP1;

SSA1 SA1;

SSoundData SoundData;

SnesModel M1SNES = {1, 3, 2};
SnesModel M2SNES = {2, 4, 3};
SnesModel* Model = &M1SNES;


uint8* C4RAM = NULL;

long OpAddress = 0;

CMemory Memory;

SSNESGameFixes SNESGameFixes;

uint8 OpenBus = 0;

#ifndef ZSNES_FX
struct FxInit_s SuperFX;
#else
uint8* SFXPlotTable = NULL;
#endif

SPPU PPU;
InternalPPU IPPU;

SDMA DMA[8];

uint8* HDMAMemPointers [8];
uint8* HDMABasePointers [8];

SBG BG;

struct SGFX GFX;
struct SLineData LineData[240];
struct SLineMatrixData LineMatrixData [240];

uint8 Mode7Depths [2];
NormalTileRenderer DrawTilePtr = NULL;
ClippedTileRenderer DrawClippedTilePtr = NULL;
NormalTileRenderer DrawHiResTilePtr = NULL;
ClippedTileRenderer DrawHiResClippedTilePtr = NULL;
LargePixelRenderer DrawLargePixelPtr = NULL;

uint32 odd_high[4][16];
uint32 odd_low[4][16];
uint32 even_high[4][16];
uint32 even_low[4][16];

SCheatData Cheat;

SoundStatus so;

int Echo [24000];
int DummyEchoBuffer [SOUND_BUFFER_SIZE];
int MixBuffer [SOUND_BUFFER_SIZE];
int EchoBuffer [SOUND_BUFFER_SIZE];
int FilterTaps [8];
unsigned long Z = 0;
int Loop [16];

uint16 SignExtend [2] =
{
   0x00, 0xff00
};

//modified per anomie Mode 5 findings
int HDMA_ModeByteCounts [8] =
{
   1, 2, 2, 4, 4, 4, 2, 4
};

uint8 BitShifts[8][4] =
{
   {2, 2, 2, 2}, // 0
   {4, 4, 2, 0}, // 1
   {4, 4, 0, 0}, // 2
   {8, 4, 0, 0}, // 3
   {8, 2, 0, 0}, // 4
   {4, 2, 0, 0}, // 5
   {4, 0, 0, 0}, // 6
   {8, 0, 0, 0}  // 7
};
uint8 TileShifts[8][4] =
{
   {4, 4, 4, 4}, // 0
   {5, 5, 4, 0}, // 1
   {5, 5, 0, 0}, // 2
   {6, 5, 0, 0}, // 3
   {6, 4, 0, 0}, // 4
   {5, 4, 0, 0}, // 5
   {5, 0, 0, 0}, // 6
   {6, 0, 0, 0}  // 7
};
uint8 PaletteShifts[8][4] =
{
   {2, 2, 2, 2}, // 0
   {4, 4, 2, 0}, // 1
   {4, 4, 0, 0}, // 2
   {0, 4, 0, 0}, // 3
   {0, 2, 0, 0}, // 4
   {4, 2, 0, 0}, // 5
   {4, 0, 0, 0}, // 6
   {0, 0, 0, 0}  // 7
};
uint8 PaletteMasks[8][4] =
{
   {7, 7, 7, 7}, // 0
   {7, 7, 7, 0}, // 1
   {7, 7, 0, 0}, // 2
   {0, 7, 0, 0}, // 3
   {0, 7, 0, 0}, // 4
   {7, 7, 0, 0}, // 5
   {7, 0, 0, 0}, // 6
   {0, 0, 0, 0}  // 7
};
uint8 Depths[8][4] =
{
   {TILE_2BIT, TILE_2BIT, TILE_2BIT, TILE_2BIT}, // 0
   {TILE_4BIT, TILE_4BIT, TILE_2BIT, 0},         // 1
   {TILE_4BIT, TILE_4BIT, 0, 0},                 // 2
   {TILE_8BIT, TILE_4BIT, 0, 0},                 // 3
   {TILE_8BIT, TILE_2BIT, 0, 0},                 // 4
   {TILE_4BIT, TILE_2BIT, 0, 0},                 // 5
   {TILE_4BIT, 0, 0, 0},                         // 6
   {0, 0, 0, 0}                                  // 7
};
uint8 BGSizes [2] =
{
   8, 16
};
uint16 DirectColourMaps [8][256];

long FilterValues[4][2] =
{
   {0, 0},
   {240, 0},
   {488, -240},
   {460, -208}
};

int NoiseFreq [32] =
{
   0, 16, 21, 25, 31, 42, 50, 63, 84, 100, 125, 167, 200, 250, 333,
   400, 500, 667, 800, 1000, 1300, 1600, 2000, 2700, 3200, 4000,
   5300, 6400, 8000, 10700, 16000, 32000
};

uint32 HeadMask [4] =
{
#ifdef LSB_FIRST
   0xffffffff, 0xffffff00, 0xffff0000, 0xff000000
#else
   0xffffffff, 0x00ffffff, 0x0000ffff, 0x000000ff
#endif
};

uint32 TailMask [5] =
{
#ifdef LSB_FIRST
   0x00000000, 0x000000ff, 0x0000ffff, 0x00ffffff, 0xffffffff
#else
   0x00000000, 0xff000000, 0xffff0000, 0xffffff00, 0xffffffff
#endif
};

uint8 APUROM [64] =
{
   0xCD, 0xEF, 0xBD, 0xE8, 0x00, 0xC6, 0x1D, 0xD0, 0xFC, 0x8F, 0xAA, 0xF4, 0x8F,
   0xBB, 0xF5, 0x78, 0xCC, 0xF4, 0xD0, 0xFB, 0x2F, 0x19, 0xEB, 0xF4, 0xD0, 0xFC,
   0x7E, 0xF4, 0xD0, 0x0B, 0xE4, 0xF5, 0xCB, 0xF4, 0xD7, 0x00, 0xFC, 0xD0, 0xF3,
   0xAB, 0x01, 0x10, 0xEF, 0x7E, 0xF4, 0x10, 0xEB, 0xBA, 0xF6, 0xDA, 0x00, 0xBA,
   0xF4, 0xC4, 0xF4, 0xDD, 0x5D, 0xD0, 0xDB, 0x1F, 0x00, 0x00, 0xC0, 0xFF
};


// Raw SPC700 instruction cycle lengths
uint16 S9xAPUCycleLengths [256] =
{
   /*        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, a, b, c, d, e, f, */
   /* 00 */  2, 8, 4, 5, 3, 4, 3, 6, 2, 6, 5, 4, 5, 4, 6, 8,
   /* 10 */  2, 8, 4, 5, 4, 5, 5, 6, 5, 5, 6, 5, 2, 2, 4, 6,
   /* 20 */  2, 8, 4, 5, 3, 4, 3, 6, 2, 6, 5, 4, 5, 4, 5, 4,
   /* 30 */  2, 8, 4, 5, 4, 5, 5, 6, 5, 5, 6, 5, 2, 2, 3, 8,
   /* 40 */  2, 8, 4, 5, 3, 4, 3, 6, 2, 6, 4, 4, 5, 4, 6, 6,
   /* 50 */  2, 8, 4, 5, 4, 5, 5, 6, 5, 5, 4, 5, 2, 2, 4, 3,
   /* 60 */  2, 8, 4, 5, 3, 4, 3, 6, 2, 6, 4, 4, 5, 4, 5, 5,
   /* 70 */  2, 8, 4, 5, 4, 5, 5, 6, 5, 5, 5, 5, 2, 2, 3, 6,
   /* 80 */  2, 8, 4, 5, 3, 4, 3, 6, 2, 6, 5, 4, 5, 2, 4, 5,
   /* 90 */  2, 8, 4, 5, 4, 5, 5, 6, 5, 5, 5, 5, 2, 2, 12, 5,
   /* a0 */  3, 8, 4, 5, 3, 4, 3, 6, 2, 6, 4, 4, 5, 2, 4, 4,
   /* b0 */  2, 8, 4, 5, 4, 5, 5, 6, 5, 5, 5, 5, 2, 2, 3, 4,
   /* c0 */  3, 8, 4, 5, 4, 5, 4, 7, 2, 5, 6, 4, 5, 2, 4, 9,
   /* d0 */  2, 8, 4, 5, 5, 6, 6, 7, 4, 5, 4, 5, 2, 2, 6, 3,
   /* e0 */  2, 8, 4, 5, 3, 4, 3, 6, 2, 4, 5, 3, 4, 3, 4, 3,
   /* f0 */  2, 8, 4, 5, 4, 5, 5, 6, 3, 4, 5, 4, 2, 2, 4, 3
};

// Actual data used by CPU emulation, will be scaled by APUReset routine
// to be relative to the 65c816 instruction lengths.
uint16 S9xAPUCycles [256] =
{
   /*        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, a, b, c, d, e, f, */
   /* 00 */  2, 8, 4, 5, 3, 4, 3, 6, 2, 6, 5, 4, 5, 4, 6, 8,
   /* 10 */  2, 8, 4, 5, 4, 5, 5, 6, 5, 5, 6, 5, 2, 2, 4, 6,
   /* 20 */  2, 8, 4, 5, 3, 4, 3, 6, 2, 6, 5, 4, 5, 4, 5, 4,
   /* 30 */  2, 8, 4, 5, 4, 5, 5, 6, 5, 5, 6, 5, 2, 2, 3, 8,
   /* 40 */  2, 8, 4, 5, 3, 4, 3, 6, 2, 6, 4, 4, 5, 4, 6, 6,
   /* 50 */  2, 8, 4, 5, 4, 5, 5, 6, 5, 5, 4, 5, 2, 2, 4, 3,
   /* 60 */  2, 8, 4, 5, 3, 4, 3, 6, 2, 6, 4, 4, 5, 4, 5, 5,
   /* 70 */  2, 8, 4, 5, 4, 5, 5, 6, 5, 5, 5, 5, 2, 2, 3, 6,
   /* 80 */  2, 8, 4, 5, 3, 4, 3, 6, 2, 6, 5, 4, 5, 2, 4, 5,
   /* 90 */  2, 8, 4, 5, 4, 5, 5, 6, 5, 5, 5, 5, 2, 2, 12, 5,
   /* a0 */  3, 8, 4, 5, 3, 4, 3, 6, 2, 6, 4, 4, 5, 2, 4, 4,
   /* b0 */  2, 8, 4, 5, 4, 5, 5, 6, 5, 5, 5, 5, 2, 2, 3, 4,
   /* c0 */  3, 8, 4, 5, 4, 5, 4, 7, 2, 5, 6, 4, 5, 2, 4, 9,
   /* d0 */  2, 8, 4, 5, 5, 6, 6, 7, 4, 5, 4, 5, 2, 2, 6, 3,
   /* e0 */  2, 8, 4, 5, 3, 4, 3, 6, 2, 4, 5, 3, 4, 3, 4, 3,
   /* f0 */  2, 8, 4, 5, 4, 5, 5, 6, 3, 4, 5, 4, 2, 2, 4, 3
};
