#include <system.h>
#include "BadgePicConfig.h"
#include "Badge.h"
#include "MRF49XA.h" 
#include "ext_chiptunesong.h"

#define SPEEDTEST

#ifdef SPEEDTEST			// Speeds up some of the user experience parameters for testing
#define TIME_ADDR 1
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
unsigned char MyMode;

#define MODE_IDLE 		0	// Idle mode (no buttons pressed) listening for RF
#define MODE_GETCMD 	1	// In (button) command mode. Suspend Other stuff
#define MODE_ETOH		2	// Getting ETOH readings, do not interrupt
#define MODE_EXEC		3	// Executing Transient Command
#define MODE_ATTEN		4	// Attention mode.. Suspend outgoing RF and Buttons
#define MODE_PRICMD		5	// Executing priority command


#define DELAYMS	20 

void main()
{
	unsigned char i, j, intensity, retchar;
	unsigned char loopcnt = 0, etoh_first = 1;
	unsigned short soundcount;

	unsigned long autocountdown = 0x20000;
	
	unsigned char rfrxdata=1, rfrxlen=1, rftxdata=1; 
	
	unsigned char cur_msecs; 

	//
	// Core Hardware Initialization
	

	
	mcu_initialize();	// Initialize MCU resources and 8KHz interrupt...
	
	nvreadbuf();		// Load the NV buffer from flash (get badge Addr and properties)
	
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
    MyBadgeID = nvget_badgeid();
    led_showbin(MyBadgeID & 0x80 ? LED_SHOW_RED : LED_SHOW_GRN, MyBadgeID & 0x7f );
    delay_s(TIME_ADDR);
    led_showbin(LED_SHOW_NONE, 0);
    
    
    //
    // Perform POV credits
    //
    led_pov(LED_SHOW_AUTO, TIME_POV);
    
    // Enable Global Interrupts
	// Use Interrupts. ISR is interrupt(), below	
	intcon.GIE=1;
    




	//tune_startsong(SONG_NYAN);
	//light_show(LIGHTSHOW_RAINBOW, 5);
	
	
	etoh_breathtest(ETOH_START, 0 );
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
		
		//switch(MyMode) {
		//}
		  
		tune_songwork();							 
		light_animate(loop_msecs);
		
		if(etoh_breathtest(ETOH_DOWORK,  loop_msecs ) == ETOH_DONE) {
			switch(etoh_getreward()) {
			  case REWARD_SOBER:
			    tune_startsong(SONG_BUZZER);
			    break;
			  case REWARD_TIPSY: 
			    tune_startsong(SONG_DYING);
			    break;
			  case REWARD_DRUNK:
			    tune_startsong(SONG_CACTUS);
			    break;
			 }
	    }
			
		
		// led_showbin(LED_SHOW_BLU, (unsigned char)(ELAPSED_SECS() & 0x7f));
									 
		
	}	
	

	
    while(1) {
       led_pov_next(LED_SHOW_AUTO);
       //autocountdown--;
       

	   //usb_putchar('A');
	   
	   //if(usb_getchar(&retchar) == 0) {
	  //     for(soundcount=0; soundcount < 1000; soundcount++) {
		//		sound_val_polled(soundcount&0x01 ? 80 : 0);
	//	   }
	  // }

       
       if ( !autocountdown || !portf.SIG_RF_FUNC_N_I || (MRF49XA_Receive_Packet(&rfrxdata,&rfrxlen) == PACKET_RECEIVED)) {
			// usb_putchar('B');
			led_showbin(0,0);
			autocountdown = 0x20000;
            MRF49XA_Reset_Radio();
		    rfrxlen = 1;
		    if ( !portf.SIG_RF_FUNC_N_I ) {
		        MRF49XA_Send_Packet(&rftxdata, 1);
		        MRF49XA_Reset_Radio();
		    }
		    
           	sound_config_polled();
			tune_init();
			tune_playsong();
			sound_config_polled();
	   }
	   
	   if(!portf.SIG_RF_AUXBUT_N_I) {
#ifdef NEVER
			led_showbin(0,0);
			portb.SIG_RB_DISPLED3_O = 1;
			portc.SIG_RC_DISP_RED_O = 0; // Red light for 10 seconds to allow sensor to head
			
			porta.SIG_RA_ETOH_HTR_N_O = 0;  // Turn on Heater
			
			delay_s(10);
			
			portc.SIG_RC_DISP_RED_O = 1; // Red light for 10 seconds to allow sensor to head
			portc.SIG_RC_DISP_GRN_O = 0; // Grean light means play
		
		    etoh_start = get_etoh();
		    
		    for(soundcount=0; soundcount < 8000; soundcount++) {
				sound_val_polled(soundcount&0x01 ? 80 : 0);
					etoh_end = get_etoh();
					if(etoh_start > etoh_end) {
						etoh_diff = etoh_start - etoh_end;
					}
					else {
						//etoh_diff = etoh_end - etoh_start;
						etoh_diff = 0;
						etoh_start = etoh_end;
					}
					if(etoh_first) {
						avg_accum = etoh_diff << 3;
						etoh_first = 0;
					}
					else {
						avg_accum -= etoh_lastdiff;
						avg_accum += etoh_diff;
					}
					etoh_lastdiff = etoh_diff;
					
					etoh_diff = avg_accum<<2; // >> 3;
					// etoh_diff >>=2;
					if (etoh_diff > 254) { etoh_diff = 254; }
					delay_10us(255 - (unsigned char)(etoh_diff & 0xff) );
				
			}
			
			porta.SIG_RA_ETOH_HTR_N_O = 1; // Turn off Heater
			portc.SIG_RC_DISP_GRN_O = 1;
			portb.SIG_RB_DISPLED3_O = 0;
#endif
		}
    }
	
	while(1) { 

		FLASHLED;
		delay_ms(200);
		if(!portf.SIG_RF_FUNC_N_I) {
			while(!portf.SIG_RF_FUNC_N_I);
			sound_config_polled();
			tune_init();
			tune_playsong();
		}
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
		
		
		if (playsong) {
		   tune_play_intr();
		} else if(playsample) {
			sample_intr();
		} else  {
			light_intr();
		}
		

	} else {						// CALL FOR HELP -- UNKNOWN INTERRUPT
		portc.SIG_RC_DISP_GRN_O = 0;
		portb.SIG_RB_DISPLED3_O = 1;
		portb.SIG_RB_DISPLED4_O = 1;
		portb.SIG_RB_DISPLED5_O = 1;
	}
	intcon.TMR0IF = 0;	
	intcon.TMR0IE = 1;

}
	

	
