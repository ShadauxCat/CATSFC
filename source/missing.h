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

#ifndef _MISSING_H_
#define _MISSING_H_

struct HDMA
{
   uint8_t used;
   uint8_t bbus_address;
   uint8_t abus_bank;
   uint16_t abus_address;
   uint8_t indirect_address;
   uint8_t force_table_address_write;
   uint8_t force_table_address_read;
   uint8_t line_count_write;
   uint8_t line_count_read;
};

struct Missing
{
   uint8_t emulate6502;
   uint8_t decimal_mode;
   uint8_t mv_8bit_index;
   uint8_t mv_8bit_acc;
   uint8_t interlace;
   uint8_t lines_239;
   uint8_t pseudo_512;
   struct HDMA hdma [8];
   uint8_t modes [8];
   uint8_t mode7_fx;
   uint8_t mode7_flip;
   uint8_t mode7_bgmode;
   uint8_t direct;
   uint8_t matrix_multiply;
   uint8_t oam_read;
   uint8_t vram_read;
   uint8_t cgram_read;
   uint8_t wram_read;
   uint8_t dma_read;
   uint8_t vram_inc;
   uint8_t vram_full_graphic_inc;
   uint8_t virq;
   uint8_t hirq;
   uint16_t virq_pos;
   uint16_t hirq_pos;
   uint8_t h_v_latch;
   uint8_t h_counter_read;
   uint8_t v_counter_read;
   uint8_t fast_rom;
   uint8_t window1 [6];
   uint8_t window2 [6];
   uint8_t sprite_priority_rotation;
   uint8_t subscreen;
   uint8_t subscreen_add;
   uint8_t subscreen_sub;
   uint8_t fixed_colour_add;
   uint8_t fixed_colour_sub;
   uint8_t mosaic;
   uint8_t sprite_double_height;
   uint8_t dma_channels;
   uint8_t dma_this_frame;
   uint8_t oam_address_read;
   uint8_t bg_offset_read;
   uint8_t matrix_read;
   uint8_t hdma_channels;
   uint8_t hdma_this_frame;
   uint16_t unknownppu_read;
   uint16_t unknownppu_write;
   uint16_t unknowncpu_read;
   uint16_t unknowncpu_write;
   uint16_t unknowndsp_read;
   uint16_t unknowndsp_write;
};

struct Missing missing;
#endif

