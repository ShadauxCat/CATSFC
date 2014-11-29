
#include <stdio.h>

#include "snes9x.h"
#include "soundux.h"
#include "memmap.h"
#include "apu.h"
#include "cheats.h"
#include "display.h"
#include "gfx.h"
#include "cpuexec.h"
#include "spc7110.h"
#include "srtc.h"
#include "sa1.h"

#ifdef PSP
#include <pspkernel.h>
#include <pspgu.h>
#endif

#include <libretro.h>


static retro_log_printf_t log_cb = NULL;
static retro_video_refresh_t video_cb = NULL;
static retro_input_poll_t poll_cb = NULL;
static retro_input_state_t input_cb = NULL;
static retro_audio_sample_batch_t audio_batch_cb = NULL;
static retro_environment_t environ_cb = NULL;
struct retro_perf_callback perf_cb;

static float samples_per_frame = 0.0;


#ifdef PERF_TEST
#define RETRO_PERFORMANCE_INIT(name) \
   retro_perf_tick_t current_ticks;\
   static struct retro_perf_counter name = {#name};\
   if (!name.registered) perf_cb.perf_register(&(name));\
   current_ticks = name.total

#define RETRO_PERFORMANCE_START(name) perf_cb.perf_start(&(name))
#define RETRO_PERFORMANCE_STOP(name) \
   perf_cb.perf_stop(&(name));\
   current_ticks = name.total - current_ticks;
#else
#define RETRO_PERFORMANCE_INIT(name)
#define RETRO_PERFORMANCE_START(name)
#define RETRO_PERFORMANCE_STOP(name)
#endif

void retro_set_environment(retro_environment_t cb)
{
   struct retro_log_callback log;

   environ_cb = cb;

   if (environ_cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &log))
      log_cb = log.log;
   else
      log_cb = NULL;

   environ_cb(RETRO_ENVIRONMENT_GET_PERF_INTERFACE, &perf_cb);

}


void retro_set_video_refresh(retro_video_refresh_t cb)
{
   video_cb = cb;
}

void retro_set_audio_sample(retro_audio_sample_t cb)
{}

void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb)
{
   audio_batch_cb = cb;
}

void retro_set_input_poll(retro_input_poll_t cb)
{
   poll_cb = cb;
}

void retro_set_input_state(retro_input_state_t cb)
{
   input_cb = cb;
}

void retro_set_controller_port_device(unsigned port, unsigned device) {}

unsigned retro_api_version()
{
   return RETRO_API_VERSION;
}

void S9xMessage(int type, int number, const char* message)
{
#if 1
#define MAX_MESSAGE_LEN (36 * 3)

   static char buffer [MAX_MESSAGE_LEN + 1];

   printf("%s\n", message);
   strncpy(buffer, message, MAX_MESSAGE_LEN);
   buffer [MAX_MESSAGE_LEN] = 0;
   S9xSetInfoString(buffer);
#endif
}

void S9xDeinitDisplay(void)
{
#ifdef DS2_DMA
   if (GFX.Screen_buffer) AlignedFree(GFX.Screen, PtrAdj.GFXScreen);
#else
   if (GFX.Screen_buffer) free(GFX.Screen_buffer);
#endif
   if (GFX.SubScreen_buffer) free(GFX.SubScreen_buffer);
   if (GFX.ZBuffer_buffer) free(GFX.ZBuffer_buffer);
   if (GFX.SubZBuffer_buffer) free(GFX.SubZBuffer_buffer);

   GFX.Screen = NULL;
   GFX.Screen_buffer = NULL;
   GFX.SubScreen = NULL;
   GFX.SubScreen_buffer = NULL;
   GFX.ZBuffer = NULL;
   GFX.ZBuffer_buffer = NULL;
   GFX.SubZBuffer = NULL;
   GFX.SubZBuffer_buffer = NULL;

}

