
#include <stdio.h>

#include "snes9x.h"
#include "soundux.h"
#include "memmap.h"
#include "apu.h"
#include "cheats.h"
#include "snapshot.h"
#include "display.h"
#include "gfx.h"
#include "cpuexec.h"
#include "spc7110.h"

#ifdef PSP
#include <psptypes.h>
#else
#ifndef PSP_LEGACY_TYPES_DEFINED
#define PSP_LEGACY_TYPES_DEFINED
typedef  uint8_t           u8;
typedef uint16_t        u16;

typedef uint32_t        u32;
typedef uint64_t        u64;

typedef int8_t          s8;
typedef int16_t            s16;

typedef int32_t            s32;
typedef int64_t            s64;
#endif
#endif

#include <libretro.h>

static retro_log_printf_t log_cb = NULL;
static retro_video_refresh_t video_cb = NULL;
static retro_input_poll_t poll_cb = NULL;
static retro_input_state_t input_cb = NULL;
static retro_audio_sample_batch_t audio_batch_cb = NULL;
static retro_environment_t environ_cb = NULL;


struct retro_perf_callback perf_cb;

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



void S9xProcessSound(unsigned int samples);

char* rom_filename = NULL;
char* SDD1_pack = NULL;

/*
 * It is only safe to manipulate saved states between frames.
 */
static bool8 LoadStateNeeded = FALSE;
static bool8 SaveStateNeeded = FALSE;

static u8 Buf[MAX_BUFFER_SIZE];

#define FIXED_POINT 0x10000
#define FIXED_POINT_SHIFT 16
#define FIXED_POINT_REMAINDER 0xffff

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

void S9xExtraUsage()
{
   /*empty*/
}

/*
*   Release display device
*/
void S9xDeinitDisplay(void)
{
#ifdef DS2_DMA
   if (GFX.Screen) AlignedFree(GFX.Screen, PtrAdj.GFXScreen);
#else
   if (GFX.Screen) free(GFX.Screen);
#endif
   if (GFX.SubScreen) free(GFX.SubScreen);
   if (GFX.ZBuffer) free(GFX.ZBuffer);
   if (GFX.SubZBuffer) free(GFX.SubZBuffer);
}

void S9xInitDisplay(void)
{
   int h = IMAGE_HEIGHT;

   GFX.Pitch = IMAGE_WIDTH * 2;
#ifdef DS2_DMA
   GFX.Screen = (unsigned char*) AlignedMalloc(GFX.Pitch * h, 32,
                &PtrAdj.GFXScreen);
#else
   GFX.Screen = (unsigned char*) malloc(GFX.Pitch * h);
#endif
   GFX.SubScreen = (unsigned char*) malloc(GFX.Pitch * h);
   GFX.ZBuffer = (unsigned char*) malloc((GFX.Pitch >> 1) * h);
   GFX.SubZBuffer = (unsigned char*) malloc((GFX.Pitch >> 1) * h);
   GFX.Delta = (GFX.SubScreen - GFX.Screen) >> 1;
}

void S9xExit()
{
   //  if(Settings.SPC7110)
   //    (*CleanUp7110)();

   //   S9xSetSoundMute (TRUE);
   //    S9xDeinitDisplay ();
   //    Memory.SaveSRAM (S9xGetFilename (".srm"));
   //    // S9xSaveCheatFile (S9xGetFilename (".chb")); // cheat binary file
   //   // Do this when loading a cheat file!
   //    Memory.Deinit ();
   //    S9xDeinitAPU ();

   //   exit(0);
}

const char* S9xBasename(const char* f)
{
   const char* p;
   if ((p = strrchr(f, '/')) != NULL || (p = strrchr(f, '\\')) != NULL)
      return (p + 1);

   return (f);
}

bool8 S9xInitUpdate()
{
   //   IPPU.RenderThisFrame = 0;
   //   video_cb(dummy_frame,256,224,512);
   //   return (FALSE);

   return (TRUE);
}


extern void NDSSFCDrawFrameAntialiased(void* screen_addr);

