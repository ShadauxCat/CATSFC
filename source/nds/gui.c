/* gui.c
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


 
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "port.h"
#include "ds2_types.h"
#include "ds2io.h"
#include "ds2_malloc.h"
#include "ds2_cpu.h"
#include "fs_api.h"
#include "key.h"
#include "gui.h"
#include "draw.h"
#include "message.h"
#include "bitmap.h"
#include "gcheat.h"

extern struct SCheatData Cheat;

char	main_path[MAX_PATH];
char	rom_path[MAX_PATH];
char	gamepak_name[MAX_PATH];
char	gcheat_filename[MAX_PATH];

// If adding a language, make sure you update the size of the array in
// message.h too.
char *lang[3] =
	{ 
		"English",					// 0
		"简体中文",					// 1
		"Français", 				// 2
	};

char *language_options[] = { (char *) &lang[0], (char *) &lang[1], (char *) &lang[2] };

/******************************************************************************
*	Macro definition
 ******************************************************************************/
#define SUBMENU_ROW_NUM	6

#define NDSSFC_VERSION "1.14"

#define SAVE_STATE_SLOT_NUM		10

#define LANGUAGE_PACK   "SYSTEM/language.msg"
#define EMU_CONFIG_FILENAME "SYSTEM/ndssfc.cfg"

//emulator configure file's header
#define EMU_CONFIG_HEADER  "NSFC1.0"
#define EMU_CONFIG_HEADER_SIZE 7
EMU_CONFIG emu_config;

//game configure file's header
#define GAME_CONFIG_HEADER     "GSFC1.1" // 1.1 removed cheat names
#define GAME_CONFIG_HEADER_SIZE 7
GAME_CONFIG game_config;

//save state file map
char savestate_map[SAVE_STATE_SLOT_NUM];
static unsigned int savestate_index;

#define MAKE_MENU(name, init_function, passive_function, key_function, end_function, \
	focus_option, screen_focus)												  \
  MENU_TYPE name##_menu =                                                     \
  {                                                                           \
    init_function,                                                            \
    passive_function,                                                         \
	key_function,															  \
	end_function,															  \
    name##_options,                                                           \
    sizeof(name##_options) / sizeof(MENU_OPTION_TYPE),                        \
	focus_option,															  \
	screen_focus															  \
  }                                                                           \

#define INIT_MENU(name, init_functions, passive_functions, key, end, focus, screen)\
	{																		  \
    name##_menu.init_function = init_functions,								  \
    name##_menu.passive_function = passive_functions,						  \
	name##_menu.key_function = key,											  \
	name##_menu.end_function = end,											  \
    name##_menu.options = name##_options,									  \
    name##_menu.num_options = sizeof(name##_options) / sizeof(MENU_OPTION_TYPE),\
	name##_menu.focus_option = focus,										  \
	name##_menu.screen_focus = screen;										  \
	}																		  \

#define CHEAT_OPTION(action_function, passive_function, number, line_number)  \
{                                                                             \
  action_function,                                                            \
  passive_function,                                                           \
  NULL,                                                                       \
  &cheat_data_ptr[number],                                                    \
  enable_disable_options,                                                     \
  &(Cheat.c[number].enabled),				                            	  \
  2,                                                                          \
  NULL,                                                                       \
  line_number,                                                                \
  STRING_SELECTION_TYPE | ACTION_TYPE                                         \
}                                                                             \

#define ACTION_OPTION(action_function, passive_function, display_string,      \
 help_string, line_number)                                                    \
{                                                                             \
  action_function,                                                            \
  passive_function,                                                           \
  NULL,                                                                       \
  display_string,                                                             \
  NULL,                                                                       \
  NULL,                                                                       \
  0,                                                                          \
  help_string,                                                                \
  line_number,                                                                \
  ACTION_TYPE                                                               \
}                                                                             \

#define SUBMENU_OPTION(sub_menu, display_string, help_string, line_number)    \
{                                                                             \
  NULL,                                                                       \
  NULL,                                                                       \
  sub_menu,                                                                   \
  display_string,                                                             \
  NULL,                                                                       \
  NULL,                                                                       \
  sizeof(sub_menu) / sizeof(MENU_OPTION_TYPE),                                \
  help_string,                                                                \
  line_number,                                                                \
  SUBMENU_TYPE                                                              \
}                                                                             \

#define SELECTION_OPTION(passive_function, display_string, options,           \
 option_ptr, num_options, help_string, line_number, type)                     \
{                                                                             \
  NULL,                                                                       \
  passive_function,                                                           \
  NULL,                                                                       \
  display_string,                                                             \
  options,                                                                    \
  option_ptr,                                                                 \
  num_options,                                                                \
  help_string,                                                                \
  line_number,                                                                \
  type                                                                        \
}                                                                             \

#define ACTION_SELECTION_OPTION(action_function, passive_function,            \
 display_string, options, option_ptr, num_options, help_string, line_number,  \
 type)                                                                        \
{                                                                             \
  action_function,                                                            \
  passive_function,                                                           \
  NULL,                                                                       \
  display_string,                                                             \
  options,                                                                    \
  option_ptr,                                                                 \
  num_options,                                                                \
  help_string,                                                                \
  line_number,                                                                \
  type | ACTION_TYPE                                                        \
}                                                                             \

#define STRING_SELECTION_OPTION(action_function, passive_function,            \
    display_string, options, option_ptr, num_options, help_string, action, line_number)\
{                                                                             \
  action_function,                                                            \
  passive_function,                                                           \
  NULL,                                                                       \
  display_string,                                                             \
  options,                                                                    \
  option_ptr,                                                                 \
  num_options,                                                                \
  help_string,                                                                \
  line_number,                                                                \
  STRING_SELECTION_TYPE | action                                              \
}

#define NUMERIC_SELECTION_OPTION(passive_function, display_string,            \
 option_ptr, num_options, help_string, line_number)                           \
  SELECTION_OPTION(passive_function, display_string, NULL, option_ptr,        \
   num_options, help_string, line_number, NUMBER_SELECTION_TYPE)              \

#define STRING_SELECTION_HIDEN_OPTION(action_function, passive_function,      \
 display_string, options, option_ptr, num_options, help_string, line_number)  \
  ACTION_SELECTION_OPTION(action_function, passive_function,                  \
   display_string,  options, option_ptr, num_options, help_string,            \
   line_number, (STRING_SELECTION_TYPE | HIDEN_TYPE))                         \

#define NUMERIC_SELECTION_ACTION_OPTION(action_function, passive_function,    \
 display_string, option_ptr, num_options, help_string, line_number)           \
  ACTION_SELECTION_OPTION(action_function, passive_function,                  \
   display_string,  NULL, option_ptr, num_options, help_string,               \
   line_number, NUMBER_SELECTION_TYPE)                                        \

#define NUMERIC_SELECTION_HIDE_OPTION(action_function, passive_function,      \
    display_string, option_ptr, num_options, help_string, line_number)        \
  ACTION_SELECTION_OPTION(action_function, passive_function,                  \
   display_string, NULL, option_ptr, num_options, help_string,                \
   line_number, NUMBER_SELECTION_TYPE)                                        \


typedef enum
{
	NUMBER_SELECTION_TYPE = 0x01,
	STRING_SELECTION_TYPE = 0x02,
	SUBMENU_TYPE          = 0x04,
	ACTION_TYPE           = 0x08,
	HIDEN_TYPE            = 0x10,
	PASSIVE_TYPE          = 0x00,
} MENU_OPTION_TYPE_ENUM;

struct _MENU_OPTION_TYPE
{
	void (* action_function)();				//Active option to process input
	void (* passive_function)();			//Passive function to process this option
	struct _MENU_TYPE *sub_menu;			//Sub-menu of this option
	char **display_string;					//Name and other things of this option
	void *options;							//output value of this option
	u32 *current_option;					//output values
	u32 num_options;						//Total output values
	char **help_string;						//Help string
	u32 line_number;						//Order id of this option in it menu
	MENU_OPTION_TYPE_ENUM option_type;		//Option types
};

struct _MENU_TYPE
{
	void (* init_function)();				//Function to initialize this menu
	void (* passive_function)();			//Function to draw this menu
	void (* key_function)();				//Function to process input
	void (* end_function)();				//End process of this menu
	struct _MENU_OPTION_TYPE *options;		//Options array
	u32	num_options;						//Total options of this menu
	u32	focus_option;						//Option which obtained focus
	u32	screen_focus;						//screen positon of the focus option
};

typedef struct _MENU_OPTION_TYPE MENU_OPTION_TYPE;
typedef struct _MENU_TYPE MENU_TYPE;

/******************************************************************************
 ******************************************************************************/
char g_default_rom_dir[MAX_PATH];
char DEFAULT_RTS_DIR[MAX_PATH];
char DEFAULT_CFG_DIR[MAX_PATH];
char DEFAULT_SS_DIR[MAX_PATH];
char DEFAULT_CHEAT_DIR[MAX_PATH];
u32 game_fast_forward= 0;
u32 game_enable_audio = 1;




/******************************************************************************
 ******************************************************************************/
static u32 menu_cheat_page = 0;
u32 clock_speed_number = 5;
u32 gamepad_config_menu;

/******************************************************************************
 ******************************************************************************/
static void get_savestate_filelist(void);
static FILE* get_savestate_snapshot(char *savestate_filename);
static void get_savestate_filename(u32 slot, char *name_buffer);
static u32 get_savestate_slot(void);
static void reorder_savestate_slot(void);
void get_newest_savestate(char *name_buffer);
static int sort_function(const void *dest_str_ptr, const void *src_str_ptr);
static u32 parse_line(char *current_line, char *current_str);
static void get_timestamp_string(char *buffer, u16 msg_id, u16 year, u16 mon, u16 day, u16 wday, u16 hour, u16 min, u16 sec, u32 msec);
static void get_time_string(char *buff, struct rtc *rtcp);
static u32 save_ss_bmp(u16 *image);
static void init_game_config(void);
static void init_emulator_config(void);
static void load_game_config_file(void);
static int load_emu_config_file(void);
static int save_game_config_file(void);
static int save_emu_config_file(void);
static void reorder_latest_file(void);
static void quit(void);

/*--------------------------------------------------------
	Get GUI input
--------------------------------------------------------*/
gui_action_type get_gui_input(void)
{
	unsigned int key;
	gui_action_type	ret;

	key = getKey();

	switch(key)
	{
		case KEY_UP:
			ret = CURSOR_UP;
			break;
		case KEY_DOWN:
			ret = CURSOR_DOWN;
			break;
		case KEY_LEFT:
			ret = CURSOR_LEFT;
			break;
		case KEY_RIGHT:
			ret = CURSOR_RIGHT;
			break;
		case KEY_L:
			ret = CURSOR_LTRIGGER;
			break;
		case KEY_R:
			ret = CURSOR_RTRIGGER;
			break;
		case KEY_A:
			ret = CURSOR_SELECT;
			break;
		case KEY_B:
			ret = CURSOR_BACK;
			break;
		case KEY_X:
			ret = CURSOR_EXIT;
			break;
		case KEY_TOUCH:
			ret = CURSOR_TOUCH;
			break;
		default:
			ret = CURSOR_NONE;
			break;
	}

	return ret;
}

/*--------------------------------------------------------
	Wait any key in [key_list] pressed
	if key_list == NULL, return at any key pressed
--------------------------------------------------------*/
unsigned int wait_Anykey_press(unsigned int key_list)
{
	unsigned int key;

	while(1)
	{
		key = getKey();
		if(key) {
			if(0 == key_list) break;
			else if(key & key_list) break;
		}
	}

	return key;
}

/*--------------------------------------------------------
	Wait all key in [key_list] released
	if key_list == NULL, return at all key released
--------------------------------------------------------*/
void wait_Allkey_release(unsigned int key_list)
{
	unsigned int key;
    struct key_buf inputdata;

	while(1)
	{
		ds2_getrawInput(&inputdata);
		key = inputdata.key;

		if(0 == key) break;
		else if(!key_list) continue;
		else if(0 == (key_list & key)) break;
	}
}

void change_ext(char *src, char *buffer, char *extension)
{
	char *dot_position;

	strcpy(buffer, src);
	dot_position = strrchr(buffer, '.');

	if(dot_position)
		strcpy(dot_position, extension);
}

/*--------------------------------------------------------
	Sort function
--------------------------------------------------------*/
static int sort_function(const void *dest_str_ptr, const void *src_str_ptr)
{
	char *dest_str = *((char **)dest_str_ptr);
	char *src_str = *((char **)src_str_ptr);

	// For files and directories, . and .. sort first.
	if(src_str[0] == '.')
		return 1;

	if(dest_str[0] == '.')
		return -1;

	return strcasecmp(dest_str, src_str);
}

static int my_array_partion(void *array, int left, int right)
{
    unsigned int pivot= *((unsigned int*)array + left);

    while(left < right)
    {
        while(sort_function((void*)((unsigned int*)array+left), (void*)((unsigned int *)array+right)) < 0) {
            right--;
        }

        if(right== left) break;
        *((unsigned int*)array + left) = *((unsigned int*)array + right);
        *((unsigned int*)array + right) = pivot;

        if(left < right)
        {
            left++;
            if(right== left) break;
        }

        while(sort_function((void*)((unsigned int*)array+right), (void*)((unsigned int *)array+left)) > 0) {
            left++;
        }

        if(left== right) break;
        *((unsigned int*)array + right) = *((unsigned int*)array + left);
        *((unsigned int*)array + left) = pivot;
        right--;
    }

    return left;
}

static void my_qsort(void *array, int left, int right)
{
    if(left < right)
    {
        int mid= my_array_partion(array, left, right);
        my_qsort(array, left, mid-1);
        my_qsort(array, mid+1, right);
    }
}

static void strupr(char *str)
{
    while(*str)
    {
        if(*str <= 0x7A && *str >= 0x61) *str -= 0x20;
        str++;
    }
}

// ******************************************************************************
//	get file list
// ******************************************************************************
#define FILE_LIST_MAX  512
#define DIR_LIST_MAX   64
#define NAME_MEM_SIZE  (320*64)

struct  FILE_LIST_INFO
{
    char current_path[MAX_PATH];
    char **wildcards;
    unsigned int file_num;
    unsigned int dir_num;
	unsigned int mem_size;
	unsigned int file_size;
	unsigned int dir_size;
    char **file_list;
    char **dir_list;
    char *filename_mem;
};

/*
*	Function: internal function to manage FILE_LIST_INFO structure
*	filelist_infop: a pointer to a predefined FILE_LIST_INFO structure
*	flag: 0	initialize the structure
*			1 increase filename memory size
*			2 increase file name buffer size
*			4 increase directory name buffer size
*			-1 free all allocated memroy
*			other value are invalide
*	return: -1 on failure
*/
static int manage_filelist_info(struct FILE_LIST_INFO *filelist_infop, int flag)
{
	//Initialize
	if(0 == flag)
	{
		filelist_infop->file_list = (char**)malloc(FILE_LIST_MAX*4);
		if(NULL == filelist_infop->file_list)
			return -1;

		filelist_infop->dir_list = (char**)malloc(DIR_LIST_MAX*4);
		if(NULL == filelist_infop->dir_list)
		{
			free((void*)filelist_infop->file_list);
			return -1;
		}

		filelist_infop->filename_mem = (char*)malloc(NAME_MEM_SIZE);
		if(NULL == filelist_infop->filename_mem)
		{
			free((void*)filelist_infop->file_list);
			free((void*)filelist_infop->dir_list);
			return -1;
		}

		filelist_infop->mem_size = NAME_MEM_SIZE;
		filelist_infop->file_size = FILE_LIST_MAX;
		filelist_infop->dir_size = DIR_LIST_MAX;
		return 0;
	}
	//free all memroy
	if(-1 == flag)
	{
		free((void*)filelist_infop->file_list);
		free((void*)filelist_infop->dir_list);
		free((void*)filelist_infop->filename_mem);
		return 0;
	}

	int i;
	void *pt;

	//Increase all 
	if(flag & 0x1)
	{
		i = NAME_MEM_SIZE;

		do
		{
			pt = (void*)realloc(filelist_infop->filename_mem, filelist_infop->mem_size+i);
			if(NULL == pt) i /= 2;
		} while(i > 256);

		if(NULL == pt) return -1;

		filelist_infop->mem_size += i;
		filelist_infop->filename_mem = (char*)pt;
		i = (int)pt - (int)filelist_infop->file_list;

		int k;
		char **m;

		k = 0;
		m = filelist_infop->file_list;
		for(k= 0; k < filelist_infop->file_num; k++)
			m[k] += i;

		k = 0;
		m = filelist_infop->dir_list;
		for(k = 0; k < filelist_infop->dir_num; k++)
			m[k] += i;
	}
	//Increase file name buffer
	if(flag & 0x2)
	{
		i = filelist_infop->file_size + FILE_LIST_MAX;

		pt = (void*)realloc(filelist_infop->file_list, i*4);
		if(NULL == pt) return -1;

		filelist_infop->file_list = (char**)pt;
		filelist_infop->file_size = i;
	}
	//Increase directory name buffer
	if(flag & 0x4)
	{
		i = filelist_infop->dir_size + DIR_LIST_MAX;

		pt = (void*)realloc(filelist_infop->dir_list, i*4);
		if(NULL == pt) return -1;

		filelist_infop->dir_list = (char**)pt;
		filelist_infop->dir_size = i;
	}

	return 0;
}

