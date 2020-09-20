/**
 * @file grid.cpp
 *
 * Implementation of general dungeon generation code.
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

Grid grid;

Tile &at(V2Di pos)
{
	assert(pos.x >= 0 && pos.y >= 0);
	assert(pos.x < MAXDUNX && pos.y < MAXDUNY);
	return tiles[pos.x][pos.y];
}

Tile &at(const size_t x, const size_t y)
{
	assert(x < MAXDUNX && y < MAXDUNY);
	return tiles[x][y];
}
Tile * &Grid::operator[](const size_t n)
{
	return tiles[n];
}
void Grid::clearTrans()
{
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++) {
			tiles[i][j].dTransVal = 0;
		}
	}
}
void Grid::clearLight()
{
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++) {
			tiles[i][j].dLight = 0;
		}
	}
}
void Grid::prelightToLight()
{
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++) {
			tiles[i][j].dLight = tiles[i][j].dPreLight;
		}
	}
}
void Grid::lightToPrelight()
{
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++) {
			tiles[i][j].dPreLight = tiles[i][j].dLight;
		}
	}
}
void Grid::init(char light) // (Dung)
{
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++) {
			Tile &tile = tiles[i][j];
			tile.dPlayer = 0;
			tile.dMonster = 0;
			tile.dDead = 0;
			tile.dObject = 0;
			tile.dItem = 0;
			tile.dMissile = 0;
			tile.dSpecial = 0;
			tile.dLight = light;
		}
	}
}

void Grid::initTown() // (Dung)
{
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++) {
			Tile &tile = tiles[i][j];
			tile.dLight = 0;
			tile.dFlags = 0;
			tile.dPlayer = 0;
			tile.dMonster = 0;
			tile.dObject = 0;
			tile.dItem = 0;
			tile.dSpecial = 0;
		}
	}
}

void Grid::movePlayer(uint8_t pnum, V2Di from, V2Di to)
{
	tiles[from.x][from.y].clearPlayer(pnum);
	tiles[to.x][to.y].setPlayerNum(pnum);
}

void Grid::addPlayer(uint8_t pnum, V2Di pos)
{
	tiles[pos.x][pos.y].setPlayerNum(pnum);
}

void Grid::removePlayer(uint8_t pnum, V2Di pos)
{
	tiles[pos.x][pos.y].clearPlayer(pnum);
}

DEVILUTION_END_NAMESPACE
