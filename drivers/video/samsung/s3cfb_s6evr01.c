/* linux/drivers/video/samsung/s3cfb_s6evr01.c
 *
 * MIPI-DSI based s6evr01 lcd panel driver.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
*/
//---------------------------------------------------------------------------------------------------
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/mutex.h>
#include <linux/wait.h>
#include <linux/ctype.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/workqueue.h>
#include <linux/backlight.h>
#include <linux/lcd.h>
#include <plat/gpio-cfg.h>
#include <plat/regs-dsim.h>
#include <mach/dsim.h>
#include <mach/mipi_ddi.h>

//---------------------------------------------------------------------------------------------------
#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif

#include "s5p-dsim.h"
#include "s3cfb.h"
#include "s6evr01_param.h"

//---------------------------------------------------------------------------------------------------
// Debug mipi write (read write register & display)
//---------------------------------------------------------------------------------------------------
//#define DEBUG_MIPI_WR

//---------------------------------------------------------------------------------------------------
struct lcd_info {
	struct mutex			    lock;

	struct device			    *dev;
	struct lcd_device		    *ld;
	struct lcd_platform_data	*lcd_pd;
	bool 						status;
};

static struct mipi_ddi_platform_data *ddi_pd;

#ifdef CONFIG_HAS_EARLYSUSPEND
    static  struct lcd_info *g_lcd;
#endif

//---------------------------------------------------------------------------------------------------
#ifdef CONFIG_HAS_EARLYSUSPEND
    void s6evr01_early_suspend  (void);
    void s6evr01_late_resume    (void);
#endif

#if defined(DEBUG_MIPI_WR)
static  void    dsim_reg_dump   (void);
#endif

static  int     s6evr01_set_link(void *pd, unsigned int dsim_base,
	                            unsigned char (*cmd_write) (unsigned int dsim_base, unsigned int data0,
                                                            unsigned int data1, unsigned int data2),
                        	    int (*cmd_read) (u32 reg_base, u8 addr, u16 count, u8 *buf));
static  int     s6evr01_read            (struct lcd_info *lcd, const u8 addr, u16 count, u8 *buf);
static  int     s6evr01_write           (struct lcd_info *lcd, const unsigned char *seq, int len);
static  int     s6evr01_probe           (struct device *dev);

//---------------------------------------------------------------------------------------------------
void s6evr01_lcd_onoff  (unsigned char on) 
{
	struct  lcd_info *lcd = g_lcd;

    if(on) {
		if(!lcd->status) {
	    	msleep(17);
	    	s6evr01_write(lcd, SLEEP_OUT, sizeof(SLEEP_OUT));
	    	msleep(120);
			s6evr01_write(lcd, LTPSCTL_SET, sizeof(LTPSCTL_SET));
		    s6evr01_write(lcd, GAMMA_UPDATE, sizeof(GAMMA_UPDATE));     
		    mdelay(100);
	    	s6evr01_write(lcd, DISPLAY_ON , sizeof(DISPLAY_ON ));
	    	lcd->status = true;
	    }
    }
    else {
		if(lcd->status) {
	    	s6evr01_write(lcd, DISPLAY_OFF, sizeof(DISPLAY_OFF));
	    	msleep(17);
	    	s6evr01_write(lcd, SLEEP_IN, sizeof(SLEEP_IN));
	    	msleep(120);
	    	lcd->status = false;
	    }
    }
}
EXPORT_SYMBOL(s6evr01_lcd_onoff);

//---------------------------------------------------------------------------------------------------
void s6evr01_lcd_init   (void) 
{
	struct  lcd_info *lcd = g_lcd;

#if defined(DEBUG_MIPI_WR)
    printk("%s\n", __func__);    dsim_reg_dump();
#endif    

    mdelay(5);

    /* Test Key ON */
    s6evr01_write(lcd, TEST_KEY1_ON, sizeof(TEST_KEY1_ON));
    s6evr01_write(lcd, TEST_KEY2_ON, sizeof(TEST_KEY2_ON));
    s6evr01_write(lcd, TEST_KEY3_ON, sizeof(TEST_KEY3_ON));

    /* Brightness condition set */
    s6evr01_write(lcd, SLEEP_OUT, sizeof(SLEEP_OUT));
    mdelay(10);
	
	/* Refresh_con off */
	s6evr01_write(lcd, REFRESH_CON, sizeof(REFRESH_CON));

    /* Brightness condition set */
    s6evr01_write(lcd, GAMMA_2_2_SET, sizeof(GAMMA_2_2_SET));
    s6evr01_write(lcd, GAMMA_UPDATE, sizeof(GAMMA_UPDATE));     
    s6evr01_write(lcd, BRIGHTNESS_CONTROL, sizeof(BRIGHTNESS_CONTROL));     
	s6evr01_write(lcd, AOR_SET, sizeof(AOR_SET));

    /* ELVSS condition set */
    s6evr01_write(lcd, ELVSS_CONTROL, sizeof(ELVSS_CONTROL));     

    /* ETC condition set */
	s6evr01_write(lcd, ACL_SET, sizeof(ACL_SET));

    mdelay(120);

	s6evr01_write(lcd, LTPSCTL_SET, sizeof(LTPSCTL_SET));
    s6evr01_write(lcd, GAMMA_UPDATE, sizeof(GAMMA_UPDATE));     
    mdelay(100);

    /* Display On Command */
    s6evr01_write(lcd, DISPLAY_ON, sizeof(DISPLAY_ON));
    lcd->status = true;

}
EXPORT_SYMBOL(s6evr01_lcd_init);

