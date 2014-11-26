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
#ifndef _SNES9X_H_
#define _SNES9X_H_

#define VERSION "1.43-dev"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <libretro.h>

extern int cprintf(const char* fmt, ...);

#include "port.h"
#include "65c816.h"
#include "messages.h"

#define ROM_NAME_LEN 23

#define STREAM FILE*
#define READ_STREAM(p,l,s) fread (p,1,l,s)
#define WRITE_STREAM(p,l,s) fwrite (p,1,l,s)
#define OPEN_STREAM(f,m) fopen (f,m)
#define REOPEN_STREAM(f,m) fdopen (f,m)
#define FIND_STREAM(f)  ftell(f)
#define REVERT_STREAM(f,o,s)   fseek(f,o,s)
#define CLOSE_STREAM(s) fclose (s)


/* SNES screen width and height */
#define SNES_WIDTH      256
#define SNES_HEIGHT     224
#define SNES_HEIGHT_EXTENDED  239
#define IMAGE_WIDTH     (Settings.SupportHiRes ? SNES_WIDTH * 2 : SNES_WIDTH)
#define IMAGE_HEIGHT    (Settings.SupportHiRes ? SNES_HEIGHT_EXTENDED * 2 : SNES_HEIGHT_EXTENDED)

#define SNES_MAX_NTSC_VCOUNTER  262
#define SNES_MAX_PAL_VCOUNTER   312
#define SNES_HCOUNTER_MAX  342
#define SPC700_TO_65C816_RATIO   2
#define AUTO_FRAMERATE     200

/* NTSC master clock signal 21.47727MHz
 * PPU: master clock / 4
 * 1 / PPU clock * 342 -> 63.695us
 * 63.695us / (1 / 3.579545MHz) -> 228 cycles per scanline
 * From Earth Worm Jim: APU executes an average of 65.14285714 cycles per
 * scanline giving an APU clock speed of 1.022731096MHz                    */

/* PAL master clock signal 21.28137MHz
 * PPU: master clock / 4
 * 1 / PPU clock * 342 -> 64.281us
 * 64.281us / (1 / 3.546895MHz) -> 228 cycles per scanline.  */

#define SNES_SCANLINE_TIME (63.695e-6)
#define SNES_CLOCK_SPEED (3579545)

#define SNES_CLOCK_LEN (1.0 / SNES_CLOCK_SPEED)

#define SNES_CYCLES_PER_SCANLINE ((uint32_t) ((SNES_SCANLINE_TIME / SNES_CLOCK_LEN) * 6 + 0.5))

#define ONE_CYCLE 6
#define SLOW_ONE_CYCLE 8
#define TWO_CYCLES 12


#define SNES_TR_MASK     (1 << 4)
#define SNES_TL_MASK     (1 << 5)
#define SNES_X_MASK      (1 << 6)
#define SNES_A_MASK      (1 << 7)
#define SNES_RIGHT_MASK     (1 << 8)
#define SNES_LEFT_MASK      (1 << 9)
#define SNES_DOWN_MASK      (1 << 10)
#define SNES_UP_MASK     (1 << 11)
#define SNES_START_MASK     (1 << 12)
#define SNES_SELECT_MASK    (1 << 13)
#define SNES_Y_MASK      (1 << 14)
#define SNES_B_MASK      (1 << 15)

enum
{
   SNES_MULTIPLAYER5,
   SNES_JOYPAD,
   SNES_MOUSE_SWAPPED,
   SNES_MOUSE,
   SNES_SUPERSCOPE,
   SNES_JUSTIFIER,
   SNES_JUSTIFIER_2,
   SNES_MAX_CONTROLLER_OPTIONS
};

#define DEBUG_MODE_FLAG     (1 << 0)
#define TRACE_FLAG         (1 << 1)
#define SINGLE_STEP_FLAG    (1 << 2)
#define BREAK_FLAG         (1 << 3)
#define SCAN_KEYS_FLAG      (1 << 4)
#define SAVE_SNAPSHOT_FLAG  (1 << 5)
#define DELAYED_NMI_FLAG    (1 << 6)
#define NMI_FLAG        (1 << 7)
#define PROCESS_SOUND_FLAG  (1 << 8)
#define FRAME_ADVANCE_FLAG  (1 << 9)
#define DELAYED_NMI_FLAG2   (1 << 10)
#define IRQ_PENDING_FLAG    (1 << 11)

typedef struct
{
   uint32_t  Flags;
   bool   BranchSkip;
   bool   NMIActive;
   bool   IRQActive;
   bool   WaitingForInterrupt;
   bool   InDMA;
   uint8_t   WhichEvent;
   uint8_t*   PC;
   uint8_t*   PCBase;
   uint8_t*   PCAtOpcodeStart;
   uint8_t*   WaitAddress;
   uint32_t  WaitCounter;
   long   Cycles;
   long   NextEvent;
   long   V_Counter;
   long   MemSpeed;
   long   MemSpeedx2;
   long   FastROMSpeed;
   uint32_t AutoSaveTimer;
   bool  SRAMModified;
   uint32_t NMITriggerPoint;
   bool  BRKTriggered;
   bool  TriedInterleavedMode2;
   uint32_t NMICycleCount;
   uint32_t IRQCycleCount;
#ifdef DEBUG_MAXCOUNT
   unsigned long GlobalLoopCount;
#endif
} SCPUState;