static int load_file_list(struct FILE_LIST_INFO *filelist_infop)
{
    DIR* current_dir;
    char    current_dir_name[MAX_PATH];
    dirent *current_file;
	struct stat st;
    char *file_name;
	unsigned int len;
    unsigned int file_name_length;
    char* name_mem_base;
    char **file_list;
    char **dir_list;
	unsigned int mem_size;
	unsigned int file_size;
	unsigned int dir_size;
    unsigned int num_files;
    unsigned int num_dirs;
    char **wildcards;
	char utf8[512+256];

    if(filelist_infop -> current_path == NULL)
        return -1;

    name_mem_base = &(filelist_infop -> filename_mem[0]);
    mem_size = filelist_infop -> mem_size;
	file_size = filelist_infop -> file_size;
	dir_size = filelist_infop -> dir_size;
    file_list = filelist_infop -> file_list;
    dir_list = filelist_infop -> dir_list;
    num_files = 0;
    num_dirs = 0;
    wildcards = filelist_infop -> wildcards;

    strcpy(current_dir_name, filelist_infop -> current_path);

	//	path formate should be: "fat:/" or "fat:/dir0" or "fat:", not "fat:/dir0/"
    current_dir = opendir(current_dir_name);
	//Open directory faiure
	if(current_dir == NULL) {
		return -1;
	}

	file_name_length = 0;
	//while((current_file = readdir(current_dir)) != NULL)
	while((current_file = readdir_ex(current_dir, &st)) != NULL)
    {
		//lstat(current_file->d_name, &st);
		file_name = current_file->d_name;

		len = strlen(file_name) +1;
		if((file_name_length+len) > mem_size)
		{
			//get more memory
			if(manage_filelist_info(filelist_infop, 1) == -1)
				break;

			name_mem_base = &(filelist_infop -> filename_mem[0]);
		    mem_size = filelist_infop -> mem_size;
		}
		//Increase file_list
		if(num_files >= file_size) {
			if(manage_filelist_info(filelist_infop, 2) == -1)
				break;
			file_size = filelist_infop -> file_size;
		}
		//Increase dir_list
		if(num_dirs >= dir_size) {
			if(manage_filelist_info(filelist_infop, 4) == -1)
				break;
			dir_size = filelist_infop -> dir_size;
		}

		//If dirctory
		if(S_ISDIR(st.st_mode))
		{
			if(file_name[0] != '.')
			{
				dir_list[num_dirs] = name_mem_base + file_name_length;
				strcpy(dir_list[num_dirs], file_name);
				num_dirs++;
				file_name_length += len;
			}
			//take ".." directory as file
			else if(file_name[1] == '.')
			{
				file_list[num_files] = name_mem_base + file_name_length;
				strcpy(file_list[num_files], file_name);
				num_files++;
				file_name_length += len;
			}
		}
		else
		{
			char *ext_pos;
			unsigned int i;

			ext_pos = (char*)strrchr((const char*)file_name, '.');
			if(NULL != ext_pos)
			for(i = 0; wildcards[i] != NULL; i++)
			{
				if(!strcasecmp(ext_pos, wildcards[i]))
				{
					file_list[num_files] = name_mem_base + file_name_length;
					strcpy(file_list[num_files], file_name);
					num_files++;
					file_name_length += len;
					break;
				}
			}
		}
    }

    closedir(current_dir);

	filelist_infop -> file_num = num_files;
	filelist_infop -> dir_num = num_dirs;
	//printf("%s: num_files %d; num_dirs %d\n", filelist_infop ->current_path, num_files, num_dirs);
#if 0
    my_qsort((void *)file_list, 0, num_files-1);
#else	//to support ".." directory, but take it as file
    my_qsort((void *)file_list, 1, num_files-1);
#endif
    my_qsort((void *)dir_list, 0, num_dirs-1);

    return 0;
}

/*--------------------------------------------------------
  读取文件
--------------------------------------------------------*/
#define FILE_LIST_ROWS 6
#define FILE_LIST_ROWS_CENTER   ((FILE_LIST_ROWS+1)/2-1)
#define FILE_LIST_POSITION 42		//Started displaying x position

s32 load_file(char **wildcards, char *result, char *default_dir_name)
{
	struct FILE_LIST_INFO filelist_info;
	u32 repeat;
	int return_value;
	gui_action_type gui_action;
	u32 selected_item_on_list;
	u32 selected_item_on_screen;
	u32 to_update_filelist;
	u32 total_items_num;
	int redraw;
    u32 path_scroll;
    u32 num_files;						//File numbers on the directory
    u32 num_dirs;                       //Directory numbers on the dirctory
    char **file_list;
    char **dir_list;
    int flag;

	//Have no path
	if(NULL == default_dir_name)
		return -1;

	//construct filelist_info struct
	if(-1 == manage_filelist_info(&filelist_info, 0))
		return -1;

	result[0] = '\0';
	strcpy(filelist_info.current_path, default_dir_name);

	filelist_info.wildcards = wildcards;
	filelist_info.file_num = 0;
	filelist_info.dir_num = 0;

	flag = load_file_list(&filelist_info);
	if(-1 == flag)
	{
		//deconstruct filelist_info struct
		manage_filelist_info(&filelist_info, -1);
		return -1;
	}

	num_files = filelist_info.file_num;
	num_dirs = filelist_info.dir_num;
	file_list = filelist_info.file_list;
	dir_list = filelist_info.dir_list;

	selected_item_on_list = 0;			//Selected item on list
	selected_item_on_screen = 0;		//Selected item on screen's position
	redraw = -1;						//redraw all
	total_items_num = num_files + num_dirs;
	to_update_filelist = 0;             //to load new file list
	path_scroll = 0x8000;				//path scroll method, first scroll left

	repeat= 1;
	return_value = -1;
	while(repeat)
	{
		struct key_buf inputdata;
		gui_action = get_gui_input();
		int mod;

		switch(gui_action)
		{
			case CURSOR_TOUCH:
				ds2_getrawInput(&inputdata);
				// ___ 33        This screen has 6 possible rows. Touches
				// ___ 60        above or below these are ignored.
				// . . . (+27)
				// ___ 192
				if(inputdata.y <= 33 || inputdata.y > 192)
					break;
				else
					mod = (inputdata.y - 33) / 27;

				if(selected_item_on_list - selected_item_on_screen + mod >= total_items_num)
					break;

				selected_item_on_list = selected_item_on_list - selected_item_on_screen + mod;

				if(selected_item_on_list + 1 <= num_files)
				{
					//The ".." directory is the parent
					if(!strcasecmp(file_list[selected_item_on_list], ".."))
					{
						char *ext_pos;

						strcpy(filelist_info.current_path, default_dir_name);
						ext_pos = strrchr(filelist_info.current_path, '/');
						if(NULL != ext_pos)	//Not root directory
						{
							*ext_pos = '\0';
							to_update_filelist= 1;
						}
					}
					else
					{
						repeat = 0;
						return_value = 0;
						strcpy(default_dir_name, filelist_info.current_path);
						strcpy(result, file_list[selected_item_on_list]);
					}
				}
				//directory selected
				else if(num_dirs > 0)
				{
					unsigned int m;

					m = selected_item_on_list - num_files;
					strcpy(filelist_info.current_path, default_dir_name);
					strcat(filelist_info.current_path, "/");
					strcat(filelist_info.current_path, dir_list[m]);
					to_update_filelist= 1;
				}
				break;
			case CURSOR_UP:
				redraw = 1;
				if(selected_item_on_screen > 0)
				{
					//Not the first item on list
					selected_item_on_list -= 1;
					//Selected item on screen center
					if(FILE_LIST_ROWS_CENTER == selected_item_on_screen) {
						if(selected_item_on_screen > selected_item_on_list)
							selected_item_on_screen -= 1;
					}
					//Selected item on down half screen
					else if(FILE_LIST_ROWS_CENTER < selected_item_on_screen) {
						selected_item_on_screen -= 1;
					}
					//Selected item on up half screen
					else {
						if(selected_item_on_screen < selected_item_on_list)
							selected_item_on_screen += 1;
						else if(selected_item_on_screen > selected_item_on_list)
							selected_item_on_screen -= 1;
					}
				}
				else if(selected_item_on_screen > selected_item_on_list) {
					selected_item_on_screen -= 1;
				}
				else
					redraw = 0;

				break;

			case CURSOR_DOWN:
			{
				unsigned int m, n;
				m = total_items_num - (selected_item_on_list + 1);
				n = FILE_LIST_ROWS - (FILE_LIST_ROWS_CENTER + 1);
				//Selected item on screen center
				if(FILE_LIST_ROWS_CENTER == selected_item_on_screen) {
					if(m > 0 && m < n)
						selected_item_on_screen += 1;
					redraw = 1;
				}
				//Selected item on down half screen
				else if(FILE_LIST_ROWS_CENTER < selected_item_on_screen) {
					if(m > 0) {
						redraw = 1;
						if(m <= n) selected_item_on_screen += 1;
						else if(m > n) selected_item_on_screen -= 1;
						else redraw = 0;	//cancel
					}
				}
				//Selected item on up half screen
				else {
					if(m > 0) {
						selected_item_on_screen += 1;
						redraw = 1;
					}
				}

				//Not reach the last item
				if((selected_item_on_list + 1) < total_items_num)
					selected_item_on_list += 1;

				break;
			}
			//scroll page down
			case CURSOR_RTRIGGER:
			{
				unsigned int m, n;
				//first item on screen in item list's position
				m = selected_item_on_list - selected_item_on_screen;
				n = total_items_num - (m+1);
				//there are more than 1 page after the m item
				if(n >= FILE_LIST_ROWS) {
					m += FILE_LIST_ROWS -1;
					selected_item_on_list = m;

					m = total_items_num - (selected_item_on_list +1);
					if(m >= (FILE_LIST_ROWS_CENTER +1))
						selected_item_on_screen = FILE_LIST_ROWS_CENTER;
					else
						selected_item_on_screen = 0;

					selected_item_on_list += selected_item_on_screen;
					redraw = 1;
				}

				break;
			}
			//scroll page up
			case CURSOR_LTRIGGER:
			{
				unsigned int m;
				//first item on screen in item list's position
				m = selected_item_on_list - selected_item_on_screen;
				//there are more than 1 page befroe the m item
				if((m+1) >= FILE_LIST_ROWS)
				{
					m -= FILE_LIST_ROWS -1;
					selected_item_on_list = m;

					selected_item_on_screen = FILE_LIST_ROWS_CENTER;
					selected_item_on_list += selected_item_on_screen;
					redraw = 1;
				}

				break;
			}
			//scroll string
			case CURSOR_RIGHT:
				redraw = -5;
				break;
			//scroll string
			case CURSOR_LEFT:
				redraw = 5;
				break;

			case CURSOR_SELECT:
				//file selected
				if(selected_item_on_list + 1 <= num_files)
				{
					//The ".." directory
					if(!strcasecmp(file_list[selected_item_on_list], ".."))
					{
						char *ext_pos;

						strcpy(filelist_info.current_path, default_dir_name);
						ext_pos = strrchr(filelist_info.current_path, '/');
						if(NULL != ext_pos)	//Not root directory
						{
							*ext_pos = '\0';
							to_update_filelist= 1;
						}
					}
					else
					{
						repeat = 0;
						return_value = 0;
						strcpy(default_dir_name, filelist_info.current_path);
						strcpy(result, file_list[selected_item_on_list]);
					}
				}
				//directory selected
				else if(num_dirs > 0)
				{
					unsigned int m;

					m = selected_item_on_list - num_files;
					strcpy(filelist_info.current_path, default_dir_name);
					strcat(filelist_info.current_path, "/");
					strcat(filelist_info.current_path, dir_list[m]);
					to_update_filelist= 1;
				}
				break;

			case CURSOR_BACK:
			{
				char *ext_pos;

				strcpy(filelist_info.current_path, default_dir_name);
				ext_pos = strrchr(filelist_info.current_path, '/');
				if(NULL != ext_pos)	//Not root directory
				{
					*ext_pos = '\0';
					to_update_filelist= 1;
				}
				else { //is root directory
					return_value = -1;
					repeat = 0;
				}
				break;
			}

			case CURSOR_EXIT:
				return_value = -1;
				repeat = 0;
				break;

			default:
				break;
		} //end switch

		if(to_update_filelist)
		{
			flag = load_file_list(&filelist_info);
			if(-1 != flag) {
				strcpy(default_dir_name, filelist_info.current_path);
				num_files = filelist_info.file_num;
				num_dirs = filelist_info.dir_num;
				total_items_num = num_files + num_dirs;;

				selected_item_on_list = 0;			//Selected item on list
				selected_item_on_screen = 0;		//Selected item on screen's position

				redraw = -1;						//redraw all
			}
			to_update_filelist = 0;
		}

		//redraw, code reuse
		if(-1 == redraw || 1 == redraw)
		{
			unsigned int m, n, k;
			char *pt;
			unsigned short color;

			//draw background
			show_icon(down_screen_addr, ICON_SUBBG, 0, 0);
			show_icon(down_screen_addr, ICON_TITLE, 0, 0);
			show_icon(down_screen_addr, ICON_TITLEICON, 12, 9);

			//release data struct to draw scrolling string
			//Path
			if(-1 == redraw) {
				draw_hscroll_over(0);
				draw_hscroll_init(down_screen_addr, 49, 10, 170, COLOR_TRANS, 
					COLOR_WHITE, default_dir_name);
				path_scroll = 0x8000;		//first scroll left
			}

			for(m = 0; m < MAX_SCROLL_STRING; m++)
				draw_hscroll_over(m+1);

			//file and directory list
			//the first displayed item
			m = selected_item_on_list - selected_item_on_screen;
			//nubers of item to displayed
			n = total_items_num - m;
			if(n > FILE_LIST_ROWS) n = FILE_LIST_ROWS;

			for(k= 0; k < n; k++, m++)
			{
				if(k == selected_item_on_screen) {
					color = COLOR_ACTIVE_ITEM;
					show_icon(down_screen_addr, ICON_SUBSELA, 6, 35 + k*27);
				}
				else
					color = COLOR_INACTIVE_ITEM;

				//directorys
				if((m+1) > num_files) {
					show_icon(down_screen_addr, ICON_DIRECTORY, 17, 37 + k*27);
					pt = dir_list[m - num_files];
				}
				//files
				else {
					pt= strrchr(file_list[m], '.');

					if(!strcasecmp(pt, ".smc") || !strcasecmp(pt, ".sfc"))
						show_icon(down_screen_addr, ICON_SFCFILE, 17, 37 + k*27);
					else if(!strcasecmp(pt, ".zip"))
						show_icon(down_screen_addr, ICON_ZIPFILE, 17, 37 + k*27);
					else if(!strcasecmp(pt, ".cht"))
						show_icon(down_screen_addr, ICON_CHTFILE, 17, 37 + k*27);
					else if(!strcasecmp(file_list[m], ".."))
						show_icon(down_screen_addr, ICON_DOTDIR, 17, 37 + k*27);
					else //Not recoganized file
						show_icon(down_screen_addr, ICON_UNKNOW, 17, 37 + k*27);

					pt = file_list[m];
				}

				draw_hscroll_init(down_screen_addr, 41, 40 + k*27, 185, 
					COLOR_TRANS, color, pt);
			}

			redraw = 0;
			ds2_flipScreen(DOWN_SCREEN, DOWN_SCREEN_UPDATE_METHOD);
		} //end if(0 != redraw)
		else if(0 != redraw) {
			unsigned int m, n;
			char *pt;

			m = selected_item_on_screen;
			show_icon(down_screen_addr, ICON_SUBSELA, 6, 35 + m*27);

			n = selected_item_on_list;
			if((n+1) > num_files)
				show_icon(down_screen_addr, ICON_DIRECTORY, 17, 37 + m*27);
			else {
				pt= strrchr(file_list[n], '.');

				if(!strcasecmp(pt, ".smc"))
					show_icon(down_screen_addr, ICON_SFCFILE, 17, 37 + m*27);
				else if(!strcasecmp(pt, ".zip"))
					show_icon(down_screen_addr, ICON_ZIPFILE, 17, 37 + m*27);
				else if(!strcasecmp(pt, ".cht"))
					show_icon(down_screen_addr, ICON_CHTFILE, 17, 37 + m*27);
				else if(!strcasecmp(file_list[m], ".."))
					show_icon(down_screen_addr, ICON_DOTDIR, 17, 37 + m*27);
				else //Not recoganized file
					show_icon(down_screen_addr, ICON_UNKNOW, 17, 37 + m*27);
			}

			draw_hscroll(m+1, redraw);
			ds2_flipScreen(DOWN_SCREEN, DOWN_SCREEN_UPDATE_METHOD);
			redraw = 0;
		}

		//Path auto scroll
		unsigned int m = path_scroll & 0xFF;
		if(m < 20) //pause 0.5sec
			path_scroll += 1;
		else {
			show_icon(down_screen_addr, ICON_TITLE, 0, 0);
			show_icon(down_screen_addr, ICON_TITLEICON, 12, 9);

			if(path_scroll & 0x8000)	//scroll left
			{
				if(draw_hscroll(0, -1) <= 1) path_scroll = 0;	 //scroll right
			}
			else
			{
				if(draw_hscroll(0, 1) <= 1) path_scroll = 0x8000; //scroll left
			}
			ds2_flipScreen(DOWN_SCREEN, DOWN_SCREEN_UPDATE_METHOD);
		}

		mdelay(50);			//about 50ms
	} //end while(repeat)

	unsigned int i;
	for(i= 0; i < (FILE_LIST_ROWS +1); i++)
		draw_hscroll_over(i);

	//deconstruct filelist_info struct
	manage_filelist_info(&filelist_info, -1);

	ds2_clearScreen(DOWN_SCREEN, COLOR_BLACK);
	ds2_flipScreen(DOWN_SCREEN, DOWN_SCREEN_UPDATE_METHOD);

	return return_value;
}

