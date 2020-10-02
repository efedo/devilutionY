/**
 * @file themes.cpp
 *
 * Implementation of the theme room placing algorithms.
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

int numthemes;
bool armorFlag;
bool ThemeGoodIn[4];
bool weaponFlag;
bool treasureFlag;
bool mFountainFlag;
bool cauldronFlag;
bool tFountainFlag;
int zharlib;
int themex;
int themey;
int themeVar1;
ThemeStruct themes[MAXTHEMES];
bool pFountainFlag;
bool bFountainFlag;
bool bCrossFlag;

int ThemeGood[4] = { THEME_GOATSHRINE, THEME_SHRINE, THEME_SKELROOM, THEME_LIBRARY };

int trm5x[] = {
	-2, -1, 0, 1, 2,
	-2, -1, 0, 1, 2,
	-2, -1, 0, 1, 2,
	-2, -1, 0, 1, 2,
	-2, -1, 0, 1, 2
};

int trm5y[] = {
	-2, -2, -2, -2, -2,
	-1, -1, -1, -1, -1,
	0, 0, 0, 0, 0,
	1, 1, 1, 1, 1,
	2, 2, 2, 2, 2
};

int trm3x[] = {
	-1, 0, 1,
	-1, 0, 1,
	-1, 0, 1
};

int trm3y[] = {
	-1, -1, -1,
	0, 0, 0,
	1, 1, 1
};

bool TFit_Shrine(int i)
{
	int xp, yp, found;

	xp = 0;
	yp = 0;
	found = 0;
	while (found == 0) {
		if (grid[xp][yp].dTransVal == themes[i].ttval) {
			if (grid[xp][yp - 1].isTrap()
			    && !grid[xp - 1][yp].isSolid()
			    && !grid[xp + 1][yp].isSolid()
			    && grid[xp - 1][yp].dTransVal == themes[i].ttval
			    && grid[xp + 1][yp].dTransVal == themes[i].ttval
			    && !grid[xp - 1][yp - 1].isObject()
			    && !grid[xp + 1][yp - 1].isObject()) {
				found = 1;
			}
			if (found == 0
			    && grid[xp - 1][yp].isTrap()
			    && !grid[xp][yp - 1].isSolid()
			    && !grid[xp][yp + 1].isSolid()
			    && grid[xp][yp - 1].dTransVal == themes[i].ttval
			    && grid[xp][yp + 1].dTransVal == themes[i].ttval
			    && !grid[xp - 1][yp - 1].isObject()
			    && !grid[xp - 1][yp + 1].isObject()) {
				found = 2;
			}
		}
		if (found == 0) {
			xp++;
			if (xp == MAXDUNX) {
				xp = 0;
				yp++;
				if (yp == MAXDUNY)
					return false;
			}
		}
	}
	themex = xp;
	themey = yp;
	themeVar1 = found;
	return true;
}

bool TFit_Obj5(int t)
{
	int xp, yp;
	int i, r, rs;
	bool found;

	xp = 0;
	yp = 0;
	r = random_(0, 5) + 1;
	rs = r;
	while (r > 0) {
		found = false;
		if (grid[xp][yp].dTransVal == themes[t].ttval && !grid[xp][yp].isSolid()) {
			found = true;
			for (i = 0; found && i < 25; i++) {
				if (grid[xp + trm5x[i]][yp + trm5y[i]].isSolid()) {
					found = false;
				}
				if (grid[xp + trm5x[i]][yp + trm5y[i]].dTransVal != themes[t].ttval) {
					found = false;
				}
			}
		}

		if (!found) {
			xp++;
			if (xp == MAXDUNX) {
				xp = 0;
				yp++;
				if (yp == MAXDUNY) {
					if (r == rs) {
						return false;
					}
					yp = 0;
				}
			}
			continue;
		}

		r--;
	}

	themex = xp;
	themey = yp;

	return true;
}

bool TFit_SkelRoom(int t)
{
	int i;

	if (lvl.type() != DunType::cathedral && lvl.type() != DunType::catacombs) {
		return false;
	}

	for (i = 0; i < nummtypes; i++) {
		if (IsSkel(beastiary[i].data.mtype)) {
			themeVar1 = i;
			return TFit_Obj5(t);
		}
	}

	return false;
}

bool TFit_GoatShrine(int t)
{
	int i;

	for (i = 0; i < nummtypes; i++) {
		if (IsGoat(beastiary[i].data.mtype)) {
			themeVar1 = i;
			return TFit_Obj5(t);
		}
	}

	return false;
}

bool CheckThemeObj3(int xp, int yp, int t, int f)
{
	for (int i = 0; i < 9; i++) {
		V2Di p = { xp + trm3x[i], yp + trm3y[i] };
		if (p.x < 0 || p.y < 0)
			return false;
		if (grid.at(p).isSolid())
			return false;
		if (grid.at(p).dTransVal != themes[t].ttval)
			return false;
		if (grid.at(p).isObject())
			return false;
		if (f != -1 && !random_(0, f))
			return false;
	}
	return true;
}

bool TFit_Obj3(int t)
{
	char objrnd[4] = { 4, 4, 3, 5 };
	for (int yp = 1; yp < MAXDUNY - 1; yp++) {
		for (int xp = 1; xp < MAXDUNX - 1; xp++) {
			if (CheckThemeObj3(xp, yp, t, objrnd[int(lvl.type()) - 1])) {
				themex = xp;
				themey = yp;
				return true;
			}
		}
	}
	return false;
}

bool CheckThemeReqs(int t)
{
	bool rv = true;
	switch (t) {
	case THEME_SHRINE:
	case THEME_SKELROOM:
	case THEME_LIBRARY:
		if (lvl.type() == DunType::caves || lvl.type() == DunType::hell) {
			rv = false;
		}
		break;
	case THEME_BLOODFOUNTAIN:
		if (!bFountainFlag) {
			rv = false;
		}
		break;
	case THEME_PURIFYINGFOUNTAIN:
		if (!pFountainFlag) {
			rv = false;
		}
		break;
	case THEME_ARMORSTAND:
		if (lvl.type() == DunType::cathedral) {
			rv = false;
		}
		break;
	case THEME_CAULDRON:
		if (lvl.type() != DunType::hell || !cauldronFlag) {
			rv = false;
		}
		break;
	case THEME_MURKYFOUNTAIN:
		if (!mFountainFlag) {
			rv = false;
		}
		break;
	case THEME_TEARFOUNTAIN:
		if (!tFountainFlag) {
			rv = false;
		}
		break;
	case THEME_WEAPONRACK:
		if (lvl.type() == DunType::cathedral) {
			rv = false;
		}
		break;
	}

	return rv;
}

bool SpecialThemeFit(int i, int t)
{
	bool rv = CheckThemeReqs(t);
	switch (t) {
	case THEME_SHRINE:
	case THEME_LIBRARY:
		if (rv) {
			rv = TFit_Shrine(i);
		}
		break;
	case THEME_SKELROOM:
		if (rv) {
			rv = TFit_SkelRoom(i);
		}
		break;
	case THEME_BLOODFOUNTAIN:
		if (rv) {
			rv = TFit_Obj5(i);
		}
		if (rv) {
			bFountainFlag = false;
		}
		break;
	case THEME_PURIFYINGFOUNTAIN:
		if (rv) {
			rv = TFit_Obj5(i);
		}
		if (rv) {
			pFountainFlag = false;
		}
		break;
	case THEME_MURKYFOUNTAIN:
		if (rv) {
			rv = TFit_Obj5(i);
		}
		if (rv) {
			mFountainFlag = false;
		}
		break;
	case THEME_TEARFOUNTAIN:
		if (rv) {
			rv = TFit_Obj5(i);
		}
		if (rv) {
			tFountainFlag = false;
		}
		break;
	case THEME_CAULDRON:
		if (rv) {
			rv = TFit_Obj5(i);
		}
		if (rv) {
			cauldronFlag = false;
		}
		break;
	case THEME_GOATSHRINE:
		if (rv) {
			rv = TFit_GoatShrine(i);
		}
		break;
	case THEME_TORTURE:
	case THEME_DECAPITATED:
	case THEME_ARMORSTAND:
	case THEME_BRNCROSS:
	case THEME_WEAPONRACK:
		if (rv) {
			rv = TFit_Obj3(i);
		}
		break;
	case THEME_TREASURE:
		rv = treasureFlag;
		if (rv) {
			treasureFlag = false;
		}
		break;
	}
	return rv;
}

bool CheckThemeRoom(int tv)
{
	for (int i = 0; i < numtrigs; i++) {
		if (grid.at(trigs[i]._t).dTransVal == tv)
			return false;
	}

	int tarea = 0;

	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++) {
			if (grid[i][j].dTransVal != tv)
				continue;
			if (grid[i][j].dFlags & BFLAG_POPULATED)
				return false;
			tarea++;
		}
	}

	if (lvl.type() == DunType::cathedral && (tarea < 9 || tarea > 100))
		return false;

	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++) {
			if (grid[i][j].dTransVal != tv || grid[i][j].isSolid())
				continue;
			if (grid[i - 1][j].dTransVal != tv && !grid[i - 1][j].isSolid())
				return false;
			if (grid[i + 1][j].dTransVal != tv && !grid[i + 1][j].isSolid())
				return false;
			if (grid[i][j - 1].dTransVal != tv && !grid[i][j - 1].isSolid())
				return false;
			if (grid[i][j + 1].dTransVal != tv && !grid[i][j + 1].isSolid())
				return false;
		}
	}

	return true;
}

void InitThemes()
{
	int i, j;

	zharlib = -1;
	numthemes = 0;
	armorFlag = true;
	bFountainFlag = true;
	cauldronFlag = true;
	mFountainFlag = true;
	pFountainFlag = true;
	tFountainFlag = true;
	treasureFlag = true;
	bCrossFlag = false;
	weaponFlag = true;

	if (lvl.currlevel == 16)
		return;

	if (lvl.type() == DunType::cathedral) {
		for (i = 0; i < sizeof(ThemeGoodIn) / sizeof(ThemeGoodIn[0]); i++)
			ThemeGoodIn[i] = false;

		for (i = 0; i < 256 && numthemes < MAXTHEMES; i++) {
			if (CheckThemeRoom(i)) {
				themes[numthemes].ttval = i;
				for (j = ThemeGood[random_(0, 4)];; j = random_(0, 17)) {
					if (SpecialThemeFit(numthemes, j)) {
						break;
					}
				}
				themes[numthemes].ttype = j;
				numthemes++;
			}
		}
	}
	if (lvl.type() == DunType::catacombs || lvl.type() == DunType::caves || lvl.type() == DunType::hell) {
		for (i = 0; i < lvl.themeCount; i++)
			themes[i].ttype = THEME_NONE;
		if (QuestStatus(Q_ZHAR)) {
			for (j = 0; j < lvl.themeCount; j++) {
				themes[j].ttval = lvl.themeLoc[j].ttval;
				if (SpecialThemeFit(j, THEME_LIBRARY)) {
					themes[j].ttype = THEME_LIBRARY;
					zharlib = j;
					break;
				}
			}
		}
		for (i = 0; i < lvl.themeCount; i++) {
			if (themes[i].ttype == THEME_NONE) {
				themes[i].ttval = lvl.themeLoc[i].ttval;
				for (j = ThemeGood[random_(0, 4)];; j = random_(0, 17)) {
					if (SpecialThemeFit(i, j)) {
						break;
					}
				}
				themes[i].ttype = j;
			}
		}
		numthemes += lvl.themeCount;
	}
}

/**
 * @brief HoldThemeRooms marks theme rooms as populated.
 */
