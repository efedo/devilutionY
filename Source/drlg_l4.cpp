/**
 * @file drlg_l4.cpp
 *
 * Implementation of the hell level generation algorithms.
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

int diabquad1x;
int diabquad1y;
int diabquad2x;
int diabquad2y;
int diabquad3x;
int diabquad3y;
int diabquad4x;
int diabquad4y;
#ifndef SPAWN
BOOL hallok[20];
int l4holdx;
int l4holdy;
int SP4x1;
int SP4y1;
int SP4x2;
int SP4y2;
BYTE L4dungeon[80][80];
BYTE dung[20][20];
//int dword_52A4DC;

const BYTE L4ConvTbl[16] = { 30, 6, 1, 6, 2, 6, 6, 6, 9, 6, 1, 6, 2, 6, 3, 6 };
const BYTE L4USTAIRS[42] = {
	4,
	5,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	0,
	0,
	0,
	0,
	36,
	38,
	35,
	0,
	37,
	34,
	33,
	32,
	0,
	0,
	31,
	0,
	0,
	0,
	0,
	0
};
const BYTE L4TWARP[42] = {
	4,
	5,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	0,
	0,
	0,
	0,
	134,
	136,
	133,
	0,
	135,
	132,
	131,
	130,
	0,
	0,
	129,
	0,
	0,
	0,
	0,
	0
};
const BYTE L4DSTAIRS[52] = {
	5,
	5,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	45,
	41,
	0,
	0,
	44,
	43,
	40,
	0,
	0,
	46,
	42,
	39,
	0,
	0,
	0,
	0,
	0,
	0
};
const BYTE L4PENTA[52] = {
	5,
	5,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	0,
	0,
	0,
	0,
	0,
	0,
	98,
	100,
	103,
	0,
	0,
	99,
	102,
	105,
	0,
	0,
	101,
	104,
	106,
	0,
	0,
	0,
	0,
	0,
	0
};
const BYTE L4PENTA2[52] = {
	5,
	5,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	0,
	0,
	0,
	0,
	0,
	0,
	107,
	109,
	112,
	0,
	0,
	108,
	111,
	114,
	0,
	0,
	110,
	113,
	115,
	0,
	0,
	0,
	0,
	0,
	0
};
const BYTE L4BTYPES[140] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
	10, 11, 12, 13, 14, 15, 16, 17, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
	6, 6, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 1, 2, 1, 2, 1, 2, 1, 1, 2,
	2, 0, 0, 0, 0, 0, 0, 15, 16, 9,
	12, 4, 5, 7, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static void DRLG_L4Shadows()
{
	int x, y;
	BOOL okflag;

	for (y = 1; y < DMAXY; y++) {
		for (x = 1; x < DMAXY; x++) {
			okflag = FALSE;
			if (dgrid[x][y].dungeon == 3) {
				okflag = TRUE;
			}
			if (dgrid[x][y].dungeon == 4) {
				okflag = TRUE;
			}
			if (dgrid[x][y].dungeon == 8) {
				okflag = TRUE;
			}
			if (dgrid[x][y].dungeon == 15) {
				okflag = TRUE;
			}
			if (!okflag) {
				continue;
			}
			if (dgrid[x - 1][y].dungeon == 6) {
				dgrid[x - 1][y].dungeon = 47;
			}
			if (dgrid[x - 1][y - 1].dungeon == 6) {
				dgrid[x - 1][y - 1].dungeon = 48;
			}
		}
	}
}

static void InitL4Dungeon()
{
	int i, j;

	memset(dung, 0, sizeof(dung));
	memset(L4dungeon, 0, sizeof(L4dungeon));

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			dgrid[i][j].dungeon = 30;
			dgrid[i][j].dflags = 0;
		}
	}
}

void DRLG_LoadL4SP()
{
	setloadflag = FALSE;
	if (QuestStatus(Q_WARLORD)) {
		pSetPiece = LoadFileInMem("Levels\\L4Data\\Warlord.DUN", NULL);
		setloadflag = TRUE;
	}
	if (level.currlevel == 15 && gbMaxPlayers != 1) {
		pSetPiece = LoadFileInMem("Levels\\L4Data\\Vile1.DUN", NULL);
		setloadflag = TRUE;
	}
}

void DRLG_FreeL4SP()
{
	MemFreeDbg(pSetPiece);
}

void DRLG_L4SetSPRoom(int rx1, int ry1)
{
	int rw, rh, i, j;
	BYTE *sp;

	rw = pSetPiece[0];
	rh = pSetPiece[2];

	setpc_x = rx1;
	setpc_y = ry1;
	setpc_w = rw;
	setpc_h = rh;

	sp = &pSetPiece[4];

	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*sp != 0) {
				dgrid[i + rx1][j + ry1].dungeon = *sp;
				dgrid[i + rx1][j + ry1].dflags |= DLRG_PROTECTED;
			} else {
				dgrid[i + rx1][j + ry1].dungeon = 6;
			}
			sp += 2;
		}
	}
}

static void L4makeDmt()
{
	int i, j, idx, val, dmtx, dmty;

	for (j = 0, dmty = 1; dmty <= 77; j++, dmty += 2) {
		for (i = 0, dmtx = 1; dmtx <= 77; i++, dmtx += 2) {
			val = 8 * L4dungeon[dmtx + 1][dmty + 1]
			    + 4 * L4dungeon[dmtx][dmty + 1]
			    + 2 * L4dungeon[dmtx + 1][dmty]
			    + L4dungeon[dmtx][dmty];
			idx = L4ConvTbl[val];
			dgrid[i][j].dungeon = idx;
		}
	}
}

static int L4HWallOk(int i, int j)
{
	int x;
	BOOL wallok;

	for (x = 1; dgrid[i + x][j].dungeon == 6; x++) {
		if (dgrid[i + x][j].dflags != 0) {
			break;
		}
		if (dgrid[i + x][j - 1].dungeon != 6) {
			break;
		}
		if (dgrid[i + x][j + 1].dungeon != 6) {
			break;
		}
	}

	wallok = FALSE;

	if (dgrid[i + x][j].dungeon == 10) {
		wallok = TRUE;
	}
	if (dgrid[i + x][j].dungeon == 12) {
		wallok = TRUE;
	}
	if (dgrid[i + x][j].dungeon == 13) {
		wallok = TRUE;
	}
	if (dgrid[i + x][j].dungeon == 15) {
		wallok = TRUE;
	}
	if (dgrid[i + x][j].dungeon == 16) {
		wallok = TRUE;
	}
	if (dgrid[i + x][j].dungeon == 21) {
		wallok = TRUE;
	}
	if (dgrid[i + x][j].dungeon == 22) {
		wallok = TRUE;
	}
	if (x <= 3) {
		wallok = FALSE;
	}

	if (wallok) {
		return x;
	} else {
		return -1;
	}
}

static int L4VWallOk(int i, int j)
{
	int y;
	BOOL wallok;

	for (y = 1; dgrid[i][j + y].dungeon == 6; y++) {
		if (dgrid[i][j + y].dflags != 0) {
			break;
		}
		if (dgrid[i - 1][j + y].dungeon != 6) {
			break;
		}
		if (dgrid[i + 1][j + y].dungeon != 6) {
			break;
		}
	}

	wallok = FALSE;

	if (dgrid[i][j + y].dungeon == 8) {
		wallok = TRUE;
	}
	if (dgrid[i][j + y].dungeon == 9) {
		wallok = TRUE;
	}
	if (dgrid[i][j + y].dungeon == 11) {
		wallok = TRUE;
	}
	if (dgrid[i][j + y].dungeon == 14) {
		wallok = TRUE;
	}
	if (dgrid[i][j + y].dungeon == 15) {
		wallok = TRUE;
	}
	if (dgrid[i][j + y].dungeon == 16) {
		wallok = TRUE;
	}
	if (dgrid[i][j + y].dungeon == 21) {
		wallok = TRUE;
	}
	if (dgrid[i][j + y].dungeon == 23) {
		wallok = TRUE;
	}
	if (y <= 3) {
		wallok = FALSE;
	}

	if (wallok) {
		return y;
	} else {
		return -1;
	}
}

static void L4HorizWall(int i, int j, int dx)
{
	int xx;

	if (dgrid[i][j].dungeon == 13) {
		dgrid[i][j].dungeon = 17;
	}
	if (dgrid[i][j].dungeon == 16) {
		dgrid[i][j].dungeon = 11;
	}
	if (dgrid[i][j].dungeon == 12) {
		dgrid[i][j].dungeon = 14;
	}

	for (xx = 1; xx < dx; xx++) {
		dgrid[i + xx][j].dungeon = 2;
	}

	if (dgrid[i + dx][j].dungeon == 15) {
		dgrid[i + dx][j].dungeon = 14;
	}
	if (dgrid[i + dx][j].dungeon == 10) {
		dgrid[i + dx][j].dungeon = 17;
	}
	if (dgrid[i + dx][j].dungeon == 21) {
		dgrid[i + dx][j].dungeon = 23;
	}
	if (dgrid[i + dx][j].dungeon == 22) {
		dgrid[i + dx][j].dungeon = 29;
	}

	xx = random_(0, dx - 3) + 1;
	dgrid[i + xx][j].dungeon = 57;
	dgrid[i + xx + 2][j].dungeon = 56;
	dgrid[i + xx + 1][j].dungeon = 60;

	if (dgrid[i + xx][j - 1].dungeon == 6) {
		dgrid[i + xx][j - 1].dungeon = 58;
	}
	if (dgrid[i + xx + 1][j - 1].dungeon == 6) {
		dgrid[i + xx + 1][j - 1].dungeon = 59;
	}
}

static void L4VertWall(int i, int j, int dy)
{
	int yy;

	if (dgrid[i][j].dungeon == 14) {
		dgrid[i][j].dungeon = 17;
	}
	if (dgrid[i][j].dungeon == 8) {
		dgrid[i][j].dungeon = 9;
	}
	if (dgrid[i][j].dungeon == 15) {
		dgrid[i][j].dungeon = 10;
	}

	for (yy = 1; yy < dy; yy++) {
		dgrid[i][j + yy].dungeon = 1;
	}

	if (dgrid[i][j + dy].dungeon == 11) {
		dgrid[i][j + dy].dungeon = 17;
	}
	if (dgrid[i][j + dy].dungeon == 9) {
		dgrid[i][j + dy].dungeon = 10;
	}
	if (dgrid[i][j + dy].dungeon == 16) {
		dgrid[i][j + dy].dungeon = 13;
	}
	if (dgrid[i][j + dy].dungeon == 21) {
		dgrid[i][j + dy].dungeon = 22;
	}
	if (dgrid[i][j + dy].dungeon == 23) {
		dgrid[i][j + dy].dungeon = 29;
	}

	yy = random_(0, dy - 3) + 1;
	dgrid[i][j + yy].dungeon = 53;
	dgrid[i][j + yy + 2].dungeon = 52;
	dgrid[i][j + yy + 1].dungeon = 6;

	if (dgrid[i - 1][j + yy].dungeon == 6) {
		dgrid[i - 1][j + yy].dungeon = 54;
	}
	if (dgrid[i - 1][j + yy - 1].dungeon == 6) {
		dgrid[i - 1][j + yy - 1].dungeon = 55;
	}
}

static void L4AddWall()
{
	int i, j, x, y;

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dgrid[i][j].dflags != 0) {
				continue;
			}
			if (dgrid[i][j].dungeon == 10 && random_(0, 100) < 100) {
				x = L4HWallOk(i, j);
				if (x != -1) {
					L4HorizWall(i, j, x);
				}
			}
			if (dgrid[i][j].dungeon == 12 && random_(0, 100) < 100) {
				x = L4HWallOk(i, j);
				if (x != -1) {
					L4HorizWall(i, j, x);
				}
			}
			if (dgrid[i][j].dungeon == 13 && random_(0, 100) < 100) {
				x = L4HWallOk(i, j);
				if (x != -1) {
					L4HorizWall(i, j, x);
				}
			}
			if (dgrid[i][j].dungeon == 15 && random_(0, 100) < 100) {
				x = L4HWallOk(i, j);
				if (x != -1) {
					L4HorizWall(i, j, x);
				}
			}
			if (dgrid[i][j].dungeon == 16 && random_(0, 100) < 100) {
				x = L4HWallOk(i, j);
				if (x != -1) {
					L4HorizWall(i, j, x);
				}
			}
			if (dgrid[i][j].dungeon == 21 && random_(0, 100) < 100) {
				x = L4HWallOk(i, j);
				if (x != -1) {
					L4HorizWall(i, j, x);
				}
			}
			if (dgrid[i][j].dungeon == 22 && random_(0, 100) < 100) {
				x = L4HWallOk(i, j);
				if (x != -1) {
					L4HorizWall(i, j, x);
				}
			}
			if (dgrid[i][j].dungeon == 8 && random_(0, 100) < 100) {
				y = L4VWallOk(i, j);
				if (y != -1) {
					L4VertWall(i, j, y);
				}
			}
			if (dgrid[i][j].dungeon == 9 && random_(0, 100) < 100) {
				y = L4VWallOk(i, j);
				if (y != -1) {
					L4VertWall(i, j, y);
				}
			}
			if (dgrid[i][j].dungeon == 11 && random_(0, 100) < 100) {
				y = L4VWallOk(i, j);
				if (y != -1) {
					L4VertWall(i, j, y);
				}
			}
			if (dgrid[i][j].dungeon == 14 && random_(0, 100) < 100) {
				y = L4VWallOk(i, j);
				if (y != -1) {
					L4VertWall(i, j, y);
				}
			}
			if (dgrid[i][j].dungeon == 15 && random_(0, 100) < 100) {
				y = L4VWallOk(i, j);
				if (y != -1) {
					L4VertWall(i, j, y);
				}
			}
			if (dgrid[i][j].dungeon == 16 && random_(0, 100) < 100) {
				y = L4VWallOk(i, j);
				if (y != -1) {
					L4VertWall(i, j, y);
				}
			}
			if (dgrid[i][j].dungeon == 21 && random_(0, 100) < 100) {
				y = L4VWallOk(i, j);
				if (y != -1) {
					L4VertWall(i, j, y);
				}
			}
			if (dgrid[i][j].dungeon == 23 && random_(0, 100) < 100) {
				y = L4VWallOk(i, j);
				if (y != -1) {
					L4VertWall(i, j, y);
				}
			}
		}
	}
}

static void L4tileFix()
{
	int i, j;

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dgrid[i][j].dungeon == 2 && dgrid[i + 1][j].dungeon == 6)
				dgrid[i + 1][j].dungeon = 5;
			if (dgrid[i][j].dungeon == 2 && dgrid[i + 1][j].dungeon == 1)
				dgrid[i + 1][j].dungeon = 13;
			if (dgrid[i][j].dungeon == 1 && dgrid[i][j + 1].dungeon == 2)
				dgrid[i][j + 1].dungeon = 14;
		}
	}
	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dgrid[i][j].dungeon == 2 && dgrid[i + 1][j].dungeon == 6)
				dgrid[i + 1][j].dungeon = 2;
			if (dgrid[i][j].dungeon == 2 && dgrid[i + 1][j].dungeon == 9)
				dgrid[i + 1][j].dungeon = 11;
			if (dgrid[i][j].dungeon == 9 && dgrid[i + 1][j].dungeon == 6)
				dgrid[i + 1][j].dungeon = 12;
			if (dgrid[i][j].dungeon == 14 && dgrid[i + 1][j].dungeon == 1)
				dgrid[i + 1][j].dungeon = 13;
			if (dgrid[i][j].dungeon == 6 && dgrid[i + 1][j].dungeon == 14)
				dgrid[i + 1][j].dungeon = 15;
			if (dgrid[i][j].dungeon == 6 && dgrid[i][j + 1].dungeon == 13)
				dgrid[i][j + 1].dungeon = 16;
			if (dgrid[i][j].dungeon == 1 && dgrid[i][j + 1].dungeon == 9)
				dgrid[i][j + 1].dungeon = 10;
			if (dgrid[i][j].dungeon == 6 && dgrid[i][j - 1].dungeon == 1)
				dgrid[i][j - 1].dungeon = 1;
		}
	}
	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dgrid[i][j].dungeon == 13 && dgrid[i][j + 1].dungeon == 30)
				dgrid[i][j + 1].dungeon = 27;
			if (dgrid[i][j].dungeon == 27 && dgrid[i + 1][j].dungeon == 30)
				dgrid[i + 1][j].dungeon = 19;
			if (dgrid[i][j].dungeon == 1 && dgrid[i][j + 1].dungeon == 30)
				dgrid[i][j + 1].dungeon = 27;
			if (dgrid[i][j].dungeon == 27 && dgrid[i + 1][j].dungeon == 1)
				dgrid[i + 1][j].dungeon = 16;
			if (dgrid[i][j].dungeon == 19 && dgrid[i + 1][j].dungeon == 27)
				dgrid[i + 1][j].dungeon = 26;
			if (dgrid[i][j].dungeon == 27 && dgrid[i + 1][j].dungeon == 30)
				dgrid[i + 1][j].dungeon = 19;
			if (dgrid[i][j].dungeon == 2 && dgrid[i + 1][j].dungeon == 15)
				dgrid[i + 1][j].dungeon = 14;
			if (dgrid[i][j].dungeon == 14 && dgrid[i + 1][j].dungeon == 15)
				dgrid[i + 1][j].dungeon = 14;
			if (dgrid[i][j].dungeon == 22 && dgrid[i + 1][j].dungeon == 1)
				dgrid[i + 1][j].dungeon = 16;
			if (dgrid[i][j].dungeon == 27 && dgrid[i + 1][j].dungeon == 1)
				dgrid[i + 1][j].dungeon = 16;
			if (dgrid[i][j].dungeon == 6 && dgrid[i + 1][j].dungeon == 27 && dgrid[i + 1][j + 1].dungeon != 0) /* check */
				dgrid[i + 1][j].dungeon = 22;
			if (dgrid[i][j].dungeon == 22 && dgrid[i + 1][j].dungeon == 30)
				dgrid[i + 1][j].dungeon = 19;
			if (dgrid[i][j].dungeon == 21 && dgrid[i + 1][j].dungeon == 1 && dgrid[i + 1][j - 1].dungeon == 1)
				dgrid[i + 1][j].dungeon = 13;
			if (dgrid[i][j].dungeon == 14 && dgrid[i + 1][j].dungeon == 30 && dgrid[i][j + 1].dungeon == 6)
				dgrid[i + 1][j].dungeon = 28;
			if (dgrid[i][j].dungeon == 16 && dgrid[i + 1][j].dungeon == 6 && dgrid[i][j + 1].dungeon == 30)
				dgrid[i][j + 1].dungeon = 27;
			if (dgrid[i][j].dungeon == 16 && dgrid[i][j + 1].dungeon == 30 && dgrid[i + 1][j + 1].dungeon == 30)
				dgrid[i][j + 1].dungeon = 27;
			if (dgrid[i][j].dungeon == 6 && dgrid[i + 1][j].dungeon == 30 && dgrid[i + 1][j - 1].dungeon == 6)
				dgrid[i + 1][j].dungeon = 21;
			if (dgrid[i][j].dungeon == 2 && dgrid[i + 1][j].dungeon == 27 && dgrid[i + 1][j + 1].dungeon == 9)
				dgrid[i + 1][j].dungeon = 29;
			if (dgrid[i][j].dungeon == 9 && dgrid[i + 1][j].dungeon == 15)
				dgrid[i + 1][j].dungeon = 14;
			if (dgrid[i][j].dungeon == 15 && dgrid[i + 1][j].dungeon == 27 && dgrid[i + 1][j + 1].dungeon == 2)
				dgrid[i + 1][j].dungeon = 29;
			if (dgrid[i][j].dungeon == 19 && dgrid[i + 1][j].dungeon == 18)
				dgrid[i + 1][j].dungeon = 24;
			if (dgrid[i][j].dungeon == 9 && dgrid[i + 1][j].dungeon == 15)
				dgrid[i + 1][j].dungeon = 14;
			if (dgrid[i][j].dungeon == 19 && dgrid[i + 1][j].dungeon == 19 && dgrid[i + 1][j - 1].dungeon == 30)
				dgrid[i + 1][j].dungeon = 24;
			if (dgrid[i][j].dungeon == 24 && dgrid[i][j - 1].dungeon == 30 && dgrid[i][j - 2].dungeon == 6)
				dgrid[i][j - 1].dungeon = 21;
			if (dgrid[i][j].dungeon == 2 && dgrid[i + 1][j].dungeon == 30)
				dgrid[i + 1][j].dungeon = 28;
			if (dgrid[i][j].dungeon == 15 && dgrid[i + 1][j].dungeon == 30)
				dgrid[i + 1][j].dungeon = 28;
			if (dgrid[i][j].dungeon == 28 && dgrid[i][j + 1].dungeon == 30)
				dgrid[i][j + 1].dungeon = 18;
			if (dgrid[i][j].dungeon == 28 && dgrid[i][j + 1].dungeon == 2)
				dgrid[i][j + 1].dungeon = 15;
			if (dgrid[i][j].dungeon == 19 && dgrid[i + 2][j].dungeon == 2 && dgrid[i + 1][j - 1].dungeon == 18 && dgrid[i + 1][j + 1].dungeon == 1)
				dgrid[i + 1][j].dungeon = 17;
			if (dgrid[i][j].dungeon == 19 && dgrid[i + 2][j].dungeon == 2 && dgrid[i + 1][j - 1].dungeon == 22 && dgrid[i + 1][j + 1].dungeon == 1)
				dgrid[i + 1][j].dungeon = 17;
			if (dgrid[i][j].dungeon == 19 && dgrid[i + 2][j].dungeon == 2 && dgrid[i + 1][j - 1].dungeon == 18 && dgrid[i + 1][j + 1].dungeon == 13)
				dgrid[i + 1][j].dungeon = 17;
			if (dgrid[i][j].dungeon == 21 && dgrid[i + 2][j].dungeon == 2 && dgrid[i + 1][j - 1].dungeon == 18 && dgrid[i + 1][j + 1].dungeon == 1)
				dgrid[i + 1][j].dungeon = 17;
			if (dgrid[i][j].dungeon == 21 && dgrid[i + 1][j + 1].dungeon == 1 && dgrid[i + 1][j - 1].dungeon == 22 && dgrid[i + 2][j].dungeon == 3)
				dgrid[i + 1][j].dungeon = 17;
			if (dgrid[i][j].dungeon == 15 && dgrid[i + 1][j].dungeon == 28 && dgrid[i + 2][j].dungeon == 30 && dgrid[i + 1][j - 1].dungeon == 6)
				dgrid[i + 1][j].dungeon = 23;
			if (dgrid[i][j].dungeon == 14 && dgrid[i + 1][j].dungeon == 28 && dgrid[i + 2][j].dungeon == 1)
				dgrid[i + 1][j].dungeon = 23;
			if (dgrid[i][j].dungeon == 15 && dgrid[i + 1][j].dungeon == 27 && dgrid[i + 1][j + 1].dungeon == 30)
				dgrid[i + 1][j].dungeon = 29;
			if (dgrid[i][j].dungeon == 28 && dgrid[i][j + 1].dungeon == 9)
				dgrid[i][j + 1].dungeon = 15;
			if (dgrid[i][j].dungeon == 21 && dgrid[i + 1][j - 1].dungeon == 21)
				dgrid[i + 1][j].dungeon = 24;
			if (dgrid[i][j].dungeon == 2 && dgrid[i + 1][j].dungeon == 27 && dgrid[i + 1][j + 1].dungeon == 30)
				dgrid[i + 1][j].dungeon = 29;
			if (dgrid[i][j].dungeon == 2 && dgrid[i + 1][j].dungeon == 18)
				dgrid[i + 1][j].dungeon = 25;
			if (dgrid[i][j].dungeon == 21 && dgrid[i + 1][j].dungeon == 9 && dgrid[i + 2][j].dungeon == 2)
				dgrid[i + 1][j].dungeon = 11;
			if (dgrid[i][j].dungeon == 19 && dgrid[i + 1][j].dungeon == 10)
				dgrid[i + 1][j].dungeon = 17;
			if (dgrid[i][j].dungeon == 15 && dgrid[i][j + 1].dungeon == 3)
				dgrid[i][j + 1].dungeon = 4;
			if (dgrid[i][j].dungeon == 22 && dgrid[i][j + 1].dungeon == 9)
				dgrid[i][j + 1].dungeon = 15;
			if (dgrid[i][j].dungeon == 18 && dgrid[i][j + 1].dungeon == 30)
				dgrid[i][j + 1].dungeon = 18;
			if (dgrid[i][j].dungeon == 24 && dgrid[i - 1][j].dungeon == 30)
				dgrid[i - 1][j].dungeon = 19;
			if (dgrid[i][j].dungeon == 21 && dgrid[i][j + 1].dungeon == 2)
				dgrid[i][j + 1].dungeon = 15;
			if (dgrid[i][j].dungeon == 21 && dgrid[i][j + 1].dungeon == 9)
				dgrid[i][j + 1].dungeon = 10;
			if (dgrid[i][j].dungeon == 22 && dgrid[i][j + 1].dungeon == 30)
				dgrid[i][j + 1].dungeon = 18;
			if (dgrid[i][j].dungeon == 21 && dgrid[i][j + 1].dungeon == 30)
				dgrid[i][j + 1].dungeon = 18;
			if (dgrid[i][j].dungeon == 16 && dgrid[i][j + 1].dungeon == 2)
				dgrid[i][j + 1].dungeon = 15;
			if (dgrid[i][j].dungeon == 13 && dgrid[i][j + 1].dungeon == 2)
				dgrid[i][j + 1].dungeon = 15;
			if (dgrid[i][j].dungeon == 22 && dgrid[i][j + 1].dungeon == 2)
				dgrid[i][j + 1].dungeon = 15;
			if (dgrid[i][j].dungeon == 21 && dgrid[i + 1][j].dungeon == 18 && dgrid[i + 2][j].dungeon == 30)
				dgrid[i + 1][j].dungeon = 24;
			if (dgrid[i][j].dungeon == 21 && dgrid[i + 1][j].dungeon == 9 && dgrid[i + 1][j + 1].dungeon == 1)
				dgrid[i + 1][j].dungeon = 16;
			if (dgrid[i][j].dungeon == 2 && dgrid[i + 1][j].dungeon == 27 && dgrid[i + 1][j + 1].dungeon == 2)
				dgrid[i + 1][j].dungeon = 29;
			if (dgrid[i][j].dungeon == 23 && dgrid[i][j + 1].dungeon == 2)
				dgrid[i][j + 1].dungeon = 15;
			if (dgrid[i][j].dungeon == 23 && dgrid[i][j + 1].dungeon == 9)
				dgrid[i][j + 1].dungeon = 15;
			if (dgrid[i][j].dungeon == 25 && dgrid[i][j + 1].dungeon == 2)
				dgrid[i][j + 1].dungeon = 15;
			if (dgrid[i][j].dungeon == 22 && dgrid[i + 1][j].dungeon == 9)
				dgrid[i + 1][j].dungeon = 11;
			if (dgrid[i][j].dungeon == 23 && dgrid[i + 1][j].dungeon == 9)
				dgrid[i + 1][j].dungeon = 11;
			if (dgrid[i][j].dungeon == 15 && dgrid[i + 1][j].dungeon == 1)
				dgrid[i + 1][j].dungeon = 16;
			if (dgrid[i][j].dungeon == 11 && dgrid[i + 1][j].dungeon == 15)
				dgrid[i + 1][j].dungeon = 14;
			if (dgrid[i][j].dungeon == 23 && dgrid[i + 1][j].dungeon == 1)
				dgrid[i + 1][j].dungeon = 16;
			if (dgrid[i][j].dungeon == 21 && dgrid[i + 1][j].dungeon == 27)
				dgrid[i + 1][j].dungeon = 26;
			if (dgrid[i][j].dungeon == 21 && dgrid[i + 1][j].dungeon == 18)
				dgrid[i + 1][j].dungeon = 24;
			if (dgrid[i][j].dungeon == 26 && dgrid[i + 1][j].dungeon == 1)
				dgrid[i + 1][j].dungeon = 16;
			if (dgrid[i][j].dungeon == 29 && dgrid[i + 1][j].dungeon == 1)
				dgrid[i + 1][j].dungeon = 16;
			if (dgrid[i][j].dungeon == 29 && dgrid[i][j + 1].dungeon == 2)
				dgrid[i][j + 1].dungeon = 15;
			if (dgrid[i][j].dungeon == 1 && dgrid[i][j - 1].dungeon == 15)
				dgrid[i][j - 1].dungeon = 10;
			if (dgrid[i][j].dungeon == 18 && dgrid[i][j + 1].dungeon == 2)
				dgrid[i][j + 1].dungeon = 15;
			if (dgrid[i][j].dungeon == 23 && dgrid[i][j + 1].dungeon == 30)
				dgrid[i][j + 1].dungeon = 18;
			if (dgrid[i][j].dungeon == 18 && dgrid[i][j + 1].dungeon == 9)
				dgrid[i][j + 1].dungeon = 10;
			if (dgrid[i][j].dungeon == 14 && dgrid[i + 1][j].dungeon == 30 && dgrid[i + 1][j + 1].dungeon == 30)
				dgrid[i + 1][j].dungeon = 23;
			if (dgrid[i][j].dungeon == 2 && dgrid[i + 1][j].dungeon == 28 && dgrid[i + 1][j - 1].dungeon == 6)
				dgrid[i + 1][j].dungeon = 23;
			if (dgrid[i][j].dungeon == 23 && dgrid[i + 1][j].dungeon == 18 && dgrid[i][j - 1].dungeon == 6)
				dgrid[i + 1][j].dungeon = 24;
			if (dgrid[i][j].dungeon == 14 && dgrid[i + 1][j].dungeon == 23 && dgrid[i + 2][j].dungeon == 30)
				dgrid[i + 1][j].dungeon = 28;
			if (dgrid[i][j].dungeon == 14 && dgrid[i + 1][j].dungeon == 28 && dgrid[i + 2][j].dungeon == 30 && dgrid[i + 1][j - 1].dungeon == 6)
				dgrid[i + 1][j].dungeon = 23;
			if (dgrid[i][j].dungeon == 23 && dgrid[i + 1][j].dungeon == 30)
				dgrid[i + 1][j].dungeon = 19;
			if (dgrid[i][j].dungeon == 29 && dgrid[i + 1][j].dungeon == 30)
				dgrid[i + 1][j].dungeon = 19;
			if (dgrid[i][j].dungeon == 29 && dgrid[i][j + 1].dungeon == 30)
				dgrid[i][j + 1].dungeon = 18;
			if (dgrid[i][j].dungeon == 19 && dgrid[i + 1][j].dungeon == 30)
				dgrid[i + 1][j].dungeon = 19;
			if (dgrid[i][j].dungeon == 21 && dgrid[i + 1][j].dungeon == 30)
				dgrid[i + 1][j].dungeon = 19;
			if (dgrid[i][j].dungeon == 26 && dgrid[i + 1][j].dungeon == 30)
				dgrid[i + 1][j].dungeon = 19;
			if (dgrid[i][j].dungeon == 16 && dgrid[i][j + 1].dungeon == 30)
				dgrid[i][j + 1].dungeon = 18;
			if (dgrid[i][j].dungeon == 13 && dgrid[i][j + 1].dungeon == 9)
				dgrid[i][j + 1].dungeon = 10;
			if (dgrid[i][j].dungeon == 25 && dgrid[i][j + 1].dungeon == 30)
				dgrid[i][j + 1].dungeon = 18;
			if (dgrid[i][j].dungeon == 18 && dgrid[i][j + 1].dungeon == 2)
				dgrid[i][j + 1].dungeon = 15;
			if (dgrid[i][j].dungeon == 11 && dgrid[i + 1][j].dungeon == 3)
				dgrid[i + 1][j].dungeon = 5;
			if (dgrid[i][j].dungeon == 19 && dgrid[i + 1][j].dungeon == 9)
				dgrid[i + 1][j].dungeon = 11;
			if (dgrid[i][j].dungeon == 19 && dgrid[i + 1][j].dungeon == 1)
				dgrid[i + 1][j].dungeon = 13;
			if (dgrid[i][j].dungeon == 19 && dgrid[i + 1][j].dungeon == 13 && dgrid[i + 1][j - 1].dungeon == 6)
				dgrid[i + 1][j].dungeon = 16;
		}
	}
	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dgrid[i][j].dungeon == 21 && dgrid[i][j + 1].dungeon == 24 && dgrid[i][j + 2].dungeon == 1)
				dgrid[i][j + 1].dungeon = 17;
			if (dgrid[i][j].dungeon == 15 && dgrid[i + 1][j + 1].dungeon == 9 && dgrid[i + 1][j - 1].dungeon == 1 && dgrid[i + 2][j].dungeon == 16)
				dgrid[i + 1][j].dungeon = 29;
			if (dgrid[i][j].dungeon == 2 && dgrid[i - 1][j].dungeon == 6)
				dgrid[i - 1][j].dungeon = 8;
			if (dgrid[i][j].dungeon == 1 && dgrid[i][j - 1].dungeon == 6)
				dgrid[i][j - 1].dungeon = 7;
			if (dgrid[i][j].dungeon == 6 && dgrid[i + 1][j].dungeon == 15 && dgrid[i + 1][j + 1].dungeon == 4)
				dgrid[i + 1][j].dungeon = 10;
			if (dgrid[i][j].dungeon == 1 && dgrid[i][j + 1].dungeon == 3)
				dgrid[i][j + 1].dungeon = 4;
			if (dgrid[i][j].dungeon == 1 && dgrid[i][j + 1].dungeon == 6)
				dgrid[i][j + 1].dungeon = 4;
			if (dgrid[i][j].dungeon == 9 && dgrid[i][j + 1].dungeon == 3)
				dgrid[i][j + 1].dungeon = 4;
			if (dgrid[i][j].dungeon == 10 && dgrid[i][j + 1].dungeon == 3)
				dgrid[i][j + 1].dungeon = 4;
			if (dgrid[i][j].dungeon == 13 && dgrid[i][j + 1].dungeon == 3)
				dgrid[i][j + 1].dungeon = 4;
			if (dgrid[i][j].dungeon == 1 && dgrid[i][j + 1].dungeon == 5)
				dgrid[i][j + 1].dungeon = 12;
			if (dgrid[i][j].dungeon == 1 && dgrid[i][j + 1].dungeon == 16)
				dgrid[i][j + 1].dungeon = 13;
			if (dgrid[i][j].dungeon == 6 && dgrid[i][j + 1].dungeon == 13)
				dgrid[i][j + 1].dungeon = 16;
			if (dgrid[i][j].dungeon == 25 && dgrid[i][j + 1].dungeon == 9)
				dgrid[i][j + 1].dungeon = 10;
			if (dgrid[i][j].dungeon == 13 && dgrid[i][j + 1].dungeon == 5)
				dgrid[i][j + 1].dungeon = 12;
			if (dgrid[i][j].dungeon == 28 && dgrid[i][j - 1].dungeon == 6 && dgrid[i + 1][j].dungeon == 1)
				dgrid[i + 1][j].dungeon = 23;
			if (dgrid[i][j].dungeon == 19 && dgrid[i + 1][j].dungeon == 10)
				dgrid[i + 1][j].dungeon = 17;
			if (dgrid[i][j].dungeon == 21 && dgrid[i + 1][j].dungeon == 9)
				dgrid[i + 1][j].dungeon = 11;
			if (dgrid[i][j].dungeon == 11 && dgrid[i + 1][j].dungeon == 3)
				dgrid[i + 1][j].dungeon = 5;
			if (dgrid[i][j].dungeon == 10 && dgrid[i + 1][j].dungeon == 4)
				dgrid[i + 1][j].dungeon = 12;
			if (dgrid[i][j].dungeon == 14 && dgrid[i + 1][j].dungeon == 4)
				dgrid[i + 1][j].dungeon = 12;
			if (dgrid[i][j].dungeon == 27 && dgrid[i + 1][j].dungeon == 9)
				dgrid[i + 1][j].dungeon = 11;
			if (dgrid[i][j].dungeon == 15 && dgrid[i + 1][j].dungeon == 4)
				dgrid[i + 1][j].dungeon = 12;
			if (dgrid[i][j].dungeon == 21 && dgrid[i + 1][j].dungeon == 1)
				dgrid[i + 1][j].dungeon = 16;
			if (dgrid[i][j].dungeon == 11 && dgrid[i + 1][j].dungeon == 4)
				dgrid[i + 1][j].dungeon = 12;
			if (dgrid[i][j].dungeon == 2 && dgrid[i + 1][j].dungeon == 3)
				dgrid[i + 1][j].dungeon = 5;
			if (dgrid[i][j].dungeon == 9 && dgrid[i + 1][j].dungeon == 3)
				dgrid[i + 1][j].dungeon = 5;
			if (dgrid[i][j].dungeon == 14 && dgrid[i + 1][j].dungeon == 3)
				dgrid[i + 1][j].dungeon = 5;
			if (dgrid[i][j].dungeon == 15 && dgrid[i + 1][j].dungeon == 3)
				dgrid[i + 1][j].dungeon = 5;
			if (dgrid[i][j].dungeon == 2 && dgrid[i + 1][j].dungeon == 5 && dgrid[i + 1][j - 1].dungeon == 16)
				dgrid[i + 1][j].dungeon = 12;
			if (dgrid[i][j].dungeon == 2 && dgrid[i + 1][j].dungeon == 4)
				dgrid[i + 1][j].dungeon = 12;
			if (dgrid[i][j].dungeon == 9 && dgrid[i + 1][j].dungeon == 4)
				dgrid[i + 1][j].dungeon = 12;
			if (dgrid[i][j].dungeon == 1 && dgrid[i][j - 1].dungeon == 8)
				dgrid[i][j - 1].dungeon = 9;
			if (dgrid[i][j].dungeon == 28 && dgrid[i + 1][j].dungeon == 23 && dgrid[i + 1][j + 1].dungeon == 3)
				dgrid[i + 1][j].dungeon = 16;
		}
	}
	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dgrid[i][j].dungeon == 21 && dgrid[i + 1][j].dungeon == 10)
				dgrid[i + 1][j].dungeon = 17;
			if (dgrid[i][j].dungeon == 17 && dgrid[i + 1][j].dungeon == 4)
				dgrid[i + 1][j].dungeon = 12;
			if (dgrid[i][j].dungeon == 10 && dgrid[i + 1][j].dungeon == 4)
				dgrid[i + 1][j].dungeon = 12;
			if (dgrid[i][j].dungeon == 17 && dgrid[i][j + 1].dungeon == 5)
				dgrid[i][j + 1].dungeon = 12;
			if (dgrid[i][j].dungeon == 29 && dgrid[i][j + 1].dungeon == 9)
				dgrid[i][j + 1].dungeon = 10;
			if (dgrid[i][j].dungeon == 13 && dgrid[i][j + 1].dungeon == 5)
				dgrid[i][j + 1].dungeon = 12;
			if (dgrid[i][j].dungeon == 9 && dgrid[i][j + 1].dungeon == 16)
				dgrid[i][j + 1].dungeon = 13;
			if (dgrid[i][j].dungeon == 10 && dgrid[i][j + 1].dungeon == 16)
				dgrid[i][j + 1].dungeon = 13;
			if (dgrid[i][j].dungeon == 16 && dgrid[i][j + 1].dungeon == 3)
				dgrid[i][j + 1].dungeon = 4;
			if (dgrid[i][j].dungeon == 11 && dgrid[i][j + 1].dungeon == 5)
				dgrid[i][j + 1].dungeon = 12;
			if (dgrid[i][j].dungeon == 10 && dgrid[i + 1][j].dungeon == 3 && dgrid[i + 1][j - 1].dungeon == 16)
				dgrid[i + 1][j].dungeon = 12;
			if (dgrid[i][j].dungeon == 16 && dgrid[i][j + 1].dungeon == 5)
				dgrid[i][j + 1].dungeon = 12;
			if (dgrid[i][j].dungeon == 1 && dgrid[i][j + 1].dungeon == 6)
				dgrid[i][j + 1].dungeon = 4;
			if (dgrid[i][j].dungeon == 21 && dgrid[i + 1][j].dungeon == 13 && dgrid[i][j + 1].dungeon == 10)
				dgrid[i + 1][j + 1].dungeon = 12;
			if (dgrid[i][j].dungeon == 15 && dgrid[i + 1][j].dungeon == 10)
				dgrid[i + 1][j].dungeon = 17;
			if (dgrid[i][j].dungeon == 22 && dgrid[i][j + 1].dungeon == 11)
				dgrid[i][j + 1].dungeon = 17;
			if (dgrid[i][j].dungeon == 15 && dgrid[i + 1][j].dungeon == 28 && dgrid[i + 2][j].dungeon == 16)
				dgrid[i + 1][j].dungeon = 23;
			if (dgrid[i][j].dungeon == 28 && dgrid[i + 1][j].dungeon == 23 && dgrid[i + 1][j + 1].dungeon == 1 && dgrid[i + 2][j].dungeon == 6)
				dgrid[i + 1][j].dungeon = 16;
		}
	}
	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dgrid[i][j].dungeon == 15 && dgrid[i + 1][j].dungeon == 28 && dgrid[i + 2][j].dungeon == 16)
				dgrid[i + 1][j].dungeon = 23;
			if (dgrid[i][j].dungeon == 21 && dgrid[i + 1][j - 1].dungeon == 21 && dgrid[i + 1][j + 1].dungeon == 13 && dgrid[i + 2][j].dungeon == 2)
				dgrid[i + 1][j].dungeon = 17;
			if (dgrid[i][j].dungeon == 19 && dgrid[i + 1][j].dungeon == 15 && dgrid[i + 1][j + 1].dungeon == 12)
				dgrid[i + 1][j].dungeon = 17;
		}
	}
}

