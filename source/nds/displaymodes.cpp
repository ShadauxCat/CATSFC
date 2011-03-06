//entry.c
#include <stdio.h>

#include "ds2_types.h"
#include "ds2_cpu.h"
#include "ds2_timer.h"
#include "ds2io.h"
#include "fs_api.h"


#include "gfx.h"


u32 y_scale_ = (224<<8) / 192;


static inline void Put_Pixel (unsigned char* screen, int y, int y_scale)
{

	memcpy(&screen[((y<<1) << 8)], &GFX.Screen [(((y*y_scale)>>8)<<1) <<8], 256*2);
}


bool Draw_Frame_Flip(bool flip)
{

	int y = 0;
		
	do
	{
			int tempy = y << 4;
			

			Put_Pixel ((unsigned char*)up_screen_addr, tempy, y_scale_);tempy++;
			Put_Pixel ((unsigned char*)up_screen_addr, tempy, y_scale_);tempy++;
			Put_Pixel ((unsigned char*)up_screen_addr, tempy, y_scale_+1);tempy++;
			Put_Pixel ((unsigned char*)up_screen_addr, tempy, y_scale_);tempy++;			
			Put_Pixel ((unsigned char*)up_screen_addr, tempy, y_scale_);tempy++;
			Put_Pixel ((unsigned char*)up_screen_addr, tempy, y_scale_);tempy++;				
			Put_Pixel ((unsigned char*)up_screen_addr, tempy, y_scale_);tempy++;
			Put_Pixel ((unsigned char*)up_screen_addr, tempy, y_scale_);tempy++;
			Put_Pixel ((unsigned char*)up_screen_addr, tempy, y_scale_);tempy++;
			Put_Pixel ((unsigned char*)up_screen_addr, tempy, y_scale_);tempy++;
			Put_Pixel ((unsigned char*)up_screen_addr, tempy, y_scale_);tempy++;
			Put_Pixel ((unsigned char*)up_screen_addr, tempy, y_scale_);tempy++;
			Put_Pixel ((unsigned char*)up_screen_addr, tempy, y_scale_);tempy++;
			Put_Pixel ((unsigned char*)up_screen_addr, tempy, y_scale_);tempy++;
			Put_Pixel ((unsigned char*)up_screen_addr, tempy, y_scale_);tempy++;
			Put_Pixel ((unsigned char*)up_screen_addr, tempy, y_scale_);
	}
	while(++y < 12);		
	return 1;
}
