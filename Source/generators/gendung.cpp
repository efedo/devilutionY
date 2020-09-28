/**
 * @file gendung.cpp
 *
 * Implementation of general dungeon generation code.
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

uint8_t *pSetPiece;
bool setloadflag;
uint8_t *pSpeedCels;
int SpeedFrameTbl[128][16];
/**
 * List of transparancy masks to use for dPieces
 */
int level_frame_count[MAXTILES];
int tile_defs[MAXTILES];
WORD level_frame_types[MAXTILES];
int level_frame_sizes[MAXTILES];
int nlevel_frames;

int gnDifficulty;

V2Di View;
V2Di ViewB;
V2Di ViewD;
V2Di LvlView;

ScrollStruct ScrollInfo;
int MicroTileLen;

void SetDungeonMicros()
{
	int i, x, y, lv, blocks;
	WORD *pPiece;
	MICROS *pMap;

	if (lvl.type() == DunType::town) {
		MicroTileLen = 16;
		blocks = 16;
	} else if (lvl.type() != DunType::hell) {
		MicroTileLen = 10;
		blocks = 10;
	} else {
		MicroTileLen = 12;
		blocks = 16;
	}

	for (y = 0; y < MAXDUNY; y++) {
		for (x = 0; x < MAXDUNX; x++) {
			pMap = &grid[x][y].dpiece_defs_map_2;
			if (grid[x][y].isPiece()) {
				lv = grid[x][y].getPiece();
				if (lvl.type() != DunType::hell && lvl.type() != DunType::town)
					pPiece = (WORD *)&lvl.pLevelPieces[20 * lv];
				else
					pPiece = (WORD *)&lvl.pLevelPieces[32 * lv];
				for (i = 0; i < blocks; i++)
					pMap->mt[i] = SDL_SwapLE16(pPiece[(i & 1) + blocks - 2 - (i & 0xE)]);
			} else {
				for (i = 0; i < blocks; i++)
					pMap->mt[i] = 0;
			}
		}
	}

	//for (y = 0; y < MAXDUNY; y++) {
	//	for (x = 0; x < MAXDUNX; x++) {
	//		if (!grid[x][y].isPiece())
	//			continue;
	//		lv = grid[x][y].getPiece();
	//		pMap = &grid[x][y].dpiece_defs_map_2;
	//		if (lv != 0) {
	//			lv--;
	//			if (lvl.type() != DunType::hell && lvl.type() != DunType::town)
	//				pPiece = (WORD *)&pLevelPieces[20 * lv];
	//			else
	//				pPiece = (WORD *)&pLevelPieces[32 * lv];
	//			for (i = 0; i < blocks; i++)
	//				pMap->mt[i] = SDL_SwapLE16(pPiece[(i & 1) + blocks - 2 - (i & 0xE)]);
	//		} else {
	//			for (i = 0; i < blocks; i++)
	//				pMap->mt[i] = 0;
	//		}
	//	}
	//}
}

void DRLG_InitTrans()
{
	grid.clearTrans();
	memset(lvl.TransList, 0, sizeof(lvl.TransList));
	lvl.TransVal = 1;
}

void DRLG_MRectTrans(int x1, int y1, int x2, int y2)
{
	int i, j;

	x1 = 2 * x1 + 17;
	y1 = 2 * y1 + 17;
	x2 = 2 * x2 + 16;
	y2 = 2 * y2 + 16;

	for (j = y1; j <= y2; j++) {
		for (i = x1; i <= x2; i++) {
			grid[i][j].dTransVal = lvl.TransVal;
		}
	}

	lvl.TransVal++;
}

void DRLG_RectTrans(int x1, int y1, int x2, int y2)
{
	int i, j;

	for (j = y1; j <= y2; j++) {
		for (i = x1; i <= x2; i++) {
			grid[i][j].dTransVal = lvl.TransVal;
		}
	}
	lvl.TransVal++;
}

void DRLG_CopyTrans(int sx, int sy, int dx, int dy)
{
	grid[dx][dy].dTransVal = grid[sx][sy].dTransVal;
}

void DRLG_ListTrans(int num, uint8_t *List)
{
	int i;
	uint8_t x1, y1, x2, y2;

	for (i = 0; i < num; i++) {
		x1 = *List++;
		y1 = *List++;
		x2 = *List++;
		y2 = *List++;
		DRLG_RectTrans(x1, y1, x2, y2);
	}
}

void DRLG_AreaTrans(int num, uint8_t *List)
{
	int i;
	uint8_t x1, y1, x2, y2;

	for (i = 0; i < num; i++) {
		x1 = *List++;
		y1 = *List++;
		x2 = *List++;
		y2 = *List++;
		DRLG_RectTrans(x1, y1, x2, y2);
		lvl.TransVal--;
	}
	lvl.TransVal++;
}

void DRLG_InitSetPC()
{
	lvl.setpc({ 0, 0, 0, 0 });
}

void DRLG_SetPC()
{
	int i, j, x, y, w, h;

	w = 2 * lvl.getpc().w;
	h = 2 * lvl.getpc().h;
	x = 2 * lvl.getpc().x + 16;
	y = 2 * lvl.getpc().y + 16;

	for (j = 0; j < h; j++) {
		for (i = 0; i < w; i++) {
			grid[i + x][j + y].dFlags |= BFLAG_POPULATED;
		}
	}
}

void Make_SetPC(int x, int y, int w, int h)
{
	int i, j, dx, dy, dh, dw;

	dw = 2 * w;
	dh = 2 * h;
	dx = 2 * x + 16;
	dy = 2 * y + 16;

	for (j = 0; j < dh; j++) {
		for (i = 0; i < dw; i++) {
			grid[i + dx][j + dy].dFlags |= BFLAG_POPULATED;
		}
	}
}