void S9xInitDisplay(void)
{
   int h = IMAGE_HEIGHT;
   const int safety = 32;

   GFX.Pitch = IMAGE_WIDTH * 2;
#ifdef DS2_DMA
   GFX.Screen_buffer = (unsigned char*) AlignedMalloc(GFX.Pitch * h + safety, 32,
                       &PtrAdj.GFXScreen);
#else
   GFX.Screen_buffer = (unsigned char*) malloc(GFX.Pitch * h + safety);
#endif
   GFX.SubScreen_buffer = (unsigned char*) malloc(GFX.Pitch * h + safety);
   GFX.ZBuffer_buffer = (unsigned char*) malloc((GFX.Pitch >> 1) * h + safety);
   GFX.SubZBuffer_buffer = (unsigned char*) malloc((GFX.Pitch >> 1) * h + safety);

   GFX.Screen = GFX.Screen_buffer + safety;
   GFX.SubScreen = GFX.SubScreen_buffer + safety;
   GFX.ZBuffer = GFX.ZBuffer_buffer + safety;
   GFX.SubZBuffer = GFX.SubZBuffer_buffer + safety;


   GFX.Delta = (GFX.SubScreen - GFX.Screen) >> 1;
}

const char* S9xBasename(const char* f)
{
   const char* p;
   if ((p = strrchr(f, '/')) != NULL || (p = strrchr(f, '\\')) != NULL)
      return (p + 1);

   return (f);
}

bool S9xInitUpdate()
{
   //   IPPU.RenderThisFrame = 0;
   //   video_cb(dummy_frame,256,224,512);
   //   return (false);

   return (true);
}
#ifndef __WIN32__
void _makepath(char* path, const char* drive, const char* dir,
               const char* fname, const char* ext)
{
   if (dir && *dir)
   {
      strcpy(path, dir);
      strcat(path, "/");
   }
   else
      *path = 0;
   strcat(path, fname);
   if (ext && *ext)
   {
      strcat(path, ".");
      strcat(path, ext);
   }
}

void _splitpath(const char* path, char* drive, char* dir, char* fname,
                char* ext)
{
   *drive = 0;

   char* slash = strrchr((char*)path, '/');
   if (!slash)
      slash = strrchr((char*)path, '\\');

   char* dot = strrchr((char*)path, '.');

   if (dot && slash && dot < slash)
      dot = NULL;

   if (!slash)
   {
      strcpy(dir, "");
      strcpy(fname, path);
      if (dot)
      {
         * (fname + (dot - path)) = 0;
         strcpy(ext, dot + 1);
      }
      else
         strcpy(ext, "");
   }
   else
   {
      strcpy(dir, path);
      * (dir + (slash - path)) = 0;
      strcpy(fname, slash + 1);
      if (dot)
      {
         * (fname + (dot - slash) - 1) = 0;
         strcpy(ext, dot + 1);
      }
      else
         strcpy(ext, "");
   }
}
#endif
const char* S9xGetFilename(const char* ex)
{
   static char filename [PATH_MAX + 1];
   char drive [_MAX_DRIVE + 1];
   char dir [_MAX_DIR + 1];
   char fname [_MAX_FNAME + 1];
   char ext [_MAX_EXT + 1];
   _splitpath(Memory.ROMFilename, drive, dir, fname, ext);
   _makepath(filename, drive, dir, fname, ex);

   return (filename);
}


void init_sfc_setting(void)
{
   memset(&Settings, 0, sizeof(Settings));
   Settings.JoystickEnabled = false;
   Settings.SoundPlaybackRate = 32000; // -> ds2sound.h for defs
   Settings.SoundBufferSize = 512;
   Settings.CyclesPercentage = 100;

   Settings.DisableSoundEcho = false;
   Settings.InterpolatedSound = true;
   Settings.APUEnabled = Settings.NextAPUEnabled = true;

   Settings.H_Max = SNES_CYCLES_PER_SCANLINE;
   Settings.SkipFrames = AUTO_FRAMERATE;
   Settings.ShutdownMaster = true;
   Settings.FrameTimePAL = 20000;
   Settings.FrameTimeNTSC = 16667;
   Settings.DisableMasterVolume = false;
   Settings.Mouse = true;
   Settings.SuperScope = true;
   Settings.MultiPlayer5 = true;
   Settings.ControllerOption = SNES_JOYPAD;

   Settings.Transparency = true;
   Settings.SupportHiRes = true;
   Settings.ThreadSound = false;
#ifdef USE_BLARGG_APU
   Settings.SoundSync = false;
#else
   Settings.ApplyCheats = true;
#endif
   Settings.StretchScreenshots = 1;

   Settings.HBlankStart = (256 * Settings.H_Max) / SNES_HCOUNTER_MAX;

}

