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
#ifndef USE_BLARGG_APU

#ifdef __DJGPP__
#include <allegro.h>
#undef true
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#define CLIP16(v) \
   if ((v) < -32768) \
    (v) = -32768; \
   else \
   if ((v) > 32767) \
(v) = 32767

#define CLIP16_latch(v,l) \
   if ((v) < -32768) \
{ (v) = -32768; (l)++; }\
   else \
   if ((v) > 32767) \
{ (v) = 32767; (l)++; }

#define CLIP24(v) \
   if ((v) < -8388608) \
    (v) = -8388608; \
   else \
   if ((v) > 8388607) \
(v) = 8388607

#define CLIP8(v) \
   if ((v) < -128) \
    (v) = -128; \
   else \
   if ((v) > 127) \
(v) = 127

#include "snes9x.h"
#include "soundux.h"
#include "apu.h"
#include "memmap.h"
#include "cpuexec.h"

extern int32_t Echo [24000];
extern int32_t DummyEchoBuffer [SOUND_BUFFER_SIZE];
extern int32_t MixBuffer [SOUND_BUFFER_SIZE];
extern int32_t EchoBuffer [SOUND_BUFFER_SIZE];
extern int32_t FilterTaps [8];
static uint8_t FilterTapDefinitionBitfield;
// In the above, bit I is set if FilterTaps[I] is non-zero.
extern uint32_t Z;
extern int32_t Loop [16];

extern long FilterValues[4][2];
extern int32_t NoiseFreq [32];

static int32_t noise_gen;

#undef ABS
#define ABS(a) ((a) < 0 ? -(a) : (a))

#define FIXED_POINT 0x10000UL
#define FIXED_POINT_REMAINDER 0xffffUL
#define FIXED_POINT_SHIFT 16

#define VOL_DIV8  0x8000
#define VOL_DIV16 0x0080
#define ENVX_SHIFT 24

void DecodeBlockAsm(int8_t*, int16_t*, int32_t*, int32_t*);
void DecodeBlockAsm2(int8_t*, int16_t*, int32_t*, int32_t*);

// F is channel's current frequency and M is the 16-bit modulation waveform
// from the previous channel multiplied by the current envelope volume level.
#define PITCH_MOD(F,M) ((F) * ((((uint32_t) (M)) + 0x800000) >> 16) >> 7)
//#define PITCH_MOD(F,M) ((F) * ((((M) & 0x7fffff) >> 14) + 1) >> 8)

#define LAST_SAMPLE 0xffffff
#define JUST_PLAYED_LAST_SAMPLE(c) ((c)->sample_pointer >= LAST_SAMPLE)

void S9xSetEightBitConsoleSound(bool Enabled)
{
   if (Settings.EightBitConsoleSound != Enabled)
   {
      Settings.EightBitConsoleSound = Enabled;
      int i;
      for (i = 0; i < 8; i++)
         SoundData.channels[i].needs_decode = true;
   }
}

STATIC inline uint8_t* S9xGetSampleAddress(int sample_number)
{
   uint32_t addr = (((APU.DSP[APU_DIR] << 8) + (sample_number << 2)) & 0xffff);
   return (IAPU.RAM + addr);
}

void S9xAPUSetEndOfSample(int i, Channel* ch)
{
   ch->state = SOUND_SILENT;
   ch->mode = MODE_NONE;
   APU.DSP [APU_ENDX] |= 1 << i;
   APU.DSP [APU_KON] &= ~(1 << i);
   APU.DSP [APU_KOFF] &= ~(1 << i);
   APU.KeyedChannels &= ~(1 << i);
}
#ifdef __DJGPP
END_OF_FUNCTION(S9xAPUSetEndOfSample)
#endif

void S9xAPUSetEndX(int ch)
{
   APU.DSP [APU_ENDX] |= 1 << ch;
}
#ifdef __DJGPP
END_OF_FUNCTION(S9xAPUSetEndX)
#endif

void S9xSetEnvRate(Channel* ch, uint32_t rate, int direction, int target)
{
   ch->envx_target = target;

   if (rate == ~0UL)
   {
      ch->direction = 0;
      rate = 0;
   }
   else
      ch->direction = direction;

   static int64_t steps [] =
   {
      // 0, 64, 1238, 1238, 256, 1, 64, 109, 64, 1238
      0,
      (int64_t) FIXED_POINT * 1000 * 64,
      (int64_t) FIXED_POINT * 1000 * 619,
      (int64_t) FIXED_POINT * 1000 * 619,
      (int64_t) FIXED_POINT * 1000 * 128,
      (int64_t) FIXED_POINT * 1000 * 1,
      (int64_t) FIXED_POINT * 1000 * 64,
      (int64_t) FIXED_POINT * 1000 * 55,
      (int64_t) FIXED_POINT * 1000 * 64,
      (int64_t) FIXED_POINT * 1000 * 619
   };

   if (rate == 0 || so.playback_rate == 0)
      ch->erate = 0;
   else
   {
      ch->erate = (uint32_t)
                  (steps [ch->state] / (rate * so.playback_rate));
   }
}

#ifdef __DJGPP
END_OF_FUNCTION(S9xSetEnvRate);
#endif

void S9xSetEnvelopeRate(int channel, uint32_t rate, int direction,
                        int target)
{
   S9xSetEnvRate(&SoundData.channels [channel], rate, direction, target);
}

#ifdef __DJGPP
END_OF_FUNCTION(S9xSetEnvelopeRate);
#endif

void S9xSetSoundVolume(int channel, int16_t volume_left, int16_t volume_right)
{
   Channel* ch = &SoundData.channels[channel];
   ch->volume_left = volume_left;
   ch->volume_right = volume_right;
   ch-> left_vol_level = (ch->envx * volume_left) / 128;
   ch->right_vol_level = (ch->envx * volume_right) / 128;
}

