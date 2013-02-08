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
#include "dsp1.h"
#include "missing.h"
#include "memmap.h"
#include <math.h>

#include "dsp1emu.c"
#include "dsp2emu.c"

void (*SetDSP)(uint8, uint16)=&DSP1SetByte;
uint8 (*GetDSP)(uint16)=&DSP1GetByte;

void S9xInitDSP1 ()
{
    static bool8 init = FALSE;
    
    if (!init)
    {
        InitDSP ();
        init = TRUE;
    }
}

void S9xResetDSP1 ()
{
    S9xInitDSP1 ();
    
    DSP1.waiting4command = TRUE;
    DSP1.in_count = 0;
    DSP1.out_count = 0;
    DSP1.in_index = 0;
    DSP1.out_index = 0;
    DSP1.first_parameter = TRUE;
}

uint8 S9xGetDSP (uint16 address)
{
    uint8 t;
	
#ifdef DEBUGGER
    if (Settings.TraceDSP)
    {
		sprintf (String, "DSP read: 0x%04X", address);
		S9xMessage (S9X_TRACE, S9X_TRACE_DSP1, String);
    }
#endif
 
	t=(*GetDSP)(address);
		//DSP1GetByte(address);
    return (t);
}

void S9xSetDSP (uint8 byte, uint16 address)
{
#ifdef DEBUGGER
    missing.unknowndsp_write = address;
    if (Settings.TraceDSP)
    {
		sprintf (String, "DSP write: 0x%04X=0x%02X", address, byte);
		S9xMessage (S9X_TRACE, S9X_TRACE_DSP1, String);
    }
#endif
	(*SetDSP)(byte, address);
	//DSP1SetByte(byte, address);
}

