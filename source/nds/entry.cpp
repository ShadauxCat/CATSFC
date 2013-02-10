//entry.c
#include <stdio.h>

#include "ds2_types.h"
#include "ds2_cpu.h"
#include "ds2_cpuclock.h"
#include "ds2_timer.h"
#include "ds2io.h"
#include "fs_api.h"

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

#include "draw.h"
#include "gui.h"
#include "entry.h"
#include "ds2sound.h"

#ifdef DS2_DMA
#include "ds2_dma.h"
#include "dma_adj.h"
#endif

void S9xProcessSound (unsigned int);

char *rom_filename = NULL;
char *SDD1_pack = NULL;

static u8 Buf[MAX_BUFFER_SIZE];

#define FIXED_POINT 0x10000
#define FIXED_POINT_SHIFT 16
#define FIXED_POINT_REMAINDER 0xffff

void S9xMessage (int /*type*/, int /*number*/, const char *message)
{
#if 1
#define MAX_MESSAGE_LEN (36 * 3)

    static char buffer [MAX_MESSAGE_LEN + 1];

    printf ("%s\n", message);
    strncpy (buffer, message, MAX_MESSAGE_LEN);
    buffer [MAX_MESSAGE_LEN] = 0;
    S9xSetInfoString (buffer);
#endif
}

void S9xExtraUsage ()
{
    /*empty*/
}

/*
*   Release display device
*/
void S9xDeinitDisplay (void)
{
#ifdef DS2_DMA
    if(GFX.Screen) AlignedFree(GFX.Screen, PtrAdj.GFXScreen);
#else
    if(GFX.Screen) free(GFX.Screen);
#endif
    if(GFX.SubScreen) free(GFX.SubScreen);
    if(GFX.ZBuffer) free(GFX.ZBuffer);
    if(GFX.SubZBuffer) free(GFX.SubZBuffer);
}

void S9xInitDisplay (int, char **)
{
    int h = IMAGE_HEIGHT;

	GFX.Pitch = IMAGE_WIDTH * 2;
#ifdef DS2_DMA
	GFX.Screen =    (unsigned char*) AlignedMalloc (GFX.Pitch * h, 32, &PtrAdj.GFXScreen);
#else
	GFX.Screen =	(unsigned char*) malloc (GFX.Pitch * h);
#endif
	GFX.SubScreen = (unsigned char*) malloc (GFX.Pitch * h);
	GFX.ZBuffer = 	(unsigned char*) malloc ((GFX.Pitch >> 1) * h);
	GFX.SubZBuffer =(unsigned char*) malloc ((GFX.Pitch >> 1) * h);
	GFX.Delta = (GFX.SubScreen - GFX.Screen) >> 1;
}

void S9xParseArg (char **argv, int &i, int argc)
{
}

void S9xParseDisplayArg (char **argv, int &ind, int)
{
}

void S9xExit ()
{
  HighFrequencyCPU(); // Crank it up to exit quickly
  if(Settings.SPC7110)
    (*CleanUp7110)();

	S9xSetSoundMute (TRUE);
    S9xDeinitDisplay ();
    Memory.SaveSRAM (S9xGetFilename (".srm"));
    // S9xSaveCheatFile (S9xGetFilename (".chb")); // cheat binary file
	// Do this when loading a cheat file!
    Memory.Deinit ();
    S9xDeinitAPU ();

#ifdef _NETPLAY_SUPPORT
    if (Settings.NetPlay)
	S9xNetPlayDisconnect ();
#endif

	exit(0);
}

const char *S9xBasename (const char *f)
{
    const char *p;
    if ((p = strrchr (f, '/')) != NULL || (p = strrchr (f, '\\')) != NULL)
	return (p + 1);

    return (f);
}

bool8 S9xInitUpdate ()
{
    return (TRUE);
}


extern void NDSSFCDrawFrameAntialiased();
	

bool8 S9xDeinitUpdate (int Width, int Height, bool8 /*sixteen_bit*/)
{
	switch(game_config.graphic)
	{
		//Up
		case 1:
#ifdef DS2_DMA
			__dcache_writeback_all();
			ds2_DMAcopy_32Byte(1 /* channel: graphics */, up_screen_addr, GFX.Screen + 256 * 32 * 2, 256 * 192 * 2);
			ds2_DMA_wait(1);
			ds2_DMA_stop(1);
#else
		    memcpy(up_screen_addr, GFX.Screen+256*32*2, 256*192*2);
#endif
			break;

		//Down
		case 2:
#ifdef DS2_DMA
			__dcache_writeback_all();
			ds2_DMAcopy_32Byte(1 /* channel: graphics */, up_screen_addr, GFX.Screen, 256 * 192 * 2);
			ds2_DMA_wait(1);
			ds2_DMA_stop(1);
#else
		    memcpy(up_screen_addr, GFX.Screen, 256*192*2);
#endif
			break;

		//Both
		case 3:
#ifdef DS2_DMA
			__dcache_writeback_all();
			ds2_DMAcopy_32Byte(1 /* channel: graphics */, up_screen_addr, GFX.Screen + 256 * 16 * 2, 256 * 192 * 2);
			ds2_DMA_wait(1);
			ds2_DMA_stop(1);
#else
		    memcpy(up_screen_addr, GFX.Screen+256*16*2, 256*192*2);
#endif
			break;
			
		case 4:
			NDSSFCDrawFrameAntialiased ();
		break;
		

		default:
		{
#ifdef DS2_DMA
			__dcache_writeback_all();
#endif
			register unsigned char *src, *dst;
			register unsigned int m;

			src = GFX.Screen;
			dst = (unsigned char*)up_screen_addr;
			for(m = 0; m < 32; m++)
			{
#ifdef DS2_DMA
				ds2_DMAcopy_32Byte(1 /* channel: graphics */, dst, src, 256 * 6 * 2);
				ds2_DMA_wait(1);
				ds2_DMA_stop(1);
#else
				memcpy(dst, src, 256*6*2);
#endif
				dst += 256*6*2;
				src += 256*7*2;
			}
		}
			break;
	}

	ds2_flipScreen(UP_SCREEN, UP_SCREEN_UPDATE_METHOD);
	// A problem with update method 1 (wait, double buffer) means that, after
	// about 15 minutes of play time, the screen starts to half-redraw every
	// frame. With update method 0, this is mitigated. (Method 2 is too slow.)

	return (TRUE);
}