void HoldThemeRooms()
{
	int i, x, y;
	char v;

	if (lvl.currlevel != 16) {
		if (lvl.type() == DunType::cathedral) {
			for (i = 0; i < numthemes; i++) {
				v = themes[i].ttval;
				for (y = 0; y < MAXDUNY; y++) {
					for (x = 0; x < MAXDUNX; x++) {
						if (grid[x][y].dTransVal == v) {
							grid[x][y].dFlags |= BFLAG_POPULATED;
						}
					}
				}
			}
		} else {
			DRLG_HoldThemeRooms();
		}
	}
}

/**
 * PlaceThemeMonsts places theme monsters with the specified frequency.
 *
 * @param t theme number (index into themes array).
 * @param f frequency (1/f likelihood of adding monster).
 */
void PlaceThemeMonsts(int t, int f)
{
	V2Di p;
	int scattertypes[111];
	int numscattypes, mtype, i;

	numscattypes = 0;
	for (i = 0; i < nummtypes; i++) {
		if (beastiary[i].data.mPlaceFlags & 1) {
			scattertypes[numscattypes] = i;
			numscattypes++;
		}
	}
	mtype = scattertypes[random_(0, numscattypes)];
	for (p.y = 0; p.y < MAXDUNY; p.y++) {
		for (p.x = 0; p.x < MAXDUNX; p.x++) {
			if (grid.at(p).dTransVal == themes[t].ttval && !grid.at(p).isSolid() && !grid.at(p).isItem() && !grid.at(p).isObject()) {
				if (random_(0, f) == 0) {
					AddMonster(p , Dir(random_(0, 8)), mtype, true);
				}
			}
		}
	}
}

