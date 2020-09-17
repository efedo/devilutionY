/**
 * @file gendung.cpp
 *
 * Implementation of general dungeon generation code.
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

DGrid dgrid;
Grid grid;

RECT32 setpc;
BYTE *pSetPiece;
BOOL setloadflag;
BYTE *pSpecialCels;
BYTE *pMegaTiles;
BYTE *pLevelPieces;
BYTE *pDungeonCels;
BYTE *pSpeedCels;
int SpeedFrameTbl[128][16];
/**
 * List of transparancy masks to use for dPieces
 */
char block_lvid[MAXTILES + 1];
int level_frame_count[MAXTILES];
int tile_defs[MAXTILES];
WORD level_frame_types[MAXTILES];
int level_frame_sizes[MAXTILES];
int nlevel_frames;

PieceInventory pieces;

V2Di dmin;
V2Di dmax;

int gnDifficulty;
//BYTE level.leveltype;
//BYTE level.currlevel;
//BOOLEAN level.setlevel;
//BYTE setlvlnum;
//char setlvltype;

Level level;


V2Di View;
V2Di ViewB;
V2Di ViewD;
V2Di LvlView;

ScrollStruct ScrollInfo;
int MicroTileLen;
char TransVal;
BOOLEAN TransList[256];

//int grid[MAXDUNX][MAXDUNY].dPiece;
//MICROS grid[MAXDUNX][MAXDUNY].dpiece_defs_map_2;
MICROS dpiece_defs_map_1[MAXDUNX * MAXDUNY];
//char grid[MAXDUNX][MAXDUNY].dTransVal;
//char grid[MAXDUNX][MAXDUNY].dLight;
//char grid[MAXDUNX][MAXDUNY].dPreLight;
//char grid[MAXDUNX][MAXDUNY].dFlags;
//char grid[MAXDUNX][MAXDUNY].dPlayer;
//int grid[MAXDUNX][MAXDUNY].dMonster;
//char grid[MAXDUNX][MAXDUNY].dDead;
//char grid[MAXDUNX][MAXDUNY].dObject;
//char grid[MAXDUNX][MAXDUNY].dItem;
//char grid[MAXDUNX][MAXDUNY].dMissile;
//char grid[MAXDUNX][MAXDUNY].dSpecial;
int themeCount;
THEME_LOC themeLoc[MAXTHEMES];

void FillSolidBlockTbls()
{
	BYTE bv;
	DWORD dwTiles;
	BYTE *pSBFile, *pTmp;
	int i;

	pieces.clear();

	switch (level.leveltype) {
	case DTYPE_TOWN:
		pSBFile = LoadFileInMem("Levels\\TownData\\Town.SOL", &dwTiles);
		break;
	case DTYPE_CATHEDRAL:
		pSBFile = LoadFileInMem("Levels\\L1Data\\L1.SOL", &dwTiles);
		break;
	case DTYPE_CATACOMBS:
		pSBFile = LoadFileInMem("Levels\\L2Data\\L2.SOL", &dwTiles);
		break;
	case DTYPE_CAVES:
		pSBFile = LoadFileInMem("Levels\\L3Data\\L3.SOL", &dwTiles);
		break;
	case DTYPE_HELL:
		pSBFile = LoadFileInMem("Levels\\L4Data\\L4.SOL", &dwTiles);
		break;
	default:
		app_fatal("FillSolidBlockTbls");
	}

	pTmp = pSBFile;

	for (i = 1; i <= dwTiles; i++) {
		bv = *pTmp++;
		if (bv & 1)
			pieces[i].nSolidTable = TRUE;
		if (bv & 2)
			pieces[i].nBlockTable = TRUE;
		if (bv & 4)
			pieces[i].nMissileTable = TRUE;
		if (bv & 8)
			pieces[i].nTransTable = TRUE;
		if (bv & 0x80)
			pieces[i].nTrapTable = TRUE;
		block_lvid[i] = (bv & 0x70) >> 4; /* beta: (bv >> 4) & 7 */
	}

	mem_free_dbg(pSBFile);
}

