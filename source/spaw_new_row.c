
#include "defs.h"
#include "common.h"

// return selected char
char spaw_new_row(field block[FIELD_H][FIELD_W], char palette[FIELD_W]) {
	char select = 0;
	for (unsigned char i = 0; i < FIELD_W; i++)
		if (GetRandomValue(0, 100) > 80) { 										// set letters occurrence rate
			block[FIELD_H - 1][i].val = GetRandomValue('A', 'Z');
			palette[i] = block[FIELD_H - 1][i].val;
			select = palette[i];
		}
		else
			block[FIELD_H - 1][i].val = BLOCK_TYPE_FREE;

	if (!select) {																// in case of zero palette asign something at last
		block[FIELD_H - 1][FIELD_W - 1].val = GetRandomValue('A', 'Z');
		palette[FIELD_W - 1] = block[FIELD_H - 1][FIELD_W - 1].val;
		select = palette[FIELD_W - 1];
	}
	return select;
}
