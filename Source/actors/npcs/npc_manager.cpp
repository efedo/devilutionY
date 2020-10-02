#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

NpcManager npcs;

void NpcManager::InitTowners()
{
	boyloadflag = false;
	smith.Init();
	healer.Init();
	if (quests[Q_BUTCHER]._qactive != QUEST_NOTAVAIL &&
	    quests[Q_BUTCHER]._qactive != QUEST_DONE)
		deadguy.Init();
	barowner.Init();
	teller.Init();
	drunk.Init();
	witch.Init();
	barmaid.Init();
	boy.Init();
	InitCows();
}


void NpcManager::InitCows()
{
	Dir dir;
	int x, y, xo, yo;

	// if ( pCowCels )
	// assertion_failed(__LINE__, __FILE__, "! pCowCels");
	Cow::pCowCels = LoadFileInMem("Towners\\Animals\\Cow.CEL", NULL);
	for (int i = 0; i < 3; i++) {
		x = TownCowX[i];
		y = TownCowY[i];
		dir = TownCowDir[i];
		cow[i].InitTownerInfo(128, false, TOWN_COW, x, y, -1, 10);
		cow[i]._tNData = Cow::pCowCels;
		SetTownerGPtrs(cow[i]._tNData, cow[i]._tNAnim);
		cow[i]._tNFrames = 12;
		cow[i].NewTownerAnim(cow[i]._tNAnim[int(dir)], cow[i]._tNFrames, 3);
		cow[i]._tAnimFrame = random_(0, 11) + 1;
		cow[i]._tSelFlag = true;
		strcpy(cow[i]._tName, "Cow");

		xo = x + cowoffx[int(dir)];
		yo = y + cowoffy[int(dir)];
		if (!grid[x][yo].isActor()) grid[x][yo].setActor(cow[0]);
		if (!grid[x][yo].isActor()) grid[xo][y].setActor(cow[1]);
		if (!grid[x][yo].isActor()) grid[xo][yo].setActor(cow[2]);
	}
}

void NpcManager::SetTownerGPtrs(uint8_t *pData, uint8_t **pAnim)
{
	int i;
	DWORD *pFrameTable;

	pFrameTable = (DWORD *)pData;

	for (i = 0; i < 8; i++) { pAnim[i] = CelGetFrameStart(pData, i); }
}

void NpcManager::ProcessTowners()
{
	smith.TownCtrlMsg();
	healer.TownCtrlMsg();
	deadguy.TownCtrlMsg();
	barowner.TownCtrlMsg();
	drunk.TownCtrlMsg();
	witch.TownCtrlMsg();
	barmaid.TownCtrlMsg();
	boy.TownCtrlMsg();
	cow[0].TownCtrlMsg();
	cow[1].TownCtrlMsg();
	cow[2].TownCtrlMsg();
}

DEVILUTION_END_NAMESPACE
