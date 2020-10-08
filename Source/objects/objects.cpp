#include "all.h"

namespace dvl {

int trapid;
int trapdir;
uint8_t *pObjCels[40];
char ObjFileList[40];
int objectactive[MAXOBJECTS];
int nobjects;
int leverid;
int objectavail[MAXOBJECTS];
ObjectStruct object[MAXOBJECTS];
bool InitObjFlag;
int numobjfiles;

int bxadd[8] = { -1, 0, 1, -1, 1, -1, 0, 1 };
int byadd[8] = { -1, -1, -1, 0, 0, 1, 1, 1 };
char *shrinestrs[NUM_SHRINETYPE] = {
	"Mysterious",
	"Hidden",
	"Gloomy",
	"Weird",
	"Magical",
	"Stone",
	"Religious",
	"Enchanted",
	"Thaumaturgic",
	"Fascinating",
	"Cryptic",
	"Magical",
	"Eldritch",
	"Eerie",
	"Divine",
	"Holy",
	"Sacred",
	"Spiritual",
	"Spooky",
	"Abandoned",
	"Creepy",
	"Quiet",
	"Secluded",
	"Ornate",
	"Glimmering",
	"Tainted"
};
char shrinemin[NUM_SHRINETYPE] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1
};
char shrinemax[NUM_SHRINETYPE] = {
	16, 16, 16, 16, 16, 16, 16, 8, 16, 16,
	16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
	16, 16, 16, 16, 16, 16
};
/** 0 - sp+mp, 1 - sp only, 2 - mp only */
uint8_t shrineavail[NUM_SHRINETYPE] = {
	0, 0, 1, 1, 0, 0, 0, 0, 1, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 2, 0,
	0, 0, 0, 0, 0, 2
};
char *StoryBookName[9] = {
	"The Great Conflict",
	"The Wages of Sin are War",
	"The Tale of the Horadrim",
	"The Dark Exile",
	"The Sin War",
	"The Binding of the Three",
	"The Realms Beyond",
	"Tale of the Three",
	"The Black King"
};
int StoryText[3][3] = {
	{ TEXT_BOOK11, TEXT_BOOK12, TEXT_BOOK13 },
	{ TEXT_BOOK21, TEXT_BOOK22, TEXT_BOOK23 },
	{ TEXT_BOOK31, TEXT_BOOK32, TEXT_BOOK33 }
};

void InitObjectGFX()
{
	bool fileload[56];
	char filestr[32];
	int i, j;

	memset(fileload, false, sizeof(fileload));

	for (i = 0; AllObjects[i].oload != -1; i++) {
		if (AllObjects[i].oload == 1
		    && (int)lvl.currlevel >= AllObjects[i].ominlvl
		    && (int)lvl.currlevel <= AllObjects[i].omaxlvl) {
			fileload[AllObjects[i].ofindex] = true;
		}
		if (AllObjects[i].otheme != THEME_NONE) {
			for (j = 0; j < numthemes; j++) {
				if (themes[j].ttype == AllObjects[i].otheme)
					fileload[AllObjects[i].ofindex] = true;
			}
		}

		if (AllObjects[i].oquest != -1) {
			if (QuestStatus(AllObjects[i].oquest))
				fileload[AllObjects[i].ofindex] = true;
		}
	}

	for (i = 0; i < 56; i++) {
		if (fileload[i]) {
			ObjFileList[numobjfiles] = i;
			sprintf(filestr, "Objects\\%s.CEL", ObjMasterLoadList[i]);
			pObjCels[numobjfiles] = LoadFileInMem(filestr, NULL);
			numobjfiles++;
		}
	}
}

void FreeObjectGFX()
{
	for (int i = 0; i < numobjfiles; i++) {
		MemFreeDbg(pObjCels[i]);
	}
	numobjfiles = 0;
}

bool RndLocOk(int x, int y)
{
	return RndLocOk({ x, y });
}

bool RndLocOk(V2Di p)
{
	if (grid.at(p).getActor())
		return false;
	if (grid.at(p).isPlayer())
		return false;
	if (grid.at(p).isObject())
		return false;
	if (grid.at(p).dFlags & DunTileFlag::POPULATED)
		return false;
	if (grid.at(p).isSolid())
		return false;
	if (lvl.type() != DunType::cathedral || grid.at(p).getPiece() <= 126 || grid.at(p).getPiece() >= 144)
		return true;
	return false;
}

void InitRndLocObj(int min, int max, int objtype)
{
	int xp, yp;
	int numobjs = random_(139, max - min) + min;
	for (int i = 0; i < numobjs; i++) {
		while (1) {
			xp = random_(139, 80) + 16;
			yp = random_(139, 80) + 16;
			if (RndLocOk(xp - 1, yp - 1) // Checks 8 squares around
			    && RndLocOk(xp, yp - 1)
			    && RndLocOk(xp + 1, yp - 1)
			    && RndLocOk(xp - 1, yp)
			    && RndLocOk(xp, yp)
			    && RndLocOk(xp + 1, yp)
			    && RndLocOk(xp - 1, yp + 1)
			    && RndLocOk(xp, yp + 1)
			    && RndLocOk(xp + 1, yp + 1)) {
				AddObject(objtype, { xp, yp });
				break;
			}
		}
	}
}

void InitRndLocBigObj(int min, int max, int objtype)
{
	int xp, yp;
	int numobjs = random_(140, max - min) + min;
	for (int i = 0; i < numobjs; i++) {
		while (1) {
			xp = random_(140, 80) + 16;
			yp = random_(140, 80) + 16;
			if (RndLocOk(xp - 1, yp - 2)
			    && RndLocOk(xp, yp - 2)
			    && RndLocOk(xp + 1, yp - 2)
			    && RndLocOk(xp - 1, yp - 1)
			    && RndLocOk(xp, yp - 1)
			    && RndLocOk(xp + 1, yp - 1)
			    && RndLocOk(xp - 1, yp)
			    && RndLocOk(xp, yp)
			    && RndLocOk(xp + 1, yp)
			    && RndLocOk(xp - 1, yp + 1)
			    && RndLocOk(xp, yp + 1)
			    && RndLocOk(xp + 1, yp + 1)) {
				AddObject(objtype, { xp, yp });
				break;
			}
		}
	}
}

void InitRndLocObj5x5(int min, int max, int objtype)
{
	bool exit;
	int xp, yp, cnt, m, n;
	int numobjs = min + random_(139, max - min);
	for (int i = 0; i < numobjs; i++) {
		cnt = 0;
		exit = false;
		while (!exit) {
			exit = true;
			xp = random_(139, 80) + 16;
			yp = random_(139, 80) + 16;
			for (n = -2; n <= 2; n++) {
				for (m = -2; m <= 2; m++) {
					if (!RndLocOk(xp + m, yp + n))
						exit = false;
				}
			}
			if (!exit) {
				cnt++;
				if (cnt > 20000)
					return;
			}
		}
		AddObject(objtype, { xp, yp });
	}
}

void ClrAllObjects()
{
	for (int i = 0; i < MAXOBJECTS; i++) {
		object[i]._o = { 0, 0 };
		object[i]._oAnimData = 0;
		object[i]._oAnimDelay = 0;
		object[i]._oAnimCnt = 0;
		object[i]._oAnimLen = 0;
		object[i]._oAnimFrame = 0;
		object[i]._oDelFlag = false;
		object[i]._oVar1 = 0;
		object[i]._oVar2 = 0;
		object[i]._oVar3 = 0;
		object[i]._oVar4 = 0;
	}
	nobjects = 0;
	for (int i = 0; i < MAXOBJECTS; i++) {
		objectavail[i] = i;
		objectactive[i] = 0;
	}
	trapdir = 0;
	trapid = 1;
	leverid = 1;
}

void AddTortures()
{
	for (int oy = 0; oy < MAXDUNY; oy++) {
		for (int ox = 0; ox < MAXDUNX; ox++) {
			if (grid[ox][oy].getPiece() == 367) {
				AddObject(ObjectType::TORTURE1, ox, oy + 1);
				AddObject(ObjectType::TORTURE3, ox + 2, oy - 1);
				AddObject(ObjectType::TORTURE2, ox, oy + 3);
				AddObject(ObjectType::TORTURE4, ox + 4, oy - 1);
				AddObject(ObjectType::TORTURE5, ox, oy + 5);
				AddObject(ObjectType::TNUDEM1, ox + 1, oy + 3);
				AddObject(ObjectType::TNUDEM2, ox + 4, oy + 5);
				AddObject(ObjectType::TNUDEM3, ox + 2, oy);
				AddObject(ObjectType::TNUDEM4, ox + 3, oy + 2);
				AddObject(ObjectType::TNUDEW1, ox + 2, oy + 4);
				AddObject(ObjectType::TNUDEW2, ox + 2, oy + 1);
				AddObject(ObjectType::TNUDEW3, ox + 4, oy + 2);
			}
		}
	}
}
void AddCandles()
{
	V2Di t = quests[QuestId::pwater]._qt;
	AddObject(ObjectType::STORYCANDLE, t.x - 2, t.y + 1);
	AddObject(ObjectType::STORYCANDLE, t.x + 3, t.y + 1);
	AddObject(ObjectType::STORYCANDLE, t.x - 1, t.y + 2);
	AddObject(ObjectType::STORYCANDLE, t.x + 2, t.y + 2);
}

void AddBookLever(V2Di /*l1*/, V2Di /*l2*/, V2Di p1, V2Di p2, int msg)
{
	int xp, yp, ob, m, n;
	int cnt = 0;
	bool exit = false;
	while (!exit) {
		exit = true;
		xp = random_(139, 80) + 16;
		yp = random_(139, 80) + 16;
		for (n = -2; n <= 2; n++) {
			for (m = -2; m <= 2; m++) {
				if (!RndLocOk(xp + m, yp + n))
					exit = false;
			}
		}
		if (!exit) {
			cnt++;
			if (cnt > 20000)
				return;
		}
	}

	if (QuestStatus(QuestId::blind))
		AddObject(ObjectType::BLINDBOOK, xp, yp);
	if (QuestStatus(QuestId::warlord))
		AddObject(ObjectType::STEELTOME, xp, yp);
	if (QuestStatus(QuestId::blood)) {
		xp = 2 * lvl.getpc().x + 25;
		yp = 2 * lvl.getpc().y + 40;
		AddObject(ObjectType::BLOODBOOK, xp, yp);
	}
	ob = grid[xp][yp].getObject();
	SetObjMapRange(ob, p1, p2, leverid);
	SetBookMsg(ob, msg);
	leverid++;
	object[ob]._oVar6 = object[ob]._oAnimFrame + 1;
}

void InitRndBarrels()
{
	int xp, yp;
	_object_id o;
	bool found;
	int p; // regulates chance to stop placing barrels in current group
	int dir;
	int t; // number of tries of placing next barrel in current group
	int c; // number of barrels in current group

	int numobjs = random_(143, 5) + 3; // number of groups of barrels to generate
	for (int i = 0; i < numobjs; i++) {
		do {
			xp = random_(143, 80) + 16;
			yp = random_(143, 80) + 16;
		} while (!RndLocOk(xp, yp));
		o = (random_(143, 4) != 0) ? ObjectType::BARREL : ObjectType::BARRELEX;
		AddObject(o, xp, yp);
		found = true;
		p = 0;
		c = 1;
		while (random_(143, p) == 0 && found) {
			t = 0;
			found = false;
			while (true) {
				if (t >= 3)
					break;
				dir = random_(143, 8);
				xp += bxadd[dir];
				yp += byadd[dir];
				found = RndLocOk(xp, yp);
				t++;
				if (found)
					break;
			}
			if (found) {
				o = (random_(143, 5) != 0) ? ObjectType::BARREL : ObjectType::BARRELEX;
				AddObject(o, xp, yp);
				c++;
			}
			p = c >> 1;
		}
	}
}

void AddL1Objs(V2Di p1, V2Di p2)
{
	for (int j = p1.y; j < p2.y; j++) {
		for (int i = p1.x; i < p2.x; i++) {
			int pn = grid[i][j].getPiece();
			if (pn == 270)
				AddObject(ObjectType::L1LIGHT, i, j);
			if (pn == 44 || pn == 51 || pn == 214)
				AddObject(ObjectType::L1LDOOR, i, j);
			if (pn == 46 || pn == 56)
				AddObject(ObjectType::L1RDOOR, i, j);
		}
	}
}

void AddL2Objs(V2Di p1, V2Di p2)
{
	for (int j = p1.y; j < p2.y; j++) {
		for (int i = p1.x; i < p2.x; i++) {
			int pn = grid[i][j].getPiece();
			if (pn == 13 || pn == 541)
				AddObject(ObjectType::L2LDOOR, i, j);
			if (pn == 17 || pn == 542)
				AddObject(ObjectType::L2RDOOR, i, j);
		}
	}
}

void AddL3Objs(V2Di p1, V2Di p2)
{
	for (int j = p1.y; j < p2.y; j++) {
		for (int i = p1.x; i < p2.x; i++) {
			int pn = grid[i][j].getPiece();
			if (pn == 531)
				AddObject(ObjectType::L3LDOOR, i, j);
			if (pn == 534)
				AddObject(ObjectType::L3RDOOR, i, j);
		}
	}
}

bool WallTrapLocOk(V2Di p)
{
	if (grid.at(p).dFlags & DunTileFlag::POPULATED) return false;
	return true;
}

void AddL2Torches()
{
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++) {
			if (!WallTrapLocOk({ i, j })) continue;

			int pn = grid[i][j].getPiece();
			if (pn == 1 && random_(145, 3) == 0)
				AddObject(ObjectType::TORCHL2, i, j);

			if (pn == 5 && random_(145, 3) == 0)
				AddObject(ObjectType::TORCHR2, i, j);

			if (pn == 37 && random_(145, 10) == 0 && !grid[i - 1][j].isObject())
				AddObject(ObjectType::TORCHL, i - 1, j);

			if (pn == 41 && random_(145, 10) == 0 && !grid[i][j - 1].isObject())
				AddObject(ObjectType::TORCHR, i, j - 1);
		}
	}
}

bool TorchLocOK(V2Di p)
{
	if (grid.at(p).dFlags & DunTileFlag::POPULATED) return false;

	if (pieces[grid.at(p).getPiece()].trap != false)
		return true;
	else
		return false;
}

void AddObjTraps()
{
	char oi_trap, oi;
	int i, j;
	int xp, yp;
	int rndv;

	if (lvl.currlevel == 1)
		rndv = 10;
	if (lvl.currlevel >= 2)
		rndv = 15;
	if (lvl.currlevel >= 5)
		rndv = 20;
	if (lvl.currlevel >= 7)
		rndv = 25;
	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++) {
			//if (grid[i][j].dObject <= 0 || random_(144, 100) >= rndv)
			//	continue;

			oi = grid[i][j].getObject();
			if (!AllObjects[object[oi]._otype].oTrapFlag)
				continue;

			if (random_(144, 2) == 0) {
				xp = i - 1;
				while (!grid[xp][j].isSolid())
					xp--;

				if (!TorchLocOK({ xp, j }) || i - xp <= 1)
					continue;

				AddObject(ObjectType::TRAPL, xp, j);
				oi_trap = grid[xp][j].getObject();
				object[oi_trap]._oVar1 = i;
				object[oi_trap]._oVar2 = j;
				object[oi]._oTrapFlag = true;
			} else {
				yp = j - 1;



				while (!grid[i][yp].isSolid())
					yp--;

				if (!TorchLocOK({ i, yp }) || j - yp <= 1)
					continue;

				AddObject(ObjectType::TRAPR, i, yp);
				oi_trap = grid[i][yp].getObject();
				object[oi_trap]._oVar1 = i;
				object[oi_trap]._oVar2 = j;
				object[oi]._oTrapFlag = true;
			}
		}
	}
}

void AddChestTraps()
{
	int i, j;
	char oi;

	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++) {
			if (grid[i][j].isObject()) {
				oi = grid[i][j].getObject();
				if (object[oi]._otype >= ObjectType::CHEST1 && object[oi]._otype <= ObjectType::CHEST3 && !object[oi]._oTrapFlag && random_(0, 100) < 10) {
					object[oi]._otype += ObjectType::BOOKCASER;
					object[oi]._oTrapFlag = true;
					if (lvl.type() == DunType::catacombs) {
						object[oi]._oVar4 = random_(0, 2);
					} else {
						object[oi]._oVar4 = random_(0, 3);
					}
				}
			}
		}
	}
}

void LoadMapObjects(uint8_t *pMap, V2Di start, V2Di p1, int w, int h, int leveridx)
{
	int rw, rh, i, j, oi;
	uint8_t *lm;
	long mapoff;

	InitObjFlag = true;

	lm = pMap + 2;
	rw = pMap[0];
	rh = *lm;
	mapoff = (rw * rh + 1) * 2;
	rw <<= 1;
	rh <<= 1;
	mapoff += rw * 2 * rh * 2;
	lm += mapoff;

	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*lm) {
				AddObject(ObjTypeConv[*lm], start.x + 16 + i, start.y + 16 + j);
				oi = ObjIndex(start.x + 16 + i, start.y + 16 + j);
				SetObjMapRange(oi, p1, { p1.x + w, p1.y + h }, leveridx);
			}
			lm += 2;
		}
	}
	InitObjFlag = false;
}

void LoadMapObjs(uint8_t *pMap, V2Di start)
{
	int rw, rh;
	int i, j;
	uint8_t *lm;
	long mapoff;

	InitObjFlag = true;
	lm = pMap;
	rw = *lm;
	lm += 2;
	rh = *lm;
	mapoff = (rw * rh + 1) * 2;
	rw <<= 1;
	rh <<= 1;
	mapoff += 2 * rw * rh * 2;
	lm += mapoff;

	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*lm) {
				AddObject(ObjTypeConv[*lm], start.x + 16 + i, start.y + 16 + j);
			}
			lm += 2;
		}
	}
	InitObjFlag = false;
}

void AddDiabObjs()
{
	uint8_t *lpSetPiece;

	lpSetPiece = LoadFileInMem("Levels\\L4Data\\diab1.DUN", NULL);
	LoadMapObjects(lpSetPiece, { 2 * diabquad1x, 2 * diabquad1y }, { diabquad2x, diabquad2y }, 11, 12, 1);
	mem_free_dbg(lpSetPiece);
	lpSetPiece = LoadFileInMem("Levels\\L4Data\\diab2a.DUN", NULL);
	LoadMapObjects(lpSetPiece, { 2 * diabquad2x, 2 * diabquad2y }, { diabquad3x, diabquad3y }, 11, 11, 2);
	mem_free_dbg(lpSetPiece);
	lpSetPiece = LoadFileInMem("Levels\\L4Data\\diab3a.DUN", NULL);
	LoadMapObjects(lpSetPiece, { 2 * diabquad3x, 2 * diabquad3y }, { diabquad4x, diabquad4y }, 9, 9, 3);
	mem_free_dbg(lpSetPiece);
}