void S9xSetMasterVolume(int16_t volume_left, int16_t volume_right)
{
   if (Settings.DisableMasterVolume || SNESGameFixes.EchoOnlyOutput)
      SoundData.master_volume [0] = SoundData.master_volume [1] = 127;
   else
   {
      SoundData.master_volume [0] = volume_left;
      SoundData.master_volume [1] = volume_right;
   }
}

void S9xSetEchoVolume(int16_t volume_left, int16_t volume_right)
{
   SoundData.echo_volume [0] = volume_left;
   SoundData.echo_volume [1] = volume_right;
}

void S9xSetEchoEnable(uint8_t byte)
{
   SoundData.echo_channel_enable = byte;
   if (!SoundData.echo_write_enabled || Settings.DisableSoundEcho)
      byte = 0;
   if (byte && !SoundData.echo_enable)
   {
      memset(Echo, 0, sizeof(Echo));
      memset(Loop, 0, sizeof(Loop));
   }

   SoundData.echo_enable = byte;
   int i;
   for (i = 0; i < NUM_CHANNELS; i++)
   {
      if (byte & (1 << i))
         SoundData.channels [i].echo_buf_ptr = EchoBuffer;
      else
         SoundData.channels [i].echo_buf_ptr = DummyEchoBuffer;
   }
}

void S9xSetEchoFeedback(int feedback)
{
   CLIP8(feedback);
   SoundData.echo_feedback = feedback;
}

void S9xSetEchoDelay(int delay)
{
   SoundData.echo_buffer_size = (512 * delay * so.playback_rate) / 32000;
   SoundData.echo_buffer_size <<= 1;
   if (SoundData.echo_buffer_size)
      SoundData.echo_ptr %= SoundData.echo_buffer_size;
   else
      SoundData.echo_ptr = 0;
   S9xSetEchoEnable(APU.DSP [APU_EON]);
}

void S9xSetEchoWriteEnable(uint8_t byte)
{
   SoundData.echo_write_enabled = byte;
   S9xSetEchoDelay(APU.DSP [APU_EDL] & 15);
}

void S9xSetFrequencyModulationEnable(uint8_t byte)
{
   SoundData.pitch_mod = byte & ~1;
}

void S9xSetSoundKeyOff(int channel)
{
   Channel* ch = &SoundData.channels[channel];

   if (ch->state != SOUND_SILENT)
   {
      ch->state = SOUND_RELEASE;
      ch->mode = MODE_RELEASE;
      S9xSetEnvRate(ch, 8, -1, 0);
   }
}

void S9xFixSoundAfterSnapshotLoad()
{
   SoundData.echo_write_enabled = !(APU.DSP [APU_FLG] & 0x20);
   SoundData.echo_channel_enable = APU.DSP [APU_EON];
   S9xSetEchoDelay(APU.DSP [APU_EDL] & 0xf);
   S9xSetEchoFeedback((signed char) APU.DSP [APU_EFB]);

   S9xSetFilterCoefficient(0, (signed char) APU.DSP [APU_C0]);
   S9xSetFilterCoefficient(1, (signed char) APU.DSP [APU_C1]);
   S9xSetFilterCoefficient(2, (signed char) APU.DSP [APU_C2]);
   S9xSetFilterCoefficient(3, (signed char) APU.DSP [APU_C3]);
   S9xSetFilterCoefficient(4, (signed char) APU.DSP [APU_C4]);
   S9xSetFilterCoefficient(5, (signed char) APU.DSP [APU_C5]);
   S9xSetFilterCoefficient(6, (signed char) APU.DSP [APU_C6]);
   S9xSetFilterCoefficient(7, (signed char) APU.DSP [APU_C7]);
   int i;
   for (i = 0; i < 8; i++)
   {
      SoundData.channels[i].needs_decode = true;
      S9xSetSoundFrequency(i, SoundData.channels[i].hertz);
      SoundData.channels [i].envxx = SoundData.channels [i].envx << ENVX_SHIFT;
      SoundData.channels [i].next_sample = 0;
      SoundData.channels [i].interpolate = 0;
      SoundData.channels [i].previous [0] = (int32_t)
                                            SoundData.channels [i].previous16 [0];
      SoundData.channels [i].previous [1] = (int32_t)
                                            SoundData.channels [i].previous16 [1];
   }
   IAPU.Scanline = 0;
}

void S9xSetFilterCoefficient(int tap, int value)
{
   FilterTaps [tap & 7] = value;
   if (value == 0 || (tap == 0 && value == 127))
      FilterTapDefinitionBitfield &= ~(1 << (tap & 7));
   else
      FilterTapDefinitionBitfield |= 1 << (tap & 7);
}

void S9xSetSoundADSR(int channel, int attack_rate, int decay_rate,
                     int sustain_rate, int sustain_level, int release_rate)
{
   Channel* ch = &SoundData.channels[channel];
   ch->attack_rate = attack_rate;
   ch->decay_rate = decay_rate;
   ch->sustain_rate = sustain_rate;
   ch->release_rate = release_rate;
   ch->sustain_level = sustain_level + 1;

   switch (SoundData.channels[channel].state)
   {
   case SOUND_ATTACK:
      S9xSetEnvRate(ch, attack_rate, 1, 127);
      break;

   case SOUND_DECAY:
      S9xSetEnvRate(ch, decay_rate, -1,
                    (MAX_ENVELOPE_HEIGHT * (sustain_level + 1)) >> 3);
      break;
   case SOUND_SUSTAIN:
      S9xSetEnvRate(ch, sustain_rate, -1, 0);
      break;
   }
}

void S9xSetEnvelopeHeight(int channel, int level)
{
   Channel* ch = &SoundData.channels[channel];

   ch->envx = level;
   ch->envxx = level << ENVX_SHIFT;

   ch->left_vol_level = (level * ch->volume_left) / 128;
   ch->right_vol_level = (level * ch->volume_right) / 128;

   if (ch->envx == 0 && ch->state != SOUND_SILENT && ch->state != SOUND_GAIN)
      S9xAPUSetEndOfSample(channel, ch);
}

