#include <system.h>
#include "Badge.h"

//#define LED_SHOW_NONE	0
//#define LED_SHOW_RED	1
//#define LED_SHOW_GRN	2
//#define LED_SHOW_BLU	3
//#define LED_SHOW_AUTO	4


rom unsigned char povdata[] = {
   63 ,  33 ,   33 ,  33 ,  30, 0, 0,   // D
   63 ,  37 ,   37 ,  37 ,  33, 0, 0,   // E
   63 ,  16 ,    8 ,   4 ,  63, 0, 0,   // N
   63 ,   4 ,    4 ,   4 ,  63, 0, 0,   // H
   15 ,  20 ,   38 ,  20 ,  15, 0, 0,   // A
   30 ,  33 ,   33 ,  33 ,  33, 0, 0,   // C
   0 ,   0 ,    0 ,   0 ,   0,  0, 0,   // <space>
   0 ,   0 ,    0 ,   0 ,   0,  0, 0    // <space>
};
 
#define DATALEN 56  // 7 * 8
 
 
 
static unsigned char charcnt = 0;





void led_showbin(unsigned char color,  unsigned char bitdata)
{


	portc.SIG_RC_DISP_RED_O = 1;
	portc.SIG_RC_DISP_BLU_O = 1;
	portc.SIG_RC_DISP_GRN_O = 1;  // TURN OFF COLORS
	
	
	if(bitdata & 00000001b) {
		portb.SIG_RB_DISPLED1_O = 1;
	}
	else {
		portb.SIG_RB_DISPLED1_O = 0;
	}
	
	if(bitdata & 00000010b) {
		portb.SIG_RB_DISPLED2_O = 1;
	}
	else {
		portb.SIG_RB_DISPLED2_O = 0;
	}	
	
	if(bitdata & 00000100b) {
		portb.SIG_RB_DISPLED3_O = 1;
	}
	else {
		portb.SIG_RB_DISPLED3_O = 0;
	}			

	if(bitdata & 00001000b) {
		portb.SIG_RB_DISPLED4_O = 1;
	}
	else {
		portb.SIG_RB_DISPLED4_O = 0;
	}	

	if(bitdata & 00010000b) {
		portb.SIG_RB_DISPLED5_O = 1;
	}
	else {
		portb.SIG_RB_DISPLED5_O = 0;
	}	
	if(bitdata & 00100000b) {
		portc.SIG_RC_DISPLED6_O = 1;
	}
	else {
		portc.SIG_RC_DISPLED6_O = 0;
	}	
	if(bitdata & 01000000b) {
		portc.SIG_RC_DISPLED7_O = 1;
	}
	else {
		portc.SIG_RC_DISPLED7_O = 0;
	}			
	
	
	switch(color) {					// TURN ON
	  case LED_SHOW_NONE:
	    break;
	  case LED_SHOW_RED:
		portc.SIG_RC_DISP_RED_O = 0;
		break;
	  case LED_SHOW_GRN:
		portc.SIG_RC_DISP_GRN_O = 0;
		break;
	  case LED_SHOW_BLU:
		portc.SIG_RC_DISP_BLU_O = 0;
		break;
	 }
	 
	 delay_100us(1);
	 portc.SIG_RC_DISP_RED_O = 1;
	 portc.SIG_RC_DISP_BLU_O = 1;
	 portc.SIG_RC_DISP_GRN_O = 1;  // TURN OFF COLORS
	 delay_100us(19);
}

void led_pov_next(unsigned char color)
{
    static unsigned char autocolor = LED_SHOW_RED;
    
	if(charcnt >= DATALEN) {
		charcnt = 0;
		autocolor++;
		if (autocolor > LED_SHOW_BLU)
		    autocolor = LED_SHOW_RED;
	}
    led_showbin(color == LED_SHOW_AUTO ? autocolor : color, povdata[charcnt]);
    charcnt++;
}
	 
	  