static void DRLG_L4Subs()
{
	int x, y, i, rv;
	BYTE c;

	for (y = 0; y < DMAXY; y++) {
		for (x = 0; x < DMAXX; x++) {
			if (random_(0, 3) == 0) {
				c = L4BTYPES[dgrid[x][y].dungeon];
				if (c != 0 && dgrid[x][y].dflags == 0) {
					rv = random_(0, 16);
					i = -1;
					while (rv >= 0) {
						i++;
						if (i == sizeof(L4BTYPES)) {
							i = 0;
						}
						if (c == L4BTYPES[i]) {
							rv--;
						}
					}
					dgrid[x][y].dungeon = i;
				}
			}
		}
	}
	for (y = 0; y < DMAXY; y++) {
		for (x = 0; x < DMAXX; x++) {
			if (random_(0, 10) == 0) {
				if (L4BTYPES[dgrid[x][y].dungeon] == 6 && dgrid[x][y].dflags == 0) {
					dgrid[x][y].dungeon = random_(0, 3) + 95;
				}
			}
		}
	}
}

static void L4makeDungeon()
{
	int i, j, k, l;

	for (j = 0; j < 20; j++) {
		for (i = 0; i < 20; i++) {
			k = i << 1;
			l = j << 1;
			L4dungeon[k][l] = dung[i][j];
			L4dungeon[k][l + 1] = dung[i][j];
			L4dungeon[k + 1][l] = dung[i][j];
			L4dungeon[k + 1][l + 1] = dung[i][j];
		}
	}
	for (j = 0; j < 20; j++) {
		for (i = 0; i < 20; i++) {
			k = i << 1;
			l = j << 1;
			L4dungeon[k][l + 40] = dung[i][19 - j];
			L4dungeon[k][l + 41] = dung[i][19 - j];
			L4dungeon[k + 1][l + 40] = dung[i][19 - j];
			L4dungeon[k + 1][l + 41] = dung[i][19 - j];
		}
	}
	for (j = 0; j < 20; j++) {
		for (i = 0; i < 20; i++) {
			k = i << 1;
			l = j << 1;
			L4dungeon[k + 40][l] = dung[19 - i][j];
			L4dungeon[k + 40][l + 1] = dung[19 - i][j];
			L4dungeon[k + 41][l] = dung[19 - i][j];
			L4dungeon[k + 41][l + 1] = dung[19 - i][j];
		}
	}
	for (j = 0; j < 20; j++) {
		for (i = 0; i < 20; i++) {
			k = i << 1;
			l = j << 1;
			L4dungeon[k + 40][l + 40] = dung[19 - i][19 - j];
			L4dungeon[k + 40][l + 41] = dung[19 - i][19 - j];
			L4dungeon[k + 41][l + 40] = dung[19 - i][19 - j];
			L4dungeon[k + 41][l + 41] = dung[19 - i][19 - j];
		}
	}
}

