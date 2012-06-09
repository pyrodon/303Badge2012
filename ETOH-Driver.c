#include <system.h>
#include "Badge.h"


void init_etoh()
{

	
	adcon1 = 00101110b; // Set timing Parameters
	adcon1.ADFM = 1;  // Turn on module, VDD-VSS Range, set to Channel 0
	
	wdtcon.ADSHR = 1;
	ancon0 = 11011100b; //
	ancon1 = 11111111b;
	wdtcon.ADSHR = 0;
	adcon0.ADON = 1; // Turn it on
	
	adcon1.ADCAL = 1;
	adcon0.GO_NOT_DONE = 1;
	while(adcon0.NOT_DONE);
	adcon1.ADCAL = 0;
	

}

unsigned short get_etoh()
{
	unsigned short result =0;
	
	adcon0.GO_NOT_DONE = 1;
	while(adcon0.NOT_DONE);
	
	result = adresh & 0x0f;
	result = result << 8;
	result |= adresl;
	
	return(result);
}
	
	
	