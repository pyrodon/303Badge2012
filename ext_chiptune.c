#include <system.h>
#include "Badge.h"

#include "ext_chiptunesong.h" 



typedef unsigned char u8;
typedef unsigned short u16;
typedef signed char s8;
typedef signed short s16;
typedef unsigned long u32;

volatile u8 callbackwait;
volatile u8 lastsample;

volatile u8 timetoplay;

volatile u8 test;
volatile u8 testwait;

u8 trackwait;
u8 trackpos;
u8 playsong;
u8 songpos;

unsigned long SongAddress = 0;



u8 light[2];

// #include "../tracker/exported.h"

/* const u16 freqtable[] = {
	0x010b, 0x011b, 0x012c, 0x013e, 0x0151, 0x0165, 0x017a, 0x0191, 0x01a9,
	0x01c2, 0x01dd, 0x01f9, 0x0217, 0x0237, 0x0259, 0x027d, 0x02a3, 0x02cb,
	0x02f5, 0x0322, 0x0352, 0x0385, 0x03ba, 0x03f3, 0x042f, 0x046f, 0x04b2,
	0x04fa, 0x0546, 0x0596, 0x05eb, 0x0645, 0x06a5, 0x070a, 0x0775, 0x07e6,
	0x085f, 0x08de, 0x0965, 0x09f4, 0x0a8c, 0x0b2c, 0x0bd6, 0x0c8b, 0x0d4a,
	0x0e14, 0x0eea, 0x0fcd, 0x10be, 0x11bd, 0x12cb, 0x13e9, 0x1518, 0x1659,
	0x17ad, 0x1916, 0x1a94, 0x1c28, 0x1dd5, 0x1f9b, 0x217c, 0x237a, 0x2596,
	0x27d3, 0x2a31, 0x2cb3, 0x2f5b, 0x322c, 0x3528, 0x3851, 0x3bab, 0x3f37,
	0x42f9, 0x46f5, 0x4b2d, 0x4fa6, 0x5462, 0x5967, 0x5eb7, 0x6459, 0x6a51,
	0x70a3, 0x7756, 0x7e6f
}; */

 const  u16 freqtable[] = {
	0x0085, 0x008d, 0x0096, 0x009f, 0x00a8, 0x00b2, 0x00bd, 0x00c8, 0x00d4,
	0x00e1, 0x00ee, 0x00fc, 0x010b, 0x011b, 0x012c, 0x013e, 0x0151, 0x0165,
	0x017a, 0x0191, 0x01a9, 0x01c2, 0x01dd, 0x01f9, 0x0217, 0x0237, 0x0259,
	0x027d, 0x02a3, 0x02cb, 0x02f5, 0x0322, 0x0352, 0x0385, 0x03ba, 0x03f3,
	0x042f, 0x046f, 0x04b2, 0x04fa, 0x0546, 0x0596, 0x05eb, 0x0645, 0x06a5,
	0x070a, 0x0775, 0x07e6, 0x085f, 0x08de, 0x0965, 0x09f4, 0x0a8c, 0x0b2c,
	0x0bd6, 0x0c8b, 0x0d4a, 0x0e14, 0x0eea, 0x0fcd, 0x10be, 0x11bd, 0x12cb,
	0x13e9, 0x1518, 0x1659, 0x17ad, 0x1916, 0x1a94, 0x1c28, 0x1dd5, 0x1f9b,
	0x217c, 0x237a, 0x2596, 0x27d3, 0x2a31, 0x2cb3, 0x2f5b, 0x322c, 0x3528,
	0x3851, 0x3bab, 0x3f37
};


rom s8 sinetable[] = {
	0, 12, 25, 37, 49, 60, 71, 81, 90, 98, 106, 112, 117, 122, 125, 126,
	127, 126, 125, 122, 117, 112, 106, 98, 90, 81, 71, 60, 49, 37, 25, 12,
	0, -12, -25, -37, -49, -60, -71, -81, -90, -98, -106, -112, -117, -122,
	-125, -126, -127, -126, -125, -122, -117, -112, -106, -98, -90, -81,
	-71, -60, -49, -37, -25, -12
};

const u8 validcmds[] = "0dfijlmtvw~+=";

enum {
	WF_TRI,
	WF_SAW,
	WF_PUL,
	WF_NOI
};

volatile struct oscillator {
	u16	freq;
	u16	phase;
	u16	duty;
	u8	waveform;
	u8	volume;	// 0-255
} osc[4];

struct trackline {
	u8	note;
	u8	instr;
	u8	cmd[2];
	u8	param[2];
	};

struct track {
	struct trackline	line[TRACKLEN];
};

struct unpacker {
	u16	nextbyte;
	u8	buffer;
	u8	bits;
};

