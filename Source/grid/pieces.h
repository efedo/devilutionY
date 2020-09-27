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
	bool block;   // Light blocking
	bool solid;   // Path blocking dPieces
	bool trans;   // Transparent
	bool missile; // List of missile blocking dPieces
	bool trap;
};

class PieceInventory {
public:
	Piece &operator[](const size_t n); // eventually, only allow access via grid/tile
	void FillSolidBlockTbls();
	void clear();
private:
	Piece list[MAXTILES + 1];
};

extern PieceInventory pieces;


DEVILUTION_END_NAMESPACE

#endif /* __PIECES_H__ */
