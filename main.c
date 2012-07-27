////////////////////////////////////////////////////////////////////////////////////////
// The 303 Defcon Badge Project for 2012
// 
// This is a hardware and software platform that can be used to make a cool badge
// It includes lightshow, sound DAC, and chiptunes subsystem
// Alcohol sensor, and an RF communciation system. 
// The software includes the ability to access functions locally, and remotely given
// sufficient badge privilege, it also has an STD simulator that probabalistically
// causes badges to infect other badges through the beacon subsystem. 
//
// Native Compiler Used: SourceBoost BoostC (www.sourceboost.com)
//
////////////////////////////////////////////////////////////////////////////////////////
//
// Credits:
//    Badge Concept: Don Kark (Th3D0n) and Mar Williams
//	  Hardware Design: Don Kark
//    Software: Don Kark
//    Case Concept and Design: Mar Williams and Trev Jackson
//    Chiptunes Composer/Arranger: Steve Pordon
//
//    Badge Production: Mar Williams, Fred Roybal, Sockwell, and numerous other 303
//        and DenHac Supporters.
//
//  Thank you to our primary sponsors who provided support to the project:
//     Dark Tangent
//     Kryus
// 
//  As well as other key sponsors and supporters:
//    moose
//    Peak Security
//    y3t1
//    Shaggy
//    FAWCR
//    Shapewerks
//
//  Finally acknowledgement for code that was adapted for use in this project
//   Microchip MRF49XA Reference Code
//   "Hardware Chiptune Project" code release by Linus Akeson (Kry0). Used by permission. 
//      http://www.linusakesson.net/hardware/chiptune.php
//
//  (c) 2012, Don Kark (Th3D0n)
//
//////////////////////////////////////////////////////////////////////////////////////

#include <system.h>
#include "BadgePicConfig.h"
#include "Badge.h"
#include "MRF49XA.h" 
#include "ext_chiptunesong.h"
#include "sub_rfcmd.h"

//#define SPEEDTEST

#ifdef SPEEDTEST			// Speeds up some of the user experience parameters for testing
#define TIME_ADDR 2
#define TIME_POV  10
#else
#define TIME_ADDR 4
#define TIME_POV  300
#endif


#define ELAPSED_SECS() (elapsed_msecs >> 10) // Not exact second but more efficient than /1000

unsigned long elapsed_msecs = 0;			// Total elapsed time in seconds
unsigned char loop_msecs = 0;		        //

volatile unsigned char intr_intfreq;		// 1-up counter every time through intr routine
volatile unsigned char intr_msecs;

unsigned char MyBadgeID;
volatile unsigned char MyMode;
unsigned char MyElev;

unsigned char rfrxbuf[PAYLOAD_MAX];

unsigned char rfrxlen;

unsigned char have_quorum;	

#ifdef SPEEDTEST
#define BEACON_BASE (5*1000)		// 5 second beacons
#define DENSITY_QUORUM_HI	3
#define DENSITY_QUORUM_LO   1 
#define ELEVATE_BASE (5 * 60 * 1000)
#define ELEVATE_VAR  1
#define ELEVATE_DUR  30000
#else
#define BEACON_BASE (2*60*1000)	// 2 minute beacons
#define DENSITY_QUORUM_HI 20		// 20 people in range
#define DENSITY_QUORUM_LO 12		// 12 people in range
#define ELEVATE_BASE (2 * 60 * 60 * 1000)   // Privilege elevation about every 2 hours
#define ELEVATE_VAR  20000					// + up to 255 * 20 secs
#define ELEVATE_DUR  (5*60*1000)			// And will last 5 minutes
#endif
#define BEACON_RNDSCALE 8		// Random bias multiplier 8*255 = 2-3 secs (introduce some jitter)
								// So beacons get out of each other's way if simultaneous
unsigned long last_beacon = 0;

#define RXBUFLEN	PAYLOAD_MAX	// Based on Microchip sample code. 

unsigned char lastmode;			

unsigned long elevatemsecs;		// How long we've been elevated
unsigned long attelapsed;		// How long we've been in "attention" mode. 



#define DELAYMS	20 


