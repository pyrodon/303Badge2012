#include <system.h>
#include "Badge.h"

//
// Global Defs Included in BADGE.h
//
// extern void light_init();
// extern void light_set(unsigned char, unsigned char, unsigned char, unsigned char);
// extern void light_pause();
// extern void light_play(); 
// extern void light_intr(); 
// extern void light_show(unsigned char , unsigned char );
// extern void light_animate(unsigned char );
// #define NUMLIGHTS 7
// #define MAXLIGHTPHASE 23
// #define NUMINTENSITY  8

// #define LIGHTSHOW_OFF	0
// #define LIGHTSHOW_CEYLON 1
// #define LIGHTSHOW_RGWAVE 2
// #define LIGHTSHOW_COUNTDOWN 3
// #define LIGHTSHOW_RAINBOW 4


rom unsigned char rlightmap[] = { 0, 1, 2, 3, 4, 5, 6, 7 }; // NUMINTENSITY COUNT
rom unsigned char glightmap[] = { 0, 4, 6, 8, 12, 15, 17, 22 }; // NUMINTENSITY COUNT
rom unsigned char blightmap[] = { 0, 2, 5, 6, 8, 11, 15, 20 }; // NUMINTENSITY COUNT



//
// This array holds the phase setting info for the individual lights.. 
// There are 7 LEDs addressed 0-7 from top of board to bottom
// An interrupt routing will process one light (R, G, and B) in sequence. 
// With each interrupt pass a counter will be incremented. If the interrupt counter
// Is greater than the stored phase for that light, the light will be turned off. This
// duty cycle approach for each color will let us get a broad spectrum of intensity and
// color. The interrupt processing can be paused if needed to support fidelity of 
// chiptunes, etc. 
//


static unsigned char r_phase[NUMLIGHTS], g_phase[NUMLIGHTS], b_phase[NUMLIGHTS];

//
// Initializes the light array to "off"
//
void light_init()
{
	unsigned char x;
	for(x=0; x< NUMLIGHTS; x++) {
		r_phase[x] = 0;
		g_phase[x] = 0;
		b_phase[x] = 0;
	}
}

void light_showbin(unsigned char bitdata, unsigned char rp, unsigned char gp, unsigned char bp)
{
	if(bitdata & 01000000b) {
		light_set(0, rp, gp, bp);
	}
	else {
		light_set(0,0,0, 0);
	}
	
	if(bitdata & 00100000b) {
		light_set(1, rp, gp, bp);
	}
	else {
		light_set(1,0,0, 0);
	}	
	
	if(bitdata & 00010000b) {
		light_set(2, rp, gp, bp);
	}
	else {
		light_set(2,0,0, 0);
	}			

	if(bitdata & 00001000b) {
		light_set(3, rp, gp, bp);
	}
	else {
		light_set(3,0,0, 0);
	}	

	if(bitdata & 00000100b) {
		light_set(4, rp, gp, bp);
	}
	else {
		light_set(4,0,0, 0);
	}	
	if(bitdata & 00000010b) {
		light_set(5, rp, gp, bp);
	}
	else {
		light_set(5,0,0, 0);
	}	
	if(bitdata & 00000001b) {
		light_set(6, rp, gp, bp);
	}
	else {
		light_set(6,0,0, 0);
	}			
}

void light_set(unsigned char lix, unsigned char rp, unsigned char gp, unsigned char bp)
{
	if(lix >= NUMLIGHTS) return;
	r_phase[lix] = rlightmap[rp < NUMINTENSITY ? rp : NUMINTENSITY - 1];
	g_phase[lix] = glightmap[gp < NUMINTENSITY ? gp : NUMINTENSITY - 1];
	b_phase[lix] = blightmap[bp < NUMINTENSITY ? bp : NUMINTENSITY - 1];
}

volatile unsigned char light_paused = 0;

