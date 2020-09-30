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

void Level::DRLG_CreateThemeRoom(int themeIndex)
{
	throw; // not implemented
}


void Level::LoadSetMap()
{
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
