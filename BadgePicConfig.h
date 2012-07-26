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
#ifndef BADGEPICCONFIG_H
#define BADGEPICCONFIG_H

//////////////////////////////////////////////////////////////////
// Watchdog Timer Enable bit:
#pragma config WDTEN = OFF // WDT disabled (control is placed on SWDTEN bit)
// #pragma config WDTEN = ON // WDT enabled

//////////////////////////////////////////////////////////////////
// Stack Overflow/Underflow Reset Enable bit:
#pragma config STVREN = OFF // Reset on stack overflow/underflow disabled
// #pragma config STVREN = ON // Reset on stack overflow/underflow enabled

//////////////////////////////////////////////////////////////////
// Extended Instruction Set Enable bit:
// #pragma config XINST = OFF // Instruction set extension and Indexed Addressing mode disabled (Legacy mode)
#pragma config XINST = OFF // Instruction set extension and Indexed Addressing mode enabled

//////////////////////////////////////////////////////////////////
// Background Debugger Enable bit:
// #pragma config DEBUG = ON // Background debugger enabled; RB6 and RB7 are dedicated to In-Circuit Debug
#pragma config DEBUG = OFF // Background debugger disabled; RB6 and RB7 configuredas general purpose I/O pins

//////////////////////////////////////////////////////////////////
// Code Protection bit:
// #pragma config CP0 = ON // Program memory is code-protected
#pragma config CP0 = OFF // Program memory is not code-protected

//////////////////////////////////////////////////////////////////
// Oscillator Selection bits:
// #pragma config FOSC = INTOSC // Internal oscillator, port function on RA6 and RA7 
// #pragma config FOSC = INTOSCO // Internal oscillator, CLKOUT on RA6 and port function on RA7
#pragma config FOSC = INTOSCPLL // INTOSC with PLL enabled, port function on RA6 and RA7
// #pragma config FOSC = INTOSCPLLO // INTOSC with PLL enabled, CLKOUT on RA6 and port function on RA7
// #pragma config FOSC = HS // HS oscillator
//#pragma config FOSC = HSPLL // HS oscillator, PLL enabled
// #pragma config FOSC = EC // EC Oscillator, CLKO on RA6
//#pragma config FOSC = ECPLL // EC Oscillator, PLL enabled, CLKO on RA6

//////////////////////////////////////////////////////////////////
// Fail-Safe Clock Monitor Enable bit:
// #pragma config FCMEN = OFF // Fail-Safe Clock Monitor disabled
#pragma config FCMEN = ON // Fail-Safe Clock Monitor enabled

//////////////////////////////////////////////////////////////////
// Two-Speed Start-up (Internal/External Oscillator Switchover) Control bit:
// #pragma config IESO = OFF // Two-Speed Start-up disabled
#pragma config IESO = ON // Two-Speed Start-up enabled

//////////////////////////////////////////////////////////////////
// Watchdog Timer Postscaler Select bits:
// #pragma config WDTPS = 1 // 1:1
// #pragma config WDTPS = 2 // 1:2
// #pragma config WDTPS = 4 // 1:4
// #pragma config WDTPS = 8 // 1:8
// #pragma config WDTPS = 16 // 1:16
// #pragma config WDTPS = 32 // 1:32
// #pragma config WDTPS = 64 // 1:64
// #pragma config WDTPS = 128 // 1:128
// #pragma config WDTPS = 256 // 1:256
// #pragma config WDTPS = 512 // 1:512
// #pragma config WDTPS = 1024 // 1:1024
// #pragma config WDTPS = 2048 // 1:2048
// #pragma config WDTPS = 4096 // 1:4096
// #pragma config WDTPS = 8192 // 1:8192
// #pragma config WDTPS = 16384 // 1:16384
#pragma config WDTPS = 32768 // 1:32768

//////////////////////////////////////////////////////////////////
// ECCP2 MUX bit:
// #pragma config CCP2MX = ALTERNATE // ECCP2/P2A is multiplexed with RE7
#pragma config CCP2MX = DEFAULT // ECCP2/P2A is multiplexed with RC1

//////////////////////////////////////////////////////////////////
// MSSP Address Masking Mode Select bit:
// #pragma config MSSPMSK = MSK5 // 5-Bit Address Masking mode enable
#pragma config MSSPMSK = MSK7 // 7-Bit Address Masking mode enable

//////////////////////////////////////////////////////////////////
// Watchdog Timer Enable bit:
#pragma config WDTEN = OFF // WDT disabled (control is placed on SWDTEN bit)
// #pragma config WDTEN = ON // WDT enabled

//#pragma DATA _CONFIG1L,  _DEBUG_OFF_1L & _XINST_ON_1L & _STVREN_ON_1L & _WDT_OFF_1L
//#pragma DATA _CONFIG1H,  _CP0_OFF_1H
//#pragma DATA _CONFIG2L,  _IESO_OFF_2L & _FCMEN_OFF_2L & _FOSC_INTOSC_2L
//#pragma DATA _CONFIG2H,  _WDTPS_32768_2H
//#pragma DATA _CONFIG3L,  _WAIT_OFF_3L & _BW_8_3L & _MODE_MM_3L & _EASHFT_OFF_3L
//#pragma DATA _CONFIG3H, _MSSP_MSK5_3H & _PPMX_DEFAULT_3H & _ECCPMX_DEFAULT_3H & _CCP2MX_DEFAULT_3H




#endif //  BADGEPICCONFIG_H