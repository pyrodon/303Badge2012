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
#include "Badge.h"

#define etoh_heater_on() porta.SIG_RA_ETOH_HTR_N_O = 0 // Turn on Heater
#define etoh_heater_off() porta.SIG_RA_ETOH_HTR_N_O = 1 // Turn off Heater

void etoh_init()
{

	adcon0 = 0;			// VSS/VCC References.. Channel 0
	
	adcon1 = 00101110b; // Set timing Parameters
	adcon1.ADFM = 1;    // Right justify the result
	
	
	wdtcon.ADSHR = 1;	// Select secondary register addressing bit
	ancon0 = 11011100b; // Select which channels will be ADC analog
	ancon1 = 11111111b;
	wdtcon.ADSHR = 0;   // Reset secondary address bit
	
	adcon0.ADON = 1; // Turn the converter on
	delay_10us(50);  // Allow to settle
	
	adcon1.ADCAL = 1;		// Do a calibration pass
	adcon0.GO_NOT_DONE = 1;
	while(adcon0.NOT_DONE);
	adcon1.ADCAL = 0;		// Calibration done, reset
	pir1.ADIF = 0;
	

}

unsigned short etoh_get()
{
	unsigned short result;
	
	pir1.ADIF = 0;
	adcon0.GO_NOT_DONE = 1;
	while(adcon0.NOT_DONE);
	
	
	result = adresh;			// Grab the high 2 bits
	result = result << 8;		// Shift right
	result |= adresl;			// Or in the LSB
	pir1.ADIF = 0;
	
	return(result);
}

#define ETOH_R1	68000	// Value of the upper leg of the bridge

static unsigned long etoh_r2_resistance;
static unsigned short etoh_125ppm_threshold_val; 
static unsigned short etoh_baseline_val; 
static unsigned short etoh_mid_val; 

#define ETOH_HEATER_TIME	10000 // in msecs

static unsigned char etoh_reward;




unsigned char  etoh_breathtest(unsigned char startkill, unsigned short msecs)
{
	static unsigned char mstate;  // Machine State
	static unsigned short elapsed, lastelapsed;
	static unsigned short measure4, measureblow;

	
	elapsed += msecs;		// Increment elapsed time in milliseconds
	
	if(startkill == ETOH_KILL) {
		etoh_heater_off();
		mstate = 0;
		return(ETOH_DONE);
	}
	else if (startkill == ETOH_START) {
		etoh_heater_on();
		elapsed = 0;
		mstate = 1;
		etoh_reward = REWARD_SOBER; 

		
	}

	
	switch(mstate) {
	  case 0: // IDLE STATE
	    return (ETOH_IDLE);
	    break;
	  case 1: // Starting Process
	    light_init();	// Turn off lights
	    light_set(0,4,0,0); // Turn on red light to indicate wait
		light_set(1,4,0,0); // Turn on red light to indicate wait
		light_set(2,4,0,0); // Turn on red light to indicate wait
		light_set(3,4,0,0); // Turn on red light to indicate wait
	  
	    etoh_heater_on(); // Turn on heater
	    mstate = 2; 	  // Advance to next stage
	    break;
	  case 2:
	    if(elapsed < 7000) return(ETOH_WORKING); // Wait 9.5 seconds for sensor to warm up
	    light_set(3,0,0,0); // Turn of  red light to indicate coutdown
	   
	    measure4 = etoh_get();
	   
	    mstate = 3;
	    break;
	  case 3:
	    if(elapsed < 8000) return(ETOH_WORKING); // Wait 9.5 seconds for sensor to warm up
	    light_set(2,0,0,0); // Turn of  red light to indicate coutdown
	 
	    measure4 += etoh_get();
	
	    mstate = 4;
	    break;	    
	  case 4:
	    if(elapsed < 9000) return(ETOH_WORKING); // Wait 9.5 seconds for sensor to warm up
	    light_set(1,0,0,0); //Turn of  red light to indicate coutdown
	    
	    measure4 += etoh_get();
	  
	    mstate = 5;
	    break;	 
	  case 5:
	    if(elapsed < 10000) return(ETOH_WORKING); // Wait 9.5 seconds for sensor to warm up
	  
	    measure4 += etoh_get();
	
	    measure4 >>= 2;			// Take 4 readings and divide by 4
	    etoh_baseline_val = measure4;  // Save - not sure if gonna use
	    
	  
	 
	    etoh_125ppm_threshold_val = etoh_baseline_val - 20;
	    
	    etoh_mid_val = etoh_baseline_val - 15;
	    
	    //light_init();
	    //light_set(0,0,4,0);    // Green light
	    //light_set(1,0,4,0);    // Green light
	    //light_set(2,0,4,0);    // Green light
		//light_set(3,0,4,0);    // Green light

	    
	    mstate = 10;
	    lastelapsed = elapsed;
	    break;	 	
	   case 10: 
	     if(elapsed > 17000) {
			mstate = 20;
			break;
		 }
		
		 measureblow = etoh_get();
		
		 light_showbin((unsigned char) measureblow & 0x7f, 0, 3, 0);
		 if((measureblow < etoh_125ppm_threshold_val) && (etoh_reward < REWARD_DRUNK)) {
			etoh_reward = REWARD_DRUNK;
			
			mstate = 20;
		 }
		 else if((measureblow < etoh_mid_val) && (etoh_reward < REWARD_TIPSY)) {
			etoh_reward = REWARD_TIPSY;
			
		 }
		 break;
		 
	   case 20:
	     etoh_heater_off();
		 mstate = 0;
		 light_init();
		 return(ETOH_DONE);
	}
	
	return(ETOH_WORKING);
}
	  
unsigned char etoh_getreward()
{
	return(etoh_reward);
}
	  
	