/**
 * Theme_Barrel initializes the barrel theme.
 *
 * @param t theme number (index into themes array).
 */
void Theme_Barrel(int t)
{
	int xp, yp, r;
	char barrnd[4] = { 2, 6, 4, 8 };
	char monstrnd[4] = { 5, 7, 3, 9 };

	for (yp = 0; yp < MAXDUNY; yp++) {
		for (xp = 0; xp < MAXDUNX; xp++) {
			if (grid[xp][yp].dTransVal == themes[t].ttval && !grid[xp][yp].isSolid()) {
				if (random_(0, barrnd[int(lvl.type()) - 1]) == 0) {
					if (random_(0, barrnd[int(lvl.type()) - 1]) == 0) {
						r = OBJ_BARREL;
					} else {
						r = OBJ_BARRELEX;
					}
					AddObject(r, xp, yp);
				}
			}
		}
	}
	PlaceThemeMonsts(t, monstrnd[int(lvl.type()) - 1]);
}

/**
 * Theme_Shrine initializes the shrine theme.
 *
 * @param t theme number (index into themes array).
 */
void Theme_Shrine(int t)
{
	char monstrnd[4] = { 6, 6, 3, 9 };

	TFit_Shrine(t);
	if (themeVar1 == 1) {
		AddObject(OBJ_CANDLE2, themex - 1, themey);
		AddObject(OBJ_SHRINER, themex, themey);
		AddObject(OBJ_CANDLE2, themex + 1, themey);
	} else {
		AddObject(OBJ_CANDLE2, themex, themey - 1);
		AddObject(OBJ_SHRINEL, themex, themey);
		AddObject(OBJ_CANDLE2, themex, themey + 1);
	}
	PlaceThemeMonsts(t, monstrnd[int(lvl.type()) - 1]);
}

