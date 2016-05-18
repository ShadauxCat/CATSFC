//Copyright (C) 1997-2001 ZSNES Team ( zsknight@zsnes.com / _demo_@zsnes.com )
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later
//version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

//#define __ZSNES__

#if (defined __ZSNES__ && __LINUX__)
#include "../gblhdr.h"
#else

#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#endif
//#define DebugDSP1

// uncomment some lines to test
//#define printinfo
//#define debug06

#define __OPT__
#define __OPT06__

#ifdef DebugDSP1

FILE* LogFile = NULL;

void Log_Message(char* Message, ...)
{
   char Msg[400];
   va_list ap;

   va_start(ap, Message);
   vsprintf(Msg, Message, ap);
   va_end(ap);

   strcat(Msg, "\r\n\0");
   fwrite(Msg, strlen(Msg), 1, LogFile);
   fflush(LogFile);
}

void Start_Log(void)
{
   char LogFileName[255];
   //  [4/15/2001]   char *p;

   strcpy(LogFileName, "dsp1emu.log\0");

   LogFile = fopen(LogFileName, "wb");
}

void Stop_Log(void)
{
   if (LogFile)
   {
      fclose(LogFile);
      LogFile = NULL;
   }
}

#endif

const uint16_t DSP1ROM[1024] =
{
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

/***************************************************************************\
*  Math tables                                                              *
\***************************************************************************/

#define INCR 2048
#define Angle(x) (((x)/(65536/INCR)) & (INCR-1))
#define Cos(x) ((double) CosTable2[x])
#define Sin(x) ((double) SinTable2[x])
#ifdef PI
#undef PI
#endif
#define PI 3.1415926535897932384626433832795
double CosTable2[INCR];
double SinTable2[INCR];


double Atan(double x)
{
   if ((x >= 1) || (x <= 1))
      return (x / (1 + 0.28 * x * x));
   else
      return (PI / 2 - Atan(1 / x));
}

#ifdef __ZSNES__
/***************************************************************************\
*  C4 C code                                                                *
\***************************************************************************/

int16_t C4WFXVal;
int16_t C4WFYVal;
int16_t C4WFZVal;
int16_t C4WFX2Val;
int16_t C4WFY2Val;
int16_t C4WFDist;
int16_t C4WFScale;
double tanval;
double c4x, c4y, c4z;
double c4x2, c4y2, c4z2;

void C4TransfWireFrame()
{
   c4x = (double)C4WFXVal;
   c4y = (double)C4WFYVal;
   c4z = (double)C4WFZVal - 0x95;

   // Rotate X
   tanval = -(double)C4WFX2Val * PI * 2 / 128;
   c4y2 = c4y * cos(tanval) - c4z * sin(tanval);
   c4z2 = c4y * sin(tanval) + c4z * cos(tanval);

   // Rotate Y
   tanval = -(double)C4WFY2Val * PI * 2 / 128;
   c4x2 = c4x * cos(tanval) + c4z2 * sin(tanval);
   c4z = c4x * -sin(tanval) + c4z2 * cos(tanval);

   // Rotate Z
   tanval = -(double)C4WFDist * PI * 2 / 128;
   c4x = c4x2 * cos(tanval) - c4y2 * sin(tanval);
   c4y = c4x2 * sin(tanval) + c4y2 * cos(tanval);

   // Scale
   C4WFXVal = (int16_t)(c4x * C4WFScale / (0x90 * (c4z + 0x95)) * 0x95);
   C4WFYVal = (int16_t)(c4y * C4WFScale / (0x90 * (c4z + 0x95)) * 0x95);
}

void C4TransfWireFrame2()
{
   c4x = (double)C4WFXVal;
   c4y = (double)C4WFYVal;
   c4z = (double)C4WFZVal;

   // Rotate X
   tanval = -(double)C4WFX2Val * PI * 2 / 128;
   c4y2 = c4y * cos(tanval) - c4z * sin(tanval);
   c4z2 = c4y * sin(tanval) + c4z * cos(tanval);

   // Rotate Y
   tanval = -(double)C4WFY2Val * PI * 2 / 128;
   c4x2 = c4x * cos(tanval) + c4z2 * sin(tanval);
   c4z = c4x * -sin(tanval) + c4z2 * cos(tanval);

   // Rotate Z
   tanval = -(double)C4WFDist * PI * 2 / 128;
   c4x = c4x2 * cos(tanval) - c4y2 * sin(tanval);
   c4y = c4x2 * sin(tanval) + c4y2 * cos(tanval);

   // Scale
   C4WFXVal = (int16_t)(c4x * C4WFScale / 0x100);
   C4WFYVal = (int16_t)(c4y * C4WFScale / 0x100);
}

void C4CalcWireFrame()
{
   C4WFXVal = C4WFX2Val - C4WFXVal;
   C4WFYVal = C4WFY2Val - C4WFYVal;
   if (abs(C4WFXVal) > abs(C4WFYVal))
   {
      C4WFDist = abs(C4WFXVal) + 1;
      C4WFYVal = (256 * (long)C4WFYVal) / abs(C4WFXVal);
      if (C4WFXVal < 0) C4WFXVal = -256;
      else C4WFXVal = 256;
   }
   else if (C4WFYVal != 0)
   {
      C4WFDist = abs(C4WFYVal) + 1;
      C4WFXVal = (256 * (long)C4WFXVal) / abs(C4WFYVal);
      if (C4WFYVal < 0) C4WFYVal = -256;
      else C4WFYVal = 256;
   }
   else C4WFDist = 0;
}

int16_t C41FXVal;
int16_t C41FYVal;
int16_t C41FAngleRes;
int16_t C41FDist;
int16_t C41FDistVal;

void C4Op1F()
{
   if (C41FXVal == 0)
   {
      if (C41FYVal > 0) C41FAngleRes = 0x80;
      else C41FAngleRes = 0x180;
   }
   else
   {
      tanval = ((double)C41FYVal) / ((double)C41FXVal);
      C41FAngleRes = (int16_t)(atan(tanval) / (PI * 2) * 512);
      C41FAngleRes = C41FAngleRes;
      if (C41FXVal < 0) C41FAngleRes += 0x100;
      C41FAngleRes &= 0x1FF;
   }
}

void C4Op15()
{
   tanval = sqrt(((double)C41FYVal) * ((double)C41FYVal) + ((double)C41FXVal) *
                 ((double)C41FXVal));
   C41FDist = (int16_t)tanval;
}

void C4Op0D()
{
   tanval = sqrt(((double)C41FYVal) * ((double)C41FYVal) + ((double)C41FXVal) *
                 ((double)C41FXVal));
   tanval = (double)C41FDistVal / tanval;
   C41FYVal = (int16_t)(((double)C41FYVal * tanval) * 0.99);
   C41FXVal = (int16_t)(((double)C41FXVal * tanval) * 0.98);
}
#endif

/***************************************************************************\
*  DSP1 code                                                                *
\***************************************************************************/

void InitDSP(void)
{
#ifdef __OPT__
   unsigned int i;
   for (i = 0; i < INCR; i++)
   {
      CosTable2[i] = (cos((double)(2 * PI * i / INCR)));
      SinTable2[i] = (sin((double)(2 * PI * i / INCR)));
   }
#endif
#ifdef DebugDSP1
   Start_Log();
#endif
}


int16_t Op00Multiplicand;
int16_t Op00Multiplier;
int16_t Op00Result;

void DSPOp00()
{
   Op00Result = Op00Multiplicand * Op00Multiplier >> 15;

#ifdef DebugDSP1
   Log_Message("OP00 MULT %d*%d/32768=%d", Op00Multiplicand, Op00Multiplier,
               Op00Result);
#endif
}

int16_t Op20Multiplicand;
int16_t Op20Multiplier;
int16_t Op20Result;

void DSPOp20()
{
   Op20Result = Op20Multiplicand * Op20Multiplier >> 15;
   Op20Result++;

#ifdef DebugDSP1
   Log_Message("OP20 MULT %d*%d/32768=%d", Op20Multiplicand, Op20Multiplier,
               Op20Result);
#endif
}

int16_t Op10Coefficient;
int16_t Op10Exponent;
int16_t Op10CoefficientR;
int16_t Op10ExponentR;

void DSP1_Inverse(int16_t Coefficient, int16_t Exponent, int16_t* iCoefficient,
                  int16_t* iExponent)
{
   // Step One: Division by Zero
   if (Coefficient == 0x0000)
   {
      *iCoefficient = 0x7fff;
      *iExponent = 0x002f;
   }
   else
   {
      int16_t Sign = 1;

      // Step Two: Remove Sign
      if (Coefficient < 0)
      {
         if (Coefficient < -32767) Coefficient = -32767;
         Coefficient = -Coefficient;
         Sign = -1;
      }

      // Step Three: Normalize
      while (Coefficient < 0x4000)
      {
         Coefficient <<= 1;
         Exponent--;
      }

      // Step Four: Special Case
      if (Coefficient == 0x4000)
         if (Sign == 1) *iCoefficient = 0x7fff;
         else
         {
            *iCoefficient = -0x4000;
            Exponent--;
         }
      else
      {
         // Step Five: Initial Guess
         int16_t i = DSP1ROM[((Coefficient - 0x4000) >> 7) + 0x0065];

         // Step Six: Iterate "estimated" Newton's Method
         i = (i + (-i * (Coefficient * i >> 15) >> 15)) << 1;
         i = (i + (-i * (Coefficient * i >> 15) >> 15)) << 1;

         *iCoefficient = i * Sign;
      }

      *iExponent = 1 - Exponent;
   }
}

void DSPOp10()
{
   DSP1_Inverse(Op10Coefficient, Op10Exponent, &Op10CoefficientR, &Op10ExponentR);
#ifdef DebugDSP1
   Log_Message("OP10 INV %d*2^%d = %d*2^%d", Op10Coefficient, Op10Exponent,
               Op10CoefficientR, Op10ExponentR);
#endif
}

int16_t Op04Angle;
int16_t Op04Radius;
int16_t Op04Sin;
int16_t Op04Cos;

const int16_t DSP1_MulTable[256] =
{
   0x0000,  0x0003,  0x0006,  0x0009,  0x000c,  0x000f,  0x0012,  0x0015,
   0x0019,  0x001c,  0x001f,  0x0022,  0x0025,  0x0028,  0x002b,  0x002f,
   0x0032,  0x0035,  0x0038,  0x003b,  0x003e,  0x0041,  0x0045,  0x0048,
   0x004b,  0x004e,  0x0051,  0x0054,  0x0057,  0x005b,  0x005e,  0x0061,
   0x0064,  0x0067,  0x006a,  0x006d,  0x0071,  0x0074,  0x0077,  0x007a,
   0x007d,  0x0080,  0x0083,  0x0087,  0x008a,  0x008d,  0x0090,  0x0093,
   0x0096,  0x0099,  0x009d,  0x00a0,  0x00a3,  0x00a6,  0x00a9,  0x00ac,
   0x00af,  0x00b3,  0x00b6,  0x00b9,  0x00bc,  0x00bf,  0x00c2,  0x00c5,
   0x00c9,  0x00cc,  0x00cf,  0x00d2,  0x00d5,  0x00d8,  0x00db,  0x00df,
   0x00e2,  0x00e5,  0x00e8,  0x00eb,  0x00ee,  0x00f1,  0x00f5,  0x00f8,
   0x00fb,  0x00fe,  0x0101,  0x0104,  0x0107,  0x010b,  0x010e,  0x0111,
   0x0114,  0x0117,  0x011a,  0x011d,  0x0121,  0x0124,  0x0127,  0x012a,
   0x012d,  0x0130,  0x0133,  0x0137,  0x013a,  0x013d,  0x0140,  0x0143,
   0x0146,  0x0149,  0x014d,  0x0150,  0x0153,  0x0156,  0x0159,  0x015c,
   0x015f,  0x0163,  0x0166,  0x0169,  0x016c,  0x016f,  0x0172,  0x0175,
   0x0178,  0x017c,  0x017f,  0x0182,  0x0185,  0x0188,  0x018b,  0x018e,
   0x0192,  0x0195,  0x0198,  0x019b,  0x019e,  0x01a1,  0x01a4,  0x01a8,
   0x01ab,  0x01ae,  0x01b1,  0x01b4,  0x01b7,  0x01ba,  0x01be,  0x01c1,
   0x01c4,  0x01c7,  0x01ca,  0x01cd,  0x01d0,  0x01d4,  0x01d7,  0x01da,
   0x01dd,  0x01e0,  0x01e3,  0x01e6,  0x01ea,  0x01ed,  0x01f0,  0x01f3,
   0x01f6,  0x01f9,  0x01fc,  0x0200,  0x0203,  0x0206,  0x0209,  0x020c,
   0x020f,  0x0212,  0x0216,  0x0219,  0x021c,  0x021f,  0x0222,  0x0225,
   0x0228,  0x022c,  0x022f,  0x0232,  0x0235,  0x0238,  0x023b,  0x023e,
   0x0242,  0x0245,  0x0248,  0x024b,  0x024e,  0x0251,  0x0254,  0x0258,
   0x025b,  0x025e,  0x0261,  0x0264,  0x0267,  0x026a,  0x026e,  0x0271,
   0x0274,  0x0277,  0x027a,  0x027d,  0x0280,  0x0284,  0x0287,  0x028a,
   0x028d,  0x0290,  0x0293,  0x0296,  0x029a,  0x029d,  0x02a0,  0x02a3,
   0x02a6,  0x02a9,  0x02ac,  0x02b0,  0x02b3,  0x02b6,  0x02b9,  0x02bc,
   0x02bf,  0x02c2,  0x02c6,  0x02c9,  0x02cc,  0x02cf,  0x02d2,  0x02d5,
   0x02d8,  0x02db,  0x02df,  0x02e2,  0x02e5,  0x02e8,  0x02eb,  0x02ee,
   0x02f1,  0x02f5,  0x02f8,  0x02fb,  0x02fe,  0x0301,  0x0304,  0x0307,
   0x030b,  0x030e,  0x0311,  0x0314,  0x0317,  0x031a,  0x031d,  0x0321
};

const int16_t DSP1_SinTable[256] =
{
   0x0000,  0x0324,  0x0647,  0x096a,  0x0c8b,  0x0fab,  0x12c8,  0x15e2,
   0x18f8,  0x1c0b,  0x1f19,  0x2223,  0x2528,  0x2826,  0x2b1f,  0x2e11,
   0x30fb,  0x33de,  0x36ba,  0x398c,  0x3c56,  0x3f17,  0x41ce,  0x447a,
   0x471c,  0x49b4,  0x4c3f,  0x4ebf,  0x5133,  0x539b,  0x55f5,  0x5842,
   0x5a82,  0x5cb4,  0x5ed7,  0x60ec,  0x62f2,  0x64e8,  0x66cf,  0x68a6,
   0x6a6d,  0x6c24,  0x6dca,  0x6f5f,  0x70e2,  0x7255,  0x73b5,  0x7504,
   0x7641,  0x776c,  0x7884,  0x798a,  0x7a7d,  0x7b5d,  0x7c29,  0x7ce3,
   0x7d8a,  0x7e1d,  0x7e9d,  0x7f09,  0x7f62,  0x7fa7,  0x7fd8,  0x7ff6,
   0x7fff,  0x7ff6,  0x7fd8,  0x7fa7,  0x7f62,  0x7f09,  0x7e9d,  0x7e1d,
   0x7d8a,  0x7ce3,  0x7c29,  0x7b5d,  0x7a7d,  0x798a,  0x7884,  0x776c,
   0x7641,  0x7504,  0x73b5,  0x7255,  0x70e2,  0x6f5f,  0x6dca,  0x6c24,
   0x6a6d,  0x68a6,  0x66cf,  0x64e8,  0x62f2,  0x60ec,  0x5ed7,  0x5cb4,
   0x5a82,  0x5842,  0x55f5,  0x539b,  0x5133,  0x4ebf,  0x4c3f,  0x49b4,
   0x471c,  0x447a,  0x41ce,  0x3f17,  0x3c56,  0x398c,  0x36ba,  0x33de,
   0x30fb,  0x2e11,  0x2b1f,  0x2826,  0x2528,  0x2223,  0x1f19,  0x1c0b,
   0x18f8,  0x15e2,  0x12c8,  0x0fab,  0x0c8b,  0x096a,  0x0647,  0x0324,
   -0x0000, -0x0324, -0x0647, -0x096a, -0x0c8b, -0x0fab, -0x12c8, -0x15e2,
   -0x18f8, -0x1c0b, -0x1f19, -0x2223, -0x2528, -0x2826, -0x2b1f, -0x2e11,
   -0x30fb, -0x33de, -0x36ba, -0x398c, -0x3c56, -0x3f17, -0x41ce, -0x447a,
   -0x471c, -0x49b4, -0x4c3f, -0x4ebf, -0x5133, -0x539b, -0x55f5, -0x5842,
   -0x5a82, -0x5cb4, -0x5ed7, -0x60ec, -0x62f2, -0x64e8, -0x66cf, -0x68a6,
   -0x6a6d, -0x6c24, -0x6dca, -0x6f5f, -0x70e2, -0x7255, -0x73b5, -0x7504,
   -0x7641, -0x776c, -0x7884, -0x798a, -0x7a7d, -0x7b5d, -0x7c29, -0x7ce3,
   -0x7d8a, -0x7e1d, -0x7e9d, -0x7f09, -0x7f62, -0x7fa7, -0x7fd8, -0x7ff6,
   -0x7fff, -0x7ff6, -0x7fd8, -0x7fa7, -0x7f62, -0x7f09, -0x7e9d, -0x7e1d,
   -0x7d8a, -0x7ce3, -0x7c29, -0x7b5d, -0x7a7d, -0x798a, -0x7884, -0x776c,
   -0x7641, -0x7504, -0x73b5, -0x7255, -0x70e2, -0x6f5f, -0x6dca, -0x6c24,
   -0x6a6d, -0x68a6, -0x66cf, -0x64e8, -0x62f2, -0x60ec, -0x5ed7, -0x5cb4,
   -0x5a82, -0x5842, -0x55f5, -0x539b, -0x5133, -0x4ebf, -0x4c3f, -0x49b4,
   -0x471c, -0x447a, -0x41ce, -0x3f17, -0x3c56, -0x398c, -0x36ba, -0x33de,
   -0x30fb, -0x2e11, -0x2b1f, -0x2826, -0x2528, -0x2223, -0x1f19, -0x1c0b,
   -0x18f8, -0x15e2, -0x12c8, -0x0fab, -0x0c8b, -0x096a, -0x0647, -0x0324
};

int16_t DSP1_Sin(int16_t Angle)
{
   if (Angle < 0)
   {
      if (Angle == -32768) return 0;
      return -DSP1_Sin(-Angle);
   }
   int S = DSP1_SinTable[Angle >> 8] + (DSP1_MulTable[Angle & 0xff] *
                                        DSP1_SinTable[0x40 + (Angle >> 8)] >> 15);
   if (S > 32767) S = 32767;
   return (int16_t) S;
}

int16_t DSP1_Cos(int16_t Angle)
{
   if (Angle < 0)
   {
      if (Angle == -32768) return -32768;
      Angle = -Angle;
   }
   int S = DSP1_SinTable[0x40 + (Angle >> 8)] - (DSP1_MulTable[Angle & 0xff] *
           DSP1_SinTable[Angle >> 8] >> 15);
   if (S < -32768) S = -32767;
   return (int16_t) S;
}

void DSP1_Normalize(int16_t m, int16_t* Coefficient, int16_t* Exponent)
{
   int16_t i = 0x4000;
   int16_t e = 0;

   if (m < 0)
      while ((m & i) && i)
      {
         i >>= 1;
         e++;
      }
   else
      while (!(m & i) && i)
      {
         i >>= 1;
         e++;
      }

   if (e > 0)
      *Coefficient = m * DSP1ROM[0x21 + e] << 1;
   else
      *Coefficient = m;

   *Exponent -= e;
}

void DSP1_NormalizeDouble(int Product, int16_t* Coefficient, int16_t* Exponent)
{
   int16_t n = Product & 0x7fff;
   int16_t m = Product >> 15;
   int16_t i = 0x4000;
   int16_t e = 0;

   if (m < 0)
      while ((m & i) && i)
      {
         i >>= 1;
         e++;
      }
   else
      while (!(m & i) && i)
      {
         i >>= 1;
         e++;
      }

   if (e > 0)
   {
      *Coefficient = m * DSP1ROM[0x0021 + e] << 1;

      if (e < 15)
         *Coefficient += n * DSP1ROM[0x0040 - e] >> 15;
      else
      {
         i = 0x4000;

         if (m < 0)
            while ((n & i) && i)
            {
               i >>= 1;
               e++;
            }
         else
            while (!(n & i) && i)
            {
               i >>= 1;
               e++;
            }

         if (e > 15)
            *Coefficient = n * DSP1ROM[0x0012 + e] << 1;
         else
            *Coefficient += n;
      }
   }
   else
      *Coefficient = m;

   *Exponent = e;
}

int16_t DSP1_Truncate(int16_t C, int16_t E)
{
   if (E > 0)
   {
      if (C > 0) return 32767;
      else if (C < 0) return -32767;
   }
   else
   {
      if (E < 0) return C * DSP1ROM[0x0031 + E] >> 15;
   }
   return C;
}

void DSPOp04()
{
   Op04Sin = DSP1_Sin(Op04Angle) * Op04Radius >> 15;
   Op04Cos = DSP1_Cos(Op04Angle) * Op04Radius >> 15;
}

int16_t Op0CA;
int16_t Op0CX1;
int16_t Op0CY1;
int16_t Op0CX2;
int16_t Op0CY2;

void DSPOp0C()
{
   Op0CX2 = (Op0CY1 * DSP1_Sin(Op0CA) >> 15) + (Op0CX1 * DSP1_Cos(Op0CA) >> 15);
   Op0CY2 = (Op0CY1 * DSP1_Cos(Op0CA) >> 15) - (Op0CX1 * DSP1_Sin(Op0CA) >> 15);
}

int16_t CentreX;
int16_t CentreY;
int16_t VOffset;

int16_t VPlane_C;
int16_t VPlane_E;

// Azimuth and Zenith angles
int16_t SinAas;
int16_t CosAas;
int16_t SinAzs;
int16_t CosAzs;

// Clipped Zenith angle
int16_t SinAZS;
int16_t CosAZS;
int16_t SecAZS_C1;
int16_t SecAZS_E1;
int16_t SecAZS_C2;
int16_t SecAZS_E2;

const int16_t MaxAZS_Exp[16] =
{
   0x38b4, 0x38b7, 0x38ba, 0x38be, 0x38c0, 0x38c4, 0x38c7, 0x38ca,
   0x38ce,  0x38d0, 0x38d4, 0x38d7, 0x38da, 0x38dd, 0x38e0, 0x38e4
};

void DSP1_Parameter(int16_t Fx, int16_t Fy, int16_t Fz, int16_t Lfe, int16_t Les,
                    int16_t Aas, int16_t Azs, int16_t* Vof, int16_t* Vva, int16_t* Cx, int16_t* Cy)
{
   int16_t CSec, C, E;

   // Copy Zenith angle for clipping
   int16_t AZS = Azs;

   // Store Sin and Cos of Azimuth and Zenith angles
   SinAas = DSP1_Sin(Aas);
   CosAas = DSP1_Cos(Aas);
   SinAzs = DSP1_Sin(Azs);
   CosAzs = DSP1_Cos(Azs);

   // Center of Projection
   CentreX = Fx + (Lfe * (SinAzs * -SinAas >> 15) >> 15);
   CentreY = Fy + (Lfe * (SinAzs * CosAas >> 15) >> 15);

   E = 0;
   DSP1_Normalize(Fz + (Lfe * (CosAzs * 0x7fff >> 15) >> 15), &C, &E);

   VPlane_C = C;
   VPlane_E = E;

   // Determine clip boundary and clip Zenith angle if necessary
   int16_t MaxAZS = MaxAZS_Exp[-E];

   if (AZS < 0)
   {
      MaxAZS = -MaxAZS;
      if (AZS < MaxAZS + 1) AZS = MaxAZS + 1;
   }
   else
   {
      if (AZS > MaxAZS) AZS = MaxAZS;
   }

   // Store Sin and Cos of clipped Zenith angle
   SinAZS = DSP1_Sin(AZS);
   CosAZS = DSP1_Cos(AZS);

   DSP1_Inverse(CosAZS, 0, &SecAZS_C1, &SecAZS_E1);
   DSP1_Normalize(C * SecAZS_C1 >> 15, &C, &E);
   E += SecAZS_E1;

   C = DSP1_Truncate(C, E) * SinAZS >> 15;

   CentreX += C * SinAas >> 15;
   CentreY -= C * CosAas >> 15;

   *Cx = CentreX;
   *Cy = CentreY;

   // Raster number of imaginary center and horizontal line
   *Vof = 0;

   if ((Azs != AZS) || (Azs == MaxAZS))
   {
      if (Azs == -32768) Azs = -32767;

      C = Azs - MaxAZS;
      if (C >= 0) C--;
      int16_t Aux = ~(C << 2);

      C = Aux * DSP1ROM[0x0328] >> 15;
      C = (C * Aux >> 15) + DSP1ROM[0x0327];
      *Vof -= (C * Aux >> 15) * Les >> 15;

      C = Aux * Aux >> 15;
      Aux = (C * DSP1ROM[0x0324] >> 15) + DSP1ROM[0x0325];
      CosAZS += (C * Aux >> 15) * CosAZS >> 15;
   }

   VOffset = Les * CosAZS >> 15;

   DSP1_Inverse(SinAZS, 0, &CSec, &E);
   DSP1_Normalize(VOffset, &C, &E);
   DSP1_Normalize(C * CSec >> 15, &C, &E);

   if (C == -32768)
   {
      C >>= 1;
      E++;
   }

   *Vva = DSP1_Truncate(-C, E);

   // Store Sec of clipped Zenith angle
   DSP1_Inverse(CosAZS, 0, &SecAZS_C2, &SecAZS_E2);
}

void DSP1_Raster(int16_t Vs, int16_t* An, int16_t* Bn, int16_t* Cn, int16_t* Dn)
{
   int16_t C, E, C1, E1;

   DSP1_Inverse((Vs * SinAzs >> 15) + VOffset, 7, &C, &E);
   E += VPlane_E;

   C1 = C * VPlane_C >> 15;
   E1 = E + SecAZS_E2;

   DSP1_Normalize(C1, &C, &E);

   C = DSP1_Truncate(C, E);

   *An = C * CosAas >> 15;
   *Cn = C * SinAas >> 15;

   DSP1_Normalize(C1 * SecAZS_C2 >> 15, &C, &E1);

   C = DSP1_Truncate(C, E1);

   *Bn = C * -SinAas >> 15;
   *Dn = C * CosAas >> 15;
}

int16_t Op02FX;
int16_t Op02FY;
int16_t Op02FZ;
int16_t Op02LFE;
int16_t Op02LES;
int16_t Op02AAS;
int16_t Op02AZS;
int16_t Op02VOF;
int16_t Op02VVA;
int16_t Op02CX;
int16_t Op02CY;

void DSPOp02()
{
   DSP1_Parameter(Op02FX, Op02FY, Op02FZ, Op02LFE, Op02LES, Op02AAS, Op02AZS,
                  &Op02VOF, &Op02VVA, &Op02CX, &Op02CY);
}

int16_t Op0AVS;
int16_t Op0AA;
int16_t Op0AB;
int16_t Op0AC;
int16_t Op0AD;

void DSPOp0A()
{
   DSP1_Raster(Op0AVS, &Op0AA, &Op0AB, &Op0AC, &Op0AD);
   Op0AVS++;
}

int16_t Op06X;
int16_t Op06Y;
int16_t Op06Z;
int16_t Op06H;
int16_t Op06V;
uint16_t Op06S;

double ObjPX;
double ObjPY;
double ObjPZ;
double ObjPX1;
double ObjPY1;
double ObjPZ1;
double ObjPX2;
double ObjPY2;
double ObjPZ2;
double DivideOp06;
int Temp;
int tanval2;

#ifdef __OPT06__
void DSPOp06()
{
   ObjPX = Op06X - Op02FX;
   ObjPY = Op06Y - Op02FY;
   ObjPZ = Op06Z - Op02FZ;

   // rotate around Z
   tanval2 = Angle(-Op02AAS + 32768);
   //   tanval2 = (-Op02AAS+32768)/(65536/INCR);
   ObjPX1 = (ObjPX * Cos(tanval2) + ObjPY * -Sin(tanval2));
   ObjPY1 = (ObjPX * Sin(tanval2) + ObjPY * Cos(tanval2));
   ObjPZ1 = ObjPZ;

   // rotate around X
   //   tanval2 = (-Op02AZS/(65536/INCR)) & 1023;
   tanval2 = Angle(-Op02AZS);
   //   tanval2 = (-Op02AZS)/256;
   ObjPX2 = ObjPX1;
   ObjPY2 = (ObjPY1 * Cos(tanval2) + ObjPZ1 * -Sin(tanval2));
   ObjPZ2 = (ObjPY1 * Sin(tanval2) + ObjPZ1 * Cos(tanval2));

#ifdef debug06
   Log_Message("ObjPX2: %f ObjPY2: %f ObjPZ2: %f\n", ObjPX2, ObjPY2, ObjPZ2);
#endif

   ObjPZ2 = ObjPZ2 - Op02LFE;

   if (ObjPZ2 < 0)
   {
      double d;
      Op06H = (int16_t)(-ObjPX2 * Op02LES / -(ObjPZ2)); //-ObjPX2*256/-ObjPZ2;
      Op06V = (int16_t)(-ObjPY2 * Op02LES / -(ObjPZ2)); //-ObjPY2*256/-ObjPZ2;
      d = (double)Op02LES;
      d *= 256.0;
      d /= (-ObjPZ2);
      if (d > 65535.0)
         d = 65535.0;
      else if (d < 0.0)
         d = 0.0;
      Op06S = (uint16_t)d;
      //Op06S=(uint16_t)(256*(double)Op02LES/-ObjPZ2);
      //Op06S=(uint16_t)((double)(256.0*((double)Op02LES)/(-ObjPZ2)));
   }
   else
   {
      Op06H = 0;
      Op06V = 14 * 16;
      Op06S = 0xFFFF;
   }


#ifdef DebugDSP1
   Log_Message("OP06 X:%d Y:%d Z:%d", Op06X, Op06Y, Op06Z);
   Log_Message("OP06 H:%d V:%d S:%d", Op06H, Op06V, Op06S);
#endif
}
#else

void DSPOp06()
{
   ObjPX = Op06X - Op02FX;
   ObjPY = Op06Y - Op02FY;
   ObjPZ = Op06Z - Op02FZ;

   // rotate around Z
   tanval = (-Op02AAS + 32768) / 65536.0 * 6.2832;
   ObjPX1 = (ObjPX * cos(tanval) + ObjPY * -sin(tanval));
   ObjPY1 = (ObjPX * sin(tanval) + ObjPY * cos(tanval));
   ObjPZ1 = ObjPZ;

#ifdef debug06
   Log_Message("Angle : %f", tanval);
   Log_Message("ObjPX1: %f ObjPY1: %f ObjPZ1: %f\n", ObjPX1, ObjPY1, ObjPZ1);
   Log_Message("cos(tanval) : %f  sin(tanval) : %f", cos(tanval), sin(tanval));
#endif

   // rotate around X
   tanval = (-Op02AZS) / 65536.0 * 6.2832;
   ObjPX2 = ObjPX1;
   ObjPY2 = (ObjPY1 * cos(tanval) + ObjPZ1 * -sin(tanval));
   ObjPZ2 = (ObjPY1 * sin(tanval) + ObjPZ1 * cos(tanval));

#ifdef debug06
   Log_Message("ObjPX2: %f ObjPY2: %f ObjPZ2: %f\n", ObjPX2, ObjPY2, ObjPZ2);
#endif

   ObjPZ2 = ObjPZ2 - Op02LFE;

   if (ObjPZ2 < 0)
   {
      Op06H = (int16_t)(-ObjPX2 * Op02LES / -(ObjPZ2)); //-ObjPX2*256/-ObjPZ2;
      Op06V = (int16_t)(-ObjPY2 * Op02LES / -(ObjPZ2)); //-ObjPY2*256/-ObjPZ2;
      double d = (double)Op02LES;
      d *= 256.0;
      d /= (-ObjPZ2);
      if (d > 65535.0)
         d = 65535.0;
      else if (d < 0.0)
         d = 0.0;
      Op06S = (uint16_t)d;
      //   Op06S=(uint16_t)(256*(double)Op02LES/-ObjPZ2);
   }
   else
   {
      Op06H = 0;
      Op06V = 14 * 16;
      Op06S = 0xFFFF;
   }

#ifdef DebugDSP1
   Log_Message("OP06 X:%d Y:%d Z:%d", Op06X, Op06Y, Op06Z);
   Log_Message("OP06 H:%d V:%d S:%d", Op06H, Op06V, Op06S);
#endif
}
#endif


int16_t matrixC[3][3];
int16_t matrixB[3][3];
int16_t matrixA[3][3];

int16_t Op01m;
int16_t Op01Zr;
int16_t Op01Xr;
int16_t Op01Yr;
int16_t Op11m;
int16_t Op11Zr;
int16_t Op11Xr;
int16_t Op11Yr;
int16_t Op21m;
int16_t Op21Zr;
int16_t Op21Xr;
int16_t Op21Yr;

void DSPOp01()
{
   int16_t SinAz = DSP1_Sin(Op01Zr);
   int16_t CosAz = DSP1_Cos(Op01Zr);
   int16_t SinAy = DSP1_Sin(Op01Yr);
   int16_t CosAy = DSP1_Cos(Op01Yr);
   int16_t SinAx = DSP1_Sin(Op01Xr);
   int16_t CosAx = DSP1_Cos(Op01Xr);

   Op01m >>= 1;

   matrixA[0][0] = (Op01m * CosAz >> 15) * CosAy >> 15;
   matrixA[0][1] = -((Op01m * SinAz >> 15) * CosAy >> 15);
   matrixA[0][2] = Op01m * SinAy >> 15;

   matrixA[1][0] = ((Op01m * SinAz >> 15) * CosAx >> 15) + (((
                      Op01m * CosAz >> 15) * SinAx >> 15) * SinAy >> 15);
   matrixA[1][1] = ((Op01m * CosAz >> 15) * CosAx >> 15) - (((
                      Op01m * SinAz >> 15) * SinAx >> 15) * SinAy >> 15);
   matrixA[1][2] = -((Op01m * SinAx >> 15) * CosAy >> 15);

   matrixA[2][0] = ((Op01m * SinAz >> 15) * SinAx >> 15) - (((
                      Op01m * CosAz >> 15) * CosAx >> 15) * SinAy >> 15);
   matrixA[2][1] = ((Op01m * CosAz >> 15) * SinAx >> 15) + (((
                      Op01m * SinAz >> 15) * CosAx >> 15) * SinAy >> 15);
   matrixA[2][2] = (Op01m * CosAx >> 15) * CosAy >> 15;
}

void DSPOp11()
{
   int16_t SinAz = DSP1_Sin(Op11Zr);
   int16_t CosAz = DSP1_Cos(Op11Zr);
   int16_t SinAy = DSP1_Sin(Op11Yr);
   int16_t CosAy = DSP1_Cos(Op11Yr);
   int16_t SinAx = DSP1_Sin(Op11Xr);
   int16_t CosAx = DSP1_Cos(Op11Xr);

   Op11m >>= 1;

   matrixB[0][0] = (Op11m * CosAz >> 15) * CosAy >> 15;
   matrixB[0][1] = -((Op11m * SinAz >> 15) * CosAy >> 15);
   matrixB[0][2] = Op11m * SinAy >> 15;

   matrixB[1][0] = ((Op11m * SinAz >> 15) * CosAx >> 15) + (((
                      Op11m * CosAz >> 15) * SinAx >> 15) * SinAy >> 15);
   matrixB[1][1] = ((Op11m * CosAz >> 15) * CosAx >> 15) - (((
                      Op11m * SinAz >> 15) * SinAx >> 15) * SinAy >> 15);
   matrixB[1][2] = -((Op11m * SinAx >> 15) * CosAy >> 15);

   matrixB[2][0] = ((Op11m * SinAz >> 15) * SinAx >> 15) - (((
                      Op11m * CosAz >> 15) * CosAx >> 15) * SinAy >> 15);
   matrixB[2][1] = ((Op11m * CosAz >> 15) * SinAx >> 15) + (((
                      Op11m * SinAz >> 15) * CosAx >> 15) * SinAy >> 15);
   matrixB[2][2] = (Op11m * CosAx >> 15) * CosAy >> 15;
}

void DSPOp21()
{
   int16_t SinAz = DSP1_Sin(Op21Zr);
   int16_t CosAz = DSP1_Cos(Op21Zr);
   int16_t SinAy = DSP1_Sin(Op21Yr);
   int16_t CosAy = DSP1_Cos(Op21Yr);
   int16_t SinAx = DSP1_Sin(Op21Xr);
   int16_t CosAx = DSP1_Cos(Op21Xr);

   Op21m >>= 1;

   matrixC[0][0] = (Op21m * CosAz >> 15) * CosAy >> 15;
   matrixC[0][1] = -((Op21m * SinAz >> 15) * CosAy >> 15);
   matrixC[0][2] = Op21m * SinAy >> 15;

   matrixC[1][0] = ((Op21m * SinAz >> 15) * CosAx >> 15) + (((
                      Op21m * CosAz >> 15) * SinAx >> 15) * SinAy >> 15);
   matrixC[1][1] = ((Op21m * CosAz >> 15) * CosAx >> 15) - (((
                      Op21m * SinAz >> 15) * SinAx >> 15) * SinAy >> 15);
   matrixC[1][2] = -((Op21m * SinAx >> 15) * CosAy >> 15);

   matrixC[2][0] = ((Op21m * SinAz >> 15) * SinAx >> 15) - (((
                      Op21m * CosAz >> 15) * CosAx >> 15) * SinAy >> 15);
   matrixC[2][1] = ((Op21m * CosAz >> 15) * SinAx >> 15) + (((
                      Op21m * SinAz >> 15) * CosAx >> 15) * SinAy >> 15);
   matrixC[2][2] = (Op21m * CosAx >> 15) * CosAy >> 15;
}

int16_t Op0DX;
int16_t Op0DY;
int16_t Op0DZ;
int16_t Op0DF;
int16_t Op0DL;
int16_t Op0DU;
int16_t Op1DX;
int16_t Op1DY;
int16_t Op1DZ;
int16_t Op1DF;
int16_t Op1DL;
int16_t Op1DU;
int16_t Op2DX;
int16_t Op2DY;
int16_t Op2DZ;
int16_t Op2DF;
int16_t Op2DL;
int16_t Op2DU;

void DSPOp0D()
{
   Op0DF = (Op0DX * matrixA[0][0] >> 15) + (Op0DY * matrixA[0][1] >> 15) +
           (Op0DZ * matrixA[0][2] >> 15);
   Op0DL = (Op0DX * matrixA[1][0] >> 15) + (Op0DY * matrixA[1][1] >> 15) +
           (Op0DZ * matrixA[1][2] >> 15);
   Op0DU = (Op0DX * matrixA[2][0] >> 15) + (Op0DY * matrixA[2][1] >> 15) +
           (Op0DZ * matrixA[2][2] >> 15);

#ifdef DebugDSP1
   Log_Message("OP0D X: %d Y: %d Z: %d / F: %d L: %d U: %d", Op0DX, Op0DY, Op0DZ,
               Op0DF, Op0DL, Op0DU);
#endif
}

void DSPOp1D()
{
   Op1DF = (Op1DX * matrixB[0][0] >> 15) + (Op1DY * matrixB[0][1] >> 15) +
           (Op1DZ * matrixB[0][2] >> 15);
   Op1DL = (Op1DX * matrixB[1][0] >> 15) + (Op1DY * matrixB[1][1] >> 15) +
           (Op1DZ * matrixB[1][2] >> 15);
   Op1DU = (Op1DX * matrixB[2][0] >> 15) + (Op1DY * matrixB[2][1] >> 15) +
           (Op1DZ * matrixB[2][2] >> 15);

#ifdef DebugDSP1
   Log_Message("OP1D X: %d Y: %d Z: %d / F: %d L: %d U: %d", Op1DX, Op1DY, Op1DZ,
               Op1DF, Op1DL, Op1DU);
#endif
}

void DSPOp2D()
{
   Op2DF = (Op2DX * matrixC[0][0] >> 15) + (Op2DY * matrixC[0][1] >> 15) +
           (Op2DZ * matrixC[0][2] >> 15);
   Op2DL = (Op2DX * matrixC[1][0] >> 15) + (Op2DY * matrixC[1][1] >> 15) +
           (Op2DZ * matrixC[1][2] >> 15);
   Op2DU = (Op2DX * matrixC[2][0] >> 15) + (Op2DY * matrixC[2][1] >> 15) +
           (Op2DZ * matrixC[2][2] >> 15);

#ifdef DebugDSP1
   Log_Message("OP2D X: %d Y: %d Z: %d / F: %d L: %d U: %d", Op2DX, Op2DY, Op2DZ,
               Op2DF, Op2DL, Op2DU);
#endif
}

int16_t Op03F;
int16_t Op03L;
int16_t Op03U;
int16_t Op03X;
int16_t Op03Y;
int16_t Op03Z;
int16_t Op13F;
int16_t Op13L;
int16_t Op13U;
int16_t Op13X;
int16_t Op13Y;
int16_t Op13Z;
int16_t Op23F;
int16_t Op23L;
int16_t Op23U;
int16_t Op23X;
int16_t Op23Y;
int16_t Op23Z;

void DSPOp03()
{
   Op03X = (Op03F * matrixA[0][0] >> 15) + (Op03L * matrixA[1][0] >> 15) +
           (Op03U * matrixA[2][0] >> 15);
   Op03Y = (Op03F * matrixA[0][1] >> 15) + (Op03L * matrixA[1][1] >> 15) +
           (Op03U * matrixA[2][1] >> 15);
   Op03Z = (Op03F * matrixA[0][2] >> 15) + (Op03L * matrixA[1][2] >> 15) +
           (Op03U * matrixA[2][2] >> 15);

#ifdef DebugDSP1
   Log_Message("OP03 F: %d L: %d U: %d / X: %d Y: %d Z: %d", Op03F, Op03L, Op03U,
               Op03X, Op03Y, Op03Z);
#endif
}

void DSPOp13()
{
   Op13X = (Op13F * matrixB[0][0] >> 15) + (Op13L * matrixB[1][0] >> 15) +
           (Op13U * matrixB[2][0] >> 15);
   Op13Y = (Op13F * matrixB[0][1] >> 15) + (Op13L * matrixB[1][1] >> 15) +
           (Op13U * matrixB[2][1] >> 15);
   Op13Z = (Op13F * matrixB[0][2] >> 15) + (Op13L * matrixB[1][2] >> 15) +
           (Op13U * matrixB[2][2] >> 15);

#ifdef DebugDSP1
   Log_Message("OP13 F: %d L: %d U: %d / X: %d Y: %d Z: %d", Op13F, Op13L, Op13U,
               Op13X, Op13Y, Op13Z);
#endif
}

void DSPOp23()
{
   Op23X = (Op23F * matrixC[0][0] >> 15) + (Op23L * matrixC[1][0] >> 15) +
           (Op23U * matrixC[2][0] >> 15);
   Op23Y = (Op23F * matrixC[0][1] >> 15) + (Op23L * matrixC[1][1] >> 15) +
           (Op23U * matrixC[2][1] >> 15);
   Op23Z = (Op23F * matrixC[0][2] >> 15) + (Op23L * matrixC[1][2] >> 15) +
           (Op23U * matrixC[2][2] >> 15);

#ifdef DebugDSP1
   Log_Message("OP23 F: %d L: %d U: %d / X: %d Y: %d Z: %d", Op23F, Op23L, Op23U,
               Op23X, Op23Y, Op23Z);
#endif
}

int16_t Op14Zr;
int16_t Op14Xr;
int16_t Op14Yr;
int16_t Op14U;
int16_t Op14F;
int16_t Op14L;
int16_t Op14Zrr;
int16_t Op14Xrr;
int16_t Op14Yrr;

void DSPOp14()
{
   int16_t CSec, ESec, CTan, CSin, C, E;

   DSP1_Inverse(DSP1_Cos(Op14Xr), 0, &CSec, &ESec);

   // Rotation Around Z
   DSP1_NormalizeDouble(Op14U * DSP1_Cos(Op14Yr) - Op14F * DSP1_Sin(Op14Yr), &C,
                        &E);

   E = ESec - E;

   DSP1_Normalize(C * CSec >> 15, &C, &E);

   Op14Zrr = Op14Zr + DSP1_Truncate(C, E);

   // Rotation Around X
   Op14Xrr = Op14Xr + (Op14U * DSP1_Sin(Op14Yr) >> 15) + (Op14F * DSP1_Cos(
                Op14Yr) >> 15);

   // Rotation Around Y
   DSP1_NormalizeDouble(Op14U * DSP1_Cos(Op14Yr) + Op14F * DSP1_Sin(Op14Yr), &C,
                        &E);

   E = ESec - E;

   DSP1_Normalize(DSP1_Sin(Op14Xr), &CSin, &E);

   CTan = CSec * CSin >> 15;

   DSP1_Normalize(-(C * CTan >> 15), &C, &E);

   Op14Yrr = Op14Yr + DSP1_Truncate(C, E) + Op14L;
}

void DSP1_Target(int16_t H, int16_t V, int16_t* X, int16_t* Y)
{
   int16_t C, E, C1, E1;

   DSP1_Inverse((V * SinAzs >> 15) + VOffset, 8, &C, &E);
   E += VPlane_E;

   C1 = C * VPlane_C >> 15;
   E1 = E + SecAZS_E1;

   H <<= 8;

   DSP1_Normalize(C1, &C, &E);

   C = DSP1_Truncate(C, E) * H >> 15;

   *X = CentreX + (C * CosAas >> 15);
   *Y = CentreY - (C * SinAas >> 15);

   V <<= 8;

   DSP1_Normalize(C1 * SecAZS_C1 >> 15, &C, &E1);

   C = DSP1_Truncate(C, E1) * V >> 15;

   *X += C * -SinAas >> 15;
   *Y += C * CosAas >> 15;
}

int16_t Op0EH;
int16_t Op0EV;
int16_t Op0EX;
int16_t Op0EY;

void DSPOp0E()
{
   DSP1_Target(Op0EH, Op0EV, &Op0EX, &Op0EY);
}

int16_t Op0BX;
int16_t Op0BY;
int16_t Op0BZ;
int16_t Op0BS;
int16_t Op1BX;
int16_t Op1BY;
int16_t Op1BZ;
int16_t Op1BS;
int16_t Op2BX;
int16_t Op2BY;
int16_t Op2BZ;
int16_t Op2BS;

void DSPOp0B()
{
   Op0BS = (Op0BX * matrixA[0][0] + Op0BY * matrixA[0][1] + Op0BZ * matrixA[0][2])
           >> 15;

#ifdef DebugDSP1
   Log_Message("OP0B");
#endif
}

void DSPOp1B()
{
   Op1BS = (Op1BX * matrixB[0][0] + Op1BY * matrixB[0][1] + Op1BZ * matrixB[0][2])
           >> 15;

#ifdef DebugDSP1
   Log_Message("OP1B X: %d Y: %d Z: %d S: %d", Op1BX, Op1BY, Op1BZ, Op1BS);
   Log_Message("     MX: %d MY: %d MZ: %d Scale: %d", (int16_t)(matrixB[0][0] * 100),
               (int16_t)(matrixB[0][1] * 100), (int16_t)(matrixB[0][2] * 100), (int16_t)(sc2 * 100));
#endif
}

void DSPOp2B()
{
   Op2BS = (Op2BX * matrixC[0][0] + Op2BY * matrixC[0][1] + Op2BZ * matrixC[0][2])
           >> 15;

#ifdef DebugDSP1
   Log_Message("OP2B");
#endif
}

int16_t Op08X, Op08Y, Op08Z, Op08Ll, Op08Lh;

void DSPOp08()
{
   int Op08Size = (Op08X * Op08X + Op08Y * Op08Y + Op08Z * Op08Z) << 1;
   Op08Ll = Op08Size & 0xffff;
   Op08Lh = (Op08Size >> 16) & 0xffff;

#ifdef DebugDSP1
   Log_Message("OP08 %d,%d,%d", Op08X, Op08Y, Op08Z);
   Log_Message("OP08 ((Op08X^2)+(Op08Y^2)+(Op08X^2))=%x", Op08Size);
#endif
}

int16_t Op18X, Op18Y, Op18Z, Op18R, Op18D;

void DSPOp18()
{
   Op18D = (Op18X * Op18X + Op18Y * Op18Y + Op18Z * Op18Z - Op18R * Op18R) >> 15;

#ifdef DebugDSP1
   Log_Message("Op18 X: %d Y: %d Z: %d R: %D DIFF %d", Op18X, Op18Y, Op38Z, Op18D);
#endif
}

int16_t Op38X, Op38Y, Op38Z, Op38R, Op38D;

void DSPOp38()
{
   Op38D = (Op38X * Op38X + Op38Y * Op38Y + Op38Z * Op38Z - Op38R * Op38R) >> 15;
   Op38D++;

#ifdef DebugDSP1
   Log_Message("OP38 X: %d Y: %d Z: %d R: %D DIFF %d", Op38X, Op38Y, Op38Z, Op38D);
#endif
}

int16_t Op28X;
int16_t Op28Y;
int16_t Op28Z;
int16_t Op28R;

void DSPOp28()
{
   int Radius = Op28X * Op28X + Op28Y * Op28Y + Op28Z * Op28Z;

   if (Radius == 0) Op28R = 0;
   else
   {
      int16_t C, E;
      DSP1_NormalizeDouble(Radius, &C, &E);
      if (E & 1) C = C * 0x4000 >> 15;

      int16_t Pos = C * 0x0040 >> 15;

      int16_t Node1 = DSP1ROM[0x00d5 + Pos];
      int16_t Node2 = DSP1ROM[0x00d6 + Pos];

      Op28R = ((Node2 - Node1) * (C & 0x1ff) >> 9) + Node1;
      Op28R >>= (E >> 1);
   }

#ifdef DebugDSP1
   Log_Message("OP28 X:%d Y:%d Z:%d", Op28X, Op28Y, Op28Z);
   Log_Message("OP28 Vector Length %d", Op28R);
#endif
}

int16_t Op1CX, Op1CY, Op1CZ;
int16_t Op1CXBR, Op1CYBR, Op1CZBR, Op1CXAR, Op1CYAR, Op1CZAR;
int16_t Op1CX1;
int16_t Op1CY1;
int16_t Op1CZ1;
int16_t Op1CX2;
int16_t Op1CY2;
int16_t Op1CZ2;

void DSPOp1C()
{
   // Rotate Around Op1CZ1
   Op1CX1 = (Op1CYBR * DSP1_Sin(Op1CZ) >> 15) + (Op1CXBR * DSP1_Cos(Op1CZ) >> 15);
   Op1CY1 = (Op1CYBR * DSP1_Cos(Op1CZ) >> 15) - (Op1CXBR * DSP1_Sin(Op1CZ) >> 15);
   Op1CXBR = Op1CX1;
   Op1CYBR = Op1CY1;

   // Rotate Around Op1CY1
   Op1CZ1 = (Op1CXBR * DSP1_Sin(Op1CY) >> 15) + (Op1CZBR * DSP1_Cos(Op1CY) >> 15);
   Op1CX1 = (Op1CXBR * DSP1_Cos(Op1CY) >> 15) - (Op1CZBR * DSP1_Sin(Op1CY) >> 15);
   Op1CXAR = Op1CX1;
   Op1CZBR = Op1CZ1;

   // Rotate Around Op1CX1
   Op1CY1 = (Op1CZBR * DSP1_Sin(Op1CX) >> 15) + (Op1CYBR * DSP1_Cos(Op1CX) >> 15);
   Op1CZ1 = (Op1CZBR * DSP1_Cos(Op1CX) >> 15) - (Op1CYBR * DSP1_Sin(Op1CX) >> 15);
   Op1CYAR = Op1CY1;
   Op1CZAR = Op1CZ1;

#ifdef DebugDSP1
   Log_Message("OP1C Apply Matrix CX:%d CY:%d CZ", Op1CXAR, Op1CYAR, Op1CZAR);
#endif
}

uint16_t Op0FRamsize;
uint16_t Op0FPass;

void DSPOp0F()
{
   Op0FPass = 0x0000;

#ifdef DebugDSP1
   Log_Message("OP0F RAM Test Pass:%d", Op0FPass);
#endif
}

int16_t Op2FUnknown;
int16_t Op2FSize;

void DSPOp2F()
{
   Op2FSize = 0x100;
}
