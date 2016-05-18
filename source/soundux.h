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

#ifndef _SOUND_H_
#define _SOUND_H_

enum { SOUND_SAMPLE = 0, SOUND_NOISE, SOUND_EXTRA_NOISE, SOUND_MUTE };
enum { SOUND_SILENT, SOUND_ATTACK, SOUND_DECAY, SOUND_SUSTAIN,
       SOUND_RELEASE, SOUND_GAIN, SOUND_INCREASE_LINEAR,
       SOUND_INCREASE_BENT_LINE, SOUND_DECREASE_LINEAR,
       SOUND_DECREASE_EXPONENTIAL
     };

enum { MODE_NONE = SOUND_SILENT, MODE_ADSR, MODE_RELEASE = SOUND_RELEASE,
       MODE_GAIN, MODE_INCREASE_LINEAR, MODE_INCREASE_BENT_LINE,
       MODE_DECREASE_LINEAR, MODE_DECREASE_EXPONENTIAL
     };

#define MAX_ENVELOPE_HEIGHT 127
#define ENVELOPE_SHIFT 7
#define MAX_VOLUME 127
#define VOLUME_SHIFT 7
#define VOL_DIV 128
#define SOUND_DECODE_LENGTH 16

#define NUM_CHANNELS    8
#define SOUND_BUFFER_SIZE (1024 * 16)
#define MAX_BUFFER_SIZE SOUND_BUFFER_SIZE
#define SOUND_BUFFER_SIZE_MASK (SOUND_BUFFER_SIZE - 1)

#define SOUND_BUFS      4

#ifdef __sgi
#  include <audio.h>
#endif /* __sgi */

typedef struct
{
   int sound_fd;
   int sound_switch;
   int playback_rate;
   int buffer_size;
   bool encoded;
#ifdef __sun
   int last_eof;
#endif
#ifdef __sgi
   ALport al_port;
#endif /* __sgi */
   int32_t  samples_mixed_so_far;
   int32_t  play_position;
   uint32_t err_counter;
   uint32_t err_rate;
} SoundStatus;

SoundStatus so;


typedef struct
{
   int state;
   int type;
   int16_t volume_left;
   int16_t volume_right;
   uint32_t hertz;
   uint32_t frequency;
   uint32_t count;
   bool loop;
   int envx;
   int16_t left_vol_level;
   int16_t right_vol_level;
   int16_t envx_target;
   uint32_t env_error;
   uint32_t erate;
   int direction;
   uint32_t attack_rate;
   uint32_t decay_rate;
   uint32_t sustain_rate;
   uint32_t release_rate;
   uint32_t sustain_level;
   int16_t sample;
   int16_t decoded [16];
   int16_t previous16 [2];
   int16_t* block;
   uint16_t sample_number;
   bool last_block;
   bool needs_decode;
   uint32_t block_pointer;
   uint32_t sample_pointer;
   int* echo_buf_ptr;
   int mode;
   int32_t envxx;
   int16_t next_sample;
   int32_t interpolate;
   int32_t previous [2];
   // Just incase they are needed in the future, for snapshot compatibility.
   uint32_t dummy [8];
} Channel;

typedef struct
{
   int echo_enable;
   int echo_feedback; /* range is -128 .. 127 */
   int echo_ptr;
   int echo_buffer_size;
   int echo_write_enabled;
   int echo_channel_enable;
   int pitch_mod;
   // Just incase they are needed in the future, for snapshot compatibility.
   uint32_t dummy [3];
   Channel channels [NUM_CHANNELS];
   // bool no_filter;
   int16_t master_volume [2]; /* range is -128 .. 127 */
   int16_t echo_volume [2]; /* range is -128 .. 127 */
   int noise_hertz;
} SSoundData;

SSoundData SoundData;

void S9xSetEightBitConsoleSound(bool Enabled);

void S9xSetSoundVolume(int channel, int16_t volume_left, int16_t volume_right);
void S9xSetSoundFrequency(int channel, int hertz);
void S9xSetSoundHertz(int channel, int hertz);
void S9xSetSoundType(int channel, int type_of_sound);
void S9xSetMasterVolume(int16_t master_volume_left, int16_t master_volume_right);
void S9xSetEchoVolume(int16_t echo_volume_left, int16_t echo_volume_right);
void S9xSetSoundControl(int sound_switch);
void S9xSetEnvelopeHeight(int channel, int height);
void S9xSetSoundADSR(int channel, int attack, int decay, int sustain,
                     int sustain_level, int release);
void S9xSetSoundKeyOff(int channel);
void S9xSetSoundDecayMode(int channel);
void S9xSetSoundAttachMode(int channel);
void S9xSoundStartEnvelope(Channel*);
void S9xSetSoundSample(int channel, uint16_t sample_number);
void S9xSetEchoFeedback(int echo_feedback);
void S9xSetEchoEnable(uint8_t byte);
void S9xSetEchoDelay(int byte);
void S9xSetEchoWriteEnable(uint8_t byte);
void S9xSetFilterCoefficient(int tap, int value);
void S9xSetFrequencyModulationEnable(uint8_t byte);
void S9xSetEnvelopeRate(int channel, uint32_t rate, int direction,
                        int target);
bool S9xSetSoundMode(int channel, int mode);
int S9xGetEnvelopeHeight(int channel);
void S9xResetSound(bool full);
void S9xFixSoundAfterSnapshotLoad();
void S9xPlaybackSoundSetting(int channel);
void S9xPlaySample(int channel);
void S9xFixEnvelope(int channel, uint8_t gain, uint8_t adsr1, uint8_t adsr2);
void S9xStartSample(int channel);

void S9xMixSamples(uint8_t* buffer, int sample_count);
void S9xMixSamplesO(uint8_t* buffer, int sample_count, int byte_offset);
bool S9xOpenSoundDevice(int, bool, int);
void S9xSetPlaybackRate(uint32_t rate);
#endif

#endif
