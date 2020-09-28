/**
 * @file grid.cpp
 *
 * Implementation of general dungeon generation code.
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

Grid grid;

Tile &Grid::at(V2Di pos)
{
	assert(pos.x >= 0 && pos.y >= 0);
	assert(pos.x < MAXDUNX && pos.y < MAXDUNY);
	return tiles[pos.x][pos.y];
}

Tile &Grid::at(const size_t x, const size_t y)
{
	assert(x < MAXDUNX && y < MAXDUNY);
	return tiles[x][y];
}

Tile * Grid::operator[](const size_t n)
{
	return tiles[n];
}

bool Grid::isValid(V2Di pos)
{
	return (pos.x >= 0 && pos.x < MAXDUNX && pos.y >= 0 && pos.y < MAXDUNY);
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
			tile.clearPlayer();
			tile.clearMonster();
			tile.dDead = 0;
			tile.clearObject();
			tile.clearItem();
			tile.clearMissile();
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
			tile.clearPlayer();
			tile.clearPlayerDraw();
			tile.clearMonster();
			tile.clearObject();
			tile.clearItem();
			tile.dSpecial = 0;
		}
	}
}

DEVILUTION_END_NAMESPACE
