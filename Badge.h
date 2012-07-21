#ifndef _BADGE_H_
#define _BADGE_H_  

#pragma CLOCK_FREQ 32000000

// TRIS REG REF: 1 = INPUT 0=OUTPUT      

#define SIG_RA_AN_ETOHSENSE_I	0	// Ethanol Sensor Input			IN
#define SIG_RA_AN_RFRSSI_I		1	// RD Sig Strength Indicator    IN
#define SIG_RA_ETOH_HTR_N_O		2   // Enable ETOH Heater (L)		OUT
#define SIG_RA_RFCS_N_O			3	// Enable RF Chip SPI Cmnd (L)	OUT
#define SIG_RA_RA4_NC			4   // Not Connected
#define SIG_RA_RA5_NC			5	// Not Connected
#define SIG_RA_RA6_NC			6	// Not Connected
#define SIG_RA_RA7_NC			7	// Not Connected
#define TRISCONFIG_RA		11110011b
#define INITIALDATA_RA	    00001100b		

#define SIG_RB_RFINT_N_I		0	// RF Interrupt from RF Chip	IN
#define SIG_RB_DISPLED1_O		1   // Anode to Display LED 		OUT	
#define SIG_RB_DISPLED2_O		2   // Anode to Display LED 		OUT
#define SIG_RB_DISPLED3_O		3   // Anode to Display LED 		OUT
#define SIG_RB_DISPLED4_O		4   // Anode to Display LED 		OUT
#define SIG_RB_DISPLED5_O		5   // Anode to Display LED 		OUT	
#define SIG_RB_PGC_I			6	// In Circuit Prog Clock		IN
#define SIG_RB_PGD_I			7	// In Circuit Prog Data			IN
#define TRISCONFIG_RB		11000001b
#define INITIALDATA_RB		00000000b

#define SIG_RC_DISP_RED_O		0	// Display Red Cathode			OUT
#define SIG_RC_DISP_GRN_O		1	// Display Green Cathode		OUT
#define SIG_RC_DISP_BLU_O		2	// Display Blue Cathode			OUT
#define SIG_RC_SPI_SCK_O		3	// SPI Clock					OUT
#ifdef BOARD_PRE_R6
#define SIG_RC_SPI_SDI_I		5	// SPI Data In	                IN (error need swap with SDO)
#define SIG_RC_SPI_SDO_O		4	// SPI Data Out					OUT (error need swap with SDI)
#else
#define SIG_RC_SPI_SDI_I		4	// SPI Data In	                IN 
#define SIG_RC_SPI_SDO_O		5	// SPI Data Out					OUT 
#endif
#define SIG_RC_DISPLED6_O		6  	// Anode to Display LED 		OUT
#define SIG_RC_DISPLED7_O		7	// Anode to Display LED 		OUT

#ifdef BOARD_PRE_R6
#define TRISCONFIG_RC		00100000b
#define INITIALDATA_RC		00000111b
#else
#define TRISCONFIG_RC		00010000b
#define INITIALDATA_RC		00000111b
#endif

#define SIG_RD_RFFSK_I			0	// Raw FSK data from RF Chip	IN (TEST: OUT for MRF49XA.c Test File)
#define SIG_RD_RFRCLK_I			1	// Recovered Clock from RF Chip	IN
#define SIG_RD_RFDIO_IO			2	// Raw Data IO from RF Chip		IN
#define SIG_RD_RFRESET_N_O		3	// RF Reset Signal				OUT
#define SIG_RD_RD4_NC			4	// Not Connected				IN	
#define SIG_RD_I2CSDA_IO		5	// I2C Data Line 				IN (per spec)
#define SIG_RD_I2CSCL_IO		6	// I2C Clock Line				IN (per spec)
#define SIG_RD_SOUND_STBY_N_O	7	// Amp Standby					OUT
#define TRISCONFIG_RD		01110110b
#define INITIALDATA_RD		00001001b

#define SIG_RE_RE0_NC			0	// Not Connected
#define SIG_RE_RE1_NC			1	// Not Connected
#define SIG_RE_RE2_NC			2	// Not Connected
#define SIG_RE_RE3_NC			3	// Not Connected
#define SIG_RE_RE4_NC			4	// Not Connected
#define SIG_RE_RE5_NC			5	// Not Connected
#define SIG_RE_RE6_NC			6	// Not Connected
#define SIG_RE_RE7_NC			7	// Not Connected
#define TRISCONFIG_RE		11111111b
#define INITIALDATA_RE		00000000b

#define SIG_RF_RF0_NC			0	// Not Connected Internally		IN
#define SIG_RF_FUNC_N_I			1	// Main Function Button			IN
#define SIG_RF_AUXBUT_N_I		2	// Auxiliary Control Button		IN
#define	SIG_RF_INIT_N_I			3	// Initialization Pads on Back	IN
#define SIG_RF_PLAY1			4	// PlayPad (hacking Expansion)	OUT
#define SIG_RF_PLAY2			5	// PlayPad (hacking Expansion)	OUT
#define SIG_RF_PLAY3			6	// PlayPad (hacking Expansion)	OUT
#define SIG_RF_PLAY4			7	// PlayPad (hacking Expansion)	OUT
#define TRISCONFIG_RF		00001110b
#define INITIALDATA_RF		00001110b

