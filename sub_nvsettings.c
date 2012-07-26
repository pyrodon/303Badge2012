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
