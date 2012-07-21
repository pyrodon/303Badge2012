

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