static void uShape()
{
	int j, i, rv;

	for (j = 19; j >= 0; j--) {
		for (i = 19; i >= 0; i--) {
			if (dung[i][j] != 1) {
				hallok[j] = FALSE;
			}
			if (dung[i][j] == 1) {
				// BUGFIX: check that i + 1 < 20 and j + 1 < 20 (fixed)
				if (i + 1 < 20 && j + 1 < 20
				    && dung[i][j + 1] == 1 && dung[i + 1][j + 1] == 0) {
					hallok[j] = TRUE;
				} else {
					hallok[j] = FALSE;
				}
				i = 0;
			}
		}
	}

	rv = random_(0, 19) + 1;
	do {
		if (hallok[rv]) {
			for (i = 19; i >= 0; i--) {
				if (dung[i][rv] == 1) {
					i = -1;
					rv = 0;
				} else {
					dung[i][rv] = 1;
					dung[i][rv + 1] = 1;
				}
			}
		} else {
			rv++;
			if (rv == 20) {
				rv = 1;
			}
		}
	} while (rv != 0);

	for (i = 19; i >= 0; i--) {
		for (j = 19; j >= 0; j--) {
			if (dung[i][j] != 1) {
				hallok[i] = FALSE;
			}
			if (dung[i][j] == 1) {
				// BUGFIX: check that i + 1 < 20 and j + 1 < 20 (fixed)
				if (i + 1 < 20 && j + 1 < 20
				    && dung[i + 1][j] == 1 && dung[i + 1][j + 1] == 0) {
					hallok[i] = TRUE;
				} else {
					hallok[i] = FALSE;
				}
				j = 0;
			}
		}
	}

	rv = random_(0, 19) + 1;
	do {
		if (hallok[rv]) {
			for (j = 19; j >= 0; j--) {
				if (dung[rv][j] == 1) {
					j = -1;
					rv = 0;
				} else {
					dung[rv][j] = 1;
					dung[rv + 1][j] = 1;
				}
			}
		} else {
			rv++;
			if (rv == 20) {
				rv = 1;
			}
		}
	} while (rv != 0);
}