void DSP1SetByte(uint8 byte, uint16 address)
{
    if( (address & 0xf000) == 0x6000 || (address & 0x7fff) < 0x4000 )
    {
//		if ((address & 1) == 0)
//		{
		if((DSP1.command==0x0A||DSP1.command==0x1A)&&DSP1.out_count!=0)
		{
			DSP1.out_count--;
			DSP1.out_index++;			
			return;
		}
		else if (DSP1.waiting4command)
		{
			DSP1.command = byte;
			DSP1.in_index = 0;
			DSP1.waiting4command = FALSE;
			DSP1.first_parameter = TRUE;
//			printf("Op%02X\n",byte);
			// Mario Kart uses 0x00, 0x02, 0x06, 0x0c, 0x28, 0x0a
			switch (byte)
			{
			case 0x00: DSP1.in_count = 2;	break;
			case 0x30:
			case 0x10: DSP1.in_count = 2;	break;
			case 0x20: DSP1.in_count = 2;	break;
			case 0x24:
			case 0x04: DSP1.in_count = 2;	break;
			case 0x08: DSP1.in_count = 3;	break;
			case 0x18: DSP1.in_count = 4;	break;
			case 0x28: DSP1.in_count = 3;	break;
			case 0x38: DSP1.in_count = 4;	break;
			case 0x2c:
			case 0x0c: DSP1.in_count = 3;	break;
			case 0x3c:
			case 0x1c: DSP1.in_count = 6;	break;
			case 0x32:
			case 0x22:
			case 0x12:
			case 0x02: DSP1.in_count = 7;	break;
			case 0x0a: DSP1.in_count = 1;	break;
			case 0x3a:
			case 0x2a:
			case 0x1a: 
				DSP1. command =0x1a;
				DSP1.in_count = 1;	break;
			case 0x16:
			case 0x26:
			case 0x36:
			case 0x06: DSP1.in_count = 3;	break;
			case 0x1e:
			case 0x2e:
			case 0x3e:
			case 0x0e: DSP1.in_count = 2;	break;
			case 0x05:
			case 0x35:
			case 0x31:
			case 0x01: DSP1.in_count = 4;	break;
			case 0x15:
			case 0x11: DSP1.in_count = 4;	break;
			case 0x25:
			case 0x21: DSP1.in_count = 4;	break;
			case 0x09:
			case 0x39:
			case 0x3d:
			case 0x0d: DSP1.in_count = 3;	break;
			case 0x19:
			case 0x1d: DSP1.in_count = 3;	break;
			case 0x29:
			case 0x2d: DSP1.in_count = 3;	break;
			case 0x33:
			case 0x03: DSP1.in_count = 3;	break;
			case 0x13: DSP1.in_count = 3;	break;
			case 0x23: DSP1.in_count = 3;	break;
			case 0x3b:
			case 0x0b: DSP1.in_count = 3;	break;
			case 0x1b: DSP1.in_count = 3;	break;
			case 0x2b: DSP1.in_count = 3;	break;
			case 0x34:
			case 0x14: DSP1.in_count = 6;	break;
			case 0x07:
			case 0x0f: DSP1.in_count = 1;	break;
			case 0x27:
			case 0x2F: DSP1.in_count=1; break;
			case 0x17:
			case 0x37:
			case 0x3F:
				DSP1.command=0x1f;
			case 0x1f: DSP1.in_count = 1;	break;
				//		    case 0x80: DSP1.in_count = 2;	break;
			default:
				//printf("Op%02X\n",byte);
			case 0x80:
				DSP1.in_count = 0;
				DSP1.waiting4command = TRUE;
				DSP1.first_parameter = TRUE;
				break;
			}
			DSP1.in_count<<=1;
		}
		else
		{
			DSP1.parameters [DSP1.in_index] = byte;
			DSP1.first_parameter = FALSE;
			DSP1.in_index++;
		}
		
		if (DSP1.waiting4command ||
			(DSP1.first_parameter && byte == 0x80))
		{
			DSP1.waiting4command = TRUE;
			DSP1.first_parameter = FALSE;
		}
		else if(DSP1.first_parameter && (DSP1.in_count != 0 || (DSP1.in_count==0&&DSP1.in_index==0)))
		{
		}
//		else if (DSP1.first_parameter)
//		{
//		}
		else
		{
			if (DSP1.in_count)
			{
				//DSP1.parameters [DSP1.in_index] |= (byte << 8);
				if (--DSP1.in_count == 0)
				{
					// Actually execute the command
					DSP1.waiting4command = TRUE;
					DSP1.out_index = 0;
					switch (DSP1.command)
					{
					case 0x1f:
						DSP1.out_count=2048;
						break;
					case 0x00:	// Multiple
						Op00Multiplicand = (int16) (DSP1.parameters [0]|(DSP1.parameters[1]<<8));
						Op00Multiplier = (int16) (DSP1.parameters [2]|(DSP1.parameters[3]<<8));
						
						DSPOp00 ();
						
						DSP1.out_count = 2;
						DSP1.output [0] = Op00Result&0xFF;
						DSP1.output [1] = (Op00Result>>8)&0xFF;
						break;

					case 0x20:	// Multiple
						Op20Multiplicand = (int16) (DSP1.parameters [0]|(DSP1.parameters[1]<<8));
						Op20Multiplier = (int16) (DSP1.parameters [2]|(DSP1.parameters[3]<<8));
						
						DSPOp20 ();
						
						DSP1.out_count = 2;
						DSP1.output [0] = Op20Result&0xFF;
						DSP1.output [1] = (Op20Result>>8)&0xFF;
						break;
						
					case 0x30:
					case 0x10:	// Inverse
						Op10Coefficient = (int16) (DSP1.parameters [0]|(DSP1.parameters[1]<<8));
						Op10Exponent = (int16) (DSP1.parameters [2]|(DSP1.parameters[3]<<8));
						
						DSPOp10 ();
						
						DSP1.out_count = 4;
						DSP1.output [0] = (uint8) (((int16) Op10CoefficientR)&0xFF);
						DSP1.output [1] = (uint8) ((((int16) Op10CoefficientR)>>8)&0xFF);
						DSP1.output [2] = (uint8) (((int16) Op10ExponentR)&0xff);
						DSP1.output [3] = (uint8) ((((int16) Op10ExponentR)>>8)&0xff);
						break;
						
					case 0x24:
					case 0x04:	// Sin and Cos of angle
						Op04Angle = (int16) (DSP1.parameters [0]|(DSP1.parameters[1]<<8));
						Op04Radius = (uint16) (DSP1.parameters [2]|(DSP1.parameters[3]<<8));
						
						DSPOp04 ();
						
						DSP1.out_count = 4;
						DSP1.output [0] = (uint8) (Op04Sin&0xFF);
						DSP1.output [1] = (uint8) ((Op04Sin>>8)&0xFF);
						DSP1.output [2] = (uint8) (Op04Cos&0xFF);
						DSP1.output [3] = (uint8) ((Op04Cos>>8)&0xFF);
						break;
						
					case 0x08:	// Radius
						Op08X = (int16) (DSP1.parameters [0]|(DSP1.parameters[1]<<8));
						Op08Y = (int16) (DSP1.parameters [2]|(DSP1.parameters[3]<<8));
						Op08Z = (int16) (DSP1.parameters [4]|(DSP1.parameters[5]<<8));
						
						DSPOp08 ();
						
						DSP1.out_count = 4;
						DSP1.output [0] = (uint8) (((int16) Op08Ll)&0xFF); 
						DSP1.output [1] = (uint8) ((((int16) Op08Ll)>>8)&0xFF); 
						DSP1.output [2] = (uint8) (((int16) Op08Lh)&0xFF);
						DSP1.output [3] = (uint8) ((((int16) Op08Lh)>>8)&0xFF);
						break;
						
					case 0x18:	// Range
						
						Op18X = (int16) (DSP1.parameters [0]|(DSP1.parameters[1]<<8));
						Op18Y = (int16) (DSP1.parameters [2]|(DSP1.parameters[3]<<8));
						Op18Z = (int16) (DSP1.parameters [4]|(DSP1.parameters[5]<<8));
						Op18R = (int16) (DSP1.parameters [6]|(DSP1.parameters[7]<<8));
						
						DSPOp18 ();
						
						DSP1.out_count = 2;
						DSP1.output [0] = (uint8) (Op18D&0xFF);
						DSP1.output [1] = (uint8) ((Op18D>>8)&0xFF);
						break;

					case 0x38:	// Range
						
						Op38X = (int16) (DSP1.parameters [0]|(DSP1.parameters[1]<<8));
						Op38Y = (int16) (DSP1.parameters [2]|(DSP1.parameters[3]<<8));
						Op38Z = (int16) (DSP1.parameters [4]|(DSP1.parameters[5]<<8));
						Op38R = (int16) (DSP1.parameters [6]|(DSP1.parameters[7]<<8));
						
						DSPOp38 ();
						
						DSP1.out_count = 2;
						DSP1.output [0] = (uint8) (Op38D&0xFF);
						DSP1.output [1] = (uint8) ((Op38D>>8)&0xFF);
						break;
						
					case 0x28:	// Distance (vector length)
						Op28X = (int16) (DSP1.parameters [0]|(DSP1.parameters[1]<<8));
						Op28Y = (int16) (DSP1.parameters [2]|(DSP1.parameters[3]<<8));
						Op28Z = (int16) (DSP1.parameters [4]|(DSP1.parameters[5]<<8));
						
						DSPOp28 ();
						
						DSP1.out_count = 2;
						DSP1.output [0] = (uint8) (Op28R&0xFF);
						DSP1.output [1] = (uint8) ((Op28R>>8)&0xFF);
						break;
						
					case 0x2c:
					case 0x0c:	// Rotate (2D rotate)
						Op0CA = (int16) (DSP1.parameters [0]|(DSP1.parameters[1]<<8));
						Op0CX1 = (int16) (DSP1.parameters [2]|(DSP1.parameters[3]<<8));
						Op0CY1 = (int16) (DSP1.parameters [4]|(DSP1.parameters[5]<<8));
						
						DSPOp0C ();
						
						DSP1.out_count = 4;
						DSP1.output [0] = (uint8) (Op0CX2&0xFF);
						DSP1.output [1] = (uint8) ((Op0CX2>>8)&0xFF);
						DSP1.output [2] = (uint8) (Op0CY2&0xFF);
						DSP1.output [3] = (uint8) ((Op0CY2>>8)&0xFF);
						break;
						
					case 0x3c:
					case 0x1c:	// Polar (3D rotate)
						Op1CZ = (DSP1.parameters [0]|(DSP1.parameters[1]<<8));
						//MK: reversed X and Y on neviksti and John's advice.
						Op1CY = (DSP1.parameters [2]|(DSP1.parameters[3]<<8));
						Op1CX = (DSP1.parameters [4]|(DSP1.parameters[5]<<8));
						Op1CXBR = (DSP1.parameters [6]|(DSP1.parameters[7]<<8));
						Op1CYBR = (DSP1.parameters [8]|(DSP1.parameters[9]<<8));
						Op1CZBR = (DSP1.parameters [10]|(DSP1.parameters[11]<<8));
						
						DSPOp1C ();
						
						DSP1.out_count = 6;
						DSP1.output [0] = (uint8) (Op1CXAR&0xFF);
						DSP1.output [1] = (uint8) ((Op1CXAR>>8)&0xFF);
						DSP1.output [2] = (uint8) (Op1CYAR&0xFF);
						DSP1.output [3] = (uint8) ((Op1CYAR>>8)&0xFF);
						DSP1.output [4] = (uint8) (Op1CZAR&0xFF);
						DSP1.output [5] = (uint8) ((Op1CZAR>>8)&0xFF);
						break;
						
					case 0x32:
					case 0x22:
					case 0x12:
					case 0x02:	// Parameter (Projection)
						Op02FX = (short)(DSP1.parameters [0]|(DSP1.parameters[1]<<8));
						Op02FY = (short)(DSP1.parameters [2]|(DSP1.parameters[3]<<8));
						Op02FZ = (short)(DSP1.parameters [4]|(DSP1.parameters[5]<<8));
						Op02LFE = (short)(DSP1.parameters [6]|(DSP1.parameters[7]<<8));
						Op02LES = (short)(DSP1.parameters [8]|(DSP1.parameters[9]<<8));
						Op02AAS = (unsigned short)(DSP1.parameters [10]|(DSP1.parameters[11]<<8));
						Op02AZS = (unsigned short)(DSP1.parameters [12]|(DSP1.parameters[13]<<8));
						
						DSPOp02 ();
						
						DSP1.out_count = 8;
						DSP1.output [0] = (uint8) (Op02VOF&0xFF);
						DSP1.output [1] = (uint8) ((Op02VOF>>8)&0xFF);
						DSP1.output [2] = (uint8) (Op02VVA&0xFF);
						DSP1.output [3] = (uint8) ((Op02VVA>>8)&0xFF);
						DSP1.output [4] = (uint8) (Op02CX&0xFF);
						DSP1.output [5] = (uint8) ((Op02CX>>8)&0xFF);
						DSP1.output [6] = (uint8) (Op02CY&0xFF);
						DSP1.output [7] = (uint8) ((Op02CY>>8)&0xFF);
						break;
						
					case 0x3a:  //1a Mirror
					case 0x2a:  //1a Mirror
					case 0x1a:	// Raster mode 7 matrix data
					case 0x0a:
						Op0AVS = (short)(DSP1.parameters [0]|(DSP1.parameters[1]<<8));
						
						DSPOp0A ();
						
						DSP1.out_count = 8;
						DSP1.output [0] = (uint8) (Op0AA&0xFF);
						DSP1.output [2] = (uint8) (Op0AB&0xFF);
						DSP1.output [4] = (uint8) (Op0AC&0xFF);
						DSP1.output [6] = (uint8) (Op0AD&0xFF);
						DSP1.output [1] = (uint8) ((Op0AA>>8)&0xFF);
						DSP1.output [3] = (uint8) ((Op0AB>>8)&0xFF);
						DSP1.output [5] = (uint8) ((Op0AC>>8)&0xFF);
						DSP1.output [7] = (uint8) ((Op0AD>>8)&0xFF);
						DSP1.in_index=0;
						break;
						
					case 0x16:
					case 0x26:
					case 0x36:
					case 0x06:	// Project object
						Op06X = (int16) (DSP1.parameters [0]|(DSP1.parameters[1]<<8));
						Op06Y = (int16) (DSP1.parameters [2]|(DSP1.parameters[3]<<8));
						Op06Z = (int16) (DSP1.parameters [4]|(DSP1.parameters[5]<<8));
						
						DSPOp06 ();
						
						DSP1.out_count = 6;
						DSP1.output [0] = (uint8) (Op06H&0xff);
						DSP1.output [1] = (uint8) ((Op06H>>8)&0xFF);
						DSP1.output [2] = (uint8) (Op06V&0xFF);
						DSP1.output [3] = (uint8) ((Op06V>>8)&0xFF);
						DSP1.output [4] = (uint8) (Op06S&0xFF);
						DSP1.output [5] = (uint8) ((Op06S>>8)&0xFF);
						break;
						
					case 0x1e:
					case 0x2e:
					case 0x3e:
					case 0x0e:	// Target
						Op0EH = (int16) (DSP1.parameters [0]|(DSP1.parameters[1]<<8));
						Op0EV = (int16) (DSP1.parameters [2]|(DSP1.parameters[3]<<8));
						
						DSPOp0E ();
						
						DSP1.out_count = 4;
						DSP1.output [0] = (uint8) (Op0EX&0xFF);
						DSP1.output [1] = (uint8) ((Op0EX>>8)&0xFF);
						DSP1.output [2] = (uint8) (Op0EY&0xFF);
						DSP1.output [3] = (uint8) ((Op0EY>>8)&0xFF);
						break;
						
						// Extra commands used by Pilot Wings
					case 0x05:
					case 0x35:
					case 0x31:
					case 0x01: // Set attitude matrix A
						Op01m = (int16) (DSP1.parameters [0]|(DSP1.parameters[1]<<8));
						Op01Zr = (int16) (DSP1.parameters [2]|(DSP1.parameters[3]<<8));
						Op01Yr = (int16) (DSP1.parameters [4]|(DSP1.parameters[5]<<8));
						Op01Xr = (int16) (DSP1.parameters [6]|(DSP1.parameters[7]<<8));
						
						DSPOp01 ();
						break;
					
					case 0x15:	
					case 0x11:	// Set attitude matrix B
						Op11m = (int16) (DSP1.parameters [0]|(DSP1.parameters[1]<<8));
						Op11Zr = (int16) (DSP1.parameters [2]|(DSP1.parameters[3]<<8));
						Op11Yr = (int16) (DSP1.parameters [4]|(DSP1.parameters[5]<<8));
						Op11Xr = (int16) (DSP1.parameters [7]|(DSP1.parameters[7]<<8));
						
						DSPOp11 ();
						break;
						
					case 0x25:
					case 0x21:	// Set attitude matrix C
						Op21m = (int16) (DSP1.parameters [0]|(DSP1.parameters[1]<<8));
						Op21Zr = (int16) (DSP1.parameters [2]|(DSP1.parameters[3]<<8));
						Op21Yr = (int16) (DSP1.parameters [4]|(DSP1.parameters[5]<<8));
						Op21Xr = (int16) (DSP1.parameters [6]|(DSP1.parameters[7]<<8));
						
						DSPOp21 ();
						break;
						
					case 0x09:
					case 0x39:
					case 0x3d:
					case 0x0d:	// Objective matrix A
						Op0DX = (int16) (DSP1.parameters [0]|(DSP1.parameters[1]<<8));
						Op0DY = (int16) (DSP1.parameters [2]|(DSP1.parameters[3]<<8));
						Op0DZ = (int16) (DSP1.parameters [4]|(DSP1.parameters[5]<<8));
						
						DSPOp0D ();
						
						DSP1.out_count = 6;
						DSP1.output [0] = (uint8) (Op0DF&0xFF);
						DSP1.output [1] = (uint8) ((Op0DF>>8)&0xFF);
						DSP1.output [2] = (uint8) (Op0DL&0xFF);
						DSP1.output [3] = (uint8) ((Op0DL>>8)&0xFF);
						DSP1.output [4] = (uint8) (Op0DU&0xFF);
						DSP1.output [5] = (uint8) ((Op0DU>>8)&0xFF);
						break;
						
					case 0x19:
					case 0x1d:	// Objective matrix B
						Op1DX = (int16) (DSP1.parameters [0]|(DSP1.parameters[1]<<8));
						Op1DY = (int16) (DSP1.parameters [2]|(DSP1.parameters[3]<<8));
						Op1DZ = (int16) (DSP1.parameters [4]|(DSP1.parameters[5]<<8));
						
						DSPOp1D ();
						
						DSP1.out_count = 6;
						DSP1.output [0] = (uint8) (Op1DF&0xFF);
						DSP1.output [1] = (uint8) ((Op1DF>>8)&0xFF);
						DSP1.output [2] = (uint8) (Op1DL&0xFF);
						DSP1.output [3] = (uint8) ((Op1DL>>8)&0xFF);
						DSP1.output [4] = (uint8) (Op1DU&0xFF);
						DSP1.output [5] = (uint8) ((Op1DU>>8)&0xFF);
						break;
						
					case 0x29:
					case 0x2d:	// Objective matrix C
						Op2DX = (int16) (DSP1.parameters [0]|(DSP1.parameters[1]<<8));
						Op2DY = (int16) (DSP1.parameters [2]|(DSP1.parameters[3]<<8));
						Op2DZ = (int16) (DSP1.parameters [4]|(DSP1.parameters[5]<<8));
						
						DSPOp2D ();
						
						DSP1.out_count = 6;
						DSP1.output [0] = (uint8) (Op2DF&0xFF);
						DSP1.output [1] = (uint8) ((Op2DF>>8)&0xFF);
						DSP1.output [2] = (uint8) (Op2DL&0xFF);
						DSP1.output [3] = (uint8) ((Op2DL>>8)&0xFF);
						DSP1.output [4] = (uint8) (Op2DU&0xFF);
						DSP1.output [5] = (uint8) ((Op2DU>>8)&0xFF);
						break;
							
					case 0x33:
					case 0x03:	// Subjective matrix A
						Op03F = (int16) (DSP1.parameters [0]|(DSP1.parameters[1]<<8));
						Op03L = (int16) (DSP1.parameters [2]|(DSP1.parameters[3]<<8));
						Op03U = (int16) (DSP1.parameters [4]|(DSP1.parameters[5]<<8));
						
						DSPOp03 ();
						
						DSP1.out_count = 6;
						DSP1.output [0] = (uint8) (Op03X&0xFF);
						DSP1.output [1] = (uint8) ((Op03X>>8)&0xFF);
						DSP1.output [2] = (uint8) (Op03Y&0xFF);
						DSP1.output [3] = (uint8) ((Op03Y>>8)&0xFF);
						DSP1.output [4] = (uint8) (Op03Z&0xFF);
						DSP1.output [5] = (uint8) ((Op03Z>>8)&0xFF);
						break;
						
					case 0x13:	// Subjective matrix B
						Op13F = (int16) (DSP1.parameters [0]|(DSP1.parameters[1]<<8));
						Op13L = (int16) (DSP1.parameters [2]|(DSP1.parameters[3]<<8));
						Op13U = (int16) (DSP1.parameters [4]|(DSP1.parameters[5]<<8));
						
						DSPOp13 ();
						
						DSP1.out_count = 6;
						DSP1.output [0] = (uint8) (Op13X&0xFF);
						DSP1.output [1] = (uint8) ((Op13X>>8)&0xFF);
						DSP1.output [2] = (uint8) (Op13Y&0xFF);
						DSP1.output [3] = (uint8) ((Op13Y>>8)&0xFF);
						DSP1.output [4] = (uint8) (Op13Z&0xFF);
						DSP1.output [5] = (uint8) ((Op13Z>>8)&0xFF);
						break;
						
					case 0x23:	// Subjective matrix C
						Op23F = (int16) (DSP1.parameters [0]|(DSP1.parameters[1]<<8));
						Op23L = (int16) (DSP1.parameters [2]|(DSP1.parameters[3]<<8));
						Op23U = (int16) (DSP1.parameters [4]|(DSP1.parameters[5]<<8));
						
						DSPOp23 ();
						
						DSP1.out_count = 6;
						DSP1.output [0] = (uint8) (Op23X&0xFF);
						DSP1.output [1] = (uint8) ((Op23X>>8)&0xFF);
						DSP1.output [2] = (uint8) (Op23Y&0xFF);
						DSP1.output [3] = (uint8) ((Op23Y>>8)&0xFF);
						DSP1.output [4] = (uint8) (Op23Z&0xFF);
						DSP1.output [5] = (uint8) ((Op23Z>>8)&0xFF);
						break;
						
					case 0x3b:
					case 0x0b:
						Op0BX = (int16) (DSP1.parameters [0]|(DSP1.parameters[1]<<8));
						Op0BY = (int16) (DSP1.parameters [2]|(DSP1.parameters[3]<<8));
						Op0BZ = (int16) (DSP1.parameters [4]|(DSP1.parameters[5]<<8));
						
						DSPOp0B ();
						
						DSP1.out_count = 2;
						DSP1.output [0] = (uint8) (Op0BS&0xFF);
						DSP1.output [1] = (uint8) ((Op0BS>>8)&0xFF);
						break;
						
					case 0x1b:
						Op1BX = (int16) (DSP1.parameters [0]|(DSP1.parameters[1]<<8));
						Op1BY = (int16) (DSP1.parameters [2]|(DSP1.parameters[3]<<8));
						Op1BZ = (int16) (DSP1.parameters [4]|(DSP1.parameters[5]<<8));
						
						DSPOp1B ();
						
						DSP1.out_count = 2;
						DSP1.output [0] = (uint8) (Op1BS&0xFF);
						DSP1.output [1] = (uint8) ((Op1BS>>8)&0xFF);
						break;
						
					case 0x2b:
						Op2BX = (int16) (DSP1.parameters [0]|(DSP1.parameters[1]<<8));
						Op2BY = (int16) (DSP1.parameters [2]|(DSP1.parameters[3]<<8));
						Op2BZ = (int16) (DSP1.parameters [4]|(DSP1.parameters[5]<<8));
						
						DSPOp2B ();
						
						DSP1.out_count = 2;
						DSP1.output [0] = (uint8) (Op2BS&0xFF);
						DSP1.output [1] = (uint8) ((Op2BS>>8)&0xFF);
						break;
						
					case 0x34:
					case 0x14:	
						Op14Zr = (int16) (DSP1.parameters [0]|(DSP1.parameters[1]<<8));
						Op14Xr = (int16) (DSP1.parameters [2]|(DSP1.parameters[3]<<8));
						Op14Yr = (int16) (DSP1.parameters [4]|(DSP1.parameters[5]<<8));
						Op14U = (int16) (DSP1.parameters [6]|(DSP1.parameters[7]<<8));
						Op14F = (int16) (DSP1.parameters [8]|(DSP1.parameters[9]<<8));
						Op14L = (int16) (DSP1.parameters [10]|(DSP1.parameters[11]<<8));
						
						DSPOp14 ();
						
						DSP1.out_count = 6;
						DSP1.output [0] = (uint8) (Op14Zrr&0xFF);
						DSP1.output [1] = (uint8) ((Op14Zrr>>8)&0xFF);
						DSP1.output [2] = (uint8) (Op14Xrr&0xFF);
						DSP1.output [3] = (uint8) ((Op14Xrr>>8)&0xFF);
						DSP1.output [4] = (uint8) (Op14Yrr&0xFF);
						DSP1.output [5] = (uint8) ((Op14Yrr>>8)&0xFF);
						break;
					
					case 0x27:
					case 0x2F:
						Op2FUnknown = (int16) (DSP1.parameters [0]|(DSP1.parameters[1]<<8));
						
						DSPOp2F ();
						
						DSP1.out_count = 2;
						DSP1.output [0] = (uint8)(Op2FSize&0xFF);
						DSP1.output [1] = (uint8)((Op2FSize>>8)&0xFF);
						break;
						
	
					case 0x07:
					case 0x0F:
						Op0FRamsize = (int16) (DSP1.parameters [0]|(DSP1.parameters[1]<<8));
						
						DSPOp0F ();
						
						DSP1.out_count = 2;
						DSP1.output [0] = (uint8)(Op0FPass&0xFF);
						DSP1.output [1] = (uint8)((Op0FPass>>8)&0xFF);
						break;
						
					default:
						break;
					}
				}
			}
		}
    }
}

