/**
 * @file items.cpp
 *
 * Implementation of item functionality.
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

ItemTemplate itemTypes[ITEMTYPES];
ItemManager items;

//int itemactive[MAXITEMS];
bool uitemflag;
//int itemavail[MAXITEMS];
ItemStruct curruitem;
ItemGetRecordStruct itemrecord[MAXITEMS];
//ItemStruct item[MAXITEMS + 1];
bool itemhold[3][3];
//uint8_t *itemanims[ITEMTYPES];
bool UniqueItemFlag[128];
//int numitems;
int gnNumGetRecords;

/* data */

uint8_t ItemCAnimTbl[] = {
	20, 16, 16, 16, 4, 4, 4, 12, 12, 12,
	12, 12, 12, 12, 12, 21, 21, 25, 12, 28,
	28, 28, 0, 0, 0, 32, 0, 0, 0, 24,
	24, 26, 2, 25, 22, 23, 24, 25, 27, 27,
	29, 0, 0, 0, 12, 12, 12, 12, 12, 0,
	8, 8, 0, 8, 8, 8, 8, 8, 8, 6,
	8, 8, 8, 6, 8, 8, 6, 8, 8, 6,
	6, 6, 8, 8, 8, 5, 9, 13, 13, 13,
	5, 5, 5, 15, 5, 5, 18, 18, 18, 30,
	5, 5, 14, 5, 14, 13, 16, 18, 5, 5,
	7, 1, 3, 17, 1, 15, 10, 14, 3, 11,
	8, 0, 1, 7, 0, 7, 15, 7, 3, 3,
	3, 6, 6, 11, 11, 11, 31, 14, 14, 14,
	6, 6, 7, 3, 8, 14, 0, 14, 14, 0,
	33, 1, 1, 1, 1, 1, 7, 7, 7, 14,
	14, 17, 17, 17, 0, 34, 1, 0, 3, 17,
	8, 8, 6, 1, 3, 3, 11, 3, 4
};
char *ItemDropNames[] = {
	"Armor2",
	"Axe",
	"FBttle",
	"Bow",
	"GoldFlip",
	"Helmut",
	"Mace",
	"Shield",
	"SwrdFlip",
	"Rock",
	"Cleaver",
	"Staff",
	"Ring",
	"CrownF",
	"LArmor",
	"WShield",
	"Scroll",
	"FPlateAr",
	"FBook",
	"Food",
	"FBttleBB",
	"FBttleDY",
	"FBttleOR",
	"FBttleBR",
	"FBttleBL",
	"FBttleBY",
	"FBttleWH",
	"FBttleDB",
	"FEar",
	"FBrain",
	"FMush",
	"Innsign",
	"Bldstn",
	"Fanvil",
	"FLazStaf",
};
uint8_t ItemAnimLs[] = {
	15,
	13,
	16,
	13,
	10,
	13,
	13,
	13,
	13,
	10,
	13,
	13,
	13,
	13,
	13,
	13,
	13,
	13,
	13,
	1,
	16,
	16,
	16,
	16,
	16,
	16,
	16,
	16,
	13,
	12,
	12,
	13,
	13,
	13,
	8,
};
int ItemDropSnds[] = {
	IS_FHARM,
	IS_FAXE,
	IS_FPOT,
	IS_FBOW,
	IS_GOLD,
	IS_FCAP,
	IS_FSWOR,
	IS_FSHLD,
	IS_FSWOR,
	IS_FROCK,
	IS_FAXE,
	IS_FSTAF,
	IS_FRING,
	IS_FCAP,
	IS_FLARM,
	IS_FSHLD,
	IS_FSCRL,
	IS_FHARM,
	IS_FBOOK,
	IS_FLARM,
	IS_FPOT,
	IS_FPOT,
	IS_FPOT,
	IS_FPOT,
	IS_FPOT,
	IS_FPOT,
	IS_FPOT,
	IS_FPOT,
	IS_FBODY,
	IS_FBODY,
	IS_FMUSH,
	IS_ISIGN,
	IS_FBLST,
	IS_FANVL,
	IS_FSTAF,
};
int ItemInvSnds[] = {
	IS_IHARM,
	IS_IAXE,
	IS_IPOT,
	IS_IBOW,
	IS_GOLD,
	IS_ICAP,
	IS_ISWORD,
	IS_ISHIEL,
	IS_ISWORD,
	IS_IROCK,
	IS_IAXE,
	IS_ISTAF,
	IS_IRING,
	IS_ICAP,
	IS_ILARM,
	IS_ISHIEL,
	IS_ISCROL,
	IS_IHARM,
	IS_IBOOK,
	IS_IHARM,
	IS_IPOT,
	IS_IPOT,
	IS_IPOT,
	IS_IPOT,
	IS_IPOT,
	IS_IPOT,
	IS_IPOT,
	IS_IPOT,
	IS_IBODY,
	IS_IBODY,
	IS_IMUSH,
	IS_ISIGN,
	IS_IBLST,
	IS_IANVL,
	IS_ISTAF,
};
int idoppely = 16;
int premiumlvladd[6] = { -1, -1, 0, 0, 1, 2 };



