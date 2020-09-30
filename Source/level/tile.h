/**
 * @file tile.h
 *
 * Grid tile class
 */
#ifndef __TILE_H__
#define __TILE_H__

#include <numeric>

DEVILUTION_BEGIN_NAMESPACE

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
	[[nodiscard]] bool isPlayer() const;
	[[nodiscard]] bool isPlayerDraw() const;
	[[nodiscard]] bool isMonster() const;
	[[nodiscard]] bool isObject() const;
	[[nodiscard]] bool isItem() const;
	[[nodiscard]] bool isMissile() const;

	// Content access
	[[nodiscard]] uint8_t getPiece() const;
	[[nodiscard]] uint16_t getPlayer() const;
	[[nodiscard]] uint16_t getPlayerDraw() const;
	[[nodiscard]] uint16_t getMonster() const;
	[[nodiscard]] uint8_t getObject() const;
	[[nodiscard]] Item * getItem() const;
	[[nodiscard]] std::unique_ptr<Item> &_getItemPtr() const;
	[[nodiscard]] uint8_t getMissile() const;

	// Unsafe content access
	[[nodiscard]] uint8_t getPieceUnsafe() const;

	// Clear content
	void clearPlayer(); // If UINT8_MAX, clears all players, otherwise only clears specific player
	void clearPlayer(const int num); // If UINT8_MAX, clears all players, otherwise only clears specific player
	void clearPlayerDraw(); // If UINT8_MAX, clears all players, otherwise only clears specific player
	void clearPlayerDraw(const int num); // If UINT8_MAX, clears all players, otherwise only clears specific player
	void clearPiece();
	void clearMonster();
	void clearObject();
	void destroyItem();
	void clearMissile();

	// Change content
	void setPlayer(const int num);
	void setPlayerDraw(const int num);
	void setPiece(int pieceNum);
	void _setPieceNegFormat(int pieceNum); // For loading from files etc.
	void setMonster(int monsterNum);
	void setObject(int objectNum);
	void swapItem(std::unique_ptr<Item> &nitem);
	void setMissile(int missileNum);

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

	void setPlayerUnsafe(const int num);
	[[nodiscard]] uint8_t getPlayerUnsafe() const;

	void setPlayerDrawUnsafe(const int num);
	[[nodiscard]] uint8_t getPlayerDrawUnsafe() const;

	uint16_t piece = UINT16_MAX;
	uint8_t player = UINT8_MAX;
	uint8_t drawplayer = UINT8_MAX; // temporary, due to original player rendering being offset from actual player position
	uint16_t monster = UINT16_MAX;
	uint8_t object = UINT8_MAX;
	std::unique_ptr<Item> item;
	uint8_t missile = UINT8_MAX;
};


DEVILUTION_END_NAMESPACE

#endif /* __TILE_H__ */
