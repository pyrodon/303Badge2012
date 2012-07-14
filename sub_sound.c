#include <system.h>
#include "Badge.h"

#define DAC_I2C_ADDRESS	0x60		// Default I2C address for 7406 Chip
#define DAC_CONFIG 10000000b		// Set to write DAC config bits; no powerr down, VDD ref
#define DAC_CMD_VALOUT	0x00		// Command to set DAC output value (volatile)

static void sound_wait_poll();
static void sound_wait_pollint();

void sound_i2c_reset()
{
//
	// Generate start condition and wait
	//

	
	pir3.SSP2IF = 0;   	// Clear Interrupt and Collision Indicators
	pir3.BCL2IF = 0;
	
	
	sound_wait_poll();   // Wait till OK to send command
	
	ssp2con2.SEN = 1;		// Send Start Condition
	
	sound_wait_poll();	// Wait until start condition finished
	


	
	pir3.SSP2IF = 0; 	// Clear Interrupt and Collision Indicators
	pir3.BCL2IF = 0;
	
	
	ssp2buf = 0xff ;		// Send "reset"  device address
 
	sound_wait_poll();  // Wait Until Complete

	pir3.SSP2IF = 0;	// Clear Interrupt and Collision Indicators
	pir3.BCL2IF = 0;	
	
	ssp2con2.RSEN = 1;  // Restart Condition
	sound_wait_poll(); // Wait Until Complete
	
	pir3.SSP2IF = 0;	// Clear Interrupt and Collision Indicators
	pir3.BCL2IF = 0;	
	
	ssp2con2.PEN = 1;  // Send Stop Condition 
	sound_wait_poll();
	
	// Turn Amp On
	//delay_ms(10);
	//sound_amp_on();
	//delay_ms(10);

}


void sound_config_polled()
{
	//
	// Generate start condition and wait
	//

	
	pir3.SSP2IF = 0;   	// Clear Interrupt and Collision Indicators
	pir3.BCL2IF = 0;
	
	
	sound_wait_poll();   // Wait till OK to send command
	
	ssp2con2.SEN = 1;		// Send Start Condition
	
	sound_wait_poll();	// Wait until start condition finished
	


	
	pir3.SSP2IF = 0; 	// Clear Interrupt and Collision Indicators
	pir3.BCL2IF = 0;
	
	
	ssp2buf = DAC_I2C_ADDRESS<<1 ;		// Send DAC device address
 
	sound_wait_poll();  // Wait Until Complete

	pir3.SSP2IF = 0;	// Clear Interrupt and Collision Indicators
	pir3.BCL2IF = 0;	
	
	ssp2buf = DAC_CONFIG;			// Send Configuration Command
	sound_wait_poll(); // Wait Until Complete
	
	pir3.SSP2IF = 0;	// Clear Interrupt and Collision Indicators
	pir3.BCL2IF = 0;	
	
	ssp2con2.PEN = 1;  // Send Stop Condition 
	sound_wait_poll();
	
	// Turn Amp On
	//delay_ms(10);
	//sound_amp_on();
	//delay_ms(10);

	
}

static void sound_wait_poll()
{
	while ((ssp2con2 & 0x17) || ssp2stat.READ_WRITE ); // Wait Until Complete
}
static void sound_wait_pollint()
{
	while ((ssp2con2 & 0x17) || ssp2stat.READ_WRITE ); // Wait Until Complete
}

void sound_val_polled(unsigned char dacval)
{
	//
	// Generate start condition and wait
	//

	
	sound_wait_poll();		// Wait for all-clear to send
	pir3.SSP2IF = 0;		// Clear Interrupt and Collision FLags
	pir3.BCL2IF = 0;
	
	ssp2con2.SEN = 1;		// Send Start Condition
	sound_wait_poll();		// Wait to complete

	
	pir3.SSP2IF = 0;				// Clear Interrupt and Collision FLags
	pir3.BCL2IF = 0;
	ssp2buf = DAC_I2C_ADDRESS<<1;	// Send device address
	sound_wait_poll();				// Wait to complete
	
	pir3.SSP2IF = 0;			// Clear Interrupt and Collision FLags
	pir3.BCL2IF = 0;	
	ssp2buf = DAC_CMD_VALOUT;	// Set to write DAC; no powerr down
	sound_wait_poll();			// Wait to complete
	
	pir3.SSP2IF = 0;		// Clear Interrupt and Collision FLags
	pir3.BCL2IF = 0;		
	ssp2buf = dacval;		// Set the DAC value
	sound_wait_poll();		// Wait to complete
	
	
	pir3.SSP2IF = 0;		// Clear Interrupt and Collision FLags
	pir3.BCL2IF = 0;		
	ssp2con2.PEN = 1;  		// Send Stop Condition Stop 
	sound_wait_poll();		// Wait to complete
	
}
	
void sound_hsstart()
{
	//
	// Generate start condition and wait
	//

	
	sound_wait_poll();			// Wait for all-clear to send
	pir3.SSP2IF = 0;				// Clear Interrupt and Collision FLags
	pir3.BCL2IF = 0;
	
	ssp2con2.SEN = 1;				// Send Start Condition
	sound_wait_poll();			    // Wait to complete


	pir3.SSP2IF = 0;				// Clear Interrupt and Collision FLags
	pir3.BCL2IF = 0;
	ssp2buf = 0x08;					// Set HS enable
	sound_wait_poll();			    // Wait to complete (does not ack they HS command)

	ssp2stat.SMP = 1; 				// Disable  slew rate control
	ssp2add = 5;	  	       		// Set to HS mode (set BRG to higher speed) (8MHz/(val) + 1) (was 3)
	pir3.SSP2IF = 0;				// Clear Interrupt and Collision FLags
	pir3.BCL2IF = 0;
	
	ssp2con2.RSEN = 1;				// Send Repeated Start Condition in HS mode 
	sound_wait_poll();			    // Wait to complete


	
	pir3.SSP2IF = 0;				// Clear Interrupt and Collision FLags
	pir3.BCL2IF = 0;
	ssp2buf = DAC_I2C_ADDRESS<<1;	// Send device address
	sound_wait_poll();			    // Wait to complete
	
	
}
	
void sound_val_hs(unsigned char dacval)
{
	//
	// Generate start condition and wait
	//

	
	sound_wait_pollint();		// Wait for all-clear to send
	
	
	pir3.SSP2IF = 0;			// Clear Interrupt and Collision FLags
	pir3.BCL2IF = 0;	
	ssp2buf = DAC_CMD_VALOUT;	// Set to write DAC; no powerr down
	sound_wait_pollint();			// Wait to complete
	
	pir3.SSP2IF = 0;		// Clear Interrupt and Collision FLags
	pir3.BCL2IF = 0;		
	ssp2buf = dacval;		// Set the DAC value
	
}

void sound_hsstop()
{
	//
	// Generate start condition and wait
	//

	
	sound_wait_poll();		// Wait for all-clear to send
	
	
	pir3.SSP2IF = 0;		// Clear Interrupt and Collision FLags
	pir3.BCL2IF = 0;		
	ssp2con2.PEN = 1;  		// Send Stop Condition Stop 
	sound_wait_poll();		// Wait to complete

	ssp2stat.SMP = 0; //Enable slew rate control
	ssp2add = 19; // Reset to Normal FS I2C rate
	

	
}
	
	
	
	