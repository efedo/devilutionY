/**
 * @file items.cpp
 *
 * Implementation of item functionality.
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

ItemType itemTypes[ITEMTYPES];
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
	if (grid.at(pos).isMonster()) return false;
	if (grid.at(pos).isPlayer()) return false;
	if (grid.at(pos).isItem()) return false;
	if (grid.at(pos).isObject()) return false;
	if (grid.at(pos).dFlags & BFLAG_POPULATED) return false;
	if (grid.at(pos).isSolid()) return false;
	return TRUE;
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
			item.GetItemAttrs(IDI_HEAL, lvl.currlevel);
		else
			item.GetItemAttrs(IDI_MANA, lvl.currlevel);
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
	GetItemAttrs(0, IDI_GOLD, 1);
	golditem._iStatFlag = TRUE;

	if (!lvl.setlevel) {
		long s = GetRndSeed(); /* unused */
		if (QuestStatus(Q_ROCK))
			SpawnRock();
		if (QuestStatus(Q_ANVIL))
			SpawnQuestItem(IDI_ANVIL, { 2 * lvl.getpc().x + 27, 2 * lvl.getpc().y + 27 }, 0, 1);
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

	return TRUE;
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

	if (pAllItem->iMiscId == IMISC_STAFF) {
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
	h->_iMagical = ITEM_QUALITY_NORMAL;
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
		h->_iCurs = ICURS_GOLD_LARGE;
	else if (h->_ivalue <= GOLD_SMALL_LIMIT)
		h->_iCurs = ICURS_GOLD_SMALL;
	else
		h->_iCurs = ICURS_GOLD_MEDIUM;
}