int S9xGetEnvelopeHeight(int channel)
{
   if ((Settings.SoundEnvelopeHeightReading ||
         SNESGameFixes.SoundEnvelopeHeightReading2) &&
         SoundData.channels[channel].state != SOUND_SILENT &&
         SoundData.channels[channel].state != SOUND_GAIN)
      return (SoundData.channels[channel].envx);

   //siren fix from XPP
   if (SNESGameFixes.SoundEnvelopeHeightReading2 &&
         SoundData.channels[channel].state != SOUND_SILENT)
      return (SoundData.channels[channel].envx);

   return (0);
}

#if 1
void S9xSetSoundSample(int channel, uint16_t sample_number)
{
}
#else
void S9xSetSoundSample(int channel, uint16_t sample_number)
{
   register Channel* ch = &SoundData.channels[channel];

   if (ch->state != SOUND_SILENT &&
         sample_number != ch->sample_number)
   {
      int keep = ch->state;
      ch->state = SOUND_SILENT;
      ch->sample_number = sample_number;
      ch->loop = false;
      ch->needs_decode = true;
      ch->last_block = false;
      ch->previous [0] = ch->previous[1] = 0;
      uint8_t* dir = S9xGetSampleAddress(sample_number);
      ch->block_pointer = READ_WORD(dir);
      ch->sample_pointer = 0;
      ch->state = keep;
   }
}
#endif

void S9xSetSoundFrequency(int channel, int hertz)
{
   if (so.playback_rate)
   {
      if (SoundData.channels[channel].type == SOUND_NOISE)
         hertz = NoiseFreq [APU.DSP [APU_FLG] & 0x1f];
      SoundData.channels[channel].frequency = (int)
                                              (((int64_t) hertz * FIXED_POINT) / so.playback_rate);
   }
}

void S9xSetSoundHertz(int channel, int hertz)
{
   SoundData.channels[channel].hertz = hertz;
   S9xSetSoundFrequency(channel, hertz);
}

void S9xSetSoundType(int channel, int type_of_sound)
{
   SoundData.channels[channel].type = type_of_sound;
}

