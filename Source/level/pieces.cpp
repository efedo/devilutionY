/**
 * @file pieces.cpp
 *
 * Grid pieces
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

Piece &PieceInventory::operator[](const size_t n)
{
	return lvllists[int(_currentDunType)].get()[n];
}

void PieceInventory::FillSolidBlockTbls(DunType duntype)
{
	uint8_t bv;
	DWORD dwTiles;
	uint8_t *pSBFile, *pTmp;
	int i;

	this->clear();
	this->lvllists[int(duntype)].reset(new Piece[MAXTILES + 1]);
	this->_currentDunType = duntype;

	switch (duntype) {
	case DunType::town:
		pSBFile = LoadFileInMem("Levels\\TownData\\Town.SOL", &dwTiles);
		break;
	case DunType::cathedral:
		pSBFile = LoadFileInMem("Levels\\L1Data\\L1.SOL", &dwTiles);
		break;
	case DunType::catacombs:
		pSBFile = LoadFileInMem("Levels\\L2Data\\L2.SOL", &dwTiles);
		break;
	case DunType::caves:
		pSBFile = LoadFileInMem("Levels\\L3Data\\L3.SOL", &dwTiles);
		break;
	case DunType::hell:
		pSBFile = LoadFileInMem("Levels\\L4Data\\L4.SOL", &dwTiles);
		break;
	default:
		app_fatal("FillSolidBlockTbls");
	}

	pTmp = pSBFile;

	for (i = 0; i < dwTiles; i++) {
	//for (i = 1; i <= dwTiles; i++) {
		bv = *pTmp++;
		if (bv & 1)
			this->operator[](i).solid = true;
		if (bv & 2)
			this->operator[](i).block = true;
		if (bv & 4)
			this->operator[](i).missile = true;
		if (bv & 8)
			this->operator[](i).trans = true;
		if (bv & 0x80)
			this->operator[](i).trap = true;
		this->operator[](i).block_lvid = (bv & 0x70) >> 4; /* beta: (bv >> 4) & 7 */
	}

	mem_free_dbg(pSBFile);
}

void PieceInventory::clear()
{
	for (auto it = lvllists.begin(); it != lvllists.end(); ++it) {
		it->reset();
	}

	//for (int i = 0; i != MAXTILES + 1; ++i) {
	//	Piece &piece = list[i];
	//	piece.block = 0;
	//	piece.solid = 0;
	//	piece.trans = 0;
	//	piece.missile = 0;
	//	piece.trap = 0;
	//}
}

PieceInventory pieces;

DEVILUTION_END_NAMESPACE
