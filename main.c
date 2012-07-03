#include <system.h>
#include "BadgePicConfig.h"
#include "Badge.h"
#include "MRF49XA.h" 


unsigned long elapsed_msecs = 0;			// Total elapsed time in seconds
unsigned char loop_msecs = 0;		        //

volatile unsigned char intr_intfreq;		// 1-up counter every time through intr routine
volatile unsigned char intr_msecs;


#define DELAYMS	20 

void main()
{
	unsigned char i, j, intensity, retchar;
	unsigned char loopcnt = 0, etoh_first = 1;
	unsigned short soundcount;
	unsigned short etoh_start, etoh_end, etoh_diff;
	unsigned short avg_accum = 0, etoh_lastdiff;
	unsigned long autocountdown = 0x20000;
	
	unsigned char rfrxdata=1, rfrxlen=1, rftxdata=1; 
	
	unsigned char cur_msecs; 

	//
	// Core Hardware Initialization
	

	
	mcu_initialize();	// Initialize MCU resources and 8KHz interrupt...
	
	
	sound_config_polled(); 		// Configure the sound subsystem chip
	
	// Enable Global Interrupts
	set_bit(intcon, GIE);	// Use Interrupts. ISR is interrupt(), below


	
	//init_etoh();
	//porta.SIG_RA_ETOH_HTR_N_O = 1; // Turn off Heater

	
	
	MRF49XA_Init();
	usb_ser_init();
	
	// 
	// Main Worker Loop
	//


	
	light_show(LIGHTSHOW_RAINBOW, 3);
	
	while(1) {
		//
		// Handle the elapsed time clocks
		//
		clear_bit(intcon, TMR0IE);
		loop_msecs = intr_msecs; // Take copy to avoid race conditions
		intr_msecs = 0;
		set_bit(intcon, TMR0IE);
		
		elapsed_msecs += loop_msecs; // May remove.. Ideally pass the current loop msecs into 
									 // state machine so they can do their own timing, but keep for now
									 
		light_animate(loop_msecs);
									 
		
	}	
	
	
	
	//
	// Testing Light Show
	//
	


	for(i=0; i < NUMLIGHTS; i++) {
		light_set(i, i, NUMLIGHTS-1-i, 0);
	}
	while(1);
	


	      
	   	   

	


    
//	tune_init();
//	tune_playsong();
	
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
	if(test_bit(intcon, TMR0IF)) {  // 8 KHz driven interrupt events
		clear_bit(intcon, TMR0IE);
		tmr0l = TIMER_REGVAL;
		intr_intfreq++;
		if(intr_intfreq == 8) {  // Convert HZ rate to msecs
			intr_intfreq = 0;
			intr_msecs++;		// Rolling msec counter (wraps every 65K seconds)
		}
		light_intr();
		tune_play_intr();
		clear_bit(intcon,TMR0IF);
		set_bit(intcon, TMR0IE);
	}	
}
	

	