uint8 DSP1GetByte(uint16 address)
{
	uint8 t;
    if ((address & 0xf000) == 0x6000 ||
//		(address >= 0x8000 && address < 0xc000))
		(address&0x7fff) < 0x4000)
    {
		if (DSP1.out_count)
		{
			//if ((address & 1) == 0)
				t = (uint8) DSP1.output [DSP1.out_index];
			//else
			//{
			//	t = (uint8) (DSP1.output [DSP1.out_index] >> 8);
				DSP1.out_index++;
				if (--DSP1.out_count == 0)
				{
					if (DSP1.command == 0x1a || DSP1.command == 0x0a)
					{
						DSPOp0A ();
						DSP1.out_count = 8;
						DSP1.out_index = 0;
						DSP1.output [0] = (Op0AA&0xFF);
						DSP1.output [1] = (Op0AA>>8)&0xFF;
						DSP1.output [2] = (Op0AB&0xFF);
						DSP1.output [3] = (Op0AB>>8)&0xFF;
						DSP1.output [4] = (Op0AC&0xFF);
						DSP1.output [5] = (Op0AC>>8)&0xFF;
						DSP1.output [6] = (Op0AD&0xFF);
						DSP1.output [7] = (Op0AD>>8)&0xFF;
					}
					if(DSP1.command==0x1f)
					{
						if((DSP1.out_index%2)!=0)
						{
							t=(uint8)DSP1ROM[DSP1.out_index>>1];
						}
						else
						{
							t=DSP1ROM[DSP1.out_index>>1]>>8;
						}
					}
				}
				DSP1.waiting4command = TRUE;
			//}
		}
		else
		{
			// Top Gear 3000 requires this value....
	//		if(4==Settings.DSPVersion)
				t = 0xff;
			//Ballz3d requires this one:
	//		else t = 0x00;
		}
    }
    else t = 0x80;
	return t;
}

