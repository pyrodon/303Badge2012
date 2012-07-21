#include <system.h>
#include "Badge.h"
#include "sub_rfcmd.h"
#include "MRF49XA.h"

unsigned char rfoutbuf[8];

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
	    break;
	}		
	
}
