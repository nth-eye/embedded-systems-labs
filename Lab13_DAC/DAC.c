// DAC.c
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// Implementation of the 4-bit digital to analog converter
// Daniel Valvano, Jonathan Valvano
// December 29, 2014
// Port B bits 3-0 have the 4-bit DAC

#include "DAC.h"
#include "..//tm4c123gh6pm.h"

// **************DAC_Init*********************
// Initialize 4-bit DAC 
// Input: none
// Output: none
void DAC_Init(void)
{
	SYSCTL_RCGC2_R |= 0x02; 		// Enable clock on B
	GPIO_PORTB_DIR_R = 0x3F;		// Set PB0-PB5 as output
	GPIO_PORTB_DEN_R = 0x3F;		// Enable digital pins PB0-PB5
	GPIO_PORTB_AMSEL_R = 0x00;	// Disable analog function for PB0-PB5	
	GPIO_PORTB_AFSEL_R = 0x00;	// Disable alternate function for PB0-PB5
	GPIO_PORTB_PCTL_R = 0x00;		// GPIO clear bit PCTL
	GPIO_PORTB_DR8R_R = 0x3F; 	// Enable DR8R for PB0-PB5 
}


// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Output: none
void DAC_Out(unsigned long data)
{
	GPIO_PORTB_DATA_R = data;
}
