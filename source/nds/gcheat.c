/* gcheat.c
 *
 * Copyright (C) 2012 GBAtemp user Nebuleon.
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

#include "port.h"
#include "string.h"
#include "fs_api.h"
#include "ds2_malloc.h"
#include "gcheat.h"
#include "charsets.h"
#include "cheats.h"

extern struct SCheatData Cheat;

// Reads a cheat text file in BSNES's format.
int NDSSFCLoadCheatFile(const char* filename)
{
	FILE* fp = fopen(filename, "r");
	if (fp == NULL)
		return -1;

	S9xDeleteCheats();

	// The construction is "a","b","c" <newline>.
	// a is ignored. In BSNES, it decides whether the code is enabled.
	// b is a series of codes separated by +. Each of the codes is in the form
	//   accepted by the Game Genie, Pro Action Replay, or the GoldFinger.
	// c is the cheat's description.
	char line[256], code[24];
	char *description, *codes_ptr;
	uint32 address;
	uint8 byte;
	uint8 bytes [3];
	bool8 sram;
	uint8 num_bytes;

	while (fgets(line, sizeof(line), fp))
	{
		char* ptr = &line[0];
		// Ignore a.
		while (*ptr && *ptr != ',')
			ptr++;
		// If there was no comma, declare a bad file.
		if (*ptr == '\0') {
			fclose(fp);
			return -2;
		}
		*ptr++; // Past the comma

		if (*ptr && *ptr == '"')
			ptr++; // Starting quote of b.
		codes_ptr = ptr; // Save this for later.
		while (*ptr && *ptr != ',')
			ptr++;
		// If there was no comma, declare a bad file.
		if (*ptr == '\0') {
			fclose(fp);
			return -2;
		}
		*ptr++; // Past the comma
		*(ptr - 1) = '\0'; // End the codes there

		uint32 i = 0;
		description = ptr; // Skip starting " in description
		while (*description && *description == '"')
			description++;
		ptr = description;
		while (*ptr && !(*ptr == '\r' || *ptr == '\n' || *ptr == '"') && i < MAX_SFCCHEAT_NAME - 1) {
			ptr++; // Remove trailing newline/quote in description
			i++; // Clip the cheat name to MAX_SFCCHEAT_NAME chars
		}
		*ptr = '\0';

		uint32 n = 0, c;
		// n is the number of cheat codes. Beware of MAX_CHEATS_T.

		// List of cheat codes having the same description.
		ptr = codes_ptr;
		while (*ptr && !(*ptr == ',' || *ptr == '"')) {
			if (n >= MAX_CHEATS_T) {
				fclose(fp);
				return 0;
			}
			i = 0;
			while (*ptr && *ptr != '+' && i < sizeof(code) - 1)
				code[i++] = *ptr++;
			code[i] = '\0';
			if (!S9xGameGenieToRaw (code, &address, &byte)) {
				S9xAddCheat (FALSE, TRUE, address, byte);
				strncpy (Cheat.c[Cheat.num_cheats - 1].name, description, MAX_SFCCHEAT_NAME);
			}
			else if (!S9xProActionReplayToRaw (code, &address, &byte)) {
				S9xAddCheat (FALSE, TRUE, address, byte);
				strncpy (Cheat.c[Cheat.num_cheats - 1].name, description, MAX_SFCCHEAT_NAME);
			}
			else if (!S9xGoldFingerToRaw (code, &address, &sram, &num_bytes, bytes))
			{
				for (c = 0; c < num_bytes; c++) {
					S9xAddCheat (FALSE, TRUE, address + c, bytes[c]);
					strncpy (Cheat.c[Cheat.num_cheats - 1].name, description, 22);
				}
			}
			else {
				fclose(fp);
				return -3; // Bad cheat format
			}
		}
	}

	fclose(fp);
	return 0;
}