void DSP2SetByte(uint8 byte, uint16 address)
{
	if ((address & 0xf000) == 0x6000 ||
		(address >= 0x8000 && address < 0xc000))
    {
		if (DSP1.waiting4command)
		{
			DSP1.command = byte;
			DSP1.in_index = 0;
			DSP1.waiting4command = FALSE;
//			DSP1.first_parameter = TRUE;
//			printf("Op%02X\n",byte);
			switch (byte)
			{
			case 0x01:DSP1.in_count=32;break;
			case 0x03:DSP1.in_count=1;break;
			case 0x05:DSP1.in_count=1;break;
			case 0x09:DSP1.in_count=4;break;
			case 0x06:DSP1.in_count=1;break;
			case 0x0D:DSP1.in_count=2;break;
			default:
				printf("Op%02X\n",byte);
			case 0x0f:DSP1.in_count=0;break;
			}
		}
		else
		{
			DSP1.parameters [DSP1.in_index] = byte;
//			DSP1.first_parameter = FALSE;
			DSP1.in_index++;
		}
		
		if (DSP1.in_count==DSP1.in_index)
		{
			//DSP1.parameters [DSP1.in_index] |= (byte << 8);
			// Actually execute the command
			DSP1.waiting4command = TRUE;
			DSP1.out_index = 0;
			switch (DSP1.command)
			{
			case 0x0D:
				if(DSP2Op0DHasLen)
				{
					DSP2Op0DHasLen=false;
					DSP1.out_count=DSP2Op0DOutLen;
					//execute Op5
					DSP2_Op0D();
				}
				else
				{
					DSP2Op0DInLen=DSP1.parameters[0];
					DSP2Op0DOutLen=DSP1.parameters[1];
					DSP1.in_index=0;
					DSP1.in_count=(DSP2Op0DInLen+1)>>1;
					DSP2Op0DHasLen=true;
					if(byte)
						DSP1.waiting4command=false;
				}
				break;
			case 0x06:
				if(DSP2Op06HasLen)
				{
					DSP2Op06HasLen=false;
					DSP1.out_count=DSP2Op06Len;
					//execute Op5
					DSP2_Op06();
				}
				else
				{
					DSP2Op06Len=DSP1.parameters[0];
					DSP1.in_index=0;
					DSP1.in_count=DSP2Op06Len;
					DSP2Op06HasLen=true;
					if(byte)
						DSP1.waiting4command=false;
				}
				break;
			case 0x01:
				DSP1.out_count=32;
				DSP2_Op01();
				break;
			case 0x09:
				// Multiply - don't yet know if this is signed or unsigned
				DSP2Op09Word1 = DSP1.parameters[0] | (DSP1.parameters[1]<<8);
                DSP2Op09Word2 = DSP1.parameters[2] | (DSP1.parameters[3]<<8);
				DSP1.out_count=4;
#ifdef FAST_LSB_WORD_ACCESS
                *(uint32 *)DSP1.output = DSP2Op09Word1 * DSP2Op09Word2;
#else
				uint32 temp;
				temp=DSP2Op09Word1 * DSP2Op09Word2;
				DSP1.output[0]=temp&0xFF;
				DSP1.output[1]=(temp>>8)&0xFF;
				DSP1.output[2]=(temp>>16)&0xFF;
				DSP1.output[3]=(temp>>24)&0xFF;
#endif
				break;
			case 0x05:
				if(DSP2Op05HasLen)
				{
					DSP2Op05HasLen=false;
					DSP1.out_count=DSP2Op05Len;
					//execute Op5
					DSP2_Op05();
				}
				else
				{
					DSP2Op05Len=DSP1.parameters[0];
					DSP1.in_index=0;
					DSP1.in_count=2*DSP2Op05Len;
					DSP2Op05HasLen=true;
					if(byte)
						DSP1.waiting4command=false;
				}
				break;

			case 0x03:
				DSP2Op05Transparent= DSP1.parameters[0];
				//DSP2Op03();
				break;
			case 0x0f:
				default:
					break;
			}
		}
	}
}