void light_pause() 	// Pause light show and turn off lights
{
	light_paused = 1;

	portb.SIG_RB_DISPLED1_O = 0;
	portb.SIG_RB_DISPLED2_O = 0;
	portb.SIG_RB_DISPLED3_O = 0;
	portb.SIG_RB_DISPLED4_O = 0;
	portb.SIG_RB_DISPLED5_O = 0;
	portc.SIG_RC_DISPLED6_O = 0;
	portc.SIG_RC_DISPLED7_O = 0;
	
	portc.SIG_RC_DISP_RED_O = 1;
	portc.SIG_RC_DISP_GRN_O = 1;
	portc.SIG_RC_DISP_BLU_O = 1;

}

void light_play() 
{
	light_paused = 0;
}



unsigned char  light_intr_ix = 0;  // Which LED we are working on (1-7 (indexed 0-6))
unsigned char light_intr_phase = 0;  // Interrupt Phase Counter

void light_intr()   // Called at 8 KHZ // 
{
	if(light_paused) return;		// If we are paused, return ASAP

	// To save code, the first cycle will likely not address the 1st light ()

	if(light_intr_phase >= r_phase[light_intr_ix]) {	// Turn Color Off when Phase Limit Reached
		portc.SIG_RC_DISP_RED_O = 1;
	}
	else if(!light_intr_phase) {	// Turn light on at start of new cycle if not off
		portc.SIG_RC_DISP_RED_O = 0;
	}
	
	if(light_intr_phase >= g_phase[light_intr_ix]) {
		portc.SIG_RC_DISP_GRN_O = 1;
	}
	else if(!light_intr_phase) {	// Turn light on at start of new cycle if not off
		portc.SIG_RC_DISP_GRN_O = 0;
	}
	
	if(light_intr_phase >= b_phase[light_intr_ix]) {
		portc.SIG_RC_DISP_BLU_O = 1;
	}
	else if(!light_intr_phase) {	// Turn light on at start of new cycle if not off
		portc.SIG_RC_DISP_BLU_O = 0;
	}
	
	light_intr_phase++;  // Add one to the phase
	
	if(light_intr_phase < MAXLIGHTPHASE) return;  //  If in middle of phase count, return
	
	// If we got here, we have hit the end of the allocated phase slots, and now need to 
	// switch to the next LED.
	//
	
	light_intr_phase = 0;	// Wrap the phase back to 0
	
	light_intr_ix++;		// Increment the LED index
	
	if (light_intr_ix >= NUMLIGHTS) {	// Check to see if we wrapped
		light_intr_ix = 0;
	}
	
	switch (light_intr_ix) {			// Turn off the previous LED nad turn on this one. 
	  case 0:
	    portc.SIG_RC_DISPLED7_O = 0;
	    portb.SIG_RB_DISPLED1_O = 1;
	    break;
	  case 1: 
	    portb.SIG_RB_DISPLED1_O = 0;
	    portb.SIG_RB_DISPLED2_O = 1;
	    break;
	  case 2:
	    portb.SIG_RB_DISPLED2_O = 0;
	    portb.SIG_RB_DISPLED3_O = 1;
	    break;
	  case 3: 
	    portb.SIG_RB_DISPLED3_O = 0;
	    portb.SIG_RB_DISPLED4_O = 1;
	    break;
	  case 4:
	    portb.SIG_RB_DISPLED4_O = 0;
	    portb.SIG_RB_DISPLED5_O = 1;
	    break;
	  case 5: 
	    portb.SIG_RB_DISPLED5_O = 0;
	    portc.SIG_RC_DISPLED6_O = 1;
	    break;
	  case 6: 
	    portc.SIG_RC_DISPLED6_O = 0;
	    portc.SIG_RC_DISPLED7_O = 1;
	    break;

	 }

}
	    
static unsigned char dir, ceyix;
static unsigned char speedfactor;
static unsigned char showix;
unsigned char lightshowrun = 0;
static unsigned char dongstate, dongcount;

static char msec_accum;
static char loopfac; 
	
