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
//
// SONG Index Definitions
//

#define SONG_NONE 	0

#define SONG_SIREN	1
#define SONG_SECRET 2
#define SONG_RICK	3
#define SONG_RAINING	4
#define SONG_PEWPEW 5
#define SONG_PACMAN 6
#define SONG_ORIG	7
#define SONG_NYAN	8
#define SONG_KRY0	9
#define SONG_KLAXON 10
#define SONG_JOURNEY3	11
#define SONG_JOURNEY2	12
#define SONG_JOURNEY1	13
#define SONG_DYING		14
#define SONG_CHIRP2	15
#define SONG_CHIRP1	16
#define SONG_CANTINA	17
#define SONG_CACTUS		18
#define SONG_BUZZER	19
#define SONG_303	20

#define MAXTRACK 0x39  // Based on longest max track of all songs. For dimension sizing

extern unsigned long SongSelAddr;
extern unsigned char SongSelMaxTrack;
extern unsigned char SongSelSongLen;
extern unsigned char SongSelRateVal;
extern unsigned char SongSelXtraVol;

extern void songselect(unsigned char songix);



