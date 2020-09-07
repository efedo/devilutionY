/**
 * @file drlg_l3.cpp
 *
 * Implementation of the caves level generation algorithms.
 */
#ifndef SPAWN

#include <algorithm>

#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

BOOLEAN lavapool;
int abyssx;
int lockoutcnt;
BOOLEAN lockout[DMAXX][DMAXY];

const BYTE L3ConvTbl[16] = { 8, 11, 3, 10, 1, 9, 12, 12, 6, 13, 4, 13, 2, 14, 5, 7 };
const BYTE L3UP[20] = { 3, 3, 8, 8, 0, 10, 10, 0, 7, 7, 0, 51, 50, 0, 48, 49, 0, 0, 0, 0 };
const BYTE L3DOWN[20] = { 3, 3, 8, 9, 7, 8, 9, 7, 0, 0, 0, 0, 47, 0, 0, 46, 0, 0, 0, 0 };
const BYTE L3HOLDWARP[20] = { 3, 3, 8, 8, 0, 10, 10, 0, 7, 7, 0, 125, 125, 0, 125, 125, 0, 0, 0, 0 };
const BYTE L3TITE1[34] = { 4, 4, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 0, 0, 0, 0, 57, 58, 0, 0, 56, 55, 0, 0, 0, 0, 0 };
const BYTE L3TITE2[34] = { 4, 4, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 0, 0, 0, 0, 61, 62, 0, 0, 60, 59, 0, 0, 0, 0, 0 };
const BYTE L3TITE3[34] = { 4, 4, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 0, 0, 0, 0, 65, 66, 0, 0, 64, 63, 0, 0, 0, 0, 0 };
const BYTE L3TITE6[42] = { 5, 4, 7, 7, 7, 7, 7, 7, 7, 7, 0, 7, 7, 7, 7, 0, 7, 7, 7, 7, 7, 7, 0, 0, 0, 0, 0, 0, 77, 78, 0, 0, 0, 76, 74, 75, 0, 0, 0, 0, 0, 0 };
const BYTE L3TITE7[42] = { 4, 5, 7, 7, 7, 7, 7, 7, 0, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 0, 0, 0, 0, 83, 0, 0, 0, 82, 80, 0, 0, 81, 79, 0, 0, 0, 0, 0 };
const BYTE L3TITE8[20] = { 3, 3, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 0, 0, 0, 52, 0, 0, 0, 0 };
const BYTE L3TITE9[20] = { 3, 3, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 0, 0, 0, 53, 0, 0, 0, 0 };
const BYTE L3TITE10[20] = { 3, 3, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 0, 0, 0, 54, 0, 0, 0, 0 };
const BYTE L3TITE11[20] = { 3, 3, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 0, 0, 0, 67, 0, 0, 0, 0 };
const BYTE L3TITE12[6] = { 2, 1, 9, 7, 68, 0 };
const BYTE L3TITE13[6] = { 1, 2, 10, 7, 69, 0 };
const BYTE L3CREV1[6] = { 2, 1, 8, 7, 84, 85 };
const BYTE L3CREV2[6] = { 2, 1, 8, 11, 86, 87 };
const BYTE L3CREV3[6] = { 1, 2, 8, 10, 89, 88 };
const BYTE L3CREV4[6] = { 2, 1, 8, 7, 90, 91 };
const BYTE L3CREV5[6] = { 1, 2, 8, 11, 92, 93 };
const BYTE L3CREV6[6] = { 1, 2, 8, 10, 95, 94 };
const BYTE L3CREV7[6] = { 2, 1, 8, 7, 96, 101 };
const BYTE L3CREV8[6] = { 1, 2, 2, 8, 102, 97 };
const BYTE L3CREV9[6] = { 2, 1, 3, 8, 103, 98 };
const BYTE L3CREV10[6] = { 2, 1, 4, 8, 104, 99 };
const BYTE L3CREV11[6] = { 1, 2, 6, 8, 105, 100 };
const BYTE L3ISLE1[14] = { 2, 3, 5, 14, 4, 9, 13, 12, 7, 7, 7, 7, 7, 7 };
const BYTE L3ISLE2[14] = { 3, 2, 5, 2, 14, 13, 10, 12, 7, 7, 7, 7, 7, 7 };
const BYTE L3ISLE3[14] = { 2, 3, 5, 14, 4, 9, 13, 12, 29, 30, 25, 28, 31, 32 };
const BYTE L3ISLE4[14] = { 3, 2, 5, 2, 14, 13, 10, 12, 29, 26, 30, 31, 27, 32 };
const BYTE L3ISLE5[10] = { 2, 2, 5, 14, 13, 12, 7, 7, 7, 7 };
const BYTE L3XTRA1[4] = { 1, 1, 7, 106 };
const BYTE L3XTRA2[4] = { 1, 1, 7, 107 };
const BYTE L3XTRA3[4] = { 1, 1, 7, 108 };
const BYTE L3XTRA4[4] = { 1, 1, 9, 109 };
const BYTE L3XTRA5[4] = { 1, 1, 10, 110 };
const BYTE L3ANVIL[244] = {
	11, 11, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 29, 26, 26, 26,
	26, 26, 30, 0, 0, 0, 29, 34, 33, 33,
	37, 36, 33, 35, 30, 0, 0, 25, 33, 37,
	27, 32, 31, 36, 33, 28, 0, 0, 25, 37,
	32, 7, 7, 7, 31, 27, 32, 0, 0, 25,
	28, 7, 7, 7, 7, 2, 2, 2, 0, 0,
	25, 35, 30, 7, 7, 7, 29, 26, 30, 0,
	0, 25, 33, 35, 26, 30, 29, 34, 33, 28,
	0, 0, 31, 36, 33, 33, 35, 34, 33, 37,
	32, 0, 0, 0, 31, 27, 27, 27, 27, 27,
	32, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0
};

static void InitL3Dungeon()
{
	int i, j;

	memset(dungeon, 0, sizeof(dungeon));

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			dgrid[i][j].dungeon = 0;
			dgrid[i][j].dflags = 0;
		}
	}
}

static BOOL DRLG_L3FillRoom(int x1, int y1, int x2, int y2)
{
	int i, j, v;

	if (x1 <= 1 || x2 >= 34 || y1 <= 1 || y2 >= 38) {
		return FALSE;
	}

	v = 0;
	for (j = y1; j <= y2; j++) {
		for (i = x1; i <= x2; i++) {
			v += dgrid[i][j].dungeon;
		}
	}

	if (v != 0) {
		return FALSE;
	}

	for (j = y1 + 1; j < y2; j++) {
		for (i = x1 + 1; i < x2; i++) {
			dgrid[i][j].dungeon = 1;
		}
	}
	for (j = y1; j <= y2; j++) {
		if (random_(0, 2) != 0) {
			dgrid[x1][j].dungeon = 1;
		}
		if (random_(0, 2) != 0) {
			dgrid[x2][j].dungeon = 1;
		}
	}
	for (i = x1; i <= x2; i++) {
		if (random_(0, 2) != 0) {
			dgrid[i][y1].dungeon = 1;
		}
		if (random_(0, 2) != 0) {
			dgrid[i][y2].dungeon = 1;
		}
	}

	return TRUE;
}