#ifdef PSP
#include <pspkernel.h>
#include <pspgu.h>
void S9xDeinitUpdate(int width, int height)
{
   static unsigned int __attribute__((aligned(16))) d_list[32];
   void* const texture_vram_p = (void*)(0x44200000 - (512 *
                                        512)); // max VRAM address - frame size

   sceKernelDcacheWritebackRange(GFX.Screen, GFX.Pitch * height);

   sceGuStart(GU_DIRECT, d_list);

   sceGuCopyImage(GU_PSM_4444, 0, 0, width, height, GFX.Pitch >> 1, GFX.Screen, 0,
                  0,
                  512, texture_vram_p);

   sceGuTexSync();
   sceGuTexImage(0, 512, 512, 512, texture_vram_p);
   sceGuTexMode(GU_PSM_5551, 0, 0, GU_FALSE);
   sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGB);
   sceGuDisable(GU_BLEND);

   sceGuFinish();


   video_cb(texture_vram_p, width, height, GFX.Pitch);
}

#else
void S9xDeinitUpdate(int width, int height)
{
   video_cb(GFX.Screen, width, height, GFX.Pitch);
}

#endif

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

void S9xProcessEvents(bool8 block)
{

}

void OutOfMemory()
{
}


const char* S9xGetROMDirectory()
{
   return "./";
}


const char* S9xGetSnapshotDirectory()
{
   return "./";
}

const char* S9xGetFilename(const char* ex)
{
   static char filename [PATH_MAX + 1];
   char drive [_MAX_DRIVE + 1];
   char dir [_MAX_DIR + 1];
   char fname [_MAX_FNAME + 1];
   char ext [_MAX_EXT + 1];

   _splitpath(Memory.ROMFilename, drive, dir, fname, ext);
   strcpy(filename, S9xGetSnapshotDirectory());
   strcat(filename, SLASH_STR);
   strcat(filename, fname);
   strcat(filename, ex);

   return (filename);
}

const char* S9xGetFilenameInc(const char* e)
{
   return e;
#if 0
   static char filename [_MAX_PATH + 1];
   char drive [_MAX_DRIVE + 1];
   char dir [_MAX_DIR + 1];
   char fname [_MAX_FNAME + 1];
   char ext [_MAX_EXT + 1];
   char* ptr;
   struct stat buf;

   if (strlen(S9xGetSnapshotDirectory()))
   {
      _splitpath(Memory.ROMFilename, drive, dir, fname, ext);
      strcpy(filename, S9xGetSnapshotDirectory());
      strcat(filename, "/");
      strcat(filename, fname);
      ptr = filename + strlen(filename);
      strcat(filename, "00/");
      strcat(filename, e);
   }
   else
   {
      _splitpath(Memory.ROMFilename, drive, dir, fname, ext);
      strcat(fname, "00/");
      _makepath(filename, drive, dir, fname, e);
      ptr = strstr(filename, "00/");
   }

   do
   {
      if (++ * (ptr + 2) > '9')
      {
         * (ptr + 2) = '0';
         if (++ * (ptr + 1) > '9')
         {
            * (ptr + 1) = '0';
            if (++*ptr > '9')
               break;
         }
      }
   }
   while (stat(filename, &buf) == 0);

   return (filename);
#endif
}

void S9xInitInputDevices()
{
#ifdef JOYSTICK_SUPPORT
   InitJoysticks();
#endif
}


void init_sfc_setting(void)
{
   ZeroMemory(&Settings, sizeof(Settings));
#ifdef JOYSTICK_SUPPORT
   Settings.JoystickEnabled = TRUE;
#else
   Settings.JoystickEnabled = FALSE;
#endif

   Settings.SoundPlaybackRate = 44100; // -> ds2sound.h for defs
   Settings.SoundBufferSize = 512;
   Settings.CyclesPercentage = 100;
   Settings.DisableSoundEcho = FALSE;
   //sound settings
   Settings.APUEnabled = Settings.NextAPUEnabled = TRUE;
   // Settings.FixFrequency = 1;

   Settings.H_Max = SNES_CYCLES_PER_SCANLINE;
   Settings.SkipFrames = AUTO_FRAMERATE;
   Settings.ShutdownMaster = TRUE;
   Settings.FrameTimePAL = 20000;
   Settings.FrameTimeNTSC = 16667;
   Settings.DisableMasterVolume = FALSE;
   Settings.Mouse = TRUE;
   Settings.SuperScope = TRUE;
   Settings.MultiPlayer5 = TRUE;
   Settings.ControllerOption = SNES_JOYPAD;

   Settings.Transparency = TRUE;
   Settings.SupportHiRes = FALSE;
   Settings.ThreadSound = FALSE;
   Settings.SoundSync = TRUE;
   Settings.ApplyCheats = TRUE;
   Settings.StretchScreenshots = 1;

   Settings.HBlankStart = (256 * Settings.H_Max) / SNES_HCOUNTER_MAX;
}

