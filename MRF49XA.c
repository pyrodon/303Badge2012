// RF Subsystem.. Adapted for this project by Don Kark
//
// Originally supplied as a MicroChip Reference Application Note. 

/*********************************************************************
* FileName:		 MRF49XA.c 
* Dependencies:    See INCLUDES section below
* Processor:       
* Compiler:        
* Company:         Microchip Technology, Inc.
*
* Software License Agreement:
*
* The software supplied herewith by Microchip Technology Incorporated
* (the "Company") for its PICmicro® Microcontroller is intended and
* supplied to you, the Company's customer, for use solely and
* exclusively on Microchip PICmicro Microcontroller products. The
* software is owned by the Company and/or its supplier, and is
* protected under applicable copyright laws. All rights are reserved.
* Any use in violation of the foregoing restrictions may subject the
* user to criminal sanctions under applicable laws, as well as to
* civil liability for the breach of the terms and conditions of this
* license.
*
* THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
* TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
* PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
* IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
* CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
*********************************************************************
* File Description:
*
* Change History:
* Author               Cristian Toma
********************************************************************/
#include <system.h>
#include "MRF49XA.h"
#include "Badge.h"

//#define SPECTRUM_TEST

//--------------------------------------------------------------------
// MRF49XA SPI commands:
//--------------------------------------------------------------------
#define    GENCREG 		 0x801f		// Cload=16 pF (*); TX registers & FIFO are disabled; 433MHz (*)
#define     PMCREG 		 0x8208		// Everything off, uC clk enabled
#define     RXCREG 		 0x94A1		// BW=135kHz, DRSSI=-97dBm, pin8=VDI, fast VDI
#define     TXBREG 		 0xB800
#define     FIFORSTREG	 0xCA81		// Sync. latch cleared, limit=8bits, disable sensitive reset
#define    BBFCREG 		 0xC22C		// Digital LPF (default)
#define     AFCCREG		 0xC4D7		// Auto AFC (default)
#define    CFSREG 		 0xA7D0		// Fo=TBDMHz (default)  (Fb = 2000dec)(*)
#define     TXCREG		 0x9860		// df=60kHz xx3x, Pmax, normal modulation polarity (* Possibly need to use 9801)
//#define    DRSREG 		 0xC611		// 9.6KB
#define    DRSREG 		 0xC611		// 14.0KB
//#define BCSREG			 0xC0E0


//--------------------------------------------------------------------
// SPI_Read ()
//--------------------------------------------------------------------
unsigned char SPI_Read(void)
{
  unsigned char i;
  unsigned char spidata = 0;

  SPI_SDO = 0;
  SPI_SCK = 0;
  for (i=0;i<8;i=i+1)
    {
    spidata=spidata<<1;
	
	if(SPI_SDI)
	 spidata |= 0x01;
	else
	 spidata &= 0xFE;

    SPI_SCK = 1;
    SPI_SCK = 0;
    }
  return(spidata);
}

//--------------------------------------------------------------------
// SPI_Write ()
//--------------------------------------------------------------------
void SPI_Write(unsigned char spidata)
{
  unsigned char i;
  
  SPI_SDO = 0;
  SPI_SCK = 0;
  for (i=0;i<8;i=i+1)
    {
    if(spidata & 0x80)
     SPI_SDO = 1;
	else
	 SPI_SDO = 0;
    
	SPI_SCK = 1;			
    SPI_SCK = 0;
	spidata=spidata<<1;
    }
  
   SPI_SDO = 0;
}

//--------------------------------------------------------------------
// SPI_Command ()
//--------------------------------------------------------------------
void SPI_Command(unsigned short spicmd)
{
  SPI_CS = 0;
  SPI_Write ((spicmd & 0xFF00) >> 8);
  SPI_Write ((spicmd & 0x00FF));
  SPI_CS = 1;
}
//--------------------------------------------------------------------
// SPI_Write16 ()
//--------------------------------------------------------------------
void SPI_Write16(unsigned short spicmd)
{
  SPI_Write ((spicmd & 0xFF00) >> 8);
  SPI_Write ((spicmd & 0x00FF));
}
//--------------------------------------------------------------------
// FIFO syncron latch re-enable
//--------------------------------------------------------------------
void MRF49XA_Reset_Radio()
{
  SPI_Command(FIFORSTREG | 0x0002);		
}
//--------------------------------------------------------------------
// Power down the radio chip
//--------------------------------------------------------------------
void MRF49XA_Power_Down(void)
{
	SPI_Command(0x8201);
}
//--------------------------------------------------------------------
// MRF49XA_Init 
// Initializes the radio chip
//--------------------------------------------------------------------
void MRF49XA_Init(){

	//int delay;
  portd.SIG_RD_RFRESET_N_O = 0; // Hard Reset
  delay_ms(5);
  portd.SIG_RD_RFRESET_N_O = 1;
  delay_ms(5);

//----  configuring the RF link --------------------------------

//---- Send init cmd
  SPI_Command( FIFORSTREG );
  SPI_Command( FIFORSTREG | 0x0002);
  SPI_Command( GENCREG);
  SPI_Command( CFSREG);
  SPI_Command( PMCREG);
  SPI_Command( RXCREG);
  SPI_Command( TXCREG);	
//  SPI_Command( BCSREG);	
//---- antenna tunning
  SPI_Command( PMCREG | 0x0020);		// turn on tx
  DelayMs(4);
//---- end of antenna tunning
  SPI_Command( PMCREG | 0x0080);		// turn off Tx, turn on receiver
  SPI_Command( GENCREG | 0x0040);		// enable the FIFO
  SPI_Command( FIFORSTREG);
  SPI_Command( FIFORSTREG | 0x0002);	// enable syncron latch	
  RF_FSEL=1;
}