void DecodeBlock(Channel* ch)
{
   int32_t out;
   unsigned char filter;
   unsigned char shift;
   signed char sample1, sample2;
   unsigned char i;
   bool invalid_header;

   if (ch->block_pointer > 0x10000 - 9)
   {
      ch->last_block = true;
      ch->loop = false;
      ch->block = ch->decoded;
      return;
   }

   if (Settings.EightBitConsoleSound)
   {
      signed char* compressed = (signed char*) &IAPU.RAM [ch->block_pointer];

      filter = *compressed;
      if ((ch->last_block = filter & 1))
         ch->loop = (filter & 2) != 0;

      int16_t interim[16];
      uint8_t interim_byte = 0;

      compressed++;
      int16_t* raw = ch->block = ch->decoded;

      // Seperate out the header parts used for decoding

      shift = filter >> 4;

      // Header validity check: if range(shift) is over 12, ignore
      // all bits of the data for that block except for the sign bit of each
      invalid_header = (shift >= 0xD);

      filter = filter & 0x0c;

      int32_t prev0 = ch->previous [0];
      int32_t prev1 = ch->previous [1];

      int16_t amplitude = 0;

      for (i = 8; i != 0; i--)
      {
         sample1 = *compressed++;
         sample2 = sample1 << 4;
         //Sample 2 = Bottom Nibble, Sign Extended.
         sample2 >>= 4;
         //Sample 1 = Top Nibble, shifted down and Sign Extended.
         sample1 >>= 4;
         if (invalid_header)
         {
            sample1 >>= 3;
            sample2 >>= 3;
         }

         int nybblesmp;
         for (nybblesmp = 0; nybblesmp < 2; nybblesmp++)
         {
            out = (((nybblesmp) ? sample2 : sample1) << shift);
            out >>= 1;

            switch (filter)
            {
            case 0x00:
               // Method0 - [Smp]
               break;

            case 0x04:
               // Method1 - [Delta]+[Smp-1](15/16)
               out += (prev0 >> 1) + ((-prev0) >> 5);
               break;

            case 0x08:
               // Method2 - [Delta]+[Smp-1](61/32)-[Smp-2](15/16)
               out += (prev0) + ((-(prev0 + (prev0 >> 1))) >> 5) - (prev1 >> 1) + (prev1 >> 5);
               break;

            default:
               // Method3 - [Delta]+[Smp-1](115/64)-[Smp-2](13/16)
               out += (prev0) + ((-(prev0 + (prev0 << 2) + (prev0 << 3))) >> 7) -
                      (prev1 >> 1) + ((prev1 + (prev1 >> 1)) >> 4);
               break;

            }
            CLIP16(out);
            int16_t result = (int16_t)(out << 1);
            if (abs(result) > amplitude)
               amplitude = abs(result);
            interim[interim_byte++] = out;
            prev1 = (int16_t)prev0;
            prev0 = (int16_t)(out << 1);
         }
      }
      ch->previous [0] = prev0;
      ch->previous [1] = prev1;

      int32_t total_deviation_from_previous = 0;
      for (i = 1; i < 16; i++)
         total_deviation_from_previous += abs(interim[i] - interim[i - 1]);
      if (total_deviation_from_previous >= (int32_t) amplitude * 4)
      {
         /* Looks like noise. Generate noise. */
         for (i = 0; i < 16; i++)
         {
            int feedback = (noise_gen << 13) ^ (noise_gen << 14);
            noise_gen = (feedback & 0x4000) ^ (noise_gen >> 1);
            ch->decoded[i] = (noise_gen << 17) >> 17;
         }
      }
      else if (interim[0] < interim[1] && interim[1] < interim[2]
               && interim[2] < interim[3]
               && interim[4] > interim[5] && interim[5] > interim[6]
               && interim[6] > interim[7] && interim[7] > interim[8]
               && interim[8] > interim[9] && interim[9] > interim[10]
               && interim[10] > interim[11]
               && interim[12] < interim[13] && interim[13] < interim[14]
               && interim[14] < interim[15])
      {
         /* Looks like a sine or triangle wave. Make it a
          * triangle wave with an amplitude equivalent to that
          * of the highest amplitude sample of the block. */
         ch->decoded[0] =  ch->decoded[8]  = 0;
         ch->decoded[1] =  ch->decoded[7]  = amplitude / 4;
         ch->decoded[2] =  ch->decoded[6]  = amplitude / 2;
         ch->decoded[3] =  ch->decoded[5]  = amplitude * 3 / 4;
         ch->decoded[4] =  amplitude;
         ch->decoded[9] =  ch->decoded[15] = -(amplitude / 4);
         ch->decoded[10] = ch->decoded[14] = -(amplitude / 2);
         ch->decoded[11] = ch->decoded[13] = -(amplitude * 3 / 4);
         ch->decoded[12] = -amplitude;
      }
      else if (interim[0] > interim[1] && interim[1] > interim[2]
               && interim[2] > interim[3]
               && interim[4] < interim[5] && interim[5] < interim[6]
               && interim[6] < interim[7] && interim[7] < interim[8]
               && interim[8] < interim[9] && interim[9] < interim[10]
               && interim[10] < interim[11]
               && interim[12] > interim[13] && interim[13] > interim[14]
               && interim[14] > interim[15])
      {
         /* Inverted triangle wave. */
         ch->decoded[0] =  ch->decoded[8]  = 0;
         ch->decoded[1] =  ch->decoded[7]  = -(amplitude / 4);
         ch->decoded[2] =  ch->decoded[6]  = -(amplitude / 2);
         ch->decoded[3] =  ch->decoded[5]  = -(amplitude * 3 / 4);
         ch->decoded[4] = -amplitude;
         ch->decoded[9] =  ch->decoded[15] = amplitude / 4;
         ch->decoded[10] = ch->decoded[14] = amplitude / 2;
         ch->decoded[11] = ch->decoded[13] = amplitude * 3 / 4;
         ch->decoded[12] = amplitude;
      }
      else if (interim[0] < interim[1] && interim[1] < interim[2]
               && interim[2] < interim[3] && interim[3] < interim[4]
               && interim[4] < interim[5] && interim[5] < interim[6]
               && interim[6] < interim[7]
               && interim[8] > interim[9] && interim[9] > interim[10]
               && interim[10] > interim[11] && interim[11] > interim[12]
               && interim[12] > interim[13] && interim[13] > interim[14]
               && interim[14] > interim[15])
      {
         /* Looks like a V wave. Make it a half-triangle wave
          * with an amplitude equivalent to that
          * of the highest amplitude sample of the block. */
         ch->decoded[0] =  0;
         ch->decoded[1] =  ch->decoded[15] = amplitude / 8;
         ch->decoded[2] =  ch->decoded[14] = amplitude / 4;
         ch->decoded[3] =  ch->decoded[13] = amplitude * 3 / 8;
         ch->decoded[4] =  ch->decoded[12] = amplitude / 2;
         ch->decoded[5] =  ch->decoded[11] = amplitude * 5 / 8;
         ch->decoded[6] =  ch->decoded[10] = amplitude * 3 / 4;
         ch->decoded[7] =  ch->decoded[9]  = amplitude * 7 / 8;
         ch->decoded[8] =  amplitude;
      }
      else if (interim[0] > interim[1] && interim[1] > interim[2]
               && interim[2] > interim[3] && interim[3] > interim[4]
               && interim[4] > interim[5] && interim[5] > interim[6]
               && interim[6] > interim[7]
               && interim[8] < interim[9] && interim[9] < interim[10]
               && interim[10] < interim[11] && interim[11] < interim[12]
               && interim[12] < interim[13] && interim[13] < interim[14]
               && interim[14] < interim[15])
      {
         /* Inverted V wave. */
         ch->decoded[0] =  0;
         ch->decoded[1] =  ch->decoded[15] = -(amplitude / 8);
         ch->decoded[2] =  ch->decoded[14] = -(amplitude / 4);
         ch->decoded[3] =  ch->decoded[13] = -(amplitude * 3 / 8);
         ch->decoded[4] =  ch->decoded[12] = -(amplitude / 2);
         ch->decoded[5] =  ch->decoded[11] = -(amplitude * 5 / 8);
         ch->decoded[6] =  ch->decoded[10] = -(amplitude * 3 / 4);
         ch->decoded[7] =  ch->decoded[9]  = -(amplitude * 7 / 8);
         ch->decoded[8] =  -amplitude;
      }
      else
      {
         // Make it a square wave with an amplitude equivalent to that
         // of the highest amplitude sample of the block.
         // But actually put half of the amplitude, because
         // square waves are just loud.
         for (i = 0; i < 8; i++)
            ch->decoded[i] = amplitude / 2;
         for (i = 8; i < 16; i++)
            ch->decoded[i] = -(amplitude / 2);
      }
   }
   else
   {
      signed char* compressed = (signed char*) &IAPU.RAM [ch->block_pointer];

      filter = *compressed;
      if ((ch->last_block = filter & 1))
         ch->loop = (filter & 2) != 0;

      compressed++;
      int16_t* raw = ch->block = ch->decoded;

      // Seperate out the header parts used for decoding

      shift = filter >> 4;

      // Header validity check: if range(shift) is over 12, ignore
      // all bits of the data for that block except for the sign bit of each
      invalid_header = (shift >= 0xD);

      filter = filter & 0x0c;

      int32_t prev0 = ch->previous [0];
      int32_t prev1 = ch->previous [1];

      for (i = 8; i != 0; i--)
      {
         sample1 = *compressed++;
         sample2 = sample1 << 4;
         //Sample 2 = Bottom Nibble, Sign Extended.
         sample2 >>= 4;
         //Sample 1 = Top Nibble, shifted down and Sign Extended.
         sample1 >>= 4;
         if (invalid_header)
         {
            sample1 >>= 3;
            sample2 >>= 3;
         }

         int nybblesmp;
         for (nybblesmp = 0; nybblesmp < 2; nybblesmp++)
         {
            out = (((nybblesmp) ? sample2 : sample1) << shift);
            out >>= 1;

            switch (filter)
            {
            case 0x00:
               // Method0 - [Smp]
               break;

            case 0x04:
               // Method1 - [Delta]+[Smp-1](15/16)
               out += (prev0 >> 1) + ((-prev0) >> 5);
               break;

            case 0x08:
               // Method2 - [Delta]+[Smp-1](61/32)-[Smp-2](15/16)
               out += (prev0) + ((-(prev0 + (prev0 >> 1))) >> 5) - (prev1 >> 1) + (prev1 >> 5);
               break;

            default:
               // Method3 - [Delta]+[Smp-1](115/64)-[Smp-2](13/16)
               out += (prev0) + ((-(prev0 + (prev0 << 2) + (prev0 << 3))) >> 7) -
                      (prev1 >> 1) + ((prev1 + (prev1 >> 1)) >> 4);
               break;

            }
            CLIP16(out);
            *raw++ = (int16_t)(out << 1);
            prev1 = (int16_t)prev0;
            prev0 = (int16_t)(out << 1);
         }
      }
      ch->previous [0] = prev0;
      ch->previous [1] = prev1;
   }
   ch->block_pointer += 9;
}