static long GetArea()
{
	int i, j;
	long rv;

	rv = 0;

	for (j = 0; j < 20; j++) {
		for (i = 0; i < 20; i++) {
			if (dung[i][j] == 1) {
				rv++;
			}
		}
	}

	return rv;
}

static void L4drawRoom(int x, int y, int width, int height)
{
	int i, j;

	for (j = 0; j < height; j++) {
		for (i = 0; i < width; i++) {
			dung[i + x][j + y] = 1;
		}
	}
}

static BOOL L4checkRoom(int x, int y, int width, int height)
{
	int i, j;

	if (x <= 0 || y <= 0) {
		return FALSE;
	}

	for (j = 0; j < height; j++) {
		for (i = 0; i < width; i++) {
			if (i + x < 0 || i + x >= 20 || j + y < 0 || j + y >= 20) {
				return FALSE;
			}
			if (dung[i + x][j + y] != 0) {
				return FALSE;
			}
		}
	}

	return TRUE;
}

static void L4roomGen(int x, int y, int w, int h, int dir)
{
	int num;
	BOOL ran, ran2;
	int width, height, rx, ry, ry2;
	int cw, ch, cx1, cy1, cx2;

	int dirProb = random_(0, 4);

	switch (dir == 1 ? dirProb != 0 : dirProb == 0) {
	case FALSE:
		num = 0;
		do {
			cw = (random_(0, 5) + 2) & ~1;
			ch = (random_(0, 5) + 2) & ~1;
			cy1 = h / 2 + y - ch / 2;
			cx1 = x - cw;
			ran = L4checkRoom(cx1 - 1, cy1 - 1, ch + 2, cw + 1); /// BUGFIX: swap args 3 and 4 ("ch+2" and "cw+1")
			num++;
		} while (ran == FALSE && num < 20);

		if (ran == TRUE)
			L4drawRoom(cx1, cy1, cw, ch);
		cx2 = x + w;
		ran2 = L4checkRoom(cx2, cy1 - 1, cw + 1, ch + 2);
		if (ran2 == TRUE)
			L4drawRoom(cx2, cy1, cw, ch);
		if (ran == TRUE)
			L4roomGen(cx1, cy1, cw, ch, 1);
		if (ran2 == TRUE)
			L4roomGen(cx2, cy1, cw, ch, 1);
		break;
	case TRUE:
		num = 0;
		do {
			width = (random_(0, 5) + 2) & ~1;
			height = (random_(0, 5) + 2) & ~1;
			rx = w / 2 + x - width / 2;
			ry = y - height;
			ran = L4checkRoom(rx - 1, ry - 1, width + 2, height + 1);
			num++;
		} while (ran == FALSE && num < 20);

		if (ran == TRUE)
			L4drawRoom(rx, ry, width, height);
		ry2 = y + h;
		ran2 = L4checkRoom(rx - 1, ry2, width + 2, height + 1);
		if (ran2 == TRUE)
			L4drawRoom(rx, ry2, width, height);
		if (ran == TRUE)
			L4roomGen(rx, ry, width, height, 0);
		if (ran2 == TRUE)
			L4roomGen(rx, ry2, width, height, 0);
		break;
	}
}

