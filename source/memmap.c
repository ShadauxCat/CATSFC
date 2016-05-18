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

#include <string.h>
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#include <ctype.h>

#ifdef __linux
#include <unistd.h>
#endif

#include "snes9x.h"
#include "memmap.h"
#include "cpuexec.h"
#include "ppu.h"
#include "display.h"
#include "cheats.h"
#include "apu.h"
#include "sa1.h"
#include "dsp1.h"
#include "srtc.h"
#include "sdd1.h"
#include "spc7110.h"
#include "seta.h"

#ifdef DS2_DMA
//#include "ds2_cpu.h"
//#include "ds2_dma.h"
//#include "dma_adj.h"
#endif

#ifdef __W32_HEAP
#include <malloc.h>
#endif

#include "fxemu.h"
extern struct FxInit_s SuperFX;

#ifndef SET_UI_COLOR
#define SET_UI_COLOR(r,g,b) ;
#endif

//you would think everyone would have these
//since they're so useful.
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

static int retry_count = 0;
static uint8_t bytes0x2000 [0x2000];
int is_bsx(unsigned char*);
int bs_name(unsigned char*);
int check_char(unsigned);
void S9xDeinterleaveType2(bool reset);
uint32_t caCRC32(uint8_t* array, uint32_t size, register uint32_t crc32);

extern char* rom_filename;

const uint32_t crc32Table[256] =
{
   0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
   0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
   0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
   0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
   0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
   0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
   0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
   0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
   0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
   0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
   0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
   0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
   0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
   0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
   0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
   0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
   0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
   0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
   0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
   0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
   0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
   0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
   0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
   0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
   0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
   0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
   0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
   0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
   0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
   0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
   0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
   0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
   0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
   0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
   0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
   0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
   0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
   0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
   0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
   0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
   0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
   0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
   0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};



void S9xDeinterleaveType1(int TotalFileSize, uint8_t* base)
{
   if (Settings.DisplayColor == 0xffff)
   {
      Settings.DisplayColor = BUILD_PIXEL(0, 31, 0);
      SET_UI_COLOR(0, 255, 0);
   }

   int i;
   int nblocks = TotalFileSize >> 16;
   uint8_t blocks [256];
   for (i = 0; i < nblocks; i++)
   {
      blocks [i * 2] = i + nblocks;
      blocks [i * 2 + 1] = i;
   }
   // DS2 DMA notes: base may or may not be 32-byte aligned
   uint8_t* tmp = (uint8_t*) malloc(0x8000);
   if (tmp)
   {
      for (i = 0; i < nblocks * 2; i++)
      {
         int j;
         for (j = i; j < nblocks * 2; j++)
         {
            if (blocks [j] == i)
            {
               // memmove converted: Different mallocs [Neb]
               memcpy(tmp, &base [blocks [j] * 0x8000], 0x8000);
               // memmove converted: Different addresses, or identical for blocks[i] == blocks[j] [Neb]
               // DS2 DMA notes: Don't do DMA at all if blocks[i] == blocks[j]
               memcpy(&base [blocks [j] * 0x8000],
                      &base [blocks [i] * 0x8000], 0x8000);
               // memmove converted: Different mallocs [Neb]
               memcpy(&base [blocks [i] * 0x8000], tmp, 0x8000);
               uint8_t b = blocks [j];
               blocks [j] = blocks [i];
               blocks [i] = b;
               break;
            }
         }
      }
      free((char*) tmp);
   }
}

void S9xDeinterleaveGD24(int TotalFileSize, uint8_t* base)
{

   if (TotalFileSize != 0x300000)
      return;

   if (Settings.DisplayColor == 0xffff)
   {
      Settings.DisplayColor = BUILD_PIXEL(0, 31, 31);
      SET_UI_COLOR(0, 255, 255);
   }

   // DS2 DMA notes: base may or may not be 32-byte aligned
   uint8_t* tmp = (uint8_t*) malloc(0x80000);
   if (tmp)
   {
      // memmove converted: Different mallocs [Neb]
      memcpy(tmp, &base[0x180000], 0x80000);
      // memmove converted: Different addresses [Neb]
      memcpy(&base[0x180000], &base[0x200000], 0x80000);
      // memmove converted: Different addresses [Neb]
      memcpy(&base[0x200000], &base[0x280000], 0x80000);
      // memmove converted: Different mallocs [Neb]
      memcpy(&base[0x280000], tmp, 0x80000);
      free((char*) tmp);

      S9xDeinterleaveType1(TotalFileSize, base);
   }
}

bool AllASCII(uint8_t* b, int size)
{
   int i;
   for (i = 0; i < size; i++)
   {
      if (b[i] < 32 || b[i] > 126)
         return (false);
   }
   return (true);
}

int ScoreHiROM(bool skip_header, int32_t romoff)
{
   int score = 0;
   int o = skip_header ? 0xff00 + 0x200 : 0xff00;

   o += romoff;

   if (Memory.ROM [o + 0xd5] & 0x1)
      score += 2;

   //Mode23 is SA-1
   if (Memory.ROM [o + 0xd5] == 0x23)
      score -= 2;

   if (Memory.ROM [o + 0xd4] == 0x20)
      score += 2;

   if ((Memory.ROM [o + 0xdc] + (Memory.ROM [o + 0xdd] << 8) +
         Memory.ROM [o + 0xde] + (Memory.ROM [o + 0xdf] << 8)) == 0xffff)
   {
      score += 2;
      if (0 != (Memory.ROM [o + 0xde] + (Memory.ROM [o + 0xdf] << 8)))
         score++;
   }

   if (Memory.ROM [o + 0xda] == 0x33)
      score += 2;
   if ((Memory.ROM [o + 0xd5] & 0xf) < 4)
      score += 2;
   if (!(Memory.ROM [o + 0xfd] & 0x80))
      score -= 6;
   if ((Memory.ROM [o + 0xfc] | (Memory.ROM [o + 0xfd] << 8)) > 0xFFB0)
      score -= 2; //reduced after looking at a scan by Cowering
   if (Memory.CalculatedSize > 1024 * 1024 * 3)
      score += 4;
   if ((1 << (Memory.ROM [o + 0xd7] - 7)) > 48)
      score -= 1;
   if (!AllASCII(&Memory.ROM [o + 0xb0], 6))
      score -= 1;
   if (!AllASCII(&Memory.ROM [o + 0xc0], ROM_NAME_LEN - 1))
      score -= 1;

   return (score);
}

int ScoreLoROM(bool skip_header, int32_t romoff)
{
   int score = 0;
   int o = skip_header ? 0x7f00 + 0x200 : 0x7f00;

   o += romoff;

   if (!(Memory.ROM [o + 0xd5] & 0x1))
      score += 3;

   //Mode23 is SA-1
   if (Memory.ROM [o + 0xd5] == 0x23)
      score += 2;

   if ((Memory.ROM [o + 0xdc] + (Memory.ROM [o + 0xdd] << 8) +
         Memory.ROM [o + 0xde] + (Memory.ROM [o + 0xdf] << 8)) == 0xffff)
   {
      score += 2;
      if (0 != (Memory.ROM [o + 0xde] + (Memory.ROM [o + 0xdf] << 8)))
         score++;
   }

   if (Memory.ROM [o + 0xda] == 0x33)
      score += 2;
   if ((Memory.ROM [o + 0xd5] & 0xf) < 4)
      score += 2;
   if (Memory.CalculatedSize <= 1024 * 1024 * 16)
      score += 2;
   if (!(Memory.ROM [o + 0xfd] & 0x80))
      score -= 6;
   if ((Memory.ROM [o + 0xfc] | (Memory.ROM [o + 0xfd] << 8)) > 0xFFB0)
      score -= 2;//reduced per Cowering suggestion
   if ((1 << (Memory.ROM [o + 0xd7] - 7)) > 48)
      score -= 1;
   if (!AllASCII(&Memory.ROM [o + 0xb0], 6))
      score -= 1;
   if (!AllASCII(&Memory.ROM [o + 0xc0], ROM_NAME_LEN - 1))
      score -= 1;

   return (score);
}

char* Safe(const char* s)
{
   static char* safe;
   static int safe_len = 0;

   if (s == NULL)
   {
      if (safe != NULL)
      {
         free((char*)safe);
         safe = NULL;
      }
      return NULL;
   }
   int len = strlen(s);
   if (!safe || len + 1 > safe_len)
   {
      if (safe)
         free((char*) safe);
      safe = (char*) malloc(safe_len = len + 1);
   }

   int i;
   for (i = 0; i < len; i++)
   {
      if (s [i] >= 32 && s [i] < 127)
         safe [i] = s[i];
      else
         safe [i] = '?';
   }
   safe [len] = 0;
   return (safe);
}

/**********************************************************************************************/
/* S9xInitMemory()                                                                                     */
/* This function allocates and zeroes all the memory needed by the emulator                   */
/**********************************************************************************************/
bool S9xInitMemory()
{
   // DS2 DMA notes: These would do well to be allocated with 32 extra bytes
   // so they can be 32-byte aligned. [Neb]
   Memory.RAM     = (uint8_t*) malloc(0x20000);
   Memory.SRAM    = (uint8_t*) malloc(0x20000);
   Memory.VRAM    = (uint8_t*) malloc(0x10000);
#ifdef DS2_DMA
   ROM     = (uint8_t*) AlignedMalloc(MAX_ROM_SIZE + 0x200 + 0x8000, 32,
                                    &PtrAdj.ROM);
#else
   Memory.ROM     = (uint8_t*) malloc(MAX_ROM_SIZE + 0x200 + 0x8000);
#endif
   memset(Memory.RAM, 0, 0x20000);
   memset(Memory.SRAM, 0, 0x20000);
   memset(Memory.VRAM, 0, 0x10000);
   // This needs to be initialised with a ROM first anyway, so don't
   // bother memsetting. [Neb]
   // memset (ROM, 0, MAX_ROM_SIZE + 0x200 + 0x8000);

   Memory.BSRAM   = (uint8_t*) malloc(0x80000);
   memset(Memory.BSRAM, 0, 0x80000);

   Memory.FillRAM = NULL;

   IPPU.TileCache [TILE_2BIT] = (uint8_t*) malloc(MAX_2BIT_TILES * 128);
   IPPU.TileCache [TILE_4BIT] = (uint8_t*) malloc(MAX_4BIT_TILES * 128);
   IPPU.TileCache [TILE_8BIT] = (uint8_t*) malloc(MAX_8BIT_TILES * 128);

   IPPU.TileCached [TILE_2BIT] = (uint8_t*) malloc(MAX_2BIT_TILES);
   IPPU.TileCached [TILE_4BIT] = (uint8_t*) malloc(MAX_4BIT_TILES);
   IPPU.TileCached [TILE_8BIT] = (uint8_t*) malloc(MAX_8BIT_TILES);

   if (!Memory.RAM || !Memory.SRAM || !Memory.VRAM || !Memory.ROM || !Memory.BSRAM
         ||
         !IPPU.TileCache [TILE_2BIT] || !IPPU.TileCache [TILE_4BIT] ||
         !IPPU.TileCache [TILE_8BIT] || !IPPU.TileCached [TILE_2BIT] ||
         !IPPU.TileCached [TILE_4BIT] ||  !IPPU.TileCached [TILE_8BIT])
   {
      S9xDeinitMemory();
      return (false);
   }

   // FillRAM uses first 32K of ROM image area, otherwise space just
   // wasted. Might be read by the SuperFX code.

   Memory.FillRAM = Memory.ROM;

   // Add 0x8000 to ROM image pointer to stop SuperFX code accessing
   // unallocated memory (can cause crash on some ports).
   Memory.ROM += 0x8000;  // still 32-byte aligned

   Memory.C4RAM    = Memory.ROM + 0x400000 + 8192 * 8;  // still 32-byte aligned
   Memory.ROM    = Memory.ROM;
   Memory.SRAM   = Memory.SRAM;

   SuperFX.pvRegisters = &Memory.FillRAM [0x3000];
   SuperFX.nRamBanks = 2; // Most only use 1.  1=64KB, 2=128KB=1024Mb
   SuperFX.pvRam = Memory.SRAM;
   SuperFX.nRomBanks = (2 * 1024 * 1024) / (32 * 1024);
   SuperFX.pvRom = (uint8_t*) Memory.ROM;

   memset(IPPU.TileCache [TILE_2BIT], 0, MAX_2BIT_TILES * 128);
   memset(IPPU.TileCache [TILE_4BIT], 0, MAX_4BIT_TILES * 128);
   memset(IPPU.TileCache [TILE_8BIT], 0, MAX_8BIT_TILES * 128);

   memset(IPPU.TileCached [TILE_2BIT], 0, MAX_2BIT_TILES);
   memset(IPPU.TileCached [TILE_4BIT], 0, MAX_4BIT_TILES);
   memset(IPPU.TileCached [TILE_8BIT], 0, MAX_8BIT_TILES);

   Memory.SDD1Data = NULL;
   Memory.SDD1Index = NULL;

   return (true);
}

void S9xDeinitMemory()
{
#ifdef __W32_HEAP
   if (_HEAPOK != _heapchk())
      MessageBox(GUI.hWnd, "Deinit", "Heap Corrupt", MB_OK);
#endif

   if (Memory.RAM)
   {
      free((char*) Memory.RAM);
      Memory.RAM = NULL;
   }
   if (Memory.SRAM)
   {
      free((char*) Memory.SRAM);
      Memory.SRAM = NULL;
   }
   if (Memory.VRAM)
   {
      free((char*) Memory.VRAM);
      Memory.VRAM = NULL;
   }
   if (Memory.ROM)
   {
      Memory.ROM -= 0x8000;
#ifdef DS2_RAM
      AlignedFree((char*) ROM, PtrAdj.ROM);
#else
      free((char*) Memory.ROM);
#endif
      Memory.ROM = NULL;
   }

   if (Memory.BSRAM)
   {
      free((char*) Memory.BSRAM);
      Memory.BSRAM = NULL;
   }

   if (IPPU.TileCache [TILE_2BIT])
   {
      free((char*) IPPU.TileCache [TILE_2BIT]);
      IPPU.TileCache [TILE_2BIT] = NULL;
   }
   if (IPPU.TileCache [TILE_4BIT])
   {
      free((char*) IPPU.TileCache [TILE_4BIT]);
      IPPU.TileCache [TILE_4BIT] = NULL;
   }
   if (IPPU.TileCache [TILE_8BIT])
   {
      free((char*) IPPU.TileCache [TILE_8BIT]);
      IPPU.TileCache [TILE_8BIT] = NULL;
   }

   if (IPPU.TileCached [TILE_2BIT])
   {
      free((char*) IPPU.TileCached [TILE_2BIT]);
      IPPU.TileCached [TILE_2BIT] = NULL;
   }
   if (IPPU.TileCached [TILE_4BIT])
   {
      free((char*) IPPU.TileCached [TILE_4BIT]);
      IPPU.TileCached [TILE_4BIT] = NULL;
   }
   if (IPPU.TileCached [TILE_8BIT])
   {
      free((char*) IPPU.TileCached [TILE_8BIT]);
      IPPU.TileCached [TILE_8BIT] = NULL;
   }
   FreeSDD1Data();
   Safe(NULL);
}

void FreeSDD1Data()
{
   if (Memory.SDD1Index)
   {
      free((char*) Memory.SDD1Index);
      Memory.SDD1Index = NULL;
   }
   if (Memory.SDD1Data)
   {
      free((char*) Memory.SDD1Data);
      Memory.SDD1Data = NULL;
   }
}

/**********************************************************************************************/
/* LoadROM()                                                                                  */
/* This function loads a Snes-Backup image                                                    */
/**********************************************************************************************/

