#if defined(__SWITCH__)
	#include <switch.h>
#endif
#include <raylib.h>

typedef struct {
	int x_pos;
	int y_pos;
	char val;
} field;

void draw_cursor(unsigned char cursor_pos, unsigned short margin_from_left);
char move_cursor(unsigned char cursor_pos, char dir, char steps);
char spaw_new_row(field block[FIELD_H][FIELD_W], char palette[FIELD_W]);
//private //void draw_block (int block_x_pos, int block_y_pos, char block_val, Font font, char font_w);
void draw_blocks(unsigned char cursor_pos_p1, field block[FIELD_H][FIELD_W], Font blkfont, char font_w);
void draw_frame(Rectangle field_rect , Color fcolor);
char get_cmd(void);
void bcmx_ctl(unsigned char ctr);
void draw_pause_menu(unsigned char pause_menu_cursor);

/* game modes */
bool arcade(const Sound* snfx);
bool multiplayer(const Sound* snfx);
bool vscpu(const Sound* snfx);
