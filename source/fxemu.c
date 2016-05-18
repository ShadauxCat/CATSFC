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
#include "fxemu.h"
#include "fxinst.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* The FxChip Emulator's internal variables */
struct FxRegs_s GSU = FxRegs_s_null;

uint32_t(**fx_ppfFunctionTable)(uint32_t) = 0;
void (**fx_ppfPlotTable)() = 0;
void (**fx_ppfOpcodeTable)() = 0;

#if 0
void fx_setCache()
{
   uint32_t c;
   GSU.bCacheActive = true;
   GSU.pvRegisters[0x3e] &= 0xf0;
   c = (uint32_t)GSU.pvRegisters[0x3e];
   c |= ((uint32_t)GSU.pvRegisters[0x3f]) << 8;
   if (c == GSU.vCacheBaseReg)
      return;
   GSU.vCacheBaseReg = c;
   GSU.vCacheFlags = 0;
   if (c < (0x10000 - 512))
   {
      uint8_t const* t = &ROM(c);
      memcpy(GSU.pvCache, t, 512);
   }
   else
   {
      uint8_t const* t1;
      uint8_t const* t2;
      uint32_t i = 0x10000 - c;
      t1 = &ROM(c);
      t2 = &ROM(0);
      memcpy(GSU.pvCache, t1, i);
      memcpy(&GSU.pvCache[i], t2, 512 - i);
   }
}
#endif

