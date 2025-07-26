#include "defs.h"
#include "common.h"

void draw_block (
	int block_x_pos,
	int block_y_pos,
	char block_val,
	Font font,
	char font_w)
{
	char current_block_letter = block_val;
	Vector2 block_pos;				/*TODO: perhaps eliminate this to			// temporary position */
	block_pos.x = block_x_pos + (font_w) / 2;
	block_pos.y = block_y_pos;

	switch (current_block_letter) {
	case BLOCK_TYPE_FREE:														/* unoccupied block */
		DrawRectangle(
			block_x_pos,
			block_y_pos,
			BLOCK_SIZE - BLOCK_MARGIN,
			BLOCK_SIZE - BLOCK_MARGIN,
			BLACK
		);
						break;
	case BLOCK_TYPE_OCCU:														/* occupied block */
		DrawRectangle(
			block_x_pos,
			block_y_pos,
			BLOCK_SIZE - BLOCK_MARGIN,
			BLOCK_SIZE - BLOCK_MARGIN,
			LIGHTGRAY
		);
		break;
	default:																	/* fully assigned block */
		DrawRectangle(
			block_x_pos,
			block_y_pos,
			BLOCK_SIZE - BLOCK_MARGIN,
			BLOCK_SIZE - BLOCK_MARGIN,
			RAYWHITE
		);																		/* block background */
		DrawTextCodepoint(
			font,
			current_block_letter,
			block_pos,
			BLOCK_SIZE,
			BLUE
		);																		/* block letter */
	}
}

void draw_blocks(unsigned char cursor_pos_p1, field block[FIELD_H][FIELD_W], Font blkfont, char font_w) {
	register unsigned char i, j;

	/* draw blocks one by one */
	for (i = 0; i < FIELD_W; i++)
		/* draw selected column's blocks who are on top of an uccupied block
		using a difrrent color */
		if (i == cursor_pos_p1) {
			/* up to an occupied block */
			for (j = 0; j < FIELD_H; j++)
				if (block[j][i].val == BLOCK_TYPE_FREE) {
					DrawRectangle(
						block[j][i].x_pos,
						block[j][i].y_pos,
						BLOCK_SIZE - BLOCK_MARGIN,
						BLOCK_SIZE - BLOCK_MARGIN,
						CURSORCOLCOLOR
					);
				} else
					break;

			/* draw the reset using normal scheme */
			for (; j < FIELD_H; j++)
				draw_block(
					block[j][i].x_pos,
					block[j][i].y_pos,
					block[j][i].val,
					blkfont,
					font_w
				);
		} else
			/* other columns than the one selected by player's cursor */
			for (j = 0; j < FIELD_H; j++)
				draw_block(
					block[j][i].x_pos,
					block[j][i].y_pos,
					block[j][i].val,
					blkfont,
					font_w
				);
}