/**
 * Theme_MonstPit initializes the monster pit theme.
 *
 * @param t theme number (index into themes array).
 */
void Theme_MonstPit(int t)
{
	int r;
	int ixp, iyp;
	char monstrnd[4] = { 6, 7, 3, 9 };

	r = random_(0, 100) + 1;
	ixp = 0;
	iyp = 0;
	if (r > 0) {
		while (true) {
			if (grid[ixp][iyp].dTransVal == themes[t].ttval && !grid[ixp][iyp].isSolid()) {
				--r;
			}
			if (r <= 0) {
				break;
			}
			ixp++;
			if (ixp == MAXDUNX) {
				ixp = 0;
				iyp++;
				if (iyp == MAXDUNY) {
					iyp = 0;
				}
			}
		}
	}
	CreateRndItem({ ixp, iyp }, true, false, true);
	ItemNoFlippy();
	PlaceThemeMonsts(t, monstrnd[int(lvl.type()) - 1]);
}

/**
 * Theme_SkelRoom initializes the skeleton room theme.
 *
 * @param t theme number (index into themes array).
 */
void Theme_SkelRoom(int t)
{
	int xp, yp, i;
	char monstrnd[4] = { 6, 7, 3, 9 };

	TFit_SkelRoom(t);

	xp = themex;
	yp = themey;

	AddObject(OBJ_SKFIRE, xp, yp);

	if (random_(0, monstrnd[int(lvl.type()) - 1]) != 0) {
		i = PreSpawnSkeleton();
		SpawnSkeleton(i, { xp - 1, yp - 1 });
	} else {
		AddObject(OBJ_BANNERL, xp - 1, yp - 1);
	}

	i = PreSpawnSkeleton();
	SpawnSkeleton(i, { xp, yp - 1 });

	if (random_(0, monstrnd[int(lvl.type()) - 1]) != 0) {
		i = PreSpawnSkeleton();
		SpawnSkeleton(i, { xp + 1, yp - 1 });
	} else {
		AddObject(OBJ_BANNERR, xp + 1, yp - 1);
	}
	if (random_(0, monstrnd[int(lvl.type()) - 1]) != 0) {
		i = PreSpawnSkeleton();
		SpawnSkeleton(i, { xp - 1, yp });
	} else {
		AddObject(OBJ_BANNERM, xp - 1, yp);
	}
	if (random_(0, monstrnd[int(lvl.type()) - 1]) != 0) {
		i = PreSpawnSkeleton();
		SpawnSkeleton(i, { xp + 1, yp });
	} else {
		AddObject(OBJ_BANNERM, xp + 1, yp);
	}
	if (random_(0, monstrnd[int(lvl.type()) - 1]) != 0) {
		i = PreSpawnSkeleton();
		SpawnSkeleton(i, { xp - 1, yp + 1 });
	} else {
		AddObject(OBJ_BANNERR, xp - 1, yp + 1);
	}

	i = PreSpawnSkeleton();
	SpawnSkeleton(i, { xp, yp + 1 });

	if (random_(0, monstrnd[int(lvl.type()) - 1]) != 0) {
		i = PreSpawnSkeleton();
		SpawnSkeleton(i, { xp + 1, yp + 1 });
	} else {
		AddObject(OBJ_BANNERL, xp + 1, yp + 1);
	}

	if (!grid[xp][yp - 3].isObject()) {
		AddObject(OBJ_SKELBOOK, xp, yp - 2);
	}
	if (!grid[xp][yp + 3].isObject()) {
		AddObject(OBJ_SKELBOOK, xp, yp + 2);
	}
}

