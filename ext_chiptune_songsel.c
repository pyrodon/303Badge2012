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

#define CASESEL( ix ) case SONG_##ix##: \
                        SongSelAddr = SONGDATA_##ix##; \
                        SongSelMaxTrack = MAXTRACK_##ix##; \
                        SongSelSongLen = SONGLEN_##ix##; \
                        break 
                        
                        
                        

void songselect(unsigned char songix)
{
	switch(songix) {
	  CASESEL(SIREN);
	  CASESEL(SECRET);
	  CASESEL(RICK);
	  CASESEL(RAINING);
	  CASESEL(PEWPEW);
	  CASESEL(PACMAN);
	  CASESEL(ORIG);
	  CASESEL(NYAN);
	  CASESEL(KRY0);
	  CASESEL(KLAXON);
	  CASESEL(JOURNEY3);
	  CASESEL(JOURNEY2);
	  CASESEL(JOURNEY1);
	  CASESEL(DYING);
	  CASESEL(CHIRP2);
	  CASESEL(CHIRP1);
	  CASESEL(CANTINA);
	  CASESEL(CACTUS);
	  CASESEL(BUZZER);
	  CASESEL(303);
	  default:
	     SongSelAddr = SONGDATA_303;
	     SongSelMaxTrack = MAXTRACK_303;
	     SongSelSongLen = SONGLEN_303;
	     break;
	}
}