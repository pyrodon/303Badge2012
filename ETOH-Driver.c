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
	    //light_set(0,4,0,0); // Turn on red light to indicate wait
		//light_set(1,4,0,0); // Turn on red light to indicate wait
		//light_set(2,4,0,0); // Turn on red light to indicate wait
		//light_set(3,4,0,0); // Turn on red light to indicate wait
	  
	    etoh_heater_on(); // Turn on heater
	    mstate = 2; 	  // Advance to next stage
	    break;
	  case 2:
	    if(elapsed < 7000) return(ETOH_WORKING); // Wait 9.5 seconds for sensor to warm up
	    //light_set(3,0,0,0); // Turn on red light to indicate wait
	    //etoh_heater_off();
	    measure4 = etoh_get();
	    //etoh_heater_on();
	    light_showbin((unsigned char) (etoh_get() >> 0) & 0x7f, 3, 0, 0);
	    mstate = 3;
	    break;
	  case 3:
	    if(elapsed < 8000) return(ETOH_WORKING); // Wait 9.5 seconds for sensor to warm up
	   // light_set(2,0,0,0); // Turn on red light to indicate wait
	    //etoh_heater_off();
	    measure4 += etoh_get();
	    //etoh_heater_on();
	    light_showbin((unsigned char) (etoh_get() >> 0) & 0x7f, 3, 0, 0);
	    mstate = 4;
	    break;	    
	  case 4:
	    if(elapsed < 9000) return(ETOH_WORKING); // Wait 9.5 seconds for sensor to warm up
	    //light_set(1,0,0,0); // Turn on red light to indicate wait
	    //etoh_heater_off();
	    measure4 += etoh_get();
	    //etoh_heater_on();
	    light_showbin((unsigned char) (etoh_get() >> 0) & 0x7f, 3, 0, 0);
	    mstate = 5;
	    break;	 
	  case 5:
	    if(elapsed < 10000) return(ETOH_WORKING); // Wait 9.5 seconds for sensor to warm up
	    //etoh_heater_off();
	    measure4 += etoh_get();
	    //etoh_heater_on();
	    light_showbin((unsigned char) (etoh_get() >> 0) & 0x7f, 3, 0, 0);
	    measure4 >>= 2;			// Take 4 readings and divide by 4
	    etoh_baseline_val = measure4;  // Save - not sure if gonna use
	    
	    // Calculate Resistance and 125ppm Threshold
	    //etoh_r2_resistance = ((unsigned long) ETOH_R1 * (unsigned long) measure4) / 
	    //  ( (unsigned long) 1024 - (unsigned long) measure4 );
	      
	    //etoh_r2_resistance = (etoh_r2_resistance * 2) / 3 ;  // Sensor is defined to have 1/3 resistance at 125ppm
	    //etoh_125ppm_threshold_val = (unsigned short) (((long) 1024 * etoh_r2_resistance) / 
	    //  (etoh_r2_resistance + (long) ETOH_R1));
	    etoh_125ppm_threshold_val = etoh_baseline_val - 4;
	    //etoh_mid_val = (etoh_baseline_val + etoh_125ppm_threshold_val) >> 1;
	    etoh_mid_val = etoh_baseline_val - 2;
	    
	    //light_init();
	    light_set(0,0,4,0);    // Green light
	    light_set(1,0,4,0);    // Green light
	    light_set(2,0,4,0);    // Green light
		light_set(3,0,4,0);    // Green light

	    
	    mstate = 10;
	    lastelapsed = elapsed;
	    break;	 	
	   case 10: 
	     if(elapsed > 17000) {
			mstate = 20;
			break;
		 }
		 //etoh_heater_off();
		 measureblow = etoh_get();
		 //etoh_heater_on();
		 light_showbin((unsigned char) (measureblow >> 0) & 0x7f, 0, 3, 0);
		 if((measureblow < etoh_125ppm_threshold_val) && (etoh_reward < REWARD_DRUNK)) {
			etoh_reward = REWARD_DRUNK;
			//light_set(2,4,0,0);
			mstate = 20;
		 }
		 else if((measureblow < etoh_mid_val) && (etoh_reward < REWARD_TIPSY)) {
			etoh_reward = REWARD_TIPSY;
			//light_set(6,4,4,0);
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
	  
	