/**
 * Theme_Treasure initializes the treasure theme.
 *
 * @param t theme number (index into themes array).
 */
void Theme_Treasure(int t)
{
	int xp, yp;
	int i;
	char treasrnd[4] = { 4, 9, 7, 10 };
	char monstrnd[4] = { 6, 8, 3, 7 };

	GetRndSeed();
	for (yp = 0; yp < MAXDUNY; yp++) {
		for (xp = 0; xp < MAXDUNX; xp++) {
			if (grid[xp][yp].dTransVal == themes[t].ttval && !grid[xp][yp].isSolid()) {
				int rv = random_(0, treasrnd[int(lvl.type()) - 1]);
				// BUGFIX: the `2*` in `2*random_(0, treasrnd...) == 0` has no effect, should probably be `random_(0, 2*treasrnd...) == 0`
				if ((2 * random_(0, treasrnd[int(lvl.type()) - 1])) == 0) {
					CreateTypeItem({ xp, yp }, false, ItemType::gold, MiscItemId::NONE, false, true);
					ItemNoFlippy();
				}
				if (rv == 0) {
					CreateRndItem({ xp, yp }, false, false, true);
					ItemNoFlippy();
				}
				if (rv == 0 || rv >= treasrnd[int(lvl.type()) - 1] - 2) {
					i = ItemNoFlippy();
					if (rv >= treasrnd[int(lvl.type()) - 1] - 2 && lvl.type() != DunType::cathedral) {
						item[i]._ivalue >>= 1;
					}
				}
			}
		}
	}
	PlaceThemeMonsts(t, monstrnd[int(lvl.type()) - 1]);
}

