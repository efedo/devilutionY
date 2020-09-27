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
extern RECT32 setpc;

extern BYTE *pSetPiece;
extern bool setloadflag;
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

extern V2Di dmin;
extern V2Di dmax;

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

extern V2Di View;
extern V2Di ViewB;
extern V2Di ViewD;
extern V2Di LvlView;

extern ScrollStruct ScrollInfo;
extern int MicroTileLen;
extern char TransVal;
extern BOOLEAN TransList[256];

extern MICROS dpiece_defs_map_1[MAXDUNX * MAXDUNY];




extern int themeCount;
extern THEME_LOC themeLoc[MAXTHEMES];

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
bool DRLG_WillThemeRoomFit(int floor, int x, int y, int minSize, int maxSize, int *width, int *height);
void DRLG_CreateThemeRoom(int themeIndex);
void DRLG_PlaceThemeRooms(int minSize, int maxSize, int floor, int freq, int rndSize);
void DRLG_HoldThemeRooms();
bool SkipThemeRoom(int x, int y);
void InitLevels();

//#ifdef __cplusplus
//}
//#endif

DEVILUTION_END_NAMESPACE

#endif /* __GENDUNG_H__ */
