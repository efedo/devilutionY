/**
 * @file drlg_l1.cpp
 *
 * Implementation of the cathedral level generation algorithms.
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

BYTE L5dungeon[80][80];
BYTE L5dflags[DMAXX][DMAXY];
BOOL L5setloadflag;
int HR1;
int HR2;
int HR3;
int VR1;
int VR2;
int VR3;
BYTE *L5pSetPiece;

const ShadowStruct SPATS[37] = {
	// clang-format off
	// strig, s1, s2, s3, nv1, nv2, nv3
	{      7, 13,  0, 13, 144,   0, 142 },
	{     16, 13,  0, 13, 144,   0, 142 },
	{     15, 13,  0, 13, 145,   0, 142 },
	{      5, 13, 13, 13, 152, 140, 139 },
	{      5, 13,  1, 13, 143, 146, 139 },
	{      5, 13, 13,  2, 143, 140, 148 },
	{      5,  0,  1,  2,   0, 146, 148 },
	{      5, 13, 11, 13, 143, 147, 139 },
	{      5, 13, 13, 12, 143, 140, 149 },
	{      5, 13, 11, 12, 150, 147, 149 },
	{      5, 13,  1, 12, 143, 146, 149 },
	{      5, 13, 11,  2, 143, 147, 148 },
	{      9, 13, 13, 13, 144, 140, 142 },
	{      9, 13,  1, 13, 144, 146, 142 },
	{      9, 13, 11, 13, 151, 147, 142 },
	{      8, 13,  0, 13, 144,   0, 139 },
	{      8, 13,  0, 12, 143,   0, 149 },
	{      8,  0,  0,  2,   0,   0, 148 },
	{     11,  0,  0, 13,   0,   0, 139 },
	{     11, 13,  0, 13, 139,   0, 139 },
	{     11,  2,  0, 13, 148,   0, 139 },
	{     11, 12,  0, 13, 149,   0, 139 },
	{     11, 13, 11, 12, 139,   0, 149 },
	{     14,  0,  0, 13,   0,   0, 139 },
	{     14, 13,  0, 13, 139,   0, 139 },
	{     14,  2,  0, 13, 148,   0, 139 },
	{     14, 12,  0, 13, 149,   0, 139 },
	{     14, 13, 11, 12, 139,   0, 149 },
	{     10,  0, 13,  0,   0, 140,   0 },
	{     10, 13, 13,  0, 140, 140,   0 },
	{     10,  0,  1,  0,   0, 146,   0 },
	{     10, 13, 11,  0, 140, 147,   0 },
	{     12,  0, 13,  0,   0, 140,   0 },
	{     12, 13, 13,  0, 140, 140,   0 },
	{     12,  0,  1,  0,   0, 146,   0 },
	{     12, 13, 11,  0, 140, 147,   0 },
	{      3, 13, 11, 12, 150,   0,   0 }
	// clang-format on
};

// BUGFIX: This array should contain an additional 0 (207 elements).
const BYTE BSTYPES[] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
	10, 11, 12, 13, 14, 15, 16, 17, 0, 0,
	0, 0, 0, 0, 0, 1, 2, 10, 4, 5,
	6, 7, 8, 9, 10, 11, 12, 14, 5, 14,
	10, 4, 14, 4, 5, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	2, 3, 4, 1, 6, 7, 16, 17, 2, 1,
	1, 2, 2, 1, 1, 2, 2, 2, 2, 2,
	1, 1, 11, 1, 13, 13, 13, 1, 2, 1,
	2, 1, 2, 1, 2, 2, 2, 2, 12, 0,
	0, 11, 1, 11, 1, 13, 0, 0, 0, 0,
	0, 0, 0, 13, 13, 13, 13, 13, 13, 13,
	13, 13, 13, 13, 13, 13, 1, 11, 2, 12,
	13, 13, 13, 12, 2, 1, 2, 2, 4, 14,
	4, 10, 13, 13, 4, 4, 1, 1, 4, 2,
	2, 13, 13, 13, 13, 25, 26, 28, 30, 31,
	41, 43, 40, 41, 42, 43, 25, 41, 43, 28,
	28, 1, 2, 25, 26, 22, 22, 25, 26, 0,
	0, 0, 0, 0, 0, 0, 0
};

// BUGFIX: This array should contain an additional 0 (207 elements).
const BYTE L5BTYPES[] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
	10, 11, 12, 13, 14, 15, 16, 17, 0, 0,
	0, 0, 0, 0, 0, 25, 26, 0, 28, 0,
	30, 31, 0, 0, 0, 0, 0, 0, 0, 0,
	40, 41, 42, 43, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 79,
	80, 0, 82, 0, 0, 0, 0, 0, 0, 79,
	0, 80, 0, 0, 79, 80, 0, 2, 2, 2,
	1, 1, 11, 25, 13, 13, 13, 1, 2, 1,
	2, 1, 2, 1, 2, 2, 2, 2, 12, 0,
	0, 11, 1, 11, 1, 13, 0, 0, 0, 0,
	0, 0, 0, 13, 13, 13, 13, 13, 13, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0
};
const BYTE STAIRSUP[] = { 4, 4, 13, 13, 13, 13, 2, 2, 2, 2, 13, 13, 13, 13, 13, 13, 13, 13, 0, 66, 6, 0, 63, 64, 65, 0, 0, 67, 68, 0, 0, 0, 0, 0 };
const BYTE L5STAIRSUP[] = { 4, 4, 22, 22, 22, 22, 2, 2, 2, 2, 13, 13, 13, 13, 13, 13, 13, 13, 0, 66, 23, 0, 63, 64, 65, 0, 0, 67, 68, 0, 0, 0, 0, 0 };
const BYTE STAIRSDOWN[] = { 4, 3, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 62, 57, 58, 0, 61, 59, 60, 0, 0, 0, 0, 0 };
const BYTE LAMPS[] = { 2, 2, 13, 0, 13, 13, 129, 0, 130, 128 };
const BYTE PWATERIN[] = { 6, 6, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 0, 0, 0, 0, 0, 0, 0, 202, 200, 200, 84, 0, 0, 199, 203, 203, 83, 0, 0, 85, 206, 80, 81, 0, 0, 0, 134, 135, 0, 0, 0, 0, 0, 0, 0, 0 };

/* data */
BYTE L5ConvTbl[16] = { 22, 13, 1, 13, 2, 13, 13, 13, 4, 13, 1, 13, 2, 13, 16, 13 };

static void DRLG_PlaceDoor(int x, int y)
{
	if ((L5dflags[x][y] & DLRG_PROTECTED) == 0) {
		BYTE df = L5dflags[x][y] & 0x7F;
		BYTE c = dgrid[x][y].dungeon;

		if (df == 1) {
			if (y != 1 && c == 2)
				dgrid[x][y].dungeon = 26;
			if (y != 1 && c == 7)
				dgrid[x][y].dungeon = 31;
			if (y != 1 && c == 14)
				dgrid[x][y].dungeon = 42;
			if (y != 1 && c == 4)
				dgrid[x][y].dungeon = 43;
			if (x != 1 && c == 1)
				dgrid[x][y].dungeon = 25;
			if (x != 1 && c == 10)
				dgrid[x][y].dungeon = 40;
			if (x != 1 && c == 6)
				dgrid[x][y].dungeon = 30;
		}
		if (df == 2) {
			if (x != 1 && c == 1)
				dgrid[x][y].dungeon = 25;
			if (x != 1 && c == 6)
				dgrid[x][y].dungeon = 30;
			if (x != 1 && c == 10)
				dgrid[x][y].dungeon = 40;
			if (x != 1 && c == 4)
				dgrid[x][y].dungeon = 41;
			if (y != 1 && c == 2)
				dgrid[x][y].dungeon = 26;
			if (y != 1 && c == 14)
				dgrid[x][y].dungeon = 42;
			if (y != 1 && c == 7)
				dgrid[x][y].dungeon = 31;
		}
		if (df == 3) {
			if (x != 1 && y != 1 && c == 4)
				dgrid[x][y].dungeon = 28;
			if (x != 1 && c == 10)
				dgrid[x][y].dungeon = 40;
			if (y != 1 && c == 14)
				dgrid[x][y].dungeon = 42;
			if (y != 1 && c == 2)
				dgrid[x][y].dungeon = 26;
			if (x != 1 && c == 1)
				dgrid[x][y].dungeon = 25;
			if (y != 1 && c == 7)
				dgrid[x][y].dungeon = 31;
			if (x != 1 && c == 6)
				dgrid[x][y].dungeon = 30;
		}
	}

	L5dflags[x][y] = DLRG_PROTECTED;
}