bool DRLG_WillThemeRoomFit(int floor, int x, int y, int minSize, int maxSize, int *width, int *height)
{
	int ii, xx, yy;
	int xSmallest, ySmallest;
	int xArray[20], yArray[20];
	int xCount, yCount;
	bool yFlag, xFlag;

	yFlag = TRUE;
	xFlag = TRUE;
	xCount = 0;
	yCount = 0;

	// BUGFIX: change '&&' to '||' (fixed)
	if (x > DMAXX - maxSize || y > DMAXY - maxSize) {
		return FALSE;
	}
	if (!SkipThemeRoom(x, y)) {
		return FALSE;
	}

	memset(xArray, 0, sizeof(xArray));
	memset(yArray, 0, sizeof(yArray));

	for (ii = 0; ii < maxSize; ii++) {
		if (xFlag) {
			for (xx = x; xx < x + maxSize; xx++) {
				if (dgrid[xx][y + ii].dungeon != floor) {
					if (xx >= minSize) {
						break;
					}
					xFlag = FALSE;
				} else {
					xCount++;
				}
			}
			if (xFlag) {
				xArray[ii] = xCount;
				xCount = 0;
			}
		}
		if (yFlag) {
			for (yy = y; yy < y + maxSize; yy++) {
				if (dgrid[x + ii][yy].dungeon != floor) {
					if (yy >= minSize) {
						break;
					}
					yFlag = FALSE;
				} else {
					yCount++;
				}
			}
			if (yFlag) {
				yArray[ii] = yCount;
				yCount = 0;
			}
		}
	}

	for (ii = 0; ii < minSize; ii++) {
		if (xArray[ii] < minSize || yArray[ii] < minSize) {
			return FALSE;
		}
	}

	xSmallest = xArray[0];
	ySmallest = yArray[0];

	for (ii = 0; ii < maxSize; ii++) {
		if (xArray[ii] < minSize || yArray[ii] < minSize) {
			break;
		}
		if (xArray[ii] < xSmallest) {
			xSmallest = xArray[ii];
		}
		if (yArray[ii] < ySmallest) {
			ySmallest = yArray[ii];
		}
	}

	*width = xSmallest - 2;
	*height = ySmallest - 2;
	return TRUE;
}

void DRLG_CreateThemeRoom(int themeIndex)
{
	lvl.DRLG_CreateThemeRoom(themeIndex);
}

void DRLG_PlaceThemeRooms(int minSize, int maxSize, int floor, int freq, int rndSize)
{
	int i, j;
	int themeW, themeH;
	int rv2, min, max;

	lvl.themeCount = 0;
	memset(lvl.themeLoc, 0, sizeof(*lvl.themeLoc));
	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dgrid[i][j].dungeon == floor && !random_(0, freq) && DRLG_WillThemeRoomFit(floor, i, j, minSize, maxSize, &themeW, &themeH)) {
				if (rndSize) {
					min = minSize - 2;
					max = maxSize - 2;
					rv2 = min + random_(0, random_(0, themeW - min + 1));
					if (rv2 >= min && rv2 <= max)
						themeW = rv2;
					else
						themeW = min;
					rv2 = min + random_(0, random_(0, themeH - min + 1));
					if (rv2 >= min && rv2 <= max)
						themeH = rv2;
					else
						themeH = min;
				}
				lvl.themeLoc[lvl.themeCount].x = i + 1;
				lvl.themeLoc[lvl.themeCount].y = j + 1;
				lvl.themeLoc[lvl.themeCount].width = themeW;
				lvl.themeLoc[lvl.themeCount].height = themeH;
				if (lvl.type() == DunType::caves)
					DRLG_RectTrans(2 * i + 20, 2 * j + 20, 2 * (i + themeW) + 15, 2 * (j + themeH) + 15);
				else
					DRLG_MRectTrans(i + 1, j + 1, i + themeW, j + themeH);
				lvl.themeLoc[lvl.themeCount].ttval = lvl.TransVal - 1;
				DRLG_CreateThemeRoom(lvl.themeCount);
				lvl.themeCount++;
			}
		}
	}
}

void DRLG_HoldThemeRooms()
{
	int i, x, y, xx, yy;

	for (i = 0; i < lvl.themeCount; i++) {
		for (y = lvl.themeLoc[i].y; y < lvl.themeLoc[i].y + lvl.themeLoc[i].height - 1; y++) {
			for (x = lvl.themeLoc[i].x; x < lvl.themeLoc[i].x + lvl.themeLoc[i].width - 1; x++) {
				xx = 2 * x + 16;
				yy = 2 * y + 16;
				grid[xx][yy].dFlags |= BFLAG_POPULATED;
				grid[xx + 1][yy].dFlags |= BFLAG_POPULATED;
				grid[xx][yy + 1].dFlags |= BFLAG_POPULATED;
				grid[xx + 1][yy + 1].dFlags |= BFLAG_POPULATED;
			}
		}
	}
}

bool SkipThemeRoom(int x, int y)
{
	int i;

	for (i = 0; i < lvl.themeCount; i++) {
		if (x >= lvl.themeLoc[i].x - 2 && x <= lvl.themeLoc[i].x + lvl.themeLoc[i].width + 2
		    && y >= lvl.themeLoc[i].y - 2 && y <= lvl.themeLoc[i].y + lvl.themeLoc[i].height + 2)
			return FALSE;
	}

	return TRUE;
}

void InitLevels()
{
	if (!leveldebug) {
		lvl.currlevel = 0;
		lvl.setType(DunType::town);
		lvl.setlevel = FALSE;
	}
}

DEVILUTION_END_NAMESPACE
