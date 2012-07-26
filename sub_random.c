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

static unsigned char rnd_ai = 1, rnd_bi = 50, rnd_ci = 3, rnd_xi = 1;

void init_rndi(unsigned char s1,unsigned char s2,unsigned char s3) //Can also be used to seed the rng with more entropy during use.
{
//XOR new entropy into key state
rnd_ai ^=s1;
rnd_bi ^=s2;
rnd_ci ^=s3;
 
rnd_xi++;
rnd_ai = (rnd_ai^rnd_ci^rnd_xi);
rnd_bi = (rnd_bi+rnd_ai);
rnd_ci = (rnd_ci+(rnd_bi>>1)^rnd_ai);
}
 
unsigned char rnd_randomizei()
{
rnd_xi++;               //x is incremented every round and is not affected by any other variable
rnd_ai = (rnd_ai^rnd_ci^rnd_xi);       //note the mix of addition and XOR
rnd_bi = (rnd_bi+rnd_ai);         //And the use of very few instructions
rnd_ci = (rnd_ci+(rnd_bi>>1)^rnd_ai);  //the right shift is to ensure that high-order bits from b can affect  
return(rnd_ci);          //low order bits of other variables
}