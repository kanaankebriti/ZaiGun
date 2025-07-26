#include <stdlib.h>
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
	unsigned char i;															/* loop counter */
	bool close_to_os = false;													/* close to OS or continue with main menu */
	signed char bckg_pos_x = -64;												/* background moves in a cycle */
	unsigned char selected_item = 0;											/* selected item index */
	Rectangle selected_rec;														/* selected item frame */
	Sound snfx[NUM_OF_SNFX];													/* sound bank */
	Icon main_menu_item[NUM_OF_ICONS];											/* main menu icons */
	Texture bckg_txt;															/* background texture */
	Music main_menu_background_music;											/* background music */
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
	SetExitKey(0);																/* escape from ESC death trap of WindowShouldClose */
	#endif

	selected_rec.height = MAIN_MENU_ICON_WH + FIELD_BORDER_WIDTH;
	selected_rec.width = MAIN_MENU_ICON_WH + FIELD_BORDER_WIDTH;

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
	/* SNFX_MN: menu navigation sound effect */
	snfx[0] = LoadSound(RESOURCE_PATH"little_robot_sound_factory_Menu_Navigate_02.wav");
	/* SNFX_MV: cursor mode sound effect */
	snfx[1] = LoadSound(RESOURCE_PATH"multimedia_button_click_007.mp3");
	/* SNFX_EL: block elimination sound effect */
	snfx[2] = LoadSound(RESOURCE_PATH"zapsplat_multimedia_game_sound_retro_arcade_style_coin_collect_007_108815.mp3");
	/* SNFX_FCL: field cleared sound effect */
	snfx[3] = LoadSound(RESOURCE_PATH"zapsplat_multimedia_game_sound_digital_retro_arcade_blip_002_98414.wav");
	/* SNFX_WA: wait sound effect */
	snfx[4] = LoadSound(RESOURCE_PATH"zapsplat_multimedia_game_sound_retro_arcade_style_coin_collect_005_108813.wav");
	/* SNFX_PU: pause sound effect */
	snfx[5] = LoadSound(RESOURCE_PATH"pause-fx_C_major.wav");

	/* load background texture */
	#if defined(__SWITCH__)
	bckg_txt = LoadTexture(RESOURCE_PATH"bckg_1280_720.png");
	#else
	bckg_txt = LoadTexture(RESOURCE_PATH"bckg_1280_720.png");
	/*bckg_txt = LoadTexture(RESOURCE_PATH"bckg_1024_768.png"); */
	#endif

	main_menu_background_music = LoadMusicStream(RESOURCE_PATH"i-music.mp3");
	PlayMusicStream(main_menu_background_music);								/* start playing background music */
	/*------------------------------------------------------------------------*/

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
				if(selected_item < NUM_OF_ICONS - 1) {
					selected_item++;
					PlaySound(snfx[SNFX_MN]);									/* play menu navigation sound effect */
				}
				break;

			#if defined(__SWITCH__)
			case CMD_CURSOR_LEFT_P1:
			#else
			case CMD_CURSOR_LEFT_P2:
			#endif
				if(selected_item != 0) {
					selected_item--;
					PlaySound(snfx[SNFX_MN]);									/* play menu navigation sound effect */
				}
				break;

			case CMD_ELIMINATE_P1:
				StopMusicStream(main_menu_background_music);
				switch (selected_item) {										/* execute game */
					case 0:
						close_to_os = arcade(snfx);
						break;
					case 1:
						close_to_os = multiplayer(snfx);
						break;
					case 2:
						close_to_os = vscpu(snfx);
						break;
				}
				PlayMusicStream(main_menu_background_music);
		}
		/*--------------------------------------------------------------------*/

		/*****************/
		/* updaing logic */
		/*****************/
		/* update backgroud position, making it move. */
		if (bckg_pos_x >= 0)
			bckg_pos_x = -64;
		else
			bckg_pos_x++;

		if (close_to_os)
			break;

		UpdateMusicStream(main_menu_background_music);							/* update background music */
		/*--------------------------------------------------------------------*/

		/***********/
		/* drawing */
		/***********/
		BeginDrawing();
		ClearBackground(DEFBACKCOLOR);
		DrawTexture(bckg_txt, bckg_pos_x, 0, WHITE);
		selected_rec.x = main_menu_item[selected_item].pos_x - FIELD_BORDER_WIDTH / 2;
		selected_rec.y = main_menu_item[selected_item].pos_y - FIELD_BORDER_WIDTH / 2;
		draw_frame(selected_rec, RED);
		/* draw all menu icons */
		for(i = 0; i < NUM_OF_ICONS; i++)
			DrawTexture(
				main_menu_item[i].txt,
				main_menu_item[i].pos_x,
				main_menu_item[i].pos_y,
				WHITE
			);
		EndDrawing();
		/*--------------------------------------------------------------------*/
	}
	/*------------------------------------------------------------------------*/

	#if defined(__SWITCH__)
	romfsExit();
	#endif

	UnloadTexture(bckg_txt);													/* uload background texture */
	for(i = 0; i < NUM_OF_ICONS; i++)
		UnloadTexture(main_menu_item[i].txt);									/* unload icon textures */
	for(i = 0; i < NUM_OF_SNFX; i++)
		UnloadSound(snfx[i]);													/* unload sounds */
	StopMusicStream(main_menu_background_music);
	UnloadMusicStream(main_menu_background_music);
	CloseAudioDevice();
	return EXIT_SUCCESS;
}