void SetDungeonMicros()
{
	int i, x, y, lv, blocks;
	WORD *pPiece;
	MICROS *pMap;

	if (level.leveltype == DTYPE_TOWN) {
		MicroTileLen = 16;
		blocks = 16;
	} else if (level.leveltype != DTYPE_HELL) {
		MicroTileLen = 10;
		blocks = 10;
	} else {
		MicroTileLen = 12;
		blocks = 16;
	}

	for (y = 0; y < MAXDUNY; y++) {
		for (x = 0; x < MAXDUNX; x++) {
			lv = grid[x][y].dPiece;
			pMap = &grid[x][y].dpiece_defs_map_2;
			if (lv != 0) {
				lv--;
				if (level.leveltype != DTYPE_HELL && level.leveltype != DTYPE_TOWN)
					pPiece = (WORD *)&pLevelPieces[20 * lv];
				else
					pPiece = (WORD *)&pLevelPieces[32 * lv];
				for (i = 0; i < blocks; i++)
					pMap->mt[i] = SDL_SwapLE16(pPiece[(i & 1) + blocks - 2 - (i & 0xE)]);
			} else {
				for (i = 0; i < blocks; i++)
					pMap->mt[i] = 0;
			}
		}
	}
}

void DRLG_InitTrans()
{
	grid.clearTrans();
	memset(TransList, 0, sizeof(TransList));
	TransVal = 1;
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
			grid[i][j].dTransVal = TransVal;
		}
	}

	TransVal++;
}

void DRLG_RectTrans(int x1, int y1, int x2, int y2)
{
	int i, j;

	for (j = y1; j <= y2; j++) {
		for (i = x1; i <= x2; i++) {
			grid[i][j].dTransVal = TransVal;
		}
	}
	TransVal++;
}

void DRLG_CopyTrans(int sx, int sy, int dx, int dy)
{
	grid[dx][dy].dTransVal = grid[sx][sy].dTransVal;
}

#ifndef SPAWN
void DRLG_ListTrans(int num, BYTE *List)
{
	int i;
	BYTE x1, y1, x2, y2;

	for (i = 0; i < num; i++) {
		x1 = *List++;
		y1 = *List++;
		x2 = *List++;
		y2 = *List++;
		DRLG_RectTrans(x1, y1, x2, y2);
	}
}

void DRLG_AreaTrans(int num, BYTE *List)
{
	int i;
	BYTE x1, y1, x2, y2;

	for (i = 0; i < num; i++) {
		x1 = *List++;
		y1 = *List++;
		x2 = *List++;
		y2 = *List++;
		DRLG_RectTrans(x1, y1, x2, y2);
		TransVal--;
	}
	TransVal++;
}
#endif

void DRLG_InitSetPC()
{
	setpc.x = 0;
	setpc.y = 0;
	setpc.w = 0;
	setpc.h = 0;
}

void DRLG_SetPC()
{
	int i, j, x, y, w, h;

	w = 2 * setpc.w;
	h = 2 * setpc.h;
	x = 2 * setpc.x + 16;
	y = 2 * setpc.y + 16;

	for (j = 0; j < h; j++) {
		for (i = 0; i < w; i++) {
			grid[i + x][j + y].dFlags |= BFLAG_POPULATED;
		}
	}
}

#ifndef SPAWN
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

