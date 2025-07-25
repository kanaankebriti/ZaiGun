#include <pthread.h>
#include "defs.h"
#include "common.h"

unsigned char bcmx_thread_ctl = BCMX_WAIT;										// control byte
bool bcmx_thread_lock;															// thread lock

#define NUM_OF_ICONS 3

/* background music thread */
void *bcmx_callback(void *arg) {
	const Music main_menu = LoadMusicStream(RESOURCE_PATH"i-music.mp3");
	const Music arcade = LoadMusicStream(RESOURCE_PATH"b-pop.mp3");
	const Music multiplayer = LoadMusicStream(RESOURCE_PATH"4_track_from_heaven.mp3");
	bool is_thread_running = true;

	while (is_thread_running)
		switch (bcmx_thread_ctl)
		{
		case BCMX_END:
			is_thread_running = false;
			break;
		case BCMX_WAIT:
			WaitTime(.2f);
			bcmx_thread_lock = false;											// unlock thread
			break;
		/* main menu */
		case BCMX_MAIN_MENU_LOAD:
			if(!IsMusicStreamPlaying(main_menu))
				PlayMusicStream(main_menu);
			bcmx_thread_lock = false;
			break;
		case BCMX_MAIN_MENU_PLAY:
			UpdateMusicStream(main_menu);
			WaitTime(.02f);
			break;
		case BCMX_MAIN_MENU_STOP:
			if(IsMusicStreamPlaying(main_menu))
				StopMusicStream(main_menu);
			bcmx_thread_ctl = BCMX_WAIT;										// wait for next command while avoiding high cpu usage
			bcmx_thread_lock = false;											// unlock thread
			break;
		/* arcade */
		case BCMX_ARCADE_LOAD:
			if(!IsMusicStreamPlaying(arcade))
				PlayMusicStream(arcade);
			bcmx_thread_lock = false;
			break;
		case BCMX_ARCADE_PLAY:
			UpdateMusicStream(arcade);
			WaitTime(.02f);
			break;
		case BCMX_ARCADE_STOP:
			if(IsMusicStreamPlaying(arcade))
				StopMusicStream(arcade);
			bcmx_thread_ctl = BCMX_WAIT;										// wait for next command while avoiding high cpu usage
			bcmx_thread_lock = false;											// unlock thread
			break;
		/* multiplayer */
		case BCMX_MULTIPLAYER_LOAD:
			if(!IsMusicStreamPlaying(multiplayer))
				PlayMusicStream(multiplayer);
			bcmx_thread_lock = false;
			break;
		case BCMX_MULTIPLAYER_PLAY:
			UpdateMusicStream(multiplayer);
			WaitTime(.02f);
			break;
		case BCMX_MULTIPLAYER_STOP:
			if(IsMusicStreamPlaying(multiplayer))
				StopMusicStream(multiplayer);
			bcmx_thread_ctl = BCMX_WAIT;										// wait for next command while avoiding high cpu usage
			bcmx_thread_lock = false;											// unlock thread
			break;
		}

	UnloadMusicStream(main_menu);
	UnloadMusicStream(arcade);
	pthread_exit(NULL);
}

