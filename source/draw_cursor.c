#include "defs.h"
#include "common.h"

void draw_cursor(unsigned char cursor_pos, unsigned short margin_from_left) {
	/* cursor at position 0 */
	Vector2 cursor_v[3] = {														// cursor polygon
		/* v0________v2
			\		 /
			 \		/
			  \    /
			   \  /
				\/
				v1
		*/
		{0.3 * BLOCK_SIZE, TOP_MARGIN - 25},									// v0
		{BLOCK_SIZE / 2, TOP_MARGIN - 25 + BLOCK_SIZE / 2},						// v1
		{BLOCK_SIZE - 0.3 * BLOCK_SIZE, TOP_MARGIN - 25}						// v2
	};

	/* move cursor to right as necessary */
	cursor_v[0].x += BLOCK_SIZE * cursor_pos + margin_from_left;
	cursor_v[1].x += BLOCK_SIZE * cursor_pos + margin_from_left;
	cursor_v[2].x += BLOCK_SIZE * cursor_pos + margin_from_left;

	DrawTriangle(cursor_v[0], cursor_v[1], cursor_v[2], DARKROSE);
}
