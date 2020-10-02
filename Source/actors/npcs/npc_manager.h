// HEADER_GOES_HERE
#ifndef __NPC_MANAGER_H__
#define __NPC_MANAGER_H__

#include "npc.h"
#include "towners.h"

DEVILUTION_BEGIN_NAMESPACE

// Non-player actor
class NpcManager
{
   public:
	void InitTowners();
	void InitCows();
	void SetTownerGPtrs(uint8_t *pData, uint8_t **pAnim);
	void FreeTownerGFX();
	void ProcessTowners();

	Smith smith;
	BarOwner barowner;
	DeadGuy deadguy;
	Witch witch;
	Barmaid barmaid;
	Boy boy;
	Healer healer;
	Teller teller;
	Drunk drunk;
	Cow cow[3];

	bool boyloadflag;

	int TownCowX[3] = {58, 56, 59};
	int TownCowY[3] = {16, 14, 20};
	Dir TownCowDir[3] = {Dir::SW, Dir::NW, Dir::N};
	int cowoffx[8] = {-1, 0, -1, -1, -1, 0, -1, -1};
	int cowoffy[8] = {-1, -1, -1, 0, -1, -1, -1, 0};
};

extern NpcManager npcs;

DEVILUTION_END_NAMESPACE

#endif __NPC_MANAGER_H__