static inline void MixStereo(int sample_count)
{
   static int32_t wave[SOUND_BUFFER_SIZE];

   int pitch_mod = SoundData.pitch_mod & ~APU.DSP[APU_NON];

   uint32_t J;
   for (J = 0; J < NUM_CHANNELS; J++)
   {
      Channel* ch = &SoundData.channels[J];

      if (ch->state == SOUND_SILENT || !(so.sound_switch & (1 << J)))
         continue;

      int32_t VL, VR;
      uint32_t freq0 = ch->frequency;

      bool mod = pitch_mod & (1 << J);

      if (ch->needs_decode)
      {
         DecodeBlock(ch);
         ch->needs_decode = false;
         ch->sample = ch->block[0];
         ch->sample_pointer = freq0 >> FIXED_POINT_SHIFT;
         if (ch->sample_pointer == 0)
            ch->sample_pointer = 1;
         if (ch->sample_pointer > SOUND_DECODE_LENGTH)
            ch->sample_pointer = SOUND_DECODE_LENGTH - 1;

         ch->next_sample = ch->block[ch->sample_pointer];
         ch->interpolate = 0;

         if (Settings.InterpolatedSound && freq0 < FIXED_POINT && !mod)
            ch->interpolate = ((ch->next_sample - ch->sample) *
                               (long) freq0) / (long) FIXED_POINT;
      }
      VL = (ch->sample * ch-> left_vol_level) / 128;
      VR = (ch->sample * ch->right_vol_level) / 128;

      uint32_t I;
      for (I = 0; I < (uint32_t) sample_count; I += 2)
      {
         uint32_t freq = freq0;

         if (mod)
            freq = PITCH_MOD(freq, wave [I / 2]);

         ch->env_error += ch->erate;
         if (ch->env_error >= FIXED_POINT)
         {
            uint32_t step = ch->env_error >> FIXED_POINT_SHIFT;

            switch (ch->state)
            {
            case SOUND_ATTACK:
               ch->env_error &= FIXED_POINT_REMAINDER;
               ch->envx += step << 1;
               ch->envxx = ch->envx << ENVX_SHIFT;

               if (ch->envx >= 126)
               {
                  ch->envx = 127;
                  ch->envxx = 127 << ENVX_SHIFT;
                  ch->state = SOUND_DECAY;
                  if (ch->sustain_level != 8)
                  {
                     S9xSetEnvRate(ch, ch->decay_rate, -1,
                                   (MAX_ENVELOPE_HEIGHT * ch->sustain_level)
                                   >> 3);
                     break;
                  }
                  ch->state = SOUND_SUSTAIN;
                  S9xSetEnvRate(ch, ch->sustain_rate, -1, 0);
               }
               break;

            case SOUND_DECAY:
               while (ch->env_error >= FIXED_POINT)
               {
                  ch->envxx = (ch->envxx >> 8) * 255;
                  ch->env_error -= FIXED_POINT;
               }
               ch->envx = ch->envxx >> ENVX_SHIFT;
               if (ch->envx <= ch->envx_target)
               {
                  if (ch->envx <= 0)
                  {
                     S9xAPUSetEndOfSample(J, ch);
                     goto stereo_exit;
                  }
                  ch->state = SOUND_SUSTAIN;
                  S9xSetEnvRate(ch, ch->sustain_rate, -1, 0);
               }
               break;

            case SOUND_SUSTAIN:
               while (ch->env_error >= FIXED_POINT)
               {
                  ch->envxx = (ch->envxx >> 8) * 255;
                  ch->env_error -= FIXED_POINT;
               }
               ch->envx = ch->envxx >> ENVX_SHIFT;
               if (ch->envx <= 0)
               {
                  S9xAPUSetEndOfSample(J, ch);
                  goto stereo_exit;
               }
               break;

            case SOUND_RELEASE:
               while (ch->env_error >= FIXED_POINT)
               {
                  ch->envxx -= (MAX_ENVELOPE_HEIGHT << ENVX_SHIFT) / 256;
                  ch->env_error -= FIXED_POINT;
               }
               ch->envx = ch->envxx >> ENVX_SHIFT;
               if (ch->envx <= 0)
               {
                  S9xAPUSetEndOfSample(J, ch);
                  goto stereo_exit;
               }
               break;

            case SOUND_INCREASE_LINEAR:
               ch->env_error &= FIXED_POINT_REMAINDER;
               ch->envx += step << 1;
               ch->envxx = ch->envx << ENVX_SHIFT;

               if (ch->envx >= 126)
               {
                  ch->envx = 127;
                  ch->envxx = 127 << ENVX_SHIFT;
                  ch->state = SOUND_GAIN;
                  ch->mode = MODE_GAIN;
                  S9xSetEnvRate(ch, 0, -1, 0);
               }
               break;

            case SOUND_INCREASE_BENT_LINE:
               if (ch->envx >= (MAX_ENVELOPE_HEIGHT * 3) / 4)
               {
                  while (ch->env_error >= FIXED_POINT)
                  {
                     ch->envxx += (MAX_ENVELOPE_HEIGHT << ENVX_SHIFT) / 256;
                     ch->env_error -= FIXED_POINT;
                  }
                  ch->envx = ch->envxx >> ENVX_SHIFT;
               }
               else
               {
                  ch->env_error &= FIXED_POINT_REMAINDER;
                  ch->envx += step << 1;
                  ch->envxx = ch->envx << ENVX_SHIFT;
               }

               if (ch->envx >= 126)
               {
                  ch->envx = 127;
                  ch->envxx = 127 << ENVX_SHIFT;
                  ch->state = SOUND_GAIN;
                  ch->mode = MODE_GAIN;
                  S9xSetEnvRate(ch, 0, -1, 0);
               }
               break;

            case SOUND_DECREASE_LINEAR:
               ch->env_error &= FIXED_POINT_REMAINDER;
               ch->envx -= step << 1;
               ch->envxx = ch->envx << ENVX_SHIFT;
               if (ch->envx <= 0)
               {
                  S9xAPUSetEndOfSample(J, ch);
                  goto stereo_exit;
               }
               break;

            case SOUND_DECREASE_EXPONENTIAL:
               while (ch->env_error >= FIXED_POINT)
               {
                  ch->envxx = (ch->envxx >> 8) * 255;
                  ch->env_error -= FIXED_POINT;
               }
               ch->envx = ch->envxx >> ENVX_SHIFT;
               if (ch->envx <= 0)
               {
                  S9xAPUSetEndOfSample(J, ch);
                  goto stereo_exit;
               }
               break;

            case SOUND_GAIN:
               S9xSetEnvRate(ch, 0, -1, 0);
               break;
            }
            ch-> left_vol_level = (ch->envx * ch->volume_left) / 128;
            ch->right_vol_level = (ch->envx * ch->volume_right) / 128;
            VL = (ch->sample * ch-> left_vol_level) / 128;
            VR = (ch->sample * ch->right_vol_level) / 128;
         }

         ch->count += freq;
         if (ch->count >= FIXED_POINT)
         {
            VL = ch->count >> FIXED_POINT_SHIFT;
            ch->sample_pointer += VL;
            ch->count &= FIXED_POINT_REMAINDER;

            ch->sample = ch->next_sample;
            if (ch->sample_pointer >= SOUND_DECODE_LENGTH)
            {
               if (JUST_PLAYED_LAST_SAMPLE(ch))
               {
                  S9xAPUSetEndOfSample(J, ch);
                  goto stereo_exit;
               }
               do
               {
                  ch->sample_pointer -= SOUND_DECODE_LENGTH;
                  if (ch->last_block)
                  {
                     if (!ch->loop)
                     {
                        ch->sample_pointer = LAST_SAMPLE;
                        ch->next_sample = ch->sample;
                        break;
                     }
                     else
                     {
                        S9xAPUSetEndX(J);
                        ch->last_block = false;
                        uint8_t* dir = S9xGetSampleAddress(ch->sample_number);
                        ch->block_pointer = READ_WORD(dir + 2);
                     }
                  }
                  DecodeBlock(ch);
               }
               while (ch->sample_pointer >= SOUND_DECODE_LENGTH);
               if (!JUST_PLAYED_LAST_SAMPLE(ch))
                  ch->next_sample = ch->block [ch->sample_pointer];
            }
            else
               ch->next_sample = ch->block [ch->sample_pointer];

            if (ch->type == SOUND_SAMPLE)
            {
               if (Settings.InterpolatedSound && freq < FIXED_POINT && !mod)
               {
                  ch->interpolate = ((ch->next_sample - ch->sample) *
                                     (long) freq) / (long) FIXED_POINT;
                  ch->sample = (int16_t)(ch->sample + (((ch->next_sample - ch->sample) *
                                                      (long)(ch->count)) / (long) FIXED_POINT));
               }
               else
                  ch->interpolate = 0;
            }
            else
            {
               // Snes9x 1.53's SPC_DSP.cpp, by blargg
               int feedback = (noise_gen << 13) ^ (noise_gen << 14);
               noise_gen = (feedback & 0x4000) ^ (noise_gen >> 1);
               ch->sample = (noise_gen << 17) >> 17;
               ch->interpolate = 0;
            }

            VL = (ch->sample * ch-> left_vol_level) / 128;
            VR = (ch->sample * ch->right_vol_level) / 128;
         }
         else
         {
            if (ch->interpolate)
            {
               int32_t s = (int32_t) ch->sample + ch->interpolate;

               CLIP16(s);
               ch->sample = (int16_t) s;
               VL = (ch->sample * ch-> left_vol_level) / 128;
               VR = (ch->sample * ch->right_vol_level) / 128;
            }
         }

         if (pitch_mod & (1 << (J + 1)))
            wave [I / 2] = ch->sample * ch->envx;

         MixBuffer [I    ] += VL;
         MixBuffer [I + 1] += VR;
         ch->echo_buf_ptr [I    ] += VL;
         ch->echo_buf_ptr [I + 1] += VR;
      }
stereo_exit:
      ;
   }
}