void main()
{
	unsigned char i;
	unsigned char savemode;
	

	//
	// Core Hardware Initialization
	

	
	mcu_initialize();	// Initialize MCU resources and 8KHz interrupt...
	
	nvreadbuf();		// Load the NV buffer from flash (get badge Addr and properties)
	proc_btn1();		// Process Buttons
	
	sound_config_polled(); 		// Configure the sound subsystem chip
	




	etoh_init();			// Initialize the Alcohol Sensor Subsystem
	MRF49XA_Init();			// Initialize the RF tranceiver
	usb_ser_init(); 		// Initialize the serial port
	light_init();
	
	

	
	
	//
	// Preliminary setup and interaction
	// These setup a precurser items will occur prior to going into the main worker loop. 
	// This is mainly the one-time powerup and "boot" sequence
	//

    // Show the address of the badge on the LEDs (since there are only 7 LED's
    // the binary number will show in green if the badge is < 128 and red if >  
    // 128 with the badge addresses 0 and 128 showing up as nothing (typically illegal 
    // numbers anyway
    
    // Note, led_showbin does not interact with the interrupt driven light show well. 
    // So only use if you may not be in interrupt mode yet of if a show is not running
    delay_s(1);
    led_showbin(LED_SHOW_RED, nvget_badgetype() | 0x40); // Startup Indicator
    delay_10us(100);
    led_showbin(LED_SHOW_RED, 0);
    delay_s(1);
    led_showbin(LED_SHOW_RED, nvget_badgeperm() | 0x40); // Same
    delay_10us(100);
    led_showbin(LED_SHOW_RED, 0);
    delay_s(1);
    
    
    // 
    // Shows address as described above
    //
    MyBadgeID = nvget_badgeid();
    led_showbin(MyBadgeID & 0x80 ? LED_SHOW_RED : LED_SHOW_GRN, MyBadgeID & 0x7f );
    delay_s(TIME_ADDR);
    led_showbin(LED_SHOW_NONE, 0);
    
    init_rnd(nvget_badgetype()+1, MyBadgeID, 0x55); // Seed tha random number generator
    
    
    //
    // Perform POV credits (spin the badge on your wrist :)
    //
    led_pov(LED_SHOW_AUTO, TIME_POV);
    
    // Enable Global Interrupts
	// Use Interrupts. ISR is interrupt(), below	
	intcon.GIE=1;
    




	//sample_play();
	
	//delay_s(2);
	
	
	MyMode = MODE_IDLE;
	
	
	modelights();  // Sets lightshow based on a few state parameters

	
	// Calculate initial elevation time
	elevatemsecs = elapsed_msecs + ELEVATE_BASE + (unsigned long) rnd_randomize() * ELEVATE_VAR;

	
	
	// 
	// Main Worker Loop
	//	
	while(1) {
		//
		// Handle the elapsed time clocks
		//
		clear_bit(intcon, TMR0IE);
		loop_msecs = intr_msecs; // Take copy to avoid race conditions
		intr_msecs = 0;
		set_bit(intcon, TMR0IE);
		elapsed_msecs += loop_msecs; // Update elapsed time in msecs
		
		
		savemode = MyMode;	// Used to detact mode changes when command processers set new mode
		switch(MyMode) {
		  case MODE_IDLE:
		    // If button pressed, handle it
		    if(btn_commandwork(loop_msecs) == BTN_WORKING) {
				MyMode = MODE_GETCMD;
				modelights();
				playsong = 0;
				//light_pause();
				break;
		    }
		    // Look for incoming RF packet
		    if(MRF49XA_Receive_Packet(rfrxbuf,&rfrxlen) == PACKET_RECEIVED) {
				MRF49XA_Reset_Radio();
				//led_showbin(LED_SHOW_RED, 2);
				//delay_10us(10);
				//led_showbin(LED_SHOW_RED, 0);
				rfcmd_execute(rfrxbuf, rfrxlen);
		        
			}
			if(savemode != MyMode) {	// Don't process remainder of idle switch if state change
				break;
			}
			// If time to send a beacon, send it. Also process quorum sounds. 
			if(elapsed_msecs > (last_beacon + (unsigned long)BEACON_BASE +  ((unsigned long)rnd_randomize()* BEACON_RNDSCALE))) { // time for beacon
				rfcmd_3send(RFCMD_BEACON, MyBadgeID, nvget_socvec1());

				last_beacon = elapsed_msecs;
				rfcmd_clrcden();
				if(have_quorum) {
					if(rfcmd_getdensity() <= DENSITY_QUORUM_LO) {
						have_quorum = 0;
						 if(!playsong) tune_startsong(SONG_CHIRP2);
					}
				}
				else {
					if(rfcmd_getdensity() >= DENSITY_QUORUM_HI) {
						have_quorum = 1;
						if (!playsong) tune_startsong(SONG_303);
					}
				}
				modelights();
			}
			break;
		case MODE_ETOH:  // Run the alcohol sensor state machine
		  if(etoh_breathtest(ETOH_DOWORK,  loop_msecs ) == ETOH_DONE) {  // worker for ETOH
			switch(etoh_getreward()) {
			  case REWARD_SOBER:
			    tune_startsong(SONG_BUZZER);
			    break;
			  case REWARD_TIPSY: 
			    tune_startsong(SONG_PEWPEW);
			    break;
			  case REWARD_DRUNK:
			    tune_startsong(SONG_CACTUS);
			    break;
			    
			 }
			 MyMode = MODE_IDLE;
		 
			 light_show(LIGHTSHOW_SOCFLASH, 5);
			 break;	 
	      }
	      break;
	    case MODE_ATTEN:
	        // Puts all badges into visual attention mode to see who's in range
	        if(MyMode != lastmode) {
				attelapsed  = 0;
			}
			attelapsed += loop_msecs;
			if(attelapsed > 120000) {	// Stays there 2 min (should be in #def)
				MyMode = MODE_IDLE;
			}
			
			if(MRF49XA_Receive_Packet(rfrxbuf,&rfrxlen) == PACKET_RECEIVED) {
				MRF49XA_Reset_Radio();
			
				rfcmd_execute(rfrxbuf, rfrxlen);
		        
			}
			modelights();
	      break;
	    case MODE_GETCMD:
	      if (btn_commandwork( loop_msecs) != BTN_WORKING) {
			MyMode = MODE_IDLE;
			modelights();
		  }
	      break;	     
		    
		} // switch
		  
		tune_songwork();					// Worker thread for songs				 
		light_animate(loop_msecs);			// worker thread for lights
		
		if(MyElev) {	// Handle the privilege elevation timer
			if(elapsed_msecs > (elevatemsecs + ELEVATE_DUR)) {
			    MyElev = 0;
			    elevatemsecs = elapsed_msecs + ELEVATE_BASE + (unsigned long) rnd_randomize() * ELEVATE_VAR;
			    modelights();
			}
		} else {
			if(elapsed_msecs > elevatemsecs) {
				MyElev = 1;
				sample_play();
				modelights();
			}
		}

				
		lastmode = MyMode;
			
		
		
									 
		
	}	
	
    	
}