bool ItemSpaceOk(int i, int j)
{
	int oi;

	if (!grid.isValid({ i, j }))
		return false;

	if (grid[i][j].isMonster())
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
		if (AllItemsList[i].iSpell == SPL_RESURRECT && plr.isSingleplayer())
			ri--;
		if (AllItemsList[i].iSpell == SPL_HEALOTHER && plr.isSingleplayer())
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
		okflag = TRUE;
		if (!AllItemsList[i].iRnd)
			okflag = false;
		if (m != -1) {
			if (monsters[m].data.mLevel < AllItemsList[i].iMinMLvl)
				okflag = false;
		} else {
			if (2 * lvl.currlevel < AllItemsList[i].iMinMLvl)
				okflag = false;
		}
		if (AllItemsList[i].itype == ITYPE_MISC)
			okflag = false;
		if (AllItemsList[i].itype == ITYPE_GOLD)
			okflag = false;
		if (AllItemsList[i].itype == ITYPE_MEAT)
			okflag = false;
		if (AllItemsList[i].iMiscId == IMISC_BOOK)
			okflag = TRUE;
		if (AllItemsList[i].iSpell == SPL_RESURRECT && plr.isSingleplayer())
			okflag = false;
		if (AllItemsList[i].iSpell == SPL_HEALOTHER && plr.isSingleplayer())
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
		if (AllItemsList[i].iSpell == SPL_RESURRECT && plr.isSingleplayer())
			ri--;
		if (AllItemsList[i].iSpell == SPL_HEALOTHER && plr.isSingleplayer())
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
		okflag = TRUE;
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

	item.GetItemAttrs(itype, lvl.currlevel);
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
		idx = IDI_BRAIN;
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

	item.GetItemAttrs(idx, lvl.currlevel);
	item.GetUniqueItem(uid);
	item.SetupItem();
	item._iMagical = ITEM_QUALITY_UNIQUE;
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

void CreateRndUseful(int pnum, V2Di pos, bool sendmsg)
{
	Item &item = items.createNewItem();
	item.GetSuperItemSpace(pos);
	item.SetupAllUseful(GetRndSeed(), lvl.currlevel);
	if (sendmsg) {
		NetSendCmdDItem(false, item);
	}
}

void CreateTypeItem(V2Di pos, bool onlygood, int itype, int imisc, bool sendmsg, bool delta)
{
	int idx;
	if (itype != ITYPE_GOLD)
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
	item.GetItemAttrs(itemid, lvl.currlevel);
	item.SetupItem();
	item._iPostDraw = TRUE;
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
		ostand = object[ii]._otype == OBJ_STAND;
	}
	if (ostand) {
		Item &item = items.createNewItem();
		V2Di n = object[ii]._o;
		item._i = n;
		grid[n.x][item._i.y].setItem(item.id);
		item.GetItemAttrs(IDI_ROCK, lvl.currlevel);
		item.SetupItem();
		item._iSelFlag = 2;
		item._iPostDraw = TRUE;
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
			if (item[ii]._iCurs == ICURS_MAGIC_ROCK) {
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

void CheckIdentify(int pnum, int cii)
{
	ItemStruct *pi;

	if (cii >= NUM_INVLOC)
		pi = &plr[pnum].data.InvList[cii - NUM_INVLOC];
	else
		pi = &plr[pnum].data.InvBody[cii];

	pi->_iIdentified = TRUE;
	CalcPlrInv(pnum, TRUE);

	if (pnum == myplr())
		SetCursor_(CURSOR_HAND);
}

void DoRepair(int pnum, int cii)
{
	ItemStruct *pi;
	Player & p = plr[pnum];
	PlaySfxLoc(IS_REPAIR, p.pos());

	if (cii >= NUM_INVLOC) {
		pi = &p.data.InvList[cii - NUM_INVLOC];
	} else {
		pi = &p.data.InvBody[cii];
	}

	RepairItem(pi, p.data._pLevel);
	CalcPlrInv(pnum, TRUE);

	if (pnum == myplr()) SetCursor_(CURSOR_HAND);
}

void RepairItem(ItemStruct *i, int lvl)
{
	int rep, d;

	if (i->_iDurability == i->_iMaxDur) {
		return;
	}

	if (i->_iMaxDur <= 0) {
		i->_itype = ITYPE_NONE;
		return;
	}

	rep = 0;
	do {
		rep += lvl + random_(37, lvl);
		d = i->_iMaxDur / (lvl + 9);
		if (d < 1)
			d = 1;
		i->_iMaxDur = i->_iMaxDur - d;
		if (!i->_iMaxDur) {
			i->_itype = ITYPE_NONE;
			return;
		}
	} while (rep + i->_iDurability < i->_iMaxDur);

	i->_iDurability += rep;
	if (i->_iDurability > i->_iMaxDur)
		i->_iDurability = i->_iMaxDur;
}

void DoRecharge(int pnum, int cii)
{
	PlayerStruct *p;
	ItemStruct *pi;
	int r;

	p = &plr[pnum].data;
	if (cii >= NUM_INVLOC) {
		pi = &p->InvList[cii - NUM_INVLOC];
	} else {
		pi = &p->InvBody[cii];
	}
	if (pi->_itype == ITYPE_STAFF && pi->_iSpell) {
		r = spelldata[pi->_iSpell].sBookLvl;
		r = random_(38, p->_pLevel / r) + 1;
		RechargeItem(pi, r);
		CalcPlrInv(pnum, TRUE);
	}

	if (pnum == myplr())
		SetCursor_(CURSOR_HAND);
}

void RechargeItem(ItemStruct *i, int r)
{
	while (i->_iCharges != i->_iMaxCharges) {
		i->_iMaxCharges--;
		if (i->_iMaxCharges == 0) {
			break;
		}
		i->_iCharges += r;
		if (i->_iCharges >= i->_iMaxCharges) {
			if (i->_iCharges > i->_iMaxCharges)
				i->_iCharges = i->_iMaxCharges;
			return;
		}
	}
}

void PrintItemOil(char IDidx)
{
	switch (IDidx) {
	case IMISC_FULLHEAL:
		strcpy(tempstr, "fully recover life");
		AddPanelString(tempstr, TRUE);
		break;
	case IMISC_HEAL:
		strcpy(tempstr, "recover partial life");
		AddPanelString(tempstr, TRUE);
		break;
	case IMISC_OLDHEAL:
		strcpy(tempstr, "recover life");
		AddPanelString(tempstr, TRUE);
		break;
	case IMISC_DEADHEAL:
		strcpy(tempstr, "deadly heal");
		AddPanelString(tempstr, TRUE);
		break;
	case IMISC_MANA:
		strcpy(tempstr, "recover mana");
		AddPanelString(tempstr, TRUE);
		break;
	case IMISC_FULLMANA:
		strcpy(tempstr, "fully recover mana");
		AddPanelString(tempstr, TRUE);
		break;
	case IMISC_ELIXSTR:
		strcpy(tempstr, "increase strength");
		AddPanelString(tempstr, TRUE);
		break;
	case IMISC_ELIXMAG:
		strcpy(tempstr, "increase magic");
		AddPanelString(tempstr, TRUE);
		break;
	case IMISC_ELIXDEX:
		strcpy(tempstr, "increase dexterity");
		AddPanelString(tempstr, TRUE);
		break;
	case IMISC_ELIXVIT:
		strcpy(tempstr, "increase vitality");
		AddPanelString(tempstr, TRUE);
		break;
	case IMISC_ELIXWEAK:
		strcpy(tempstr, "decrease strength");
		AddPanelString(tempstr, TRUE);
		break;
	case IMISC_ELIXDIS:
		strcpy(tempstr, "decrease strength");
		AddPanelString(tempstr, TRUE);
		break;
	case IMISC_ELIXCLUM:
		strcpy(tempstr, "decrease dexterity");
		AddPanelString(tempstr, TRUE);
		break;
	case IMISC_ELIXSICK:
		strcpy(tempstr, "decrease vitality");
		AddPanelString(tempstr, TRUE);
		break;
	case IMISC_REJUV:
		strcpy(tempstr, "recover life and mana");
		AddPanelString(tempstr, TRUE);
		break;
	case IMISC_FULLREJUV:
		strcpy(tempstr, "fully recover life and mana");
		AddPanelString(tempstr, TRUE);
		break;
	}
}

void PrintItemPower(char plidx, ItemStruct *x)
{
	switch (plidx) {
	case IPL_TOHIT:
	case IPL_TOHIT_CURSE:
		sprintf(tempstr, "chance to hit : %+i%%", x->_iPLToHit);
		break;
	case IPL_DAMP:
	case IPL_DAMP_CURSE:
		sprintf(tempstr, "%+i%% damage", x->_iPLDam);
		break;
	case IPL_TOHIT_DAMP:
	case IPL_TOHIT_DAMP_CURSE:
		sprintf(tempstr, "to hit: %+i%%, %+i%% damage", x->_iPLToHit, x->_iPLDam);
		break;
	case IPL_ACP:
	case IPL_ACP_CURSE:
		sprintf(tempstr, "%+i%% armor", x->_iPLAC);
		break;
	case IPL_SETAC:
		sprintf(tempstr, "armor class: %i", x->_iAC);
		break;
	case IPL_AC_CURSE:
		sprintf(tempstr, "armor class: %i", x->_iAC);
		break;
	case IPL_FIRERES:
		if (x->_iPLFR < 75)
			sprintf(tempstr, "Resist Fire : %+i%%", x->_iPLFR);
		if (x->_iPLFR >= 75)
			sprintf(tempstr, "Resist Fire : 75%% MAX");
		break;
	case IPL_LIGHTRES:
		if (x->_iPLLR < 75)
			sprintf(tempstr, "Resist Lightning : %+i%%", x->_iPLLR);
		if (x->_iPLLR >= 75)
			sprintf(tempstr, "Resist Lightning : 75%% MAX");
		break;
	case IPL_MAGICRES:
		if (x->_iPLMR < 75)
			sprintf(tempstr, "Resist Magic : %+i%%", x->_iPLMR);
		if (x->_iPLMR >= 75)
			sprintf(tempstr, "Resist Magic : 75%% MAX");
		break;
	case IPL_ALLRES:
		if (x->_iPLFR < 75)
			sprintf(tempstr, "Resist All : %+i%%", x->_iPLFR);
		if (x->_iPLFR >= 75)
			sprintf(tempstr, "Resist All : 75%% MAX");
		break;
	case IPL_SPLLVLADD:
		if (x->_iSplLvlAdd == 1)
			strcpy(tempstr, "spells are increased 1 level");
		if (x->_iSplLvlAdd == 2)
			strcpy(tempstr, "spells are increased 2 levels");
		if (x->_iSplLvlAdd < 1)
			strcpy(tempstr, "spells are decreased 1 level");
		break;
	case IPL_CHARGES:
		strcpy(tempstr, "Extra charges");
		break;
	case IPL_SPELL:
		sprintf(tempstr, "%i %s charges", x->_iMaxCharges, spelldata[x->_iSpell].sNameText);
		break;
	case IPL_FIREDAM:
		sprintf(tempstr, "Fire hit damage: %i-%i", x->_iFMinDam, x->_iFMaxDam);
		break;
	case IPL_LIGHTDAM:
		sprintf(tempstr, "Lightning hit damage: %i-%i", x->_iLMinDam, x->_iLMaxDam);
		break;
	case IPL_STR:
	case IPL_STR_CURSE:
		sprintf(tempstr, "%+i to strength", x->_iPLStr);
		break;
	case IPL_MAG:
	case IPL_MAG_CURSE:
		sprintf(tempstr, "%+i to magic", x->_iPLMag);
		break;
	case IPL_DEX:
	case IPL_DEX_CURSE:
		sprintf(tempstr, "%+i to dexterity", x->_iPLDex);
		break;
	case IPL_VIT:
	case IPL_VIT_CURSE:
		sprintf(tempstr, "%+i to vitality", x->_iPLVit);
		break;
	case IPL_ATTRIBS:
	case IPL_ATTRIBS_CURSE:
		sprintf(tempstr, "%+i to all attributes", x->_iPLStr);
		break;
	case IPL_GETHIT_CURSE:
	case IPL_GETHIT:
		sprintf(tempstr, "%+i damage from enemies", x->_iPLGetHit);
		break;
	case IPL_LIFE:
	case IPL_LIFE_CURSE:
		sprintf(tempstr, "Hit Points : %+i", x->_iPLHP >> 6);
		break;
	case IPL_MANA:
	case IPL_MANA_CURSE:
		sprintf(tempstr, "Mana : %+i", x->_iPLMana >> 6);
		break;
	case IPL_DUR:
		strcpy(tempstr, "high durability");
		break;
	case IPL_DUR_CURSE:
		strcpy(tempstr, "decreased durability");
		break;
	case IPL_INDESTRUCTIBLE:
		strcpy(tempstr, "indestructible");
		break;
	case IPL_LIGHT:
		sprintf(tempstr, "+%i%% light radius", 10 * x->_iPLLight);
		break;
	case IPL_LIGHT_CURSE:
		sprintf(tempstr, "-%i%% light radius", -10 * x->_iPLLight);
		break;
	case IPL_FIRE_ARROWS:
		sprintf(tempstr, "fire arrows damage: %i-%i", x->_iFMinDam, x->_iFMaxDam);
		break;
	case IPL_LIGHT_ARROWS:
		sprintf(tempstr, "lightning arrows damage %i-%i", x->_iLMinDam, x->_iLMaxDam);
		break;
	case IPL_THORNS:
		strcpy(tempstr, "attacker takes 1-3 damage");
		break;
	case IPL_NOMANA:
		strcpy(tempstr, "user loses all mana");
		break;
	case IPL_NOHEALPLR:
		strcpy(tempstr, "you can't heal");
		break;
	case IPL_ABSHALFTRAP:
		strcpy(tempstr, "absorbs half of trap damage");
		break;
	case IPL_KNOCKBACK:
		strcpy(tempstr, "knocks target back");
		break;
	case IPL_3XDAMVDEM:
		strcpy(tempstr, "+200% damage vs. demons");
		break;
	case IPL_ALLRESZERO:
		strcpy(tempstr, "All Resistance equals 0");
		break;
	case IPL_NOHEALMON:
		strcpy(tempstr, "hit monster doesn't heal");
		break;
	case IPL_STEALMANA:
		if (x->_iFlags & ISPL_STEALMANA_3)
			strcpy(tempstr, "hit steals 3% mana");
		if (x->_iFlags & ISPL_STEALMANA_5)
			strcpy(tempstr, "hit steals 5% mana");
		break;
	case IPL_STEALLIFE:
		if (x->_iFlags & ISPL_STEALLIFE_3)
			strcpy(tempstr, "hit steals 3% life");
		if (x->_iFlags & ISPL_STEALLIFE_5)
			strcpy(tempstr, "hit steals 5% life");
		break;
	case IPL_TARGAC:
		strcpy(tempstr, "damages target's armor");
		break;
	case IPL_FASTATTACK:
		if (x->_iFlags & ISPL_QUICKATTACK)
			strcpy(tempstr, "quick attack");
		if (x->_iFlags & ISPL_FASTATTACK)
			strcpy(tempstr, "fast attack");
		if (x->_iFlags & ISPL_FASTERATTACK)
			strcpy(tempstr, "faster attack");
		if (x->_iFlags & ISPL_FASTESTATTACK)
			strcpy(tempstr, "fastest attack");
		break;
	case IPL_FASTRECOVER:
		if (x->_iFlags & ISPL_FASTRECOVER)
			strcpy(tempstr, "fast hit recovery");
		if (x->_iFlags & ISPL_FASTERRECOVER)
			strcpy(tempstr, "faster hit recovery");
		if (x->_iFlags & ISPL_FASTESTRECOVER)
			strcpy(tempstr, "fastest hit recovery");
		break;
	case IPL_FASTBLOCK:
		strcpy(tempstr, "fast block");
		break;
	case IPL_DAMMOD:
		sprintf(tempstr, "adds %i points to damage", x->_iPLDamMod);
		break;
	case IPL_RNDARROWVEL:
		strcpy(tempstr, "fires random speed arrows");
		break;
	case IPL_SETDAM:
		sprintf(tempstr, "unusual item damage");
		break;
	case IPL_SETDUR:
		strcpy(tempstr, "altered durability");
		break;
	case IPL_FASTSWING:
		strcpy(tempstr, "Faster attack swing");
		break;
	case IPL_ONEHAND:
		strcpy(tempstr, "one handed sword");
		break;
	case IPL_DRAINLIFE:
		strcpy(tempstr, "constantly lose hit points");
		break;
	case IPL_RNDSTEALLIFE:
		strcpy(tempstr, "life stealing");
		break;
	case IPL_NOMINSTR:
		strcpy(tempstr, "no strength requirement");
		break;
	case IPL_INFRAVISION:
		strcpy(tempstr, "see with infravision");
		break;
	case IPL_INVCURS:
		strcpy(tempstr, " ");
		break;
	case IPL_ADDACLIFE:
		strcpy(tempstr, "Armor class added to life");
		break;
	case IPL_ADDMANAAC:
		strcpy(tempstr, "10% of mana added to armor");
		break;
	case IPL_FIRERESCLVL:
		if (x->_iPLFR <= 0)
			sprintf(tempstr, " ");
		else if (x->_iPLFR >= 1)
			sprintf(tempstr, "Resist Fire : %+i%%", x->_iPLFR);
		break;
	default:
		strcpy(tempstr, "Another ability (NW)");
		break;
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
		PrintUString({ 0 + RIGHT_PANEL - SPANEL_WIDTH, 2 }, TRUE, UniqueItemList[uid].UIName, 3);
		DrawULine(5);
		PrintItemPower(UniqueItemList[uid].UIPower1, &curruitem);
		y = 6 - UniqueItemList[uid].UINumPL + 8;
		PrintUString({ 0 + RIGHT_PANEL - SPANEL_WIDTH, y }, TRUE, tempstr, 0);
		if (UniqueItemList[uid].UINumPL > 1) {
			PrintItemPower(UniqueItemList[uid].UIPower2, &curruitem);
			PrintUString({ 0 + RIGHT_PANEL - SPANEL_WIDTH, y + 2 }, TRUE, tempstr, 0);
		}
		if (UniqueItemList[uid].UINumPL > 2) {
			PrintItemPower(UniqueItemList[uid].UIPower3, &curruitem);
			PrintUString({ 0 + RIGHT_PANEL - SPANEL_WIDTH, y + 4 }, TRUE, tempstr, 0);
		}
		if (UniqueItemList[uid].UINumPL > 3) {
			PrintItemPower(UniqueItemList[uid].UIPower4, &curruitem);
			PrintUString({ 0 + RIGHT_PANEL - SPANEL_WIDTH, y + 6 }, TRUE, tempstr, 0);
		}
		if (UniqueItemList[uid].UINumPL > 4) {
			PrintItemPower(UniqueItemList[uid].UIPower5, &curruitem);
			PrintUString({ 0 + RIGHT_PANEL - SPANEL_WIDTH, y + 8 }, TRUE, tempstr, 0);
		}
		if (UniqueItemList[uid].UINumPL > 5) {
			PrintItemPower(UniqueItemList[uid].UIPower6, &curruitem);
			PrintUString({ 0 + RIGHT_PANEL - SPANEL_WIDTH, y + 10 }, TRUE, tempstr, 0);
		}
	}
}

void PrintItemMisc(ItemStruct *x)
{
	if (x->_iMiscId == IMISC_SCROLL) {
		strcpy(tempstr, "Right-click to read");
		AddPanelString(tempstr, TRUE);
	}
	if (x->_iMiscId == IMISC_SCROLLT) {
		strcpy(tempstr, "Right-click to read, then");
		AddPanelString(tempstr, TRUE);
		strcpy(tempstr, "left-click to target");
		AddPanelString(tempstr, TRUE);
	}
	if (x->_iMiscId >= IMISC_USEFIRST && x->_iMiscId <= IMISC_USELAST) {
		PrintItemOil(x->_iMiscId);
		strcpy(tempstr, "Right click to use");
		AddPanelString(tempstr, TRUE);
	}
	if (x->_iMiscId == IMISC_BOOK) {
		strcpy(tempstr, "Right click to read");
		AddPanelString(tempstr, TRUE);
	}
	if (x->_iMiscId == IMISC_MAPOFDOOM) {
		strcpy(tempstr, "Right click to view");
		AddPanelString(tempstr, TRUE);
	}
	if (x->_iMiscId == IMISC_EAR) {
		sprintf(tempstr, "Level : %i", x->_ivalue);
		AddPanelString(tempstr, TRUE);
	}
}

void PrintItemDetails(ItemStruct *x)
{
	if (x->_iClass == ICLASS_WEAPON) {
		if (x->_iMaxDur == DUR_INDESTRUCTIBLE)
			sprintf(tempstr, "damage: %i-%i  Indestructible", x->_iMinDam, x->_iMaxDam);
		else
			sprintf(tempstr, "damage: %i-%i  Dur: %i/%i", x->_iMinDam, x->_iMaxDam, x->_iDurability, x->_iMaxDur);
		AddPanelString(tempstr, TRUE);
	}
	if (x->_iClass == ICLASS_ARMOR) {
		if (x->_iMaxDur == DUR_INDESTRUCTIBLE)
			sprintf(tempstr, "armor: %i  Indestructible", x->_iAC);
		else
			sprintf(tempstr, "armor: %i  Dur: %i/%i", x->_iAC, x->_iDurability, x->_iMaxDur);
		AddPanelString(tempstr, TRUE);
	}
	if (x->_iMiscId == IMISC_STAFF && x->_iMaxCharges) {
		sprintf(tempstr, "dam: %i-%i  Dur: %i/%i", x->_iMinDam, x->_iMaxDam, x->_iDurability, x->_iMaxDur);
		sprintf(tempstr, "Charges: %i/%i", x->_iCharges, x->_iMaxCharges);
		AddPanelString(tempstr, TRUE);
	}
	if (x->_iPrePower != -1) {
		PrintItemPower(x->_iPrePower, x);
		AddPanelString(tempstr, TRUE);
	}
	if (x->_iSufPower != -1) {
		PrintItemPower(x->_iSufPower, x);
		AddPanelString(tempstr, TRUE);
	}
	if (x->_iMagical == ITEM_QUALITY_UNIQUE) {
		AddPanelString("unique item", TRUE);
		uitemflag = TRUE;
		curruitem = *x;
	}
	PrintItemMisc(x);
	if (x->_iMinMag + x->_iMinDex + x->_iMinStr) {
		strcpy(tempstr, "Required:");
		if (x->_iMinStr)
			sprintf(tempstr, "%s %i Str", tempstr, x->_iMinStr);
		if (x->_iMinMag)
			sprintf(tempstr, "%s %i Mag", tempstr, x->_iMinMag);
		if (x->_iMinDex)
			sprintf(tempstr, "%s %i Dex", tempstr, x->_iMinDex);
		AddPanelString(tempstr, TRUE);
	}
	pinfoflag = TRUE;
}

void PrintItemDur(ItemStruct *x)
{
	if (x->_iClass == ICLASS_WEAPON) {
		if (x->_iMaxDur == DUR_INDESTRUCTIBLE)
			sprintf(tempstr, "damage: %i-%i  Indestructible", x->_iMinDam, x->_iMaxDam);
		else
			sprintf(tempstr, "damage: %i-%i  Dur: %i/%i", x->_iMinDam, x->_iMaxDam, x->_iDurability, x->_iMaxDur);
		AddPanelString(tempstr, TRUE);
		if (x->_iMiscId == IMISC_STAFF && x->_iMaxCharges) {
			sprintf(tempstr, "Charges: %i/%i", x->_iCharges, x->_iMaxCharges);
			AddPanelString(tempstr, TRUE);
		}
		if (x->_iMagical != ITEM_QUALITY_NORMAL)
			AddPanelString("Not Identified", TRUE);
	}
	if (x->_iClass == ICLASS_ARMOR) {
		if (x->_iMaxDur == DUR_INDESTRUCTIBLE)
			sprintf(tempstr, "armor: %i  Indestructible", x->_iAC);
		else
			sprintf(tempstr, "armor: %i  Dur: %i/%i", x->_iAC, x->_iDurability, x->_iMaxDur);
		AddPanelString(tempstr, TRUE);
		if (x->_iMagical != ITEM_QUALITY_NORMAL)
			AddPanelString("Not Identified", TRUE);
		if (x->_iMiscId == IMISC_STAFF && x->_iMaxCharges) {
			sprintf(tempstr, "Charges: %i/%i", x->_iCharges, x->_iMaxCharges);
			AddPanelString(tempstr, TRUE);
		}
	}
	if (x->_itype == ITYPE_RING || x->_itype == ITYPE_AMULET)
		AddPanelString("Not Identified", TRUE);
	PrintItemMisc(x);
	if (x->_iMinMag + x->_iMinDex + x->_iMinStr) {
		strcpy(tempstr, "Required:");
		if (x->_iMinStr)
			sprintf(tempstr, "%s %i Str", tempstr, x->_iMinStr);
		if (x->_iMinMag)
			sprintf(tempstr, "%s %i Mag", tempstr, x->_iMinMag);
		if (x->_iMinDex)
			sprintf(tempstr, "%s %i Dex", tempstr, x->_iMinDex);
		AddPanelString(tempstr, TRUE);
	}
	pinfoflag = TRUE;
}

bool StoreStatOk(ItemStruct *h)
{
	bool sf;

	sf = TRUE;
	if (myplr().data._pStrength < h->_iMinStr)
		sf = false;
	if (myplr().data._pMagic < h->_iMinMag)
		sf = false;
	if (myplr().data._pDexterity < h->_iMinDex)
		sf = false;

	return sf;
}

bool SmithItemOk(int i)
{
	bool rv;

	rv = TRUE;
	if (AllItemsList[i].itype == ITYPE_MISC)
		rv = false;
	if (AllItemsList[i].itype == ITYPE_GOLD)
		rv = false;
	if (AllItemsList[i].itype == ITYPE_MEAT)
		rv = false;
	if (AllItemsList[i].itype == ITYPE_STAFF)
		rv = false;
	if (AllItemsList[i].itype == ITYPE_RING)
		rv = false;
	if (AllItemsList[i].itype == ITYPE_AMULET)
		rv = false;

	return rv;
}

int RndSmithItem(int lvl)
{
	int i, ri;
	int ril[512];

	ri = 0;
	for (i = 1; AllItemsList[i].iLoc != ILOC_INVALID; i++) {
		if (AllItemsList[i].iRnd && SmithItemOk(i) && lvl >= AllItemsList[i].iMinMLvl) {
			ril[ri] = i;
			ri++;
			if (AllItemsList[i].iRnd == 2) {
				ril[ri] = i;
				ri++;
			}
		}
	}

	return ril[random_(50, ri)] + 1;
}

void BubbleSwapItem(ItemStruct *a, ItemStruct *b)
{
	ItemStruct h;

	h = *a;
	*a = *b;
	*b = h;
}

void SortSmith()
{
	int j, k;
	bool sorted;

	j = 0;
	while (smithitem[j + 1]._itype != ITYPE_NONE) {
		j++;
	}

	sorted = false;
	while (j > 0 && !sorted) {
		sorted = TRUE;
		for (k = 0; k < j; k++) {
			if (smithitem[k].IDidx > smithitem[k + 1].IDidx) {
				BubbleSwapItem(&smithitem[k], &smithitem[k + 1]);
				sorted = false;
			}
		}
		j--;
	}
}

void SpawnSmith(int lvl)
{
	int i, iCnt, idata;

	iCnt = random_(50, SMITH_ITEMS - 10) + 10;
	for (i = 0; i < iCnt; i++) {
		do {
			item[0]._iSeed = GetRndSeed();
			SetRndSeed(item[0]._iSeed);
			idata = RndSmithItem(lvl) - 1;
			GetItemAttrs(0, idata, lvl);
		} while (item[0]._iIvalue > SMITH_MAX_VALUE);
		smithitem[i] = item[0];
		smithitem[i]._iCreateInfo = lvl | 0x400;
		smithitem[i]._iIdentified = TRUE;
		smithitem[i]._iStatFlag = StoreStatOk(&smithitem[i]);
	}
	for (i = iCnt; i < SMITH_ITEMS; i++)
		smithitem[i]._itype = ITYPE_NONE;

	SortSmith();
}

bool PremiumItemOk(int i)
{
	bool rv = TRUE;
	if (AllItemsList[i].itype == ITYPE_MISC)
		rv = false;
	if (AllItemsList[i].itype == ITYPE_GOLD)
		rv = false;
	if (AllItemsList[i].itype == ITYPE_MEAT)
		rv = false;
	if (AllItemsList[i].itype == ITYPE_STAFF)
		rv = false;

	if (plr.isMultiplayer()) {
		if (AllItemsList[i].itype == ITYPE_RING)
			rv = false;
		if (AllItemsList[i].itype == ITYPE_AMULET)
			rv = false;
	}
	return rv;
}

int RndPremiumItem(int minlvl, int maxlvl)
{
	int i, ri;
	int ril[512];

	ri = 0;
	for (i = 1; AllItemsList[i].iLoc != ILOC_INVALID; i++) {
		if (AllItemsList[i].iRnd) {
			if (PremiumItemOk(i)) {
				if (AllItemsList[i].iMinMLvl >= minlvl && AllItemsList[i].iMinMLvl <= maxlvl) {
					ril[ri] = i;
					ri++;
				}
			}
		}
	}
	return ril[random_(50, ri)] + 1;
}

void SpawnOnePremium(int i, int plvl)
{
	int itype;
	ItemStruct holditem;

	holditem = item[0];
	if (plvl > 30)
		plvl = 30;
	if (plvl < 1)
		plvl = 1;
	do {
		item[0]._iSeed = GetRndSeed();
		SetRndSeed(item[0]._iSeed);
		itype = RndPremiumItem(plvl >> 2, plvl) - 1;
		GetItemAttrs(0, itype, plvl);
		GetItemBonus(0, itype, plvl >> 1, plvl, TRUE);
	} while (item[0]._iIvalue > SMITH_MAX_PREMIUM_VALUE);
	premiumitem[i] = item[0];
	premiumitem[i]._iCreateInfo = plvl | 0x800;
	premiumitem[i]._iIdentified = TRUE;
	premiumitem[i]._iStatFlag = StoreStatOk(&premiumitem[i]);
	item[0] = holditem;
}

void SpawnPremium(int lvl)
{
	int i;

	if (numpremium < SMITH_PREMIUM_ITEMS) {
		for (i = 0; i < SMITH_PREMIUM_ITEMS; i++) {
			if (premiumitem[i]._itype == ITYPE_NONE)
				SpawnOnePremium(i, premiumlevel + premiumlvladd[i]);
		}
		numpremium = SMITH_PREMIUM_ITEMS;
	}
	while (premiumlevel < lvl) {
		premiumlevel++;
		premiumitem[0] = premiumitem[2];
		premiumitem[1] = premiumitem[3];
		premiumitem[2] = premiumitem[4];
		SpawnOnePremium(3, premiumlevel + premiumlvladd[3]);
		premiumitem[4] = premiumitem[5];
		SpawnOnePremium(5, premiumlevel + premiumlvladd[5]);
	}
}

bool WitchItemOk(int i)
{
	bool rv = false;
	if (AllItemsList[i].itype == ITYPE_MISC)
		rv = TRUE;
	if (AllItemsList[i].itype == ITYPE_STAFF)
		rv = TRUE;
	if (AllItemsList[i].iMiscId == IMISC_MANA)
		rv = false;
	if (AllItemsList[i].iMiscId == IMISC_FULLMANA)
		rv = false;
	if (AllItemsList[i].iSpell == SPL_TOWN)
		rv = false;
	if (AllItemsList[i].iMiscId == IMISC_FULLHEAL)
		rv = false;
	if (AllItemsList[i].iMiscId == IMISC_HEAL)
		rv = false;
	if (AllItemsList[i].iSpell == SPL_RESURRECT && plr.isSingleplayer())
		rv = false;
	if (AllItemsList[i].iSpell == SPL_HEALOTHER && plr.isSingleplayer())
		rv = false;

	return rv;
}

int RndWitchItem(int lvl)
{
	int i, ri;
	int ril[512];

	ri = 0;
	for (i = 1; AllItemsList[i].iLoc != ILOC_INVALID; i++) {
		if (AllItemsList[i].iRnd && WitchItemOk(i) && lvl >= AllItemsList[i].iMinMLvl) {
			ril[ri] = i;
			ri++;
		}
	}

	return ril[random_(51, ri)] + 1;
}

void SortWitch()
{
	int j, k;
	bool sorted;

	j = 3;
	while (witchitem[j + 1]._itype != ITYPE_NONE) {
		j++;
	}

	sorted = false;
	while (j > 3 && !sorted) {
		sorted = TRUE;
		for (k = 3; k < j; k++) {
			if (witchitem[k].IDidx > witchitem[k + 1].IDidx) {
				BubbleSwapItem(&witchitem[k], &witchitem[k + 1]);
				sorted = false;
			}
		}
		j--;
	}
}

void WitchBookLevel(int ii)
{
	int slvl;

	if (witchitem[ii]._iMiscId == IMISC_BOOK) {
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

void SpawnWitch(int lvl)
{
	int i, iCnt;
	int idata, maxlvl;

	GetItemAttrs(0, IDI_MANA, 1);

	witchitem[0] = item[0];
	witchitem[0]._iCreateInfo = lvl;
	witchitem[0]._iStatFlag = TRUE;
	GetItemAttrs(0, IDI_FULLMANA, 1);
	witchitem[1] = item[0];
	witchitem[1]._iCreateInfo = lvl;
	witchitem[1]._iStatFlag = TRUE;
	GetItemAttrs(0, IDI_PORTAL, 1);
	witchitem[2] = item[0];
	witchitem[2]._iCreateInfo = lvl;
	witchitem[2]._iStatFlag = TRUE;
	iCnt = random_(51, 8) + 10;

	for (i = 3; i < iCnt; i++) {
		do {
			item[0]._iSeed = GetRndSeed();
			SetRndSeed(item[0]._iSeed);
			idata = RndWitchItem(lvl) - 1;
			GetItemAttrs(0, idata, lvl);
			maxlvl = -1;
			if (random_(51, 100) <= 5)
				maxlvl = 2 * lvl;
			if (maxlvl == -1 && item[0]._iMiscId == IMISC_STAFF)
				maxlvl = 2 * lvl;
			if (maxlvl != -1)
				GetItemBonus(0, idata, maxlvl >> 1, maxlvl, TRUE);
		} while (item[0]._iIvalue > 140000);
		witchitem[i] = item[0];
		witchitem[i]._iCreateInfo = lvl | 0x2000;
		witchitem[i]._iIdentified = TRUE;
		WitchBookLevel(i);
		witchitem[i]._iStatFlag = StoreStatOk(&witchitem[i]);
	}

	for (i = iCnt; i < 20; i++)
		witchitem[i]._itype = ITYPE_NONE;

	SortWitch();
}

int RndBoyItem(int lvl)
{
	int i, ri;
	int ril[512];

	ri = 0;
	for (i = 1; AllItemsList[i].iLoc != ILOC_INVALID; i++) {
		if (AllItemsList[i].iRnd && PremiumItemOk(i) && lvl >= AllItemsList[i].iMinMLvl) {
			ril[ri] = i;
			ri++;
		}
	}

	return ril[random_(49, ri)] + 1;
}

void SpawnBoy(int lvl)
{
	int itype;

	if (boylevel<lvl>> 1 || boyitem._itype == ITYPE_NONE) {
		do {
			item[0]._iSeed = GetRndSeed();
			SetRndSeed(item[0]._iSeed);
			itype = RndBoyItem(lvl) - 1;
			GetItemAttrs(0, itype, lvl);
			GetItemBonus(0, itype, lvl, 2 * lvl, TRUE);
		} while (item[0]._iIvalue > 90000);
		boyitem = item[0];
		boyitem._iCreateInfo = lvl | 0x1000;
		boyitem._iIdentified = TRUE;
		boyitem._iStatFlag = StoreStatOk(&boyitem);
		boylevel = lvl >> 1;
	}
}

bool HealerItemOk(int i)
{
	bool result;

	result = false;
	if (AllItemsList[i].itype != ITYPE_MISC)
		return false;

	if (AllItemsList[i].iMiscId == IMISC_SCROLL && AllItemsList[i].iSpell == SPL_HEAL)
		result = TRUE;
	if (AllItemsList[i].iMiscId == IMISC_SCROLLT && AllItemsList[i].iSpell == SPL_RESURRECT && plr.isMultiplayer())
		result = false;
	if (AllItemsList[i].iMiscId == IMISC_SCROLLT && AllItemsList[i].iSpell == SPL_HEALOTHER && plr.isMultiplayer())
		result = TRUE;

	if (plr.isSingleplayer()) {
		if (AllItemsList[i].iMiscId == IMISC_ELIXSTR)
			result = TRUE;
		if (AllItemsList[i].iMiscId == IMISC_ELIXMAG)
			result = TRUE;
		if (AllItemsList[i].iMiscId == IMISC_ELIXDEX)
			result = TRUE;
		if (AllItemsList[i].iMiscId == IMISC_ELIXVIT)
			result = TRUE;
	}

	if (AllItemsList[i].iMiscId == IMISC_FULLHEAL) // BUGFIX this is a duplicate with the wrong case
		result = TRUE;

	if (AllItemsList[i].iMiscId == IMISC_REJUV)
		result = TRUE;
	if (AllItemsList[i].iMiscId == IMISC_FULLREJUV)
		result = TRUE;
	if (AllItemsList[i].iMiscId == IMISC_HEAL)
		result = false;
	if (AllItemsList[i].iMiscId == IMISC_FULLHEAL)
		result = false;
	if (AllItemsList[i].iMiscId == IMISC_MANA)
		result = false;
	if (AllItemsList[i].iMiscId == IMISC_FULLMANA)
		result = false;

	return result;
}

int RndHealerItem(int lvl)
{
	int i, ri;
	int ril[512];

	ri = 0;
	for (i = 1; AllItemsList[i].iLoc != ILOC_INVALID; i++) {
		if (AllItemsList[i].iRnd && HealerItemOk(i) && lvl >= AllItemsList[i].iMinMLvl) {
			ril[ri] = i;
			ri++;
		}
	}

	return ril[random_(50, ri)] + 1;
}

void SortHealer()
{
	int j, k;
	bool sorted;

	j = 2;
	while (healitem[j + 1]._itype != ITYPE_NONE) {
		j++;
	}

	sorted = false;
	while (j > 2 && !sorted) {
		sorted = TRUE;
		for (k = 2; k < j; k++) {
			if (healitem[k].IDidx > healitem[k + 1].IDidx) {
				BubbleSwapItem(&healitem[k], &healitem[k + 1]);
				sorted = false;
			}
		}
		j--;
	}
}

void SpawnHealer(int lvl)
{
	int i, nsi, srnd, itype;

	GetItemAttrs(0, IDI_HEAL, 1);
	healitem[0] = item[0];
	healitem[0]._iCreateInfo = lvl;
	healitem[0]._iStatFlag = TRUE;

	GetItemAttrs(0, IDI_FULLHEAL, 1);
	healitem[1] = item[0];
	healitem[1]._iCreateInfo = lvl;
	healitem[1]._iStatFlag = TRUE;

	if (plr.isMultiplayer()) {
		GetItemAttrs(0, IDI_RESURRECT, 1);
		healitem[2] = item[0];
		healitem[2]._iCreateInfo = lvl;
		healitem[2]._iStatFlag = TRUE;

		srnd = 3;
	} else {
		srnd = 2;
	}
	nsi = random_(50, 8) + 10;
	for (i = srnd; i < nsi; i++) {
		item[0]._iSeed = GetRndSeed();
		SetRndSeed(item[0]._iSeed);
		itype = RndHealerItem(lvl) - 1;
		GetItemAttrs(0, itype, lvl);
		healitem[i] = item[0];
		healitem[i]._iCreateInfo = lvl | 0x4000;
		healitem[i]._iIdentified = TRUE;
		healitem[i]._iStatFlag = StoreStatOk(&healitem[i]);
	}
	for (i = nsi; i < 20; i++) {
		healitem[i]._itype = ITYPE_NONE;
	}
	SortHealer();
}

void SpawnStoreGold()
{
	GetItemAttrs(0, IDI_GOLD, 1);
	golditem = item[0];
	golditem._iStatFlag = TRUE;
}

void RecalcStoreStats()
{
	int i;

	for (i = 0; i < SMITH_ITEMS; i++) {
		if (smithitem[i]._itype != ITYPE_NONE) {
			smithitem[i]._iStatFlag = StoreStatOk(&smithitem[i]);
		}
	}
	for (i = 0; i < SMITH_PREMIUM_ITEMS; i++) {
		if (premiumitem[i]._itype != ITYPE_NONE) {
			premiumitem[i]._iStatFlag = StoreStatOk(&premiumitem[i]);
		}
	}
	for (i = 0; i < 20; i++) {
		if (witchitem[i]._itype != ITYPE_NONE) {
			witchitem[i]._iStatFlag = StoreStatOk(&witchitem[i]);
		}
	}
	for (i = 0; i < 20; i++) {
		if (healitem[i]._itype != ITYPE_NONE) {
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
	int idx = RndTypeItems(ITYPE_MISC, IMISC_BOOK);
	Item &item = items.createNewItem();
	item.GetSuperItemSpace(pos);
	while (!done) {
		item.SetupAllItems(idx, GetRndSeed(), 2 * lvl.currlevel, 1, TRUE, false, delta);
		if (item._iMiscId == IMISC_BOOK && item._iSpell == ispell)
			done = TRUE;
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
	int idx = RndTypeItems(imisc, IMISC_NONE);
	while (!done) {
		item.SetupAllItems(idx, GetRndSeed(), 2 * lvl.currlevel, 1, TRUE, false, delta);
		if (item._iCurs == icurs)
			done = TRUE;
		else
			idx = RndTypeItems(imisc, IMISC_NONE);
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
	int idx = RndTypeItems(imisc, IMISC_NONE);
	while (!done) {
		item.SetupAllItems(idx, GetRndSeed(), 2 * lvl.currlevel, 1, TRUE, false, delta);
		if (item._iCurs == icurs)
			done = TRUE;
		else
			idx = RndTypeItems(imisc, IMISC_NONE);
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

	return TRUE;
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
