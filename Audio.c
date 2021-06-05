#ifndef AUDIO_C
#define AUDIO_C
#include "Global.c"
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

void set_PWM(double frequency) {
	static double current_frequency;
	
	if (frequency != current_frequency) {
		if(!frequency) {TCCR3B &= 0x08;}
		else {TCCR3B |= 0x03;}
		
		if (frequency < 0.954) {OCR3A = 0xFFFF;}
		
		else if (frequency > 31250) {OCR3A = 0x0000;}
		
		else {OCR3A = (short)(8000000 / (128*frequency)) - 1;}
		
		TCNT3 = 0;
		current_frequency = frequency;
	}
}

void PWM_on() {
	TCCR3A = (1 << COM3A0);
	
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	
	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}


double notes[] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25, 587.33, 659.25, 698.46, 783.99, 880.00, 987.77, 1046.50}; 
					// C4, D, E, F, G, A, B, C5, D, E, F,  G,  A,  B,  C6
					// 0,  1, 2, 3, 4, 5, 6, 7,  8, 9, 10, 11, 12, 13, 14
unsigned char startSequence[] = {7,12,11};
unsigned char startDurations[] = {2,2,4};
	
unsigned char btnSequence[2];
unsigned char btnDurations[] = {2,4};

unsigned char wonSequence[] = {12, 14, 11, 12, 9, 11, 8, 9, 7, 8, 5, 7, 4, 5, 2, 4, 1, 2, 0, 1, 2, 4, 5, 7, 8, 9, 11, 12, 14};
unsigned char wonDurations[] = {2, 2,  2,  2,  2, 2,  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  2,  2};

unsigned char lostSequence[] = {14, 13, 12, 9, 7, 6, 5};
unsigned char lostDurations[] = {6, 6,  6,  6, 6, 6, 18};

unsigned char btn;
unsigned char duration;
unsigned char seqNum;
enum sound_States { sound_waitStart, sound_start, sound_btnPress, sound_gamePlaying, sound_gameWon, sound_gameLost } sound_State;
int soundSMTick(int state) {
	btn = ~PIND & 0x80;
	switch (state) {
		case sound_waitStart:
			if (start) {
				state = sound_start; 
				PWM_on(); 
				seqNum = 0; 
				duration = (startDurations[seqNum]);
			}
			break;
		case sound_start:
			if (seqNum > 2) {
				PWM_off();
				state = sound_gamePlaying;
			}
			break;
		case sound_gamePlaying:
			if (btn) {
				state = sound_btnPress;
				PWM_on();
				seqNum = 0;
				btnSequence[0] = level+1;
				btnSequence[1] = level+6;
				duration = (btnDurations[seqNum]);
			}
			else if (won) {
				state = sound_gameWon; 
				songPlaying = 1;
				PWM_on();
				seqNum = 0;
				duration = (wonDurations[seqNum]);
			}
			else if (lost) {
				state = sound_gameLost; 
				songPlaying = 1;
				PWM_on();
				seqNum = 0;
				duration = (lostDurations[seqNum]);
			}
			break;
		case sound_btnPress:
			if (seqNum > 1) {
				PWM_off();
				state = sound_gamePlaying;
			}
			break;
		case sound_gameWon:
			if (seqNum > 28) {
				PWM_off();
				songPlaying = 0;
				state = sound_waitStart;
			}
			break;
		case sound_gameLost:
			if (seqNum > 6) {
				PWM_off();
				songPlaying = 0;
				state = sound_waitStart;
			}
			break;
		default: break;
	}
	switch (state) {
		case sound_start:
			set_PWM(notes[startSequence[seqNum]]);
			--duration;
			if (duration <= 0) {
				duration = (startDurations[++seqNum]);
			}
			break;
		case sound_btnPress:
			set_PWM(notes[btnSequence[seqNum]]);
			--duration;
			if (duration <= 0) {
				duration = (btnDurations[++seqNum]);
			}
			break;
		case sound_gameWon:
			set_PWM(notes[wonSequence[seqNum]]);
			--duration;
			if (duration <= 0) {
				duration = (wonDurations[++seqNum]);
			}
			break;
		case sound_gameLost:
			set_PWM(notes[lostSequence[seqNum]]);
			--duration;
			if (duration <= 0) {
				duration = (lostDurations[++seqNum]);
			}
			break;
		default: break;
	}
	return state;
};

#endif