static void DRLG_L1Shadows()
{
	int x, y, i;
	BYTE sd[2][2];
	BYTE tnv3;
	BOOL patflag;

	for (y = 1; y < DMAXY; y++) {
		for (x = 1; x < DMAXX; x++) {
			sd[0][0] = BSTYPES[dgrid[x][y].dungeon];
			sd[1][0] = BSTYPES[dgrid[x - 1][y].dungeon];
			sd[0][1] = BSTYPES[dgrid[x][y - 1].dungeon];
			sd[1][1] = BSTYPES[dgrid[x - 1][y - 1].dungeon];

			for (i = 0; i < 37; i++) {
				if (SPATS[i].strig == sd[0][0]) {
					patflag = TRUE;
					if (SPATS[i].s1 && SPATS[i].s1 != sd[1][1])
						patflag = FALSE;
					if (SPATS[i].s2 && SPATS[i].s2 != sd[0][1])
						patflag = FALSE;
					if (SPATS[i].s3 && SPATS[i].s3 != sd[1][0])
						patflag = FALSE;
					if (patflag == TRUE) {
						if (SPATS[i].nv1 && !L5dflags[x - 1][y - 1])
							dgrid[x - 1][y - 1].dungeon = SPATS[i].nv1;
						if (SPATS[i].nv2 && !L5dflags[x][y - 1])
							dgrid[x][y - 1].dungeon = SPATS[i].nv2;
						if (SPATS[i].nv3 && !L5dflags[x - 1][y])
							dgrid[x - 1][y].dungeon = SPATS[i].nv3;
					}
				}
			}
		}
	}

	for (y = 1; y < DMAXY; y++) {
		for (x = 1; x < DMAXX; x++) {
			if (dgrid[x - 1][y].dungeon == 139 && !L5dflags[x - 1][y]) {
				tnv3 = 139;
				if (dgrid[x][y].dungeon == 29)
					tnv3 = 141;
				if (dgrid[x][y].dungeon == 32)
					tnv3 = 141;
				if (dgrid[x][y].dungeon == 35)
					tnv3 = 141;
				if (dgrid[x][y].dungeon == 37)
					tnv3 = 141;
				if (dgrid[x][y].dungeon == 38)
					tnv3 = 141;
				if (dgrid[x][y].dungeon == 39)
					tnv3 = 141;
				dgrid[x - 1][y].dungeon = tnv3;
			}
			if (dgrid[x - 1][y].dungeon == 149 && !L5dflags[x - 1][y]) {
				tnv3 = 149;
				if (dgrid[x][y].dungeon == 29)
					tnv3 = 153;
				if (dgrid[x][y].dungeon == 32)
					tnv3 = 153;
				if (dgrid[x][y].dungeon == 35)
					tnv3 = 153;
				if (dgrid[x][y].dungeon == 37)
					tnv3 = 153;
				if (dgrid[x][y].dungeon == 38)
					tnv3 = 153;
				if (dgrid[x][y].dungeon == 39)
					tnv3 = 153;
				dgrid[x - 1][y].dungeon = tnv3;
			}
			if (dgrid[x - 1][y].dungeon == 148 && !L5dflags[x - 1][y]) {
				tnv3 = 148;
				if (dgrid[x][y].dungeon == 29)
					tnv3 = 154;
				if (dgrid[x][y].dungeon == 32)
					tnv3 = 154;
				if (dgrid[x][y].dungeon == 35)
					tnv3 = 154;
				if (dgrid[x][y].dungeon == 37)
					tnv3 = 154;
				if (dgrid[x][y].dungeon == 38)
					tnv3 = 154;
				if (dgrid[x][y].dungeon == 39)
					tnv3 = 154;
				dgrid[x - 1][y].dungeon = tnv3;
			}
		}
	}
}

static int DRLG_PlaceMiniSet(const BYTE *miniset, int tmin, int tmax, int cx, int cy, BOOL setview, int noquad, int ldir)
{
	int sx, sy, sw, sh, xx, yy, i, ii, numt, found, t;
	BOOL abort;

	sw = miniset[0];
	sh = miniset[1];

	if (tmax - tmin == 0)
		numt = 1;
	else
		numt = random_(0, tmax - tmin) + tmin;

	for (i = 0; i < numt; i++) {
		sx = random_(0, DMAXX - sw);
		sy = random_(0, DMAXY - sh);
		abort = FALSE;
		found = 0;

		while (abort == FALSE) {
			abort = TRUE;
			if (cx != -1 && sx >= cx - sw && sx <= cx + 12) {
				sx++;
				abort = FALSE;
			}
			if (cy != -1 && sy >= cy - sh && sy <= cy + 12) {
				sy++;
				abort = FALSE;
			}

			switch (noquad) {
			case 0:
				if (sx < cx && sy < cy)
					abort = FALSE;
				break;
			case 1:
				if (sx > cx && sy < cy)
					abort = FALSE;
				break;
			case 2:
				if (sx < cx && sy > cy)
					abort = FALSE;
				break;
			case 3:
				if (sx > cx && sy > cy)
					abort = FALSE;
				break;
			}

			ii = 2;

			for (yy = 0; yy < sh && abort == TRUE; yy++) {
				for (xx = 0; xx < sw && abort == TRUE; xx++) {
					if (miniset[ii] && dgrid[xx + sx][sy + yy].dungeon != miniset[ii])
						abort = FALSE;
					if (L5dflags[xx + sx][sy + yy])
						abort = FALSE;
					ii++;
				}
			}

			if (abort == FALSE) {
				if (++sx == DMAXX - sw) {
					sx = 0;
					if (++sy == DMAXY - sh)
						sy = 0;
				}
				if (++found > 4000)
					return -1;
			}
		}

		ii = sw * sh + 2;

		for (yy = 0; yy < sh; yy++) {
			for (xx = 0; xx < sw; xx++) {
				if (miniset[ii])
					dgrid[xx + sx][sy + yy].dungeon = miniset[ii];
				ii++;
			}
		}
	}

	if (miniset == PWATERIN) {
		t = TransVal;
		TransVal = 0;
		DRLG_MRectTrans(sx, sy + 2, sx + 5, sy + 4);
		TransVal = t;

		quests[Q_PWATER]._qtx = 2 * sx + 21;
		quests[Q_PWATER]._qty = 2 * sy + 22;
	}

	if (setview == TRUE) {
		ViewX = 2 * sx + 19;
		ViewY = 2 * sy + 20;
	}

	if (ldir == 0) {
		LvlViewX = 2 * sx + 19;
		LvlViewY = 2 * sy + 20;
	}

	if (sx < cx && sy < cy)
		return 0;
	if (sx > cx && sy < cy)
		return 1;
	if (sx < cx && sy > cy)
		return 2;
	else
		return 3;
}