void S9xAutoSaveSRAM()
{
   SaveSRAM(S9xGetFilename("srm"));
}

#ifdef USE_BLARGG_APU
static void S9xAudioCallback()
{
   size_t avail;
   /* Just pick a big buffer. We won't use it all. */
   static int16_t audio_buf[0x10000];

   S9xFinalizeSamples();
   avail = S9xGetSampleCount();
   S9xMixSamples(audio_buf, avail);
   audio_batch_cb(audio_buf, avail >> 1);
}
#endif

void retro_init(void)
{
   struct retro_log_callback log;
   enum retro_pixel_format rgb565;

   if (environ_cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &log))
      log_cb = log.log;
   else
      log_cb = NULL;

   rgb565 = RETRO_PIXEL_FORMAT_RGB565;
   if (environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &rgb565) && log_cb)
      log_cb(RETRO_LOG_INFO,
             "Frontend supports RGB565 - will use that instead of XRGB1555.\n");

   init_sfc_setting();
   S9xInitMemory();
   S9xInitAPU();
   S9xInitDisplay();
   S9xInitGFX();
#ifdef USE_BLARGG_APU
   S9xInitSound(16, 0);
   S9xSetSamplesAvailableCallback(S9xAudioCallback);
#else
   S9xInitSound(Settings.SoundPlaybackRate,
                true,
                Settings.SoundBufferSize);
#endif

}

void retro_deinit(void)
{

   if (Settings.SPC7110)
      (*CleanUp7110)();

   SaveSRAM(S9xGetFilename("srm"));

   S9xDeinitGFX();
   S9xDeinitDisplay();
   S9xDeinitAPU();
   S9xDeinitMemory();

#ifdef PERF_TEST
   perf_cb.perf_log();
#endif


}

uint32_t S9xReadJoypad(int port)
{
   static const uint32_t snes_lut[] =
   {
      SNES_B_MASK,
      SNES_Y_MASK,
      SNES_SELECT_MASK,
      SNES_START_MASK,
      SNES_UP_MASK,
      SNES_DOWN_MASK,
      SNES_LEFT_MASK,
      SNES_RIGHT_MASK,
      SNES_A_MASK,
      SNES_X_MASK,
      SNES_TL_MASK,
      SNES_TR_MASK
   };

   int i;
   uint32_t joypad = 0;

   for (i = RETRO_DEVICE_ID_JOYPAD_B; i <= RETRO_DEVICE_ID_JOYPAD_R; i++)
      if (input_cb(port, RETRO_DEVICE_JOYPAD, 0, i))
         joypad |= snes_lut[i];

   return joypad;
}

#ifdef PSP
#define FRAMESKIP
#endif

