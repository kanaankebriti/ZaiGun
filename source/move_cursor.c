#include "defs.h"

// returns new cursor position
// TODO: emlimination of direction by introduction of negative numbers for steps
char move_cursor(unsigned char cursor_pos, char dir, char steps) {
	switch (dir) {
	case CURSOR_MOV_R:
		if (cursor_pos + steps >= FIELD_W)										// respect boundry
			cursor_pos = FIELD_W - 1;											// full stop at the edge
		else
			cursor_pos += steps;												// move cursor right
		break;
	case CURSOR_MOV_L:
		if (cursor_pos - steps < 0)												// respect boundry
			cursor_pos =  0;													// full stop at the edge
		else																	// teleport left
			cursor_pos -= steps;												// move cursor left
		break;
	}
	return cursor_pos;
}