static void DRLG_L1Floor()
{
	int i, j;
	LONG rv;

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (L5dflags[i][j] == 0 && dgrid[i][j].dungeon == 13) {
				rv = random_(0, 3);

				if (rv == 1)
					dgrid[i][j].dungeon = 162;
				if (rv == 2)
					dgrid[i][j].dungeon = 163;
			}
		}
	}
}

static void DRLG_L1Pass3()
{
	int i, j, xx, yy;
	long v1, v2, v3, v4, lv;
	WORD *MegaTiles;

	lv = 22 - 1;

	MegaTiles = (WORD *)&pMegaTiles[lv * 8];
	v1 = SDL_SwapLE16(*(MegaTiles + 0)) + 1;
	v2 = SDL_SwapLE16(*(MegaTiles + 1)) + 1;
	v3 = SDL_SwapLE16(*(MegaTiles + 2)) + 1;
	v4 = SDL_SwapLE16(*(MegaTiles + 3)) + 1;

	for (j = 0; j < MAXDUNY; j += 2)
	{
		for (i = 0; i < MAXDUNX; i += 2) {
			grid[i][j].dPiece = v1;
			grid[i + 1][j].dPiece = v2;
			grid[i][j + 1].dPiece = v3;
			grid[i + 1][j + 1].dPiece = v4;
		}
	}

	yy = 16;
	for (j = 0; j < DMAXY; j++) {
		xx = 16;
		for (i = 0; i < DMAXX; i++) {
			lv = dgrid[i][j].dungeon - 1;
			/// ASSERT: assert(lv >= 0);
			MegaTiles = (WORD *)&pMegaTiles[lv * 8];
			v1 = SDL_SwapLE16(*(MegaTiles + 0)) + 1;
			v2 = SDL_SwapLE16(*(MegaTiles + 1)) + 1;
			v3 = SDL_SwapLE16(*(MegaTiles + 2)) + 1;
			v4 = SDL_SwapLE16(*(MegaTiles + 3)) + 1;
			grid[xx][yy].dPiece = v1;
			grid[xx + 1][yy].dPiece = v2;
			grid[xx][yy + 1].dPiece = v3;
			grid[xx + 1][yy + 1].dPiece = v4;
			xx += 2;
		}
		yy += 2;
	}
}

static void DRLG_LoadL1SP()
{
	L5setloadflag = FALSE;
	if (QuestStatus(Q_BUTCHER)) {
		L5pSetPiece = LoadFileInMem("Levels\\L1Data\\rnd6.DUN", NULL);
		L5setloadflag = TRUE;
	}
	if (QuestStatus(Q_SKELKING) && gbMaxPlayers == 1) {
		L5pSetPiece = LoadFileInMem("Levels\\L1Data\\SKngDO.DUN", NULL);
		L5setloadflag = TRUE;
	}
	if (QuestStatus(Q_LTBANNER)) {
		L5pSetPiece = LoadFileInMem("Levels\\L1Data\\Banner2.DUN", NULL);
		L5setloadflag = TRUE;
	}
}

static void DRLG_FreeL1SP()
{
	MemFreeDbg(L5pSetPiece);
}

void DRLG_Init_Globals()
{
	char c;
	if (!lightflag) {
		if (light4flag)
			c = 3;
		else
			c = 15;
	} else {
		c = 0;
	}
	grid.init(c);
}

static void DRLG_InitL1Vals()
{
	int i, j, pc;

	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++) {
			if (grid[i][j].dPiece == 12) {
				pc = 1;
			} else if (grid[i][j].dPiece == 11) {
				pc = 2;
			} else if (grid[i][j].dPiece == 71) {
				pc = 1;
			} else if (grid[i][j].dPiece == 253) {
				pc = 3;
			} else if (grid[i][j].dPiece == 267) {
				pc = 6;
			} else if (grid[i][j].dPiece == 259) {
				pc = 5;
			} else if (grid[i][j].dPiece == 249) {
				pc = 2;
			} else if (grid[i][j].dPiece == 325) {
				pc = 2;
			} else if (grid[i][j].dPiece == 321) {
				pc = 1;
			} else if (grid[i][j].dPiece == 255) {
				pc = 4;
			} else if (grid[i][j].dPiece == 211) {
				pc = 1;
			} else if (grid[i][j].dPiece == 344) {
				pc = 2;
			} else if (grid[i][j].dPiece == 341) {
				pc = 1;
			} else if (grid[i][j].dPiece == 331) {
				pc = 2;
			} else if (grid[i][j].dPiece == 418) {
				pc = 1;
			} else if (grid[i][j].dPiece == 421) {
				pc = 2;
			} else {
				continue;
			}
			grid[i][j].dSpecial = pc;
		}
	}
}

#ifndef SPAWN
void LoadL1Dungeon(char *sFileName, int vx, int vy)
{
	int i, j, rw, rh;
	BYTE *pLevelMap, *lm;

	dminx = 16;
	dminy = 16;
	dmaxx = 96;
	dmaxy = 96;

	DRLG_InitTrans();
	pLevelMap = LoadFileInMem(sFileName, NULL);

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			dgrid[i][j].dungeon = 22;
			L5dflags[i][j] = 0;
		}
	}

	lm = pLevelMap;
	rw = *lm;
	lm += 2;
	rh = *lm;
	lm += 2;

	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*lm != 0) {
				dgrid[i][j].dungeon = *lm;
				L5dflags[i][j] |= DLRG_PROTECTED;
			} else {
				dgrid[i][j].dungeon = 13;
			}
			lm += 2;
		}
	}

	DRLG_L1Floor();
	ViewX = vx;
	ViewY = vy;
	DRLG_L1Pass3();
	DRLG_Init_Globals();
	DRLG_InitL1Vals();
	SetMapMonsters(pLevelMap, 0, 0);
	SetMapObjects(pLevelMap, 0, 0);
	mem_free_dbg(pLevelMap);
}

void LoadPreL1Dungeon(char *sFileName, int vx, int vy)
{
	int i, j, rw, rh;
	BYTE *pLevelMap, *lm;

	dminx = 16;
	dminy = 16;
	dmaxx = 96;
	dmaxy = 96;

	pLevelMap = LoadFileInMem(sFileName, NULL);

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			dgrid[i][j].dungeon = 22;
			L5dflags[i][j] = 0;
		}
	}

	lm = pLevelMap;
	rw = *lm;
	lm += 2;
	rh = *lm;
	lm += 2;

	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*lm != 0) {
				dgrid[i][j].dungeon = *lm;
				L5dflags[i][j] |= DLRG_PROTECTED;
			} else {
				dgrid[i][j].dungeon = 13;
			}
			lm += 2;
		}
	}

	DRLG_L1Floor();

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			dgrid[i][j].pdungeon = dgrid[i][j].dungeon;
		}
	}

	mem_free_dbg(pLevelMap);
}
#endif

static void InitL5Dungeon()
{
	int i, j;

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			dgrid[i][j].dungeon = 0;
			L5dflags[i][j] = 0;
		}
	}
}

static void L5ClearFlags()
{
	int i, j;

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			L5dflags[i][j] &= 0xBF;
		}
	}
}