/**
 * Theme_Library initializes the library theme.
 *
 * @param t theme number (index into themes array).
 */
void Theme_Library(int t)
{
	int xp, yp, oi;
	char librnd[4] = { 1, 2, 2, 5 };
	char monstrnd[4] = { 5, 7, 3, 9 };

	TFit_Shrine(t);

	if (themeVar1 == 1) {
		AddObject(OBJ_BOOKCANDLE, themex - 1, themey);
		AddObject(OBJ_BOOKCASER, themex, themey);
		AddObject(OBJ_BOOKCANDLE, themex + 1, themey);
	} else {
		AddObject(OBJ_BOOKCANDLE, themex, themey - 1);
		AddObject(OBJ_BOOKCASEL, themex, themey);
		AddObject(OBJ_BOOKCANDLE, themex, themey + 1);
	}

	for (yp = 1; yp < MAXDUNY - 1; yp++) {
		for (xp = 1; xp < MAXDUNX - 1; xp++) {
			if (CheckThemeObj3(xp, yp, t, -1) && grid[xp][yp].getActor() == 0 && random_(0, librnd[int(lvl.type()) - 1]) == 0) {
				AddObject(OBJ_BOOKSTAND, xp, yp);
				if (random_(0, 2 * librnd[int(lvl.type()) - 1]) != 0 && grid[xp][yp].isObject()) { /// BUGFIX: check grid[xp][yp].dObject was populated by AddObject (fixed)
					oi = grid[xp][yp].getObject();
					object[oi]._oSelFlag = 0;
					object[oi]._oAnimFrame += 2;
				}
			}
		}
	}

	if (QuestStatus(Q_ZHAR)) {
		if (t == zharlib) {
			return;
		}
		PlaceThemeMonsts(t, monstrnd[int(lvl.type())]); /// BUGFIX: `int(lvl.type()) - 1`
	} else {
		PlaceThemeMonsts(t, monstrnd[int(lvl.type())]); /// BUGFIX: `int(lvl.type()) - 1`
	}
}

/**
 * Theme_Torture initializes the torture theme.
 *
 * @param t theme number (index into themes array).
 */
void Theme_Torture(int t)
{
	int xp, yp;
	char tortrnd[4] = { 6, 8, 3, 8 };
	char monstrnd[4] = { 6, 8, 3, 9 };

	for (yp = 1; yp < MAXDUNY - 1; yp++) {
		for (xp = 1; xp < MAXDUNX - 1; xp++) {
			if (grid[xp][yp].dTransVal == themes[t].ttval && !grid[xp][yp].isSolid()) {
				if (CheckThemeObj3(xp, yp, t, -1)) {
					if (random_(0, tortrnd[int(lvl.type()) - 1]) == 0) {
						AddObject(OBJ_TNUDEM2, xp, yp);
					}
				}
			}
		}
	}
	PlaceThemeMonsts(t, monstrnd[int(lvl.type()) - 1]);
}

/**
 * Theme_BloodFountain initializes the blood fountain theme.
 * @param t Theme number (index into themes array).
 */
void Theme_BloodFountain(int t)
{
	char monstrnd[4] = { 6, 8, 3, 9 };

	TFit_Obj5(t);
	AddObject(OBJ_BLOODFTN, themex, themey);
	PlaceThemeMonsts(t, monstrnd[int(lvl.type()) - 1]);
}

/**
 * Theme_Decap initializes the decapitated theme.
 *
 * @param t theme number (index into themes array).
 */
void Theme_Decap(int t)
{
	int xp, yp;
	char decaprnd[4] = { 6, 8, 3, 8 };
	char monstrnd[4] = { 6, 8, 3, 9 };

	for (yp = 1; yp < MAXDUNY - 1; yp++) {
		for (xp = 1; xp < MAXDUNX - 1; xp++) {
			if (grid[xp][yp].dTransVal == themes[t].ttval && !grid[xp][yp].isSolid()) {
				if (CheckThemeObj3(xp, yp, t, -1)) {
					if (random_(0, decaprnd[int(lvl.type()) - 1]) == 0) {
						AddObject(OBJ_DECAP, xp, yp);
					}
				}
			}
		}
	}
	PlaceThemeMonsts(t, monstrnd[int(lvl.type()) - 1]);
}

