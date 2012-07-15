#include <system.h>
#include "Badge.h"
#include "CharData/Char5x7Data.h" 

#define NUMROW	7
#define NUMCOL	5



rom  char *povstring = "Thank You DT and Kryus !!!  303 & DENHAC   ";
 
#define  NUMCHARS 43


unsigned char readcharmap(unsigned char chr, unsigned char column)
{
    unsigned long caddress;
    unsigned short offset;
    unsigned char cresult;
    
    offset = (unsigned short) chr * NUMCOL;
    offset += column;
    
    caddress = CHARDATA + offset;
   
 
    asm 
    {
	MOVF _caddress + 2, W
	MOVWF _tblptru
	MOVF _caddress + 1, W
	MOVWF _tblptrh
	MOVF _caddress, W
	MOVWF _tblptrl
	TBLRD*
	MOVF _tablat, W
	MOVWF _cresult
	// RETURN			
    }
    return(cresult);
 }

//#define LED_SHOW_NONE	0
//#define LED_SHOW_RED	1
//#define LED_SHOW_GRN	2
//#define LED_SHOW_BLU	3
//#define LED_SHOW_AUTO	4

 

void led_showbin(unsigned char color,  unsigned char bitdata) 
{

	portc.SIG_RC_DISP_RED_O = 1;
	portc.SIG_RC_DISP_BLU_O = 1;
	portc.SIG_RC_DISP_GRN_O = 1;  // TURN OFF COLORS
	
	
	if(bitdata & 01000000b) {
		portb.SIG_RB_DISPLED1_O = 1;
	}
	else {
		portb.SIG_RB_DISPLED1_O = 0;
	}
	
	if(bitdata & 00100000b) {
		portb.SIG_RB_DISPLED2_O = 1;
	}
	else {
		portb.SIG_RB_DISPLED2_O = 0;
	}	
	
	if(bitdata & 00010000b) {
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

	if(bitdata & 00000100b) {
		portb.SIG_RB_DISPLED5_O = 1;
	}
	else {
		portb.SIG_RB_DISPLED5_O = 0;
	}	
	if(bitdata & 00000010b) {
		portc.SIG_RC_DISPLED6_O = 1;
	}
	else {
		portc.SIG_RC_DISPLED6_O = 0;
	}	
	if(bitdata & 00000001b) {
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
	 

}

volatile unsigned short povcycles = 0;

void led_pov_next(unsigned char color)
{
    static unsigned char autocolor;
    static unsigned char charcnt;	// Character in the string
    static unsigned char colcnt;	// Column in the character	

    
	if(charcnt >= NUMCHARS) {
		charcnt = 0;
		povcycles++;
		autocolor++;
		if (autocolor > LED_SHOW_BLU) {
		    autocolor = LED_SHOW_RED;
		}
	}
    led_showbin(color == LED_SHOW_AUTO ? autocolor : color, readcharmap(povstring[charcnt], colcnt));
    delay_10us(40);
    led_showbin(color == LED_SHOW_AUTO ? autocolor : color, 0);
    colcnt++;
    if(colcnt >= NUMCOL) {
		colcnt = 0;
		charcnt++;
		delay_10us(80);
	}
    
}

void led_pov(unsigned char color, unsigned short cycles)
{
	povcycles = 0;
	
	while (povcycles < cycles) {
		led_pov_next(color);
	}
}
	 
	  