//#define NO_VIDEO_OUTPUT
static float samples_to_play = 0.0;
void retro_run(void)
{
   int i, port;

#ifdef NO_VIDEO_OUTPUT
   video_cb(NULL, IPPU.RenderedScreenWidth, IPPU.RenderedScreenHeight, GFX.Pitch);
   IPPU.RenderThisFrame = false;
#endif

   poll_cb();

   RETRO_PERFORMANCE_INIT(S9xMainLoop_func);
   RETRO_PERFORMANCE_START(S9xMainLoop_func);
   S9xMainLoop();
   RETRO_PERFORMANCE_STOP(S9xMainLoop_func);

#ifndef USE_BLARGG_APU
   static int16_t audio_buf[2048];

   samples_to_play += samples_per_frame;

   if (samples_to_play > 512)
   {
      S9xMixSamples((void*)audio_buf, ((int)samples_to_play) * 2);
      audio_batch_cb(audio_buf, (int)samples_to_play);
      samples_to_play -= (int)samples_to_play;
   }
#endif

#ifdef  NO_VIDEO_OUTPUT
   return;
#endif

#ifdef FRAMESKIP
   if (IPPU.RenderThisFrame)
   {
#endif

#ifdef PSP
      static unsigned int __attribute__((aligned(16))) d_list[32];
      void* const texture_vram_p = (void*)(0x44200000 - (512 *
                                           512)); // max VRAM address - frame size

      sceKernelDcacheWritebackRange(GFX.Screen,
                                    GFX.Pitch * IPPU.RenderedScreenHeight);

      sceGuStart(GU_DIRECT, d_list);

      sceGuCopyImage(GU_PSM_4444, 0, 0, IPPU.RenderedScreenWidth,
                     IPPU.RenderedScreenHeight, GFX.Pitch >> 1, GFX.Screen, 0,
                     0,
                     512, texture_vram_p);

      sceGuTexSync();
      sceGuTexImage(0, 512, 512, 512, texture_vram_p);
      sceGuTexMode(GU_PSM_5551, 0, 0, GU_FALSE);
      sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGB);
      sceGuDisable(GU_BLEND);

      sceGuFinish();


      video_cb(texture_vram_p, IPPU.RenderedScreenWidth, IPPU.RenderedScreenHeight,
               GFX.Pitch);
#else
      video_cb(GFX.Screen, IPPU.RenderedScreenWidth, IPPU.RenderedScreenHeight,
               GFX.Pitch);
#endif

#ifdef FRAMESKIP
      IPPU.RenderThisFrame = false;
   }
   else
   {
      video_cb(NULL, IPPU.RenderedScreenWidth, IPPU.RenderedScreenHeight, GFX.Pitch);
      IPPU.RenderThisFrame = true;
   }
#endif

   //   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated)
   //      check_variables();

}

void S9xGenerateSound()
{

}


void S9xProcessSound(unsigned int samples)
{

}

void S9xLoadSDD1Data()
{

}

bool S9xReadMousePosition(int which1, int* x, int* y, uint32_t* buttons)
{
   return (false);
}

bool S9xReadSuperScopePosition(int* x, int* y, uint32_t* buttons)
{
   return (true);
}

bool JustifierOffscreen()
{
   return (false);
}

void JustifierButtons(uint32_t* justifiers)
{
}

char* osd_GetPackDir()
{
   static char filename[_MAX_PATH];
   memset(filename, 0, _MAX_PATH);

   char dir [_MAX_DIR + 1];
   char drive [_MAX_DRIVE + 1];
   char name [_MAX_FNAME + 1];
   char ext [_MAX_EXT + 1];
   _splitpath(Memory.ROMFilename, drive, dir, name, ext);
   _makepath(filename, drive, dir, NULL, NULL);


   if (!strncmp((char*)&Memory.ROM [0xffc0], "SUPER POWER LEAG 4   ", 21))
   {
      if (getenv("SPL4PACK"))
         return getenv("SPL4PACK");
      else
         strcat(filename, "/SPL4-SP7");
   }
   else if (!strncmp((char*)&Memory.ROM [0xffc0], "MOMOTETSU HAPPY      ", 21))
   {
      if (getenv("MDHPACK"))
         return getenv("MDHPACK");
      else
         strcat(filename, "/SMHT-SP7");
   }
   else if (!strncmp((char*)&Memory.ROM [0xffc0], "HU TENGAI MAKYO ZERO ", 21))
   {
      if (getenv("FEOEZPACK"))
         return getenv("FEOEZPACK");
      else
         strcat(filename, "/FEOEZSP7");
   }
   else if (!strncmp((char*)&Memory.ROM [0xffc0], "JUMP TENGAIMAKYO ZERO", 21))
   {
      if (getenv("SJNSPACK"))
         return getenv("SJNSPACK");
      else
         strcat(filename, "/SJUMPSP7");
   }
   else strcat(filename, "/MISC-SP7");
   return filename;
}

