// MeasurementOfDistance.c
// Runs on LM4F120/TM4C123
// Use SysTick interrupts to periodically initiate a software-
// triggered ADC conversion, convert the sample to a fixed-
// point decimal distance, and store the result in a mailbox.
// The foreground thread takes the result from the mailbox,
// converts the result to a string, and prints it to the
// Nokia5110 LCD.  The display is optional.
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

// Slide pot pin 3 connected to +3.3V
// Slide pot pin 2 connected to PE2(Ain1) and PD3
// Slide pot pin 1 connected to ground


#include "ADC.h"
#include "..//tm4c123gh6pm.h"
#include "Nokia5110.h"
#include "TExaS.h"

void EnableInterrupts(void);  // Enable interrupts

unsigned char String[10]; // null-terminated ASCII string
unsigned long Distance;   // units 0.001 cm
unsigned long ADCdata;    // 12-bit 0 to 4095 sample
unsigned long Flag;       // 1 means valid Distance, 0 means Distance is empty

//********Convert****************
// Convert a 12-bit binary ADC sample into a 32-bit unsigned
// fixed-point distance (resolution 0.001 cm).  Calibration
// data is gathered using known distances and reading the
// ADC value measured on PE1.  
// Overflow and dropout should be considered 
// Input: sample  12-bit ADC sample
// Output: 32-bit distance (resolution 0.001cm)
unsigned long Convert(unsigned long sample)
{
  return (unsigned long)(sample * 0.48828125 + 1);
}

void SysTick_Init(unsigned long period)
{
    NVIC_ST_CTRL_R = 0;				// disable SysTick during setup
    NVIC_ST_RELOAD_R = period-1;	// number of counts to wait
    NVIC_ST_CURRENT_R = 0;      	// any write to current clears it
    NVIC_ST_CTRL_R |= 0x07;			// enable SysTick with core clock
}

// Initialize SysTick interrupts to trigger at 40 Hz, 25 ms
void SysTick_Handler(void)
{ 
    GPIO_PORTF_DATA_R ^= 0x02;
    GPIO_PORTF_DATA_R ^= 0x02;
    ADCdata = ADC0_In();
    Distance = Convert(ADCdata);
    Flag = 1;
    GPIO_PORTF_DATA_R ^= 0x02;
}

void PortF_Init(void)
{ 
		volatile unsigned long delay;
    SYSCTL_RCGC2_R |= 0x20;            // 1) F clock
		delay = SYSCTL_RCGC2_R;            // delay to allow clock to stabilize 
    GPIO_PORTF_AMSEL_R &= 0x00;        // 2) disable analog function
    GPIO_PORTF_PCTL_R &= 0x00000000;   // 3) GPIO clear bit PCTL  
    GPIO_PORTF_DIR_R |= 0x02;          // 4) PF1 output  
    GPIO_PORTF_AFSEL_R &= 0x00;        // 5) no alternate function       
    GPIO_PORTF_DEN_R |= 0x02;          // 7) enable digital pins PF1
}

//-----------------------UART_ConvertDistance-----------------------
// Converts a 32-bit distance into an ASCII string
// Input: 32-bit number to be converted (resolution 0.001cm)
// Output: store the conversion in global variable String[10]
// Fixed format 1 digit, point, 3 digits, space, units, null termination
// Examples
//    4 to "0.004 cm"  
//   31 to "0.031 cm" 
//  102 to "0.102 cm" 
// 2210 to "2.210 cm"
//10000 to "*.*** cm"  any value larger than 9999 converted to "*.*** cm"
void UART_ConvertDistance(unsigned long n)
{
	const unsigned long div[] = {100, 10};
	
	memset(String, 0, sizeof String);
	strcpy(String, "*.*** cm");
	
	if (n < 10000) {
		unsigned tmp = n / 1000;
		unsigned i;
		String[0] = tmp + '0';
		n -= tmp * 1000;
		for (i = 0; i < 2; ++i) {
			tmp = n / div[i];
			String[2 + i] = tmp + '0';
			n -= tmp * div[i];
		}
		String[4] = n + '0';
	}
}

// main1 is a simple main program allowing you to debug the ADC interface
int main1(void){ 
  TExaS_Init(ADC0_AIN1_PIN_PE2, UART0_Emulate_Nokia5110_NoScope);
  ADC0_Init();    // initialize ADC0, channel 1, sequencer 3
  EnableInterrupts();
  while(1){ 
    ADCdata = ADC0_In();
  }
}
// once the ADC is operational, you can use main2 to debug the convert to distance
int main2(void){ 
  TExaS_Init(ADC0_AIN1_PIN_PE2, UART0_Emulate_Nokia5110_NoScope);
  ADC0_Init();    // initialize ADC0, channel 1, sequencer 3
  Nokia5110_Init();             // initialize Nokia5110 LCD
  EnableInterrupts();
  while(1){ 
    ADCdata = ADC0_In();
    Nokia5110_SetCursor(0, 0);
    Distance = Convert(ADCdata);
    UART_ConvertDistance(Distance); // from Lab 11
    Nokia5110_OutString(String);    // output to Nokia5110 LCD (optional)
  }
}
// once the ADC and convert to distance functions are operational,
// you should use this main to build the final solution with interrupts and mailbox
int main(void)
{ 
    volatile unsigned long delay;
    TExaS_Init(ADC0_AIN1_PIN_PE2, SSI0_Real_Nokia5110_Scope);
    // initialize ADC0, channel 1, sequencer 3
    // initialize Nokia5110 LCD (optional)
    // initialize SysTick for 40 Hz interrupts
    // initialize profiling on PF1 (optional)
    // wait for clock to stabilize
    ADC0_Init();
    Nokia5110_Init();
    SysTick_Init(2000000); // 25 ms at 80 MHz
		PortF_Init();
    EnableInterrupts();
    // print a welcome message  (optional)
    while (1) {
        // read mailbox
        // output to Nokia5110 LCD (optional)
        Flag = 0;
        while (!Flag);
        UART_ConvertDistance(Distance);
				Nokia5110_SetCursor(0, 0);
				Nokia5110_OutString(String);
    }
}