#ifdef LOAD_FROM_MEMORY_TEST
bool LoadROM(const struct retro_game_info* game)
#else
bool LoadROM(const char* filename)
#endif
{
   int32_t TotalFileSize = 0;
   bool Interleaved = false;
   bool Tales = false;

   uint8_t* RomHeader = Memory.ROM;

   Memory.ExtendedFormat = NOPE;


   if (CleanUp7110 != NULL)
      (*CleanUp7110)();

   memset(&SNESGameFixes, 0, sizeof(SNESGameFixes));
   SNESGameFixes.SRAMInitialValue = 0x60;

   memset(bytes0x2000, 0, 0x2000);
   CPU.TriedInterleavedMode2 = false;

   Memory.CalculatedSize = 0;
   retry_count = 0;

again:
   Settings.DisplayColor = 0xffff;
   SET_UI_COLOR(255, 255, 255);

#ifdef LOAD_FROM_MEMORY_TEST
   strncpy(Memory.ROMFilename, game->path, sizeof(Memory.ROMFilename));

   Memory.HeaderCount = 0;
   TotalFileSize = game->size;
   const uint8_t* src = game->data;
   Memory.HeaderCount = 0;

   if ((((game->size & 0x1FFF) == 0x200) && !Settings.ForceNoHeader)
         || Settings.ForceHeader)
   {
      S9xMessage(S9X_INFO, S9X_HEADERS_INFO,
                 "Found ROM file header (and ignored it).");
      TotalFileSize -= 0x200;
      src      += 0x200;
      Memory.HeaderCount = 1;

   }
   else
   {
      S9xMessage(S9X_INFO, S9X_HEADERS_INFO, "No ROM file header found.");
   }
   if (TotalFileSize > MAX_ROM_SIZE)
      return false;

   memcpy(Memory.ROM, src, TotalFileSize);

#else
   TotalFileSize = FileLoader(Memory.ROM, filename, MAX_ROM_SIZE);

   if (!TotalFileSize)
      return false;     // it ends here
   else if (!Settings.NoPatch)
      CheckForIPSPatch(filename, Memory.HeaderCount != 0, &TotalFileSize);
#endif
   //fix hacked games here.
   if ((strncmp("HONKAKUHA IGO GOSEI", (char*)&Memory.ROM[0x7FC0], 19) == 0)
         && (Memory.ROM[0x7FD5] != 0x31))
   {
      Memory.ROM[0x7FD5] = 0x31;
      Memory.ROM[0x7FD6] = 0x02;
      Settings.DisplayColor = BUILD_PIXEL(31, 0, 0);
      SET_UI_COLOR(255, 0, 0);
      S9xMessage(S9X_ERROR, S9X_ROM_CONFUSING_FORMAT_INFO, "Warning! Hacked Dump!");
   }

   if ((strncmp("HONKAKUHA IGO GOSEI", (char*)&Memory.ROM[0xFFC0], 19) == 0)
         && (Memory.ROM[0xFFD5] != 0x31))
   {
      Memory.ROM[0xFFD5] = 0x31;
      Memory.ROM[0xFFD6] = 0x02;
      Settings.DisplayColor = BUILD_PIXEL(31, 0, 0);
      SET_UI_COLOR(255, 0, 0);
      S9xMessage(S9X_ERROR, S9X_ROM_CONFUSING_FORMAT_INFO, "Warning! Hacked Dump!");
   }

   if ((Memory.ROM[0x7FD5] == 0x42) && (Memory.ROM[0x7FD6] == 0x13)
         && (strncmp("METAL COMBAT", (char*)&Memory.ROM[0x7FC0], 12) == 0))
   {
      Settings.DisplayColor = BUILD_PIXEL(31, 0, 0);
      SET_UI_COLOR(255, 0, 0);
      S9xMessage(S9X_ERROR, S9X_ROM_CONFUSING_FORMAT_INFO, "Warning! Hacked Dump!");
   }

   int hi_score=ScoreHiROM(true, 0);
   int lo_score=ScoreLoROM(true, 0);

   if (Memory.HeaderCount == 0 && !Settings.ForceNoHeader &&
         ((hi_score > lo_score && ScoreHiROM(true, 0) > hi_score) ||
          (hi_score <= lo_score && ScoreLoROM(true, 0) > lo_score)))
   {
#ifdef DS2_DMA
      __dcache_writeback_all();
      {
         unsigned int i;
         for (i = 0; i < TotalFileSize; i += 512)
         {
            ds2_DMAcopy_32Byte(2 /* channel: emu internal */, Memory.ROM + i,
                               Memory.ROM + i + 512, 512);
            ds2_DMA_wait(2);
            ds2_DMA_stop(2);
         }
      }
#else
      // memmove required: Overlapping addresses [Neb]
      memmove(Memory.ROM, Memory.ROM + 512, TotalFileSize - 512);
#endif
      TotalFileSize -= 512;
      S9xMessage(S9X_INFO, S9X_HEADER_WARNING,
                 "Try specifying the -nhd command line option if the game doesn't work\n");
      //modifying ROM, so we need to rescore
      hi_score = ScoreHiROM(false, 0);
      lo_score = ScoreLoROM(false, 0);
   }

   Memory.CalculatedSize = TotalFileSize & ~0x1FFF; // round down to lower 0x2000
   memset(Memory.ROM + Memory.CalculatedSize, 0,
              MAX_ROM_SIZE - Memory.CalculatedSize);

   if (Memory.CalculatedSize > 0x400000 &&
         !(Memory.ROM[0x7FD5] == 0x32 && ((Memory.ROM[0x7FD6] & 0xF0) == 0x40))
         && //exclude S-DD1
         !(Memory.ROM[0xFFD5] == 0x3A
           && ((Memory.ROM[0xFFD6] & 0xF0) == 0xF0))) //exclude SPC7110
   {
      //you might be a Jumbo!
      Memory.ExtendedFormat = YEAH;
   }

   //If both vectors are invalid, it's type 1 LoROM

   if (Memory.ExtendedFormat == NOPE
         && ((Memory.ROM[0x7FFC] | (Memory.ROM[0x7FFD] << 8)) < 0x8000)
         && ((Memory.ROM[0xFFFC] | (Memory.ROM[0xFFFD] << 8)) < 0x8000))
   {
      if (Settings.DisplayColor == 0xffff)
      {
         Settings.DisplayColor = BUILD_PIXEL(0, 31, 0);
         SET_UI_COLOR(0, 255, 0);
      }
      if (!Settings.ForceInterleaved)
         S9xDeinterleaveType1(TotalFileSize, Memory.ROM);
   }

   //CalculatedSize is now set, so rescore
   hi_score = ScoreHiROM(false, 0);
   lo_score = ScoreLoROM(false, 0);

   if (Memory.ExtendedFormat != NOPE)
   {
      int loromscore, hiromscore, swappedlorom, swappedhirom;
      loromscore = ScoreLoROM(false, 0);
      hiromscore = ScoreHiROM(false, 0);
      swappedlorom = ScoreLoROM(false, 0x400000);
      swappedhirom = ScoreHiROM(false, 0x400000);

      //set swapped here.

      if (max(swappedlorom, swappedhirom) >= max(loromscore, hiromscore))
      {
         Memory.ExtendedFormat = BIGFIRST;
         hi_score = swappedhirom;
         lo_score = swappedlorom;
         RomHeader = Memory.ROM + 0x400000;
      }
      else
      {
         Memory.ExtendedFormat = SMALLFIRST;
         lo_score = loromscore;
         hi_score = hiromscore;
         RomHeader = Memory.ROM;
      }


   }

   Interleaved = Settings.ForceInterleaved || Settings.ForceInterleaved2;
   if (Settings.ForceLoROM || (!Settings.ForceHiROM && lo_score >= hi_score))
   {
      Memory.LoROM = true;
      Memory.HiROM = false;

      // Ignore map type byte if not 0x2x or 0x3x
      if ((RomHeader [0x7fd5] & 0xf0) == 0x20 || (RomHeader [0x7fd5] & 0xf0) == 0x30)
      {
         switch (RomHeader [0x7fd5] & 0xf)
         {
         case 1:
            Interleaved = true;
            break;
         case 5:
            Interleaved = true;
            Tales = true;
            break;
         }
      }
   }
   else
   {
      if ((RomHeader [0xffd5] & 0xf0) == 0x20 || (RomHeader [0xffd5] & 0xf0) == 0x30)
      {
         switch (RomHeader [0xffd5] & 0xf)
         {
         case 0:
         case 3:
            Interleaved = true;
            break;
         }
      }
      Memory.LoROM = false;
      Memory.HiROM = true;
   }

   // More
   if (!Settings.ForceHiROM && !Settings.ForceLoROM &&
         !Settings.ForceInterleaved && !Settings.ForceInterleaved2 &&
         !Settings.ForceNotInterleaved && !Settings.ForcePAL &&
         !Settings.ForceSuperFX && !Settings.ForceDSP1 &&
         !Settings.ForceSA1 && !Settings.ForceC4 &&
         !Settings.ForceSDD1)
   {


#ifdef DETECT_NASTY_FX_INTERLEAVE
      //MK: Damn. YI trips a BRK currently. Maybe even on a real cart.

#ifdef MSB_FIRST
      if (strncmp((char*) &ROM [0x7fc0], "YOSHI'S ISLAND", 14) == 0
            && (ROM[0x7FDE] + (ROM[0x7FDF] << 8)) == 57611 && ROM[0x10002] == 0xA9)
#else
      if (strncmp((char*) &ROM [0x7fc0], "YOSHI'S ISLAND", 14) == 0
            && (*(uint16_t*)&ROM[0x7FDE]) == 57611 && ROM[0x10002] == 0xA9)
#endif
      {
         Interleaved = true;
         Settings.ForceInterleaved2 = true;
      }
#endif
      if (strncmp((char*) &Memory.ROM [0x7fc0], "YUYU NO QUIZ DE GO!GO!", 22) == 0)
      {
         Memory.LoROM = true;
         Memory.HiROM = false;
         Interleaved = false;
      }
   }

   if (!Settings.ForceNotInterleaved && Interleaved)
   {
      CPU.TriedInterleavedMode2 = true;
      S9xMessage(S9X_INFO, S9X_ROM_INTERLEAVED_INFO,
                 "ROM image is in interleaved format - converting...");

      if (Tales)
      {
         if (Memory.ExtendedFormat == BIGFIRST)
         {
            S9xDeinterleaveType1(0x400000, Memory.ROM);
            S9xDeinterleaveType1(Memory.CalculatedSize - 0x400000, Memory.ROM + 0x400000);
         }
         else
         {
            S9xDeinterleaveType1(Memory.CalculatedSize - 0x400000, Memory.ROM);
            S9xDeinterleaveType1(0x400000, Memory.ROM + Memory.CalculatedSize - 0x400000);

         }

         Memory.LoROM = false;
         Memory.HiROM = true;


      }
      else if (Settings.ForceInterleaved2)
         S9xDeinterleaveType2(false);
      else if (Settings.ForceInterleaveGD24 && Memory.CalculatedSize == 0x300000)
      {
         bool t = Memory.LoROM;

         Memory.LoROM = Memory.HiROM;
         Memory.HiROM = t;
         S9xDeinterleaveGD24(Memory.CalculatedSize, Memory.ROM);
      }
      else
      {
         if (Settings.DisplayColor == 0xffff)
         {
            Settings.DisplayColor = BUILD_PIXEL(0, 31, 0);
            SET_UI_COLOR(0, 255, 0);
         }
         bool t = Memory.LoROM;

         Memory.LoROM = Memory.HiROM;
         Memory.HiROM = t;

         S9xDeinterleaveType1(Memory.CalculatedSize, Memory.ROM);
      }

      hi_score = ScoreHiROM(false, 0);
      lo_score = ScoreLoROM(false, 0);

      if ((Memory.HiROM &&
            (lo_score >= hi_score || hi_score < 0)) ||
            (Memory.LoROM &&
             (hi_score > lo_score || lo_score < 0)))
      {
         if (retry_count == 0)
         {
            S9xMessage(S9X_INFO, S9X_ROM_CONFUSING_FORMAT_INFO,
                       "ROM lied about its type! Trying again.");
            Settings.ForceNotInterleaved = true;
            Settings.ForceInterleaved = false;
            retry_count++;
            goto again;
         }
      }
   }

   if (Memory.ExtendedFormat == SMALLFIRST)
      Tales = true;

   FreeSDD1Data();
   InitROM(Tales);
#ifdef WANT_CHEATS
   S9xLoadCheatFile(S9xGetFilename("cht"));
   S9xInitCheatData();
   S9xApplyCheats();
#endif

   S9xReset();

   return (true);
}

#ifndef LOAD_FROM_MEMORY_TEST
uint32_t FileLoader(uint8_t* buffer, const char* filename, int32_t maxsize)
{


   FILE* ROMFile;
   int32_t TotalFileSize = 0;
   int len = 0;

   char dir [_MAX_DIR + 1];
   char drive [_MAX_DRIVE + 1];
   char name [_MAX_FNAME + 1];
   char ext [_MAX_EXT + 1];
   char fname [_MAX_PATH + 1];

   unsigned long FileSize = 0;

   _splitpath(filename, drive, dir, name, ext);
   _makepath(fname, drive, dir, name, ext);

#ifdef __WIN32__
   // memmove required: Overlapping addresses [Neb]
   memmove(&ext [0], &ext[1], 4);
#endif

   if ((ROMFile = fopen(fname, "rb")) == NULL)
      return (0);

   strcpy(Memory.ROMFilename, fname);

   Memory.HeaderCount = 0;
   uint8_t* ptr = buffer;
   bool more = false;

   do
   {
      FileSize = fread(ptr, 1, maxsize + 0x200 - (ptr - Memory.ROM), ROMFile);
      fclose(ROMFile);

      int calc_size = FileSize & ~0x1FFF; // round to the lower 0x2000

      if ((FileSize - calc_size == 512 && !Settings.ForceNoHeader) ||
            Settings.ForceHeader)
      {
         // memmove required: Overlapping addresses [Neb]
         // DS2 DMA notes: Can be split into 512-byte DMA blocks [Neb]
#ifdef DS2_DMA
         __dcache_writeback_all();
         {
            unsigned int i;
            for (i = 0; i < calc_size; i += 512)
            {
               ds2_DMAcopy_32Byte(2 /* channel: emu internal */, ptr + i, ptr + i + 512, 512);
               ds2_DMA_wait(2);
               ds2_DMA_stop(2);
            }
         }
#else
         memmove(ptr, ptr + 512, calc_size);
#endif
         Memory.HeaderCount++;
         FileSize -= 512;
      }

      ptr += FileSize;
      TotalFileSize += FileSize;


      // check for multi file roms

      if ((ptr - Memory.ROM) < (maxsize + 0x200) &&
            (isdigit(ext [0]) && ext [1] == 0 && ext [0] < '9'))
      {
         more = true;
         ext [0]++;
#ifdef __WIN32__
         // memmove required: Overlapping addresses [Neb]
         memmove(&ext [1], &ext [0], 4);
         ext [0] = '.';
#endif
         _makepath(fname, drive, dir, name, ext);
      }
      else if (ptr - Memory.ROM < maxsize + 0x200 &&
               (((len = strlen(name)) == 7 || len == 8) &&
                strncasecmp(name, "sf", 2) == 0 &&
                isdigit(name [2]) && isdigit(name [3]) && isdigit(name [4]) &&
                isdigit(name [5]) && isalpha(name [len - 1])))
      {
         more = true;
         name [len - 1]++;
#ifdef __WIN32__
         // memmove required: Overlapping addresses [Neb]
         memmove(&ext [1], &ext [0], 4);
         ext [0] = '.';
#endif
         _makepath(fname, drive, dir, name, ext);
      }
      else
         more = false;

   }
   while (more && (ROMFile = fopen(fname, "rb")) != NULL);



   if (Memory.HeaderCount == 0)
      S9xMessage(S9X_INFO, S9X_HEADERS_INFO, "No ROM file header found.");
   else
   {
      if (Memory.HeaderCount == 1)
         S9xMessage(S9X_INFO, S9X_HEADERS_INFO,
                    "Found ROM file header (and ignored it).");
      else
         S9xMessage(S9X_INFO, S9X_HEADERS_INFO,
                    "Found multiple ROM file headers (and ignored them).");
   }

   return TotalFileSize;

}
#endif

//compatibility wrapper
void S9xDeinterleaveMode2()
{
   S9xDeinterleaveType2(true);
}

void S9xDeinterleaveType2(bool reset)
{
   if (Settings.DisplayColor == 0xffff
         || Settings.DisplayColor == BUILD_PIXEL(0, 31, 0))
   {
      Settings.DisplayColor = BUILD_PIXEL(31, 14, 6);
      SET_UI_COLOR(255, 119, 25);

   }
   S9xMessage(S9X_INFO, S9X_ROM_INTERLEAVED_INFO,
              "ROM image is in interleaved format - converting...");

   int nblocks = Memory.CalculatedSize >> 16;
   int step = 64;

   while (nblocks <= step)
      step >>= 1;

   nblocks = step;
   uint8_t blocks [256];
   int i;

   for (i = 0; i < nblocks * 2; i++)
   {
      blocks [i] = (i & ~0xF) | ((i & 3) << 2) |
                   ((i & 12) >> 2);
   }

#ifdef DS2_DMA
   unsigned int TmpAdj;
   uint8_t* tmp = (uint8_t*) AlignedMalloc(0x10000, 32, &TmpAdj);
#else
   uint8_t* tmp = (uint8_t*) malloc(0x10000);
#endif

   if (tmp)
   {
#ifdef DS2_DMA
      __dcache_writeback_all();
#endif
      for (i = 0; i < nblocks * 2; i++)
      {
         int j;
         for (j = i; j < nblocks * 2; j++)
         {
            if (blocks [j] == i)
            {
#ifdef DS2_DMA
               ds2_DMAcopy_32Byte(2 /* channel: emu internal */, tmp,
                                  &Memory.ROM [blocks [j] * 0x10000], 0x10000);
               ds2_DMA_wait(2);
               ds2_DMA_stop(2);

               ds2_DMAcopy_32Byte(2 /* channel: emu internal */,
                                  &Memory.ROM [blocks [j] * 0x10000],
                                  &Memory.ROM [blocks [i] * 0x10000], 0x10000);
               ds2_DMA_wait(2);
               ds2_DMA_stop(2);

               ds2_DMAcopy_32Byte(2 /* channel: emu internal */,
                                  &Memory.ROM [blocks [i] * 0x10000], tmp, 0x10000);
               ds2_DMA_wait(2);
               ds2_DMA_stop(2);
#else
               // memmove converted: Different mallocs [Neb]
               memcpy(tmp, &Memory.ROM [blocks [j] * 0x10000], 0x10000);

               // memmove converted: Different addresses, or identical if blocks[i] == blocks[j] [Neb]
               memcpy(&Memory.ROM [blocks [j] * 0x10000],
                      &Memory.ROM [blocks [i] * 0x10000], 0x10000);
               // memmove converted: Different mallocs [Neb]
               memcpy(&Memory.ROM [blocks [i] * 0x10000], tmp, 0x10000);
#endif
               uint8_t b = blocks [j];
               blocks [j] = blocks [i];
               blocks [i] = b;
               break;
            }
         }
      }
      free((char*) tmp);
      tmp = NULL;
   }
   if (reset)
   {
      InitROM(false);
      S9xReset();
   }
}

//CRC32 for char arrays
uint32_t caCRC32(uint8_t* array, uint32_t size, register uint32_t crc32)
{
   register uint32_t i;
   for (i = 0; i < size; i++)
      crc32 = ((crc32 >> 8) & 0x00FFFFFF) ^ crc32Table[(crc32 ^ array[i]) & 0xFF];
   return ~crc32;
}

