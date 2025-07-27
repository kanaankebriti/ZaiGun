#include <stdlib.h>
#include <math.h>
#include "common.h"
#include "const.h"

#define NUM_OF_ICONS 3
#define NUM_OF_SNFX 6

int main(void) {
	/*************/
	/* variables */
	/*************/
	typedef struct Icon {
		short pos_x;
		short pos_y;
		Texture txt;
	} Icon;

	/* integer types */
	unsigned char i;
	unsigned char selected_item = 0;											/* selected item index */
	bool close_to_os = false;													/* close to OS or continue with main menu */
	signed char bckg_pos_x = -64;												/* background moves in a cycle */

	/* floating-point types */
	double t;																	/* time for pulsing */
	float pulse;																/* scale multiplier for box pulse */
	float moveSpeed = 0.2f;														/* box lerp speed */
	float scaleSpeed = 0.2f;													/* icon lerp speed */
	float tx, ty, tw;															/* target frame values */

	/* arrays and structs */
	float icon_scales[NUM_OF_ICONS];											/* per-icon scale */
	Sound snfx[NUM_OF_SNFX];													/* sound bank */
	Icon main_menu_item[NUM_OF_ICONS];											/* main menu icons */
	Texture bckg_txt;															/* background texture */
	Music main_menu_background_music;											/* background music */

	/* rectangles and color */
	Rectangle current_rec;														/* current select box frame */
	Rectangle selected_rec;														/* temp for drawing highlight */
	Color select_box_color;														/* highlight color */
	/*------------------------------------------------------------------------*/

	/********/
	/* init */
	/********/
	InitWindow(WIN_W, WIN_H, "ratest");
	InitAudioDevice();
	#if defined(__SWITCH__)
		romfsInit();
	#endif
	SetTargetFPS(FPS);
	#if !defined(__SWITCH__)
		SetExitKey(0);															/* escape from ESC death trap */
	#endif

	/* load textures and populate icons */
	main_menu_item[0].pos_x = 50;
	main_menu_item[0].pos_y = (WIN_H - MAIN_MENU_ICON_WH) / 2;
	main_menu_item[0].txt   = LoadTexture(RESOURCE_PATH"sp.png");				/* single player icon */

	main_menu_item[1].pos_x = 400;
	main_menu_item[1].pos_y = (WIN_H - MAIN_MENU_ICON_WH) / 2;
	main_menu_item[1].txt   = LoadTexture(RESOURCE_PATH"mp.png");				/* multi player icon */

	main_menu_item[2].pos_x = 750;
	main_menu_item[2].pos_y = (WIN_H - MAIN_MENU_ICON_WH) / 2;
	main_menu_item[2].txt   = LoadTexture(RESOURCE_PATH"spcp.png");				/* vs cpu icon */

	/* load sound bank */
	snfx[0] = LoadSound(RESOURCE_PATH"little_robot_sound_factory_Menu_Navigate_02.wav");
	snfx[1] = LoadSound(RESOURCE_PATH"multimedia_button_click_007.mp3");
	snfx[2] = LoadSound(RESOURCE_PATH"zapsplat_multimedia_game_sound_retro_arcade_style_coin_collect_007_108815.mp3");
	snfx[3] = LoadSound(RESOURCE_PATH"zapsplat_multimedia_game_sound_digital_retro_arcade_blip_002_98414.wav");
	snfx[4] = LoadSound(RESOURCE_PATH"zapsplat_multimedia_game_sound_retro_arcade_style_coin_collect_005_108813.wav");
	snfx[5] = LoadSound(RESOURCE_PATH"pause-fx_C_major.wav");

	/* load background texture */
	#if defined(__SWITCH__)
		bckg_txt = LoadTexture(RESOURCE_PATH"bckg_1280_720.png");
	#else
		bckg_txt = LoadTexture(RESOURCE_PATH"bckg_1280_720.png");
		/*bckg_txt = LoadTexture(RESOURCE_PATH"bckg_1024_768.png"); */
	#endif

	/* load & start background music */
	main_menu_background_music = LoadMusicStream(RESOURCE_PATH"i-music.mp3");
	PlayMusicStream(main_menu_background_music);								/* start playing background music */
	select_box_color = RED;														/* assign select_box color */

	/* initialize animation state */
	for (i = 0; i < NUM_OF_ICONS; i++) {
		icon_scales[i] = (i == selected_item) ? 1.05f : 1.0f;
	}
	/* initial select box frame matches first icon */
	current_rec.x	  = main_menu_item[selected_item].pos_x - FIELD_BORDER_WIDTH/2;
	current_rec.y	  = main_menu_item[selected_item].pos_y - FIELD_BORDER_WIDTH/2;
	current_rec.width  = MAIN_MENU_ICON_WH + FIELD_BORDER_WIDTH;
	current_rec.height = current_rec.width;

	/*************
	 * main loop *
	 *************/
	#if defined(__SWITCH__)
	while (true) {
	#else
	while (!WindowShouldClose()) {
	#endif
		/****************
		 * handle input *
		 ****************/
		char cmd = get_cmd();

		switch (cmd) {
			#if defined(__SWITCH__)
			case CMD_CURSOR_RIGHT_P1:
			#else
			case CMD_CURSOR_RIGHT_P2:
			#endif
				if (selected_item < NUM_OF_ICONS - 1) {
					selected_item++;
					PlaySound(snfx[SNFX_MN]);									/* play menu navigation sound effect */
				}
				break;

			#if defined(__SWITCH__)
			case CMD_CURSOR_LEFT_P1:
			#else
			case CMD_CURSOR_LEFT_P2:
			#endif
				if (selected_item != 0) {
					selected_item--;
					PlaySound(snfx[SNFX_MN]);									/* play menu navigation sound effect */
				}
				break;

			case CMD_ELIMINATE_P1:
				StopMusicStream(main_menu_background_music);
				switch (selected_item) {			 /* execute game */
					case 0: close_to_os = arcade(snfx);	break;
					case 1: close_to_os = multiplayer(snfx);break;
					case 2: close_to_os = vscpu(snfx);	  break;
				}
				PlayMusicStream(main_menu_background_music);
				break;
		}
		/*--------------------------------------------------------------------*/

		/*****************/
		/* updating logic */
		/*****************/
		/* update background position, making it move. */
		if (bckg_pos_x >= 0)
			bckg_pos_x = -64;
		else
			bckg_pos_x++;

		if (close_to_os)
			break;

		UpdateMusicStream(main_menu_background_music);							/* update background music */

		/* pulsing select_box parameters */
		t	 = GetTime();														/* current time */
		pulse = 0.03f * (float)sin(t * 8.0) + 1.03f;							/* 1.0 → 1.06 */
		select_box_color.a =
			(unsigned char)(140.0f + 60.0f * (float)sin(t * 6.0f));				/* glow alpha */

		/* compute and lerp current_rec toward target without extra scope */
		tx = main_menu_item[selected_item].pos_x - FIELD_BORDER_WIDTH/2;
		ty = main_menu_item[selected_item].pos_y - FIELD_BORDER_WIDTH/2;
		tw = MAIN_MENU_ICON_WH + FIELD_BORDER_WIDTH;
		current_rec.x	  += (tx - current_rec.x) * moveSpeed;
		current_rec.y	  += (ty - current_rec.y) * moveSpeed;
		current_rec.width  += (tw - current_rec.width) * moveSpeed;
		current_rec.height += (tw - current_rec.height) * moveSpeed;

		/* lerp icon scales toward target size */
		for (i = 0; i < NUM_OF_ICONS; i++) {
			float targetScale = (i == selected_item) ? 1.05f : 1.0f;
			icon_scales[i] += (targetScale - icon_scales[i]) * scaleSpeed;
		}

		/***********/
		/* drawing */
		/***********/
		BeginDrawing();
		ClearBackground(DEFBACKCOLOR);
		DrawTexture(bckg_txt, bckg_pos_x, 0, WHITE);							/* draw moving background */

		/* derive and draw pulsating highlight directly from current_rec */
		selected_rec.x = current_rec.x - (current_rec.width * (pulse - 1.0f) * 0.5f);
		selected_rec.y = current_rec.y - (current_rec.height * (pulse - 1.0f) * 0.5f);
		selected_rec.width  = (int)(current_rec.width  * pulse);
		selected_rec.height = selected_rec.width;
		DrawRectangleRoundedLinesEx(
			selected_rec,
			FRAME_ROUNDNESS,
			4,
			FIELD_BORDER_WIDTH,
			select_box_color
		);																		/* draw pulsating select box */

		/* draw all menu icons */
		for (i = 0; i < NUM_OF_ICONS; i++) {
			Vector2 pos;
			float   scale = icon_scales[i];
			/* center scaled icons */
			pos.x = main_menu_item[i].pos_x + (MAIN_MENU_ICON_WH * (1.0f - scale) * 0.5f);
			pos.y = main_menu_item[i].pos_y + (MAIN_MENU_ICON_WH * (1.0f - scale) * 0.5f);
			DrawTextureEx(
				main_menu_item[i].txt,
				pos,
				0.0f,
				scale,
				WHITE
			);
		}

		EndDrawing();
		/*--------------------------------------------------------------------*/
	}
	/*------------------------------------------------------------------------*/

	/********** */
	/* teardown */
	/********** */
	#if defined(__SWITCH__)
	romfsExit();
	#endif

	UnloadTexture(bckg_txt);													/* unload background texture */
	for (i = 0; i < NUM_OF_ICONS; i++)
		UnloadTexture(main_menu_item[i].txt);									/* unload icon textures */
	for (i = 0; i < NUM_OF_SNFX; i++)
		UnloadSound(snfx[i]);													/* unload sounds */
	StopMusicStream(main_menu_background_music);
	UnloadMusicStream(main_menu_background_music);
	CloseAudioDevice();															/* close audio device */
	return EXIT_SUCCESS;
}