#define SIG_RG_USBRSET_N_O		0	// Reset on the USB translater
#define SIG_RG_SERTX_O			1	// Serial Port TX
#define SIG_RG_SERRX_I			2	// Serial Recieved Data
#define SIG_RTS_N_O				3	// Ready to Send (L)
#define SIG_CTS_N_I				4	// Clear to Send
#define TRISCONFIG_RG		11110110b  // TX needs tris to be '1' as USART overrides it
#define INITIALDATA_RG		00000001b // USB enabled


//
//  FUNCTION EXTERNAL DEFINITIONS
//

//
// In File: mcu_initcontrol.c (MCU initialization  and  runtime utils
//
//#define TIMER_REGVAL 133  EXACT 8KHZ, but interrupt routine taking too long (120?)
//#define TIMER_REGVAL 105 (last good)
#define TIMER_REGVAL 133

void mcu_initialize();


//
// In File: sub_sound.c (Sound Subsystem)
//
extern void sound_config_polled();
extern void sound_val_polled(unsigned char );
extern void sound_hsstart();
extern void sound_hsstop();
extern void sound_val_hs(unsigned char );
extern void sound_i2c_reset();
void sound_beep_polled();
#define sound_amp_on() portd.SIG_RD_SOUND_STBY_N_O = 1
#define sound_amp_off() portd.SIG_RD_SOUND_STBY_N_O = 0


//
// In File ext_chiptune.c (Chiptune Player)
//
void tune_init();
void tune_play_intr();
void tune_playsong();
void tune_startsong(unsigned char );
void tune_songwork();
extern unsigned char playsong;

//#ifdef MRF49XA_LOCAL
// Defines to get the MicroChip test program working (MRF49ZA.c)
//
#define SPI_SDO portc.SIG_RC_SPI_SDO_O  // OUTPUT
#define SPI_SDI portc.SIG_RC_SPI_SDI_I  // INPUT
#define SPI_SCK portc.SIG_RC_SPI_SCK_O  // OUTPUT
#define SPI_CS  porta.SIG_RA_RFCS_N_O  // OUTPUT
#define RF_SDO	SPI_SDI
#define RF_SDI  SPI_SDO
#define RF_SCK  SPI_SDK
#define RF_FSEL portd.SIG_RD_RFFSK_I  // OUTPUT
#define DelayMs delay_ms
//#endif

//
//
extern void etoh_init();
extern unsigned short etoh_get();
extern unsigned char etoh_breathtest(unsigned char , unsigned short );
extern unsigned char etoh_getreward();
#define ETOH_START 1			// Commands
#define ETOH_KILL 2
#define ETOH_DOWORK 0

#define ETOH_WORKING 2			// Machine Status
#define ETOH_DONE    1 
#define ETOH_IDLE	 0


#define REWARD_SOBER 0			// Reward Status
#define REWARD_TIPSY  1
#define REWARD_DRUNK 2

// 
// POV module
//
#define LED_SHOW_NONE	0
#define LED_SHOW_RED	1
#define LED_SHOW_GRN	2
#define LED_SHOW_BLU	3
#define LED_SHOW_AUTO	4
extern void led_showbin(unsigned char,  unsigned char);
extern void led_pov_next(unsigned char );
extern void led_pov(unsigned char, unsigned short  );


//
// Serial / USB

extern void usb_ser_init();
extern void usb_putchar(unsigned char);
unsigned char usb_getchar(char *ch);

// sub_lights.c

extern void light_init();
extern void light_set(unsigned char, unsigned char, unsigned char, unsigned char);
extern void light_showbin(unsigned char, unsigned char, unsigned char, unsigned char);
extern void light_pause();
extern void light_play(); 
extern void light_intr(); 
extern void light_show(unsigned char , unsigned char );
extern void light_animate(unsigned char );
extern unsigned char lightshowrun;

#define NUMLIGHTS 7
#define MAXLIGHTPHASE 23
#define NUMINTENSITY  8
#define LIGHTSHOW_OFF	0
#define LIGHTSHOW_CEYLON 1
#define LIGHTSHOW_RGWAVE 2
#define LIGHTSHOW_COUNTDOWN 3
#define LIGHTSHOW_RAINBOW 4
#define LIGHTSHOW_DONGS 5

// sub_sample.c
extern void sample_play();
extern void sample_intr();
extern unsigned char playsample;

// sub_nvsettings.c



#define NVBT303		0x00		// 303 Badge
#define NVBTHAC		0x01		// Denhac Badge
#define NVBTSKY		0x02		// Skytalks Badge	


#define NVPRMUSER		0x00   // Plain old user
#define NBPRMELEV		0x01   // Elevated User
#define NBPRMROOT		0x02   // Master User (Root)



extern void nvreadbuf();
extern void nvsavebuf();
extern unsigned char nvget_badgeid();
extern void nvset_badgeid(unsigned char );
extern unsigned char nvget_badgetype();
extern void nvset_badgetype(unsigned char);
extern unsigned char nvget_badgeperm();
extern void nvset_badgeperm(unsigned char );
extern unsigned char nvget_socvec1();
extern void nvset_socvec1(unsigned char );

// sub_btn.c
extern void proc_btn1();


//
// Flash LED Macro (for debugging purposes)
//
#define FLASHLED {portc.SIG_RC_DISP_RED_O = 0;portb.SIG_RB_DISPLED1_O = 1;delay_s(1);portc.SIG_RC_DISP_RED_O = 1;portb.SIG_RB_DISPLED1_O = 0; delay_ms(100);}



#endif //_BADGE_H_