void InitROM(bool Interleaved)
{
   SuperFX.nRomBanks = Memory.CalculatedSize >> 15;
   Settings.MultiPlayer5Master = Settings.MultiPlayer5;
   Settings.MouseMaster = Settings.Mouse;
   Settings.SuperScopeMaster = Settings.SuperScope;
   Settings.DSP1Master = Settings.ForceDSP1;
   Settings.SuperFX = false;
   Settings.SA1 = false;
   Settings.C4 = false;
   Settings.SDD1 = false;
   Settings.SRTC = false;
   Settings.SPC7110 = false;
   Settings.SPC7110RTC = false;
   Settings.BS = false;
   Settings.OBC1 = false;
   Settings.SETA = false;
   s7r.DataRomSize = 0;
   Memory.CalculatedChecksum = 0;
   uint8_t* RomHeader;

   RomHeader = Memory.ROM + 0x7FB0;

   if (Memory.ExtendedFormat == BIGFIRST)
      RomHeader += 0x400000;

   if (Memory.HiROM)
      RomHeader += 0x8000;

   if (!Settings.BS)
   {
      Settings.BS = (-1 != is_bsx(Memory.ROM + 0x7FC0));

      if (Settings.BS)
      {
         Memory.LoROM = true;
         Memory.HiROM = false;
      }

      else
      {
         Settings.BS = (-1 != is_bsx(Memory.ROM + 0xFFC0));
         if (Settings.BS)
         {
            Memory.HiROM = true;
            Memory.LoROM = false;
         }
      }
   }

   memset(Memory.BlockIsRAM, 0, MEMMAP_NUM_BLOCKS);
   memset(Memory.BlockIsROM, 0, MEMMAP_NUM_BLOCKS);

   Memory.SRAM = Memory.SRAM;
   memset(Memory.ROMId, 0, 5);
   memset(Memory.CompanyId, 0, 3);

   ParseSNESHeader(RomHeader);

   // Try to auto-detect the DSP1 chip
   if (!Settings.ForceNoDSP1 &&
         (Memory.ROMType & 0xf) >= 3 && (Memory.ROMType & 0xf0) == 0)
      Settings.DSP1Master = true;

   if (Memory.HiROM)
   {
      // Enable S-RTC (Real Time Clock) emulation for Dai Kaijyu Monogatari 2
      Settings.SRTC = ((Memory.ROMType & 0xf0) >> 4) == 5;

      if (((Memory.ROMSpeed & 0x0F) == 0x0A) && ((Memory.ROMType & 0xF0) == 0xF0))
      {
         Settings.SPC7110 = true;
         if ((Memory.ROMType & 0x0F) == 0x09)
            Settings.SPC7110RTC = true;
      }

      if (Settings.BS)
         BSHiROMMap();
      else if (Settings.SPC7110)
         SPC7110HiROMMap();
      else if ((Memory.ROMSpeed & ~0x10) == 0x25)
         TalesROMMap(Interleaved);
      else HiROMMap();
   }
   else
   {
      Settings.SuperFX = Settings.ForceSuperFX;

      if (Memory.ROMType == 0x25)
         Settings.OBC1 = true;

      //BS-X BIOS
      if (Memory.ROMType == 0xE5)
         Settings.BS = true;

      if ((Memory.ROMType & 0xf0) == 0x10)
         Settings.SuperFX = !Settings.ForceNoSuperFX;

      Settings.SDD1 = Settings.ForceSDD1;
      if ((Memory.ROMType & 0xf0) == 0x40)
         Settings.SDD1 = !Settings.ForceNoSDD1;

      if (((Memory.ROMType & 0xF0) == 0xF0) & ((Memory.ROMSpeed & 0x0F) != 5))
      {
         Memory.SRAMSize = 2;
         SNESGameFixes.SRAMInitialValue = 0x00;
         if ((Memory.ROMType & 0x0F) == 6)
         {
            if (Memory.ROM[0x7FD7] == 0x09)
            {
               Settings.SETA = ST_011;
               SetSETA = &S9xSetST011;
               GetSETA = &S9xGetST011;
            }
            else
            {
               Settings.SETA = ST_010;
               SetSETA = &S9xSetST010;
               GetSETA = &S9xGetST010;
            }
         }
         else
         {
            Settings.SETA = ST_018;
            Memory.SRAMSize = 2;
         }
      }
      Settings.C4 = Settings.ForceC4;
      if ((Memory.ROMType & 0xf0) == 0xf0 &&
            (strncmp(Memory.ROMName, "MEGAMAN X", 9) == 0 ||
             strncmp(Memory.ROMName, "ROCKMAN X", 9) == 0))
         Settings.C4 = !Settings.ForceNoC4;

      if (Settings.SETA && Settings.SETA != ST_018)
         SetaDSPMap();
      else if (Settings.SuperFX)
      {
         //::SRAM = ROM + 1024 * 1024 * 4;
         SuperFXROMMap();
         Settings.MultiPlayer5Master = false;
         //Settings.MouseMaster = false;
         //Settings.SuperScopeMaster = false;
         Settings.DSP1Master = false;
         Settings.SA1 = false;
         Settings.C4 = false;
         Settings.SDD1 = false;
      }
      else if (Settings.ForceSA1 ||
               (!Settings.ForceNoSA1 && (Memory.ROMSpeed & ~0x10) == 0x23 &&
                (Memory.ROMType & 0xf) > 3 && (Memory.ROMType & 0xf0) == 0x30))
      {
         Settings.SA1 = true;
         //       Settings.MultiPlayer5Master = false;
         //Settings.MouseMaster = false;
         //Settings.SuperScopeMaster = false;
         Settings.DSP1Master = false;
         Settings.C4 = false;
         Settings.SDD1 = false;
         SA1ROMMap();
      }
      else if ((Memory.ROMSpeed & ~0x10) == 0x25)
         TalesROMMap(Interleaved);
      else if (Memory.ExtendedFormat != NOPE)
         JumboLoROMMap(Interleaved);
      else if (strncmp((char*) &Memory.ROM [0x7fc0], "SOUND NOVEL-TCOOL", 17) == 0 ||
               strncmp((char*) &Memory.ROM [0x7fc0], "DERBY STALLION 96", 17) == 0)
      {
         LoROM24MBSMap();
         Settings.DSP1Master = false;
      }

      else if (strncmp((char*) &Memory.ROM [0x7fc0], "THOROUGHBRED BREEDER3",
                       21) == 0 ||
               strncmp((char*) &Memory.ROM [0x7fc0], "RPG-TCOOL 2", 11) == 0)
      {
         SRAM512KLoROMMap();
         Settings.DSP1Master = false;
      }
      else if (strncmp((char*) &Memory.ROM [0x7fc0], "ADD-ON BASE CASSETE", 19) == 0)
      {
         Settings.MultiPlayer5Master = false;
         Settings.MouseMaster = false;
         Settings.SuperScopeMaster = false;
         Settings.DSP1Master = false;
         SufamiTurboLoROMMap();
         Memory.SRAMSize = 3;
      }
      else if ((Memory.ROMSpeed & ~0x10) == 0x22 &&
               strncmp(Memory.ROMName, "Super Street Fighter", 20) != 0)
         AlphaROMMap();
      else if (Settings.BS)
         BSLoROMMap();
      else LoROMMap();
   }

   if (Settings.BS)
      Memory.ROMRegion = 0;

   uint32_t sum1 = 0;
   uint32_t sum2 = 0;
   if (0 == Memory.CalculatedChecksum)
   {
      int power2 = 0;
      int size = Memory.CalculatedSize;

      while (size >>= 1)
         power2++;

      size = 1 << power2;
      uint32_t remainder = Memory.CalculatedSize - size;


      int i;

      for (i = 0; i < size; i++)
         sum1 += Memory.ROM [i];

      for (i = 0; i < (int) remainder; i++)
         sum2 += Memory.ROM [size + i];

      int sub = 0;
      if (Settings.BS && Memory.ROMType != 0xE5)
      {
         if (Memory.HiROM)
         {
            for (i = 0; i < 48; i++)
               sub += Memory.ROM[0xffb0 + i];
         }
         else if (Memory.LoROM)
         {
            for (i = 0; i < 48; i++)
               sub += Memory.ROM[0x7fb0 + i];
         }
         sum1 -= sub;
      }


      if (remainder)
         sum1 += sum2 * (size / remainder);


      sum1 &= 0xffff;
      Memory.CalculatedChecksum = sum1;
   }
   //now take a CRC32
   Memory.ROMCRC32 = caCRC32(Memory.ROM, Memory.CalculatedSize, 0xFFFFFFFF);

   if (Settings.ForceNTSC)
      Settings.PAL = false;
   else if (Settings.ForcePAL)
      Settings.PAL = true;
   else
   {
      //Korea refers to South Korea, which uses NTSC
      switch (Memory.ROMRegion)
      {
      case 13:
      case 1:
      case 0:
         Settings.PAL = false;
         break;
      default:
         Settings.PAL = true;
         break;
      }
   }
   if (Settings.PAL)
   {
      Settings.FrameTime = Settings.FrameTimePAL;
      Memory.ROMFramesPerSecond = 50;
   }
   else
   {
      Settings.FrameTime = Settings.FrameTimeNTSC;
      Memory.ROMFramesPerSecond = 60;
   }

   Memory.ROMName[ROM_NAME_LEN - 1] = 0;
   if (strlen(Memory.ROMName))
   {
      char* p = Memory.ROMName + strlen(Memory.ROMName) - 1;

      while (p > Memory.ROMName && *(p - 1) == ' ')
         p--;
      *p = 0;
   }

   {
      Memory.SRAMMask = Memory.SRAMSize ?
                        ((1 << (Memory.SRAMSize + 3)) * 128) - 1 : 0;
   }
   if ((Memory.ROMChecksum + Memory.ROMComplementChecksum != 0xffff)
         || Memory.ROMChecksum != Memory.CalculatedChecksum
         || ((uint32_t)Memory.CalculatedSize > (uint32_t)(((1 << (Memory.ROMSize - 7)) * 128)
               * 1024)))
   {
      if (Settings.DisplayColor == 0xffff
            || Settings.DisplayColor != BUILD_PIXEL(31, 0, 0))
      {
         Settings.DisplayColor = BUILD_PIXEL(31, 31, 0);
         SET_UI_COLOR(255, 255, 0);
      }
   }

#ifndef USE_BLARGG_APU
   IAPU.OneCycle = ONE_APU_CYCLE;
#endif
   Settings.Shutdown = Settings.ShutdownMaster;

   SetDSP = &DSP1SetByte;
   GetDSP = &DSP1GetByte;

   ResetSpeedMap();
   ApplyROMFixes();
   sprintf(Memory.ROMName, "%s", Safe(Memory.ROMName));
   sprintf(Memory.ROMId, "%s", Safe(Memory.ROMId));
   sprintf(Memory.CompanyId, "%s", Safe(Memory.CompanyId));

   sprintf(String,
           "\"%s\" [%s] %s, %s, Type: %s, Mode: %s, TV: %s, S-RAM: %s, ROMId: %s Company: %2.2s CRC32: %08X",
           Memory.ROMName,
           (Memory.ROMChecksum + Memory.ROMComplementChecksum != 0xffff ||
            Memory.ROMChecksum != Memory.CalculatedChecksum) ? "bad checksum" :
           "checksum ok",
           MapType(),
           Size(),
           KartContents(),
           MapMode(),
           TVStandard(),
           StaticRAMSize(),
           Memory.ROMId,
           Memory.CompanyId,
           Memory.ROMCRC32);

   S9xMessage(S9X_INFO, S9X_ROM_INFO, String);

   Settings.ForceHeader = Settings.ForceHiROM = Settings.ForceLoROM =
                             Settings.ForceInterleaved = Settings.ForceNoHeader =
                                      Settings.ForceNotInterleaved =
                                         Settings.ForceInterleaved2 = false;
}

bool LoadSRAM(const char* filename)
{
   int size = Memory.SRAMSize ?
              (1 << (Memory.SRAMSize + 3)) * 128 : 0;

   memset(Memory.SRAM, SNESGameFixes.SRAMInitialValue, 0x20000);

   if (size > 0x20000)
      size = 0x20000;

   if (size)
   {
      FILE* file;
      if ((file = fopen(filename, "rb")))
      {
         int len = fread((unsigned char*) Memory.SRAM, 1, 0x20000, file);
         fclose(file);
         if (len - size == 512)
         {
            // S-RAM file has a header - remove it
            // memmove required: Overlapping addresses [Neb]
            memmove(Memory.SRAM, Memory.SRAM + 512, size);
         }
         if (len == size + SRTC_SRAM_PAD)
         {
            S9xSRTCPostLoadState();
            S9xResetSRTC();
            rtc.index = -1;
            rtc.mode = MODE_READ;
         }
         else
            S9xHardResetSRTC();

         if (Settings.SPC7110RTC)
            S9xLoadSPC7110RTC(&rtc_f9);

         return (true);
      }
      S9xHardResetSRTC();
      return (false);
   }

   return (true);
}

bool SaveSRAM(const char* filename)
{
   if (Settings.SuperFX && Memory.ROMType < 0x15)
      return true;
   if (Settings.SA1 && Memory.ROMType == 0x34)
      return true;

   int size = Memory.SRAMSize ?
              (1 << (Memory.SRAMSize + 3)) * 128 : 0;
   if (Settings.SRTC)
   {
      size += SRTC_SRAM_PAD;
      S9xSRTCPreSaveState();
   }

   if (size > 0x20000)
      size = 0x20000;

   if (size && *Memory.ROMFilename)
   {

      FILE* file = fopen(filename, "w");
      if (file)
      {
         fwrite((unsigned char*) Memory.SRAM, size, 1, file);
         fclose(file);
         if (Settings.SPC7110RTC)
            S9xSaveSPC7110RTC(&rtc_f9);

         return (true);
      }
   }
   return (false);
}

void FixROMSpeed()
{
   int c;

   if (CPU.FastROMSpeed == 0)
      CPU.FastROMSpeed = SLOW_ONE_CYCLE;


   for (c = 0x800; c < 0x1000; c++)
   {
      if (c & 0x8 || c & 0x400)
         Memory.MemorySpeed [c] = (uint8_t) CPU.FastROMSpeed;
   }
}


void ResetSpeedMap()
{
   int i;
   memset(Memory.MemorySpeed, SLOW_ONE_CYCLE, 0x1000);
   for (i = 0; i < 0x400; i += 0x10)
   {
      Memory.MemorySpeed[i + 2] = Memory.MemorySpeed[0x800 + i + 2] = ONE_CYCLE;
      Memory.MemorySpeed[i + 3] = Memory.MemorySpeed[0x800 + i + 3] = ONE_CYCLE;
      Memory.MemorySpeed[i + 4] = Memory.MemorySpeed[0x800 + i + 4] = ONE_CYCLE;
      Memory.MemorySpeed[i + 5] = Memory.MemorySpeed[0x800 + i + 5] = ONE_CYCLE;
   }
   FixROMSpeed();
}

void WriteProtectROM()
{
   // memmove converted: Different mallocs [Neb]
   memcpy((void*) Memory.WriteMap, (void*) Memory.Map, sizeof(Memory.Map));
   int c;
   for (c = 0; c < 0x1000; c++)
   {
      if (Memory.BlockIsROM [c])
         Memory.WriteMap [c] = (uint8_t*) MAP_NONE;
   }
}

void MapRAM()
{
   int c;

   if (Memory.LoROM && !Settings.SDD1)
   {
      // Banks 70->77, S-RAM
      for (c = 0; c < 0x0f; c++)
      {
         int i;
         for (i = 0; i < 8; i++)
         {
            Memory.Map [(c << 4) + 0xF00 + i] = Memory.Map [(c << 4) + 0x700 + i] =
                                                   (uint8_t*) MAP_LOROM_SRAM;
            Memory.BlockIsRAM [(c << 4) + 0xF00 + i] = Memory.BlockIsRAM [(c << 4) + 0x700 +
                  i] = true;
            Memory.BlockIsROM [(c << 4) + 0xF00 + i] = Memory.BlockIsROM [(c << 4) + 0x700 +
                  i] = false;
         }
      }
   }
   else if (Memory.LoROM && Settings.SDD1)
   {
      // Banks 70->77, S-RAM
      for (c = 0; c < 0x0f; c++)
      {
         int i;
         for (i = 0; i < 8; i++)
         {
            Memory.Map [(c << 4) + 0x700 + i] = (uint8_t*) MAP_LOROM_SRAM;
            Memory.BlockIsRAM [(c << 4) + 0x700 + i] = true;
            Memory.BlockIsROM [(c << 4) + 0x700 + i] = false;
         }
      }
   }
   // Banks 7e->7f, RAM
   for (c = 0; c < 16; c++)
   {
      Memory.Map [c + 0x7e0] = Memory.RAM;
      Memory.Map [c + 0x7f0] = Memory.RAM + 0x10000;
      Memory.BlockIsRAM [c + 0x7e0] = true;
      Memory.BlockIsRAM [c + 0x7f0] = true;
      Memory.BlockIsROM [c + 0x7e0] = false;
      Memory.BlockIsROM [c + 0x7f0] = false;
   }
   WriteProtectROM();
}

void MapExtraRAM()
{
   int c;

   // Banks 7e->7f, RAM
   for (c = 0; c < 16; c++)
   {
      Memory.Map [c + 0x7e0] = Memory.RAM;
      Memory.Map [c + 0x7f0] = Memory.RAM + 0x10000;
      Memory.BlockIsRAM [c + 0x7e0] = true;
      Memory.BlockIsRAM [c + 0x7f0] = true;
      Memory.BlockIsROM [c + 0x7e0] = false;
      Memory.BlockIsROM [c + 0x7f0] = false;
   }

   // Banks 70->73, S-RAM
   for (c = 0; c < 16; c++)
   {
      Memory.Map [c + 0x700] = Memory.SRAM;
      Memory.Map [c + 0x710] = Memory.SRAM + 0x8000;
      Memory.Map [c + 0x720] = Memory.SRAM + 0x10000;
      Memory.Map [c + 0x730] = Memory.SRAM + 0x18000;

      Memory.BlockIsRAM [c + 0x700] = true;
      Memory.BlockIsROM [c + 0x700] = false;
      Memory.BlockIsRAM [c + 0x710] = true;
      Memory.BlockIsROM [c + 0x710] = false;
      Memory.BlockIsRAM [c + 0x720] = true;
      Memory.BlockIsROM [c + 0x720] = false;
      Memory.BlockIsRAM [c + 0x730] = true;
      Memory.BlockIsROM [c + 0x730] = false;
   }
}

void LoROMMap()
{
   int c;
   int i;
   int j;
   int mask[4];
   for (j = 0; j < 4; j++)
      mask[j] = 0x00ff;

   mask[0] = (Memory.CalculatedSize / 0x8000) - 1;

   int x;
   bool foundZeros;
   bool pastZeros;

   for (j = 0; j < 3; j++)
   {
      x = 1;
      foundZeros = false;
      pastZeros = false;

      mask[j + 1] = mask[j];

      while (x > 0x100 && !pastZeros)
      {
         if (mask[j]&x)
         {
            x <<= 1;
            if (foundZeros)
               pastZeros = true;
         }
         else
         {
            foundZeros = true;
            pastZeros = false;
            mask[j + 1] |= x;
            x <<= 1;
         }
      }
   }


   // Banks 00->3f and 80->bf
   for (c = 0; c < 0x400; c += 16)
   {
      Memory.Map [c + 0] = Memory.Map [c + 0x800] = Memory.RAM;
      Memory.Map [c + 1] = Memory.Map [c + 0x801] = Memory.RAM;
      Memory.BlockIsRAM [c + 0] = Memory.BlockIsRAM [c + 0x800] = true;
      Memory.BlockIsRAM [c + 1] = Memory.BlockIsRAM [c + 0x801] = true;

      Memory.Map [c + 2] = Memory.Map [c + 0x802] = (uint8_t*) MAP_PPU;
      if (Settings.SETA == ST_018)
         Memory.Map [c + 3] = Memory.Map [c + 0x803] = (uint8_t*) MAP_SETA_RISC;
      else Memory.Map [c + 3] = Memory.Map [c + 0x803] = (uint8_t*) MAP_PPU;
      Memory.Map [c + 4] = Memory.Map [c + 0x804] = (uint8_t*) MAP_CPU;
      Memory.Map [c + 5] = Memory.Map [c + 0x805] = (uint8_t*) MAP_CPU;
      if (Settings.DSP1Master)
      {
         Memory.Map [c + 6] = Memory.Map [c + 0x806] = (uint8_t*) MAP_DSP;
         Memory.Map [c + 7] = Memory.Map [c + 0x807] = (uint8_t*) MAP_DSP;
      }
      else if (Settings.C4)
      {
         Memory.Map [c + 6] = Memory.Map [c + 0x806] = (uint8_t*) MAP_C4;
         Memory.Map [c + 7] = Memory.Map [c + 0x807] = (uint8_t*) MAP_C4;
      }
      else if (Settings.OBC1)
      {
         Memory.Map [c + 6] = Memory.Map [c + 0x806] = (uint8_t*) MAP_OBC_RAM;
         Memory.Map [c + 7] = Memory.Map [c + 0x807] = (uint8_t*) MAP_OBC_RAM;
      }
      else
      {
         Memory.Map [c + 6] = Memory.Map [c + 0x806] = (uint8_t*) bytes0x2000 - 0x6000;
         Memory.Map [c + 7] = Memory.Map [c + 0x807] = (uint8_t*) bytes0x2000 - 0x6000;
      }

      for (i = c + 8; i < c + 16; i++)
      {
         int e = 3;
         int d = c >> 4;
         while (d > mask[0])
         {
            d &= mask[e];
            e--;
         }
         Memory.Map [i] = Memory.Map [i + 0x800] = Memory.ROM + (((d) - 1) * 0x8000);
         Memory.BlockIsROM [i] = Memory.BlockIsROM [i + 0x800] = true;
      }
   }

   if (Settings.DSP1Master)
   {
      // Banks 30->3f and b0->bf
      for (c = 0x300; c < 0x400; c += 16)
      {
         for (i = c + 8; i < c + 16; i++)
         {
            Memory.Map [i] = Memory.Map [i + 0x800] = (uint8_t*) MAP_DSP;
            Memory.BlockIsROM [i] = Memory.BlockIsROM [i + 0x800] = false;
         }
      }
   }

   // Banks 40->7f and c0->ff
   for (c = 0; c < 0x400; c += 16)
   {
      for (i = c; i < c + 8; i++)
         Memory.Map [i + 0x400] = Memory.Map [i + 0xc00] = &Memory.ROM [(c << 11) %
                                  Memory.CalculatedSize];

      for (i = c + 8; i < c + 16; i++)
      {
         int e = 3;
         int d = (c + 0x400) >> 4;
         while (d > mask[0])
         {
            d &= mask[e];
            e--;
         }

         Memory.Map [i + 0x400] = Memory.Map [i + 0xc00] = Memory.ROM + (((
                                     d) - 1) * 0x8000);
      }

      for (i = c; i < c + 16; i++)
         Memory.BlockIsROM [i + 0x400] = Memory.BlockIsROM [i + 0xc00] = true;
   }

   if (Settings.DSP1Master)
   {
      for (c = 0; c < 0x100; c++)
      {
         Memory.Map [c + 0xe00] = (uint8_t*) MAP_DSP;
         Memory.BlockIsROM [c + 0xe00] = false;
      }
   }

   int sum = 0, k, l, bankcount;
   bankcount = 1 << (Memory.ROMSize - 7); //Mbits

   //safety for corrupt headers
   if (bankcount > 128)
      bankcount = (Memory.CalculatedSize / 0x8000) / 4;
   bankcount *= 4; //to banks
   bankcount <<= 4; //Map banks
   bankcount += 0x800; //normalize
   for (k = 0x800; k < (bankcount); k += 16)
   {
      uint8_t* bank = 0x8000 + Memory.Map[k + 8];
      for (l = 0; l < 0x8000; l++)
         sum += bank[l];
   }
   Memory.CalculatedChecksum = sum & 0xFFFF;

   MapRAM();
   WriteProtectROM();
}

void SetaDSPMap()
{
   int c;
   int i;
   int j;
   int mask[4];
   for (j = 0; j < 4; j++)
      mask[j] = 0x00ff;

   mask[0] = (Memory.CalculatedSize / 0x8000) - 1;

   int x;
   bool foundZeros;
   bool pastZeros;

   for (j = 0; j < 3; j++)
   {
      x = 1;
      foundZeros = false;
      pastZeros = false;

      mask[j + 1] = mask[j];

      while (x > 0x100 && !pastZeros)
      {
         if (mask[j]&x)
         {
            x <<= 1;
            if (foundZeros)
               pastZeros = true;
         }
         else
         {
            foundZeros = true;
            pastZeros = false;
            mask[j + 1] |= x;
            x <<= 1;
         }
      }
   }


   // Banks 00->3f and 80->bf
   for (c = 0; c < 0x400; c += 16)
   {
      Memory.Map [c + 0] = Memory.Map [c + 0x800] = Memory.RAM;
      Memory.Map [c + 1] = Memory.Map [c + 0x801] = Memory.RAM;
      Memory.BlockIsRAM [c + 0] = Memory.BlockIsRAM [c + 0x800] = true;
      Memory.BlockIsRAM [c + 1] = Memory.BlockIsRAM [c + 0x801] = true;

      Memory.Map [c + 2] = Memory.Map [c + 0x802] = (uint8_t*) MAP_PPU;
      Memory.Map [c + 3] = Memory.Map [c + 0x803] = (uint8_t*) MAP_PPU;
      Memory.Map [c + 4] = Memory.Map [c + 0x804] = (uint8_t*) MAP_CPU;
      Memory.Map [c + 5] = Memory.Map [c + 0x805] = (uint8_t*) MAP_CPU;
      Memory.Map [c + 6] = Memory.Map [c + 0x806] = (uint8_t*) bytes0x2000 - 0x6000;
      Memory.Map [c + 7] = Memory.Map [c + 0x807] = (uint8_t*) bytes0x2000 - 0x6000;

      for (i = c + 8; i < c + 16; i++)
      {
         int e = 3;
         int d = c >> 4;
         while (d > mask[0])
         {
            d &= mask[e];
            e--;
         }
         Memory.Map [i] = Memory.Map [i + 0x800] = Memory.ROM + (((d) - 1) * 0x8000);
         Memory.BlockIsROM [i] = Memory.BlockIsROM [i + 0x800] = true;
      }
   }

   // Banks 40->7f and c0->ff
   for (c = 0; c < 0x400; c += 16)
   {
      for (i = c + 8; i < c + 16; i++)
      {
         int e = 3;
         int d = (c + 0x400) >> 4;
         while (d > mask[0])
         {
            d &= mask[e];
            e--;
         }

         Memory.Map [i + 0x400] = Memory.Map [i + 0xc00] = Memory.ROM + (((
                                     d) - 1) * 0x8000);
      }

      //only upper half is ROM
      for (i = c + 8; i < c + 16; i++)
         Memory.BlockIsROM [i + 0x400] = Memory.BlockIsROM [i + 0xc00] = true;
   }

   memset(Memory.SRAM, 0, 0x1000);
   for (c = 0x600; c < 0x680; c += 0x10)
   {
      for (i = 0; i < 0x08; i++)
      {
         //where does the SETA chip access, anyway?
         //please confirm this?
         Memory.Map[c + 0x80 + i] = (uint8_t*)MAP_SETA_DSP;
         Memory.BlockIsROM [c + 0x80 + i] = false;
         Memory.BlockIsRAM [c + 0x80 + i] = true;
      }

      for (i = 0; i < 0x04; i++)
      {
         //and this!
         Memory.Map[c + i] = (uint8_t*)MAP_SETA_DSP;
         Memory.BlockIsROM [c + i] = false;
      }
   }

   int sum = 0, k, l, bankcount;
   bankcount = 1 << (Memory.ROMSize - 7); //Mbits
   //safety for corrupt headers
   if (bankcount > 128)
      bankcount = (Memory.CalculatedSize / 0x8000) / 4;
   bankcount *= 4; //to banks
   bankcount <<= 4; //Map banks
   bankcount += 0x800; //normalize
   for (k = 0x800; k < (bankcount); k += 16)
   {
      uint8_t* bank = 0x8000 + Memory.Map[k + 8];
      for (l = 0; l < 0x8000; l++)
         sum += bank[l];
   }
   Memory.CalculatedChecksum = sum & 0xFFFF;

   MapRAM();
   WriteProtectROM();
}