/*--------------------------------------------------------
  放映幻灯片
--------------------------------------------------------*/
u32 play_screen_snapshot(void)
{
    struct FILE_LIST_INFO filelist_info;
    char *file_ext[] = { ".bmp", NULL };
    u32 file_num;
    char **file_list;
    s32 flag;
    u32 repeat, i;
    u16 *screenp;
    u32 color_bg;

    screenp= (u16*)malloc(256*192*2);
    if(screenp == NULL)
    {
        screenp = (u16*)down_screen_addr;
        color_bg = COLOR_BG;
    }
    else
    {
        memcpy(screenp, down_screen_addr, 256*192*2);
        color_bg = COLOR16(43, 11, 11);
    }

	//construct filelist_info struct
	if(-1 == manage_filelist_info(&filelist_info, 0))
		return -1;

	strcpy(filelist_info.current_path, DEFAULT_SS_DIR);

	filelist_info.wildcards = file_ext;
	filelist_info.file_num = 0;
	filelist_info.dir_num = 0;

	flag = load_file_list(&filelist_info);
	if(-1 == flag)
	{
		//construct filelist_info struct
		manage_filelist_info(&filelist_info, -1);
		return -1;
	}

    flag = load_file_list(&filelist_info);
    file_num = filelist_info.file_num;
    file_list = filelist_info.file_list;

    if(flag < 0 || file_num== 0)
    {
        draw_message(down_screen_addr, screenp, 28, 31, 227, 165, color_bg);
        draw_string_vcenter(down_screen_addr, 36, 55, 190, COLOR_MSSG, msg[MSG_NO_SLIDE]);
        ds2_flipScreen(DOWN_SCREEN, DOWN_SCREEN_UPDATE_METHOD);

        if(screenp) free((void*)screenp);
        //construct filelist_info struct
		manage_filelist_info(&filelist_info, -1);

        if(draw_yesno_dialog(DOWN_SCREEN, 115, "Yes(A)", "No(A)"))
            return 1;
        else 
            return 0;
    }

    char bmp_path[MAX_PATH];
	BMPINFO SbmpInfo;

    u32 buff[256*192];
    u32 time0= 10;
    u32 pause= 0;
	unsigned int type;

    draw_message(down_screen_addr, screenp, 28, 31, 227, 165, color_bg);
    draw_string_vcenter(down_screen_addr, 36, 70, 190, COLOR_MSSG, msg[MSG_PLAY_SLIDE1]);
    draw_string_vcenter(down_screen_addr, 36, 85, 190, COLOR_MSSG, msg[MSG_PLAY_SLIDE2]);
    draw_string_vcenter(down_screen_addr, 36, 100, 190, COLOR_MSSG, msg[MSG_PLAY_SLIDE3]);
    draw_string_vcenter(down_screen_addr, 36, 115, 190, COLOR_MSSG, msg[MSG_PLAY_SLIDE4]);
    draw_string_vcenter(down_screen_addr, 36, 130, 190, COLOR_MSSG, msg[MSG_PLAY_SLIDE5]);
    draw_string_vcenter(down_screen_addr, 36, 145, 190, COLOR_MSSG, msg[MSG_PLAY_SLIDE6]);
    ds2_flipScreen(DOWN_SCREEN, DOWN_SCREEN_UPDATE_METHOD);

    repeat= 1;
    i= 0;
    while(repeat)
    {
        sprintf(bmp_path, "%s/%s", filelist_info.current_path, file_list[i]);

		flag = openBMP(&SbmpInfo, (const char*)bmp_path);
		if(flag == BMP_OK)
		{
			type = SbmpInfo.bmpHead.bfImghead.imBitpixel >>3;
			if(2 == type || 3 == type)	//2 bytes per pixel
			{
			    u16 *dst, *dst0;
			    u32 w, h, x, y;

				w= SbmpInfo.bmpHead.bfImghead.imBitmapW;
				h= SbmpInfo.bmpHead.bfImghead.imBitmapH;
				if(w > 256) w = 256;
				if(h > 192) h = 192;

				flag = readBMP(&SbmpInfo, 0, 0, w, h, (void*)buff);
				if(0 == flag)
				{
					ds2_clearScreen(UP_SCREEN, 0);

					dst= (unsigned short*)up_screen_addr + (192-h)/2*256 +(256-w)/2;
					dst0 = dst;
					if(2 == type) {
						unsigned short* pt;

						pt = (unsigned short*) buff;
						for(y= 0; y<h; y++) {
	    	            	for(x= 0; x < w; x++) {
		    	                *dst++ = RGB16_15(pt);
		    	                pt += 1;
		    	            }
							dst0 += 256;
							dst = dst0;
						}
					}
					else {
						unsigned char* pt;

						pt= (char*)buff;
	    		        for(y= 0; y< h; y++) {
    			            for(x= 0; x < w; x++) {
    			                *dst++ = RGB24_15(pt);
    			                pt += 3;
    			            }
							dst0 += 256;
							dst = dst0;
    			        }
					}

					ds2_flipScreen(UP_SCREEN, 1);
				}

				closeBMP(&SbmpInfo);
			}
			else
				closeBMP(&SbmpInfo);
		}

        if(i+1 < file_num) i++;
        else i= 0;

        gui_action_type gui_action;
        u32 ticks= 0;
        u32 time1;

        time1= time0;
        while(ticks < time1)
        {
            gui_action = get_gui_input();

            switch(gui_action)
            {
                case CURSOR_UP:
                    if(!pause)
                    {
                        if(time0 > 1) time0 -= 1;
                        time1= time0;
                    }
                    break;
                    
                case CURSOR_DOWN:
                    if(!pause)
                    {
                        time0 += 1;
                        time1= time0;
                    }
                    break;
                    
                case CURSOR_LEFT:
                    time1 = ticks;
                    if(i > 1) i -= 2;
                    else if(i == 1) i= file_num -1;
                    else i= file_num -2;
                    break;
                
                case CURSOR_RIGHT:
                    time1 = ticks;
                    break;
                
                case CURSOR_SELECT:
                    if(!pause)
                    { 
                        time1 = -1;
                        pause= 1;
                    }
                    else
                    {
                        time1 = ticks;
                        pause= 0;
                    } 
                    break;
                    
                case CURSOR_BACK: 
                    if(screenp) free((void*)screenp);
                    //deconstruct filelist_info struct
					manage_filelist_info(&filelist_info, -1);
					repeat = 0;
					break;                    

                default: gui_action= CURSOR_NONE;
                    break;
            }

			if(gui_action != CURSOR_BACK)
				mdelay(100);
            if(!pause)
                ticks ++;
        }
    }

	return 0;
}


/*
*	Function: search directory on directory_path
*	directory: directory name will be searched
*	directory_path: path, note that the buffer which hold directory_path should
*		be large enough for nested
*	return: 0= found, directory lay on directory_path
*/
int search_dir(char *directory, char* directory_path)
{
    DIR *current_dir;
    dirent *current_file;
	struct stat st;
    int directory_path_len;

    current_dir= opendir(directory_path);
    if(current_dir == NULL)
        return -1;

	directory_path_len = strlen(directory_path);

	//while((current_file = readdir(current_dir)) != NULL)
	while((current_file = readdir_ex(current_dir, &st)) != NULL)
	{
		//Is directory 
		if(S_ISDIR(st.st_mode))
		{
			if(strcmp(".", current_file->d_name) || strcmp("..", current_file->d_name))
				continue;

			strcpy(directory_path+directory_path_len, current_file->d_name);

			if(!strcasecmp(current_file->d_name, directory))
			{//dirctory find
				closedir(current_dir);
				return 0;
			}

			if(search_dir(directory, directory_path) == 0)
			{//dirctory find
				closedir(current_dir);
				return 0;
			}

			directory_path[directory_path_len] = '\0';
		}
    }

    closedir(current_dir);
    return -1;
}


const u32 gamepad_config_map_init[MAX_GAMEPAD_MAP] =
{
    BUTTON_ID_A,        /* 0    A */
    BUTTON_ID_B,        /* 1    B */
    BUTTON_ID_SELECT,   /* 2    [SELECT] */
    BUTTON_ID_START,    /* 3    [START] */
    BUTTON_ID_RIGHT,    /* 4    → */
    BUTTON_ID_LEFT,     /* 5    ← */
    BUTTON_ID_UP,       /* 6    ↑ */
    BUTTON_ID_DOWN,     /* 7    ↓ */
    BUTTON_ID_R,        /* 8    [R] */
    BUTTON_ID_L,        /* 9    [L] */
    BUTTON_ID_NONE,     /* 10   FA */
    BUTTON_ID_NONE,     /* 11   FB */
    BUTTON_ID_TOUCH,    /* 12   MENU */
    BUTTON_ID_NONE,     /* 13   NONE */
    BUTTON_ID_NONE,     /* 14   NONE */
    BUTTON_ID_NONE      /* 15   NONE */
};

u32 gamepad_config_map[MAX_GAMEPAD_MAP];

#define BUTTON_MAP_A    gamepad_config_map[0]
#define BUTTON_MAP_B    gamepad_config_map[1]
#define BUTTON_MAP_FA   gamepad_config_map[10]
#define BUTTON_MAP_FB   gamepad_config_map[11]
#define BUTTON_MAP_MENU gamepad_config_map[12]

int load_state(char* file)
{
	int flag;
	FILE* fp;
	unsigned int n;
	char tmp_path[MAX_PATH];

	sprintf(tmp_path, "%s/%s", DEFAULT_RTS_DIR, file);

	flag = game_load_state(tmp_path);
	if(0 != flag)
		return -1;

	fp = fopen(tmp_path, "r");
	if(NULL == fp)
		return -1;

	n = 0;
	if(fread((void*)&n, 1, 4, fp) < 4)
	{
		fclose(fp);
		return -1;
	}

	fseek(fp, n+sizeof(struct rtc), SEEK_SET);
	fread(up_screen_addr, 1, 256*192*2, fp);
	fclose(fp);

	ds2_flipScreen(UP_SCREEN, 1);

	return 0;
}

int load_game_stat_snapshot(char* file)
{
	FILE* fp;
	char tmp_path[MAX_PATH];
	unsigned int n, m;

	sprintf(tmp_path, "%s/%s", DEFAULT_RTS_DIR, file);
	fp = fopen(tmp_path, "r");
	if(NULL == fp)
		return -1;

	m = fread((void*)&n, 1, 4, fp);
	if(m < 4)
	{
		fclose(fp);
		return - 2;
	}

	fseek(fp, n+sizeof(struct rtc), SEEK_SET);

	m = fread(up_screen_addr, 1, 256*192*2, fp);
	if(m < 256*192*2)
	{
		fclose(fp);
		return -4;
	}

	fclose(fp);
	ds2_flipScreen(UP_SCREEN, 1);
	return 0;
}

#if 0
int bak_file(char* file)
{
	FILE *fp1, *fp2;
	char tmp_path[MAX_PATH];
	char buff[512];
	int m;

	fp1= fopen(file, "r");
	if(NULL == fp1)
		return -1;

	strcpy(tmp_path, file);
	strcat(tmp_path, ".bak");
	fp2 = fopen(tmp_path, "w");
	if(NULL == fp2)
	{
		fclose(fp1);
		return -2;
	}

	while(1)
	{
		m= fread(buff, 1, 512, fp1);
		if(m <= 0) break;
		fwrite(buff, 1, m, fp2);
	}

	fclose(fp1);
	fclose(fp2);
	return 0;
}
#endif

int save_state(char* file, void* screen)
{
	int flag;
	FILE *fp;
	unsigned int n;
	struct rtc time;
	char str[64];
	char tmp_path[MAX_PATH];

	sprintf(tmp_path, "%s/%s", DEFAULT_RTS_DIR, file);

	flag = game_save_state(tmp_path);
	if(0 != flag)
		return -1;

	fp = fopen(tmp_path, "r+");
	if(NULL == fp)
		return -1;

	fseek(fp, 0, SEEK_END);
	n = ftell(fp);

	ds2_getTime(&time);
    sprintf(str, "%02d-%02d %02d:%02d:%02d", 
		time.month, time.day, time.hours, time.minutes, time.seconds);

	PRINT_STRING_BG(screen, str, COLOR_WHITE, COLOR_BLACK, 0, 0);
	fwrite((void*)&time, 1, sizeof(struct rtc), fp);
	fwrite(screen, 1, 256*192*2, fp);

	fseek(fp, 0, SEEK_SET);
	fwrite((void*)&n, 1, 4, fp);

	fclose(fp);

	return 0;
}

void set_cpu_clock(u32 num)
{
	u32 clock_speed_table[6] = {6, 9, 10, 11, 12, 13};	//240, 300, 336, 360, 384, 394

	if(num <= 5)
		ds2_setCPUclocklevel(clock_speed_table[num]);
}

void savefast_int(void)
{

}

#if 1
void dump_mem(unsigned char* addr, unsigned int len)
{
	unsigned int i;

	for(i= 0; i < len; i++)
	{
		cprintf("%02x ", addr[i]);
		if((i+1)%16 == 0) cprintf("\n");
	}
}
#endif

unsigned int frame_interval;

