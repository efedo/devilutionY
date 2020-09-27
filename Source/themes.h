/**
 * @file themes.h
 *
 * Interface of the theme room placing algorithms.
 */
#ifndef __THEMES_H__
#define __THEMES_H__

DEVILUTION_BEGIN_NAMESPACE

//#ifdef __cplusplus
//extern "C" {
//#endif

extern int numthemes;
extern bool armorFlag;
extern bool ThemeGoodIn[4];
extern bool weaponFlag;
extern bool treasureFlag;
extern bool mFountainFlag;
extern bool cauldronFlag;
extern bool tFountainFlag;
extern int zharlib;
extern int themex;
extern int themey;
extern int themeVar1;
extern ThemeStruct themes[MAXTHEMES];
extern bool pFountainFlag;
extern bool bFountainFlag;
extern bool bCrossFlag;

bool TFit_Shrine(int i);
bool TFit_Obj5(int t);
bool TFit_SkelRoom(int t);
bool TFit_GoatShrine(int t);
bool CheckThemeObj3(int xp, int yp, int t, int f);
bool TFit_Obj3(int t);
bool CheckThemeReqs(int t);
bool SpecialThemeFit(int i, int t);
bool CheckThemeRoom(int tv);
void InitThemes();
void HoldThemeRooms();
void PlaceThemeMonsts(int t, int f);
void Theme_Barrel(int t);
void Theme_Shrine(int t);
void Theme_MonstPit(int t);
void Theme_SkelRoom(int t);
void Theme_Treasure(int t);
void Theme_Library(int t);
void Theme_Torture(int t);
void Theme_BloodFountain(int t);
void Theme_Decap(int t);
void Theme_PurifyingFountain(int t);
void Theme_ArmorStand(int t);
void Theme_GoatShrine(int t);
void Theme_Cauldron(int t);
void Theme_MurkyFountain(int t);
void Theme_TearFountain(int t);
void Theme_BrnCross(int t);
void Theme_WeaponRack(int t);
void UpdateL4Trans();
void CreateThemeRooms();

/* rdata */

extern int ThemeGood[4];
extern int trm5x[25];
extern int trm5y[25];
extern int trm3x[9];
extern int trm3y[9];

//#ifdef __cplusplus
//}
//#endif

DEVILUTION_END_NAMESPACE

#endif /* __THEMES_H__ */