//--------------------------------------------------------------------
// MRF49XA_Send_Packet
// Sends a data packet
//--------------------------------------------------------------------
void MRF49XA_Send_Packet(unsigned char *data, unsigned char length){

	unsigned char a;
#ifdef EXTERNAL_AMP
	portf.SIG_RF_PLAY3 = 1; 		// Turn on optional power amplifier
	delay_10us(50);
#endif
	//---- turn off receiver , enable Tx register
	SPI_Command(PMCREG);				// turn off the transmitter and receiver
	SPI_Command(GENCREG | 0x0080);		// Enable the Tx register
	//---- Packet transmission
	// Reset value of the Tx regs are [AA AA], we can start transmission
	//---- Enable Tx
	SPI_Command(PMCREG |0x0020);		// turn on tx

		SPI_CS=0;						// chip select low
		while(!RF_SDO);
			SPI_Write16(TXBREG | 0xAA);	// preamble 
		while(!RF_SDO);
			SPI_Write16(TXBREG | 0x2D);	// sync pattern 1st byte
		while(!RF_SDO);
			SPI_Write16(TXBREG | 0xD4);	// sync pattern 2nd byte
		while(!RF_SDO);	
			SPI_Write16(TXBREG | length);
			
			
#ifdef SPECTRUM_TEST
			while(1) {
			     while(!RF_SDO);				// wait for SDO, when buffer data <=8 bits
			     SPI_Write16(0xAA);
			}
#else
			for (a=0;a<length;a++){				// send payload

			while(!RF_SDO);				// wait for SDO, when buffer data <=8 bits
			
			SPI_Write16(TXBREG | data[a]); // write a byte to tx register

			}
#endif
		
		while(!RF_SDO);				
		SPI_Write16(TXBREG |0x00);	// write a dummy byte since the previous byte is still in buffer 		
		while (!RF_SDO);				// wait for the last byte transmission end
		SPI_CS=1;						// chip select high, end transmission
		
	//---- Turn off Tx disable the Tx register
	SPI_Command(PMCREG | 0x0080);		// turn off Tx, turn on the receiver
	SPI_Command(GENCREG | 0x0040);		// disable the Tx register, Enable the FIFO
#ifdef EXTERNAL_AMP
	delay_10us(3);
	portf.SIG_RF_PLAY3 = 0; 		// Turn on optional power amplifier
#endif
}
//--------------------------------------------------------------------
// MRF49XA_Receive_Packet
// Receives a data packet
// This function should be called periodically until it returns
// a valid data packet
// The return values are (see MRF49XA.h) :
// NODATA 			0			no data received
// DATA_RECEIVED 		1			data received, data capture in progress
// PACKET_RECEIVED	2			data received, data packet received ok
//
// This function should be called periodically , at least once every 800us for 9600bps.
// For higher speed, the function should be called more rapidly.
// When a complete packet has been received, the function returns a PACKET_RECEIVED.
// In order to "re-arm" the receiver to receive a new packet call the 
// MRF49XA_Reset_Radio function.
//
// Use a call similar to :
// if ( MRF49XA_Receive_Packet(dataPacket,&length) == PACKET_RECEIVED)
// { 
//   .... do something with the received packet....
// }
// // re-arm for next packet
// MRF49XA_Reset_Radio();
//
//--------------------------------------------------------------------

unsigned char MRF49XA_Receive_Packet(unsigned char *data, unsigned char *length ){

	static unsigned char PacketLen =0;
	static unsigned char nrbytes =0;
	unsigned char bl;

	SPI_CS	=0;							// chip select low

	if(RF_SDO){								// in case of FINT then read out
		SPI_CS =1;	
		RF_FSEL = 0;
		if(PacketLen) {	// is this the first byte? go on if not
			data[nrbytes]=SPI_Read();// number of bytes in payload is the first byte
			nrbytes++;
			if (nrbytes >=PacketLen){   // end of packet
				SPI_Command(FIFORSTREG);// reset FIFO	
				RF_FSEL=1;
				// whole packet received
				*length = nrbytes;				
				nrbytes=0;
				PacketLen =0;
				return PACKET_RECEIVED;	
			}
		}
		else  {	// the first byte received
			bl=SPI_Read();
			if((bl>0) && (bl<PAYLOAD_MAX))
			{   // check if correct number of bytes in payload
				PacketLen=bl;
				nrbytes=0; 
			}
			else 
			{		
				RF_FSEL =1;						// bad packet length
				SPI_Command(PMCREG);			// turn off tx and rx
				SPI_Command(FIFORSTREG);		// reset FIFO
				SPI_Command(GENCREG);			// disable FIFO , Tx_latch
				SPI_Command(GENCREG | 0x0040);	// enable the FIFO	
				SPI_Command(PMCREG | 0x0080);	// turn on receiver
				SPI_Command(FIFORSTREG | 0x0002);   // FIFO syncron latch re-enable
				return NODATA;
			}
		}
		RF_FSEL =1;
		return DATA_RECEIVED;
	}
	else {
		SPI_CS = 1;
		return NODATA;  
	} 

}

















