uint8 DSP2GetByte(uint16 address)
{
	uint8 t;
    if ((address & 0xf000) == 0x6000 ||
		(address >= 0x8000 && address < 0xc000))
    {
		if (DSP1.out_count)
		{
			t = (uint8) DSP1.output [DSP1.out_index];
			DSP1.out_index++;
			if(DSP1.out_count==DSP1.out_index)
				DSP1.out_count=0;
		}
		else
		{
			t = 0xff;
		}
    }
    else t = 0x80;
	return t;
}

//Disable non-working chips?
#ifdef DSP_DUMMY_LOOPS
//static const uint16	DSP1ROM[1024]  from SNES9X v1.53
uint16 Dsp3Rom[1024] = {
         0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,
	 0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,
	 0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,
	 0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,
	 0x0000,  0x0000,  0x0001,  0x0002,  0x0004,  0x0008,  0x0010,  0x0020,
	 0x0040,  0x0080,  0x0100,  0x0200,  0x0400,  0x0800,  0x1000,  0x2000,
	 0x4000,  0x7fff,  0x4000,  0x2000,  0x1000,  0x0800,  0x0400,  0x0200,
	 0x0100,  0x0080,  0x0040,  0x0020,  0x0001,  0x0008,  0x0004,  0x0002,
	 0x0001,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,
	 0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,
	 0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,
	 0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,
	 0x0000,  0x0000,  0x8000,  0xffe5,  0x0100,  0x7fff,  0x7f02,  0x7e08,
	 0x7d12,  0x7c1f,  0x7b30,  0x7a45,  0x795d,  0x7878,  0x7797,  0x76ba,
	 0x75df,  0x7507,  0x7433,  0x7361,  0x7293,  0x71c7,  0x70fe,  0x7038,
	 0x6f75,  0x6eb4,  0x6df6,  0x6d3a,  0x6c81,  0x6bca,  0x6b16,  0x6a64,
	 0x69b4,  0x6907,  0x685b,  0x67b2,  0x670b,  0x6666,  0x65c4,  0x6523,
	 0x6484,  0x63e7,  0x634c,  0x62b3,  0x621c,  0x6186,  0x60f2,  0x6060,
	 0x5fd0,  0x5f41,  0x5eb5,  0x5e29,  0x5d9f,  0x5d17,  0x5c91,  0x5c0c,
	 0x5b88,  0x5b06,  0x5a85,  0x5a06,  0x5988,  0x590b,  0x5890,  0x5816,
	 0x579d,  0x5726,  0x56b0,  0x563b,  0x55c8,  0x5555,  0x54e4,  0x5474,
	 0x5405,  0x5398,  0x532b,  0x52bf,  0x5255,  0x51ec,  0x5183,  0x511c,
	 0x50b6,  0x5050,  0x4fec,  0x4f89,  0x4f26,  0x4ec5,  0x4e64,  0x4e05,
	 0x4da6,  0x4d48,  0x4cec,  0x4c90,  0x4c34,  0x4bda,  0x4b81,  0x4b28,
	 0x4ad0,  0x4a79,  0x4a23,  0x49cd,  0x4979,  0x4925,  0x48d1,  0x487f,
	 0x482d,  0x47dc,  0x478c,  0x473c,  0x46ed,  0x469f,  0x4651,  0x4604,
	 0x45b8,  0x456c,  0x4521,  0x44d7,  0x448d,  0x4444,  0x43fc,  0x43b4,
	 0x436d,  0x4326,  0x42e0,  0x429a,  0x4255,  0x4211,  0x41cd,  0x4189,
	 0x4146,  0x4104,  0x40c2,  0x4081,  0x4040,  0x3fff,  0x41f7,  0x43e1,
	 0x45bd,  0x478d,  0x4951,  0x4b0b,  0x4cbb,  0x4e61,  0x4fff,  0x5194,
	 0x5322,  0x54a9,  0x5628,  0x57a2,  0x5914,  0x5a81,  0x5be9,  0x5d4a,
	 0x5ea7,  0x5fff,  0x6152,  0x62a0,  0x63ea,  0x6530,  0x6672,  0x67b0,
	 0x68ea,  0x6a20,  0x6b53,  0x6c83,  0x6daf,  0x6ed9,  0x6fff,  0x7122,
	 0x7242,  0x735f,  0x747a,  0x7592,  0x76a7,  0x77ba,  0x78cb,  0x79d9,
	 0x7ae5,  0x7bee,  0x7cf5,  0x7dfa,  0x7efe,  0x7fff,  0x0000,  0x0324,
	 0x0647,  0x096a,  0x0c8b,  0x0fab,  0x12c8,  0x15e2,  0x18f8,  0x1c0b,
	 0x1f19,  0x2223,  0x2528,  0x2826,  0x2b1f,  0x2e11,  0x30fb,  0x33de,
	 0x36ba,  0x398c,  0x3c56,  0x3f17,  0x41ce,  0x447a,  0x471c,  0x49b4,
	 0x4c3f,  0x4ebf,  0x5133,  0x539b,  0x55f5,  0x5842,  0x5a82,  0x5cb4,
	 0x5ed7,  0x60ec,  0x62f2,  0x64e8,  0x66cf,  0x68a6,  0x6a6d,  0x6c24,
	 0x6dca,  0x6f5f,  0x70e2,  0x7255,  0x73b5,  0x7504,  0x7641,  0x776c,
	 0x7884,  0x798a,  0x7a7d,  0x7b5d,  0x7c29,  0x7ce3,  0x7d8a,  0x7e1d,
	 0x7e9d,  0x7f09,  0x7f62,  0x7fa7,  0x7fd8,  0x7ff6,  0x7fff,  0x7ff6,
	 0x7fd8,  0x7fa7,  0x7f62,  0x7f09,  0x7e9d,  0x7e1d,  0x7d8a,  0x7ce3,
	 0x7c29,  0x7b5d,  0x7a7d,  0x798a,  0x7884,  0x776c,  0x7641,  0x7504,
	 0x73b5,  0x7255,  0x70e2,  0x6f5f,  0x6dca,  0x6c24,  0x6a6d,  0x68a6,
	 0x66cf,  0x64e8,  0x62f2,  0x60ec,  0x5ed7,  0x5cb4,  0x5a82,  0x5842,
	 0x55f5,  0x539b,  0x5133,  0x4ebf,  0x4c3f,  0x49b4,  0x471c,  0x447a,
	 0x41ce,  0x3f17,  0x3c56,  0x398c,  0x36ba,  0x33de,  0x30fb,  0x2e11,
	 0x2b1f,  0x2826,  0x2528,  0x2223,  0x1f19,  0x1c0b,  0x18f8,  0x15e2,
	 0x12c8,  0x0fab,  0x0c8b,  0x096a,  0x0647,  0x0324,  0x7fff,  0x7ff6,
	 0x7fd8,  0x7fa7,  0x7f62,  0x7f09,  0x7e9d,  0x7e1d,  0x7d8a,  0x7ce3,
	 0x7c29,  0x7b5d,  0x7a7d,  0x798a,  0x7884,  0x776c,  0x7641,  0x7504,
	 0x73b5,  0x7255,  0x70e2,  0x6f5f,  0x6dca,  0x6c24,  0x6a6d,  0x68a6,
	 0x66cf,  0x64e8,  0x62f2,  0x60ec,  0x5ed7,  0x5cb4,  0x5a82,  0x5842,
	 0x55f5,  0x539b,  0x5133,  0x4ebf,  0x4c3f,  0x49b4,  0x471c,  0x447a,
	 0x41ce,  0x3f17,  0x3c56,  0x398c,  0x36ba,  0x33de,  0x30fb,  0x2e11,
	 0x2b1f,  0x2826,  0x2528,  0x2223,  0x1f19,  0x1c0b,  0x18f8,  0x15e2,
	 0x12c8,  0x0fab,  0x0c8b,  0x096a,  0x0647,  0x0324,  0x0000,  0xfcdc,
	 0xf9b9,  0xf696,  0xf375,  0xf055,  0xed38,  0xea1e,  0xe708,  0xe3f5,
	 0xe0e7,  0xdddd,  0xdad8,  0xd7da,  0xd4e1,  0xd1ef,  0xcf05,  0xcc22,
	 0xc946,  0xc674,  0xc3aa,  0xc0e9,  0xbe32,  0xbb86,  0xb8e4,  0xb64c,
	 0xb3c1,  0xb141,  0xaecd,  0xac65,  0xaa0b,  0xa7be,  0xa57e,  0xa34c,
	 0xa129,  0x9f14,  0x9d0e,  0x9b18,  0x9931,  0x975a,  0x9593,  0x93dc,
	 0x9236,  0x90a1,  0x8f1e,  0x8dab,  0x8c4b,  0x8afc,  0x89bf,  0x8894,
	 0x877c,  0x8676,  0x8583,  0x84a3,  0x83d7,  0x831d,  0x8276,  0x81e3,
	 0x8163,  0x80f7,  0x809e,  0x8059,  0x8028,  0x800a,  0x6488,  0x0080,
	 0x03ff,  0x0116,  0x0002,  0x0080,  0x4000,  0x3fd7,  0x3faf,  0x3f86,
	 0x3f5d,  0x3f34,  0x3f0c,  0x3ee3,  0x3eba,  0x3e91,  0x3e68,  0x3e40,
	 0x3e17,  0x3dee,  0x3dc5,  0x3d9c,  0x3d74,  0x3d4b,  0x3d22,  0x3cf9,
	 0x3cd0,  0x3ca7,  0x3c7f,  0x3c56,  0x3c2d,  0x3c04,  0x3bdb,  0x3bb2,
	 0x3b89,  0x3b60,  0x3b37,  0x3b0e,  0x3ae5,  0x3abc,  0x3a93,  0x3a69,
	 0x3a40,  0x3a17,  0x39ee,  0x39c5,  0x399c,  0x3972,  0x3949,  0x3920,
	 0x38f6,  0x38cd,  0x38a4,  0x387a,  0x3851,  0x3827,  0x37fe,  0x37d4,
	 0x37aa,  0x3781,  0x3757,  0x372d,  0x3704,  0x36da,  0x36b0,  0x3686,
	 0x365c,  0x3632,  0x3609,  0x35df,  0x35b4,  0x358a,  0x3560,  0x3536,
	 0x350c,  0x34e1,  0x34b7,  0x348d,  0x3462,  0x3438,  0x340d,  0x33e3,
	 0x33b8,  0x338d,  0x3363,  0x3338,  0x330d,  0x32e2,  0x32b7,  0x328c,
	 0x3261,  0x3236,  0x320b,  0x31df,  0x31b4,  0x3188,  0x315d,  0x3131,
	 0x3106,  0x30da,  0x30ae,  0x3083,  0x3057,  0x302b,  0x2fff,  0x2fd2,
	 0x2fa6,  0x2f7a,  0x2f4d,  0x2f21,  0x2ef4,  0x2ec8,  0x2e9b,  0x2e6e,
	 0x2e41,  0x2e14,  0x2de7,  0x2dba,  0x2d8d,  0x2d60,  0x2d32,  0x2d05,
	 0x2cd7,  0x2ca9,  0x2c7b,  0x2c4d,  0x2c1f,  0x2bf1,  0x2bc3,  0x2b94,
	 0x2b66,  0x2b37,  0x2b09,  0x2ada,  0x2aab,  0x2a7c,  0x2a4c,  0x2a1d,
	 0x29ed,  0x29be,  0x298e,  0x295e,  0x292e,  0x28fe,  0x28ce,  0x289d,
	 0x286d,  0x283c,  0x280b,  0x27da,  0x27a9,  0x2777,  0x2746,  0x2714,
	 0x26e2,  0x26b0,  0x267e,  0x264c,  0x2619,  0x25e7,  0x25b4,  0x2581,
	 0x254d,  0x251a,  0x24e6,  0x24b2,  0x247e,  0x244a,  0x2415,  0x23e1,
	 0x23ac,  0x2376,  0x2341,  0x230b,  0x22d6,  0x229f,  0x2269,  0x2232,
	 0x21fc,  0x21c4,  0x218d,  0x2155,  0x211d,  0x20e5,  0x20ad,  0x2074,
	 0x203b,  0x2001,  0x1fc7,  0x1f8d,  0x1f53,  0x1f18,  0x1edd,  0x1ea1,
	 0x1e66,  0x1e29,  0x1ded,  0x1db0,  0x1d72,  0x1d35,  0x1cf6,  0x1cb8,
	 0x1c79,  0x1c39,  0x1bf9,  0x1bb8,  0x1b77,  0x1b36,  0x1af4,  0x1ab1,
	 0x1a6e,  0x1a2a,  0x19e6,  0x19a1,  0x195c,  0x1915,  0x18ce,  0x1887,
	 0x183f,  0x17f5,  0x17ac,  0x1761,  0x1715,  0x16c9,  0x167c,  0x162e,
	 0x15df,  0x158e,  0x153d,  0x14eb,  0x1497,  0x1442,  0x13ec,  0x1395,
	 0x133c,  0x12e2,  0x1286,  0x1228,  0x11c9,  0x1167,  0x1104,  0x109e,
	 0x1036,  0x0fcc,  0x0f5f,  0x0eef,  0x0e7b,  0x0e04,  0x0d89,  0x0d0a,
	 0x0c86,  0x0bfd,  0x0b6d,  0x0ad6,  0x0a36,  0x098d,  0x08d7,  0x0811,
	 0x0736,  0x063e,  0x0519,  0x039a,  0x0000,  0x7fff,  0x0100,  0x0080,
	 0x021d,  0x00c8,  0x00ce,  0x0048,  0x0a26,  0x277a,  0x00ce,  0x6488,
	 0x14ac,  0x0001,  0x00f9,  0x00fc,  0x00ff,  0x00fc,  0x00f9,  0xffff,
	 0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,
	 0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,
	 0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,
	 0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,
	 0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,
	 0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,
	 0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,
	 0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,
	 0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,
	 0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,
	 0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,
	 0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,
	 0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,
	 0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,
	 0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,
	 0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,
	 0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,
	 0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,
	 0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,
	 0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,
	 0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,
	 0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,
	 0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,
	 0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,
	 0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,
	 0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff

};