static void L4firstRoom()
{
	int x, y, w, h, rndx, rndy, xmin, xmax, ymin, ymax, tx, ty;

	if (level.currlevel != 16) {
		if (level.currlevel == quests[Q_WARLORD]._qlevel && quests[Q_WARLORD]._qactive) {
			/// ASSERT: assert(gbMaxPlayers == 1);
			w = 11;
			h = 11;
		} else if (level.currlevel == quests[Q_BETRAYER]._qlevel && gbMaxPlayers != 1) {
			w = 11;
			h = 11;
		} else {
			w = random_(0, 5) + 2;
			h = random_(0, 5) + 2;
		}
	} else {
		w = 14;
		h = 14;
	}

	xmin = (20 - w) >> 1;
	xmax = 19 - w;
	rndx = random_(0, xmax - xmin + 1) + xmin;
	if (rndx + w > 19) {
		tx = w + rndx - 19;
		x = rndx - tx + 1;
	} else {
		x = rndx;
	}
	ymin = (20 - h) >> 1;
	ymax = 19 - h;
	rndy = random_(0, ymax - ymin + 1) + ymin;
	if (rndy + h > 19) {
		ty = h + rndy - 19;
		y = rndy - ty + 1;
	} else {
		y = rndy;
	}

	if (level.currlevel == 16) {
		l4holdx = x;
		l4holdy = y;
	}
	if (QuestStatus(Q_WARLORD) || level.currlevel == quests[Q_BETRAYER]._qlevel && gbMaxPlayers != 1) {
		SP4x1 = x + 1;
		SP4y1 = y + 1;
		SP4x2 = SP4x1 + w;
		SP4y2 = SP4y1 + h;
	} else {
		SP4x1 = 0;
		SP4y1 = 0;
		SP4x2 = 0;
		SP4y2 = 0;
	}

	L4drawRoom(x, y, w, h);
	L4roomGen(x, y, w, h, random_(0, 2));
}

