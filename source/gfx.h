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

#ifndef _GFX_H_
#define _GFX_H_

#include "port.h"
#include "snes9x.h"

void S9xStartScreenRefresh();
void S9xDrawScanLine(uint8_t Line);
void S9xEndScreenRefresh();
void S9xSetupOBJ();
void S9xUpdateScreen();
void RenderLine(uint8_t line);
void S9xBuildDirectColourMaps();

// External port interface which must be implemented or initialised for each
// port.
extern struct SGFX GFX;

bool S9xInitGFX();
void S9xDeinitGFX();
bool S9xInitUpdate(void);
#if 0
void S9xSyncSpeed();
#else
#define S9xSyncSpeed()
#endif


struct SGFX
{
   // Initialize these variables
   uint8_t*  Screen_buffer;
   uint8_t*  SubScreen_buffer;
   uint8_t*  ZBuffer_buffer;
   uint8_t*  SubZBuffer_buffer;

   uint8_t*  Screen;
   uint8_t*  SubScreen;
   uint8_t*  ZBuffer;
   uint8_t*  SubZBuffer;
   uint32_t Pitch;

   // Setup in call to S9xInitGFX()
   int   Delta;
   uint16_t* X2;
   uint16_t* ZERO_OR_X2;
   uint16_t* ZERO;
   uint32_t RealPitch; // True pitch of Screen buffer.
   uint32_t Pitch2;    // Same as RealPitch except while using speed up hack for Glide.
   uint32_t ZPitch;    // Pitch of ZBuffer
   uint32_t PPL;         // Number of pixels on each of Screen buffer
   uint32_t PPLx2;
   uint32_t PixSize;
   uint8_t S_safety_margin[8];
   uint8_t*  S;
   uint8_t DB_safety_margin[8];
   uint8_t*  DB;
   uint32_t DepthDelta;
   uint8_t  Z1;          // Depth for comparison
   uint8_t  Z2;          // Depth to save
   uint8_t  ZSprite;     // Used to ensure only 1st sprite is drawn per pixel
   uint32_t FixedColour;
   const char* InfoString;
   uint32_t InfoStringTimeout;
   uint32_t StartY;
   uint32_t EndY;
   struct ClipData* pCurrentClip;
   uint32_t Mode7Mask;
   uint32_t Mode7PriorityMask;
   uint8_t  OBJWidths[128];
   uint8_t  OBJVisibleTiles[128];
   struct
   {
      uint8_t RTOFlags;
      int16_t Tiles;
      struct
      {
         int8_t Sprite;
         uint8_t Line;
      } OBJ[32];
   } OBJLines [SNES_HEIGHT_EXTENDED];

   uint8_t  r212c;
   uint8_t  r212d;
   uint8_t  r2130;
   uint8_t  r2131;
   bool  Pseudo;

};

struct SLineData
{
   struct
   {
      uint16_t VOffset;
      uint16_t HOffset;
   } BG [4];
};

#define H_FLIP 0x4000
#define V_FLIP 0x8000
#define BLANK_TILE 2

typedef struct
{
   uint32_t TileSize;
   uint32_t BitShift;
   uint32_t TileShift;
   uint32_t TileAddress;
   uint32_t NameSelect;
   uint32_t SCBase;

   uint32_t StartPalette;
   uint32_t PaletteShift;
   uint32_t PaletteMask;

   uint8_t* Buffer;
   uint8_t* Buffered;
   bool  DirectColourMode;
} SBG;

struct SLineMatrixData
{
   short MatrixA;
   short MatrixB;
   short MatrixC;
   short MatrixD;
   short CentreX;
   short CentreY;
};

extern uint32_t odd_high [4][16];
extern uint32_t odd_low [4][16];
extern uint32_t even_high [4][16];
extern uint32_t even_low [4][16];
extern SBG BG;
extern uint16_t DirectColourMaps [8][256];

extern uint8_t add32_32 [32][32];
extern uint8_t add32_32_half [32][32];
extern uint8_t sub32_32 [32][32];
extern uint8_t sub32_32_half [32][32];
extern uint8_t mul_brightness [16][32];

// Could use BSWAP instruction on Intel port...
#define SWAP_DWORD(dw) dw = ((dw & 0xff) << 24) | ((dw & 0xff00) << 8) | \
                  ((dw & 0xff0000) >> 8) | ((dw & 0xff000000) >> 24)