unsigned retro_get_region(void)
{
   return Settings.PAL ? RETRO_REGION_PAL : RETRO_REGION_NTSC;
}
void retro_get_system_info(struct retro_system_info* info)
{
#ifdef LOAD_FROM_MEMORY_TEST
   info->need_fullpath = false;
#else
   info->need_fullpath = true;
#endif
   info->valid_extensions = "smc|fig|sfc|gd3|gd7|dx2|bsx|swc";
   info->library_version = "v1.36";
   info->library_name = "CATSFC(SNES9x)";
   info->block_extract = false;
}

void retro_get_system_av_info(struct retro_system_av_info* info)
{
   info->geometry.base_width = 256;
   info->geometry.base_height = 224;
   info->geometry.max_width = 512;
   info->geometry.max_height = 512;
   info->geometry.aspect_ratio = 4.0 / 3.0;

   if (!Settings.PAL)
      info->timing.fps = (SNES_CLOCK_SPEED * 6.0 / (SNES_CYCLES_PER_SCANLINE *
                          SNES_MAX_NTSC_VCOUNTER));
   else
      info->timing.fps = (SNES_CLOCK_SPEED * 6.0 / (SNES_CYCLES_PER_SCANLINE *
                          SNES_MAX_PAL_VCOUNTER));

   info->timing.sample_rate = (((SNES_CLOCK_SPEED * 6) / (32 * ONE_APU_CYCLE)));

   // small hack to improve av sync
   // since S9xSetPlaybackRate only accepts integral numbers.

   info->timing.fps = info->timing.fps * 32000 / info->timing.sample_rate;
   info->timing.sample_rate = 32000;
}

void retro_reset(void)
{
   CPU.Flags = 0;
   S9xReset();
}


size_t retro_serialize_size(void)
{
   return sizeof(CPU) + sizeof(ICPU) + sizeof(PPU) + sizeof(DMA) +
          0x10000 + 0x20000 + 0x20000 + 0x8000 +
#ifndef USE_BLARGG_APU
          sizeof(APU) + sizeof(IAPU)
#endif
          + 0x10000 + sizeof(SA1) +
          sizeof(s7r) + sizeof(rtc_f9);
}