void L4SaveQuads()
{
	int i, j, x, y;

	x = l4holdx;
	y = l4holdy;

	for (j = 0; j < 14; j++) {
		for (i = 0; i < 14; i++) {
			dgrid[i + x][j + y].dflags = 1;
			dgrid[DMAXX - 1 - i - x][j + y].dflags = 1;
			dgrid[i + x][DMAXY - 1 - j - y].dflags = 1;
			dgrid[DMAXX - 1 - i - x][DMAXY - 1 - j - y].dflags = 1;
		}
	}
}

void DRLG_L4SetRoom(BYTE *pSetPiece, int rx1, int ry1)
{
	int rw, rh, i, j;
	BYTE *sp;

	rw = pSetPiece[0];
	rh = pSetPiece[2];
	sp = &pSetPiece[4];

	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*sp != 0) {
				dgrid[i + rx1][j + ry1].dungeon = *sp;
				dgrid[i + rx1][j + ry1].dflags |= DLRG_PROTECTED;
			} else {
				dgrid[i + rx1][j + ry1].dungeon = 6;
			}
			sp += 2;
		}
	}
}

void DRLG_LoadDiabQuads(BOOL preflag)
{
	BYTE *lpSetPiece;

	lpSetPiece = LoadFileInMem("Levels\\L4Data\\diab1.DUN", NULL);
	diabquad1x = 4 + l4holdx;
	diabquad1y = 4 + l4holdy;
	DRLG_L4SetRoom(lpSetPiece, diabquad1x, diabquad1y);
	mem_free_dbg(lpSetPiece);

	if (preflag) {
		lpSetPiece = LoadFileInMem("Levels\\L4Data\\diab2b.DUN", NULL);
	} else {
		lpSetPiece = LoadFileInMem("Levels\\L4Data\\diab2a.DUN", NULL);
	}
	diabquad2x = 27 - l4holdx;
	diabquad2y = 1 + l4holdy;
	DRLG_L4SetRoom(lpSetPiece, diabquad2x, diabquad2y);
	mem_free_dbg(lpSetPiece);

	if (preflag) {
		lpSetPiece = LoadFileInMem("Levels\\L4Data\\diab3b.DUN", NULL);
	} else {
		lpSetPiece = LoadFileInMem("Levels\\L4Data\\diab3a.DUN", NULL);
	}
	diabquad3x = 1 + l4holdx;
	diabquad3y = 27 - l4holdy;
	DRLG_L4SetRoom(lpSetPiece, diabquad3x, diabquad3y);
	mem_free_dbg(lpSetPiece);

	if (preflag) {
		lpSetPiece = LoadFileInMem("Levels\\L4Data\\diab4b.DUN", NULL);
	} else {
		lpSetPiece = LoadFileInMem("Levels\\L4Data\\diab4a.DUN", NULL);
	}
	diabquad4x = 28 - l4holdx;
	diabquad4y = 28 - l4holdy;
	DRLG_L4SetRoom(lpSetPiece, diabquad4x, diabquad4y);
	mem_free_dbg(lpSetPiece);
}

