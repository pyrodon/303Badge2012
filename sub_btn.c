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
#include "sub_rfcmd.h"

extern unsigned char MyMode;
extern unsigned char MyBadgeID;


void proc_btn1() 
{
	unsigned char i;
	
	if(portf.SIG_RF_INIT_N_I == 0) {  // We are doing an initial perm configuration
	    if(portf.SIG_RF_FUNC_N_I == 0) {
			nvset_badgeperm(NBPRMELEV);
			sound_beep_polled();
			delay_ms(10);
#ifdef ROOTBADGE
			if(portf.SIG_RF_AUXBUT_N_I == 0) {
				nvset_badgeperm(NBPRMDHAC);
				sound_beep_polled();
			}
#endif
		}
	} else if ((portf.SIG_RF_FUNC_N_I == 0) && (portf.SIG_RF_AUXBUT_N_I == 0)) {
		nvset_badgetype(NVBT303);
		sound_beep_polled();
		delay_ms(10);
	} else 	if(portf.SIG_RF_FUNC_N_I == 0) {
		nvset_badgetype(NVBTSKYGRUNT);
		sound_beep_polled();
		delay_ms(10);
	    while(portf.SIG_RF_FUNC_N_I == 0) {
	        if(portf.SIG_RF_AUXBUT_N_I == 0) {
	            while(portf.SIG_RF_AUXBUT_N_I == 0);
	            nvset_badgetype(nvget_badgetype() + 1);
	            sound_beep_polled();
				delay_ms(10);
	        }
	    }
		
	} else if(portf.SIG_RF_AUXBUT_N_I == 0) {
		nvset_badgetype(NVBTHAC);
		sound_beep_polled();
	} else {
		return;
	}
	nvsavebuf();
	
}

#define DEBOUNCE 10 // 10 ms
#define LONGPRESS 2500
#define TERMINATEGAP 1200
#define XTERMGAP	 3000

#define BT_INACTIVE 	0
#define BT_ACTIVE	 	1

#define EV_NONE	0
#define EV_PRESSED 1
#define EV_RELEASED 2



#define MENU_IDLE			0
#define MENU_SHORTFUNC		1
#define MENU_XCOMADDR		2
#define MENU_XCOMCMD		3
#define MENU_XCOMEXEC		4
#define MENU_ETOH			5


unsigned char fbutton_laststate = BT_INACTIVE;
unsigned char fbutton_newstate;
unsigned short fbutton_timeactive;
unsigned short fbutton_timeinactive;
unsigned char  fbutton_event;

unsigned char abutton_laststate = BT_INACTIVE;
unsigned char abutton_newstate;
unsigned short abutton_timeactive;
unsigned short abutton_timeinactive;
unsigned char  abutton_event;

unsigned short menu_timer;
unsigned char  menustate;
unsigned char longbeep;

unsigned char funcpulses;	// Counts menu clicks
extern void execute_shortcmd(unsigned char );

unsigned char destbadge, destcmd, countbits;
unsigned short mininactive;