void InitItemGFX()
{
	int i;
	char arglist[64];

	for (i = 0; i < ITEMTYPES; i++) {
		sprintf(arglist, "Items\\%s.CEL", ItemDropNames[i]);
		itemTypes[i].anims = LoadFileInMem(arglist, NULL);
	}
	memset(UniqueItemFlag, 0, sizeof(UniqueItemFlag));
}

bool ItemPlace(V2Di pos)
{
	if (grid.at(pos).isActor()) return false;
	if (grid.at(pos).isPlayer()) return false;
	if (grid.at(pos).isItem()) return false;
	if (grid.at(pos).isObject()) return false;
	if (grid.at(pos).dFlags & DunTileFlag::POPULATED) return false;
	if (grid.at(pos).isSolid()) return false;
	return true;
}

void AddInitItems()
{
	V2Di pos;
	int rnd = random_(11, 3) + 3;
	for (int j = 0; j < rnd; j++) {
		Item * item = items.createNewItem();
		pos.x = random_(12, 80) + 16;
		pos.y = random_(12, 80) + 16;
		while (!ItemPlace(pos)) {
			pos.x = random_(12, 80) + 16;
			pos.y = random_(12, 80) + 16;
		}
		item._i = pos;
		grid.at(pos).setItem(item.id);
		item._iSeed = GetRndSeed();
		SetRndSeed(item._iSeed);
		if (random_(12, 2))
			item.loadPresetAttributes(ItemIndex::HEAL, lvl.currlevel);
		else
			item.loadPresetAttributes(ItemIndex::MANA, lvl.currlevel);
		item._iCreateInfo = lvl.currlevel - 0x8000;
		item.SetupItem();
		item._iAnimFrame = item._iAnimLen;
		item._iAnimFlag = false;
		item._iSelFlag = 1;
		DeltaAddItem(item);
	}
}

void InitItems()
{
	golditem = item[0];
	loadPresetAttributes(0, ItemIndex::GOLD, 1);
	golditem._iStatFlag = true;

	if (!lvl.setlevel) {
		long s = GetRndSeed(); /* unused */
		if (QuestStatus(Q_ROCK))
			SpawnRock();
		if (QuestStatus(Q_ANVIL))
			SpawnQuestItem(ItemIndex::ANVIL, { 2 * lvl.getpc().x + 27, 2 * lvl.getpc().y + 27 }, 0, 1);
		if (lvl.currlevel > 0 && lvl.currlevel < 16)
			AddInitItems();
	}

	uitemflag = false;
}

bool ItemMinStats(PlayerStruct *p, ItemStruct *x)
{
	if (p->_pMagic < x->_iMinMag)
		return false;

	if (p->_pStrength < x->_iMinStr)
		return false;

	if (p->_pDexterity < x->_iMinDex)
		return false;

	return true;
}

