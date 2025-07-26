#include "defs.h"
#include "common.h"

unsigned char get_cmd(void) {
	/* determine input source */
	/* nintendo switch */
	#if defined(__SWITCH__)
	static char cursor_p1_lck = 0;											/* player 1 analog joystick lock mechanism */
	static char cursor_p2_lck = 0;											/* player 2 analog joystick lock mechanism */
	/* multi player or single player with only one rumble! */
	if (IsGamepadAvailable(0) && IsGamepadAvailable(1)) {											/* handle gamepad */
		/* player 1 */
		if (cursor_p1_lck == 0) {
			if (GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_Y) > 0.9f) {	/* cursor to right */
				return CMD_CURSOR_RIGHT_P1;
				cursor_p1_lck = JOY_ULCK_SPD;								/* lock jostick */
			}
			else if (GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_Y) < -0.9f) {	/* cursor to left */
				return CMD_CURSOR_LEFT_P1;
				cursor_p1_lck = JOY_ULCK_SPD;								/* lock jostick */
			}
		} else {
				cursor_p1_lck--;											/* gradually unlock joystick */
		}
		if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_UP))	/* perform elimination */
			return CMD_ELIMINATE_P1;
		else if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_2))	/* perform jump right */
			return CMD_CURSOR_JRIGHT_P1;
		else if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_TRIGGER_2))	/* perform jump left */
			return CMD_CURSOR_JLEFT_P1;

		/* player 2 */
		if (cursor_p2_lck == 0) {
			if (GetGamepadAxisMovement(1, GAMEPAD_AXIS_LEFT_Y) > 0.9f) {	/* cursor to left */
				return CMD_CURSOR_LEFT_P2;
				cursor_p2_lck = JOY_ULCK_SPD;								/* lock jostick */
			}
			else if (GetGamepadAxisMovement(1, GAMEPAD_AXIS_LEFT_Y) < -0.9f) {	/* cursor to right */
				return CMD_CURSOR_RIGHT_P2;
				cursor_p2_lck = JOY_ULCK_SPD;								/* lock jostick */
			}
		} else {
				cursor_p2_lck--;											/* gradually unlock joystick */
		}
		if (IsGamepadButtonPressed(1, GAMEPAD_BUTTON_LEFT_FACE_DOWN))/* perform elimination */
			return CMD_ELIMINATE_P2;
		else if (IsGamepadButtonPressed(1, GAMEPAD_BUTTON_RIGHT_TRIGGER_2))	/* perform jump right */
			return CMD_CURSOR_JRIGHT_P2;
		else if (IsGamepadButtonPressed(1, GAMEPAD_BUTTON_LEFT_TRIGGER_2))	/* perform jump left */
			return CMD_CURSOR_JLEFT_P2;
	/* single player */
	} else {
		if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT))		/* cursor to right */
			return CMD_CURSOR_RIGHT_P1;
		else if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT))	/* cursor to left */
			return CMD_CURSOR_LEFT_P1;
		else if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT))/* perform elimination */
			return CMD_ELIMINATE_P1;
		else if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_2))	/* perform jump right */
			return CMD_CURSOR_JRIGHT_P1;
		else if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_TRIGGER_2))	/* perform jump left */
			return CMD_CURSOR_JLEFT_P1;
		else if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_UP))			/* cursor to down */
			return CMD_CURSOR_UP_P1;
		else if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN))	/* cursor to up */
			return CMD_CURSOR_DOWN_P1;
		else if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT))	/* stop/resume game flow */
			return CMD_PAUSE;
	}
	#else
	/* PC */
	switch (GetKeyPressed()) {												/* handle keyboard */
		/* player 1 */
		case KEY_D:				return CMD_CURSOR_RIGHT_P1;						/* cursor to right */
		case KEY_A:				return CMD_CURSOR_LEFT_P1;						/* cursor to left */
		case KEY_W:				return CMD_CURSOR_JRIGHT_P1;					/* cursor jump right */
		case KEY_S:				return CMD_CURSOR_JLEFT_P1;						/* cursor jump left */
		case KEY_SPACE:			return CMD_ELIMINATE_P1;						/* perform elimination */
		/* player 2*/
		case KEY_RIGHT:			return CMD_CURSOR_RIGHT_P2;						/* cursor to right */
		case KEY_LEFT:			return CMD_CURSOR_LEFT_P2;						/* cursor to left */
		case KEY_UP:			return CMD_CURSOR_JRIGHT_P2;					/* cursor jump right */
		case KEY_DOWN:			return CMD_CURSOR_JLEFT_P2;						/* cursor jump left */
		case KEY_RIGHT_CONTROL:	return CMD_ELIMINATE_P2;						/* perform elimination */

		case KEY_ESCAPE:		return CMD_PAUSE;								/* stop/resume game flow */
	}
	#endif
	return CMD_NULL;
}