void BSLoROMMap()
{
   int c;
   int i;

   if (Settings.BS)
      Memory.SRAMSize = 5;

   // Banks 00->3f and 80->bf
   for (c = 0; c < 0x400; c += 16)
   {
      Memory.Map [c + 0] = Memory.Map [c + 0x800] = Memory.RAM;
      Memory.Map [c + 1] = Memory.Map [c + 0x801] = Memory.RAM;
      Memory.BlockIsRAM [c + 0] = Memory.BlockIsRAM [c + 0x800] = true;
      Memory.BlockIsRAM [c + 1] = Memory.BlockIsRAM [c + 0x801] = true;

      Memory.Map [c + 2] = Memory.Map [c + 0x802] = (uint8_t*) MAP_PPU;
      Memory.Map [c + 3] = Memory.Map [c + 0x803] = (uint8_t*) MAP_PPU;
      Memory.Map [c + 4] = Memory.Map [c + 0x804] = (uint8_t*) MAP_CPU;
      Memory.Map [c + 5] = Memory.Map [c + 0x805] = (uint8_t*) Memory.RAM;
      //    Memory.Map [c + 5] = Memory.Map [c + 0x805] = (uint8_t *) Memory.SRAM;
      Memory.BlockIsRAM [c + 5] = Memory.BlockIsRAM [c + 0x805] = true;

      //    Memory.Map [c + 6] = Memory.Map [c + 0x806] = (uint8_t *)MAP_NONE;
      //    Memory.Map [c + 7] = Memory.Map [c + 0x807] = (uint8_t *)MAP_NONE;
      Memory.Map [c + 6] = Memory.Map [c + 0x806] = (uint8_t*) Memory.RAM;
      //    Memory.Map [c + 5] = Memory.Map [c + 0x805] = (uint8_t *) SRAM;
      Memory.BlockIsRAM [c + 6] = Memory.BlockIsRAM [c + 0x806] = true;
      Memory.Map [c + 7] = Memory.Map [c + 0x807] = (uint8_t*) Memory.RAM;
      //    Memory.Map [c + 5] = Memory.Map [c + 0x805] = (uint8_t *) Memory.SRAM;
      Memory.BlockIsRAM [c + 7] = Memory.BlockIsRAM [c + 0x807] = true;
      for (i = c + 8; i < c + 16; i++)
      {
         Memory.Map [i] = Memory.Map [i + 0x800] = &Memory.ROM [(c << 11) %
                          Memory.CalculatedSize] - 0x8000;
         Memory.BlockIsROM [i] = Memory.BlockIsROM [i + 0x800] = true;
      }
   }

   for (c = 0; c < 8; c++)
   {
      Memory.Map[(c << 4) + 0x105] = (uint8_t*)MAP_LOROM_SRAM;
      Memory.BlockIsROM [(c << 4) + 0x105] = false;
      Memory.BlockIsRAM [(c << 4) + 0x105] = true;
   }


   /*  // Banks 40->7f and c0->ff
     for (c = 0; c < 0x400; c += 16)
     {
       for (i = c; i < c + 8; i++)
          Map [i + 0x400] = Map [i + 0xc00] = &ROM [(c << 11) % CalculatedSize];

       for (i = c + 8; i < c + 16; i++)
          Map [i + 0x400] = Map [i + 0xc00] = &ROM [((c << 11) + 0x200000) % CalculatedSize - 0x8000];

       for (i = c; i < c + 16; i++)
       {
          BlockIsROM [i + 0x400] = BlockIsROM [i + 0xc00] = true;
       }
     }
    */
   for (c = 1; c <= 4; c++)
   {
      for (i = 0; i < 16; i++)
      {
         Memory.Map[0x400 + i + (c << 4)] = (uint8_t*)MAP_LOROM_SRAM;
         Memory.BlockIsRAM[0x400 + i + (c << 4)] = true;
         Memory.BlockIsROM[0x400 + i + (c << 4)] = false;
      }
   }

   for (i = 0; i < 0x80; i++)
   {
      Memory.Map[0x700 + i] = &Memory.BSRAM[0x10000 * (i / 16)];
      Memory.BlockIsRAM[0x700 + i] = true;
      Memory.BlockIsROM[0x700 + i] = false;
   }
   for (i = 0; i < 8; i++)
   {
      Memory.Map[0x205 + (i << 4)] = Memory.Map[0x285 + (i << 4)] = Memory.Map[0x305
                                     + (i << 4)] = Memory.Map[0x385 + (i << 4)] = Memory.Map[0x705 + (i << 4)];
      Memory.BlockIsRAM[0x205 + (i << 4)] = Memory.BlockIsRAM[0x285 +
                                            (i << 4)] = Memory.BlockIsRAM[0x305 + (i << 4)] = Memory.BlockIsRAM[0x385 +
                                                  (i << 4)] = true;
      Memory.BlockIsROM[0x205 + (i << 4)] = Memory.BlockIsROM[0x285 +
                                            (i << 4)] = Memory.BlockIsROM[0x305 + (i << 4)] = Memory.BlockIsROM[0x385 +
                                                  (i << 4)] = false;
   }
   for (c = 0; c < 8; c++)
   {
      Memory.Map[(c << 4) + 0x005] = Memory.BSRAM - 0x5000;
      Memory.BlockIsROM [(c << 4) + 0x005] = false;
      Memory.BlockIsRAM [(c << 4) + 0x005] = true;
   }
   MapRAM();
   WriteProtectROM();


}

void HiROMMap()
{
   int i;
   int c;
   int j;

   int mask[4];
   for (j = 0; j < 4; j++)
      mask[j] = 0x00ff;

   mask[0] = (Memory.CalculatedSize / 0x10000) - 1;

   if (Settings.ForceSA1 ||
         (!Settings.ForceNoSA1 && (Memory.ROMSpeed & ~0x10) == 0x23 &&
          (Memory.ROMType & 0xf) > 3 && (Memory.ROMType & 0xf0) == 0x30))
   {
      Settings.DisplayColor = BUILD_PIXEL(31, 0, 0);
      SET_UI_COLOR(255, 0, 0);
   }


   int x;
   bool foundZeros;
   bool pastZeros;

   for (j = 0; j < 3; j++)
   {
      x = 1;
      foundZeros = false;
      pastZeros = false;

      mask[j + 1] = mask[j];

      while (x > 0x100 && !pastZeros)
      {
         if (mask[j]&x)
         {
            x <<= 1;
            if (foundZeros)
               pastZeros = true;
         }
         else
         {
            foundZeros = true;
            pastZeros = false;
            mask[j + 1] |= x;
            x <<= 1;
         }
      }
   }

   // Banks 00->3f and 80->bf
   for (c = 0; c < 0x400; c += 16)
   {
      Memory.Map [c + 0] = Memory.Map [c + 0x800] = Memory.RAM;
      Memory.BlockIsRAM [c + 0] = Memory.BlockIsRAM [c + 0x800] = true;
      Memory.Map [c + 1] = Memory.Map [c + 0x801] = Memory.RAM;
      Memory.BlockIsRAM [c + 1] = Memory.BlockIsRAM [c + 0x801] = true;

      Memory.Map [c + 2] = Memory.Map [c + 0x802] = (uint8_t*) MAP_PPU;
      Memory.Map [c + 3] = Memory.Map [c + 0x803] = (uint8_t*) MAP_PPU;
      Memory.Map [c + 4] = Memory.Map [c + 0x804] = (uint8_t*) MAP_CPU;
      Memory.Map [c + 5] = Memory.Map [c + 0x805] = (uint8_t*) MAP_CPU;

      if (Settings.DSP1Master)
      {
         Memory.Map [c + 6] = Memory.Map [c + 0x806] = (uint8_t*) MAP_DSP;
         Memory.Map [c + 7] = Memory.Map [c + 0x807] = (uint8_t*) MAP_DSP;
      }
      else
      {
         Memory.Map [c + 6] = Memory.Map [c + 0x806] = (uint8_t*) MAP_NONE;
         Memory.Map [c + 7] = Memory.Map [c + 0x807] = (uint8_t*) MAP_NONE;
      }

      for (i = c + 8; i < c + 16; i++)
      {
         int e = 3;
         int d = c >> 4;
         while (d > mask[0])
         {
            d &= mask[e];
            e--;
         }
         Memory.Map [i] = Memory.Map [i + 0x800] = Memory.ROM + (d * 0x10000);
         Memory.BlockIsROM [i] = Memory.BlockIsROM [i + 0x800] = true;
      }
   }

   // Banks 30->3f and b0->bf, address ranges 6000->7fff is S-RAM.
   for (c = 0; c < 16; c++)
   {
      Memory.Map [0x306 + (c << 4)] = (uint8_t*) MAP_HIROM_SRAM;
      Memory.Map [0x307 + (c << 4)] = (uint8_t*) MAP_HIROM_SRAM;
      Memory.Map [0xb06 + (c << 4)] = (uint8_t*) MAP_HIROM_SRAM;
      Memory.Map [0xb07 + (c << 4)] = (uint8_t*) MAP_HIROM_SRAM;
      Memory.BlockIsRAM [0x306 + (c << 4)] = true;
      Memory.BlockIsRAM [0x307 + (c << 4)] = true;
      Memory.BlockIsRAM [0xb06 + (c << 4)] = true;
      Memory.BlockIsRAM [0xb07 + (c << 4)] = true;
   }

   // Banks 40->7f and c0->ff
   for (c = 0; c < 0x400; c += 16)
   {
      for (i = c; i < c + 16; i++)
      {
         int e = 3;
         int d = (c) >> 4;
         while (d > mask[0])
         {
            d &= mask[e];
            e--;
         }
         Memory.Map [i + 0x400] = Memory.Map [i + 0xc00] = Memory.ROM + (d * 0x10000);
         Memory.BlockIsROM [i + 0x400] = Memory.BlockIsROM [i + 0xc00] = true;
      }
   }

   int bankmax = 0x40 + (1 << (Memory.ROMSize - 6));
   //safety for corrupt headers
   if (bankmax > 128)
      bankmax = 0x80;
   int sum = 0;
   for (i = 0x40; i < bankmax; i++)
   {
      uint8_t* bank_low = (uint8_t*)Memory.Map[i << 4];
      for (c = 0; c < 0x10000; c++)
         sum += bank_low[c];
   }
   Memory.CalculatedChecksum = sum & 0xFFFF;

   MapRAM();
   WriteProtectROM();
}

void TalesROMMap(bool Interleaved)
{
   int c;
   int i;

   if (Interleaved)
   {
      if (Settings.DisplayColor == 0xffff)
      {
         Settings.DisplayColor = BUILD_PIXEL(0, 31, 0);
         SET_UI_COLOR(0, 255, 0);
      }
   }
   uint32_t OFFSET0 = 0x400000;
   uint32_t OFFSET1 = 0x400000;
   uint32_t OFFSET2 = 0x000000;

   if (Interleaved)
   {
      OFFSET0 = 0x000000;
      OFFSET1 = 0x000000;
      OFFSET2 = Memory.CalculatedSize -
                0x400000; //changed to work with interleaved DKJM2.
   }

   // Banks 00->3f and 80->bf
   for (c = 0; c < 0x400; c += 16)
   {
      Memory.Map [c + 0] = Memory.Map [c + 0x800] = Memory.RAM;
      Memory.Map [c + 1] = Memory.Map [c + 0x801] = Memory.RAM;
      Memory.BlockIsRAM [c + 0] = Memory.BlockIsRAM [c + 0x800] = true;
      Memory.BlockIsRAM [c + 1] = Memory.BlockIsRAM [c + 0x801] = true;

      Memory.Map [c + 2] = Memory.Map [c + 0x802] = (uint8_t*) MAP_PPU;
      Memory.Map [c + 3] = Memory.Map [c + 0x803] = (uint8_t*) MAP_PPU;
      Memory.Map [c + 4] = Memory.Map [c + 0x804] = (uint8_t*) MAP_CPU;
      Memory.Map [c + 5] = Memory.Map [c + 0x805] = (uint8_t*) MAP_CPU;

      //makes more sense to map the range here.
      //ToP seems to use sram to skip intro???
      if (c >= 0x300)
      {
         Memory.Map [c + 6] = Memory.Map [c + 0x806] = (uint8_t*) MAP_HIROM_SRAM;
         Memory.Map [c + 7] = Memory.Map [c + 0x807] = (uint8_t*) MAP_HIROM_SRAM;
         Memory.BlockIsRAM [6 + c] = Memory.BlockIsRAM [7 + c] =
                                        Memory.BlockIsRAM [0x806 + c] = Memory.BlockIsRAM [0x807 + c] = true;
      }
      else
      {
         Memory.Map [c + 6] = Memory.Map [c + 0x806] = (uint8_t*) MAP_NONE;
         Memory.Map [c + 7] = Memory.Map [c + 0x807] = (uint8_t*) MAP_NONE;

      }
      for (i = c + 8; i < c + 16; i++)
      {
         Memory.Map [i] = &Memory.ROM [((c << 12) % (Memory.CalculatedSize - 0x400000)) +
                                       OFFSET0];
         Memory.Map [i + 0x800] = &Memory.ROM [((c << 12) % 0x400000) + OFFSET2];
         Memory.BlockIsROM [i] = true;
         Memory.BlockIsROM [i + 0x800] = true;
      }
   }

   // Banks 40->7f and c0->ff
   for (c = 0; c < 0x400; c += 16)
   {
      for (i = c; i < c + 8; i++)
      {
         Memory.Map [i + 0x400] = &Memory.ROM [((c << 12) % (Memory.CalculatedSize -
                                                0x400000)) + OFFSET1];
         Memory.Map [i + 0x408] = &Memory.ROM [((c << 12) % (Memory.CalculatedSize -
                                                0x400000)) + OFFSET1];
         Memory.Map [i + 0xc00] = &Memory.ROM [((c << 12) % 0x400000) + OFFSET2];
         Memory.Map [i + 0xc08] = &Memory.ROM [((c << 12) % 0x400000) + OFFSET2];
         Memory.BlockIsROM [i + 0x400] = true;
         Memory.BlockIsROM [i + 0x408] = true;
         Memory.BlockIsROM [i + 0xc00] = true;
         Memory.BlockIsROM [i + 0xc08] = true;
      }
   }

   if ((strncmp("TALES", (char*)Memory.Map[8] + 0xFFC0, 5) == 0))
   {
      if (((*(Memory.Map[8] + 0xFFDE)) == (*(Memory.Map[0x808] + 0xFFDE))))
      {
         Settings.DisplayColor = BUILD_PIXEL(31, 0, 0);
         SET_UI_COLOR(255, 0, 0);
      }
   }

   Memory.ROMChecksum = *(Memory.Map[8] + 0xFFDE) + (*(Memory.Map[8] + 0xFFDF) <<
                        8);
   Memory.ROMComplementChecksum = *(Memory.Map[8] + 0xFFDC) + (*
                                  (Memory.Map[8] + 0xFFDD) << 8);

   int sum = 0;
   for (i = 0x40; i < 0x80; i++)
   {
      uint8_t* bank_low = (uint8_t*)Memory.Map[i << 4];
      uint8_t* bank_high = (uint8_t*)Memory.Map[(i << 4) + 0x800];
      for (c = 0; c < 0x10000; c++)
      {
         sum += bank_low[c];
         sum += bank_high[c];
      }
   }

   Memory.CalculatedChecksum = sum & 0xFFFF;

   MapRAM();
   WriteProtectROM();
}

void AlphaROMMap()
{
   int c;
   int i;

   // Banks 00->3f and 80->bf
   for (c = 0; c < 0x400; c += 16)
   {
      Memory.Map [c + 0] = Memory.Map [c + 0x800] = Memory.RAM;
      Memory.Map [c + 1] = Memory.Map [c + 0x801] = Memory.RAM;
      Memory.BlockIsRAM [c + 0] = Memory.BlockIsRAM [c + 0x800] = true;
      Memory.BlockIsRAM [c + 1] = Memory.BlockIsRAM [c + 0x801] = true;

      Memory.Map [c + 2] = Memory.Map [c + 0x802] = (uint8_t*) MAP_PPU;
      Memory.Map [c + 3] = Memory.Map [c + 0x803] = (uint8_t*) MAP_PPU;
      Memory.Map [c + 4] = Memory.Map [c + 0x804] = (uint8_t*) MAP_CPU;
      Memory.Map [c + 5] = Memory.Map [c + 0x805] = (uint8_t*) MAP_CPU;
      Memory.Map [c + 6] = Memory.Map [c + 0x806] = (uint8_t*) MAP_NONE;
      Memory.Map [c + 7] = Memory.Map [c + 0x807] = (uint8_t*) MAP_NONE;

      for (i = c + 8; i < c + 16; i++)
      {
         Memory.Map [i] = Memory.Map [i + 0x800] = &Memory.ROM [c << 11] - 0x8000;
         Memory.BlockIsROM [i] = true;
      }
   }

   // Banks 40->7f and c0->ff

   for (c = 0; c < 0x400; c += 16)
   {
      for (i = c; i < c + 16; i++)
      {
         Memory.Map [i + 0x400] = &Memory.ROM [(c << 12) % Memory.CalculatedSize];
         Memory.Map [i + 0xc00] = &Memory.ROM [(c << 12) % Memory.CalculatedSize];
         Memory.BlockIsROM [i + 0x400] = Memory.BlockIsROM [i + 0xc00] = true;
      }
   }

   MapRAM();
   WriteProtectROM();
}

void DetectSuperFxRamSize()
{
   if (Memory.ROM[0x7FDA] == 0x33)
      Memory.SRAMSize = Memory.ROM[0x7FBD];
   else
   {
      if (strncmp(Memory.ROMName, "STAR FOX 2", 10) == 0)
         Memory.SRAMSize = 6;
      else Memory.SRAMSize = 5;
   }
}

