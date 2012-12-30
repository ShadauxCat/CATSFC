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
#ifndef _TILE_H_
#define _TILE_H_

#define TILE_PREAMBLE \
    uint8 *pCache; \
\
    uint32 TileAddr = BG.TileAddress + ((Tile & 0x3ff) << BG.TileShift); \
    if ((Tile & 0x1ff) >= 256) \
	TileAddr += BG.NameSelect; \
\
    TileAddr &= 0xffff; \
\
    uint32 TileNumber; \
    pCache = &BG.Buffer[(TileNumber = (TileAddr >> BG.TileShift)) << 6]; \
\
    if (!BG.Buffered [TileNumber]) \
	BG.Buffered[TileNumber] = ConvertTile (pCache, TileAddr); \
\
    if (BG.Buffered [TileNumber] == BLANK_TILE) \
	return; \
\
    register uint32 l; \
    uint16 *ScreenColors; \
    if (BG.DirectColourMode) \
    { \
	if (IPPU.DirectColourMapsNeedRebuild) \
            S9xBuildDirectColourMaps (); \
        ScreenColors = DirectColourMaps [(Tile >> 10) & BG.PaletteMask]; \
    } \
    else \
	ScreenColors = &IPPU.ScreenColors [(((Tile >> 10) & BG.PaletteMask) << BG.PaletteShift) + BG.StartPalette];

#define RENDER_TILE(NORMAL, FLIPPED, N) \
    switch (Tile & (V_FLIP | H_FLIP)) \
    { \
    case 0: \
	bp = pCache + StartLine; \
	for (l = LineCount; l != 0; l--, bp += 8, Offset += GFX.PPL) \
	{ \
	    if (*(uint32 *) bp) \
		NORMAL (Offset, bp, ScreenColors); \
	    if (*(uint32 *) (bp + 4)) \
		NORMAL (Offset + N, bp + 4, ScreenColors); \
	} \
        break; \
    case H_FLIP: \
	bp = pCache + StartLine; \
	for (l = LineCount; l != 0; l--, bp += 8, Offset += GFX.PPL) \
	{ \
	    if (*(uint32 *) (bp + 4)) \
		FLIPPED (Offset, bp + 4, ScreenColors); \
	    if (*(uint32 *) bp) \
		FLIPPED (Offset + N, bp, ScreenColors); \
	} \
        break; \
    case H_FLIP | V_FLIP: \
	bp = pCache + 56 - StartLine; \
	for (l = LineCount; l != 0; l--, bp -= 8, Offset += GFX.PPL) \
	{ \
	    if (*(uint32 *) (bp + 4)) \
		FLIPPED (Offset, bp + 4, ScreenColors); \
	    if (*(uint32 *) bp) \
		FLIPPED (Offset + N, bp, ScreenColors); \
	} \
        break; \
    case V_FLIP: \
	bp = pCache + 56 - StartLine; \
	for (l = LineCount; l != 0; l--, bp -= 8, Offset += GFX.PPL) \
	{ \
	    if (*(uint32 *) bp) \
		NORMAL (Offset, bp, ScreenColors); \
	    if (*(uint32 *) (bp + 4)) \
		NORMAL (Offset + N, bp + 4, ScreenColors); \
	} \
        break; \
    default: \
        break; \
    }

#define TILE_CLIP_PREAMBLE \
    uint32 d1; \
    uint32 d2; \
\
    if (StartPixel < 4) \
    { \
	d1 = HeadMask [StartPixel]; \
	if (StartPixel + Width < 4) \
	    d1 &= TailMask [StartPixel + Width]; \
    } \
    else \
	d1 = 0; \
\
    if (StartPixel + Width > 4) \
    { \
	if (StartPixel > 4) \
	    d2 = HeadMask [StartPixel - 4]; \
	else \
	    d2 = 0xffffffff; \
\
	d2 &= TailMask [(StartPixel + Width - 4)]; \
    } \
    else \
	d2 = 0;

