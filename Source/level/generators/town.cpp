#include "all.h"

namespace dvl {


LvlTown::LvlTown()
    : Level(DunType::town)
{
	_automapFile = "";
}

void LvlTown::create(int lvldir)
{
	CreateTown(lvldir);
	InitTownTriggers();
	LoadRndLvlPal(DunType::town);
};

void LvlTown::loadGFX()
{
	assert(!pDungeonCels);
	pDungeonCels = LoadFileInMem("Levels\\TownData\\Town.CEL", NULL);
	pMegaTiles = LoadFileInMem("Levels\\TownData\\Town.TIL", NULL);
	pLevelPieces = LoadFileInMem("Levels\\TownData\\Town.MIN", NULL);
	pSpecialCels = LoadFileInMem("Levels\\TownData\\TownS.CEL", NULL);
}

/**
 * @brief Load level data into dPiece
 * @param P3Tiles Tile set
 * @param pSector Sector data
 * @param xi upper left destination
 * @param yi upper left destination
 * @param w width of sector
 * @param h height of sector
 */
void T_FillSector(uint8_t *P3Tiles, uint8_t *pSector, int xi, int yi, int w, int h)
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
				v1 = SDL_SwapLE16(*(Sector + 0));
				v2 = SDL_SwapLE16(*(Sector + 1));
				v3 = SDL_SwapLE16(*(Sector + 2));
				v4 = SDL_SwapLE16(*(Sector + 3));

			} else {
				v1 = 0;
				v2 = 0;
				v3 = 0;
				v4 = 0;
			}

			grid[xx][yy].setPiece(v1);
			grid[xx + 1][yy].setPiece(v2);
			grid[xx][yy + 1].setPiece(v3);
			grid[xx + 1][yy + 1].setPiece(v4);
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
void T_FillTile(uint8_t *P3Tiles, int xx, int yy, int t)
{
	long v1, v2, v3, v4;
	WORD *Tiles;

	Tiles = ((WORD *)&P3Tiles[(t - 1) * 8]);
	v1 = SDL_SwapLE16(*(Tiles + 0)); // +1;
	v2 = SDL_SwapLE16(*(Tiles + 1)); // +1;
	v3 = SDL_SwapLE16(*(Tiles + 2)); // +1;
	v4 = SDL_SwapLE16(*(Tiles + 3)); // +1;

	grid[xx][yy].setPiece(v1);
	grid[xx + 1][yy].setPiece(v2);
	grid[xx][yy + 1].setPiece(v3);
	grid[xx + 1][yy + 1].setPiece(v4);
}

/**
 * @brief Initialize all of the levels data
 */
void T_Pass3()
{
	int xx, yy, x;
	uint8_t *P3Tiles, *pSector;

	for (yy = 0; yy < MAXDUNY; yy += 2) {
		for (xx = 0; xx < MAXDUNX; xx += 2) {
			grid[xx][yy].clearPiece();
			grid[xx + 1][yy].clearPiece();
			grid[xx][yy + 1].clearPiece();
			grid[xx + 1][yy + 1].clearPiece();
		}
	}

	P3Tiles = LoadFileInMem("Levels\\TownData\\Town.TIL", NULL);
	pSector = LoadFileInMem("Levels\\TownData\\Sector1s.DUN", NULL);
	T_FillSector(P3Tiles, pSector, 46, 46, 25, 25);
	mem_free_dbg(pSector);

	return;

	pSector = LoadFileInMem("Levels\\TownData\\Sector2s.DUN", NULL);
	T_FillSector(P3Tiles, pSector, 46, 0, 25, 23);
	mem_free_dbg(pSector);
	pSector = LoadFileInMem("Levels\\TownData\\Sector3s.DUN", NULL);
	T_FillSector(P3Tiles, pSector, 0, 46, 23, 25);
	mem_free_dbg(pSector);
	pSector = LoadFileInMem("Levels\\TownData\\Sector4s.DUN", NULL);
	T_FillSector(P3Tiles, pSector, 0, 0, 23, 23);
	mem_free_dbg(pSector);

	if (game.isSingleplayer()) {
		if (!(myplr().data.pTownWarps & 1)) {
			T_FillTile(P3Tiles, 48, 20, 320);
		}
		if (!(myplr().data.pTownWarps & 2)) {
			T_FillTile(P3Tiles, 16, 68, 332);
			T_FillTile(P3Tiles, 16, 70, 331);
		}
		if (!(myplr().data.pTownWarps & 4)) {
			for (x = 36; x < 46; x++) {
				T_FillTile(P3Tiles, x, 78, random_(0, 4) + 1);
			}
		}
	}

	if (quests[QuestId::pwater]._qactive != QuestState::done && quests[QuestId::pwater]._qactive) {
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

	lvl.dmin = { 10, 10 };
	lvl.dmax = { 84, 84 };
	DRLG_InitTrans();
	DRLG_Init_Globals();

	if (entry == 0) {
		View = { 75, 68 };
	} else if (entry == 1) {
		View = { 25, 31 };
	} else if (entry == 7) {
		if (TWarpFrom == 5) {
			View = { 49, 22 };
		}
		if (TWarpFrom == 9) {
			View = { 18, 69 };
		}
		if (TWarpFrom == 13) {
			View = { 41, 81 };
		}
	}

	T_Pass3();
	grid.initTown(); // Clears almost everything?

	for (y = 0; y < MAXDUNY; y++) {
		for (x = 0; x < MAXDUNX; x++) {
			if (!grid[x][y].isPiece())
				continue;
			if (grid[x][y].getPiece() == 360 ) {
				grid[x][y].dSpecial = 1;
			} else if (grid[x][y].getPiece() == 358) {
				grid[x][y].dSpecial = 2;
			} else if (grid[x][y].getPiece() == 129) {
				grid[x][y].dSpecial = 6;
			} else if (grid[x][y].getPiece() == 130) {
				grid[x][y].dSpecial = 7;
			} else if (grid[x][y].getPiece() == 128) {
				grid[x][y].dSpecial = 8;
			} else if (grid[x][y].getPiece() == 117) {
				grid[x][y].dSpecial = 9;
			} else if (grid[x][y].getPiece() == 157) {
				grid[x][y].dSpecial = 10;
			} else if (grid[x][y].getPiece() == 158) {
				grid[x][y].dSpecial = 11;
			} else if (grid[x][y].getPiece() == 156) {
				grid[x][y].dSpecial = 12;
			} else if (grid[x][y].getPiece() == 162 ) {
				grid[x][y].dSpecial = 13;
			} else if (grid[x][y].getPiece() == 160) {
				grid[x][y].dSpecial = 14;
			} else if (grid[x][y].getPiece() == 214) {
				grid[x][y].dSpecial = 15;
			} else if (grid[x][y].getPiece() == 212) {
				grid[x][y].dSpecial = 16;
			} else if (grid[x][y].getPiece() == 217) {
				grid[x][y].dSpecial = 17;
			} else if (grid[x][y].getPiece() == 216) {
				grid[x][y].dSpecial = 18;
			}
		}
	}
}

}
