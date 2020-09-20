/**
 * @file tile.cpp
 *
 * Implementation of general dungeon generation code.
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

void Tile::setPlayerNum(const uint8_t pnum)
{
	assert(plr.isValidPlayer(num));
	// Check that position is not blocked (or in player code?)
	player = num;
}

void Tile::setPlayerNumUnsafe(const uint8_t pnum)
{
	player = num;
}

void Tile::clearPlayer(const uint8_t pnum)
{
	assert(player == pnum);
	player = UINT8_MAX;
}

bool Tile::isPlayer() const
{
	return (player != UINT8_MAX);
}

uint8_t Tile::getPlayerNum() const
{
	assert(player != UINT8_MAX);
	return player;
}

uint8_t Tile::getPlayerNumUnsafe() const
{
	return player;
}
DEVILUTION_END_NAMESPACE