/**
 * Theme_PurifyingFountain initializes the purifying fountain theme.
 *
 * @param t theme number (index into themes array).
 */
void Theme_PurifyingFountain(int t)
{
	char monstrnd[4] = { 6, 7, 3, 9 };

	TFit_Obj5(t);
	AddObject(OBJ_PURIFYINGFTN, themex, themey);
	PlaceThemeMonsts(t, monstrnd[int(lvl.type()) - 1]);
}

/**
 * Theme_ArmorStand initializes the armor stand theme.
 *
 * @param t theme number (index into themes array).
 */
void Theme_ArmorStand(int t)
{
	int xp, yp;
	char armorrnd[4] = { 6, 8, 3, 8 };
	char monstrnd[4] = { 6, 7, 3, 9 };

	if (armorFlag) {
		TFit_Obj3(t);
		AddObject(OBJ_ARMORSTAND, themex, themey);
	}
	for (yp = 0; yp < MAXDUNY; yp++) {
		for (xp = 0; xp < MAXDUNX; xp++) {
			if (grid[xp][yp].dTransVal == themes[t].ttval && !grid[xp][yp].isSolid()) {
				if (CheckThemeObj3(xp, yp, t, -1)) {
					if (random_(0, armorrnd[int(lvl.type()) - 1]) == 0) {
						AddObject(OBJ_ARMORSTANDN, xp, yp);
					}
				}
			}
		}
	}
	PlaceThemeMonsts(t, monstrnd[int(lvl.type()) - 1]);
	armorFlag = false;
}

/**
 * Theme_GoatShrine initializes the goat shrine theme.
 *
 * @param t theme number (index into themes array).
 */
void Theme_GoatShrine(int t)
{
	int xx, yy;

	TFit_GoatShrine(t);
	AddObject(OBJ_GOATSHRINE, themex, themey);
	for (yy = themey - 1; yy <= themey + 1; yy++) {
		for (xx = themex - 1; xx <= themex + 1; xx++) {
			if (grid[xx][yy].dTransVal == themes[t].ttval && !grid[xx][yy].isSolid() && (xx != themex || yy != themey)) {
				AddMonster({ xx, yy }, Dir::SW, themeVar1, true);
			}
		}
	}
}

/**
 * Theme_Cauldron initializes the cauldron theme.
 *
 * @param t theme number (index into themes array).
 */
void Theme_Cauldron(int t)
{
	char monstrnd[4] = { 6, 7, 3, 9 };

	TFit_Obj5(t);
	AddObject(OBJ_CAULDRON, themex, themey);
	PlaceThemeMonsts(t, monstrnd[int(lvl.type()) - 1]);
}

/**
 * Theme_MurkyFountain initializes the murky fountain theme.
 *
 * @param t theme number (index into themes array).
 */
void Theme_MurkyFountain(int t)
{
	char monstrnd[4] = { 6, 7, 3, 9 };

	TFit_Obj5(t);
	AddObject(OBJ_MURKYFTN, themex, themey);
	PlaceThemeMonsts(t, monstrnd[int(lvl.type()) - 1]);
}

/**
 * Theme_TearFountain initializes the tear fountain theme.
 *
 * @param t theme number (index into themes array).
 */
void Theme_TearFountain(int t)
{
	char monstrnd[4] = { 6, 7, 3, 9 };

	TFit_Obj5(t);
	AddObject(OBJ_TEARFTN, themex, themey);
	PlaceThemeMonsts(t, monstrnd[int(lvl.type()) - 1]);
}

/**
 * Theme_BrnCross initializes the burning cross theme.
 *
 * @param t theme number (index into themes array).
 */