static unsigned char soundval = 0;

void interrupt( void ) 
{

	if(intcon.TMR0IF) {  // 8 KHz driven interrupt events
	    intcon.TMR0IE = 0;

		tmr0l = TIMER_REGVAL;
		intr_intfreq++;
		if(intr_intfreq >= 8) {  // Convert HZ rate to msecs
			intr_intfreq = 0;
			intr_msecs++;		// Rolling msec counter (wraps every 65K seconds)
		}
		
		
		if (playsong) {		// Call respective intr drivers. We don't have enough 
							// Time for all at 8KHz, so this enforces a priority
		   tune_play_intr();
		} else if(playsample) {
			sample_intr();
		} else  {
			light_intr();
		}
		

	} else {						// CALL FOR HELP -- UNKNOWN INTERRUPT (DEBUG STUFF)
		portc.SIG_RC_DISP_GRN_O = 0;
		portb.SIG_RB_DISPLED3_O = 1;
		portb.SIG_RB_DISPLED4_O = 1;
		portb.SIG_RB_DISPLED5_O = 1;
	}
	intcon.TMR0IF = 0;	
	intcon.TMR0IE = 1;

}
	


void modelights()  // Call whenever MyMode, TmpPrivs changes, density
{
	
	
	switch(MyMode) {
	  case MODE_IDLE:
		switch(nvget_badgetype()) {
		  case NVBT303:
		  case NVBTHAC:
			if(MyElev) {
				light_show(LIGHTSHOW_DONGS, 2);
			}
			else {
				if(have_quorum) {
					light_show(LIGHTSHOW_SOCFLASH, 1);
				}
				else {
					light_show(LIGHTSHOW_SOCFLASH, 3);				
				}
			}
			break;
		  case NVBTSKYGRUNT:
			light_show(LIGHTSHOW_SKYGRUNT, 5);
			break;
		  case NVBTSKYENFORCER:
			light_show(LIGHTSHOW_SKYENFORCER, 3);
			break;
		  case NVBTSKYSPEAKER:
			light_show(LIGHTSHOW_SKYSPEAKER, 5);
			break;
		 } // switch nvget_badgetype
		 break;
	  case MODE_ATTEN:
	    light_show(LIGHTSHOW_RAINBOW, 5);
	    break;
	  case MODE_ETOH:
	    light_show(LIGHTSHOW_OFF, 5);
	    break;
	  case MODE_GETCMD:
	    light_show(LIGHTSHOW_OFF, 5);
	    break;
	}  // Switch mymode
}
			 


	