BOOL DRLG_WillThemeRoomFit(int floor, int x, int y, int minSize, int maxSize, int *width, int *height)
{
	int ii, xx, yy;
	int xSmallest, ySmallest;
	int xArray[20], yArray[20];
	int xCount, yCount;
	BOOL yFlag, xFlag;

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
	int xx, yy;

	for (yy = themeLoc[themeIndex].y; yy < themeLoc[themeIndex].y + themeLoc[themeIndex].height; yy++) {
		for (xx = themeLoc[themeIndex].x; xx < themeLoc[themeIndex].x + themeLoc[themeIndex].width; xx++) {
			if (level.leveltype == DTYPE_CATACOMBS) {
				if (yy == themeLoc[themeIndex].y
				        && xx >= themeLoc[themeIndex].x
				        && xx <= themeLoc[themeIndex].x + themeLoc[themeIndex].width
				    || yy == themeLoc[themeIndex].y + themeLoc[themeIndex].height - 1
				        && xx >= themeLoc[themeIndex].x
				        && xx <= themeLoc[themeIndex].x + themeLoc[themeIndex].width) {
					dgrid[xx][yy].dungeon = 2;
				} else if (xx == themeLoc[themeIndex].x
				        && yy >= themeLoc[themeIndex].y
				        && yy <= themeLoc[themeIndex].y + themeLoc[themeIndex].height
				    || xx == themeLoc[themeIndex].x + themeLoc[themeIndex].width - 1
				        && yy >= themeLoc[themeIndex].y
				        && yy <= themeLoc[themeIndex].y + themeLoc[themeIndex].height) {
					dgrid[xx][yy].dungeon = 1;
				} else {
					dgrid[xx][yy].dungeon = 3;
				}
			}
			if (level.leveltype == DTYPE_CAVES) {
				if (yy == themeLoc[themeIndex].y
				        && xx >= themeLoc[themeIndex].x
				        && xx <= themeLoc[themeIndex].x + themeLoc[themeIndex].width
				    || yy == themeLoc[themeIndex].y + themeLoc[themeIndex].height - 1
				        && xx >= themeLoc[themeIndex].x
				        && xx <= themeLoc[themeIndex].x + themeLoc[themeIndex].width) {
					dgrid[xx][yy].dungeon = 134;
				} else if (xx == themeLoc[themeIndex].x
				        && yy >= themeLoc[themeIndex].y
				        && yy <= themeLoc[themeIndex].y + themeLoc[themeIndex].height
				    || xx == themeLoc[themeIndex].x + themeLoc[themeIndex].width - 1
				        && yy >= themeLoc[themeIndex].y
				        && yy <= themeLoc[themeIndex].y + themeLoc[themeIndex].height) {
					dgrid[xx][yy].dungeon = 137;
				} else {
					dgrid[xx][yy].dungeon = 7;
				}
			}
			if (level.leveltype == DTYPE_HELL) {
				if (yy == themeLoc[themeIndex].y
				        && xx >= themeLoc[themeIndex].x
				        && xx <= themeLoc[themeIndex].x + themeLoc[themeIndex].width
				    || yy == themeLoc[themeIndex].y + themeLoc[themeIndex].height - 1
				        && xx >= themeLoc[themeIndex].x
				        && xx <= themeLoc[themeIndex].x + themeLoc[themeIndex].width) {
					dgrid[xx][yy].dungeon = 2;
				} else if (xx == themeLoc[themeIndex].x
				        && yy >= themeLoc[themeIndex].y
				        && yy <= themeLoc[themeIndex].y + themeLoc[themeIndex].height
				    || xx == themeLoc[themeIndex].x + themeLoc[themeIndex].width - 1
				        && yy >= themeLoc[themeIndex].y
				        && yy <= themeLoc[themeIndex].y + themeLoc[themeIndex].height) {
					dgrid[xx][yy].dungeon = 1;
				} else {
					dgrid[xx][yy].dungeon = 6;
				}
			}
		}
	}

	if (level.leveltype == DTYPE_CATACOMBS) {
		dgrid[themeLoc[themeIndex].x][themeLoc[themeIndex].y].dungeon = 8;
		dgrid[themeLoc[themeIndex].x + themeLoc[themeIndex].width - 1][themeLoc[themeIndex].y].dungeon = 7;
		dgrid[themeLoc[themeIndex].x][themeLoc[themeIndex].y + themeLoc[themeIndex].height - 1].dungeon = 9;
		dgrid[themeLoc[themeIndex].x + themeLoc[themeIndex].width - 1][themeLoc[themeIndex].y + themeLoc[themeIndex].height - 1].dungeon = 6;
	}
	if (level.leveltype == DTYPE_CAVES) {
		dgrid[themeLoc[themeIndex].x][themeLoc[themeIndex].y].dungeon = 150;
		dgrid[themeLoc[themeIndex].x + themeLoc[themeIndex].width - 1][themeLoc[themeIndex].y].dungeon = 151;
		dgrid[themeLoc[themeIndex].x][themeLoc[themeIndex].y + themeLoc[themeIndex].height - 1].dungeon = 152;
		dgrid[themeLoc[themeIndex].x + themeLoc[themeIndex].width - 1][themeLoc[themeIndex].y + themeLoc[themeIndex].height - 1].dungeon = 138;
	}
	if (level.leveltype == DTYPE_HELL) {
		dgrid[themeLoc[themeIndex].x][themeLoc[themeIndex].y].dungeon = 9;
		dgrid[themeLoc[themeIndex].x + themeLoc[themeIndex].width - 1][themeLoc[themeIndex].y].dungeon = 16;
		dgrid[themeLoc[themeIndex].x][themeLoc[themeIndex].y + themeLoc[themeIndex].height - 1].dungeon = 15;
		dgrid[themeLoc[themeIndex].x + themeLoc[themeIndex].width - 1][themeLoc[themeIndex].y + themeLoc[themeIndex].height - 1].dungeon = 12;
	}

	if (level.leveltype == DTYPE_CATACOMBS) {
		switch (random_(0, 2)) {
		case 0:
			dgrid[themeLoc[themeIndex].x + themeLoc[themeIndex].width - 1][themeLoc[themeIndex].y + themeLoc[themeIndex].height / 2].dungeon = 4;
			break;
		case 1:
			dgrid[themeLoc[themeIndex].x + themeLoc[themeIndex].width / 2][themeLoc[themeIndex].y + themeLoc[themeIndex].height - 1].dungeon = 5;
			break;
		}
	}
	if (level.leveltype == DTYPE_CAVES) {
		switch (random_(0, 2)) {
		case 0:
			dgrid[themeLoc[themeIndex].x + themeLoc[themeIndex].width - 1][themeLoc[themeIndex].y + themeLoc[themeIndex].height / 2].dungeon = 147;
			break;
		case 1:
			dgrid[themeLoc[themeIndex].x + themeLoc[themeIndex].width / 2][themeLoc[themeIndex].y + themeLoc[themeIndex].height - 1].dungeon = 146;
			break;
		}
	}
	if (level.leveltype == DTYPE_HELL) {
		switch (random_(0, 2)) {
		case 0:
			dgrid[themeLoc[themeIndex].x + themeLoc[themeIndex].width - 1][themeLoc[themeIndex].y + themeLoc[themeIndex].height / 2 - 1].dungeon = 53;
			dgrid[themeLoc[themeIndex].x + themeLoc[themeIndex].width - 1][themeLoc[themeIndex].y + themeLoc[themeIndex].height / 2].dungeon = 6;
			dgrid[themeLoc[themeIndex].x + themeLoc[themeIndex].width - 1][themeLoc[themeIndex].y + themeLoc[themeIndex].height / 2 + 1].dungeon = 52;
			dgrid[themeLoc[themeIndex].x + themeLoc[themeIndex].width - 2][themeLoc[themeIndex].y + themeLoc[themeIndex].height / 2 - 1].dungeon = 54;
			break;
		case 1:
			dgrid[themeLoc[themeIndex].x + themeLoc[themeIndex].width / 2 - 1][themeLoc[themeIndex].y + themeLoc[themeIndex].height - 1].dungeon = 57;
			dgrid[themeLoc[themeIndex].x + themeLoc[themeIndex].width / 2][themeLoc[themeIndex].y + themeLoc[themeIndex].height - 1].dungeon = 6;
			dgrid[themeLoc[themeIndex].x + themeLoc[themeIndex].width / 2 + 1][themeLoc[themeIndex].y + themeLoc[themeIndex].height - 1].dungeon = 56;
			dgrid[themeLoc[themeIndex].x + themeLoc[themeIndex].width / 2][themeLoc[themeIndex].y + themeLoc[themeIndex].height - 2].dungeon = 59;
			dgrid[themeLoc[themeIndex].x + themeLoc[themeIndex].width / 2 - 1][themeLoc[themeIndex].y + themeLoc[themeIndex].height - 2].dungeon = 58;
			break;
		}
	}
}

