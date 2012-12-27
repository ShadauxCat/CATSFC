//entry.c
#include <stdio.h>

#include "ds2_types.h"
#include "ds2_cpu.h"
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
#include "ds2sound.h"

void S9xProcessSound (unsigned int);

char *rom_filename = NULL;
char *SDD1_pack = NULL;

static u8 Buf[MAX_BUFFER_SIZE];

#define FIXED_POINT 0x10000
#define FIXED_POINT_SHIFT 16
#define FIXED_POINT_REMAINDER 0xffff

static volatile bool8 block_signal = FALSE;
static volatile bool8 block_generate_sound = FALSE;
static volatile bool8 pending_signal = FALSE;

static void Init_Timer (void);

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
    if(GFX.Screen) free(GFX.Screen);
    if(GFX.SubScreen) free(GFX.SubScreen);
    if(GFX.ZBuffer) free(GFX.ZBuffer);
    if(GFX.SubZBuffer) free(GFX.SubZBuffer);
}

void S9xInitDisplay (int, char **)
{
    int h = IMAGE_HEIGHT;

	GFX.Pitch = IMAGE_WIDTH * 2;
	GFX.Screen =	(unsigned char*) malloc (GFX.Pitch * h);
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
  ds2_setCPUclocklevel(13); // Crank it up to exit quickly
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



bool frame_flip = 0;

extern bool Draw_Frame_Flip(bool flip);
	

bool8 S9xDeinitUpdate (int Width, int Height, bool8 /*sixteen_bit*/)
{
	
	
	switch(game_config.graphic)
	{
		//Up
		case 1:
		    memcpy(up_screen_addr, GFX.Screen+256*32*2, 256*192*2);
			break;

		//Down
		case 2:
		    memcpy(up_screen_addr, GFX.Screen, 256*192*2);
			break;

		//Both
		case 3:
		    memcpy(up_screen_addr, GFX.Screen+256*16*2, 256*192*2);
			break;
			
		case 4:
			frame_flip =  Draw_Frame_Flip(frame_flip);
		break;
		

		default:
		{
			unsigned char *src, *dst;
			unsigned int m, n;

			src = GFX.Screen;
			dst = (unsigned char*)up_screen_addr;
			for(m = 0; m < 32; m++)
			{
				memcpy(dst, src, 256*6*2);
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
		Settings.APUEnabled = Settings.NextAPUEnabled = TRUE;
		S9xSetSoundMute (FALSE);
	}
	else
	{
		Settings.APUEnabled = Settings.NextAPUEnabled = FALSE;
		S9xSetSoundMute (TRUE);
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
    Settings.Stereo = TRUE;
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
    Settings.FrameTime = Settings.FrameTimeNTSC;
    Settings.DisableSampleCaching = FALSE;
    Settings.DisableMasterVolume = FALSE;
    Settings.Mouse = TRUE;
    Settings.SuperScope = TRUE;
    Settings.MultiPlayer5 = TRUE;
    Settings.ControllerOption = SNES_JOYPAD;

    Settings.Transparency = TRUE;
#ifndef FOREVER_16_BIT
    Settings.SixteenBit = TRUE;
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
    Settings.TurboSkipFrames = 40;
    Settings.StretchScreenshots = 1;

	Settings.HBlankStart = (256 * Settings.H_Max) / SNES_HCOUNTER_MAX;
}

extern "C" {
	int game_load_state(char* file);
	int game_save_state(char* file);
	void S9xAutoSaveSRAM ();
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

extern "C" void game_restart(void);

void game_restart(void)
{
	CPU.Flags = 0;
	S9xReset ();
}

extern "C" int load_gamepak(char* file);

int load_gamepak(char* file)
{
	game_enable_audio = 1;
	game_disableAudio();

	CPU.Flags = 0;
	S9xReset ();
	// mdelay(50); // Delete this delay
	if (!Memory.LoadROM (file))
		return -1;

	Memory.LoadSRAM (S9xGetFilename (".srm"));
	// mdelay(50); // Delete this delay
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

	// mdelay(50); // Delete this delay
    if (!Settings.APUEnabled)
	    S9xSetSoundMute (FALSE);

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

    S9xInitSound (Settings.SoundPlaybackRate, Settings.Stereo,
                  Settings.SoundBufferSize);

    if (!Settings.APUEnabled)
		S9xSetSoundMute (TRUE);

#ifdef GFX_MULTI_FORMAT
//    S9xSetRenderPixelFormat (RGB565);
    S9xSetRenderPixelFormat (BGR555);
#endif

#ifdef JOYSTICK_SUPPORT
    uint32 JoypadSkip = 0;
#endif

//    Init_Timer ();

    /* FIXME: Is someone using this dead code, or should it go? */
#if 0
    {
	FILE *fs = fopen ("test.bin", "r");
	if (fs)
	{
	    memset (IAPU.RAM, 0, 1024 * 64);
	    int bytes = fread (IAPU.RAM + 1024, 1, 13, fs);
	    bytes = fread (IAPU.RAM + 1024, 1, 1024 * 63, fs);
	    fclose (fs);
#ifdef SPCTOOL
	    _FixSPC (1024, 0, 0, 0, 0, 0xff);
#else
	    IAPU.PC = IAPU.RAM + 1024;
#endif
	    APU.Flags ^= TRACE_FLAG;
	    extern FILE *apu_trace;
	    if (APU.Flags & TRACE_FLAG)
	    {
#ifdef SPCTOOL
		printf ("ENABLED\n");
		_SetSPCDbg (TraceSPC);                   //Install debug handler
#endif
		if (apu_trace == NULL)
		    apu_trace = fopen ("aputrace.log", "wb");
	    }
	    CPU.Cycles = 1024 * 10;
	    APU_EXECUTE ();
	    exit (0);
	}
    }
#endif

	Settings.Paused = 1;

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
			menu(screen);
			Settings.Paused = 0;
			game_disableAudio();
		}

#ifdef JOYSTICK_SUPPORT
		//if (Settings.JoystickEnabled && (JoypadSkip++ & 1) == 0)
		if (Settings.JoystickEnabled)
			ReadJoysticks ();
#endif

    }

    return (0);
}

void S9xSyncSpeed ()
{
#if 0
#ifdef _NETPLAY_SUPPORT
    if (Settings.NetPlay)
    {
	// XXX: Send joypad position update to server
	// XXX: Wait for heart beat from server
	S9xNetPlaySendJoypadUpdate (joypads [0]);
	if (!S9xNetPlayCheckForHeartBeat ())
	{
	    do
	    {
		CHECK_SOUND ();
//		S9xProcessEvents (FALSE);
	    } while (!S9xNetPlayCheckForHeartBeat ());
	    IPPU.RenderThisFrame = TRUE;
	    IPPU.SkippedFrames = 0;
	}
	else
	{
	    if (IPPU.SkippedFrames < 10)
	    {
		IPPU.SkippedFrames++;
		IPPU.RenderThisFrame = FALSE;
	    }
	    else
	    {
		IPPU.RenderThisFrame = TRUE;
		IPPU.SkippedFrames = 0;
	    }
	}
    }
    else
#endif

#if 0
    if (Settings.SoundSync == 2)
    {
	IPPU.RenderThisFrame = TRUE;
	IPPU.SkippedFrames = 0;
	return;
    }
#endif

#if 0
    if (Settings.TurboMode)
    {
        if(++IPPU.FrameSkip >= Settings.TurboSkipFrames)
        {
            IPPU.FrameSkip = 0;
            IPPU.SkippedFrames = 0;
            IPPU.RenderThisFrame = TRUE;
        }
        else
        {
            ++IPPU.SkippedFrames;
            IPPU.RenderThisFrame = FALSE;
        }
        return;
    }
#endif

#ifdef __sgi
    /* BS: saves on CPU usage */
    sginap(1);
#endif

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
	so.sixteen_bit = TRUE;
    so.stereo = stereo;
    so.playback_rate = SND_SAMPLE_RATE;
    S9xSetPlaybackRate (so.playback_rate);

    if (buffer_size == 0)
	    buffer_size = DS2_BUFFER_SIZE;

    if (buffer_size > MAX_BUFFER_SIZE / 4)
	    buffer_size = MAX_BUFFER_SIZE / 4;
    if (so.sixteen_bit)
	    buffer_size *= 2;
    if (so.stereo)
	    buffer_size *= 2;

	so.buffer_size = buffer_size;

    return (TRUE);
}

void S9xGenerateSound ()
{
    int bytes_so_far = so.sixteen_bit ? (so.samples_mixed_so_far << 1) :
			so.samples_mixed_so_far;

	if (bytes_so_far >= so.buffer_size)
		return;

    block_signal = TRUE;

    so.err_counter += so.err_rate;
    if (so.err_counter >= FIXED_POINT)
    {
        int sample_count = so.err_counter >> FIXED_POINT_SHIFT;
		int byte_offset;
		int byte_count;

        so.err_counter &= FIXED_POINT_REMAINDER;
		if (so.stereo)
			sample_count <<= 1;
		byte_offset = bytes_so_far + so.play_position;

		do
		{
			int sc = sample_count;
			byte_count = sample_count;
			if (so.sixteen_bit)
				byte_count <<= 1;

			if ((byte_offset & SOUND_BUFFER_SIZE_MASK) + byte_count > SOUND_BUFFER_SIZE)
			{
				sc = SOUND_BUFFER_SIZE - (byte_offset & SOUND_BUFFER_SIZE_MASK);
				byte_count = sc;
				if (so.sixteen_bit)
					sc >>= 1;
			}

			if (bytes_so_far + byte_count > so.buffer_size)
			{
				byte_count = so.buffer_size - bytes_so_far;
				if (byte_count == 0)
					break;
				sc = byte_count;
				if (so.sixteen_bit)
					sc >>= 1;
			}

			S9xMixSamplesO (Buf, sc, byte_offset & SOUND_BUFFER_SIZE_MASK);
			so.samples_mixed_so_far += sc;
			sample_count -= sc;
			bytes_so_far = so.sixteen_bit ? (so.samples_mixed_so_far << 1) :
				so.samples_mixed_so_far;
			byte_offset += byte_count;
		} while (sample_count > 0);
    }

    block_signal = FALSE;

    if (pending_signal)
    {
		S9xProcessSound (0);
		pending_signal = FALSE;
    }
}

void S9xProcessSound (unsigned int)
{
	unsigned short *audiobuff;

	if (!Settings.APUEnabled || so.mute_sound )
		return;

	if(ds2_checkAudiobuff() > 4)
		return;

	/* Number of samples to generate now */
	int sample_count = so.buffer_size;

	if (so.sixteen_bit)
	{
		/* to prevent running out of buffer space,
		* create less samples
		*/
		sample_count >>= 1;
	}

	if (block_signal)
	{
		pending_signal = TRUE;
		return;
	}

//	block_generate_sound = TRUE;

	audiobuff = (unsigned short*)ds2_getAudiobuff();
	if(NULL == audiobuff)	//There are audio queue in sending or wait to send
	{
		return;
	}

	/* If we need more audio samples */
	if (so.samples_mixed_so_far < sample_count)
	{
		/* Where to put the samples to */
		unsigned byte_offset = so.play_position + 
		(so.sixteen_bit ? (so.samples_mixed_so_far << 1) : so.samples_mixed_so_far);

		//printf ("%d:", sample_count - so.samples_mixed_so_far); fflush (stdout);
		if (Settings.SoundSync == 2)
		{
			/*memset (Buf + (byte_offset & SOUND_BUFFER_SIZE_MASK), 0,
			sample_count - so.samples_mixed_so_far);*/
		}
		else
		{
			/* Mix the missing samples */
			S9xMixSamplesO (Buf, sample_count - so.samples_mixed_so_far,
				byte_offset & SOUND_BUFFER_SIZE_MASK);
		}
		so.samples_mixed_so_far = sample_count;
	}

//    if (!so.mute_sound)
	{
		unsigned bytes_to_write = sample_count;
		if(so.sixteen_bit) bytes_to_write <<= 1;

		unsigned byte_offset = so.play_position;
		so.play_position += bytes_to_write;
		so.play_position &= SOUND_BUFFER_SIZE_MASK; /* wrap to beginning */

//		block_generate_sound = FALSE;

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
			byte_offset += I;
			byte_offset &= SOUND_BUFFER_SIZE_MASK; /* wrap */
		}

		ds2_updateAudio();

		/* All data sent. */
	}

	so.samples_mixed_so_far -= sample_count;
}

void Init_Timer (void)
{
}



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

unsigned int S9xReadJoypad (int which1)
{
    struct key_buf inputdata;

	ds2_getrawInput(&inputdata);
	if(inputdata.key & KEY_TOUCH)	//Active menu
		Settings.Paused = 1;

	if(which1 < 1)
	{
		unsigned int key;
		unsigned int i;
	
		key = 0;
		for(i= 0; i < 12; i++)	//remap key
		{
			key |= (inputdata.key & (1<<i)) ? keymap[i] : 0;
		}

		// return (key | 0x80000000);
		return key; // ??? [Neb]
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