void DSP3SetByte(uint8 byte, uint16 address)
{
	if ((address & 0xf000) == 0x6000 ||
		(address >= 0x8000 && address < 0xc000))
    {
		if (DSP1.waiting4command)
		{
			DSP1.command = byte;
			DSP1.in_index = 0;
			DSP1.waiting4command = FALSE;
//			DSP1.first_parameter = TRUE;
//			printf("Op%02X\n",byte);
			switch (byte)
			{
			case 0x2F:DSP1.in_count=2;break;
			case 0x1F:DSP1.in_count=2;break;
			case 0x0F:DSP1.in_count=2;break;
			case 0x38:DSP1.in_count=4;break;
			default:
//				printf("Op%02X\n",byte);
				break;
			}
		}
		else
		{
			DSP1.parameters [DSP1.in_index] = byte;
//			DSP1.first_parameter = FALSE;
			DSP1.in_index++;
		}
		
		if (DSP1.in_count==DSP1.in_index)
		{
			//DSP1.parameters [DSP1.in_index] |= (byte << 8);
			// Actually execute the command
			DSP1.waiting4command = TRUE;
			DSP1.out_index = 0;
			switch (DSP1.command)
			{
			case 0x2F:DSP1.out_count=2;break;
			case 0x1F:DSP1.out_count=2048;break;
			case 0x0F:DSP1.out_count=2;
					DSP1.output[0]=0;
					DSP1.output[1]=0;
					break;
			case 0x38:
				{
					DSP1.out_count=2;
					// 176B
					DSP1.output[0]=0;
					DSP1.output[1]=0x80;

					break;
				}
				default:
					break;
			}
		}
	}
}

