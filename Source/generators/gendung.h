/**
 * @file gendung.h
 *
 * Interface of general dungeon generation code.
 */
#ifndef __GENDUNG_H__
#define __GENDUNG_H__

DEVILUTION_BEGIN_NAMESPACE

//#ifdef __cplusplus
//extern "C" {
//#endif

class DTile {
public:
	BYTE dungeon;
	BYTE pdungeon;
	char dflags;
};

class DGrid {
public:
	auto &operator[](const size_t n)
	{
		return tiles[n];
	}

	void init()
	{
		for (int j = 0; j < DMAXY; j++) {
			for (int i = 0; i < DMAXX; i++) {
				DTile &tile = tiles[i][j];
				tile.dungeon = 0;
				tile.dflags = 0;
			}
		}
	}

	void copytopdungeon()
	{
		for (int j = 0; j < DMAXY; j++) {
			for (int i = 0; i < DMAXX; i++) {
				tiles[i][j].pdungeon = tiles[i][j].dungeon;
			}
		}
	}

private:
	DTile tiles[DMAXX][DMAXY];
};

extern DGrid dgrid;

extern int setpc_x;
extern int setpc_y;
extern int setpc_w;
extern int setpc_h;
extern BYTE *pSetPiece;
extern BOOL setloadflag;
extern BYTE *pSpecialCels;
extern BYTE *pMegaTiles;
extern BYTE *pLevelPieces;
extern BYTE *pDungeonCels;
extern BYTE *pSpeedCels;
extern int SpeedFrameTbl[128][16];
extern char block_lvid[MAXTILES + 1];
extern int level_frame_count[MAXTILES];
extern int tile_defs[MAXTILES];
extern WORD level_frame_types[MAXTILES];
extern int level_frame_sizes[MAXTILES];
extern int nlevel_frames;

class PieceProperties {
public:
	BOOLEAN nBlockTable;   // List of light blocking dPieces
	BOOLEAN nSolidTable;   // List of path blocking dPieces
	BOOLEAN nTransTable;   // List of transparent dPieces
	BOOLEAN nMissileTable; // List of missile blocking dPieces
	BOOLEAN nTrapTable;
};

class PieceInventory {
public:
	auto &operator[](const size_t n)
	{
		return list[n];
	}
	void clear()
	{
		for (int i = 0; i != MAXTILES + 1; ++i) {
			PieceProperties & piece = list[i];
			piece.nBlockTable = 0;
			piece.nSolidTable = 0;
			piece.nTransTable = 0;
			piece.nMissileTable = 0;
			piece.nTrapTable = 0;
		}
	}
private:
	PieceProperties list[MAXTILES + 1];
};

extern PieceInventory pieces;

extern int dminx;
extern int dminy;
extern int dmaxx;
extern int dmaxy;
extern int gnDifficulty;

class Level {
public:
	BYTE leveltype;
	BYTE currlevel;
	BOOLEAN setlevel;
	BYTE setlvlnum;
	char setlvltype;
};

extern Level level;

extern int ViewX;
extern int ViewY;
extern int ViewBX;
extern int ViewBY;
extern int ViewDX;
extern int ViewDY;
extern ScrollStruct ScrollInfo;
extern int LvlViewX;
extern int LvlViewY;
extern int MicroTileLen;
extern char TransVal;
extern BOOLEAN TransList[256];

extern MICROS dpiece_defs_map_1[MAXDUNX * MAXDUNY];

class Tile {
public:
	int dPiece;
	MICROS dpiece_defs_map_2;
	char dTransVal;
	char dLight;
	char dPreLight;
	char dFlags;
	char dPlayer;
	int dMonster;
	char dDead;
	char dObject;
	char dItem;
	char dMissile;
	char dSpecial;
};

class Grid {
public:
	auto &operator[](const size_t n) { return tiles[n]; }
	void clearTrans() {
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++) {
				tiles[i][j].dTransVal = 0;
			}
		}
	}
	void clearLight()
	{
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++) {
				tiles[i][j].dLight = 0;
			}
		}
	}
	void prelightToLight()
	{
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++) {
				tiles[i][j].dLight = tiles[i][j].dPreLight;
			}
		}
	}
	void lightToPrelight()
	{
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++) {
				tiles[i][j].dPreLight = tiles[i][j].dLight;
			}
		}
	}
	void init(char light) // (Dung)
	{
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++) {
				Tile &tile = tiles[i][j];
				tile.dPlayer = 0;
				tile.dMonster = 0;
				tile.dDead = 0;
				tile.dObject = 0;
				tile.dItem = 0;
				tile.dMissile = 0;
				tile.dSpecial = 0;
				tile.dLight = light;
			}
		}
	}

	void initTown() // (Dung)
	{
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++) {
				Tile &tile = tiles[i][j];
				tile.dLight = 0;
				tile.dFlags = 0;
				tile.dPlayer = 0;
				tile.dMonster = 0;
				tile.dObject = 0;
				tile.dItem = 0;
				tile.dSpecial = 0;
			}
		}
	}


private:
	Tile tiles[MAXDUNX][MAXDUNY];
};

extern Grid grid;

extern int themeCount;
extern THEME_LOC themeLoc[MAXTHEMES];

void FillSolidBlockTbls();
void SetDungeonMicros();
void DRLG_InitTrans();
void DRLG_MRectTrans(int x1, int y1, int x2, int y2);
void DRLG_RectTrans(int x1, int y1, int x2, int y2);
void DRLG_CopyTrans(int sx, int sy, int dx, int dy);
void DRLG_ListTrans(int num, BYTE *List);
void DRLG_AreaTrans(int num, BYTE *List);
void DRLG_InitSetPC();
void DRLG_SetPC();
void Make_SetPC(int x, int y, int w, int h);
BOOL DRLG_WillThemeRoomFit(int floor, int x, int y, int minSize, int maxSize, int *width, int *height);
void DRLG_CreateThemeRoom(int themeIndex);
void DRLG_PlaceThemeRooms(int minSize, int maxSize, int floor, int freq, int rndSize);
void DRLG_HoldThemeRooms();
BOOL SkipThemeRoom(int x, int y);
void InitLevels();

//#ifdef __cplusplus
//}
//#endif

DEVILUTION_END_NAMESPACE

#endif /* __GENDUNG_H__ */