/*--------------------------------------------------------
	Main Menu
--------------------------------------------------------*/
u32 menu(u16 *screen)
{
	mdelay(50); // to prevent the touch key from being applied too soon?
    gui_action_type gui_action;
    u32 i;
    u32 repeat;
    u32 return_value = 0;
    u32 first_load = 0;
    char tmp_filename[MAX_FILE];
    char line_buffer[512];
    char cheat_data_str[MAX_CHEATS_T][5];
    // ^ Holds the index inside Cheat, as a number in an ASCIIZ string
    char* cheat_data_ptr[MAX_CHEATS_T];

    MENU_TYPE *current_menu;
    MENU_OPTION_TYPE *current_option;
    MENU_OPTION_TYPE *display_option;
    
    u32 current_option_num;
//    u32 parent_option_num;
    u32 string_select;

    u16 *bg_screenp;
    u32 bg_screenp_color;

	auto void choose_menu();
	auto void menu_return();
	auto void menu_exit();
	auto void menu_load();
	auto void menu_restart();
	auto void menu_save_state();
	auto void menu_load_state();
	auto void menu_load_cheat_file();
	auto void others_menu_init();
	auto void keyremap_show();
	auto void main_menu_passive();
	auto void main_menu_key();
	auto void delette_savestate();
	auto void save_screen_snapshot();
	auto void browse_screen_snapshot();
	auto void keyremap();
	auto void time_backward_action();
	auto void time_period_passive();
	auto void time_period_action();
	auto void tools_menu_init();
	auto void load_default_setting();
	auto void check_gbaemu_version();
	auto void load_lastest_played();
	auto void latest_game_menu_passive();
	auto void latest_game_menu_init();
	auto void latest_game_menu_key();
	auto void latest_game_menu_end();
	auto void language_set();
	auto void game_fastforward();
	auto void show_card_space();
	auto void savestate_selitem(u32 sel, u32 y_pos);
	auto void game_state_menu_passive();
	auto void gamestate_delette_menu_passive();
	auto void cheat_menu_init();
	auto void cheat_menu_end();
	auto void reload_cheats_page();
	auto void cheat_option_action();
	auto void cheat_option_passive();
	auto void dynamic_cheat_menu_end();

//Local function definition

	void menu_exit()
	{
		ds2_setCPUclocklevel(13); // Crank it up, leave quickly
        if(gamepak_name[0] != 0)
        {
			game_config.clock_speed_number = clock_speed_number;

            reorder_latest_file();
			S9xAutoSaveSRAM ();
			save_game_config_file();
		}
		save_emu_config_file();
		quit();
	}

	void menu_load()
	{
		char *file_ext[] = { ".smc", ".sfc", ".zip", NULL };

		if(gamepak_name[0] != 0)
		{
			S9xAutoSaveSRAM ();
			save_game_config_file();
		}

		if(load_file(file_ext, tmp_filename, g_default_rom_dir) != -1)
		{
			strcpy(line_buffer, g_default_rom_dir);
			strcat(line_buffer, "/");
			strcat(line_buffer, tmp_filename);

			draw_message(down_screen_addr, bg_screenp, 28, 31, 227, 165, bg_screenp_color); 
			draw_string_vcenter(down_screen_addr, 36, 100, 190, COLOR_MSSG, msg[MSG_LOADING_GAME]);
			ds2_flipScreen(DOWN_SCREEN, DOWN_SCREEN_UPDATE_METHOD);

			ds2_setCPUclocklevel(13);
			int load_result = load_gamepak(&line_buffer);
			ds2_setCPUclocklevel(0);
			if(load_result == -1)
			{
				first_load = 1;
				gamepak_name[0] = '\0';
				return;
			}

			strcpy(gamepak_name, tmp_filename);
			first_load = 0;
			load_game_config_file();
//			time_period_action();

			return_value = 1;
			repeat = 0;

			reorder_latest_file();
			get_savestate_filelist();

			game_fast_forward= 0;
		}
		else
		{
			choose_menu(current_menu);
		}
	}
	
	bool Get_Args(char *file, char **filebuf)
	{
		FILE* dat = fat_fopen(file, "rb");
		if(dat)
		{
			int i = 0;
			while(!fat_feof (dat))
			{
				fat_fgets(filebuf[i], 512, dat);
				int len = strlen(filebuf[i]);
				if(filebuf[i][len - 1] == '\n')
					filebuf[i][len - 1] = '\0';
				i++;
			}
				
			fat_fclose(dat);
			fat_remove(file);
			return i;
		}
		return 0;
	}

	int CheckLoad_Arg()
	{
		char args[2][512];
		char *argarray[2];
		
		argarray[0] = args[0];
		argarray[1] = args[1];
		
		if(!Get_Args("/plgargs.dat", argarray))
			return 0;
	
		fat_remove("plgargs.dat");
	
		draw_message(down_screen_addr, bg_screenp, 28, 31, 227, 165, bg_screenp_color); 
		draw_string_vcenter(down_screen_addr, 36, 100, 190, COLOR_MSSG, msg[MSG_LOADING_GAME]);
		ds2_flipScreen(DOWN_SCREEN, DOWN_SCREEN_UPDATE_METHOD);

		ds2_setCPUclocklevel(13);
		int load_result = load_gamepak(args[1]);
		ds2_setCPUclocklevel(0);

		if(load_result == -1)
		{
			first_load = 1;
			gamepak_name[0] = '\0';
			return 0;
		}	

		strcpy(gamepak_name, args[1]);
		first_load = 0;
		load_game_config_file();

		return_value = 1;
		repeat = 0;

		reorder_latest_file();
		get_savestate_filelist();

		game_fast_forward= 0;
		return 1;		
	
	}

	void menu_restart()
	{
		if(!first_load)
		{
			game_restart();
			return_value = 1;
			repeat = 0;
		}
	}

	void menu_return()
	{
		if(!first_load)
			repeat = 0;
	}

	void clear_savestate_slot(u32 slot_index)
	{
		get_savestate_filename(slot_index, tmp_filename);
		sprintf(line_buffer, "%s/%s", DEFAULT_RTS_DIR, tmp_filename);
		remove(line_buffer);
		if(savestate_map[slot_index] > 0)
			savestate_map[slot_index]= -savestate_map[slot_index];
		reorder_savestate_slot();
	}

    void savestate_selitem(u32 selected, u32 y_pos)
    {
        u32 k;

        for(k= 0; k < 10; k++)	//Only display 10 slot
        {
			if(selected != k)
			{
				if(savestate_map[k] > 0)	//full
					show_icon((unsigned short*)down_screen_addr, ICON_NSTATEFULL, 28+k*21, y_pos);
				else
					show_icon((unsigned short*)down_screen_addr, ICON_NSTATEEMPTY, 28+k*21, y_pos);
			}
			else	//Highlight
			{
				if(savestate_map[k] > 0)	//full
					show_icon((unsigned short*)down_screen_addr, ICON_STATEFULL, 28+k*21, y_pos);
				else
					show_icon((unsigned short*)down_screen_addr, ICON_STATEEMPTY, 28+k*21, y_pos);
			}
        }
    }

	void game_state_menu_passive()
	{
		unsigned short color;
		unsigned int line[3] = {0, 1, 3};

		//draw background
		show_icon(down_screen_addr, ICON_SUBBG, 0, 0);
		show_icon(down_screen_addr, ICON_TITLE, 0, 0);
		show_icon(down_screen_addr, ICON_TITLEICON, 12, 9);

		if(current_option_num == 0)
			show_icon(down_screen_addr, ICON_BACK, 229, 10);
		else
			show_icon(down_screen_addr, ICON_NBACK, 229, 10);

		strcpy(line_buffer, *(display_option->display_string));
		draw_string_vcenter(down_screen_addr, 0, 9, 256, COLOR_ACTIVE_ITEM, line_buffer);

		display_option += 1;
        for(i= 0; i < 3; i++, display_option++)
        {
            unsigned short color;

			if(display_option == current_option)
				show_icon(down_screen_addr, ICON_SUBSELA, 6, 35 + line[i]*27);

			if(display_option->option_type & NUMBER_SELECTION_TYPE)
			{
				sprintf(line_buffer, *(display_option->display_string),
					*(display_option->current_option)+1);	//ADD 1 here
			}
			else if(display_option->option_type & STRING_SELECTION_TYPE)
			{
				sprintf(line_buffer, *(display_option->display_string),
					*((u32*)(((u32 *)display_option->options)[*(display_option->current_option)])));
			}
			else
			{
				strcpy(line_buffer, *(display_option->display_string));
			}

			if(display_option == current_option)
				color= COLOR_ACTIVE_ITEM;
			else
				color= COLOR_INACTIVE_ITEM;

			PRINT_STRING_BG(down_screen_addr, line_buffer, color, COLOR_TRANS, 23, 40 + line[i]*27);
        }

		int slot_index;
		unsigned int selected;

		selected = -1;
		//write save
        slot_index= get_savestate_slot();

        sprintf(line_buffer, "%d", (slot_index+2) > SAVE_STATE_SLOT_NUM ? SAVE_STATE_SLOT_NUM : (slot_index+2));
        PRINT_STRING_BG(down_screen_addr, line_buffer, COLOR_INACTIVE_ITEM, COLOR_TRANS, 146, 40 + 0*27);
		if(current_option_num == 1)
			selected = slot_index+1;

		//Read save
		if(current_option_num == 2)
			selected = savestate_index;

		savestate_selitem(selected, 93);
	}

    u32 delette_savestate_num= 0;
	void gamestate_delette_menu_passive()
	{
		unsigned short color;
		unsigned int line[2] = {0, 1};

		//draw background
		show_icon(down_screen_addr, ICON_SUBBG, 0, 0);
		show_icon(down_screen_addr, ICON_TITLE, 0, 0);
		show_icon(down_screen_addr, ICON_TITLEICON, 12, 9);

		if(current_option_num == 0)
			show_icon(down_screen_addr, ICON_BACK, 229, 6);
		else
			show_icon(down_screen_addr, ICON_NBACK, 229, 6);

		strcpy(line_buffer, *(display_option->display_string));
		draw_string_vcenter(down_screen_addr, 0, 9, 256, COLOR_ACTIVE_ITEM, line_buffer);

		display_option += 1;
        for(i= 0; i < 2; i++, display_option++)
        {
            unsigned short color;

			if(display_option == current_option)
				show_icon(down_screen_addr, ICON_SUBSELA, 6, 35 + line[i]*27);

			if(display_option->option_type & NUMBER_SELECTION_TYPE)
			{
				sprintf(line_buffer, *(display_option->display_string),
					*(display_option->current_option)+1);
			}
			else if(display_option->option_type & STRING_SELECTION_TYPE)
			{
				sprintf(line_buffer, *(display_option->display_string),
					*((u32*)(((u32 *)display_option->options)[*(display_option->current_option)])));
			}
			else
			{
				strcpy(line_buffer, *(display_option->display_string));
			}

			if(display_option == current_option)
				color= COLOR_ACTIVE_ITEM;
			else
				color= COLOR_INACTIVE_ITEM;

			PRINT_STRING_BG(down_screen_addr, line_buffer, color, COLOR_TRANS, 23, 40 + line[i]*27);
        }

		if(current_option_num == 2)
			savestate_selitem(delette_savestate_num, 93);
        else
            savestate_selitem(-1, 93);
	}

	void menu_save_state()
	{
		if(gui_action == CURSOR_SELECT)
		{
			//The slot is empty
			if(!first_load)
			{
				u32 key;
				s32 slot_index;

				slot_index= get_savestate_slot();
				//the slot already have a savestate file
				if(slot_index >= 9)
				{
					draw_message(down_screen_addr, NULL, 28, 31, 227, 165, 0);
					draw_string_vcenter(down_screen_addr, 36, 74, 190, COLOR_MSSG, msg[MSG_SAVESTATE_FULL]);
					if(draw_yesno_dialog(DOWN_SCREEN, 115, "Yes(A)", "No(B)") == 0)
						return;

					clear_savestate_slot(0);
				}
				else
					slot_index += 1;

				savestate_map[slot_index]= -savestate_map[slot_index];
				get_savestate_filename(slot_index, tmp_filename);

				draw_message(down_screen_addr, NULL, 28, 31, 227, 165, 0);
				draw_string_vcenter(down_screen_addr, 36, 100, 190, COLOR_MSSG, msg[MSG_SAVESTATE_DOING]);
				ds2_flipScreen(DOWN_SCREEN, DOWN_SCREEN_UPDATE_METHOD);

				int flag = save_state(tmp_filename, (void*)screen);
				//clear message
				draw_message(down_screen_addr, NULL, 28, 31, 227, 96, 0);
				if(flag < 0)
				{
					draw_string_vcenter(down_screen_addr, 36, 74, 190, COLOR_MSSG, msg[MSG_SAVESTATE_FAILUER]);
					savestate_map[slot_index]= -savestate_map[slot_index];
				}
				else
				{
					draw_string_vcenter(down_screen_addr, 36, 100, 190, COLOR_MSSG, msg[MSG_SAVESTATE_SUCCESS]);
					savestate_index = slot_index;
				}

				ds2_flipScreen(DOWN_SCREEN, DOWN_SCREEN_UPDATE_METHOD);

				//save game config
				reorder_latest_file();
				save_game_config_file();

				// mdelay(500); // Delete this delay
			}
		}
	}
   
	void menu_load_state()
	{
		if(!first_load)
		{
			FILE *fp= NULL;

			if(bg_screenp != NULL)
			{
				bg_screenp_color = COLOR16(43, 11, 11);
				memcpy(bg_screenp, down_screen_addr, 256*192*2);
			}
			else
				bg_screenp_color = COLOR_BG;

			//Slot not emtpy
			if(savestate_map[savestate_index] > 0)
			{
				get_savestate_filename(savestate_index, tmp_filename);
				sprintf(line_buffer, "%s/%s", DEFAULT_RTS_DIR, tmp_filename);
				fp= fopen(line_buffer, "r");

				//file error
				if(fp == NULL)
				{
					draw_message(down_screen_addr, bg_screenp, 28, 31, 227, 165, bg_screenp_color);
					draw_string_vcenter(down_screen_addr, 36, 80, 190, COLOR_MSSG, msg[MSG_SAVESTATE_FILE_BAD]);
					ds2_flipScreen(DOWN_SCREEN, DOWN_SCREEN_UPDATE_METHOD);

					wait_Allkey_release(0);
					if(gui_action == CURSOR_SELECT)
						wait_Anykey_press(0);
					else
						mdelay(1000);
					return;
				}

				fclose(fp);
				//right
				if(gui_action == CURSOR_SELECT)
				{
					draw_message(down_screen_addr, bg_screenp, 28, 31, 227, 165, bg_screenp_color); 
					draw_string_vcenter(up_screen_addr, 36, 75, 190, COLOR_MSSG, msg[MSG_LOADSTATE_DOING]);

					int flag = load_state(tmp_filename);
					if(0 == flag)
					{
						return_value = 1;
						repeat = 0;
						draw_string_vcenter(down_screen_addr, 36, 75, 190, COLOR_MSSG, msg[MSG_LOADSTATE_SUCCESS]);
					}
					else
						draw_string_vcenter(down_screen_addr, 36, 75, 190, COLOR_MSSG, msg[MSG_LOADSTATE_FAILURE]);
				}
				else	//load screen snapshot
				{
					load_game_stat_snapshot(tmp_filename);
				}
			}
			else
			{
				ds2_clearScreen(UP_SCREEN, COLOR_BLACK);
				draw_string_vcenter(up_screen_addr, 36, 75, 190, COLOR_WHITE, msg[MSG_SAVESTATE_SLOT_EMPTY]);
				ds2_flipScreen(UP_SCREEN, 1);
			}
		}
	}

	void delette_savestate()
	{
		if(!first_load && (gui_action == CURSOR_SELECT))
		{
			if(bg_screenp != NULL)
			{
				bg_screenp_color = COLOR16(43, 11, 11);
				memcpy(bg_screenp, down_screen_addr, 256*192*2);
			}
			else
				bg_screenp_color = COLOR_BG;

			wait_Allkey_release(0);
			if(current_option_num == 1)         //delette all
			{
				u32 i, flag;

				draw_message(down_screen_addr, bg_screenp, 28, 31, 227, 165, bg_screenp_color);
				draw_string_vcenter(down_screen_addr, 36, 75, 190, COLOR_MSSG, msg[MSG_DELETTE_ALL_SAVESTATE_WARING]);

				flag= 0;
				for(i= 0; i < SAVE_STATE_SLOT_NUM; i++)
					if(savestate_map[i] > 0)
					{flag= 1; break;}

				if(flag)
				{
					if(draw_yesno_dialog(DOWN_SCREEN, 115, "Yes(A)", "No(B)"))
					{
						wait_Allkey_release(0);
						for(i= 0; i < SAVE_STATE_SLOT_NUM; i++)
						{
							get_savestate_filename(i, tmp_filename);
							sprintf(line_buffer, "%s/%s", DEFAULT_RTS_DIR, tmp_filename);
							remove(line_buffer);
							if(savestate_map[i] > 0)
								savestate_map[i]= -savestate_map[i];
						}
						savestate_index= 0;
					}
				}
				else
				{
					draw_message(down_screen_addr, bg_screenp, 28, 31, 227, 165, bg_screenp_color);
					draw_string_vcenter(down_screen_addr, 36, 90, 190, COLOR_MSSG, msg[MSG_DELETTE_SAVESTATE_NOTHING]);
					ds2_flipScreen(DOWN_SCREEN, DOWN_SCREEN_UPDATE_METHOD);
					mdelay(500);
				}
			}
			else if(current_option_num == 2)    //delette single
			{
				draw_message(down_screen_addr, bg_screenp, 28, 31, 227, 165, bg_screenp_color); 

				if(savestate_map[delette_savestate_num] > 0)
				{
					sprintf(line_buffer, msg[MSG_DELETTE_SINGLE_SAVESTATE_WARING], delette_savestate_num);
					draw_string_vcenter(down_screen_addr, 36, 75, 190, COLOR_MSSG, line_buffer);

					if(draw_yesno_dialog(DOWN_SCREEN, 115, "Yes(A)", "No(B)")) {
						wait_Allkey_release(0);
						clear_savestate_slot(delette_savestate_num);
}
				}
				else
				{
					draw_string_vcenter(down_screen_addr, 36, 90, 190, COLOR_MSSG, msg[MSG_DELETTE_SAVESTATE_NOTHING]);
					ds2_flipScreen(DOWN_SCREEN, DOWN_SCREEN_UPDATE_METHOD);
					mdelay(500);
				}
			}
		}
	}

	MENU_TYPE cheats_menu;
	MENU_TYPE *dynamic_cheat_menu = NULL;
	MENU_OPTION_TYPE *dynamic_cheat_options = NULL;
	unsigned char *dynamic_cheat_msg = NULL;
	unsigned char **dynamic_cheat_pt = NULL;
	unsigned int dynamic_cheat_active;
	int dynamic_cheat_scroll_value= 0;

	void cheat_menu_init()
	{
	    for(i = 0; i < MAX_CHEATS_T; i++)
		{
			sprintf(cheat_data_str[i], "%d", i);
			cheat_data_ptr[i] = &cheat_data_str[i][0];
		}

		reload_cheats_page();
	}

	void cheat_menu_end()
	{
		// Honour current cheat selections.
		uint32 i;
		for (i = 0; i < Cheat.num_cheats; i++) {
			if (Cheat.c[i].enabled)
				S9xApplyCheat(i);
			else
				S9xRemoveCheat(i);
		}
		// Save current cheat selections to the cheat binary file.
		strcpy(line_buffer, (char *) S9xGetFilename (".chb"));
		S9xSaveCheatFile (line_buffer); // cheat binary
	}

	void dynamic_cheat_key()
	{
		unsigned int m, n;

		switch(gui_action)
		{
			case CURSOR_DOWN:
				if(current_menu->screen_focus > 0 && (current_option_num+1) < current_menu->num_options)
				{
					if(current_menu->screen_focus < SUBMENU_ROW_NUM)
					{
						m= current_menu->screen_focus -1;
						draw_hscroll_over(m+1);
						draw_hscroll_init(down_screen_addr, 23, 40 + m*27, 200, 
							COLOR_TRANS, COLOR_INACTIVE_ITEM, *dynamic_cheat_options[current_option_num].display_string);
					}
					else
					{
						for(n= 0; n < SUBMENU_ROW_NUM; n++)
							draw_hscroll_over(n+1);

						m= current_menu->focus_option - current_menu->screen_focus+2;
						for(n= 0; n < SUBMENU_ROW_NUM-1; n++)
							draw_hscroll_init(down_screen_addr, 23, 40 + n*27, 200, 
								COLOR_TRANS, COLOR_INACTIVE_ITEM, *dynamic_cheat_options[m+n].display_string);
					}
				}

				if(current_option_num == 0)
				{
					draw_hscroll_over(0);
					draw_hscroll_init(down_screen_addr, 50, 9, 180, 
						COLOR_TRANS, COLOR_ACTIVE_ITEM, *dynamic_cheat_options[0].display_string);
				}

				current_option_num += 1;
				if(current_option_num >= current_menu->num_options)
					current_option_num -=1;
				else
				{
					m= current_menu->screen_focus;
					if(m >= SUBMENU_ROW_NUM) m -= 1;

					draw_hscroll_over(m+1);
					draw_hscroll_init(down_screen_addr, 23, 40 + m*27, 200, 
						COLOR_TRANS, COLOR_ACTIVE_ITEM, *dynamic_cheat_options[current_option_num].display_string);
				}

				current_option = current_menu->options + current_option_num;
				break;

			case CURSOR_UP:
				if(current_menu->screen_focus > 0)
				{
					if(current_menu->screen_focus > 1 || current_option_num < 2)
					{
						m = current_menu->screen_focus -1;
						draw_hscroll_over(m+1);
						draw_hscroll_init(down_screen_addr, 23, 40 + m*27, 200, 
							COLOR_TRANS, COLOR_INACTIVE_ITEM, *dynamic_cheat_options[current_option_num].display_string);
					}
					else
					{
						unsigned int k;

						for(n= 1; n < SUBMENU_ROW_NUM; n++)
							draw_hscroll_over(n+1);

						m = current_option_num -1;
						k = current_menu->num_options - m -1;
						if(k > SUBMENU_ROW_NUM) k = SUBMENU_ROW_NUM;

						for(n= 1; n < k; n++)
							draw_hscroll_init(down_screen_addr, 23, 40 + n*27, 200, 
								COLOR_TRANS, COLOR_INACTIVE_ITEM, *dynamic_cheat_options[m+n].display_string);
					}
				}

				if(current_option_num)
				{
					current_option_num--;

					if(current_option_num == 0)
					{
						draw_hscroll_over(0);
						draw_hscroll_init(down_screen_addr, 50, 9, 180, 
							COLOR_TRANS, COLOR_ACTIVE_ITEM, *dynamic_cheat_options[0].display_string);
					}
				}

				current_option = current_menu->options + current_option_num;

				if(current_option_num > 0)
				{
					if(current_menu->screen_focus > 1)
						m = current_menu->screen_focus -2;
					else
						m = current_menu->screen_focus -1;

					draw_hscroll_over(m+1);
					draw_hscroll_init(down_screen_addr, 23, 40 + m*27, 200, 
						COLOR_TRANS, COLOR_ACTIVE_ITEM, *dynamic_cheat_options[current_option_num].display_string);
				}
		        break;

			case CURSOR_RIGHT:
				dynamic_cheat_scroll_value= -5;
				break;

			case CURSOR_LEFT:
				dynamic_cheat_scroll_value= 5;
	        	break;
		}
	}

	void dynamic_cheat_action()
	{
		dynamic_cheat_active &= 1;
		dynamic_cheat_active |= (current_option_num -1) << 16;
	}

	void dynamic_cheat_menu_passive()
	{
		unsigned int m, n, k;
		u32 line_num, screen_focus, focus_option;

		line_num = current_option_num;
		screen_focus = current_menu -> screen_focus;
		focus_option = current_menu -> focus_option;

		if(focus_option < line_num)	//focus next option
		{
			focus_option = line_num - focus_option;
			screen_focus += focus_option;
			if(screen_focus > SUBMENU_ROW_NUM)	//Reach max row numbers can display
				screen_focus = SUBMENU_ROW_NUM;

			current_menu -> screen_focus = screen_focus;
			focus_option = line_num;
		}
		else if(focus_option > line_num)	//focus last option
		{
			focus_option = focus_option - line_num;
			if(screen_focus > focus_option)
				screen_focus -= focus_option;
			else
				screen_focus = 0;

			if(screen_focus == 0 && line_num > 0)
				screen_focus = 1;

			current_menu -> screen_focus = screen_focus;
			focus_option = line_num;
		}
		current_menu -> focus_option = focus_option;

		//draw background
		show_icon(down_screen_addr, ICON_SUBBG, 0, 0);
		show_icon(down_screen_addr, ICON_TITLE, 0, 0);
		show_icon(down_screen_addr, ICON_TITLEICON, 12, 9);

		if(current_menu -> screen_focus > 0)
			show_icon(down_screen_addr, ICON_SUBSELA, 6, 35 + (current_menu -> screen_focus-1)*27);

		if(current_menu->screen_focus == 0)
		{
			draw_hscroll(0, dynamic_cheat_scroll_value);
			dynamic_cheat_scroll_value = 0;
			show_icon(down_screen_addr, ICON_BACK, 229, 10);
		}
		else
		{
			draw_hscroll(0, 0);
			show_icon(down_screen_addr, ICON_NBACK, 229, 10);
		}

		k = current_menu->num_options -1;
		if(k > SUBMENU_ROW_NUM) k = SUBMENU_ROW_NUM;

		m = (dynamic_cheat_active>>16) +1;
		n = current_option_num - current_menu->screen_focus + 1;

		for(i= 0; i < k; i++)
		{
			if((i+1) == current_menu->screen_focus)
			{
				draw_hscroll(i+1, dynamic_cheat_scroll_value);
				dynamic_cheat_scroll_value = 0;
			}
			else
				draw_hscroll(i+1, 0);

			if(m == (n +i))
			{
				if(dynamic_cheat_active & 1)
					show_icon((unsigned short*)down_screen_addr, ICON_STATEFULL, 230, 40 + i*27);
				else
					show_icon((unsigned short*)down_screen_addr, ICON_NSTATEFULL, 230, 40 + i*27);
			}
			else
			{
				if(dynamic_cheat_active & 1)
					show_icon((unsigned short*)down_screen_addr, ICON_STATEEMPTY, 230, 40 + i*27);
				else
					show_icon((unsigned short*)down_screen_addr, ICON_NSTATEEMPTY, 230, 40 + i*27);
			}
		}
	}

	void cheat_option_action()
	{
	}

#define CHEAT_NUMBER_X 26
#define CHEAT_DESC_X   52
#define CHEAT_DESC_SX  163
#define CHEAT_ACTIVE_X 225

	void cheat_option_passive()
	{
		unsigned short color;
		unsigned char tmp_buf[512];
		unsigned int len;
		unsigned char *pt;

		if(display_option == current_option)
			color= COLOR_ACTIVE_ITEM;
		else
			color= COLOR_INACTIVE_ITEM;

		//sprintf("%A") will have problem ?
		strcpy(tmp_buf, *(display_option->display_string));
		// This is the number of the cheat to display

		int i = atoi(tmp_buf);

		sprintf(line_buffer, "%d.", i + 1);
		PRINT_STRING_BG(down_screen_addr, line_buffer, color, COLOR_TRANS, CHEAT_NUMBER_X, 40 + display_option-> line_number*27);

		if (i >= Cheat.num_cheats) {
			PRINT_STRING_BG(down_screen_addr, msg[MSG_CHEAT_MENU_NON_LOAD], color, COLOR_TRANS, CHEAT_DESC_X, 40 + display_option-> line_number*27);
		}
		else {
			strcpy(line_buffer, Cheat.c[i].name);
			len = BDF_cut_string(line_buffer, 0, 2);
			if(len > CHEAT_DESC_SX)
			{
				len = BDF_cut_string(line_buffer, CHEAT_DESC_SX, 1);
				line_buffer[len] = '\0';
				strcat(line_buffer, "...");
			}
			PRINT_STRING_BG(down_screen_addr, line_buffer, color, COLOR_TRANS, CHEAT_DESC_X, 40 + display_option-> line_number*27);

			if (Cheat.c[i].enabled)
				strcpy(line_buffer, "+");
			else
				strcpy(line_buffer, "-");
			PRINT_STRING_BG(down_screen_addr, line_buffer, color, COLOR_TRANS, CHEAT_ACTIVE_X, 40 + display_option-> line_number*27);
		}
	}

	void dynamic_cheat_menu_end()
	{
		unsigned int m, k;

		m = cheats_menu.focus_option-1;
		// game_config.cheats_flag[(CHEATS_PER_PAGE * menu_cheat_page) + m].sub_active = dynamic_cheat_active >> 16;
		// REVISIT [Neb]

		k = SUBMENU_ROW_NUM +1;
		for(m= 0; m<k; m++)
			draw_hscroll_over(m);
	}

	void destroy_dynamic_cheats()
	{
		if(dynamic_cheat_menu) free((void*)dynamic_cheat_menu);
		if(dynamic_cheat_options) free((void*)dynamic_cheat_options);
		if(dynamic_cheat_msg) free((void*)dynamic_cheat_msg);
		if(dynamic_cheat_pt) free((void*)dynamic_cheat_pt);
		dynamic_cheat_menu = NULL;
		dynamic_cheat_options = NULL;
		dynamic_cheat_msg = NULL;
		dynamic_cheat_pt = NULL;
	}

    void menu_load_cheat_file()
    {
        char *file_ext[] = { ".cht", NULL };
        u32 i, string_num, string_len;
		int flag;

		if(load_file(file_ext, tmp_filename, DEFAULT_CHEAT_DIR) != -1)
		{
			sprintf(line_buffer, "%s/%s", DEFAULT_CHEAT_DIR, tmp_filename);
			flag = NDSSFCLoadCheatFile(line_buffer);

			strcpy(line_buffer, (char *) S9xGetFilename (".chb"));
			S9xSaveCheatFile (line_buffer); // cheat binary

			if(0 != flag)
			{	//load cheat file failure
				S9xDeleteCheats();

				cheat_menu_init();
				return;
			}

			menu_cheat_page = 0;
			cheat_menu_init();

		}
    }

    void save_screen_snapshot()
    {
        if((gui_action == CURSOR_SELECT))
        {
            if(bg_screenp != NULL)
            {
                bg_screenp_color = COLOR16(43, 11, 11);
                memcpy(bg_screenp, down_screen_addr, 256*192*2);
            }
            else
                bg_screenp_color = COLOR_BG;

            draw_message(down_screen_addr, bg_screenp, 28, 31, 227, 165, bg_screenp_color);
            if(!first_load)
            {
                draw_string_vcenter(down_screen_addr, 36, 70, 190, COLOR_MSSG, msg[MSG_SAVE_SNAPSHOT]);
                ds2_flipScreen(DOWN_SCREEN, DOWN_SCREEN_UPDATE_METHOD);
                if(save_ss_bmp(screen))
                    draw_string_vcenter(down_screen_addr, 36, 90, 190, COLOR_MSSG, msg[MSG_SAVE_SNAPSHOT_COMPLETE]);
                else
                    draw_string_vcenter(down_screen_addr, 36, 90, 190, COLOR_MSSG, msg[MSG_SAVE_SNAPSHOT_FAILURE]);
                ds2_flipScreen(DOWN_SCREEN, DOWN_SCREEN_UPDATE_METHOD);
				mdelay(500);
            }
            else
            {
                draw_string_vcenter(down_screen_addr, 36, 90, 190, COLOR_MSSG, msg[MSG_SAVESTATE_SLOT_EMPTY]);
                ds2_flipScreen(DOWN_SCREEN, DOWN_SCREEN_UPDATE_METHOD);
				mdelay(500);
            }
        }
    }

    void browse_screen_snapshot()
    {
        if(current_option_num == 2)
            play_screen_snapshot();
    }

    void keyremap()
    {
        if(gui_action== CURSOR_RIGHT || gui_action== CURSOR_LEFT)
        {
//          if(current_option_num != 0)
//              game_config.use_default_gamepad_map = 0;
          switch(current_option_num)
          {
            case 1: //GBA KEY A
                if(BUTTON_MAP_A == BUTTON_ID_A)
                    BUTTON_MAP_A = BUTTON_ID_Y;
                else if(BUTTON_MAP_A == BUTTON_ID_Y)
                    BUTTON_MAP_A = BUTTON_ID_A;

                BUTTON_MAP_FA = BUTTON_ID_NONE;
                BUTTON_MAP_FB = BUTTON_ID_NONE;
              break;
            case 2: //GBA KEY B(fixed B)
              break;
            case 3: //GBA KEY FA(fixed X)
                if(BUTTON_MAP_FA == BUTTON_ID_NONE)
                {
                   BUTTON_MAP_FA = BUTTON_ID_X;
                }
                else
                   BUTTON_MAP_FA = BUTTON_ID_NONE;

              break;
            case 4: //GBA KEY FB
                if(BUTTON_MAP_A == BUTTON_ID_A)
                {
                    if(BUTTON_MAP_FB == BUTTON_ID_Y)
                        BUTTON_MAP_FB = BUTTON_ID_NONE;
                    else
                        BUTTON_MAP_FB = BUTTON_ID_Y;
                }
                else if(BUTTON_MAP_A == BUTTON_ID_Y)
                {
                    if(BUTTON_MAP_FB == BUTTON_ID_NONE)
                        BUTTON_MAP_FB = BUTTON_ID_A;
                    else
                        BUTTON_MAP_FB = BUTTON_ID_NONE;
                }
              break;
            case 5: //Awaking menu
                if(BUTTON_MAP_MENU == BUTTON_ID_X)
                {
                    if(BUTTON_MAP_A != BUTTON_ID_Y && BUTTON_MAP_FB != BUTTON_ID_Y)
                        BUTTON_MAP_MENU = BUTTON_ID_Y;
                    else
                        BUTTON_MAP_MENU = BUTTON_ID_TOUCH;
                }
                else if(BUTTON_MAP_MENU == BUTTON_ID_TOUCH)
                {
                    if(BUTTON_MAP_FA != BUTTON_ID_X)
                        BUTTON_MAP_MENU = BUTTON_ID_X;
                    else if(BUTTON_MAP_A != BUTTON_ID_Y && BUTTON_MAP_FB != BUTTON_ID_Y)
                        BUTTON_MAP_MENU = BUTTON_ID_Y;
                }
                else
                    BUTTON_MAP_MENU = BUTTON_ID_TOUCH;
              break;
            default:
              break;
          }
        }

        if(BUTTON_MAP_MENU == BUTTON_ID_X && BUTTON_MAP_FA == BUTTON_ID_X)
        {
            if(BUTTON_MAP_A == BUTTON_ID_Y || BUTTON_MAP_FB == BUTTON_ID_Y)
                BUTTON_MAP_MENU = BUTTON_ID_TOUCH;
            else
                BUTTON_MAP_MENU = BUTTON_ID_Y;
        }
        else if(BUTTON_MAP_MENU == BUTTON_ID_Y)
        {
            if(BUTTON_MAP_A == BUTTON_ID_Y || BUTTON_MAP_FB == BUTTON_ID_Y)
            {
                if(BUTTON_MAP_FA == BUTTON_ID_X)
                    BUTTON_MAP_MENU = BUTTON_ID_TOUCH;
                else
                    BUTTON_MAP_MENU = BUTTON_ID_X;
            }
        }

//        gamepad_config_menu = BUTTON_MAP_MENU;
    }

	void time_backward_action()
	{
		tools_menu_init();
		if(game_config.backward)
			savefast_int();
	}

	void time_period_passive()
	{
		char* str[] = {"0.2", "0.5", "1.0", "2.0", "5.0", "10 "};
		unsigned int mm;
		unsigned short color;

		if(display_option == current_option)
			color= COLOR_ACTIVE_ITEM;
		else
			color= COLOR_INACTIVE_ITEM;

		mm = *(display_option->current_option);
		sprintf(line_buffer, *(display_option->display_string), str[mm]);
		
        PRINT_STRING_BG(down_screen_addr, line_buffer, color, COLOR_TRANS, 27,
            38 + (display_option-> line_number)*32);
	}

	void time_period_action()
	{
		switch(game_config.backward_time)
		{
			case 0 : frame_interval = 15; break;
			case 1 : frame_interval = 30; break;
			case 2 : frame_interval = 60; break;
			case 3 : frame_interval = 120; break;
			case 4 : frame_interval = 300; break;
			case 5 : frame_interval = 600; break;
			default: frame_interval = 60; break;
		}
	}

    void load_default_setting()
    {
        if(bg_screenp != NULL)
        {
            bg_screenp_color = COLOR16(43, 11, 11);
            memcpy(bg_screenp, down_screen_addr, 256*192*2);
        }
        else
            bg_screenp_color = COLOR_BG;

        draw_message(down_screen_addr, bg_screenp, 28, 31, 227, 165, bg_screenp_color);
        draw_string_vcenter(down_screen_addr, 36, 70, 190, COLOR_MSSG, msg[MSG_LOAD_DEFAULT_WARING]);

        if(draw_yesno_dialog(DOWN_SCREEN, 115, "Yes", "No"))
        {
			wait_Allkey_release(0);
            draw_message(down_screen_addr, bg_screenp, 28, 31, 227, 165, bg_screenp_color);
            draw_string_vcenter(down_screen_addr, 36, 80, 190, COLOR_MSSG, msg[MSG_DEFAULT_LOADING]);
            ds2_flipScreen(DOWN_SCREEN, DOWN_SCREEN_UPDATE_METHOD);

            sprintf(line_buffer, "%s/%s", main_path, EMU_CONFIG_FILENAME);
            remove(line_buffer);

            first_load= 1;
            init_emulator_config();
            init_game_config();

			ds2_clearScreen(UP_SCREEN, 0);
            draw_string_vcenter(up_screen_addr, 0, 80, 256, COLOR_WHITE, msg[MSG_NON_LOAD_GAME]);
			ds2_flipScreen(UP_SCREEN, 1);

			// mdelay(500); // Delete this delay
        }
    }

    void check_gbaemu_version()
    {
        if(bg_screenp != NULL)
        {
            bg_screenp_color = COLOR16(43, 11, 11);
            memcpy(bg_screenp, down_screen_addr, 256*192*2);
        }
        else
            bg_screenp_color = COLOR_BG;

        draw_message(down_screen_addr, bg_screenp, 28, 31, 227, 165, bg_screenp_color);
        draw_string_vcenter(down_screen_addr, 36, 80, 190, COLOR_MSSG, msg[MSG_EMU_VERSION0]);
        sprintf(line_buffer, "%s %s", msg[MSG_EMU_VERSION1], NDSSFC_VERSION);
        draw_string_vcenter(down_screen_addr, 36, 95, 190, COLOR_MSSG, line_buffer);
        ds2_flipScreen(DOWN_SCREEN, DOWN_SCREEN_UPDATE_METHOD);

		wait_Anykey_press(0);
    }

    void language_set()
    {
        if(gui_action == CURSOR_LEFT || gui_action == CURSOR_RIGHT)
        {
            if(bg_screenp != NULL)
            {
                bg_screenp_color = COLOR16(43, 11, 11);
                memcpy(bg_screenp, down_screen_addr, 256*192*2);
            }
            else
                bg_screenp_color = COLOR_BG;

            draw_message(down_screen_addr, bg_screenp, 28, 31, 227, 165, bg_screenp_color);
            draw_string_vcenter(down_screen_addr, 36, 75, 190, COLOR_MSSG, msg[MSG_CHANGE_LANGUAGE]);
            draw_string_vcenter(down_screen_addr, 36, 95, 190, COLOR_MSSG, msg[MSG_CHANGE_LANGUAGE_WAITING]);
            ds2_flipScreen(DOWN_SCREEN, DOWN_SCREEN_UPDATE_METHOD);

            load_language_msg(LANGUAGE_PACK, emu_config.language);    
            gui_change_icon(emu_config.language);

            if(first_load)
            {
				ds2_clearScreen(UP_SCREEN, 0);
                draw_string_vcenter(up_screen_addr, 0, 80, 256, COLOR_WHITE, msg[MSG_NON_LOAD_GAME]);
				ds2_flipScreen(UP_SCREEN, 1);
            }

			save_emu_config_file();
			wait_Allkey_release(0);
        }
    }

	unsigned int freespace;
    void show_card_space ()
    {
        u32 line_num;
        u32 num_byte;

        strcpy(line_buffer, *(display_option->display_string));
        line_num= display_option-> line_number;
        PRINT_STRING_BG(down_screen_addr, line_buffer, COLOR_INACTIVE_ITEM, COLOR_TRANS, 27,
            40 + (display_option->line_number)*27);

		num_byte = freespace;

        if(num_byte <= 9999*2)
        { /* < 9999KB */
            sprintf(line_buffer, "%d", num_byte/2);
            if(num_byte & 1)
                strcat(line_buffer, ".5 KB");
            else
                strcat(line_buffer, ".0 KB");
        }
        else if(num_byte <= 9999*1024*2)
        { /* < 9999MB */
            num_byte /= 1024;
            sprintf(line_buffer, "%d", num_byte/2);
            if(num_byte & 1)
                strcat(line_buffer, ".5 MB");
            else
                strcat(line_buffer, ".0 MB");
        }
        else
        {
            num_byte /= 1024*1024;
            sprintf(line_buffer, "%d", num_byte/2);
            if(num_byte & 1)
                strcat(line_buffer, ".5 GB");
            else
                strcat(line_buffer, ".0 GB");
        }

        PRINT_STRING_BG(down_screen_addr, line_buffer, COLOR_INACTIVE_ITEM, COLOR_TRANS, 147,
            40 + (display_option->line_number)*27);
    }

    char *screen_ratio_options[] = { (char*)&msg[MSG_SCREEN_RATIO_0],
									(char*)&msg[MSG_SCREEN_RATIO_1],
									(char*)&msg[MSG_SCREEN_RATIO_2],
									(char*)&msg[MSG_SCREEN_RATIO_3],
									(char*)&msg[MSG_SCREEN_RATIO_4]};
    
    char *frameskip_options[] = { (char*)&msg[MSG_FRAMESKIP_0], (char*)&msg[MSG_FRAMESKIP_1] };

    char *on_off_options[] = { (char*)&msg[MSG_ON_OFF_0], (char*)&msg[MSG_ON_OFF_1] };

//    char *sound_seletion[] = { (char*)&msg[MSG_SOUND_SWITCH_0], (char*)&msg[MSG_SOUND_SWITCH_1] };

//    char *snap_frame_options[] = { (char*)&msg[MSG_SNAP_FRAME_0], (char*)&msg[MSG_SNAP_FRAME_1] };

    char *enable_disable_options[] = { (char*)&msg[MSG_EN_DIS_ABLE_0], (char*)&msg[MSG_EN_DIS_ABLE_1] };

    char *keyremap_options[] = {(char*)&msg[MSG_KEY_MAP_NONE], (char*)&msg[MSG_KEY_MAP_A], (char*)&msg[MSG_KEY_MAP_B], 
                                (char*)&msg[MSG_KEY_MAP_SL], (char*)&msg[MSG_KEY_MAP_ST], (char*)&msg[MSG_KEY_MAP_RT], 
                                (char*)&msg[MSG_KEY_MAP_LF], (char*)&msg[MSG_KEY_MAP_UP], (char*)&msg[MSG_KEY_MAP_DW], 
                                (char*)&msg[MSG_KEY_MAP_R], (char*)&msg[MSG_KEY_MAP_L], (char*)&msg[MSG_KEY_MAP_X], 
                                (char*)&msg[MSG_KEY_MAP_Y], (char*)&msg[MSG_KEY_MAP_TOUCH]
                                };

  /*--------------------------------------------------------
    Video & Audio
  --------------------------------------------------------*/
	MENU_OPTION_TYPE graphics_options[] =
	{
	/* 00 */ SUBMENU_OPTION(NULL, &msg[MSG_MAIN_MENU_0], NULL, 0),

    /* 01 */ STRING_SELECTION_OPTION(NULL, NULL, &msg[MSG_SUB_MENU_00], screen_ratio_options, 
        &game_config.graphic, 5, NULL, PASSIVE_TYPE, 1),

	/* 02 */ STRING_SELECTION_OPTION(game_fastforward, NULL, &msg[MSG_SUB_MENU_01], on_off_options, 
		&game_fast_forward, 2, NULL, ACTION_TYPE, 2),
		
	/* 03 */	STRING_SELECTION_OPTION(game_disableAudio, NULL, &msg[MSG_SUB_MENU_04],on_off_options,
		&game_enable_audio, 2, NULL, ACTION_TYPE, 3)
	};

	MAKE_MENU(graphics, NULL, NULL, NULL, NULL, 0, 0);

  /*--------------------------------------------------------
     Game state -- delette
  --------------------------------------------------------*/
	MENU_TYPE game_state_menu;

	MENU_OPTION_TYPE gamestate_delette_options[] =
	{
	/* 00 */ SUBMENU_OPTION(&game_state_menu, &msg[MSG_SUB_MENU_13], NULL, 0),

	/* 01 */ ACTION_OPTION(delette_savestate, NULL, &msg[MSG_SUB_MENU_130], NULL, 1),

	/* 02 */ NUMERIC_SELECTION_ACTION_OPTION(delette_savestate, NULL,
		&msg[MSG_SUB_MENU_131], &delette_savestate_num, 10, NULL, 2)
	};

	MAKE_MENU(gamestate_delette, NULL, gamestate_delette_menu_passive, NULL, NULL, 0, 0);

  /*--------------------------------------------------------
     Game state
  --------------------------------------------------------*/
	MENU_OPTION_TYPE game_state_options[] =
	{
	/* 00 */ SUBMENU_OPTION(NULL, &msg[MSG_MAIN_MENU_1], NULL, 0),

	/* 01 */ ACTION_OPTION(menu_save_state, NULL, &msg[MSG_SUB_MENU_10], NULL, 1),

	/* 02 */ NUMERIC_SELECTION_ACTION_OPTION(menu_load_state, NULL,
        &msg[MSG_SUB_MENU_11], &savestate_index, 10, NULL, 2),

	/* 03 */ SUBMENU_OPTION(&gamestate_delette_menu, &msg[MSG_SUB_MENU_13], NULL, 5),
	};

	INIT_MENU(game_state, NULL, game_state_menu_passive, NULL, NULL, 0, 0);

  /*--------------------------------------------------------
     Cheat options
  --------------------------------------------------------*/
	MENU_OPTION_TYPE cheats_options[] =
	{
	/* 00 */ SUBMENU_OPTION(NULL, &msg[MSG_MAIN_MENU_2], NULL,0),

	/* 01 */ CHEAT_OPTION(cheat_option_action, cheat_option_passive, 
		((CHEATS_PER_PAGE * menu_cheat_page) + 0), 1),
	/* 02 */ CHEAT_OPTION(cheat_option_action, cheat_option_passive, 
		((CHEATS_PER_PAGE * menu_cheat_page) + 1), 2),
	/* 03 */ CHEAT_OPTION(cheat_option_action, cheat_option_passive,
		((CHEATS_PER_PAGE * menu_cheat_page) + 2), 3),
	/* 04 */ CHEAT_OPTION(cheat_option_action, cheat_option_passive,
		((CHEATS_PER_PAGE * menu_cheat_page) + 3), 4),

	/* 05 */ NUMERIC_SELECTION_ACTION_OPTION(reload_cheats_page, NULL, &msg[MSG_SUB_MENU_20],
        &menu_cheat_page, MAX_CHEATS_PAGE, NULL, 5),

	/* 06 */ ACTION_OPTION(menu_load_cheat_file, NULL, &msg[MSG_SUB_MENU_21],
        NULL, 6)
	};

	INIT_MENU(cheats, cheat_menu_init, NULL, NULL, cheat_menu_end, 0, 0);

  /*--------------------------------------------------------
     Tools-screensanp
  --------------------------------------------------------*/
    MENU_TYPE tools_menu;

    MENU_OPTION_TYPE tools_screensnap_options[] =
    {
	/* 00 */ SUBMENU_OPTION(&tools_menu, &msg[MSG_SUB_MENU_30], NULL, 0),

	/* 01 */ ACTION_OPTION(save_screen_snapshot, NULL, &msg[MSG_SUB_MENU_300], NULL, 1),

	/* 02 */ ACTION_OPTION(browse_screen_snapshot, NULL, &msg[MSG_SUB_MENU_301], NULL, 2)
    };

    MAKE_MENU(tools_screensnap, NULL, NULL, NULL, NULL, 0, 0);

  /*--------------------------------------------------------
     Tools-keyremap
  --------------------------------------------------------*/
    MENU_OPTION_TYPE tools_keyremap_options[] = 
    {
	/* 00 */ SUBMENU_OPTION(&tools_menu, &msg[MSG_SUB_MENU_31], NULL, 0),
        
	/* 01 */ STRING_SELECTION_OPTION(keyremap, keyremap_show, &msg[MSG_SUB_MENU_310], 
		NULL, &string_select, 2, NULL, ACTION_TYPE, 1),
        
	/* 02 */ STRING_SELECTION_OPTION(keyremap, keyremap_show, &msg[MSG_SUB_MENU_311], 
		NULL, &string_select, 1, NULL, ACTION_TYPE | HIDEN_TYPE, 2),
        
	/* 03 */ STRING_SELECTION_OPTION(keyremap, keyremap_show, &msg[MSG_SUB_MENU_312], 
		NULL, &string_select, 2, NULL, ACTION_TYPE, 3),
        
	/* 04 */ STRING_SELECTION_OPTION(keyremap, keyremap_show, &msg[MSG_SUB_MENU_313], 
		NULL, &string_select, 3, NULL, ACTION_TYPE, 4),
        
	/* 05 */ STRING_SELECTION_OPTION(keyremap, keyremap_show, &msg[MSG_SUB_MENU_314], 
		NULL, &string_select, 3, NULL, ACTION_TYPE, 5)
    };

    MAKE_MENU(tools_keyremap, NULL, NULL, NULL, NULL, 0, 0);
  /*--------------------------------------------------------
     Tools
  --------------------------------------------------------*/
    MENU_OPTION_TYPE tools_options[] = 
    {
	/* 00 */ SUBMENU_OPTION(NULL, &msg[MSG_MAIN_MENU_3], NULL, 0),

	/* 01 */ SUBMENU_OPTION(&tools_screensnap_menu, &msg[MSG_SUB_MENU_30], NULL, 1),

//	/* 02 */ SUBMENU_OPTION(&tools_keyremap_menu, &msg[MSG_SUB_MENU_31], NULL, 2),

//	/* 03 */ STRING_SELECTION_OPTION(time_backward_action, NULL, &msg[MSG_SUB_MENU_302], enable_disable_options, 
//			&game_config.backward, 2, NULL, ACTION_TYPE, 3),

//	/* 04 */ NUMERIC_SELECTION_ACTION_OPTION(time_period_action, time_period_passive, &msg[MSG_SUB_MENU_32], 
//		&game_config.backward_time, 6, NULL, 4)
    };

    INIT_MENU(tools, tools_menu_init, NULL, NULL, NULL, 0, 0);

  /*--------------------------------------------------------
     Others
  --------------------------------------------------------*/
    u32 desert= 0;
	MENU_OPTION_TYPE others_options[] = 
	{
	/* 00 */ SUBMENU_OPTION(NULL, &msg[MSG_MAIN_MENU_4], NULL, 0),

	//CPU speed
	/* 01 */ NUMERIC_SELECTION_OPTION(NULL, &msg[MSG_SUB_MENU_42], &clock_speed_number, 6, NULL, 1),

	/* 02 */ STRING_SELECTION_OPTION(language_set, NULL, &msg[MSG_SUB_MENU_41], language_options, 
        &emu_config.language, sizeof(language_options) / sizeof(language_options[0]) /* number of possible languages */, NULL, ACTION_TYPE, 2),

	/* 03 */ STRING_SELECTION_OPTION(NULL, show_card_space, &msg[MSG_SUB_MENU_43], NULL, 
        &desert, 2, NULL, PASSIVE_TYPE | HIDEN_TYPE, 3),

	/* 04 */ ACTION_OPTION(load_default_setting, NULL, &msg[MSG_SUB_MENU_44], NULL, 4),

	/* 05 */ ACTION_OPTION(check_gbaemu_version, NULL, &msg[MSG_SUB_MENU_45], NULL, 5),
	};

	MAKE_MENU(others, others_menu_init, NULL, NULL, NULL, 1, 1);

  /*--------------------------------------------------------
     Load_game
  --------------------------------------------------------*/
    MENU_TYPE latest_game_menu;
  
    MENU_OPTION_TYPE load_game_options[] =
    {
	/* 00 */ SUBMENU_OPTION(NULL, &msg[MSG_SUB_MENU_62], NULL, 0),

	/* 01 */ ACTION_OPTION(menu_load, NULL, &msg[MSG_SUB_MENU_61], NULL, 1),

	/* 02 */ SUBMENU_OPTION(&latest_game_menu, &msg[MSG_SUB_MENU_60], NULL, 2)
    };

    MAKE_MENU(load_game, NULL, NULL, NULL, NULL, 1, 1);

  /*--------------------------------------------------------
     Latest game
  --------------------------------------------------------*/
    MENU_OPTION_TYPE latest_game_options[] =
    {
	/* 00 */ SUBMENU_OPTION(&load_game_menu, &msg[MSG_SUB_MENU_60], NULL, 0),

	/* 01 */ ACTION_OPTION(load_lastest_played, NULL, NULL, NULL, 1),

	/* 02 */ ACTION_OPTION(load_lastest_played, NULL, NULL, NULL, 2),

	/* 03 */ ACTION_OPTION(load_lastest_played, NULL, NULL, NULL, 3),

	/* 04 */ ACTION_OPTION(load_lastest_played, NULL, NULL, NULL, 4),

	/* 05 */ ACTION_OPTION(load_lastest_played, NULL, NULL, NULL, 5)
    };

    INIT_MENU(latest_game, latest_game_menu_init, latest_game_menu_passive,
		latest_game_menu_key, latest_game_menu_end, 0, 0);

  /*--------------------------------------------------------
     MAIN MENU
  --------------------------------------------------------*/
	MENU_OPTION_TYPE main_options[] =
	{
    /* 00 */ SUBMENU_OPTION(&graphics_menu, &msg[MSG_MAIN_MENU_0], NULL, 0),

    /* 01 */ SUBMENU_OPTION(&game_state_menu, &msg[MSG_MAIN_MENU_1], NULL, 1),

    /* 02 */ SUBMENU_OPTION(&cheats_menu, &msg[MSG_MAIN_MENU_2], NULL, 2),

    /* 03 */ SUBMENU_OPTION(&tools_menu, &msg[MSG_MAIN_MENU_3], NULL, 3),

    /* 04 */ SUBMENU_OPTION(&others_menu, &msg[MSG_MAIN_MENU_4], NULL, 4),

    /* 05 */ ACTION_OPTION(menu_exit, NULL, &msg[MSG_MAIN_MENU_5], NULL, 5),

    /* 06 */ SUBMENU_OPTION(&load_game_menu, NULL, NULL, 6),

    /* 07 */ ACTION_OPTION(menu_return, NULL, NULL, NULL, 7),

    /* 08 */ ACTION_OPTION(menu_restart, NULL, NULL, NULL, 8)
	};

	MAKE_MENU(main, NULL, main_menu_passive, main_menu_key, NULL, 6, 0);

	void main_menu_passive()
	{
		show_icon(down_screen_addr, ICON_MAINBG, 0, 0);
		current_menu -> focus_option = current_option -> line_number;

		//Audio/Video
		strcpy(line_buffer, *(display_option->display_string));
		if(display_option++ == current_option) {
			show_icon(down_screen_addr, ICON_AVO, 22, 2);
			show_icon(down_screen_addr, ICON_MSEL, 11, 57);
		}
		else {
			show_icon(down_screen_addr, ICON_NAVO, 22, 2);
			show_icon(down_screen_addr, ICON_MNSEL, 11, 57);
		}
		draw_string_vcenter(down_screen_addr, 13, 57, 76, COLOR_WHITE, line_buffer);

		//Save
		strcpy(line_buffer, *(display_option->display_string));
		if(display_option++ == current_option) {
			show_icon(down_screen_addr, ICON_SAVO, 102, 2);
			show_icon(down_screen_addr, ICON_MSEL, 92, 57);
		}
		else {
			show_icon(down_screen_addr, ICON_NSAVO, 102, 2);
			show_icon(down_screen_addr, ICON_MNSEL, 92, 57);
		}
		draw_string_vcenter(down_screen_addr, 95, 57, 72, COLOR_WHITE, line_buffer);

		//Cheat
		strcpy(line_buffer, *(display_option->display_string));
		if(display_option++ == current_option) {
			show_icon(down_screen_addr, ICON_CHEAT, 182, 2);
			show_icon(down_screen_addr, ICON_MSEL, 173, 57);
		}
		else {
			show_icon(down_screen_addr, ICON_NCHEAT, 182, 2);
			show_icon(down_screen_addr, ICON_MNSEL, 173, 57);
		}
		draw_string_vcenter(down_screen_addr, 176, 57, 68, COLOR_WHITE, line_buffer);

		//Tools
		strcpy(line_buffer, *(display_option->display_string));
		if(display_option++ == current_option) {
			show_icon(down_screen_addr, ICON_TOOL, 22, 75);
			show_icon(down_screen_addr, ICON_MSEL, 11, 131);
		}
		else {
			show_icon(down_screen_addr, ICON_NTOOL, 22, 75);
			show_icon(down_screen_addr, ICON_MNSEL, 11, 131);
		}
		draw_string_vcenter(down_screen_addr, 14, 131, 68, COLOR_WHITE, line_buffer);

		//Other
		strcpy(line_buffer, *(display_option->display_string));
		if(display_option++ == current_option) {
			show_icon(down_screen_addr, ICON_OTHER, 102, 75);
			show_icon(down_screen_addr, ICON_MSEL, 92, 131);
		}
		else {
			show_icon(down_screen_addr, ICON_NOTHER, 102, 75);
			show_icon(down_screen_addr, ICON_MNSEL, 92, 131);
		}
		draw_string_vcenter(down_screen_addr, 95, 131, 68, COLOR_WHITE, line_buffer);

		//Exit
		strcpy(line_buffer, *(display_option->display_string));
		if(display_option++ == current_option) {
			show_icon(down_screen_addr, ICON_EXIT, 182, 75);
			show_icon(down_screen_addr, ICON_MSEL, 173, 131);
		}
		else {
			show_icon(down_screen_addr, ICON_NEXIT, 182, 75);	
			show_icon(down_screen_addr, ICON_MNSEL, 173, 131);
		}
		draw_string_vcenter(down_screen_addr, 176, 131, 68, COLOR_WHITE, line_buffer);

		//New
		if(display_option++ == current_option)
			show_icon(down_screen_addr, ICON_NEW, 0, 154);
		else
			show_icon(down_screen_addr, ICON_NNEW, 0, 154);

		//Restart
		if(display_option++ == current_option)
			show_icon(down_screen_addr, ICON_RETURN, 89, 154);
		else
			show_icon(down_screen_addr, ICON_NRETURN, 89, 154);

		//Return
		if(display_option++ == current_option)
			show_icon(down_screen_addr, ICON_RESET, 170, 154);
		else
			show_icon(down_screen_addr, ICON_NRESET, 170, 154);
	}

    void main_menu_key()
    {
        switch(gui_action)
        {
            case CURSOR_DOWN:
				if(current_option_num < 6)	current_option_num += 3;
				else current_option_num -= 6;

                current_option = current_menu->options + current_option_num;
              break;

            case CURSOR_UP:
				if(current_option_num < 3)	current_option_num += 6;
				else current_option_num -= 3;

                current_option = current_menu->options + current_option_num;
              break;

            case CURSOR_RIGHT:
				if(current_option_num == 2)	current_option_num -= 2;
				else if(current_option_num == 5)	current_option_num -= 2;
				else if(current_option_num == 8)	current_option_num -= 2;
				else current_option_num += 1;

                current_option = main_menu.options + current_option_num;
              break;

            case CURSOR_LEFT:
				if(current_option_num == 0)	current_option_num += 2;
				else if(current_option_num == 3)	current_option_num += 2;
				else if(current_option_num == 6)	current_option_num += 2;
				else current_option_num -= 1;

                current_option = main_menu.options + current_option_num;
              break;

            default:
              break;
        }// end swith
    }

	void tools_menu_init()
	{
		if(game_config.backward)
			tools_options[4].option_type &= ~HIDEN_TYPE;
		else
			tools_options[4].option_type |= HIDEN_TYPE;
	}

	int lastest_game_menu_scroll_value;
    void latest_game_menu_init()
    {
        u32 k;
        char *ext_pos;

        for(k= 0; k < 5; k++)
        {
            ext_pos= strrchr(emu_config.latest_file[k], '/');
            if(ext_pos != NULL)
                draw_hscroll_init(down_screen_addr, 26, 40 + k*27, 200, 
                    COLOR_TRANS, COLOR_INACTIVE_ITEM, ext_pos+1);
			else
				break;
        }

		if(k < 5)
		{
			latest_game_menu.num_options = k+1;
		}
		else
			latest_game_menu.num_options = 6;

		latest_game_menu.num_options = k+1;

        for(; k < 5; k++)
        {
            latest_game_options[k+1].option_type |= HIDEN_TYPE;
        }

		lastest_game_menu_scroll_value = 0;
    }

    void latest_game_menu_passive()
    {
        u32 k;
		unsigned short color;

		//draw background
		show_icon(down_screen_addr, ICON_SUBBG, 0, 0);
		show_icon(down_screen_addr, ICON_TITLE, 0, 0);
		show_icon(down_screen_addr, ICON_TITLEICON, 12, 9);

		if(current_option_num == 0)
			show_icon(down_screen_addr, ICON_BACK, 229, 10);
		else
		{
			show_icon(down_screen_addr, ICON_NBACK, 229, 10);
			show_icon(down_screen_addr, ICON_SUBSELA, 6, 35 + (current_option_num-1)*27);
		}

		strcpy(line_buffer, *(display_option->display_string));
		draw_string_vcenter(down_screen_addr, 0, 9, 256, COLOR_ACTIVE_ITEM, line_buffer);

		for(k= 0; k<5; k++)
        if(emu_config.latest_file[k][0] != '\0')
        {
            if(current_option_num != k+1)
                draw_hscroll(k, 0);
            else
			{
                draw_hscroll(k, lastest_game_menu_scroll_value);
				lastest_game_menu_scroll_value = 0;
			}
        }
    }

    void latest_game_menu_end()
    {
        u32 k;
        
        for(k= 0; k < 5; k++)
        {
            if(emu_config.latest_file[k][0] != '\0')
                draw_hscroll_over(k);
        }
    }

	void latest_game_menu_key()
	{
		char *ext_pos;

		switch(gui_action)
        {
            case CURSOR_DOWN:
				//clear last option's bg
				if(current_option_num != 0)
				{
					draw_hscroll_over(current_option_num-1);
	            	ext_pos= strrchr(emu_config.latest_file[current_option_num-1], '/');
                	draw_hscroll_init(down_screen_addr, 26, 40 + (current_option_num-1)*27, 200, 
                	    COLOR_TRANS, COLOR_INACTIVE_ITEM, ext_pos+1);
				}

				current_option_num += 1;
				if(current_option_num >= latest_game_menu.num_options)	
					current_option_num = 0;
                current_option = current_menu->options + current_option_num;

				//Set current bg
				if(current_option_num != 0)
				{
					draw_hscroll_over(current_option_num-1);
	            	ext_pos= strrchr(emu_config.latest_file[current_option_num-1], '/');
                	draw_hscroll_init(down_screen_addr, 26, 40 + (current_option_num-1)*27, 200, 
                	    COLOR_TRANS, COLOR_ACTIVE_ITEM, ext_pos+1);
				}

              break;

            case CURSOR_UP:
				//clear last option's bg
				if(current_option_num != 0)
				{
					draw_hscroll_over(current_option_num-1);
	            	ext_pos= strrchr(emu_config.latest_file[current_option_num-1], '/');
                	draw_hscroll_init(down_screen_addr, 26, 40 + (current_option_num-1)*27, 200, 
                	    COLOR_TRANS, COLOR_INACTIVE_ITEM, ext_pos+1);
				}

				if(current_option_num > 0)	current_option_num -= 1;
				else current_option_num = latest_game_menu.num_options -1;
                current_option = current_menu->options + current_option_num;

				//Set current bg
				if(current_option_num != 0)
				{
					draw_hscroll_over(current_option_num-1);
	            	ext_pos= strrchr(emu_config.latest_file[current_option_num-1], '/');
                	draw_hscroll_init(down_screen_addr, 26, 40 + (current_option_num-1)*27, 200, 
                	    COLOR_TRANS, COLOR_ACTIVE_ITEM, ext_pos+1);
				}

		      break;

            case CURSOR_RIGHT:
				lastest_game_menu_scroll_value = -5;
              break;

            case CURSOR_LEFT:
				lastest_game_menu_scroll_value = 5;
              break;

            default:
              break;
        }// end swith
	}

    void load_lastest_played()
    {
		char *ext_pos;

		draw_message(down_screen_addr, bg_screenp, 28, 31, 227, 165, 0);
		draw_string_vcenter(down_screen_addr, 36, 100, 190, COLOR_MSSG, msg[MSG_LOADING_GAME]);
		ds2_flipScreen(DOWN_SCREEN, DOWN_SCREEN_UPDATE_METHOD);

		if(gamepak_name[0] != 0)
		{
			S9xAutoSaveSRAM ();
			save_game_config_file();
		}

		if(bg_screenp != NULL) {
			bg_screenp_color = COLOR16(43, 11, 11);
		}
		else
			bg_screenp_color = COLOR_BG;

		ext_pos= strrchr(emu_config.latest_file[current_option_num -1], '/');
		*ext_pos= '\0';
		strcpy(rom_path, emu_config.latest_file[current_option_num -1]);
		*ext_pos= '/';

		ext_pos = emu_config.latest_file[current_option_num -1];

		ds2_setCPUclocklevel(13);
		int load_result = load_gamepak(ext_pos);
		ds2_setCPUclocklevel(0);

		if(load_result == -1) {
			first_load = 1;
			return;
		}

		strcpy(g_default_rom_dir, ext_pos);
		ext_pos = strrchr(g_default_rom_dir, '/');
		*ext_pos= '\0';
		strcpy(gamepak_name, ext_pos+1);


		load_game_config_file();
//		time_period_action();

		reorder_latest_file();
		get_savestate_filelist();
		game_fast_forward= 0;

		get_newest_savestate(tmp_filename);
		if(tmp_filename[0] != '\0')
		{
			load_state(tmp_filename);
		}

		return_value = 1;
		repeat = 0;
    }

    void keyremap_show()
    {
        unsigned short color;
        u32 line_num;
        u32 index;

        if(display_option->option_type & STRING_SELECTION_TYPE)
        {
            switch(i+1)
            {
                case 1: index= BUTTON_MAP_A; break;
                case 2: index= BUTTON_MAP_B; break;
                case 3: index= BUTTON_MAP_FA; break;
                case 4: index= BUTTON_MAP_FB; break;
                case 5: index= BUTTON_MAP_MENU; break;
                default: index= 0; break;
            }

            line_num= 0;
            while(index) {index >>= 1; line_num++;}
            sprintf(line_buffer, *(display_option->display_string), *((u32*)keyremap_options[line_num]));
        }
        else
            strcpy(line_buffer, *(display_option->display_string));
    
        line_num= display_option-> line_number;
        if(display_option == current_option)
            color= COLOR_ACTIVE_ITEM;
        else
            color= COLOR_INACTIVE_ITEM;
    
        PRINT_STRING_BG(down_screen_addr, line_buffer, color, COLOR_TRANS, 26, 37 + line_num*32);
    }

    void game_fastforward()
    {
    }
	


	void reload_cheats_page()
	{
		for(i = 0; i < CHEATS_PER_PAGE; i++)
		{
			cheats_options[i+1].display_string = &cheat_data_ptr[(CHEATS_PER_PAGE * menu_cheat_page) + i];
			cheats_options[i+1].current_option = &(Cheat.c[(CHEATS_PER_PAGE * menu_cheat_page) + i].enabled);
		}
	}

    void others_menu_init()
    {
		unsigned int total, used;

		//get card space info
		freespace = 0;
		fat_getDiskSpaceInfo("fat:", &total, &used, &freespace);
    }

	void choose_menu(MENU_TYPE *new_menu)
	{
		if(new_menu == NULL)
			new_menu = &main_menu;

		if(NULL != current_menu) {
			if(current_menu->end_function)
				current_menu->end_function();
		}

		current_menu = new_menu;
		current_option_num= current_menu -> focus_option;
		current_option = new_menu->options + current_option_num;
		if(current_menu->init_function)
			current_menu->init_function();
	}

//----------------------------------------------------------------------------//
//	Menu Start
	ds2_setCPUclocklevel(0);
	// mdelay(200); // Delete this delay
	ds2_setBacklight(3);
	
	
	wait_Allkey_release(0);
    bg_screenp= (u16*)malloc(256*192*2);

	repeat = 1;

	if(gamepak_name[0] == 0)
	{
		first_load = 1;
		if(CheckLoad_Arg())
			repeat = 0;
		else
		{
			ds2_clearScreen(UP_SCREEN, COLOR_BLACK);
			draw_string_vcenter(up_screen_addr, 0, 80, 256, COLOR_WHITE, msg[MSG_NON_LOAD_GAME]);
			ds2_flipScreen(UP_SCREEN, 1);
		}
	}
	else
	{
		/*
		 * It's pretty complicated. These two flips are needed because,
		 * otherwise, the menu freezes if S9xAutoSaveSRAM was called after
		 * loading from a save state.
		 */
		ds2_flipScreen(UP_SCREEN, 1);
		ds2_flipScreen(UP_SCREEN, 1);
	}

	choose_menu(&main_menu);
//	Menu loop
	
	while(repeat)
	{
	
		
	
		display_option = current_menu->options;
		string_select= 0;

		if(current_menu -> passive_function)
			current_menu -> passive_function();
		else
		{
			u32 line_num, screen_focus, focus_option;

			//draw background
			show_icon(down_screen_addr, ICON_SUBBG, 0, 0);
			show_icon(down_screen_addr, ICON_TITLE, 0, 0);
			show_icon(down_screen_addr, ICON_TITLEICON, 12, 9);

			strcpy(line_buffer, *(display_option->display_string));
			draw_string_vcenter(down_screen_addr, 0, 9, 256, COLOR_ACTIVE_ITEM, line_buffer);

			line_num = current_option_num;
			screen_focus = current_menu -> screen_focus;
			focus_option = current_menu -> focus_option;

			if(focus_option < line_num)	//focus next option
			{
				focus_option = line_num - focus_option;
				screen_focus += focus_option;
				if(screen_focus > SUBMENU_ROW_NUM)	//Reach max row numbers can display
					screen_focus = SUBMENU_ROW_NUM;

				current_menu -> screen_focus = screen_focus;
				focus_option = line_num;
			}
			else if(focus_option > line_num)	//focus last option
			{
				focus_option = focus_option - line_num;
				if(screen_focus > focus_option)
					screen_focus -= focus_option;
				else
					screen_focus = 0;

				if(screen_focus == 0 && line_num > 0)
					screen_focus = 1;

				current_menu -> screen_focus = screen_focus;
				focus_option = line_num;
			}
			current_menu -> focus_option = focus_option;
		
			i = focus_option - screen_focus;
			display_option += i +1;

			line_num = current_menu->num_options-1;
			if(line_num > SUBMENU_ROW_NUM)
				line_num = SUBMENU_ROW_NUM;

			if(focus_option == 0)
				show_icon(down_screen_addr, ICON_BACK, 229, 10);
			else
				show_icon(down_screen_addr, ICON_NBACK, 229, 10);

			for(i= 0; i < line_num; i++, display_option++)
    	    {
    	        unsigned short color;

				if(display_option == current_option)
					show_icon(down_screen_addr, ICON_SUBSELA, 6, 35 + i*27);

				if(display_option->passive_function)
				{
					display_option->line_number = i;
					display_option->passive_function();
				}
				else if(display_option->option_type & NUMBER_SELECTION_TYPE)
				{
					sprintf(line_buffer, *(display_option->display_string),
						*(display_option->current_option));
				}
				else if(display_option->option_type & STRING_SELECTION_TYPE)
				{
					sprintf(line_buffer, *(display_option->display_string),
						*((u32*)(((u32 *)display_option->options)[*(display_option->current_option)])));
				}
				else
				{
					strcpy(line_buffer, *(display_option->display_string));
				}

				if(display_option->passive_function == NULL)
				{
					if(display_option == current_option)
						color= COLOR_ACTIVE_ITEM;
					else
						color= COLOR_INACTIVE_ITEM;
	
					PRINT_STRING_BG(down_screen_addr, line_buffer, color, COLOR_TRANS, 23, 40 + i*27);
				}
    	    }
    	}

		struct key_buf inputdata;
		gui_action = get_gui_input();

		switch(gui_action)
		{
			case CURSOR_TOUCH:
				ds2_getrawInput(&inputdata);
				/* Back button at the top of every menu but the main one */
				if(current_menu != &main_menu && inputdata.x > 231 && inputdata.y <= 25)
				{
					choose_menu(current_menu->options->sub_menu);
					break;
				}
				/* Main menu */
				if(current_menu == &main_menu)
				{
					// 0     86    172   256
					//  _____ _____ _____  0
					// |0VID_|1SAV_|2CHT_| 80
					// |3TLS_|4OPT_|5EXI_| 160
					// |6NEW_|7RET_|8RST_| 192

					current_option_num = (inputdata.y / 80) * 3 + (inputdata.x / 86);
					current_option = current_menu->options + current_option_num;
					
					if(current_option -> option_type & HIDEN_TYPE)
						break;
					else if(current_option->option_type & ACTION_TYPE)
						current_option->action_function();
					else if(current_option->option_type & SUBMENU_TYPE)
						choose_menu(current_option->sub_menu);
				}
				/* This is the majority case, covering all menus except save states, screen shots, and game loading */
				else if(current_menu != (main_menu.options + 1)->sub_menu 
				&& current_menu != ((main_menu.options +1)->sub_menu->options + 3)->sub_menu
				&& current_menu != (main_menu.options +3)->sub_menu 
				&& current_menu != ((main_menu.options +3)->sub_menu->options + 1)->sub_menu
				&& current_menu != ((main_menu.options +3)->sub_menu->options + 2)->sub_menu
				&& current_menu != (main_menu.options +6)->sub_menu
				&& current_menu != ((main_menu.options +6)->sub_menu->options + 2)->sub_menu)
				{
					if (inputdata.y <= 33 || inputdata.y > 192)
						break;
					// ___ 33        This screen has 6 possible rows. Touches
					// ___ 60        above or below these are ignored.
					// . . . (+27)   The row between 33 and 60 is [1], though!
					// ___ 192
					current_option_num = (inputdata.y - 33) / 27 + 1;

					current_option = current_menu->options + current_option_num;

					if(current_option -> option_type & HIDEN_TYPE)
						break;

					if(!current_option)
						break;

					if(current_menu->key_function)
					{
						gui_action = CURSOR_RIGHT;
						current_menu->key_function();
					}
					else if(current_option->option_type & (NUMBER_SELECTION_TYPE | STRING_SELECTION_TYPE))
					{
						u32 current_option_val = *(current_option->current_option);

						if(current_option_val <  current_option->num_options -1)
							current_option_val++;
						else
							current_option_val= 0;
						*(current_option->current_option) = current_option_val;

						if(current_option->action_function)
							current_option->action_function();
					}
				}
				/* Save states */
				else if(current_menu == (main_menu.options + 1)->sub_menu)
				{
					if(inputdata.y <= 33)
						break;
					else if(inputdata.y <= 60)
						current_option_num = 1;
					else if(inputdata.y <= 87)
						break;
					else if(inputdata.y <= 114)
						current_option_num = 2;
					else if(inputdata.y <= 141)
						current_option_num = 3;
					else
						break;
					
					current_option = current_menu->options + current_option_num;
					
					if(current_option_num == 2)
					{
						u32 current_option_val = *(current_option->current_option);
						u32 old_option_val = current_option_val;

						if(inputdata.x <= 23 || inputdata.x > 233)
							break;
						// |  |  |  |  |  |  |  |  |  |  |
						// 23 44 65 86 ... (+21)         233
						// This row has 10 cells for save states, each 21
						// pixels wide.
						current_option_val = (inputdata.x - 23) / 21;

						*(current_option->current_option) = current_option_val;

						if(current_option_val == old_option_val)
						{
							gui_action = CURSOR_SELECT;
							if(current_option->option_type & ACTION_TYPE)
								current_option->action_function();
							else if(current_option->option_type & SUBMENU_TYPE)
								choose_menu(current_option->sub_menu);
						}
						break;
					}
					
					gui_action = CURSOR_SELECT;
					if(current_option -> option_type & HIDEN_TYPE)
						break;
					else if(current_option->option_type & ACTION_TYPE)
						current_option->action_function();
					else if(current_option->option_type & SUBMENU_TYPE)
						choose_menu(current_option->sub_menu);
				}
				/* Delete state sub menu */
				else if(current_menu == ((main_menu.options + 1)->sub_menu->options + 3)->sub_menu)
				{
					if(inputdata.y <= 33)
						break;
					else if(inputdata.y <= 60)
						current_option_num = 1;
					else if(inputdata.y <= 87)
						break;
					else if(inputdata.y <= 114)
						current_option_num = 2;
					else
						break;
					
					current_option = current_menu->options + current_option_num;
					
					if(current_option_num == 2)
					{
						u32 current_option_val = *(current_option->current_option);
						u32 old_option_val = current_option_val;

						if(inputdata.x <= 23 || inputdata.x > 233)
							break;
						// |  |  |  |  |  |  |  |  |  |  |
						// 23 44 65 86 ... (+21)         233
						// This row has 10 cells for save states, each 21
						// pixels wide.
						current_option_val = (inputdata.x - 23) / 21;

						*(current_option->current_option) = current_option_val;

						if(current_option_val == old_option_val)
						{
							gui_action = CURSOR_SELECT;
							if(current_option->option_type & ACTION_TYPE)
								current_option->action_function();
							else if(current_option->option_type & SUBMENU_TYPE)
								choose_menu(current_option->sub_menu);
						}
						break;
					}
					
					gui_action = CURSOR_SELECT;
					if(current_option -> option_type & HIDEN_TYPE)
						break;
					else if(current_option->option_type & ACTION_TYPE)
						current_option->action_function();
					else if(current_option->option_type & SUBMENU_TYPE)
						choose_menu(current_option->sub_menu);
				}
				/* Screenshots and new game loading */
				else if(current_menu == (main_menu.options + 3)->sub_menu
				|| current_menu == ((main_menu.options +3)->sub_menu->options + 1)->sub_menu
				|| current_menu == (main_menu.options + 6)->sub_menu
				|| current_menu == ((main_menu.options +6)->sub_menu->options + 2)->sub_menu)
				{
					if (inputdata.y <= 33 || inputdata.y > 192)
						break;
					// ___ 33        This screen has 6 possible rows. Touches
					// ___ 60        above or below these are ignored.
					// . . . (+27)   The row between 33 and 60 is [1], though!
					// ___ 192
					current_option_num = (inputdata.y - 33) / 27 + 1;

					current_option = current_menu->options + current_option_num;

					if(current_option -> option_type & HIDEN_TYPE)
						break;
					else if(current_option->option_type & ACTION_TYPE)
						current_option->action_function();
					else if(current_option->option_type & SUBMENU_TYPE)
						choose_menu(current_option->sub_menu);
				}
				break;
			case CURSOR_DOWN:
				if(current_menu->key_function)
					current_menu->key_function();
				else
				{
					current_option_num = (current_option_num + 1) % current_menu->num_options;
					current_option = current_menu->options + current_option_num;

					while(current_option -> option_type & HIDEN_TYPE)
					{
						current_option_num = (current_option_num + 1) % current_menu->num_options;
						current_option = current_menu->options + current_option_num;
					}
				}
				break;

			case CURSOR_UP:
				if(current_menu->key_function)
					current_menu->key_function();
				else
				{
					if(current_option_num)
						current_option_num--;
					else
						current_option_num = current_menu->num_options - 1;
					current_option = current_menu->options + current_option_num;

					while(current_option -> option_type & HIDEN_TYPE)
					{
						if(current_option_num)
							current_option_num--;
						else
							current_option_num = current_menu->num_options - 1;
						current_option = current_menu->options + current_option_num;
					}
				}
				break;

			case CURSOR_RIGHT:
				if(current_menu->key_function)
					current_menu->key_function();
				else
				{
					if(current_option->option_type & (NUMBER_SELECTION_TYPE | STRING_SELECTION_TYPE))
					{
						u32 current_option_val = *(current_option->current_option);

						if(current_option_val <  current_option->num_options -1)
							current_option_val++;
						else
							current_option_val= 0;
						*(current_option->current_option) = current_option_val;

						if(current_option->action_function)
							current_option->action_function();
					}
				}
				break;

			case CURSOR_LEFT:
				if(current_menu->key_function)
					current_menu->key_function();
				else
				{
					if(current_option->option_type & (NUMBER_SELECTION_TYPE | STRING_SELECTION_TYPE))
					{
						u32 current_option_val = *(current_option->current_option);

						if(current_option_val)
							current_option_val--;
						else
							current_option_val = current_option->num_options - 1;
						*(current_option->current_option) = current_option_val;

						if(current_option->action_function)
							current_option->action_function();
					}
				}
				break;

			case CURSOR_EXIT:
				break;

			case CURSOR_SELECT:
				if(current_option->option_type & ACTION_TYPE)
					current_option->action_function();
				else if(current_option->option_type & SUBMENU_TYPE)
					choose_menu(current_option->sub_menu);
				break;

			case CURSOR_BACK: 
				if(current_menu != &main_menu)
					choose_menu(current_menu->options->sub_menu);
				else
					menu_return();
				break;

			default:
				break;
		} // end swith

		ds2_flipScreen(DOWN_SCREEN, DOWN_SCREEN_UPDATE_METHOD);
	} // end while
	destroy_dynamic_cheats();
	if(bg_screenp != NULL) free((void*)bg_screenp);
	
	if(gamepak_name[0] != 0)
	{
		game_config.clock_speed_number = clock_speed_number;

		reorder_latest_file();
		S9xAutoSaveSRAM ();
		save_game_config_file();
	}
	save_emu_config_file();
	set_cpu_clock(clock_speed_number);
	
	ds2_clearScreen(DOWN_SCREEN, 0);
	ds2_flipScreen(DOWN_SCREEN, 1);
	copy_screen(up_screen_addr, (void*) screen, 0, 0, 256, 192);
	ds2_flipScreen(UP_SCREEN, UP_SCREEN_UPDATE_METHOD);
	ds2_setBacklight(2);

	wait_Allkey_release(0);

	return return_value;
}