void S9xAutoSaveSRAM()
{
   SaveSRAM(S9xGetFilename(".srm"));
}

int game_load_state(char* file)
{
   int flag;

   flag = 0;
   if (S9xUnfreezeGame(file) == FALSE)
      flag = -1;

   return flag;
}

int game_save_state(char* file)
{
   int flag;

   flag = 0;
   if (S9xFreezeGame(file) == FALSE)
      flag = -1;

   S9xAutoSaveSRAM();

   return flag;
}

void game_restart(void)
{
   CPU.Flags = 0;
   S9xReset();
}

int load_gamepak(const char* file)
{
   CPU.Flags = 0;
   // mdelay(50); // Delete this delay
   if (!LoadROM(file))
      return -1;
   S9xReset();

   Settings.FrameTime = (Settings.PAL ? Settings.FrameTimePAL :
                         Settings.FrameTimeNTSC);

   LoadSRAM(S9xGetFilename(".srm"));
   S9xLoadCheatFile(
      S9xGetFilename(".chb"));  // cheat binary file, as opposed to text

   return 0;
}

void sfc_main(void);

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

   if (!Init() || !S9xInitAPU())
      OutOfMemory();

   S9xInitDisplay();
   if (!S9xGraphicsInit())
      OutOfMemory();

   S9xInitSound(Settings.SoundPlaybackRate,
                TRUE,
                Settings.SoundBufferSize);

#ifdef JOYSTICK_SUPPORT
   uint32 JoypadSkip = 0;
#endif


}


uint32 S9xReadJoypad(int port)
{
   static const uint32 snes_lut[] =
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
   uint32 joypad = 0;

   for (i = RETRO_DEVICE_ID_JOYPAD_B; i <= RETRO_DEVICE_ID_JOYPAD_R; i++)
      if (input_cb(port, RETRO_DEVICE_JOYPAD, 0, i))
         joypad |= snes_lut[i];

   return joypad;
}

//#define FRAMESKIP
void retro_run(void)
{
   int i, port;

   //   IPPU.RenderThisFrame = FALSE;
   //   video_cb(GFX.Screen, 256, 224, 512);

   poll_cb();

   S9xSetPlaybackRate(32040);
   so.mute_sound = FALSE;
   SoundData.echo_enable = FALSE;

   S9xMainLoop();

   static s16 audio_buf[534 << 1];
   S9xMixSamples((uint8*)audio_buf, 534 << 1);
   audio_batch_cb(audio_buf, 534);


#ifdef FRAMESKIP
   if (IPPU.RenderThisFrame)
      IPPU.RenderThisFrame = false;
   else
   {
      video_cb(NULL, 256, 224, GFX.Pitch);
      IPPU.RenderThisFrame = true;
   }
#endif

   //   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated)
   //      check_variables();

}


static unsigned int sync_last = 0;
static unsigned int sync_next = 0;

static unsigned int skip_rate = 0;

void S9xSyncSpeed()
{
}

bool8 S9xOpenSoundDevice(int mode, bool8 stereo, int buffer_size)
{
   return (TRUE);
}

void S9xGenerateSound()
{
   //   static s16 audio_buf[855 << 1];
   //   S9xMixSamples ((uint8*)audio_buf, 855 << 1);
   //   audio_batch_cb(audio_buf, 855);



}

void S9xGenerateSound0()
{
   int bytes_so_far = so.samples_mixed_so_far << 1;

   if (bytes_so_far >= so.buffer_size)
      return;

   so.err_counter += so.err_rate;
   if (so.err_counter >= FIXED_POINT)
   {
      // Write this many samples overall
      int samples_to_write = so.err_counter >> FIXED_POINT_SHIFT;
      samples_to_write <<= 1;
      int byte_offset = (bytes_so_far + so.play_position) & SOUND_BUFFER_SIZE_MASK;

      so.err_counter &= FIXED_POINT_REMAINDER;

      do
      {
         int bytes_this_run = samples_to_write;
         bytes_this_run <<= 1;

         if (byte_offset + bytes_this_run > SOUND_BUFFER_SIZE)
            bytes_this_run = SOUND_BUFFER_SIZE - byte_offset;

         if (bytes_so_far + bytes_this_run > so.buffer_size)
         {
            bytes_this_run = so.buffer_size - bytes_so_far;
            if (bytes_this_run == 0)
               break;
         }

         int samples_this_run = bytes_this_run;
         samples_this_run >>= 1;

         S9xMixSamples(Buf + byte_offset, samples_this_run);
         so.samples_mixed_so_far += samples_this_run;
         samples_to_write -= samples_this_run;
         bytes_so_far += samples_this_run << 1;
         byte_offset = (byte_offset + bytes_this_run) & SOUND_BUFFER_SIZE_MASK;
      }
      while (samples_to_write > 0);
   }
}