static void L5drawRoom(int x, int y, int w, int h)
{
	int i, j;

	for (j = 0; j < h; j++) {
		for (i = 0; i < w; i++) {
			dgrid[x + i][y + j].dungeon = 1;
		}
	}
}

static BOOL L5checkRoom(int x, int y, int width, int height)
{
	int i, j;

	for (j = 0; j < height; j++) {
		for (i = 0; i < width; i++) {
			if (i + x < 0 || i + x >= DMAXX || j + y < 0 || j + y >= DMAXY)
				return FALSE;
			if (dgrid[i + x][j + y].dungeon)
				return FALSE;
		}
	}

	return TRUE;
}

static void L5roomGen(int x, int y, int w, int h, int dir)
{
	int num, dirProb;
	BOOL ran, ran2;
	int width, height, rx, ry, ry2;
	int cw, ch, cx1, cy1, cx2;

	dirProb = random_(0, 4);

	switch (dir == 1 ? dirProb != 0 : dirProb == 0) {
	case FALSE:
		num = 0;
		do {
			cw = (random_(0, 5) + 2) & 0xFFFFFFFE;
			ch = (random_(0, 5) + 2) & 0xFFFFFFFE;
			cy1 = h / 2 + y - ch / 2;
			cx1 = x - cw;
			ran = L5checkRoom(cx1 - 1, cy1 - 1, ch + 2, cw + 1); /// BUGFIX: swap args 3 and 4 ("ch+2" and "cw+1")
			num++;
		} while (ran == FALSE && num < 20);

		if (ran == TRUE)
			L5drawRoom(cx1, cy1, cw, ch);
		cx2 = x + w;
		ran2 = L5checkRoom(cx2, cy1 - 1, cw + 1, ch + 2);
		if (ran2 == TRUE)
			L5drawRoom(cx2, cy1, cw, ch);
		if (ran == TRUE)
			L5roomGen(cx1, cy1, cw, ch, 1);
		if (ran2 == TRUE)
			L5roomGen(cx2, cy1, cw, ch, 1);
		break;
	case TRUE:
		num = 0;
		do {
			width = (random_(0, 5) + 2) & 0xFFFFFFFE;
			height = (random_(0, 5) + 2) & 0xFFFFFFFE;
			rx = w / 2 + x - width / 2;
			ry = y - height;
			ran = L5checkRoom(rx - 1, ry - 1, width + 2, height + 1);
			num++;
		} while (ran == FALSE && num < 20);

		if (ran == TRUE)
			L5drawRoom(rx, ry, width, height);
		ry2 = y + h;
		ran2 = L5checkRoom(rx - 1, ry2, width + 2, height + 1);
		if (ran2 == TRUE)
			L5drawRoom(rx, ry2, width, height);
		if (ran == TRUE)
			L5roomGen(rx, ry, width, height, 0);
		if (ran2 == TRUE)
			L5roomGen(rx, ry2, width, height, 0);
		break;
	}
}

static void L5firstRoom()
{
	int ys, ye, y;
	int xs, xe, x;

	if (random_(0, 2) == 0) {
		ys = 1;
		ye = DMAXY - 1;

		VR1 = random_(0, 2);
		VR2 = random_(0, 2);
		VR3 = random_(0, 2);

		if (VR1 + VR3 <= 1)
			VR2 = 1;
		if (VR1)
			L5drawRoom(15, 1, 10, 10);
		else
			ys = 18;

		if (VR2)
			L5drawRoom(15, 15, 10, 10);
		if (VR3)
			L5drawRoom(15, 29, 10, 10);
		else
			ye = 22;

		for (y = ys; y < ye; y++) {
			dgrid[17][y].dungeon = 1;
			dgrid[18][y].dungeon = 1;
			dgrid[19][y].dungeon = 1;
			dgrid[20][y].dungeon = 1;
			dgrid[21][y].dungeon = 1;
			dgrid[22][y].dungeon = 1;
		}

		if (VR1)
			L5roomGen(15, 1, 10, 10, 0);
		if (VR2)
			L5roomGen(15, 15, 10, 10, 0);
		if (VR3)
			L5roomGen(15, 29, 10, 10, 0);

		HR3 = 0;
		HR2 = 0;
		HR1 = 0;
	} else {
		xs = 1;
		xe = DMAXX - 1;

		HR1 = random_(0, 2);
		HR2 = random_(0, 2);
		HR3 = random_(0, 2);

		if (HR1 + HR3 <= 1)
			HR2 = 1;
		if (HR1)
			L5drawRoom(1, 15, 10, 10);
		else
			xs = 18;

		if (HR2)
			L5drawRoom(15, 15, 10, 10);
		if (HR3)
			L5drawRoom(29, 15, 10, 10);
		else
			xe = 22;

		for (x = xs; x < xe; x++) {
			dgrid[x][17].dungeon = 1;
			dgrid[x][18].dungeon = 1;
			dgrid[x][19].dungeon = 1;
			dgrid[x][20].dungeon = 1;
			dgrid[x][21].dungeon = 1;
			dgrid[x][22].dungeon = 1;
		}

		if (HR1)
			L5roomGen(1, 15, 10, 10, 1);
		if (HR2)
			L5roomGen(15, 15, 10, 10, 1);
		if (HR3)
			L5roomGen(29, 15, 10, 10, 1);

		VR3 = 0;
		VR2 = 0;
		VR1 = 0;
	}
}

static int L5GetArea()
{
	int i, j;
	int rv;

	rv = 0;

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dgrid[i][j].dungeon == 1)
				rv++;
		}
	}

	return rv;
}

static void L5makeDungeon()
{
	int i, j;
	int i_2, j_2;

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			j_2 = j << 1;
			i_2 = i << 1;
			L5dungeon[i_2][j_2] = dgrid[i][j].dungeon;
			L5dungeon[i_2][j_2 + 1] = dgrid[i][j].dungeon;
			L5dungeon[i_2 + 1][j_2] = dgrid[i][j].dungeon;
			L5dungeon[i_2 + 1][j_2 + 1] = dgrid[i][j].dungeon;
		}
	}
}

static void L5makeDmt()
{
	int i, j, idx, val, dmtx, dmty;

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			dgrid[i][j].dungeon = 22;
		}
	}

	for (j = 0, dmty = 1; dmty <= 77; j++, dmty += 2) {
		for (i = 0, dmtx = 1; dmtx <= 77; i++, dmtx += 2) {
			val = 8 * L5dungeon[dmtx + 1][dmty + 1]
			    + 4 * L5dungeon[dmtx][dmty + 1]
			    + 2 * L5dungeon[dmtx + 1][dmty]
			    + L5dungeon[dmtx][dmty];
			idx = L5ConvTbl[val];
			dgrid[i][j].dungeon = idx;
		}
	}
}

static int L5HWallOk(int i, int j)
{
	int x;
	BOOL wallok;

	for (x = 1; dgrid[i + x][j].dungeon == 13; x++) {
		if (dgrid[i + x][j - 1].dungeon != 13 || dgrid[i + x][j + 1].dungeon != 13 || L5dflags[i + x][j])
			break;
	}

	wallok = FALSE;
	if (dgrid[i + x][j].dungeon >= 3 && dgrid[i + x][j].dungeon <= 7)
		wallok = TRUE;
	if (dgrid[i + x][j].dungeon >= 16 && dgrid[i + x][j].dungeon <= 24)
		wallok = TRUE;
	if (dgrid[i + x][j].dungeon == 22)
		wallok = FALSE;
	if (x == 1)
		wallok = FALSE;

	if (wallok)
		return x;
	else
		return -1;
}