void _makepath (char *path, const char *, const char *dir,
		const char *fname, const char *ext)
{
    if (dir && *dir)
    {
	strcpy (path, dir);
	strcat (path, "/");
    }
    else
	*path = 0;
    strcat (path, fname);
    if (ext && *ext)
    {
        strcat (path, ".");
        strcat (path, ext);
    }
}

void _splitpath (const char *path, char *drive, char *dir, char *fname,
		 char *ext)
{
    *drive = 0;

    char *slash = strrchr (path, '/');
    if (!slash)
	slash = strrchr (path, '\\');

    char *dot = strrchr (path, '.');

    if (dot && slash && dot < slash)
	dot = NULL;

    if (!slash)
    {
	strcpy (dir, "");
	strcpy (fname, path);
        if (dot)
        {
	    *(fname + (dot - path)) = 0;
	    strcpy (ext, dot + 1);
        }
	else
	    strcpy (ext, "");
    }
    else
    {
	strcpy (dir, path);
	*(dir + (slash - path)) = 0;
	strcpy (fname, slash + 1);
        if (dot)
	{
	    *(fname + (dot - slash) - 1) = 0;
    	    strcpy (ext, dot + 1);
	}
	else
	    strcpy (ext, "");
    }
}

void S9xProcessEvents (bool8 block)
{

}

void OutOfMemory ()
{
}


const char *S9xGetROMDirectory ()
{
	return ((const char*)g_default_rom_dir);
}


const char *S9xGetSnapshotDirectory ()
{
    return ((const char*)DEFAULT_RTS_DIR);
}

const char *S9xGetFilename (const char *ex)
{
    static char filename [PATH_MAX + 1];
    char drive [_MAX_DRIVE + 1];
    char dir [_MAX_DIR + 1];
    char fname [_MAX_FNAME + 1];
    char ext [_MAX_EXT + 1];

    _splitpath (Memory.ROMFilename, drive, dir, fname, ext);
    strcpy (filename, S9xGetSnapshotDirectory ());
    strcat (filename, SLASH_STR);
    strcat (filename, fname);
    strcat (filename, ex);

    return (filename);
}
 
const char *S9xGetFilenameInc (const char *e)
{
    return e;
#if 0
    static char filename [_MAX_PATH + 1];
    char drive [_MAX_DRIVE + 1];
    char dir [_MAX_DIR + 1];
    char fname [_MAX_FNAME + 1];
    char ext [_MAX_EXT + 1];
    char *ptr;
    struct stat buf;

    if (strlen (S9xGetSnapshotDirectory()))
    {
        _splitpath (Memory.ROMFilename, drive, dir, fname, ext);
        strcpy (filename, S9xGetSnapshotDirectory());
        strcat (filename, "/");
        strcat (filename, fname);
        ptr = filename + strlen (filename);
        strcat (filename, "00/");
        strcat (filename, e);
    }
    else
    {
        _splitpath (Memory.ROMFilename, drive, dir, fname, ext);
        strcat (fname, "00/");
        _makepath (filename, drive, dir, fname, e);
        ptr = strstr (filename, "00/");
    }

    do
    {
        if (++*(ptr + 2) > '9')
        {
            *(ptr + 2) = '0';
            if (++*(ptr + 1) > '9')
            {
                *(ptr + 1) = '0';
                if (++*ptr > '9')
                    break;
            }
        }
    } while( stat(filename, &buf) == 0 );

    return (filename);
#endif
}

void S9xInitInputDevices ()
{
#ifdef JOYSTICK_SUPPORT
    InitJoysticks ();
#endif
}



void game_disableAudio()
{
	if( game_enable_audio == 1)
	{
		S9xSetSoundMute (FALSE);
	}
	else
	{
		S9xSetSoundMute (TRUE);
	}
}

void game_set_frameskip()
{
	if( game_config.frameskip_value == 0)
	{
		Settings.SkipFrames = AUTO_FRAMERATE;
	}
	else
	{
		Settings.SkipFrames = game_config.frameskip_value - 1 /* 1 -> 0 and so on */;
	}
}

void game_set_fluidity()
{
	if( game_config.SoundSync == 1)
	{
		Settings.SoundSync = TRUE;
	}
	else
	{
		Settings.SoundSync = FALSE;
	}
}

void game_set_retro(void)
{
	if (game_config.RetroSound == 1)
	{
		Settings.InterpolatedSound = FALSE;
		S9xSetEightBitConsoleSound (TRUE);
	}
	else
	{
		Settings.InterpolatedSound = TRUE;
		S9xSetEightBitConsoleSound (FALSE);
	}
}
	