void Theme_BrnCross(int t)
{
	int xp, yp;
	char monstrnd[4] = { 6, 8, 3, 9 };
	char bcrossrnd[4] = { 5, 7, 3, 8 };

	for (yp = 0; yp < MAXDUNY; yp++) {
		for (xp = 0; xp < MAXDUNX; xp++) {
			if (grid[xp][yp].dTransVal == themes[t].ttval && !grid[xp][yp].isSolid()) {
				if (CheckThemeObj3(xp, yp, t, -1)) {
					if (random_(0, bcrossrnd[int(lvl.type()) - 1]) == 0) {
						AddObject(OBJ_TBCROSS, xp, yp);
					}
				}
			}
		}
	}
	PlaceThemeMonsts(t, monstrnd[int(lvl.type()) - 1]);
	bCrossFlag = true;
}

/**
 * Theme_WeaponRack initializes the weapon rack theme.
 *
 * @param t theme number (index into themes array).
 */
void Theme_WeaponRack(int t)
{
	int xp, yp;
	char weaponrnd[4] = { 6, 8, 5, 8 };
	char monstrnd[4] = { 6, 7, 3, 9 };

	if (weaponFlag) {
		TFit_Obj3(t);
		AddObject(OBJ_WEAPONRACK, themex, themey);
	}
	for (yp = 0; yp < MAXDUNY; yp++) {
		for (xp = 0; xp < MAXDUNX; xp++) {
			if (grid[xp][yp].dTransVal == themes[t].ttval && !grid[xp][yp].isSolid()) {
				if (CheckThemeObj3(xp, yp, t, -1)) {
					if (random_(0, weaponrnd[int(lvl.type()) - 1]) == 0) {
						AddObject(OBJ_WEAPONRACKN, xp, yp);
					}
				}
			}
		}
	}
	PlaceThemeMonsts(t, monstrnd[int(lvl.type()) - 1]);
	weaponFlag = false;
}

/**
 * UpdateL4Trans sets each value of the transparency map to 1.
 */
void UpdateL4Trans()
{
	int i, j;

	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++) {
			if (grid[i][j].dTransVal != 0) {
				grid[i][j].dTransVal = 1;
			}
		}
	}
}

/**
 * CreateThemeRooms adds thematic elements to rooms.
 */
void CreateThemeRooms()
{
	int i;

	if (lvl.currlevel == 16) {
		return;
	}
	InitObjFlag = true;
	for (i = 0; i < numthemes; i++) {
		themex = 0;
		themey = 0;
		switch (themes[i].ttype) {
		case THEME_BARREL:
			Theme_Barrel(i);
			break;
		case THEME_SHRINE:
			Theme_Shrine(i);
			break;
		case THEME_MONSTPIT:
			Theme_MonstPit(i);
			break;
		case THEME_SKELROOM:
			Theme_SkelRoom(i);
			break;
		case THEME_TREASURE:
			Theme_Treasure(i);
			break;
		case THEME_LIBRARY:
			Theme_Library(i);
			break;
		case THEME_TORTURE:
			Theme_Torture(i);
			break;
		case THEME_BLOODFOUNTAIN:
			Theme_BloodFountain(i);
			break;
		case THEME_DECAPITATED:
			Theme_Decap(i);
			break;
		case THEME_PURIFYINGFOUNTAIN:
			Theme_PurifyingFountain(i);
			break;
		case THEME_ARMORSTAND:
			Theme_ArmorStand(i);
			break;
		case THEME_GOATSHRINE:
			Theme_GoatShrine(i);
			break;
		case THEME_CAULDRON:
			Theme_Cauldron(i);
			break;
		case THEME_MURKYFOUNTAIN:
			Theme_MurkyFountain(i);
			break;
		case THEME_TEARFOUNTAIN:
			Theme_TearFountain(i);
			break;
		case THEME_BRNCROSS:
			Theme_BrnCross(i);
			break;
		case THEME_WEAPONRACK:
			Theme_WeaponRack(i);
			break;
		}
	}
	InitObjFlag = false;
	if (lvl.type() == DunType::hell && lvl.themeCount > 0) {
		UpdateL4Trans();
	}
}

DEVILUTION_END_NAMESPACE
