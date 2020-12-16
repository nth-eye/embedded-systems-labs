// ADC.c
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0 SS3 to be triggered by
// software and trigger a conversion, wait for it to finish,
// and return the result. 
// Daniel Valvano
// January 15, 2016

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2015

 Copyright 2016 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

#include "ADC.h"
#include "..//tm4c123gh6pm.h"

// This initialization function sets up the ADC 
// Max sample rate: <=125,000 samples/second
// SS3 triggering event: software trigger
// SS3 1st sample source:  channel 1
// SS3 interrupts: enabled but not promoted to controller
void ADC0_Init(void)
{ 
		volatile unsigned long delay;
    /*SYSCTL_RCGC2_R |= 0x10; 	// activate port E
		delay = SYSCTL_RCGC2_R;
    GPIO_PORTE_DIR_R = ~0x04;	// Set PE2 as input
    GPIO_PORTE_DEN_R = 0x00;	// Disable digital pins PE2
    GPIO_PORTE_AMSEL_R = 0x04;	// Enable analog function for PE2
    GPIO_PORTE_AFSEL_R = 0x04;  // Enable alternate function for PE2
    GPIO_PORTE_PCTL_R = 0x00;	// GPIO clear bit PCTL
    
    SYSCTL_RCGCADC_R = 0x01;
    ADC0_PC_R = 0x1;			// set ADC speed to 125 KHz
    ADC0_SSPRI_R = 0x0123;      // set sequence 3 to highest priority
    ADC0_ACTSS_R = 0x08;        // enable ADC0
	ADC0_EMUX_R &= ~0xF000;     // seq3 is software trigger
    ADC0_SSMUX3_R = 1;          // use channel 1
    ADC0_SSCTL3_R = 0x06;       // enable IE0 and END0
    ADC0_IM_R &= ~0x0008;       // disable SS3 interrupts*/
	
		/*Port E initialization*/
  SYSCTL_RCGC2_R |= 0x10; 		// activate port E
	SYSCTL_RCGCADC_R |= 0x01;   // activate ADC0
	delay = SYSCTL_RCGC2_R;
	GPIO_PORTE_DIR_R = ~0x04;		// Set PE2 as input
	GPIO_PORTE_DEN_R &= ~0x04;		// Disable digital pins PE2
	GPIO_PORTE_AMSEL_R |= 0x04;	// Enable analog function for PE2
	GPIO_PORTE_AFSEL_R |= 0x04;	// Enable alternate function for PE2
	GPIO_PORTE_PCTL_R = 0x00;		// GPIO clear bit PCTL
	/*ADC0 initialization*/
	ADC0_PC_R &= ~0xF;
	ADC0_PC_R |= 0x1;           //Set ADC0 rate for 125K sample/sec
	ADC0_SSPRI_R = 0x0123;      //Sequence 3 is highest priority 
	ADC0_ACTSS_R &= ~0x08;      //disable sample sequencer 3 for initialization
	ADC0_EMUX_R &= ~0xF000;     //seq3 is software trigger
  ADC0_SSMUX3_R &= ~0x000F;
  ADC0_SSMUX3_R += 1;         //set channel
  ADC0_SSCTL3_R = 0x0006;     //no TS0 D0, yes IE0 END0
  ADC0_IM_R &= ~0x0008;       //disable SS3 interrupts
  ADC0_ACTSS_R |= 0x0008;     //enable sample sequencer 3
}


//------------ADC0_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
unsigned long ADC0_In(void)
{  
	unsigned long result;
  ADC0_PSSI_R = 0x0008;							// 1) initiate SS3
  while(!(ADC0_RIS_R & 0x08));			// 2) wait for conversion done
  result = ADC0_SSFIFO3_R & 0xFFF;	// 3) read result
  ADC0_ISC_R = 0x0008;							// 4) acknowledge completion

  return result;
}