void FxCacheWriteAccess(uint16_t vAddress)
{
#if 0
   if (!GSU.bCacheActive)
   {
      uint8_t v = GSU.pvCache[GSU.pvCache[vAddress & 0x1ff];
                            fx_setCache();
                            GSU.pvCache[GSU.pvCache[vAddress & 0x1ff] = v;
   }
#endif
                         if ((vAddress & 0x00f) == 0x00f)
                            GSU.vCacheFlags |= 1 << ((vAddress & 0x1f0) >> 4);
}

          void FxFlushCache()
{
   GSU.vCacheFlags = 0;
   GSU.vCacheBaseReg = 0;
   GSU.bCacheActive = false;
   //    GSU.vPipe = 0x1;
}

static void fx_backupCache()
{
#if 0
   uint32_t i;
   uint32_t v = GSU.vCacheFlags;
   uint32_t c = USEX16(GSU.vCacheBaseReg);
   if (v)
      for (i = 0; i < 32; i++)
      {
         if (v & 1)
         {
            if (c < (0x10000 - 16))
            {
               uint8_t* t = &GSU.pvPrgBank[c];
               memcpy(&GSU.avCacheBackup[i << 4], t, 16);
               memcpy(t, &GSU.pvCache[i << 4], 16);
            }
            else
            {
               uint8_t* t1;
               uint8_t* t2;
               uint32_t a = 0x10000 - c;
               t1 = &GSU.pvPrgBank[c];
               t2 = &GSU.pvPrgBank[0];
               memcpy(&GSU.avCacheBackup[i << 4], t1, a);
               memcpy(t1, &GSU.pvCache[i << 4], a);
               memcpy(&GSU.avCacheBackup[(i << 4) + a], t2, 16 - a);
               memcpy(t2, &GSU.pvCache[(i << 4) + a], 16 - a);
            }
         }
         c = USEX16(c + 16);
         v >>= 1;
      }
#endif
}

static void fx_restoreCache()
{
#if 0
   uint32_t i;
   uint32_t v = GSU.vCacheFlags;
   uint32_t c = USEX16(GSU.vCacheBaseReg);
   if (v)
      for (i = 0; i < 32; i++)
      {
         if (v & 1)
         {
            if (c < (0x10000 - 16))
            {
               uint8_t* t = &GSU.pvPrgBank[c];
               memcpy(t, &GSU.avCacheBackup[i << 4], 16);
               memcpy(&GSU.pvCache[i << 4], t, 16);
            }
            else
            {
               uint8_t* t1;
               uint8_t* t2;
               uint32_t a = 0x10000 - c;
               t1 = &GSU.pvPrgBank[c];
               t2 = &GSU.pvPrgBank[0];
               memcpy(t1, &GSU.avCacheBackup[i << 4], a);
               memcpy(&GSU.pvCache[i << 4], t1, a);
               memcpy(t2, &GSU.avCacheBackup[(i << 4) + a], 16 - a);
               memcpy(&GSU.pvCache[(i << 4) + a], t2, 16 - a);
            }
         }
         c = USEX16(c + 16);
         v >>= 1;
      }
#endif
}

void fx_flushCache()
{
   fx_restoreCache();
   GSU.vCacheFlags = 0;
   GSU.bCacheActive = false;
}


void fx_updateRamBank(uint8_t Byte)
{
   // Update BankReg and Bank pointer
   GSU.vRamBankReg = (uint32_t)Byte & (FX_RAM_BANKS - 1);
   GSU.pvRamBank = GSU.apvRamBank[Byte & 0x3];
}


static void fx_readRegisterSpace()
{
   int i;
   uint8_t* p;
   static uint32_t avHeight[] = { 128, 160, 192, 256 };
   static uint32_t avMult[] = { 16, 32, 32, 64 };

   GSU.vErrorCode = 0;

   /* Update R0-R15 */
   p = GSU.pvRegisters;
   for (i = 0; i < 16; i++)
   {
      GSU.avReg[i] = *p++;
      GSU.avReg[i] += ((uint32_t)(*p++)) << 8;
   }

   /* Update other registers */
   p = GSU.pvRegisters;
   GSU.vStatusReg = (uint32_t)p[GSU_SFR];
   GSU.vStatusReg |= ((uint32_t)p[GSU_SFR + 1]) << 8;
   GSU.vPrgBankReg = (uint32_t)p[GSU_PBR];
   GSU.vRomBankReg = (uint32_t)p[GSU_ROMBR];
   GSU.vRamBankReg = ((uint32_t)p[GSU_RAMBR]) & (FX_RAM_BANKS - 1);
   GSU.vCacheBaseReg = (uint32_t)p[GSU_CBR];
   GSU.vCacheBaseReg |= ((uint32_t)p[GSU_CBR + 1]) << 8;

   /* Update status register variables */
   GSU.vZero = !(GSU.vStatusReg & FLG_Z);
   GSU.vSign = (GSU.vStatusReg & FLG_S) << 12;
   GSU.vOverflow = (GSU.vStatusReg & FLG_OV) << 16;
   GSU.vCarry = (GSU.vStatusReg & FLG_CY) >> 2;

   /* Set bank pointers */
   GSU.pvRamBank = GSU.apvRamBank[GSU.vRamBankReg & 0x3];
   GSU.pvRomBank = GSU.apvRomBank[GSU.vRomBankReg];
   GSU.pvPrgBank = GSU.apvRomBank[GSU.vPrgBankReg];

   /* Set screen pointers */
   GSU.pvScreenBase = &GSU.pvRam[ USEX8(p[GSU_SCBR]) << 10 ];
   i = (int)(!!(p[GSU_SCMR] & 0x04));
   i |= ((int)(!!(p[GSU_SCMR] & 0x20))) << 1;
   GSU.vScreenHeight = GSU.vScreenRealHeight = avHeight[i];
   GSU.vMode = p[GSU_SCMR] & 0x03;
#if 0
   if (GSU.vMode == 2)
      error illegal color depth GSU.vMode;
#endif
   if (i == 3)
      GSU.vScreenSize = (256 / 8) * (256 / 8) * 32;
   else
      GSU.vScreenSize = (GSU.vScreenHeight / 8) * (256 / 8) * avMult[GSU.vMode];
   if (GSU.vPlotOptionReg & 0x10)
   {
      /* OBJ Mode (for drawing into sprites) */
      GSU.vScreenHeight = 256;
   }
#if 0
   if (GSU.pvScreenBase + GSU.vScreenSize > GSU.pvRam + (GSU.nRamBanks * 65536))
      error illegal address for screen base register
#else
   if (GSU.pvScreenBase + GSU.vScreenSize > GSU.pvRam + (GSU.nRamBanks * 65536))
      GSU.pvScreenBase =  GSU.pvRam + (GSU.nRamBanks * 65536) - GSU.vScreenSize;
#endif
      GSU.pfPlot = fx_apfPlotTable[GSU.vMode];
   GSU.pfRpix = fx_apfPlotTable[GSU.vMode + 5];

   fx_ppfOpcodeTable[0x04c] = GSU.pfPlot;
   fx_ppfOpcodeTable[0x14c] = GSU.pfRpix;
   fx_ppfOpcodeTable[0x24c] = GSU.pfPlot;
   fx_ppfOpcodeTable[0x34c] = GSU.pfRpix;

   fx_computeScreenPointers();

   fx_backupCache();
}

void fx_dirtySCBR()
{
   GSU.vSCBRDirty = true;
}

void fx_computeScreenPointers()
{
   if (GSU.vMode != GSU.vPrevMode ||
         GSU.vPrevScreenHeight != GSU.vScreenHeight ||
         GSU.vSCBRDirty)
   {
      int i;

      GSU.vSCBRDirty = false;

      /* Make a list of pointers to the start of each screen column */
      switch (GSU.vScreenHeight)
      {
      case 128:
         switch (GSU.vMode)
         {
         case 0:
            for (i = 0; i < 32; i++)
            {
               GSU.apvScreen[i] = GSU.pvScreenBase + (i << 4);
               GSU.x[i] = i << 8;
            }
            break;
         case 1:
            for (i = 0; i < 32; i++)
            {
               GSU.apvScreen[i] = GSU.pvScreenBase + (i << 5);
               GSU.x[i] = i << 9;
            }
            break;
         case 2:
         case 3:
            for (i = 0; i < 32; i++)
            {
               GSU.apvScreen[i] = GSU.pvScreenBase + (i << 6);
               GSU.x[i] = i << 10;
            }
            break;
         }
         break;
      case 160:
         switch (GSU.vMode)
         {
         case 0:
            for (i = 0; i < 32; i++)
            {
               GSU.apvScreen[i] = GSU.pvScreenBase + (i << 4);
               GSU.x[i] = (i << 8) + (i << 6);
            }
            break;
         case 1:
            for (i = 0; i < 32; i++)
            {
               GSU.apvScreen[i] = GSU.pvScreenBase + (i << 5);
               GSU.x[i] = (i << 9) + (i << 7);
            }
            break;
         case 2:
         case 3:
            for (i = 0; i < 32; i++)
            {
               GSU.apvScreen[i] = GSU.pvScreenBase + (i << 6);
               GSU.x[i] = (i << 10) + (i << 8);
            }
            break;
         }
         break;
      case 192:
         switch (GSU.vMode)
         {
         case 0:
            for (i = 0; i < 32; i++)
            {
               GSU.apvScreen[i] = GSU.pvScreenBase + (i << 4);
               GSU.x[i] = (i << 8) + (i << 7);
            }
            break;
         case 1:
            for (i = 0; i < 32; i++)
            {
               GSU.apvScreen[i] = GSU.pvScreenBase + (i << 5);
               GSU.x[i] = (i << 9) + (i << 8);
            }
            break;
         case 2:
         case 3:
            for (i = 0; i < 32; i++)
            {
               GSU.apvScreen[i] = GSU.pvScreenBase + (i << 6);
               GSU.x[i] = (i << 10) + (i << 9);
            }
            break;
         }
         break;
      case 256:
         switch (GSU.vMode)
         {
         case 0:
            for (i = 0; i < 32; i++)
            {
               GSU.apvScreen[i] = GSU.pvScreenBase +
                                  ((i & 0x10) << 9) + ((i & 0xf) << 8);
               GSU.x[i] = ((i & 0x10) << 8) + ((i & 0xf) << 4);
            }
            break;
         case 1:
            for (i = 0; i < 32; i++)
            {
               GSU.apvScreen[i] = GSU.pvScreenBase +
                                  ((i & 0x10) << 10) + ((i & 0xf) << 9);
               GSU.x[i] = ((i & 0x10) << 9) + ((i & 0xf) << 5);
            }
            break;
         case 2:
         case 3:
            for (i = 0; i < 32; i++)
            {
               GSU.apvScreen[i] = GSU.pvScreenBase +
                                  ((i & 0x10) << 11) + ((i & 0xf) << 10);
               GSU.x[i] = ((i & 0x10) << 10) + ((i & 0xf) << 6);
            }
            break;
         }
         break;
      }
      GSU.vPrevMode = GSU.vMode;
      GSU.vPrevScreenHeight = GSU.vScreenHeight;
   }
}

static void fx_writeRegisterSpace()
{
   int i;
   uint8_t* p;

   p = GSU.pvRegisters;
   for (i = 0; i < 16; i++)
   {
      *p++ = (uint8_t)GSU.avReg[i];
      *p++ = (uint8_t)(GSU.avReg[i] >> 8);
   }

   /* Update status register */
   if (USEX16(GSU.vZero) == 0) SF(Z);
   else CF(Z);
   if (GSU.vSign & 0x8000) SF(S);
   else CF(S);
   if (GSU.vOverflow >= 0x8000 || GSU.vOverflow < -0x8000) SF(OV);
   else CF(OV);
   if (GSU.vCarry) SF(CY);
   else CF(CY);

   p = GSU.pvRegisters;
   p[GSU_SFR] = (uint8_t)GSU.vStatusReg;
   p[GSU_SFR + 1] = (uint8_t)(GSU.vStatusReg >> 8);
   p[GSU_PBR] = (uint8_t)GSU.vPrgBankReg;
   p[GSU_ROMBR] = (uint8_t)GSU.vRomBankReg;
   p[GSU_RAMBR] = (uint8_t)GSU.vRamBankReg;
   p[GSU_CBR] = (uint8_t)GSU.vCacheBaseReg;
   p[GSU_CBR + 1] = (uint8_t)(GSU.vCacheBaseReg >> 8);

   fx_restoreCache();
}

/* Reset the FxChip */
void FxReset(struct FxInit_s* psFxInfo)
{
   int i;
   static uint32_t(**appfFunction[])(uint32_t) =
   {
      &fx_apfFunctionTable[0],
#if 0
      &fx_a_apfFunctionTable[0],
      &fx_r_apfFunctionTable[0],
      &fx_ar_apfFunctionTable[0],
#endif
   };
   static void (**appfPlot[])() =
   {
      &fx_apfPlotTable[0],
#if 0
      &fx_a_apfPlotTable[0],
      &fx_r_apfPlotTable[0],
      &fx_ar_apfPlotTable[0],
#endif
   };
   static void (**appfOpcode[])() =
   {
      &fx_apfOpcodeTable[0],
#if 0
      &fx_a_apfOpcodeTable[0],
      &fx_r_apfOpcodeTable[0],
      &fx_ar_apfOpcodeTable[0],
#endif
   };

   /* Get function pointers for the current emulation mode */
   fx_ppfFunctionTable = appfFunction[psFxInfo->vFlags & 0x3];
   fx_ppfPlotTable = appfPlot[psFxInfo->vFlags & 0x3];
   fx_ppfOpcodeTable = appfOpcode[psFxInfo->vFlags & 0x3];

   /* Clear all internal variables */
   memset((uint8_t*)&GSU, 0, sizeof(struct FxRegs_s));

   /* Set default registers */
   GSU.pvSreg = GSU.pvDreg = &R0;

   /* Set RAM and ROM pointers */
   GSU.pvRegisters = psFxInfo->pvRegisters;
   GSU.nRamBanks = psFxInfo->nRamBanks;
   GSU.pvRam = psFxInfo->pvRam;
   GSU.nRomBanks = psFxInfo->nRomBanks;
   GSU.pvRom = psFxInfo->pvRom;
   GSU.vPrevScreenHeight = ~0;
   GSU.vPrevMode = ~0;

   /* The GSU can't access more than 2mb (16mbits) */
   if (GSU.nRomBanks > 0x20)
      GSU.nRomBanks = 0x20;

   /* Clear FxChip register space */
   memset(GSU.pvRegisters, 0, 0x300);

   /* Set FxChip version Number */
   GSU.pvRegisters[0x3b] = 0;

   /* Make ROM bank table */
   for (i = 0; i < 256; i++)
   {
      uint32_t b = i & 0x7f;
      if (b >= 0x40)
      {
         if (GSU.nRomBanks > 1)
            b %= GSU.nRomBanks;
         else
            b &= 1;

         GSU.apvRomBank[i] = &GSU.pvRom[ b << 16 ];
      }
      else
      {
         b %= GSU.nRomBanks * 2;
         GSU.apvRomBank[i] = &GSU.pvRom[(b << 16) + 0x200000];
      }
   }

   /* Make RAM bank table */
   for (i = 0; i < 4; i++)
   {
      GSU.apvRamBank[i] = &GSU.pvRam[(i % GSU.nRamBanks) << 16];
      GSU.apvRomBank[0x70 + i] = GSU.apvRamBank[i];
   }

   /* Start with a nop in the pipe */
   GSU.vPipe = 0x01;

   /* Set pointer to GSU cache */
   GSU.pvCache = &GSU.pvRegisters[0x100];

   fx_readRegisterSpace();
}

static bool fx_checkStartAddress()
{
   /* Check if we start inside the cache */
   if (GSU.bCacheActive && R15 >= GSU.vCacheBaseReg
         && R15 < (GSU.vCacheBaseReg + 512))
      return true;

   /*  Check if we're in an unused area */
   if (GSU.vPrgBankReg < 0x40 && R15 < 0x8000)
      return false;
   if (GSU.vPrgBankReg >= 0x60 && GSU.vPrgBankReg <= 0x6f)
      return false;
   if (GSU.vPrgBankReg >= 0x74)
      return false;

   /* Check if we're in RAM and the RAN flag is not set */
   if (GSU.vPrgBankReg >= 0x70 && GSU.vPrgBankReg <= 0x73 && !(SCMR & (1 << 3)))
      return false;

   /* If not, we're in ROM, so check if the RON flag is set */
   if (!(SCMR & (1 << 4)))
      return false;

   return true;
}

/* Execute until the next stop instruction */
int FxEmulate(uint32_t nInstructions)
{
   uint32_t vCount;

   /* Read registers and initialize GSU session */
   fx_readRegisterSpace();

   /* Check if the start address is valid */
   if (!fx_checkStartAddress())
   {
      CF(G);
      fx_writeRegisterSpace();
#if 0
      GSU.vIllegalAddress = (GSU.vPrgBankReg << 24) | R15;
      return FX_ERROR_ILLEGAL_ADDRESS;
#else
      return 0;
#endif
   }

   /* Execute GSU session */
   CF(IRQ);

   if (GSU.bBreakPoint)
      vCount = fx_ppfFunctionTable[FX_FUNCTION_RUN_TO_BREAKPOINT](nInstructions);
   else
      vCount = fx_ppfFunctionTable[FX_FUNCTION_RUN](nInstructions);

   /* Store GSU registers */
   fx_writeRegisterSpace();

   /* Check for error code */
   if (GSU.vErrorCode)
      return GSU.vErrorCode;
   else
      return vCount;
}

/* Breakpoints */
void FxBreakPointSet(uint32_t vAddress)
{
   GSU.bBreakPoint = true;
   GSU.vBreakPoint = USEX16(vAddress);
}
void FxBreakPointClear()
{
   GSU.bBreakPoint = false;
}

/* Step by step execution */
int FxStepOver(uint32_t nInstructions)
{
   uint32_t vCount;
   fx_readRegisterSpace();

   /* Check if the start address is valid */
   if (!fx_checkStartAddress())
   {
      CF(G);
#if 0
      GSU.vIllegalAddress = (GSU.vPrgBankReg << 24) | R15;
      return FX_ERROR_ILLEGAL_ADDRESS;
#else
      return 0;
#endif
   }

   if (PIPE >= 0xf0)
      GSU.vStepPoint = USEX16(R15 + 3);
   else if ((PIPE >= 0x05 && PIPE <= 0x0f) || (PIPE >= 0xa0 && PIPE <= 0xaf))
      GSU.vStepPoint = USEX16(R15 + 2);
   else
      GSU.vStepPoint = USEX16(R15 + 1);
   vCount = fx_ppfFunctionTable[FX_FUNCTION_STEP_OVER](nInstructions);
   fx_writeRegisterSpace();
   if (GSU.vErrorCode)
      return GSU.vErrorCode;
   else
      return vCount;
}

/* Errors */
int FxGetErrorCode()
{
   return GSU.vErrorCode;
}

int FxGetIllegalAddress()
{
   return GSU.vIllegalAddress;
}

/* Access to internal registers */
uint32_t FxGetColorRegister()
{
   return GSU.vColorReg & 0xff;
}

uint32_t FxGetPlotOptionRegister()
{
   return GSU.vPlotOptionReg & 0x1f;
}

uint32_t FxGetSourceRegisterIndex()
{
   return GSU.pvSreg - GSU.avReg;
}

uint32_t FxGetDestinationRegisterIndex()
{
   return GSU.pvDreg - GSU.avReg;
}

uint8_t FxPipe()
{
   return GSU.vPipe;
}

