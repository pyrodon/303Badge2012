#include <system.h>
#include <flash.h>
#include "Badge.h"

unsigned char nvolbuf[64];

#define NVBASE		0x1ac00	// Base of the 64 byte configuration
#define NVBASE2		(NVBASE+64)


#define NVBADGEID_IX	0x00		// 8 bit badge address 
#define NVBPERM_IX		0x02		// Badge control permission
#define NVBTYPE_IX		0x01		// Badget Type
#define SOCVEC1_IX		0x03		// Social Vector 1

#pragma DATA NVBASE,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
void nvreadbuf()
{	
	unsigned char i;
	flash_read((unsigned long)NVBASE, nvolbuf);
	//if(nvolbuf[1] = 0xff) {
	//	for(i=1;i<64;i++){
	//		nvolbuf[i] = 0;
	//	}
	//}
	
}

void nvsavebuf()
{
	flash_erase((unsigned long)NVBASE);
	
	delay_100us(2);
	flash_write((unsigned long)NVBASE, nvolbuf);
	
	delay_100us(2);
	if(flash_verify((unsigned long)NVBASE, nvolbuf)) {
		led_showbin(LED_SHOW_GRN, 0x08);
	}
	else {
		led_showbin(LED_SHOW_RED, 0x08);
	}
	delay_100us(10);

}

unsigned char nvget_badgeid()
{
	return(nvolbuf[NVBADGEID_IX]);
}

void nvset_badgeid(unsigned char num)
{
	nvolbuf[NVBADGEID_IX] = num;
}

unsigned char nvget_badgetype()
{
	return(nvolbuf[NVBTYPE_IX]);
}

void nvset_badgetype(unsigned char num)
{
	nvolbuf[NVBTYPE_IX] = num;
}

unsigned char nvget_badgeperm()
{
	return(nvolbuf[NVBPERM_IX]);
}

void nvset_badgeperm(unsigned char num)
{
	nvolbuf[NVBPERM_IX] = num;
}

unsigned char nvget_socvec1()
{
	return(nvolbuf[SOCVEC1_IX]);
}

void nvset_socvec1(unsigned char num)
{
	nvolbuf[SOCVEC1_IX] = num;
}
