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

unsigned char lastrfsender;

void rfcmd_3send(unsigned char cmd, unsigned char parm1, unsigned char parm2)
{

	if(nvget_badgeperm() & NBPRMROOT) {
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

extern void proc_beacon_std(unsigned char );

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
	}
	
	switch (cmd) {
	  case RFCMD_BEACON:
	    //sound_beep_polled();
	     proc_beacon_std(rx_socvec);
	    curbeacon_density++;
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
	    if(!(crc & 0x80)) break;
	    songselect(SONG_JOURNEY1);
	    goto songcommon;
	  case RFCMD_PLAYCANT:  // Star Wars Cantina
	    songselect(SONG_CANTINA);
	    goto songcommon;
	  case RFCMD_PLAYSECR:  // Song Secret
	    songselect(SONG_SECRET);
	    goto songcommon;
	  case RFCMD_PLAYKRY0:  // Long Ass Chiptune Annoyance Song 
	    if(!(crc & 0x80)) break;
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
	
	  case RFCMD_ADMNSOC:   // Set Social vector bit (**)
	    break;
	  case RFCMD_ADMNSPEC:  // Send Special (**)
	    break;
	  case RFCMD_DON1:      // Special Reserved (**)
	    break;
	  case RFCMD_ATTEN:     // Special Alert (**)
	    if(!(crc & 0x80)) break;
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
	
	if((cmd != RFCMD_BEACON) && (cmd < RFCMD_ADMNSOC)) {
	    lastrfsender = sender;
	}
	else {
		lastrfsender = 0;
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