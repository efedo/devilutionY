/**
 * @file pieces.h
 *
 * Grid pieces
 */
#ifndef __PIECES_H__
#define __PIECES_H__

#include "tile.h"

DEVILUTION_BEGIN_NAMESPACE

class Piece {
public:
	bool block = 0;   // Light blocking
	bool solid = 0;   // Path blocking dPieces
	bool trans = 0;   // Transparent
	bool missile = 0; // List of missile blocking dPieces
	bool trap = 0;
	char block_lvid = 0; // char block_lvid[MAXTILES + 1];
};

class PieceInventory {
public:
	Piece &operator[](const size_t n); // eventually, only allow access via grid/tile
	void FillSolidBlockTbls(DunType duntype);
	void clear();
private:
	DunType _currentDunType;
	//Piece list[MAXTILES + 1];
	std::array<std::unique_ptr<Piece>, 5> lvllists; // list of pieces for each level
};

extern PieceInventory pieces;


DEVILUTION_END_NAMESPACE

#endif /* __PIECES_H__ */
