//[*]--------------------------------------------------------------------------------------------------[*]
//
//
//  Solomon Config Initailize
//  2012.10.1
//
//
//[*]--------------------------------------------------------------------------------------------------[*]


//[*]--------------------------------------------------------------------------------------------------[*]
struct ChipSetting {
	unsigned char No;
	unsigned char Reg;
	unsigned char Data1;
	unsigned char Data2;
};

//[*]--------------------------------------------------------------------------------------------------[*]
struct ChipSetting Config[]={
	{ 0, 0x04, 0x00, 0x00}, 	// SSD2533 wakeup
	{-1, 0x30, 0x00, 0x00},	// mdelay

       { 1, 0x06, 0x11, 0x00},   	//Set drive line 0=1, 1F=
	{ 1, 0x07, 0x1e, 0x00},  	//Set sense line 0=1, 3F=	
	
	{ 2, 0x08, 0x01, 0x91},		//Set 1 drive line reg
	{ 2, 0x09, 0x01, 0x90},		//Set 2 drive line reg
	{ 2, 0x0A, 0x01, 0x8F},		//Set 3 drive line reg
	{ 2, 0x0B, 0x01, 0x8E},		//Set 4 drive line reg
	{ 2, 0x0C, 0x01, 0x8D},		//Set 5 drive line reg
	{ 2, 0x0D, 0x01, 0x8C},		//Set 6 drive line reg
	{ 2, 0x0E, 0x01, 0x8B},		//Set 7 drive line reg
	{ 2, 0x0F, 0x01, 0x8A},		//Set 8 drive line reg
	{ 2, 0x10, 0x01, 0x89},		//Set 9 drive line reg
	{ 2, 0x11, 0x01, 0x88},		//Set 10 drive line reg
	{ 2, 0x12, 0x01, 0x87},		//Set 11 drive line reg
	{ 2, 0x13, 0x00, 0x86},		//Set 12 drive line reg
	{ 2, 0x14, 0x00, 0x85},		//Set 13 drive line reg
	{ 2, 0x15, 0x00, 0x84},		//Set 14 drive line reg
	{ 2, 0x16, 0x00, 0x83},		//Set 15 drive line reg
	{ 2, 0x17, 0x00, 0x82},		//Set 16 drive line reg
	{ 2, 0x18, 0x00, 0x81},		//Set 17 drive line reg
	{ 2, 0x19, 0x00, 0x80},		//Set 18 drive line reg

       { 1, 0xd5, 0x06, 0x00},        //Set Driving voltage 0(5.5V) to 7(9V)
       { 1, 0xd8, 0x04, 0x00},        //Sense Bias R     	
	{ 1, 0x2A, 0x07, 0x00},		//Set sub-frame default=1, range 0 to F
	{ 1, 0x2C, 0x01, 0x00},		//Median Filter 0:disable to 1:enable
	{ 1, 0x2E, 0x0B, 0x00},		//Sub-frame Drive pulse number default=0x17	
	{ 1, 0x2F, 0x01, 0x00},		//Integration Gain

	{ 1, 0x30, 0x03, 0x00},		//start integrate 125ns/div	
	{ 1, 0x31, 0x0B, 0x00},		//stop integrate 125n/div 

	{ 2, 0x33, 0x00, AREA_MAX},		//Set Min. Finger area 
	{ 2, 0x34, 0x00, FINGER_LEVEL},		//Set Min. Finger level 
	{ 2, 0x35, 0x00, 0x00},		              //Set Min. Finger weight
	{ 2, 0x36, 0x00, PRESSURE_MAX},	//Set Max. Finger pressure

	{ 1, 0x37, 0x00, 0x00},		//Segmentation Depth
	{ 1, 0x39, 0x02, 0x00},        //Select Finger CG calculation method
	{ 1, 0x3D, 0x01, 0x00},		// 2D filter
	
	//{ 1, 0x3E, 0x01, 0x00},	// Switch OFF Auto Cal	
       
       { 1, 0x40, 0xfa, 0x00},       //added
       { 1, 0x44, 0x01, 0x00},       //added

	{ 1, 0x53, 0x01, 0x00},	//Event move tolerance
	{ 2, 0x54, 0x00, 0x80},		//X tracking
	{ 2, 0x55, 0x00, 0x80},		//Y tracking	
	{ 1, 0x56, MOVE_AVR_FILTER, 0x00},	//Moving Average Filter 0:null, 1:5-3(Set), 2:6-2  3:7-1
	{ 1, 0x58, 0x00, 0x00},				//Finger weight scaling
	{ 1, 0x59, 0x01, 0x00},				//Enable Random walk
	{ 1, 0x5B, 0x01, 0x00},				//Set Random walk window ++++

	{ 1, 0x65, 0x01, 0x00},		//XY Mapping :: Y-Inverrt
       { 2, 0x66, 0x89, 0xc0},	       //Scaling factor for X coordinate.
       { 2, 0x67, 0x8A, 0x15},        //Scaling factor for Y coordinate.

	// IRQ MODE : Event
	{ 2, 0x7A, 0xFF, 0xC7},		//Event Mask - Enable Leave Event
	{ 2, 0x7B, 0xFF, 0xF0},		//IRQ Mask - Mask off All Fingers interrupt except Event
	{ 1, 0x89, 0x00, 0x00},		//Enable Event IRQ mode
	
	{ 1, 0x8A, MAX_FINGERS, 0x00},	//Max finger
	//{ 1, 0x8B, 0x10, 0x00},			//Edge Remap
	{ 1, 0x8C, 0xB0, 0x00},		//Edge Suppress

	 // 2012.06 new option	
    //{ 1, 0x8b, 0x01, 0x00},   //added
    //{ 1, 0x8c, 0xb0, 0x00},   //added
    //

	   // 2012.06 new option	
    { 1, 0xac, 0x03, 0x00},
    { 1, 0xad, 0x02, 0x00},
    { 1, 0xae, 0x0f, 0x00},
    { 1, 0xaf, 0x40, 0x00},
    { 1, 0xb0, 0x00, 0x00},
    { 1, 0xbb, 0x00, 0x00},
    { 1, 0xbc, 0x01, 0x00},
    
    { 1, 0xD7, 0x04, 0x00},         //ADC range default=4, 0 to 7 
    { 1, 0xDB, 0x02, 0x00},	     //Set integration cap default=0, 0 to 7 	    

    { 1, 0x25, 0x01, 0x00},	//Set scan mode A
};

//[*]--------------------------------------------------------------------------------------------------[*]

