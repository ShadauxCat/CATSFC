/* cheats3.cpp
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
#include <ctype.h>
#include <string.h>
#include "snes9x.h"
#include "cheats.h"
#include "memmap.h"
#include "gcheat.h"

extern SCheatData Cheat;

int S9xAddCheat_ex (unsigned int address, unsigned char* cheat_dat, unsigned int cheat_dat_len, 
		unsigned int cheat_cell_num, unsigned int part_id, unsigned int str_num)
{
	if(cheat_cell_num < MAX_CHEATS_T)
	{
		Cheat.c[cheat_cell_num].address = address;
		Cheat.c[cheat_cell_num].enabled = FALSE;

		if(cheat_dat_len > 1)
			memcpy(Cheat.c[cheat_cell_num].name, cheat_dat, cheat_dat_len);
		else
			Cheat.c[cheat_cell_num].byte = cheat_dat[0];

		Cheat.c[cheat_cell_num].total_part = 0;				//default are sub-part
		Cheat.c[cheat_cell_num].part_id = part_id;
		Cheat.c[cheat_cell_num].part_len = cheat_dat_len;
		Cheat.c[cheat_cell_num].cheat_type = 0;				//default are sub-part
		Cheat.c[cheat_cell_num].name_id = str_num;

		return 0;
    }

	return -1;
}

void S9xAddCheat_ov(unsigned int cheat_cell_num, unsigned int total_part)
{
	if(cheat_cell_num < MAX_CHEATS_T)
	{
		Cheat.c[cheat_cell_num].total_part = total_part;	//default are sub-part
		Cheat.c[cheat_cell_num].cheat_type = 0x80;
    }
}

static unsigned int S9xGetSub_id(unsigned int start, unsigned int sub_part)
{
	unsigned int i, m, n;

	if(0 == sub_part)
		return start;

	if((start+1) >= g_cheat_cell_num)
		return start;

	m = 0;
	for(i= start; i < g_cheat_cell_num; )
	{
		n = Cheat.c[i].total_part;
		i += n;
		m += 1;
		if(m == sub_part) break;
	}

	return i;
}

unsigned int S9xGetCheat_nameid(unsigned int start, unsigned int part)
{
#if 0
	unsigned int m, n, i;
	unsigned int ret;
	unsigned int cell_num;

	cell_num = g_cheat_cell_num;

	ret = Cheat.c[start].name_id;
	if((start+1) >= cell_num)
		return ret;

	m = 0;
	for(i = start; i < cell_num; ) {
		if(m == part) break;
		n = Cheat.c[i].total_part;
		i += n;
		m += 1;
	}

	if(i < cell_num)
		ret = Cheat.c[i].name_id;

	return ret;
#else
	unsigned int i;

	i = S9xGetSub_id(start, part);
	return Cheat.c[i].name_id;
#endif
}

void S9xCheat_switch(unsigned int start, unsigned int sub_part, unsigned int enable)
{
	unsigned int i, m, n;

	if((start+1) >= g_cheat_cell_num)
		return;

	i = S9xGetSub_id(start, sub_part);
	m = Cheat.c[i].total_part;
	for(n = 0; n < m; n++)
		Cheat.c[i+n].enabled = enable;
}

static inline void S9xApplyCheat_ex(unsigned int start, unsigned int num)
{
	unsigned int i, m;
	unsigned int address, len;

	for(i = 0; i < num; i++)
	{
		address = Cheat.c[start+i].address;
		len = Cheat.c[start+i].part_len;

	    int block = (address >> MEMMAP_SHIFT) & MEMMAP_MASK;
		unsigned char *ptr = Memory.Map [block];

		if(1 == len)
		{
		    if (ptr >= (uint8 *) CMemory::MAP_LAST)
				*(ptr + (address & 0xffff)) = Cheat.c[start+i].byte;
		    else
				S9xSetByte (Cheat.c[start+i].byte, address);
		}
		else
		{
			for(m= 0; m < len; m++)
			{
			    if (ptr >= (uint8 *) CMemory::MAP_LAST)
					*(ptr + (address & 0xffff)) = Cheat.c[start+i].name[m];
			    else
					S9xSetByte (Cheat.c[start+i].name[m], address);
			}
		}
	}
}

void S9xApplyCheats_ex(void)
{
	unsigned int i, m, n;

	if (Settings.ApplyCheats)
	{
		for(i= 0; i < g_cheat_cell_num; i++)
		{
			m = Cheat.c[i].total_part;
			if(Cheat.c[i].enabled)
				S9xApplyCheat_ex(i, m);
			i += m;
		}
	}
}

#if 1
extern "C" void dump_mem(unsigned char* addr, unsigned int len);

void S9x_dumpcheat(unsigned int id)
{
	cprintf("\nid %d------------\n", id);
	cprintf("total %d; part %d\n", Cheat.c[id].total_part, Cheat.c[id].part_id);
	cprintf("address: %08x; data: %d\n", Cheat.c[id].address, Cheat.c[id].part_len);
	if(Cheat.c[id].part_len == 1)
		cprintf("data: %02x\n", Cheat.c[id].byte);
	else
		dump_mem((unsigned char*)Cheat.c[id].name, Cheat.c[id].part_len);
	cprintf("           ------\n");
}
#endif

void S9xCheat_Disable(void)
{
	Settings.ApplyCheats = FALSE;
}

void S9xCheat_Enable(void)
{
	Settings.ApplyCheats = TRUE;
}