struct channel {
	struct unpacker		trackup;
	u8			tnum;
	s8			transp;
	u8			tnote;
	u8			lastinstr;
	u8			inum;
	u16			iptr;
	u8			iwait;
	u8			inote;
	s8			bendd;
	s16			bend;
	s8			volumed;
	s16			dutyd;
	u8			vdepth;
	u8			vrate;
	u8			vpos;
	s16			inertia;
	u16			slur;
} channel[4];

u16 resources[16 + MAXTRACK];

struct unpacker songup;

static unsigned char rnd_a = 1, rnd_b = 50, rnd_c = 3, rnd_x = 1;

void init_rnd(unsigned char s1,unsigned char s2,unsigned char s3) //Can also be used to seed the rng with more entropy during use.
{
//XOR new entropy into key state
rnd_a ^=s1;
rnd_b ^=s2;
rnd_c ^=s3;
 
rnd_x++;
rnd_a = (rnd_a^rnd_c^rnd_x);
rnd_b = (rnd_b+rnd_a);
rnd_c = (rnd_c+(rnd_b>>1)^rnd_a);
}
 
unsigned char rnd_randomize()
{
rnd_x++;               //x is incremented every round and is not affected by any other variable
rnd_a = (rnd_a^rnd_c^rnd_x);       //note the mix of addition and XOR
rnd_b = (rnd_b+rnd_a);         //And the use of very few instructions
rnd_c = (rnd_c+(rnd_b>>1)^rnd_a);  //the right shift is to ensure that high-order bits from b can affect  
return(rnd_c);          //low order bits of other variables
}


u8 readsongbyte(u16 offset)
{
    u16 address;
    u8 result;
    
    address = SONGDATA + offset;
 
    asm 
    {
	MOVLW	0x00
	MOVWF _tblptru
	MOVF _address + 1, W
	MOVWF _tblptrh
	MOVF _address, W
	MOVWF _tblptrl
	TBLRD*
	MOVF _tablat, W
	MOVWF _result
	// RETURN			
    }
    return(result);
 }
    
    
    
    


void initup(struct unpacker *up, u16 offset) {
	up->nextbyte = offset;
	up->bits = 0;
}

u8 readbit(struct unpacker *up) { 
	u8 val;

	if(!up->bits) {
		up->buffer = readsongbyte(up->nextbyte++);
		up->bits = 8;
	}

	up->bits--;
	val = up->buffer & 1;
	up->buffer >>= 1;

	return val;
}

u16 readchunk(struct unpacker *up, u8 n) {
	u16 val = 0;
	u8 i;

	for(i = 0; i < n; i++) {
		if(readbit(up)) {
			val |= (1 << i);
		}
	}

	return val;
}

void readinstr(u8 num, u8 pos, u8 *dest) {
	//dest[0] = readsongbyte(resources[num] + 2 * pos + 0); ORIG
	//dest[1] = readsongbyte(resources[num] + 2 * pos + 1); ORIG
	dest[0] = readsongbyte(resources[num] + (pos << 1));
	dest[1] = readsongbyte(resources[num] + (pos << 1) + 1);
}

void runcmd(u8 ch, u8 cmd, u8 param) {
    
	switch(validcmds[cmd]) {
		case '0':
			channel[ch].inum = 0;
			break;
		case 'd':
			osc[ch].duty = param << 8;
			break;
		case 'f':
			channel[ch].volumed = (s8) param;
			break;
		case 'i':
			channel[ch].inertia = param << 1;
			break;
		case 'j':
			channel[ch].iptr = param;
			break;
		case 'l':
			channel[ch].bendd = (s8) param;
			break;
		case 'm':
			channel[ch].dutyd = param << 6;
			break;
		case 't':
			channel[ch].iwait = param;
			break;
		case 'v':
			osc[ch].volume = param;
			break;
		case 'w':
			osc[ch].waveform = param;
			break;
		case '+':
			// channel[ch].inote = param + channel[ch].tnote - 12 * 4; ORIG
			channel[ch].inote = param + channel[ch].tnote - (12 << 2);

			break;
		case '=':
			channel[ch].inote = param;
			break;
		case '~':
			if(channel[ch].vdepth != (param >> 4)) {
				channel[ch].vpos = 0;
			}
			channel[ch].vdepth = param >> 4;
			channel[ch].vrate = param & 15;
			break;
	}
}

