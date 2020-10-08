/**
 * @file tile.cpp
 *
 * Implementation of general dungeon generation code.
 */
#include "all.h"
#include <functional>

namespace dvl {

void Tile::setActor(ActorId id)
{
	actor = id;  // Check that position is not blocked (or in player code?)
}

void Tile::setActorDraw(ActorId id)
{
	if (!this->drawActors) {
		this->drawActors.reset(new std::set<ActorId>);
	}
	if (this->drawActors->count(id)) return;
	this->drawActors->insert(id);
}

void Tile::clearActor(ActorId id)
{
	if (actor != id) return;
	actor.setInvalid();
}

void Tile::clearActorDraw(ActorId id)
{
	if (!this->drawActors) return;
	if (this->drawActors->count(id)) {
		this->drawActors->erase(id);
	}

	if (this->drawActors->empty()) {
		this->drawActors = 0;
	}
}

void Tile::clearPiece()
{
	piece = UINT16_MAX;
}

void Tile::clearObject()
{
	object = UINT8_MAX;
}

void Tile::destroyItem()
{
	item.reset();
}

void Tile::clearMissile()
{
	//grid.at(missile[mi]._mi).dFlags &= ~DunTileFlag::MISSILE;
	missile = UINT8_MAX;
}

[[nodiscard]] bool Tile::isPlayer() const
{
	if (!actor.isValid()) return false;
	return (actors.getPlayer(actor));
}

[[nodiscard]] bool Tile::isMonster() const
{
	if (!actor.isValid()) return false;
	return (actors.getMonster(actor));
}

[[nodiscard]] bool Tile::isTowner() const
{
	if (!actor.isValid()) return false;
	return (actors.getNpc(actor));
}

[[nodiscard]] bool Tile::isActorDraw() const
{
	return (this->drawActors != 0);
}

[[nodiscard]] bool Tile::isActor() const
{
	return actor.isValid();
	return (actors.getActor(actor));
}

[[nodiscard]] bool Tile::isObject() const
{
	return (object != UINT8_MAX);
}

[[nodiscard]] bool Tile::isPiece() const
{
	return (piece != UINT16_MAX);
}

[[nodiscard]] bool Tile::isItem() const
{
	return (item.get());
}

// Rename this to isPieceSolid and make a new
// isSolid function that also checks if placed objects are solid
[[nodiscard]] bool Tile::isSolid() const
{
	return (pieces[piece].solid);

	//			bv = grid.at(pos).getObject();
	//if (object[bv]._oSolidFlag)
	//	return false;
}

[[nodiscard]] bool Tile::isMissile() const
{
	return (missile != UINT8_MAX);
}

void Tile::setPiece(int pieceNum)
{
	assert(pieceNum >= 0 && pieceNum < UINT16_MAX);
	piece = pieceNum;
}

void Tile::setObject(int objectNum)
{
	assert(objectNum >= 0 && objectNum < UINT8_MAX);
	object = objectNum;
}

void Tile::swapItem(std::unique_ptr<Item> & nitem)
{
	item.swap(nitem);
}

void Tile::setMissile(int missileNum)
{
	assert(missileNum >= 0 && missileNum < UINT8_MAX);
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

[[nodiscard]] bool Tile::isWall() const
{
	//#define IsWall(x, y) (!grid[x][y].isPiece() || grid[x][y].isSolid() || grid[x][y].dSpecial != 0)
	if (!isPiece()) return true;
	if (isSolid()) return true;
	if (dSpecial != 0) return true;
	return false;
}

[[nodiscard]] bool Tile::isWalkable() const
{
	//#define IsWalkable(x, y) (grid[x][y].isPiece() && !grid[x][y].isSolid())
	return isPiece() && !isSolid();
}


[[nodiscard]] bool Tile::canPutItem() const
{
	if (isItem()) return false;
	if (isSolid()) return false;
	if (isActor()) return false;
	if (isObject()) {
		int oi = getObject();
		ObjectStruct & obj = ::dvl::object[oi];
		if (obj._oSolidFlag) return false;
		if (obj._oSelFlag) return false;
	}
	return true;
}

[[nodiscard]] uint8_t Tile::getPiece() const
{
	assert(piece != UINT16_MAX);
	return piece;
}

[[nodiscard]] uint8_t Tile::getPieceUnsafe() const
{
	return piece;
}

[[nodiscard]] Actor& Tile::getActor() const
{
	assert(actor.isValid());
	assert(actors.getActor(actor));
	return *actors.getActor(actor);
}

[[nodiscard]] Player & Tile::getPlayer() const
{
	assert(actor.isValid());
	assert(actors.getPlayer(actor));
	return *actors.getPlayer(actor);
}

[[nodiscard]] Monster& Tile::getMonster() const
{
	assert(actor.isValid());
	assert(actors.getMonster(actor));
	return *actors.getMonster(actor);
}

[[nodiscard]] Towner& Tile::getTowner() const
{
	assert(actor.isValid());
	assert(actors.getNpc(actor));
	return *actors.getNpc(actor);
}

[[nodiscard]] const std::set<ActorId>& Tile::getActorDrawSet() const
{
	assert(drawActors);
	return *drawActors;
}

[[nodiscard]] uint8_t Tile::getObject() const
{
	assert(object != UINT8_MAX);
	return object;
}

[[nodiscard]] Item & Tile::getItem() const
{
	assert(item);
	return *item;
}

[[nodiscard]] uint8_t Tile::getMissile() const
{
	assert(missile != UINT8_MAX);
	return missile;
}

}