void AddStoryBooks()
{
	int xp, yp, xx, yy;
	int cnt;
	bool done;

	cnt = 0;
	done = false;
	while (!done) {
		done = true;
		xp = random_(139, 80) + 16;
		yp = random_(139, 80) + 16;
		for (yy = -2; yy <= 2; yy++) {
			for (xx = -3; xx <= 3; xx++) {
				if (!RndLocOk(xx + xp, yy + yp))
					done = false;
			}
		}
		if (!done) {
			cnt++;
			if (cnt > 20000)
				return;
		}
	}
	AddObject(ObjectType::STORYBOOK, xp, yp);
	AddObject(ObjectType::STORYCANDLE, xp - 2, yp + 1);
	AddObject(ObjectType::STORYCANDLE, xp - 2, yp);
	AddObject(ObjectType::STORYCANDLE, xp - 1, yp - 1);
	AddObject(ObjectType::STORYCANDLE, xp + 1, yp - 1);
	AddObject(ObjectType::STORYCANDLE, xp + 2, yp);
	AddObject(ObjectType::STORYCANDLE, xp + 2, yp + 1);
}

void AddHookedBodies(int freq)
{
	int i, j, ii, jj;

	for (j = 0; j < 40; j++) {
		jj = 16 + j * 2;
		for (i = 0; i < 40; i++) {
			ii = 16 + i * 2;
			if (dgrid[i][j].dungeon != 1 && dgrid[i][j].dungeon != 2)
				continue;
			if (random_(0, freq) != 0)
				continue;
			if (!SkipThemeRoom(i, j))
				continue;
			if (dgrid[i][j].dungeon == 1 && dgrid[i + 1][j].dungeon == 6) {
				switch (random_(0, 3)) {
				case 0:
					AddObject(ObjectType::TORTURE1, ii + 1, jj);
					break;
				case 1:
					AddObject(ObjectType::TORTURE2, ii + 1, jj);
					break;
				case 2:
					AddObject(ObjectType::TORTURE5, ii + 1, jj);
					break;
				}
				continue;
			}
			if (dgrid[i][j].dungeon == 2 && dgrid[i][j + 1].dungeon == 6) {
				switch (random_(0, 2)) {
				case 0:
					AddObject(ObjectType::TORTURE3, ii, jj);
					break;
				case 1:
					AddObject(ObjectType::TORTURE4, ii, jj);
					break;
				}
			}
		}
	}
}

void AddL4Goodies()
{
	AddHookedBodies(6);
	InitRndLocObj(2, 6, ObjectType::TNUDEM1);
	InitRndLocObj(2, 6, ObjectType::TNUDEM2);
	InitRndLocObj(2, 6, ObjectType::TNUDEM3);
	InitRndLocObj(2, 6, ObjectType::TNUDEM4);
	InitRndLocObj(2, 6, ObjectType::TNUDEW1);
	InitRndLocObj(2, 6, ObjectType::TNUDEW2);
	InitRndLocObj(2, 6, ObjectType::TNUDEW3);
	InitRndLocObj(2, 6, ObjectType::DECAP);
	InitRndLocObj(1, 3, ObjectType::CAULDRON);
}

void AddLazStand()
{
	int xp, yp, xx, yy;
	int cnt;
	bool found;

	cnt = 0;
	found = false;
	while (!found) {
		found = true;
		xp = random_(139, 80) + 16;
		yp = random_(139, 80) + 16;
		for (yy = -3; yy <= 3; yy++) {
			for (xx = -2; xx <= 3; xx++) {
				if (!RndLocOk(xp + xx, yp + yy))
					found = false;
			}
		}
		if (!found) {
			cnt++;
			if (cnt > 10000) {
				InitRndLocObj(1, 1, ObjectType::LAZSTAND);
				return;
			}
		}
	}
	AddObject(ObjectType::LAZSTAND, xp, yp);
	AddObject(ObjectType::TNUDEM2, xp, yp + 2);
	AddObject(ObjectType::STORYCANDLE, xp + 1, yp + 2);
	AddObject(ObjectType::TNUDEM3, xp + 2, yp + 2);
	AddObject(ObjectType::TNUDEW1, xp, yp - 2);
	AddObject(ObjectType::STORYCANDLE, xp + 1, yp - 2);
	AddObject(ObjectType::TNUDEW2, xp + 2, yp - 2);
	AddObject(ObjectType::STORYCANDLE, xp - 1, yp - 1);
	AddObject(ObjectType::TNUDEW3, xp - 1, yp);
	AddObject(ObjectType::STORYCANDLE, xp - 1, yp + 1);
}

void InitObjects()
{
	int sp_id;
	uint8_t *mem;

	ClrAllObjects();
	if (lvl.currlevel == 16) {
		AddDiabObjs();
	} else {
		InitObjFlag = true;
		GetRndSeed();
		if (lvl.currlevel == 9 && game.isSingleplayer())
			AddSlainHero();
		if (lvl.currlevel == quests[QuestId::mushroom]._qlevel && quests[QuestId::mushroom]._qactive == QuestState::init)
			AddMushPatch();
		if (lvl.currlevel == 4)
			AddStoryBooks();
		if (lvl.currlevel == 8)
			AddStoryBooks();
		if (lvl.currlevel == 12)
			AddStoryBooks();
		if (lvl.type() == DunType::cathedral) {
			if (QuestStatus(QuestId::butcher))
				AddTortures();
			if (QuestStatus(QuestId::pwater))
				AddCandles();
			if (QuestStatus(QuestId::ltbanner))
				AddObject(ObjectType::SIGNCHEST, 2 * lvl.getpc().x + 26, 2 * lvl.getpc().y + 19);
			InitRndLocBigObj(10, 15, ObjectType::SARC);
			AddL1Objs({ 0, 0 }, { MAXDUNX, MAXDUNY });
			InitRndBarrels();
		}
		if (lvl.type() == DunType::catacombs) {
			if (QuestStatus(QuestId::rock))
				InitRndLocObj5x5(1, 1, ObjectType::STAND);
			if (QuestStatus(QuestId::schamb))
				InitRndLocObj5x5(1, 1, ObjectType::BOOK2R);
			AddL2Objs({ 0, 0 }, { MAXDUNX, MAXDUNY });
			AddL2Torches();
			if (QuestStatus(QuestId::blind)) {
				if (myplr().data._pClass == PlayerClass::warrior) {
					sp_id = TEXT_BLINDING;
				} else if (myplr().data._pClass == PlayerClass::rogue) {
					sp_id = TEXT_RBLINDING;
				} else if (myplr().data._pClass == PlayerClass::sorceror) {
					sp_id = TEXT_MBLINDING;
				}
				quests[QuestId::blind]._qmsg = sp_id;
				AddBookLever({ 0, 0 }, { MAXDUNX, MAXDUNY }, { lvl.getpc().x, lvl.getpc().y }, { lvl.getpc().w + lvl.getpc().x + 1, lvl.getpc().h + lvl.getpc().y + 1 }, sp_id);
				mem = LoadFileInMem("Levels\\L2Data\\Blind2.DUN", NULL);
				LoadMapObjs(mem, { 2 * lvl.getpc().x, 2 * lvl.getpc().y });
				mem_free_dbg(mem);
			}
			if (QuestStatus(QuestId::blood)) {
				if (myplr().data._pClass == PlayerClass::warrior) {
					sp_id = TEXT_BLOODY;
				} else if (myplr().data._pClass == PlayerClass::rogue) {
					sp_id = TEXT_RBLOODY;
				} else if (myplr().data._pClass == PlayerClass::sorceror) {
					sp_id = TEXT_MBLOODY;
				}
				quests[QuestId::blood]._qmsg = sp_id;
				AddBookLever({ 0, 0 }, { MAXDUNX, MAXDUNY }, { lvl.getpc().x, lvl.getpc().y + 3 }, { lvl.getpc().x + 2, lvl.getpc().y + 7 }, sp_id);
				AddObject(ObjectType::PEDISTAL, 2 * lvl.getpc().x + 25, 2 * lvl.getpc().y + 32);
			}
			InitRndBarrels();
		}
		if (lvl.type() == DunType::caves) {
			AddL3Objs({ 0, 0 }, { MAXDUNX, MAXDUNY });
			InitRndBarrels();
		}
		if (lvl.type() == DunType::hell) {
			if (QuestStatus(QuestId::warlord)) {
				if (myplr().data._pClass == PlayerClass::warrior) {
					sp_id = TEXT_BLOODWAR;
				} else if (myplr().data._pClass == PlayerClass::rogue) {
					sp_id = TEXT_RBLOODWAR;
				} else if (myplr().data._pClass == PlayerClass::sorceror) {
					sp_id = TEXT_MBLOODWAR;
				}
				quests[QuestId::warlord]._qmsg = sp_id;
				AddBookLever({ 0, 0 }, { MAXDUNX, MAXDUNY }, { lvl.getpc().x, lvl.getpc().y }, { lvl.getpc().x + lvl.getpc().w, lvl.getpc().y + lvl.getpc().h }, sp_id);
				mem = LoadFileInMem("Levels\\L4Data\\Warlord.DUN", NULL);
				LoadMapObjs(mem, { 2 * lvl.getpc().x, 2 * lvl.getpc().y });
				mem_free_dbg(mem);
			}
			if (QuestStatus(QuestId::betrayer) && game.isSingleplayer())
				AddLazStand();
			InitRndBarrels();
			AddL4Goodies();
		}
		InitRndLocObj(5, 10, ObjectType::CHEST1);
		InitRndLocObj(3, 6, ObjectType::CHEST2);
		InitRndLocObj(1, 5, ObjectType::CHEST3);
		if (lvl.type() != DunType::hell)
			AddObjTraps();
		if (lvl.type() > DunType::cathedral)
			AddChestTraps();
		InitObjFlag = false;
	}
}

void SetMapObjects(uint8_t *pMap, V2Di start)
{
	int rw, rh;
	int i, j;
	uint8_t *lm, *h;
	long mapoff;
	int fileload[56];
	char filestr[32];

	ClrAllObjects();
	for (i = 0; i < 56; i++)
		fileload[i] = false;
	InitObjFlag = true;

	for (i = 0; AllObjects[i].oload != -1; i++) {
		if (AllObjects[i].oload == 1 && lvl.type() == AllObjects[i].olvltype)
			fileload[AllObjects[i].ofindex] = true;
	}

	lm = pMap;
	rw = *lm;
	lm += 2;
	rh = *lm;
	mapoff = (rw * rh + 1) * 2;
	rw <<= 1;
	rh <<= 1;
	mapoff += 2 * rw * rh * 2;
	lm += mapoff;
	h = lm;

	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*lm) {
				fileload[AllObjects[ObjTypeConv[*lm]].ofindex] = true;
			}
			lm += 2;
		}
	}

	for (i = 0; i < 56; i++) {
		if (!fileload[i])
			continue;

		ObjFileList[numobjfiles] = i;
		sprintf(filestr, "Objects\\%s.CEL", ObjMasterLoadList[i]);
		pObjCels[numobjfiles] = LoadFileInMem(filestr, NULL);
		numobjfiles++;
	}

	lm = h;
	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*lm)
				AddObject(ObjTypeConv[*lm], { start.x + 16 + i, start.y + 16 + j });
			lm += 2;
		}
	}
	InitObjFlag = false;
}

void DeleteObject_(int oi, int i)
{
	V2Di o = object[oi]._o;
	grid.at(o).clearObject();
	objectavail[-nobjects + MAXOBJECTS] = oi;
	nobjects--;
	if (nobjects > 0 && i != nobjects)
		objectactive[i] = objectactive[nobjects];
}

void SetupObject(int i, V2Di pos, int ot)
{
	object[i]._otype = ot;
	int ofi = AllObjects[ot].ofindex;
	object[i]._o = pos;
	int j = 0;
	while (ObjFileList[j] != ofi) {
		j++;
	}
	object[i]._oAnimData = pObjCels[j];
	object[i]._oAnimFlag = AllObjects[ot].oAnimFlag;
	if (AllObjects[ot].oAnimFlag) {
		object[i]._oAnimDelay = AllObjects[ot].oAnimDelay;
		object[i]._oAnimCnt = random_(146, AllObjects[ot].oAnimDelay);
		object[i]._oAnimLen = AllObjects[ot].oAnimLen;
		object[i]._oAnimFrame = random_(146, AllObjects[ot].oAnimLen - 1) + 1;
	} else {
		object[i]._oAnimDelay = 1000;
		object[i]._oAnimCnt = 0;
		object[i]._oAnimLen = AllObjects[ot].oAnimLen;
		object[i]._oAnimFrame = AllObjects[ot].oAnimDelay;
	}
	object[i]._oAnimWidth = AllObjects[ot].oAnimWidth;
	object[i]._oSolidFlag = AllObjects[ot].oSolidFlag;
	object[i]._oMissFlag = AllObjects[ot].oMissFlag;
	object[i]._oLight = AllObjects[ot].oLightFlag;
	object[i]._oDelFlag = false;
	object[i]._oBreak = AllObjects[ot].oBreak;
	object[i]._oSelFlag = AllObjects[ot].oSelFlag;
	object[i]._oPreFlag = false;
	object[i]._oTrapFlag = false;
	object[i]._oDoorFlag = false;
}

void SetObjMapRange(int i, V2Di p1, V2Di p2, int v)
{
	object[i]._oVar1 = p1.x;
	object[i]._oVar2 = p1.y;
	object[i]._oVar3 = p2.x;
	object[i]._oVar4 = p2.y;
	object[i]._oVar8 = v;
}

void SetBookMsg(int i, int msg)
{
	object[i]._oVar7 = msg;
}

void AddL1Door(int i, V2Di pos, int ot)
{
	object[i]._oDoorFlag = true;
	if (ot == 1) {
		object[i]._oVar1 = grid.at(pos).getPiece();
		object[i]._oVar2 = grid[pos.x][pos.y - 1].getPiece();
	} else {
		object[i]._oVar1 = grid.at(pos).getPiece();
		object[i]._oVar2 = grid[pos.x - 1][pos.y].getPiece();
	}
	object[i]._oVar4 = 0;
}

void AddSCambBook(int i)
{
	object[i]._oVar1 = lvl.getpc().x;
	object[i]._oVar2 = lvl.getpc().y;
	object[i]._oVar3 = lvl.getpc().w + lvl.getpc().x + 1;
	object[i]._oVar4 = lvl.getpc().h + lvl.getpc().y + 1;
	object[i]._oVar6 = object[i]._oAnimFrame + 1;
}

void AddChest(int i, int t)
{
	if (!random_(147, 2)) object[i]._oAnimFrame += 3;
	object[i]._oRndSeed = GetRndSeed();
	switch (t) {
	case ObjectType::CHEST1:
	case ObjectType::TCHEST1:
		if (lvl.setlevel) {
			object[i]._oVar1 = 1;
			break;
		}
		object[i]._oVar1 = random_(147, 2);
		break;
	case ObjectType::TCHEST2:
	case ObjectType::CHEST2:
		if (lvl.setlevel) {
			object[i]._oVar1 = 2;
			break;
		}
		object[i]._oVar1 = random_(147, 3);
		break;
	case ObjectType::TCHEST3:
	case ObjectType::CHEST3:
		if (lvl.setlevel) {
			object[i]._oVar1 = 3;
			break;
		}
		object[i]._oVar1 = random_(147, 4);
		break;
	}
	object[i]._oVar2 = random_(147, 8);
}

void AddL2Door(int i, V2Di p, int ot)
{
	object[i]._oDoorFlag = true;
	if (ot == ObjectType::L2LDOOR)
		ObjSetMicro(p, 538);
	else
		ObjSetMicro(p, 540);
	object[i]._oVar4 = 0;
}

void AddL3Door(int i, V2Di p, int ot)
{
	object[i]._oDoorFlag = true;
	if (ot == ObjectType::L3LDOOR)
		ObjSetMicro(p, 531);
	else
		ObjSetMicro(p, 534);
	object[i]._oVar4 = 0;
}

void AddSarc(int i)
{
	grid[object[i]._o.x][object[i]._o.y - 1].setObject(i);
	object[i]._oVar1 = random_(153, 10);
	object[i]._oRndSeed = GetRndSeed();
	if (object[i]._oVar1 >= 8)
		object[i]._oVar2 = PreSpawnSkeleton();
}

void AddFlameTrap(int i)
{
	object[i]._oVar1 = trapid;
	object[i]._oVar2 = 0;
	object[i]._oVar3 = trapdir;
	object[i]._oVar4 = 0;
}

void AddFlameLvr(int i)
{
	object[i]._oVar1 = trapid;
	object[i]._oVar2 = MissileType::FLAMEC;
}

void AddTrap(int i, int ot)
{
	int mt = lvl.currlevel / 3 + 1;
	mt = random_(148, mt);
	if (mt == 0)
		object[i]._oVar3 = MissileType::ARROW;
	if (mt == 1)
		object[i]._oVar3 = MissileType::FIREBOLT;
	if (mt == 2)
		object[i]._oVar3 = MissileType::LIGHTCTRL;
	object[i]._oVar4 = 0;
}

void AddObjLight(int i, int r)
{
	if (InitObjFlag) {
		DoLighting(object[i]._o, r, -1);
		object[i]._oVar1 = -1;
	} else {
		object[i]._oVar1 = 0;
	}
}

void AddBarrel(int i, int t)
{
	object[i]._oVar1 = 0;
	object[i]._oRndSeed = GetRndSeed();
	object[i]._oVar2 = (t == ObjectType::BARRELEX) ? 0 : random_(149, 10);
	object[i]._oVar3 = random_(149, 3);
	if (object[i]._oVar2 >= 8) object[i]._oVar4 = PreSpawnSkeleton();
}

void AddShrine(int i)
{
	int val, j;
	bool slist[NUM_SHRINETYPE];

	object[i]._oPreFlag = true;
	for (j = 0; j < NUM_SHRINETYPE; j++) {
		if (lvl.currlevel < shrinemin[j] || lvl.currlevel > shrinemax[j]) {
			slist[j] = 0;
		} else {
			slist[j] = 1;
		}
		if (game.isMultiplayer() && shrineavail[j] == 1) {
			slist[j] = 0;
		}
		if (game.isSingleplayer() && shrineavail[j] == 2) {
			slist[j] = 0;
		}
	}
	do {
		val = random_(150, NUM_SHRINETYPE);
	} while (!slist[val]);

	object[i]._oVar1 = val;
	if (random_(150, 2)) {
		object[i]._oAnimFrame = 12;
		object[i]._oAnimLen = 22;
	}
}

void AddBookcase(int i)
{
	object[i]._oRndSeed = GetRndSeed();
	object[i]._oPreFlag = true;
}

void AddBookstand(int i)
{
	object[i]._oRndSeed = GetRndSeed();
}

void AddBloodFtn(int i)
{
	object[i]._oRndSeed = GetRndSeed();
}

void AddPurifyingFountain(int i)
{
	V2Di o = object[i]._o;
	grid[o.x][o.y - 1].setObject(i);
	grid[o.x - 1][o.y].setObject(i);
	grid[o.x - 1][o.y - 1].setObject(i);
	object[i]._oRndSeed = GetRndSeed();
}

void AddArmorStand(int i)
{
	if (!armorFlag) {
		object[i]._oAnimFlag = 2;
		object[i]._oSelFlag = 0;
	}
	object[i]._oRndSeed = GetRndSeed();
}

void AddGoatShrine(int i)
{
	object[i]._oRndSeed = GetRndSeed();
}

void AddCauldron(int i)
{
	object[i]._oRndSeed = GetRndSeed();
}

void AddMurkyFountain(int i)
{
	V2Di o = object[i]._o;
	grid[o.x][o.y - 1].setObject(i);
	grid[o.x - 1][o.y].setObject(i);
	grid[o.x - 1][o.y - 1].setObject(i);
	object[i]._oRndSeed = GetRndSeed();
}