#ifdef __DJGPP
END_OF_FUNCTION(MixStereo);
#endif

#ifdef __sun
extern uint8_t int2ulaw(int);
#endif

// For backwards compatibility with older port specific code
void S9xMixSamplesO(uint8_t* buffer, int sample_count, int byte_offset)
{
   S9xMixSamples(buffer + byte_offset, sample_count);
}
#ifdef __DJGPP
END_OF_FUNCTION(S9xMixSamplesO);
#endif

void S9xMixSamples(uint8_t* buffer, int sample_count)
{
   int J;
   int I;

   if (SoundData.echo_enable)
      memset(EchoBuffer, 0, sample_count * sizeof(EchoBuffer [0]));
   memset(MixBuffer, 0, sample_count * sizeof(MixBuffer [0]));
   MixStereo(sample_count);

   /* Mix and convert waveforms */
   int byte_count = sample_count << 1;

   if (SoundData.echo_enable && SoundData.echo_buffer_size)
   {
      // 16-bit stereo sound with echo enabled ...
      if (FilterTapDefinitionBitfield == 0)
      {
         // ... but no filter defined.
         for (J = 0; J < sample_count; J++)
         {
            int E = Echo [SoundData.echo_ptr];

            Echo [SoundData.echo_ptr] = (E * SoundData.echo_feedback) / 128 +
                                        EchoBuffer [J];

            if ((SoundData.echo_ptr += 1) >= SoundData.echo_buffer_size)
               SoundData.echo_ptr = 0;

            I = (MixBuffer [J] *
                 SoundData.master_volume [J & 1] +
                 E * SoundData.echo_volume [J & 1]) / VOL_DIV16;

            CLIP16(I);
            ((int16_t*) buffer)[J] = I;
         }
      }
      else
      {
         // ... with filter defined.
         for (J = 0; J < sample_count; J++)
         {
            int E = Echo [SoundData.echo_ptr];

            Loop [(Z - 0) & 15] = E;
            E =  E                    * FilterTaps [0];
            if (FilterTapDefinitionBitfield & 0x02) E += Loop [(Z -  2) & 15] *
                     FilterTaps [1];
            if (FilterTapDefinitionBitfield & 0x04) E += Loop [(Z -  4) & 15] *
                     FilterTaps [2];
            if (FilterTapDefinitionBitfield & 0x08) E += Loop [(Z -  6) & 15] *
                     FilterTaps [3];
            if (FilterTapDefinitionBitfield & 0x10) E += Loop [(Z -  8) & 15] *
                     FilterTaps [4];
            if (FilterTapDefinitionBitfield & 0x20) E += Loop [(Z - 10) & 15] *
                     FilterTaps [5];
            if (FilterTapDefinitionBitfield & 0x40) E += Loop [(Z - 12) & 15] *
                     FilterTaps [6];
            if (FilterTapDefinitionBitfield & 0x80) E += Loop [(Z - 14) & 15] *
                     FilterTaps [7];
            E /= 128;
            Z++;

            Echo [SoundData.echo_ptr] = (E * SoundData.echo_feedback) / 128 +
                                        EchoBuffer [J];

            if ((SoundData.echo_ptr += 1) >= SoundData.echo_buffer_size)
               SoundData.echo_ptr = 0;

            I = (MixBuffer [J] *
                 SoundData.master_volume [J & 1] +
                 E * SoundData.echo_volume [J & 1]) / VOL_DIV16;

            CLIP16(I);
            ((int16_t*) buffer)[J] = I;
         }
      }
   }
   else
   {
      // 16-bit mono or stereo sound, no echo
      for (J = 0; J < sample_count; J++)
      {
         I = (MixBuffer [J] *
              SoundData.master_volume [J & 1]) / VOL_DIV16;

         CLIP16(I);
         ((int16_t*) buffer)[J] = I;
      }
   }

}