void playroutine() {			// called at 50 Hz
	u8 ch;
	u8 lights;
	struct channel *cptr;

	if(playsong) {
		if(trackwait) {
			trackwait--;
		} else {
			trackwait = 4;

			if(!trackpos) {
				if(playsong) {
					if(songpos >= SONGLEN) {
						playsong = 0;
					} else {
						for(ch = 0; ch < 4; ch++) {
							u8 gottransp;
							u8 transp;
							cptr = &channel[ch];

							gottransp = readchunk(&songup, 1) & 0xff; // DK (s8)
							cptr->tnum = readchunk(&songup, 6) & 0xff;
							if(gottransp) {
								transp = readchunk(&songup, 4) & 0xff;
								if(transp & 0x8) transp |= 0xf0;
							} else {
								transp = 0;
							}
							cptr->transp = (s8) transp;
							if(cptr->tnum) {
								initup(&channel[ch].trackup, resources[16 + cptr->tnum - 1]);
							}
							//cptr++;
						}
						songpos++;
					}
				}
			}

			if(playsong) {
				for(ch = 0; ch < 4; ch++) {
				    cptr=&channel[ch];
					if(cptr->tnum) {
						u8 note, instr, cmd, param;
						u8 fields;

						fields = readchunk(&cptr->trackup, 3) & 0xff;
						note = 0;
						instr = 0;
						cmd = 0;
						param = 0;
						if(fields & 1) note = readchunk(&cptr->trackup, 7) & 0xff;
						if(fields & 2) instr = readchunk(&cptr->trackup, 4) & 0xff;
						if(fields & 4) {
							cmd = readchunk(&cptr->trackup, 4) & 0xff;
							param = readchunk(&cptr->trackup, 8) & 0xff;
						}
						if(note) {
							cptr->tnote = note + cptr->transp;
							if(!instr) instr = cptr->lastinstr;
						}
						if(instr) {
							if(instr == 2) light[1] = 5;
							if(instr == 1) {
								light[0] = 5;
								if(cptr->tnum == 4) {
									light[0] = light[1] = 3;
								}
							}
							if(instr == 7) {
								light[0] = light[1] = 30;
							}
							cptr->lastinstr = instr;
							cptr->inum = instr;
							cptr->iptr = 0;
							cptr->iwait = 0;
							cptr->bend = 0;
							cptr->bendd = 0;
							cptr->volumed = 0;
							cptr->dutyd = 0;
							cptr->vdepth = 0;
						}
						if(cmd) runcmd(ch, cmd, param);
					}
					//cptr++;
				}

				trackpos++;
				trackpos &= 31;
			}
		}
	}

	for(ch = 0, cptr=&channel[0]; ch < 4; ch++) { // ***
		s16 vol;
		u16 duty;
		u16 slur;

		while(cptr->inum && !cptr->iwait) {
			u8 il[2];

			readinstr(cptr->inum, cptr->iptr & 0xff, il);
			cptr->iptr++;

			runcmd(ch, il[0], il[1]);
		}
		if(cptr->iwait) cptr->iwait--;

		if(cptr->inertia) {
			s16 diff;

			slur = cptr->slur;
			//diff = (freqtable[channel[ch].inote]<<1)  - slur; // DK FREQ
			diff = freqtable[cptr->inote]  - slur; // DK FREQ

			//diff >>= channel[ch].inertia;
			if(diff > 0) {
				if(diff > cptr->inertia) diff = channel[ch].inertia;
			} else if(diff < 0) {
				if(diff < -cptr->inertia) diff = -channel[ch].inertia;
			}
			slur = (s16) slur + diff; // DK
			cptr->slur = slur;
		} else {
			slur = freqtable[cptr->inote]<<1; 
		}
		osc[ch].freq =
			(s16) slur +
			cptr->bend +
			(((s8)cptr->vdepth * sinetable[cptr->vpos & 63]) >> 2);
		cptr->bend += cptr->bendd;
		vol = osc[ch].volume + cptr->volumed;
		if(vol < 0) vol = 0;
		if(vol > 255) vol = 255;
		osc[ch].volume = vol & 0xff ;

		duty = osc[ch].duty + cptr->dutyd;
		if(duty > 0xe000) duty = 0x2000;
		if(duty < 0x2000) duty = 0xe000;
		osc[ch].duty = duty;

		cptr->vpos += cptr->vrate;
		cptr++;
	}

	portb.SIG_RB_DISPLED1_O = 0;
	portb.SIG_RB_DISPLED2_O = 0;
	portb.SIG_RB_DISPLED3_O = 0;
	portb.SIG_RB_DISPLED4_O = 0;
	portc.SIG_RC_DISP_BLU_O = 1;
	portc.SIG_RC_DISP_RED_O = 1;
	portc.SIG_RC_DISP_GRN_O = 1;

	lights = 0;
	if(light[0]) {
		light[0]--;	
		lights |= 0x01;
	}
	if(light[1]) {
		light[1]--;
		lights |= 0x02;
	}
	switch (lights) {
	  case 1:
		portc.SIG_RC_DISP_RED_O = 0;
		portb.SIG_RB_DISPLED1_O = 1;
	    break;
	  case 2:
		portc.SIG_RC_DISP_GRN_O = 0;
		portb.SIG_RB_DISPLED4_O = 1;
		break;
	  case 3:
		portc.SIG_RC_DISP_BLU_O = 0;
		portb.SIG_RB_DISPLED2_O = 1;
		portb.SIG_RB_DISPLED3_O = 1;
		break;
	}
			
	//PORTC = lights;

}

