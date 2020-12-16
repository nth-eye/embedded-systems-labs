// TuningFork.c Lab 12
// Runs on LM4F120/TM4C123
// Use SysTick interrupts to create a squarewave at 440Hz.  
// There is a positive logic switch connected to PA3, PB3, or PE3.
// There is an output on PA2, PB2, or PE2. The output is 
//   connected to headphones through a 1k resistor.
// The volume-limiting resistor can be any value from 680 to 2000 ohms
// The tone is initially off, when the switch goes from
// not touched to touched, the tone toggles on/off.
//                   |---------|               |---------|     
// Switch   ---------|         |---------------|         |------
//
//                    |-| |-| |-| |-| |-| |-| |-|
// Tone     ----------| |-| |-| |-| |-| |-| |-| |---------------
//
// Daniel Valvano, Jonathan Valvano
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


#include "TExaS.h"
#include "..//tm4c123gh6pm.h"

unsigned long toggled = 0;
unsigned long prev_state = 0x08;

// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void WaitForInterrupt(void);  // low power mode

// input from PA3, output from PA2, SysTick interrupts
void Sound_Init(void)
{ 
	unsigned long volatile delay;
	
  SYSCTL_RCGC2_R |= 0x00000001; // activate port A
  delay = SYSCTL_RCGC2_R;

	GPIO_PORTA_DIR_R = ~0x08;		// Set PA3 as input
	GPIO_PORTA_DEN_R = 0x0C;		// Enable digital pins PA2-PA3
	GPIO_PORTA_AMSEL_R = 0x00;	// Disable analog function for PA2-PA3
	GPIO_PORTA_AFSEL_R = 0x00;	// Disable alternate function for PA2-PA3
	GPIO_PORTA_PCTL_R = 0x00;		// GPIO clear bit PCTL
	
	NVIC_ST_CTRL_R = 0;					// disable SysTick during setup
	NVIC_ST_RELOAD_R = 90908;		// number of counts to wait
	NVIC_ST_CURRENT_R = 0;      // any write to current clears it
	NVIC_ST_CTRL_R |= 0x07;			// enable SysTick with core clock
}

// called at 880 Hz
void SysTick_Handler(void)
{
	unsigned long state = GPIO_PORTA_DATA_R & 0x08;
	
	if (!prev_state && state != prev_state)
		toggled ^= 1;
	prev_state = state;
	
	if (toggled)
		GPIO_PORTA_DATA_R ^= 0x04;	// toggle PA2
	else
		GPIO_PORTA_DATA_R = 0x00;   // disable PA2
}

int main(void) 
{
	// activate grader and set system clock to 80 MHz
  TExaS_Init(SW_PIN_PA3, HEADPHONE_PIN_PA2,ScopeOn); 
  Sound_Init();       
  EnableInterrupts();   // enable after all initialization are done
	
  while(1) {
    // main program is free to perform other tasks
    // do not use WaitForInterrupt() here, it may cause the TExaS to crash
  }
}
