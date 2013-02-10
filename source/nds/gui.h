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

#ifdef DS2_DMA
#define UP_SCREEN_UPDATE_METHOD   1
#else
#define UP_SCREEN_UPDATE_METHOD   0
#endif

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
	u32 clock_speed_number;
	u32  Reserved0;
	u32 frameskip_value;
	u32 graphic;
	u32 enable_audio;
	u32 Reserved1;
	u32 backward;
	u32 backward_time;
	u32 HotkeyReturnToMenu;
	u32 HotkeyTemporaryFastForward;
	u32 HotkeyToggleSound;
	u32 SoundSync;
	/*
	 * PreviouslyUsed_20130206_1 was for a second meaning of
	 * frameskip_value that is now dropped.
	 * THIS VALUE IS NOT GUARANTEED TO BE RESERVED AND SET TO 0.
	 */
	u32 PreviouslyUsed_20130206_1;
	/*
	 * PreviouslyUsed_20130206_2 was for a second meaning of
	 * clock_speed_number that is now dropped.
	 * THIS VALUE IS NOT GUARANTEED TO BE RESERVED AND SET TO 0.
	 */
	u32 PreviouslyUsed_20130206_2;
	u32 RetroSound;
	u32  Reserved2[41];
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
extern void game_set_retro();
extern void LowFrequencyCPU();
extern void HighFrequencyCPU();
extern void GameFrequencyCPU();
extern int load_language_msg(char *filename, u32 language);

#ifdef __cplusplus
}
#endif

#endif //__GUI_H__