uint8 DSP3GetByte(uint16 address)
{
	uint8 t;
    if ((address & 0xf000) == 0x6000 ||
		(address >= 0x8000 && address < 0xc000))
    {
		if(DSP1.command==0x38&&DSP1.out_index==1)
		{
			t=4;
		}

		if (DSP1.out_count)
		{
			if(DSP1.command==0x1f)
			{
				if((DSP1.out_index%2)!=0)
				{
					t=(uint8)Dsp3Rom[DSP1.out_index>>1];
				}
				else
				{
					t=Dsp3Rom[DSP1.out_index>>1]>>8;
				}
//				t=Dsp3Rom[DSP1.out_index];
				DSP1.out_index++;
			}
			else
			{
				t = (uint8) DSP1.output [DSP1.out_index];
				DSP1.out_index++;
				DSP1.out_index%=512;
				if(DSP1.out_count==DSP1.out_index)
					DSP1.out_count=0;
			}
		}
		else
		{
			t = 0xff;
		}
    }
    else
	{
		t = 0x80;
/*		if(DSP1.command=0x38&&DSP1.out_count==0)
		{
			t=0xC0;
			static int Op38c;
			if(Op38c==14)
			{
				Op38c=0;
				t=0x80;
				DSP1.in_count=4;
			}
			Op38c++;
		}*/
	}
	return t;
}