void initresources() {
	u8 i;
	struct unpacker up;

	initup(&up, 0);
	for(i = 0; i < 16 + MAXTRACK; i++) {
		resources[i] = readchunk(&up, 13);
	}

	initup(&songup, resources[0]);
}


#ifdef NEVER
int main() {
	asm("cli");
	watchdogoff();
	CLKPR = 0x80;
	CLKPR = 0x80;

	DDRC = 0x12;
	DDRD = 0xff;

	PORTC = 0;

	timetoplay = 0;
	trackwait = 0;
	trackpos = 0;
	playsong = 1;
	songpos = 0;

	osc[0].volume = 0;
	channel[0].inum = 0;
	osc[1].volume = 0;
	channel[1].inum = 0;
	osc[2].volume = 0;
	channel[2].inum = 0;
	osc[3].volume = 0;
	channel[3].inum = 0;

	initresources();

	TCCR0A = 0x02;
	TCCR0B = 0x02;	// clkIO/8, so 1/8 MHz
	OCR0A = 62;//125; // 8 KHz

	TIMSK0 = 0x02;

	asm("sei");
	for(;;) {
		while(!timetoplay);

		timetoplay--;
		playroutine();
	}
}
#endif


void tune_play_intr()		// called at 8 KHz
{
	static u8 i;
	static s16 acc;
	static u8 newbit;
	static s8 value; // [-32,31]
	static struct oscillator *optr;
	
	if(!playsong) return;

	sound_val_hs(lastsample);   // Send the previously calculated value to the DAC


	if(callbackwait) {
		callbackwait--;
	} else {
		timetoplay++;
		callbackwait = 93;
	}

	acc = 0;
	for(i = 0; i < 4; i++) {  // i < 4 ORIG
	     optr = &osc[i];


		switch(osc[i].waveform) {
			case WF_TRI:
				if(optr->phase < 0x8000) {
					value = -32 + (s8)(optr->phase >> 9);
				} else {
					value = 31 - (s8)((optr->phase - 0x8000) >> 9);
				}
				break;
			case WF_SAW:
				value = -32 + (s8)(optr->phase >> 10);
				break;
			case WF_PUL:
				value = (optr->phase > optr->duty)? -32 : 31;
				break;
			case WF_NOI:
				value = (s8)(rnd_randomize() & 63) - 32;

				break;
			default:
				value = 0;
				break;
		}
		optr->phase += optr->freq;

		// the following needs the value to be cast to s16 to work, even though the internal multiplier
		// takes 2 8-bit numbers and generates 16 bits, the compiler doesn't know how to deal 
		// with it withouth the cast - DK
		acc += (s16) value  *  (u8) optr->volume; // rhs = [-8160,7905]
		//acc += (u8) (value) * (u8) osc[i].volume; // rhs = [-8160,7905]
		//optr++;
	}
	// acc [-32640,31620]
	lastsample = (128 + (acc >> 8));	// [1,251]
	//lastsample =  (acc >> 8);	// [1,251]
}

void tune_init()
{
	timetoplay = 0;
	trackwait = 0;
	trackpos = 0;
	playsong = 1;
	songpos = 0;

	osc[0].volume = 0;
	channel[0].inum = 0;
	osc[1].volume = 0;
	channel[1].inum = 0;
	osc[2].volume = 0;
	channel[2].inum = 0;
	osc[3].volume = 0;
	channel[3].inum = 0;

	initresources();
}

void tune_startsong(unsigned char songnum)
{
	sound_amp_on();
}

void tune_songwork()
{
	if(timetoplay) {
		timetoplay--;
	}
	playroutine();
	
	if(playsong) return;
	osc[0].volume = 0;
	channel[0].inum = 0;
	osc[1].volume = 0;
	channel[1].inum = 0;
	osc[2].volume = 0;
	channel[2].inum = 0;
	osc[3].volume = 0;
	channel[3].inum = 0;
	sound_hsstop();
	sound_amp_off();
}


void tune_playsong()
{
	sound_amp_on();
	sound_hsstart();
	for(;playsong;) {
		while(!timetoplay);

		timetoplay--;
		playroutine();
	}
	osc[0].volume = 0;
	channel[0].inum = 0;
	osc[1].volume = 0;
	channel[1].inum = 0;
	osc[2].volume = 0;
	channel[2].inum = 0;
	osc[3].volume = 0;
	channel[3].inum = 0;
	sound_hsstop();
	sound_amp_off();
}
