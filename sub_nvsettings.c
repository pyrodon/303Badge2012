#include <system.h>
#include <flash.h>
#include "Badge.h"


unsigned char nvolbuf[64];


#define NVBADGEID	0x00		// 8 bit badge address 

#define NVBPERM		0x10		// Badge control permission
#define NVBTYPE		0x11		// Badget Type
#define SOCVEC1		0x12		// Social Vector 1





#define NVBT303		0x00		// 303 Badge
#define NVBTHAC		0x01		// Denhac Badge
#define NVBTSKY		0x02		// Skytalks Badge	


#define NVPRMUSER		0x00   // Plain old user
#define NBPRMELEV		0x01   // Elevated User
#define NBPRMROOT		0x02   // Master User (Root)

#define NVBASE		0x1ff00		// Base of the 64 byte configuration



void nvreadbuf()
{	
	flash_read(NVBASE, nvolbuf);
}

void nvsavebuf()
{
	flash_erase(NVBASE);
	flash_write(NVBASE, nvolbuf);

}

unsigned char nvget_badgeid()
{
	return(nvolbuf[NVBADGEID]);
}

void nvset_badgeid(unsigned char num)
{
	nvolbuf[NVBADGEID] = num;
}

unsigned char nvget_badgetype()
{
	return(nvolbuf[NVBTYPE]);
}

void nvset_badgetype(unsigned char num)
{
	nvolbuf[NVBTYPE] = num;
}

unsigned char nvget_badgeperm()
{
	return(nvolbuf[NVBPERM]);
}

void nvset_badgeperm(unsigned char num)
{
	nvolbuf[NVBPERM] = num;
}

unsigned char nvget_socvec1()
{
	return(nvolbuf[SOCVEC1]);
}

void nvset_socvec1(unsigned char num)
{
	nvolbuf[SOCVEC1] = num;
}
