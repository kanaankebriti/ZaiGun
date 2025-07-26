#include <math.h>
#include "defs.h"
#include "common.h"

typedef struct {
	short letter;
	Vector2 position;
	Vector2 velocity;
	Vector2 origin;
	float angle;
	float angular_speed;
	unsigned char size;
} particle;

particle letters[BG_MAX_LETTERS];

void InitializeLetters() {
	unsigned char i;
	for (i = 0; i < BG_MAX_LETTERS; i++) {
		letters[i].letter = GetRandomValue(0, 25) * 128;						/* Random letter A-Z from texture */
		letters[i].angle = GetRandomValue(0, 359);								/* Random angle */
		letters[i].angular_speed = GetRandomValue(-8, 8);						/* Random rotation speed */
		letters[i].position = (Vector2){ GetRandomValue(0, WIN_W), WIN_H / 2 };
		/* letters[i].position = (Vector2){ WIN_W / 2, WIN_H / 2 }; */
		letters[i].size = GetRandomValue(BG_FONT_MIN, BG_FONT_MAX);				/* Random font size */
		/*if(((letters[i].size) % 2) == 0) */
			/*letters[i].size++; */
		letters[i].origin = (Vector2){letters[i].size / 2, letters[i].size / 2};/* rotation axis */
		/* Random angle in radians */
		float angle = (float)GetRandomValue(0, 359) * (PI / 180.f);
		float speed = (float)GetRandomValue(200, 350);							/* high initial speec */
		letters[i].velocity.x = cos(angle) * speed;
		letters[i].velocity.y = sin(angle) * speed;
	}
}

void update_background_letters(float deltaTime) {
	unsigned char i;
	for (i = 0; i < BG_MAX_LETTERS; i++) {
		letters[i].position.x += letters[i].velocity.x * deltaTime;
		letters[i].position.y += letters[i].velocity.y * deltaTime;
		letters[i].angle += letters[i].angular_speed;
		/* Check if the letter is outside the screen bounds */
		if (letters[i].position.x < -BG_FONT_MAX || letters[i].position.x > WIN_W + BG_FONT_MAX ||
			letters[i].position.y < -BG_FONT_MAX || letters[i].position.y > WIN_H + BG_FONT_MAX) {
			letters[i].position = (Vector2){ WIN_W / 2, WIN_H / 2 };			/* reset position */
			letters[i].angle = 0;												/* reset angle */
			float angle = (float)GetRandomValue(0, 359) * (PI / 180);			/* random angle in radians */
			float speed = (float)GetRandomValue(80, 350);						/* random speed */
			letters[i].velocity.x = cos(angle) * speed;							/* new random v_x */
			letters[i].velocity.y = sin(angle) * speed;							/* new radom v_y */
		}
	}
}

void draw_background_letters(Texture bgfont) {
	unsigned char i;
	for (i = 0; i < BG_MAX_LETTERS; i++)
		DrawTexturePro(
			bgfont,
			(Rectangle){letters[i].letter, 0, 128, 128},
			(Rectangle){letters[i].position.x, letters[i].position.y, letters[i].size, letters[i].size},
			letters[i].origin,
			letters[i].angle,
			CHERENKOV
		);
}