/*--------------------------------------------------------
	Initialize default path
--------------------------------------------------------*/
void initial_path_config(void)
{
    //Initial directory path
    sprintf(g_default_rom_dir, "%s/gamepak", main_path);
    sprintf(DEFAULT_RTS_DIR, "%s/gamerts", main_path);
    sprintf(DEFAULT_CFG_DIR, "%s/gamerts", main_path);
    sprintf(DEFAULT_SS_DIR, "%s/gamepic", main_path);
    sprintf(DEFAULT_CHEAT_DIR, "%s/gamecht", main_path);
}

/*--------------------------------------------------------
	Load language message
--------------------------------------------------------*/
int load_language_msg(char *filename, u32 language)
{
    FILE *fp;
    char msg_path[MAX_PATH];
    char string[256];
    char start[32];
    char end[32];
    char *pt, *dst;
    u32 cmplen;
    u32 loop, offset, len;
    int ret;

    sprintf(msg_path, "%s/%s", main_path, filename);
    fp = fopen(msg_path, "rb");
    if(fp == NULL)
        return -1;

    switch(language)
    {
    case ENGLISH:
	default:
        strcpy(start, "STARTENGLISH");
        strcpy(end, "ENDENGLISH");
        cmplen= 12;
        break;
    case CHINESE_SIMPLIFIED:
        strcpy(start, "STARTCHINESESIM");
        strcpy(end, "ENDCHINESESIM");
        cmplen= 15;
        break;
	case FRENCH:
        strcpy(start, "STARTFRENCH");
        strcpy(end, "ENDFRENCH");
        cmplen= 11;
        break;
    }
    //find the start flag
    ret= 0;
    while(1)
    {
        pt= fgets(string, 256, fp);
        if(pt == NULL)
        {
            ret= -2;
            goto load_language_msg_error;
        }

        if(!strncmp(pt, start, cmplen))
            break;
    }

    loop= 0;
    offset= 0;
    dst= msg_data;
    msg[0]= dst;

    while(loop != MSG_END)
    {
        while(1)
        {
            pt = fgets(string, 256, fp);
            if(pt[0] == '#' || pt[0] == 0x0D || pt[0] == 0x0A)
                continue;
            if(pt != NULL)
                break;
            else
            {
                ret = -3;
                goto load_language_msg_error;
            }
        }

        if(!strncmp(pt, end, cmplen-2))
            break;

        len= strlen(pt);
        memcpy(dst, pt, len);

        dst += len;
        //at a line return, when "\n" paded, this message not end
        if(*(dst-1) == 0x0A)
        {
            pt = strrchr(pt, '\\');
            if((pt != NULL) && (*(pt+1) == 'n'))
            {
                if(*(dst-2) == 0x0D)
                {
                    *(dst-4)= '\n';
                    dst -= 3;
                }
                else
                {
                    *(dst-3)= '\n';
                    dst -= 2;
                }
            }
            else//a message end
            {
                if(*(dst-2) == 0x0D)
                    dst -= 1;
                *(dst-1) = '\0';
                msg[++loop] = dst;
            }
        }
    }

#if 0
	loop= 0;
	printf("------\n");
	while(loop != MSG_END)
		printf("%d: %s\n",loop, msg[loop++]);
#endif

load_language_msg_error:
	fclose(fp);
	return ret;
}

