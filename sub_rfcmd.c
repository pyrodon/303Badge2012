#include <system.h>
#include "Badge.h"
#include "sub_rfcmd.h"
#include "MRF49XA.h"

unsigned char rfoutbuf[8];
static unsigned short curbeacon_density;	
static unsigned short lastbeacon_density;

void rfcmd_3send(unsigned char cmd, unsigned char parm1, unsigned char parm2)
{
	rfoutbuf[0] = cmd;
	rfoutbuf[1] = parm1; 
	rfoutbuf[2] = parm2;
	
	MRF49XA_Send_Packet(rfoutbuf, 3);
	MRF49XA_Reset_Radio();
	
}


 void rfcmd_execute(unsigned char *rbuf, unsigned char rlen)
{

	switch (rbuf[0]) {
	  case RFCMD_BEACON:
	    sound_beep_polled();
	    curbeacon_density++;
	    break;
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