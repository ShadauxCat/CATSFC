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

#ifndef _memmap_h_
#define _memmap_h_

#include "snes9x.h"

#ifdef FAST_LSB_WORD_ACCESS
#define READ_WORD(s) (*(uint16_t *) (s))
#define READ_DWORD(s) (*(uint32_t *) (s))
#define WRITE_WORD(s, d) (*(uint16_t *) (s)) = (d)
#define WRITE_DWORD(s, d) (*(uint32_t *) (s)) = (d)

#define READ_3WORD(s) (0x00ffffff & *(uint32_t *) (s))
#define WRITE_3WORD(s, d) *(uint16_t *) (s) = (uint16_t)(d),\
                          *((uint8_t *) (s) + 2) = (uint8_t) ((d) >> 16)


#else
#define READ_WORD(s) ( *(uint8_t *) (s) |\
            (*((uint8_t *) (s) + 1) << 8))
#define READ_DWORD(s) ( *(uint8_t *) (s) |\
             (*((uint8_t *) (s) + 1) << 8) |\
             (*((uint8_t *) (s) + 2) << 16) |\
             (*((uint8_t *) (s) + 3) << 24))
#define WRITE_WORD(s, d) *(uint8_t *) (s) = (d), \
                         *((uint8_t *) (s) + 1) = (d) >> 8
#define WRITE_DWORD(s, d) *(uint8_t *) (s) = (uint8_t) (d), \
                          *((uint8_t *) (s) + 1) = (uint8_t) ((d) >> 8),\
                          *((uint8_t *) (s) + 2) = (uint8_t) ((d) >> 16),\
                          *((uint8_t *) (s) + 3) = (uint8_t) ((d) >> 24)
#define WRITE_3WORD(s, d) *(uint8_t *) (s) = (uint8_t) (d), \
                          *((uint8_t *) (s) + 1) = (uint8_t) ((d) >> 8),\
                          *((uint8_t *) (s) + 2) = (uint8_t) ((d) >> 16)
#define READ_3WORD(s) ( *(uint8_t *) (s) |\
                       (*((uint8_t *) (s) + 1) << 8) |\
                       (*((uint8_t *) (s) + 2) << 16))
#endif

#define MEMMAP_BLOCK_SIZE (0x1000)
#define MEMMAP_NUM_BLOCKS (0x1000000 / MEMMAP_BLOCK_SIZE)
#define MEMMAP_BLOCKS_PER_BANK (0x10000 / MEMMAP_BLOCK_SIZE)
#define MEMMAP_SHIFT 12
#define MEMMAP_MASK (MEMMAP_BLOCK_SIZE - 1)
#define MEMMAP_MAX_SDD1_LOGGED_ENTRIES (0x10000 / 8)

//Extended ROM Formats
#define NOPE 0
#define YEAH 1
#define BIGFIRST 2
#define SMALLFIRST 3

#ifdef LOAD_FROM_MEMORY_TEST
bool LoadROM(const struct retro_game_info* game);
#else
bool LoadROM(const char*);
uint32_t FileLoader(uint8_t* buffer, const char* filename, int32_t maxsize);
#endif
void  InitROM(bool);
bool LoadSRAM(const char*);
bool SaveSRAM(const char*);
bool S9xInitMemory();
void  S9xDeinitMemory();
void  FreeSDD1Data();

void WriteProtectROM();
void FixROMSpeed();
void MapRAM();
void MapExtraRAM();
char* Safe(const char*);

void BSLoROMMap();
void JumboLoROMMap(bool);
void LoROMMap();
void LoROM24MBSMap();
void SRAM512KLoROMMap();
//    void SRAM1024KLoROMMap ();
void SufamiTurboLoROMMap();
void HiROMMap();
void SuperFXROMMap();
void TalesROMMap(bool);
void AlphaROMMap();
void SA1ROMMap();
void BSHiROMMap();
void SPC7110HiROMMap();
void SPC7110Sram(uint8_t);
void SetaDSPMap();
bool AllASCII(uint8_t* b, int size);
int  ScoreHiROM(bool skip_header, int32_t offset);
int  ScoreLoROM(bool skip_header, int32_t offset);
#if 0
void SufamiTurboAltROMMap();
#endif
void ApplyROMFixes();
void CheckForIPSPatch(const char* rom_filename, bool header,
                      int32_t* rom_size);