void SetPlrHandItem(ItemStruct *h, int idata)
{
	ItemDataStruct *pAllItem;

	pAllItem = &AllItemsList[idata];

	// zero-initialize struct
	memset(h, 0, sizeof(*h));

	h->_itype = pAllItem->itype;
	h->_iCurs = pAllItem->iCurs;
	strcpy(h->_iName, pAllItem->iName);
	strcpy(h->_iIName, pAllItem->iName);
	h->_iLoc = pAllItem->iLoc;
	h->_iClass = pAllItem->iClass;
	h->_iMinDam = pAllItem->iMinDam;
	h->_iMaxDam = pAllItem->iMaxDam;
	h->_iAC = pAllItem->iMinAC;
	h->_iMiscId = pAllItem->iMiscId;
	h->_iSpell = pAllItem->iSpell;

	if (pAllItem->iMiscId == MiscItemId::STAFF) {
		h->_iCharges = 40;
	}

	h->_iMaxCharges = h->_iCharges;
	h->_iDurability = pAllItem->iDurability;
	h->_iMaxDur = pAllItem->iDurability;
	h->_iMinStr = pAllItem->iMinStr;
	h->_iMinMag = pAllItem->iMinMag;
	h->_iMinDex = pAllItem->iMinDex;
	h->_ivalue = pAllItem->iValue;
	h->_iIvalue = pAllItem->iValue;
	h->_iPrePower = -1;
	h->_iSufPower = -1;
	h->_iMagical = ItemQuality::normal;
	h->IDidx = idata;
}

void GetPlrHandSeed(ItemStruct *h)
{
	h->_iSeed = GetRndSeed();
}


void SetPlrHandSeed(ItemStruct *h, int iseed)
{
	h->_iSeed = iseed;
}

void SetPlrHandGoldCurs(ItemStruct *h)
{
	if (h->_ivalue >= GOLD_MEDIUM_LIMIT)
		h->_iCurs = ItemCursor::GOLD_LARGE;
	else if (h->_ivalue <= GOLD_SMALL_LIMIT)
		h->_iCurs = ItemCursor::GOLD_SMALL;
	else
		h->_iCurs = ItemCursor::GOLD_MEDIUM;
}

bool ItemSpaceOk(int i, int j)
{
	int oi;

	if (!grid.isValid({ i, j }))
		return false;

	if (grid[i][j].isActor())
		return false;

	if (grid[i][j].isPlayer())
		return false;

	if (grid[i][j].isItem())
		return false;

	if (grid[i][j].isObject()) {
		oi = grid[i][j].getObject();
		if (object[oi]._oSolidFlag)
			return false;
	}

	if (grid[i + 1][j + 1].isObject() && object[grid[i + 1][j + 1].getObject()]._oSelFlag != 0)
		return false;

	if (grid[i + 1][j + 1].isObject() && object[-(grid[i + 1][j + 1].getObject())]._oSelFlag != 0)
		return false;

	if (grid[i + 1][j].isObject()
	    && grid[i][j + 1].isObject()
	    && object[grid[i + 1][j].getObject()]._oSelFlag != 0
	    && object[grid[i][j + 1].getObject()]._oSelFlag != 0) {
		return false;
	}

	return !grid[i][j].isSolid();
}

void GetSuperItemLoc(V2Di pos, V2Di & r)
{
	for (int k = 1; k < 50; k++) {
		for (int j = -k; j <= k; j++) {
			r.y = pos.y + j;
			for (int i = -k; i <= k; i++) {
				r.x = i + pos.x;
				if (ItemSpaceOk(r.x, r.y)) {
					return;
				}
			}
		}
	}
}

int RndPL(int param1, int param2)
{
	return param1 + random_(22, param2 - param1 + 1);
}

int PLVal(int pv, int p1, int p2, int minv, int maxv)
{
	if (p1 == p2)
		return minv;
	if (minv == maxv)
		return minv;
	return minv + (maxv - minv) * (100 * (pv - p1) / (p2 - p1)) / 100;
}

