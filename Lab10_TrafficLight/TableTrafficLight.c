// ***** 0. Documentation Section *****
// TableTrafficLight.c for Lab 10
// Runs on LM4F120/TM4C123
// Index implementation of a Moore finite state machine to operate a traffic light.  
// Daniel Valvano, Jonathan Valvano
// January 15, 2016

// east/west red light connected to PB5
// east/west yellow light connected to PB4
// east/west green light connected to PB3
// north/south facing red light connected to PB2
// north/south facing yellow light connected to PB1
// north/south facing green light connected to PB0
// pedestrian detector connected t PE2 (1=pedestrian present)
// north/south car detector connected to PE1 (1=car present)
// east/west car detector connected to PE0 (1=car present)
// "walk" light connected to PF3 (built-in green LED)
// "don't walk" light connected to PF1 (built-in red LED)

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"

// ***** 2. Global Declarations Section *****

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

void init_ports();
void SysTick_Wait(unsigned long delay); // 12.5 ns assuming 80MHz
#define SysTick_Wait10ms(delay) SysTick_Wait(delay * 800000) // assuming 80MHz

enum { GO_WEST, WAIT_WEST, GO_SOUTH, WAIT_SOUTH, WALK, STOP_ON_1, STOP_OFF_1, STOP_ON_2, STOP_OFF_2 };

typedef struct {
	unsigned long time;
	unsigned long out[2]; // PORTB and PORTF
	unsigned long next[8];
} State;

const State FSM[] = {
	{500, {0x0C, 0x2}, {GO_WEST, GO_WEST, WAIT_WEST, GO_WEST, WAIT_WEST, GO_WEST, WAIT_WEST, WAIT_WEST}},
	{500, {0x14, 0x2}, {GO_WEST, GO_WEST, GO_SOUTH, GO_SOUTH, WALK, WALK, GO_SOUTH, GO_SOUTH}},
	{500, {0x21, 0x2}, {GO_SOUTH, WAIT_SOUTH, GO_SOUTH, GO_SOUTH, WAIT_SOUTH, WAIT_SOUTH, GO_SOUTH, WAIT_SOUTH}},
	{500, {0x22, 0x2}, {GO_SOUTH, GO_WEST, GO_SOUTH, GO_WEST, WALK, GO_WEST, WALK, WALK}},
	
	{500, {0x24, 0x8}, {WALK, STOP_ON_1, STOP_ON_1, STOP_ON_1, WALK, WALK, WALK, STOP_ON_1}},
	
	{500, {0x24, 0x2}, {STOP_OFF_1, STOP_OFF_1, STOP_OFF_1, STOP_OFF_1, STOP_OFF_1, STOP_OFF_1, STOP_OFF_1, STOP_OFF_1}},
	{500, {0x24, 0x0}, {STOP_ON_2, STOP_ON_2, STOP_ON_2, STOP_ON_2, STOP_ON_2, STOP_ON_2, STOP_ON_2, STOP_ON_2}},
	{500, {0x24, 0x2}, {STOP_OFF_2, STOP_OFF_2, STOP_OFF_2, STOP_OFF_2, STOP_OFF_2, STOP_OFF_2, STOP_OFF_2, STOP_OFF_2}},
	{500, {0x24, 0x0}, {WALK, GO_WEST, GO_SOUTH, GO_WEST, WALK, GO_WEST, GO_SOUTH, GO_WEST}}
};

unsigned long current_state = GO_WEST;

// ***** 3. Subroutines Section *****

int main(void) 
{ 
	TExaS_Init(SW_PIN_PE210, LED_PIN_PB543210,ScopeOff); // activate grader and set system clock to 80 MHz
	EnableInterrupts();

	init_ports();

	while (1) {
		GPIO_PORTB_DATA_R = FSM[current_state].out[0];
		GPIO_PORTF_DATA_R = FSM[current_state].out[1];
		SysTick_Wait10ms(FSM[current_state].time);
		current_state = FSM[current_state].next[GPIO_PORTE_DATA_R]; 
	}
}

void init_ports()
{
	SYSCTL_RCGC2_R |= (0x20 | 0x10 | 0x02); // Enable clock on F E B

	GPIO_PORTB_DIR_R = 0x3F;		// Set PB0-PB5 as output
	GPIO_PORTB_DEN_R = 0x3F;		// Enable digital pins PB0-PB5
	GPIO_PORTB_AMSEL_R = 0x00;	// Disable analog function for PB0-PB5	
	GPIO_PORTB_AFSEL_R = 0x00;	// Disable alternate function for PB0-PB5
	GPIO_PORTB_PCTL_R = 0x00;		// GPIO clear bit PCTL

	GPIO_PORTE_DIR_R = ~0x07;		// Set PE0-P02 as input
	GPIO_PORTE_DEN_R = 0x07;		// Enable digital pins PE0-PE2 
	GPIO_PORTE_AMSEL_R = 0x00;	// Disable analog function for PE0-P02	
	GPIO_PORTE_AFSEL_R = 0x00;	// Disable alternate function for PE0-P02
	GPIO_PORTE_PCTL_R = 0x00;		// GPIO clear bit PCTL 

	GPIO_PORTF_DIR_R = 0x0A;		// Set PF1 and PF3 as input
	GPIO_PORTF_DEN_R = 0x0A;		// Enable digital pins PF1 and PF3 
	GPIO_PORTF_AMSEL_R = 0x00;	// Disable analog function for PF1 and PF3	
	GPIO_PORTF_AFSEL_R = 0x00;	// Disable alternate function for PF1 and PF3
	GPIO_PORTF_PCTL_R = 0x00;		// GPIO clear bit PCTL 
}

void SysTick_Wait(unsigned long delay)
{
	NVIC_ST_CTRL_R = 0;					// disable SysTick during setup
	NVIC_ST_RELOAD_R = delay-1;	// number of counts to wait
	NVIC_ST_CTRL_R |= 0x5;			// enable SysTick with core clock

	while(!(NVIC_ST_CTRL_R & (1 << 16)));	// wait for COUNT flag
	
	NVIC_ST_CTRL_R = 0;	// disable SysTick Timer
}