const char* TVStandard();
const char* Speed();
const char* StaticRAMSize();
const char* MapType();
const char* MapMode();
const char* KartContents();
const char* Size();
const char* Headers();
const char* ROMID();
const char* CompanyID();
void ParseSNESHeader(uint8_t*);
enum
{
   MAP_PPU, MAP_CPU, MAP_DSP, MAP_LOROM_SRAM, MAP_HIROM_SRAM,
   MAP_NONE, MAP_DEBUG, MAP_C4, MAP_BWRAM, MAP_BWRAM_BITMAP,
   MAP_BWRAM_BITMAP2, MAP_SA1RAM, MAP_SPC7110_ROM, MAP_SPC7110_DRAM,
   MAP_RONLY_SRAM, MAP_OBC_RAM, MAP_SETA_DSP, MAP_SETA_RISC, MAP_LAST
};
enum { MAX_ROM_SIZE = 0x800000 };

typedef struct
{
   uint8_t* RAM;
   uint8_t* ROM;
   uint8_t* VRAM;
   uint8_t* SRAM;
   uint8_t* BWRAM;
   uint8_t* FillRAM;
   uint8_t* C4RAM;
   bool HiROM;
   bool LoROM;
   uint32_t SRAMMask;
   uint8_t SRAMSize;
   uint8_t* Map [MEMMAP_NUM_BLOCKS];
   uint8_t* WriteMap [MEMMAP_NUM_BLOCKS];
   uint8_t MemorySpeed [MEMMAP_NUM_BLOCKS];
   uint8_t BlockIsRAM [MEMMAP_NUM_BLOCKS];
   uint8_t BlockIsROM [MEMMAP_NUM_BLOCKS];
   char  ROMName [ROM_NAME_LEN];
   char  ROMId [5];
   char  CompanyId [3];
   uint8_t ROMSpeed;
   uint8_t ROMType;
   uint8_t ROMSize;
   int32_t ROMFramesPerSecond;
   int32_t HeaderCount;
   uint32_t CalculatedSize;
   uint32_t CalculatedChecksum;
   uint32_t ROMChecksum;
   uint32_t ROMComplementChecksum;
   uint8_t*  SDD1Index;
   uint8_t*  SDD1Data;
   uint32_t SDD1Entries;
   uint32_t SDD1LoggedDataCountPrev;
   uint32_t SDD1LoggedDataCount;
   uint8_t  SDD1LoggedData [MEMMAP_MAX_SDD1_LOGGED_ENTRIES];
   char ROMFilename [_MAX_PATH];
   uint8_t ROMRegion;
   uint32_t ROMCRC32;
   uint8_t ExtendedFormat;
   uint8_t* BSRAM;
#if 0
   bool LoadMulti(const char*, const char*, const char*);
#endif
} CMemory;

void ResetSpeedMap();

extern CMemory Memory;
void S9xDeinterleaveMode2();
bool LoadZip(const char* zipname,
              int32_t* TotalFileSize,
              int32_t* headers,
              uint8_t* buffer);


void S9xAutoSaveSRAM();

#ifdef NO_INLINE_SET_GET
uint8_t S9xGetByte(uint32_t Address);
uint16_t S9xGetWord(uint32_t Address);
void S9xSetByte(uint8_t Byte, uint32_t Address);
void S9xSetWord(uint16_t Byte, uint32_t Address);
void S9xSetPCBase(uint32_t Address);
uint8_t* S9xGetMemPointer(uint32_t Address);
uint8_t* GetBasePointer(uint32_t Address);

extern uint8_t OpenBus;

#else
#ifndef INLINE
#define INLINE static inline
#endif
#include "getset.h"
#endif // NO_INLINE_SET_GET

#endif // _memmap_h_

