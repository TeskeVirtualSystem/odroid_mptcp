/* linux/drivers/video/samsung/s3cfb_wa101s.c
 *
 * Copyright (c) 2010 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com/
 *
 * 101WA01S 10.1" Landscape LCD module driver for the SMDK
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include "s3cfb.h"

static struct s3cfb_lcd ej70na01c = {
	.width	= 1024,
	.height	= 600,
	.bpp	= 24,
	.freq	= 60,
	.timing = {
		.h_fp	= 35,
		.h_sw	= 30,
		.h_bp	= 50,			
		
		.v_fp	= 5,
		.v_bp	= 10,
		.v_sw	= 5,		
		.v_fpe	= 1,
		.v_bpe	= 1,
	},

	.polarity = {
		.rise_vclk	= 1,
		.inv_hsync	= 1,
		.inv_vsync	= 1,
		.inv_vden	= 0,
	},
};

/* name should be fixed as 's3cfb_set_lcd_info' */
void s3cfb_set_lcd_info(struct s3cfb_global *ctrl)
{
	ej70na01c.init_ldi = NULL;
	ctrl->lcd = &ej70na01c;
	printk("registerd EJ70NA01C LCD Driver.\n");
}