void S9xProcessSound(unsigned int samples)
{

}

/*
const unsigned int keymap[12] = {
      0x80,    //KEY_A
      0x8000,     //KEY_B
      0x2000,     //KEY_SELECT
      0x1000,     //KEY_START
      0x100,      //KEY_RIGHT
      0x200,      //KEY_LEFT
      0x800,      //KEY_UP
      0x400,      //KEY_DOWN
      0x10,    //KEY_R
      0x20,    //KEY_L
      0x40,    //KEY_X
      0x4000      //KEY_Y
   };
*/

#ifdef ACCUMULATE_JOYPAD
// These are kept as DS key bitfields until it's time to send them to Snes9x.
static uint32 PreviousControls = 0x00000000;
static uint32 ControlsPressed  = 0x00000000;
static uint32 ControlsReleased = 0x00000000;

void NDSSFCAccumulateJoypad()
{
   struct key_buf inputdata;
   ds2_getrawInput(&inputdata);

   ControlsPressed |= inputdata.key & ~PreviousControls;
   ControlsReleased |= PreviousControls & ~inputdata.key;
}
#endif // ACCUMULATE_JOYPAD

static int S9xCompareSDD1IndexEntries(const void* p1, const void* p2)
{
   return (* (uint32*) p1 - * (uint32*) p2);
}

void S9xLoadSDD1Data()
{

}

bool8 S9xReadMousePosition(int which1, int* x, int* y, uint32* buttons)
{
   return (FALSE);
}

bool8 S9xReadSuperScopePosition(int* x, int* y, uint32* buttons)
{
   return (TRUE);
}

bool JustifierOffscreen()
{
   return (FALSE);
}

void JustifierButtons(uint32* justifiers)
{
}

char* osd_GetPackDir()
{
   static char filename[_MAX_PATH];
   memset(filename, 0, _MAX_PATH);

   if (strlen(S9xGetSnapshotDirectory()) != 0)
      strcpy(filename, S9xGetSnapshotDirectory());
   else
   {
      char dir [_MAX_DIR + 1];
      char drive [_MAX_DRIVE + 1];
      char name [_MAX_FNAME + 1];
      char ext [_MAX_EXT + 1];
      _splitpath(Memory.ROMFilename, drive, dir, name, ext);
      _makepath(filename, drive, dir, NULL, NULL);
   }

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

void retro_deinit(void)
{
}
unsigned retro_get_region(void)
{
   return Settings.PAL ? RETRO_REGION_PAL : RETRO_REGION_NTSC;
}
void retro_get_system_info(struct retro_system_info* info)
{
   info->need_fullpath = true;
   info->valid_extensions = "smc|fig|sfc|gd3|gd7|dx2|bsx|swc";
   info->library_version = "v1.4";
   info->library_name = "SNES9x(CATSFC)";
   info->block_extract = false;
}
void retro_get_system_av_info(struct retro_system_av_info* info)
{
   info->geometry.base_width = 256;
   info->geometry.base_height = 224;
   info->geometry.max_width = 512;
   info->geometry.max_height = 512;
   info->geometry.aspect_ratio = 4.0 / 3.0;
   //   if (!Settings.PAL)
   //      info->timing.fps = 21477272.0 / 357366.0;
   //   else
   //      info->timing.fps = 21281370.0 / 425568.0;
   //   info->timing.sample_rate = 32040.5;


   info->timing.fps = 60.0;
   info->timing.sample_rate = 32040.0;
}

void retro_reset(void)
{

}


size_t retro_serialize_size(void)
{
   return 0;
}

bool retro_serialize(void* data, size_t size)
{
   return false;
}
bool retro_unserialize(const void* data, size_t size)
{
   return false;
}

void retro_cheat_reset(void)
{

}
void retro_cheat_set(unsigned index, bool enabled, const char* code)
{

}

bool retro_load_game(const struct retro_game_info* game)

{
   LoadROM(game->path);
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
