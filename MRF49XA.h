// RF Subsystem.. Adapted for this project by Don Kark
//
// Originally supplied as a MicroChip Reference Application Note. 
/*********************************************************************
* FileName:        MRF49XA.h
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
#define NODATA 			0
#define DATA_RECEIVED 	1
#define PACKET_RECEIVED	2
#define	PAYLOAD_MAX		20


// un-comment this line in order to test 
// the spectrum and center frequency
// #define SPECTRUM_TEST


unsigned char  SPI_Read(void);
void SPI_Write(unsigned char spidata);
void SPI_Write16(unsigned short spicmd);
void SPI_Command(unsigned short spicmd);
void MRF49XA_Init();
void MRF49XA_Send_Packet(unsigned char *, unsigned char );
unsigned char  MRF49XA_Receive_Packet(unsigned char *, unsigned char *);
void MRF49XA_Power_Down(void);
void MRF49XA_Read_Status(unsigned char *);
void MRF49XA_Reset_Radio(void);