static int L5VWallOk(int i, int j)
{
	int y;
	BOOL wallok;

	for (y = 1; dgrid[i][j + y].dungeon == 13; y++) {
		if (dgrid[i - 1][j + y].dungeon != 13 || dgrid[i + 1][j + y].dungeon != 13 || L5dflags[i][j + y])
			break;
	}

	wallok = FALSE;
	if (dgrid[i][j + y].dungeon >= 3 && dgrid[i][j + y].dungeon <= 7)
		wallok = TRUE;
	if (dgrid[i][j + y].dungeon >= 16 && dgrid[i][j + y].dungeon <= 24)
		wallok = TRUE;
	if (dgrid[i][j + y].dungeon == 22)
		wallok = FALSE;
	if (y == 1)
		wallok = FALSE;

	if (wallok)
		return y;
	else
		return -1;
}

static void L5HorizWall(int i, int j, char p, int dx)
{
	int xx;
	char wt, dt;

	switch (random_(0, 4)) {
	case 0:
	case 1:
		dt = 2;
		break;
	case 2:
		dt = 12;
		if (p == 2)
			p = 12;
		if (p == 4)
			p = 10;
		break;
	case 3:
		dt = 36;
		if (p == 2)
			p = 36;
		if (p == 4)
			p = 27;
		break;
	}

	if (random_(0, 6) == 5)
		wt = 12;
	else
		wt = 26;
	if (dt == 12)
		wt = 12;

	dgrid[i][j].dungeon = p;

	for (xx = 1; xx < dx; xx++) {
		dgrid[i + xx][j].dungeon = dt;
	}

	xx = random_(0, dx - 1) + 1;

	if (wt == 12) {
		dgrid[i + xx][j].dungeon = wt;
	} else {
		dgrid[i + xx][j].dungeon = 2;
		L5dflags[i + xx][j] |= DLRG_HDOOR;
	}
}

static void L5VertWall(int i, int j, char p, int dy)
{
	int yy;
	char wt, dt;

	switch (random_(0, 4)) {
	case 0:
	case 1:
		dt = 1;
		break;
	case 2:
		dt = 11;
		if (p == 1)
			p = 11;
		if (p == 4)
			p = 14;
		break;
	case 3:
		dt = 35;
		if (p == 1)
			p = 35;
		if (p == 4)
			p = 37;
		break;
	}

	if (random_(0, 6) == 5)
		wt = 11;
	else
		wt = 25;
	if (dt == 11)
		wt = 11;

	dgrid[i][j].dungeon = p;

	for (yy = 1; yy < dy; yy++) {
		dgrid[i][j + yy].dungeon = dt;
	}

	yy = random_(0, dy - 1) + 1;

	if (wt == 11) {
		dgrid[i][j + yy].dungeon = wt;
	} else {
		dgrid[i][j + yy].dungeon = 1;
		L5dflags[i][j + yy] |= DLRG_VDOOR;
	}
}

static void L5AddWall()
{
	int i, j, x, y;

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (!L5dflags[i][j]) {
				if (dgrid[i][j].dungeon == 3 && random_(0, 100) < 100) {
					x = L5HWallOk(i, j);
					if (x != -1)
						L5HorizWall(i, j, 2, x);
				}
				if (dgrid[i][j].dungeon == 3 && random_(0, 100) < 100) {
					y = L5VWallOk(i, j);
					if (y != -1)
						L5VertWall(i, j, 1, y);
				}
				if (dgrid[i][j].dungeon == 6 && random_(0, 100) < 100) {
					x = L5HWallOk(i, j);
					if (x != -1)
						L5HorizWall(i, j, 4, x);
				}
				if (dgrid[i][j].dungeon == 7 && random_(0, 100) < 100) {
					y = L5VWallOk(i, j);
					if (y != -1)
						L5VertWall(i, j, 4, y);
				}
				if (dgrid[i][j].dungeon == 2 && random_(0, 100) < 100) {
					x = L5HWallOk(i, j);
					if (x != -1)
						L5HorizWall(i, j, 2, x);
				}
				if (dgrid[i][j].dungeon == 1 && random_(0, 100) < 100) {
					y = L5VWallOk(i, j);
					if (y != -1)
						L5VertWall(i, j, 1, y);
				}
			}
		}
	}
}

static void DRLG_L5GChamber(int sx, int sy, BOOL topflag, BOOL bottomflag, BOOL leftflag, BOOL rightflag)
{
	int i, j;

	if (topflag == TRUE) {
		dgrid[sx + 2][sy].dungeon = 12;
		dgrid[sx + 3][sy].dungeon = 12;
		dgrid[sx + 4][sy].dungeon = 3;
		dgrid[sx + 7][sy].dungeon = 9;
		dgrid[sx + 8][sy].dungeon = 12;
		dgrid[sx + 9][sy].dungeon = 2;
	}
	if (bottomflag == TRUE) {
		sy += 11;
		dgrid[sx + 2][sy].dungeon = 10;
		dgrid[sx + 3][sy].dungeon = 12;
		dgrid[sx + 4][sy].dungeon = 8;
		dgrid[sx + 7][sy].dungeon = 5;
		dgrid[sx + 8][sy].dungeon = 12;
		if (dgrid[sx + 9][sy].dungeon != 4) {
			dgrid[sx + 9][sy].dungeon = 21;
		}
		sy -= 11;
	}
	if (leftflag == TRUE) {
		dgrid[sx][sy + 2].dungeon = 11;
		dgrid[sx][sy + 3].dungeon = 11;
		dgrid[sx][sy + 4].dungeon = 3;
		dgrid[sx][sy + 7].dungeon = 8;
		dgrid[sx][sy + 8].dungeon = 11;
		dgrid[sx][sy + 9].dungeon = 1;
	}
	if (rightflag == TRUE) {
		sx += 11;
		dgrid[sx][sy + 2].dungeon = 14;
		dgrid[sx][sy + 3].dungeon = 11;
		dgrid[sx][sy + 4].dungeon = 9;
		dgrid[sx][sy + 7].dungeon = 5;
		dgrid[sx][sy + 8].dungeon = 11;
		if (dgrid[sx][sy + 9].dungeon != 4) {
			dgrid[sx][sy + 9].dungeon = 21;
		}
		sx -= 11;
	}

	for (j = 1; j < 11; j++) {
		for (i = 1; i < 11; i++) {
			dgrid[i + sx][j + sy].dungeon = 13;
			L5dflags[i + sx][j + sy] |= DLRG_CHAMBER;
		}
	}

	dgrid[sx + 4][sy + 4].dungeon = 15;
	dgrid[sx + 7][sy + 4].dungeon = 15;
	dgrid[sx + 4][sy + 7].dungeon = 15;
	dgrid[sx + 7][sy + 7].dungeon = 15;
}

static void DRLG_L5GHall(int x1, int y1, int x2, int y2)
{
	int i;

	if (y1 == y2) {
		for (i = x1; i < x2; i++) {
			dgrid[i][y1].dungeon = 12;
			dgrid[i][y1 + 3].dungeon = 12;
		}
	} else {
		for (i = y1; i < y2; i++) {
			dgrid[x1][i].dungeon = 11;
			dgrid[x1 + 3][i].dungeon = 11;
		}
	}
}