static void DRLG_L3CreateBlock(int x, int y, int obs, int dir)
{
	int blksizex, blksizey, x1, y1, x2, y2;
	BOOL contflag;

	blksizex = random_(0, 2) + 3;
	blksizey = random_(0, 2) + 3;

	if (dir == 0) {
		y2 = y - 1;
		y1 = y2 - blksizey;
		if (blksizex < obs) {
			x1 = random_(0, blksizex) + x;
		}
		if (blksizex == obs) {
			x1 = x;
		}
		if (blksizex > obs) {
			x1 = x - random_(0, blksizex);
		}
		x2 = blksizex + x1;
	}
	if (dir == 3) {
		x2 = x - 1;
		x1 = x2 - blksizex;
		if (blksizey < obs) {
			y1 = random_(0, blksizey) + y;
		}
		if (blksizey == obs) {
			y1 = y;
		}
		if (blksizey > obs) {
			y1 = y - random_(0, blksizey);
		}
		y2 = y1 + blksizey;
	}
	if (dir == 2) {
		y1 = y + 1;
		y2 = y1 + blksizey;
		if (blksizex < obs) {
			x1 = random_(0, blksizex) + x;
		}
		if (blksizex == obs) {
			x1 = x;
		}
		if (blksizex > obs) {
			x1 = x - random_(0, blksizex);
		}
		x2 = blksizex + x1;
	}
	if (dir == 1) {
		x1 = x + 1;
		x2 = x1 + blksizex;
		if (blksizey < obs) {
			y1 = random_(0, blksizey) + y;
		}
		if (blksizey == obs) {
			y1 = y;
		}
		if (blksizey > obs) {
			y1 = y - random_(0, blksizey);
		}
		y2 = y1 + blksizey;
	}

	if (DRLG_L3FillRoom(x1, y1, x2, y2) == TRUE) {
		contflag = random_(0, 4);
		if (contflag && dir != 2) {
			DRLG_L3CreateBlock(x1, y1, blksizey, 0);
		}
		if (contflag && dir != 3) {
			DRLG_L3CreateBlock(x2, y1, blksizex, 1);
		}
		if (contflag && dir != 0) {
			DRLG_L3CreateBlock(x1, y2, blksizey, 2);
		}
		if (contflag && dir != 1) {
			DRLG_L3CreateBlock(x1, y1, blksizex, 3);
		}
	}
}

static void DRLG_L3FloorArea(int x1, int y1, int x2, int y2)
{
	int i, j;

	for (j = y1; j <= y2; j++) {
		for (i = x1; i <= x2; i++) {
			dgrid[i][j].dungeon = 1;
		}
	}
}

static void DRLG_L3FillDiags()
{
	int i, j, v;

	for (j = 0; j < DMAXY - 1; j++) {
		for (i = 0; i < DMAXX - 1; i++) {
			v = dgrid[i + 1][j + 1].dungeon + 2 * dgrid[i][j + 1].dungeon + 4 * dgrid[i + 1][j].dungeon + 8 * dgrid[i][j].dungeon;
			if (v == 6) {
				if (random_(0, 2) == 0) {
					dgrid[i][j].dungeon = 1;
				} else {
					dgrid[i + 1][j + 1].dungeon = 1;
				}
			}
			if (v == 9) {
				if (random_(0, 2) == 0) {
					dgrid[i + 1][j].dungeon = 1;
				} else {
					dgrid[i][j + 1].dungeon = 1;
				}
			}
		}
	}
}

static void DRLG_L3FillSingles()
{
	int i, j;

	for (j = 1; j < DMAXY - 1; j++) {
		for (i = 1; i < DMAXX - 1; i++) {
			if (dgrid[i][j].dungeon == 0
			    && dgrid[i][j - 1].dungeon + dgrid[i - 1][j - 1].dungeon + dgrid[i + 1][j - 1].dungeon == 3
			    && dgrid[i + 1][j].dungeon + dgrid[i - 1][j].dungeon == 2
			    && dgrid[i][j + 1].dungeon + dgrid[i - 1][j + 1].dungeon + dgrid[i + 1][j + 1].dungeon == 3) {
				dgrid[i][j].dungeon = 1;
			}
		}
	}
}

static void DRLG_L3FillStraights()
{
	int i, j, xc, xs, yc, ys, k, rv;

	for (j = 0; j < DMAXY - 1; j++) {
		xs = 0;
		for (i = 0; i < 37; i++) {
			if (dgrid[i][j].dungeon == 0 && dgrid[i][j + 1].dungeon == 1) {
				if (xs == 0) {
					xc = i;
				}
				xs++;
			} else {
				if (xs > 3 && random_(0, 2) != 0) {
					for (k = xc; k < i; k++) {
						rv = random_(0, 2);
						dgrid[k][j].dungeon = rv;
					}
				}
				xs = 0;
			}
		}
	}
	for (j = 0; j < DMAXY - 1; j++) {
		xs = 0;
		for (i = 0; i < 37; i++) {
			if (dgrid[i][j].dungeon == 1 && dgrid[i][j + 1].dungeon == 0) {
				if (xs == 0) {
					xc = i;
				}
				xs++;
			} else {
				if (xs > 3 && random_(0, 2) != 0) {
					for (k = xc; k < i; k++) {
						rv = random_(0, 2);
						dgrid[k][j + 1].dungeon = rv;
					}
				}
				xs = 0;
			}
		}
	}
	for (i = 0; i < DMAXX - 1; i++) {
		ys = 0;
		for (j = 0; j < 37; j++) {
			if (dgrid[i][j].dungeon == 0 && dgrid[i + 1][j].dungeon == 1) {
				if (ys == 0) {
					yc = j;
				}
				ys++;
			} else {
				if (ys > 3 && random_(0, 2) != 0) {
					for (k = yc; k < j; k++) {
						rv = random_(0, 2);
						dgrid[i][k].dungeon = rv;
					}
				}
				ys = 0;
			}
		}
	}
	for (i = 0; i < DMAXX - 1; i++) {
		ys = 0;
		for (j = 0; j < 37; j++) {
			if (dgrid[i][j].dungeon == 1 && dgrid[i + 1][j].dungeon == 0) {
				if (ys == 0) {
					yc = j;
				}
				ys++;
			} else {
				if (ys > 3 && random_(0, 2) != 0) {
					for (k = yc; k < j; k++) {
						rv = random_(0, 2);
						dgrid[i + 1][k].dungeon = rv;
					}
				}
				ys = 0;
			}
		}
	}
}

static void DRLG_L3Edges()
{
	int i, j;

	for (j = 0; j < DMAXY; j++) {
		dgrid[DMAXX - 1][j].dungeon = 0;
	}
	for (i = 0; i < DMAXX; i++) {
		dgrid[i][DMAXY - 1].dungeon = 0;
	}
}

static int DRLG_L3GetFloorArea()
{
	int i, j, gfa;

	gfa = 0;

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			gfa += dgrid[i][j].dungeon;
		}
	}

	return gfa;
}

static void DRLG_L3MakeMegas()
{
	int i, j, v, rv;

	for (j = 0; j < DMAXY - 1; j++) {
		for (i = 0; i < DMAXX - 1; i++) {
			v = dgrid[i + 1][j + 1].dungeon + 2 * dgrid[i][j + 1].dungeon + 4 * dgrid[i + 1][j].dungeon + 8 * dgrid[i][j].dungeon;
			if (v == 6) {
				rv = random_(0, 2);
				if (rv == 0) {
					v = 12;
				} else {
					v = 5;
				}
			}
			if (v == 9) {
				rv = random_(0, 2);
				if (rv == 0) {
					v = 13;
				} else {
					v = 14;
				}
			}
			dgrid[i][j].dungeon = L3ConvTbl[v];
		}
		dgrid[DMAXX - 1][j].dungeon = 8;
	}
	for (i = 0; i < DMAXX; i++) {
		dgrid[i][DMAXY - 1].dungeon = 8;
	}
}

