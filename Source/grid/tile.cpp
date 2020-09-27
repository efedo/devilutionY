/**
 * @file tile.cpp
 *
 * Implementation of general dungeon generation code.
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

void Tile::setPlayer(const int pnum)
{
	assert(pnum >= 0);
	assert(plr.isValidPlayer(pnum));
	player = pnum; // Check that position is not blocked (or in player code?)
}

void Tile::setPlayerUnsafe(const int pnum)
{
	assert(pnum >= 0);
	player = pnum;
}

void Tile::clearPlayer()
{
	player = UINT8_MAX;
}

void Tile::clearPlayer(const int pnum)
{
	if (player != pnum) return;
	player = UINT8_MAX;
}

void Tile::clearPiece()
{
	piece = UINT8_MAX;
}

void Tile::setPlayerDraw(const int pnum)
{
	assert(pnum >= 0);
	assert(plr.isValidPlayer(pnum));
	drawplayer = pnum; // Check that position is not blocked (or in player code?)
}

void Tile::setPlayerDrawUnsafe(const int pnum)
{
	assert(pnum >= 0);
	drawplayer = pnum;
}

void Tile::clearPlayerDraw()
{
	drawplayer = UINT8_MAX;
}

void Tile::clearPlayerDraw(const int pnum)
{
	if (drawplayer != pnum) return;
	drawplayer = UINT8_MAX;
}

void Tile::clearMonster()
{
	monster = UINT8_MAX;
}

void Tile::clearObject()
{
	object = UINT8_MAX;
}

void Tile::clearItem()
{
	item = UINT8_MAX;
}

void Tile::clearMissile()
{
	//grid.at(missile[mi]._mi).dFlags &= ~BFLAG_MISSILE;
	missile = UINT8_MAX;
}

[[nodiscard]] bool Tile::isPlayer() const
{
	return (player != UINT8_MAX);
}

[[nodiscard]] bool Tile::isPlayerDraw() const
{
	return (drawplayer != UINT8_MAX);
}

[[nodiscard]] bool Tile::isMonster() const
{
	return (monster != UINT8_MAX);
}

[[nodiscard]] bool Tile::isObject() const
{
	return (object != UINT8_MAX);
}

[[nodiscard]] bool Tile::isPiece() const
{
	return (piece != UINT8_MAX);
}

[[nodiscard]] bool Tile::isItem() const
{
	return (item != UINT8_MAX);
}

// Rename this to isPieceSolid and make a new
// isSolid function that also checks if placed objects are solid
[[nodiscard]] bool Tile::isSolid() const
{
	return (pieces[piece].solid);

	//			bv = grid.at(pos).getObject();
	//if (object[bv]._oSolidFlag)
	//	return FALSE;
}

[[nodiscard]] bool Tile::isMissile() const
{
	return (missile != UINT8_MAX);
}

void Tile::setPiece(int pieceNum)
{
	assert(pieceNum >= 0);
	piece = pieceNum;
}

void Tile::setMonster(int monsterNum)
{
	assert(monsterNum >= 0);
	monster = monsterNum;
}

void Tile::setObject(int objectNum)
{
	assert(objectNum >= 0);
	object = objectNum;
}

void Tile::setItem(int itemNum)
{
	assert(itemNum >= 0);
	item = itemNum;
}

void Tile::setMissile(int missileNum)
{
	assert(missileNum >= 0);
	missile = missileNum;
}

[[nodiscard]] bool Tile::blocksMissile() const
{
	return !(pieces[piece].missile);
}

[[nodiscard]] bool Tile::blocksLight() const
{
	return !(pieces[piece].block);
}

[[nodiscard]] bool Tile::isTransparent() const
{
	return !(pieces[piece].trans);
}

[[nodiscard]] bool Tile::isTrap() const
{
	return !(pieces[piece].trap);
}

[[nodiscard]] uint8_t Tile::getPiece() const
{
	assert(piece != UINT8_MAX);
	return piece;
}

[[nodiscard]] uint8_t Tile::getPieceUnsafe() const
{
	return piece;
}

[[nodiscard]] uint16_t Tile::getPlayer() const
{
	assert(player != UINT8_MAX);
	return player;
}

[[nodiscard]] uint16_t Tile::getPlayerDraw() const
{
	assert(drawplayer != UINT8_MAX);
	return drawplayer;
}


[[nodiscard]] uint16_t Tile::getMonster() const
{
	assert(monster != UINT8_MAX);
	return monster;
}

[[nodiscard]] uint8_t Tile::getObject() const
{
	assert(object != UINT8_MAX);
	return object;
}

[[nodiscard]] uint8_t Tile::getItem() const
{
	assert(item != UINT8_MAX);
	return item;
}

[[nodiscard]] uint8_t Tile::getMissile() const
{
	assert(missile != UINT8_MAX);
	return missile;
}

[[nodiscard]] uint8_t Tile::getPlayerUnsafe() const
{
	return player;
}


[[nodiscard]] uint8_t Tile::getPlayerDrawUnsafe() const
{
	return drawplayer;
}

DEVILUTION_END_NAMESPACE