void DRLG_PlaceThemeRooms(int minSize, int maxSize, int floor, int freq, int rndSize)
{
	int i, j;
	int themeW, themeH;
	int rv2, min, max;

	themeCount = 0;
	memset(themeLoc, 0, sizeof(*themeLoc));
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
				themeLoc[themeCount].x = i + 1;
				themeLoc[themeCount].y = j + 1;
				themeLoc[themeCount].width = themeW;
				themeLoc[themeCount].height = themeH;
				if (level.leveltype == DTYPE_CAVES)
					DRLG_RectTrans(2 * i + 20, 2 * j + 20, 2 * (i + themeW) + 15, 2 * (j + themeH) + 15);
				else
					DRLG_MRectTrans(i + 1, j + 1, i + themeW, j + themeH);
				themeLoc[themeCount].ttval = TransVal - 1;
				DRLG_CreateThemeRoom(themeCount);
				themeCount++;
			}
		}
	}
}
#endif

void DRLG_HoldThemeRooms()
{
	int i, x, y, xx, yy;

	for (i = 0; i < themeCount; i++) {
		for (y = themeLoc[i].y; y < themeLoc[i].y + themeLoc[i].height - 1; y++) {
			for (x = themeLoc[i].x; x < themeLoc[i].x + themeLoc[i].width - 1; x++) {
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

BOOL SkipThemeRoom(int x, int y)
{
	int i;

	for (i = 0; i < themeCount; i++) {
		if (x >= themeLoc[i].x - 2 && x <= themeLoc[i].x + themeLoc[i].width + 2
		    && y >= themeLoc[i].y - 2 && y <= themeLoc[i].y + themeLoc[i].height + 2)
			return FALSE;
	}

	return TRUE;
}

void InitLevels()
{
	if (!leveldebug) {
		level.currlevel = 0;
		level.leveltype = DTYPE_TOWN;
		level.setlevel = FALSE;
	}
}

DEVILUTION_END_NAMESPACE