/*	return false: close game *
	return true: continue with main menu
*/
bool arcade(Sound* snfx) {
	unsigned char game_state = GAME_STAT_RUN;									/* game flow state (run, pause, end) */
	int captials[26] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
	field block[FIELD_H][FIELD_W];
	register unsigned char i, j;												/* loop counters */
	signed char palette[FIELD_W] = { 0 };										/* random chars set of headline */
	char select = 0;															/* selected char from palette */
	unsigned char cursor_pos = 0;												/* cursor position */
	unsigned char cmd_from_input;												/* input command. in order to handle gamepad as well. */
	unsigned char field_clear_effect = 0;
	unsigned long score = 0;
	double delta = 0;															/* keeps time flow. init value ensures immediate start. */
	Vector2 block_pos;				/*TODO: perhaps eliminate this to			// temporary position */
	const Rectangle field_frame_rect = {
		LEFT_MARGIN,
		TOP_MARGIN,
		BLOCK_SIZE * FIELD_W - BLOCK_MARGIN,
		BLOCK_SIZE * FIELD_H - BLOCK_MARGIN,
	};
	const Music arcade_background_music = LoadMusicStream(RESOURCE_PATH"b-pop.mp3");

	/********
	 * init *
	 ********/
	/*-------------------------------------------------------------------------- */
	const Font deffont = GetFontDefault();
	const Texture bgfont = LoadTexture(
			RESOURCE_PATH"alpha2.png"
		);
	const Font blkfont = LoadFontEx(
			RESOURCE_PATH"ProcrastinatingPixie-WyVOO.ttf",
			128,
			captials,
			26
		);																		/* fonts dedicated to glyphs on blocks */
	const char font_w = (BLOCK_SIZE / deffont.baseSize) * deffont.recs->width;

	/* initilize field blocks*/
	for (j = 0; j < FIELD_H; j++)
		for (i = 0; i < FIELD_W; i++) {
			/* compute blocks position */
			block[j][i].x_pos = i * BLOCK_SIZE + LEFT_MARGIN;
			block[j][i].y_pos = j * BLOCK_SIZE + TOP_MARGIN;
			block[j][i].val = BLOCK_TYPE_FREE;
		}

	InitializeLetters();
	/* intro waiting */
	/*for (signed char wait = 3; wait >= 0; wait--) {							signed is required by arm64!
		BeginDrawing();
		ClearBackground(DEFBACKCOLOR);
		draw_frame(field_frame_rect, CYAN);										draw frame around the field
		draw_blocks(cursor_pos, block, blkfont, font_w);						draw field
		draw_cursor(cursor_pos, LEFT_MARGIN);									draw cursor for player 1
		DrawText(
			TextFormat("%d", wait),
			LEFT_MARGIN + (FIELD_W * BLOCK_SIZE) / 2 - 2 * font_w,				horizontal alignment
			TOP_MARGIN + (FIELD_W * BLOCK_SIZE) / 2 + 100,						vertical alignment
			100,
			DARKROSE
		);																		draw countdown
		EndDrawing();
		PlaySound(snfx[SNFX_WA]);												play wait sound effect
		WaitTime(1.f);
	}
	PlaySound(snfx[SNFX_EL]);													play elimination sound effect as the game begins!
	WaitTime(0.5f);
	*/
	PlayMusicStream(arcade_background_music);									/* start playing background music */
	select = spaw_new_row(block, palette);										/* spawn first row */

	/*************
	 * main loop *
	 *************/
	/*-------------------------------------------------------------------------- */
	/*while (game_state <= GAME_STAT_PAUSE) {	// game_state == run or pause; faster code this way but less clear :( */

	#if defined(__SWITCH__)
	while(game_state <= GAME_STAT_PAUSE)										/* consoles don't possess a window manager */
	#else
	while(!WindowShouldClose() && game_state <= GAME_STAT_PAUSE)				/* game_state == run or pause; faster code this way but less clear :( */
	#endif
	{
		/************
	 	* game loop *
	 	************/
		if(game_state == GAME_STAT_RUN) {
			/*****************
			 * updaing logic *
			 *****************/
			/*------------------------------------------------------------------ */
			if (GetTime() - delta > get_interval_quadratic(score)) {
				/* move each block upward */
				for (j = 1; j < FIELD_H; j++)
					for (i = 0; i < FIELD_W; i++)
						block[j - 1][i].val = block[j][i].val;
				/* spawn new blocks */
				for (i = 0; i < FIELD_W; i++)
					if (GetRandomValue(0, 100) > 80)							/* set letters occurrence rate */
						block[FIELD_H - 1][i].val = BLOCK_TYPE_OCCU;
					else
						block[FIELD_H - 1][i].val = BLOCK_TYPE_FREE;
				delta = GetTime();
			}

			/* set letters at head row */
			for (i = 0; i < FIELD_W; i++)
				for (j = 1; j < FIELD_H; j++) {
					char current_block_letter = block[j][i].val;
					if (current_block_letter >= 'A')							/* skip blocks under headline */
						break;
					else if ((current_block_letter == BLOCK_TYPE_OCCU) ) {
						palette[i] = GetRandomValue('A', 'Z');					/* save rand char to palette */
						block[j][i].val = palette[i];							/* assign a random character to a block */
						break;													/* don't apply to blocks under eliminated block */
					}
				}

			update_background_letters(GetFrameTime());							/* update animating background */
			UpdateMusicStream(arcade_background_music);							/* update background music */
			/*------------------------------------------------------------------ */

			/***********
			 * drawing *
			 ***********/
			/*---------------------------------------------------------------------- */
			BeginDrawing();
			/* stupid backround flashing! */
			if (field_clear_effect) {
				PlaySound(snfx[SNFX_FCL]);										/* play field cleared sound effect */
				score += 100;													/* gradually add score for more satisfaction! */
				ClearBackground(CLITERAL(Color){ GetRandomValue(0, 255), GetRandomValue(0, 255), GetRandomValue(0, 255), 255 });
				field_clear_effect--;
			} else
				ClearBackground(DEFBACKCOLOR);


			draw_background_letters(bgfont);									/* draw animating background */

			/* draw selected character */
			block_pos.x = LEFT_MARGIN - 100;									/* selected character display position */
			block_pos.y = (WIN_H - 100) / 2;
			DrawTextCodepoint(blkfont, select, block_pos, 98, RED );			/* draw selected character */
			block_pos.x = LEFT_MARGIN + FIELD_W * BLOCK_SIZE + BLOCK_MARGIN + FIELD_BORDER_WIDTH;
			DrawTextEx(deffont, TextFormat("%ld", score), block_pos, 100, 1, WHITE);/* draw score */

			draw_frame(field_frame_rect, CYAN);									/* draw filed frame */
			draw_blocks(cursor_pos, block, blkfont, font_w);					/* draw field blocks */
			draw_cursor(cursor_pos, LEFT_MARGIN);								/* draw cursor */

			/*DrawText(TextFormat("G1=%d LX=%f LY=%f RX=%f RY%f", IsGamepadAvailable(0), GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X), GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y), GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_X), GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_Y)), 0, 0, 28, RED); */
			/*DrawText(TextFormat("G2=%d LX=%f LY=%f RX=%f RY%f", IsGamepadAvailable(1), GetGamepadAxisMovement(1, GAMEPAD_AXIS_LEFT_X), GetGamepadAxisMovement(1, GAMEPAD_AXIS_LEFT_Y), GetGamepadAxisMovement(1, GAMEPAD_AXIS_RIGHT_X), GetGamepadAxisMovement(1, GAMEPAD_AXIS_RIGHT_Y)), 0, 30, 28, RED); */
			EndDrawing();
			/*---------------------------------------------------------------------- */

			/****************
			 * handle input *
			 ****************/
			/*---------------------------------------------------------------------- */
			cmd_from_input = get_cmd();											/* retrives players input */

			/* perform actions */
			/* Note: player 2 commands since i prefer arrows than WASD but i also prefer space from player 1 :) */
			/* Note: adds much complexity casue it should also work on nsw :) */
			/* Note: will become even more complex in case of detached nsw controller :) */
			/* TODO: perhaps reduce above Note's nonesene complexity! */
			switch (cmd_from_input) {
				#if defined(__SWITCH__)
					case CMD_CURSOR_LEFT_P1:
				#else
					case CMD_CURSOR_LEFT_P2:
				#endif
					PlaySound(snfx[SNFX_MV]);									/* play move sound effect */
					cursor_pos = move_cursor(cursor_pos, CURSOR_MOV_L, CURSOR_MOV_U);
					break;
				#if defined(__SWITCH__)
					case CMD_CURSOR_RIGHT_P1:
				#else
					case CMD_CURSOR_RIGHT_P2:
				#endif
					PlaySound(snfx[SNFX_MV]);									/* play move sound effect */
					cursor_pos = move_cursor(cursor_pos, CURSOR_MOV_R, CURSOR_MOV_U);
					break;
				#if defined(__SWITCH__)
					case CMD_CURSOR_JRIGHT_P1:
				#else
					case CMD_CURSOR_JRIGHT_P2:
				#endif
					PlaySound(snfx[SNFX_MV]);									/* play move sound effect */
					cursor_pos = move_cursor(cursor_pos, CURSOR_MOV_R, CURSOR_JMP_U);
					break;
				#if defined(__SWITCH__)
					case CMD_CURSOR_JLEFT_P1:
				#else
					case CMD_CURSOR_JLEFT_P2:
				#endif
					PlaySound(snfx[SNFX_MV]);									/* play move sound effect */
					cursor_pos = move_cursor(cursor_pos, CURSOR_MOV_L, CURSOR_JMP_U);
					break;
				case CMD_ELIMINATE_P1:
					for (j = 0; j < FIELD_H; j++)
						if (block[j][cursor_pos].val == select) {				/* if hit the correct block */
							select = 0;											/* reset select. this will be used later to prevent having an empty palette by chance. */
							PlaySound(snfx[SNFX_EL]);							/* play elimination sound effect */
							unsigned int psum = 0;								/* detect empty screen */
							score += 10;
							block[j][cursor_pos].val = BLOCK_TYPE_FREE;			/* free the block up */
							palette[cursor_pos] = 0;							/* palette will be assigned later */
							for (i = 0; i < FIELD_W; i++)
								psum += palette[i];
							if (psum)											/* skip empty palette */
								do {											/* select a random char from visible blocks */
									select = palette[GetRandomValue(0, FIELD_W - 1)];
								} while (select == 0);
							else {												/* spawn first row */
								field_clear_effect = 20;
								select = spaw_new_row(block, palette);
							}
							break;												/* don't apply to blocks under eliminated block */
						}
					break;
				case CMD_PAUSE:
					PlaySound(snfx[SNFX_PU]);									/* play pause sound effect */
					game_state = GAME_STAT_PAUSE;
			}
			cmd_from_input = CMD_NULL;											/* reset for next command */
			/*---------------------------------------------------------------------- */

			#if !defined(__SWITCH__)
			continue;															/* skip break by window manager */
			#endif

		/******************
		* pause menu loop *
		*******************/
		} else if(game_state == GAME_STAT_PAUSE) {
			unsigned char pause_menu_select = 0;

			#if defined(__SWITCH__)
			while(game_state == GAME_STAT_PAUSE)								/* consoles don't possess a window manager */
			#else
			while(!WindowShouldClose() && game_state == GAME_STAT_PAUSE)
			#endif
			{
				/* handle input */
				cmd_from_input = get_cmd();										/* retrives players input */
				/*TODO: a unified cursor selection function */
				switch (cmd_from_input) {
					#if defined(__SWITCH__)
					case CMD_CURSOR_DOWN_P1:
					#else
					case CMD_CURSOR_JLEFT_P2:
					#endif
						if(pause_menu_select < PAUSE_MENU_ITEMS_COUNT - 1)
							pause_menu_select++;
						else
							pause_menu_select = 0;								/* teleport to top */
						PlaySound(snfx[SNFX_MN]);								/* play menu navigation sound effect */
						break;
					#if defined(__SWITCH__)
					case CMD_CURSOR_UP_P1:
					#else
					case CMD_CURSOR_JRIGHT_P2:
					#endif
						if(pause_menu_select > 0)
							pause_menu_select--;
						else
							pause_menu_select = PAUSE_MENU_ITEMS_COUNT - 1;		/* teleport to bottom */
						PlaySound(snfx[SNFX_MN]);								/* play menu navigation sound effect */
						break;
					case CMD_ELIMINATE_P1:
						switch (pause_menu_select)
						{
						case 0:		/* continue */
							game_state = GAME_STAT_RUN;
							break;
						case 1:		/* main menu */
							game_state = GAME_STAT_END;
							break;
						#if !defined(__SWITCH__)
							/* Note: a console game shall not provide an "exit to OS" option */
							case 2:		/* exit to window */
								game_state = GAME_STAT_EXIT;
						#endif
						}
					#if !defined(__SWITCH__)									/* start button toggeling does not work on nintendo homebrew, results in double input (pressed ounced, twice received) */
						break;
					case CMD_PAUSE:
						game_state = GAME_STAT_RUN;
					#endif
				}
				cmd_from_input = CMD_NULL;										/* reset for next command */

				BeginDrawing();
				ClearBackground(DEFBACKCOLOR);
				draw_background_letters(bgfont);								/* draw animating background */
				/* draw */
				draw_frame(field_frame_rect, CYAN);								/* draw filed frame */
				draw_blocks(cursor_pos, block, blkfont, font_w);				/* draw field blocks */
				draw_cursor(cursor_pos, LEFT_MARGIN);							/* draw cursor */
				draw_pause_menu(pause_menu_select);								/* draw pause menu */
				EndDrawing();
			}

			/*#if !defined(__SWITCH__) */
			/*continue;															// skip break by window manager */
			/*#endif */
		}

		/* break by window manager */
		/*#if !defined(__SWITCH__) */
		/*game_state = GAME_STAT_EXIT; */
		/*#endif */
	}
	/*-------------------------------------------------------------------------- */

	UnloadFont(blkfont);
	UnloadFont(deffont);
	StopMusicStream(arcade_background_music);
	UnloadMusicStream(arcade_background_music);
	if(game_state == GAME_STAT_END)
		return false;
	else /* GAME_STAT_EXIT e.g. exit to OS */
		return true;
}
