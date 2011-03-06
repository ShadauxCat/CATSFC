/* gcheat.c
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

#include "string.h"
#include "fs_api.h"
#include "ds2_malloc.h"
#include "gcheat.h"
#include "charsets.h"

#define MAX_SFCCHEAT_NAME 24


//GCHEAT_STRUCT gcheat[MAX_CHEATS];
unsigned int g_cheat_cell_num;
unsigned int g_cheat_num;

#define SKIP_SPACE(pt)	while(' ' == *pt) pt++

static unsigned char* check_is_cht(unsigned char *str)
{
	unsigned char *pt, *pt1;

	if(*str == '\0') return NULL;

	pt = str;
	while(*pt == ' ') pt++;			//Skip leading space
	if(*pt != '[') return NULL;		//valid entry should be:[string]

	pt1 = strrchr(str, ']');
	if(pt1 == NULL) return NULL;

	while(*(--pt1) == ' ');
	*(pt1+1) = '\0';				//Cut trailing space between string and ']'

	while(*(++pt) == ' ');			//Cut space between '[' and string

	return pt;
}

static unsigned int sscanf_hex_value(unsigned char* str, unsigned int *value)
{
	unsigned char *pt;
	unsigned int tmp;
	unsigned char ch;
	unsigned int len;

	pt = str;
	len = 0;
	tmp = 0;
	while(*pt && len < 8)
	{
		ch = *pt;
		if(ch >= 'a' && ch <= 'f') ch = ch - 'a' + 0xa;
		else if(ch >= 'A' && ch <= 'F') ch = ch - 'A' + 0xa;
		else if(ch >= '0' && ch <= '9') ch = ch - '0';
		else if(ch == ' ') continue;
		else break;

		tmp = (tmp << 4) | ch;
		pt++;
		len += 1;
	}

	*value = tmp;
	return len;
}

/*
*	Convert the src string to UTF8 coding dst string, and cut to length
*/
int string2utf8(unsigned char *src, unsigned char* dst, unsigned int length)
{
	unsigned char *pt;
	unsigned char ch;
	unsigned short ucode;
	unsigned int type;
	unsigned int len;

	len = 0;
	type = 0;
	pt = src;
	while(*pt)
	{
		pt = utf8decode(pt, &ucode);
		if(ucode < 0x4e00) {
			if(ucode == 0 || ucode > 0x7F) {
				type = 1;
				break;
			}
		} else if(ucode > 0x9FCF) {
			type = 1;
			break;
		}
		else
			len++;

		if(len >= 3) break;	//There is enough UTF8, so it is, to save time(>_*)
	}

	if(type == 0)	//UTF8
	{
		while(*src)
		{
			ch = *src++;
			*dst++ = ch;

			if(ch < 0x80) {
				if(length > 1) length -= 1;
				else break;
			} else if (ch < 0xe0) { /* U-00000080 - U-000007FF, 2 bytes */
				if(length > 2) length -= 2;
				else break;
				*dst++ = *src++;
			} else if (ch < 0xf0) { /* U-00000800 - U-0000FFFF, 3 bytes */
				if(length > 3) length -= 3;
				else break;
				*dst++ = *src++;
				*dst++ = *src++;
			} else if (ch < 0xf5) { /* U-00010000 - U-001FFFFF, 4 bytes */
				if(length > 4) length -= 4;
				else break;
				*dst++ = *src++;
				*dst++ = *src++;
				*dst++ = *src++;
			} else {
				break;
			}
		}
		*dst = '\0';
	}
	else //assume it is GBK code
	{
		//GBK to UTF8
		while(*src)
		{
			ch = *src;
			if(ch < 0x80)
			{
				if(length > 1) length -= 1;
				else break;

				*dst++= ch;
				src ++;
			}
			else
			{
				ucode = charsets_gbk_to_ucs(src);

				if (ucode < 0x800) //2 bytes
				{
					if(length > 2) length -= 2;
					else break;
	
					*dst++ = 0xC0 | ((ucode >> 6) & 0x1F);
					*dst++ = 0x80 | (ucode & 0x3F);
				}
				else //3 bytes
				{
					if(length > 3) length -= 3;
					else break;

					*dst++ = 0xE0 | (ucode >> 12);
					*dst++ = 0x80 | ((ucode >>6) & 0x3F);
					*dst++ = 0x80 | (ucode & 0x3F);
				}

				src += 2;
			}
		}
		*dst = '\0';
	}

	return 0;
}