int RndItem(int m)
{
	int i, ri;
	int ril[512];

	if ((monsters[m].data.MData->mTreasure & 0x8000) != 0)
		return -1 - (monsters[m].data.MData->mTreasure & 0xFFF);

	if (monsters[m].data.MData->mTreasure & 0x4000)
		return 0;

	if (random_(24, 100) > 40)
		return 0;

	if (random_(24, 100) > 25)
		return 1;

	ri = 0;
	for (i = 0; AllItemsList[i].iLoc != ILOC_INVALID; i++) {
		if (AllItemsList[i].iRnd == 2 && monsters[m].data.mLevel >= AllItemsList[i].iMinMLvl) {
			ril[ri] = i;
			ri++;
		}
		if (AllItemsList[i].iRnd && monsters[m].data.mLevel >= AllItemsList[i].iMinMLvl) {
			ril[ri] = i;
			ri++;
		}
		if (AllItemsList[i].iSpell == SpellId::RESURRECT && plr.isSingleplayer())
			ri--;
		if (AllItemsList[i].iSpell == SpellId::HEALOTHER && plr.isSingleplayer())
			ri--;
	}

	return ril[random_(24, ri)] + 1;
}

int RndUItem(int m)
{
	int i, ri;
	int ril[512];
	bool okflag;

	if (m != -1 && (monsters[m].data.MData->mTreasure & 0x8000) != 0 && plr.isSingleplayer())
		return -1 - (monsters[m].data.MData->mTreasure & 0xFFF);

	ri = 0;
	for (i = 0; AllItemsList[i].iLoc != ILOC_INVALID; i++) {
		okflag = true;
		if (!AllItemsList[i].iRnd)
			okflag = false;
		if (m != -1) {
			if (monsters[m].data.mLevel < AllItemsList[i].iMinMLvl)
				okflag = false;
		} else {
			if (2 * lvl.currlevel < AllItemsList[i].iMinMLvl)
				okflag = false;
		}
		if (AllItemsList[i].itype == ItemType::misc)
			okflag = false;
		if (AllItemsList[i].itype == ItemType::gold)
			okflag = false;
		if (AllItemsList[i].itype == ItemType::meat)
			okflag = false;
		if (AllItemsList[i].iMiscId == MiscItemId::BOOK)
			okflag = true;
		if (AllItemsList[i].iSpell == SpellId::RESURRECT && plr.isSingleplayer())
			okflag = false;
		if (AllItemsList[i].iSpell == SpellId::HEALOTHER && plr.isSingleplayer())
			okflag = false;
		if (okflag) {
			ril[ri] = i;
			ri++;
		}
	}

	return ril[random_(25, ri)];
}

int RndAllItems()
{
	int i, ri;
	int ril[512];

	if (random_(26, 100) > 25)
		return 0;

	ri = 0;
	for (i = 0; AllItemsList[i].iLoc != ILOC_INVALID; i++) {
		if (AllItemsList[i].iRnd && 2 * lvl.currlevel >= AllItemsList[i].iMinMLvl) {
			ril[ri] = i;
			ri++;
		}
		if (AllItemsList[i].iSpell == SpellId::RESURRECT && plr.isSingleplayer())
			ri--;
		if (AllItemsList[i].iSpell == SpellId::HEALOTHER && plr.isSingleplayer())
			ri--;
	}

	return ril[random_(26, ri)];
}

int RndTypeItems(int itype, int imid)
{
	int i, ri;
	bool okflag;
	int ril[512];

	ri = 0;
	for (i = 0; AllItemsList[i].iLoc != ILOC_INVALID; i++) {
		okflag = true;
		if (!AllItemsList[i].iRnd)
			okflag = false;
		if (lvl.currlevel << 1 < AllItemsList[i].iMinMLvl)
			okflag = false;
		if (AllItemsList[i].itype != itype)
			okflag = false;
		if (imid != -1 && AllItemsList[i].iMiscId != imid)
			okflag = false;
		if (okflag) {
			ril[ri] = i;
			ri++;
		}
	}

	return ril[random_(27, ri)];
}

void SpawnUnique(int uid, V2Di pos)
{
	Item & item = items.createNewItem();
	item.GetSuperItemSpace(pos);

	int itype = 0;
	while (AllItemsList[itype].iItemId != UniqueItemList[uid].UIItemId) {
		itype++;
	}

	item.loadPresetAttributes(itype, lvl.currlevel);
	item.GetUniqueItem(uid);
	item.SetupItem();
}

