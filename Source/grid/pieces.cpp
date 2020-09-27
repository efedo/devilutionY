/**
 * @file pieces.cpp
 *
 * Grid pieces
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

Piece &PieceInventory::operator[](const size_t n)
{
	return list[n];
}

void PieceInventory::FillSolidBlockTbls()
{
	BYTE bv;
	DWORD dwTiles;
	BYTE *pSBFile, *pTmp;
	int i;

	pieces.clear();

	switch (level.leveltype) {
	case DTYPE_TOWN:
		pSBFile = LoadFileInMem("Levels\\TownData\\Town.SOL", &dwTiles);
		break;
	case DTYPE_CATHEDRAL:
		pSBFile = LoadFileInMem("Levels\\L1Data\\L1.SOL", &dwTiles);
		break;
	case DTYPE_CATACOMBS:
		pSBFile = LoadFileInMem("Levels\\L2Data\\L2.SOL", &dwTiles);
		break;
	case DTYPE_CAVES:
		pSBFile = LoadFileInMem("Levels\\L3Data\\L3.SOL", &dwTiles);
		break;
	case DTYPE_HELL:
		pSBFile = LoadFileInMem("Levels\\L4Data\\L4.SOL", &dwTiles);
		break;
	default:
		app_fatal("FillSolidBlockTbls");
	}

	pTmp = pSBFile;

	for (i = 1; i <= dwTiles; i++) {
		bv = *pTmp++;
		if (bv & 1)
			pieces[i].solid = true;
		if (bv & 2)
			pieces[i].block = true;
		if (bv & 4)
			pieces[i].missile = true;
		if (bv & 8)
			pieces[i].trans = true;
		if (bv & 0x80)
			pieces[i].trap = true;
		block_lvid[i] = (bv & 0x70) >> 4; /* beta: (bv >> 4) & 7 */
	}

	mem_free_dbg(pSBFile);
}

void PieceInventory::clear()
{
	for (int i = 0; i != MAXTILES + 1; ++i) {
		Piece &piece = list[i];
		piece.block = 0;
		piece.solid = 0;
		piece.trans = 0;
		piece.missile = 0;
		piece.trap = 0;
	}
}

PieceInventory pieces;

DEVILUTION_END_NAMESPACE