/*--------------------------------------------------------
--------------------------------------------------------*/


/*--------------------------------------------------------
	Load font library
--------------------------------------------------------*/
u32 load_font()
{
    return (u32)BDF_font_init();
}

/*--------------------------------------------------------
	Game configure initialize
--------------------------------------------------------*/
void init_game_config(void)
{
    u32 i;

	game_config.clock_speed_number = 5;	// 396 MHz by default
	clock_speed_number = 5;
	game_config.graphic = 3; // By default, have a good-looking aspect ratio

	game_config.gamepad_config_menu = BUTTON_ID_TOUCH;
    memcpy(game_config.gamepad_config_map, gamepad_config_map_init, sizeof(gamepad_config_map_init));

	game_config.backward = 0;	//time backward disable
	game_config.backward_time = 2;	//time backward granularity 1s

    savestate_index= 0;
    for(i= 0; i < SAVE_STATE_SLOT_NUM; i++)
    {
        savestate_map[i]= (char)(-(i+1));	//empty
    }
}

/*--------------------------------------------------------
	Emulator configure initialize
--------------------------------------------------------*/
void init_emulator_config(void)
{
	emu_config.language = 0;     //defalut language= English

	emu_config.rom_file[0]= 0;
	emu_config.rom_path[0]= 0;
	memset(emu_config.latest_file, 0, sizeof(emu_config.latest_file));

	gamepak_name[0] = '\0';
}

