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

#ifndef _DSP4_H_
#define _DSP4_H_

// debug
int block;                       // current block number
extern int c;

// op control
int8_t DSP4_Logic;              // controls op flow

// projection format
const int16_t PLANE_START = 0x7fff;   // starting distance

int16_t view_plane;             // viewer location
int16_t far_plane;              // next milestone into screen
int16_t segments;                  // # raster segments to draw
int16_t raster;                    // current raster line

int16_t project_x;              // current x-position
int16_t project_y;              // current y-position

int16_t project_centerx;     // x-target of projection
int16_t project_centery;     // y-target of projection

int16_t project_x1;             // current x-distance
int16_t project_x1low;       // lower 16-bits
int16_t project_y1;             // current y-distance
int16_t project_y1low;       // lower 16-bits

int16_t project_x2;             // next projected x-distance
int16_t project_y2;             // next projected y-distance

int16_t project_pitchx;         // delta center
int16_t project_pitchxlow;   // lower 16-bits
int16_t project_pitchy;         // delta center
int16_t project_pitchylow;   // lower 16-bits

int16_t project_focalx;         // x-point of projection at viewer plane
int16_t project_focaly;         // y-point of projection at viewer plane

int16_t project_ptr;            // data structure pointer

// render window
int16_t center_x;                  // x-center of viewport
int16_t center_y;                  // y-center of viewport
int16_t viewport_left;       // x-left of viewport
int16_t viewport_right;         // x-right of viewport
int16_t viewport_top;           // y-top of viewport
int16_t viewport_bottom;     // y-bottom of viewport

// sprite structure
int16_t sprite_x;                  // projected x-pos of sprite
int16_t sprite_y;                  // projected y-pos of sprite
int16_t sprite_offset;       // data pointer offset
int8_t sprite_type;             // vehicle, terrain
bool sprite_size;            // sprite size: 8x8 or 16x16

// path strips
int16_t path_clipRight[4];      // value to clip to for x>b
int16_t path_clipLeft[4];       // value to clip to for x<a
int16_t path_pos[4];               // x-positions of lanes
int16_t path_ptr[4];               // data structure pointers
int16_t path_raster[4];            // current raster
int16_t path_top[4];               // viewport_top

int16_t path_y[2];                 // current y-position
int16_t path_x[2];                 // current focals
int16_t path_plane[2];          // previous plane

// op09 window sorting
int16_t multi_index1;              // index counter
int16_t multi_index2;              // index counter
bool op09_mode;                 // window mode

// multi-op storage
int16_t multi_focaly[64];       // focal_y values
int16_t multi_farplane[4];      // farthest drawn distance
int16_t multi_raster[4];        // line where track stops

// OAM
int8_t op06_OAM[32];               // OAM (size,MSB) data
int8_t op06_index;                 // index into OAM table
int8_t op06_offset;                // offset into OAM table

#endif
