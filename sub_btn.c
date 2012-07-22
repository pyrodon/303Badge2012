#include <system.h>
#include "Badge.h"
#include "sub_rfcmd.h"

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
#ifdef ROOTBADGE
			if(portf.SIG_RF_AUXBUT_N_I == 0) {
				nvset_badgeperm(NBPRMROOT);
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


#define BT_UP 0
#define BT_DN 1


#define STAGE_IDLE		0
#define STAGE_INIT		1
#define STAGE_CMD_DEST	2
#define STAGE_CMD_CMD	3



unsigned char fst, ast;
unsigned char machstate = STAGE_IDLE;

unsigned short btn_funcdowntime;
unsigned short btn_auxdowntime;



unsigned char btn_commandwork( unsigned char msecs)
{
#ifdef NEVER
	if(portf.SIG_RF_FUNC_N_I == 0) {
		fst = BT_DN;
	} else {
		fst = BT_UP;
	}
	if(portf.SIG_RF_AUXBUT_N_I == 0) {
		ast = BT_DN;
	}
	else {
	    ast = BT_UP)
	}
	
	switch(machstate) {
	  case STAGE_IDLE:
		if(!ast && !bst) {
			break; 				// No Buttons pressed, nothing to do
		
		if(fst) {
		    btn_funcdowntime = 0;  Reset counter
		}
			
	  
	  
	  
#endif
}

		
			