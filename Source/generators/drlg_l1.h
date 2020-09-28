/**
 * @file drlg_l1.h
 *
 * Interface of the cathedral level generation algorithms.
 */
#ifndef __DRLG_L1_H__
#define __DRLG_L1_H__

#include "level/level.h"

DEVILUTION_BEGIN_NAMESPACE

class LvlCathedral : public Level {
	LvlCathedral();
	void create(int lvldir) override;
	void loadGFX() override;
	void LoadSetMap() override;

	void CreateL5Dungeon(DWORD rseed, int entry);
	void LoadL1Dungeon(char *sFileName, int vx, int vy);
	void DRLG_L1Pass3();

	void DRLG_CreateThemeRoom(int themeIndex) override;
};

extern uint8_t L5dungeon[80][80];
extern uint8_t L5dflags[DMAXX][DMAXY];
extern bool L5setloadflag;
extern int HR1;
extern int HR2;
extern int HR3;
extern int VR1;
extern int VR2;
extern int VR3;
extern uint8_t *L5pSetPiece;

void DRLG_Init_Globals();
void LoadPreL1Dungeon(char *sFileName, int vx, int vy);

/* rdata */
extern const ShadowStruct SPATS[37];

// BUGFIX: These arrays should contain an additional 0 (207 elements).
extern const uint8_t BSTYPES[207];
extern const uint8_t L5BTYPES[207];

extern const uint8_t STAIRSUP[];
extern const uint8_t L5STAIRSUP[];
extern const uint8_t STAIRSDOWN[];
extern const uint8_t LAMPS[];
extern const uint8_t PWATERIN[];

/* data */
extern uint8_t L5ConvTbl[16];

//#ifdef __cplusplus
//}
//#endif

DEVILUTION_END_NAMESPACE

#endif /* __DRLG_L1_H__ */
