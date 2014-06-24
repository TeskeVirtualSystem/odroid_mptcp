//[*]--------------------------------------------------------------------------------------------------[*]
//
//
// 
//  ODROID Board : ODROID sysfs driver (charles.park)
//  2012.01.17
// 
//
//[*]--------------------------------------------------------------------------------------------------[*]
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/sysfs.h>
#include <linux/input.h>
#include <linux/gpio.h>
#include <linux/io.h>

#include <mach/gpio.h>
#include <mach/regs-gpio.h>
#include <mach/regs-pmu.h>
#include <plat/gpio-cfg.h>

//[*]--------------------------------------------------------------------------------------------------[*]
#define	DEBUG_PM_MSG

//[*]--------------------------------------------------------------------------------------------------[*]
// Sleep disable flage
//[*]--------------------------------------------------------------------------------------------------[*]
#define	SLEEP_DISABLE_FLAG

#if defined(SLEEP_DISABLE_FLAG)
	#ifdef CONFIG_HAS_WAKELOCK
		#include <linux/wakelock.h>
		static struct wake_lock 	sleep_wake_lock;
	#endif
#endif

//[*]--------------------------------------------------------------------------------------------------[*]
//   Control GPIO Define
//[*]--------------------------------------------------------------------------------------------------[*]
enum	{
	// WIFI Control Port
	WIFI_ENABLE,
	WIFI_HOST_WAKE,
	WIFI_NRST,
	
	// Bluetooth Control Port
	BT_ENABLE,
	BT_WAKE,
	BT_HOST_WAKE,
	BT_NRST,

	// Audio Enable port
	AUDIO_EN,

	// Power Control
	SYSTEM_POWER_2V8,		// BUCK6 Enable Control
	SYSTEM_POWER_3V3,		// BUCK6 Enable Control
	SYSTEM_POWER_5V0,		// USB HOST Power
	SYSTEM_POWER_12V0,		// VLED Control (Backlight)
	SYSTEM_OUTPUT_485,		// 5.-V Enable

	// 3G Modem Control Port
	MODEM_POWER,
	MODEM_RESET,
	MODEM_DISABLE1,
	MODEM_DISABLE2,
	
	// Status LED Display
	STATUS_LED_RED,
	STATUS_LED_GREEN,
	STATUS_LED_BLUE,
	
	GPIO_INDEX_END
};

//[*]--------------------------------------------------------------------------------------------------[*]
//
// GPIOs Defined Header file
//
//[*]--------------------------------------------------------------------------------------------------[*]
#if defined(CONFIG_BOARD_ODROID_Q) || defined(CONFIG_BOARD_ODROID_Q2)// Odroid-Q
	#include	"odroidq-sysfs.h"
#elif defined(CONFIG_BOARD_ODROID_X)||defined(CONFIG_BOARD_ODROID_X2) // Odroid-X Series
	#include	"odroidx-sysfs.h"
#elif defined(CONFIG_BOARD_ODROID_U) // Odroid-U Series
	#include	"odroidu-sysfs.h"
#elif defined(CONFIG_BOARD_SMD_A2) // smda2
	#include	"smda2-sysfs.h"
#endif
//[*]--------------------------------------------------------------------------------------------------[*]
//
// GPIO IRQ Register struct
//
//[*]--------------------------------------------------------------------------------------------------[*]
static struct {
	int 	gpio;			// GPIO Number
	char	*name;			// GPIO Name == sysfs attr name (must)
	int		pud;			// Pull up/down register setting : S3C_GPIO_PULL_DOWN, UP, NONE
} sIrqGpios[] = {
	{	0,	NULL,	S3C_GPIO_PULL_NONE	},	// END
};