int load_cheatname(const char* filename, unsigned int string_num, unsigned int string_len, MSG_TABLE* mssg_table)
{
	FILE *fp;
	unsigned char current_line[256];
	unsigned char current_line_tmp[256];
	int len, m;
	unsigned char** indexp;
	unsigned char* msg;
	unsigned char* pt;

	mssg_table->msg_index = (unsigned char**)malloc(string_num*4);
	if(NULL == mssg_table->msg_index)
		return -1;

	string_len = string_len + string_len/2;
	mssg_table->msg_pool = (unsigned char*)malloc((string_len+31)&(~31));
	if(NULL == mssg_table->msg_pool) {
		free((void*)mssg_table->msg_index);
		return -1;
	}

	fp = fopen(filename, "r");
	if(fp == NULL) {
		free((void*)mssg_table->msg_index);
		free((void*)mssg_table->msg_pool);
		return -1;
	}

	len = 0;
	m= 0;
	indexp = mssg_table->msg_index;
	msg = mssg_table->msg_pool;
	while(fgets(current_line, 256, fp))
    {
		unsigned int str_len;

		if((pt = check_is_cht(current_line)) != NULL)
		{
			if(!strcasecmp(pt, "gameinfo"))
				continue;

			string2utf8(pt, current_line_tmp, 255);

			str_len = strlen(current_line_tmp);
			strncpy(msg+len, current_line_tmp, str_len);

			indexp[m++] = msg+len;
			len += str_len;
			msg[len] = '\0';
			len += 1;

			if(len >= string_len) break;
			if(m >= string_num) break;

			while(fgets(current_line, 256, fp))
			{
				str_len = strlen(current_line);
				if(str_len < 4) break;

				if((pt = strchr(current_line, '=')) == NULL)	//valid cheat item
					break;

				*pt = '\0';
				pt = current_line;

				string2utf8(pt, current_line_tmp, 255);

				str_len = strlen(current_line_tmp);
				strncpy(msg+len, current_line_tmp, str_len);
			
				indexp[m++] = msg+len;
				len += str_len;
				msg[len] = '\0';
				len += 1;

				if(len >= string_len) break;
				if(m >= string_num) break;
			}

			if(len >= string_len) break;
			if(m >= string_num) break;
		}
    }

	mssg_table -> msg_num = m;
    fclose(fp);

#if 0
cprintf("string_len %d; len %d\n", string_len, len);
for(m= 0; m<mssg_table -> msg_num; m++)
{
cprintf("msg%d:%s\n", m, indexp[m]);
}
#endif

	return 0;
}

#define MAX_CHEAT_DATE_LEN	(MAX_SFCCHEAT_NAME/2)	//other part hold the saved data