static void DRLG_L3River()
{
	int rx, ry, px, py, dir, pdir, nodir, nodir2, dircheck;
	int river[3][100];
	int rivercnt, riveramt;
	int i, trys, found, bridge, lpcnt;
	BOOL bail;

	rivercnt = 0;
	bail = FALSE;
	trys = 0;
	/// BUGFIX: pdir is uninitialized, add code `pdir = -1;`(fixed)
	pdir = -1;

	while (trys < 200 && rivercnt < 4) {
		bail = FALSE;
		while (!bail && trys < 200) {
			trys++;
			rx = 0;
			ry = 0;
			i = 0;
			// BUGFIX: Replace with `(ry >= DMAXY || dgrid[rx][ry].dungeon < 25 || dgrid[rx][ry].dungeon > 28) && i < 100` (fixed)
			while ((ry >= DMAXY || dgrid[rx][ry].dungeon < 25 || dgrid[rx][ry].dungeon > 28) && i < 100) {
				rx = random_(0, DMAXX);
				ry = random_(0, DMAXY);
				i++;
				// BUGFIX: Move `ry < DMAXY` check before dungeon checks (fixed)
				while (ry < DMAXY && (dgrid[rx][ry].dungeon < 25 || dgrid[rx][ry].dungeon > 28)) {
					rx++;
					if (rx >= DMAXX) {
						rx = 0;
						ry++;
					}
				}
			}
			// BUGFIX: Continue if `ry >= DMAXY` (fixed)
			if (ry >= DMAXY)
				continue;
			if (i >= 100) {
				return;
			}
			switch (dgrid[rx][ry].dungeon) {
			case 25:
				dir = 3;
				nodir = 2;
				river[2][0] = 40;
				break;
			case 26:
				dir = 0;
				nodir = 1;
				river[2][0] = 38;
				break;
			case 27:
				dir = 1;
				nodir = 0;
				river[2][0] = 41;
				break;
			case 28:
				dir = 2;
				nodir = 3;
				river[2][0] = 39;
				break;
			}
			river[0][0] = rx;
			river[1][0] = ry;
			riveramt = 1;
			nodir2 = 4;
			dircheck = 0;
			while (dircheck < 4 && riveramt < 100) {
				px = rx;
				py = ry;
				if (dircheck == 0) {
					dir = random_(0, 4);
				} else {
					dir = (dir + 1) & 3;
				}
				dircheck++;
				while (dir == nodir || dir == nodir2) {
					dir = (dir + 1) & 3;
					dircheck++;
				}
				if (dir == 0 && ry > 0) {
					ry--;
				}
				if (dir == 1 && ry < DMAXY) {
					ry++;
				}
				if (dir == 2 && rx < DMAXX) {
					rx++;
				}
				if (dir == 3 && rx > 0) {
					rx--;
				}
				if (dgrid[rx][ry].dungeon == 7) {
					dircheck = 0;
					if (dir < 2) {
						river[2][riveramt] = (BYTE)random_(0, 2) + 17;
					}
					if (dir > 1) {
						river[2][riveramt] = (BYTE)random_(0, 2) + 15;
					}
					river[0][riveramt] = rx;
					river[1][riveramt] = ry;
					riveramt++;
					if (dir == 0 && pdir == 2 || dir == 3 && pdir == 1) {
						if (riveramt > 2) {
							river[2][riveramt - 2] = 22;
						}
						if (dir == 0) {
							nodir2 = 1;
						} else {
							nodir2 = 2;
						}
					}
					if (dir == 0 && pdir == 3 || dir == 2 && pdir == 1) {
						if (riveramt > 2) {
							river[2][riveramt - 2] = 21;
						}
						if (dir == 0) {
							nodir2 = 1;
						} else {
							nodir2 = 3;
						}
					}
					if (dir == 1 && pdir == 2 || dir == 3 && pdir == 0) {
						if (riveramt > 2) {
							river[2][riveramt - 2] = 20;
						}
						if (dir == 1) {
							nodir2 = 0;
						} else {
							nodir2 = 2;
						}
					}
					if (dir == 1 && pdir == 3 || dir == 2 && pdir == 0) {
						if (riveramt > 2) {
							river[2][riveramt - 2] = 19;
						}
						if (dir == 1) {
							nodir2 = 0;
						} else {
							nodir2 = 3;
						}
					}
					pdir = dir;
				} else {
					rx = px;
					ry = py;
				}
			}
			// BUGFIX: Check `ry >= 2` (fixed)
			if (dir == 0 && ry >= 2 && dgrid[rx][ry - 1].dungeon == 10 && dgrid[rx][ry - 2].dungeon == 8) {
				river[0][riveramt] = rx;
				river[1][riveramt] = ry - 1;
				river[2][riveramt] = 24;
				if (pdir == 2) {
					river[2][riveramt - 1] = 22;
				}
				if (pdir == 3) {
					river[2][riveramt - 1] = 21;
				}
				bail = TRUE;
			}
			// BUGFIX: Check `ry + 2 < DMAXY` (fixed)
			if (dir == 1 && ry + 2 < DMAXY && dgrid[rx][ry + 1].dungeon == 2 && dgrid[rx][ry + 2].dungeon == 8) {
				river[0][riveramt] = rx;
				river[1][riveramt] = ry + 1;
				river[2][riveramt] = 42;
				if (pdir == 2) {
					river[2][riveramt - 1] = 20;
				}
				if (pdir == 3) {
					river[2][riveramt - 1] = 19;
				}
				bail = TRUE;
			}
			// BUGFIX: Check `rx + 2 < DMAXX` (fixed)
			if (dir == 2 && rx + 2 < DMAXX && dgrid[rx + 1][ry].dungeon == 4 && dgrid[rx + 2][ry].dungeon == 8) {
				river[0][riveramt] = rx + 1;
				river[1][riveramt] = ry;
				river[2][riveramt] = 43;
				if (pdir == 0) {
					river[2][riveramt - 1] = 19;
				}
				if (pdir == 1) {
					river[2][riveramt - 1] = 21;
				}
				bail = TRUE;
			}
			// BUGFIX: Check `rx >= 2` (fixed)
			if (dir == 3 && rx >= 2 && dgrid[rx - 1][ry].dungeon == 9 && dgrid[rx - 2][ry].dungeon == 8) {
				river[0][riveramt] = rx - 1;
				river[1][riveramt] = ry;
				river[2][riveramt] = 23;
				if (pdir == 0) {
					river[2][riveramt - 1] = 20;
				}
				if (pdir == 1) {
					river[2][riveramt - 1] = 22;
				}
				bail = TRUE;
			}
		}
		if (bail == TRUE && riveramt < 7) {
			bail = FALSE;
		}
		if (bail == TRUE) {
			found = 0;
			lpcnt = 0;
			while (found == 0 && lpcnt < 30) {
				lpcnt++;
				bridge = random_(0, riveramt);
				if ((river[2][bridge] == 15 || river[2][bridge] == 16)
				    && dgrid[river[0][bridge].dungeon][river[1][bridge] - 1] == 7
				    && dgrid[river[0][bridge].dungeon][river[1][bridge] + 1] == 7) {
					found = 1;
				}
				if ((river[2][bridge] == 17 || river[2][bridge] == 18)
				    && dgrid[river[0][bridge].dungeon - 1][river[1][bridge]] == 7
				    && dgrid[river[0][bridge].dungeon + 1][river[1][bridge]] == 7) {
					found = 2;
				}
				for (i = 0; i < riveramt && found != 0; i++) {
					if (found == 1
					    && (river[1][bridge] - 1 == river[1][i] || river[1][bridge] + 1 == river[1][i])
					    && river[0][bridge] == river[0][i]) {
						found = 0;
					}
					if (found == 2
					    && (river[0][bridge] - 1 == river[0][i] || river[0][bridge] + 1 == river[0][i])
					    && river[1][bridge] == river[1][i]) {
						found = 0;
					}
				}
			}
			if (found != 0) {
				if (found == 1) {
					river[2][bridge] = 44;
				} else {
					river[2][bridge] = 45;
				}
				rivercnt++;
				for (bridge = 0; bridge <= riveramt; bridge++) {
					dgrid[river[0][bridge].dungeon][river[1][bridge]] = river[2][bridge];
				}
			} else {
				bail = FALSE;
			}
		}
	}
}

