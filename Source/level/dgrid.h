/**
 * @file dgrid.h
 *
 * Course dungeon grid
 */
#ifndef __DGRID_H__
#define __DGRID_H__

DEVILUTION_BEGIN_NAMESPACE

class DTile {
public:
	BYTE dungeon;
	BYTE pdungeon;
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

DEVILUTION_END_NAMESPACE

#endif /* __DGRID_H__ */
