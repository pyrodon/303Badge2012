////////////////////////////////////////////////////////////////////////////////////////
// The 303 Defcon Badge Project for 2012
// 
// This is a hardware and software platform that can be used to make a cool badge
// It includes lightshow, sound DAC, and chiptunes subsystem
// Alcohol sensor, and an RF communciation system. 
// The software includes the ability to access functions locally, and remotely given
// sufficient badge privilege, it also has an STD simulator that probabalistically
// causes badges to infect other badges through the beacon subsystem. 
////////////////////////////////////////////////////////////////////////////////////////
//
// Please see general credits and information in the main.c file
//
//  (c) 2012, Don Kark (Th3D0n)
//
////////////////////////////////////////////////////////////////////////////////////////
#include <system.h>
#include "Badge.h"
#include "ext_chiptunesong.h"

#define BC_GETLAST	1
#define BC_DONGS	2  
#define BC_RICKROLL	3
#define BC_NYAN		4
#define BC_JOURNEY	5
#define BC_CANTINA  6
#define BC_RAINING  7
#define BC_PEWPEW	8
#define BC_KLAXON 	9
#define BC_KRY0		10


#define BC_MAX		10

extern unsigned char lastrfsender;

void execute_shortcmd(unsigned char num)
{
	if((num == 0) || (num > BC_MAX)) {
		tune_startsong(SONG_BUZZER);
		return;
	}
	
	switch(num) {
	  case BC_GETLAST:
	    light_show(LIGHTSHOW_OFF, 1);
		light_showbin(lastrfsender & 0x7f, lastrfsender & 0x80 ? 7 : 0, lastrfsender & 0x80 ? 0 : 7, 0 );
		delay_s(3);
		modelights();
		break;
	  case BC_PEWPEW:
		tune_startsong(SONG_PEWPEW);
		break;
	  case BC_KLAXON:
	  	tune_startsong(SONG_KLAXON);
		break;
	  case BC_DONGS: 
		sample_play();
		break;
	  case BC_RICKROLL:
	  	tune_startsong(SONG_RICK);
		break;
	  case BC_NYAN:
	  	tune_startsong(SONG_NYAN);
		break;
	  case BC_JOURNEY:
	  	tune_startsong(SONG_JOURNEY1);
		break;
	  case BC_CANTINA:
	  	tune_startsong(SONG_CANTINA);
		break;
	  case BC_RAINING:
	  	tune_startsong(SONG_RAINING);
		break;
	  case BC_KRY0: 
	  	tune_startsong(SONG_KRY0);
		break;
	}
}