void AddTearFountain(int i)
{
	object[i]._oRndSeed = GetRndSeed();
}

void AddDecap(int i)
{
	object[i]._oRndSeed = GetRndSeed();
	object[i]._oAnimFrame = random_(151, 8) + 1;
	object[i]._oPreFlag = true;
}

void AddVilebook(int i)
{
	if (lvl.setlevel && lvl.setlvlnum == SetLvl::VileBetrayer) {
		object[i]._oAnimFrame = 4;
	}
}

void AddMagicCircle(int i)
{
	object[i]._oRndSeed = GetRndSeed();
	object[i]._oPreFlag = true;
	object[i]._oVar6 = 0;
	object[i]._oVar5 = 1;
}

void AddBrnCross(int i)
{
	object[i]._oRndSeed = GetRndSeed();
}

void AddPedistal(int i)
{
	object[i]._oVar1 = lvl.getpc().x;
	object[i]._oVar2 = lvl.getpc().y;
	object[i]._oVar3 = lvl.getpc().x + lvl.getpc().w;
	object[i]._oVar4 = lvl.getpc().y + lvl.getpc().h;
}

void AddStoryBook(int i)
{
	SetRndSeed(glSeedTbl[16]);
	int bookframe = random_(0, 3);

	object[i]._oVar1 = bookframe;
	if (lvl.currlevel == 4)
		object[i]._oVar2 = StoryText[bookframe][0];
	if (lvl.currlevel == 8)
		object[i]._oVar2 = StoryText[bookframe][1];
	if (lvl.currlevel == 12)
		object[i]._oVar2 = StoryText[bookframe][2];
	object[i]._oVar3 = (lvl.currlevel >> 2) + 3 * bookframe - 1;
	object[i]._oAnimFrame = 5 - 2 * bookframe;
	object[i]._oVar4 = object[i]._oAnimFrame + 1;
}

void AddWeaponRack(int i)
{
	if (!weaponFlag) {
		object[i]._oAnimFlag = 2;
		object[i]._oSelFlag = 0;
	}
	object[i]._oRndSeed = GetRndSeed();
}

void AddTorturedBody(int i)
{
	object[i]._oRndSeed = GetRndSeed();
	object[i]._oAnimFrame = random_(0, 4) + 1;
	object[i]._oPreFlag = true;
}

void GetRndObjLoc(int randarea, int *xx, int *yy)
{
	bool failed;
	if (randarea == 0) return;
	int tries = 0;
	while (1) {
		tries++;
		if (tries > 1000 && randarea > 1) randarea--;
		*xx = random_(0, MAXDUNX);
		*yy = random_(0, MAXDUNY);
		failed = false;
		for (int i = 0; i < randarea && !failed; i++) {
			for (int j = 0; j < randarea && !failed; j++) {
				failed = !RndLocOk(i + *xx, j + *yy);
			}
		}
		if (!failed) break;
	}
}

void AddMushPatch()
{
	int y, x;
	if (nobjects < MAXOBJECTS) {
		int i = objectavail[0];
		GetRndObjLoc(5, &x, &y);
		grid[x + 1][y + 1].setObject(i);
		grid[x + 2][y + 1].setObject(i);
		grid[x + 1][y + 2].setObject(i);
		AddObject(ObjectType::MUSHPATCH, x + 2, y + 2);
	}
}

void AddSlainHero()
{
	int x, y;
	GetRndObjLoc(5, &x, &y);
	AddObject(ObjectType::SLAINHERO, x + 2, y + 2);
}

void AddObject(int ot, int ox, int oy)
{
	return AddObject(ot, { ox, oy });
}

void AddObject(int ot, V2Di o)
{
	if (nobjects >= MAXOBJECTS) return;

	int oi = objectavail[0];
	objectavail[0] = objectavail[126 - nobjects];
	objectactive[nobjects] = oi;
	grid.at(o).setObject(oi);
	SetupObject(oi, o, ot);
	switch (ot) {
	case ObjectType::L1LIGHT:
		AddObjLight(oi, 5);
		break;
	case ObjectType::SKFIRE:
	case ObjectType::CANDLE1:
	case ObjectType::CANDLE2:
	case ObjectType::BOOKCANDLE:
		AddObjLight(oi, 5);
		break;
	case ObjectType::STORYCANDLE:
		AddObjLight(oi, 3);
		break;
	case ObjectType::TORCHL:
	case ObjectType::TORCHR:
	case ObjectType::TORCHL2:
	case ObjectType::TORCHR2:
		AddObjLight(oi, 8);
		break;
	case ObjectType::L1LDOOR:
	case ObjectType::L1RDOOR:
		AddL1Door(oi, o, ot);
		break;
	case ObjectType::L2LDOOR:
	case ObjectType::L2RDOOR:
		AddL2Door(oi, o, ot);
		break;
	case ObjectType::L3LDOOR:
	case ObjectType::L3RDOOR:
		AddL3Door(oi, o, ot);
		break;
	case ObjectType::BOOK2R:
		AddSCambBook(oi);
		break;
	case ObjectType::CHEST1:
	case ObjectType::CHEST2:
	case ObjectType::CHEST3:
	case ObjectType::TCHEST1:
	case ObjectType::TCHEST2:
	case ObjectType::TCHEST3:
		AddChest(oi, ot);
		break;
	case ObjectType::SARC:
		AddSarc(oi);
		break;
	case ObjectType::FLAMEHOLE:
		AddFlameTrap(oi);
		break;
	case ObjectType::FLAMELVR:
		AddFlameLvr(oi);
		break;
	case ObjectType::WATER:
		object[oi]._oAnimFrame = 1;
		break;
	case ObjectType::TRAPL:
	case ObjectType::TRAPR:
		AddTrap(oi, ot);
		break;
	case ObjectType::BARREL:
	case ObjectType::BARRELEX:
		AddBarrel(oi, ot);
		break;
	case ObjectType::SHRINEL:
	case ObjectType::SHRINER:
		AddShrine(oi);
		break;
	case ObjectType::BOOKCASEL:
	case ObjectType::BOOKCASER:
		AddBookcase(oi);
		break;
	case ObjectType::SKELBOOK:
	case ObjectType::BOOKSTAND:
		AddBookstand(oi);
		break;
	case ObjectType::BLOODFTN:
		AddBloodFtn(oi);
		break;
	case ObjectType::DECAP:
		AddDecap(oi);
		break;
	case ObjectType::PURIFYINGFTN:
		AddPurifyingFountain(oi);
		break;
	case ObjectType::ARMORSTAND:
	case ObjectType::WARARMOR:
		AddArmorStand(oi);
		break;
	case ObjectType::GOATSHRINE:
		AddGoatShrine(oi);
		break;
	case ObjectType::CAULDRON:
		AddCauldron(oi);
		break;
	case ObjectType::MURKYFTN:
		AddMurkyFountain(oi);
		break;
	case ObjectType::TEARFTN:
		AddTearFountain(oi);
		break;
	case ObjectType::BOOK2L:
		AddVilebook(oi);
		break;
	case ObjectType::MCIRCLE1:
	case ObjectType::MCIRCLE2:
		AddMagicCircle(oi);
		break;
	case ObjectType::STORYBOOK:
		AddStoryBook(oi);
		break;
	case ObjectType::BCROSS:
	case ObjectType::TBCROSS:
		AddBrnCross(oi);
		AddObjLight(oi, 5);
		break;
	case ObjectType::PEDISTAL:
		AddPedistal(oi);
		break;
	case ObjectType::WARWEAP:
	case ObjectType::WEAPONRACK:
		AddWeaponRack(oi);
		break;
	case ObjectType::TNUDEM2:
		AddTorturedBody(oi);
		break;
	}
	object[oi]._oAnimWidth2 = (object[oi]._oAnimWidth - 64) >> 1;
	nobjects++;
}

void Obj_Light(int i, int lr)
{
	V2Di o, d;
	int p, tr;
	bool turnon;

	if (object[i]._oVar1 != -1) {
		o = object[i]._o;
		tr = lr + 10;
		turnon = false;
		if (!lightflag) {
			for (p = 0; p < MAX_PLRS && !turnon; p++) {
				if (plr[p].data.plractive) {
					if (lvl.currlevel == plr[p].data.plrlevel) {
						const int dist = (plr[p].pos() - o).maxabs();
						if (dist < tr)
							turnon = true;
					}
				}
			}
		}
		if (turnon) {
			if (!object[i]._oVar1)
				object[i]._olid = AddLight(o, lr);
			object[i]._oVar1 = 1;
		} else {
			if (object[i]._oVar1 == 1)
				AddUnLight(object[i]._olid);
			object[i]._oVar1 = 0;
		}
	}
}

void Obj_Circle(int i)
{
	V2Di o = object[i]._o;
	V2Di w = myplr().pos();
	if (o == w) {
		if (object[i]._otype == ObjectType::MCIRCLE1)
			object[i]._oAnimFrame = 2;
		if (object[i]._otype == ObjectType::MCIRCLE2)
			object[i]._oAnimFrame = 4;
		if (o.x == 45 && o.y == 47) {
			object[i]._oVar6 = 2;
		} else if (o.x == 26 && o.y == 46) {
			object[i]._oVar6 = 1;
		} else {
			object[i]._oVar6 = 0;
		}
		if (o.x == 35 && o.y == 36 && object[i]._oVar5 == 3) {
			object[i]._oVar6 = 4;
			ObjChangeMapResync({ object[i]._oVar1, object[i]._oVar2 }, { object[i]._oVar3, object[i]._oVar4 });
			if (quests[QuestId::betrayer]._qactive == QuestState::active)
				quests[QuestId::betrayer]._qvar1 = 4;
			AddMissile(myplr().pos(), { 35, 46 }, myplr().data._pdir, MissileType::RNDTELEPORT, 0, myplr(), 0, 0);
			track_repeat_walk(false);
			sgbMouseDown = 0;
			myplr().ClrPlrPath();
			myplr().StartStand(Dir(0));
		}
	} else {
		if (object[i]._otype == ObjectType::MCIRCLE1)
			object[i]._oAnimFrame = 1;
		if (object[i]._otype == ObjectType::MCIRCLE2)
			object[i]._oAnimFrame = 3;
		object[i]._oVar6 = 0;
	}
}

void Obj_StopAnim(int i)
{
	if (object[i]._oAnimFrame == object[i]._oAnimLen) {
		object[i]._oAnimCnt = 0;
		object[i]._oAnimDelay = 1000;
	}
}

void Obj_Door(int i)
{
	V2Di d;
	bool dok;
	if (object[i]._oVar4 == 0) {
		object[i]._oSelFlag = 3;
		object[i]._oMissFlag = false;
	} else {
		d = object[i]._o;
		dok = !grid.at(d).getActor();
		dok = dok & !grid.at(d).isItem();
		dok = dok & !grid.at(d).dDead;
		dok = dok & !grid.at(d).isPlayer();
		object[i]._oSelFlag = 2;
		object[i]._oVar4 = dok ? 1 : 2;
		object[i]._oMissFlag = true;
	}
}

void Obj_Sarc(int i)
{
	if (object[i]._oAnimFrame == object[i]._oAnimLen)
		object[i]._oAnimFlag = 0;
}

void ActivateTrapLine(int ttype, int tid)
{
	int i, oi;

	for (i = 0; i < nobjects; i++) {
		oi = objectactive[i];
		if (object[oi]._otype == ttype && object[oi]._oVar1 == tid) {
			object[oi]._oVar4 = 1;
			object[oi]._oAnimFlag = 1;
			object[oi]._oAnimDelay = 1;
			object[oi]._olid = AddLight(object[oi]._o, 1);
		}
	}
}

void Obj_FlameTrap(int i)
{
	int x, y;
	int j, k;
	if (object[i]._oVar2) {
		if (object[i]._oVar4) {
			object[i]._oAnimFrame--;
			if (object[i]._oAnimFrame == 1) {
				object[i]._oVar4 = 0;
				AddUnLight(object[i]._olid);
			} else if (object[i]._oAnimFrame <= 4) {
				ChangeLightRadius(object[i]._olid, object[i]._oAnimFrame);
			}
		}
	} else if (!object[i]._oVar4) {
		if (object[i]._oVar3 == 2) {
			x = object[i]._o.x - 2;
			y = object[i]._o.y;
			for (j = 0; j < 5; j++) {
				if (grid[x][y].isPlayer() || grid[x][y].getActor())
					object[i]._oVar4 = 1;
				x++;
			}
		} else {
			x = object[i]._o.x;
			y = object[i]._o.y - 2;
			for (k = 0; k < 5; k++) {
				if (grid[x][y].isPlayer() || grid[x][y].getActor())
					object[i]._oVar4 = 1;
				y++;
			}
		}
		if (object[i]._oVar4)
			ActivateTrapLine(object[i]._otype, object[i]._oVar1);
	} else {
		if (object[i]._oAnimFrame == object[i]._oAnimLen)
			object[i]._oAnimFrame = 11;
		if (object[i]._oAnimFrame <= 5)
			ChangeLightRadius(object[i]._olid, object[i]._oAnimFrame);
	}
}

void Obj_Trap(int i)
{
	int oti;
	V2Di s, d;
	Dir dir;

	bool otrig = false;
	if (!object[i]._oVar4) {
		oti = grid[object[i]._oVar1][object[i]._oVar2].getObject();
		switch (object[oti]._otype) {
		case ObjectType::L1LDOOR:
		case ObjectType::L1RDOOR:
		case ObjectType::L2LDOOR:
		case ObjectType::L2RDOOR:
		case ObjectType::L3LDOOR:
		case ObjectType::L3RDOOR:
			if (object[oti]._oVar4)
				otrig = true;
			break;
		case ObjectType::LEVER:
		case ObjectType::CHEST1:
		case ObjectType::CHEST2:
		case ObjectType::CHEST3:
		case ObjectType::SWITCHSKL:
		case ObjectType::SARC:
			if (object[oti]._oSelFlag == 0)
				otrig = true;
			break;
		}
		if (otrig) {
			object[i]._oVar4 = 1;
			s = object[i]._o;
			d = object[oti]._o;
			for (int y = d.y - 1; y <= object[oti]._o.y + 1; y++) {
				for (int x = object[oti]._o.x - 1; x <= object[oti]._o.x + 1; x++) {
					if (grid[x][y].isPlayer()) {
						d = { x, y };
					}
				}
			}
			if (!deltaload) {
				dir = GetDirection(s, d);
				AddMissile(s, d, dir, object[i]._oVar3, 1, -1, 0, 0);
				PlaySfxLoc(IS_TRAP, object[oti]._o);
			}
			object[oti]._oTrapFlag = false;
		}
	}
}

void Obj_BCrossDamage(int i)
{
	int fire_resist;
	int damage[4] = { 6, 8, 10, 12 };

	if (myplr().data._pmode == PlayerMode::DEATH)
		return;

	fire_resist = myplr().data._pFireResist;
	if (fire_resist > 0)
		damage[int(lvl.type()) - 1] -= fire_resist * damage[int(lvl.type()) - 1] / 100;

	if (myplr().pos().x != object[i]._o.x || myplr().pos().y != object[i]._o.y - 1)
		return;

	myplr().data._pHitPoints -= damage[int(lvl.type()) - 1];
	myplr().data._pHPBase -= damage[int(lvl.type()) - 1];
	if (myplr().data._pHitPoints >> 6 <= 0) {
		myplr().SyncPlrKill();
	} else {
		if (myplr().data._pClass == PlayerClass::warrior) {
			PlaySfxLoc(PS_WARR68, myplr().pos());
		} else if (myplr().data._pClass == PlayerClass::rogue) {
			PlaySfxLoc(PS_ROGUE68, myplr().pos());
		} else if (myplr().data._pClass == PlayerClass::sorceror) {
			PlaySfxLoc(PS_MAGE68, myplr().pos());
		}
	}
	redrawhpflag = true;
}

void ProcessObjects()
{
	int oi;
	int i;

	for (i = 0; i < nobjects; ++i) {
		oi = objectactive[i];
		switch (object[oi]._otype) {
		case ObjectType::L1LIGHT:
			Obj_Light(oi, 10);
			break;
		case ObjectType::SKFIRE:
		case ObjectType::CANDLE2:
		case ObjectType::BOOKCANDLE:
			Obj_Light(oi, 5);
			break;
		case ObjectType::STORYCANDLE:
			Obj_Light(oi, 3);
			break;
		case ObjectType::CRUX1:
		case ObjectType::CRUX2:
		case ObjectType::CRUX3:
		case ObjectType::BARREL:
		case ObjectType::BARRELEX:
		case ObjectType::SHRINEL:
		case ObjectType::SHRINER:
			Obj_StopAnim(oi);
			break;
		case ObjectType::L1LDOOR:
		case ObjectType::L1RDOOR:
		case ObjectType::L2LDOOR:
		case ObjectType::L2RDOOR:
		case ObjectType::L3LDOOR:
		case ObjectType::L3RDOOR:
			Obj_Door(oi);
			break;
		case ObjectType::TORCHL:
		case ObjectType::TORCHR:
		case ObjectType::TORCHL2:
		case ObjectType::TORCHR2:
			Obj_Light(oi, 8);
			break;
		case ObjectType::SARC:
			Obj_Sarc(oi);
			break;
		case ObjectType::FLAMEHOLE:
			Obj_FlameTrap(oi);
			break;
		case ObjectType::TRAPL:
		case ObjectType::TRAPR:
			Obj_Trap(oi);
			break;
		case ObjectType::MCIRCLE1:
		case ObjectType::MCIRCLE2:
			Obj_Circle(oi);
			break;
		case ObjectType::BCROSS:
		case ObjectType::TBCROSS:
			Obj_Light(oi, 10);
			Obj_BCrossDamage(oi);
			break;
		}
		if (object[oi]._oAnimFlag == 0)
			continue;

		object[oi]._oAnimCnt++;

		if (object[oi]._oAnimCnt < object[oi]._oAnimDelay)
			continue;

		object[oi]._oAnimCnt = 0;
		object[oi]._oAnimFrame++;
		if (object[oi]._oAnimFrame > object[oi]._oAnimLen)
			object[oi]._oAnimFrame = 1;
	}
	i = 0;
	while (i < nobjects) {
		oi = objectactive[i];
		if (object[oi]._oDelFlag) {
			DeleteObject_(oi, i);
			i = 0;
		} else {
			i++;
		}
	}
}

void ObjSetMicro(V2Di d, int pn)
{
	WORD *v;
	MICROS *defs;
	int i;

	grid.at(d).setPiece(pn);
	pn--;
	defs = &grid.at(d).dpiece_defs_map_2;
	if (lvl.type() != DunType::hell) {
		v = (WORD *)lvl.pLevelPieces + 10 * pn;
		for (i = 0; i < 10; i++) {
			defs->mt[i] = SDL_SwapLE16(v[(i & 1) - (i & 0xE) + 8]);
		}
	} else {
		v = (WORD *)lvl.pLevelPieces + 16 * pn;
		for (i = 0; i < 16; i++) {
			defs->mt[i] = SDL_SwapLE16(v[(i & 1) - (i & 0xE) + 14]);
		}
	}
}

