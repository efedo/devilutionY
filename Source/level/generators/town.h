//HEADER_GOES_HERE
#ifndef __TOWN_H__
#define __TOWN_H__

#include "level/level.h"

DEVILUTION_BEGIN_NAMESPACE

class LvlTown : public Level {
	LvlTown();
	void create(int lvldir) override;
	void loadGFX() override;
};

void SetTownMicros();
void T_FillSector(uint8_t *P3Tiles, uint8_t *pSector, int xi, int yi, int w, int h);
void T_FillTile(uint8_t *P3Tiles, int xx, int yy, int t);
void T_Pass3();
void CreateTown(int entry);

DEVILUTION_END_NAMESPACE

#endif /* __TOWN_H__ */
