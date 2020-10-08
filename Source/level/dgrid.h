/**
 * @file dgrid.h
 *
 * Course dungeon grid
 */
#ifndef __DGRID_H__
#define __DGRID_H__

namespace dvl {

class DTile {
public:
	uint8_t dungeon;
	uint8_t pdungeon;
	char dflags;
};

class DGrid {
public:
	DTile * operator[](const size_t n);
	void init();
	void copytopdungeon();
private:
	DTile tiles[DMAXX][DMAXY];
};

extern DGrid dgrid;

}

#endif /* __DGRID_H__ */