void objects_set_door_piece(V2Di p)
{
	int pn = grid.at(p).getPiece() - 1;
	int v1 = *((WORD *)lvl.pLevelPieces + 10 * pn + 8);
	int v2 = *((WORD *)lvl.pLevelPieces + 10 * pn + 9);
	grid.at(p).dpiece_defs_map_2.mt[0] = SDL_SwapLE16(v1);
	grid.at(p).dpiece_defs_map_2.mt[1] = SDL_SwapLE16(v2);
}

void ObjSetMini(V2Di p, int v)
{
	int xx, yy;
	long v1, v2, v3, v4;
	WORD *MegaTiles;

	MegaTiles = (WORD *)&lvl.pMegaTiles[((WORD)v - 1) * 8];
	v1 = SDL_SwapLE16(*(MegaTiles + 0)); //+1;
	v2 = SDL_SwapLE16(*(MegaTiles + 1)); //+1;
	v3 = SDL_SwapLE16(*(MegaTiles + 2)); //+1;
	v4 = SDL_SwapLE16(*(MegaTiles + 3)); //+1;

	xx = 2 * p.x + 16;
	yy = 2 * p.y + 16;
	ObjSetMicro({ xx, yy }, v1);
	ObjSetMicro({ xx + 1, yy }, v2);
	ObjSetMicro({ xx, yy + 1 }, v3);
	ObjSetMicro({ xx + 1, yy + 1 }, v4);
}

void ObjL1Special(V2Di p1, V2Di p2)
{
	for (int i = p1.y; i <= p2.y; ++i) {
		for (int j = p1.x; j <= p2.x; ++j) {
			grid[j][i].dSpecial = 0;
			if (grid[j][i].getPiece() == 12)
				grid[j][i].dSpecial = 1;
			if (grid[j][i].getPiece() == 11)
				grid[j][i].dSpecial = 2;
			if (grid[j][i].getPiece() == 71)
				grid[j][i].dSpecial = 1;
			if (grid[j][i].getPiece() == 253)
				grid[j][i].dSpecial = 3;
			if (grid[j][i].getPiece() == 267)
				grid[j][i].dSpecial = 6;
			if (grid[j][i].getPiece() == 259)
				grid[j][i].dSpecial = 5;
			if (grid[j][i].getPiece() == 249)
				grid[j][i].dSpecial = 2;
			if (grid[j][i].getPiece() == 325)
				grid[j][i].dSpecial = 2;
			if (grid[j][i].getPiece() == 321)
				grid[j][i].dSpecial = 1;
			if (grid[j][i].getPiece() == 255)
				grid[j][i].dSpecial = 4;
			if (grid[j][i].getPiece() == 211)
				grid[j][i].dSpecial = 1;
			if (grid[j][i].getPiece() == 344)
				grid[j][i].dSpecial = 2;
			if (grid[j][i].getPiece() == 341)
				grid[j][i].dSpecial = 1;
			if (grid[j][i].getPiece() == 331)
				grid[j][i].dSpecial = 2;
			if (grid[j][i].getPiece() == 418)
				grid[j][i].dSpecial = 1;
			if (grid[j][i].getPiece() == 421)
				grid[j][i].dSpecial = 2;
		}
	}
}

void ObjL2Special(V2Di p1, V2Di p2)
{
	for (int i = p1.y; i <= p2.y; ++i) {
		for (int j = p1.x; j <= p2.x; ++j) {
			grid[i][j].dSpecial = 0;
			if (grid[i][j].getPiece() == 541)
				grid[i][j].dSpecial = 5;
			if (grid[i][j].getPiece() == 178)
				grid[i][j].dSpecial = 5;
			if (grid[i][j].getPiece() == 551)
				grid[i][j].dSpecial = 5;
			if (grid[i][j].getPiece() == 542)
				grid[i][j].dSpecial = 6;
			if (grid[i][j].getPiece() == 553)
				grid[i][j].dSpecial = 6;
			if (grid[i][j].getPiece() == 13)
				grid[i][j].dSpecial = 5;
			if (grid[i][j].getPiece() == 17)
				grid[i][j].dSpecial = 6;
		}
	}
	for (int i = p1.y; i <= p2.y; ++i) {
		for (int j = p1.x; j <= p2.x; ++j) {
			if (grid[i][j].getPiece() == 132) {
				grid[i][j + 1].dSpecial = 2;
				grid[i][j + 2].dSpecial = 1;
			}
			if (grid[i][j].getPiece() == 135 || grid[i][j].getPiece() == 139) {
				grid[i + 1][j].dSpecial = 3;
				grid[i + 2][j].dSpecial = 4;
			}
		}
	}
}

void DoorSet(int oi, V2Di d)
{
	int pn;

	pn = grid.at(d).getPiece();
	if (pn == 43)
		ObjSetMicro(d, 392);
	if (pn == 45)
		ObjSetMicro(d, 394);
	if (pn == 50 && object[oi]._otype == ObjectType::L1LDOOR)
		ObjSetMicro(d, 411);
	if (pn == 50 && object[oi]._otype == ObjectType::L1RDOOR)
		ObjSetMicro(d, 412);
	if (pn == 54)
		ObjSetMicro(d, 397);
	if (pn == 55)
		ObjSetMicro(d, 398);
	if (pn == 61)
		ObjSetMicro(d, 399);
	if (pn == 67)
		ObjSetMicro(d, 400);
	if (pn == 68)
		ObjSetMicro(d, 401);
	if (pn == 69)
		ObjSetMicro(d, 403);
	if (pn == 70)
		ObjSetMicro(d, 404);
	if (pn == 72)
		ObjSetMicro(d, 406);
	if (pn == 212)
		ObjSetMicro(d, 407);
	if (pn == 354)
		ObjSetMicro(d, 409);
	if (pn == 355)
		ObjSetMicro(d, 410);
	if (pn == 411)
		ObjSetMicro(d, 396);
	if (pn == 412)
		ObjSetMicro(d, 396);
}

void RedoPlayerVision()
{
	int p;

	for (p = 0; p < MAX_PLRS; p++) {
		if (plr[p].data.plractive && lvl.currlevel == plr[p].data.plrlevel) {
			ChangeVisionXY(plr[p].data._pvid, plr[p].pos());
		}
	}
}

bool _SharedDoorStart(int oi)
{
	if (object[oi]._oVar4 == 2) {
		if (!deltaload)
			PlaySfxLoc(IS_DOORCLOS, object[oi]._o);
		return false;
	}
	return true;
}

void _SharedDoorOpen(int pnum, int oi, bool sendflag)
{
	if (pnum == myplr() && sendflag)
		NetSendCmdParam1(true, Cmd::OPENDOOR, oi);
	if (!deltaload)
		PlaySfxLoc(IS_DOOROPEN, object[oi]._o);
	object[oi]._oAnimFrame += 2;
	object[oi]._oPreFlag = true;
	object[oi]._oVar4 = 1;
	object[oi]._oSelFlag = 2;
	RedoPlayerVision();
}

void _SharedDoorClose(int pnum, int oi, bool sendflag)
{
	if (pnum == myplr() && sendflag)
		NetSendCmdParam1(true, Cmd::CLOSEDOOR, oi);
	object[oi]._oVar4 = 0;
	object[oi]._oSelFlag = 3;
	object[oi]._oAnimFrame -= 2;
	object[oi]._oPreFlag = false;
	RedoPlayerVision();
}

bool _isDoorFree(V2Di p)
{
	return (((grid.at(p).dDead != 0 ? 0 : 1) & (grid.at(p).getActor() != 0 ? 0 : 1) & (grid.at(p).isItem() ? 0 : 1)) != 0);
}

void OperateL1RDoor(int pnum, int oi, bool sendflag)
{
	if (!_SharedDoorStart(oi)) return;

	V2Di p = object[oi]._o;
	if (object[oi]._oVar4 == 0) {
		ObjSetMicro(p, 395);
		grid.at(p).dSpecial = 8;
		objects_set_door_piece({ p.x, p.y - 1 });
		DoorSet(oi, { p.x - 1, p.y });
		_SharedDoorOpen(pnum, oi, sendflag);
		return;
	}

	if (!deltaload)
		PlaySfxLoc(IS_DOORCLOS, object[oi]._o);
	if (_isDoorFree(p)) {
		ObjSetMicro(p, object[oi]._oVar1);
		if (object[oi]._oVar2 != 50) {
			ObjSetMicro({p.x - 1, p.y}, object[oi]._oVar2);
		} else {
			if (grid[p.x - 1][p.y].getPiece() == 396) ObjSetMicro({p.x- 1, p.y}, 411);
			else ObjSetMicro({ p.x - 1, p.y }, 50);
		}
		_SharedDoorClose(pnum, oi, sendflag);
	} else {
		object[oi]._oVar4 = 2;
	}
}

void OperateL1LDoor(int pnum, int oi, bool sendflag)
{
	if (!_SharedDoorStart(oi)) return;

	V2Di p = object[oi]._o;
	if (object[oi]._oVar4 == 0) {
		if (object[oi]._oVar1 == 214)
			ObjSetMicro(p, 408);
		else
			ObjSetMicro(p, 393);
		grid.at(p).dSpecial = 7;
		objects_set_door_piece({p.x - 1, p.y});
		DoorSet(oi, {p.x, p.y - 1 });
		_SharedDoorOpen(pnum, oi, sendflag);
		return;
	}

	if (!deltaload)
		PlaySfxLoc(IS_DOORCLOS, object[oi]._o);
	if (_isDoorFree(p)) {
		ObjSetMicro(p, object[oi]._oVar1);
		if (object[oi]._oVar2 != 50) {
			ObjSetMicro({ p.x, p.y - 1}, object[oi]._oVar2);
		} else {
			if (grid[p.x][p.y - 1].getPiece() == 396)
				ObjSetMicro({ p.x, p.y - 1 }, 412);
			else
				ObjSetMicro({ p.x, p.y - 1 }, 50);
		}
		_SharedDoorClose(pnum, oi, sendflag);
	} else {
		object[oi]._oVar4 = 2;
	}
}

void OperateL2RDoor(int pnum, int oi, bool sendflag)
{
	if (!_SharedDoorStart(oi)) return;

	V2Di p = object[oi]._o;
	if (object[oi]._oVar4 == 0) {
		ObjSetMicro(p, 17);
		_SharedDoorOpen(pnum, oi, sendflag);
		return;
	}

	if (!deltaload)
		PlaySfxLoc(IS_DOORCLOS, object[oi]._o);
	if (_isDoorFree(p)) {
		ObjSetMicro(p, 540);
		_SharedDoorClose(pnum, oi, sendflag);
	} else {
		object[oi]._oVar4 = 2;
	}
}

void OperateL2LDoor(int pnum, int oi, bool sendflag)
{
	if (!_SharedDoorStart(oi)) return;

	V2Di p = object[oi]._o;
	if (object[oi]._oVar4 == 0) {
		ObjSetMicro(p, 13);
		_SharedDoorOpen(pnum, oi, sendflag);
		return;
	}

	if (!deltaload)
		PlaySfxLoc(IS_DOORCLOS, object[oi]._o);
	if (_isDoorFree(p)) {
		ObjSetMicro(p, 538);
		_SharedDoorClose(pnum, oi, sendflag);
	} else {
		object[oi]._oVar4 = 2;
	}
}

void OperateL3RDoor(int pnum, int oi, bool sendflag)
{
	if (!_SharedDoorStart(oi)) return;

	V2Di p = object[oi]._o;
	if (object[oi]._oVar4 == 0) {
		ObjSetMicro(p, 541);
		_SharedDoorOpen(pnum, oi, sendflag);
		return;
	}

	if (!deltaload)
		PlaySfxLoc(IS_DOORCLOS, object[oi]._o);
	if (_isDoorFree(p)) {
		ObjSetMicro(p, 534);
		_SharedDoorClose(pnum, oi, sendflag);
	} else {
		object[oi]._oVar4 = 2;
	}
}

void OperateL3LDoor(int pnum, int oi, bool sendflag)
{
	if (!_SharedDoorStart(oi)) return;

	V2Di p = object[oi]._o;
	if (object[oi]._oVar4 == 0) {
		ObjSetMicro(p, 538);
		_SharedDoorOpen(pnum, oi, sendflag);
		return;
	}

	if (!deltaload)
		PlaySfxLoc(IS_DOORCLOS, object[oi]._o);
	if (_isDoorFree(p)) {
		ObjSetMicro(p, 531);
		_SharedDoorClose(pnum, oi, sendflag);
	} else {
		object[oi]._oVar4 = 2;
	}
}

void MonstCheckDoors(int n)
{
	int i, oi;
	V2Di dp, m;
	m = monsters[n].data._m;
	if (grid[m.x - 1][m.y - 1].isObject()
	    || grid[m.x][m.y - 1].isObject()
	    || grid[m.x + 1][m.y - 1].isObject()
	    || grid[m.x - 1][m.y].isObject()
	    || grid[m.x + 1][m.y].isObject()
	    || grid[m.x - 1][m.y + 1].isObject()
	    || grid[m.x][m.y + 1].isObject()
	    || grid[m.x + 1][m.y + 1].isObject()) {
		for (i = 0; i < nobjects; ++i) {
			oi = objectactive[i];
			if ((object[oi]._otype == ObjectType::L1LDOOR || object[oi]._otype == ObjectType::L1RDOOR) && !object[oi]._oVar4) {
				dp.x = abs(object[oi]._o.x - m.x);
				dp.y = abs(object[oi]._o.y - m.y);
				if (dp.x == 1 && dp.y <= 1 && object[oi]._otype == ObjectType::L1LDOOR)
					OperateL1LDoor(myplr(), oi, true);
				if (dp.x <= 1 && dp.y == 1 && object[oi]._otype == ObjectType::L1RDOOR)
					OperateL1RDoor(myplr(), oi, true);
			}
			if ((object[oi]._otype == ObjectType::L2LDOOR || object[oi]._otype == ObjectType::L2RDOOR) && !object[oi]._oVar4) {
				dp.x = abs(object[oi]._o.x - m.x);
				dp.y = abs(object[oi]._o.y - m.y);
				if (dp.x == 1 && dp.y <= 1 && object[oi]._otype == ObjectType::L2LDOOR)
					OperateL2LDoor(myplr(), oi, true);
				if (dp.x <= 1 && dp.y == 1 && object[oi]._otype == ObjectType::L2RDOOR)
					OperateL2RDoor(myplr(), oi, true);
			}
			if ((object[oi]._otype == ObjectType::L3LDOOR || object[oi]._otype == ObjectType::L3RDOOR) && !object[oi]._oVar4) {
				dp.x = abs(object[oi]._o.x - m.x);
				dp.y = abs(object[oi]._o.y - m.y);
				if (dp.x == 1 && dp.y <= 1 && object[oi]._otype == ObjectType::L3RDOOR)
					OperateL3RDoor(myplr(), oi, true);
				if (dp.x <= 1 && dp.y == 1 && object[oi]._otype == ObjectType::L3LDOOR)
					OperateL3LDoor(myplr(), oi, true);
			}
		}
	}
}

void ObjChangeMap(V2Di p1, V2Di p2)
{
	for (int j = p1.y; j <= p2.y; j++) {
		for (int i = p1.x; i <= p2.x; i++) {
			ObjSetMini({ i, j }, dgrid[i][j].pdungeon);
			dgrid[i][j].dungeon = dgrid[i][j].pdungeon;
		}
	}
	if (lvl.type() == DunType::cathedral) {
		ObjL1Special({2 * p1.x + 16, 2 * p1.y + 16 }, { 2 * p2.x + 17, 2 * p2.y + 17 });
		AddL1Objs({ 2 * p1.x + 16, 2 * p1.y + 16 }, { 2 * p2.x + 17, 2 * p2.y + 17 });
	}
	if (lvl.type() == DunType::catacombs) {
		ObjL2Special({ 2 * p1.x + 16, 2 * p1.y + 16 }, { 2 * p2.x + 17, 2 * p2.y + 17 });
		AddL2Objs({ 2 * p1.x + 16, 2 * p1.y + 16 }, { 2 * p2.x + 17, 2 * p2.y + 17 });
	}
}

void ObjChangeMapResync(V2Di p1, V2Di p2)
{
	for (int j = p1.y; j <= p2.y; j++) {
		for (int i = p1.x; i <= p2.x; i++) {
			ObjSetMini({ i, j }, dgrid[i][j].pdungeon);
			dgrid[i][j].dungeon = dgrid[i][j].pdungeon;
		}
	}
	if (lvl.type() == DunType::cathedral) {
		ObjL1Special({ 2 * p1.x + 16, 2 * p1.y + 16 }, { 2 * p2.x + 17, 2 * p2.y + 17 });
	}
	if (lvl.type() == DunType::catacombs) {
		ObjL2Special({ 2 * p1.x + 16, 2 * p1.y + 16 }, { 2 * p2.x + 17, 2 * p2.y + 17 });
	}
}

void OperateL1Door(int pnum, int i, bool sendflag)
{
	V2Di dp = (object[i]._o - plr[pnum].pos()).abs();
	if (dp.x == 1 && dp.y <= 1 && object[i]._otype == ObjectType::L1LDOOR)
		OperateL1LDoor(pnum, i, sendflag);
	if (dp.x <= 1 && dp.y == 1 && object[i]._otype == ObjectType::L1RDOOR)
		OperateL1RDoor(pnum, i, sendflag);
}

void OperateLever(int pnum, int i)
{
	int j, oi;
	bool mapflag;

	if (object[i]._oSelFlag != 0) {
		if (!deltaload) PlaySfxLoc(IS_LEVER, object[i]._o);
		object[i]._oSelFlag = 0;
		object[i]._oAnimFrame++;
		mapflag = true;
		if (lvl.currlevel == 16) {
			for (j = 0; j < nobjects; j++) {
				oi = objectactive[j];
				if (object[oi]._otype == ObjectType::SWITCHSKL
				    && object[i]._oVar8 == object[oi]._oVar8
				    && object[oi]._oSelFlag != 0) {
					mapflag = false;
				}
			}
		}
		if (mapflag)
			ObjChangeMap({ object[i]._oVar1, object[i]._oVar2 }, { object[i]._oVar3, object[i]._oVar4 });
		if (pnum == myplr())
			NetSendCmdParam1(false, Cmd::OPERATEOBJ, i);
	}
}