void SpawnItem(int m, V2Di pos, bool sendmsg)
{
	int onlygood, idx;

	if (monsters[m].data._uniqtype || ((monsters[m].data.MData->mTreasure & 0x8000) && plr.isMultiplayer())) {
		idx = RndUItem(m);
		if (idx < 0) {
			SpawnUnique(-(idx + 1), pos);
			return;
		}
		onlygood = 1;
	} else if (quests[Q_MUSHROOM]._qactive != QUEST_ACTIVE || quests[Q_MUSHROOM]._qvar1 != QS_MUSHGIVEN) {
		idx = RndItem(m);
		if (!idx)
			return;
		if (idx > 0) {
			idx--;
			onlygood = 0;
		} else {
			SpawnUnique(-(idx + 1), pos);
			return;
		}
	} else {
		idx = ItemIndex::BRAIN;
		quests[Q_MUSHROOM]._qvar1 = QS_BRAINSPAWNED;
	}

	Item & item = items.createNewItem();

	if (monsters[m].data._uniqtype) {
		item.SetupAllItems(idx, GetRndSeed(), monsters[m].data.MData->mLevel, 15, onlygood, false, false);
	} else {
		item.SetupAllItems(idx, GetRndSeed(), monsters[m].data.MData->mLevel, 1, onlygood, false, false);
	}
	if (sendmsg)
		NetSendCmdDItem(false, item);
}

void CreateItem(int uid, V2Di pos)
{
	Item &item = items.createNewItem();
	item.GetSuperItemSpace(pos);

	int idx = 0;
	while (AllItemsList[idx].iItemId != UniqueItemList[uid].UIItemId) {
		idx++;
	}

	item.loadPresetAttributes(idx, lvl.currlevel);
	item.GetUniqueItem(uid);
	item.SetupItem();
	item._iMagical = ItemQuality::unique;
}

void CreateRndItem(V2Di pos, bool onlygood, bool sendmsg, bool delta)
{
	int idx;

	if (onlygood)
		idx = RndUItem(-1);
	else
		idx = RndAllItems();

	Item &item = items.createNewItem();
	item.GetSuperItemSpace(pos);
	item.SetupAllItems(idx, GetRndSeed(), 2 * lvl.currlevel, 1, onlygood, false, delta);
	if (sendmsg)
		NetSendCmdDItem(false, item);
	if (delta)
		DeltaAddItem(item);
}

void CreateTypeItem(V2Di pos, bool onlygood, int itype, int imisc, bool sendmsg, bool delta)
{
	int idx;
	if (itype != ItemType::gold)
		idx = RndTypeItems(itype, imisc);
	else
		idx = 0;

	Item &item = items.createNewItem();
	item.GetSuperItemSpace(pos);
	item.SetupAllItems(idx, GetRndSeed(), 2 * lvl.currlevel, 1, onlygood, false, delta);
	if (sendmsg) NetSendCmdDItem(false, item);
	if (delta) DeltaAddItem(item);
}

void SpawnQuestItem(int itemid, V2Di pos, int randarea, int selflag)
{
	bool failed;
	int i, j, tries;

	if (randarea) {
		tries = 0;
		while (1) {
			tries++;
			if (tries > 1000 && randarea > 1)
				randarea--;
			pos.x = random_(0, MAXDUNX);
			pos.y = random_(0, MAXDUNY);
			failed = false;
			for (i = 0; i < randarea && !failed; i++) {
				for (j = 0; j < randarea && !failed; j++) {
					failed = !ItemSpaceOk(i + pos.x, j + pos.y);
				}
			}
			if (!failed)
				break;
		}
	}

	Item & item = items.createNewItem();
	item._i = pos;
	grid.at(pos).setItem(item.id);
	item.loadPresetAttributes(itemid, lvl.currlevel);
	item.SetupItem();
	item._iPostDraw = true;
	if (selflag) {
		item._iSelFlag = selflag;
		item._iAnimFrame = item._iAnimLen;
		item._iAnimFlag = false;
	}
}

void SpawnRock()
{
	int ii;
	int ostand = false;
	for (int i = 0; i < nobjects && !ostand; i++) {
		ii = objectactive[i];
		ostand = object[ii]._otype == ObjectType::STAND;
	}
	if (ostand) {
		Item &item = items.createNewItem();
		V2Di n = object[ii]._o;
		item._i = n;
		grid[n.x][item._i.y].setItem(item.id);
		item.loadPresetAttributes(ItemIndex::ROCK, lvl.currlevel);
		item.SetupItem();
		item._iSelFlag = 2;
		item._iPostDraw = true;
		item._iAnimFrame = 11;
	}
}

