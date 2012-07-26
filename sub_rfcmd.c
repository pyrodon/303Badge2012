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
#include "MRF49XA.h"
#include "ext_chiptunesong.h"

unsigned char rfoutbuf[8];
static unsigned short curbeacon_density;	
static unsigned short lastbeacon_density;

extern unsigned char MyBadgeID;
extern unsigned char MyElev;
extern unsigned char MyMode;
extern unsigned long elapsed_msecs;

unsigned char lastrfsender;
unsigned char mysoc;

void rfcmd_3send(unsigned char cmd, unsigned char parm1, unsigned char parm2)
{

	if(nvget_badgeperm() & NBPRMDHAC) {
		cmd |= 0x80;
	}
	if((nvget_badgeperm() & NBPRMELEV) || MyElev) {
		cmd |= 0x40;
	}
	rfoutbuf[0] = cmd;
	rfoutbuf[1] = parm1; 
	rfoutbuf[2] = parm2;
	
	
	MRF49XA_Send_Packet(rfoutbuf, 3);
	MRF49XA_Reset_Radio();
	
}

extern void std_proc_beacon(unsigned char, unsigned char );
extern unsigned char std_imaslut();


extern unsigned long clear_chlam ;
extern unsigned long clear_syph ;
extern unsigned long clear_ghon;

void rfcmd_execute(unsigned char *rbuf, unsigned char rlen)
{
	unsigned char cmd, crc, sender, rx_socvec;
	
	if(rlen < 3) {
	   return;
	}
	
	cmd = rbuf[0];
	sender = rbuf[1];
	rx_socvec = rbuf[2];
	
	
	crc = cmd & 0xc0;
	cmd = cmd^crc;
	
	
	if(cmd != RFCMD_BEACON) {
	
		if(!(crc&0x80)) {
			if(rx_socvec != MyBadgeID) {
				return;
			}
		} else {
			if((rx_socvec != MyBadgeID) && (rx_socvec != 0)) {
				return;
			}
		}
		if((crc == 0) && (std_imaslut() == 0)) {
			return;
		}
	}
	
	if(cmd != RFCMD_BEACON) {
			lastrfsender = sender;
	}
	
	
	
	
	switch (cmd) {
	  case RFCMD_BEACON:
	  
	    std_proc_beacon(rx_socvec, sender);
	    curbeacon_density++;
	    break;
	  case RFCMD_PLAYDONG: // Dong
	    sample_play();
	    break;
	  case RFCMD_PLAYRICK:  // Rick Roll
	    songselect(SONG_RICK);
	    goto songcommon;
	  case RFCMD_PLAYNYAN:  // Nyan Song
	    songselect(SONG_NYAN);
	    light_show(LIGHTSHOW_RAINBOW, 5);
	    goto songcommon;
	  case RFCMD_PLAYRAIN:  // Slayer Raining
	    songselect(SONG_RAINING);
	    goto songcommon;
	  case RFCMD_PLAYJOUR:  // Journey Don't Stop Believing (main)
	    if(!(crc & 0xc0)) break;
	    songselect(SONG_JOURNEY1);
	    goto songcommon;
	  case RFCMD_PLAYCANT:  // Star Wars Cantina
	    songselect(SONG_CANTINA);
	    goto songcommon;
	  case RFCMD_PLAYSECR:  // Song Secret
	    songselect(SONG_SECRET);
	    goto songcommon;
	  case RFCMD_PLAYKRY0:  // Long Ass Chiptune Annoyance Song 
	    if(!(crc & 0xc0)) break;
	    songselect(SONG_KRY0);
	    goto songcommon;	
	  case RFCMD_PLAYPAC:   // Pacman Sound
	    songselect(SONG_PACMAN);
	    goto songcommon;
	  case RFCMD_PLAYPEW:   // Pew Pew
	    songselect(SONG_PEWPEW);
	    goto songcommon;
	  case RFCMD_PLAYKLAX:  // Klaxon
	    songselect(SONG_KLAXON);
	    goto songcommon;
	  case RFCMD_PLAYSIREN: // Siren
	    songselect(SONG_SIREN);	    
songcommon:
        tune_startsong(0xff);	// Start song with previously selected items. 
        break;
	
	  case RFCMD_ADMNSOC0:   // Set Social vector bit (**)
	  case RFCMD_ADMNSOC1:   // Set Social vector bit (**)
	  case RFCMD_ADMNSOC2:   // Set Social vector bit (**)
	  case RFCMD_ADMNSOC3:   // Set Social vector bit (**)
	  case RFCMD_ADMNSOC4:   // Set Social vector bit (**)
	  case RFCMD_ADMNSOC5:   // Set Social vector bit (**)
	    if((crc & 0x80) == 0) break;
	    
		mysoc = nvget_socvec1();
		mysoc |= (1<< (cmd & 0x07));
		if(cmd & 0x01) clear_chlam = elapsed_msecs + (3600*1000);
		if(cmd & 0x02) clear_syph = elapsed_msecs + (3600*1000);
		if(cmd & 0x03) clear_ghon = elapsed_msecs + (3600*1000);
		nvset_socvec1(mysoc);
		nvsavebuf();
	    break;
	  case RFCMD_ADMNSOCC:   // Clear Social Vector Bit
	    if((crc & 0x80) == 0) break;
	    nvset_socvec1(0);	// Cured
	    nvsavebuf();
	    break;
	  case RFCMD_ESET:  // Send Special (**)
	    if((crc & 0x80) == 0) break;
	    mysoc = nvget_badgeperm();
	    mysoc |= NBPRMELEV;
	    nvset_badgeperm(mysoc);
	    nvsavebuf();
	    break;
	  case RFCMD_ECLR:  // Send Special (**)
	    if((crc & 0x80) == 0) break;
	    mysoc = nvget_badgeperm();
	    mysoc &= ~NBPRMELEV;
	    nvset_badgeperm(mysoc);
	    nvsavebuf();
	    break;
	  case RFCMD_DON1:      // Special Reserved (**)
		  foo();
	    break;
	  case RFCMD_ATTEN:     // Special Alert (**)
	    if(!(crc & 0x80)) break;
	      MyMode = MODE_ATTEN;
	      modelights();
	    break;
	  case RFCMD_PERF1:     // Special Performance (**)
	    if(!(crc & 0x80)) break;
	    if((MyBadgeID % 3) == 0) {
			tune_startsong(SONG_JOURNEY1);
		}
		else if((MyBadgeID % 3) == 1) {
			tune_startsong(SONG_JOURNEY2);
		}
		else {
			tune_startsong(SONG_JOURNEY3);
		}

	} // switch cmd		
	
	if(cmd != RFCMD_BEACON) {
			lastrfsender = sender;
	}
	
}


void rfcmd_clrcden()
{
	lastbeacon_density = curbeacon_density; 
	curbeacon_density = 0; 
}

unsigned short rfcmd_getdensity()
{
	return(lastbeacon_density);
}

unsigned char rfcmd_lastsender()
{
	return(lastrfsender);
}