/*--------------------------------------------------------
	Load game configure file
--------------------------------------------------------*/
void load_game_config_file(void)
{
	char game_config_filename[MAX_PATH];
	FILE* fp;
	char *pt;

	//Set default
    init_game_config();

	sprintf(game_config_filename, "%s/%s", DEFAULT_CFG_DIR, gamepak_name);
	pt= strrchr(game_config_filename, '.');
	if(NULL == pt)
		return;
	*pt= 0;
	strcat(game_config_filename, "_0.rts");

	fp = fopen(game_config_filename, "r");
    if(NULL == fp)
		return;

	//Check file header
	pt= game_config_filename;
	fread(pt, 1, GAME_CONFIG_HEADER_SIZE, fp);

	if (!strncmp(pt, GAME_CONFIG_HEADER, GAME_CONFIG_HEADER_SIZE))
	{
		fread(&game_config, 1, sizeof(GAME_CONFIG), fp);

		memcpy(gamepad_config_map, game_config.gamepad_config_map, sizeof(game_config.gamepad_config_map));
		gamepad_config_menu = game_config.gamepad_config_menu;
		clock_speed_number = game_config.clock_speed_number;
	}

	fclose(fp);
}

/*--------------------------------------------------------
	Load emulator configure file
--------------------------------------------------------*/
int load_emu_config_file(void)
{
	char tmp_path[MAX_PATH];
	FILE* fp;
	char *pt;

	sprintf(tmp_path, "%s/%s", main_path, EMU_CONFIG_FILENAME);

	fp = fopen(tmp_path, "r");
	if(NULL != fp)
	{
		// check the file header
		pt= tmp_path;
		fread(pt, 1, EMU_CONFIG_HEADER_SIZE, fp);
		pt[EMU_CONFIG_HEADER_SIZE]= 0;
		if(!strcmp(pt, EMU_CONFIG_HEADER))
		{
			fread(&emu_config, 1, sizeof(emu_config), fp);
			fclose(fp);
			return 0;
		}
	}

	//have no confiure file, set default
	init_emulator_config();
    return -1;
}

