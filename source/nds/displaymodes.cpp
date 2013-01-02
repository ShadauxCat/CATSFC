/* displaymodes.cpp
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
#include <stdio.h>

#include "ds2_types.h"
#include "ds2io.h"

#include "gfx.h"

static uint16 SevenToSixScanlineResize (uint16 TopColour, uint16 BottomColour, uint8 TopFraction, uint8 BottomFraction)
{
	// Speed hacks!
	if (TopColour == BottomColour) // Two colours identical (no calc)
		return TopColour;
	if (TopColour == 0x0000) // From black to a non-identical colour
		return
			// Red component
			( ( ((BottomColour >> 10) & 0x1F) * BottomFraction / 7 ) << 10 )
			// Green component
		|	( ( ((BottomColour >>  5) & 0x1F) * BottomFraction / 7 ) << 5 )
			// Blue component
		|	( (  (BottomColour        & 0x1F) * BottomFraction / 7 )      )
		;
	if (BottomColour == 0x0000) // To black from a non-identical colour
		return
			// Red component
			( ( ((TopColour >> 10) & 0x1F) * TopFraction / 7 ) << 10 )
			// Green component
		|	( ( ((TopColour >>  5) & 0x1F) * TopFraction / 7 ) << 5 )
			// Blue component
		|	( (  (TopColour        & 0x1F) * TopFraction / 7 )      )
		;
	return
		// Red component
		( ( ((TopColour >> 10) & 0x1F) * TopFraction / 7 + ((BottomColour >> 10) & 0x1F) * BottomFraction / 7 ) << 10 )
		// Green component
	|	( ( ((TopColour >>  5) & 0x1F) * TopFraction / 7 + ((BottomColour >>  5) & 0x1F) * BottomFraction / 7 ) << 5 )
		// Blue component
	|	( (  (TopColour        & 0x1F) * TopFraction / 7 +  (BottomColour        & 0x1F) * BottomFraction / 7 )      )
	;
}

void NDSSFCDrawFrameAntialiased ()
{
	uint16 X, Y;
	uint16 *SrcTop = (uint16 *) GFX.Screen, *SrcBottom = SrcTop + 256, *Dest = (uint16 *) up_screen_addr;

	for (Y = 0; Y < 224; Y += 7)
	{
		for (X = 0; X < 256; X++)
			*Dest++ = SevenToSixScanlineResize (*SrcTop++, *SrcBottom++, 6, 1);
			// At the end of this loop, line 1 for this block of 6
			// has been drawn from the 2 first lines in the block
			// of 7. Do the rest.
		for (X = 0; X < 256; X++)
			*Dest++ = SevenToSixScanlineResize (*SrcTop++, *SrcBottom++, 5, 2);
		for (X = 0; X < 256; X++)
			*Dest++ = SevenToSixScanlineResize (*SrcTop++, *SrcBottom++, 4, 3);
		for (X = 0; X < 256; X++)
			*Dest++ = SevenToSixScanlineResize (*SrcTop++, *SrcBottom++, 3, 4);
		for (X = 0; X < 256; X++)
			*Dest++ = SevenToSixScanlineResize (*SrcTop++, *SrcBottom++, 2, 5);
		for (X = 0; X < 256; X++)
			*Dest++ = SevenToSixScanlineResize (*SrcTop++, *SrcBottom++, 1, 6);
		// At the end of these loops, SrcTop and SrcBottom point to
		// the first element in the 7th line of the current block and
		// the first element in the 1st line of the next.
		// We need to increase the pointer to start on lines 1 & 2.
		SrcTop += 256;
		SrcBottom += 256;
	}
}