static BOOL DRLG_L4PlaceMiniSet(const BYTE *miniset, int tmin, int tmax, int cx, int cy, BOOL setview, int ldir)
{
	int sx, sy, sw, sh, xx, yy, i, ii, numt, bailcnt;
	BOOL found;

	sw = miniset[0];
	sh = miniset[1];

	if (tmax - tmin == 0) {
		numt = 1;
	} else {
		numt = random_(0, tmax - tmin) + tmin;
	}

	for (i = 0; i < numt; i++) {
		sx = random_(0, DMAXX - sw);
		sy = random_(0, DMAXY - sh);
		found = FALSE;
		for (bailcnt = 0; !found && bailcnt < 200; bailcnt++) {
			found = TRUE;
			if (sx >= SP4x1 && sx <= SP4x2 && sy >= SP4y1 && sy <= SP4y2) {
				found = FALSE;
			}
			if (cx != -1 && sx >= cx - sw && sx <= cx + 12) {
				sx = random_(0, DMAXX - sw);
				sy = random_(0, DMAXY - sh);
				found = FALSE;
			}
			if (cy != -1 && sy >= cy - sh && sy <= cy + 12) {
				sx = random_(0, DMAXX - sw);
				sy = random_(0, DMAXY - sh);
				found = FALSE;
			}
			ii = 2;
			for (yy = 0; yy < sh && found == TRUE; yy++) {
				for (xx = 0; xx < sw && found == TRUE; xx++) {
					if (miniset[ii] != 0 && dgrid[xx + sx][yy + sy].dungeon != miniset[ii]) {
						found = FALSE;
					}
					if (dgrid[xx + sx][yy + sy].dflags != 0) {
						found = FALSE;
					}
					ii++;
				}
			}
			if (!found) {
				sx++;
				if (sx == DMAXX - sw) {
					sx = 0;
					sy++;
					if (sy == DMAXY - sh) {
						sy = 0;
					}
				}
			}
		}
		if (bailcnt >= 200) {
			return FALSE;
		}
		ii = sw * sh + 2;
		for (yy = 0; yy < sh; yy++) {
			for (xx = 0; xx < sw; xx++) {
				if (miniset[ii] != 0) {
					dgrid[xx + sx][yy + sy].dungeon = miniset[ii];
					dgrid[xx + sx][yy + sy].dflags |= 8;
				}
				ii++;
			}
		}
	}

	if (level.currlevel == 15) {
		quests[Q_BETRAYER]._qtx = sx + 1;
		quests[Q_BETRAYER]._qty = sy + 1;
	}
	if (setview == TRUE) {
		ViewX = 2 * sx + 21;
		ViewY = 2 * sy + 22;
	}
	if (ldir == 0) {
		LvlViewX = 2 * sx + 21;
		LvlViewY = 2 * sy + 22;
	}

	return TRUE;
}

static void DRLG_L4FTVR(int i, int j, int x, int y, int d)
{
	if (grid[x][y].dTransVal != 0 || dgrid[i][j].dungeon != 6) {
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
		if (d == 5) {
			grid[x + 1][y + 1].dTransVal = TransVal;
		}
		if (d == 6) {
			grid[x][y + 1].dTransVal = TransVal;
		}
		if (d == 7) {
			grid[x + 1][y].dTransVal = TransVal;
		}
		if (d == 8) {
			grid[x][y].dTransVal = TransVal;
		}
	} else {
		grid[x][y].dTransVal = TransVal;
		grid[x + 1][y].dTransVal = TransVal;
		grid[x][y + 1].dTransVal = TransVal;
		grid[x + 1][y + 1].dTransVal = TransVal;
		DRLG_L4FTVR(i + 1, j, x + 2, y, 1);
		DRLG_L4FTVR(i - 1, j, x - 2, y, 2);
		DRLG_L4FTVR(i, j + 1, x, y + 2, 3);
		DRLG_L4FTVR(i, j - 1, x, y - 2, 4);
		DRLG_L4FTVR(i - 1, j - 1, x - 2, y - 2, 5);
		DRLG_L4FTVR(i + 1, j - 1, x + 2, y - 2, 6);
		DRLG_L4FTVR(i - 1, j + 1, x - 2, y + 2, 7);
		DRLG_L4FTVR(i + 1, j + 1, x + 2, y + 2, 8);
	}
}

static void DRLG_L4FloodTVal()
{
	int i, j, xx, yy;

	yy = 16;
	for (j = 0; j < DMAXY; j++) {
		xx = 16;
		for (i = 0; i < DMAXX; i++) {
			if (dgrid[i][j].dungeon == 6 && grid[xx][yy].dTransVal == 0) {
				DRLG_L4FTVR(i, j, xx, yy, 0);
				TransVal++;
			}
			xx += 2;
		}
		yy += 2;
	}
}

BOOL IsDURWall(char d)
{
	if (d == 25) {
		return TRUE;
	}
	if (d == 28) {
		return TRUE;
	}
	if (d == 23) {
		return TRUE;
	}

	return FALSE;
}

BOOL IsDLLWall(char dd)
{
	if (dd == 27) {
		return TRUE;
	}
	if (dd == 26) {
		return TRUE;
	}
	if (dd == 22) {
		return TRUE;
	}

	return FALSE;
}

