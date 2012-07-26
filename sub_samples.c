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

#include "Samples/d2.h"

static unsigned short sampoffset;
unsigned char playsample=0;

void sample_play()
{
    sampoffset = 0;
    
    sound_amp_on();
	sound_hsstart();
	
	playsample = 1;
	
	while(playsample);
	
	sound_hsstop();
	sound_amp_off();
}
    
    
void sample_intr()
{
	static unsigned char lastsample = 0x80;
	static unsigned long address;

	
	if(!playsample) return;
	
	if(sampoffset >= SAMPLE_LEN) {
		playsample = 0;
		return;
	}
	
	sound_val_hs(lastsample);   // Send the previously calculated sample to DAC
	
    address = SAMPLE_DATA + sampoffset++;
 
    asm 
    {
	MOVF _address + 2, W
	MOVWF _tblptru
	MOVF _address + 1, W
	MOVWF _tblptrh
	MOVF _address, W
	MOVWF _tblptrl
	TBLRD*
	MOVF _tablat, W
	MOVWF _lastsample		
    }
    
    return;

 }