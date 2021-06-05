/*	Author: ejack023
 *	 *	Lab Section: 023
 *	  *	Assignment: Lab #10  Exercise #4
 *	   *
 *	    *	I acknowledge all content contained herein, excluding template or example
 *	     *	code, is my own original work.
 *	      *
 *	       *	Demo Link: https://www.youtube.com/watch?v=Jv_eFe8WnDE&ab_channel=EthanJackson
 *	        */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "Global.c"
#include "Lights.c"
#include "Audio.c"
#include "Matrix.c"
#include "Combine.c"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

typedef struct _task {
	signed char state;
	unsigned long int period;
	unsigned long int elapsedTime;
	int (*TickFct) (int);
} task;

int main(void)
{
	DDRA = 0xFF; PORTA = 0x00;
	DDRB = 0x40; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0x7F; PORTD = 0x80;
	
	unsigned short period = 1;
	
	TimerSet(period);
	TimerOn();
	
	task tasks[4];
	const unsigned char tasksNum = 4;
	unsigned char i = 0;
	tasks[i].state = matrix_waitStart;
	tasks[i].period = 10;
	tasks[i].elapsedTime = 10;
	tasks[i].TickFct = &matrixSMTick;
	++i;
	tasks[i].state = lights_waitStart;
	tasks[i].period = 50;
	tasks[i].elapsedTime = 50;
	tasks[i].TickFct = &lightsSMTick;
	++i;
	tasks[i].state = sound_waitStart;
	tasks[i].period = 50;
	tasks[i].elapsedTime = 50;
	tasks[i].TickFct = &soundSMTick;
	++i;
	tasks[i].state = combine_waitStart;
	tasks[i].period = period;
	tasks[i].period = period;
	tasks[i].TickFct = &combineSMTick;
	
	//PORTA = 0xFF;
	//PWM_on();
	//set_PWM(440);
	//start=1;
	//won=1;
	//lost=1;
	while(1) {
		for (i = 0; i < tasksNum; ++i) {
			 if ( tasks[i].elapsedTime >= tasks[i].period ) { 
				 tasks[i].state = tasks[i].TickFct(tasks[i].state);
				 tasks[i].elapsedTime = 0;
			 }
			 tasks[i].elapsedTime += period;
		}
		while(!TimerFlag); 
		TimerFlag = 0;
	}
}