static void L5tileFix()
{
	int i, j;

	// BUGFIX: Bounds checks are required in all loop bodies.
	// See https://github.com/diasurgical/devilutionX/pull/401

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (i + 1 < DMAXX) {
				if (dgrid[i][j].dungeon == 2 && dgrid[i + 1][j].dungeon == 22)
					dgrid[i + 1][j].dungeon = 23;
				if (dgrid[i][j].dungeon == 13 && dgrid[i + 1][j].dungeon == 22)
					dgrid[i + 1][j].dungeon = 18;
				if (dgrid[i][j].dungeon == 13 && dgrid[i + 1][j].dungeon == 2)
					dgrid[i + 1][j].dungeon = 7;
				if (dgrid[i][j].dungeon == 6 && dgrid[i + 1][j].dungeon == 22)
					dgrid[i + 1][j].dungeon = 24;
			}
			if (j + 1 < DMAXY) {
				if (dgrid[i][j].dungeon == 1 && dgrid[i][j + 1].dungeon == 22)
					dgrid[i][j + 1].dungeon = 24;
				if (dgrid[i][j].dungeon == 13 && dgrid[i][j + 1].dungeon == 1)
					dgrid[i][j + 1].dungeon = 6;
				if (dgrid[i][j].dungeon == 13 && dgrid[i][j + 1].dungeon == 22)
					dgrid[i][j + 1].dungeon = 19;
			}
		}
	}

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (i + 1 < DMAXX) {
				if (dgrid[i][j].dungeon == 13 && dgrid[i + 1][j].dungeon == 19)
					dgrid[i + 1][j].dungeon = 21;
				if (dgrid[i][j].dungeon == 13 && dgrid[i + 1][j].dungeon == 22)
					dgrid[i + 1][j].dungeon = 20;
				if (dgrid[i][j].dungeon == 7 && dgrid[i + 1][j].dungeon == 22)
					dgrid[i + 1][j].dungeon = 23;
				if (dgrid[i][j].dungeon == 13 && dgrid[i + 1][j].dungeon == 24)
					dgrid[i + 1][j].dungeon = 21;
				if (dgrid[i][j].dungeon == 19 && dgrid[i + 1][j].dungeon == 22)
					dgrid[i + 1][j].dungeon = 20;
				if (dgrid[i][j].dungeon == 2 && dgrid[i + 1][j].dungeon == 19)
					dgrid[i + 1][j].dungeon = 21;
				if (dgrid[i][j].dungeon == 19 && dgrid[i + 1][j].dungeon == 1)
					dgrid[i + 1][j].dungeon = 6;
				if (dgrid[i][j].dungeon == 7 && dgrid[i + 1][j].dungeon == 19)
					dgrid[i + 1][j].dungeon = 21;
				if (dgrid[i][j].dungeon == 2 && dgrid[i + 1][j].dungeon == 1)
					dgrid[i + 1][j].dungeon = 6;
				if (dgrid[i][j].dungeon == 3 && dgrid[i + 1][j].dungeon == 22)
					dgrid[i + 1][j].dungeon = 24;
				if (dgrid[i][j].dungeon == 21 && dgrid[i + 1][j].dungeon == 1)
					dgrid[i + 1][j].dungeon = 6;
				if (dgrid[i][j].dungeon == 7 && dgrid[i + 1][j].dungeon == 1)
					dgrid[i + 1][j].dungeon = 6;
				if (dgrid[i][j].dungeon == 7 && dgrid[i + 1][j].dungeon == 24)
					dgrid[i + 1][j].dungeon = 21;
				if (dgrid[i][j].dungeon == 4 && dgrid[i + 1][j].dungeon == 16)
					dgrid[i + 1][j].dungeon = 17;
				if (dgrid[i][j].dungeon == 7 && dgrid[i + 1][j].dungeon == 13)
					dgrid[i + 1][j].dungeon = 17;
				if (dgrid[i][j].dungeon == 2 && dgrid[i + 1][j].dungeon == 24)
					dgrid[i + 1][j].dungeon = 21;
				if (dgrid[i][j].dungeon == 2 && dgrid[i + 1][j].dungeon == 13)
					dgrid[i + 1][j].dungeon = 17;
			}
			if (i > 0) {
				if (dgrid[i][j].dungeon == 23 && dgrid[i - 1][j].dungeon == 22)
					dgrid[i - 1][j].dungeon = 19;
				if (dgrid[i][j].dungeon == 19 && dgrid[i - 1][j].dungeon == 23)
					dgrid[i - 1][j].dungeon = 21;
				if (dgrid[i][j].dungeon == 6 && dgrid[i - 1][j].dungeon == 22)
					dgrid[i - 1][j].dungeon = 24;
				if (dgrid[i][j].dungeon == 6 && dgrid[i - 1][j].dungeon == 23)
					dgrid[i - 1][j].dungeon = 21;
			}
			if (j + 1 < DMAXY) {
				if (dgrid[i][j].dungeon == 1 && dgrid[i][j + 1].dungeon == 2)
					dgrid[i][j + 1].dungeon = 7;
				if (dgrid[i][j].dungeon == 6 && dgrid[i][j + 1].dungeon == 18)
					dgrid[i][j + 1].dungeon = 21;
				if (dgrid[i][j].dungeon == 18 && dgrid[i][j + 1].dungeon == 2)
					dgrid[i][j + 1].dungeon = 7;
				if (dgrid[i][j].dungeon == 6 && dgrid[i][j + 1].dungeon == 2)
					dgrid[i][j + 1].dungeon = 7;
				if (dgrid[i][j].dungeon == 21 && dgrid[i][j + 1].dungeon == 2)
					dgrid[i][j + 1].dungeon = 7;
				if (dgrid[i][j].dungeon == 6 && dgrid[i][j + 1].dungeon == 22)
					dgrid[i][j + 1].dungeon = 24;
				if (dgrid[i][j].dungeon == 6 && dgrid[i][j + 1].dungeon == 13)
					dgrid[i][j + 1].dungeon = 16;
				if (dgrid[i][j].dungeon == 1 && dgrid[i][j + 1].dungeon == 13)
					dgrid[i][j + 1].dungeon = 16;
				if (dgrid[i][j].dungeon == 13 && dgrid[i][j + 1].dungeon == 16)
					dgrid[i][j + 1].dungeon = 17;
			}
			if (j > 0) {
				if (dgrid[i][j].dungeon == 6 && dgrid[i][j - 1].dungeon == 22)
					dgrid[i][j - 1].dungeon = 7;
				if (dgrid[i][j].dungeon == 6 && dgrid[i][j - 1].dungeon == 22)
					dgrid[i][j - 1].dungeon = 24;
				if (dgrid[i][j].dungeon == 7 && dgrid[i][j - 1].dungeon == 24)
					dgrid[i][j - 1].dungeon = 21;
				if (dgrid[i][j].dungeon == 18 && dgrid[i][j - 1].dungeon == 24)
					dgrid[i][j - 1].dungeon = 21;
			}
		}
	}

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (j + 1 < DMAXY && dgrid[i][j].dungeon == 4 && dgrid[i][j + 1].dungeon == 2)
				dgrid[i][j + 1].dungeon = 7;
			if (i + 1 < DMAXX && dgrid[i][j].dungeon == 2 && dgrid[i + 1][j].dungeon == 19)
				dgrid[i + 1][j].dungeon = 21;
			if (j + 1 < DMAXY && dgrid[i][j].dungeon == 18 && dgrid[i][j + 1].dungeon == 22)
				dgrid[i][j + 1].dungeon = 20;
		}
	}
}

