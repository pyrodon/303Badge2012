#include <system.h>
#include "Badge.h"

#define SIG_RF_FUNC_N_I			1	// Main Function Button			IN
#define SIG_RF_AUXBUT_N_I		2	// Auxiliary Control Button		IN
#define	SIG_RF_INIT_N_I			3	// Initialization Pads on Back	IN

void proc_btn1()
{
	unsigned char i;
	
	if(portf.SIG_RF_INIT_N_I == 0) {  // We are doing an initial perm configuration
	    if(portf.SIG_RF_FUNC_N_I == 0) {
			nvset_badgeperm(NBPRMELEV);
			sound_beep_polled();
			delay_ms(10);
			if(portf.SIG_RF_AUXBUT_N_I == 0) {
				nvset_badgeperm(NBPRMROOT);
				sound_beep_polled();
			}
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
		
			