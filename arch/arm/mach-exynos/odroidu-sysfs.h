//[*]--------------------------------------------------------------------------------------------------[*]
//
//
// 
//  ODROID Board : ODROID sysfs driver (charles.park)
//  2012.01.17
// 
//
//[*]--------------------------------------------------------------------------------------------------[*]
static struct {
	int		gpio_index;		// Control Index
	int 	gpio;			// GPIO Number
	char	*name;			// GPIO Name == sysfs attr name (must)
	bool 	output;			// 1 = Output, 0 = Input
	int 	value;			// Default Value(only for output)
	int		pud;			// Pull up/down register setting : S3C_GPIO_PULL_DOWN, UP, NONE
} sControlGpios[] = {
	{	WIFI_ENABLE,       	0,					"wifi_enable",		0,	0,	S3C_GPIO_PULL_NONE	},
	{	WIFI_HOST_WAKE,    	0,					"wifi_host_wake",	0,	0,	S3C_GPIO_PULL_NONE	},
	{	WIFI_NRST,   	 	0,					"wifi_nrst",		0,	0,	S3C_GPIO_PULL_NONE	},

	{	BT_ENABLE,         	0,					"bt_enable",		0,	0,	S3C_GPIO_PULL_NONE	},
	{	BT_WAKE,           	0,					"bt_wake",			0,	0,	S3C_GPIO_PULL_NONE	},
	{	BT_HOST_WAKE,      	0,					"bt_host_wake",		0,	0,	S3C_GPIO_PULL_NONE	},
	{	BT_NRST,           	0,					"bt_nrst",			0,	0,	S3C_GPIO_PULL_NONE	},

	{	AUDIO_EN,          	0,					"audio_en",			0,	0,	S3C_GPIO_PULL_DOWN	},

	{	SYSTEM_POWER_2V8,  	0,					"power_2v8",		0,	0,	S3C_GPIO_PULL_NONE	},
	{	SYSTEM_POWER_3V3,  	EXYNOS4_GPA1(1),	"power_3v3",		1,	1,	S3C_GPIO_PULL_DOWN	},
	{	SYSTEM_POWER_5V0,  	0,					"power_5v0",		0,	0,	S3C_GPIO_PULL_DOWN	},
	{	SYSTEM_POWER_12V0, 	0,					"power_12v0",		0,	0,	S3C_GPIO_PULL_NONE	},
	{	SYSTEM_OUTPUT_485, 	0,					"output_485",		0,	0,	S3C_GPIO_PULL_NONE	},

	{	MODEM_POWER,       	0,					"modem_power",		0,	0,	S3C_GPIO_PULL_NONE	},
	{	MODEM_RESET,       	0,					"modem_reset",		0,	0,	S3C_GPIO_PULL_NONE	},
	{	MODEM_DISABLE1,    	0,					"modem_disable1",	0,	0,	S3C_GPIO_PULL_NONE	},
	{	MODEM_DISABLE2,    	0,					"modem_disable2",	0,	0,	S3C_GPIO_PULL_NONE	},
                                    			
	{	STATUS_LED_RED,    	0,					"led_red",			0,	0,	S3C_GPIO_PULL_NONE	},
	{	STATUS_LED_GREEN,  	0,					"led_green",		0,	0,	S3C_GPIO_PULL_NONE	},
	{	STATUS_LED_BLUE,   	0,					"led_blue",			0,	0,	S3C_GPIO_PULL_NONE	},
};
//[*]--------------------------------------------------------------------------------------------------[*]

MODULE_DESCRIPTION("SYSFS driver for odroid-Dev board");
MODULE_AUTHOR("Hard-Kernel");
MODULE_LICENSE("GPL");  
//[*]--------------------------------------------------------------------------------------------------[*]
//[*]--------------------------------------------------------------------------------------------------[*]