//[*]--------------------------------------------------------------------------------------------------[*]
//
// GPIO Default Register struct
//
//[*]--------------------------------------------------------------------------------------------------[*]
static struct {
	int 	gpio;			// GPIO Number
	char	*name;			// GPIO Name == sysfs attr name (must)
	bool 	output;			// 1 = Output, 0 = Input
	int 	value;			// Default Value(only for output)
	int		pud;			// Pull up/down register setting : S3C_GPIO_PULL_DOWN, UP, NONE
} sDefaultGpios[] = {
#if defined(CONFIG_BOARD_ODROID_X)||defined(CONFIG_BOARD_ODROID_X2)||defined(CONFIG_BOARD_ODROID_U)
    // EXT_INT3 : HDMI Resolution Select
	{	EXYNOS4_GPX0(3), "EXYNOS4_GPX0(3)",	0,	0,	S3C_GPIO_PULL_DOWN	},
	// EXT_INT18 : User Key (HDMI Resolution Select)
	{	EXYNOS4_GPX2(2), "EXYNOS4_GPX2(2)",	0,	0,	S3C_GPIO_PULL_DOWN	},
    // EXT_INT27 : No Interrupt
	{	EXYNOS4_GPX3(3), "EXYNOS4_GPX3(3)",	0,	0,	S3C_GPIO_PULL_NONE	},
	// Power On LED(Kernel on status indicate)
	{	EXYNOS4_GPC1(2), "EXYNOS4_GPC1(2)",	1,	0,	S3C_GPIO_PULL_NONE	},
	{	EXYNOS4_GPC1(0), "EXYNOS4_GPC1(0)",	1,	1,	S3C_GPIO_PULL_NONE	},
#endif	
#if defined(CONFIG_BOARD_ODROID_Q) || defined(CONFIG_BOARD_ODROID_Q2)
    // EXT_INT3 : HDMI Resolution Select
	{	EXYNOS4_GPX0(3), "EXYNOS4_GPA1(0)",	1,	1,	S3C_GPIO_PULL_NONE	},
    // EXT_INT27 : No Interrupt
	{	EXYNOS4_GPX3(3), "EXYNOS4_GPA1(1)",	1,	0,	S3C_GPIO_PULL_NONE	},
#endif
	{	0, NULL,	0,	0,	S3C_GPIO_PULL_NONE	},
};

//[*]--------------------------------------------------------------------------------------------------[*]
int	odroid_get_wifi_irqnum	(void)
{
	return	0;
}

EXPORT_SYMBOL(odroid_get_wifi_irqnum);

//[*]--------------------------------------------------------------------------------------------------[*]
#if defined(CONFIG_BOARD_ODROID_X)||defined(CONFIG_BOARD_ODROID_X2)||defined(CONFIG_BOARD_ODROID_U)
static  unsigned char   HdmiBootArgs[5];
static  unsigned char   VOutArgs[5];
static  unsigned char   AInArgs[5];

// Bootargs parsing
static int __init hdmi_resolution_setup(char *line)
{
    sprintf(HdmiBootArgs, "%s", line);
    return  0;
}
__setup("hdmi_phy_res=", hdmi_resolution_setup);

static int __init vout_mode_setup(char *line)
{
    sprintf(VOutArgs, "%s", line);
    return  0;
}
__setup("v_out=", vout_mode_setup);

static int __init audio_in_setup(char *line)
{
    sprintf(AInArgs, "%s", line);
    return  0;
}
__setup("audio_in=", audio_in_setup);

int odroid_get_hdmi_resolution  (void)
{
    // Bootarg setup 1080p
    if(!strncmp("1080", HdmiBootArgs, 4))   return   3;
    else if(!strncmp("720", HdmiBootArgs, 4))   return   2;
    else if(!strncmp("480", HdmiBootArgs, 4))   return   1;
        
    return  (gpio_get_value(EXYNOS4_GPX2(2)) || gpio_get_value(EXYNOS4_GPX0(3)));
}
EXPORT_SYMBOL(odroid_get_hdmi_resolution);

int odroid_get_vout_mode  (void)
{
    // Bootarg setup dvi
    if(!strncmp("dvi", VOutArgs, 3))   	return   1;
	else 								return   0;
}
EXPORT_SYMBOL(odroid_get_vout_mode);

