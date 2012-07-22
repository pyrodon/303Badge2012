

#define RFCMD_BEACON	0x01 // 1-3 reserved for autonomous outbound commands  

#define RFCMD_PLAYDONG	0x04  // Eddie's DONG voice
#define RFCMD_PLAYRICK  0x05  // Rick Roll
#define RFCMD_PLAYNYAN  0x06  // Nyan Song
#define RFCMD_PLAYKRY0  0x07  // Long Ass Chiptune Punishment Song (**)
#define RFCMD_PLAYRAIN  0x08  // Slayer Raining
#define RFCMD_PLAYJOUR	0x09  // Journey Don't Stop Believing (main)
#define RFCMD_PLAYCANT  0x0a  // Star Wars Cantina
#define RFCMD_PLAYSECR	0x0b  // Song Secret

#define RFCMD_PLAYPAC   0x10  // Pacman Sound
#define RFCMD_PLAYPEW	0x11  // Pew Pew
#define RFCMD_PLAYKLAX  0x12  // Klaxon
#define RFCMD_PLAYSIREN	0x13  // Siren

#define RFCMD_ADMNSOC	0x20  // Set Social vector bit (**)
#define RFCMD_ADMNSPEC	0x21  // Send Special (**)
#define RFCMD_DON1		0x22  // Special Reserved (**)
#define RFCMD_ALERT		0x23  // Special Alert (**)
#define RFCMD_PERF1		0x24  // Special Performance (**)



//
// 
// Fuctions
extern void rfcmd_3send(unsigned char , unsigned char , unsigned char );
extern void rfcmd_execute(unsigned char *, unsigned char );
extern void rfcmd_clrcden();
extern unsigned short rfcmd_getdensity();