/**
 * @file tile.h
 *
 * Grid tile class
 */
#ifndef __TILE_H__
#define __TILE_H__

#include <numeric>
#include <set>

namespace dvl {

class Piece;

class Tile {
public:
	// Piece properties
	[[nodiscard]] bool isSolid() const;
	[[nodiscard]] bool blocksMissile() const;
	[[nodiscard]] bool blocksLight() const;
	[[nodiscard]] bool isTransparent() const;
	[[nodiscard]] bool isTrap() const;
	[[nodiscard]] bool isWall() const;
	[[nodiscard]] bool isWalkable() const;
	[[nodiscard]] bool canPutItem() const;

	// Content checks
	[[nodiscard]] bool isPiece() const;
	[[nodiscard]] bool isObject() const;
	[[nodiscard]] bool isItem() const;
	[[nodiscard]] bool isMissile() const;
	[[nodiscard]] bool isActor() const;
	[[nodiscard]] bool isPlayer() const;
	[[nodiscard]] bool isMonster() const;
	[[nodiscard]] bool isTowner() const;
	[[nodiscard]] bool isActorDraw() const;

	// Content access
	[[nodiscard]] uint8_t					getPiece() const;
	[[nodiscard]] Actor &					getActor() const;
	[[nodiscard]] Player &					getPlayer() const;
	[[nodiscard]] Monster &					getMonster() const;
	[[nodiscard]] Towner &					getTowner() const;
	[[nodiscard]] const std::set<ActorId> &	getActorDrawSet() const;
	[[nodiscard]] uint8_t					getObject() const;
	[[nodiscard]] Item &					getItem() const;
	[[nodiscard]] uint8_t					getMissile() const;

	// Unsafe content access
	[[nodiscard]] uint8_t getPieceUnsafe() const;

	// Clear content
	void clearActor(ActorId id);
	void clearActorDraw(ActorId id);
	void clearPiece();
	void clearObject();
	void destroyItem();
	void clearMissile();

	// Change content
	void setActor(ActorId id);
	void setActorDraw(ActorId id);
	void setPiece(int pieceNum);
	void setObject(int objectNum);
	void swapItem(std::unique_ptr<Item> &nitem);
	void setMissile(int missileNum);

	// For rendering
	std::unique_ptr<std::set<ActorId>> drawActors = 0; // list of actors whose rendering is deferred TO this tile

	MICROS dpiece_defs_map_2;
	char dTransVal;
	char dLight;
	char dPreLight;
	char dFlags;
	char dDead;
	char dSpecial;
private:
	friend class Grid;
	friend void SaveGame();
	friend void LoadGame(bool);

	uint16_t piece = UINT16_MAX;
	uint8_t object = UINT8_MAX;
	uint8_t missile = UINT8_MAX;
	ActorId actor = ActorId::getInvalid();
	ActorId drawactor = ActorId::getInvalid();
	std::unique_ptr<Item> item;
};


}

#endif /* __TILE_H__ */