void ItemDoppel()
{
	if (plr.isMultiplayer()) {
		for (int idoppelx = 16; idoppelx < 96; idoppelx++) {
			if (grid[idoppelx][idoppely].isItem()) {
				ItemStruct * i = &items.get(grid[idoppelx][idoppely].getItem());
				if (i->_i.x != idoppelx || i->_i.y != idoppely)
					grid[idoppelx][idoppely].clearItem();
			}
		}
		idoppely++;
		if (idoppely == 96)
			idoppely = 16;
	}
}

void ProcessItems()
{
	for (int i = 0; i < numitems; i++) {
		int ii = itemactive[i];
		if (item[ii]._iAnimFlag) {
			item[ii]._iAnimFrame++;
			if (item[ii]._iCurs == ItemCursor::MAGIC_ROCK) {
				if (item[ii]._iSelFlag == 1 && item[ii]._iAnimFrame == 11)
					item[ii]._iAnimFrame = 1;
				if (item[ii]._iSelFlag == 2 && item[ii]._iAnimFrame == 21)
					item[ii]._iAnimFrame = 11;
			} else {
				if (item[ii]._iAnimFrame == item[ii]._iAnimLen >> 1)
					PlaySfxLoc(ItemDropSnds[ItemCAnimTbl[item[ii]._iCurs]], item[ii]._i);

				if (item[ii]._iAnimFrame >= item[ii]._iAnimLen) {
					item[ii]._iAnimFrame = item[ii]._iAnimLen;
					item[ii]._iAnimFlag = false;
					item[ii]._iSelFlag = 1;
				}
			}
		}
	}
	ItemDoppel();
}

void FreeItemGFX()
{
	int i;

	for (i = 0; i < ITEMTYPES; i++) {
		MemFreeDbg(itemTypes[i].anims);
	}
}

void DrawUTextBack()
{
	CelDraw(RIGHT_PANEL_X - SPANEL_WIDTH + 24, SCREEN_Y + 327, pSTextBoxCels, 1, 271);
	trans_rect(RIGHT_PANEL - SPANEL_WIDTH + 27, 28, 265, 297);
}

void PrintUString(V2Di pos, bool cjustflag, char *str, int col)
{
	int len, width, i, k;
	uint8_t c;
	V2Di s;
	s = { pos.x + 96, pos.y * 12 + 204 };
	len = strlen(str);
	k = 0;
	if (cjustflag) {
		width = 0;
		for (i = 0; i < len; i++)
			width += fontkern[fontframe[gbFontTransTbl[(uint8_t)str[i]]]] + 1;
		if (width < 257)
			k = (257 - width) >> 1;
		s.x += k;
	}

	for (i = 0; i < len; i++) {
		c = fontframe[gbFontTransTbl[(uint8_t)str[i]]];
		k += fontkern[c] + 1;
		if (c && k <= 257) {
			PrintChar(s, c, col);
		}
		s.x += fontkern[c] + 1;
	}
}

void DrawULine(int y)
{
	assert(gpBuffer);

	int i;
	uint8_t *src, *dst;

	src = &gpBuffer[SCREENXY(26 + RIGHT_PANEL - SPANEL_WIDTH, 25)];
	dst = &gpBuffer[BUFFER_WIDTH * (y * 12 + 198) + 26 + RIGHT_PANEL_X - SPANEL_WIDTH];

	for (i = 0; i < 3; i++, src += BUFFER_WIDTH, dst += BUFFER_WIDTH)
		memcpy(dst, src, 266);
}