void SuperFXROMMap()
{
   int c;
   int i;

   DetectSuperFxRamSize();

   // Banks 00->3f and 80->bf
   for (c = 0; c < 0x400; c += 16)
   {
      Memory.Map [c + 0] = Memory.Map [c + 0x800] = Memory.RAM;
      Memory.Map [c + 1] = Memory.Map [c + 0x801] = Memory.RAM;
      Memory.BlockIsRAM [c + 0] = Memory.BlockIsRAM [c + 0x800] = true;
      Memory.BlockIsRAM [c + 1] = Memory.BlockIsRAM [c + 0x801] = true;

      Memory.Map [c + 2] = Memory.Map [c + 0x802] = (uint8_t*) MAP_PPU;
      Memory.Map [c + 3] = Memory.Map [c + 0x803] = (uint8_t*) MAP_PPU;
      Memory.Map [c + 4] = Memory.Map [c + 0x804] = (uint8_t*) MAP_CPU;
      Memory.Map [c + 5] = Memory.Map [c + 0x805] = (uint8_t*) MAP_CPU;
      Memory.Map [0x006 + c] = Memory.Map [0x806 + c] = (uint8_t*) Memory.SRAM - 0x6000;
      Memory.Map [0x007 + c] = Memory.Map [0x807 + c] = (uint8_t*) Memory.SRAM - 0x6000;
      Memory.BlockIsRAM [0x006 + c] = Memory.BlockIsRAM [0x007 + c] =
                                         Memory.BlockIsRAM [0x806 + c] = Memory.BlockIsRAM [0x807 + c] = true;

      for (i = c + 8; i < c + 16; i++)
      {
         Memory.Map [i] = Memory.Map [i + 0x800] = &Memory.ROM [c << 11] - 0x8000;
         Memory.BlockIsROM [i] = Memory.BlockIsROM [i + 0x800] = true;
      }
   }

   // Banks 40->7f and c0->ff
   for (c = 0; c < 0x400; c += 16)
   {
      for (i = c; i < c + 16; i++)
      {
         Memory.Map [i + 0x400] = Memory.Map [i + 0xc00] = &Memory.ROM [(c << 12) %
                                  Memory.CalculatedSize];
         Memory.BlockIsROM [i + 0x400] = Memory.BlockIsROM [i + 0xc00] = true;
      }
   }

   // Banks 7e->7f, RAM
   for (c = 0; c < 16; c++)
   {
      Memory.Map [c + 0x7e0] = Memory.RAM;
      Memory.Map [c + 0x7f0] = Memory.RAM + 0x10000;
      Memory.BlockIsRAM [c + 0x7e0] = true;
      Memory.BlockIsRAM [c + 0x7f0] = true;
      Memory.BlockIsROM [c + 0x7e0] = false;
      Memory.BlockIsROM [c + 0x7f0] = false;
   }

   // Banks 70->71, S-RAM
   for (c = 0; c < 32; c++)
   {
      Memory.Map [c + 0x700] = Memory.SRAM + (((c >> 4) & 1) << 16);
      Memory.BlockIsRAM [c + 0x700] = true;
      Memory.BlockIsROM [c + 0x700] = false;
   }

   // Replicate the first 2Mb of the ROM at ROM + 2MB such that each 32K
   // block is repeated twice in each 64K block.
#ifdef DS2_DMA
   __dcache_writeback_all();
#endif
   for (c = 0; c < 64; c++)
   {
#ifdef DS2_DMA
      ds2_DMAcopy_32Byte(2 /* channel: emu internal */, &ROM [0x200000 + c * 0x10000],
                         &ROM [c * 0x8000], 0x8000);
      ds2_DMAcopy_32Byte(3 /* channel: emu internal 2 */,
                         &ROM [0x208000 + c * 0x10000], &ROM [c * 0x8000], 0x8000);
      ds2_DMA_wait(2);
      ds2_DMA_wait(3);
      ds2_DMA_stop(2);
      ds2_DMA_stop(3);
#else
      // memmove converted: Different addresses [Neb]
      memcpy(&Memory.ROM [0x200000 + c * 0x10000], &Memory.ROM [c * 0x8000], 0x8000);
      // memmove converted: Different addresses [Neb]
      memcpy(&Memory.ROM [0x208000 + c * 0x10000], &Memory.ROM [c * 0x8000], 0x8000);
#endif
   }

   WriteProtectROM();
}

void SA1ROMMap()
{
   int c;
   int i;

   // Banks 00->3f and 80->bf
   for (c = 0; c < 0x400; c += 16)
   {
      Memory.Map [c + 0] = Memory.Map [c + 0x800] = Memory.RAM;
      Memory.Map [c + 1] = Memory.Map [c + 0x801] = Memory.RAM;
      Memory.BlockIsRAM [c + 0] = Memory.BlockIsRAM [c + 0x800] = true;
      Memory.BlockIsRAM [c + 1] = Memory.BlockIsRAM [c + 0x801] = true;

      Memory.Map [c + 2] = Memory.Map [c + 0x802] = (uint8_t*) MAP_PPU;
      Memory.Map [c + 3] = Memory.Map [c + 0x803] = (uint8_t*) &Memory.FillRAM [0x3000]
                           - 0x3000;
      Memory.Map [c + 4] = Memory.Map [c + 0x804] = (uint8_t*) MAP_CPU;
      Memory.Map [c + 5] = Memory.Map [c + 0x805] = (uint8_t*) MAP_CPU;
      Memory.Map [c + 6] = Memory.Map [c + 0x806] = (uint8_t*) MAP_BWRAM;
      Memory.Map [c + 7] = Memory.Map [c + 0x807] = (uint8_t*) MAP_BWRAM;
      for (i = c + 8; i < c + 16; i++)
      {
         Memory.Map [i] = Memory.Map [i + 0x800] = &Memory.ROM [c << 11] - 0x8000;
         Memory.BlockIsROM [i] = Memory.BlockIsROM [i + 0x800] = true;
      }
   }

   // Banks 40->7f
   for (c = 0; c < 0x400; c += 16)
   {
      for (i = c; i < c + 16; i++)
         Memory.Map [i + 0x400] = (uint8_t*) &Memory.SRAM [(c << 12) & 0x1ffff];

      for (i = c; i < c + 16; i++)
         Memory.BlockIsROM [i + 0x400] = false;
   }

   // c0->ff
   for (c = 0; c < 0x400; c += 16)
   {
      for (i = c;  i < c + 16; i++)
      {
         Memory.Map [i + 0xc00] = &Memory.ROM [(c << 12) % Memory.CalculatedSize];
         Memory.BlockIsROM [i + 0xc00] = true;
      }
   }

   for (c = 0; c < 16; c++)
   {
      Memory.Map [c + 0x7e0] = Memory.RAM;
      Memory.Map [c + 0x7f0] = Memory.RAM + 0x10000;
      Memory.BlockIsRAM [c + 0x7e0] = true;
      Memory.BlockIsRAM [c + 0x7f0] = true;
      Memory.BlockIsROM [c + 0x7e0] = false;
      Memory.BlockIsROM [c + 0x7f0] = false;
   }
   WriteProtectROM();

   // Now copy the map and correct it for the SA1 CPU.
   // memmove converted: Different mallocs [Neb]
   memcpy((void*) SA1.WriteMap, (void*) Memory.WriteMap, sizeof(Memory.WriteMap));
   // memmove converted: Different mallocs [Neb]
   memcpy((void*) SA1.Map, (void*) Memory.Map, sizeof(Memory.Map));

   // Banks 00->3f and 80->bf
   for (c = 0; c < 0x400; c += 16)
   {
      SA1.Map [c + 0] = SA1.Map [c + 0x800] = &Memory.FillRAM [0x3000];
      SA1.Map [c + 1] = SA1.Map [c + 0x801] = (uint8_t*) MAP_NONE;
      SA1.WriteMap [c + 0] = SA1.WriteMap [c + 0x800] = &Memory.FillRAM [0x3000];
      SA1.WriteMap [c + 1] = SA1.WriteMap [c + 0x801] = (uint8_t*) MAP_NONE;
   }

   // Banks 60->6f
   for (c = 0; c < 0x100; c++)
      SA1.Map [c + 0x600] = SA1.WriteMap [c + 0x600] = (uint8_t*) MAP_BWRAM_BITMAP;

   Memory.BWRAM = Memory.SRAM;
}

void LoROM24MBSMap()
{
   int c;
   int i;

   // Banks 00->3f and 80->bf
   for (c = 0; c < 0x400; c += 16)
   {
      Memory.Map [c + 0] = Memory.Map [c + 0x800] = Memory.RAM;
      Memory.Map [c + 1] = Memory.Map [c + 0x801] = Memory.RAM;
      Memory.BlockIsRAM [c + 0] = Memory.BlockIsRAM [c + 0x800] = true;
      Memory.BlockIsRAM [c + 1] = Memory.BlockIsRAM [c + 0x801] = true;

      Memory.Map [c + 2] = Memory.Map [c + 0x802] = (uint8_t*) MAP_PPU;
      Memory.Map [c + 3] = Memory.Map [c + 0x803] = (uint8_t*) MAP_PPU;
      Memory.Map [c + 4] = Memory.Map [c + 0x804] = (uint8_t*) MAP_CPU;
      Memory.Map [c + 5] = Memory.Map [c + 0x805] = (uint8_t*) MAP_CPU;
      Memory.Map [c + 6] = Memory.Map [c + 0x806] = (uint8_t*) MAP_NONE;
      Memory.Map [c + 7] = Memory.Map [c + 0x807] = (uint8_t*) MAP_NONE;

      for (i = c + 8; i < c + 16; i++)
      {
         Memory.Map [i] = Memory.Map [i + 0x800] = &Memory.ROM [c << 11] - 0x8000;
         Memory.BlockIsROM [i] = Memory.BlockIsROM [i + 0x800] = true;
      }
   }

   // Banks 00->3f and 80->bf
   for (c = 0; c < 0x200; c += 16)
   {
      Memory.Map [c + 0x800] = Memory.RAM;
      Memory.Map [c + 0x801] = Memory.RAM;
      Memory.BlockIsRAM [c + 0x800] = true;
      Memory.BlockIsRAM [c + 0x801] = true;

      Memory.Map [c + 0x802] = (uint8_t*) MAP_PPU;
      Memory.Map [c + 0x803] = (uint8_t*) MAP_PPU;
      Memory.Map [c + 0x804] = (uint8_t*) MAP_CPU;
      Memory.Map [c + 0x805] = (uint8_t*) MAP_CPU;
      Memory.Map [c + 0x806] = (uint8_t*) MAP_NONE;
      Memory.Map [c + 0x807] = (uint8_t*) MAP_NONE;

      for (i = c + 8; i < c + 16; i++)
      {
         Memory.Map [i + 0x800] = &Memory.ROM [c << 11] - 0x8000 + 0x200000;
         Memory.BlockIsROM [i + 0x800] = true;
      }
   }

   // Banks 40->7f and c0->ff
   for (c = 0; c < 0x400; c += 16)
   {
      for (i = c; i < c + 8; i++)
         Memory.Map [i + 0x400] = Memory.Map [i + 0xc00] = &Memory.ROM [(c << 11) +
                                  0x200000];

      for (i = c + 8; i < c + 16; i++)
         Memory.Map [i + 0x400] = Memory.Map [i + 0xc00] = &Memory.ROM [(c << 11) +
                                  0x200000 - 0x8000];

      for (i = c; i < c + 16; i++)
         Memory.BlockIsROM [i + 0x400] = Memory.BlockIsROM [i + 0xc00] = true;
   }

   MapExtraRAM();
   WriteProtectROM();
}

void SufamiTurboLoROMMap()
{
   int c;
   int i;

   // Banks 00->3f and 80->bf
   for (c = 0; c < 0x400; c += 16)
   {
      Memory.Map [c + 0] = Memory.Map [c + 0x800] = Memory.RAM;
      Memory.Map [c + 1] = Memory.Map [c + 0x801] = Memory.RAM;
      Memory.BlockIsRAM [c + 0] = Memory.BlockIsRAM [c + 0x800] = true;
      Memory.BlockIsRAM [c + 1] = Memory.BlockIsRAM [c + 0x801] = true;

      Memory.Map [c + 2] = Memory.Map [c + 0x802] = (uint8_t*) MAP_PPU;
      Memory.Map [c + 3] = Memory.Map [c + 0x803] = (uint8_t*) MAP_PPU;
      Memory.Map [c + 4] = Memory.Map [c + 0x804] = (uint8_t*) MAP_CPU;
      Memory.Map [c + 5] = Memory.Map [c + 0x805] = (uint8_t*) MAP_CPU;
      Memory.Map [c + 6] = Memory.Map [c + 0x806] = (uint8_t*) MAP_NONE;
      Memory.Map [c + 7] = Memory.Map [c + 0x807] = (uint8_t*) MAP_NONE;
      for (i = c + 8; i < c + 16; i++)
      {
         Memory.Map [i] = Memory.Map [i + 0x800] = &Memory.ROM [c << 11] - 0x8000;
         Memory.BlockIsROM [i] = Memory.BlockIsROM [i + 0x800] = true;
      }
   }

   // Banks 40->7f and c0->ff
   for (c = 0; c < 0x400; c += 16)
   {
      for (i = c; i < c + 8; i++)
         Memory.Map [i + 0x400] = Memory.Map [i + 0xc00] = &Memory.ROM [(c << 11) +
                                  0x200000];

      for (i = c + 8; i < c + 16; i++)
         Memory.Map [i + 0x400] = Memory.Map [i + 0xc00] = &Memory.ROM [(c << 11) +
                                  0x200000 - 0x8000];

      for (i = c; i < c + 16; i++)
         Memory.BlockIsROM [i + 0x400] = Memory.BlockIsROM [i + 0xc00] = true;
   }

   if (Settings.DSP1Master)
   {
      for (c = 0; c < 0x100; c++)
      {
         Memory.Map [c + 0xe00] = (uint8_t*) MAP_DSP;
         Memory.BlockIsROM [c + 0xe00] = false;
      }
   }

   // Banks 7e->7f, RAM
   for (c = 0; c < 16; c++)
   {
      Memory.Map [c + 0x7e0] = Memory.RAM;
      Memory.Map [c + 0x7f0] = Memory.RAM + 0x10000;
      Memory.BlockIsRAM [c + 0x7e0] = true;
      Memory.BlockIsRAM [c + 0x7f0] = true;
      Memory.BlockIsROM [c + 0x7e0] = false;
      Memory.BlockIsROM [c + 0x7f0] = false;
   }

   // Banks 60->67, S-RAM
   for (c = 0; c < 0x80; c++)
   {
      Memory.Map [c + 0x600] = (uint8_t*) MAP_LOROM_SRAM;
      Memory.BlockIsRAM [c + 0x600] = true;
      Memory.BlockIsROM [c + 0x600] = false;
   }

   WriteProtectROM();
}

#if 0

//untested!!
void SameGameMap()
{
   int i;
   int c;
   int j;

   int mask[4];
   int mask2[4];
   for (j = 0; j < 4; j++)
      mask[j] = mask2[j] = 0x00ff;

   mask[0] = (CalculatedSize / 0x10000) - 1;
   mask2[0] = (Slot1Size / 0x10000) - 1;

   int x;
   bool foundZeros;
   bool pastZeros;

   for (j = 0; j < 3; j++)
   {
      x = 1;
      foundZeros = false;
      pastZeros = false;

      mask[j + 1] = mask[j];

      while (x > 0x100 && !pastZeros)
      {
         if (mask[j]&x)
         {
            x <<= 1;
            if (foundZeros)
               pastZeros = true;
         }
         else
         {
            foundZeros = true;
            pastZeros = false;
            mask[j + 1] |= x;
            x <<= 1;
         }
      }
   }

   for (j = 0; j < 3; j++)
   {
      x = 1;
      foundZeros = false;
      pastZeros = false;

      mask2[j + 1] = mask2[j];

      while (x > 0x100 && !pastZeros)
      {
         if (mask2[j]&x)
         {
            x <<= 1;
            if (foundZeros)
               pastZeros = true;
         }
         else
         {
            foundZeros = true;
            pastZeros = false;
            mask2[j + 1] |= x;
            x <<= 1;
         }
      }
   }


   // Banks 00->3f and 80->bf
   for (c = 0; c < 0x400; c += 16)
   {
      Memory.Map [c + 0] = Memory.Map [c + 0x800] = RAM;
      Memory.BlockIsRAM [c + 0] = Memory.BlockIsRAM [c + 0x800] = true;
      Memory.Map [c + 1] = Memory.Map [c + 0x801] = RAM;
      Memory.BlockIsRAM [c + 1] = Memory.BlockIsRAM [c + 0x801] = true;

      Memory.Map [c + 2] = Memory.Map [c + 0x802] = (uint8_t*) MAP_PPU;
      Memory.Map [c + 3] = Memory.Map [c + 0x803] = (uint8_t*) MAP_PPU;
      Memory.Map [c + 4] = Memory.Map [c + 0x804] = (uint8_t*) MAP_CPU;
      Memory.Map [c + 5] = Memory.Map [c + 0x805] = (uint8_t*) MAP_CPU;

      Memory.Map [c + 6] = Memory.Map [c + 0x806] = (uint8_t*) MAP_NONE;
      Memory.Map [c + 7] = Memory.Map [c + 0x807] = (uint8_t*) MAP_NONE;
   }

   // Banks 30->3f and b0->bf, address ranges 6000->7fff is S-RAM.
   for (c = 0; c < 16; c++)
   {
      Memory.Map [0x306 + (c << 4)] = (uint8_t*) MAP_HIROM_SRAM;
      Memory.Map [0x307 + (c << 4)] = (uint8_t*) MAP_HIROM_SRAM;
      Memory.Map [0xb06 + (c << 4)] = (uint8_t*) MAP_HIROM_SRAM;
      Memory.Map [0xb07 + (c << 4)] = (uint8_t*) MAP_HIROM_SRAM;
      Memory.BlockIsRAM [0x306 + (c << 4)] = true;
      Memory.BlockIsRAM [0x307 + (c << 4)] = true;
      Memory.BlockIsRAM [0xb06 + (c << 4)] = true;
      Memory.BlockIsRAM [0xb07 + (c << 4)] = true;
   }

   for c = 0;
c < 0x200;
c += 16)
{
   for (i = 0; i < 8; i++)
      {
         int e = 3;
         int d = c >> 4;
         while (d > mask[0])
         {
            d &= mask[e];
            e--;
         }

         int f = 3;
         int g = c >> 4;
         while (g > mask2[0])
         {
            g &= mask2[f];
            f--;
         }

         //stuff in HiROM areas
         Memory.Map[c + 0x400 + i] = &ROM[d * 0x10000];
         Memory.Map[c + 0xC00 + i] = &ROM[d * 0x10000];
         //MINI
         Memory.Map[c + 0x600 + i] = &ROMOffset1[g * 0x10000];
         Memory.Map[c + 0xE00 + i] = &ROMOffset1[g * 0x10000];

      }
      for (i = 8; i < 16; i++)
      {
         int e = 3;
         int d = c >> 4;
         while (d > mask[0])
         {
            d &= mask[e];
            e--;
         }

         int f = 3;
         int g = c >> 4;
         while (g > mask2[0])
         {
            g &= mask2[f];
            f--;
         }


         //all stuff
         //BASE
         Memory.Map[c + i] = &ROM[d * 0x10000];
         Memory.Map[c + 0x800 + i] = &ROM[d * 0x10000];
         Memory.Map[c + 0x400 + i] = &ROM[d * 0x10000];
         Memory.Map[c + 0xC00 + i] = &ROM[d * 0x10000];
         //MINI
         Memory.Map[c + 0x200 + i] = &ROMOffset1[g * 0x10000];
         Memory.Map[c + 0xA00 + i] = &ROMOffset1[g * 0x10000];
         Memory.Map[c + 0x600 + i] = &ROMOffset1[g * 0x10000];
         Memory.Map[c + 0xE00 + i] = &ROMOffset1[g * 0x10000];
      }

   }

   int bankmax = 0x40 + (1 << (ROMSize - 6));
   //safety for corrupt headers
   if (bankmax > 128)
   bankmax = 0x80;
   int sum = 0;
   for (i = 0x40; i < bankmax; i++)
{
   uint8_t* bank_low = (uint8_t*)Memory.Map[i << 4];
      for (c = 0; c < 0x10000; c++)
         sum += bank_low[c];
   }
   CalculatedChecksum = sum & 0xFFFF;

   MapRAM();
   WriteProtectROM();
}


