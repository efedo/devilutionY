/**
 * @file tile.h
 *
 * Grid tile class
 */
#ifndef __TILE_H__
#define __TILE_H__

#include <numeric>

DEVILUTION_BEGIN_NAMESPACE

class Tile {
public:
	bool isPlayer() const;
	uint8_t getPlayerNum() const;

	int dPiece;
	MICROS dpiece_defs_map_2;
	char dTransVal;
	char dLight;
	char dPreLight;
	char dFlags;
	int dMonster;
	char dDead;
	char dObject;
	char dItem;
	char dMissile;
	char dSpecial;
private:
	friend class Grid;
	void setPlayerNum(const uint8_t num);
	void setPlayerNumUnsafe(const uint8_t num);
	void clearPlayer(const uint8_t num); // If UINT8_MAX, clears all players, otherwise only clears specific player
	uint8_t getPlayerNumUnsafe() const;

	// Should nest further?
	uint8_t player = UINT8_MAX;
};


DEVILUTION_END_NAMESPACE

#endif /* __TILE_H__ */
