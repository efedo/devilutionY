/**
 * @file level.cpp
 *
 * Game level class
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

Level::Level()
{
}

Level::Level(DunType duntype)
    : _leveltype(duntype)
{
}

Level::~Level()
{
	if (pDungeonCels) MemFreeDbg(pDungeonCels);
	if (pMegaTiles) MemFreeDbg(pMegaTiles);
	if (pLevelPieces) MemFreeDbg(pLevelPieces);
	if (pSpecialCels) MemFreeDbg(pSpecialCels);
}

void Level::create(int lvldir)
{
	app_fatal("CreateLevel");
};

void Level::loadGFX()
{
	assert(!pDungeonCels);
	app_fatal("LoadLvlGFX");
}

void Level::LoadSetMap()
{
	switch (lvl.setlvlnum) {
	case SetLvl::BoneChamb:
		LoadPreL2Dungeon("Levels\\L2Data\\Bonecha2.DUN", 69, 39);
		LoadL2Dungeon("Levels\\L2Data\\Bonecha1.DUN", 69, 39);
		LoadPalette("Levels\\L2Data\\L2_2.pal");
		DRLG_ListTrans(sizeof(SkelChamTrans1) / 4, &SkelChamTrans1[0]);
		DRLG_AreaTrans(sizeof(SkelChamTrans2) / 4, &SkelChamTrans2[0]);
		DRLG_ListTrans(sizeof(SkelChamTrans3) / 4, &SkelChamTrans3[0]);
		AddL2Objs({ 0, 0 }, { MAXDUNX, MAXDUNY });
		AddSChamObjs();
		InitSChambTriggers();
		break;
	case SetLvl::PoisonWater:
		if (quests[Q_PWATER]._qactive == QUEST_INIT)
			quests[Q_PWATER]._qactive = QUEST_ACTIVE;
		LoadPreL3Dungeon("Levels\\L3Data\\Foulwatr.DUN", 19, 50);
		LoadL3Dungeon("Levels\\L3Data\\Foulwatr.DUN", 31, 83);
		LoadPalette("Levels\\L3Data\\L3pfoul.pal");
		InitPWaterTriggers();
		break;
	}
}

void Level::setType(DunType duntype)
{
	_leveltype = duntype;
}


void Level::setpc(RECT32 newsetpc)
{
	_setpc = newsetpc;
}

const RECT32 &Level::getpc()
{
	return _setpc;
}

DunType Level::type()
{
	return _leveltype;
}

std::string Level::automapFile()
{
	return _automapFile;
}

bool Level::hasAutomapFile()
{
	return !_automapFile.empty();
}

Level lvl;

DEVILUTION_END_NAMESPACE