void init_sfc_setting(void)
{
    ZeroMemory (&Settings, sizeof (Settings));
#ifdef JOYSTICK_SUPPORT
    Settings.JoystickEnabled = TRUE;
#else
    Settings.JoystickEnabled = FALSE;
#endif

    Settings.SoundPlaybackRate = SNES9X_SRATE_ID;	// -> ds2sound.h for defs
#ifndef FOREVER_STEREO
    Settings.Stereo = TRUE;
#endif
    Settings.SoundBufferSize = DS2_BUFFER_SIZE;
    Settings.CyclesPercentage = 100;
    Settings.DisableSoundEcho = FALSE;
	//sound settings
    Settings.APUEnabled = Settings.NextAPUEnabled = TRUE;
	Settings.FixFrequency = 1;

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
#ifndef FOREVER_16_BIT
    Settings.SixteenBit = TRUE;
#endif
#ifndef FOREVER_16_BIT_SOUND
    Settings.SixteenBitSound = TRUE;
#endif

    Settings.SupportHiRes = FALSE;
    Settings.ThreadSound = FALSE;
	Settings.SoundSync = TRUE;
    Settings.AutoSaveDelay = 0;
#ifdef _NETPLAY_SUPPORT
    Settings.NetPlay = FALSE;
    Settings.ServerName [0] = 0;
    Settings.Port = NP_DEFAULT_PORT;
#endif
    Settings.ApplyCheats = TRUE;
    Settings.TurboMode = FALSE;
    Settings.TurboSkipFrames = 10;
    Settings.StretchScreenshots = 1;

	Settings.HBlankStart = (256 * Settings.H_Max) / SNES_HCOUNTER_MAX;
}

void S9xAutoSaveSRAM ()
{
    Memory.SaveSRAM (S9xGetFilename (".srm"));
}

int game_load_state(char* file)
{
	int flag;

	flag = 0;
	if(S9xUnfreezeGame(file) == FALSE)
		flag = -1;

	return flag;
}

int game_save_state(char* file)
{
	int flag;

	flag = 0;
	if(S9xFreezeGame(file) == FALSE)
		flag = -1;

	S9xAutoSaveSRAM ();

	return flag;
}

void game_restart(void)
{
	CPU.Flags = 0;
	S9xReset ();
}

int load_gamepak(const char* file)
{
	CPU.Flags = 0;
	// ds2_mdelay(50); // Delete this delay
	if (!Memory.LoadROM (file))
		return -1;
	S9xReset ();

	Settings.FrameTime = (Settings.PAL ? Settings.FrameTimePAL : Settings.FrameTimeNTSC);

	Memory.LoadSRAM (S9xGetFilename (".srm"));
	// ds2_mdelay(50); // Delete this delay
	S9xLoadCheatFile (S9xGetFilename (".chb")); // cheat binary file, as opposed to text

#ifdef _NETPLAY_SUPPORT
    if (strlen (Settings.ServerName) == 0)
    {
	char *server = getenv ("S9XSERVER");
	if (server)
	{
	    strncpy (Settings.ServerName, server, 127);
	    Settings.ServerName [127] = 0;
	}
    }
    char *port = getenv ("S9XPORT");
    if (Settings.Port >= 0 && port)
    	Settings.Port = atoi (port);
    else if (Settings.Port < 0)
        Settings.Port = -Settings.Port;

    if (Settings.NetPlay)
    {
	int player;

	if (!S9xNetPlayConnectToServer (Settings.ServerName, Settings.Port,
					Memory.ROMName, player))
	{
	    fprintf (stderr, "Failed to connected to Snes9x netplay"
                     " server \"%s\" on port %d.\n",
                     Settings.ServerName, Settings.Port);
	    S9xExit ();
	}
	fprintf (stderr, "Connected to \"%s\" on port %d as"
                 " player #%d playing \"%s\"\n",
		 Settings.ServerName, Settings.Port, player, Memory.ROMName);
    }
    
#endif
/*
    if (snapshot_filename)
    {
        int Flags = CPU.Flags & (DEBUG_MODE_FLAG | TRACE_FLAG);
        if (!S9xLoadSnapshot (snapshot_filename))
            exit (1);
        CPU.Flags |= Flags;
    }
*/

	// ds2_mdelay(50); // Delete this delay

	return 0;
}

extern "C" int sfc_main (int argc, char **argv);

