#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

/**
 * @brief Load level data into dPiece
 * @param P3Tiles Tile set
 * @param pSector Sector data
 * @param xi upper left destination
 * @param yi upper left destination
 * @param w width of sector
 * @param h height of sector
 */
void T_FillSector(BYTE *P3Tiles, BYTE *pSector, int xi, int yi, int w, int h)
{
	int i, j, xx, yy, nMap;
	long v1, v2, v3, v4, ii;
	WORD *Sector;

	ii = 4;
	yy = yi;
	for (j = 0; j < h; j++) {
		xx = xi;
		for (i = 0; i < w; i++) {
			WORD *Map;

			Map = (WORD *)&pSector[ii];
			nMap = SDL_SwapLE16(*Map);
			if (nMap) {
				Sector = (((WORD *)&P3Tiles[(nMap - 1) * 8]));
				v1 = SDL_SwapLE16(*(Sector + 0)) + 1;
				v2 = SDL_SwapLE16(*(Sector + 1)) + 1;
				v3 = SDL_SwapLE16(*(Sector + 2)) + 1;
				v4 = SDL_SwapLE16(*(Sector + 3)) + 1;

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
			ii += 2;
		}
		yy += 2;
	}
}

/**
 * @brief Load a tile in to dPiece
 * @param P3Tiles Tile set
 * @param xx upper left destination
 * @param yy upper left destination
 * @param t tile id
 */
void T_FillTile(BYTE *P3Tiles, int xx, int yy, int t)
{
	long v1, v2, v3, v4;
	WORD *Tiles;

	Tiles = ((WORD *)&P3Tiles[(t - 1) * 8]);
	v1 = SDL_SwapLE16(*(Tiles + 0)) + 1;
	v2 = SDL_SwapLE16(*(Tiles + 1)) + 1;
	v3 = SDL_SwapLE16(*(Tiles + 2)) + 1;
	v4 = SDL_SwapLE16(*(Tiles + 3)) + 1;

	grid[xx][yy].dPiece = v1;
	grid[xx + 1][yy].dPiece = v2;
	grid[xx][yy + 1].dPiece = v3;
	grid[xx + 1][yy + 1].dPiece = v4;
}

/**
 * @brief Initialize all of the levels data
 */
void T_Pass3()
{
	int xx, yy, x;
	BYTE *P3Tiles, *pSector;

	for (yy = 0; yy < MAXDUNY; yy += 2) {
		for (xx = 0; xx < MAXDUNX; xx += 2) {
			grid[xx][yy].dPiece = 0;
			grid[xx + 1][yy].dPiece = 0;
			grid[xx][yy + 1].dPiece = 0;
			grid[xx + 1][yy + 1].dPiece = 0;
		}
	}

	P3Tiles = LoadFileInMem("Levels\\TownData\\Town.TIL", NULL);
	pSector = LoadFileInMem("Levels\\TownData\\Sector1s.DUN", NULL);
	T_FillSector(P3Tiles, pSector, 46, 46, 25, 25);
	mem_free_dbg(pSector);
	pSector = LoadFileInMem("Levels\\TownData\\Sector2s.DUN", NULL);
	T_FillSector(P3Tiles, pSector, 46, 0, 25, 23);
	mem_free_dbg(pSector);
	pSector = LoadFileInMem("Levels\\TownData\\Sector3s.DUN", NULL);
	T_FillSector(P3Tiles, pSector, 0, 46, 23, 25);
	mem_free_dbg(pSector);
	pSector = LoadFileInMem("Levels\\TownData\\Sector4s.DUN", NULL);
	T_FillSector(P3Tiles, pSector, 0, 0, 23, 23);
	mem_free_dbg(pSector);

#ifndef SPAWN
	if (gbMaxPlayers == 1) {
		if (!(plr[myplr].pTownWarps & 1)) {
#endif
			T_FillTile(P3Tiles, 48, 20, 320);
#ifndef SPAWN
		}
		if (!(plr[myplr].pTownWarps & 2)) {
#endif
			T_FillTile(P3Tiles, 16, 68, 332);
			T_FillTile(P3Tiles, 16, 70, 331);
#ifndef SPAWN
		}
		if (!(plr[myplr].pTownWarps & 4)) {
#endif
			for (x = 36; x < 46; x++) {
				T_FillTile(P3Tiles, x, 78, random_(0, 4) + 1);
			}
#ifndef SPAWN
		}
	}
#endif

	if (quests[Q_PWATER]._qactive != QUEST_DONE && quests[Q_PWATER]._qactive) {
		T_FillTile(P3Tiles, 60, 70, 342);
	} else {
		T_FillTile(P3Tiles, 60, 70, 71);
	}

	mem_free_dbg(P3Tiles);
}

/**
 * @brief Initialize town level
 * @param entry Methode of entry
 */
void CreateTown(int entry)
{
	int x, y;

	dminx = 10;
	dminy = 10;
	dmaxx = 84;
	dmaxy = 84;
	DRLG_InitTrans();
	DRLG_Init_Globals();

	if (entry == 0) {
		ViewX = 75;
		ViewY = 68;
	} else if (entry == 1) {
		ViewX = 25;
		ViewY = 31;
	} else if (entry == 7) {
		if (TWarpFrom == 5) {
			ViewX = 49;
			ViewY = 22;
		}
		if (TWarpFrom == 9) {
			ViewX = 18;
			ViewY = 69;
		}
		if (TWarpFrom == 13) {
			ViewX = 41;
			ViewY = 81;
		}
	}

	T_Pass3();
	memset(dLight, 0, sizeof(dLight));
	memset(dFlags, 0, sizeof(dFlags));
	memset(dPlayer, 0, sizeof(dPlayer));
	memset(dMonster, 0, sizeof(dMonster));
	memset(dObject, 0, sizeof(dObject));
	memset(dItem, 0, sizeof(dItem));
	memset(dSpecial, 0, sizeof(dSpecial));

	for (y = 0; y < MAXDUNY; y++) {
		for (x = 0; x < MAXDUNX; x++) {
			if (grid[x][y].dPiece == 360) {
				grid[x][y].dSpecial = 1;
			} else if (grid[x][y].dPiece == 358) {
				grid[x][y].dSpecial = 2;
			} else if (grid[x][y].dPiece == 129) {
				grid[x][y].dSpecial = 6;
			} else if (grid[x][y].dPiece == 130) {
				grid[x][y].dSpecial = 7;
			} else if (grid[x][y].dPiece == 128) {
				grid[x][y].dSpecial = 8;
			} else if (grid[x][y].dPiece == 117) {
				grid[x][y].dSpecial = 9;
			} else if (grid[x][y].dPiece == 157) {
				grid[x][y].dSpecial = 10;
			} else if (grid[x][y].dPiece == 158) {
				grid[x][y].dSpecial = 11;
			} else if (grid[x][y].dPiece == 156) {
				grid[x][y].dSpecial = 12;
			} else if (grid[x][y].dPiece == 162) {
				grid[x][y].dSpecial = 13;
			} else if (grid[x][y].dPiece == 160) {
				grid[x][y].dSpecial = 14;
			} else if (grid[x][y].dPiece == 214) {
				grid[x][y].dSpecial = 15;
			} else if (grid[x][y].dPiece == 212) {
				grid[x][y].dSpecial = 16;
			} else if (grid[x][y].dPiece == 217) {
				grid[x][y].dSpecial = 17;
			} else if (grid[x][y].dPiece == 216) {
				grid[x][y].dSpecial = 18;
			}
		}
	}
}

DEVILUTION_END_NAMESPACE