#ifdef FAST_LSB_WORD_ACCESS
#define READ_2BYTES(s) (*(uint16_t *) (s))
#define WRITE_2BYTES(s, d) *(uint16_t *) (s) = (d)
#else
#ifdef LSB_FIRST
#define READ_2BYTES(s) (*(uint8_t *) (s) | (*((uint8_t *) (s) + 1) << 8))
#define WRITE_2BYTES(s, d) *(uint8_t *) (s) = (d), \
            *((uint8_t *) (s) + 1) = (d) >> 8
#else  // else MSB_FISRT
#define READ_2BYTES(s) (*(uint8_t *) (s) | (*((uint8_t *) (s) + 1) << 8))
#define WRITE_2BYTES(s, d) *(uint8_t *) (s) = (d), \
            *((uint8_t *) (s) + 1) = (d) >> 8
#endif // LSB_FIRST
#endif // i386

#define SUB_SCREEN_DEPTH 0
#define MAIN_SCREEN_DEPTH 32

#if defined(OLD_COLOUR_BLENDING)
#define COLOR_ADD(C1, C2) \
GFX.X2 [((((C1) & RGB_REMOVE_LOW_BITS_MASK) + \
     ((C2) & RGB_REMOVE_LOW_BITS_MASK)) >> 1) + \
   ((C1) & (C2) & RGB_LOW_BITS_MASK)]
#else
static inline uint16_t COLOR_ADD(uint16_t, uint16_t);

static inline uint16_t COLOR_ADD(uint16_t C1, uint16_t C2)
{
   if (C1 == 0)
      return C2;
   else if (C2 == 0)
      return C1;
   else
      return GFX.X2 [(((C1 & RGB_REMOVE_LOW_BITS_MASK) + (C2 &
                       RGB_REMOVE_LOW_BITS_MASK)) >> 1) + (C1 & C2 & RGB_LOW_BITS_MASK)] | ((
                                C1 ^ C2) & RGB_LOW_BITS_MASK);
}
#endif

#define COLOR_ADD1_2(C1, C2) \
(((((C1) & RGB_REMOVE_LOW_BITS_MASK) + \
          ((C2) & RGB_REMOVE_LOW_BITS_MASK)) >> 1) + \
         (((C1) & (C2) & RGB_LOW_BITS_MASK) | ALPHA_BITS_MASK))

#if defined(OLD_COLOUR_BLENDING)
#define COLOR_SUB(C1, C2) \
GFX.ZERO_OR_X2 [(((C1) | RGB_HI_BITS_MASKx2) - \
       ((C2) & RGB_REMOVE_LOW_BITS_MASK)) >> 1]
#elif !defined(NEW_COLOUR_BLENDING)
#define COLOR_SUB(C1, C2) \
(GFX.ZERO_OR_X2 [(((C1) | RGB_HI_BITS_MASKx2) - \
                  ((C2) & RGB_REMOVE_LOW_BITS_MASK)) >> 1] + \
((C1) & RGB_LOW_BITS_MASK) - ((C2) & RGB_LOW_BITS_MASK))
#else
inline uint16_t COLOR_SUB(uint16_t, uint16_t);

inline uint16_t COLOR_SUB(uint16_t C1, uint16_t C2)
{
   uint16_t   mC1, mC2, v = 0;

   mC1 = C1 & FIRST_COLOR_MASK;
   mC2 = C2 & FIRST_COLOR_MASK;
   if (mC1 > mC2) v += (mC1 - mC2);

   mC1 = C1 & SECOND_COLOR_MASK;
   mC2 = C2 & SECOND_COLOR_MASK;
   if (mC1 > mC2) v += (mC1 - mC2);

   mC1 = C1 & THIRD_COLOR_MASK;
   mC2 = C2 & THIRD_COLOR_MASK;
   if (mC1 > mC2) v += (mC1 - mC2);

   return v;
}
#endif

#define COLOR_SUB1_2(C1, C2) \
GFX.ZERO [(((C1) | RGB_HI_BITS_MASKx2) - \
      ((C2) & RGB_REMOVE_LOW_BITS_MASK)) >> 1]

typedef void (*NormalTileRenderer)(uint32_t Tile, int32_t Offset,
                                   uint32_t StartLine, uint32_t LineCount);
typedef void (*ClippedTileRenderer)(uint32_t Tile, int32_t Offset,
                                    uint32_t StartPixel, uint32_t Width,
                                    uint32_t StartLine, uint32_t LineCount);
typedef void (*LargePixelRenderer)(uint32_t Tile, int32_t Offset,
                                   uint32_t StartPixel, uint32_t Pixels,
                                   uint32_t StartLine, uint32_t LineCount);

#endif

