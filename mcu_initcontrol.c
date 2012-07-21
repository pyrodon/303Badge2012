#include <system.h>
#include "Badge.h"

#define TIMER_PRESCALE_256	0x07
#define TIMER_PRESCALE_128  0x06
#define TIMER_PRESCALE_64  0x05
#define TIMER_PRESCALE_32  0x04
#define TIMER_PRESCALE_16  0x03
#define TIMER_PRESCALE_8  0x02
#define TIMER_PRESCALE_4  0x01
#define TIMER_PRESCALE_2  0x00


//#define TIMER_REGVAL ((unsigned char) -125)
#define TIMER_PRESCALE TIMER_PRESCALE_8 


void mcu_initialize()
{
	intcon.GIE=0;
	// 
	// Setup main oscillator. Main oscillator will run at 8MHz (internal) and will be scaled
	// to 32MHz by enabling the Phase Plocked Loop (PLL)
	//
	osccon = 0x72;  // Internal 8MHz oscillator (was 72)
	delay_ms(1);
	set_bit(osctune, PLLEN);  // Turn on (1) PLL for 4x speed
	
	
	//
	// Set up all pin directions and initial data vectors
	// Important to do first to avoid spurious activity on the board. 
	//
	trisa = TRISCONFIG_RA;	  
	porta = INITIALDATA_RA;

	trisb = TRISCONFIG_RB;	
	portb = INITIALDATA_RB;

	trisc = TRISCONFIG_RC;	
	portc = INITIALDATA_RC;

	trisd = TRISCONFIG_RD;	
	portd = INITIALDATA_RD;

	trise = TRISCONFIG_RE;	
	porte = INITIALDATA_RE;

	trisf = TRISCONFIG_RF;	
	portf = INITIALDATA_RF;

	trisg = TRISCONFIG_RG;	
	portg = INITIALDATA_RG;	
	

	
	// 
	// Set up the AD Conversion Hardware
	// 
	
	// adcon0 remains in powerup configuration. Internal Voltage References, Select Channel 0
	adcon0 = 0;
	adcon1 = 00101110b; // Set timing Parameters
	adcon1.ADFM = 1;    // Set results to be right justified in adresh and adresl
	
	wdtcon.ADSHR = 1;   // Select the shared bit to access the alternate setup registers
	ancon0 = 11011100b; // Set up AN0 and AN1 for analog input. Other configurable pins as digital
	ancon1 = 11111111b;
	wdtcon.ADSHR = 0;   // Turn off the shared register bit
	adcon0.ADON = 1;    // Turn the converter subsystem on
	
	adcon1.ADCAL = 1;	// Set up a self calibration cycle
	adcon0.GO_NOT_DONE = 1;  // Start Conversion
	while(adcon0.NOT_DONE);  // Wait for completion
	adcon1.ADCAL = 0;        // reset the calibration bit
	//adcon0 = (1<<ADON);      // Clear all bits except on bit
	pir1.ADIF = 0;

	
	
	//
	// Set up the second MSSP port as an I2C interface to the sound A/D Converter
	ssp2con1 = 0; // Default settings (reset condition, should not be needed)
	ssp2add = 19;  // Clock scaler - use 4 for 8 mhz oscillator and 19 for 32 MHz for 400KHz
	ssp2con1 = 0x08; // I2C Master mode, clock = FOSC/(4 * (SSPxADD + 1))
	ssp2con2 = 0;    // IDLE the I2C INterface (reset condition, should not be needed)
	ssp2stat = 0;    // Mostly read-only, but enable slew rate control for 400KHz coms
	// ssp2stat.SMP = 1; DIsable slew rate control if neede
	
	pir3.SSP2IF = 0;  // Clear the I2C Interrupt Flags
	pir3.BCL2IF = 0;  // Clear the buss collision interrupt flag
	

	
	ssp2con1.SSPEN = 1;  // Enable the I2C interface
	
	//
	// Set up 8KHz Interrupt
	//
	
	t0con = (1<<T08BIT)  | TIMER_PRESCALE ;	// 8 bit timer with prescale
	tmr0l = TIMER_REGVAL;
	t0con.TMR0ON = 1;

    intcon.TMR0IE = 1;		// Enable Timer Interrtups
	
}


	
	
	
	