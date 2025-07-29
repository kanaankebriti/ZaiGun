#include <raylib.h>
#include "defs.h"
#include "const.h"

#define SLIDER_HEIGHT 20
#define SLIDER_WIDTH WIN_W - 15
#define SLIDER_Y_POS WIN_H - 27
#define RULER_TXT_W 64				/* ruler texture width */
#define RULER_TXT_H 64				/* ruler texture height */
#define RULER_W 34					/* ruler width as it appears on the screen */
#define RULER_H 34					/* ruler height as it appears on the screen  */
/* slider that illustrates who is winning */
void draw_slider(Texture2D ruler_txt, short score) {
	Rectangle ruler_dst;
	static const Rectangle ruler_src = { 0, 0, RULER_TXT_W, RULER_TXT_H };
	static const Vector2 ruler_origin = {20, 20};
	static const Rectangle slider_border = {
		7,					/* border left most position */
		SLIDER_Y_POS,		/* border top most position */
		SLIDER_WIDTH,		/* border right most position */
		SLIDER_HEIGHT
	};
	DrawRectangleRoundedLinesEx(
		slider_border,
		FRAME_ROUNDNESS,
		0,
		FIELD_BORDER_WIDTH,
		RAYGHOST
	);																			/* draw slider frame */

	/* draw the filled portion */
	if (score != 0) {
		if (score > 0)
			DrawRectangle(WIN_W/2, SLIDER_Y_POS, score, SLIDER_HEIGHT, COLOR_P2);
		else
			DrawRectangle(WIN_W/2 + score, SLIDER_Y_POS, -score, SLIDER_HEIGHT, COLOR_P1);
	}

	/* scale down ruler texture */
	ruler_dst.width = RULER_W;
	ruler_dst.height = RULER_H;
	ruler_dst.x = (WIN_W + 9)/2 + score;
	ruler_dst.y = SLIDER_Y_POS + 13;

	/* draw the ruler texture */
	DrawTexturePro(
		ruler_txt,
		ruler_src,
		ruler_dst,
		ruler_origin,
		0.f,
		WHITE
	);
}