#ifdef __DJGPP
END_OF_FUNCTION(S9xMixSamples);
#endif

void S9xResetSound(bool full)
{
   int i;
   for (i = 0; i < 8; i++)
   {
      SoundData.channels[i].state = SOUND_SILENT;
      SoundData.channels[i].mode = MODE_NONE;
      SoundData.channels[i].type = SOUND_SAMPLE;
      SoundData.channels[i].volume_left = 0;
      SoundData.channels[i].volume_right = 0;
      SoundData.channels[i].hertz = 0;
      SoundData.channels[i].count = 0;
      SoundData.channels[i].loop = false;
      SoundData.channels[i].envx_target = 0;
      SoundData.channels[i].env_error = 0;
      SoundData.channels[i].erate = 0;
      SoundData.channels[i].envx = 0;
      SoundData.channels[i].envxx = 0;
      SoundData.channels[i].left_vol_level = 0;
      SoundData.channels[i].right_vol_level = 0;
      SoundData.channels[i].direction = 0;
      SoundData.channels[i].attack_rate = 0;
      SoundData.channels[i].decay_rate = 0;
      SoundData.channels[i].sustain_rate = 0;
      SoundData.channels[i].release_rate = 0;
      SoundData.channels[i].sustain_level = 0;
      SoundData.echo_ptr = 0;
      SoundData.echo_feedback = 0;
      SoundData.echo_buffer_size = 1;
   }
   FilterTaps [0] = 127;
   FilterTaps [1] = 0;
   FilterTaps [2] = 0;
   FilterTaps [3] = 0;
   FilterTaps [4] = 0;
   FilterTaps [5] = 0;
   FilterTaps [6] = 0;
   FilterTaps [7] = 0;
   FilterTapDefinitionBitfield = 0;
   noise_gen = 1;
   so.sound_switch = 255;
   so.samples_mixed_so_far = 0;
   so.play_position = 0;
   so.err_counter = 0;

   if (full)
   {
      SoundData.echo_enable = 0;
      SoundData.echo_write_enabled = 0;
      SoundData.echo_channel_enable = 0;
      SoundData.pitch_mod = 0;
      SoundData.dummy[0] = 0;
      SoundData.dummy[1] = 0;
      SoundData.dummy[2] = 0;
      SoundData.master_volume[0] = 0;
      SoundData.master_volume[1] = 0;
      SoundData.echo_volume[0] = 0;
      SoundData.echo_volume[1] = 0;
      SoundData.noise_hertz = 0;
   }

   SoundData.master_volume [0] = SoundData.master_volume [1] = 127;
   if (so.playback_rate)
      so.err_rate = (uint32_t)(FIXED_POINT * SNES_SCANLINE_TIME /
                             (1.0 / so.playback_rate));
   else
      so.err_rate = 0;
}

