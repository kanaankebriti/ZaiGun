#include <raylib.h>
#include <rlgl.h>
#include "defs.h"
#include "common.h"
#include "const.h"

void draw_background(Rectangle field_rect) {
	DrawRectangleRec(field_rect, CHARCOAL);
}

void draw_border_with_round_corners(Rectangle field_rect , Color fcolor) {
	DrawRectangleRoundedLinesEx(
		field_rect,
		FRAME_ROUNDNESS,
		4,
		FIELD_BORDER_WIDTH,
		fcolor
	);
}

/**
 * @brief Draws a rectangle frame with cut (chamfered) corners.
 *
 * @param rec The base rectangle definition.
 * @param cutLength The length of the cut from the corner along the edge.
 * @param color The color of the frame.
 */
void DrawRectangleCutLinesEx(Rectangle rec, float lineThick, float cutLength, Color color)
{
	float halfWidth;
	float halfHeight;
	float maxCut;
	Vector2 innerPoints[8];
	Vector2 outerPoints[8];
	unsigned char i, j;

	if (cutLength < 0) return;

	halfWidth = rec.width / 2.0f;
	halfHeight = rec.height / 2.0f;
	maxCut = (halfWidth < halfHeight) ? halfWidth : halfHeight;
	if (cutLength > maxCut) cutLength = maxCut;

	/* Define the 8 vertices of the inner octagon */
	innerPoints[0].x = rec.x + cutLength;
	innerPoints[0].y = rec.y;
	innerPoints[1].x = rec.x + rec.width - cutLength;
	innerPoints[1].y = rec.y;
	innerPoints[2].x = rec.x + rec.width;
	innerPoints[2].y = rec.y + cutLength;
	innerPoints[3].x = rec.x + rec.width;
	innerPoints[3].y = rec.y + rec.height - cutLength;
	innerPoints[4].x = rec.x + rec.width - cutLength;
	innerPoints[4].y = rec.y + rec.height;
	innerPoints[5].x = rec.x + cutLength;
	innerPoints[5].y = rec.y + rec.height;
	innerPoints[6].x = rec.x;
	innerPoints[6].y = rec.y + rec.height - cutLength;
	innerPoints[7].x = rec.x;
	innerPoints[7].y = rec.y + cutLength;

	/* Define the 8 vertices of the outer octagon */
	outerPoints[0].x = rec.x + cutLength;
	outerPoints[0].y = rec.y - lineThick;
	outerPoints[1].x = rec.x + rec.width - cutLength;
	outerPoints[1].y = rec.y - lineThick;
	outerPoints[2].x = rec.x + rec.width + lineThick;
	outerPoints[2].y = rec.y + cutLength;
	outerPoints[3].x = rec.x + rec.width + lineThick;
	outerPoints[3].y = rec.y + rec.height - cutLength;
	outerPoints[4].x = rec.x + rec.width - cutLength;
	outerPoints[4].y = rec.y + rec.height + lineThick;
	outerPoints[5].x = rec.x + cutLength;
	outerPoints[5].y = rec.y + rec.height + lineThick;
	outerPoints[6].x = rec.x - lineThick;
	outerPoints[6].y = rec.y + rec.height - cutLength;
	outerPoints[7].x = rec.x - lineThick;
	outerPoints[7].y = rec.y + cutLength;

	Texture2D texShapes = GetShapesTexture();
	rlSetTexture(texShapes.id);
	Rectangle shapeRect = GetShapesTextureRectangle();

	rlBegin(RL_QUADS);
	rlColor4ub(color.r, color.g, color.b, color.a);

	/* Draw the 8 quadrilateral segments that form the frame */
	for (i = 0; i < 8; i++) {
		j = (i + 1) % 8; /* Index of the next vertex, wrapping around */
		rlColor4ub(color.r, color.g, color.b, color.a);
		rlTexCoord2f(shapeRect.x/texShapes.width, shapeRect.y/texShapes.height);
		rlVertex2f(outerPoints[i].x, outerPoints[i].y);
		rlVertex2f(innerPoints[i].x, innerPoints[i].y);
		rlVertex2f(innerPoints[j].x, innerPoints[j].y);
		rlVertex2f(outerPoints[j].x, outerPoints[j].y);
	}

	rlEnd();
	rlSetTexture(0);
}

void draw_frame(Rectangle field_rect , Color fcolor, unsigned char frame_type) {
	switch (frame_type)
	{
	case FRAME_TYPE_ROUNDED:
		draw_border_with_round_corners(field_rect , fcolor);
		break;
	case FRAME_TYPE_CUT:
		DrawRectangleCutLinesEx(field_rect, FIELD_BORDER_WIDTH, 1.f, fcolor);
		break;
	default:
		break;
	}
	draw_background(field_rect);
}