int sfc_main (int argc, char **argv)
{
	//Initialize GUI
	gui_init(0);

	init_sfc_setting();

    if (!Memory.Init () || !S9xInitAPU())
		OutOfMemory ();

	S9xInitDisplay (argc, argv);
    if (!S9xGraphicsInit())
		OutOfMemory ();

    S9xInitSound (Settings.SoundPlaybackRate,
#ifndef FOREVER_STEREO
                  Settings.Stereo,
#else
                  TRUE,
#endif
                  Settings.SoundBufferSize);
#ifdef GFX_MULTI_FORMAT
//    S9xSetRenderPixelFormat (RGB565);
    S9xSetRenderPixelFormat (BGR555);
#endif

#ifdef JOYSTICK_SUPPORT
    uint32 JoypadSkip = 0;
#endif

	Settings.Paused = 1;
	bool8 FirstInvocation = TRUE;

    while (1)
    {
		if (!Settings.Paused
#ifdef DEBUGGER
			|| (CPU.Flags & (DEBUG_MODE_FLAG | SINGLE_STEP_FLAG)
#endif
			)
			S9xMainLoop ();


#ifdef DEBUGGER
		if (CPU.Flags & DEBUG_MODE_FLAG)
		{
			S9xDoDebug ();
		}
		else
#endif
		if (Settings.Paused)
		{
			S9xSetSoundMute (TRUE);
			unsigned short screen[256*192];

			copy_screen((void*)screen, up_screen_addr, 0, 0, 256, 192);
			menu(screen, FirstInvocation);
			FirstInvocation = FALSE;
			game_disableAudio();
			Settings.Paused = 0;
		}

#ifdef JOYSTICK_SUPPORT
		//if (Settings.JoystickEnabled && (JoypadSkip++ & 1) == 0)
		if (Settings.JoystickEnabled)
			ReadJoysticks ();
#endif

    }

    return (0);
}

static unsigned int sync_last= 0;
static unsigned int sync_next = 0;

static unsigned int skip_rate= 0;

void S9xSyncSpeed ()
{
	uint32 syncnow;
	int32 syncdif;
	unsigned int LastAutoCPUFrequency = AutoCPUFrequency;

#if 0
    if (Settings.SoundSync == 2)
    {
	IPPU.RenderThisFrame = TRUE;
	IPPU.SkippedFrames = 0;
	return;
    }
#endif
	syncnow = getSysTime();

	bool8 FastForward = game_fast_forward || temporary_fast_forward /* hotkey is held */;

	if (FastForward)
	{
		sync_last = syncnow;
		sync_next = syncnow;

		if(++skip_rate < Settings.TurboSkipFrames)
			IPPU.RenderThisFrame = false;
		else
		{
			skip_rate = 0;
			IPPU.RenderThisFrame = true;
		}
	}
	else
	{
		// Manual or automatic frame skipping, no fast-forward.
		if (Settings.SkipFrames == AUTO_FRAMERATE)
		{
			// frame_time is in getSysTime units: 42.667 microseconds.
			int32 frame_time = Settings.PAL ? 468 /* = 20.0 ms */ : 391 /* = 16.67 ms */;
			if (sync_last > syncnow) // Overflow occurred! (every 50 hrs)
			{
				// Render this frame regardless, set the
				// sync_next, and get the hell out.
				IPPU.RenderThisFrame = TRUE;
				sync_last = syncnow;
				sync_next = syncnow + frame_time;
				return;
			}
			sync_last = syncnow;
			// If this is positive, we have syncdif*42.66 microseconds to
			// spare.
			// If this is negative, we're late by syncdif*42.66
			// microseconds.
			syncdif = sync_next - syncnow;
			if(skip_rate < 2 /* did not skip 2 frames yet */)
			{
				// Skip a minimum of 2 frames between rendered frames.
				// This prevents the DSTwo-DS link from being too busy
				// to return button statuses.
				++skip_rate;
				IPPU.RenderThisFrame = FALSE;
				sync_next += frame_time;
			}
			else if(syncdif < 0)
			{
				/*
				 * If we're consistently late, delay up to 8 frames.
				 * 
				 * That really helps with certain games, such as
				 * Super Mario RPG and Yoshi's Island.
				 */
				if(++skip_rate < 10)
				{
					if(syncdif >= -11719 /* not more than 500.0 ms late */)
					{
						IPPU.RenderThisFrame = FALSE;
						sync_next += frame_time;
					}
					else
					{	//lag more than 0.5s, maybe paused
						IPPU.RenderThisFrame = TRUE;
						sync_next = syncnow + frame_time;
					}
				}
				else
				{
					skip_rate = 0;
					IPPU.RenderThisFrame = TRUE;
					sync_next = syncnow + frame_time;
				}
			}
			else // Early
			{
				skip_rate = 0;
				if (syncdif > 0)
				{
					do {
						S9xProcessSound (0);
#ifdef ACCUMULATE_JOYPAD
/*
 * This call allows NDSSFC to synchronise the DS controller more often.
 * If porting a later version of Snes9x into NDSSFC, it is essential to
 * preserve it.
 */
						NDSSFCAccumulateJoypad ();
#endif
						syncdif = sync_next - getSysTime();
					} while (syncdif > 0);
				}

				IPPU.RenderThisFrame = TRUE;
				sync_next += frame_time;
			}
#if 0
			if(++framenum >= 60)
			{
				syncdif = syncnow - sync_last;
				sync_last = syncnow;
				framenum = 0;
				//printf("T %d %d\n", syncdif*42667/1000, realframe);
				realframe = 0;
			}
#endif
		}
		else /* if (Settings.SkipFrames != AUTO_FRAMERATE) */
		{
			// frame_time is in getSysTime units: 42.667 microseconds.
			uint32 frame_time = Settings.PAL ? 468 /* = 20.0 ms */ : 391 /* = 16.67 ms */;
			sync_last = syncnow;
			if (++skip_rate > Settings.SkipFrames)
			{
				skip_rate = 0;
				IPPU.RenderThisFrame = TRUE;
				// Are we early?
				syncdif = sync_next - syncnow;
				if (syncdif > 0)
				{
					do {
						S9xProcessSound (0);
#ifdef ACCUMULATE_JOYPAD
/*
 * This call allows NDSSFC to synchronise the DS controller more often.
 * If porting a later version of Snes9x into NDSSFC, it is essential to
 * preserve it.
 */
						NDSSFCAccumulateJoypad ();
#endif
						syncdif = sync_next - getSysTime();
					} while (syncdif > 0);
					// After that little delay, what time is it?
					syncnow = getSysTime();
				}
				sync_next = syncnow + frame_time * (Settings.SkipFrames + 1);
			}
			else
			{
				IPPU.RenderThisFrame = FALSE;
			}
		}
	}

#ifdef __sgi
    /* BS: saves on CPU usage */
    sginap(1);
#endif

#if 0
    /* Check events */
    
    static struct timeval next1 = {0, 0};
    struct timeval now;

    CHECK_SOUND();
//  S9xProcessEvents(FALSE);

    while (gettimeofday (&now, NULL) < 0) ;
    
    /* If there is no known "next" frame, initialize it now */
    if (next1.tv_sec == 0) { next1 = now; ++next1.tv_usec; }

    /* If we're on AUTO_FRAMERATE, we'll display frames always
     * only if there's excess time.
     * Otherwise we'll display the defined amount of frames.
     */
    unsigned limit = Settings.SkipFrames == AUTO_FRAMERATE
                     ? (timercmp(&next1, &now, <) ? 10 : 1)
                     : Settings.SkipFrames;
    
    IPPU.RenderThisFrame = ++IPPU.SkippedFrames >= limit;
    if(IPPU.RenderThisFrame)
    {
        IPPU.SkippedFrames = 0;
    }
    else
    {
        /* If we were behind the schedule, check how much it is */
        if(timercmp(&next1, &now, <))
        {
            unsigned lag =
                (now.tv_sec - next1.tv_sec) * 1000000
               + now.tv_usec - next1.tv_usec;
            if(lag >= 1000000)
            {
                /* More than a second behind means probably
                 * pause. The next line prevents the magic
                 * fast-forward effect.
                 */
                next1 = now;
            }
        }
    }
    
    /* Delay until we're completed this frame */

    /* Can't use setitimer because the sound code already could
     * be using it. We don't actually need it either.
     */

    while(timercmp(&next1, &now, >))
    {
        /* If we're ahead of time, sleep a while */
        unsigned timeleft =
            (next1.tv_sec - now.tv_sec) * 1000000
           + next1.tv_usec - now.tv_usec;
        //fprintf(stderr, "<%u>", timeleft);
        usleep(timeleft);

        CHECK_SOUND();
//  S9xProcessEvents(FALSE);

        while (gettimeofday (&now, NULL) < 0) ;
        /* Continue with a while-loop because usleep()
         * could be interrupted by a signal
         */
    }

    /* Calculate the timestamp of the next frame. */
    next1.tv_usec += Settings.FrameTime;
    if (next1.tv_usec >= 1000000)
    {
        next1.tv_sec += next1.tv_usec / 1000000;
        next1.tv_usec %= 1000000;
    }
#endif
}

bool8 S9xOpenSoundDevice (int mode, bool8 stereo, int buffer_size)
{
#ifndef FOREVER_16_BIT_SOUND
	so.sixteen_bit = TRUE;
#endif
#ifndef FOREVER_STEREO
    so.stereo = stereo;
#endif
    so.playback_rate = SND_SAMPLE_RATE;
    S9xSetPlaybackRate (so.playback_rate);

    if (buffer_size == 0)
	    buffer_size = DS2_BUFFER_SIZE;

    if (buffer_size > MAX_BUFFER_SIZE / 4)
	    buffer_size = MAX_BUFFER_SIZE / 4;
#ifndef FOREVER_16_BIT_SOUND
    if (so.sixteen_bit)
#endif
	    buffer_size *= 2;
#ifndef FOREVER_STEREO
    if (so.stereo)
#endif
	    buffer_size *= 2;

	so.buffer_size = buffer_size;

    return (TRUE);
}

void S9xGenerateSound ()
{
#ifndef FOREVER_16_BIT_SOUND
	int bytes_so_far = so.sixteen_bit ? (so.samples_mixed_so_far << 1) :
		so.samples_mixed_so_far;
#else
	int bytes_so_far = so.samples_mixed_so_far << 1;
#endif

	if (bytes_so_far >= so.buffer_size)
		return;

	so.err_counter += so.err_rate;
	if (so.err_counter >= FIXED_POINT)
	{
		// Write this many samples overall
		int samples_to_write = so.err_counter >> FIXED_POINT_SHIFT;
#ifndef FOREVER_STEREO
		if (so.stereo)
#endif
			samples_to_write <<= 1;
		int byte_offset = (bytes_so_far + so.play_position) & SOUND_BUFFER_SIZE_MASK;

		so.err_counter &= FIXED_POINT_REMAINDER;

		do
		{
			int bytes_this_run = samples_to_write;
#ifndef FOREVER_16_BIT_SOUND
			if (so.sixteen_bit)
#endif
				bytes_this_run <<= 1;

			if (byte_offset + bytes_this_run > SOUND_BUFFER_SIZE)
			{
				bytes_this_run = SOUND_BUFFER_SIZE - byte_offset;
			}

			if (bytes_so_far + bytes_this_run > so.buffer_size)
			{
				bytes_this_run = so.buffer_size - bytes_so_far;
				if (bytes_this_run == 0)
					break;
			}

			int samples_this_run = bytes_this_run;
#ifndef FOREVER_16_BIT_SOUND
			if (so.sixteen_bit)
#endif
				samples_this_run >>= 1;

			S9xMixSamples (Buf + byte_offset, samples_this_run);
			so.samples_mixed_so_far += samples_this_run;
			samples_to_write -= samples_this_run;
#ifndef FOREVER_16_BIT_SOUND
			bytes_so_far += so.sixteen_bit ? (samples_this_run << 1) :
				samples_this_run;
#else
			bytes_so_far += samples_this_run << 1;
#endif
			byte_offset = (byte_offset + bytes_this_run) & SOUND_BUFFER_SIZE_MASK;
		} while (samples_to_write > 0);
	}
}

#define SOUND_EMISSION_INTERVAL ((unsigned int) ((((unsigned long long) DS2_BUFFER_SIZE * 1000000) / SND_SAMPLE_RATE) * 3 / 128))
#define TRUE_SOUND_EMISSION_INTERVAL ((((double) DS2_BUFFER_SIZE * 1000000) / SND_SAMPLE_RATE) * 3 / 128)
#define SOUND_EMISSION_INTERVAL_ERROR ((int) ((TRUE_SOUND_EMISSION_INTERVAL - SOUND_EMISSION_INTERVAL) * FIXED_POINT))
static unsigned int LastSoundEmissionTime = 0;

/*
 * Accumulated error in the sound emission time. The unit is as follows:
 * FIXED_POINT = 42.667 microseconds.
 * As the error goes past FIXED_POINT, the new target for sound emission
 * becomes 42.667 microseconds LATER. This helps with sound buffer overruns,
 * correctly dealing with the fact that 42.667 microseconds does not fit
 * an integer number of times in 1/32000 second (or whatever sampling rate).
 */
static unsigned int SoundEmissionTimeError = 0;

void S9xProcessSound (unsigned int)
{
	if (!game_enable_audio)
		return;

	unsigned int Now = getSysTime();
	if (Now - LastSoundEmissionTime >= SOUND_EMISSION_INTERVAL)
	{
		if(ds2_checkAudiobuff() > AUDIO_BUFFER_COUNT - 1)
		{
			LastSoundEmissionTime++;
			return;
		}

		unsigned short *audiobuff;

		if (Now - LastSoundEmissionTime >= 11719 /* 500 milliseconds */)
		{
			LastSoundEmissionTime = Now;
			// We were probably paused. Restart sending sound,
			// synchronising from now.
		}
		else
		{
			LastSoundEmissionTime += SOUND_EMISSION_INTERVAL;
			SoundEmissionTimeError += SOUND_EMISSION_INTERVAL_ERROR;
			if (SoundEmissionTimeError >= FIXED_POINT)
			{
				LastSoundEmissionTime += SoundEmissionTimeError >> FIXED_POINT_SHIFT;
				SoundEmissionTimeError &= FIXED_POINT_REMAINDER;
			}
		}
		/* Number of samples to generate now */
		int sample_count = so.buffer_size;
#ifndef FOREVER_16_BIT_SOUND
		if (so.sixteen_bit)
		{
#endif
			/* to prevent running out of buffer space,
			* create less samples
			*/
			sample_count >>= 1;
#ifndef FOREVER_16_BIT_SOUND
		}
#endif

		audiobuff = (unsigned short*)ds2_getAudiobuff();
		while (audiobuff == NULL) //There are audio queue in sending or wait to send
		{
#ifdef ACCUMULATE_JOYPAD
			NDSSFCAccumulateJoypad ();
#endif
			audiobuff = (unsigned short*)ds2_getAudiobuff();
		}

		/* If we need more audio samples */
		if (so.samples_mixed_so_far < sample_count)
		{
			/* Where to put the samples to */
#ifndef FOREVER_16_BIT_SOUND
			unsigned byte_offset = (so.play_position + 
			(so.sixteen_bit ? (so.samples_mixed_so_far << 1) : so.samples_mixed_so_far)) & SOUND_BUFFER_SIZE_MASK;
#else
			unsigned byte_offset = (so.play_position + 
			(so.samples_mixed_so_far << 1)) & SOUND_BUFFER_SIZE_MASK;
#endif

			if (Settings.SoundSync == 2)
			{
				/*memset (Buf + (byte_offset & SOUND_BUFFER_SIZE_MASK), 0,
				sample_count - so.samples_mixed_so_far);*/
			}
			else
			{
				/* Mix the missing samples */
#ifndef FOREVER_16_BIT_SOUND
				int bytes_so_far = so.sixteen_bit ? (so.samples_mixed_so_far << 1) :
					so.samples_mixed_so_far;
#else
				int bytes_so_far = so.samples_mixed_so_far << 1;
#endif

				uint32 samples_to_write = sample_count - so.samples_mixed_so_far;
				do
				{
					int bytes_this_run = samples_to_write;
#ifndef FOREVER_16_BIT_SOUND
					if (so.sixteen_bit)
#endif
						bytes_this_run <<= 1;

					if (byte_offset + bytes_this_run > SOUND_BUFFER_SIZE)
					{
						bytes_this_run = SOUND_BUFFER_SIZE - byte_offset;
					}

					if (bytes_so_far + bytes_this_run > so.buffer_size)
					{
						bytes_this_run = so.buffer_size - bytes_so_far;
						if (bytes_this_run == 0)
							break;
					}

					int samples_this_run = bytes_this_run;
#ifndef FOREVER_16_BIT_SOUND
					if (so.sixteen_bit)
#endif
						samples_this_run >>= 1;

					S9xMixSamples (Buf + byte_offset, samples_this_run);
					so.samples_mixed_so_far += samples_this_run;
					samples_to_write -= samples_this_run;
#ifndef FOREVER_16_BIT_SOUND
					bytes_so_far += so.sixteen_bit ? (samples_this_run << 1) :
						samples_this_run;
#else
					bytes_so_far += samples_this_run << 1;
#endif
					byte_offset = (byte_offset + bytes_this_run) & SOUND_BUFFER_SIZE_MASK;
				} while (samples_to_write > 0);
			}
		}

	//    if (!so.mute_sound)
		{
			unsigned bytes_to_write = sample_count;
#ifndef FOREVER_16_BIT_SOUND
			if(so.sixteen_bit)
#endif
				bytes_to_write <<= 1;

			unsigned byte_offset = so.play_position;
			so.play_position = (so.play_position + bytes_to_write) & SOUND_BUFFER_SIZE_MASK; /* wrap to beginning */

			unsigned short *dst_pt = audiobuff;
			unsigned short *dst_pt1 = dst_pt + DS2_BUFFER_SIZE;

			/* Feed the samples to the soundcard until nothing is left */
			for(;;)
			{
				int I = bytes_to_write;
				if (byte_offset + I > SOUND_BUFFER_SIZE)
				{
					I = SOUND_BUFFER_SIZE - byte_offset;
				}
				if(I == 0) break;

	//			memcpy(dst_pt, (char *) Buf + byte_offset, I);
	//			dst_pt += I;

				unsigned short *src_pt= (unsigned short*)(Buf + byte_offset);
				for(int m= 0; m < I/4; m++)
				{
					*dst_pt++= *src_pt++;//(*src_pt++) <<1;
					*dst_pt1++= *src_pt++;//(*src_pt++) <<1;
				}

				bytes_to_write -= I;
				byte_offset = (byte_offset + I) & SOUND_BUFFER_SIZE_MASK; /* wrap */
			}

			ds2_updateAudio();

			/* All data sent. */
		}

		so.samples_mixed_so_far -= sample_count;
	}
}

/*
const unsigned int keymap[12] = {
		0x80,		//KEY_A
		0x8000,		//KEY_B
		0x2000,		//KEY_SELECT
		0x1000,		//KEY_START
		0x100,		//KEY_RIGHT
		0x200,		//KEY_LEFT
		0x800,		//KEY_UP
		0x400,		//KEY_DOWN
		0x10,		//KEY_R
		0x20,		//KEY_L
		0x40,		//KEY_X
		0x4000		//KEY_Y
	};
*/

static bool8 SoundToggleWasHeld = FALSE;

#ifdef ACCUMULATE_JOYPAD
// These are kept as DS key bitfields until it's time to send them to Snes9x.
static uint32 PreviousControls = 0x00000000;
static uint32 ControlsPressed  = 0x00000000;
static uint32 ControlsReleased = 0x00000000;

void NDSSFCAccumulateJoypad ()
{
	struct key_buf inputdata;
	ds2_getrawInput(&inputdata);

	ControlsPressed |= inputdata.key & ~PreviousControls;
	ControlsReleased |= PreviousControls & ~inputdata.key;
}
#endif // ACCUMULATE_JOYPAD

uint32 S9xReadJoypad (int which1)
{
	if(which1 < 1)
	{
		uint32 Controls;
#ifdef ACCUMULATE_JOYPAD
		Controls = (PreviousControls | ControlsPressed) & ~ControlsReleased;
		PreviousControls = Controls;
		ControlsPressed = ControlsReleased = 0x00000000;
#else
		{
			struct key_buf inputdata;
			ds2_getrawInput(&inputdata);

			Controls = inputdata.key;
		}
#endif

		if (Controls & KEY_LID)
		{
			LowFrequencyCPU();
			ds2_setSupend();
			struct key_buf inputdata;
			do {
				ds2_getrawInput(&inputdata);
				mdelay(1);
			} while (inputdata.key & KEY_LID);
			ds2_wakeup();
			// Before starting to emulate again, turn off the lower
			// screen's backlight.
			mdelay(100); // needed to avoid ds2_setBacklight crashing
			ds2_setBacklight(2);
			GameFrequencyCPU();
		}

		u32 HotkeyReturnToMenu = game_config.HotkeyReturnToMenu != 0 ? game_config.HotkeyReturnToMenu : emu_config.HotkeyReturnToMenu;
		u32 HotkeyTemporaryFastForward = game_config.HotkeyTemporaryFastForward != 0 ? game_config.HotkeyTemporaryFastForward : emu_config.HotkeyTemporaryFastForward;
		u32 HotkeyToggleSound = game_config.HotkeyToggleSound != 0 ? game_config.HotkeyToggleSound : emu_config.HotkeyToggleSound;

		if(Controls & KEY_TOUCH ||
			(HotkeyReturnToMenu && ((Controls & HotkeyReturnToMenu) == HotkeyReturnToMenu))
		)	//Active menu
			Settings.Paused = 1;

		temporary_fast_forward =
			(HotkeyTemporaryFastForward && ((Controls & HotkeyTemporaryFastForward) == HotkeyTemporaryFastForward))
		;

		bool8 SoundToggleIsHeld = 
			(HotkeyToggleSound && ((Controls & HotkeyToggleSound) == HotkeyToggleSound))
		;

		if (SoundToggleIsHeld && !SoundToggleWasHeld)
		{
			game_enable_audio = !game_enable_audio;
			game_disableAudio();
		}

		SoundToggleWasHeld = SoundToggleIsHeld;

		uint32 key = 0x80000000;  // Required by Snes9x

		                                           //   DS   ->  SNES
		key |= (Controls & KEY_A     ) <<  7;      // 0x0001 -> 0x0080
		key |= (Controls & KEY_B     ) << 14;      // 0x0002 -> 0x8000
		key |= (Controls & KEY_SELECT) << 11;      // 0x0004 -> 0x2000
		key |= (Controls & KEY_START ) <<  9;      // 0x0008 -> 0x1000
		key |= (Controls & KEY_UP    ) <<  5;      // 0x0040 -> 0x0800
		// 0x0010 -> 0x0100; 0x0020 -> 0x0200
		// 0x0030 -> 0x0300
		key |= (Controls & (KEY_RIGHT | KEY_LEFT))  <<  4;
		// 0x0100 -> 0x0010; 0x0200 -> 0x0020; 0x0400 -> 0x0040
		// 0x0700 -> 0x0070
		key |= (Controls & (KEY_R | KEY_L | KEY_X)) >>  4;
		// 0x0080 -> 0x0400; 0x0800 -> 0x4000
		// 0x0880 -> 0x4400
		key |= (Controls & (KEY_DOWN | KEY_Y))      <<  3;
/*
		for(i= 0; i < 12; i++)	//remap key
		{
			key |= (inputdata.key & (1<<i)) ? keymap[i] : 0;
		}
*/

		return key;
	}
	else
		return 0;
}

static int S9xCompareSDD1IndexEntries (const void *p1, const void *p2)
{
    return (*(uint32 *) p1 - *(uint32 *) p2);
}

void S9xLoadSDD1Data ()
{
    char filename [_MAX_PATH + 1];
    char index [_MAX_PATH + 1];
    char data [_MAX_PATH + 1];
    char patch [_MAX_PATH + 1];

    Memory.FreeSDD1Data ();

    strcpy (filename, S9xGetSnapshotDirectory ());

    Settings.SDD1Pack=FALSE;
    if (strncmp (Memory.ROMName, "Star Ocean", 10) == 0){
        if(SDD1_pack) strcpy (filename, SDD1_pack);
#ifdef SDD1_DECOMP
        else Settings.SDD1Pack=TRUE;
#else
	strcat (filename, "/socnsdd1");
#endif
    } else if(strncmp(Memory.ROMName, "STREET FIGHTER ALPHA2", 21)==0){
        if(SDD1_pack) strcpy (filename, SDD1_pack);
#ifdef SDD1_DECOMP
        else Settings.SDD1Pack=TRUE;
#else
	strcat (filename, "/sfa2sdd1");
#endif
    } else {
        if(SDD1_pack) strcpy (filename, SDD1_pack);
#ifdef SDD1_DECOMP
        else Settings.SDD1Pack=TRUE;
#else
	S9xMessage(S9X_WARNING, S9X_ROM_INFO, "WARNING: No default SDD1 pack for this ROM");
#endif
    }

    if(Settings.SDD1Pack) return;

    DIR *dir = opendir (filename);

    index [0] = 0;
    data [0] = 0;
    patch [0] = 0;

    if (dir)
    {
//	struct dirent *d;
	dirent *d;
	
	while ((d = readdir (dir)))
	{
	    if (strcasecmp (d->d_name, "SDD1GFX.IDX") == 0)
	    {
		strcpy (index, filename);
		strcat (index, "/");
		strcat (index, d->d_name);
	    }
	    else
	    if (strcasecmp (d->d_name, "SDD1GFX.DAT") == 0)
	    {
		strcpy (data, filename);
		strcat (data, "/");
		strcat (data, d->d_name);
	    }
	    if (strcasecmp (d->d_name, "SDD1GFX.PAT") == 0)
	    {
		strcpy (patch, filename);
		strcat (patch, "/");
		strcat (patch, d->d_name);
	    }
	}
	closedir (dir);

	if (strlen (index) && strlen (data))
	{
	    FILE *fs = fopen (index, "rb");
	    int len = 0;

	    if (fs)
	    {
		// Index is stored as a sequence of entries, each entry being
		// 12 bytes consisting of:
		// 4 byte key: (24bit address & 0xfffff * 16) | translated block
		// 4 byte ROM offset
		// 4 byte length
		fseek (fs, 0, SEEK_END);
		len = ftell (fs);
		//rewind (fs);
        fseek (fs, 0, SEEK_SET);
		Memory.SDD1Index = (uint8 *) malloc (len);
		fread (Memory.SDD1Index, 1, len, fs);
		fclose (fs);
		Memory.SDD1Entries = len / 12;

		if (!(fs = fopen (data, "rb")))
		{
		    free ((char *) Memory.SDD1Index);
		    Memory.SDD1Index = NULL;
		    Memory.SDD1Entries = 0;
		}
		else
		{
		    fseek (fs, 0, SEEK_END);
		    len = ftell (fs);
		    //rewind (fs);
            fseek (fs, 0, SEEK_SET);
		    Memory.SDD1Data = (uint8 *) malloc (len);
		    fread (Memory.SDD1Data, 1, len, fs);
		    fclose (fs);

		    if (strlen (patch) > 0 &&
			(fs = fopen (patch, "rb")))
		    {
			fclose (fs);
		    }
#ifdef MSB_FIRST
		    // Swap the byte order of the 32-bit value triplets on
		    // MSBFirst machines.
		    uint8 *ptr = Memory.SDD1Index;
		    for (int i = 0; i < Memory.SDD1Entries; i++, ptr += 12)
		    {
			SWAP_DWORD ((*(uint32 *) (ptr + 0)));
			SWAP_DWORD ((*(uint32 *) (ptr + 4)));
			SWAP_DWORD ((*(uint32 *) (ptr + 8)));
		    }
#endif
		    qsort (Memory.SDD1Index, Memory.SDD1Entries, 12,
			   S9xCompareSDD1IndexEntries);
		}
	    }
	}
	else
	{
	    fprintf (stderr, "Decompressed data pack not found in '%s'.\n", 
                     filename);
	}
    }
}

bool8 S9xReadMousePosition (int which1, int &x, int &y, uint32 &buttons)
{
    return (FALSE);
}

bool8 S9xReadSuperScopePosition (int &x, int &y, uint32 &buttons)
{
    return (TRUE);
}

bool JustifierOffscreen()
{
    return (FALSE);
}

void JustifierButtons(uint32& justifiers)
{
}

START_EXTERN_C
char* osd_GetPackDir()
{
  static char filename[_MAX_PATH];
  memset(filename, 0, _MAX_PATH);
  
  if(strlen(S9xGetSnapshotDirectory())!=0)
    strcpy (filename, S9xGetSnapshotDirectory());
  else
  {
    char dir [_MAX_DIR + 1];
    char drive [_MAX_DRIVE + 1];
    char name [_MAX_FNAME + 1];
    char ext [_MAX_EXT + 1];
    _splitpath(Memory.ROMFilename, drive, dir, name, ext);
    _makepath(filename, drive, dir, NULL, NULL);
  }
  
  if(!strncmp((char*)&Memory.ROM [0xffc0], "SUPER POWER LEAG 4   ", 21))
  {
    if (getenv("SPL4PACK"))
      return getenv("SPL4PACK");
    else 
      strcat(filename, "/SPL4-SP7");
  }
  else if(!strncmp((char*)&Memory.ROM [0xffc0], "MOMOTETSU HAPPY      ",21))
  {
    if (getenv("MDHPACK"))
      return getenv("MDHPACK");
    else 
      strcat(filename, "/SMHT-SP7");
  }
  else if(!strncmp((char*)&Memory.ROM [0xffc0], "HU TENGAI MAKYO ZERO ", 21))
  {
    if (getenv("FEOEZPACK"))
      return getenv("FEOEZPACK");
    else 
      strcat(filename, "/FEOEZSP7");
  }
  else if(!strncmp((char*)&Memory.ROM [0xffc0], "JUMP TENGAIMAKYO ZERO",21))
  {
    if (getenv("SJNSPACK"))
      return getenv("SJNSPACK");
    else 
      strcat(filename, "/SJUMPSP7");
  } else strcat(filename, "/MISC-SP7");
  return filename;
}
END_EXTERN_C


