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

#define RFCMD_BEACON	0x01 // 1-3 reserved for autonomous outbound commands  

#define RFCMD_PLAYDONG	0x04  // Eddie's DONG voice
#define RFCMD_PLAYRICK  0x05  // Rick Roll
#define RFCMD_PLAYNYAN  0x06  // Nyan Song
#define RFCMD_PLAYRAIN  0x07  // Slayer Raining
#define RFCMD_PLAYJOUR	0x08  // Journey Don't Stop Believing (main)
#define RFCMD_PLAYCANT  0x09  // Star Wars Cantina
#define RFCMD_PLAYSECR	0x0a  // Song Secret
#define RFCMD_PLAYKRY0  0x0b  // Long Ass Chiptune Annoyance Song 

#define RFCMD_PLAYPAC   0x10  // Pacman Sound
#define RFCMD_PLAYPEW	0x11  // Pew Pew
#define RFCMD_PLAYKLAX  0x12  // Klaxon
#define RFCMD_PLAYSIREN	0x13  // Siren


#define RFCMD_ADMNSOC0	0x20  // Set Social vector bit (**)
#define RFCMD_ADMNSOC1	0x21  // Set Social vector bit (**)
#define RFCMD_ADMNSOC2	0x22  // Set Social vector bit (**)
#define RFCMD_ADMNSOC3	0x23  // Set Social vector bit (**)
#define RFCMD_ADMNSOC4	0x24  // Set Social vector bit (**)
#define RFCMD_ADMNSOC5	0x25  // Set Social vector bit (**)
#define RFCMD_ADMNSOCC  0x28  // Clear Vec

#define RFCMD_ESET		0x30  		// Send Special El (**)
#define RFCMD_ECLR		0x31  		// 
#define RFCMD_DON1		0x34  // Special Reserved (**)
#define RFCMD_ATTEN		0x35  // Special AttenMode (**)
#define RFCMD_PERF1		0x36  // Special Performance (**)



//
// 
// Fuctions
extern void rfcmd_3send(unsigned char , unsigned char , unsigned char );
extern void rfcmd_execute(unsigned char *, unsigned char );
extern void rfcmd_clrcden();
extern unsigned short rfcmd_getdensity();
extern unsigned char   rfcmd_lastsender();