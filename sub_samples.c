#include <system.h>
#include "Badge.h"

#include "Samples/d2.h"

static unsigned short sampoffset;
volatile unsigned char playsample=0;

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