/* gcheat.h
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

#ifndef __GCHEAT_H__
#define __GCHEAT_H__

#ifdef __cplusplus
extern "C" {
#endif

#define CHEAT_NAME_LENGTH (32)
#define MAX_CHEATS_PAGE 10
#define CHEATS_PER_PAGE 4
#define MAX_CHEATS (MAX_CHEATS_PAGE * CHEATS_PER_PAGE)

//Support EMU Cheat(emulator cheat) code
typedef struct
{
	u32 name_id;	//name ID in another table
	u32 active;		//status
	u16 item_num;	//sub-item number
	u16 sub_active;
	u32 item_id;	//There is another struct array to store the cheat data
	char name_shot[CHEAT_NAME_LENGTH];
	u32	reserved;
} GCHEAT_STRUCT;

typedef struct
{
	unsigned char** msg_index;
	unsigned char* msg_pool;
	unsigned int msg_num;
} MSG_TABLE;

extern GCHEAT_STRUCT gcheat[MAX_CHEATS];
extern unsigned int g_cheat_cell_num;
extern unsigned int g_cheat_num;

extern int load_cheatfile(const char* filename, unsigned int *string_num, 
	unsigned int *string_len, GCHEAT_STRUCT *gcheat);
extern int load_cheatname(const char* filename, unsigned int string_num, 
	unsigned int string_len, MSG_TABLE* mssg_table);
extern void gcheat_Managment(GCHEAT_STRUCT *gcheat);

#ifdef __cplusplus
}
#endif

#endif //__GCHEAT_H__