//Untested!!
void GNextROMMap()
{
   int c;
   int i;

   // Banks 00->3f and 80->bf
   for (c = 0; c < 0x400; c += 16)
   {
      Memory.Map [c + 0] = Memory.Map [c + 0x800] = RAM;
      Memory.Map [c + 1] = Memory.Map [c + 0x801] = RAM;
      Memory.BlockIsRAM [c + 0] = Memory.BlockIsRAM [c + 0x800] = true;
      Memory.BlockIsRAM [c + 1] = Memory.BlockIsRAM [c + 0x801] = true;

      Memory.Map [c + 2] = Memory.Map [c + 0x802] = (uint8_t*) MAP_PPU;
      Memory.Map [c + 3] = Memory.Map [c + 0x803] = (uint8_t*) &Memory.FillRAM [0x3000]
                           - 0x3000;
      Memory.Map [c + 4] = Memory.Map [c + 0x804] = (uint8_t*) MAP_CPU;
      Memory.Map [c + 5] = Memory.Map [c + 0x805] = (uint8_t*) MAP_CPU;
      Memory.Map [c + 6] = Memory.Map [c + 0x806] = (uint8_t*) MAP_BWRAM;
      Memory.Map [c + 7] = Memory.Map [c + 0x807] = (uint8_t*) MAP_BWRAM;
      for (i = c + 8; i < c + 16; i++)
      {
         Memory.Map [i] = Memory.Map [i + 0x800] = &ROM [c << 11] - 0x8000;
         BlockIsROM [i] = BlockIsROM [i + 0x800] = true;
      }
   }


   // Banks 40->4f (was 7f, but SNES docs and GNext overdumping shows nothing here.)
   for (c = 0; c < 0x100; c += 16)
   {
      for (i = c; i < c + 16; i++)
         Memory.Map [i + 0x400] = (uint8_t*) &SRAM [(c << 12) & 0x1ffff];

      for (i = c; i < c + 16; i++)
         BlockIsROM [i + 0x400] = false;
   }

   for (c = 0; c < 0x100; c += 16)
   {
      for (i = c; i < c + 16; i++)
         Memory.Map [i + 0x700] = (uint8_t*) &ROMOffset1 [(c << 12) & (Slot1Size - 1)];
   }

   // c0->ff
   for (c = 0; c < 0x400; c += 16)
   {
      for (i = c;  i < c + 16; i++)
      {
         Memory.Map [i + 0xc00] = &ROM [(c << 12) % CalculatedSize];
         BlockIsROM [i + 0xc00] = true;
      }
   }

   for (c = 0; c < 16; c++)
   {
      Memory.Map [c + 0x7e0] = RAM;
      Memory.Map [c + 0x7f0] = RAM + 0x10000;
      Memory.BlockIsRAM [c + 0x7e0] = true;
      Memory.BlockIsRAM [c + 0x7f0] = true;
      BlockIsROM [c + 0x7e0] = false;
      BlockIsROM [c + 0x7f0] = false;
   }
   WriteProtectROM();

   // Now copy the map and correct it for the SA1 CPU.
   // memmove converted: Different mallocs [Neb]
   memcpy((void*) SA1.WriteMap, (void*) WriteMap, sizeof(WriteMap));
   // memmove converted: Different mallocs [Neb]
   memcpy((void*) SA1.Map, (void*) Memory.Map, sizeof(Memory.Map));

   // Banks 00->3f and 80->bf
   for (c = 0; c < 0x400; c += 16)
   {
      SA1.Map [c + 0] = SA1.Map [c + 0x800] = &Memory.FillRAM [0x3000];
      SA1.Map [c + 1] = SA1.Map [c + 0x801] = (uint8_t*) MAP_NONE;
      SA1.WriteMap [c + 0] = SA1.WriteMap [c + 0x800] = &Memory.FillRAM [0x3000];
      SA1.WriteMap [c + 1] = SA1.WriteMap [c + 0x801] = (uint8_t*) MAP_NONE;
   }

   // Banks 60->6f
   for (c = 0; c < 0x100; c++)
      SA1.Map [c + 0x600] = SA1.WriteMap [c + 0x600] = (uint8_t*) MAP_BWRAM_BITMAP;

   BWRAM = SRAM;
}

void SufamiTurboAltROMMap()
{
   int c;
   int i;

   if (Slot1Size != 0)
      Slot1SRAMSize = (1 << ((uint8_t)ROMOffset1[0x32])) * 1024;
   else Slot1Size = 0x8000;
   if (Slot2Size != 0)
      Slot2SRAMSize = (1 << ((uint8_t)ROMOffset2[0x32])) * 1024;
   else Slot2Size = 0x8000;

   // Banks 00->3f and 80->bf
   for (c = 0; c < 0x400; c += 16)
   {
      Memory.Map [c + 0] = Memory.Map [c + 0x800] = RAM;
      Memory.Map [c + 1] = Memory.Map [c + 0x801] = RAM;
      Memory.BlockIsRAM [c + 0] = Memory.BlockIsRAM [c + 0x800] = true;
      Memory.BlockIsRAM [c + 1] = Memory.BlockIsRAM [c + 0x801] = true;

      Memory.Map [c + 2] = Memory.Map [c + 0x802] = (uint8_t*) MAP_PPU;
      Memory.Map [c + 3] = Memory.Map [c + 0x803] = (uint8_t*) MAP_PPU;
      Memory.Map [c + 4] = Memory.Map [c + 0x804] = (uint8_t*) MAP_CPU;
      Memory.Map [c + 5] = Memory.Map [c + 0x805] = (uint8_t*) MAP_CPU;
      Memory.Map [c + 6] = Memory.Map [c + 0x806] = (uint8_t*) MAP_NONE;
      Memory.Map [c + 7] = Memory.Map [c + 0x807] = (uint8_t*) MAP_NONE;

      //    for (i = c + 8; i < c + 16; i++)
      //    {
      //       Memory.Map [i] = Memory.Map [i + 0x800] = &ROM [c << 11] - 0x8000;
      //       BlockIsROM [i] = BlockIsROM [i + 0x800] = true;
      //    }

   }

   //Map Bios

   for (c = 0; c < 0x200; c += 16)
   {
      for (i = c + 8; i < c + 16; i++)
      {
         Memory.Map [i] = Memory.Map [i + 0x800] = &ROM [((c >> 4) * 0x8000) %
                          CalculatedSize] - 0x8000;
         BlockIsROM [i] = BlockIsROM [i + 0x800] = true;
      }

   }


   for (c = 0x200; c < 0x400; c += 16)
   {
      for (i = c + 8; i < c + 16; i++)
      {
         if (Slot1Size != 0)
         {
            Memory.Map [i] = Memory.Map [i + 0x800] = &ROMOffset1 [(((
                                c >> 4) * 0x8000) % Slot1Size)] - 0x8000;
            BlockIsROM [i] = BlockIsROM [i + 0x800] = true;
         }
         else Memory.Map [i] = Memory.Map [i + 0x800] = (uint8_t*)MAP_NONE;
      }

   }

   for (c = 0x400; c < 0x600; c += 16)
   {
      for (i = c; i < c + 8; i++)
      {
         if (Slot2Size != 0)
         {
            Memory.Map [i] = Memory.Map [i + 0x800] = &ROMOffset2[(((
                                c >> 4) * 0x8000) % Slot2Size)];
            BlockIsROM [i] = BlockIsROM [i + 0x800] = true;
         }
         else Memory.Map [i] = Memory.Map [i + 0x800] = (uint8_t*)MAP_NONE;

      }
      for (i = c + 8; i < c + 16; i++)
      {
         if (Slot2Size != 0)
         {
            Memory.Map [i] = Memory.Map [i + 0x800] = &ROMOffset2[(((
                                c >> 4) * 0x8000) % Slot2Size)] - 0x8000;
            BlockIsROM [i] = BlockIsROM [i + 0x800] = true;
         }
         else Memory.Map [i] = Memory.Map [i + 0x800] = (uint8_t*)MAP_NONE;

      }

   }

   // Banks 60->67 (7F?), S-RAM
   if (Slot1SRAMSize != 0)
   {
      for (c = 0; c < 0x100; c++)
      {
         Memory.Map [c + 0xE00] = Memory.Map [c + 0x600] = (uint8_t*) MAP_LOROM_SRAM;
         Memory.BlockIsRAM [c + 0xE00] = Memory.BlockIsRAM [c + 0x600] = true;
         BlockIsROM [c + 0xE00] = BlockIsROM [c + 0x600] = false;
      }
   }
   if (Slot2SRAMSize != 0)
   {
      for (c = 0; c < 0x100; c++)
      {
         Memory.Map [c + 0xF00] = Memory.Map [c + 0x700] = (uint8_t*) MAP_LOROM_SRAM;
         Memory.BlockIsRAM [c + 0xF00] = Memory.BlockIsRAM [c + 0x700] = true;
         BlockIsROM [c + 0xF00] = BlockIsROM [c + 0x700] = false;
      }
   }

   // Banks 7e->7f, RAM
   for (c = 0; c < 16; c++)
   {
      Memory.Map [c + 0x7e0] = RAM;
      Memory.Map [c + 0x7f0] = RAM + 0x10000;
      Memory.BlockIsRAM [c + 0x7e0] = true;
      Memory.BlockIsRAM [c + 0x7f0] = true;
      BlockIsROM [c + 0x7e0] = false;
      BlockIsROM [c + 0x7f0] = false;
   }

   WriteProtectROM();
}
#endif


void SRAM512KLoROMMap()
{
   int c;
   int i;

   // Banks 00->3f and 80->bf
   for (c = 0; c < 0x400; c += 16)
   {
      Memory.Map [c + 0] = Memory.Map [c + 0x800] = Memory.RAM;
      Memory.Map [c + 1] = Memory.Map [c + 0x801] = Memory.RAM;
      Memory.BlockIsRAM [c + 0] = Memory.BlockIsRAM [c + 0x800] = true;
      Memory.BlockIsRAM [c + 1] = Memory.BlockIsRAM [c + 0x801] = true;

      Memory.Map [c + 2] = Memory.Map [c + 0x802] = (uint8_t*) MAP_PPU;
      Memory.Map [c + 3] = Memory.Map [c + 0x803] = (uint8_t*) MAP_PPU;
      Memory.Map [c + 4] = Memory.Map [c + 0x804] = (uint8_t*) MAP_CPU;
      Memory.Map [c + 5] = Memory.Map [c + 0x805] = (uint8_t*) MAP_CPU;
      Memory.Map [c + 6] = Memory.Map [c + 0x806] = (uint8_t*) MAP_NONE;
      Memory.Map [c + 7] = Memory.Map [c + 0x807] = (uint8_t*) MAP_NONE;

      for (i = c + 8; i < c + 16; i++)
      {
         Memory.Map [i] = Memory.Map [i + 0x800] = &Memory.ROM [c << 11] - 0x8000;
         Memory.BlockIsROM [i] = Memory.BlockIsROM [i + 0x800] = true;
      }
   }

   // Banks 40->7f and c0->ff
   for (c = 0; c < 0x400; c += 16)
   {
      for (i = c; i < c + 8; i++)
         Memory.Map [i + 0x400] = Memory.Map [i + 0xc00] = &Memory.ROM [(c << 11) +
                                  0x200000];

      for (i = c + 8; i < c + 16; i++)
         Memory.Map [i + 0x400] = Memory.Map [i + 0xc00] = &Memory.ROM [(c << 11) +
                                  0x200000 - 0x8000];

      for (i = c; i < c + 16; i++)
         Memory.BlockIsROM [i + 0x400] = Memory.BlockIsROM [i + 0xc00] = true;
   }

   MapExtraRAM();
   WriteProtectROM();
}

void BSHiROMMap()
{
   int c;
   int i;

   Memory.SRAMSize = 5;

   // Banks 00->3f and 80->bf
   for (c = 0; c < 0x400; c += 16)
   {
      Memory.Map [c + 0] = Memory.Map [c + 0x800] = Memory.RAM;
      Memory.BlockIsRAM [c + 0] = Memory.BlockIsRAM [c + 0x800] = true;
      Memory.Map [c + 1] = Memory.Map [c + 0x801] = Memory.RAM;
      Memory.BlockIsRAM [c + 1] = Memory.BlockIsRAM [c + 0x801] = true;

      Memory.Map [c + 2] = Memory.Map [c + 0x802] = (uint8_t*) MAP_PPU;
      Memory.Map [c + 3] = Memory.Map [c + 0x803] = (uint8_t*) MAP_PPU;
      Memory.Map [c + 4] = Memory.Map [c + 0x804] = (uint8_t*) MAP_CPU;
      // XXX: How large is SRAM??
      Memory.Map [c + 5] = Memory.Map [c + 0x805] = (uint8_t*) Memory.RAM;
      //    Memory.Map [c + 5] = Memory.Map [c + 0x805] = (uint8_t *) SRAM;
      Memory.BlockIsRAM [c + 5] = Memory.BlockIsRAM [c + 0x805] = true;
      //    Memory.Map [c + 6] = Memory.Map [c + 0x806] = (uint8_t *) MAP_NONE;
      //    Memory.Map [c + 7] = Memory.Map [c + 0x807] = (uint8_t *) MAP_NONE;

      Memory.Map [c + 6] = Memory.Map [c + 0x806] = (uint8_t*) Memory.RAM;
      //    Memory.Map [c + 5] = Memory.Map [c + 0x805] = (uint8_t *) SRAM;
      Memory.BlockIsRAM [c + 6] = Memory.BlockIsRAM [c + 0x806] = true;
      Memory.Map [c + 7] = Memory.Map [c + 0x807] = (uint8_t*) Memory.RAM;
      //    Memory.Map [c + 5] = Memory.Map [c + 0x805] = (uint8_t *) Memory.SRAM;
      Memory.BlockIsRAM [c + 7] = Memory.BlockIsRAM [c + 0x807] = true;

      for (i = c + 8; i < c + 16; i++)
      {
         Memory.Map [i] = Memory.Map [i + 0x800] = &Memory.ROM [(c << 12) %
                          Memory.CalculatedSize];
         Memory.BlockIsROM [i] = Memory.BlockIsROM [i + 0x800] = true;
      }
   }

   // Banks 60->7d offset 0000->7fff & 60->7f offset 8000->ffff PSRAM
   // XXX: How large is PSRAM?

   //not adjusted, but The Dumper says "4 Mbits"
   for (c = 0x600; c < 0x7e0; c += 16)
   {
      for (i = c; i < c + 8; i++)
      {
         Memory.Map [i] = &Memory.ROM [0x400000 + (c << 11)];
         Memory.BlockIsRAM [i] = true;
      }
      for (i = c + 8; i < c + 16; i++)
      {
         Memory.Map [i] = &Memory.ROM [0x400000 + (c << 11) - 0x8000];
         Memory.BlockIsRAM [i] = true;
      }
   }

   // Banks 40->7f and c0->ff
   for (c = 0; c < 0x400; c += 16)
   {
      for (i = c; i < c + 16; i++)
      {
         Memory.Map [i + 0x400] = Memory.Map [i + 0xc00] = &Memory.ROM [(c << 12) %
                                  Memory.CalculatedSize];
         Memory.BlockIsROM [i + 0x400] = Memory.BlockIsROM [i + 0xc00] = true;
      }
   }
   for (i = 0; i < 0x80; i++)
   {
      Memory.Map[0x700 + i] = &Memory.BSRAM[0x10000 * (i / 16)];
      Memory.BlockIsRAM[0x700 + i] = true;
      Memory.BlockIsROM[0x700 + i] = false;
   }
   for (i = 0; i < 8; i++)
   {
      Memory.Map[0x205 + (i << 4)] = Memory.Map[0x285 + (i << 4)] = Memory.Map[0x305
                                     + (i << 4)] = Memory.Map[0x385 + (i << 4)] = Memory.Map[0x705 + (i << 4)];
      Memory.BlockIsRAM[0x205 + (i << 4)] = Memory.BlockIsRAM[0x285 +
                                            (i << 4)] = Memory.BlockIsRAM[0x305 + (i << 4)] = Memory.BlockIsRAM[0x385 +
                                                  (i << 4)] = true;
      Memory.BlockIsROM[0x205 + (i << 4)] = Memory.BlockIsROM[0x285 +
                                            (i << 4)] = Memory.BlockIsROM[0x305 + (i << 4)] = Memory.BlockIsROM[0x385 +
                                                  (i << 4)] = false;
   }

   MapRAM();
   WriteProtectROM();
}

void JumboLoROMMap(bool Interleaved)
{
   int c;
   int i;

   uint32_t OFFSET0 = 0x400000;
//   uint32_t OFFSET1 = 0x400000;
   uint32_t OFFSET2 = 0x000000;

   if (Interleaved)
   {
      OFFSET0 = 0x000000;
//      OFFSET1 = 0x000000;
      OFFSET2 = Memory.CalculatedSize -
                0x400000; //changed to work with interleaved DKJM2.
   }
   // Banks 00->3f and 80->bf
   for (c = 0; c < 0x400; c += 16)
   {
      Memory.Map [c + 0] = Memory.Map [c + 0x800] = Memory.RAM;
      Memory.Map [c + 1] = Memory.Map [c + 0x801] = Memory.RAM;
      Memory.BlockIsRAM [c + 0] = Memory.BlockIsRAM [c + 0x800] = true;
      Memory.BlockIsRAM [c + 1] = Memory.BlockIsRAM [c + 0x801] = true;

      Memory.Map [c + 2] = Memory.Map [c + 0x802] = (uint8_t*) MAP_PPU;
      Memory.Map [c + 3] = Memory.Map [c + 0x803] = (uint8_t*) MAP_PPU;
      Memory.Map [c + 4] = Memory.Map [c + 0x804] = (uint8_t*) MAP_CPU;
      Memory.Map [c + 5] = Memory.Map [c + 0x805] = (uint8_t*) MAP_CPU;
      if (Settings.DSP1Master)
      {
         Memory.Map [c + 6] = Memory.Map [c + 0x806] = (uint8_t*) MAP_DSP;
         Memory.Map [c + 7] = Memory.Map [c + 0x807] = (uint8_t*) MAP_DSP;
      }
      else if (Settings.C4)
      {
         Memory.Map [c + 6] = Memory.Map [c + 0x806] = (uint8_t*) MAP_C4;
         Memory.Map [c + 7] = Memory.Map [c + 0x807] = (uint8_t*) MAP_C4;
      }
      else
      {
         Memory.Map [c + 6] = Memory.Map [c + 0x806] = (uint8_t*) bytes0x2000 - 0x6000;
         Memory.Map [c + 7] = Memory.Map [c + 0x807] = (uint8_t*) bytes0x2000 - 0x6000;
      }

      for (i = c + 8; i < c + 16; i++)
      {
         Memory.Map [i] = &Memory.ROM [((c << 11) % (Memory.CalculatedSize - 0x400000)) +
                                       OFFSET0] - 0x8000;
         Memory.Map [i + 0x800] = &Memory.ROM [((c << 11) % (0x400000)) + OFFSET2] -
                                  0x8000;
         Memory.BlockIsROM [i + 0x800] = Memory.BlockIsROM [i] = true;
      }
   }

   if (Settings.DSP1Master)
   {
      // Banks 30->3f and b0->bf
      for (c = 0x300; c < 0x400; c += 16)
      {
         for (i = c + 8; i < c + 16; i++)
         {
            Memory.Map [i + 0x800] = (uint8_t*) MAP_DSP;
            Memory.BlockIsROM [i] = Memory.BlockIsROM [i + 0x800] = false;
         }
      }
   }

   // Banks 40->7f and c0->ff
   for (c = 0x400; c < 0x800; c += 16)
   {
      //updated mappings to correct A15 mirroring
      for (i = c; i < c + 8; i++)
      {
         Memory.Map [i] = &Memory.ROM [((c << 11) % (Memory.CalculatedSize - 0x400000)) +
                                       OFFSET0];
         Memory.Map [i + 0x800] = &Memory.ROM [((c << 11) % 0x400000) + OFFSET2];
      }

      for (i = c + 8; i < c + 16; i++)
      {
         Memory.Map [i] = &Memory.ROM [((c << 11) % (Memory.CalculatedSize - 0x400000)) +
                                       OFFSET0] - 0x8000;
         Memory.Map [i + 0x800] = &Memory.ROM [((c << 11) % 0x400000) + OFFSET2 ] -
                                  0x8000;
      }

      for (i = c; i < c + 16; i++)
         Memory.BlockIsROM [i] = Memory.BlockIsROM [i + 0x800] = true;
   }

   //ROM type has to be 64 Mbit header!
   int sum = 0, k, l;
   for (k = 0; k < 256; k++)
   {
      uint8_t* bank = 0x8000 + Memory.Map[8 + (k <<
                                             4)]; //use upper half of the banks, and adjust for LoROM.
      for (l = 0; l < 0x8000; l++)
         sum += bank[l];
   }
   Memory.CalculatedChecksum = sum & 0xFFFF;

   MapRAM();
   WriteProtectROM();
}

