#ifndef MATRIX_C
#define MATRIX_C	

#include "Global.c"
#include <math.h>

unsigned char btn;
enum matrix_States {matrix_waitStart, matrix_initGame, matrix_moveLeft, matrix_moveRight, matrix_nextLevel, matrix_nextLevelRelease, matrix_gameWon} matrix_State;
int matrixSMTick(int state) {
	btn = ~PIND & 0x80;
	
	static unsigned char gameSpeed = 4;
	static unsigned char gameTimer = 0;
	
	static unsigned char length = 0;
	static unsigned char i;
	static unsigned char tmp;
	static unsigned char low;
	static unsigned char high;
	
	static unsigned char moveLeft = 0;
	static unsigned char moveRight = 0;
	
	switch (state) {
		case matrix_waitStart:
			if (btn) {
				state = matrix_initGame;
				pattern = 0x00;
				row=0xF1;
				i=0;
				length=0;
				start=1;
			}
			break;
		case matrix_initGame:
			if (initDone) {
				state = matrix_moveLeft;
				level=0;
				moveLeft = 1;
				moveRight = 0;
				initDone=0;
				gameSpeed = 12;
				gameTimer = 0;
				patterns[0] = 0x00;
				patterns[1] = 0x01;
				patterns[2] = 0x00;
				patterns[3] = 0x00;
				patterns[4] = 0x00;
			}
			break;
		case matrix_moveLeft:
			if (moveRight) {
				state = matrix_moveRight;
				moveLeft = 0;
			}
			if (btn) {
				state = matrix_nextLevel;
			}
			break;
		case matrix_moveRight:
			if (moveLeft) {
				state = matrix_moveLeft;
				moveRight = 0;
			}
			if (btn) {
				state = matrix_nextLevel;
			}
			break;
		case matrix_nextLevel:
			state = matrix_nextLevelRelease;
			break;
		case matrix_nextLevelRelease:
			if (!btn) {
				if (level > 7) { 
					if (songPlaying) {
						state = matrix_gameWon; 
						patterns[0] = 0x3C;
						patterns[1] = 0x02;
						patterns[2] = 0x0C;
						patterns[3] = 0x02;
						patterns[4] = 0x3C;
					}
					won = 1; 
				}
				else if (moveLeft) state = matrix_moveLeft;
				else state = matrix_moveRight;
			}
			break;
		case matrix_gameWon:
			if (!songPlaying) { 
				state = matrix_waitStart;
				pattern = 0x80;
				row = 0xFE;
				won = 0;
				start = 0;
				patterns[0] = 0x00;
				patterns[1] = 0x00;
				patterns[2] = 0x00;
				patterns[3] = 0x00;
				patterns[4] = 0x00;
			}
			break;
		default:
			state = matrix_waitStart;
			break;
	}
	switch (state) {
		case matrix_waitStart:
			if (gameTimer <= 0) {
				if (row == 0xEF && pattern == 0x01) {
					pattern = 0x80;
					row = 0xFE;
				} else if (pattern == 0x01) {
					pattern = 0x80;
					row	= (row << 1 ) | 0x01;
				} else {
					pattern >>= 1;
				}
				gameTimer = gameSpeed;
			} else {
			--gameTimer;
			}
			break;
		case matrix_initGame:
			if (gameTimer <= 0) {
				++i;
				if (pattern == 0x00) pattern = 0x80;
				else if (pattern != 0xFF && i<8) pattern = (pattern >> 1) | 0x80;
				else {
					pattern >>= 1;
					if (pattern==0x01) {initDone = 1; i = 0;}
				}
				gameTimer = gameSpeed;
			} else {
				--gameTimer;
			}
			break;
		case matrix_moveLeft:
			tmp = 1;
			for (i = 0; i < level; ++i) tmp *= 2;
			if (gameTimer <= 0) {
				i = 4;
				while (!(patterns[i] & tmp)) {		//find last empty row
					--i;
				}
				high = i;
				if (i >= 3) moveRight = 1;
				i = 0;
				while (!(patterns[i] & tmp)) {		//find first nonempty row
					++i;
				}
				low = i;
				for (i = 0; i <= 4; ++i) {
					if (i <= high + 1 && i > low) patterns[i] |= tmp;
					else patterns[i] &= ~tmp; 
				}
				gameTimer = gameSpeed;
			} else {
				--gameTimer;
			}
			break;
		case matrix_moveRight:
			tmp = 1;
			for (i = 0; i < level; ++i) tmp *= 2;
			if (gameTimer <= 0) {
				i = 0;
				while (!(patterns[i] & tmp)) {		//find last empty row
					++i;
				}
				low = i;
				if (i <= 1) moveLeft = 1;
				i = 4;
				while (!(patterns[i] & tmp)) {		//find first nonempty row
					--i;
				}
				high = i;
				for (i = 0; i <= 4; ++i) {
					if (i < high && i >= low - 1) patterns[i] |= tmp;
					else patterns[i] &= ~tmp;
				}
				gameTimer = gameSpeed;
			} else {
				--gameTimer;
			}
			break;
		case matrix_nextLevel:
			++level;
			--gameSpeed;
			tmp = 1;
			for (i = 0; i < level; ++i) tmp *= 2;
			patterns[1] |= tmp;
			//patterns[2] |= tmp;
			//patterns[3] |= tmp;
			break;
		case matrix_gameWon:
			break;
		default:
			break;
	}
	return state;
}

#endif