#define HBLANK_START_EVENT 0
#define HBLANK_END_EVENT 1
#define HTIMER_BEFORE_EVENT 2
#define HTIMER_AFTER_EVENT 3
#define NO_EVENT 4

typedef struct
{
   /* CPU options */
   bool  APUEnabled;
   bool  Shutdown;
   uint8_t  SoundSkipMethod;
   long   H_Max;
   long   HBlankStart;
   long   CyclesPercentage;
   bool  DisableIRQ;
   bool  Paused;
   bool  ForcedPause;
   bool  StopEmulation;
   bool  FrameAdvance;

   /* Tracing options */
   bool  TraceDMA;
   bool  TraceHDMA;
   bool  TraceVRAM;
   bool  TraceUnknownRegisters;
   bool  TraceDSP;

   /* Joystick options */
   bool  SwapJoypads;
   bool  JoystickEnabled;

   /* ROM timing options (see also H_Max above) */
   bool  ForcePAL;
   bool  ForceNTSC;
   bool  PAL;
   uint32_t FrameTimePAL;
   uint32_t FrameTimeNTSC;
   uint32_t FrameTime;
   uint32_t SkipFrames;

   /* ROM image options */
   bool  ForceLoROM;
   bool  ForceHiROM;
   bool  ForceHeader;
   bool  ForceNoHeader;
   bool  ForceInterleaved;
   bool  ForceInterleaved2;
   bool  ForceNotInterleaved;

   /* Peripherial options */
   bool  ForceSuperFX;
   bool  ForceNoSuperFX;
   bool  ForceDSP1;
   bool  ForceNoDSP1;
   bool  ForceSA1;
   bool  ForceNoSA1;
   bool  ForceC4;
   bool  ForceNoC4;
   bool  ForceSDD1;
   bool  ForceNoSDD1;
   bool  MultiPlayer5;
   bool  Mouse;
   bool  SuperScope;
   bool  SRTC;
   uint32_t ControllerOption;

   bool  ShutdownMaster;
   bool  MultiPlayer5Master;
   bool  SuperScopeMaster;
   bool  MouseMaster;
   bool  SuperFX;
   bool  DSP1Master;
   bool  SA1;
   bool  C4;
   bool  SDD1;
   bool  SPC7110;
   bool  SPC7110RTC;
   bool  OBC1;
   /* Sound options */
   uint32_t SoundPlaybackRate;
#ifdef USE_BLARGG_APU
   uint32_t SoundInputRate;
#endif
   bool  TraceSoundDSP;
   bool  EightBitConsoleSound;  // due to caching, this needs S9xSetEightBitConsoleSound()
   int    SoundBufferSize;
   int    SoundMixInterval;
   bool  SoundEnvelopeHeightReading;
   bool  DisableSoundEcho;
   bool  DisableMasterVolume;
   bool  SoundSync;
   bool  InterpolatedSound;
   bool  ThreadSound;
   bool  Mute;
   bool  NextAPUEnabled;

   /* Graphics options */
   bool  Transparency;
   bool  SupportHiRes;
   bool  Mode7Interpolate;

   /* SNES graphics options */
   bool  BGLayering;
   bool  DisableGraphicWindows;
   bool  ForceTransparency;
   bool  ForceNoTransparency;
   bool  DisableHDMA;
   bool  DisplayFrameRate;
   bool  DisableRangeTimeOver; /* XXX: unused */

   /* Others */
   bool  ApplyCheats;

   /* Fixes for individual games */
   bool  StarfoxHack;
   bool  WinterGold;
   bool  BS; /* Japanese Satellite System games. */
   bool  DaffyDuck;
   uint8_t  APURAMInitialValue;
   bool  SampleCatchup;
   bool  JustifierMaster;
   bool  Justifier;
   bool  SecondJustifier;
   int8_t   SETA;
   bool  TakeScreenshot;
   int8_t   StretchScreenshots;
   uint16_t DisplayColor;
   int    SoundDriver;
   int    AIDOShmId;
   bool  SDD1Pack;
   bool  NoPatch;
   bool  ForceInterleaveGD24;
#ifdef DEBUG_MAXCOUNT
   unsigned int MaxCount;
#endif
} SSettings;

typedef struct
{
   uint8_t alienVSpredetorFix;
   uint8_t APU_OutPorts_ReturnValueFix;
   uint8_t SoundEnvelopeHeightReading2;
   uint8_t SRAMInitialValue;
   uint8_t Uniracers;
   bool EchoOnlyOutput;
} SSNESGameFixes;

extern SSettings Settings;
extern SCPUState CPU;
extern SSNESGameFixes SNESGameFixes;
extern char String [513];

void S9xMessage(int type, int number, const char* message);
void S9xLoadSDD1Data();

void S9xSetPause(uint32_t mask);
void S9xClearPause(uint32_t mask);

#endif

