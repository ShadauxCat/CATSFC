/* cheatgrp.c
 *
 * Copyright (C) 2013 GBAtemp user Nebuleon.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
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

#include "cheatgrp.h"
#include "string.h"
#include "ds2_malloc.h"

extern struct SCheatData Cheat;

/*
 * Obtains the names of cheat groups currently defined in the Snes9x cheat
 * data.
 * Cheats are grouped by name, because multipart codes share the same name
 * when loaded.
 * This function only handles consecutive codes with the same name. If two
 * runs of codes have the same name, two identically-named groups will be
 * written. Enabling or disabling either of these groups will also enable
 * or disable the other if using NDSSFCEnableCheatGroup or
 * NDSSFCDisableCheatGroup.
 * OUT: NamePointers, an array of MAX_CHEATS_T + 1 pointers which are updated
 *      by this function. All pointers beyond the last group name are updated
 *      to point to NULL.
 *      NameMemory, an array of MAX_CHEATS_T * MAX_SFCCHEAT_NAME char values
 *      which is updated to hold the names of the groups.
 */
void NDSSFCGetCheatGroups (char** NamePointers, char* NameMemory)
{
	unsigned int NameIndex = 0, cheat;
	char* dst = NameMemory;
	for (cheat = 0; cheat < Cheat.num_cheats; cheat++)
	{
		if (cheat == 0 || strcmp(Cheat.c [cheat].name, Cheat.c [cheat - 1].name) != 0)
		{
			memcpy(dst, Cheat.c [cheat].name, MAX_SFCCHEAT_NAME * sizeof (char));
			NamePointers[NameIndex] = dst;
			dst += MAX_SFCCHEAT_NAME;
			NameIndex++;
		}
	}
	for (; NameIndex < MAX_CHEATS_T + 1; NameIndex++)
	{
		NamePointers[NameIndex] = NULL;
	}
}

void NDSSFCEnableCheatGroup (char* GroupName)
{
	uint32 cheat;
	for (cheat = 0; cheat < Cheat.num_cheats; cheat++)
	{
		if (strcmp(Cheat.c [cheat].name, GroupName) == 0)
		{
			S9xEnableCheat (cheat);
		}
	}
}

void NDSSFCDisableCheatGroup (char* GroupName)
{
	uint32 cheat;
	for (cheat = 0; cheat < Cheat.num_cheats; cheat++)
	{
		if (strcmp(Cheat.c [cheat].name, GroupName) == 0)
		{
			S9xDisableCheat (cheat);
		}
	}
}

bool8 NDSSFCIsCheatGroupEnabled (char* GroupName)
{
	bool8 NameFound = FALSE;
	uint32 cheat;
	for (cheat = 0; cheat < Cheat.num_cheats; cheat++)
	{
		if (strcmp(Cheat.c [cheat].name, GroupName) == 0)
		{
			if (!Cheat.c [cheat].enabled)
				return FALSE;
			NameFound = TRUE;
		}
	}
	return NameFound;
}