static BOOL DRLG_L3Spawn(int x, int y, int *totarea);

static BOOL DRLG_L3SpawnEdge(int x, int y, int *totarea)
{
	BYTE i;
	static BYTE spawntable[15] = { 0, 0x0A, 0x43, 0x05, 0x2C, 0x06, 0x09, 0, 0, 0x1C, 0x83, 0x06, 0x09, 0x0A, 0x05 };

	if (*totarea > 40) {
		return TRUE;
	}
	if (x < 0 || y < 0 || x >= DMAXX || y >= DMAXY) {
		return TRUE;
	}
	if (dgrid[x][y].dungeon & 0x80) {
		return FALSE;
	}
	if (dgrid[x][y].dungeon > 15) {
		return TRUE;
	}

	i = dgrid[x][y].dungeon;
	dgrid[x][y].dungeon |= 0x80;
	*totarea += 1;

	if (spawntable[i] & 8 && DRLG_L3SpawnEdge(x, y - 1, totarea) == TRUE) {
		return TRUE;
	}
	if (spawntable[i] & 4 && DRLG_L3SpawnEdge(x, y + 1, totarea) == TRUE) {
		return TRUE;
	}
	if (spawntable[i] & 2 && DRLG_L3SpawnEdge(x + 1, y, totarea) == TRUE) {
		return TRUE;
	}
	if (spawntable[i] & 1 && DRLG_L3SpawnEdge(x - 1, y, totarea) == TRUE) {
		return TRUE;
	}
	if (spawntable[i] & 0x80 && DRLG_L3Spawn(x, y - 1, totarea) == TRUE) {
		return TRUE;
	}
	if (spawntable[i] & 0x40 && DRLG_L3Spawn(x, y + 1, totarea) == TRUE) {
		return TRUE;
	}
	if (spawntable[i] & 0x20 && DRLG_L3Spawn(x + 1, y, totarea) == TRUE) {
		return TRUE;
	}
	if (spawntable[i] & 0x10 && DRLG_L3Spawn(x - 1, y, totarea) == TRUE) {
		return TRUE;
	}

	return FALSE;
}

static BOOL DRLG_L3Spawn(int x, int y, int *totarea)
{
	BYTE i;
	static BYTE spawntable[15] = { 0, 0x0A, 0x03, 0x05, 0x0C, 0x06, 0x09, 0, 0, 0x012, 0x03, 0x06, 0x09, 0x0A, 0x05 };

	if (*totarea > 40) {
		return TRUE;
	}
	if (x < 0 || y < 0 || x >= DMAXX || y >= DMAXY) {
		return TRUE;
	}
	if (dgrid[x][y].dungeon & 0x80) {
		return FALSE;
	}
	if (dgrid[x][y].dungeon > 15) {
		return TRUE;
	}

	i = dgrid[x][y].dungeon;
	dgrid[x][y].dungeon |= 0x80;
	*totarea += 1;

	if (i != 8) {
		if (spawntable[i] & 8 && DRLG_L3SpawnEdge(x, y - 1, totarea) == TRUE) {
			return TRUE;
		}
		if (spawntable[i] & 4 && DRLG_L3SpawnEdge(x, y + 1, totarea) == TRUE) {
			return TRUE;
		}
		if (spawntable[i] & 2 && DRLG_L3SpawnEdge(x + 1, y, totarea) == TRUE) {
			return TRUE;
		}
		if (spawntable[i] & 1 && DRLG_L3SpawnEdge(x - 1, y, totarea) == TRUE) {
			return TRUE;
		}
	} else {
		if (DRLG_L3Spawn(x + 1, y, totarea) == TRUE) {
			return TRUE;
		}
		if (DRLG_L3Spawn(x - 1, y, totarea) == TRUE) {
			return TRUE;
		}
		if (DRLG_L3Spawn(x, y + 1, totarea) == TRUE) {
			return TRUE;
		}
		if (DRLG_L3Spawn(x, y - 1, totarea) == TRUE) {
			return TRUE;
		}
	}

	return FALSE;
}

/**
 * Flood fills dirt and wall tiles looking for
 * an area of at most 40 tiles and disconnected from the map edge.
 * If it finds one, converts it to lava tiles and sets lavapool to TRUE.
 */
static void DRLG_L3Pool()
{
	int i, j, dunx, duny, totarea, poolchance;
	BOOL found;
	BYTE k;
	static BYTE poolsub[15] = { 0, 35, 26, 36, 25, 29, 34, 7, 33, 28, 27, 37, 32, 31, 30 };

	for (duny = 0; duny < DMAXY; duny++) {
		for (dunx = 0; dunx < DMAXY; dunx++) {
			if (dgrid[dunx][duny].dungeon != 8) {
				continue;
			}
			dgrid[dunx][duny].dungeon |= 0x80;
			totarea = 1;
			if (dunx + 1 < DMAXX) {
				found = DRLG_L3Spawn(dunx + 1, duny, &totarea);
			} else {
				found = TRUE;
			}
			if (dunx - 1 > 0 && !found) {
				found = DRLG_L3Spawn(dunx - 1, duny, &totarea);
			} else {
				found = TRUE;
			}
			if (duny + 1 < DMAXY && !found) {
				found = DRLG_L3Spawn(dunx, duny + 1, &totarea);
			} else {
				found = TRUE;
			}
			if (duny - 1 > 0 && !found) {
				found = DRLG_L3Spawn(dunx, duny - 1, &totarea);
			} else {
				found = TRUE;
			}
			poolchance = random_(0, 100);
			for (j = std::max(duny - totarea, 0); j < std::min(duny + totarea, DMAXY); j++) {
				for (i = std::max(dunx - totarea, 0); i < std::min(dunx + totarea, DMAXX); i++) {
					// BUGFIX: In the following swap the order to first do the
					// index checks and only then access dgrid[i][j].dungeon (fixed)
					if (dgrid[i][j].dungeon & 0x80) {
						dgrid[i][j].dungeon &= ~0x80;
						if (totarea > 4 && poolchance < 25 && !found) {
							k = poolsub[dgrid[i][j].dungeon];
							if (k != 0 && k <= 37) {
								dgrid[i][j].dungeon = k;
							}
							lavapool = TRUE;
						}
					}
				}
			}
		}
	}
}

