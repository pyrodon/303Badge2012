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


#include "SongData/303.h"
#include "SongData/buzzer.h"
#include "SongData/cactus.h"
#include "SongData/cantina.h"
#include "SongData/chirp1.h"
#include "SongData/chirp2.h"
#include "SongData/dying.h"
#include "SongData/journey1.h"
#include "SongData/journey2.h"
#include "SongData/journey3.h"
#include "SongData/klaxon.h"
#include "SongData/kry0.h"
#include "SongData/nyan.h"
#include "SongData/orig.h"
#include "SongData/pacman.h"
#include "SongData/pewpew.h"
#include "SongData/raining.h"
#include "SongData/ricktroll.h"
#include "SongData/secret.h"
#include "SongData/siren.h"

unsigned long SongSelAddr;
unsigned char SongSelMaxTrack;
unsigned char SongSelSongLen;
unsigned char SongSelRateVal;
unsigned char SongSelXtraVol;

#define CASESEL( ix , r, v ) case SONG_##ix##: \
                        SongSelAddr = SONGDATA_##ix##; \
                        SongSelMaxTrack = MAXTRACK_##ix##; \
                        SongSelSongLen = SONGLEN_##ix##; \
                        SongSelRateVal = (r) ; \
                        SongSelXtraVol = (v); \
                        break 
                        
                        
                        

void songselect(unsigned char songix)
{
    playsong = 0;  // terminate any other playing song
    
	switch(songix) {
	  CASESEL(SIREN, 93, 7);
	  CASESEL(SECRET, 93, 7);
	  CASESEL(RICK, 93, 7);
	  CASESEL(RAINING, 93, 8);
	  CASESEL(PEWPEW, 93, 7);
	  CASESEL(PACMAN, 93, 7);
	  CASESEL(ORIG, 103, 8);
	  CASESEL(NYAN, 95, 8);
	  CASESEL(KRY0, 93, 8);
	  CASESEL(KLAXON, 93, 7);
	  CASESEL(JOURNEY3, 93, 8);
	  CASESEL(JOURNEY2, 93, 8);
	  CASESEL(JOURNEY1, 93, 8);
	  CASESEL(DYING, 93, 7);
	  CASESEL(CHIRP2, 93, 6);
	  CASESEL(CHIRP1, 93, 6);
	  CASESEL(CANTINA, 93, 8);
	  CASESEL(CACTUS, 95, 7);
	  CASESEL(BUZZER, 93, 7);
	  CASESEL(303, 93, 7);
	  default:
	     SongSelAddr = SONGDATA_303;
	     SongSelMaxTrack = MAXTRACK_303;
	     SongSelSongLen = SONGLEN_303;
		 SongSelRateVal = 93 ; 
		SongSelXtraVol = 7; 
	     break;
	}
}