static void DRLG_L4TransFix()
{
	int i, j, xx, yy;

	yy = 16;
	for (j = 0; j < DMAXY; j++) {
		xx = 16;
		for (i = 0; i < DMAXX; i++) {
			if (IsDURWall(dgrid[i][j].dungeon) && dgrid[i][j - 1].dungeon == 18) {
				grid[xx + 1][yy].dTransVal = grid[xx][yy].dTransVal;
				grid[xx + 1][yy + 1].dTransVal = grid[xx][yy].dTransVal;
			}
			if (IsDLLWall(dgrid[i][j].dungeon) && dgrid[i + 1][j].dungeon == 19) {
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
			if (dgrid[i][j].dungeon == 24) {
				grid[xx + 1][yy].dTransVal = grid[xx][yy].dTransVal;
				grid[xx][yy + 1].dTransVal = grid[xx][yy].dTransVal;
				grid[xx + 1][yy + 1].dTransVal = grid[xx][yy].dTransVal;
			}
			if (dgrid[i][j].dungeon == 57) {
				grid[xx - 1][yy].dTransVal = grid[xx][yy + 1].dTransVal;
				grid[xx][yy].dTransVal = grid[xx][yy + 1].dTransVal;
			}
			if (dgrid[i][j].dungeon == 53) {
				grid[xx][yy - 1].dTransVal = grid[xx + 1][yy].dTransVal;
				grid[xx][yy].dTransVal = grid[xx + 1][yy].dTransVal;
			}
			xx += 2;
		}
		yy += 2;
	}
}

static void DRLG_L4Corners()
{
	int i, j;

	for (j = 1; j < DMAXY - 1; j++) {
		for (i = 1; i < DMAXX - 1; i++) {
			if (dgrid[i][j].dungeon >= 18 && dgrid[i][j].dungeon <= 30) {
				if (dgrid[i + 1][j].dungeon < 18) {
					dgrid[i][j].dungeon += 98;
				} else if (dgrid[i][j + 1].dungeon < 18) {
					dgrid[i][j].dungeon += 98;
				}
			}
		}
	}
}

void L4FixRim()
{
	int i, j;

	for (i = 0; i < 20; i++) {
		dung[i][0] = 0;
	}
	for (j = 0; j < 20; j++) {
		dung[0][j] = 0;
	}
}

void DRLG_L4GeneralFix()
{
	int i, j;

	for (j = 0; j < DMAXY - 1; j++) {
		for (i = 0; i < DMAXX - 1; i++) {
			if ((dgrid[i][j].dungeon == 24 || dgrid[i][j].dungeon == 122) && dgrid[i + 1][j].dungeon == 2 && dgrid[i][j + 1].dungeon == 5) {
				dgrid[i][j].dungeon = 17;
			}
		}
	}
}

static void DRLG_L4(int entry)
{
	int i, j, spi, spj, ar;
	BOOL doneflag;

	do {
		DRLG_InitTrans();
		do {
			InitL4Dungeon();
			L4firstRoom();
			L4FixRim();
			ar = GetArea();
			if (ar >= 173) {
				uShape();
			}
		} while (ar < 173);
		L4makeDungeon();
		L4makeDmt();
		L4tileFix();
		if (level.currlevel == 16) {
			L4SaveQuads();
		}
		if (QuestStatus(Q_WARLORD) || level.currlevel == quests[Q_BETRAYER]._qlevel && gbMaxPlayers != 1) {
			for (spi = SP4x1; spi < SP4x2; spi++) {
				for (spj = SP4y1; spj < SP4y2; spj++) {
					dgrid[spi][spj].dflags = 1;
				}
			}
		}
		L4AddWall();
		DRLG_L4FloodTVal();
		DRLG_L4TransFix();
		if (setloadflag) {
			DRLG_L4SetSPRoom(SP4x1, SP4y1);
		}
		if (level.currlevel == 16) {
			DRLG_LoadDiabQuads(TRUE);
		}
		if (QuestStatus(Q_WARLORD)) {
			if (entry == 0) {
				doneflag = DRLG_L4PlaceMiniSet(L4USTAIRS, 1, 1, -1, -1, 1, 0);
				if (doneflag && level.currlevel == 13) {
					doneflag = DRLG_L4PlaceMiniSet(L4TWARP, 1, 1, -1, -1, 0, 6);
				}
				ViewX++;
			} else if (entry == 1) {
				doneflag = DRLG_L4PlaceMiniSet(L4USTAIRS, 1, 1, -1, -1, 0, 0);
				if (doneflag && level.currlevel == 13) {
					doneflag = DRLG_L4PlaceMiniSet(L4TWARP, 1, 1, -1, -1, 0, 6);
				}
				ViewX = 2 * setpc_x + 22;
				ViewY = 2 * setpc_y + 22;
			} else {
				doneflag = DRLG_L4PlaceMiniSet(L4USTAIRS, 1, 1, -1, -1, 0, 0);
				if (doneflag && level.currlevel == 13) {
					doneflag = DRLG_L4PlaceMiniSet(L4TWARP, 1, 1, -1, -1, 1, 6);
				}
				ViewX++;
			}
		} else if (level.currlevel != 15) {
			if (entry == 0) {
				doneflag = DRLG_L4PlaceMiniSet(L4USTAIRS, 1, 1, -1, -1, 1, 0);
				if (doneflag && level.currlevel != 16) {
					doneflag = DRLG_L4PlaceMiniSet(L4DSTAIRS, 1, 1, -1, -1, 0, 1);
				}
				if (doneflag && level.currlevel == 13) {
					doneflag = DRLG_L4PlaceMiniSet(L4TWARP, 1, 1, -1, -1, 0, 6);
				}
				ViewX++;
			} else if (entry == 1) {
				doneflag = DRLG_L4PlaceMiniSet(L4USTAIRS, 1, 1, -1, -1, 0, 0);
				if (doneflag && level.currlevel != 16) {
					doneflag = DRLG_L4PlaceMiniSet(L4DSTAIRS, 1, 1, -1, -1, 1, 1);
				}
				if (doneflag && level.currlevel == 13) {
					doneflag = DRLG_L4PlaceMiniSet(L4TWARP, 1, 1, -1, -1, 0, 6);
				}
				ViewY++;
			} else {
				doneflag = DRLG_L4PlaceMiniSet(L4USTAIRS, 1, 1, -1, -1, 0, 0);
				if (doneflag && level.currlevel != 16) {
					doneflag = DRLG_L4PlaceMiniSet(L4DSTAIRS, 1, 1, -1, -1, 0, 1);
				}
				if (doneflag && level.currlevel == 13) {
					doneflag = DRLG_L4PlaceMiniSet(L4TWARP, 1, 1, -1, -1, 1, 6);
				}
				ViewX++;
			}
		} else {
			if (entry == 0) {
				doneflag = DRLG_L4PlaceMiniSet(L4USTAIRS, 1, 1, -1, -1, 1, 0);
				if (doneflag) {
					if (gbMaxPlayers == 1 && quests[Q_DIABLO]._qactive != QUEST_ACTIVE) {
						doneflag = DRLG_L4PlaceMiniSet(L4PENTA, 1, 1, -1, -1, 0, 1);
					} else {
						doneflag = DRLG_L4PlaceMiniSet(L4PENTA2, 1, 1, -1, -1, 0, 1);
					}
				}
				ViewX++;
			} else {
				doneflag = DRLG_L4PlaceMiniSet(L4USTAIRS, 1, 1, -1, -1, 0, 0);
				if (doneflag) {
					if (gbMaxPlayers == 1 && quests[Q_DIABLO]._qactive != QUEST_ACTIVE) {
						doneflag = DRLG_L4PlaceMiniSet(L4PENTA, 1, 1, -1, -1, 1, 1);
					} else {
						doneflag = DRLG_L4PlaceMiniSet(L4PENTA2, 1, 1, -1, -1, 1, 1);
					}
				}
				ViewY++;
			}
		}
	} while (!doneflag);

	DRLG_L4GeneralFix();

	if (level.currlevel != 16) {
		DRLG_PlaceThemeRooms(7, 10, 6, 8, 1);
	}

	DRLG_L4Shadows();
	DRLG_L4Corners();
	DRLG_L4Subs();
	DRLG_Init_Globals();

	if (QuestStatus(Q_WARLORD)) {
		for (j = 0; j < DMAXY; j++) {
			for (i = 0; i < DMAXX; i++) {
				dgrid[i][j].pdungeon = dgrid[i][j].dungeon;
			}
		}
	}

	DRLG_CheckQuests(SP4x1, SP4y1);

	if (level.currlevel == 15) {
		for (j = 0; j < DMAXY; j++) {
			for (i = 0; i < DMAXX; i++) {
				if (dgrid[i][j].dungeon == 98) {
					Make_SetPC(i - 1, j - 1, 5, 5);
				}
				if (dgrid[i][j].dungeon == 107) {
					Make_SetPC(i - 1, j - 1, 5, 5);
				}
			}
		}
	}
	if (level.currlevel == 16) {
		for (j = 0; j < DMAXY; j++) {
			for (i = 0; i < DMAXX; i++) {
				dgrid[i][j].pdungeon = dgrid[i][j].dungeon;
			}
		}
		DRLG_LoadDiabQuads(FALSE);
	}
}

static void DRLG_L4Pass3()
{
	int i, j, xx, yy;
	long v1, v2, v3, v4, lv;
	WORD *MegaTiles;

	lv = 30 - 1;

	MegaTiles = (WORD *)&pMegaTiles[lv * 8];
	v1 = SDL_SwapLE16(*(MegaTiles + 0)) + 1;
	v2 = SDL_SwapLE16(*(MegaTiles + 1)) + 1;
	v3 = SDL_SwapLE16(*(MegaTiles + 2)) + 1;
	v4 = SDL_SwapLE16(*(MegaTiles + 3)) + 1;

	for (j = 0; j < MAXDUNY; j += 2) {
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
			if (lv >= 0) {
				MegaTiles = (WORD *)&pMegaTiles[lv * 8];
				v1 = SDL_SwapLE16(*(MegaTiles + 0)) + 1;
				v2 = SDL_SwapLE16(*(MegaTiles + 1)) + 1;
				v3 = SDL_SwapLE16(*(MegaTiles + 2)) + 1;
				v4 = SDL_SwapLE16(*(MegaTiles + 3)) + 1;
			} else {
				v1 = 0;
				v2 = 0;
				v3 = 0;
				v4 = 0;
			}
			grid[xx][yy].dPiece = v1;
			grid[xx + 1][yy].dPiece = v2;
			grid[xx][yy + 1].dPiece = v3;
			grid[xx + 1][yy + 1].dPiece = v4;
			xx += 2;
		}
		yy += 2;
	}
}

void CreateL4Dungeon(DWORD rseed, int entry)
{
	SetRndSeed(rseed);

	dminx = 16;
	dminy = 16;
	dmaxx = 96;
	dmaxy = 96;

	ViewX = 40;
	ViewY = 40;

	DRLG_InitSetPC();
	DRLG_LoadL4SP();
	DRLG_L4(entry);
	DRLG_L4Pass3();
	DRLG_FreeL4SP();
	DRLG_SetPC();
}

void LoadL4Dungeon(char *sFileName, int vx, int vy)
{
	int i, j, rw, rh;
	BYTE *pLevelMap, *lm;

	dminx = 16;
	dminy = 16;
	dmaxx = 96;
	dmaxy = 96;

	DRLG_InitTrans();
	InitL4Dungeon();
	pLevelMap = LoadFileInMem(sFileName, NULL);


	lm = pLevelMap;
	rw = *lm;
	lm += 2;
	rh = *lm;
	lm += 2;

	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*lm != 0) {
				dgrid[i][j].dungeon = *lm;
				dgrid[i][j].dflags |= 0x80;
			} else {
				dgrid[i][j].dungeon = 6;
			}
			lm += 2;
		}
	}

	ViewX = vx;
	ViewY = vy;
	DRLG_L4Pass3();
	DRLG_Init_Globals();

	SetMapMonsters(pLevelMap, 0, 0);
	SetMapObjects(pLevelMap, 0, 0);
	mem_free_dbg(pLevelMap);
}

void LoadPreL4Dungeon(char *sFileName, int vx, int vy)
{
	int i, j, rw, rh;
	BYTE *pLevelMap, *lm;

	dminx = 16;
	dminy = 16;
	dmaxx = 96;
	dmaxy = 96;

	InitL4Dungeon();

	pLevelMap = LoadFileInMem(sFileName, NULL);

	lm = pLevelMap;
	rw = *lm;
	lm += 2;
	rh = *lm;
	lm += 2;

	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*lm != 0) {
				dgrid[i][j].dungeon = *lm;
				dgrid[i][j].dflags |= DLRG_PROTECTED;
			} else {
				dgrid[i][j].dungeon = 6;
			}
			lm += 2;
		}
	}
	mem_free_dbg(pLevelMap);
}
#endif

DEVILUTION_END_NAMESPACE