void OperateBook(int pnum, int i)
{
	int j, oi;
	V2Di d;
	int otype;
	bool do_add_missile, missile_added;

	if (object[i]._oSelFlag == 0)
		return;
	if (lvl.setlevel && lvl.setlvlnum == SetLvl::VileBetrayer) {
		do_add_missile = false;
		missile_added = false;
		for (j = 0; j < nobjects; j++) {
			oi = objectactive[j];
			otype = object[oi]._otype;
			if (otype == ObjectType::MCIRCLE2 && object[oi]._oVar6 == 1) {
				d = { 27, 29 };
				object[oi]._oVar6 = 4;
				do_add_missile = true;
			}
			if (otype == ObjectType::MCIRCLE2 && object[oi]._oVar6 == 2) {
				d = { 43, 29 };
				object[oi]._oVar6 = 4;
				do_add_missile = true;
			}
			if (do_add_missile) {
				object[grid[35][36].getObject()]._oVar5++;
				AddMissile(plr[pnum].pos(), d, plr[pnum].data._pdir, MissileType::RNDTELEPORT, 0, pnum, 0, 0);
				missile_added = true;
				do_add_missile = false;
			}
		}
		if (!missile_added)
			return;
	}
	object[i]._oSelFlag = 0;
	object[i]._oAnimFrame++;
	if (!lvl.setlevel)
		return;

	if (lvl.setlvlnum == SetLvl::BoneChamb) {
		myplr().data._pMemorySpells |= ((__int64)1 << (SpellId::GUARDIAN - 1));
		if (plr[pnum].data._pSplLvl[SpellId::GUARDIAN] < 15)
			myplr().data._pSplLvl[SpellId::GUARDIAN]++;
		quests[QuestId::schamb]._qactive = QuestState::done;
		if (!deltaload)
			PlaySfxLoc(IS_QUESTDN, object[i]._o);
		InitDiabloMsg(EMSG_BONECHAMB);
		AddMissile(myplr().pos(), { object[i]._o.x - 2, object[i]._o.y - 4 }, myplr().data._pdir,
		    MissileType::GUARDIAN, 0, myplr(), 0, 0);
	}
	if (lvl.setlevel && lvl.setlvlnum == SetLvl::VileBetrayer) {
		ObjChangeMapResync( { object[i]._oVar1, object[i]._oVar2 },
		    { object[i]._oVar3, object[i]._oVar4 });
		for (j = 0; j < nobjects; j++)
			SyncObjectAnim(objectactive[j]);
	}
}

void OperateBookLever(int pnum, int i)
{
	int tren;
	V2Di p;
	p.x = 2 * lvl.getpc().x + 16;
	p.y = 2 * lvl.getpc().y + 16;
	if (object[i]._oSelFlag != 0 && !qtextflag) {
		if (object[i]._otype == ObjectType::BLINDBOOK && !quests[QuestId::blind]._qvar1) {
			quests[QuestId::blind]._qactive = QuestState::active;
			quests[QuestId::blind]._qlog = 1;
			quests[QuestId::blind]._qvar1 = 1;
		}
		if (object[i]._otype == ObjectType::BLOODBOOK && !quests[QuestId::blood]._qvar1) {
			quests[QuestId::blood]._qactive = QuestState::active;
			quests[QuestId::blood]._qlog = 1;
			quests[QuestId::blood]._qvar1 = 1;
			SpawnQuestItem(ItemIndex::BLDSTONE, { 2 * lvl.getpc().x + 19, 2 * lvl.getpc().y + 26 }, 0, 1);
			SpawnQuestItem(ItemIndex::BLDSTONE, { 2 * lvl.getpc().x + 31, 2 * lvl.getpc().y + 26 }, 0, 1);
			SpawnQuestItem(ItemIndex::BLDSTONE, { 2 * lvl.getpc().x + 25, 2 * lvl.getpc().y + 33 }, 0, 1);
		}
		object[i]._otype = object[i]._otype;
		if (object[i]._otype == ObjectType::STEELTOME && !quests[QuestId::warlord]._qvar1) {
			quests[QuestId::warlord]._qactive = QuestState::active;
			quests[QuestId::warlord]._qlog = 1;
			quests[QuestId::warlord]._qvar1 = 1;
		}
		if (object[i]._oAnimFrame != object[i]._oVar6) {
			if (object[i]._otype != ObjectType::BLOODBOOK)
				ObjChangeMap({ object[i]._oVar1, object[i]._oVar2 }, { object[i]._oVar3, object[i]._oVar4 });
			if (object[i]._otype == ObjectType::BLINDBOOK) {
				CreateItem(3, { p.x + 5, p.y + 5 });
				tren = lvl.TransVal;
				lvl.TransVal = 9;
				DRLG_MRectTrans(object[i]._oVar1, object[i]._oVar2, object[i]._oVar3, object[i]._oVar4);
				lvl.TransVal = tren;
			}
		}
		object[i]._oAnimFrame = object[i]._oVar6;
		InitQTextMsg(object[i]._oVar7);
		if (pnum == myplr())
			NetSendCmdParam1(false, Cmd::OPERATEOBJ, i);
	}
}

void OperateSChambBk(int pnum, int i)
{
	int j, textdef;

	if (object[i]._oSelFlag != 0 && !qtextflag) {
		if (object[i]._oAnimFrame != object[i]._oVar6) {
			ObjChangeMapResync({ object[i]._oVar1, object[i]._oVar2 }, { object[i]._oVar3, object[i]._oVar4 });
			for (j = 0; j < nobjects; j++)
				SyncObjectAnim(objectactive[j]);
		}
		object[i]._oAnimFrame = object[i]._oVar6;
		if (quests[QuestId::schamb]._qactive == QuestState::init) {
			quests[QuestId::schamb]._qactive = QuestState::active;
			quests[QuestId::schamb]._qlog = 1;
		}
		if (myplr().data._pClass == PlayerClass::warrior) {
			textdef = TEXT_BONER;
		} else if (myplr().data._pClass == PlayerClass::rogue) {
			textdef = TEXT_RBONER;
		} else if (myplr().data._pClass == PlayerClass::sorceror) {
			textdef = TEXT_MBONER;
		}
		quests[QuestId::schamb]._qmsg = textdef;
		InitQTextMsg(textdef);
	}
}

void OperateChest(int pnum, int i, bool sendmsg)
{
	int j, mtype;
	Dir mdir;

	if (object[i]._oSelFlag != 0) {
		if (!deltaload) PlaySfxLoc(IS_CHEST, object[i]._o);
		object[i]._oSelFlag = 0;
		object[i]._oAnimFrame += 2;
		if (!deltaload) {
			SetRndSeed(object[i]._oRndSeed);
			if (lvl.setlevel) {
				for (j = 0; j < object[i]._oVar1; j++) {
					CreateRndItem(object[i]._o, true, sendmsg, false);
				}
			} else {
				for (j = 0; j < object[i]._oVar1; j++) {
					if (object[i]._oVar2)
						CreateRndItem(object[i]._o, false, sendmsg, false);
					else
						CreateRndUseful(pnum, object[i]._o, sendmsg);
				}
			}
			if (object[i]._oTrapFlag && object[i]._otype >= ObjectType::TCHEST1 && object[i]._otype <= ObjectType::TCHEST3) {
				mdir = GetDirection(object[i]._o, plr[pnum].pos());
				switch (object[i]._oVar4) {
				case 0:
					mtype = MissileType::ARROW;
					break;
				case 1:
					mtype = MissileType::FARROW;
					break;
				case 2:
					mtype = MissileType::NOVA;
					break;
				}
				AddMissile(object[i]._o, plr[pnum].pos(), mdir, mtype, 1, -1, 0, 0);
				object[i]._oTrapFlag = false;
			}
			if (pnum == myplr())
				NetSendCmdParam2(false, Cmd::PLROPOBJ, pnum, i);
			return;
		}
	}
}

void OperateMushPatch(int pnum, int i)
{
	V2Di p;
	if (quests[QuestId::mushroom]._qactive != QuestState::active || quests[QuestId::mushroom]._qvar1 < QuestMushState::tomegiven) {
		if (!deltaload && pnum == myplr()) {
			if (myplr().data._pClass == PlayerClass::warrior) {
				PlaySFX(PS_WARR13);
			} else if (myplr().data._pClass == PlayerClass::rogue) {
				PlaySFX(PS_ROGUE13);
			} else if (myplr().data._pClass == PlayerClass::sorceror) {
				PlaySFX(PS_MAGE13);
			}
		}
	} else {
		if (object[i]._oSelFlag != 0) {
			if (!deltaload)
				PlaySfxLoc(IS_CHEST, object[i]._o);
			object[i]._oSelFlag = 0;
			object[i]._oAnimFrame++;
			if (!deltaload) {
				GetSuperItemLoc(object[i]._o, p);
				SpawnQuestItem(ItemIndex::MUSHROOM, p, 0, 0);
				quests[QuestId::mushroom]._qvar1 = QuestMushState::mushspawned;
			}
		}
	}
}

void OperateInnSignChest(int pnum, int i)
{
	V2Di p;
	if (quests[QuestId::ltbanner]._qvar1 != 2) {
		if (!deltaload && pnum == myplr()) {
			if (myplr().data._pClass == PlayerClass::warrior) {
				PlaySFX(PS_WARR24);
			} else if (myplr().data._pClass == PlayerClass::rogue) {
				PlaySFX(PS_ROGUE24);
			} else if (myplr().data._pClass == PlayerClass::sorceror) {
				PlaySFX(PS_MAGE24);
			}
		}
	} else {
		if (object[i]._oSelFlag != 0) {
			if (!deltaload)
				PlaySfxLoc(IS_CHEST, object[i]._o);
			object[i]._oSelFlag = 0;
			object[i]._oAnimFrame += 2;
			if (!deltaload) {
				GetSuperItemLoc(object[i]._o, p);
				SpawnQuestItem(ItemIndex::BANNER, p, 0, 0);
			}
		}
	}
}

void OperateSlainHero(int pnum, int i, bool sendmsg)
{
	if (object[i]._oSelFlag != 0) {
		object[i]._oSelFlag = 0;
		if (!deltaload) {
			if (plr[pnum].data._pClass == PlayerClass::warrior) {
				CreateMagicArmor(object[i]._o, ItemType::heavy_armor, ItemCursor::BREAST_PLATE, false, true);
				PlaySfxLoc(PS_WARR9, myplr().pos());
			} else if (plr[pnum].data._pClass == PlayerClass::rogue) {
				CreateMagicWeapon(object[i]._o, ItemType::bow, ItemCursor::LONGBOW, false, true);
				PlaySfxLoc(PS_ROGUE9, myplr().pos());
			} else if (plr[pnum].data._pClass == PlayerClass::sorceror) {
				CreateSpellBook(object[i]._o, SpellId::LIGHTNING, false, true);
				PlaySfxLoc(PS_MAGE9, myplr().pos());
			}
			if (pnum == myplr())
				NetSendCmdParam1(false, Cmd::OPERATEOBJ, i);
		}
	}
}

void OperateTrapLvr(int i)
{
	int frame, j, oi;

	frame = object[i]._oAnimFrame;
	j = 0;

	if (frame == 1) {
		object[i]._oAnimFrame = 2;
		for (; j < nobjects; j++) {
			oi = objectactive[j];
			if (object[oi]._otype == object[i]._oVar2 && object[oi]._oVar1 == object[i]._oVar1) {
				object[oi]._oVar2 = 1;
				object[oi]._oAnimFlag = 0;
			}
		}
		return;
	}

	object[i]._oAnimFrame = frame - 1;
	for (; j < nobjects; j++) {
		oi = objectactive[j];
		if (object[oi]._otype == object[i]._oVar2 && object[oi]._oVar1 == object[i]._oVar1) {
			object[oi]._oVar2 = 0;
			if (object[oi]._oVar4 != 0)
				object[oi]._oAnimFlag = 1;
		}
	}
}

void OperateSarc(int pnum, int i, bool sendmsg)
{
	if (object[i]._oSelFlag != 0) {
		if (!deltaload)
			PlaySfxLoc(IS_SARC, object[i]._o);
		object[i]._oSelFlag = 0;
		if (deltaload) {
			object[i]._oAnimFrame = object[i]._oAnimLen;
		} else {
			object[i]._oAnimFlag = 1;
			object[i]._oAnimDelay = 3;
			SetRndSeed(object[i]._oRndSeed);
			if (object[i]._oVar1 <= 2)
				CreateRndItem(object[i]._o, false, sendmsg, false);
			if (object[i]._oVar1 >= 8)
				SpawnSkeleton(object[i]._oVar2, object[i]._o);
			if (pnum == myplr())
				NetSendCmdParam1(false, Cmd::OPERATEOBJ, i);
		}
	}
}

void OperateL2Door(int pnum, int i, bool sendflag)
{
	V2Di dp = (object[i]._o - plr[pnum].pos()).abs();
	if (dp.x == 1 && dp.y <= 1 && object[i]._otype == ObjectType::L2LDOOR)
		OperateL2LDoor(pnum, i, sendflag);
	if (dp.x <= 1 && dp.y == 1 && object[i]._otype == ObjectType::L2RDOOR)
		OperateL2RDoor(pnum, i, sendflag);
}

void OperateL3Door(int pnum, int i, bool sendflag)
{
	V2Di dp = (object[i]._o - plr[pnum].pos()).abs();
	if (dp.x == 1 && dp.y <= 1 && object[i]._otype == ObjectType::L3RDOOR)
		OperateL3RDoor(pnum, i, sendflag);
	if (dp.x <= 1 && dp.y == 1 && object[i]._otype == ObjectType::L3LDOOR)
		OperateL3LDoor(pnum, i, sendflag);
}

void OperatePedistal(int pnum, int i)
{
	uint8_t *mem;
	int iv;

	if (object[i]._oVar6 != 3) {
		if (HasItem(pnum, ItemIndex::BLDSTONE, &iv) != NULL) {
			plr[pnum].inventory.RemoveInvItem(iv);
			object[i]._oAnimFrame++;
			object[i]._oVar6++;
		}
		if (object[i]._oVar6 == 1) {
			if (!deltaload)
				PlaySfxLoc(LS_PUDDLE, object[i]._o);
			ObjChangeMap({ lvl.getpc().x, lvl.getpc().y + 3 }, { lvl.getpc().x + 2, lvl.getpc().y + 7 });
		}
		if (object[i]._oVar6 == 2) {
			if (!deltaload)
				PlaySfxLoc(LS_PUDDLE, object[i]._o);
			ObjChangeMap({ lvl.getpc().x + 6, lvl.getpc().y + 3 }, { lvl.getpc().x + lvl.getpc().w, lvl.getpc().y + 7 });
		}
		if (object[i]._oVar6 == 3) {
			if (!deltaload)
				PlaySfxLoc(LS_BLODSTAR, object[i]._o);
			ObjChangeMap({ object[i]._oVar1, object[i]._oVar2 }, { object[i]._oVar3, object[i]._oVar4 });
			mem = LoadFileInMem("Levels\\L2Data\\Blood2.DUN", NULL);
			LoadMapObjs(mem, { 2 * lvl.getpc().x, 2 * lvl.getpc().y });
			mem_free_dbg(mem);
			CreateItem(7, { 2 * lvl.getpc().x + 25, 2 * lvl.getpc().y + 19 });
			object[i]._oSelFlag = 0;
		}
	}
}

void TryDisarm(int pnum, int i)
{
	int j, oi, oti, trapdisper;
	bool checkflag;
	if (pnum == myplr())
		SetCursor_(Cursor::HAND);
	if (object[i]._oTrapFlag) {
		trapdisper = 2 * plr[pnum].data._pDexterity - 5 * lvl.currlevel;
		if (random_(154, 100) <= trapdisper) {
			for (j = 0; j < nobjects; j++) {
				checkflag = false;
				oi = objectactive[j];
				oti = object[oi]._otype;
				if (oti == ObjectType::TRAPL)
					checkflag = true;
				if (oti == ObjectType::TRAPR)
					checkflag = true;
				if (checkflag && grid[object[oi]._oVar1][object[oi]._oVar2].getObject() == i) {
					object[oi]._oVar4 = 1;
					object[i]._oTrapFlag = false;
				}
			}
			oti = object[i]._otype;
			if (oti >= ObjectType::TCHEST1 && oti <= ObjectType::TCHEST3)
				object[i]._oTrapFlag = false;
		}
	}
}

int ItemMiscIdIdx(int imiscid)
{
	int i = 0;
	while (AllItemsList[i].iRnd == 0 || AllItemsList[i].iMiscId != imiscid) {
		i++;
	}
	return i;
}

