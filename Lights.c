#ifndef LIGHTS_C
#define LIGHTS_C
#include "Global.c"

enum lights_States { lights_waitStart, lights_gamePlaying, lights_gameWon, lights_gameLost } lights_State;
int lightsSMTick (int state) {
	lightsOutput = PORTA;
	switch (state) {
		case lights_waitStart:
			if (start) state = lights_gamePlaying;
			break;
		case lights_gamePlaying:
			if (won && songPlaying) {state = lights_gameWon; lightsOutput = 0x08;}
			else if (lost && songPlaying) {state = lights_gameLost; lightsOutput = 0x10;}
			break;
		case lights_gameWon:
		case lights_gameLost:
			if (!songPlaying) {state = lights_waitStart; lightsOutput = 0x00;}
			break;
		default:
			break;
	}
	switch(state) {
		case lights_gamePlaying:
			switch(level) {
				case 1:
					lightsOutput = 0x80;
					break;
				case 2:
					lightsOutput = 0xC0;
					break;
				case 3:
					lightsOutput = 0xE0;
					break;
				case 4:
					lightsOutput = 0xF0;
					break;
				case 5:
					lightsOutput = 0xF8;
					break;
				case 6:
					lightsOutput = 0xFC;
					break;
				case 7:
					lightsOutput = 0xFE;
					break;
				case 8:
					lightsOutput = 0xFF;
					break;
				default:
					break;
			}
			break;
		case lights_gameWon:
			if (lightsOutput != 0x01) {
				lightsOutput >>= 1;
			}
			else lightsOutput = 0x08;
			break;
		case lights_gameLost:
			if (lightsOutput != 0x80) {
				lightsOutput <<= 1;
			}
			else lightsOutput = 0x10;
			break;
		default:
			break;
	}
	return state;	
};

#endif