bool retro_serialize(void* data, size_t size)
{
   int i;

   S9xUpdateRTC();
   S9xSRTCPreSaveState();
#ifndef USE_BLARGG_APU
   for (i = 0; i < 8; i++)
   {
      SoundData.channels[i].previous16[0] = (int16_t)
                                            SoundData.channels[i].previous[0];
      SoundData.channels[i].previous16[1] = (int16_t)
                                            SoundData.channels[i].previous[1];
   }
#endif
   uint8_t* buffer = data;
   memcpy(buffer, &CPU, sizeof(CPU));
   buffer += sizeof(CPU);
   memcpy(buffer, &ICPU, sizeof(ICPU));
   buffer += sizeof(ICPU);
   memcpy(buffer, &PPU, sizeof(PPU));
   buffer += sizeof(PPU);
   memcpy(buffer, &DMA, sizeof(DMA));
   buffer += sizeof(DMA);
   memcpy(buffer, Memory.VRAM, 0x10000);
   buffer += 0x10000;
   memcpy(buffer, Memory.RAM, 0x20000);
   buffer += 0x20000;
   memcpy(buffer, Memory.SRAM, 0x20000);
   buffer += 0x20000;
   memcpy(buffer, Memory.FillRAM, 0x8000);
   buffer += 0x8000;
#ifndef USE_BLARGG_APU
   memcpy(buffer, &APU, sizeof(APU));
   buffer += sizeof(APU);
   memcpy(buffer, &IAPU, sizeof(IAPU));
   buffer += sizeof(IAPU);
   memcpy(buffer, IAPU.RAM, 0x10000);
   buffer += 0x10000;
#endif

   SA1.Registers.PC = SA1.PC - SA1.PCBase;
   S9xSA1PackStatus();

   memcpy(buffer, &SA1, sizeof(SA1));
   buffer += sizeof(SA1);
   memcpy(buffer, &s7r, sizeof(s7r));
   buffer += sizeof(s7r);
   memcpy(buffer, &rtc_f9, sizeof(rtc_f9));
   buffer += sizeof(rtc_f9);

   return true;
}
bool retro_unserialize(const void* data, size_t size)
{
   const uint8_t* buffer = data;

   if (size != retro_serialize_size())
      return false;

   S9xReset();
#ifndef USE_BLARGG_APU
   uint8_t* IAPU_RAM_current = IAPU.RAM;
#endif
   memcpy(&CPU, buffer, sizeof(CPU));
   buffer += sizeof(CPU);
   memcpy(&ICPU, buffer, sizeof(ICPU));
   buffer += sizeof(ICPU);
   memcpy(&PPU, buffer, sizeof(PPU));
   buffer += sizeof(PPU);
   memcpy(&DMA, buffer, sizeof(DMA));
   buffer += sizeof(DMA);
   memcpy(Memory.VRAM, buffer, 0x10000);
   buffer += 0x10000;
   memcpy(Memory.RAM, buffer, 0x20000);
   buffer += 0x20000;
   memcpy(Memory.SRAM, buffer, 0x20000);
   buffer += 0x20000;
   memcpy(Memory.FillRAM, buffer, 0x8000);
   buffer += 0x8000;
#ifndef USE_BLARGG_APU
   memcpy(&APU, buffer, sizeof(APU));
   buffer += sizeof(APU);
   memcpy(&IAPU, buffer, sizeof(IAPU));
   buffer += sizeof(IAPU);
   IAPU.PC = IAPU_RAM_current + (IAPU.PC - IAPU.RAM);
   IAPU.DirectPage = IAPU_RAM_current + (IAPU.DirectPage - IAPU.RAM);
   IAPU.RAM = IAPU_RAM_current;
   memcpy(IAPU.RAM, buffer, 0x10000);
   buffer += 0x10000;
#endif

   memcpy(&SA1, buffer, sizeof(SA1));
   buffer += sizeof(SA1);
   memcpy(&s7r, buffer, sizeof(s7r));
   buffer += sizeof(s7r);
   memcpy(&rtc_f9, buffer, sizeof(rtc_f9));
   buffer += sizeof(rtc_f9);

   S9xFixSA1AfterSnapshotLoad();
   FixROMSpeed();
   IPPU.ColorsChanged = true;
   IPPU.OBJChanged = true;
   CPU.InDMA = false;
   S9xFixColourBrightness();

   S9xSA1UnpackStatus();
#ifndef USE_BLARGG_APU
   S9xAPUUnpackStatus();
   S9xFixSoundAfterSnapshotLoad();
#endif
   ICPU.ShiftedPB = ICPU.Registers.PB << 16;
   ICPU.ShiftedDB = ICPU.Registers.DB << 16;
   S9xSetPCBase(ICPU.ShiftedPB + ICPU.Registers.PC);
   S9xUnpackStatus();
   S9xFixCycles();
   S9xReschedule();

   return true;
}

void retro_cheat_reset(void)
{

}
void retro_cheat_set(unsigned index, bool enabled, const char* code)
{

}

bool retro_load_game(const struct retro_game_info* game)

{
   CPU.Flags = 0;

#ifdef LOAD_FROM_MEMORY_TEST
   if (!LoadROM(game))
#else
   if (!LoadROM(game->path))
#endif
      return false;

   Settings.FrameTime = (Settings.PAL ? Settings.FrameTimePAL :
                         Settings.FrameTimeNTSC);

   LoadSRAM(S9xGetFilename("srm"));

   struct retro_system_av_info av_info;
   retro_get_system_av_info(&av_info);

   samples_per_frame = av_info.timing.sample_rate / av_info.timing.fps;

#ifdef USE_BLARGG_APU
   Settings.SoundPlaybackRate = av_info.timing.sample_rate;
#else
   S9xSetPlaybackRate(av_info.timing.sample_rate);
#endif

   return true;
}

bool retro_load_game_special(unsigned game_type,
                             const struct retro_game_info* info, size_t num_info)
{
   return false;
}
void retro_unload_game(void)
{

}

void* retro_get_memory_data(unsigned id)
{
   return NULL;
}

size_t retro_get_memory_size(unsigned id)
{
   return 0;
}
