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

#ifndef _PPU_H_
#define _PPU_H_

#define FIRST_VISIBLE_LINE 1

extern uint8_t GetBank;
extern uint16_t SignExtend [2];

#define TILE_2BIT 0
#define TILE_4BIT 1
#define TILE_8BIT 2

#define MAX_2BIT_TILES 4096
#define MAX_4BIT_TILES 2048
#define MAX_8BIT_TILES 1024

#define PPU_H_BEAM_IRQ_SOURCE (1 << 0)
#define PPU_V_BEAM_IRQ_SOURCE (1 << 1)
#define GSU_IRQ_SOURCE     (1 << 2)
#define SA1_IRQ_SOURCE     (1 << 7)
#define SA1_DMA_IRQ_SOURCE (1 << 5)

struct ClipData
{
   uint32_t  Count [6];
   uint32_t  Left [6][6];
   uint32_t  Right [6][6];
};

typedef struct
{
   bool  ColorsChanged;
   uint8_t  HDMA;
   bool  HDMAStarted;
   uint8_t  MaxBrightness;
   bool  LatchedBlanking;
   bool  OBJChanged;
   bool  RenderThisFrame;
   bool  DirectColourMapsNeedRebuild;
   uint32_t FrameCount;
   uint32_t RenderedFramesCount;
   uint32_t DisplayedRenderedFrameCount;
   uint32_t SkippedFrames;
   uint32_t FrameSkip;
   uint8_t*  TileCache [3];
   uint8_t*  TileCached [3];
#ifdef CORRECT_VRAM_READS
   uint16_t VRAMReadBuffer;
#else
   bool  FirstVRAMRead;
#endif
   bool  DoubleHeightPixels;
   bool  Interlace;
   bool  InterlaceSprites;
   bool  DoubleWidthPixels;
   bool  HalfWidthPixels;
   int    RenderedScreenHeight;
   int    RenderedScreenWidth;
   uint32_t Red [256];
   uint32_t Green [256];
   uint32_t Blue [256];
   uint8_t*  XB;
   uint16_t ScreenColors [256];
   int     PreviousLine;
   int     CurrentLine;
   int     Controller;
   uint32_t Joypads[5];
   uint32_t SuperScope;
   uint32_t Mouse[2];
   int    PrevMouseX[2];
   int    PrevMouseY[2];
   struct ClipData Clip [2];
} InternalPPU;

struct SOBJ
{
   short  HPos;
   uint16_t VPos;
   uint16_t Name;
   uint8_t  VFlip;
   uint8_t  HFlip;
   uint8_t  Priority;
   uint8_t  Palette;
   uint8_t  Size;
};

typedef struct
{
   uint8_t  BGMode;
   uint8_t  BG3Priority;
   uint8_t  Brightness;

   struct
   {
      bool High;
      uint8_t Increment;
      uint16_t Address;
      uint16_t Mask1;
      uint16_t FullGraphicCount;
      uint16_t Shift;
   } VMA;

   struct
   {
      uint16_t SCBase;
      uint16_t VOffset;
      uint16_t HOffset;
      uint8_t BGSize;
      uint16_t NameBase;
      uint16_t SCSize;
   } BG [4];

   bool  CGFLIP;
   uint16_t CGDATA [256];
   uint8_t  FirstSprite;
   uint8_t  LastSprite;
   struct SOBJ OBJ [128];
   uint8_t  OAMPriorityRotation;
   uint16_t OAMAddr;
   uint8_t  RangeTimeOver;

   uint8_t  OAMFlip;
   uint16_t OAMTileAddress;
   uint16_t IRQVBeamPos;
   uint16_t IRQHBeamPos;
   uint16_t VBeamPosLatched;
   uint16_t HBeamPosLatched;

   uint8_t  HBeamFlip;
   uint8_t  VBeamFlip;
   uint8_t  HVBeamCounterLatched;

   short  MatrixA;
   short  MatrixB;
   short  MatrixC;
   short  MatrixD;
   short  CentreX;
   short  CentreY;
   uint8_t  Joypad1ButtonReadPos;
   uint8_t  Joypad2ButtonReadPos;

   uint8_t  CGADD;
   uint8_t  FixedColourRed;
   uint8_t  FixedColourGreen;
   uint8_t  FixedColourBlue;
   uint16_t SavedOAMAddr;
   uint16_t ScreenHeight;
   uint32_t WRAM;
   uint8_t  BG_Forced;
   bool  ForcedBlanking;
   bool  OBJThroughMain;
   bool  OBJThroughSub;
   uint8_t  OBJSizeSelect;
   uint16_t OBJNameBase;
   bool  OBJAddition;
   uint8_t  OAMReadFlip;
   uint8_t  OAMData [512 + 32];
   bool  VTimerEnabled;
   bool  HTimerEnabled;
   short  HTimerPosition;
   uint8_t  Mosaic;
   bool  BGMosaic [4];
   bool  Mode7HFlip;
   bool  Mode7VFlip;
   uint8_t  Mode7Repeat;
   uint8_t  Window1Left;
   uint8_t  Window1Right;
   uint8_t  Window2Left;
   uint8_t  Window2Right;
   uint8_t  ClipCounts [6];
   uint8_t  ClipWindowOverlapLogic [6];
   uint8_t  ClipWindow1Enable [6];
   uint8_t  ClipWindow2Enable [6];
   bool  ClipWindow1Inside [6];
   bool  ClipWindow2Inside [6];
   bool  RecomputeClipWindows;
   uint8_t  CGFLIPRead;
   uint16_t OBJNameSelect;
   bool  Need16x8Mulitply;
   uint8_t  Joypad3ButtonReadPos;
   uint8_t  MouseSpeed[2];

   // XXX Do these need to be added to snapshot.cpp?
   uint16_t OAMWriteRegister;
   uint8_t BGnxOFSbyte;
   uint8_t OpenBus1;
   uint8_t OpenBus2;
} SPPU;