void S9xSetPlaybackRate(uint32_t playback_rate)
{
   so.playback_rate = playback_rate;
   so.err_rate = (uint32_t)(SNES_SCANLINE_TIME * FIXED_POINT / (1.0 /
                          (double) so.playback_rate));
   S9xSetEchoDelay(APU.DSP [APU_EDL] & 0xf);
   int i;
   for (i = 0; i < 8; i++)
      S9xSetSoundFrequency(i, SoundData.channels [i].hertz);
}

bool S9xInitSound(int mode, bool stereo, int buffer_size)
{
   so.sound_fd = -1;
   so.sound_switch = 255;

   so.playback_rate = 0;
   so.buffer_size = 0;
   so.encoded = false;

   if (!(mode & 7))
      return (1);

   return (1);
}

bool S9xSetSoundMode(int channel, int mode)
{
   Channel* ch = &SoundData.channels[channel];

   switch (mode)
   {
   case MODE_RELEASE:
      if (ch->mode != MODE_NONE)
      {
         ch->mode = MODE_RELEASE;
         return (true);
      }
      break;

   case MODE_DECREASE_LINEAR:
   case MODE_DECREASE_EXPONENTIAL:
   case MODE_GAIN:
      if (ch->mode != MODE_RELEASE)
      {
         ch->mode = mode;
         if (ch->state != SOUND_SILENT)
            ch->state = mode;

         return (true);
      }
      break;

   case MODE_INCREASE_LINEAR:
   case MODE_INCREASE_BENT_LINE:
      if (ch->mode != MODE_RELEASE)
      {
         ch->mode = mode;
         if (ch->state != SOUND_SILENT)
            ch->state = mode;

         return (true);
      }
      break;

   case MODE_ADSR:
      if (ch->mode == MODE_NONE || ch->mode == MODE_ADSR)
      {
         ch->mode = mode;
         return (true);
      }
   }

   return (false);
}

void S9xSetSoundControl(int sound_switch)
{
   so.sound_switch = sound_switch;
}

void S9xPlaySample(int channel)
{
   Channel* ch = &SoundData.channels[channel];

   ch->state = SOUND_SILENT;
   ch->mode = MODE_NONE;
   ch->envx = 0;
   ch->envxx = 0;

   S9xFixEnvelope(channel,
                  APU.DSP [APU_GAIN  + (channel << 4)],
                  APU.DSP [APU_ADSR1 + (channel << 4)],
                  APU.DSP [APU_ADSR2 + (channel << 4)]);

   ch->sample_number = APU.DSP [APU_SRCN + channel * 0x10];
   if (APU.DSP [APU_NON] & (1 << channel))
      ch->type = SOUND_NOISE;
   else
      ch->type = SOUND_SAMPLE;

   S9xSetSoundFrequency(channel, ch->hertz);
   ch->loop = false;
   ch->needs_decode = true;
   ch->last_block = false;
   ch->previous [0] = ch->previous[1] = 0;
   uint8_t* dir = S9xGetSampleAddress(ch->sample_number);
   ch->block_pointer = READ_WORD(dir);
   ch->sample_pointer = 0;
   ch->env_error = 0;
   ch->next_sample = 0;
   ch->interpolate = 0;
   switch (ch->mode)
   {
   case MODE_ADSR:
      if (ch->attack_rate == 0)
      {
         if (ch->decay_rate == 0 || ch->sustain_level == 8)
         {
            ch->state = SOUND_SUSTAIN;
            ch->envx = (MAX_ENVELOPE_HEIGHT * ch->sustain_level) >> 3;
            S9xSetEnvRate(ch, ch->sustain_rate, -1, 0);
         }
         else
         {
            ch->state = SOUND_DECAY;
            ch->envx = MAX_ENVELOPE_HEIGHT;
            S9xSetEnvRate(ch, ch->decay_rate, -1,
                          (MAX_ENVELOPE_HEIGHT * ch->sustain_level) >> 3);
         }
         ch-> left_vol_level = (ch->envx * ch->volume_left) / 128;
         ch->right_vol_level = (ch->envx * ch->volume_right) / 128;
      }
      else
      {
         ch->state = SOUND_ATTACK;
         ch->envx = 0;
         ch->left_vol_level = 0;
         ch->right_vol_level = 0;
         S9xSetEnvRate(ch, ch->attack_rate, 1, MAX_ENVELOPE_HEIGHT);
      }
      ch->envxx = ch->envx << ENVX_SHIFT;
      break;

   case MODE_GAIN:
      ch->state = SOUND_GAIN;
      break;

   case MODE_INCREASE_LINEAR:
      ch->state = SOUND_INCREASE_LINEAR;
      break;

   case MODE_INCREASE_BENT_LINE:
      ch->state = SOUND_INCREASE_BENT_LINE;
      break;

   case MODE_DECREASE_LINEAR:
      ch->state = SOUND_DECREASE_LINEAR;
      break;

   case MODE_DECREASE_EXPONENTIAL:
      ch->state = SOUND_DECREASE_EXPONENTIAL;
      break;

   default:
      break;
   }

   S9xFixEnvelope(channel,
                  APU.DSP [APU_GAIN  + (channel << 4)],
                  APU.DSP [APU_ADSR1 + (channel << 4)],
                  APU.DSP [APU_ADSR2 + (channel << 4)]);
}


#endif
