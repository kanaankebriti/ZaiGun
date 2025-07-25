#include "defs.h"
#include "common.h"

/* draw a frame around a rect */
void draw_frame(Rectangle field_rect , Color fcolor) {
	DrawRectangleRoundedLinesEx(
			field_rect,
			FRAME_ROUNDNESS,
			4,
			FIELD_BORDER_WIDTH,
			fcolor
		);																		// frame
	DrawRectangleRec(
		field_rect,
		CHARCOAL
	);																			// grid
}