void OperateShrine(int pnum, int i, int sType)
{
	int cnt;
	int r, j;
	DWORD lv, t;
	V2Di p;
	int v1, v2, v3, v4;
	unsigned __int64 spell, spells;

	if (dropGoldFlag) {
		dropGoldFlag = false;
		dropGoldValue = 0;
	}

	assert((DWORD)i < MAXOBJECTS);

	if (object[i]._oSelFlag == 0)
		return;

	SetRndSeed(object[i]._oRndSeed);
	object[i]._oSelFlag = 0;

	if (!deltaload) {
		PlaySfxLoc(sType, object[i]._o);
		object[i]._oAnimFlag = 1;
		object[i]._oAnimDelay = 1;
	} else {
		object[i]._oAnimFlag = 0;
		object[i]._oAnimFrame = object[i]._oAnimLen;
	}
	switch (object[i]._oVar1) {
	case Shrine::MYSTERIOUS:
		if (deltaload)
			return;
		if (pnum != myplr())
			return;
		plr[pnum].ModifyPlrStr(-1);
		plr[pnum].ModifyPlrMag(-1);
		plr[pnum].ModifyPlrDex(-1);
		plr[pnum].ModifyPlrVit(-1);
		switch (random_(0, 4)) {
		case 0:
			plr[pnum].ModifyPlrStr(6);
			break;
		case 1:
			plr[pnum].ModifyPlrMag(6);
			break;
		case 2:
			plr[pnum].ModifyPlrDex(6);
			break;
		case 3:
			plr[pnum].ModifyPlrVit(6);
			break;
		}
		plr[pnum].CheckStats();
		InitDiabloMsg(EMSG_SHRINE_MYSTERIOUS);
		break;
	case Shrine::HIDDEN:
		cnt = 0;
		if (deltaload)
			return;
		if (pnum != myplr())
			return;
		for (j = 0; j < MAXINVITEMS; j++) {
			if (plr[pnum].data.InvBody[j]._itype != ItemType::none)
				cnt++;
		}
		if (cnt > 0) {
			for (j = 0; j < MAXINVITEMS; j++) {
				if (plr[pnum].data.InvBody[j]._itype != ItemType::none
				    && plr[pnum].data.InvBody[j]._iMaxDur != DUR_INDESTRUCTIBLE
				    && plr[pnum].data.InvBody[j]._iMaxDur != 0) {
					plr[pnum].data.InvBody[j]._iDurability += 10;
					plr[pnum].data.InvBody[j]._iMaxDur += 10;
					if (plr[pnum].data.InvBody[j]._iDurability > plr[pnum].data.InvBody[j]._iMaxDur)
						plr[pnum].data.InvBody[j]._iDurability = plr[pnum].data.InvBody[j]._iMaxDur;
				}
			}
			while (true) {
				cnt = 0;
				for (j = 0; j < MAXINVITEMS; j++) {
					if (plr[pnum].data.InvBody[j]._itype != ItemType::none
					    && plr[pnum].data.InvBody[j]._iMaxDur != DUR_INDESTRUCTIBLE
					    && plr[pnum].data.InvBody[j]._iMaxDur != 0)
						cnt++;
				}
				if (cnt == 0)
					break;
				r = random_(0, MAXINVITEMS);
				if (plr[pnum].data.InvBody[r]._itype == ItemType::none || plr[pnum].data.InvBody[r]._iMaxDur == DUR_INDESTRUCTIBLE || plr[pnum].data.InvBody[r]._iMaxDur == 0)
					continue;

				plr[pnum].data.InvBody[r]._iDurability -= 20;
				plr[pnum].data.InvBody[r]._iMaxDur -= 20;
				if (plr[pnum].data.InvBody[r]._iDurability <= 0)
					plr[pnum].data.InvBody[r]._iDurability = 1;
				if (plr[pnum].data.InvBody[r]._iMaxDur <= 0)
					plr[pnum].data.InvBody[r]._iMaxDur = 1;
				break;
			}
		}
		InitDiabloMsg(EMSG_SHRINE_HIDDEN);
		break;
	case Shrine::GLOOMY:
		if (deltaload)
			return;
		if (pnum != myplr())
			break;
		if (plr[pnum].data.InvBody[INVLOC_HEAD]._itype != ItemType::none)
			plr[pnum].data.InvBody[INVLOC_HEAD]._iAC += 2;
		if (plr[pnum].data.InvBody[INVLOC_CHEST]._itype != ItemType::none)
			plr[pnum].data.InvBody[INVLOC_CHEST]._iAC += 2;
		if (plr[pnum].data.InvBody[INVLOC_HAND_LEFT]._itype != ItemType::none) {
			if (plr[pnum].data.InvBody[INVLOC_HAND_LEFT]._itype == ItemType::shield) {
				plr[pnum].data.InvBody[INVLOC_HAND_LEFT]._iAC += 2;
			} else {
				plr[pnum].data.InvBody[INVLOC_HAND_LEFT]._iMaxDam--;
				if (plr[pnum].data.InvBody[INVLOC_HAND_LEFT]._iMaxDam < plr[pnum].data.InvBody[INVLOC_HAND_LEFT]._iMinDam)
					plr[pnum].data.InvBody[INVLOC_HAND_LEFT]._iMaxDam = plr[pnum].data.InvBody[INVLOC_HAND_LEFT]._iMinDam;
			}
		}
		if (plr[pnum].data.InvBody[INVLOC_HAND_RIGHT]._itype != ItemType::none) {
			if (plr[pnum].data.InvBody[INVLOC_HAND_RIGHT]._itype == ItemType::shield) {
				plr[pnum].data.InvBody[INVLOC_HAND_RIGHT]._iAC += 2;
			} else {
				plr[pnum].data.InvBody[INVLOC_HAND_RIGHT]._iMaxDam--;
				if (plr[pnum].data.InvBody[INVLOC_HAND_RIGHT]._iMaxDam < plr[pnum].data.InvBody[INVLOC_HAND_RIGHT]._iMinDam)
					plr[pnum].data.InvBody[INVLOC_HAND_RIGHT]._iMaxDam = plr[pnum].data.InvBody[INVLOC_HAND_RIGHT]._iMinDam;
			}
		}
		for (j = 0; j < plr[pnum].data._pNumInv; j++) {
			switch (plr[pnum].data.InvList[j]._itype) {
			case ItemType::sword:
			case ItemType::axe:
			case ItemType::bow:
			case ItemType::mace:
			case ItemType::staff:
				plr[pnum].data.InvList[j]._iMaxDam--;
				if (plr[pnum].data.InvList[j]._iMaxDam < plr[pnum].data.InvList[j]._iMinDam)
					plr[pnum].data.InvList[j]._iMaxDam = plr[pnum].data.InvList[j]._iMinDam;
				break;
			case ItemType::shield:
			case ItemType::light_armor:
			case ItemType::helm:
			case ItemType::medium_armor:
			case ItemType::heavy_armor:
				plr[pnum].data.InvList[j]._iAC += 2;
				break;
			}
		}
		InitDiabloMsg(EMSG_SHRINE_GLOOMY);
		break;
	case Shrine::WEIRD:
		if (deltaload)
			return;
		if (pnum != myplr())
			break;

		if (plr[pnum].data.InvBody[INVLOC_HAND_LEFT]._itype != ItemType::none && plr[pnum].data.InvBody[INVLOC_HAND_LEFT]._itype != ItemType::shield)
			plr[pnum].data.InvBody[INVLOC_HAND_LEFT]._iMaxDam++;
		if (plr[pnum].data.InvBody[INVLOC_HAND_RIGHT]._itype != ItemType::none && plr[pnum].data.InvBody[INVLOC_HAND_RIGHT]._itype != ItemType::shield)
			plr[pnum].data.InvBody[INVLOC_HAND_RIGHT]._iMaxDam++;
		for (j = 0; j < plr[pnum].data._pNumInv; j++) {
			switch (plr[pnum].data.InvList[j]._itype) {
			case ItemType::sword:
			case ItemType::axe:
			case ItemType::bow:
			case ItemType::mace:
			case ItemType::staff:
				plr[pnum].data.InvList[j]._iMaxDam++;
				break;
			}
		}
		InitDiabloMsg(EMSG_SHRINE_WEIRD);
		break;
	case Shrine::MAGICAL:

	case Shrine::MAGICAL2:
		if (deltaload)
			return;
		AddMissile(
		    plr[pnum].pos(),
		    plr[pnum].pos(),
		    plr[pnum].data._pdir,
		    MissileType::MANASHIELD,
		    -1,
		    pnum,
		    0,
		    2 * int(lvl.type()));
		if (pnum != myplr())
			return;
		InitDiabloMsg(EMSG_SHRINE_MAGICAL);
		break;
	case Shrine::STONE:
		if (deltaload)
			return;
		if (pnum != myplr())
			break;

		for (j = 0; j < MAXINVITEMS; j++) {
			if (plr[pnum].data.InvBody[j]._itype == ItemType::staff)
				plr[pnum].data.InvBody[j]._iCharges = plr[pnum].data.InvBody[j]._iMaxCharges;
		}
		for (j = 0; j < plr[pnum].data._pNumInv; j++) {
			if (plr[pnum].data.InvList[j]._itype == ItemType::staff)
				plr[pnum].data.InvList[j]._iCharges = plr[pnum].data.InvList[j]._iMaxCharges;
		}
		for (j = 0; j < MAXBELTITEMS; j++) {
			if (plr[pnum].data.SpdList[j]._itype == ItemType::staff)
				plr[pnum].data.SpdList[j]._iCharges = plr[pnum].data.SpdList[j]._iMaxCharges; // belt items don't have charges?
		}
		InitDiabloMsg(EMSG_SHRINE_STONE);
		break;
	case Shrine::RELIGIOUS:
		if (deltaload)
			return;
		if (pnum != myplr())
			break;

		for (j = 0; j < MAXINVITEMS; j++)
			plr[pnum].data.InvBody[j]._iDurability = plr[pnum].data.InvBody[j]._iMaxDur;
		for (j = 0; j < plr[pnum].data._pNumInv; j++)
			plr[pnum].data.InvList[j]._iDurability = plr[pnum].data.InvList[j]._iMaxDur;
		for (j = 0; j < MAXBELTITEMS; j++)
			plr[pnum].data.SpdList[j]._iDurability = plr[pnum].data.SpdList[j]._iMaxDur; // belt items don't have durability?
		InitDiabloMsg(EMSG_SHRINE_RELIGIOUS);
		break;
	case Shrine::ENCHANTED:
		if (deltaload)
			return;
		if (pnum != myplr())
			return;
		cnt = 0;
		spell = 1;
		spells = plr[pnum].data._pMemorySpells;
		for (j = 0; j < MAX_SPELLS; j++) {
			if (spell & spells)
				cnt++;
			spell <<= 1;
		}
		if (cnt > 1) {
			spell = 1;
			for (j = 1; j <= MAX_SPELLS; j++) {
				if (plr[pnum].data._pMemorySpells & spell) {
					if (plr[pnum].data._pSplLvl[j] < 15)
						plr[pnum].data._pSplLvl[j]++;
				}
				spell <<= 1;
			}
			do {
				r = random_(0, 37);
			} while (!(plr[pnum].data._pMemorySpells & ((__int64)1 << r)));
			if (plr[pnum].data._pSplLvl[r] >= 2)
				plr[pnum].data._pSplLvl[r] -= 2;
			else
				plr[pnum].data._pSplLvl[r] = 0;
		}
		InitDiabloMsg(EMSG_SHRINE_ENCHANTED);
		break;
	case Shrine::THAUMATURGIC:
		for (j = 0; j < nobjects; j++) {
			v1 = objectactive[j];
			assert((DWORD)v1 < MAXOBJECTS);
			if ((object[v1]._otype == ObjectType::CHEST1
			        || object[v1]._otype == ObjectType::CHEST2
			        || object[v1]._otype == ObjectType::CHEST3)
			    && object[v1]._oSelFlag == 0) {
				object[v1]._oRndSeed = GetRndSeed();
				object[v1]._oAnimFrame -= 2;
				object[v1]._oSelFlag = 1;
			}
		}
		if (deltaload)
			return;
		if (pnum == myplr())
			InitDiabloMsg(EMSG_SHRINE_THAUMATURGIC);
		break;
	case Shrine::FASCINATING:
		if (deltaload)
			return;
		if (pnum != myplr())
			return;
		plr[pnum].data._pMemorySpells |= (__int64)1 << (SpellId::FIREBOLT - 1);
		if (plr[pnum].data._pSplLvl[SpellId::FIREBOLT] < 15)
			plr[pnum].data._pSplLvl[SpellId::FIREBOLT]++;
		if (plr[pnum].data._pSplLvl[SpellId::FIREBOLT] < 15)
			plr[pnum].data._pSplLvl[SpellId::FIREBOLT]++;
		t = plr[pnum].data._pMaxManaBase / 10;
		v1 = plr[pnum].data._pMana - plr[pnum].data._pManaBase;
		v2 = plr[pnum].data._pMaxMana - plr[pnum].data._pMaxManaBase;

		plr[pnum].data._pManaBase -= t;
		plr[pnum].data._pMana -= t;
		plr[pnum].data._pMaxMana -= t;
		plr[pnum].data._pMaxManaBase -= t;
		if (plr[pnum].data._pMana >> 6 <= 0) {
			plr[pnum].data._pManaBase = 0;
			plr[pnum].data._pMana = v1;
		}
		if (plr[pnum].data._pMaxMana >> 6 <= 0) {
			plr[pnum].data._pMaxManaBase = 0;
			plr[pnum].data._pMaxMana = v2;
		}
		InitDiabloMsg(EMSG_SHRINE_FASCINATING);
		break;
	case Shrine::CRYPTIC:
		if (deltaload)
			return;
		AddMissile(
		    plr[pnum].pos(),
		    plr[pnum].pos(),
		    plr[pnum].data._pdir,
		    MissileType::NOVA,
		    -1,
		    pnum,
		    0,
		    2 * int(lvl.type()));
		if (pnum != myplr())
			return;
		plr[pnum].data._pMana = plr[pnum].data._pMaxMana;
		plr[pnum].data._pManaBase = plr[pnum].data._pMaxManaBase;
		InitDiabloMsg(EMSG_SHRINE_CRYPTIC);
		break;
	case Shrine::ELDRITCH: /// BUGFIX: change `plr[pnum].data.HoldItem` to use a temporary buffer to prevent deleting item in hand
		if (deltaload)
			return;
		if (pnum != myplr())
			break;
		for (j = 0; j < plr[pnum].data._pNumInv; j++) {
			if (plr[pnum].data.InvList[j]._itype == ItemType::misc) {
				if (plr[pnum].data.InvList[j]._iMiscId == MiscItemId::HEAL
				    || plr[pnum].data.InvList[j]._iMiscId == MiscItemId::MANA) {
					SetPlrHandItem(&plr[pnum].data.HoldItem, ItemMiscIdIdx(MiscItemId::REJUV));
					GetPlrHandSeed(&plr[pnum].data.HoldItem);
					plr[pnum].data.HoldItem._iStatFlag = true;
					plr[pnum].data.InvList[j] = plr[pnum].data.HoldItem;
				}
				if (plr[pnum].data.InvList[j]._iMiscId == MiscItemId::FULLHEAL
				    || plr[pnum].data.InvList[j]._iMiscId == MiscItemId::FULLMANA) {
					SetPlrHandItem(&plr[pnum].data.HoldItem, ItemMiscIdIdx(MiscItemId::FULLREJUV));
					GetPlrHandSeed(&plr[pnum].data.HoldItem);
					plr[pnum].data.HoldItem._iStatFlag = true;
					plr[pnum].data.InvList[j] = plr[pnum].data.HoldItem;
				}
			}
		}
		for (j = 0; j < MAXBELTITEMS; j++) {
			if (plr[pnum].data.SpdList[j]._itype == ItemType::misc) {
				if (plr[pnum].data.SpdList[j]._iMiscId == MiscItemId::HEAL
				    || plr[pnum].data.SpdList[j]._iMiscId == MiscItemId::MANA) {
					SetPlrHandItem(&plr[pnum].data.HoldItem, ItemMiscIdIdx(MiscItemId::REJUV));
					GetPlrHandSeed(&plr[pnum].data.HoldItem);
					plr[pnum].data.HoldItem._iStatFlag = true;
					plr[pnum].data.SpdList[j] = plr[pnum].data.HoldItem;
				}
				if (plr[pnum].data.SpdList[j]._iMiscId == MiscItemId::FULLHEAL
				    || plr[pnum].data.SpdList[j]._iMiscId == MiscItemId::FULLMANA) {
					SetPlrHandItem(&plr[pnum].data.HoldItem, ItemMiscIdIdx(MiscItemId::FULLREJUV));
					GetPlrHandSeed(&plr[pnum].data.HoldItem);
					plr[pnum].data.HoldItem._iStatFlag = true;
					plr[pnum].data.SpdList[j] = plr[pnum].data.HoldItem;
				}
			}
		}
		InitDiabloMsg(EMSG_SHRINE_ELDRITCH);
		break;
	case Shrine::EERIE:
		if (deltaload)
			return;
		if (pnum != myplr())
			return;
		plr[pnum].ModifyPlrMag(2);
		plr[pnum].CheckStats();
		InitDiabloMsg(EMSG_SHRINE_EERIE);
		break;
	case Shrine::DIVINE:
		if (deltaload)
			return;
		if (pnum != myplr())
			return;
		if (2 * lvl.currlevel < 7) {
			CreateTypeItem(object[i]._o, false, ItemType::misc, MiscItemId::FULLMANA, false, true);
			CreateTypeItem(object[i]._o, false, ItemType::misc, MiscItemId::FULLHEAL, false, true);
		} else {
			CreateTypeItem(object[i]._o, false, ItemType::misc, MiscItemId::FULLREJUV, false, true);
			CreateTypeItem(object[i]._o, false, ItemType::misc, MiscItemId::FULLREJUV, false, true);
		}
		plr[pnum].data._pMana = plr[pnum].data._pMaxMana;
		plr[pnum].data._pManaBase = plr[pnum].data._pMaxManaBase;
		plr[pnum].data._pHitPoints = plr[pnum].data._pMaxHP;
		plr[pnum].data._pHPBase = plr[pnum].data._pMaxHPBase;
		InitDiabloMsg(EMSG_SHRINE_DIVINE);
		break;
	case Shrine::HOLY:
		if (deltaload)
			return;
		j = 0;
		do {
			p.x = random_(159, MAXDUNX);
			p.y = random_(159, MAXDUNY);
			j++;
			if (j > MAXDUNX * 112)
				break;
			lv = grid.at(p).getPiece();
		} while (pieces[lv].solid || grid.at(p).isObject() || grid.at(p).getActor());
		AddMissile(plr[pnum].pos(), p, plr[pnum].data._pdir, MissileType::RNDTELEPORT, -1, pnum, 0, 2 * int(lvl.type()));
		if (pnum != myplr())
			return;
		InitDiabloMsg(EMSG_SHRINE_HOLY);
		break;
	case Shrine::SACRED:
		if (deltaload || pnum != myplr())
			return;
		plr[pnum].data._pMemorySpells |= (__int64)1 << (SpellId::CBOLT - 1);
		if (plr[pnum].data._pSplLvl[SpellId::CBOLT] < 15)
			plr[pnum].data._pSplLvl[SpellId::CBOLT]++;
		if (plr[pnum].data._pSplLvl[SpellId::CBOLT] < 15)
			plr[pnum].data._pSplLvl[SpellId::CBOLT]++;
		t = plr[pnum].data._pMaxManaBase / 10;
		v1 = plr[pnum].data._pMana - plr[pnum].data._pManaBase;
		v2 = plr[pnum].data._pMaxMana - plr[pnum].data._pMaxManaBase;
		plr[pnum].data._pManaBase -= t;
		plr[pnum].data._pMana -= t;
		plr[pnum].data._pMaxMana -= t;
		plr[pnum].data._pMaxManaBase -= t;
		if (plr[pnum].data._pMana >> 6 <= 0) {
			plr[pnum].data._pMana = v1;
			plr[pnum].data._pManaBase = 0;
		}
		if (plr[pnum].data._pMaxMana >> 6 <= 0) {
			plr[pnum].data._pMaxMana = v2;
			plr[pnum].data._pMaxManaBase = 0;
		}
		InitDiabloMsg(EMSG_SHRINE_SACRED);
		break;
	case Shrine::SPIRITUAL:
		if (deltaload)
			return;
		if (pnum != myplr())
			return;
		for (j = 0; j < MAXINVITEMS; j++) {
			if (!plr[pnum].data.InvGrid[j]) {
				r = 5 * int(lvl.type()) + random_(160, 10 * int(lvl.type()));
				t = plr[pnum].data._pNumInv; // check
				plr[pnum].data.InvList[t] = golditem;
				plr[pnum].data.InvList[t]._iSeed = GetRndSeed();
				plr[pnum].data._pNumInv++;
				plr[pnum].data.InvGrid[j] = plr[pnum].data._pNumInv;
				plr[pnum].data.InvList[t]._ivalue = r;
				plr[pnum].data._pGold += r;
				SetGoldCurs(pnum, t);
			}
		}
		InitDiabloMsg(EMSG_SHRINE_SPIRITUAL);
		break;
	case Shrine::SPOOKY:
		if (deltaload)
			return;
		if (pnum == myplr()) {
			InitDiabloMsg(EMSG_SHRINE_SPOOKY1);
		} else {
			InitDiabloMsg(EMSG_SHRINE_SPOOKY2);
			myplr().data._pHitPoints = myplr().data._pMaxHP;
			myplr().data._pHPBase = myplr().data._pMaxHPBase;
			myplr().data._pMana = myplr().data._pMaxMana;
			myplr().data._pManaBase = myplr().data._pMaxManaBase;
		}
		break;
	case Shrine::ABANDONED:
		if (deltaload)
			return;
		if (pnum != myplr())
			return;
		plr[pnum].ModifyPlrDex(2);
		plr[pnum].CheckStats();
		if (pnum == myplr())
			InitDiabloMsg(EMSG_SHRINE_ABANDONED);
		break;
	case Shrine::CREEPY:
		if (deltaload)
			return;
		if (pnum != myplr())
			return;
		plr[pnum].ModifyPlrStr(2);
		plr[pnum].CheckStats();
		if (pnum == myplr())
			InitDiabloMsg(EMSG_SHRINE_CREEPY);
		break;
	case Shrine::QUIET:
		if (deltaload)
			return;
		if (pnum != myplr())
			return;
		plr[pnum].ModifyPlrVit(2);
		plr[pnum].CheckStats();
		if (pnum == myplr())
			InitDiabloMsg(EMSG_SHRINE_QUIET);
		break;
	case Shrine::SECLUDED:
		if (deltaload)
			return;
		if (pnum != myplr())
			break;

		for (p.y = 0; p.y < DMAXY; p.y++) {
			for (p.x = 0; p.x < DMAXX; p.x++) {
				automap.getView()[p.x][p.y] = true;
			}
		}
		InitDiabloMsg(EMSG_SHRINE_SECLUDED);
		break;
	case Shrine::ORNATE:
		if (deltaload)
			return;
		if (pnum != myplr())
			return;
		plr[pnum].data._pMemorySpells |= (__int64)1 << (SpellId::HBOLT - 1);
		if (plr[pnum].data._pSplLvl[SpellId::HBOLT] < 15)
			plr[pnum].data._pSplLvl[SpellId::HBOLT]++;
		if (plr[pnum].data._pSplLvl[SpellId::HBOLT] < 15)
			plr[pnum].data._pSplLvl[SpellId::HBOLT]++;
		t = plr[pnum].data._pMaxManaBase / 10;
		v1 = plr[pnum].data._pMana - plr[pnum].data._pManaBase;
		v2 = plr[pnum].data._pMaxMana - plr[pnum].data._pMaxManaBase;
		plr[pnum].data._pManaBase -= t;
		plr[pnum].data._pMana -= t;
		plr[pnum].data._pMaxMana -= t;
		plr[pnum].data._pMaxManaBase -= t;
		if (plr[pnum].data._pMana >> 6 <= 0) {
			plr[pnum].data._pMana = v1;
			plr[pnum].data._pManaBase = 0;
		}
		if (plr[pnum].data._pMaxMana >> 6 <= 0) {
			plr[pnum].data._pMaxMana = v2;
			plr[pnum].data._pMaxManaBase = 0;
		}
		InitDiabloMsg(EMSG_SHRINE_ORNATE);
		break;
	case Shrine::GLIMMERING:
		if (deltaload)
			return;
		if (pnum != myplr())
			return;
		for (j = 0; j < MAXINVITEMS; j++) {
			if (plr[pnum].data.InvBody[j]._iMagical && !plr[pnum].data.InvBody[j]._iIdentified)
				plr[pnum].data.InvBody[j]._iIdentified = true;
		}
		for (j = 0; j < plr[pnum].data._pNumInv; j++) {
			if (plr[pnum].data.InvList[j]._iMagical && !plr[pnum].data.InvList[j]._iIdentified)
				plr[pnum].data.InvList[j]._iIdentified = true;
		}
		for (j = 0; j < 8; j++) {
			if (plr[pnum].data.SpdList[j]._iMagical && !plr[pnum].data.SpdList[j]._iIdentified)
				plr[pnum].data.SpdList[j]._iIdentified = true; // belt items can't be magical?
		}
		InitDiabloMsg(EMSG_SHRINE_GLIMMERING);
		break;
	case Shrine::TAINTED:
		if (deltaload)
			return;
		if (pnum == myplr()) {
			InitDiabloMsg(EMSG_SHRINE_TAINTED1);
		} else {
			InitDiabloMsg(EMSG_SHRINE_TAINTED2);
			r = random_(155, 4);

			if (r != 0)
				v1 = -1;
			else
				v1 = 1;
			if (r != 1)
				v2 = -1;
			else
				v2 = 1;
			if (r == 2)
				v3 = 1;
			else
				v3 = -1;
			if (r == 3)
				v4 = 1;
			else
				v4 = -1;

			plr[pnum].ModifyPlrStr(v1);
			plr[pnum].ModifyPlrMag(v2);
			plr[pnum].ModifyPlrDex(v3);
			plr[pnum].ModifyPlrVit(v4);
			plr[pnum].CheckStats();
		}
		break;
	}

	CalcPlrInv(pnum, true);
	force_redraw = 255;

	if (pnum == myplr())
		NetSendCmdParam2(false, Cmd::PLROPOBJ, pnum, i);
}

