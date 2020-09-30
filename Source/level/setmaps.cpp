#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

// BUGFIX: constant data should be const
uint8_t SkelKingTrans1[] = {
	19, 47, 26, 55,
	26, 49, 30, 53
};

uint8_t SkelKingTrans2[] = {
	33, 19, 47, 29,
	37, 29, 43, 39
};

uint8_t SkelKingTrans3[] = {
	27, 53, 35, 61,
	27, 35, 34, 42,
	45, 35, 53, 43,
	45, 53, 53, 61,
	31, 39, 49, 57
};

uint8_t SkelKingTrans4[] = {
	49, 45, 58, 51,
	57, 31, 62, 37,
	63, 31, 69, 40,
	59, 41, 73, 55,
	63, 55, 69, 65,
	73, 45, 78, 51,
	79, 43, 89, 53
};

uint8_t SkelChamTrans1[] = {
	43, 19, 50, 26,
	51, 19, 59, 26,
	35, 27, 42, 34,
	43, 27, 49, 34,
	50, 27, 59, 34
};

uint8_t SkelChamTrans2[] = {
	19, 31, 34, 47,
	34, 35, 42, 42
};

uint8_t SkelChamTrans3[] = {
	43, 35, 50, 42,
	51, 35, 62, 42,
	63, 31, 66, 46,
	67, 31, 78, 34,
	67, 35, 78, 42,
	67, 43, 78, 46,
	35, 43, 42, 51,
	43, 43, 49, 51,
	50, 43, 59, 51
};

/** Maps from quest level to quest level names. */
char *quest_level_names[] = {
	"",
	"Skeleton King's Lair",
	"Bone Chamber",
	"Maze",
	"Poisoned Water Supply",
	"Archbishop Lazarus' Lair",
};

int ObjIndex(int x, int y)
{
	int i;
	int oi;

	for (i = 0; i < nobjects; i++) {
		oi = objectactive[i];
		if (object[oi]._o.x == x && object[oi]._o.y == y)
			return oi;
	}
	app_fatal("ObjIndex: Active object not found at (%d,%d)", x, y);
	return -1;
}

void AddSKingObjs()
{
	SetObjMapRange(ObjIndex(64, 34), { 20, 7 }, { 23, 10 }, 1);
	SetObjMapRange(ObjIndex(64, 59), { 20, 14 }, { 21, 16 }, 2);
	SetObjMapRange(ObjIndex(27, 37), { 8, 1 }, { 15, 11 }, 3);
	SetObjMapRange(ObjIndex(46, 35), { 8, 1 }, { 15, 11 }, 3);
	SetObjMapRange(ObjIndex(49, 53), { 8, 1 }, { 15, 11 }, 3);
	SetObjMapRange(ObjIndex(27, 53), { 8, 1 }, { 15, 11 }, 3);
}

void AddSChamObjs()
{
	SetObjMapRange(ObjIndex(37, 30), { 17, 0 }, { 21, 5 }, 1);
	SetObjMapRange(ObjIndex(37, 46), { 13, 0 }, { 16, 5 }, 2);
}

void AddVileObjs()
{
	SetObjMapRange(ObjIndex(26, 45), { 1, 1 }, { 9, 10 }, 1);
	SetObjMapRange(ObjIndex(45, 46), { 11, 1 }, { 20, 10 }, 2);
	SetObjMapRange(ObjIndex(35, 36), { 7, 11 }, { 13, 18 }, 3);
}

void DRLG_SetMapTrans(char *sFileName)
{
	int x, y;
	int i, j;
	uint8_t *pLevelMap;
	uint8_t *d;
	DWORD dwOffset;

	pLevelMap = LoadFileInMem(sFileName, NULL);
	d = pLevelMap + 2;
	x = pLevelMap[0];
	y = *d;
	dwOffset = (x * y + 1) * 2;
	x <<= 1;
	y <<= 1;
	dwOffset += 3 * x * y * 2;
	d += dwOffset;

	for (j = 0; j < y; j++) {
		for (i = 0; i < x; i++) {
			grid[16 + i][16 + j].dTransVal = *d;
			d += 2;
		}
	}
	mem_free_dbg(pLevelMap);
}

void LoadSetMap()
{
	lvl.LoadSetMap();
}

DEVILUTION_END_NAMESPACE
