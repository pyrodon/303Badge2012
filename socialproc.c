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

#define STD_BIT_CHLAM	0x01
#define STD_BIT_SYPH    0x02
#define STD_BIT_GHON	0x04
#define STD_BIT_HERP    0x08
#define STD_BIT_HEPC    0x10
#define STD_BIT_HIV     0x20

#define NUMSTD 6

#define RTOP 0xffff

#define PROB_CHLAM	 400
#define PROB_SYPH    800
#define PROB_GHON	 900
#define PROB_HERP    200
#define PROB_HEPC    1500
#define PROB_HIV     2500

#define BASE_CHLAM	 3000
#define BASE_SYPH    6000
#define BASE_GHON	 9000
#define BASE_HERP    12000
#define BASE_HEPC    15000
#define BASE_HIV     18000


static unsigned char stdmap[256];



#define STD_TREATABLE (STD_BIT_CHLAM|STD_BIT_SYPH|STD_BIT_GHON)

unsigned short synthrand;
extern unsigned long elapsed_msecs;

#ifdef SPEEDTEST
#define CURETIME (60 * 1000) // 60 secs
#else
#define CURETIME (3600 * 3 * 1000) // 3 hours
#endif

unsigned long clear_chlam = CURETIME;
unsigned long clear_syph = CURETIME;
unsigned long clear_ghon = CURETIME;

void std_proc_beacon(unsigned char vector, unsigned char snd)
{

    unsigned char mystd, newstd;
    unsigned char origstd;
    
    
	mystd = nvget_socvec1();
	stdmap[snd] = mystd;
	origstd = mystd;
	
	newstd = vector & ~mystd;

	
	
	
	
	synthrand = (unsigned short) rnd_randomize() * 
	  (unsigned short) (elapsed_msecs & 0xff); // 0-65K
	   
	   	
	if (newstd & STD_BIT_CHLAM) {
	    if (synthrand > BASE_CHLAM && synthrand < (BASE_CHLAM + (RTOP / PROB_CHLAM))) {
			mystd |= STD_BIT_CHLAM;
			clear_chlam = elapsed_msecs + CURETIME;
		}
	}
	
	synthrand = (unsigned short) rnd_randomize() * 
	  (unsigned short) (elapsed_msecs & 0xff); // 0-65K
	  
	if (newstd & STD_BIT_SYPH) {
	    if (synthrand > BASE_SYPH && synthrand < (BASE_SYPH + (RTOP / PROB_SYPH))) {
			mystd |= STD_BIT_SYPH;
			clear_syph = elapsed_msecs + CURETIME;
		}
	}	
	
	synthrand = (unsigned short) rnd_randomize() * 
	  (unsigned short) (elapsed_msecs & 0xff); // 0-65K
	  
	if (newstd & STD_BIT_GHON) {
	    if (synthrand > BASE_GHON && synthrand < (BASE_GHON + (RTOP / PROB_GHON))) {
			mystd |= STD_BIT_GHON;
			clear_ghon = elapsed_msecs + CURETIME;
		}
	}	
	
	synthrand = (unsigned short) rnd_randomize() * 
	  (unsigned short) (elapsed_msecs & 0xff); // 0-65K
	  		
	if (newstd & STD_BIT_HERP) {
	    if (synthrand > BASE_HERP && synthrand < (BASE_HERP + (RTOP / PROB_HERP))) {
			mystd |= STD_BIT_HERP;
		}
	}	   
	
	synthrand = (unsigned short) rnd_randomize() * 
	  (unsigned short) (elapsed_msecs & 0xff); // 0-65K
	  
	if (newstd & STD_BIT_HEPC ) {
	    if (synthrand > BASE_HEPC && synthrand < (BASE_HEPC + (RTOP / PROB_HEPC))) {
			mystd |= STD_BIT_HEPC ;
		}
	}	
	
	synthrand = (unsigned short) rnd_randomize() * 
	  (unsigned short) (elapsed_msecs & 0xff); // 0-65K
	  
	if (newstd & STD_BIT_HIV) {
	    if (synthrand > BASE_HIV && synthrand < (BASE_HIV + (RTOP / PROB_HIV))) {
			mystd |= STD_BIT_HIV;
		}
	}	
	
	
	if(elapsed_msecs > clear_chlam) {
		mystd &= ~STD_BIT_CHLAM;
	}
	if(elapsed_msecs > clear_syph) {
		mystd &= ~STD_BIT_SYPH;
	}
	if(elapsed_msecs > clear_ghon) {
		mystd &= ~STD_BIT_GHON;
	}
	
	if(mystd != origstd) {
		nvset_socvec1(mystd);
		nvsavebuf();
	}
}

#define SLUTLEV 4

unsigned char std_isaslut(unsigned char addr)
{
	unsigned char stdvec, i, stdcount;
	
	stdvec = stdmap[addr];
	stdcount = 0;
	
	for(i=0; i < NUMSTD; i++) {
		if(stdvec & 0x01) stdcount++;
		stdvec >>= 1;
	}
	if(stdcount >= SLUTLEV) {
		return(1);
	}
	else  {
		return(0);
	}
}

unsigned char std_imaslut()
{
	unsigned char stdvec, i, stdcount;
	
	stdvec = nvget_socvec1();
	stdcount = 0;
	
	for(i=0; i < NUMSTD; i++) {
		if(stdvec & 0x01) stdcount++;
		stdvec >>= 1;
	}
	if(stdcount >= SLUTLEV) {
		return(1);
	}
	else  {
		return(0);
	}
}