void OperateSkelBook(int pnum, int i, bool sendmsg)
{
	if (object[i]._oSelFlag != 0) {
		if (!deltaload)
			PlaySfxLoc(IS_ISCROL, object[i]._o);
		object[i]._oSelFlag = 0;
		object[i]._oAnimFrame += 2;
		if (!deltaload) {
			SetRndSeed(object[i]._oRndSeed);
			if (random_(161, 5))
				CreateTypeItem(object[i]._o, false, ItemType::misc, MiscItemId::SCROLL, sendmsg, false);
			else
				CreateTypeItem(object[i]._o, false, ItemType::misc, MiscItemId::BOOK, sendmsg, false);
			if (pnum == myplr())
				NetSendCmdParam1(false, Cmd::OPERATEOBJ, i);
		}
	}
}

void OperateBookCase(int pnum, int i, bool sendmsg)
{
	if (object[i]._oSelFlag != 0) {
		if (!deltaload)
			PlaySfxLoc(IS_ISCROL, object[i]._o);
		object[i]._oSelFlag = 0;
		object[i]._oAnimFrame -= 2;
		if (!deltaload) {
			SetRndSeed(object[i]._oRndSeed);
			CreateTypeItem(object[i]._o, false, ItemType::misc, MiscItemId::BOOK, sendmsg, false);
			if (QuestStatus(QuestId::zhar)
			    && monsters[MAX_PLRS].data.mName == UniqMonst[UniqueMonsterType::ZHAR].mName
			    && monsters[MAX_PLRS].data._msquelch == UINT8_MAX
			    && monsters[MAX_PLRS].data._mhitpoints) {
				monsters[MAX_PLRS].data.mtalkmsg = TEXT_ZHAR2;
				monsters[0].M_StartStand(monsters[MAX_PLRS].data._mdir);
				monsters[MAX_PLRS].data._mgoal = MonsterGoal::SHOOT;
				monsters[MAX_PLRS].data._mmode = MonsterMode::TALK;
			}
			if (pnum == myplr())
				NetSendCmdParam1(false, Cmd::OPERATEOBJ, i);
		}
	}
}

void OperateDecap(int pnum, int i, bool sendmsg)
{
	if (object[i]._oSelFlag != 0) {
		object[i]._oSelFlag = 0;
		if (!deltaload) {
			SetRndSeed(object[i]._oRndSeed);
			CreateRndItem(object[i]._o, false, sendmsg, false);
			if (pnum == myplr())
				NetSendCmdParam1(false, Cmd::OPERATEOBJ, i);
		}
	}
}

void OperateArmorStand(int pnum, int i, bool sendmsg)
{
	bool uniqueRnd;

	if (object[i]._oSelFlag != 0) {
		object[i]._oSelFlag = 0;
		object[i]._oAnimFrame++;
		if (!deltaload) {
			SetRndSeed(object[i]._oRndSeed);
			uniqueRnd = random_(0, 2);
			if (lvl.currlevel <= 5) {
				CreateTypeItem(object[i]._o, true, ItemType::light_armor, MiscItemId::NONE, sendmsg, false);
			} else if (lvl.currlevel >= 6 && lvl.currlevel <= 9) {
				CreateTypeItem(object[i]._o, uniqueRnd, ItemType::medium_armor, MiscItemId::NONE, sendmsg, false);
			} else if (lvl.currlevel >= 10 && lvl.currlevel <= 12) {
				CreateTypeItem(object[i]._o, false, ItemType::heavy_armor, MiscItemId::NONE, sendmsg, false);
			} else if (lvl.currlevel >= 13 && lvl.currlevel <= 16) {
				CreateTypeItem(object[i]._o, true, ItemType::heavy_armor, MiscItemId::NONE, sendmsg, false);
			}
			if (pnum == myplr())
				NetSendCmdParam1(false, Cmd::OPERATEOBJ, i);
			return;
		}
	}
}

int FindValidShrine(int i)
{
	bool done;
	int rv;

	while (1) {
		done = false;
		while (!done) {
			rv = random_(0, NUM_SHRINETYPE);
			if (lvl.currlevel >= shrinemin[rv] && lvl.currlevel <= shrinemax[rv] && rv != 8)
				done = true;
		}

		if (game.isMultiplayer()) {
			if (shrineavail[rv] != 1) {
				break;
			}
		} else {
			if (shrineavail[rv] != 2) {
				break;
			}
		}
	}

	return rv;
}

void OperateGoatShrine(int pnum, int i, int sType)
{
	SetRndSeed(object[i]._oRndSeed);
	object[i]._oVar1 = FindValidShrine(i);
	OperateShrine(pnum, i, sType);
	object[i]._oAnimDelay = 2;
	force_redraw = 255;
}

void OperateCauldron(int pnum, int i, int sType)
{
	SetRndSeed(object[i]._oRndSeed);
	object[i]._oVar1 = FindValidShrine(i);
	OperateShrine(pnum, i, sType);
	object[i]._oAnimFrame = 3;
	object[i]._oAnimFlag = 0;
	force_redraw = 255;
}

bool OperateFountains(int pnum, int i)
{
	int prev, add, rnd, cnt;
	bool applied, done;

	applied = false;
	SetRndSeed(object[i]._oRndSeed);
	switch (object[i]._otype) {
	case ObjectType::BLOODFTN:
		if (deltaload)
			return false;
		if (pnum != myplr())
			return false;

		if (plr[pnum].data._pHitPoints < plr[pnum].data._pMaxHP) {
			PlaySfxLoc(LS_FOUNTAIN, object[i]._o);
			plr[pnum].data._pHitPoints += 64;
			plr[pnum].data._pHPBase += 64;
			if (plr[pnum].data._pHitPoints > plr[pnum].data._pMaxHP) {
				plr[pnum].data._pHitPoints = plr[pnum].data._pMaxHP;
				plr[pnum].data._pHPBase = plr[pnum].data._pMaxHPBase;
			}
			applied = true;
		} else
			PlaySfxLoc(LS_FOUNTAIN, object[i]._o);
		break;
	case ObjectType::PURIFYINGFTN:
		if (deltaload)
			return false;
		if (pnum != myplr())
			return false;

		if (plr[pnum].data._pMana < plr[pnum].data._pMaxMana) {
			PlaySfxLoc(LS_FOUNTAIN, object[i]._o);

			plr[pnum].data._pMana += 64;
			plr[pnum].data._pManaBase += 64;
			if (plr[pnum].data._pMana > plr[pnum].data._pMaxMana) {
				plr[pnum].data._pMana = plr[pnum].data._pMaxMana;
				plr[pnum].data._pManaBase = plr[pnum].data._pMaxManaBase;
			}

			applied = true;
		} else
			PlaySfxLoc(LS_FOUNTAIN, object[i]._o);
		break;
	case ObjectType::MURKYFTN:
		if (object[i]._oSelFlag == 0)
			break;
		if (!deltaload)
			PlaySfxLoc(LS_FOUNTAIN, object[i]._o);
		object[i]._oSelFlag = 0;
		if (deltaload)
			return false;
		AddMissile(
		    plr[pnum].pos(),
		    plr[pnum].pos(),
		    plr[pnum].data._pdir,
		    MissileType::INFRA,
		    -1,
		    pnum,
		    0,
		    2 * int(lvl.type()));
		applied = true;
		if (pnum == myplr())
			NetSendCmdParam1(false, Cmd::OPERATEOBJ, i);
		break;
	case ObjectType::TEARFTN:
		if (object[i]._oSelFlag == 0)
			break;
		prev = -1;
		add = -1;
		done = false;
		cnt = 0;
		if (!deltaload)
			PlaySfxLoc(LS_FOUNTAIN, object[i]._o);
		object[i]._oSelFlag = 0;
		if (deltaload)
			return false;
		if (pnum != myplr())
			return false;
		while (!done) {
			rnd = random_(0, 4);
			if (rnd != prev) {
				switch (rnd) {
				case 0:
					plr[pnum].ModifyPlrStr(add);
					break;
				case 1:
					plr[pnum].ModifyPlrMag(add);
					break;
				case 2:
					plr[pnum].ModifyPlrDex(add);
					break;
				case 3:
					plr[pnum].ModifyPlrVit(add);
					break;
				}
				prev = rnd;
				add = 1;
				cnt++;
			}
			if (cnt <= 1)
				continue;

			done = true;
		}
		plr[pnum].CheckStats();
		applied = true;
		if (pnum == myplr())
			NetSendCmdParam1(false, Cmd::OPERATEOBJ, i);
		break;
	}
	force_redraw = 255;
	return applied;
}

void OperateWeaponRack(int pnum, int i, bool sendmsg)
{
	int weaponType;

	if (object[i]._oSelFlag == 0)
		return;
	SetRndSeed(object[i]._oRndSeed);

	switch (random_(0, 4) + 1) {
	case ItemType::sword:
		weaponType = ItemType::sword;
		break;
	case ItemType::axe:
		weaponType = ItemType::axe;
		break;
	case ItemType::bow:
		weaponType = ItemType::bow;
		break;
	case ItemType::mace:
		weaponType = ItemType::mace;
		break;
	}

	object[i]._oSelFlag = 0;
	object[i]._oAnimFrame++;
	if (deltaload)
		return;

	if (int(lvl.type()) > 1)
		CreateTypeItem(object[i]._o, true, weaponType, MiscItemId::NONE, sendmsg, false);
	else
		CreateTypeItem(object[i]._o, false, weaponType, MiscItemId::NONE, sendmsg, false);
	if (pnum == myplr())
		NetSendCmdParam1(false, Cmd::OPERATEOBJ, i);
}

void OperateStoryBook(int pnum, int i)
{
	if (object[i]._oSelFlag != 0 && !deltaload && !qtextflag && pnum == myplr()) {
		object[i]._oAnimFrame = object[i]._oVar4;
		PlaySfxLoc(IS_ISCROL, object[i]._o);
		InitQTextMsg(object[i]._oVar2);
		NetSendCmdParam1(false, Cmd::OPERATEOBJ, i);
	}
}

void OperateLazStand(int pnum, int i)
{
	V2Di p;
	if (object[i]._oSelFlag != 0 && !deltaload && !qtextflag && pnum == myplr()) {
		object[i]._oAnimFrame++;
		object[i]._oSelFlag = 0;
		GetSuperItemLoc(object[i]._o, p);
		SpawnQuestItem(ItemIndex::LAZSTAFF, p, 0, 0);
	}
}

void OperateObject(int pnum, int i, bool TeleFlag)
{
	bool sendmsg;
	sendmsg = (pnum == myplr());
	switch (object[i]._otype) {
	case ObjectType::L1LDOOR:
	case ObjectType::L1RDOOR:
		if (TeleFlag) {
			if (object[i]._otype == ObjectType::L1LDOOR)
				OperateL1LDoor(pnum, i, true);
			if (object[i]._otype == ObjectType::L1RDOOR)
				OperateL1RDoor(pnum, i, true);
			break;
		}
		if (pnum == myplr())
			OperateL1Door(pnum, i, true);
		break;
	case ObjectType::L2LDOOR:
	case ObjectType::L2RDOOR:
		if (TeleFlag) {
			if (object[i]._otype == ObjectType::L2LDOOR)
				OperateL2LDoor(pnum, i, true);
			if (object[i]._otype == ObjectType::L2RDOOR)
				OperateL2RDoor(pnum, i, true);
			break;
		}
		if (pnum == myplr())
			OperateL2Door(pnum, i, true);
		break;
	case ObjectType::L3LDOOR:
	case ObjectType::L3RDOOR:
		if (TeleFlag) {
			if (object[i]._otype == ObjectType::L3LDOOR)
				OperateL3LDoor(pnum, i, true);
			if (object[i]._otype == ObjectType::L3RDOOR)
				OperateL3RDoor(pnum, i, true);
			break;
		}
		if (pnum == myplr())
			OperateL3Door(pnum, i, true);
		break;
	case ObjectType::LEVER:
	case ObjectType::SWITCHSKL:
		OperateLever(pnum, i);
		break;
	case ObjectType::BOOK2L:
		OperateBook(pnum, i);
		break;
	case ObjectType::BOOK2R:
		OperateSChambBk(pnum, i);
		break;
	case ObjectType::CHEST1:
	case ObjectType::CHEST2:
	case ObjectType::CHEST3:
	case ObjectType::TCHEST1:
	case ObjectType::TCHEST2:
	case ObjectType::TCHEST3:
		OperateChest(pnum, i, sendmsg);
		break;
	case ObjectType::SARC:
		OperateSarc(pnum, i, sendmsg);
		break;
	case ObjectType::FLAMELVR:
		OperateTrapLvr(i);
		break;
	case ObjectType::BLINDBOOK:
	case ObjectType::BLOODBOOK:
	case ObjectType::STEELTOME:
		OperateBookLever(pnum, i);
		break;
	case ObjectType::SHRINEL:
	case ObjectType::SHRINER:
		OperateShrine(pnum, i, IS_MAGIC);
		break;
	case ObjectType::SKELBOOK:
	case ObjectType::BOOKSTAND:
		OperateSkelBook(pnum, i, sendmsg);
		break;
	case ObjectType::BOOKCASEL:
	case ObjectType::BOOKCASER:
		OperateBookCase(pnum, i, sendmsg);
		break;
	case ObjectType::DECAP:
		OperateDecap(pnum, i, sendmsg);
		break;
	case ObjectType::ARMORSTAND:
	case ObjectType::WARARMOR:
		OperateArmorStand(pnum, i, sendmsg);
		break;
	case ObjectType::GOATSHRINE:
		OperateGoatShrine(pnum, i, LS_GSHRINE);
		break;
	case ObjectType::CAULDRON:
		OperateCauldron(pnum, i, LS_CALDRON);
		break;
	case ObjectType::BLOODFTN:
	case ObjectType::PURIFYINGFTN:
	case ObjectType::MURKYFTN:
	case ObjectType::TEARFTN:
		OperateFountains(pnum, i);
		break;
	case ObjectType::STORYBOOK:
		OperateStoryBook(pnum, i);
		break;
	case ObjectType::PEDISTAL:
		OperatePedistal(pnum, i);
		break;
	case ObjectType::WARWEAP:
	case ObjectType::WEAPONRACK:
		OperateWeaponRack(pnum, i, sendmsg);
		break;
	case ObjectType::MUSHPATCH:
		OperateMushPatch(pnum, i);
		break;
	case ObjectType::LAZSTAND:
		OperateLazStand(pnum, i);
		break;
	case ObjectType::SLAINHERO:
		OperateSlainHero(pnum, i, sendmsg);
		break;
	case ObjectType::SIGNCHEST:
		OperateInnSignChest(pnum, i);
		break;
	}
}

void SyncOpL1Door(int pnum, int cmd, int i)
{
	bool do_sync;

	if (pnum == myplr())
		return;

	do_sync = false;
	if (cmd == Cmd::OPENDOOR && object[i]._oVar4 == 0) {
		do_sync = true;
	}
	if (cmd == Cmd::CLOSEDOOR && object[i]._oVar4 == 1)
		do_sync = true;
	if (do_sync) {
		if (object[i]._otype == ObjectType::L1LDOOR)
			OperateL1LDoor(-1, i, false);
		if (object[i]._otype == ObjectType::L1RDOOR)
			OperateL1RDoor(-1, i, false);
	}
}

void SyncOpL2Door(int pnum, int cmd, int i)
{
	bool do_sync;

	if (pnum == myplr())
		return;

	do_sync = false;
	if (cmd == Cmd::OPENDOOR && object[i]._oVar4 == 0) {
		do_sync = true;
	}
	if (cmd == Cmd::CLOSEDOOR && object[i]._oVar4 == 1)
		do_sync = true;
	if (do_sync) {
		if (object[i]._otype == ObjectType::L2LDOOR)
			OperateL2LDoor(-1, i, false);
		if (object[i]._otype == ObjectType::L2RDOOR)
			OperateL2RDoor(-1, i, false);
	}
}

