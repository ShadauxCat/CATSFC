/* gui.h
 *
 * Copyright (C) 2010 dking <dking024@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public Licens e as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef __GUI_H__
#define __GUI_H__

#include "ds2_types.h"
#include "fs_api.h"
#include "gcheat.h"

#define UP_SCREEN_UPDATE_METHOD   1
#define DOWN_SCREEN_UPDATE_METHOD 2

#define MAX_GAMEPAD_MAP 16

#ifdef __cplusplus
extern "C" {
#endif

//
struct _EMU_CONFIG
{
  u32 language;
  char rom_file[256];
  char rom_path[256];
  char latest_file[5][512];
  u32 HotkeyReturnToMenu;
  u32 HotkeyTemporaryFastForward;
  u32 HotkeyToggleSound;
  u32 Reserved[61];
};

struct _GAME_CONFIG
{
	/*
	 * PreviouslyUsed_20130205_2 was formerly known as
	 * 'clock_speed_number'; its values were in [0, 5]. [0, 5] were mapped
	 * to 240, 300, 336, 360, 384 and 394 MHz respectively.
	 * Version 1.29 changes the value range for 'clock_speed_number' to
	 * [0, 6], with 0 as an automatic CPU speed setting.
	 * Change rationale: The default value becomes 0 instead of 5.
	 * If this variable were to be used as is, the meaning of the default
	 * value would change. Games which had a configuration file before
	 * 1.29 would be using the older default of 5 (394 MHz), the meaning
	 * of which would become 384 MHz instead of "staying the default".
	 * Games which did not have a configuration file before 1.29 would be
	 * using the correct default.
	 * This would confuse users or cause undue hassle.
	 * THIS VALUE IS NOT GUARANTEED TO BE RESERVED AND SET TO 0.
	 * DO NOT USE THIS VALUE FOR ANY PURPOSE OTHER THAN EXACTLY THE ONE
	 * FOR WHICH IT WAS INTENDED.
	 */
	u32 PreviouslyUsed_20130205_2;
	u32  Reserved0;
	/*
	 * PreviouslyUsed_20130205_1 was formerly known as 'frameskip_value';
	 * its values were in [0, 10]. 0 was automatic frameskipping and
	 * [1, 10] were mapped to skip 0 to 9 frames respectively.
	 * Version 1.29 changes the value range for 'frameskip_value' to
	 * [0, 8], with 0 as automatic frameskipping and [1, 10] to skip 2 to
	 * 9 frames.
	 * Change rationale: Frame skip values under 2 cause too much
	 * communication between the DSTwo and the DS, therefore the DS cannot
	 * timely send controller information.
	 * If this variable were to be used as is, the meaning of the option
	 * would be changed for values in [1, 8], and values in [9, 10] would
	 * cause undefined behavior, including crashes.
	 * THIS VALUE IS NOT GUARANTEED TO BE RESERVED AND SET TO 0.
	 * DO NOT USE THIS VALUE FOR ANY PURPOSE OTHER THAN EXACTLY THE ONE
	 * FOR WHICH IT WAS INTENDED.
	 */
	u32 PreviouslyUsed_20130205_1;
	u32 graphic;
	u32 enable_audio;
	u32 Reserved1;
	u32 backward;
	u32 backward_time;
	u32 HotkeyReturnToMenu;
	u32 HotkeyTemporaryFastForward;
	u32 HotkeyToggleSound;
	u32 SoundSync;
	u32 frameskip_value;
	u32 clock_speed_number;
	u32  Reserved2[42];
};

typedef enum
{
  CURSOR_NONE = 0,
  CURSOR_UP,
  CURSOR_DOWN,
  CURSOR_LEFT,
  CURSOR_RIGHT,
  CURSOR_SELECT,
  CURSOR_BACK,
  CURSOR_EXIT,
  CURSOR_RTRIGGER,
  CURSOR_LTRIGGER,
  CURSOR_KEY_SELECT,
  CURSOR_TOUCH
} gui_action_type;

typedef enum
{
  BUTTON_ID_A   = 0x01,
  BUTTON_ID_B   = 0x02,
  BUTTON_ID_SELECT  = 0x04,
  BUTTON_ID_START   = 0x08,
  BUTTON_ID_RIGHT   = 0x10,
  BUTTON_ID_LEFT    = 0x20,
  BUTTON_ID_UP      = 0x40,
  BUTTON_ID_DOWN    = 0x80,
  BUTTON_ID_R       = 0x100,
  BUTTON_ID_L       = 0x200,
  BUTTON_ID_X       = 0x400,
  BUTTON_ID_Y       = 0x800,
  BUTTON_ID_TOUCH   = 0x1000,
  BUTTON_ID_LID     = 0x2000,
  BUTTON_ID_FA      = 0x4000,
  BUTTON_ID_FB      = 0x8000,
  BUTTON_ID_NONE    = 0
} input_buttons_id_type;

extern char main_path[MAX_PATH];
extern char rom_path[MAX_PATH];

extern u32 game_enable_audio;
extern u32 clock_speed_number;
extern u32 game_fast_forward;
extern u32 temporary_fast_forward;

/******************************************************************************
 ******************************************************************************/
extern char g_default_rom_dir[MAX_PATH];
extern char DEFAULT_RTS_DIR[MAX_PATH];
extern char DEFAULT_CFG_DIR[MAX_PATH];
extern char DEFAULT_SS_DIR[MAX_PATH];
extern char DEFAULT_CHEAT_DIR[MAX_PATH];

typedef struct _EMU_CONFIG		EMU_CONFIG;
typedef struct _GAME_CONFIG		GAME_CONFIG;

extern EMU_CONFIG	emu_config;
extern GAME_CONFIG	game_config;

/******************************************************************************
 ******************************************************************************/
extern void gui_init(u32 lang_id);
extern u32 menu(u16 *original_screen, bool8 FirstInvocation);
extern void game_disableAudio();
extern void game_set_frameskip();
extern void game_set_fluidity();
extern void LowFrequencyCPU();
extern void HighFrequencyCPU();
extern void GameFrequencyCPU();
extern int load_language_msg(char *filename, u32 language);

#ifdef __cplusplus
}
#endif

#endif //__GUI_H__