/*--------------------------------------------------------
	Save game configure file
--------------------------------------------------------*/
int save_game_config_file(void)
{
    char game_config_filename[MAX_PATH];
    FILE* fp;
    char *pt;

    if(gamepak_name[0] == 0) return -1;

    memcpy(game_config.gamepad_config_map, gamepad_config_map, sizeof(game_config.gamepad_config_map));
    game_config.gamepad_config_menu = gamepad_config_menu;

    sprintf(game_config_filename, "%s/%s", DEFAULT_CFG_DIR, gamepak_name);
    pt = strrchr(game_config_filename, '.');
	if(NULL == pt) 
		return -1;

    *pt = '\0';
    strcat(pt, "_0.rts");

	fp = fopen(game_config_filename, "w");
	if(NULL != fp)
    {
		fwrite(GAME_CONFIG_HEADER, 1, GAME_CONFIG_HEADER_SIZE, fp);
		fwrite(&game_config, 1, sizeof(game_config), fp);
		fclose(fp);
        return 0;
    }

    return -1;
}

/*--------------------------------------------------------
	Save emulator confiure file
--------------------------------------------------------*/
int save_emu_config_file()
{
    char tmp_path[MAX_PATH];
	FILE* fp;

    sprintf(tmp_path, "%s/%s", main_path, EMU_CONFIG_FILENAME);
	fp = fopen(tmp_path, "w");
	if(NULL != fp)
    {
		fwrite(EMU_CONFIG_HEADER, 1, EMU_CONFIG_HEADER_SIZE, fp);
		fwrite(&emu_config, 1, sizeof(emu_config), fp);
		fclose(fp);
        return 0;
    }

    return -1;
}

/*--------------------------------------------------------
	Reorder latest player game recorder
--------------------------------------------------------*/
void reorder_latest_file(void)
{
    char *ext_pos, *ext_pos1;
    u32 i, len;
    char full_file[512];

    if(gamepak_name[0] == '\0')
        return;

    for(i= 0; i < 5; i++)
    {
        ext_pos= strrchr(emu_config.latest_file[i], '/');
        if(ext_pos != NULL)
        {
            ext_pos1= strrchr(ext_pos + 1, '.');
            len= ext_pos1 - ext_pos -1;
            if(!strncasecmp(ext_pos + 1, gamepak_name, len))
                break;
        }
    }

    //some one match, move to last
    if(i < 5)
    {
        if(i < 4)
        {
            strcpy(full_file, emu_config.latest_file[i]);
            for(i+=1; i < 5; i++)
            {
                if(emu_config.latest_file[i][0] != '\0')
                    strcpy(emu_config.latest_file[i-1], emu_config.latest_file[i]);
                else
                    break;
            }
            
            strcpy(emu_config.latest_file[i-1], full_file);
        }
        return ;
    }

    //none match
    for(i= 0; i < 5; i++)
    {
        if(emu_config.latest_file[i][0] == '\0')
        {
            sprintf(emu_config.latest_file[i], "%s/%s", g_default_rom_dir, gamepak_name);
            break;
        }
    }

    if(i < 5) return;

    //queue full
    for(i=1; i < 5; i++)
        strcpy(emu_config.latest_file[i-1], emu_config.latest_file[i]);

    sprintf(emu_config.latest_file[i-1], "%s/%s", g_default_rom_dir, gamepak_name);
}

/*--------------------------------------------------------
	Save state related
--------------------------------------------------------*/
static int rtc_time_cmp(struct rtc *t1, struct rtc *t2)
{
    int result;

    result= (int)((unsigned int)(t1 -> year) - (unsigned int)(t2 -> year));
    if(result != 0)
        return result;
    result= (int)((unsigned int)(t1 -> month) - (unsigned int)(t2 -> month));
    if(result != 0)
        return result;
    result= (int)((unsigned int)(t1 -> day) - (unsigned int)(t2 -> day));
    if(result != 0)
        return result;
    result= (int)((unsigned int)(t1 -> weekday) - (unsigned int)(t2 -> weekday));
    if(result != 0)
        return result;
    result= (int)((unsigned int)(t1 -> hours) - (unsigned int)(t2 -> hours));
    if(result != 0)
        return result;
    result= (int)((unsigned int)(t1 -> minutes) - (unsigned int)(t2 -> minutes));
    if(result != 0)
        return result;
    result= (int)((unsigned int)(t1 -> seconds) - (unsigned int)(t2 -> seconds));
//    if(result != 0)
	return result;
}

static void get_savestate_filelist(void)
{
	struct rtc  current_time[SAVE_STATE_SLOT_NUM];
	int i;
	char savestate_path[MAX_PATH];
	char postdrix[8];
	char *pt;
	FILE *fp;
	unsigned int n, m;

	memset((char*)current_time, 0xFF, sizeof(current_time));
	for(i= 0; i < SAVE_STATE_SLOT_NUM; i++)
	{
		savestate_map[i]= (char)(-(i+1));
	}

	sprintf(savestate_path, "%s/%s", DEFAULT_RTS_DIR, gamepak_name);
	pt= strrchr(savestate_path, '.');
	for(i= 0; i < SAVE_STATE_SLOT_NUM; i++)
	{
		sprintf(postdrix, "_%d.rts", i+1);
		strcpy(pt, postdrix);

		fp= fopen(savestate_path, "r");
		if (fp != NULL)
		{
			m = fread((void*)&n, 1, 4, fp);
			if(m < 4) {
				fclose(fp);
				continue;
			}

			fseek(fp, n, SEEK_SET);
			/* Read back the time stamp */
			fread((char*)&current_time[i], 1, sizeof(struct rtc), fp);
			savestate_map[i] = (char)(i+1);

			fclose(fp);
		}
	}

	int k, reslut;
	struct rtc current_time_tmp;
	for(i= 0; i < SAVE_STATE_SLOT_NUM-1; i++)
	{
		for(k=0; k < SAVE_STATE_SLOT_NUM-1-i; k++)
		{
			reslut= rtc_time_cmp(&current_time[k], &current_time[k+1]);
			if(reslut > 0)
			{
				current_time_tmp = current_time[k];
				current_time[k] = current_time[k+1];
				current_time[k+1] = current_time_tmp;
				reslut = savestate_map[k];
				savestate_map[k] = savestate_map[k+1];
				savestate_map[k+1] = reslut;
			}
		}
	}

	savestate_index= get_savestate_slot();
	if(savestate_index < 0) savestate_index = 0;
}

static void get_savestate_filename(u32 slot, char *name_buffer)
{
	char savestate_ext[16];
	char *pt;

	sprintf(savestate_ext, "_%d.rts", savestate_map[slot]);
	pt = strrchr(gamepak_name, '/');
	if(NULL == pt)
		pt = gamepak_name;
	else
		pt += 1;

	change_ext(pt, name_buffer, savestate_ext);
}

static u32 get_savestate_slot(void)
{
    s32 i;
    char *ptr;

    i= SAVE_STATE_SLOT_NUM -1;
    ptr= savestate_map;
    while(i >= 0)
    {
        if(ptr[i] > 0) break;
		i--;
    }

    return i;
}

static void reorder_savestate_slot(void)
{
    u32 x, y;
    char *ptr;
    s32 tmp;
    
    ptr= savestate_map;
    for(x= 0; x < SAVE_STATE_SLOT_NUM; x++)
    {
        tmp= ptr[x];
        if(tmp< 0)
        {
            for(y= x+1; y< SAVE_STATE_SLOT_NUM; y++)
                ptr[y-1]= ptr[y];
            ptr[SAVE_STATE_SLOT_NUM-1]= tmp;	//empty are moved to last
        }
    }
}

void get_newest_savestate(char *name_buffer)
{
    int i;

    i= get_savestate_slot();
    if (i < 0)
    {
        name_buffer[0]= '\0';
        return;
    }

    get_savestate_filename(i, name_buffer);
}

static u32 parse_line(char *current_line, char *current_str)
{
	char *line_ptr;
	char *line_ptr_new;

	line_ptr = current_line;
	/* NULL or comment or other */
	if((current_line[0] == 0) || (current_line[0] == '#') || (current_line[0] != '!'))
		return -1;

	line_ptr++;

	line_ptr_new = strchr(line_ptr, '\r');
	while (line_ptr_new != NULL)
	{
		*line_ptr_new = '\n';
		line_ptr_new = strchr(line_ptr, '\r');
	}

	line_ptr_new = strchr(line_ptr, '\n');
	if (line_ptr_new == NULL)
		return -1;

	*line_ptr_new = 0;

	// "\n" to '\n'
	line_ptr_new = strstr(line_ptr, "\\n");
	while (line_ptr_new != NULL)
	{
		*line_ptr_new = '\n';
		memmove((line_ptr_new + 1), (line_ptr_new + 2), (strlen(line_ptr_new + 2) + 1));
		line_ptr_new = strstr(line_ptr_new, "\\n");
	}

	strcpy(current_str, line_ptr);
	return 0;
}

static void get_timestamp_string(char *buffer, u16 msg_id, u16 year, u16 mon, 
    u16 day, u16 wday, u16 hour, u16 min, u16 sec, u32 msec)
{
    char *weekday_strings[] =
    {
        "SUN", "MON", "TUE", "WED", "TUR", "FRI", "SAT"
    };

    sprintf(buffer, "%s %02d/%02d/%04d %02d:%02d:%02d", weekday_strings[wday], 
        day, mon, year, hour, min, sec);
}

static void get_time_string(char *buff, struct rtc *rtcp)
{
    get_timestamp_string(buff, 0,
                            rtcp -> year +2000,
                            rtcp -> month,
                            rtcp -> day,
                            rtcp -> weekday,
                            rtcp -> hours,
                            rtcp -> minutes,
                            rtcp -> seconds,
                            0);
}

static u32 save_ss_bmp(u16 *image)
{
    static unsigned char header[] ={ 'B',  'M',  0x00, 0x00, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00,
                                    0x28, 0x00, 0x00, 0x00,  0, 0x01, 0x00,
                                    0x00,  192, 0x00, 0x00, 0x00, 0x01, 0x00,
                                    0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00, 0x00};

    char ss_filename[MAX_FILE];
    char save_ss_path[MAX_PATH];
    struct rtc current_time;
    char rgb_data[256*192*3];
    unsigned int x,y;
    unsigned short col;
    unsigned char r,g,b;

    change_ext(gamepak_name, ss_filename, "_");
	ds2_getTime(&current_time);
    sprintf(save_ss_path, "%s/%s%02d%02d%02d%02d%02d.bmp", DEFAULT_SS_DIR, ss_filename, 
    current_time.month, current_time.day, current_time.hours, current_time.minutes, current_time.seconds);

    for(y = 0; y < 192; y++)
    {
        for(x = 0; x < 256; x++)
        {
            col = image[x + y * 256];
            r = (col >> 10) & 0x1F;
            g = (col >> 5) & 0x1F;
            b = (col) & 0x1F;

            rgb_data[(191-y)*256*3+x*3+2] = b << 3;
            rgb_data[(191-y)*256*3+x*3+1] = g << 3;
            rgb_data[(191-y)*256*3+x*3+0] = r << 3;
        }
    }

    FILE *ss = fopen( save_ss_path, "wb" );
    if( ss == NULL ) return 0;
    fwrite( header, sizeof(header), 1, ss );
    fwrite( rgb_data, 1, 256*192*3, ss);
    fclose( ss );

    return 1;
}

void quit(void)
{
/*
  u32 reg_ra;

  __asm__ __volatile__("or %0, $0, $ra"
                        : "=r" (reg_ra)
                        :);
  
  dbg_printf("return address= %08x\n", reg_ra);
*/

#ifdef USE_DEBUG
	fclose(g_dbg_file);
#endif

	ds2_plug_exit();
	while(1);
}

u32 file_length(FILE* file)
{
  u32 pos, size;
  pos= ftell(file);
  fseek(file, 0, SEEK_END);
  size= ftell(file);
  fseek(file, pos, SEEK_SET);

  return size;
}

/*
*	GUI Initialize
*/
void gui_init(u32 lang_id)
{
	int flag;

	ds2_setCPUclocklevel(13); // Crank it up. When the menu starts, -> 0.
	printf_clock();

    //Find the "CATSFC" system directory
    DIR *current_dir;

    strcpy(main_path, "fat:/CATSFC");
    current_dir = opendir(main_path);
    if(current_dir)
        closedir(current_dir);
    else
    {
        strcpy(main_path, "fat:/_SYSTEM/PLUGINS/CATSFC");
        current_dir = opendir(main_path);
        if(current_dir)
            closedir(current_dir);
        else
        {
            strcpy(main_path, "fat:");
            if(search_dir("CATSFC", main_path) == 0)
            {
                printf("Found CATSFC directory\r\nDossier CATSFC trouve\r\n\r\n%s\r\n", main_path);
            }
            else
            {
				err_msg(DOWN_SCREEN, "/CATSFC: Directory missing\r\nPress any key to return to\r\nthe menu\r\n\r\n/CATSFC: Dossier manquant\r\nAppuyer sur une touche pour\r\nretourner au menu");
                goto gui_init_err;
            }
        }
    }

	show_log(down_screen_addr);
	ds2_flipScreen(DOWN_SCREEN, DOWN_SCREEN_UPDATE_METHOD);

	flag = icon_init(lang_id);
	if(0 != flag)
	{
		err_msg(DOWN_SCREEN, "Some icons are missing\r\nLoad them onto your card\r\nPress any key to return to\r\nthe menu\r\n\r\nDes icones sont manquantes\r\nChargez-les sur votre carte\r\nAppuyer sur une touche pour\r\nretourner au menu");
		goto gui_init_err;
	}


	flag = load_font();
	if(0 != flag)
	{
		char message[512];
		sprintf(message, "Font library initialisation\r\nerror (%d)\r\nPress any key to return to\r\nthe menu\r\n\r\nErreur d'initalisation de la\r\npolice de caracteres (%d)\r\nAppuyer sur une touche pour\r\nretourner au menu", flag, flag);
		err_msg(DOWN_SCREEN, message);
		goto gui_init_err;
	}

	load_emu_config_file();
	lang_id = emu_config.language;

	flag = load_language_msg(LANGUAGE_PACK, lang_id);
	if(0 != flag)
	{
		char message[512];
		sprintf(message, "Language pack initialisation\r\nerror (%d)\r\nPress any key to return to\r\nthe menu\r\n\r\nErreur d'initalisation du\r\npack de langue (%d)\r\nAppuyer sur une touche pour\r\nretourner au menu", flag, flag);
		err_msg(DOWN_SCREEN, message);
		goto gui_init_err;
	}

	initial_path_config();

	return;

gui_init_err:
	ds2_flipScreen(DOWN_SCREEN, DOWN_SCREEN_UPDATE_METHOD);
	wait_Anykey_press(0);
	quit();
}