#define CLIP_OR 0
#define CLIP_AND 1
#define CLIP_XOR 2
#define CLIP_XNOR 3

typedef struct
{
   bool  TransferDirection;
   bool  AAddressFixed;
   bool  AAddressDecrement;
   uint8_t  TransferMode;

   uint8_t  ABank;
   uint16_t AAddress;
   uint16_t Address;
   uint8_t  BAddress;

   // General DMA only:
   uint16_t TransferBytes;

   // H-DMA only:
   bool  HDMAIndirectAddressing;
   uint16_t IndirectAddress;
   uint8_t  IndirectBank;
   uint8_t  Repeat;
   uint8_t  LineCount;
   uint8_t  FirstLine;
} SDMA;

void S9xUpdateScreen();
void S9xResetPPU();
void S9xSoftResetPPU();
void S9xFixColourBrightness();
void S9xUpdateJoypads();
void S9xProcessMouse(int which1);
void S9xSuperFXExec();

void S9xSetPPU(uint8_t Byte, uint16_t Address);
uint8_t S9xGetPPU(uint16_t Address);
void S9xSetCPU(uint8_t Byte, uint16_t Address);
uint8_t S9xGetCPU(uint16_t Address);

void S9xInitC4();
void S9xSetC4(uint8_t Byte, uint16_t Address);
uint8_t S9xGetC4(uint16_t Address);
void S9xSetC4RAM(uint8_t Byte, uint16_t Address);
uint8_t S9xGetC4RAM(uint16_t Address);

extern SPPU PPU;
extern SDMA DMA [8];
extern InternalPPU IPPU;

#include "gfx.h"
#include "memmap.h"

typedef struct
{
   uint8_t _5C77;
   uint8_t _5C78;
   uint8_t _5A22;
} SnesModel;

extern SnesModel* Model;
extern SnesModel M1SNES;
extern SnesModel M2SNES;

#define MAX_5C77_VERSION 0x01
#define MAX_5C78_VERSION 0x03
#define MAX_5A22_VERSION 0x02

extern uint8_t REGISTER_4212();
extern void FLUSH_REDRAW();
extern void REGISTER_2104(uint8_t byte);
extern void REGISTER_2118(uint8_t Byte);
extern void REGISTER_2118_tile(uint8_t Byte);
extern void REGISTER_2118_linear(uint8_t Byte);
extern void REGISTER_2119(uint8_t Byte);
extern void REGISTER_2119_tile(uint8_t Byte);
extern void REGISTER_2119_linear(uint8_t Byte);
extern void REGISTER_2122(uint8_t Byte);
extern void REGISTER_2180(uint8_t Byte);

//Platform specific input functions used by PPU.CPP
void JustifierButtons(uint32_t*);
bool JustifierOffscreen();

#endif

