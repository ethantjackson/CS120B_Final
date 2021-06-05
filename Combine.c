#ifndef COMBINE_C
#define COMBINE_C

#include "Global.c"

enum combine_States {combine_waitStart, combine_displayPatterns} combine_State;
int combineSMTick(int state) {
	static unsigned char i;
	
	switch(state) {
		case combine_waitStart:
			if (start && initDone) {
				state = combine_displayPatterns;
				i = 0;
				row = 0xFF;
			}
			break;
		case combine_displayPatterns:
			if (!start) state = combine_waitStart;
			break;
		default: break;
	}
	switch(state) {
		case combine_waitStart:
			
			break;
		case combine_displayPatterns:
			row <<= 1;
			if (i != 0) row |= 0x01;
		    pattern = patterns[i];
			++i;
			if (i >= 5) i = 0;
			break;
		default: break;
	}
	PORTC = pattern;
	PORTD = row;
	PORTA = lightsOutput;
	return state;
};

#endif