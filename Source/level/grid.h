/**
 * @file grid.h
 *
 * In-game grid class
 */
#ifndef __GRID_H__
#define __GRID_H__

#include "tile.h"

DEVILUTION_BEGIN_NAMESPACE

class Grid {
public:
	Tile &at(V2Di pos);
	Tile &at(const size_t x, const size_t y);
	Tile* operator[](const size_t n);
	bool isValid(V2Di pos); // is a valid position

	// Setup
	void clearTrans();
	void clearLight();
	void prelightToLight();
	void lightToPrelight();
	void init(char light);
	void initTown();
private:
	Tile tiles[MAXDUNX][MAXDUNY];
};

extern Grid grid;

DEVILUTION_END_NAMESPACE

#endif /* __GRID_H__ */