void DrawUniqueInfo()
{
	int uid, y;

	if ((!chrflag && !questlog) || SCREEN_WIDTH >= SPANEL_WIDTH * 3) {
		uid = curruitem._iUid;
		DrawUTextBack();
		PrintUString({ 0 + RIGHT_PANEL - SPANEL_WIDTH, 2 }, true, UniqueItemList[uid].UIName, 3);
		DrawULine(5);
		PrintItemPower(UniqueItemList[uid].UIPower1, &curruitem);
		y = 6 - UniqueItemList[uid].UINumPL + 8;
		PrintUString({ 0 + RIGHT_PANEL - SPANEL_WIDTH, y }, true, tempstr, 0);
		if (UniqueItemList[uid].UINumPL > 1) {
			PrintItemPower(UniqueItemList[uid].UIPower2, &curruitem);
			PrintUString({ 0 + RIGHT_PANEL - SPANEL_WIDTH, y + 2 }, true, tempstr, 0);
		}
		if (UniqueItemList[uid].UINumPL > 2) {
			PrintItemPower(UniqueItemList[uid].UIPower3, &curruitem);
			PrintUString({ 0 + RIGHT_PANEL - SPANEL_WIDTH, y + 4 }, true, tempstr, 0);
		}
		if (UniqueItemList[uid].UINumPL > 3) {
			PrintItemPower(UniqueItemList[uid].UIPower4, &curruitem);
			PrintUString({ 0 + RIGHT_PANEL - SPANEL_WIDTH, y + 6 }, true, tempstr, 0);
		}
		if (UniqueItemList[uid].UINumPL > 4) {
			PrintItemPower(UniqueItemList[uid].UIPower5, &curruitem);
			PrintUString({ 0 + RIGHT_PANEL - SPANEL_WIDTH, y + 8 }, true, tempstr, 0);
		}
		if (UniqueItemList[uid].UINumPL > 5) {
			PrintItemPower(UniqueItemList[uid].UIPower6, &curruitem);
			PrintUString({ 0 + RIGHT_PANEL - SPANEL_WIDTH, y + 10 }, true, tempstr, 0);
		}
	}
}


void BubbleSwapItem(ItemStruct *a, ItemStruct *b)
{
	ItemStruct h;

	h = *a;
	*a = *b;
	*b = h;
}

void WitchBookLevel(int ii)
{
	int slvl;

	if (witchitem[ii]._iMiscId == MiscItemId::BOOK) {
		witchitem[ii]._iMinMag = spelldata[witchitem[ii]._iSpell].sMinInt;
		slvl = myplr().data._pSplLvl[witchitem[ii]._iSpell];
		while (slvl) {
			witchitem[ii]._iMinMag += 20 * witchitem[ii]._iMinMag / 100;
			slvl--;
			if (witchitem[ii]._iMinMag + 20 * witchitem[ii]._iMinMag / 100 > 255) {
				witchitem[ii]._iMinMag = 255;
				slvl = 0;
			}
		}
	}
}







void SpawnStoreGold()
{
	loadPresetAttributes(0, ItemIndex::GOLD, 1);
	golditem = item[0];
	golditem._iStatFlag = true;
}

void RecalcStoreStats()
{
	int i;

	for (i = 0; i < SMITH_ITEMS; i++) {
		if (smithitem[i]._itype != ItemType::none) {
			smithitem[i]._iStatFlag = StoreStatOk(&smithitem[i]);
		}
	}
	for (i = 0; i < SMITH_PREMIUM_ITEMS; i++) {
		if (premiumitem[i]._itype != ItemType::none) {
			premiumitem[i]._iStatFlag = StoreStatOk(&premiumitem[i]);
		}
	}
	for (i = 0; i < 20; i++) {
		if (witchitem[i]._itype != ItemType::none) {
			witchitem[i]._iStatFlag = StoreStatOk(&witchitem[i]);
		}
	}
	for (i = 0; i < 20; i++) {
		if (healitem[i]._itype != ItemType::none) {
			healitem[i]._iStatFlag = StoreStatOk(&healitem[i]);
		}
	}
	boyitem._iStatFlag = StoreStatOk(&boyitem);
}

// Makes last created item no flippy
int ItemNoFlippy()
{
	int r = itemactive[numitems - 1];
	item[r]._iAnimFrame = item[r]._iAnimLen;
	item[r]._iAnimFlag = false;
	item[r]._iSelFlag = 1;
	return r;
}

