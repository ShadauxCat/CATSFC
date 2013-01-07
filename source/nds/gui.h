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

#define UP_SCREEN_UPDATE_METHOD   0
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
	u32  Reserved2;
	u32  Reserved3;
	u32  Reserved4;
	u32  Reserved5;
	u32  Reserved6;
	u32  Reserved7;
	u32  Reserved8;
	u32  Reserved9;
	u32  Reserved10;
	u32  Reserved11;
	u32  Reserved12;
	u32  Reserved13;
	u32  Reserved14;
	u32  Reserved15;
	u32  Reserved16;
	u32  Reserved17;
	u32  Reserved18;
	u32  Reserved19;
	u32  Reserved20;
	u32  Reserved21;
	u32  Reserved22;
	u32  Reserved23;
	u32  Reserved24;
	u32  Reserved25;
	u32  Reserved26;
	u32  Reserved27;
	u32  Reserved28;
	u32  Reserved29;
	u32  Reserved30;
	u32  Reserved31;
	u32  Reserved32;

	u32  Reserved33;
	u32  Reserved34;
	u32  Reserved35;
	u32  Reserved36;
	u32  Reserved37;
	u32  Reserved38;
	u32  Reserved39;
	u32  Reserved40;
	u32  Reserved41;
	u32  Reserved42;
	u32  Reserved43;
	u32  Reserved44;
	u32  Reserved45;
	u32  Reserved46;
	u32  Reserved47;
	u32  Reserved48;
	u32  Reserved49;
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
extern u32 menu(u16 *original_screen);
extern void game_disableAudio();
extern void game_set_frameskip();
extern void set_cpu_clock(u32 num);

#ifdef __cplusplus
}
#endif

#endif //__GUI_H__