int main(void) {
	typedef struct icon
	{
		short pos_x;
		short pos_y;
		Texture txt;
	} icon;

	bool close_to_os = false;													// close to OS or continue with main menu
	signed char bckg_pos_x = -64;												// background moves in a cycle
	pthread_t bcmx_thread;														// background music
	unsigned char selected_item = 0;											// selected item index
	Rectangle selected_rec;														// selected item frame
	selected_rec.height = MAIN_MENU_ICON_WH + FIELD_BORDER_WIDTH;
	selected_rec.width = MAIN_MENU_ICON_WH + FIELD_BORDER_WIDTH;

	/********
	 * init *
	 ********/
	//--------------------------------------------------------------------------
	InitWindow(WIN_W, WIN_H, "ratest");
	InitAudioDevice();
	#if defined(__SWITCH__)
		romfsInit();
	#endif
	SetTargetFPS(FPS);
	pthread_create(&bcmx_thread, NULL, bcmx_callback, NULL);					// register background music thread
	/* load textures and populate icons */
	const icon main_menu_item[NUM_OF_ICONS] = {
		{50, (WIN_H - MAIN_MENU_ICON_WH) / 2, LoadTexture(RESOURCE_PATH"sp.png")},// single player icon
		{400, (WIN_H - MAIN_MENU_ICON_WH) / 2, LoadTexture(RESOURCE_PATH"mp.png")},// multi player icon
		{750, (WIN_H - MAIN_MENU_ICON_WH) / 2, LoadTexture(RESOURCE_PATH"spcp.png")}// vs cpu icon
	};
	SetExitKey(0);																// escape from ESC death trap of WindowShouldClose

	/* load sound bank */
	const Sound snfx[] = {
		/* SNFX_MN: menu navigation sound effect */
		LoadSound(RESOURCE_PATH"little_robot_sound_factory_Menu_Navigate_02.wav"),
		/* SNFX_MV: cursor mode sound effect */
		LoadSound(RESOURCE_PATH"multimedia_button_click_007.mp3"),
		/* SNFX_EL: block elimination sound effect */
		LoadSound(RESOURCE_PATH"zapsplat_multimedia_game_sound_retro_arcade_style_coin_collect_007_108815.mp3"),
		/* SNFX_FCL: field cleared sound effect */
		LoadSound(RESOURCE_PATH"zapsplat_multimedia_game_sound_digital_retro_arcade_blip_002_98414.wav"),
		/* SNFX_WA: wait sound effect */
		LoadSound(RESOURCE_PATH"zapsplat_multimedia_game_sound_retro_arcade_style_coin_collect_005_108813.wav"),
		/* SNFX_PU: pause sound effect */
		LoadSound(RESOURCE_PATH"pause-fx_C_major.wav")
	};

	#if defined(__SWITCH__)
		const Texture bckg_txt = LoadTexture(RESOURCE_PATH"bckg_1280_720.png");
	#else
		const Texture bckg_txt = LoadTexture(RESOURCE_PATH"bckg_1280_720.png");
		//const Texture bckg_txt = LoadTexture(RESOURCE_PATH"bckg_1024_768.png");
	#endif
	bcmx_ctl(BCMX_MAIN_MENU_LOAD);
	bcmx_ctl(BCMX_MAIN_MENU_PLAY);

	/*************
	 * main loop *
	 *************/
	//--------------------------------------------------------------------------
	while (!WindowShouldClose()) {
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
				PlaySound(snfx[SNFX_MN]);										// play menu navigation sound effect
			}
			break;
		#if defined(__SWITCH__)
				case CMD_CURSOR_LEFT_P1:
		#else
				case CMD_CURSOR_LEFT_P2:
		#endif
			if(selected_item != 0) {
				selected_item--;
				PlaySound(snfx[SNFX_MN]);										// play menu navigation sound effect
			}
			break;
		case CMD_ELIMINATE_P1:
			bcmx_ctl(BCMX_MAIN_MENU_STOP);										// stop background music
			switch (selected_item) {											// execute game
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
			bcmx_ctl(BCMX_MAIN_MENU_LOAD);										// reset background music on returning from a game
			bcmx_ctl(BCMX_MAIN_MENU_PLAY);
			break;
		}
		//----------------------------------------------------------------------

		if(bckg_pos_x == -1)
			bckg_pos_x = -64;
		else
			bckg_pos_x++;

		if (close_to_os)
			break;


		/***********
		 * drawing *
		 ***********/
		//----------------------------------------------------------------------
		BeginDrawing();
		ClearBackground(DEFBACKCOLOR);
		DrawTexture(bckg_txt, bckg_pos_x, 0, WHITE);
		selected_rec.x = main_menu_item[selected_item].pos_x - FIELD_BORDER_WIDTH / 2;
		selected_rec.y = main_menu_item[selected_item].pos_y - FIELD_BORDER_WIDTH / 2;
		draw_frame(selected_rec, RED);
		/* draw all menu icons */
		for(unsigned char i = 0; i < NUM_OF_ICONS; i++)
			DrawTexture(
				main_menu_item[i].txt,
				main_menu_item[i].pos_x,
				main_menu_item[i].pos_y,
				WHITE
			);
		EndDrawing();
		//----------------------------------------------------------------------
	}
	//--------------------------------------------------------------------------

	#if defined(__SWITCH__)
		romfsExit();
	#endif

	//TODO: does not work cause the thread is already dead
	//bcmx_ctl(BCMX_END);														// stop background music thread


	bcmx_thread_ctl = BCMX_END;
	pthread_join(bcmx_thread, NULL);											// Wait for the music thread to finish
	for(unsigned char i = 0; i < NUM_OF_ICONS; i++)
		UnloadTexture(main_menu_item[i].txt);									// unload textures
	for(unsigned char i = 0; i < sizeof(snfx) / sizeof(Sound); i++)
		UnloadSound(snfx[i]);													// unload sounds
	return 0;
}