#endif

struct SDSP4 {
    bool8 waiting4command;
    bool8 half_command;
    uint16 command;
    uint32 in_count;
    uint32 in_index;
    uint32 out_count;
    uint32 out_index;
    uint8 parameters [512];
    uint8 output [512];
};

SDSP4 DSP4;

#include "dsp4emu.cpp"

bool DSP4_init=FALSE;

void DSP4SetByte(uint8 byte, uint16 address)
{
	if(!DSP4_init)
	{
		// bootup
		DSP4.waiting4command=1;
		DSP4_init=TRUE;
	}

	if ((address & 0xf000) == 0x6000 ||
			(address >= 0x8000 && address < 0xc000))
	{
		if(DSP4.out_index<DSP4.out_count)
		{
			DSP4.out_index++;
			return;
		}

		if (DSP4.waiting4command)
		{
			if(DSP4.half_command)
			{
				DSP4.command |= (byte<<8);
				DSP4.in_index = 0;
				DSP4.waiting4command = FALSE;
	//			DSP4.first_parameter = TRUE;
				DSP4.half_command=0;
				DSP4.out_count=0;
				DSP4.out_index=0;
				DSP4_Logic=0;

				switch (DSP4.command)
				{
				case 0x0000:DSP4.in_count=4;break;
				case 0x0001:DSP4.in_count=36;break;
				case 0x0003:DSP4.in_count=0;break;
				case 0x0005:DSP4.in_count=0;break;
				case 0x0006:DSP4.in_count=0;break;
				case 0x0007:DSP4.in_count=22;break;
				case 0x0008:DSP4.in_count=72;break;
				case 0x0009:DSP4.in_count=14;break;
				case 0x000A:DSP4.in_count=6;break;
				case 0x000B:DSP4.in_count=6;break;
				case 0x000D:DSP4.in_count=34;break;
				case 0x000E:DSP4.in_count=0;break;
				case 0x0011:DSP4.in_count=8;break;
				default:
					DSP4.waiting4command=TRUE;
					//printf("(line %d) Unknown Op%02X\n",line,DSP4.command);
					break;
				}
			}
			else
			{
				DSP4.command=byte;
				DSP4.half_command=1;
			}
		}
		else
		{
			DSP4.parameters [DSP4.in_index] = byte;
//			DSP4.first_parameter = FALSE;
			DSP4.in_index++;
		}
		
		if (!DSP4.waiting4command && DSP4.in_count==DSP4.in_index)
		{
			//DSP4.parameters [DSP4.in_index] |= (byte << 8);
			// Actually execute the command
			DSP4.waiting4command = TRUE;
			DSP4.out_index = 0;
			DSP4.in_index=0;
			switch (DSP4.command)
			{
			// 16-bit multiplication
			case 0x0000:
				{
					int16 multiplier, multiplicand;
					int product;
					
					multiplier = DSP4_READ_WORD(0);
					multiplicand = DSP4_READ_WORD(2);

					DSP4_Multiply(multiplicand,multiplier,product);

					DSP4.out_count = 4;
					DSP4_WRITE_WORD(0,product);
					DSP4_WRITE_WORD(2,product>>16);
				}
				break;

			// unknown: horizontal mapping command
			case 0x0011:
				{
					int16 a,b,c,d,m;

					a = DSP4_READ_WORD(6);
					b = DSP4_READ_WORD(4);
					c = DSP4_READ_WORD(2);
					d = DSP4_READ_WORD(0);

					DSP4_UnknownOP11(a,b,c,d,m);

					DSP4.out_count = 2;
					DSP4_WRITE_WORD(0,m);
					break;
				}

			// track projection
			case 0x0001: DSP4_Op01(); break;

			// track projection (pass 2)
			case 0x0007: DSP4_Op07(); break;

			// zone projections (fuel/repair/lap/teleport/...)
			case 0x0008: DSP4_Op08(); break;

			// sprite transformation
			case 0x0009: DSP4_Op09(); break;

			// fast track projection
			case 0x000D: DSP4_Op0D(); break;

			// internal memory management (01)
			case 0x0003:
				{
					// reset op09 data
					op09_mode = 0;
					break;
				}

			// internal memory management (06)
			case 0x0005:
				{
					// clear OAM tables
					op06_index = 0;
					op06_offset = 0;
					for( int lcv=0; lcv<32; lcv++ )
						op06_OAM[lcv] = 0;
					break;
				}

			// internal memory management (0D)
			case 0x000E:
				{
					// reset op09 data
					op09_mode = 1;
					break;
				}			

			// sprite OAM post-table data
			case 0x0006:
				{
					DSP4.out_count = 32;
					for( int lcv=0; lcv<32; lcv++ )
						DSP4.output[lcv] = op06_OAM[lcv];
				}
				break;

			// unknown
			case 0x000A:
				{
					int16 in1a = DSP4_READ_WORD(0);
					int16 in2a = DSP4_READ_WORD(2);
					int16 in3a = DSP4_READ_WORD(4);
					int16 out1a,out2a;

					out1a=(short)0xff40;
					out2a=(short)0x00c0;

					DSP4.out_count=8;

					DSP4_WRITE_WORD(0,out1a);
					DSP4_WRITE_WORD(2,out2a);
					DSP4_WRITE_WORD(4,out1a);
					DSP4_WRITE_WORD(6,out2a);
				}
				break;

			// render player positions around track
			case 0x000B:
				{
					int16 sp_x = DSP4_READ_WORD(0);
					int16 sp_y = DSP4_READ_WORD(2);
					int16 oam = DSP4_READ_WORD(4);

					// Only allow 1p/1p-split to yield output (???)
					if(!op09_mode)
					{
						// yield OAM output
						DSP4.out_count = 6;
						DSP4_WRITE_WORD(0,1);

						// pack OAM data: x,y,name,attr
						DSP4.output[2] = sp_x & 0xff;
						DSP4.output[3] = sp_y & 0xff;
						DSP4_WRITE_WORD(4,oam);

						// OAM: size,msb data
						DSP4_Op06(0,0);
					}
					// 4p mode
					else
					{
						// no OAM available
						DSP4.out_count=0;
						DSP4_WRITE_WORD(0,0);
					}
				}
				break;
			
			default: break;
			}
		}
	}
}

uint8 DSP4GetByte(uint16 address)
{
	uint8 t;
	if ((address & 0xf000) == 0x6000 ||
			(address >= 0x8000 && address < 0xc000))
	{
		if (DSP4.out_count)
		{
			t = (uint8) DSP4.output [DSP4.out_index];
			DSP4.out_index++;
			if(DSP4.out_count==DSP4.out_index)
				DSP4.out_count=0;
		}
		else
			t = 0xff;
	}
	else
	{
		t = 0x80;
	}

	return t;
}