static void DRLG_L3PoolFix()
{
	int dunx, duny;

	for (duny = 1; duny < DMAXY - 1; duny++) {     // BUGFIX: Change '0' to '1' and 'DMAXY' to 'DMAXY - 1' (fixed)
		for (dunx = 1; dunx < DMAXX - 1; dunx++) { // BUGFIX: Change '0' to '1' and 'DMAXX' to 'DMAXX - 1' (fixed)
			if (dgrid[dunx][duny].dungeon == 8) {
				if (dgrid[dunx - 1][duny - 1].dungeon >= 25 && dgrid[dunx - 1][duny - 1].dungeon <= 41
				    && dgrid[dunx - 1][duny].dungeon >= 25 && dgrid[dunx - 1][duny].dungeon <= 41
				    && dgrid[dunx - 1][duny + 1].dungeon >= 25 && dgrid[dunx - 1][duny + 1].dungeon <= 41
				    && dgrid[dunx][duny - 1].dungeon >= 25 && dgrid[dunx][duny - 1].dungeon <= 41
				    && dgrid[dunx][duny + 1].dungeon >= 25 && dgrid[dunx][duny + 1].dungeon <= 41
				    && dgrid[dunx + 1][duny - 1].dungeon >= 25 && dgrid[dunx + 1][duny - 1].dungeon <= 41
				    && dgrid[dunx + 1][duny].dungeon >= 25 && dgrid[dunx + 1][duny].dungeon <= 41
				    && dgrid[dunx + 1][duny + 1].dungeon >= 25 && dgrid[dunx + 1][duny + 1].dungeon <= 41) {
					dgrid[dunx][duny].dungeon = 33;
				}
			}
		}
	}
}

static BOOL DRLG_L3PlaceMiniSet(const BYTE *miniset, int tmin, int tmax, int cx, int cy, BOOL setview, int ldir)
{
	int sx, sy, sw, sh, xx, yy, i, ii, numt, trys;
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
		trys = 0;
		while (!found && trys < 200) {
			trys++;
			found = TRUE;
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
		if (trys >= 200) {
			return TRUE;
		}
		ii = sw * sh + 2;
		for (yy = 0; yy < sh; yy++) {
			for (xx = 0; xx < sw; xx++) {
				if (miniset[ii] != 0) {
					dgrid[xx + sx][yy + sy].dungeon = miniset[ii];
				}
				ii++;
			}
		}
	}

	if (setview == TRUE) {
		ViewX = 2 * sx + 17;
		ViewY = 2 * sy + 19;
	}
	if (ldir == 0) {
		LvlViewX = 2 * sx + 17;
		LvlViewY = 2 * sy + 19;
	}

	return FALSE;
}

static void DRLG_L3PlaceRndSet(const BYTE *miniset, int rndper)
{
	int sx, sy, sw, sh, xx, yy, ii, kk;
	BOOL found;

	sw = miniset[0];
	sh = miniset[1];

	for (sy = 0; sy < DMAXX - sh; sy++) {
		for (sx = 0; sx < DMAXY - sw; sx++) {
			found = TRUE;
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
			kk = sw * sh + 2;
			if (miniset[kk] >= 84 && miniset[kk] <= 100 && found == TRUE) {
				// BUGFIX: accesses to dungeon can go out of bounds (fixed)
				// BUGFIX: Comparisons vs 100 should use same tile as comparisons vs 84.
				if (sx - 1 >= 0 && dgrid[sx - 1][sy].dungeon >= 84 && dgrid[sx - 1][sy].dungeon <= 100) {
					found = FALSE;
				}
				if (sx + 1 < 40 && sx - 1 >= 0 && dgrid[sx + 1][sy].dungeon >= 84 && dgrid[sx - 1][sy].dungeon <= 100) {
					found = FALSE;
				}
				if (sy + 1 < 40 && sx - 1 >= 0 && dgrid[sx][sy + 1].dungeon >= 84 && dgrid[sx - 1][sy].dungeon <= 100) {
					found = FALSE;
				}
				if (sy - 1 >= 0 && sx - 1 >= 0 && dgrid[sx][sy - 1].dungeon >= 84 && dgrid[sx - 1][sy].dungeon <= 100) {
					found = FALSE;
				}
			}
			if (found == TRUE && random_(0, 100) < rndper) {
				for (yy = 0; yy < sh; yy++) {
					for (xx = 0; xx < sw; xx++) {
						if (miniset[kk] != 0) {
							dgrid[xx + sx][yy + sy].dungeon = miniset[kk];
						}
						kk++;
					}
				}
			}
		}
	}
}

static BOOL WoodVertU(int i, int y)
{
	if ((dgrid[i + 1][y].dungeon > 152 || dgrid[i + 1][y].dungeon < 130)
	    && (dgrid[i - 1][y].dungeon > 152 || dgrid[i - 1][y].dungeon < 130)) {
		if (dgrid[i][y].dungeon == 7) {
			return TRUE;
		}
		if (dgrid[i][y].dungeon == 10) {
			return TRUE;
		}
		if (dgrid[i][y].dungeon == 126) {
			return TRUE;
		}
		if (dgrid[i][y].dungeon == 129) {
			return TRUE;
		}
		if (dgrid[i][y].dungeon == 134) {
			return TRUE;
		}
		if (dgrid[i][y].dungeon == 136) {
			return TRUE;
		}
	}

	return FALSE;
}

static BOOL WoodVertD(int i, int y)
{
	if ((dgrid[i + 1][y].dungeon > 152 || dgrid[i + 1][y].dungeon < 130)
	    && (dgrid[i - 1][y].dungeon > 152 || dgrid[i - 1][y].dungeon < 130)) {
		if (dgrid[i][y].dungeon == 7) {
			return TRUE;
		}
		if (dgrid[i][y].dungeon == 2) {
			return TRUE;
		}
		if (dgrid[i][y].dungeon == 134) {
			return TRUE;
		}
		if (dgrid[i][y].dungeon == 136) {
			return TRUE;
		}
	}

	return FALSE;
}

static BOOL WoodHorizL(int x, int j)
{
	if ((dgrid[x][j + 1].dungeon > 152 || dgrid[x][j + 1].dungeon < 130)
	    && (dgrid[x][j - 1].dungeon > 152 || dgrid[x][j - 1].dungeon < 130)) {
		if (dgrid[x][j].dungeon == 7) {
			return TRUE;
		}
		if (dgrid[x][j].dungeon == 9) {
			return TRUE;
		}
		if (dgrid[x][j].dungeon == 121) {
			return TRUE;
		}
		if (dgrid[x][j].dungeon == 124) {
			return TRUE;
		}
		if (dgrid[x][j].dungeon == 135) {
			return TRUE;
		}
		if (dgrid[x][j].dungeon == 137) {
			return TRUE;
		}
	}

	return FALSE;
}

static BOOL WoodHorizR(int x, int j)
{
	if ((dgrid[x][j + 1].dungeon > 152 || dgrid[x][j + 1].dungeon < 130)
	    && (dgrid[x][j - 1].dungeon > 152 || dgrid[x][j - 1].dungeon < 130)) {
		if (dgrid[x][j].dungeon == 7) {
			return TRUE;
		}
		if (dgrid[x][j].dungeon == 4) {
			return TRUE;
		}
		if (dgrid[x][j].dungeon == 135) {
			return TRUE;
		}
		if (dgrid[x][j].dungeon == 137) {
			return TRUE;
		}
	}

	return FALSE;
}

void AddFenceDoors()
{
	int i, j;

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dgrid[i][j].dungeon == 7) {
				if (dgrid[i - 1][j].dungeon <= 152 && dgrid[i - 1][j].dungeon >= 130
				    && dgrid[i + 1][j].dungeon <= 152 && dgrid[i + 1][j].dungeon >= 130) {
					dgrid[i][j].dungeon = 146;
					continue;
				}
			}
			if (dgrid[i][j].dungeon == 7) {
				if (dgrid[i][j - 1].dungeon <= 152 && dgrid[i][j - 1].dungeon >= 130
				    && dgrid[i][j + 1].dungeon <= 152 && dgrid[i][j + 1].dungeon >= 130) {
					dgrid[i][j].dungeon = 147;
					continue;
				}
			}
		}
	}
}

