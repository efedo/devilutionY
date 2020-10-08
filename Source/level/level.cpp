/**
 * @file level.cpp
 *
 * Game level class
 */
#include "all.h"

namespace dvl {

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


void Level::PlrClrTrans(V2Di pos)
{
	for (int i = pos.y - 1; i <= pos.y + 1; i++) {
		for (int j = pos.x - 1; j <= pos.x + 1; j++) {
			lvl.TransList[grid[j][i].dTransVal] = false;
		}
	}
}

void Level::PlrDoTrans(V2Di pos)
{
	if (lvl.type() != DunType::cathedral && lvl.type() != DunType::catacombs) {
		lvl.TransList[1] = true;
	} else {
		for (int i = pos.y - 1; i <= pos.y + 1; i++) {
			for (int j = pos.x - 1; j <= pos.x + 1; j++) {
				if (!grid[j][i].isSolid() && grid[j][i].dTransVal) {
					lvl.TransList[grid[j][i].dTransVal] = true;
				}
			}
		}
	}
}


Level lvl;

}