/*
*	Load cheat file
*/
int load_cheatfile(const char* filename, unsigned int *string_num, unsigned int *string_len, 
		GCHEAT_STRUCT *gcheat)
{
	FILE *cheats_file;
	unsigned char current_line[256];
	unsigned char current_line_tmp[256];
	unsigned int current_line_len;
	unsigned char *pt;
	int gcheat_num;

	unsigned int str_num;
	unsigned int str_len;
	unsigned int cheat_cell_num;
	int flag;

	cheats_file = fopen(filename, "r");
	if(NULL == cheats_file)
		return -1;
	g_cheat_cell_num = 0;
	g_cheat_num = 0;
	cheat_cell_num = 0;
	gcheat_num = 0;
	str_num = 0;
	str_len = 0;
	flag = 0;

	while(fgets(current_line, 256, cheats_file))
	{
		if((pt = check_is_cht(current_line)) == NULL)	//Check valid cht cheat
			continue;

		if(!strcasecmp(pt, "gameinfo"))				//maybe file end
			continue;

		gcheat[gcheat_num].name_id = str_num;
		gcheat[gcheat_num].item_id = cheat_cell_num;
		gcheat[gcheat_num].item_num = 0;

		string2utf8(pt, current_line_tmp, CHEAT_NAME_LENGTH);
		strcpy(gcheat[gcheat_num].name_shot, current_line_tmp);		//store a cut name shot
		//Initialize other parameter of gcheat
		gcheat[gcheat_num].active = 0;
		gcheat[gcheat_num].sub_active = 0;

		current_line_len = strlen(pt);
		str_len += current_line_len +1;
		str_num++;

		//Cheat items
		while(fgets(current_line, 256, cheats_file) != NULL)
		{
			if(strlen(current_line) < 4)
				break;

			if((pt = strchr(current_line, '=')) == NULL) //No valid content
				break;

			//one sub item each pass
			unsigned int first_part;			//first part of a cheat item
			unsigned int first_part_id;
			unsigned int sub_part_id;
			unsigned int hex_len;

			unsigned int cheat_addr;
			unsigned char cheat_dat[MAX_CHEAT_DATE_LEN];
			unsigned int cheat_dat_len;
			unsigned int str_num_saved;

			str_num_saved = str_num;
			str_len += pt - current_line +1;
			str_num++;

			first_part = 1;
			first_part_id = cheat_cell_num;
			sub_part_id = 0;

			//skip name part
			pt += 1;
			current_line_len = strlen(pt);

			//data part
			while(1)
			{
				//fill current_line buffer as full as possible
				if(current_line_len < (MAX_CHEAT_DATE_LEN*3+8))
				{	//the data length can fill a cheat cell
					if(NULL == strchr(pt, 0x0A)) {	//this line not end
						memmove(current_line, pt, current_line_len+1);
						fgets(current_line+current_line_len, 256-current_line_len, cheats_file);
						pt = current_line;
						current_line_len = strlen(pt);
					}
				}
#if 0
cprintf("------\n");
cprintf("new %d:[%s]\n", current_line_len, pt);
dump_mem(pt, strlen(pt));
cprintf("\n------\n");
#endif
				//get address
				if(first_part)
				{
					hex_len = sscanf_hex_value(pt, &cheat_addr);
					if(0 == hex_len) {
						goto load_cheatfile_error;
					}

					pt += hex_len;
					current_line_len -= hex_len +1;
					// strict to follow the formate
					if(',' != *pt++ || '\0' == *pt || 0x0D == *pt || 0x0A == *pt) {
						goto load_cheatfile_error;
					}

					if(cheat_addr < 0x10000)
						cheat_addr |= 0x7e0000;
					else {
						cheat_addr &= 0xffff;
						cheat_addr |= 0x7f0000;
					}
				}

				//get data
				unsigned int tmp, m;

				m = 0;
				cheat_dat_len = 0;
				while(m++ < MAX_CHEAT_DATE_LEN)
				{
					hex_len = sscanf_hex_value(pt, &tmp);
					if(0 == hex_len) break;

					cheat_dat[cheat_dat_len++] = (unsigned char)tmp;

					pt += hex_len;
					current_line_len -= hex_len +1;
					if(',' == *pt) pt++;
				}

				//In first part, get data error
				if(0 == cheat_dat_len) {
					if(0 == sub_part_id)
						goto load_cheatfile_error;
				}
				else {
					//record data
					flag = S9xAddCheat_ex(cheat_addr, cheat_dat, cheat_dat_len, cheat_cell_num++, sub_part_id++, str_num_saved);
					if(0 != flag) {
						cheat_cell_num -= sub_part_id;
						break;
					}
				}

				if(0 == *pt || 0x0D == *pt || 0x0A == *pt) break;	//a line over

				first_part = 0;
				if(';' == *pt) first_part = 1, pt += 1;	//other address of the cheat cell
				else cheat_addr += cheat_dat_len;	//more data
			} //data part

			//have no enough cheat_cell struct to store cheat
			if(0 != flag) break;

			S9xAddCheat_ov(first_part_id, sub_part_id);
			gcheat[gcheat_num].item_num += 1;
		} //Cheat items

		if(0 != flag) break;

		gcheat_num += 1;
		if(gcheat_num >= MAX_CHEATS)
			break;
    }

	g_cheat_cell_num = cheat_cell_num;
	g_cheat_num = gcheat_num;
	*string_num = str_num;
	*string_len = str_len;
	fclose(cheats_file);

#if 0
cprintf("g_cheat_num %d; g_cheat_cell_num %d\n", g_cheat_num, g_cheat_cell_num);

int i;
for(i= 0; i < g_cheat_cell_num; i++)
S9x_dumpcheat(i);

for(i= 0; i < g_cheat_num; i++)
{
	cprintf("cheat %d\n", i);
	cprintf("item num %d; item id %d\n", gcheat[i].item_num, gcheat[i].item_id);
}
#endif

	return 0;

load_cheatfile_error:
    fclose(cheats_file);
	return -1;
}

void gcheat_Managment(GCHEAT_STRUCT *gcheat)
{
	unsigned int i, enable, m, en_flag;
	unsigned int active, item_id, sub_active, item_num;

	//no cheat
	if(0 == g_cheat_num || 0 == g_cheat_cell_num) {
		S9xCheat_Disable();
		return;
	}

	enable = 0;
	for(i = 0; i < g_cheat_num; i++)
	{
		active = gcheat[i].active & 0x1;
		item_id = gcheat[i].item_id;
		item_num = gcheat[i].item_num;
		sub_active = gcheat[i].sub_active;

		for(m = 0; m < item_num; m++)
		{
			en_flag = sub_active == m ? active : 0;
			S9xCheat_switch(item_id, m, en_flag);
		}

		if(active) enable = 1;
	}

	if(enable)
		S9xCheat_Enable();
}

