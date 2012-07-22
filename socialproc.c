#include <system.h>
#include "Badge.h"

#define STD_BIT_CHLAM	0x01
#define STD_BIT_SYPH    0x02
#define STD_BIT_GHON	0x04
#define STD_BIT_HERP    0x08
#define STD_BIT_HEPC    0x10
#define STD_BIT_HIV     0x20

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






#define STD_TREATABLE (STD_BIT_CHLAM|STD_BIT_SYPH|STD_BIT_GHON)

unsigned short synthrand;
extern unsigned long elapsed_msecs;

unsigned long clear_chlam = (3600 * 3 * 1000);
unsigned long clear_syph = (3600 * 3 * 1000);
unsigned long clear_ghon = (3600 * 3 * 1000);

void proc_beacon_std(unsigned char vector)
{

    unsigned char mystd, newstd;
    unsigned char origstd;
    
    
	mystd = nvget_socvec1();
	origstd = mystd;
	
	newstd = vector & ~mystd;

	
	
	
	
	synthrand = (unsigned short) rnd_randomize() * 
	  (unsigned short) (elapsed_msecs & 0xff); // 0-65K
	   
	   	
	if (newstd & STD_BIT_CHLAM) {
	    if (synthrand > BASE_CHLAM && synthrand < (BASE_CHLAM + (RTOP / PROB_CHLAM))) {
			mystd |= STD_BIT_CHLAM;
			clear_chlam = elapsed_msecs + (3600 * 3 * 1000);
		}
	}
	
	synthrand = (unsigned short) rnd_randomize() * 
	  (unsigned short) (elapsed_msecs & 0xff); // 0-65K
	  
	if (newstd & STD_BIT_SYPH) {
	    if (synthrand > BASE_SYPH && synthrand < (BASE_SYPH + (RTOP / PROB_SYPH))) {
			mystd |= STD_BIT_SYPH;
			clear_syph = elapsed_msecs + (3600 * 3 * 1000);
		}
	}	
	
	synthrand = (unsigned short) rnd_randomize() * 
	  (unsigned short) (elapsed_msecs & 0xff); // 0-65K
	  
	if (newstd & STD_BIT_GHON) {
	    if (synthrand > BASE_GHON && synthrand < (BASE_GHON + (RTOP / PROB_GHON))) {
			mystd |= STD_BIT_GHON;
			clear_ghon = elapsed_msecs + (3600 * 3 * 1000);
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