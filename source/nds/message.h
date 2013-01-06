/* message.h
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

#ifndef __MESSAGE_H__
#define __MESSAGE_H__

enum MSG
{
	MSG_MAIN_MENU_VIDEO_AUDIO,
	MSG_MAIN_MENU_SAVED_STATES,
	MSG_MAIN_MENU_CHEATS,
	MSG_MAIN_MENU_TOOLS,
	MSG_MAIN_MENU_OPTIONS,
	MSG_MAIN_MENU_EXIT,
	FMT_VIDEO_ASPECT_RATIO,
	MSG_VIDEO_FAST_FORWARD,
	FMT_VIDEO_FRAME_SKIP_AUTOMATIC,
	FMT_VIDEO_FRAME_SKIP_MANUAL,
	FMT_AUDIO_SOUND,
	MSG_SUB_MENU_05,                  // unused
	MSG_SAVED_STATE_CREATE,
	FMT_SAVED_STATE_LOAD,
	MSG_SUB_MENU_12,                  // unused
	MSG_SAVED_STATE_DELETE_GENERAL,
	MSG_SUB_MENU_14,                  // unused
	FMT_CHEAT_PAGE,
	MSG_CHEAT_LOAD_FROM_FILE,
	MSG_SUB_MENU_22,                  // unused
	MSG_SUB_MENU_23,                  // unused
	MSG_SUB_MENU_24,                  // unused
	MSG_TOOLS_SCREENSHOT_GENERAL,
	MSG_SUB_MENU_31,                  // unused
	MSG_SUB_MENU_32,                  // unused
	MSG_SUB_MENU_40,                  // unused
	FMT_OPTIONS_LANGUAGE,
	FMT_OPTIONS_CPU_FREQUENCY,
	MSG_OPTIONS_CARD_CAPACITY,
	MSG_OPTIONS_RESET,
	MSG_OPTIONS_VERSION,
	MSG_SCREENSHOT_CREATE,
	MSG_SCREENSHOT_BROWSE,
	MSG_SUB_MENU_302,                 // unused
	MSG_SUB_MENU_310,                 // unused
	MSG_SUB_MENU_311,                 // unused
	MSG_SUB_MENU_312,                 // unused
	MSG_SUB_MENU_313,                 // unused
	MSG_SUB_MENU_314,                 // unused
	MSG_SUB_MENU_315,                 // unused
	MSG_LOAD_GAME_RECENTLY_PLAYED,
	MSG_LOAD_GAME_FROM_CARD,
	MSG_LOAD_GAME_MENU_TITLE,

	MSG_VIDEO_ASPECT_RATIO_0,
	MSG_VIDEO_ASPECT_RATIO_1,
	MSG_VIDEO_ASPECT_RATIO_2,
	MSG_VIDEO_ASPECT_RATIO_3,
	MSG_VIDEO_ASPECT_RATIO_4,

	MSG_FRAMESKIP_0,                  // unused
	MSG_FRAMESKIP_1,                  // unused

	MSG_ON_OFF_0,
	MSG_ON_OFF_1,

	MSG_AUDIO_ENABLED,
	MSG_AUDIO_MUTED,

	MSG_SNAP_FRAME_0,                 // unused
	MSG_SNAP_FRAME_1,                 // unused

	MSG_EN_DIS_ABLE_0,                // unused
	MSG_EN_DIS_ABLE_1,                // unused

	MSG_TOP_SCREEN_NO_GAME_LOADED,
	MSG_CHEAT_ELEMENT_NOT_LOADED,
	MSG_CHEAT_MENU_LOADED,            // unused

	MSG_LOAD_STATE,                   // unused
	MSG_LOAD_STATE_END,               // unused
	MSG_SAVE_STATE,                   // unused
	MSG_SAVE_STATE_END,               // unused

	MSG_KEY_MAP_NONE,                 // unused
	MSG_KEY_MAP_A,                    // unused
	MSG_KEY_MAP_B,                    // unused
	MSG_KEY_MAP_SL,                   // unused
	MSG_KEY_MAP_ST,                   // unused
	MSG_KEY_MAP_RT,                   // unused
	MSG_KEY_MAP_LF,                   // unused
	MSG_KEY_MAP_UP,                   // unused
	MSG_KEY_MAP_DW,                   // unused
	MSG_KEY_MAP_R,                    // unused
	MSG_KEY_MAP_L,                    // unused
	MSG_KEY_MAP_X,                    // unused
	MSG_KEY_MAP_Y,                    // unused
	MSG_KEY_MAP_TOUCH,                // unused

	MSG_SAVESTATE_EMPTY,              // unused
	MSG_SAVESTATE_FULL,
	MSG_PROGRESS_SAVED_STATE_CREATING,
	MSG_PROGRESS_SAVED_STATE_CREATION_FAILED,
	MSG_PROGRESS_SAVED_STATE_CREATION_SUCCEEDED,
	MSG_TOP_SCREEN_NO_SAVED_STATE_IN_SLOT,
	MSG_PROGRESS_SAVED_STATE_CORRUPTED,
	MSG_PROGRESS_SAVED_STATE_LOADING,
	MSG_PROGRESS_SAVED_STATE_LOAD_FAILED,
	MSG_PROGRESS_SAVED_STATE_LOAD_SUCCEEDED,

	MSG_WARING_DIALOG,                // unused
	MSG_TIME_FORMATE,                 // unused

	MSG_SAVED_STATE_DELETE_ALL,
	FMT_SAVED_STATE_DELETE_ONE,

	MSG_DIALOG_SAVED_STATE_DELETE_ALL,
	FMT_DIALOG_SAVED_STATE_DELETE_ONE,
	MSG_PROGRESS_SAVED_STATE_ALREADY_EMPTY,

	MSG_PROGRESS_SCREENSHOT_CREATING,
	MSG_PROGRESS_SCREENSHOT_CREATION_SUCCEEDED,
	MSG_PROGRESS_SCREENSHOT_CREATION_FAILED,

	MSG_CHANGE_LANGUAGE,
	MSG_CHANGE_LANGUAGE_WAITING,

	MSG_NO_SLIDE,
	MSG_PLAYING_SLIDE,
	MSG_PAUSE_SLIDE,
	MSG_PLAY_SLIDE1,
	MSG_PLAY_SLIDE2,
	MSG_PLAY_SLIDE3,
	MSG_PLAY_SLIDE4,
	MSG_PLAY_SLIDE5,
	MSG_PLAY_SLIDE6,

	MSG_PROGRESS_LOADING_GAME,

	MSG_EMULATOR_NAME,
	MSG_WORD_EMULATOR_VERSION,

	MSG_DIALOG_RESET,
	MSG_PROGRESS_RESETTING,

	MSG_BACK,                        // unused

	MSG_END
};

enum LANGUAGE {
	ENGLISH,
	CHINESE_SIMPLIFIED,
	FRENCH
};

extern char* lang[3]; // Allocated in gui.c, needs to match the languages ^

char *msg[MSG_END+1];
char msg_data[16 * 1024];

#endif //__MESSAGE_H__