unsigned char btn_commandwork( unsigned char msecs)
{
	fbutton_event = EV_NONE;
	abutton_event = EV_NONE;

	if(fbutton_laststate == BT_ACTIVE) {
		if(fbutton_timeactive < 60000) {
			fbutton_timeactive += msecs;
		}
	}
	else {
		if(fbutton_timeinactive < 60000) {
			fbutton_timeinactive += msecs;
		}
	}	
	if(portf.SIG_RF_FUNC_N_I == 0) {
		if((fbutton_laststate == BT_INACTIVE) && (fbutton_timeinactive > DEBOUNCE)) {
			fbutton_newstate = BT_ACTIVE;
			fbutton_timeactive = 0;
			fbutton_event = EV_PRESSED;
			fbutton_laststate = fbutton_newstate;
		}
	} else {
		if((fbutton_laststate == BT_ACTIVE) && (fbutton_timeactive > DEBOUNCE)) {
			fbutton_newstate = BT_INACTIVE;
			fbutton_timeinactive = 0;
			fbutton_event = EV_RELEASED;
			fbutton_laststate = fbutton_newstate;
		}
	}

	if(abutton_laststate == BT_ACTIVE) {
		if(abutton_timeactive < 60000) {
			abutton_timeactive += msecs;
		}
	}
	else {
		if(abutton_timeinactive < 60000) {
			abutton_timeinactive += msecs;
		}
	}	
	if(portf.SIG_RF_AUXBUT_N_I == 0) {
		if((abutton_laststate == BT_INACTIVE) && (abutton_timeinactive > DEBOUNCE)) {
			abutton_newstate = BT_ACTIVE;
			abutton_timeactive = 0;
			abutton_event = EV_PRESSED;
			abutton_laststate = abutton_newstate;
		}
	} else {
		if((abutton_laststate == BT_ACTIVE) && (abutton_timeactive > DEBOUNCE)) {
			abutton_newstate = BT_INACTIVE;
			abutton_timeinactive = 0;
			abutton_event = EV_RELEASED;
			abutton_laststate = abutton_newstate;
		}
	}		
	
	menu_timer += msecs;
	
	switch(menustate) {
	  case MENU_IDLE: 
	    if(fbutton_newstate == BT_ACTIVE) {
			if(fbutton_timeactive >= LONGPRESS) {
				if(longbeep == 0) {
					sound_beep_polled();
					longbeep = 1;
				}
			}
		}
		else {
			longbeep = 0;
		}
				    
		if(fbutton_event == EV_RELEASED) {
			if(fbutton_timeactive < LONGPRESS) {
				menustate = MENU_SHORTFUNC;
				menu_timer = 0;
				funcpulses = 1;
			}
			else {
				menustate = MENU_XCOMADDR;
				menu_timer = 0;
				destbadge = destcmd = countbits = 0;
		    }
		} 
		else if(abutton_event == EV_RELEASED) {
				menustate = MENU_ETOH;
				//light_show(LIGHTSHOW_OFF, 1);
				menu_timer = 0;
			
		}
		return(BTN_IDLE);
		break;
	  case MENU_SHORTFUNC:
	    if(menu_timer > TERMINATEGAP) {
	        execute_shortcmd(funcpulses);
	        menustate = MENU_IDLE;
	        MyMode = MODE_IDLE;
	        //light_play();
	        return(BTN_DONE);
	    }
	    if(fbutton_event == EV_RELEASED) {
			menu_timer = 0;
			funcpulses++;
		}
		break;
	 case MENU_ETOH:
		MyMode = MODE_ETOH;
		modelights();
		//light_pause();
		etoh_breathtest(ETOH_START, 0);
		menustate = MENU_IDLE;
		return(BTN_DONE);
		break;
     case MENU_XCOMADDR:
       //
       // Gather desitnation address
       //
       mininactive = abutton_timeinactive < fbutton_timeinactive ? 
         abutton_timeinactive : fbutton_timeinactive;
       if(mininactive > XTERMGAP) { // Terminate this segment for inactivity
		  menustate = MENU_XCOMCMD;
		  sound_beep_polled();
		  menu_timer = 0;
		  countbits = 0;
		  fbutton_timeinactive = 0;
		  abutton_timeinactive = 0;
		  
		  break;
	   }
	   if(fbutton_event == EV_RELEASED) {
	    destbadge <<= 1;
		destbadge |= 0x01;
		menu_timer = 0;
		countbits++;
	   } else if(abutton_event == EV_RELEASED) {
	    destbadge <<= 1;
	    	
		menu_timer = 0;
		countbits++;
	   }
	   led_showbin(destbadge & 0x80 ? LED_SHOW_RED : LED_SHOW_GRN, destbadge & 0x7f );
	   if(countbits == 8) {
	   	  menustate = MENU_XCOMCMD;
	   	  countbits = 0;
	   	  menu_timer = 0;
		  sound_beep_polled();
	   }
	   break;
     case MENU_XCOMCMD:
       //
       // Gather desitnation address
       //
       mininactive = abutton_timeinactive < fbutton_timeinactive ? 
         abutton_timeinactive : fbutton_timeinactive;
       if(mininactive > XTERMGAP) { // Terminate this segment for inactivity
		  menustate = MENU_XCOMEXEC;
		  sound_beep_polled();
		  menu_timer = 0;
		  fbutton_timeinactive = 0;
		  abutton_timeinactive = 0;
		  break;
	   }
	   if(fbutton_event == EV_RELEASED) {
	    destcmd <<= 1;
		destcmd |= 0x01;
		menu_timer = 0;
		countbits++;
	   } else if(abutton_event == EV_RELEASED) {
	    destcmd <<= 1;
	    	
		menu_timer = 0;
		countbits++;
	   }
	   led_showbin(destcmd & 0x80 ? LED_SHOW_RED : LED_SHOW_GRN, destcmd & 0x7f );
	   if(countbits == 8) {
	   	  menustate = MENU_XCOMEXEC;
		  sound_beep_polled();
		  menu_timer = 0;
	   }
	   break;   
	  case MENU_XCOMEXEC:
	    rfcmd_3send(destcmd, MyBadgeID, destbadge);
	    MyMode = MODE_IDLE;
		modelights();
		//light_play();
		menustate = MENU_IDLE;
		return(BTN_DONE);
		break;
	    
	}
	return(BTN_WORKING);
	 
	
}

		
			