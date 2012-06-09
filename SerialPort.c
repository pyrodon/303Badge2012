#include <system.h>
#include "Badge.h"

#define SER_9600 	51 // BRGH=0; BRG16=0  0.16% error


void usb_ser_init()
{
	portg.SIG_RG_USBRSET_N_O = 1; // Release reset if set
	delay_ms(1); 
	
	spbrg2 = SER_9600;
	txsta2 = (1<<TXEN);
	rcsta2 = (1<<SPEN) | (1<<CREN);
	
	portg.SIG_RTS_N_O = 0; // Set Ready to Send Low
	
}

void usb_putchar(unsigned char c)
{
	while(txsta2.TRMT == 0);  // If a previous character is still transmitting. 
	txreg2 = c;
}


unsigned char usb_getchar(char *ch)
{
	if(test_bit(rcsta2, FERR) || test_bit(rcsta2, OERR)) { // throw away errors for now.
		clear_bit(rcsta2, CREN);
		set_bit(rcsta2, CREN);
		return(2);
		//rxerror++;  // Keep a reference count of errors
	}
	
	if(test_bit(pir3, RC2IF)) {
		*ch = rcreg2;
		clear_bit(pir3, RC2IF);
		return(0);
    }
    else {
		return (1);
	}
}
	