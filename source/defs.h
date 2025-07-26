#if defined(__SWITCH__)
	#define WIN_W 1280
	#define WIN_H 720
	#define RESOURCE_PATH "romfs:/"
	#define PAUSE_MENU_ITEMS_COUNT 2											/* a console game shall not provide an "exit to OS" option */
#else
	#define WIN_W 1280
	#define WIN_H 720
	#define RESOURCE_PATH "romfs/"
	#define PAUSE_MENU_ITEMS_COUNT 3
#endif

#define FPS 60																	/* default framerate */

#define DEFBACKCOLOR DARKGRAY													/* background color */
#define CURSORCOLCOLOR DARKERGRAY												/* cursor column color */

#define CURSOR_MOV_R 0															/* cursor unit move to right */
#define CURSOR_MOV_L 1															/* cursor unit move to left */
#define CURSOR_MOV_U 1															/* cursor move units */
#define CURSOR_JMP_U 3															/* cursor jump units */
#define JOY_ULCK_SPD 4															/* analog joystick unlocking speed of locking mechanism */
#define MAIN_MENU_ICON_WH 256													/* texture and display size of icons of main menu */

/* animating background used for arcade mode */
#define BG_MAX_LETTERS 100														/* letters count for animating background of arcade mode */
#define BG_FONT_MIN 10
#define BG_FONT_MAX 70

/* timings */
#define MAX_INTERVAL 4.0f														/* maximum waiting */
#define MIN_INTERVAL 1.5f														/* minimum waiting */
#define SCORE_CAP 100000.0f														/* new spawn is score based */

/* block fields */
#define GRIDCOLOR CHARCOAL														/* grid color */
#define BLOCK_TYPE_FREE 0
#define BLOCK_TYPE_OCCU 1
#define BLOCK_SIZE 32															/* size of each block in pixels */
#define BLOCK_MARGIN 2															/* won't affect BLOCK_SIZE */
#define FIELD_W 10																/* number of horizontal blocks */
#define FIELD_H 20																/* number of vertical blocks */
#define FIELD_BORDER_WIDTH 7													/* border width around the field */
#define FRAME_ROUNDNESS .04f													/* frame roundness */
#define TOP_MARGIN (WIN_H - FIELD_H * BLOCK_SIZE) / 2							/* vertical alignment */
#define LEFT_MARGIN (WIN_W - FIELD_W * BLOCK_SIZE) / 2							/* horizontal alignment */
#define LEFT_MARGIN_MUL_P1 (WIN_W / 2 - FIELD_W * BLOCK_SIZE + 2 * BLOCK_SIZE) / 2	/* multiplayer: horizontal alignment + move two blocks to right to have space for selected char */
#define LEFT_MARGIN_MUL_P2 (WIN_W / 2 + LEFT_MARGIN_MUL_P1)						/* multiplayer: middle of the screen + margin of player 1 */
#define COLOR_P1 CYAN															/* main color theme use for player 1 */
#define COLOR_P2 DARKROSE														/* main color theme use for player 2 */

/* user input commands */
#define CMD_NULL 255																/* command: nothing! mainly to silence warnings */
#define CMD_CURSOR_RIGHT_P1 1													/* command: player 1 cursor move to right */
#define CMD_CURSOR_LEFT_P1 2													/* command: player 1 cursor move to left */
#define CMD_CURSOR_UP_P1 3
#define CMD_CURSOR_DOWN_P1 4
#define CMD_CURSOR_JRIGHT_P1 5													/* command: player 1 cursor jump to right */
#define CMD_CURSOR_JLEFT_P1 6													/* command: player 1 cursor jump to left */
#define CMD_ELIMINATE_P1 7														/* command: player 1 block elimination */
#define CMD_CURSOR_RIGHT_P2 8													/* command: player 2 cursor move to right */
#define CMD_CURSOR_LEFT_P2 9													/* command: player 2 cursor move to left */
#define CMD_CURSOR_JRIGHT_P2 10													/* command: player 2 cursor jump to right */
#define CMD_CURSOR_JLEFT_P2 11													/* command: player 2 cursor jump to left */
#define CMD_ELIMINATE_P2 12														/* command: player 2 block elimination */
#define CMD_PAUSE 13															/* command: stop game flow */

/* sound effects index */
#define SNFX_MN 0																/* menu navigation */
#define SNFX_MV 1																/* cursor move sound effect */
#define SNFX_EL 2																/* block elimination sound effect */
#define SNFX_FCL 3 																/* field cleared sound effect */
#define SNFX_WA 4
#define SNFX_PU 5																/* game paused */

/* game execution state */
#define GAME_STAT_RUN 0															/* game is running */
#define GAME_STAT_PAUSE 1														/* game is paused */
#define GAME_STAT_END 2															/* game is ended */
#define GAME_STAT_EXIT 3														/* exit to OS */

/* colors */
#define CHARCOAL	CLITERAL(Color){ 43, 43, 43, 255 }							/* color: CHARCOAL */
#define CHERENKOV	CLITERAL(Color){ 170, 200, 220, 255 }						/* color: CHERENKOV */
#define CYAN		CLITERAL(Color){ 0, 255, 255, 255 }	 						/* color: CYAN */
#define DARKESTGRAY	CLITERAL(Color){ 25, 25, 28, 255 }							/* color: DARKESTGRAY */
#define DARKERGRAY	CLITERAL(Color){ 35, 35, 35, 255 }							/* color: DARKERGRAY */
#define DARKROSE	CLITERAL(Color){ 230, 41, 55, 255 }							/* color: DARKROSE */
#define RAYGHOST	CLITERAL(Color){ 245, 245, 245, 245 }						/* color: RAYGHOST */