static void DRLG_L5Subs()
{
	int x, y, rv, i;

	for (y = 0; y < DMAXY; y++) {
		for (x = 0; x < DMAXX; x++) {
			if (!random_(0, 4)) {
				BYTE c = L5BTYPES[dgrid[x][y].dungeon];

				if (c && !L5dflags[x][y]) {
					rv = random_(0, 16);
					i = -1;

					while (rv >= 0) {
						if (++i == sizeof(L5BTYPES))
							i = 0;
						if (c == L5BTYPES[i])
							rv--;
					}

					// BUGFIX: Add `&& y > 0` to the if statement. (fixed)
					if (i == 89 && y > 0) {
						if (L5BTYPES[dgrid[x][y - 1].dungeon] != 79 || L5dflags[x][y - 1])
							i = 79;
						else
							dgrid[x][y - 1].dungeon = 90;
					}
					// BUGFIX: Add `&& x + 1 < DMAXX` to the if statement. (fixed)
					if (i == 91 && x + 1 < DMAXX) {
						if (L5BTYPES[dgrid[x + 1][y].dungeon] != 80 || L5dflags[x + 1][y])
							i = 80;
						else
							dgrid[x + 1][y].dungeon = 92;
					}
					dgrid[x][y].dungeon = i;
				}
			}
		}
	}
}

static void DRLG_L5SetRoom(int rx1, int ry1)
{
	int rw, rh, i, j;
	BYTE *sp;

	rw = *L5pSetPiece;
	rh = *(L5pSetPiece + 2);

	setpc_x = rx1;
	setpc_y = ry1;
	setpc_w = rw;
	setpc_h = rh;

	sp = L5pSetPiece + 4;

	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*sp) {
				dgrid[rx1 + i][ry1 + j].dungeon = *sp;
				L5dflags[rx1 + i][ry1 + j] |= DLRG_PROTECTED;
			} else {
				dgrid[rx1 + i][ry1 + j].dungeon = 13;
			}
			sp += 2;
		}
	}
}

static void L5FillChambers()
{
	int c;

	if (HR1)
		DRLG_L5GChamber(0, 14, 0, 0, 0, 1);

	if (HR2) {
		if (HR1 && !HR3)
			DRLG_L5GChamber(14, 14, 0, 0, 1, 0);
		if (!HR1 && HR3)
			DRLG_L5GChamber(14, 14, 0, 0, 0, 1);
		if (HR1 && HR3)
			DRLG_L5GChamber(14, 14, 0, 0, 1, 1);
		if (!HR1 && !HR3)
			DRLG_L5GChamber(14, 14, 0, 0, 0, 0);
	}

	if (HR3)
		DRLG_L5GChamber(28, 14, 0, 0, 1, 0);
	if (HR1 && HR2)
		DRLG_L5GHall(12, 18, 14, 18);
	if (HR2 && HR3)
		DRLG_L5GHall(26, 18, 28, 18);
	if (HR1 && !HR2 && HR3)
		DRLG_L5GHall(12, 18, 28, 18);
	if (VR1)
		DRLG_L5GChamber(14, 0, 0, 1, 0, 0);

	if (VR2) {
		if (VR1 && !VR3)
			DRLG_L5GChamber(14, 14, 1, 0, 0, 0);
		if (!VR1 && VR3)
			DRLG_L5GChamber(14, 14, 0, 1, 0, 0);
		if (VR1 && VR3)
			DRLG_L5GChamber(14, 14, 1, 1, 0, 0);
		if (!VR1 && !VR3)
			DRLG_L5GChamber(14, 14, 0, 0, 0, 0);
	}

	if (VR3)
		DRLG_L5GChamber(14, 28, 1, 0, 0, 0);
	if (VR1 && VR2)
		DRLG_L5GHall(18, 12, 18, 14);
	if (VR2 && VR3)
		DRLG_L5GHall(18, 26, 18, 28);
	if (VR1 && !VR2 && VR3)
		DRLG_L5GHall(18, 12, 18, 28);

	if (L5setloadflag) {
		if (VR1 || VR2 || VR3) {
			c = 1;
			if (!VR1 && VR2 && VR3 && random_(0, 2))
				c = 2;
			if (VR1 && VR2 && !VR3 && random_(0, 2))
				c = 0;

			if (VR1 && !VR2 && VR3) {
				if (random_(0, 2))
					c = 0;
				else
					c = 2;
			}

			if (VR1 && VR2 && VR3)
				c = random_(0, 3);

			switch (c) {
			case 0:
				DRLG_L5SetRoom(16, 2);
				break;
			case 1:
				DRLG_L5SetRoom(16, 16);
				break;
			case 2:
				DRLG_L5SetRoom(16, 30);
				break;
			}
		} else {
			c = 1;
			if (!HR1 && HR2 && HR3 && random_(0, 2))
				c = 2;
			if (HR1 && HR2 && !HR3 && random_(0, 2))
				c = 0;

			if (HR1 && !HR2 && HR3) {
				if (random_(0, 2))
					c = 0;
				else
					c = 2;
			}

			if (HR1 && HR2 && HR3)
				c = random_(0, 3);

			switch (c) {
			case 0:
				DRLG_L5SetRoom(2, 16);
				break;
			case 1:
				DRLG_L5SetRoom(16, 16);
				break;
			case 2:
				DRLG_L5SetRoom(30, 16);
				break;
			}
		}
	}
}

static void DRLG_L5FTVR(int i, int j, int x, int y, int d)
{
	if (grid[x][y].dTransVal || dgrid[i][j].dungeon != 13) {
		if (d == 1) {
			grid[x][y].dTransVal = TransVal;
			grid[x][y + 1].dTransVal = TransVal;
		}
		if (d == 2) {
			grid[x + 1][y].dTransVal = TransVal;
			grid[x + 1][y + 1].dTransVal = TransVal;
		}
		if (d == 3) {
			grid[x][y].dTransVal = TransVal;
			grid[x + 1][y].dTransVal = TransVal;
		}
		if (d == 4) {
			grid[x][y + 1].dTransVal = TransVal;
			grid[x + 1][y + 1].dTransVal = TransVal;
		}
		if (d == 5)
			grid[x + 1][y + 1].dTransVal = TransVal;
		if (d == 6)
			grid[x][y + 1].dTransVal = TransVal;
		if (d == 7)
			grid[x + 1][y].dTransVal = TransVal;
		if (d == 8)
			grid[x][y].dTransVal = TransVal;
	} else {
		grid[x][y].dTransVal = TransVal;
		grid[x + 1][y].dTransVal = TransVal;
		grid[x][y + 1].dTransVal = TransVal;
		grid[x + 1][y + 1].dTransVal = TransVal;
		DRLG_L5FTVR(i + 1, j, x + 2, y, 1);
		DRLG_L5FTVR(i - 1, j, x - 2, y, 2);
		DRLG_L5FTVR(i, j + 1, x, y + 2, 3);
		DRLG_L5FTVR(i, j - 1, x, y - 2, 4);
		DRLG_L5FTVR(i - 1, j - 1, x - 2, y - 2, 5);
		DRLG_L5FTVR(i + 1, j - 1, x + 2, y - 2, 6);
		DRLG_L5FTVR(i - 1, j + 1, x - 2, y + 2, 7);
		DRLG_L5FTVR(i + 1, j + 1, x + 2, y + 2, 8);
	}
}

static void DRLG_L5FloodTVal()
{
	int xx, yy, i, j;

	yy = 16;

	for (j = 0; j < DMAXY; j++) {
		xx = 16;

		for (i = 0; i < DMAXX; i++) {
			if (dgrid[i][j].dungeon == 13 && !grid[xx][yy].dTransVal) {
				DRLG_L5FTVR(i, j, xx, yy, 0);
				TransVal++;
			}
			xx += 2;
		}
		yy += 2;
	}
}

