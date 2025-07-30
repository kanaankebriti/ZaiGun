#if defined(__SWITCH__)
#include <switch.h>
#endif
#include <raylib.h>
#include "defs.h"

typedef struct {
	int x_pos;			/* block x position */
	int y_pos;			/* block y position */
	signed char val;	/* block character. note that 'signed' is required when ARM is targeted. */
} field;

unsigned char get_cmd(void);
float get_interval_linear(int score);
float get_interval_quadratic(int score);
/*private //void draw_block (int block_x_pos, int block_y_pos, char block_val, Font font, char font_w); */
void draw_blocks(unsigned char cursor_pos_p1, field block[FIELD_H][FIELD_W], Font blkfont, char font_w);
void draw_cursor(unsigned char cursor_pos, unsigned short margin_from_left);
void draw_frame(Rectangle field_rect , Color fcolor, unsigned char frame_type);
void draw_pause_menu(unsigned char pause_menu_cursor);
void draw_slider(Texture2D ruler_txt, short score);
char move_cursor(unsigned char cursor_pos, char dir, char steps);
char spaw_new_row(field block[FIELD_H][FIELD_W], signed char palette[FIELD_W]);

/* game modes */
bool arcade(Sound* snfx);
bool multiplayer(Sound* snfx);
bool vscpu(Sound* snfx);