void SyncOpL3Door(int pnum, int cmd, int i)
{
	bool do_sync;

	if (pnum == myplr())
		return;

	do_sync = false;
	if (cmd == Cmd::OPENDOOR && object[i]._oVar4 == 0) {
		do_sync = true;
	}
	if (cmd == Cmd::CLOSEDOOR && object[i]._oVar4 == 1)
		do_sync = true;
	if (do_sync) {
		if (object[i]._otype == ObjectType::L3LDOOR)
			OperateL2LDoor(-1, i, false);
		if (object[i]._otype == ObjectType::L3RDOOR)
			OperateL2RDoor(-1, i, false);
	}
}

void SyncOpObject(int pnum, int cmd, int i)
{
	switch (object[i]._otype) {
	case ObjectType::L1LDOOR:
	case ObjectType::L1RDOOR:
		SyncOpL1Door(pnum, cmd, i);
		break;
	case ObjectType::L2LDOOR:
	case ObjectType::L2RDOOR:
		SyncOpL2Door(pnum, cmd, i);
		break;
	case ObjectType::L3LDOOR:
	case ObjectType::L3RDOOR:
		SyncOpL3Door(pnum, cmd, i);
		break;
	case ObjectType::LEVER:
	case ObjectType::SWITCHSKL:
		OperateLever(pnum, i);
		break;
	case ObjectType::CHEST1:
	case ObjectType::CHEST2:
	case ObjectType::CHEST3:
	case ObjectType::TCHEST1:
	case ObjectType::TCHEST2:
	case ObjectType::TCHEST3:
		OperateChest(pnum, i, false);
		break;
	case ObjectType::SARC:
		OperateSarc(pnum, i, false);
		break;
	case ObjectType::BLINDBOOK:
	case ObjectType::BLOODBOOK:
	case ObjectType::STEELTOME:
		OperateBookLever(pnum, i);
		break;
	case ObjectType::SHRINEL:
	case ObjectType::SHRINER:
		OperateShrine(pnum, i, IS_MAGIC);
		break;
	case ObjectType::SKELBOOK:
	case ObjectType::BOOKSTAND:
		OperateSkelBook(pnum, i, false);
		break;
	case ObjectType::BOOKCASEL:
	case ObjectType::BOOKCASER:
		OperateBookCase(pnum, i, false);
		break;
	case ObjectType::DECAP:
		OperateDecap(pnum, i, false);
		break;
	case ObjectType::ARMORSTAND:
	case ObjectType::WARARMOR:
		OperateArmorStand(pnum, i, false);
		break;
	case ObjectType::GOATSHRINE:
		OperateGoatShrine(pnum, i, LS_GSHRINE);
		break;
	case ObjectType::CAULDRON:
		OperateCauldron(pnum, i, LS_CALDRON);
		break;
	case ObjectType::MURKYFTN:
	case ObjectType::TEARFTN:
		OperateFountains(pnum, i);
		break;
	case ObjectType::STORYBOOK:
		OperateStoryBook(pnum, i);
		break;
	case ObjectType::PEDISTAL:
		OperatePedistal(pnum, i);
		break;
	case ObjectType::WARWEAP:
	case ObjectType::WEAPONRACK:
		OperateWeaponRack(pnum, i, false);
		break;
	case ObjectType::MUSHPATCH:
		OperateMushPatch(pnum, i);
		break;
	case ObjectType::SLAINHERO:
		OperateSlainHero(pnum, i, false);
		break;
	case ObjectType::SIGNCHEST:
		OperateInnSignChest(pnum, i);
		break;
	}
}

void BreakCrux(int i)
{
	int j, oi;
	bool triggered;

	object[i]._oAnimFlag = 1;
	object[i]._oAnimFrame = 1;
	object[i]._oAnimDelay = 1;
	object[i]._oSolidFlag = true;
	object[i]._oMissFlag = true;
	object[i]._oBreak = -1;
	object[i]._oSelFlag = 0;
	triggered = true;
	for (j = 0; j < nobjects; j++) {
		oi = objectactive[j];
		if (object[oi]._otype != ObjectType::CRUX1 && object[oi]._otype != ObjectType::CRUX2 && object[oi]._otype != ObjectType::CRUX3)
			continue;
		if (object[i]._oVar8 != object[oi]._oVar8 || object[oi]._oBreak == -1)
			continue;
		triggered = false;
	}
	if (!triggered)
		return;
	if (!deltaload)
		PlaySfxLoc(IS_LEVER, object[i]._o);
	ObjChangeMap({ object[i]._oVar1, object[i]._oVar2 }, { object[i]._oVar3, object[i]._oVar4 });
}

void BreakBarrel(int pnum, int i, int dam, bool forcebreak, bool sendmsg)
{
	int oi;
	int xp, yp;

	if (object[i]._oSelFlag == 0)
		return;
	if (forcebreak) {
		object[i]._oVar1 = 0;
	} else {
		object[i]._oVar1 -= dam;
		if (pnum != myplr() && object[i]._oVar1 <= 0)
			object[i]._oVar1 = 1;
	}
	if (object[i]._oVar1 > 0) {
		if (deltaload)
			return;

		PlaySfxLoc(IS_IBOW, object[i]._o);
		return;
	}

	object[i]._oBreak = -1;
	object[i]._oVar1 = 0;
	object[i]._oAnimFlag = 1;
	object[i]._oAnimFrame = 1;
	object[i]._oAnimDelay = 1;
	object[i]._oSolidFlag = false;
	object[i]._oMissFlag = true;
	object[i]._oSelFlag = 0;
	object[i]._oPreFlag = true;
	if (deltaload) {
		object[i]._oAnimCnt = 0;
		object[i]._oAnimFrame = object[i]._oAnimLen;
		object[i]._oAnimDelay = 1000;
		return;
	}

	if (object[i]._otype == ObjectType::BARRELEX) {
		PlaySfxLoc(IS_BARLFIRE, object[i]._o);
		for (yp = object[i]._o.y - 1; yp <= object[i]._o.y + 1; yp++) {
			for (xp = object[i]._o.x - 1; xp <= object[i]._o.x + 1; xp++) {
				if (grid[xp][yp].isActor())
					MonsterTrapHit(grid[xp][yp].getActor() - 1, 1, 4, 0, 1, false);
				if (grid[xp][yp].isPlayer())
					PlayerMHit(grid[xp][yp].getPlayer(), -1, 0, 8, 16, 1, false, 0);
				if (grid[xp][yp].isObject()) {
					oi = grid[xp][yp].getObject();
					if (object[oi]._otype == ObjectType::BARRELEX && object[oi]._oBreak != -1)
						BreakBarrel(pnum, oi, dam, true, sendmsg);
				}
			}
		}
	} else {
		PlaySfxLoc(IS_BARREL, object[i]._o);
		SetRndSeed(object[i]._oRndSeed);
		if (object[i]._oVar2 <= 1) {
			if (!object[i]._oVar3)
				CreateRndUseful(pnum, object[i]._o, sendmsg);
			else
				CreateRndItem(object[i]._o, false, sendmsg, false);
		}
		if (object[i]._oVar2 >= 8)
			SpawnSkeleton(object[i]._oVar4, object[i]._o);
	}
	if (pnum == myplr())
		NetSendCmdParam2(false, Cmd::BREAKOBJ, pnum, i);
}

void BreakObject(int pnum, int oi)
{
	int objdam, mind, maxd;

	if (pnum != -1) {
		mind = plr[pnum].data._pIMinDam;
		maxd = random_(163, plr[pnum].data._pIMaxDam - mind + 1);
		objdam = maxd + mind;
		objdam += plr[pnum].data._pDamageMod + plr[pnum].data._pIBonusDamMod + objdam * plr[pnum].data._pIBonusDam / 100;
	} else {
		objdam = 10;
	}
	switch (object[oi]._otype) {
	case ObjectType::CRUX1:
	case ObjectType::CRUX2:
	case ObjectType::CRUX3:
		BreakCrux(oi);
		break;
	case ObjectType::BARREL:
	case ObjectType::BARRELEX:
		BreakBarrel(pnum, oi, objdam, false, true);
		break;
	}
}

void SyncBreakObj(int pnum, int oi)
{
	if (object[oi]._otype >= ObjectType::BARREL && object[oi]._otype <= ObjectType::BARRELEX)
		BreakBarrel(pnum, oi, 0, true, false);
}

void SyncL1Doors(int i)
{
	if (object[i]._oVar4 == 0) {
		object[i]._oMissFlag = false;
		return;
	}
	V2Di p = object[i]._o;
	object[i]._oMissFlag = true;
	object[i]._oSelFlag = 2;
	if (object[i]._otype == ObjectType::L1LDOOR) {
		if (object[i]._oVar1 == 214)
			ObjSetMicro(p, 408);
		else
			ObjSetMicro(p, 393);
		grid.at(p).dSpecial = 7;
		objects_set_door_piece({ p.x - 1, p.y });
		p.y--;
	} else {
		ObjSetMicro({ p.x, p.y }, 395);
		grid.at(p).dSpecial = 8;
		objects_set_door_piece({ p.x, p.y - 1 });
		p.x--;
	}
	DoorSet(i, p);
}

void SyncCrux(int i)
{
	int j, oi, type;

	bool found = true;
	for (j = 0; j < nobjects; j++) {
		oi = objectactive[j];
		type = object[oi]._otype;
		if (type != ObjectType::CRUX1 && type != ObjectType::CRUX2 && type != ObjectType::CRUX3)
			continue;
		if (object[i]._oVar8 != object[oi]._oVar8 || object[oi]._oBreak == -1)
			continue;
		found = false;
	}
	if (found)
		ObjChangeMap({ object[i]._oVar1, object[i]._oVar2 }, { object[i]._oVar3, object[i]._oVar4 });
}

void SyncLever(int i)
{
	if (object[i]._oSelFlag == 0)
		ObjChangeMap({ object[i]._oVar1, object[i]._oVar2 }, { object[i]._oVar3, object[i]._oVar4 });
}

void SyncQSTLever(int i)
{
	int tren;

	if (object[i]._oAnimFrame == object[i]._oVar6) {
		ObjChangeMapResync({ object[i]._oVar1, object[i]._oVar2 }, { object[i]._oVar3, object[i]._oVar4 });
		if (object[i]._otype == ObjectType::BLINDBOOK) {
			tren = lvl.TransVal;
			lvl.TransVal = 9;
			DRLG_MRectTrans( object[i]._oVar1, object[i]._oVar2 , object[i]._oVar3, object[i]._oVar4 );
			lvl.TransVal = tren;
		}
	}
}

void SyncPedistal(int i)
{
	uint8_t *setp;

	if (object[i]._oVar6 == 1)
		ObjChangeMapResync({ lvl.getpc().x, lvl.getpc().y + 3 }, { lvl.getpc().x + 2, lvl.getpc().y + 7 });
	if (object[i]._oVar6 == 2) {
		ObjChangeMapResync({ lvl.getpc().x, lvl.getpc().y + 3 }, { lvl.getpc().x + 2, lvl.getpc().y + 7 });
		ObjChangeMapResync({ lvl.getpc().x + 6, lvl.getpc().y + 3 }, { lvl.getpc().x + lvl.getpc().w, lvl.getpc().y + 7 });
	}
	if (object[i]._oVar6 == 3) {
		ObjChangeMapResync({ object[i]._oVar1, object[i]._oVar2 }, { object[i]._oVar3, object[i]._oVar4 });
		setp = LoadFileInMem("Levels\\L2Data\\Blood2.DUN", NULL);
		LoadMapObjs(setp, { 2 * lvl.getpc().x, 2 * lvl.getpc().y });
		mem_free_dbg(setp);
	}
}

void SyncL2Doors(int i)
{
	if (object[i]._oVar4 == 0)
		object[i]._oMissFlag = false;
	else
		object[i]._oMissFlag = true;
	V2Di p = object[i]._o;
	object[i]._oSelFlag = 2;
	if (object[i]._otype == ObjectType::L2LDOOR && object[i]._oVar4 == 0) {
		ObjSetMicro(p, 538);
	} else if (object[i]._otype == ObjectType::L2LDOOR && (object[i]._oVar4 == 1 || object[i]._oVar4 == 2)) {
		ObjSetMicro(p, 13);
	} else if (object[i]._otype == ObjectType::L2RDOOR && object[i]._oVar4 == 0) {
		ObjSetMicro(p, 540);
	} else if (object[i]._otype == ObjectType::L2RDOOR && (object[i]._oVar4 == 1 || object[i]._oVar4 == 2)) {
		ObjSetMicro(p, 17);
	}
}

void SyncL3Doors(int i)
{
	object[i]._oMissFlag = true;
	V2Di p = object[i]._o;
	object[i]._oSelFlag = 2;
	if (object[i]._otype == ObjectType::L3LDOOR && object[i]._oVar4 == 0) {
		ObjSetMicro(p, 531);
	} else if (object[i]._otype == ObjectType::L3LDOOR && (object[i]._oVar4 == 1 || object[i]._oVar4 == 2)) {
		ObjSetMicro(p, 538);
	} else if (object[i]._otype == ObjectType::L3RDOOR && object[i]._oVar4 == 0) {
		ObjSetMicro(p, 534);
	} else if (object[i]._otype == ObjectType::L3RDOOR && (object[i]._oVar4 == 1 || object[i]._oVar4 == 2)) {
		ObjSetMicro(p, 541);
	}
}

void SyncObjectAnim(int o)
{
	int file;
	int i;
	int ofindex;

	file = ObjFileList[0];
	ofindex = AllObjects[object[o]._otype].ofindex;
	i = 0;
	while (file != ofindex) {
		file = ObjFileList[i + 1];
		i++;
	}
	object[o]._oAnimData = pObjCels[i];
	switch (object[o]._otype) {
	case ObjectType::BOOK2R:
	case ObjectType::BLINDBOOK:
	case ObjectType::STEELTOME:
		SyncQSTLever(o);
		break;
	case ObjectType::L1LIGHT:
		break;
	case ObjectType::L1LDOOR:
	case ObjectType::L1RDOOR:
		SyncL1Doors(o);
		break;
	case ObjectType::L2LDOOR:
	case ObjectType::L2RDOOR:
		SyncL2Doors(o);
		break;
	case ObjectType::L3LDOOR:
	case ObjectType::L3RDOOR:
		SyncL3Doors(o);
		break;
	case ObjectType::LEVER:
	case ObjectType::BOOK2L:
	case ObjectType::SWITCHSKL:
		SyncLever(o);
		break;
	case ObjectType::CRUX1:
	case ObjectType::CRUX2:
	case ObjectType::CRUX3:
		SyncCrux(o);
		break;
	case ObjectType::PEDISTAL:
		SyncPedistal(o);
		break;
	}
}

void GetObjectStr(int i)
{
	switch (object[i]._otype) {
	case ObjectType::LEVER:
	case ObjectType::FLAMELVR:
		strcpy(infostr, "Lever");
		break;
	case ObjectType::CHEST1:
	case ObjectType::TCHEST1:
		strcpy(infostr, "Small Chest");
		break;
	case ObjectType::L1LDOOR:
	case ObjectType::L1RDOOR:
	case ObjectType::L2LDOOR:
	case ObjectType::L2RDOOR:
	case ObjectType::L3LDOOR:
	case ObjectType::L3RDOOR:
		if (object[i]._oVar4 == 1)
			strcpy(infostr, "Open Door");
		if (object[i]._oVar4 == 0)
			strcpy(infostr, "Closed Door");
		if (object[i]._oVar4 == 2)
			strcpy(infostr, "Blocked Door");
		break;
	case ObjectType::BOOK2L:
		if (lvl.setlevel) {
			if (lvl.setlvlnum == SetLvl::BoneChamb) {
				strcpy(infostr, "Ancient Tome");
			} else if (lvl.setlvlnum == SetLvl::VileBetrayer) {
				strcpy(infostr, "Book of Vileness");
			}
		}
		break;
	case ObjectType::SWITCHSKL:
		strcpy(infostr, "Skull Lever");
		break;
	case ObjectType::BOOK2R:
		strcpy(infostr, "Mythical Book");
		break;

	case ObjectType::CHEST2:
	case ObjectType::TCHEST2:
		strcpy(infostr, "Chest");
		break;
	case ObjectType::CHEST3:
	case ObjectType::TCHEST3:
	case ObjectType::SIGNCHEST:
		strcpy(infostr, "Large Chest");
		break;
	case ObjectType::CRUX1:
	case ObjectType::CRUX2:
	case ObjectType::CRUX3:
		strcpy(infostr, "Crucified Skeleton");
		break;
	case ObjectType::SARC:
		strcpy(infostr, "Sarcophagus");
		break;
	case ObjectType::BOOKSHELF:
		strcpy(infostr, "Bookshelf");
		break;
	case ObjectType::BARREL:
	case ObjectType::BARRELEX:
		strcpy(infostr, "Barrel");
		break;
	case ObjectType::SKELBOOK:
		strcpy(infostr, "Skeleton Tome");
		break;
	case ObjectType::SHRINEL:
	case ObjectType::SHRINER:
		sprintf(tempstr, "%s Shrine", shrinestrs[object[i]._oVar1]);
		strcpy(infostr, tempstr);
		break;
	case ObjectType::BOOKCASEL:
	case ObjectType::BOOKCASER:
		strcpy(infostr, "Bookcase");
		break;
	case ObjectType::BOOKSTAND:
		strcpy(infostr, "Library Book");
		break;
	case ObjectType::BLOODFTN:
		strcpy(infostr, "Blood Fountain");
		break;
	case ObjectType::DECAP:
		strcpy(infostr, "Decapitated Body");
		break;
	case ObjectType::BLINDBOOK:
		strcpy(infostr, "Book of the Blind");
		break;
	case ObjectType::BLOODBOOK:
		strcpy(infostr, "Book of Blood");
		break;
	case ObjectType::PEDISTAL:
		strcpy(infostr, "Pedestal of Blood");
		break;
	case ObjectType::PURIFYINGFTN:
		strcpy(infostr, "Purifying Spring");
		break;
	case ObjectType::ARMORSTAND:
	case ObjectType::WARARMOR:
		strcpy(infostr, "Armor");
		break;
	case ObjectType::GOATSHRINE:
		strcpy(infostr, "Goat Shrine");
		break;
	case ObjectType::CAULDRON:
		strcpy(infostr, "Cauldron");
		break;
	case ObjectType::MURKYFTN:
		strcpy(infostr, "Murky Pool");
		break;
	case ObjectType::TEARFTN:
		strcpy(infostr, "Fountain of Tears");
		break;
	case ObjectType::STEELTOME:
		strcpy(infostr, "Steel Tome");
		break;
	case ObjectType::STORYBOOK:
		strcpy(infostr, StoryBookName[object[i]._oVar3]);
		break;
	case ObjectType::WARWEAP:
	case ObjectType::WEAPONRACK:
		strcpy(infostr, "Weapon Rack");
		break;
	case ObjectType::MUSHPATCH:
		strcpy(infostr, "Mushroom Patch");
		break;
	case ObjectType::LAZSTAND:
		strcpy(infostr, "Vile Stand");
		break;
	case ObjectType::SLAINHERO:
		strcpy(infostr, "Slain Hero");
		break;
	}
	if (myplr().data._pClass == PlayerClass::rogue) {
		if (object[i]._oTrapFlag) {
			sprintf(tempstr, "Trapped %s", infostr);
			strcpy(infostr, tempstr);
			infoclr = COL_RED;
		}
	}
}

}