void FenceDoorFix()
{
	int i, j;

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dgrid[i][j].dungeon == 146) {
				if (dgrid[i + 1][j].dungeon > 152 || dgrid[i + 1][j].dungeon < 130
				    || dgrid[i - 1][j].dungeon > 152 || dgrid[i - 1][j].dungeon < 130) {
					dgrid[i][j].dungeon = 7;
					continue;
				}
			}
			if (dgrid[i][j].dungeon == 146) {
				if (dgrid[i + 1][j].dungeon != 130 && dgrid[i - 1][j].dungeon != 130
				    && dgrid[i + 1][j].dungeon != 132 && dgrid[i - 1][j].dungeon != 132
				    && dgrid[i + 1][j].dungeon != 133 && dgrid[i - 1][j].dungeon != 133
				    && dgrid[i + 1][j].dungeon != 134 && dgrid[i - 1][j].dungeon != 134
				    && dgrid[i + 1][j].dungeon != 136 && dgrid[i - 1][j].dungeon != 136
				    && dgrid[i + 1][j].dungeon != 138 && dgrid[i - 1][j].dungeon != 138
				    && dgrid[i + 1][j].dungeon != 140 && dgrid[i - 1][j].dungeon != 140) {
					dgrid[i][j].dungeon = 7;
					continue;
				}
			}
			if (dgrid[i][j].dungeon == 147) {
				if (dgrid[i][j + 1].dungeon > 152 || dgrid[i][j + 1].dungeon < 130
				    || dgrid[i][j - 1].dungeon > 152 || dgrid[i][j - 1].dungeon < 130) {
					dgrid[i][j].dungeon = 7;
					continue;
				}
			}
			if (dgrid[i][j].dungeon == 147) {
				if (dgrid[i][j + 1].dungeon != 131 && dgrid[i][j - 1].dungeon != 131
				    && dgrid[i][j + 1].dungeon != 132 && dgrid[i][j - 1].dungeon != 132
				    && dgrid[i][j + 1].dungeon != 133 && dgrid[i][j - 1].dungeon != 133
				    && dgrid[i][j + 1].dungeon != 135 && dgrid[i][j - 1].dungeon != 135
				    && dgrid[i][j + 1].dungeon != 137 && dgrid[i][j - 1].dungeon != 137
				    && dgrid[i][j + 1].dungeon != 138 && dgrid[i][j - 1].dungeon != 138
				    && dgrid[i][j + 1].dungeon != 139 && dgrid[i][j - 1].dungeon != 139) {
					dgrid[i][j].dungeon = 7;
					continue;
				}
			}
		}
	}
}

static void DRLG_L3Wood()
{
	int i, j, x, y, xx, yy, rt, rp, x1, y1, x2, y2;
	BOOL skip;

	for (j = 1; j < DMAXY - 1; j++) {     // BUGFIX: Change '0' to '1' (fixed)
		for (i = 1; i < DMAXX - 1; i++) { // BUGFIX: Change '0' to '1' (fixed)
			if (dgrid[i][j].dungeon == 10 && random_(0, 2) != 0) {
				x = i;
				while (dgrid[x][j].dungeon == 10) {
					x++;
				}
				x--;
				if (x - i > 0) {
					dgrid[i][j].dungeon = 127;
					for (xx = i + 1; xx < x; xx++) {
						if (random_(0, 2) != 0) {
							dgrid[xx][j].dungeon = 126;
						} else {
							dgrid[xx][j].dungeon = 129;
						}
					}
					dgrid[x][j].dungeon = 128;
				}
			}
			if (dgrid[i][j].dungeon == 9 && random_(0, 2) != 0) {
				y = j;
				while (dgrid[i][y].dungeon == 9) {
					y++;
				}
				y--;
				if (y - j > 0) {
					dgrid[i][j].dungeon = 123;
					for (yy = j + 1; yy < y; yy++) {
						if (random_(0, 2) != 0) {
							dgrid[i][yy].dungeon = 121;
						} else {
							dgrid[i][yy].dungeon = 124;
						}
					}
					dgrid[i][y].dungeon = 122;
				}
			}
			if (dgrid[i][j].dungeon == 11 && dgrid[i + 1][j].dungeon == 10 && dgrid[i][j + 1].dungeon == 9 && random_(0, 2) != 0) {
				dgrid[i][j].dungeon = 125;
				x = i + 1;
				while (dgrid[x][j].dungeon == 10) {
					x++;
				}
				x--;
				for (xx = i + 1; xx < x; xx++) {
					if (random_(0, 2) != 0) {
						dgrid[xx][j].dungeon = 126;
					} else {
						dgrid[xx][j].dungeon = 129;
					}
				}
				dgrid[x][j].dungeon = 128;
				y = j + 1;
				while (dgrid[i][y].dungeon == 9) {
					y++;
				}
				y--;
				for (yy = j + 1; yy < y; yy++) {
					if (random_(0, 2) != 0) {
						dgrid[i][yy].dungeon = 121;
					} else {
						dgrid[i][yy].dungeon = 124;
					}
				}
				dgrid[i][y].dungeon = 122;
			}
		}
	}

	for (j = 1; j < DMAXY; j++) {     // BUGFIX: Change '0' to '1' (fixed)
		for (i = 1; i < DMAXX; i++) { // BUGFIX: Change '0' to '1' (fixed)
			if (dgrid[i][j].dungeon == 7 && random_(0, 1) == 0 && SkipThemeRoom(i, j)) {
				rt = random_(0, 2);
				if (rt == 0) {
					y1 = j;
					// BUGFIX: Check `y1 >= 0` first (fixed)
					while (y1 >= 0 && WoodVertU(i, y1)) {
						y1--;
					}
					y1++;
					y2 = j;
					// BUGFIX: Check `y2 < DMAXY` first (fixed)
					while (y2 < DMAXY && WoodVertD(i, y2)) {
						y2++;
					}
					y2--;
					skip = TRUE;
					if (dgrid[i][y1].dungeon == 7) {
						skip = FALSE;
					}
					if (dgrid[i][y2].dungeon == 7) {
						skip = FALSE;
					}
					if (y2 - y1 > 1 && skip) {
						rp = random_(0, y2 - y1 - 1) + y1 + 1;
						for (y = y1; y <= y2; y++) {
							if (y == rp) {
								continue;
							}
							if (dgrid[i][y].dungeon == 7) {
								if (random_(0, 2) != 0) {
									dgrid[i][y].dungeon = 135;
								} else {
									dgrid[i][y].dungeon = 137;
								}
							}
							if (dgrid[i][y].dungeon == 10) {
								dgrid[i][y].dungeon = 131;
							}
							if (dgrid[i][y].dungeon == 126) {
								dgrid[i][y].dungeon = 133;
							}
							if (dgrid[i][y].dungeon == 129) {
								dgrid[i][y].dungeon = 133;
							}
							if (dgrid[i][y].dungeon == 2) {
								dgrid[i][y].dungeon = 139;
							}
							if (dgrid[i][y].dungeon == 134) {
								dgrid[i][y].dungeon = 138;
							}
							if (dgrid[i][y].dungeon == 136) {
								dgrid[i][y].dungeon = 138;
							}
						}
					}
				}
				if (rt == 1) {
					x1 = i;
					// BUGFIX: Check `x1 >= 0` first (fixed)
					while (x1 >= 0 && WoodHorizL(x1, j)) {
						x1--;
					}
					x1++;
					x2 = i;
					// BUGFIX: Check `x2 < DMAXX` first (fixed)
					while (x2 < DMAXX && WoodHorizR(x2, j)) {
						x2++;
					}
					x2--;
					skip = TRUE;
					if (dgrid[x1][j].dungeon == 7) {
						skip = FALSE;
					}
					if (dgrid[x2][j].dungeon == 7) {
						skip = FALSE;
					}
					if (x2 - x1 > 1 && skip) {
						rp = random_(0, x2 - x1 - 1) + x1 + 1;
						for (x = x1; x <= x2; x++) {
							if (x == rp) {
								continue;
							}
							if (dgrid[x][j].dungeon == 7) {
								if (random_(0, 2) != 0) {
									dgrid[x][j].dungeon = 134;
								} else {
									dgrid[x][j].dungeon = 136;
								}
							}
							if (dgrid[x][j].dungeon == 9) {
								dgrid[x][j].dungeon = 130;
							}
							if (dgrid[x][j].dungeon == 121) {
								dgrid[x][j].dungeon = 132;
							}
							if (dgrid[x][j].dungeon == 124) {
								dgrid[x][j].dungeon = 132;
							}
							if (dgrid[x][j].dungeon == 4) {
								dgrid[x][j].dungeon = 140;
							}
							if (dgrid[x][j].dungeon == 135) {
								dgrid[x][j].dungeon = 138;
							}
							if (dgrid[x][j].dungeon == 137) {
								dgrid[x][j].dungeon = 138;
							}
						}
					}
				}
			}
		}
	}

	AddFenceDoors();
	FenceDoorFix();
}