#define RENDER_CLIPPED_TILE(NORMAL, FLIPPED, N) \
    uint32 dd; \
    switch (Tile & (V_FLIP | H_FLIP)) \
    { \
    case 0: \
	bp = pCache + StartLine; \
	for (l = LineCount; l != 0; l--, bp += 8, Offset += GFX.PPL) \
	{ \
            /* This is perfectly OK, regardless of endian. The tiles are \
             * cached in leftmost-endian order (when not horiz flipped) by \
             * the ConvertTile function. \
             */ \
	    if ((dd = (*(uint32 *) bp) & d1)) \
		NORMAL (Offset, (uint8 *) &dd, ScreenColors); \
	    if ((dd = (*(uint32 *) (bp + 4)) & d2)) \
		NORMAL (Offset + N, (uint8 *) &dd, ScreenColors); \
	} \
        break; \
    case H_FLIP: \
	bp = pCache + StartLine; \
	SWAP_DWORD (d1); \
	SWAP_DWORD (d2); \
	for (l = LineCount; l != 0; l--, bp += 8, Offset += GFX.PPL) \
	{ \
	    if ((dd = *(uint32 *) (bp + 4) & d1)) \
		FLIPPED (Offset, (uint8 *) &dd, ScreenColors); \
	    if ((dd = *(uint32 *) bp & d2)) \
		FLIPPED (Offset + N, (uint8 *) &dd, ScreenColors); \
	} \
        break; \
    case H_FLIP | V_FLIP: \
	bp = pCache + 56 - StartLine; \
	SWAP_DWORD (d1); \
	SWAP_DWORD (d2); \
	for (l = LineCount; l != 0; l--, bp -= 8, Offset += GFX.PPL) \
	{ \
	    if ((dd = *(uint32 *) (bp + 4) & d1)) \
		FLIPPED (Offset, (uint8 *) &dd, ScreenColors); \
	    if ((dd = *(uint32 *) bp & d2)) \
		FLIPPED (Offset + N, (uint8 *) &dd, ScreenColors); \
	} \
        break; \
    case V_FLIP: \
	bp = pCache + 56 - StartLine; \
	for (l = LineCount; l != 0; l--, bp -= 8, Offset += GFX.PPL) \
	{ \
	    if ((dd = (*(uint32 *) bp) & d1)) \
		NORMAL (Offset, (uint8 *) &dd, ScreenColors); \
	    if ((dd = (*(uint32 *) (bp + 4)) & d2)) \
		NORMAL (Offset + N, (uint8 *) &dd, ScreenColors); \
	} \
        break; \
    default: \
        break; \
    }

#define RENDER_TILE_LARGE(PIXEL, FUNCTION) \
    switch (Tile & (V_FLIP | H_FLIP)) \
    { \
    case H_FLIP: \
	StartPixel = 7 - StartPixel; \
        /* fallthrough for no-flip case - above was a horizontal flip */ \
    case 0: \
	if ((pixel = *(pCache + StartLine + StartPixel))) \
	{ \
	    pixel = PIXEL; \
	    for (l = LineCount; l != 0; l--, sp += GFX.PPL, Depth += GFX.PPL) \
	    { \
		for (int z = Pixels - 1; z >= 0; z--) \
		    if (GFX.Z1 > Depth [z]) \
		    { \
			sp [z] = FUNCTION(sp + z, pixel); \
			Depth [z] = GFX.Z2; \
		    }\
	    } \
	} \
        break; \
    case H_FLIP | V_FLIP: \
	StartPixel = 7 - StartPixel; \
        /* fallthrough for V_FLIP-only case - above was a horizontal flip */ \
    case V_FLIP: \
	if ((pixel = *(pCache + 56 - StartLine + StartPixel))) \
	{ \
	    pixel = PIXEL; \
	    for (l = LineCount; l != 0; l--, sp += GFX.PPL, Depth += GFX.PPL) \
	    { \
		for (int z = Pixels - 1; z >= 0; z--) \
		    if (GFX.Z1 > Depth [z]) \
		    { \
			sp [z] = FUNCTION(sp + z, pixel); \
			Depth [z] = GFX.Z2; \
		    }\
	    } \
	} \
        break; \
    default: \
        break; \
    }
#endif