void CreateSpellBook(V2Di pos, int ispell, bool sendmsg, bool delta)
{
	bool done = false;
	int idx = RndTypeItems(ItemType::misc, MiscItemId::BOOK);
	Item &item = items.createNewItem();
	item.GetSuperItemSpace(pos);
	while (!done) {
		item.SetupAllItems(idx, GetRndSeed(), 2 * lvl.currlevel, 1, true, false, delta);
		if (item._iMiscId == MiscItemId::BOOK && item._iSpell == ispell)
			done = true;
	}
	if (sendmsg)
		NetSendCmdDItem(false, item);
	if (delta)
		DeltaAddItem(item);
}

void CreateMagicArmor(V2Di pos, int imisc, int icurs, bool sendmsg, bool delta)
{
	bool done = false;
	Item &item = items.createNewItem();
	item.GetSuperItemSpace(pos);
	int idx = RndTypeItems(imisc, MiscItemId::NONE);
	while (!done) {
		item.SetupAllItems(idx, GetRndSeed(), 2 * lvl.currlevel, 1, true, false, delta);
		if (item._iCurs == icurs)
			done = true;
		else
			idx = RndTypeItems(imisc, MiscItemId::NONE);
	}
	if (sendmsg)
		NetSendCmdDItem(false, item);
	if (delta)
		DeltaAddItem(item);
}

void CreateMagicWeapon(V2Di pos, int imisc, int icurs, bool sendmsg, bool delta)
{
	bool done = false;
	Item &item = items.createNewItem();
	item.GetSuperItemSpace(pos);
	int idx = RndTypeItems(imisc, MiscItemId::NONE);
	while (!done) {
		item.SetupAllItems(idx, GetRndSeed(), 2 * lvl.currlevel, 1, true, false, delta);
		if (item._iCurs == icurs)
			done = true;
		else
			idx = RndTypeItems(imisc, MiscItemId::NONE);
	}
	if (sendmsg)
		NetSendCmdDItem(false, item);
	if (delta)
		DeltaAddItem(item);
}

bool GetItemRecord(int nSeed, WORD wCI, int nIndex)
{
	int i;
	DWORD dwTicks;

	dwTicks = SDL_GetTicks();

	for (i = 0; i < gnNumGetRecords; i++) {
		if (dwTicks - itemrecord[i].dwTimestamp > 6000) {
			NextItemRecord(i);
			i--;
		} else if (nSeed == itemrecord[i].nSeed && wCI == itemrecord[i].wCI && nIndex == itemrecord[i].nIndex) {
			return false;
		}
	}

	return true;
}

void NextItemRecord(int i)
{
	gnNumGetRecords--;

	if (gnNumGetRecords == 0) {
		return;
	}

	itemrecord[i].dwTimestamp = itemrecord[gnNumGetRecords].dwTimestamp;
	itemrecord[i].nSeed = itemrecord[gnNumGetRecords].nSeed;
	itemrecord[i].wCI = itemrecord[gnNumGetRecords].wCI;
	itemrecord[i].nIndex = itemrecord[gnNumGetRecords].nIndex;
}

void SetItemRecord(int nSeed, WORD wCI, int nIndex)
{
	DWORD dwTicks;

	dwTicks = SDL_GetTicks();

	if (gnNumGetRecords == MAXITEMS) {
		return;
	}

	itemrecord[gnNumGetRecords].dwTimestamp = dwTicks;
	itemrecord[gnNumGetRecords].nSeed = nSeed;
	itemrecord[gnNumGetRecords].wCI = wCI;
	itemrecord[gnNumGetRecords].nIndex = nIndex;
	gnNumGetRecords++;
}

void PutItemRecord(int nSeed, WORD wCI, int nIndex)
{
	int i;
	DWORD dwTicks;

	dwTicks = SDL_GetTicks();

	for (i = 0; i < gnNumGetRecords; i++) {
		if (dwTicks - itemrecord[i].dwTimestamp > 6000) {
			NextItemRecord(i);
			i--;
		} else if (nSeed == itemrecord[i].nSeed && wCI == itemrecord[i].wCI && nIndex == itemrecord[i].nIndex) {
			NextItemRecord(i);
			break;
		}
	}
}

DEVILUTION_END_NAMESPACE