BOOL DRLG_L3Anvil()
{
	int sx, sy, sw, sh, xx, yy, ii, trys;
	BOOL found;

	sw = L3ANVIL[0];
	sh = L3ANVIL[1];
	sx = random_(0, DMAXX - sw);
	sy = random_(0, DMAXY - sh);

	found = FALSE;
	trys = 0;
	while (!found && trys < 200) {
		trys++;
		found = TRUE;
		ii = 2;
		for (yy = 0; yy < sh && found == TRUE; yy++) {
			for (xx = 0; xx < sw && found == TRUE; xx++) {
				if (L3ANVIL[ii] != 0 && dgrid[xx + sx][yy + sy].dungeon != L3ANVIL[ii]) {
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
	if (trys >= 200) {
		return TRUE;
	}

	ii = sw * sh + 2;
	for (yy = 0; yy < sh; yy++) {
		for (xx = 0; xx < sw; xx++) {
			if (L3ANVIL[ii] != 0) {
				dgrid[xx + sx][yy + sy].dungeon = L3ANVIL[ii];
			}
			dgrid[xx + sx][yy + sy].dflags |= DLRG_PROTECTED;
			ii++;
		}
	}

	setpc_x = sx;
	setpc_y = sy;
	setpc_w = sw;
	setpc_h = sh;

	return FALSE;
}

void FixL3Warp()
{
	int i, j;

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dgrid[i][j].dungeon == 125 && dgrid[i + 1][j].dungeon == 125 && dgrid[i][j + 1].dungeon == 125 && dgrid[i + 1][j + 1].dungeon == 125) {
				dgrid[i][j].dungeon = 156;
				dgrid[i + 1][j].dungeon = 155;
				dgrid[i][j + 1].dungeon = 153;
				dgrid[i + 1][j + 1].dungeon = 154;
				return;
			}
			if (dgrid[i][j].dungeon == 5 && dgrid[i + 1][j + 1].dungeon == 7) {
				dgrid[i][j].dungeon = 7;
			}
		}
	}
}

void FixL3HallofHeroes()
{
	int i, j;

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dgrid[i][j].dungeon == 5 && dgrid[i + 1][j + 1].dungeon == 7) {
				dgrid[i][j].dungeon = 7;
			}
		}
	}
	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dgrid[i][j].dungeon == 5 && dgrid[i + 1][j + 1].dungeon == 12 && dgrid[i + 1][j].dungeon == 7) {
				dgrid[i][j].dungeon = 7;
				dgrid[i][j + 1].dungeon = 7;
				dgrid[i + 1][j + 1].dungeon = 7;
			}
			if (dgrid[i][j].dungeon == 5 && dgrid[i + 1][j + 1].dungeon == 12 && dgrid[i][j + 1].dungeon == 7) {
				dgrid[i][j].dungeon = 7;
				dgrid[i + 1][j].dungeon = 7;
				dgrid[i + 1][j + 1].dungeon = 7;
			}
		}
	}
}

void DRLG_L3LockRec(int x, int y)
{
	if (!lockout[x][y]) {
		return;
	}

	lockout[x][y] = FALSE;
	lockoutcnt++;
	DRLG_L3LockRec(x, y - 1);
	DRLG_L3LockRec(x, y + 1);
	DRLG_L3LockRec(x - 1, y);
	DRLG_L3LockRec(x + 1, y);
}

BOOL DRLG_L3Lockout()
{
	int i, j, t, fx, fy;

	t = 0;
	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dgrid[i][j].dungeon != 0) {
				lockout[i][j] = TRUE;
				fx = i;
				fy = j;
				t++;
			} else {
				lockout[i][j] = FALSE;
			}
		}
	}

	lockoutcnt = 0;
	DRLG_L3LockRec(fx, fy);

	return t == lockoutcnt;
}

