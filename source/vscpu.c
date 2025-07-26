#include "defs.h"
#include "common.h"

bool vscpu(const Sound* snfx) {
	char game_state = GAME_STAT_RUN;											// game flow state (run, pause, end)
	int captials[26] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
	field block_p1[FIELD_H][FIELD_W];											// player 1 blocks
	field block_p2[FIELD_H][FIELD_W];											// player 2 blocks
	register unsigned char i, j;												// loop counters
	signed char palette_p1[FIELD_W] = { 0 };									// random chars set of headline for player 1
	signed char palette_p2[FIELD_W] = { 0 };									// random chars set of headline for player 2
	signed char select_p1 = 0;													// selected char from palette_p1 for player 1
	signed char select_p2 = 0;													// selected char from palette_p2 for player 2
	unsigned char cursor_pos_p1 = 0;											// player 1 cursor position
	unsigned char cursor_pos_p2 = 0;											// player 1 cursor position
	char cmd_from_input = CMD_NULL;												// input command. in order to handle gamepad as well.
	char field_clear_bonus_p1 = 0;												// player 1 has cleared the entire fiels
	char field_clear_bonus_p2 = 0;												// player 2 has cleared the entire fiels
	short score = 0;
	double delta = 0;															// keeps time flow. init value ensures immediate start.
	Vector2 block_pos;				//TODO: perhaps eliminate this to			// temporary position
	const Rectangle field_frame_rect_p1 = {
		LEFT_MARGIN_MUL_P1,
		TOP_MARGIN,
		BLOCK_SIZE * FIELD_W - BLOCK_MARGIN,
		BLOCK_SIZE * FIELD_H - BLOCK_MARGIN,
	};																			// player 1 field frame
	const Rectangle field_frame_rect_p2 = {
		LEFT_MARGIN_MUL_P2,
		TOP_MARGIN,
		BLOCK_SIZE * FIELD_W - BLOCK_MARGIN,
		BLOCK_SIZE * FIELD_H - BLOCK_MARGIN,
	};																			// player 2 field frame


	#if defined(__SWITCH__)
		char cursor_p1_lck = 0;														// player 1 analog joystick lock mechanism
		char cursor_p2_lck = 0;														// player 2 analog joystick lock mechanism
	#endif


	/********
	 * init *
	 ********/
	//--------------------------------------------------------------------------
	const Font deffont = GetFontDefault();
	const Font blkfont = LoadFontEx(
			RESOURCE_PATH"ProcrastinatingPixie-WyVOO.ttf",
			128,
			captials,
			26
		);																		// fonts dedicated to glyphs on blocks
	const char font_w = (BLOCK_SIZE / deffont.baseSize) * deffont.recs->width;

	/* initilize field blocks*/
	/* player 1 */
	for (j = 0; j < FIELD_H; j++)
		for (i = 0; i < FIELD_W; i++) {
			/* compute blocks position */
			block_p1[j][i].x_pos = i * BLOCK_SIZE + LEFT_MARGIN_MUL_P1;
			block_p1[j][i].y_pos = j * BLOCK_SIZE + TOP_MARGIN;
			block_p1[j][i].val = BLOCK_TYPE_FREE;
		}
	/* player 2 */
	for (j = 0; j < FIELD_H; j++)
		for (i = 0; i < FIELD_W; i++) {
			/* compute blocks position */
			block_p2[j][i].x_pos = i * BLOCK_SIZE + LEFT_MARGIN_MUL_P2;
			block_p2[j][i].y_pos = j * BLOCK_SIZE + TOP_MARGIN;
			block_p2[j][i].val = BLOCK_TYPE_FREE;
		}

	/* intro waiting */
/*	for (signed char wait = 3; wait >= 0; wait--) {								// signed is required by arm64!
		BeginDrawing();
		ClearBackground(DEFBACKCOLOR);
		// draw frames
		draw_frame(field_frame_rect_p1, COLOR_P1);
		draw_frame(field_frame_rect_p2, COLOR_P2);
		// draw fields
		draw_blocks(cursor_pos_p1, block_p1, blkfont, font_w);
		draw_blocks(cursor_pos_p2, block_p2, blkfont, font_w);
		draw_cursor(cursor_pos_p1, LEFT_MARGIN_MUL);							// draw cursor for player 1
		draw_cursor(cursor_pos_p2, WIN_W / 2 + LEFT_MARGIN_MUL);				// draw cursor for player 2
		// draw countdown
		DrawText(
			TextFormat("%d", wait),
			WIN_W / 2 - font_w,													// horizontal alignment
			TOP_MARGIN + (FIELD_W * BLOCK_SIZE) / 2 + 100,						// vertical alignment
			100,
			GREEN
		);
		EndDrawing();
		PlaySound(snfx[SNFX_WA]);												// play wait sound effect
		WaitTime(1.f);
	}
	PlaySound(snfx[SNFX_EL]);													// play elimination sound effect as the game begins!
	WaitTime(0.5f);
*/
	select_p1 = spaw_new_row(block_p1, palette_p1);								// spawn first row for player 1
	select_p2 = spaw_new_row(block_p2, palette_p2);								// spawn first row for player 2
	bcmx_ctl(BCMX_MULTIPLAYER_LOAD);
	bcmx_ctl(BCMX_MULTIPLAYER_PLAY);

	/*************
	 * main loop *
	 *************/
	//--------------------------------------------------------------------------
	#if defined(__SWITCH__)
		while(game_state <= GAME_STAT_PAUSE)									// consoles don't possess a window manager
	#else
		while(!WindowShouldClose() && game_state <= GAME_STAT_PAUSE)			// game_state == run or pause; faster code this way but less clear :(
	#endif
	{
		/************
	 	* game loop *
	 	************/
		if(game_state == GAME_STAT_RUN) {
			/*****************
			 * updaing logic *
			 *****************/
			//----------------------------------------------------------------------
			if (GetTime() - delta > 4.f) {
				/* move each block upward for both players */
				for (j = 1; j < FIELD_H; j++)
					for (i = 0; i < FIELD_W; i++) {
						block_p1[j - 1][i].val = block_p1[j][i].val;
						block_p2[j - 1][i].val = block_p2[j][i].val;
					}
				/* spawn new blocks for both players */
				for (i = 0; i < FIELD_W; i++)
					if (GetRandomValue(0, 100) > 80) {								// set letters occurrence rate
						block_p1[FIELD_H - 1][i].val = BLOCK_TYPE_OCCU;
						block_p2[FIELD_H - 1][i].val = BLOCK_TYPE_OCCU;
					} else {
						block_p1[FIELD_H - 1][i].val = BLOCK_TYPE_FREE;
						block_p2[FIELD_H - 1][i].val = BLOCK_TYPE_FREE;
					}
				delta = GetTime();
			}

			/* set letters at head row */
			/* TODO: requires a function to reduce redundency in source */
			/* player 1 */
			for (i = 0; i < FIELD_W; i++)
				for (j = 1; j < FIELD_H; j++) {
					char current_block_letter = block_p1[j][i].val;					// operate on this block
					if (current_block_letter >= 'A')								// skip blocks under headline
						break;
					else if ((current_block_letter == BLOCK_TYPE_OCCU) ) {
						palette_p1[i] = GetRandomValue('A', 'Z');					// save rand char to palette
						block_p1[j][i].val = palette_p1[i];							// assign rand char to block
						break;														// don't apply to blocks under eliminated block
					}
				}
			/* player 2 */
			for (i = 0; i < FIELD_W; i++)
				for (j = 1; j < FIELD_H; j++) {
					char current_block_letter = block_p2[j][i].val;					// operate on this block
					if (current_block_letter >= 'A')								// skip blocks under headline
						break;
					else if ((current_block_letter == BLOCK_TYPE_OCCU) ) {
						palette_p2[i] = GetRandomValue('A', 'Z');					// save rand char to palette
						block_p2[j][i].val = palette_p2[i];							// assign rand char to block
						break;														// don't apply to blocks under eliminated block
					}
				}
			//----------------------------------------------------------------------

			/***********
			 * drawing *
			 ***********/
			//----------------------------------------------------------------------
			BeginDrawing();
			/* stupid backround flashing! */
			if (field_clear_bonus_p1) {
				PlaySound(snfx[SNFX_FCL]);											// play field cleared sound effect
				score -= 5;															// gradually add score for more satisfaction!
				ClearBackground(CLITERAL(Color){ GetRandomValue(0, 255), GetRandomValue(0, 255), GetRandomValue(0, 255), 255 });
				field_clear_bonus_p1--;
			} else if (field_clear_bonus_p2) {
				PlaySound(snfx[SNFX_FCL]);											// play field cleared sound effect
				score += 5;															// gradually add score for more satisfaction!
				ClearBackground(CLITERAL(Color){ GetRandomValue(0, 255), GetRandomValue(0, 255), GetRandomValue(0, 255), 255 });
				field_clear_bonus_p2--;
			}
			else
				ClearBackground(DEFBACKCOLOR);
			draw_cursor(cursor_pos_p1, LEFT_MARGIN_MUL_P1);						// draw cursor for player 1
			draw_cursor(cursor_pos_p2, LEFT_MARGIN_MUL_P2);						// draw cursor for player 2

			/* draw selected character */
			block_pos.x = LEFT_MARGIN_MUL_P1 - 100;								// selected character display position
			block_pos.y = (WIN_H - 100) / 2;
			DrawTextCodepoint(blkfont, select_p1, block_pos, 100, COLOR_P1);	// draw selected character for player 1
			block_pos.x = LEFT_MARGIN_MUL_P2 - 100;
			DrawTextCodepoint(blkfont, select_p2, block_pos, 100, COLOR_P2);	// draw selected character for player 2

			/* draw field background */
			draw_frame(field_frame_rect_p1, COLOR_P1);
			draw_frame(field_frame_rect_p2, COLOR_P2);
			draw_blocks(cursor_pos_p1, block_p1, blkfont, font_w);
			draw_blocks(cursor_pos_p2, block_p2, blkfont, font_w);

			if (score != 0) {
				if (score > 0)
					DrawRectangle(WIN_W / 2, WIN_H - 45, score, 30, COLOR_P2);
				else
					DrawRectangle(score + WIN_W / 2, WIN_H - 45, score * -1, 30, COLOR_P1);
			}

			//DrawText(TextFormat("G1=%d LX=%f LY=%f RX=%f RY%f", IsGamepadAvailable(0), GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X), GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y), GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_X), GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_Y)), 0, 0, 28, RED);
			//DrawText(TextFormat("G2=%d LX=%f LY=%f RX=%f RY%f", IsGamepadAvailable(1), GetGamepadAxisMovement(1, GAMEPAD_AXIS_LEFT_X), GetGamepadAxisMovement(1, GAMEPAD_AXIS_LEFT_Y), GetGamepadAxisMovement(1, GAMEPAD_AXIS_RIGHT_X), GetGamepadAxisMovement(1, GAMEPAD_AXIS_RIGHT_Y)), 0, 30, 28, RED);
			//DrawText(TextFormat("score=%d", score), 0, 60, 28, RED);
			EndDrawing();
			//----------------------------------------------------------------------

			/****************
			 * handle input *
			 ****************/
			//----------------------------------------------------------------------
			cmd_from_input = get_cmd();

			/* perform actions */
			/* TODO: re-write elimination case using functions */
			switch (cmd_from_input) {
				/* player 1 */
				case CMD_CURSOR_LEFT_P1:
					PlaySound(snfx[SNFX_MV]);									// play move sound effect
					cursor_pos_p1 = move_cursor(cursor_pos_p1, CURSOR_MOV_L, CURSOR_MOV_U);
					break;
				case CMD_CURSOR_RIGHT_P1:
					PlaySound(snfx[SNFX_MV]);									// play move sound effect
					cursor_pos_p1 = move_cursor(cursor_pos_p1, CURSOR_MOV_R, CURSOR_MOV_U);
					break;
				case CMD_CURSOR_JRIGHT_P1:
					PlaySound(snfx[SNFX_MV]);									// play move sound effect
					cursor_pos_p1 = move_cursor(cursor_pos_p1, CURSOR_MOV_R, CURSOR_JMP_U);
					break;
				case CMD_CURSOR_JLEFT_P1:
					PlaySound(snfx[SNFX_MV]);									// play move sound effect
					cursor_pos_p1 = move_cursor(cursor_pos_p1, CURSOR_MOV_L, CURSOR_JMP_U);
					break;
				case CMD_ELIMINATE_P1:
					for (j = 0; j < FIELD_H; j++)
						if (block_p1[j][cursor_pos_p1].val == select_p1) {		// if hit the correct block
							select_p1 = 0;										// reset select. this will be used later to prevent having an empty palette by chance.
							PlaySound(snfx[SNFX_EL]);							// play elimination sound effect
							unsigned int psum = 0;								// detect empty screen
							score -= 10;
							block_p1[j][cursor_pos_p1].val = BLOCK_TYPE_FREE;	// free the block up
							palette_p1[cursor_pos_p1] = 0;						// palette will be assigned later
							for (i = 0; i < FIELD_W; i++)
								psum += palette_p1[i];
							if (psum)											// skip empty palette
								do {											// select a random char from visible blocks
									select_p1 = palette_p1[GetRandomValue(0, FIELD_W - 1)];
								} while (select_p1 == 0);
							else {												// spawn first row
								field_clear_bonus_p1 = 20;
								select_p1 = spaw_new_row(block_p1, palette_p1);
							}
							break;												// don't apply to blocks under eliminated block
						}
					break;

				/* player 2 */
				case CMD_CURSOR_LEFT_P2:
					PlaySound(snfx[SNFX_MV]);									// play move sound effect
					cursor_pos_p2 = move_cursor(cursor_pos_p2, CURSOR_MOV_L, CURSOR_MOV_U);
					break;
				case CMD_CURSOR_RIGHT_P2:
					PlaySound(snfx[SNFX_MV]);									// play move sound effect
					cursor_pos_p2 = move_cursor(cursor_pos_p2, CURSOR_MOV_R, CURSOR_MOV_U);
					break;
				case CMD_CURSOR_JRIGHT_P2:
					PlaySound(snfx[SNFX_MV]);									// play move sound effect
					cursor_pos_p2 = move_cursor(cursor_pos_p2, CURSOR_MOV_R, CURSOR_JMP_U);
					break;
				case CMD_CURSOR_JLEFT_P2:
					PlaySound(snfx[SNFX_MV]);									// play move sound effect
					cursor_pos_p2 = move_cursor(cursor_pos_p2, CURSOR_MOV_L, CURSOR_JMP_U);
					break;
				case CMD_ELIMINATE_P2:
					for (j = 0; j < FIELD_H; j++)
						if (block_p2[j][cursor_pos_p2].val == select_p2) {		// if hit the correct block
							select_p2 = 0;										// reset select. this will be used later to prevent having an empty palette by chance.
							PlaySound(snfx[SNFX_EL]);							// play elimination sound effect
							unsigned int psum = 0;								// detect empty screen
							score += 10;
							block_p2[j][cursor_pos_p2].val = BLOCK_TYPE_FREE;	// free the block up
							palette_p2[cursor_pos_p2] = 0;						// palette will be assigned later
							for (i = 0; i < FIELD_W; i++)
								psum += palette_p2[i];
							if (psum)											// skip empty palette
								do {											// select a random char from visible blocks
									select_p2 = palette_p2[GetRandomValue(0, FIELD_W - 1)];
								} while (select_p2 == 0);
							else {												// spawn first row
								field_clear_bonus_p2 = 20;
								select_p2 = spaw_new_row(block_p2, palette_p2);
							}
							break;												// don't apply to blocks under eliminated block
						}
					break;

				/* pause menu */
				case CMD_PAUSE:
					PlaySound(snfx[SNFX_PU]);									// play pause sound effect
					bcmx_ctl(BCMX_WAIT);										// pause background music
					game_state = GAME_STAT_PAUSE;
			}
			cmd_from_input = CMD_NULL;											// reset for next command
			//----------------------------------------------------------------------

			#if !defined(__SWITCH__)
			continue;															// skip break by window manager
			#endif
		/******************
		* pause menu loop *
		*******************/
		} else if(game_state == GAME_STAT_PAUSE) {
			unsigned char pause_menu_select = 0;

			#if defined(__SWITCH__)
				while(game_state == GAME_STAT_PAUSE)							// consoles don't possess a window manager
			#else
				while(!WindowShouldClose() && game_state == GAME_STAT_PAUSE)
			#endif
			{
				/* handle input */
				cmd_from_input = get_cmd();										// retrives players input
				//TODO: a unified cursor selection function
				switch (cmd_from_input) {
				#if defined(__SWITCH__)
					case CMD_CURSOR_DOWN_P1:
				#else
					case CMD_CURSOR_JLEFT_P2:
				#endif
					if(pause_menu_select < PAUSE_MENU_ITEMS_COUNT - 1)
						pause_menu_select++;
					else
						pause_menu_select = 0;									// teleport to top
					PlaySound(snfx[SNFX_MN]);									// play menu navigation sound effect
					break;
				#if defined(__SWITCH__)
					case CMD_CURSOR_UP_P1:
				#else
					case CMD_CURSOR_JRIGHT_P2:
				#endif
					if(pause_menu_select > 0)
						pause_menu_select--;
					else
						pause_menu_select = PAUSE_MENU_ITEMS_COUNT - 1;			// teleport to bottom
					PlaySound(snfx[SNFX_MN]);									// play menu navigation sound effect
					break;
				case CMD_PAUSE:
					game_state = GAME_STAT_RUN;
					break;
				case CMD_ELIMINATE_P1:
					switch (pause_menu_select)
					{
					case 0:		// continue
						game_state = GAME_STAT_RUN;
						break;
					case 1:		// main menu
						game_state = GAME_STAT_END;
						break;
					#if !defined(__SWITCH__)
						// Note: a console game shall not provide an "exit to OS" option
						case 2:		// exit to window
							game_state = GAME_STAT_EXIT;
					#endif
					}
				}
				cmd_from_input = CMD_NULL;										// reset for next command

				BeginDrawing();
				ClearBackground(DEFBACKCOLOR);
				/* draw */
				draw_frame(field_frame_rect_p1, COLOR_P1);						// draw filed frame for player 1
				draw_frame(field_frame_rect_p2, COLOR_P2);						// draw filed frame for player 2
				draw_blocks(cursor_pos_p1, block_p1, blkfont, font_w);			// draw field blocks for player 1
				draw_blocks(cursor_pos_p2, block_p2, blkfont, font_w);			// draw field blocks for player 2
				draw_cursor(cursor_pos_p1, LEFT_MARGIN);						// draw cursor for player 1
				draw_cursor(cursor_pos_p2, LEFT_MARGIN);						// draw cursor for player 2
				draw_pause_menu(pause_menu_select);								// draw pause menu
				EndDrawing();
			}
			if(game_state == GAME_STAT_RUN)										// play background music only if game has not been ended
				bcmx_ctl(BCMX_MULTIPLAYER_PLAY);

			#if !defined(__SWITCH__)
			continue;															// skip break by window manager
			#endif
		}

	}
	//--------------------------------------------------------------------------

	UnloadFont(blkfont);
	UnloadFont(deffont);
	bcmx_ctl(BCMX_MULTIPLAYER_STOP);
	if(game_state == GAME_STAT_END)
		return false;
	else /* GAME_STAT_EXIT e.g. exit to OS */
		return true;
}