void light_show(unsigned char which, unsigned char factor)
{
    speedfactor = factor;
    light_init();
    msec_accum = loopfac = 0;
    lightshowrun = 1;
	switch(which) {
	  case LIGHTSHOW_CEYLON:
	    light_set(0, 4, 0, 0);
	    dir = 1; // For this one, it control the direction. INitially up
	    ceyix = 0;
	    break;
	  case LIGHTSHOW_RGWAVE:
	    break;
	  case LIGHTSHOW_COUNTDOWN:
	    break;
	  case LIGHTSHOW_RAINBOW:
	  	light_set(0, 0, 0, 6);
		light_set(1, 0, 2, 4);
		light_set(2, 0, 4, 2);
		light_set(3, 0, 6, 0);
		light_set(4, 2, 4, 0);
		light_set(5, 4, 2, 0);
		light_set(6, 6, 0, 0);
	    break;
	  case LIGHTSHOW_DONGS:			/// All behavior in the animation
	    dongstate = 0;
	    dongcount = 0;
	    break;
	  case LIGHTSHOW_OFF:
		light_init();
		lightshowrun = 0;
		break;
	}
	showix = which;
}
	  
void light_animate(unsigned char msecs)
{
	unsigned char tmp_r, tmp_g, tmp_b;
	unsigned char i;
	
	if(!lightshowrun) return;
	
	msec_accum += msecs;
	while (msec_accum >= 20) { // 20 msecs translates to base timing
	   loopfac++;
	   msec_accum -= 20;
	}
	
	if(loopfac < speedfactor) return;
	
	loopfac = 0;

	switch(showix) {
	  case LIGHTSHOW_CEYLON:
		light_set(ceyix, 0, 0, 0);
		if(dir == 1) {
			ceyix++;
			if (ceyix == NUMLIGHTS-1) dir=0;
		}
		else {
			ceyix--;
			if (ceyix == 0) dir=1;
		}
		light_set(ceyix, 4, 0, 0);
	    break;
	  case LIGHTSHOW_RGWAVE:
	    break;
	  case LIGHTSHOW_COUNTDOWN:
	    break;
	  case LIGHTSHOW_RAINBOW:
		tmp_r = r_phase[0];
		tmp_g = g_phase[0];
		tmp_b = b_phase[0];
		for(i=0; i < NUMLIGHTS-1; i++) {
			r_phase[i] = r_phase[i+1];
			g_phase[i] = g_phase[i+1];
			b_phase[i] = b_phase[i+1];
		}
		r_phase[NUMLIGHTS-1] = tmp_r;
		g_phase[NUMLIGHTS-1] = tmp_g;
		b_phase[NUMLIGHTS-1] = tmp_b;
	    break;
	  case LIGHTSHOW_DONGS:	
	    dongcount--;
	    switch(dongstate) {
	      case 0:
	        light_set(6, 1, 0, 0); // Set the lowest light red
	        dongcount = 50; 	   // Wait 50 clicks to next state
	        dongstate = 1;
	        break;
	      case 1: 
	        //dongcount--;
	        if(dongcount) break;
	        light_set(5,1,0,0);
	        dongcount=50;
	        dongstate = 2;
	        break;
	      case 2:
	      	//dongcount--;
	        if(dongcount) break;
	        light_set(4,1,0,0);
	        dongcount=50;
	        dongstate = 3;
	        break;
	      case 3:
	      	//dongcount--;
	        if(dongcount) break;
	        light_set(3,1,0,0);
	        dongcount=130;
	        dongstate = 4;
	        break;
	      case 4:
	      	//dongcount--;
	        if(dongcount) break;
	        //light_set(0,0,0,0);
	        light_set(2,4,4,4);
	        dongcount=3;
	        dongstate = 5;
	        break;
	      case 5:
	      	//dongcount--;
	        if(dongcount) break;
	        light_set(2,0,0,0);
	        light_set(1,4,4,4);
	        dongcount=3;
	        dongstate = 6;
	        break;
	      case 6:
	      	//dongcount--;
	        if(dongcount) break;
	        light_set(1,0,0,0);
	        light_set(0,4,4,4);
	        dongcount=3;
	        dongstate = 7;
	        break;
	      case 7:
	      	//dongcount--;
	        if(dongcount) break;
	        light_set(0,0,0,0);
	        dongcount=18;
	        dongstate = 4;
	        break;
	    }
	    break;	
	  case LIGHTSHOW_OFF:
		light_init();
		lightshowrun = 0;
		break;
	}
} 
	