int odroid_get_audio_in(void)
{
    // Bootarg setup dvi
    if(!strncmp("dmic", AInArgs, 4))   	return   1;
	else 								return   0;
}
EXPORT_SYMBOL(odroid_get_audio_in);


#endif
//[*]--------------------------------------------------------------------------------------------------[*]
//
//   sysfs function prototype define
//
//[*]--------------------------------------------------------------------------------------------------[*]
static	ssize_t show_gpio	(struct device *dev, struct device_attribute *attr, char *buf);
static 	ssize_t set_gpio	(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
static	ssize_t show_hdmi	(struct device *dev, struct device_attribute *attr, char *buf);

#if defined(CONFIG_BOARD_ODROID_X)||defined(CONFIG_BOARD_ODROID_X2)||defined(CONFIG_BOARD_ODROID_U)||defined(CONFIG_BOARD_ODROID_Q) || defined(CONFIG_BOARD_ODROID_Q2)
#if defined(CONFIG_BOARD_ODROID_X)||defined(CONFIG_BOARD_ODROID_X2)||defined(CONFIG_BOARD_ODROID_U)
static	ssize_t show_resolution         (struct device *dev, struct device_attribute *attr, char *buf);
static	ssize_t show_vout_mode			(struct device *dev, struct device_attribute *attr, char *buf);
static 	ssize_t set_led_blink           (struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
#endif
static 	ssize_t set_poweroff_trigger    (struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
static	ssize_t show_boot_mode          (struct device *dev, struct device_attribute *attr, char *buf);
#endif
static	ssize_t show_inform	            (struct device *dev, struct device_attribute *attr, char *buf);
static 	ssize_t set_inform	            (struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
//[*]--------------------------------------------------------------------------------------------------[*]
//[*]--------------------------------------------------------------------------------------------------[*]
static	DEVICE_ATTR(wifi_enable, 		S_IRWXUGO, show_gpio, set_gpio);
static	DEVICE_ATTR(wifi_host_wake, 	S_IRWXUGO, show_gpio, set_gpio);
static	DEVICE_ATTR(wifi_nrst,	 		S_IRWXUGO, show_gpio, set_gpio);
//[*]--------------------------------------------------------------------------------------------------[*]
static	DEVICE_ATTR(bt_enable, 			S_IRWXUGO, show_gpio, set_gpio);
static	DEVICE_ATTR(bt_wake, 			S_IRWXUGO, show_gpio, set_gpio);
static	DEVICE_ATTR(bt_host_wake, 		S_IRWXUGO, show_gpio, set_gpio);
static	DEVICE_ATTR(bt_nrst,			S_IRWXUGO, show_gpio, set_gpio);
//[*]--------------------------------------------------------------------------------------------------[*]
//[*]--------------------------------------------------------------------------------------------------[*]
static	DEVICE_ATTR(audio_en,			S_IRWXUGO, show_gpio, set_gpio);
//[*]--------------------------------------------------------------------------------------------------[*]
//[*]--------------------------------------------------------------------------------------------------[*]
static	DEVICE_ATTR(power_2v8,			S_IRWXUGO, show_gpio, set_gpio);
static	DEVICE_ATTR(power_3v3,			S_IRWXUGO, show_gpio, set_gpio);
static	DEVICE_ATTR(power_5v0,			S_IRWXUGO, show_gpio, set_gpio);
static	DEVICE_ATTR(power_12v0,			S_IRWXUGO, show_gpio, set_gpio);
static	DEVICE_ATTR(output_485,			S_IRWXUGO, show_gpio, set_gpio);
//[*]--------------------------------------------------------------------------------------------------[*]
//[*]--------------------------------------------------------------------------------------------------[*]
static	DEVICE_ATTR(modem_power, 		S_IRWXUGO, show_gpio, set_gpio);
static	DEVICE_ATTR(modem_reset,		S_IRWXUGO, show_gpio, set_gpio);
static	DEVICE_ATTR(modem_disable1,		S_IRWXUGO, show_gpio, set_gpio);
static	DEVICE_ATTR(modem_disable2,		S_IRWXUGO, show_gpio, set_gpio);
//[*]--------------------------------------------------------------------------------------------------[*]
//[*]--------------------------------------------------------------------------------------------------[*]
static	DEVICE_ATTR(led_red, 			S_IRWXUGO, show_gpio, set_gpio);
static	DEVICE_ATTR(led_green,			S_IRWXUGO, show_gpio, set_gpio);
static	DEVICE_ATTR(led_blue,			S_IRWXUGO, show_gpio, set_gpio);
//[*]--------------------------------------------------------------------------------------------------[*]
//[*]--------------------------------------------------------------------------------------------------[*]
static	DEVICE_ATTR(hdmi_state,			S_IRWXUGO, show_hdmi, NULL);
//[*]--------------------------------------------------------------------------------------------------[*]
#if defined(CONFIG_BOARD_ODROID_X)||defined(CONFIG_BOARD_ODROID_X2)||defined(CONFIG_BOARD_ODROID_U)||defined(CONFIG_BOARD_ODROID_Q) || defined(CONFIG_BOARD_ODROID_Q2)
#if defined(CONFIG_BOARD_ODROID_X)||defined(CONFIG_BOARD_ODROID_X2)||defined(CONFIG_BOARD_ODROID_U)
static	DEVICE_ATTR(hdmi_resolution,	S_IRWXUGO, show_resolution, NULL);
static	DEVICE_ATTR(vout_mode,			S_IRWXUGO, show_vout_mode, NULL);
static  DEVICE_ATTR(led_blink,          S_IRWXUGO, NULL, set_led_blink);
#endif
static	DEVICE_ATTR(poweroff_trigger,	S_IRWXUGO, NULL, set_poweroff_trigger);
static	DEVICE_ATTR(boot_mode,			S_IRWXUGO, show_boot_mode, NULL);
#endif
//[*]--------------------------------------------------------------------------------------------------[*]
#define INFORM_BASE     0x0800
#define INFORM_OFFSET   0x0004
#define INFORM_REG_CNT  8

static  DEVICE_ATTR(inform0, S_IRWXUGO, show_inform, set_inform);
static  DEVICE_ATTR(inform2, S_IRWXUGO, show_inform, set_inform);
static  DEVICE_ATTR(inform5, S_IRWXUGO, show_inform, set_inform);
static  DEVICE_ATTR(inform6, S_IRWXUGO, show_inform, set_inform);
static  DEVICE_ATTR(inform7, S_IRWXUGO, show_inform, set_inform);
//[*]--------------------------------------------------------------------------------------------------[*]
static struct attribute *odroid_sysfs_entries[] = {
	&dev_attr_wifi_enable.attr,
	&dev_attr_wifi_host_wake.attr,
	&dev_attr_wifi_nrst.attr,

	&dev_attr_bt_enable.attr,
	&dev_attr_bt_wake.attr,
	&dev_attr_bt_host_wake.attr,
	&dev_attr_bt_nrst.attr,

	&dev_attr_audio_en.attr,

	&dev_attr_power_2v8.attr,
	&dev_attr_power_3v3.attr,
	&dev_attr_power_5v0.attr,
	&dev_attr_power_12v0.attr,
	&dev_attr_output_485.attr,

	&dev_attr_modem_power.attr,
	&dev_attr_modem_reset.attr,
	&dev_attr_modem_disable1.attr,
	&dev_attr_modem_disable2.attr,

	&dev_attr_led_red.attr,
	&dev_attr_led_green.attr,
	&dev_attr_led_blue.attr,

	&dev_attr_hdmi_state.attr,
#if defined(CONFIG_BOARD_ODROID_X)||defined(CONFIG_BOARD_ODROID_X2)||defined(CONFIG_BOARD_ODROID_U)||defined(CONFIG_BOARD_ODROID_Q)||defined(CONFIG_BOARD_ODROID_Q2)
#if defined(CONFIG_BOARD_ODROID_X)||defined(CONFIG_BOARD_ODROID_X2)||defined(CONFIG_BOARD_ODROID_U)
	&dev_attr_hdmi_resolution.attr,
	&dev_attr_vout_mode.attr,
	&dev_attr_led_blink.attr,
#endif
	&dev_attr_poweroff_trigger.attr,
	&dev_attr_boot_mode.attr,
#endif	

    &dev_attr_inform0.attr,     // value clear xreset signal (used bootloader to kernel)
    &dev_attr_inform2.attr,     // value clear xreset signal (used bootloader to kernel)
    &dev_attr_inform5.attr,     // value clear power reset signal (used kernel to bootloader)
    &dev_attr_inform6.attr,     // value clear power reset signal (used kernel to bootloader)
    &dev_attr_inform7.attr,     // value clear power reset signal (used kernel to bootloader)
	NULL
};

static struct attribute_group odroid_sysfs_attr_group = {
	.name   = NULL,
	.attrs  = odroid_sysfs_entries,
};

//[*]--------------------------------------------------------------------------------------------------[*]
#define REG_INFORM0            (S5P_INFORM0)

//[*]--------------------------------------------------------------------------------------------------[*]
static 	ssize_t show_inform0    (struct device *dev, struct device_attribute *attr, char *buf)
{
    return  sprintf(buf, "0x%08X\n", readl(REG_INFORM0));
}

//[*]--------------------------------------------------------------------------------------------------[*]
static 	ssize_t set_inform0     (struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    unsigned int	val;

    if(buf[0] == '0' && ((buf[1] == 'X') || (buf[1] == 'x')))
        val = simple_strtol(buf, NULL, 16);
    else
        val = simple_strtol(buf, NULL, 10);

    writel(val, REG_INFORM0);
	
	return  count;
}

//[*]--------------------------------------------------------------------------------------------------[*]
static	ssize_t show_boot_mode (struct device *dev, struct device_attribute *attr, char *buf)
{
	int OM_STAT=0;
	
	OM_STAT = readl(EXYNOS4_OM_STAT);
	
	if(OM_STAT == BOOT_MMCSD) 	OM_STAT=1;
	else 						OM_STAT=0;
	
	return	sprintf(buf, "%d\n", OM_STAT);
}

//[*]--------------------------------------------------------------------------------------------------[*]
//[*]--------------------------------------------------------------------------------------------------[*]
static 	ssize_t show_gpio		(struct device *dev, struct device_attribute *attr, char *buf)
{
	int	i;

	for (i = 0; i < ARRAY_SIZE(sControlGpios); i++) {
		if(sControlGpios[i].gpio)	{
			if(!strcmp(sControlGpios[i].name, attr->attr.name))
				return	sprintf(buf, "%d\n", (gpio_get_value(sControlGpios[i].gpio) ? 1 : 0));
		}
	}
	
	return	sprintf(buf, "ERROR! : Not found gpio!\n");
}

//[*]--------------------------------------------------------------------------------------------------[*]
#if defined(CONFIG_FB_S5P_S6E8AA1)
    extern void s6e8aa1_lcd_onoff  (unsigned char on);
#endif
#if defined(CONFIG_FB_S5P_S6EVR01)
    extern void s6evr01_lcd_onoff  (unsigned char on);
#endif

static 	ssize_t set_gpio		(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    unsigned int	val, i;

    if(!(sscanf(buf, "%d\n", &val))) 	return	-EINVAL;

	for (i = 0; i < ARRAY_SIZE(sControlGpios); i++) {
		if(sControlGpios[i].gpio)	{
			if(!strcmp(sControlGpios[i].name, attr->attr.name))	{
				if(sControlGpios[i].output) {
                    #if defined(CONFIG_FB_S5P_S6E8AA1)
            			if(!strcmp("power_3v3", attr->attr.name))
                            s6e8aa1_lcd_onoff  (((val != 0) ? 1 : 0));
                    #endif
                    #if defined(CONFIG_FB_S5P_S6EVR01)
            			if(!strcmp("power_3v3", attr->attr.name))
                            s6evr01_lcd_onoff  (((val != 0) ? 1 : 0));
                    #endif
    				gpio_set_value(sControlGpios[i].gpio, ((val != 0) ? 1 : 0));
				}
				else
					printk("This GPIO Configuration is INPUT!!\n");
			    return count;
			}
		}
	}

	printk("%s[%d] : undefined gpio!\n", __func__,__LINE__);
    return 	count;
}

//[*]--------------------------------------------------------------------------------------------------[*]
static 	ssize_t show_inform		(struct device *dev, struct device_attribute *attr, char *buf)
{
	int	    i;
	char    name[7];

	for (i = 0; i < INFORM_REG_CNT; i++) {
	    
	    memset(name, 0x00, sizeof(name));	    sprintf(name, "inform%d", i);
	    
	    if(!strncmp(attr->attr.name, name, sizeof(name)))    {
	        return  sprintf(buf, "0x%08X\n", readl(EXYNOS_PMUREG(INFORM_BASE + i * INFORM_OFFSET)));
	    }
	}
	
	return	sprintf(buf, "ERROR! : Not found %s reg!\n", attr->attr.name);
}

//[*]--------------------------------------------------------------------------------------------------[*]
static 	ssize_t set_inform		(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    unsigned int	val, i;
	char            name[7];

    if(buf[0] == '0' && ((buf[1] == 'X') || (buf[1] == 'x')))
        val = simple_strtol(buf, NULL, 16);
    else
        val = simple_strtol(buf, NULL, 10);

	for (i = 0; i < INFORM_REG_CNT; i++) {
	    
	    memset(name, 0x00, sizeof(name));	    sprintf(name, "inform%d", i);
	    
	    if(!strncmp(attr->attr.name, name, sizeof(name)))    {
	        writel(val, EXYNOS_PMUREG(INFORM_BASE + i * INFORM_OFFSET)); 
		    return count;
	    }
	}

	printk("ERROR! : Not found %s reg!\n", attr->attr.name);
	return  count;
}


//[*]--------------------------------------------------------------------------------------------------[*]
#if defined(CONFIG_BOARD_ODROID_X)||defined(CONFIG_BOARD_ODROID_X2)||defined(CONFIG_BOARD_ODROID_U)||defined(CONFIG_BOARD_ODROID_Q)||defined(CONFIG_BOARD_ODROID_Q2)

extern	void    odroid_keypad_trigger(unsigned int  press_time_sec);

static 	ssize_t set_poweroff_trigger    (struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    unsigned int	val;

    if(!(sscanf(buf, "%d\n", &val))) 	return	-EINVAL;

    //press power off button
    if((val != 0) && (val < 5)) odroid_keypad_trigger(val % 5);
    
    return 	count;
}

#endif

#if defined(CONFIG_BOARD_ODROID_X)||defined(CONFIG_BOARD_ODROID_X2)||defined(CONFIG_BOARD_ODROID_U)

static	ssize_t show_resolution (struct device *dev, struct device_attribute *attr, char *buf)
{
	return	sprintf(buf, "%d\n", odroid_get_hdmi_resolution());
}
static	ssize_t show_vout_mode (struct device *dev, struct device_attribute *attr, char *buf)
{
	return	sprintf(buf, "%d\n", odroid_get_vout_mode() ? 1 : 0);
}

extern	void    odroid_keypad_led_blink(unsigned int  time_sec);

static 	ssize_t set_led_blink   (struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    unsigned int	val;

    if(!(sscanf(buf, "%d\n", &val))) 	return	-EINVAL;

    // value : 0 ~ 255 (0 : OFF, 1 ~ 255 : Blink period)
    if((val >= 0) && (val < 256))   odroid_keypad_led_blink(val);
    
    return 	count;
}

#endif
//[*]--------------------------------------------------------------------------------------------------[*]
extern	bool s5p_hpd_get_status(void);

static	ssize_t show_hdmi	(struct device *dev, struct device_attribute *attr, char *buf)
{
#if defined(CONFIG_VIDEO_TVOUT)
	int status = s5p_hpd_get_status();
	
	if(status)	return	sprintf(buf, "%s\n", "on");
	else		return	sprintf(buf, "%s\n", "off");
#else
	return	sprintf(buf, "%s\n", "off");
#endif
}

//[*]--------------------------------------------------------------------------------------------------[*]
void 	SYSTEM_POWER_CONTROL	(int power, int val)
{
	int	index;
	
	switch(power)	{
		case	0:	index = SYSTEM_POWER_3V3;		break;
		case	1:	index = SYSTEM_POWER_5V0;		break;
		case	3:	index = SYSTEM_POWER_12V0;		break;	
		case	2:	// 2v8
		case	4:	// out485
		default	:									return;
	}

	if(sControlGpios[index].gpio)
		gpio_set_value(sControlGpios[index].gpio, ((val != 0) ? 1 : 0));
	else	
		printk("ERROR : Not found gpio!\n");
}

EXPORT_SYMBOL(SYSTEM_POWER_CONTROL);

//[*]--------------------------------------------------------------------------------------------------[*]
//[*]--------------------------------------------------------------------------------------------------[*]
static int	odroid_sysfs_resume(struct platform_device *dev)
{
	#if	defined(DEBUG_PM_MSG)
		printk("%s\n", __FUNCTION__);
	#endif

    return  0;
}

//[*]--------------------------------------------------------------------------------------------------[*]
static int	odroid_sysfs_suspend(struct platform_device *dev, pm_message_t state)
{
	#if	defined(DEBUG_PM_MSG)
		printk("%s\n", __FUNCTION__);
	#endif
	
    return  0;
}

//[*]--------------------------------------------------------------------------------------------------[*]
static	int		odroid_sysfs_probe		(struct platform_device *pdev)	
{
	int	i;

	// Control GPIO Init
	for (i = 0; i < ARRAY_SIZE(sControlGpios); i++) {
		if(sControlGpios[i].gpio)	{
			if(gpio_request(sControlGpios[i].gpio, sControlGpios[i].name))	{
				printk("%s : %s gpio reqest err!\n", __FUNCTION__, sControlGpios[i].name);
			}
			else	{
				if(sControlGpios[i].output)		gpio_direction_output	(sControlGpios[i].gpio, sControlGpios[i].value);
				else							gpio_direction_input	(sControlGpios[i].gpio);
	
				s3c_gpio_setpull		(sControlGpios[i].gpio, sControlGpios[i].pud);
			}
		}
	}
	
	// GPIO IRQ Init
	for(i = 0; i < ARRAY_SIZE(sIrqGpios); i++)	{
		if(sIrqGpios[i].gpio)	{
			if(gpio_request(sIrqGpios[i].gpio, sIrqGpios[i].name))	{
				printk("%s : %s gpio reqest err!\n", __FUNCTION__, sIrqGpios[i].name);
			}
			else	{
				gpio_direction_input(sIrqGpios[i].gpio);
				s3c_gpio_setpull	(sIrqGpios[i].gpio, sIrqGpios[i].pud);
				printk("Register Irq Gpio : %s gpio = %d, irq num = %d\n"	, sIrqGpios[i].name
																			, sIrqGpios[i].gpio
																			, s5p_register_gpio_interrupt(sIrqGpios[i].gpio));
			}
		}
	}
	// Default GPIO Init
	for(i = 0; i < ARRAY_SIZE(sDefaultGpios); i++)	{
		if(sDefaultGpios[i].gpio)	{
			if(gpio_request(sDefaultGpios[i].gpio, sDefaultGpios[i].name))	{
				printk("%s : %s gpio reqest err!\n", __FUNCTION__, sDefaultGpios[i].name);
			}
			else	{
				if(sDefaultGpios[i].output)		gpio_direction_output	(sDefaultGpios[i].gpio, sDefaultGpios[i].value);
				else							gpio_direction_input	(sDefaultGpios[i].gpio);
	
				s3c_gpio_setpull		(sDefaultGpios[i].gpio, sDefaultGpios[i].pud);
			}
		}
	}
	
	for(i = 0; i < ARRAY_SIZE(sDefaultGpios); i++)	{
		if(sDefaultGpios[i].gpio)		gpio_free(sDefaultGpios[i].gpio);
	}
	
#if defined(SLEEP_DISABLE_FLAG)
	#ifdef CONFIG_HAS_WAKELOCK
		wake_lock(&sleep_wake_lock);
	#endif
#endif

	return	sysfs_create_group(&pdev->dev.kobj, &odroid_sysfs_attr_group);
}

//[*]--------------------------------------------------------------------------------------------------[*]
static	int		odroid_sysfs_remove		(struct platform_device *pdev)	
{
	int	i;
	
	for (i = 0; i < ARRAY_SIZE(sControlGpios); i++) 	{
		if(sControlGpios[i].gpio)	gpio_free(sControlGpios[i].gpio);
	}

	for(i = 0; i < ARRAY_SIZE(sIrqGpios); i++)	{
		if(sIrqGpios[i].gpio)		gpio_free(sIrqGpios[i].gpio);
	}
	
#if defined(SLEEP_DISABLE_FLAG)
	#ifdef CONFIG_HAS_WAKELOCK
		wake_unlock(&sleep_wake_lock);
	#endif
#endif

    sysfs_remove_group(&pdev->dev.kobj, &odroid_sysfs_attr_group);
    
    return	0;
}

//[*]--------------------------------------------------------------------------------------------------[*]
static struct platform_driver odroid_sysfs_driver = {
	.driver = {
		.name = "odroid-sysfs",
		.owner = THIS_MODULE,
	},
	.probe 		= odroid_sysfs_probe,
	.remove 	= odroid_sysfs_remove,
	.suspend	= odroid_sysfs_suspend,
	.resume		= odroid_sysfs_resume,
};

//[*]--------------------------------------------------------------------------------------------------[*]
static int __init odroid_sysfs_init(void)
{	
#if defined(SLEEP_DISABLE_FLAG)
	#ifdef CONFIG_HAS_WAKELOCK
		printk("--------------------------------------------------------\n");
		printk("%s(%d) : Sleep Disable Flag SET!!(Wake_lock_init)\n", __FUNCTION__, __LINE__);
		printk("--------------------------------------------------------\n");

	    wake_lock_init(&sleep_wake_lock, WAKE_LOCK_SUSPEND, "sleep_wake_lock");
	#endif
#else
	printk("--------------------------------------------------------\n");
	printk("%s(%d) : Sleep Enable !! \n", __FUNCTION__, __LINE__);
	printk("--------------------------------------------------------\n");
#endif

    return platform_driver_register(&odroid_sysfs_driver);
}

//[*]--------------------------------------------------------------------------------------------------[*]
static void __exit odroid_sysfs_exit(void)
{
#if defined(SLEEP_DISABLE_FLAG)
	#ifdef CONFIG_HAS_WAKELOCK
	    wake_lock_destroy(&sleep_wake_lock);
	#endif
#endif
    platform_driver_unregister(&odroid_sysfs_driver);
}

//[*]--------------------------------------------------------------------------------------------------[*]
module_init(odroid_sysfs_init);
module_exit(odroid_sysfs_exit);

//[*]--------------------------------------------------------------------------------------------------[*]
MODULE_DESCRIPTION("SYSFS driver for odroid-Dev board");
MODULE_AUTHOR("Hard-Kernel");
MODULE_LICENSE("GPL");

//[*]--------------------------------------------------------------------------------------------------[*]
//[*]--------------------------------------------------------------------------------------------------[*]