static void DRLG_L5TransFix()
{
	int xx, yy, i, j;

	yy = 16;

	for (j = 0; j < DMAXY; j++) {
		xx = 16;

		for (i = 0; i < DMAXX; i++) {
			// BUGFIX: Should check for `j > 0` first. (fixed)
			if (dgrid[i][j].dungeon == 23 && j > 0 && dgrid[i][j - 1].dungeon == 18) {
				grid[xx + 1][yy].dTransVal = grid[xx][yy].dTransVal;
				grid[xx + 1][yy + 1].dTransVal = grid[xx][yy].dTransVal;
			}
			// BUGFIX: Should check for `i + 1 < DMAXY` first. (fixed)
			if (dgrid[i][j].dungeon == 24 && i + 1 < DMAXY && dgrid[i + 1][j].dungeon == 19) {
				grid[xx][yy + 1].dTransVal = grid[xx][yy].dTransVal;
				grid[xx + 1][yy + 1].dTransVal = grid[xx][yy].dTransVal;
			}
			if (dgrid[i][j].dungeon == 18) {
				grid[xx + 1][yy].dTransVal = grid[xx][yy].dTransVal;
				grid[xx + 1][yy + 1].dTransVal = grid[xx][yy].dTransVal;
			}
			if (dgrid[i][j].dungeon == 19) {
				grid[xx][yy + 1].dTransVal = grid[xx][yy].dTransVal;
				grid[xx + 1][yy + 1].dTransVal = grid[xx][yy].dTransVal;
			}
			if (dgrid[i][j].dungeon == 20) {
				grid[xx + 1][yy].dTransVal = grid[xx][yy].dTransVal;
				grid[xx][yy + 1].dTransVal = grid[xx][yy].dTransVal;
				grid[xx + 1][yy + 1].dTransVal = grid[xx][yy].dTransVal;
			}
			xx += 2;
		}
		yy += 2;
	}
}

static void DRLG_L5DirtFix()
{
	int i, j;

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dgrid[i][j].dungeon == 21 && dgrid[i + 1][j].dungeon != 19)
				dgrid[i][j].dungeon = 202;
			if (dgrid[i][j].dungeon == 19 && dgrid[i + 1][j].dungeon != 19)
				dgrid[i][j].dungeon = 200;
			if (dgrid[i][j].dungeon == 24 && dgrid[i + 1][j].dungeon != 19)
				dgrid[i][j].dungeon = 205;
			if (dgrid[i][j].dungeon == 18 && dgrid[i][j + 1].dungeon != 18)
				dgrid[i][j].dungeon = 199;
			if (dgrid[i][j].dungeon == 21 && dgrid[i][j + 1].dungeon != 18)
				dgrid[i][j].dungeon = 202;
			if (dgrid[i][j].dungeon == 23 && dgrid[i][j + 1].dungeon != 18)
				dgrid[i][j].dungeon = 204;
		}
	}
}

static void DRLG_L5CornerFix()
{
	int i, j;

	for (j = 1; j < DMAXY - 1; j++) {
		for (i = 1; i < DMAXX - 1; i++) {
			if (!(L5dflags[i][j] & DLRG_PROTECTED) && dgrid[i][j].dungeon == 17 && dgrid[i - 1][j].dungeon == 13 && dgrid[i][j - 1].dungeon == 1) {
				dgrid[i][j].dungeon = 16;
				L5dflags[i][j - 1] &= DLRG_PROTECTED;
			}
			if (dgrid[i][j].dungeon == 202 && dgrid[i + 1][j].dungeon == 13 && dgrid[i][j + 1].dungeon == 1) {
				dgrid[i][j].dungeon = 8;
			}
		}
	}
}

static void DRLG_L5(int entry)
{
	int i, j;
	LONG minarea;
	BOOL doneflag;

	switch (level.currlevel) {
	case 1:
		minarea = 533;
		break;
	case 2:
		minarea = 693;
		break;
	case 3:
	case 4:
		minarea = 761;
		break;
	}

	do {
		DRLG_InitTrans();

		do {
			InitL5Dungeon();
			L5firstRoom();
		} while (L5GetArea() < minarea);

		L5makeDungeon();
		L5makeDmt();
		L5FillChambers();
		L5tileFix();
		L5AddWall();
		L5ClearFlags();
		DRLG_L5FloodTVal();

		doneflag = TRUE;

		if (QuestStatus(Q_PWATER)) {
			if (entry == 0) {
				if (DRLG_PlaceMiniSet(PWATERIN, 1, 1, 0, 0, 1, -1, 0) < 0)
					doneflag = FALSE;
			} else {
				if (DRLG_PlaceMiniSet(PWATERIN, 1, 1, 0, 0, 0, -1, 0) < 0)
					doneflag = FALSE;
				ViewY--;
			}
		}
		if (QuestStatus(Q_LTBANNER)) {
			if (entry == 0) {
				if (DRLG_PlaceMiniSet(STAIRSUP, 1, 1, 0, 0, 1, -1, 0) < 0)
					doneflag = FALSE;
			} else {
				if (DRLG_PlaceMiniSet(STAIRSUP, 1, 1, 0, 0, 0, -1, 0) < 0)
					doneflag = FALSE;
				if (entry == 1) {
					ViewX = 2 * setpc_x + 20;
					ViewY = 2 * setpc_y + 28;
				} else {
					ViewY--;
				}
			}
		} else if (entry == 0) {
			if (DRLG_PlaceMiniSet(L5STAIRSUP, 1, 1, 0, 0, 1, -1, 0) < 0)
				doneflag = FALSE;
			else if (DRLG_PlaceMiniSet(STAIRSDOWN, 1, 1, 0, 0, 0, -1, 1) < 0)
				doneflag = FALSE;
		} else {
			if (DRLG_PlaceMiniSet(L5STAIRSUP, 1, 1, 0, 0, 0, -1, 0) < 0)
				doneflag = FALSE;
			else if (DRLG_PlaceMiniSet(STAIRSDOWN, 1, 1, 0, 0, 1, -1, 1) < 0)
				doneflag = FALSE;
			ViewY--;
		}
	} while (doneflag == FALSE);

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dgrid[i][j].dungeon == 64) {
				int xx = 2 * i + 16; /* todo: fix loop */
				int yy = 2 * j + 16;
				DRLG_CopyTrans(xx, yy + 1, xx, yy);
				DRLG_CopyTrans(xx + 1, yy + 1, xx + 1, yy);
			}
		}
	}

	DRLG_L5TransFix();
	DRLG_L5DirtFix();
	DRLG_L5CornerFix();

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (L5dflags[i][j] & 0x7F)
				DRLG_PlaceDoor(i, j);
		}
	}

	DRLG_L5Subs();
	DRLG_L1Shadows();
	DRLG_PlaceMiniSet(LAMPS, 5, 10, 0, 0, 0, -1, 4);
	DRLG_L1Floor();

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			dgrid[i][j].pdungeon = dgrid[i][j].dungeon;
		}
	}

	DRLG_Init_Globals();
	DRLG_CheckQuests(setpc_x, setpc_y);
}

void CreateL5Dungeon(DWORD rseed, int entry)
{
	SetRndSeed(rseed);

	dminx = 16;
	dminy = 16;
	dmaxx = 96;
	dmaxy = 96;

	DRLG_InitTrans();
	DRLG_InitSetPC();
	DRLG_LoadL1SP();
	DRLG_L5(entry);
	DRLG_L1Pass3();
	DRLG_FreeL1SP();
	DRLG_InitL1Vals();
	DRLG_SetPC();
}

DEVILUTION_END_NAMESPACE
