#include <raylib.h>
#include "defs.h"

void draw_pause_menu(unsigned char pause_menu_cursor) {
	Rectangle pause_menu_select_rect = {
		(WIN_W - 256) / 2,
		#if defined(__SWITCH__)
			(WIN_H - 64) / 2 + (pause_menu_cursor * 32),
		#else
			(WIN_H - 96) / 2 + (pause_menu_cursor * 32),
		#endif
		256,
		30
	};																			/* cursor rect */

	DrawRectangle((WIN_W - 512) / 2, (WIN_H - 128) / 2, 512, 128, RAYGHOST);	/* pause menu background */
	#if defined(__SWITCH__)
		/* Note: a console game shall not provide an "exit to OS" option */
		DrawText("Contiue",(WIN_W - 128) / 2, (WIN_H - 64) / 2, 32, BLACK);
		DrawText("Main menu",(WIN_W - 128) / 2, (WIN_H) / 2, 32, BLACK);
	#else
		DrawText("Contiue",(WIN_W - 128) / 2, (WIN_H - 96) / 2, 32, BLACK);
		DrawText("Main menu",(WIN_W - 128) / 2, (WIN_H - 32) / 2, 32, BLACK);
		DrawText("Exit",(WIN_W - 128) / 2, (WIN_H + 32) / 2, 32, BLACK);
	#endif
	DrawRectangleRoundedLinesEx(pause_menu_select_rect, FRAME_ROUNDNESS, 1, 4, BLACK);
}