void SPC7110HiROMMap()
{
   int c;
   int i;

   // Banks 00->3f and 80->bf
   for (c = 0; c < 0x400; c += 16)
   {
      Memory.Map [c + 0] = Memory.Map [c + 0x800] = Memory.RAM;
      Memory.BlockIsRAM [c + 0] = Memory.BlockIsRAM [c + 0x800] = true;
      Memory.Map [c + 1] = Memory.Map [c + 0x801] = Memory.RAM;
      Memory.BlockIsRAM [c + 1] = Memory.BlockIsRAM [c + 0x801] = true;

      Memory.Map [c + 2] = Memory.Map [c + 0x802] = (uint8_t*) MAP_PPU;
      Memory.Map [c + 3] = Memory.Map [c + 0x803] = (uint8_t*) MAP_PPU;
      Memory.Map [c + 4] = Memory.Map [c + 0x804] = (uint8_t*) MAP_CPU;
      Memory.Map [c + 5] = Memory.Map [c + 0x805] = (uint8_t*) MAP_CPU;

      Memory.Map [c + 6] /*= Memory.Map [c + 0x806]*/ = (uint8_t*) MAP_HIROM_SRAM;
      Memory.Map [c + 7] /*= Memory.Map [c + 0x807]*/ = (uint8_t*) MAP_HIROM_SRAM;
      Memory.Map [c + 0x806] = Memory.Map [c + 0x807] = (uint8_t*) MAP_NONE;

      for (i = c + 8; i < c + 16; i++)
      {
         Memory.Map [i] = Memory.Map [i + 0x800] = &Memory.ROM [(c << 12) %
                          Memory.CalculatedSize];
         Memory.BlockIsROM [i] = Memory.BlockIsROM [i + 0x800] = true;
      }
   }

   // Banks 30->3f and b0->bf, address ranges 6000->7fff is S-RAM.
   for (c = 0; c < 16; c++)
   {
      Memory.Map [0x306 + (c << 4)] = (uint8_t*) MAP_HIROM_SRAM;
      Memory.Map [0x307 + (c << 4)] = (uint8_t*) MAP_HIROM_SRAM;
      Memory.Map [0xb06 + (c << 4)] = (uint8_t*) MAP_NONE;
      Memory.Map [0xb07 + (c << 4)] = (uint8_t*) MAP_NONE;
      Memory.BlockIsRAM [0x306 + (c << 4)] = true;
      Memory.BlockIsRAM [0x307 + (c << 4)] = true;
      // Memory.BlockIsRAM [0xb06 + (c << 4)] = true;
      // Memory.BlockIsRAM [0xb07 + (c << 4)] = true;
   }

   // Banks 40->7f and c0->ff
   for (c = 0; c < 0x400; c += 16)
   {
      for (i = c; i < c + 16; i++)
      {
         Memory.Map [i + 0x400] = Memory.Map [i + 0xc00] = &Memory.ROM [(c << 12) %
                                  Memory.CalculatedSize];
         Memory.BlockIsROM [i + 0x400] = Memory.BlockIsROM [i + 0xc00] = true;
      }
   }

   for (c = 0; c < 0x10; c++)
   {
      Memory.Map [0x500 + c] = (uint8_t*)MAP_SPC7110_DRAM;
      Memory.BlockIsROM [0x500 + c] = true;
   }

   for (c = 0; c < 0x100; c++)
   {
      Memory.Map [0xD00 + c] = (uint8_t*) MAP_SPC7110_ROM;
      Memory.Map [0xE00 + c] = (uint8_t*) MAP_SPC7110_ROM;
      Memory.Map [0xF00 + c] = (uint8_t*) MAP_SPC7110_ROM;
      Memory.BlockIsROM [0xD00 + c] = Memory.BlockIsROM [0xE00 + c] =
                                         Memory.BlockIsROM [0xF00 + c] = true;

   }
   S9xSpc7110Init();

   int sum = 0;
   for (i = 0; i < (int)Memory.CalculatedSize; i++)
      sum += Memory.ROM[i];

   if (Memory.CalculatedSize == 0x300000)
      sum <<= 1;
   Memory.CalculatedChecksum = sum & 0xFFFF;

   MapRAM();
   WriteProtectROM();
}
void SPC7110Sram(uint8_t newstate)
{
   if (newstate & 0x80)
   {
      Memory.Map[6] = (uint8_t*)MAP_HIROM_SRAM;
      Memory.Map[7] = (uint8_t*)MAP_HIROM_SRAM;
      Memory.Map[0x306] = (uint8_t*)MAP_HIROM_SRAM;
      Memory.Map[0x307] = (uint8_t*)MAP_HIROM_SRAM;


   }
   else
   {
      Memory.Map[6] = (uint8_t*)MAP_RONLY_SRAM;
      Memory.Map[7] = (uint8_t*)MAP_RONLY_SRAM;
      Memory.Map[0x306] = (uint8_t*)MAP_RONLY_SRAM;
      Memory.Map[0x307] = (uint8_t*)MAP_RONLY_SRAM;
   }
}
const char* TVStandard()
{
   return (Settings.PAL ? "PAL" : "NTSC");
}

const char* Speed()
{
   return (Memory.ROMSpeed & 0x10 ? "120ns" : "200ns");
}

const char* MapType()
{
   return (Memory.HiROM ? "HiROM" : "LoROM");
}

const char* StaticRAMSize()
{
   static char tmp [20];

   if (Memory.SRAMSize > 16)
      return ("Corrupt");
   sprintf(tmp, "%dKB", (Memory.SRAMMask + 1) / 1024);
   return (tmp);
}

const char* Size()
{
   static char tmp [20];

   if (Memory.ROMSize < 7 || Memory.ROMSize - 7 > 23)
      return ("Corrupt");
   sprintf(tmp, "%dMbits", 1 << (Memory.ROMSize - 7));
   return (tmp);
}

const char* KartContents()
{
   static char tmp [30];
   static const char* CoPro [16] =
   {
      "DSP1", "SuperFX", "OBC1", "SA-1", "S-DD1", "S-RTC", "CoPro#6",
      "CoPro#7", "CoPro#8", "CoPro#9", "CoPro#10", "CoPro#11", "CoPro#12",
      "CoPro#13", "CoPro#14", "CoPro-Custom"
   };
   static const char* Contents [3] =
   {
      "ROM", "ROM+RAM", "ROM+RAM+BAT"
   };
   if (Memory.ROMType == 0 && !Settings.BS)
      return ("ROM only");

   sprintf(tmp, "%s", Contents [(Memory.ROMType & 0xf) % 3]);


   if (Settings.BS)
      sprintf(tmp, "%s+%s", tmp, "BSX");
   else if (Settings.SPC7110 && Settings.SPC7110RTC)
      sprintf(tmp, "%s+%s", tmp, "SPC7110+RTC");
   else if (Settings.SPC7110)
      sprintf(tmp, "%s+%s", tmp, "SPC7110");
   else if (Settings.SETA != 0)
   {
      switch (Settings.SETA)
      {
      case ST_010:
         sprintf(tmp, "%s+%s", tmp, "ST-010");
         break;
      case ST_011:
         sprintf(tmp, "%s+%s", tmp, "ST-011");
         break;

      case ST_018:
         sprintf(tmp, "%s+%s", tmp, "ST-018");
         break;

      }
   }
   else if ((Memory.ROMType & 0xf) >= 3)
      sprintf(tmp, "%s+%s", tmp, CoPro [(Memory.ROMType & 0xf0) >> 4]);

   return (tmp);
}

const char* MapMode()
{
   static char tmp [4];
   sprintf(tmp, "%02x", Memory.ROMSpeed & ~0x10);
   return (tmp);
}

const char* ROMID()
{
   return (Memory.ROMId);
}