static void DRLG_L3(int entry)
{
	int x1, y1, x2, y2, i, j;
	BOOL found, genok;

	lavapool = FALSE;

	do {
		do {
			do {
				InitL3Dungeon();
				x1 = random_(0, 20) + 10;
				y1 = random_(0, 20) + 10;
				x2 = x1 + 2;
				y2 = y1 + 2;
				DRLG_L3FillRoom(x1, y1, x2, y2);
				DRLG_L3CreateBlock(x1, y1, 2, 0);
				DRLG_L3CreateBlock(x2, y1, 2, 1);
				DRLG_L3CreateBlock(x1, y2, 2, 2);
				DRLG_L3CreateBlock(x1, y1, 2, 3);
				if (QuestStatus(Q_ANVIL)) {
					x1 = random_(0, 10) + 10;
					y1 = random_(0, 10) + 10;
					x2 = x1 + 12;
					y2 = y1 + 12;
					DRLG_L3FloorArea(x1, y1, x2, y2);
				}
				DRLG_L3FillDiags();
				DRLG_L3FillSingles();
				DRLG_L3FillStraights();
				DRLG_L3FillDiags();
				DRLG_L3Edges();
				if (DRLG_L3GetFloorArea() >= 600) {
					found = DRLG_L3Lockout();
				} else {
					found = FALSE;
				}
			} while (!found);
			DRLG_L3MakeMegas();
			if (entry == 0) {
				genok = DRLG_L3PlaceMiniSet(L3UP, 1, 1, -1, -1, 1, 0);
				if (!genok) {
					genok = DRLG_L3PlaceMiniSet(L3DOWN, 1, 1, -1, -1, 0, 1);
					if (!genok && currlevel == 9) {
						genok = DRLG_L3PlaceMiniSet(L3HOLDWARP, 1, 1, -1, -1, 0, 6);
					}
				}
			} else if (entry == 1) {
				genok = DRLG_L3PlaceMiniSet(L3UP, 1, 1, -1, -1, 0, 0);
				if (!genok) {
					genok = DRLG_L3PlaceMiniSet(L3DOWN, 1, 1, -1, -1, 1, 1);
					ViewX += 2;
					ViewY -= 2;
					if (!genok && currlevel == 9) {
						genok = DRLG_L3PlaceMiniSet(L3HOLDWARP, 1, 1, -1, -1, 0, 6);
					}
				}
			} else {
				genok = DRLG_L3PlaceMiniSet(L3UP, 1, 1, -1, -1, 0, 0);
				if (!genok) {
					genok = DRLG_L3PlaceMiniSet(L3DOWN, 1, 1, -1, -1, 0, 1);
					if (!genok && currlevel == 9) {
						genok = DRLG_L3PlaceMiniSet(L3HOLDWARP, 1, 1, -1, -1, 1, 6);
					}
				}
			}
			if (!genok && QuestStatus(Q_ANVIL)) {
				genok = DRLG_L3Anvil();
			}
		} while (genok == TRUE);
		DRLG_L3Pool();
	} while (!lavapool);

	DRLG_L3PoolFix();
	FixL3Warp();
	DRLG_L3PlaceRndSet(L3ISLE1, 70);
	DRLG_L3PlaceRndSet(L3ISLE2, 70);
	DRLG_L3PlaceRndSet(L3ISLE3, 30);
	DRLG_L3PlaceRndSet(L3ISLE4, 30);
	DRLG_L3PlaceRndSet(L3ISLE1, 100);
	DRLG_L3PlaceRndSet(L3ISLE2, 100);
	DRLG_L3PlaceRndSet(L3ISLE5, 90);
	FixL3HallofHeroes();
	DRLG_L3River();

	if (QuestStatus(Q_ANVIL)) {
		dgrid[setpc_x + 7][setpc_y + 5].dungeon = 7;
		dgrid[setpc_x + 8][setpc_y + 5].dungeon = 7;
		dgrid[setpc_x + 9][setpc_y + 5].dungeon = 7;
		if (dgrid[setpc_x + 10][setpc_y + 5].dungeon == 17 || dgrid[setpc_x + 10][setpc_y + 5].dungeon == 18) {
			dgrid[setpc_x + 10][setpc_y + 5].dungeon = 45;
		}
	}

	DRLG_PlaceThemeRooms(5, 10, 7, 0, 0);
	DRLG_L3Wood();
	DRLG_L3PlaceRndSet(L3TITE1, 10);
	DRLG_L3PlaceRndSet(L3TITE2, 10);
	DRLG_L3PlaceRndSet(L3TITE3, 10);
	DRLG_L3PlaceRndSet(L3TITE6, 20);
	DRLG_L3PlaceRndSet(L3TITE7, 20);
	DRLG_L3PlaceRndSet(L3TITE8, 20);
	DRLG_L3PlaceRndSet(L3TITE9, 20);
	DRLG_L3PlaceRndSet(L3TITE10, 20);
	DRLG_L3PlaceRndSet(L3TITE11, 30);
	DRLG_L3PlaceRndSet(L3TITE12, 20);
	DRLG_L3PlaceRndSet(L3TITE13, 20);
	DRLG_L3PlaceRndSet(L3CREV1, 30);
	DRLG_L3PlaceRndSet(L3CREV2, 30);
	DRLG_L3PlaceRndSet(L3CREV3, 30);
	DRLG_L3PlaceRndSet(L3CREV4, 30);
	DRLG_L3PlaceRndSet(L3CREV5, 30);
	DRLG_L3PlaceRndSet(L3CREV6, 30);
	DRLG_L3PlaceRndSet(L3CREV7, 30);
	DRLG_L3PlaceRndSet(L3CREV8, 30);
	DRLG_L3PlaceRndSet(L3CREV9, 30);
	DRLG_L3PlaceRndSet(L3CREV10, 30);
	DRLG_L3PlaceRndSet(L3CREV11, 30);
	DRLG_L3PlaceRndSet(L3XTRA1, 25);
	DRLG_L3PlaceRndSet(L3XTRA2, 25);
	DRLG_L3PlaceRndSet(L3XTRA3, 25);
	DRLG_L3PlaceRndSet(L3XTRA4, 25);
	DRLG_L3PlaceRndSet(L3XTRA5, 25);

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			dgrid[i][j].pdungeon = dgrid[i][j].dungeon;
		}
	}

	DRLG_Init_Globals();
}

static void DRLG_L3Pass3()
{
	int i, j, xx, yy;
	long v1, v2, v3, v4, lv;
	WORD *MegaTiles;

	lv = 8 - 1;

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

void CreateL3Dungeon(DWORD rseed, int entry)
{
	int i, j;

	SetRndSeed(rseed);
	dminx = 16;
	dminy = 16;
	dmaxx = 96;
	dmaxy = 96;
	DRLG_InitTrans();
	DRLG_InitSetPC();
	DRLG_L3(entry);
	DRLG_L3Pass3();

	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++) {
			if (grid[i][j].dPiece >= 56 && grid[i][j].dPiece <= 147) {
				DoLighting(i, j, 7, -1);
			} else if (grid[i][j].dPiece >= 154 && grid[i][j].dPiece <= 161) {
				DoLighting(i, j, 7, -1);
			} else if (grid[i][j].dPiece == 150) {
				DoLighting(i, j, 7, -1);
			} else if (grid[i][j].dPiece == 152) {
				DoLighting(i, j, 7, -1);
			}
		}
	}

	DRLG_SetPC();
}

void LoadL3Dungeon(char *sFileName, int vx, int vy)
{
	int i, j, rw, rh;
	BYTE *pLevelMap, *lm;

	InitL3Dungeon();
	dminx = 16;
	dminy = 16;
	dmaxx = 96;
	dmaxy = 96;
	DRLG_InitTrans();
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
			} else {
				dgrid[i][j].dungeon = 7;
			}
			lm += 2;
		}
	}
	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dgrid[i][j].dungeon == 0) {
				dgrid[i][j].dungeon = 8;
			}
		}
	}

	abyssx = MAXDUNX; // Unused
	DRLG_L3Pass3();
	DRLG_Init_Globals();
	ViewX = vx;
	ViewY = vy;
	SetMapMonsters(pLevelMap, 0, 0);
	SetMapObjects(pLevelMap, 0, 0);

	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++) {
			if (grid[i][j].dPiece >= 56 && grid[i][j].dPiece <= 147) {
				DoLighting(i, j, 7, -1);
			} else if (grid[i][j].dPiece >= 154 && grid[i][j].dPiece <= 161) {
				DoLighting(i, j, 7, -1);
			} else if (grid[i][j].dPiece == 150) {
				DoLighting(i, j, 7, -1);
			} else if (grid[i][j].dPiece == 152) {
				DoLighting(i, j, 7, -1);
			}
		}
	}

	mem_free_dbg(pLevelMap);
}

void LoadPreL3Dungeon(char *sFileName, int vx, int vy)
{
	int i, j, rw, rh;
	BYTE *pLevelMap, *lm;

	InitL3Dungeon();
	DRLG_InitTrans();
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
			} else {
				dgrid[i][j].dungeon = 7;
			}
			lm += 2;
		}
	}
	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dgrid[i][j].dungeon == 0) {
				dgrid[i][j].dungeon = 8;
			}
		}
	}

	memcpy(pdungeon, dungeon, sizeof(pdungeon));
	mem_free_dbg(pLevelMap);
}

DEVILUTION_END_NAMESPACE
#endif
