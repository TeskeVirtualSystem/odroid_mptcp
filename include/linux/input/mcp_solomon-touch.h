//[*]--------------------------------------------------------------------------------------------------[*]
//
//
// 
//  I2C Touchscreen driver (platform data struct)
//  2012.10.1 
// 
//

//[*]--------------------------------------------------------------------------------------------------[*]
#ifdef CONFIG_HAS_EARLYSUSPEND
	#include <linux/earlysuspend.h>
#endif

//[*]--------------------------------------------------------------------------------------------------[*]
//       Software AVR Filter
//[*]--------------------------------------------------------------------------------------------------[*]
#if defined(CONFIG_ANDROID_PARANOID_NETWORK)
  //  #define SOFT_AVR_FILTER_ENABLE
#endif

#if defined(SOFT_AVR_FILTER_ENABLE)
    #define SOFT_AVR_COUNT                   10
    #define SOFT_AVR_MOVE_TOL_X         20   // First move tol
    #define SOFT_AVR_MOVE_TOL_Y         20   // First move tol
    #define SOFT_AVR_ENABLE_SPEED       5
#endif

#if defined(SOFT_AVR_FILTER_ENABLE)
    #define	MOVE_AVR_FILTER		0x00		// 0x56 Register (0x00 ~ 0x03 : 0x00 default)
#else    
    #define	MOVE_AVR_FILTER		0x02		// 0x56 Register (0x00 ~ 0x03 : 0x00 default)
#endif

//[*]--------------------------------------------------------------------------------------------------[*]
//      Defines
//[*]--------------------------------------------------------------------------------------------------[*]
#define FINGER_LEVEL        0x35
#define MAX_FINGERS		    10
#define	DRIVE_DATA_MAX	    600 
#define	SENSE_DATA_MAX	    1024
#define	TRACKING_ID_MAX	    16
#define	AREA_MAX            0x00
#define	PRESSURE_MAX		0x30

#define	EVENT_FIFO_SCLR	    0x87
#define	EVENT_UNKNOWN 	    0x00
#define	EVENT_PRESS		    0x03
#define	EVENT_MOVE		    0x04
#define	EVENT_RELEASE		0x05

#define	NOP					0x00	
#define	DEVICE_ID			0x02	
#define	VERSION_ID			0x03	
#define	TOUCH_STATUS		0x79
#define	EVENT_STACK		    0x86

//[*]--------------------------------------------------------------------------------------------------[*]
typedef	struct	status__t	{
	unsigned short	fifo_valid		:1;
	unsigned short	fifo_overflow	:1;
	unsigned short	large_object	:1;
	unsigned short	abnomal_status	:1;
	unsigned short	fingers			:10;
	unsigned short	reserved		:2;
}	__attribute__ ((packed))	status_t;

typedef union	status__u	{
	unsigned char	byte[sizeof(status_t)];
	status_t		bits;
}	__attribute__ ((packed))	status_u;

typedef	struct	finger_data__t	{	
	unsigned char	speed		:4;	// LSB
	unsigned char	pressure	:4;
	unsigned char	msb_y		:4;
	unsigned char	msb_x		:4;
	unsigned char	lsb_y		:8;
	unsigned char	lsb_x		:8;	// MSB
}	__attribute__ ((packed))	finger_data_t;

typedef union	finger_data__u	{
	unsigned char	byte[sizeof(finger_data_t)];
	finger_data_t	bits;
}	__attribute__ ((packed))	finger_data_u;

typedef	struct	event_stack__t	{	
	unsigned char	speed		:4;	// LSB
	unsigned char	pressure	:4;
	unsigned char	msb_y		:4;
	unsigned char	msb_x		:4;
	unsigned char	lsb_y		:8;
	unsigned char	lsb_x		:8;
	unsigned char	event		:4;
	unsigned char	number		:4;	// MSB
}	__attribute__ ((packed))	event_stack_t;

typedef union	event_stack__u	{
	unsigned char	byte[sizeof(event_stack_t)];
	event_stack_t	bits;
}	__attribute__ ((packed))	event_stack_u;

//[*]--------------------------------------------------------------------------------------------------[*]
//
// Solomon-SSD253x Control Function define
//
//[*]--------------------------------------------------------------------------------------------------[*]
extern	int     SSD253x_calibration	(struct touch *ts);
extern	int 	SSD253x_i2c_read	(struct i2c_client *client, unsigned char *cmd, unsigned int cmd_len, unsigned char *data, unsigned int len);
extern	void	SSD253x_work		(struct touch *ts);
extern	void	SSD253x_enable		(struct touch *ts);
extern	void	SSD253x_disable		(struct touch *ts);
extern	int		SSD253x_early_probe	(struct touch *ts);
extern	int		SSD253x_probe		(struct touch *ts);

//[*]--------------------------------------------------------------------------------------------------[*]