//---------------------------------------------------------------------------------------------------
#if defined(DEBUG_MIPI_WR)
static  void    dsim_reg_dump   (void)
{
    unsigned int i;

	for (i = 0; i < 25; i++) {
		printk("[DSIM REG]0x11c8_00%02X = 0x%08X\n", (i*4), readl(ddi_pd->dsim_base + i*4));
	}
}
#endif

//---------------------------------------------------------------------------------------------------
static int s6evr01_set_link(void *pd, unsigned int dsim_base,
	                        unsigned char (*cmd_write) (unsigned int dsim_base, unsigned int data0,
                                                        unsigned int data1, unsigned int data2),
                        	int (*cmd_read) (u32 reg_base, u8 addr, u16 count, u8 *buf))
{
	struct mipi_ddi_platform_data *temp_pd = NULL;

	temp_pd = (struct mipi_ddi_platform_data *) pd;
	if (temp_pd == NULL) {
		printk(KERN_ERR "mipi_ddi_platform_data is null.\n");
		return -EPERM;
	}

	ddi_pd = temp_pd;

	ddi_pd->dsim_base = dsim_base;

	if (cmd_write)
		ddi_pd->cmd_write = cmd_write;
	else
		printk(KERN_WARNING "cmd_write function is null.\n");

	if (cmd_read)
		ddi_pd->cmd_read = cmd_read;
	else
		printk(KERN_WARNING "cmd_read function is null.\n");

	return 0;
}

//---------------------------------------------------------------------------------------------------
static int s6evr01_read(struct lcd_info *lcd, const u8 addr, u16 count, u8 *buf)
{
	int ret = 0;

	mutex_lock(&lcd->lock);

	if (ddi_pd->cmd_read)
		ret = ddi_pd->cmd_read(ddi_pd->dsim_base, addr, count, buf);

	mutex_unlock(&lcd->lock);

	return ret;
}

//---------------------------------------------------------------------------------------------------
static int s6evr01_write(struct lcd_info *lcd, const unsigned char *seq, int len)
{
	int size;
	const unsigned char *wbuf;

	mutex_lock(&lcd->lock);

	size = len;
	wbuf = seq;

	if (size == 1)
		ddi_pd->cmd_write(ddi_pd->dsim_base, DCS_WR_NO_PARA, wbuf[0], 0);
	else if (size == 2)
		ddi_pd->cmd_write(ddi_pd->dsim_base, DCS_WR_1_PARA, wbuf[0], wbuf[1]);
	else
		ddi_pd->cmd_write(ddi_pd->dsim_base, DCS_LONG_WR, (unsigned int)wbuf, size);

	mutex_unlock(&lcd->lock);

#if defined(DEBUG_MIPI_WR)
    {
        unsigned char   rbuf[50], i;
        
        s6evr01_read(lcd, wbuf[0], size -1, &rbuf[0]);
        
        if(size > 1)    {
            printk(" reg read 0x%02X :", wbuf[0]);

            for(i = 0; i < size -1; i++)    printk(" 0x%02X", rbuf[i]);
            
            printk("\n");
        }
    }
#endif    
    
	return 0;
}

//---------------------------------------------------------------------------------------------------
static int s6evr01_probe(struct device *dev)
{
	int     ret = 0;
	struct  lcd_info *lcd;

	if (!(lcd = kzalloc(sizeof(struct lcd_info), GFP_KERNEL))) {
		printk("%s : failed to allocate for lcd\n", __func__);		ret = -ENOMEM;
		goto err_alloc;
	}

#ifdef CONFIG_HAS_EARLYSUSPEND
    // for early_suspend/late_resume
	g_lcd = lcd;
#endif	

	lcd->dev = dev;

	dev_set_drvdata(dev, lcd);

	mutex_init(&lcd->lock);

	printk("s6evr01 lcd panel driver has been probed.\n");

	return 0;

err_alloc:
	return ret;
}

//---------------------------------------------------------------------------------------------------
//
// early_suspend/late_resume function call from s3cfb_main.c
//
//---------------------------------------------------------------------------------------------------
#ifdef CONFIG_HAS_EARLYSUSPEND

void s6evr01_early_suspend(void)
{
    printk("%s\n", __func__);
}

//---------------------------------------------------------------------------------------------------
void s6evr01_late_resume(void)
{
    printk("%s\n", __func__);
    s6evr01_lcd_init();
}

#endif
//---------------------------------------------------------------------------------------------------
static int __devexit s6evr01_remove(struct device *dev)
{
	return  0;
}

/* Power down all displays on reboot, poweroff or halt. */
//---------------------------------------------------------------------------------------------------
static void s6evr01_shutdown(struct device *dev)
{
}

//---------------------------------------------------------------------------------------------------
static struct mipi_lcd_driver s6evr01_mipi_driver = {
	.name           = "s6evr01_mipi_lcd",
	.set_link		= s6evr01_set_link,
	.probe			= s6evr01_probe,
	.remove			= __devexit_p(s6evr01_remove),
	.shutdown		= s6evr01_shutdown,
};

//---------------------------------------------------------------------------------------------------
static int s6evr01_init(void)
{
	return s5p_dsim_register_lcd_driver(&s6evr01_mipi_driver);
}

//---------------------------------------------------------------------------------------------------
static void s6evr01_exit(void)
{
	return;
}

//---------------------------------------------------------------------------------------------------
module_init(s6evr01_init);
module_exit(s6evr01_exit);

//---------------------------------------------------------------------------------------------------
MODULE_DESCRIPTION("MIPI-DSI s6evr01 (720x1280) Panel Driver");
MODULE_LICENSE("GPL");

//---------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------
