/* linux/drivers/video/samsung/s3cfb_lp101wh1.c
 *
 * LG Display LP101WH1 10.1" WSVGA Display Panel Support
 *
 * Hakjoo Kim, Copyright (c) 2010 Hardkernel Inc.
 * 	ruppi.kim@hardkernel.com/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include "s3cfb.h"

static struct s3cfb_lcd u133wa01 = {
#if defined(CONFIG_TOUCHSCREEN_NEXIO_USB)   // 42" HDMI demo
	.width = 1920,
	.height = 1080,
#else
	.width = 1360,
	.height = 768,
#endif	
	.bpp = 24,
	.freq = 60,

	.timing = {
#if defined(CONFIG_TOUCHSCREEN_NEXIO_USB)   // 42" HDMI demo
		.h_fp = 1,
		.h_bp = 1,
		.h_sw = 1,
		.v_fp = 1,
		.v_fpe = 1,
		.v_bp = 1,
		.v_bpe = 1,
		.v_sw = 1,
#else
		.h_fp = 48,
		.h_bp = 80,
		.h_sw = 32,
		.v_fp = 3,
		.v_fpe = 2,
		.v_bp = 14,
		.v_bpe = 2,
		.v_sw = 5,
#endif		
	},

	.polarity = {
		.rise_vclk = 0,
		.inv_hsync = 1,
		.inv_vsync = 1,
		.inv_vden = 0,
	},
};

static struct s3cfb_lcd fake_fb = {
    .width = 1280,
    .height = 720,
    .bpp = 24,
    .freq = 60,
    
    .timing = {
        .h_fp = 1,
        .h_bp = 1,
        .h_sw = 1,
        .v_fp = 1,
        .v_fpe = 1,
        .v_bp = 1,
        .v_bpe = 1,
        .v_sw = 1,
    },
    
    .polarity = {
        .rise_vclk = 0,
        .inv_hsync = 1,
        .inv_vsync = 1,
        .inv_vden = 0,
    },
};

static int atoi(const char *str)
{
    int val = 0;

    for(;;str++)   {
        switch(*str)   {
            case    '0'...'9':  val = 10 * val + (*str-'0');    break;
            default :                                           return  val;
        }
    }
    return  val;
}

static  unsigned char   FbBootArgsX[5], FbBootArgsY[5], FakeFbBootArgs[6];
static  unsigned char   SetEnableX = false, SetEnableY = false, FakeFB = false;

unsigned short FrameBufferSizeX = 0;
EXPORT_SYMBOL(FrameBufferSizeX);
unsigned short FrameBufferSizeY = 0;
EXPORT_SYMBOL(FrameBufferSizeY);

static int __init lcd_x_res(char *line)
{
    sprintf(FbBootArgsX, "%s", line);    SetEnableX = true;
    return  0;
}
__setup("fb_x_res=", lcd_x_res);

static int __init lcd_y_res(char *line)
{
    sprintf(FbBootArgsY, "%s", line);    SetEnableY = true;
    return  0;
}
__setup("fb_y_res=", lcd_y_res);

static int __init fake_fb_enable(char *line)
{
    sprintf(FakeFbBootArgs, "%s", line);    

    return  0;
}
__setup("fake_fb=", fake_fb_enable);

/* name should be fixed as 's3cfb_set_lcd_info' */
void s3cfb_set_lcd_info(struct s3cfb_global *ctrl)
{
    if(!strncmp(FakeFbBootArgs, "true", 4))	FakeFB = true;
	if(FakeFB)	{
		fake_fb.init_ldi = NULL;
                if(SetEnableX)  fake_fb.width    = atoi(FbBootArgsX);
                if(SetEnableY)  fake_fb.height   = atoi(FbBootArgsY);
		ctrl->lcd = &fake_fb;
		printk("Registerd Fake FB Driver.\n");
		printk("Fake FB res X = %d\n", fake_fb.width);
		printk("Fake FB res Y = %d\n", fake_fb.height);
		FrameBufferSizeX = fake_fb.width;
		FrameBufferSizeY = fake_fb.height;
	}
	else	{
		u133wa01.init_ldi = NULL;
		ctrl->lcd = &u133wa01;
		printk("Registerd u133wa01 LCD Driver.\n");
		printk("u133wa01 res X = %d\n", u133wa01.width);
		printk("u133wa01 res Y = %d\n", u133wa01.height);
		FrameBufferSizeX = u133wa01.width;
		FrameBufferSizeY = u133wa01.height;
	}
}

