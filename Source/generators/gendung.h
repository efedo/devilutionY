/**
 * @file gendung.h
 *
 * Interface of general dungeon generation code.
 */
#ifndef __GENDUNG_H__
#define __GENDUNG_H__

DEVILUTION_BEGIN_NAMESPACE




extern uint8_t *pSetPiece;
extern bool setloadflag;
extern uint8_t *pSpeedCels;
extern int SpeedFrameTbl[128][16];

extern int tile_defs[MAXTILES];
extern int level_frame_count[MAXTILES];
extern WORD level_frame_types[MAXTILES];
extern int level_frame_sizes[MAXTILES];
extern int nlevel_frames;

extern int gnDifficulty;

extern V2Di View;
extern V2Di ViewB;
extern V2Di ViewD;
extern V2Di LvlView;

extern ScrollStruct ScrollInfo;
extern int MicroTileLen;

void SetDungeonMicros();
void DRLG_InitTrans();
void DRLG_MRectTrans(int x1, int y1, int x2, int y2);
void DRLG_RectTrans(int x1, int y1, int x2, int y2);
void DRLG_CopyTrans(int sx, int sy, int dx, int dy);
void DRLG_ListTrans(int num, uint8_t *List);
void DRLG_AreaTrans(int num, uint8_t *List);
void DRLG_InitSetPC();
void DRLG_SetPC();
void Make_SetPC(int x, int y, int w, int h);
bool DRLG_WillThemeRoomFit(int floor, int x, int y, int minSize, int maxSize, int *width, int *height);

void DRLG_CreateThemeRoom(int themeIndex);

void DRLG_PlaceThemeRooms(int minSize, int maxSize, int floor, int freq, int rndSize);
void DRLG_HoldThemeRooms();
bool SkipThemeRoom(int x, int y);
void InitLevels();

DEVILUTION_END_NAMESPACE

#endif /* __GENDUNG_H__ */
