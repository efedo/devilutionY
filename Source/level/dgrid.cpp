/**
 * @file dgrid.cpp
 *
 * Course dungeon grid
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

DTile * DGrid::operator[](const size_t n)
{
	return tiles[n];
}

void DGrid::init()
{
	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			DTile &tile = tiles[i][j];
			tile.dungeon = 0;
			tile.dflags = 0;
		}
	}
}

void DGrid::copytopdungeon()
{
	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			tiles[i][j].pdungeon = tiles[i][j].dungeon;
		}
	}
}

DGrid dgrid;

DEVILUTION_END_NAMESPACE