void ApplyROMFixes()
{
#ifdef __W32_HEAP
   if (_HEAPOK != _heapchk())
      MessageBox(GUI.hWnd, "ApplyROMFixes", "Heap Corrupt", MB_OK);
#endif

   //don't steal my work! -MK
   if (Memory.ROMCRC32 == 0x1B4A5616
         && strncmp(Memory.ROMName, "RUDORA NO HIHOU", 15) == 0)
   {
      strncpy(Memory.ROMName, "THIS SCRIPT WAS STOLEN", 22);
      Settings.DisplayColor = BUILD_PIXEL(31, 0, 0);
      SET_UI_COLOR(255, 0, 0);
   }

   /*
   HACKS NSRT can fix that we hadn't detected before.
   [14:25:13] <@Nach>     case 0x0c572ef0: //So called Hook (US)(2648)
   [14:25:13] <@Nach>     case 0x6810aa95: //Bazooka Blitzkreig swapped sizes hack -handled
   [14:25:17] <@Nach>     case 0x61E29C06: //The Tick region hack
   [14:25:19] <@Nach>     case 0x1EF90F74: //Jikkyou Keiba Simulation Stable Star PAL hack
   [14:25:23] <@Nach>     case 0x4ab225b5: //So called Krusty's Super Fun House (E)
   [14:25:25] <@Nach>     case 0x77fd806a: //Donkey Kong Country 2 (E) v1.1 bad dump -handled
   [14:25:27] <@Nach>     case 0x340f23e5: //Donkey Kong Country 3 (U) copier hack - handled
   */

   if (Memory.ROMCRC32 == 0x6810aa95 || Memory.ROMCRC32 == 0x340f23e5
         || Memory.ROMCRC32 == 0x77fd806a ||
         strncmp(Memory.ROMName, "HIGHWAY BATTLE 2", 16) == 0 ||
         (strcmp(Memory.ROMName, "FX SKIING NINTENDO 96") == 0
          && Memory.ROM[0x7FDA] == 0))
   {
      Settings.DisplayColor = BUILD_PIXEL(31, 0, 0);
      SET_UI_COLOR(255, 0, 0);
   }

   //Ambiguous chip function pointer assignments

   //DSP switching:
   if (strncmp(Memory.ROMName, "DUNGEON MASTER", 14) == 0)
   {
      //Set DSP-2
      SetDSP = &DSP2SetByte;
      GetDSP = &DSP2GetByte;
   }

#ifdef DSP_DUMMY_LOOPS
   if (strncmp(ROMName, "SD\x0b6\x0de\x0dd\x0c0\x0de\x0d1GX", 10) == 0)
   {
      //Set DSP-3
      SetDSP = &DSP3SetByte;
      GetDSP = &DSP3GetByte;
   }
#endif

   if (strncmp(Memory.ROMName, "TOP GEAR 3000", 13) == 0
         || strncmp(Memory.ROMName, "PLANETS CHAMP TG3000", 20) == 0)
   {
      //Set DSP-4
      SetDSP = &DSP4SetByte;
      GetDSP = &DSP4GetByte;
   }

   //memory map corrections
   if (strncmp(Memory.ROMName, "XBAND", 5) == 0)
   {
      int c;
      for (c = 0xE00; c < 0xE10; c++)
      {
         Memory.Map [c] = (uint8_t*) MAP_LOROM_SRAM;
         Memory.BlockIsRAM [c] = true;
         Memory.BlockIsROM [c] = false;
      }
      WriteProtectROM();
   }

   //not MAD-1 compliant
   if (strcmp(Memory.ROMName, "WANDERERS FROM YS") == 0)
   {
      int c;
      for (c = 0; c < 0xE0; c++)
      {
         Memory.Map[c + 0x700] = (uint8_t*)MAP_LOROM_SRAM;
         Memory.BlockIsROM[c + 0x700] = false;
         Memory.BlockIsRAM[c + 0x700] = true;
      }
      WriteProtectROM();
   }

   if (strcmp(Memory.ROMName, "GOGO ACKMAN3") == 0 ||
         strcmp(Memory.ROMName, "HOME ALONE") == 0)
   {
      // Banks 00->3f and 80->bf
      int c;
      for (c = 0; c < 0x400; c += 16)
      {
         Memory.Map [c + 6] = Memory.Map [c + 0x806] = Memory.SRAM;
         Memory.Map [c + 7] = Memory.Map [c + 0x807] = Memory.SRAM;
         Memory.BlockIsROM [c + 6] = Memory.BlockIsROM [c + 0x806] = false;
         Memory.BlockIsROM [c + 7] = Memory.BlockIsROM [c + 0x807] = false;
         Memory.BlockIsRAM [c + 6] = Memory.BlockIsRAM [c + 0x806] = true;
         Memory.BlockIsRAM [c + 7] = Memory.BlockIsRAM [c + 0x807] = true;
      }
      WriteProtectROM();
   }

   if (strcmp(Memory.ROMName, "RADICAL DREAMERS") == 0 ||
         strcmp(Memory.ROMName, "TREASURE CONFLIX") == 0)
   {
      int c;

      for (c = 0; c < 0x80; c++)
      {
         Memory.Map [c + 0x700] = Memory.ROM + 0x200000 + 0x1000 * (c & 0xf0);
         Memory.BlockIsRAM [c + 0x700] = true;
         Memory.BlockIsROM [c + 0x700] = false;
      }
      for (c = 0; c < 0x400; c += 16)
      {
         Memory.Map [c + 5] = Memory.Map [c + 0x805] = Memory.ROM + 0x300000;
         Memory.BlockIsRAM [c + 5] = Memory.BlockIsRAM [c + 0x805] = true;
      }
      WriteProtectROM();
   }

   if (strncmp(Memory.ROMName, "WAR 2410", 8) == 0)
   {
      Memory.Map [0x005] = (uint8_t*) Memory.RAM;
      Memory.BlockIsRAM [0x005] = true;
      Memory.BlockIsROM [0x005] = false;
   }

   if (strcmp(Memory.ROMName, "BATMAN--REVENGE JOKER") == 0)
   {
      Memory.HiROM = false;
      Memory.LoROM = true;
      LoROMMap();
   }


   //NMI hacks
   CPU.NMITriggerPoint = 4;
   if (strcmp(Memory.ROMName, "CACOMA KNIGHT") == 0)
      CPU.NMITriggerPoint = 25;

   //Disabling a speed-up
   // Games which spool sound samples between the SNES and sound CPU using
   // H-DMA as the sample is playing.
   if (strcmp(Memory.ROMName, "EARTHWORM JIM 2") == 0 ||
         strcmp(Memory.ROMName, "PRIMAL RAGE") == 0 ||
         strcmp(Memory.ROMName, "CLAY FIGHTER") == 0 ||
         strcmp(Memory.ROMName, "ClayFighter 2") == 0 ||
         strncasecmp(Memory.ROMName, "MADDEN", 6) == 0 ||
         strncmp(Memory.ROMName, "NHL", 3) == 0 ||
         strcmp(Memory.ROMName, "WeaponLord") == 0 ||
         strncmp(Memory.ROMName, "WAR 2410", 8) == 0)
      Settings.Shutdown = false;


   //APU timing hacks

#ifndef USE_BLARGG_APU
   // Stunt Racer FX
   if (strcmp(Memory.ROMId, "CQ  ") == 0 ||
         // Illusion of Gaia
         strncmp(Memory.ROMId, "JG", 2) == 0 ||
         strcmp(Memory.ROMName, "GAIA GENSOUKI 1 JPN") == 0)
      IAPU.OneCycle = 13;


   // RENDERING RANGER R2
   if (strcmp(Memory.ROMId, "AVCJ") == 0 ||
         //Mark Davis
         strncmp(Memory.ROMName, "THE FISHING MASTER", 18) == 0
         || //needs >= actual APU timing. (21 is .002 Mhz slower)
         // Star Ocean
         strncmp(Memory.ROMId, "ARF", 3) == 0 ||
         // Tales of Phantasia
         strncmp(Memory.ROMId, "ATV", 3) == 0 ||
         // Act Raiser 1 & 2
         strncasecmp(Memory.ROMName, "ActRaiser", 9) == 0 ||
         // Soulblazer
         strcmp(Memory.ROMName, "SOULBLAZER - 1 USA") == 0 ||
         strcmp(Memory.ROMName, "SOULBLADER - 1") == 0 ||

         // Terranigma
         strncmp(Memory.ROMId, "AQT", 3) == 0 ||
         // Robotrek
         strncmp(Memory.ROMId, "E9 ", 3) == 0 ||
         strcmp(Memory.ROMName, "SLAP STICK 1 JPN") == 0 ||
         // ZENNIHON PURORESU2
         strncmp(Memory.ROMId, "APR", 3) == 0 ||
         // Bomberman 4
         strncmp(Memory.ROMId, "A4B", 3) == 0 ||
         // UFO KAMEN YAKISOBAN
         strncmp(Memory.ROMId, "Y7 ", 3) == 0 ||
         strncmp(Memory.ROMId, "Y9 ", 3) == 0 ||
         // Panic Bomber World
         strncmp(Memory.ROMId, "APB", 3) == 0 ||
         ((strncmp(Memory.ROMName, "Parlor", 6) == 0 ||
           strcmp(Memory.ROMName, "HEIWA Parlor!Mini8") == 0 ||
           strncmp(Memory.ROMName, "SANKYO Fever! ̨��ް!", 21) == 0) &&
          strcmp(Memory.CompanyId, "A0") == 0) ||
         strcmp(Memory.ROMName, "DARK KINGDOM") == 0 ||
         strcmp(Memory.ROMName, "ZAN3 SFC") == 0 ||
         strcmp(Memory.ROMName, "HIOUDEN") == 0 ||
         strcmp(Memory.ROMName, "�ݼɳ�") == 0 ||   //Tenshi no Uta
         strcmp(Memory.ROMName, "FORTUNE QUEST") == 0 ||
         strcmp(Memory.ROMName, "FISHING TO BASSING") == 0 ||
         strncmp(Memory.ROMName, "TokyoDome '95Battle 7", 21) == 0 ||
         strcmp(Memory.ROMName, "OHMONO BLACKBASS") == 0 ||
         strncmp(Memory.ROMName, "SWORD WORLD SFC", 15) == 0 ||
         strcmp(Memory.ROMName, "MASTERS") == 0 || //Augusta 2 J
         strcmp(Memory.ROMName, "SFC ���ײ�ް") == 0
         ||  //Kamen Rider
         strncmp(Memory.ROMName, "LETs PACHINKO(", 14) == 0)  //A set of BS games
      IAPU.OneCycle = 15;
#endif

   //Specific game fixes

   Settings.StarfoxHack = strcmp(Memory.ROMName, "STAR FOX") == 0 ||
                          strcmp(Memory.ROMName, "STAR WING") == 0;
   Settings.WinterGold = strcmp(Memory.ROMName, "FX SKIING NINTENDO 96") == 0 ||
                         strcmp(Memory.ROMName, "DIRT RACER") == 0 ||
                         Settings.StarfoxHack;


   if ((strcmp(Memory.ROMName, "LEGEND") == 0 && !Settings.PAL) ||
         strcmp(Memory.ROMName, "King Arthurs World") == 0)
      SNESGameFixes.EchoOnlyOutput = true;


   Settings.DaffyDuck = (strcmp(Memory.ROMName, "DAFFY DUCK: MARV MISS") == 0) ||
                        (strcmp(Memory.ROMName, "ROBOCOP VS THE TERMIN") == 0) ||
                        (strcmp(Memory.ROMName, "ROBOCOP VS TERMINATOR") == 0);  //ROBOCOP VS THE TERMIN
   Settings.HBlankStart = (256 * Settings.H_Max) / SNES_HCOUNTER_MAX;

   //OAM hacks because we don't fully understand the
   //behavior of the SNES.

   //Totally wacky display...
   //seems to need a disproven behavior, so
   //we're definitely overlooking some other bug?
   if (strncmp(Memory.ROMName, "UNIRACERS", 9) == 0)
      SNESGameFixes.Uniracers = true;


   //is this even useful now?
   if (strcmp(Memory.ROMName, "ALIENS vs. PREDATOR") == 0)
      SNESGameFixes.alienVSpredetorFix = true;

   if (strcmp(Memory.ROMName, "���̧߰н�") == 0
         ||   //Super Famista
         strcmp(Memory.ROMName, "���̧߰н� 2") == 0
         ||  //Super Famista 2
         strcmp(Memory.ROMName, "ZENKI TENCHIMEIDOU") == 0 ||
         strcmp(Memory.ROMName, "GANBA LEAGUE") == 0)
      SNESGameFixes.APU_OutPorts_ReturnValueFix = true;

   if (strcmp(Memory.ROMName, "FURAI NO SIREN") == 0)
      SNESGameFixes.SoundEnvelopeHeightReading2 = true;

   //CPU timing hacks
   Settings.H_Max = (SNES_CYCLES_PER_SCANLINE *
                     Settings.CyclesPercentage) / 100;

   //no need to ifdef for right now...
   //#ifdef HDMA_HACKS

   // A Couple of HDMA related hacks - Lantus
   if ((strcmp(Memory.ROMName, "SFX SUPERBUTOUDEN2") == 0) ||
         (strcmp(Memory.ROMName, "ALIEN vs. PREDATOR") == 0) ||
         (strcmp(Memory.ROMName, "STONE PROTECTORS") == 0) ||
         (strcmp(Memory.ROMName, "SUPER BATTLETANK 2") == 0))
      Settings.H_Max = (SNES_CYCLES_PER_SCANLINE * 130) / 100;

   if (strcmp(Memory.ROMName, "HOME IMPROVEMENT") == 0)
      Settings.H_Max = (SNES_CYCLES_PER_SCANLINE * 200) / 100;

   // End HDMA hacks
   //#endif


   if (strcmp(Memory.ROMId, "ASRJ") == 0 && Settings.CyclesPercentage == 100)
      // Street Racer
      Settings.H_Max = (SNES_CYCLES_PER_SCANLINE * 95) / 100;

   // Power Rangers Fight
   if (strncmp(Memory.ROMId, "A3R", 3) == 0 ||
         // Clock Tower
         strncmp(Memory.ROMId, "AJE", 3) == 0)
      Settings.H_Max = (SNES_CYCLES_PER_SCANLINE * 103) / 100;


   if (strncmp(Memory.ROMId, "A3M", 3) == 0 && Settings.CyclesPercentage == 100)
      // Mortal Kombat 3. Fixes cut off speech sample
      Settings.H_Max = (SNES_CYCLES_PER_SCANLINE * 110) / 100;

   //Darkness Beyond Twilight
   //Crimson beyond blood that flows
   //buried in the stream of time
   //is where your power grows
   //I pledge myself to conquer
   //all the foes who stand
   //before the might gift betsowed
   //in my unworthy hand
   if (strcmp(Memory.ROMName, "\x0bd\x0da\x0b2\x0d4\x0b0\x0bd\x0de") == 0 &&
         Settings.CyclesPercentage == 100)
      Settings.H_Max = (SNES_CYCLES_PER_SCANLINE * 101) / 100;


#ifdef DETECT_NASTY_FX_INTERLEAVE
   //XXX: Test without these. Win32 port indicates they aren't needed?
   //Apparently are needed!
   if (strcmp(Memory.ROMName, "WILD TRAX") == 0 ||
         strcmp(Memory.ROMName, "STAR FOX 2") == 0 ||
         strcmp(Memory.ROMName, "YOSSY'S ISLAND") == 0 ||
         strcmp(Memory.ROMName, "YOSHI'S ISLAND") == 0)
      CPU.TriedInterleavedMode2 = true;
#endif

   // Start Trek: Deep Sleep 9
   if (strncmp(Memory.ROMId, "A9D", 3) == 0 && Settings.CyclesPercentage == 100)
      Settings.H_Max = (SNES_CYCLES_PER_SCANLINE * 110) / 100;


   //SA-1 Speedup settings
   SA1.WaitAddress = NULL;
   SA1.WaitByteAddress1 = NULL;
   SA1.WaitByteAddress2 = NULL;

   /* Bass Fishing */
   if (strcmp(Memory.ROMId, "ZBPJ") == 0)
   {
      SA1.WaitAddress = SA1.Map [0x0093f1 >> MEMMAP_SHIFT] + 0x93f1;
      SA1.WaitByteAddress1 = Memory.FillRAM + 0x304a;
   }
   /* DAISENRYAKU EXPERTWW2 */
   if (strcmp(Memory.ROMId, "AEVJ") == 0)
   {
      SA1.WaitAddress = SA1.Map [0x0ed18d >> MEMMAP_SHIFT] + 0xd18d;
      SA1.WaitByteAddress1 = Memory.FillRAM + 0x3000;
   }
   /* debjk2 */
   if (strcmp(Memory.ROMId, "A2DJ") == 0)
      SA1.WaitAddress = SA1.Map [0x008b62 >> MEMMAP_SHIFT] + 0x8b62;
   /* Dragon Ballz HD */
   if (strcmp(Memory.ROMId, "AZIJ") == 0)
   {
      SA1.WaitAddress = SA1.Map [0x008083 >> MEMMAP_SHIFT] + 0x8083;
      SA1.WaitByteAddress1 = Memory.FillRAM + 0x3020;
   }
   /* SFC SDGUNDAMGNEXT */
   if (strcmp(Memory.ROMId, "ZX3J") == 0)
   {
      SA1.WaitAddress = SA1.Map [0x0087f2 >> MEMMAP_SHIFT] + 0x87f2;
      SA1.WaitByteAddress1 = Memory.FillRAM + 0x30c4;
   }
   /* ShougiNoHanamichi */
   if (strcmp(Memory.ROMId, "AARJ") == 0)
   {
      SA1.WaitAddress = SA1.Map [0xc1f85a >> MEMMAP_SHIFT] + 0xf85a;
      SA1.WaitByteAddress1 = Memory.SRAM + 0x0c64;
      SA1.WaitByteAddress2 = Memory.SRAM + 0x0c66;
   }
   /* KATO HIFUMI9DAN SYOGI */
   if (strcmp(Memory.ROMId, "A23J") == 0)
   {
      SA1.WaitAddress = SA1.Map [0xc25037 >> MEMMAP_SHIFT] + 0x5037;
      SA1.WaitByteAddress1 = Memory.SRAM + 0x0c06;
      SA1.WaitByteAddress2 = Memory.SRAM + 0x0c08;
   }
   /* idaten */
   if (strcmp(Memory.ROMId, "AIIJ") == 0)
   {
      SA1.WaitAddress = SA1.Map [0xc100be >> MEMMAP_SHIFT] + 0x00be;
      SA1.WaitByteAddress1 = Memory.SRAM + 0x1002;
      SA1.WaitByteAddress2 = Memory.SRAM + 0x1004;
   }
   /* igotais */
   if (strcmp(Memory.ROMId, "AITJ") == 0)
      SA1.WaitAddress = SA1.Map [0x0080b7 >> MEMMAP_SHIFT] + 0x80b7;
   /* J96 DREAM STADIUM */
   if (strcmp(Memory.ROMId, "AJ6J") == 0)
      SA1.WaitAddress = SA1.Map [0xc0f74a >> MEMMAP_SHIFT] + 0xf74a;
   /* JumpinDerby */
   if (strcmp(Memory.ROMId, "AJUJ") == 0)
      SA1.WaitAddress = SA1.Map [0x00d926 >> MEMMAP_SHIFT] + 0xd926;
   /* JKAKINOKI SHOUGI */
   if (strcmp(Memory.ROMId, "AKAJ") == 0)
      SA1.WaitAddress = SA1.Map [0x00f070 >> MEMMAP_SHIFT] + 0xf070;
   /* HOSHI NO KIRBY 3 & KIRBY'S DREAM LAND 3 JAP & US */
   if (strcmp(Memory.ROMId, "AFJJ") == 0 || strcmp(Memory.ROMId, "AFJE") == 0)
   {
      SA1.WaitAddress = SA1.Map [0x0082d4 >> MEMMAP_SHIFT] + 0x82d4;
      SA1.WaitByteAddress1 = Memory.SRAM + 0x72a4;
   }
   /* KIRBY SUPER DELUXE JAP */
   if (strcmp(Memory.ROMId, "AKFJ") == 0)
   {
      SA1.WaitAddress = SA1.Map [0x008c93 >> MEMMAP_SHIFT] + 0x8c93;
      SA1.WaitByteAddress1 = Memory.FillRAM + 0x300a;
      SA1.WaitByteAddress2 = Memory.FillRAM + 0x300e;
   }
   /* KIRBY SUPER DELUXE US */
   if (strcmp(Memory.ROMId, "AKFE") == 0)
   {
      SA1.WaitAddress = SA1.Map [0x008cb8 >> MEMMAP_SHIFT] + 0x8cb8;
      SA1.WaitByteAddress1 = Memory.FillRAM + 0x300a;
      SA1.WaitByteAddress2 = Memory.FillRAM + 0x300e;
   }
   /* SUPER MARIO RPG JAP & US */
   if (strcmp(Memory.ROMId, "ARWJ") == 0 || strcmp(Memory.ROMId, "ARWE") == 0)
   {
      SA1.WaitAddress = SA1.Map [0xc0816f >> MEMMAP_SHIFT] + 0x816f;
      SA1.WaitByteAddress1 = Memory.FillRAM + 0x3000;
   }
   /* marvelous.zip */
   if (strcmp(Memory.ROMId, "AVRJ") == 0)
   {
      SA1.WaitAddress = SA1.Map [0x0085f2 >> MEMMAP_SHIFT] + 0x85f2;
      SA1.WaitByteAddress1 = Memory.FillRAM + 0x3024;
   }
   /* AUGUSTA3 MASTERS NEW */
   if (strcmp(Memory.ROMId, "AO3J") == 0)
   {
      SA1.WaitAddress = SA1.Map [0x00dddb >> MEMMAP_SHIFT] + 0xdddb;
      SA1.WaitByteAddress1 = Memory.FillRAM + 0x37b4;
   }
   /* OSHABERI PARODIUS */
   if (strcmp(Memory.ROMId, "AJOJ") == 0)
      SA1.WaitAddress = SA1.Map [0x8084e5 >> MEMMAP_SHIFT] + 0x84e5;
   /* PANIC BOMBER WORLD */
   if (strcmp(Memory.ROMId, "APBJ") == 0)
      SA1.WaitAddress = SA1.Map [0x00857a >> MEMMAP_SHIFT] + 0x857a;
   /* PEBBLE BEACH NEW */
   if (strcmp(Memory.ROMId, "AONJ") == 0)
   {
      SA1.WaitAddress = SA1.Map [0x00df33 >> MEMMAP_SHIFT] + 0xdf33;
      SA1.WaitByteAddress1 = Memory.FillRAM + 0x37b4;
   }
   /* PGA EUROPEAN TOUR */
   if (strcmp(Memory.ROMId, "AEPE") == 0)
   {
      SA1.WaitAddress = SA1.Map [0x003700 >> MEMMAP_SHIFT] + 0x3700;
      SA1.WaitByteAddress1 = Memory.FillRAM + 0x3102;
   }
   /* PGA TOUR 96 */
   if (strcmp(Memory.ROMId, "A3GE") == 0)
   {
      SA1.WaitAddress = SA1.Map [0x003700 >> MEMMAP_SHIFT] + 0x3700;
      SA1.WaitByteAddress1 = Memory.FillRAM + 0x3102;
   }
   /* POWER RANGERS 4 */
   if (strcmp(Memory.ROMId, "A4RE") == 0)
   {
      SA1.WaitAddress = SA1.Map [0x009899 >> MEMMAP_SHIFT] + 0x9899;
      SA1.WaitByteAddress1 = Memory.FillRAM + 0x3000;
   }
   /* PACHISURO PALUSUPE */
   if (strcmp(Memory.ROMId, "AGFJ") == 0)
   {
      // Never seems to turn on the SA-1!
   }
   /* SD F1 GRAND PRIX */
   if (strcmp(Memory.ROMId, "AGFJ") == 0)
      SA1.WaitAddress = SA1.Map [0x0181bc >> MEMMAP_SHIFT] + 0x81bc;
   /* SHOUGI MARJONG */
   if (strcmp(Memory.ROMId, "ASYJ") == 0)
   {
      SA1.WaitAddress = SA1.Map [0x00f2cc >> MEMMAP_SHIFT] + 0xf2cc;
      SA1.WaitByteAddress1 = Memory.SRAM + 0x7ffe;
      SA1.WaitByteAddress2 = Memory.SRAM + 0x7ffc;
   }
   /* shogisai2 */
   if (strcmp(Memory.ROMId, "AX2J") == 0)
      SA1.WaitAddress = SA1.Map [0x00d675 >> MEMMAP_SHIFT] + 0xd675;

   /* SHINING SCORPION */
   if (strcmp(Memory.ROMId, "A4WJ") == 0)
      SA1.WaitAddress = SA1.Map [0xc048be >> MEMMAP_SHIFT] + 0x48be;
   /* SHIN SHOUGI CLUB */
   if (strcmp(Memory.ROMId, "AHJJ") == 0)
   {
      SA1.WaitAddress = SA1.Map [0xc1002a >> MEMMAP_SHIFT] + 0x002a;
      SA1.WaitByteAddress1 = Memory.SRAM + 0x0806;
      SA1.WaitByteAddress2 = Memory.SRAM + 0x0808;
   }


   //Other

   // Additional game fixes by sanmaiwashi ...
   if (strcmp(Memory.ROMName,
              "SFX ŲĶ������ɶ��� 1") ==
         0) //Gundam Knight Story
   {
      bytes0x2000 [0xb18] = 0x4c;
      bytes0x2000 [0xb19] = 0x4b;
      bytes0x2000 [0xb1a] = 0xea;
      SNESGameFixes.SRAMInitialValue = 0x6b;
   }


   // HITOMI3
   if (strcmp(Memory.ROMName, "HITOMI3") == 0)
   {
      Memory.SRAMSize = 1;
      Memory.SRAMMask = Memory.SRAMSize ?
                        ((1 << (Memory.SRAMSize + 3)) * 128) - 1 : 0;
   }

   //sram value fixes
   if (strcmp(Memory.ROMName, "SUPER DRIFT OUT") == 0 ||
         strcmp(Memory.ROMName, "SATAN IS OUR FATHER!") == 0 ||
         strcmp(Memory.ROMName, "goemon 4") == 0)
      SNESGameFixes.SRAMInitialValue = 0x00;

#if 0
   if (strcmp(Memory.ROMName, "XBAND JAPANESE MODEM") == 0)
   {
      for (c = 0x200; c < 0x400; c += 16)
      {
         for (int i = c; i < c + 16; i++)
         {
            Memory.Map [i + 0x400] = Memory.Map [i + 0xc00] = &ROM[c * 0x1000];
            Memory.BlockIsRAM [i + 0x400] = Memory.BlockIsRAM [i + 0xc00] = true;
            BlockIsROM [i + 0x400] = BlockIsROM [i + 0xc00] = false;
         }
      }
      WriteProtectROM();
   }
#endif

#define RomPatch(adr,ov,nv) \
   if (Memory.ROM [adr] == ov) \
    Memory.ROM [adr] = nv


   // Love Quest
   if (strcmp(Memory.ROMName, "LOVE QUEST") == 0)
   {
      RomPatch(0x1385ec, 0xd0, 0xea);
      RomPatch(0x1385ed, 0xb2, 0xea);
   }
   //BNE D0 into nops

   //seems like the next instruction is a BRA
   //otherwise, this one's too complex for MKendora
   // Nangoku Syonen Papuwa Kun
   if (strcmp(Memory.ROMName, "NANGOKUSYONEN PAPUWA") == 0)
      RomPatch(0x1f0d1, 0xa0, 0x6b);
   //turns an LDY into an RTL?

   //this is a cmp on $00:2140
   // Super Batter Up
   if (strcmp(Memory.ROMName, "Super Batter Up") == 0)
   {
      RomPatch(0x27ae0, 0xd0, 0xea);
      RomPatch(0x27ae1, 0xfa, 0xea);
   }
   //BNE
}

// Read variable size MSB int from a file
static long ReadInt(FILE* f, unsigned nbytes)
{
   long v = 0;
   while (nbytes--)
   {
      int c = fgetc(f);
      if (c == EOF)
         return -1;
      v = (v << 8) | (c & 0xFF);
   }
   return (v);
}

#define IPS_EOF 0x00454F46l

void CheckForIPSPatch(const char* rom_filename, bool header,
                      int32_t* rom_size)
{
   char  dir [_MAX_DIR + 1];
   char  drive [_MAX_DRIVE + 1];
   char  name [_MAX_FNAME + 1];
   char  ext [_MAX_EXT + 1];
   char  fname [_MAX_PATH + 1];
   FILE*  patch_file  = NULL;
   long  offset = header ? 512 : 0;

   _splitpath(rom_filename, drive, dir, name, ext);
   _makepath(fname, drive, dir, name, "ips");

   if (!(patch_file = fopen(fname, "rb")))
   {
      if (!(patch_file = fopen(S9xGetFilename("ips"), "rb")))
         return;
   }

   if (fread((unsigned char*)fname, 1, 5, patch_file) != 5 ||
         strncmp(fname, "PATCH", 5) != 0)
   {
      fclose(patch_file);
      return;
   }

   int32_t ofs;

   for (;;)
   {
      long len;
      long rlen;
      int  rchar;

      ofs = ReadInt(patch_file, 3);
      if (ofs == -1)
         goto err_eof;

      if (ofs == IPS_EOF)
         break;

      ofs -= offset;

      len = ReadInt(patch_file, 2);
      if (len == -1)
         goto err_eof;

      /* Apply patch block */
      if (len)
      {
         if (ofs + len > MAX_ROM_SIZE)
            goto err_eof;

         while (len--)
         {
            rchar = fgetc(patch_file);
            if (rchar == EOF)
               goto err_eof;
            Memory.ROM [ofs++] = (uint8_t) rchar;
         }
         if (ofs > *rom_size)
            *rom_size = ofs;
      }
      else
      {
         rlen = ReadInt(patch_file, 2);
         if (rlen == -1)
            goto err_eof;

         rchar = fgetc(patch_file);
         if (rchar == EOF)
            goto err_eof;

         if (ofs + rlen > MAX_ROM_SIZE)
            goto err_eof;

         while (rlen--)
            Memory.ROM [ofs++] = (uint8_t) rchar;

         if (ofs > *rom_size)
            *rom_size = ofs;
      }
   }

   // Check if ROM image needs to be truncated
   ofs = ReadInt(patch_file, 3);
   if (ofs != -1 && ofs - offset < *rom_size)
   {
      // Need to truncate ROM image
      *rom_size = ofs - offset;
   }
   fclose(patch_file);
   return;

err_eof:
   if (patch_file)
      fclose(patch_file);
}

int is_bsx(unsigned char* p)
{
   unsigned c;

   if (p[0x19] & 0x4f)
      goto notbsx;
   c = p[0x1a];
   if ((c != 0x33) && (c != 0xff))   // 0x33 = Manufacturer: Nintendo
      goto notbsx;
   c = (p[0x17] << 8) | p[0x16];
   if ((c != 0x0000) && (c != 0xffff))
   {
      if ((c & 0x040f) != 0)
         goto notbsx;
      if ((c & 0xff) > 0xc0)
         goto notbsx;
   }
   c = p[0x18];
   if ((c & 0xce) || ((c & 0x30) == 0))
      goto notbsx;
   if ((p[0x15] & 0x03) != 0)
      goto notbsx;
   c = p[0x13];
   if ((c != 0x00) && (c != 0xff))
      goto notbsx;
   if (p[0x14] != 0x00)
      goto notbsx;
   if (bs_name(p) != 0)
      goto notbsx;
   return 0; // It's a Satellaview ROM!
notbsx:
   return -1;
}
int bs_name(unsigned char* p)
{
   unsigned c;
   int lcount;
   int numv; // number of valid name characters seen so far
   numv = 0;
   for (lcount = 16; lcount > 0; lcount--)
   {
      if (check_char(c = *p++) != 0)
      {
         c = *p++;
         if (c < 0x20)
         {
            if ((numv != 0x0b) || (c != 0))   // Dr. Mario Hack
               goto notBsName;
         }

         numv++;
         lcount--;
         continue;
      }
      else
      {
         if (c == 0)
         {
            if (numv == 0)
               goto notBsName;
            continue;
         }

         if (c < 0x20)
            goto notBsName;
         if (c >= 0x80)
         {
            if ((c < 0xa0) || (c >= 0xf0))
               goto notBsName;
         }
         numv++;
      }
   }
   if (numv > 0)
      return 0;
notBsName:
   return -1;
}
int check_char(unsigned c)
{
   if ((c & 0x80) == 0)
      return 0;
   if ((c - 0x20) & 0x40)
      return 1;
   else
      return 0;
}

void ParseSNESHeader(uint8_t* RomHeader)
{
   Memory.SRAMSize = RomHeader [0x28];
   strncpy(Memory.ROMName, (char*) &RomHeader[0x10], ROM_NAME_LEN - 1);
   Memory.ROMSpeed = RomHeader [0x25];
   Memory.ROMType = RomHeader [0x26];
   Memory.ROMSize = RomHeader [0x27];
   Memory.ROMChecksum = RomHeader [0x2e] + (RomHeader [0x2f] << 8);
   Memory.ROMComplementChecksum = RomHeader [0x2c] + (RomHeader [0x2d] << 8);
   Memory.ROMRegion = RomHeader[0x29];
   // memmove converted: Different mallocs [Neb]
   memcpy(Memory.ROMId, &RomHeader [0x2], 4);
   if (RomHeader[0x2A] == 0x33)
      // memmove converted: Different mallocs [Neb]
      memcpy(Memory.CompanyId, &RomHeader [0], 2);
   else sprintf(Memory.CompanyId, "%02X", RomHeader[0x2A]);
}

#undef INLINE
#define INLINE
#include "getset.h"

