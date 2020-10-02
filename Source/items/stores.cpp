#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

int stextup;
int storenumh;
int stextlhold;

int stextshold;

uint8_t *pSTextBoxCels;

int talker;
STextStruct stext[24];
char stextsize;

int stextsmax;
ItemStruct storehold[48];
int gossipstart;
bool stextscrl;


ItemStruct golditem;
char storehidx[48];
uint8_t *pSTextSlidCels;
int stextvhold;
int stextsel;
char stextscrldbtn;
int gossipend;
uint8_t *pSPentSpn2Cels;
uint8_t PentSpn2Frame;
DWORD PentSpn2Tick;
int stextsval;


int stextdown;
char stextscrlubtn;
char stextflag;

/** Maps from towner IDs to NPC names. */
char *talkname[9] = {
	"Griswold",
	"Pepin",
	"",
	"Ogden",
	"Cain",
	"Farnham",
	"Adria",
	"Gillian",
	"Wirt"
};

void InitStores()
{
	int i;

	pSTextBoxCels = LoadFileInMem("Data\\TextBox2.CEL", NULL);
	pSPentSpn2Cels = LoadFileInMem("Data\\PentSpn2.CEL", NULL);
	pSTextSlidCels = LoadFileInMem("Data\\TextSlid.CEL", NULL);
	ClearSText(0, 24);
	stextflag = STORE_NONE;
	PentSpn2Frame = 1;
	stextsize = false;
	stextscrl = false;
	numpremium = 0;
	premiumlevel = 1;

	for (i = 0; i < 6; i++)
		premiumitem[i]._itype = ITYPE_NONE;

	boyitem._itype = ITYPE_NONE;
	boylevel = 0;
}

void PentSpn2Spin()
{
	DWORD ticks = SDL_GetTicks();
	if (ticks - PentSpn2Tick > 50) {
		PentSpn2Frame = (PentSpn2Frame & 7) + 1;
		PentSpn2Tick = ticks;
	}
}

void SetupTownStores()
{
	int i, l;

	SetRndSeed(glSeedTbl[lvl.currlevel] * SDL_GetTicks());
	if (plr.isSingleplayer()) {
		l = 0;
		for (i = 0; i < NUMLEVELS; i++) {
			if (myplr().data._pLvlVisited[i])
				l = i;
		}
	} else {
		l = myplr().data._pLevel >> 1;
	}
	l += 2;
	if (l < 6)
		l = 6;
	if (l > 16)
		l = 16;
	SpawnStoreGold();
	SpawnSmith(l);
	SpawnWitch(l);
	SpawnHealer(l);
	SpawnBoy(myplr().data._pLevel);
	SpawnPremium(myplr().data._pLevel);
}

void FreeStoreMem()
{
	MemFreeDbg(pSTextBoxCels);
	MemFreeDbg(pSPentSpn2Cels);
	MemFreeDbg(pSTextSlidCels);
}

void StoreAutoPlace()
{
	int i, w, h, idx;

	SetICursor(myplr().data.HoldItem._iCurs + CURSOR_FIRSTITEM);
	w = icursW28;
	h = icursH28;
	bool done = false;
	if (w == 1 && h == 1) {
		idx = myplr().data.HoldItem.IDidx;
		if (myplr().data.HoldItem._iStatFlag && AllItemsList[idx].iUsable) {
			for (i = 0; i < 8 && !done; i++) { // try to place in speed list
				if (myplr().data.SpdList[i]._itype == ITYPE_NONE) {
					myplr().data.SpdList[i] = myplr().data.HoldItem;
					done = true;
				}
			}
		}
		for (i = 30; i <= 39 && !done; i++) {
			done = myplr().inventory.AutoPlace(i, { w, h }, true);
		}
		for (i = 20; i <= 29 && !done; i++) {
			done = myplr().inventory.AutoPlace(i, { w, h }, true);
		}
		for (i = 10; i <= 19 && !done; i++) {
			done = myplr().inventory.AutoPlace(i, { w, h }, true);
		}
		for (i = 0; i <= 9 && !done; i++) {
			done = myplr().inventory.AutoPlace(i, { w, h }, true);
		}
	}
	if (w == 1 && h == 2) {
		for (i = 29; i >= 20 && !done; i--) {
			done = myplr().inventory.AutoPlace(i, { w, h }, true);
		}
		for (i = 9; i >= 0 && !done; i--) {
			done = myplr().inventory.AutoPlace(i, { w, h }, true);
		}
		for (i = 19; i >= 10 && !done; i--) {
			done = myplr().inventory.AutoPlace(i, { w, h }, true);
		}
	}
	if (w == 1 && h == 3) {
		for (i = 0; i < 20 && !done; i++) {
			done = myplr().inventory.AutoPlace(i, { w, h }, true);
		}
	}
	if (w == 2 && h == 2) {
		for (i = 0; i < 10 && !done; i++) {
			done = myplr().inventory.AutoPlace(AP2x2Tbl[i], { w, h }, true);
		}
		for (i = 21; i < 29 && !done; i += 2) {
			done = myplr().inventory.AutoPlace(i, { w, h }, true);
		}
		for (i = 1; i < 9 && !done; i += 2) {
			done = myplr().inventory.AutoPlace(i, { w, h }, true);
		}
		for (i = 10; i < 19 && !done; i++) {
			done = myplr().inventory.AutoPlace(i, { w, h }, true);
		}
	}
	if (w == 2 && h == 3) {
		for (i = 0; i < 9 && !done; i++) {
			done = myplr().inventory.AutoPlace(i, { w, h }, true);
		}
		for (i = 10; i < 19 && !done; i++) {
			done = myplr().inventory.AutoPlace(i, { w, h }, true);
		}
	}
}



void PrintStoreItem(ItemStruct *x, int l, char iclr)
{
	char sstr[128];
	char str, dex;
	uint8_t mag;

	sstr[0] = '\0';
	if (x->_iIdentified) {
		if (x->_iMagical != ITEM_QUALITY_UNIQUE) {
			if (x->_iPrePower != -1) {
				PrintItemPower(x->_iPrePower, x);
				strcat(sstr, tempstr);
			}
		}
		if (x->_iSufPower != -1) {
			PrintItemPower(x->_iSufPower, x);
			if (sstr[0])
				strcat(sstr, ",  ");
			strcat(sstr, tempstr);
		}
	}
	if (x->_iMiscId == IMISC_STAFF && x->_iMaxCharges) {
		sprintf(tempstr, "Charges: %i/%i", x->_iCharges, x->_iMaxCharges);
		if (sstr[0])
			strcat(sstr, ",  ");
		strcat(sstr, tempstr);
	}
	if (sstr[0]) {
		AddSText(40, l, false, sstr, iclr, false);
		l++;
	}
	sstr[0] = '\0';
	if (x->_iClass == ICLASS_WEAPON)
		sprintf(sstr, "Damage: %i-%i  ", x->_iMinDam, x->_iMaxDam);
	if (x->_iClass == ICLASS_ARMOR)
		sprintf(sstr, "Armor: %i  ", x->_iAC);
	if (x->_iMaxDur != DUR_INDESTRUCTIBLE && x->_iMaxDur) {
		sprintf(tempstr, "Dur: %i/%i,  ", x->_iDurability, x->_iMaxDur);
		strcat(sstr, tempstr);
	} else {
		strcat(sstr, "Indestructible,  ");
	}
	if (x->_itype == ITYPE_MISC)
		sstr[0] = '\0';
	str = x->_iMinStr;
	dex = x->_iMinDex;
	mag = x->_iMinMag;
	if (!(str + mag + dex)) {
		strcat(sstr, "No required attributes");
	} else {
		strcpy(tempstr, "Required:");
		if (x->_iMinStr)
			sprintf(tempstr, "%s %i Str", tempstr, x->_iMinStr);
		if (x->_iMinMag)
			sprintf(tempstr, "%s %i Mag", tempstr, x->_iMinMag);
		if (x->_iMinDex)
			sprintf(tempstr, "%s %i Dex", tempstr, x->_iMinDex);
		strcat(sstr, tempstr);
	}
	AddSText(40, l++, false, sstr, iclr, false);
	if (x->_iMagical == ITEM_QUALITY_UNIQUE) {
		if (x->_iIdentified)
			AddSText(40, l, false, "Unique Item", iclr, false);
	}
}

void AddStoreHoldRepair(ItemStruct *itm, int i)
{
	ItemStruct *item;
	int v;

	item = &storehold[storenumh];
	storehold[storenumh] = *itm;
	if (item->_iMagical != ITEM_QUALITY_NORMAL && item->_iIdentified)
		item->_ivalue = 30 * item->_iIvalue / 100;
	v = item->_ivalue * (100 * (item->_iMaxDur - item->_iDurability) / item->_iMaxDur) / 100;
	if (!v) {
		if (item->_iMagical != ITEM_QUALITY_NORMAL && item->_iIdentified)
			return;
		v = 1;
	}
	if (v > 1)
		v >>= 1;
	item->_iIvalue = v;
	item->_ivalue = v;
	storehidx[storenumh] = i;
	storenumh++;
}







void S_StartNoMoney()
{
	StartStore(stextshold);
	stextscrl = false;
	stextsize = true;
	ClearSText(5, 23);
	AddSText(0, 14, true, "You do not have enough gold", COL_WHITE, true);
}

void S_StartNoRoom()
{
	StartStore(stextshold);
	stextscrl = false;
	ClearSText(5, 23);
	AddSText(0, 14, true, "You do not have enough room in inventory", COL_WHITE, true);
}

void S_StartConfirm()
{
	bool idprint;
	char iclr;

	StartStore(stextshold);
	stextscrl = false;
	ClearSText(5, 23);
	iclr = COL_WHITE;

	if (myplr().data.HoldItem._iMagical != ITEM_QUALITY_NORMAL)
		iclr = COL_BLUE;
	if (!myplr().data.HoldItem._iStatFlag)
		iclr = COL_RED;

	idprint = myplr().data.HoldItem._iMagical != ITEM_QUALITY_NORMAL;

	if (stextshold == STORE_SIDENTIFY)
		idprint = false;
	if (myplr().data.HoldItem._iMagical != ITEM_QUALITY_NORMAL && !myplr().data.HoldItem._iIdentified) {
		if (stextshold == STORE_SSELL)
			idprint = false;
		if (stextshold == STORE_WSELL)
			idprint = false;
		if (stextshold == STORE_SREPAIR)
			idprint = false;
		if (stextshold == STORE_WRECHARGE)
			idprint = false;
	}
	if (idprint)
		AddSText(20, 8, false, myplr().data.HoldItem._iIName, iclr, false);
	else
		AddSText(20, 8, false, myplr().data.HoldItem._iName, iclr, false);

	AddSTextVal(8, myplr().data.HoldItem._iIvalue);
	PrintStoreItem(&myplr().data.HoldItem, 9, iclr);

	switch (stextshold) {
	case STORE_BBOY:
		strcpy(tempstr, "Do we have a deal?");
		break;
	case STORE_SIDENTIFY:
		strcpy(tempstr, "Are you sure you want to identify this item?");
		break;
	case STORE_HBUY:
	case STORE_SPBUY:
	case STORE_WBUY:
	case STORE_SBUY:
		strcpy(tempstr, "Are you sure you want to buy this item?");
		break;
	case STORE_WRECHARGE:
		strcpy(tempstr, "Are you sure you want to recharge this item?");
		break;
	case STORE_SSELL:
	case STORE_WSELL:
		strcpy(tempstr, "Are you sure you want to sell this item?");
		break;
	case STORE_SREPAIR:
		strcpy(tempstr, "Are you sure you want to repair this item?");
		break;
	}
	AddSText(0, 15, true, tempstr, COL_WHITE, false);
	AddSText(0, 18, true, "Yes", COL_WHITE, true);
	AddSText(0, 20, true, "No", COL_WHITE, true);
}






void S_StartStory()
{
	stextsize = false;
	stextscrl = false;
	AddSText(0, 2, true, "The Town Elder", COL_GOLD, false);
	AddSText(0, 9, true, "Would you like to:", COL_GOLD, false);
	AddSText(0, 12, true, "Talk to Cain", COL_BLUE, true);
	AddSText(0, 14, true, "Identify an item", COL_WHITE, true);
	AddSText(0, 18, true, "Say goodbye", COL_WHITE, true);
	AddSLine(5);
}

bool IdItemOk(ItemStruct *i)
{
	if (i->_itype == ITYPE_NONE) {
		return false;
	}
	if (i->_iMagical == ITEM_QUALITY_NORMAL) {
		return false;
	}
	return !i->_iIdentified;
}

void AddStoreHoldId(ItemStruct itm, int i)
{
	storehold[storenumh] = itm;
	storehold[storenumh]._ivalue = 100;
	storehold[storenumh]._iIvalue = 100;
	storehidx[storenumh] = i;
	storenumh++;
}

void S_StartSIdentify()
{
	bool idok;
	int i;

	idok = false;
	stextsize = true;
	storenumh = 0;

	for (i = 0; i < 48; i++)
		storehold[i]._itype = ITYPE_NONE;

	if (IdItemOk(&myplr().data.InvBody[INVLOC_HEAD])) {
		idok = true;
		AddStoreHoldId(myplr().data.InvBody[INVLOC_HEAD], -1);
	}
	if (IdItemOk(&myplr().data.InvBody[INVLOC_CHEST])) {
		idok = true;
		AddStoreHoldId(myplr().data.InvBody[INVLOC_CHEST], -2);
	}
	if (IdItemOk(&myplr().data.InvBody[INVLOC_HAND_LEFT])) {
		idok = true;
		AddStoreHoldId(myplr().data.InvBody[INVLOC_HAND_LEFT], -3);
	}
	if (IdItemOk(&myplr().data.InvBody[INVLOC_HAND_RIGHT])) {
		idok = true;
		AddStoreHoldId(myplr().data.InvBody[INVLOC_HAND_RIGHT], -4);
	}
	if (IdItemOk(&myplr().data.InvBody[INVLOC_RING_LEFT])) {
		idok = true;
		AddStoreHoldId(myplr().data.InvBody[INVLOC_RING_LEFT], -5);
	}
	if (IdItemOk(&myplr().data.InvBody[INVLOC_RING_RIGHT])) {
		idok = true;
		AddStoreHoldId(myplr().data.InvBody[INVLOC_RING_RIGHT], -6);
	}
	if (IdItemOk(&myplr().data.InvBody[INVLOC_AMULET])) {
		idok = true;
		AddStoreHoldId(myplr().data.InvBody[INVLOC_AMULET], -7);
	}

	for (i = 0; i < myplr().data._pNumInv; i++) {
		if (IdItemOk(&myplr().data.InvList[i])) {
			idok = true;
			AddStoreHoldId(myplr().data.InvList[i], i);
		}
	}

	if (!idok) {
		stextscrl = false;
		sprintf(tempstr, "You have nothing to identify.            Your gold : %i", myplr().data._pGold);
		AddSText(0, 1, true, tempstr, COL_GOLD, false);
		AddSLine(3);
		AddSLine(21);
		AddSText(0, 22, true, "Back", COL_WHITE, true);
		OffsetSTextY(22, 6);
	} else {
		stextscrl = true;
		stextsval = 0;
		stextsmax = myplr().data._pNumInv;
		sprintf(tempstr, "Identify which item?            Your gold : %i", myplr().data._pGold);
		AddSText(0, 1, true, tempstr, COL_GOLD, false);
		AddSLine(3);
		AddSLine(21);
		S_ScrollSSell(stextsval);
		AddSText(0, 22, true, "Back", COL_WHITE, true);
		OffsetSTextY(22, 6);
	}
}

void S_StartIdShow()
{
	char iclr;

	StartStore(stextshold);
	stextscrl = false;
	ClearSText(5, 23);
	iclr = COL_WHITE;

	if (myplr().data.HoldItem._iMagical != ITEM_QUALITY_NORMAL)
		iclr = COL_BLUE;
	if (!myplr().data.HoldItem._iStatFlag)
		iclr = COL_RED;

	AddSText(0, 7, true, "This item is:", COL_WHITE, false);
	AddSText(20, 11, false, myplr().data.HoldItem._iIName, iclr, false);
	PrintStoreItem(&myplr().data.HoldItem, 12, iclr);
	AddSText(0, 18, true, "Done", COL_WHITE, true);
}

void S_StartTalk()
{
	int i, sn, sn2, la;

	stextsize = false;
	stextscrl = false;
	sprintf(tempstr, "Talk to %s", talkname[talker]);
	AddSText(0, 2, true, tempstr, COL_GOLD, false);
	AddSLine(5);
	sn = 0;
	for (i = 0; i < MAXQUESTS; i++) {
		if (quests[i]._qactive == QUEST_ACTIVE && ((DWORD *)&Qtalklist[talker])[i] != -1 && quests[i]._qlog)
			sn++;
	}

	if (sn > 6) {
		sn = 14 - (sn >> 1);
		la = 1;
	} else {
		sn = 15 - sn;
		la = 2;
	}

	sn2 = sn - 2;

	for (i = 0; i < MAXQUESTS; i++) {
		if (quests[i]._qactive == QUEST_ACTIVE && ((DWORD *)&Qtalklist[talker])[i] != -1 && quests[i]._qlog) {
			AddSText(0, sn, true, questlist[i]._qlstr, COL_WHITE, true);
			sn += la;
		}
	}
	AddSText(0, sn2, true, "Gossip", COL_BLUE, true);
	AddSText(0, 22, true, "Back", COL_WHITE, true);
}

void S_StartTavern()
{
	stextsize = false;
	stextscrl = false;
	AddSText(0, 1, true, "Welcome to the", COL_GOLD, false);
	AddSText(0, 3, true, "Rising Sun", COL_GOLD, false);
	AddSText(0, 9, true, "Would you like to:", COL_GOLD, false);
	AddSText(0, 12, true, "Talk to Ogden", COL_BLUE, true);
	AddSText(0, 18, true, "Leave the tavern", COL_WHITE, true);
	AddSLine(5);
	storenumh = 20;
}

void S_StartBarMaid()
{
	stextsize = false;
	stextscrl = false;
	AddSText(0, 2, true, "Gillian", COL_GOLD, false);
	AddSText(0, 9, true, "Would you like to:", COL_GOLD, false);
	AddSText(0, 12, true, "Talk to Gillian", COL_BLUE, true);
	AddSText(0, 18, true, "Say goodbye", COL_WHITE, true);
	AddSLine(5);
	storenumh = 20;
}

void S_StartDrunk()
{
	stextsize = false;
	stextscrl = false;
	AddSText(0, 2, true, "Farnham the Drunk", COL_GOLD, false);
	AddSText(0, 9, true, "Would you like to:", COL_GOLD, false);
	AddSText(0, 12, true, "Talk to Farnham", COL_BLUE, true);
	AddSText(0, 18, true, "Say Goodbye", COL_WHITE, true);
	AddSLine(5);
	storenumh = 20;
}

void StartStore(char s)
{
	char t;
	int i;

	for (t = s;; t = STORE_SMITH) {
		sbookflag = false;
		invflag = false;
		chrflag = false;
		questlog = false;
		dropGoldFlag = false;
		ClearSText(0, 24);
		ReleaseStoreBtn();
		switch (t) {
		case STORE_SMITH:
			S_StartSmith();
			break;
		case STORE_SBUY:
			if (storenumh > 0)
				S_StartSBuy();
			break;
		case STORE_SSELL:
			S_StartSSell();
			break;
		case STORE_SREPAIR:
			S_StartSRepair();
			break;
		case STORE_WITCH:
			S_StartWitch();
			break;
		case STORE_WBUY:
			if (storenumh > 0)
				S_StartWBuy();
			break;
		case STORE_WSELL:
			S_StartWSell();
			break;
		case STORE_WRECHARGE:
			S_StartWRecharge();
			break;
		case STORE_NOMONEY:
			S_StartNoMoney();
			break;
		case STORE_NOROOM:
			S_StartNoRoom();
			break;
		case STORE_CONFIRM:
			S_StartConfirm();
			break;
		case STORE_BOY:
			S_StartBoy();
			break;
		case STORE_BBOY:
			S_StartBBoy();
			break;
		case STORE_HEALER:
			S_StartHealer();
			break;
		case STORE_STORY:
			S_StartStory();
			break;
		case STORE_HBUY:
			if (storenumh > 0)
				StoreHealer::S_StartBuy();
			break;
		case STORE_SIDENTIFY:
			S_StartSIdentify();
			break;
		case STORE_SPBUY:
			if (!S_StartSPBuy())
				return;
			break;
		case STORE_GOSSIP:
			S_StartTalk();
			break;
		case STORE_IDSHOW:
			S_StartIdShow();
			break;
		case STORE_TAVERN:
			S_StartTavern();
			break;
		case STORE_DRUNK:
			S_StartDrunk();
			break;
		case STORE_BARMAID:
			S_StartBarMaid();
			break;
		}

		for (i = 0; i < 24; i++) {
			if (stext[i]._ssel)
				break;
		}

		stextsel = i == 24 ? -1 : i;
		stextflag = t;
		if (t != STORE_SBUY || storenumh)
			break;
	}
}

void SetGoldCurs(int pnum, int i)
{
	if (plr[pnum].data.InvList[i]._ivalue >= GOLD_MEDIUM_LIMIT)
		plr[pnum].data.InvList[i]._iCurs = ICURS_GOLD_LARGE;
	else if (plr[pnum].data.InvList[i]._ivalue <= GOLD_SMALL_LIMIT)
		plr[pnum].data.InvList[i]._iCurs = ICURS_GOLD_SMALL;
	else
		plr[pnum].data.InvList[i]._iCurs = ICURS_GOLD_MEDIUM;
}

void SetSpdbarGoldCurs(int pnum, int i)
{
	if (plr[pnum].data.SpdList[i]._ivalue >= GOLD_MEDIUM_LIMIT)
		plr[pnum].data.SpdList[i]._iCurs = ICURS_GOLD_LARGE;
	else if (plr[pnum].data.SpdList[i]._ivalue <= GOLD_SMALL_LIMIT)
		plr[pnum].data.SpdList[i]._iCurs = ICURS_GOLD_SMALL;
	else
		plr[pnum].data.SpdList[i]._iCurs = ICURS_GOLD_MEDIUM;
}

void TakePlrsMoney(int cost)
{
	int i;

	myplr().data._pGold = myplr().inventory.CalculateGold() - cost;
	for (i = 0; i < MAXBELTITEMS && cost > 0; i++) {
		if (myplr().data.SpdList[i]._itype == ITYPE_GOLD && myplr().data.SpdList[i]._ivalue != GOLD_MAX_LIMIT) {
			if (cost < myplr().data.SpdList[i]._ivalue) {
				myplr().data.SpdList[i]._ivalue -= cost;
				SetSpdbarGoldCurs(myplr(), i);
				cost = 0;
			} else {
				cost -= myplr().data.SpdList[i]._ivalue;
				myplr().inventory.removeBeltItem(i);
				i = -1;
			}
		}
	}
	if (cost > 0) {
		for (i = 0; i < MAXBELTITEMS && cost > 0; i++) {
			if (myplr().data.SpdList[i]._itype == ITYPE_GOLD) {
				if (cost < myplr().data.SpdList[i]._ivalue) {
					myplr().data.SpdList[i]._ivalue -= cost;
					SetSpdbarGoldCurs(myplr(), i);
					cost = 0;
				} else {
					cost -= myplr().data.SpdList[i]._ivalue;
					myplr().inventory.removeBeltItem(i);
					i = -1;
				}
			}
		}
	}
	force_redraw = 255;
	if (cost > 0) {
		for (i = 0; i < myplr().data._pNumInv && cost > 0; i++) {
			if (myplr().data.InvList[i]._itype == ITYPE_GOLD && myplr().data.InvList[i]._ivalue != GOLD_MAX_LIMIT) {
				if (cost < myplr().data.InvList[i]._ivalue) {
					myplr().data.InvList[i]._ivalue -= cost;
					SetGoldCurs(myplr(), i);
					cost = 0;
				} else {
					cost -= myplr().data.InvList[i]._ivalue;
					myplr().inventory.RemoveInvItem(i);
					i = -1;
				}
			}
		}
		if (cost > 0) {
			for (i = 0; i < myplr().data._pNumInv && cost > 0; i++) {
				if (myplr().data.InvList[i]._itype == ITYPE_GOLD) {
					if (cost < myplr().data.InvList[i]._ivalue) {
						myplr().data.InvList[i]._ivalue -= cost;
						SetGoldCurs(myplr(), i);
						cost = 0;
					} else {
						cost -= myplr().data.InvList[i]._ivalue;
						myplr().inventory.RemoveInvItem(i);
						i = -1;
					}
				}
			}
		}
	}
}


bool StoreGoldFit(int idx)
{
	int i, sz, cost, numsqrs;

	cost = storehold[idx]._iIvalue;
	sz = cost / GOLD_MAX_LIMIT;
	if (cost % GOLD_MAX_LIMIT)
		sz++;

	SetCursor_(storehold[idx]._iCurs + CURSOR_FIRSTITEM);
	numsqrs = cursW / 28 * (cursH / 28);
	SetCursor_(CURSOR_HAND);

	if (numsqrs >= sz)
		return true;

	for (i = 0; i < 40; i++) {
		if (!myplr().data.InvGrid[i])
			numsqrs++;
	}

	for (i = 0; i < myplr().data._pNumInv; i++) {
		if (myplr().data.InvList[i]._itype == ITYPE_GOLD && myplr().data.InvList[i]._ivalue != GOLD_MAX_LIMIT) {
			if (cost + myplr().data.InvList[i]._ivalue <= GOLD_MAX_LIMIT)
				cost = 0;
			else
				cost -= GOLD_MAX_LIMIT - myplr().data.InvList[i]._ivalue;
		}
	}

	sz = cost / GOLD_MAX_LIMIT;
	if (cost % GOLD_MAX_LIMIT)
		sz++;

	return numsqrs >= sz;
}

void PlaceStoreGold(int v)
{
	bool done;
	int ii, xx, yy, i;

	done = false;

	for (i = 0; i < 40 && !done; i++) {
		yy = 10 * (i / 10);
		xx = i % 10;
		if (!myplr().data.InvGrid[xx + yy]) {
			ii = myplr().data._pNumInv;
			GetGoldSeed(myplr(), &golditem);
			myplr().data.InvList[ii] = golditem;
			myplr().data._pNumInv++;
			myplr().data.InvGrid[xx + yy] = myplr().data._pNumInv;
			myplr().data.InvList[ii]._ivalue = v;
			SetGoldCurs(myplr(), ii);
			done = true;
		}
	}
}

void StoreSellItem()
{
	int i, idx, cost;

	idx = stextvhold + ((stextlhold - stextup) >> 2);
	if (storehidx[idx] >= 0)
		myplr().inventory.RemoveInvItem(storehidx[idx]);
	else
		myplr().inventory.removeBeltItem(-(storehidx[idx] + 1));
	cost = storehold[idx]._iIvalue;
	storenumh--;
	if (idx != storenumh) {
		while (idx < storenumh) {
			storehold[idx] = storehold[idx + 1];
			storehidx[idx] = storehidx[idx + 1];
			idx++;
		}
	}
	myplr().data._pGold += cost;
	for (i = 0; i < myplr().data._pNumInv && cost > 0; i++) {
		if (myplr().data.InvList[i]._itype == ITYPE_GOLD && myplr().data.InvList[i]._ivalue != GOLD_MAX_LIMIT) {
			if (cost + myplr().data.InvList[i]._ivalue <= GOLD_MAX_LIMIT) {
				myplr().data.InvList[i]._ivalue += cost;
				SetGoldCurs(myplr(), i);
				cost = 0;
			} else {
				cost -= GOLD_MAX_LIMIT - myplr().data.InvList[i]._ivalue;
				myplr().data.InvList[i]._ivalue = GOLD_MAX_LIMIT;
				SetGoldCurs(myplr(), i);
			}
		}
	}
	if (cost > 0) {
		while (cost > GOLD_MAX_LIMIT) {
			PlaceStoreGold(GOLD_MAX_LIMIT);
			cost -= GOLD_MAX_LIMIT;
		}
		PlaceStoreGold(cost);
	}
}







void StoryIdItem()
{
	int idx;

	idx = storehidx[((stextlhold - stextup) >> 2) + stextvhold];
	if (idx < 0) {
		if (idx == -1)
			myplr().data.InvBody[INVLOC_HEAD]._iIdentified = true;
		if (idx == -2)
			myplr().data.InvBody[INVLOC_CHEST]._iIdentified = true;
		if (idx == -3)
			myplr().data.InvBody[INVLOC_HAND_LEFT]._iIdentified = true;
		if (idx == -4)
			myplr().data.InvBody[INVLOC_HAND_RIGHT]._iIdentified = true;
		if (idx == -5)
			myplr().data.InvBody[INVLOC_RING_LEFT]._iIdentified = true;
		if (idx == -6)
			myplr().data.InvBody[INVLOC_RING_RIGHT]._iIdentified = true;
		if (idx == -7)
			myplr().data.InvBody[INVLOC_AMULET]._iIdentified = true;
	} else {
		myplr().data.InvList[idx]._iIdentified = true;
	}
	myplr().data.HoldItem._iIdentified = true;
	TakePlrsMoney(myplr().data.HoldItem._iIvalue);
	CalcPlrInv(myplr(), true);
}

void S_ConfirmEnter()
{
	if (stextsel == 18) {
		switch (stextshold) {
		case STORE_SBUY:
			SmithBuyItem();
			break;
		case STORE_SSELL:
		case STORE_WSELL:
			StoreSellItem();
			break;
		case STORE_SREPAIR:
			SmithRepairItem();
			break;
		case STORE_WBUY:
			WitchBuyItem();
			break;
		case STORE_WRECHARGE:
			WitchRechargeItem();
			break;
		case STORE_BBOY:
			BoyBuyItem();
			break;
		case STORE_HBUY:
			HealerBuyItem();
			break;
		case STORE_SIDENTIFY:
			StoryIdItem();
			StartStore(STORE_IDSHOW);
			return;
		case STORE_SPBUY:
			SmithBuyPItem();
			break;
		}
		StartStore(stextshold);
	} else {
		StartStore(stextshold);
		stextsel = stextlhold;
		stextsval = stextvhold;
	}
}


void S_StoryEnter()
{
	switch (stextsel) {
	case 12:
		stextlhold = 12;
		talker = 4;
		stextshold = STORE_STORY;
		gossipstart = TEXT_STORY2;
		gossipend = TEXT_STORY11;
		StartStore(STORE_GOSSIP);
		break;
	case 14:
		StartStore(STORE_SIDENTIFY);
		break;
	case 18:
		stextflag = STORE_NONE;
		break;
	}
}

void S_SIDEnter()
{
	int idx;

	if (stextsel == 22) {
		StartStore(STORE_STORY);
		stextsel = 14;
	} else {
		stextshold = STORE_SIDENTIFY;
		stextlhold = stextsel;
		stextvhold = stextsval;
		idx = stextsval + ((stextsel - stextup) >> 2);
		myplr().data.HoldItem = storehold[idx];
		if (myplr().data._pGold < storehold[idx]._iIvalue)
			StartStore(STORE_NOMONEY);
		else
			StartStore(STORE_CONFIRM);
	}
}

void S_TalkEnter()
{
	int i, tq, sn, la;

	if (stextsel == 22) {
		StartStore(stextshold);
		stextsel = stextlhold;
		return;
	}

	sn = 0;
	for (i = 0; i < MAXQUESTS; i++) {
		if (quests[i]._qactive == QUEST_ACTIVE && ((DWORD *)&Qtalklist[talker])[i] != -1 && quests[i]._qlog)
			sn++;
	}
	if (sn > 6) {
		sn = 14 - (sn >> 1);
		la = 1;
	} else {
		sn = 15 - sn;
		la = 2;
	}

	if (stextsel == sn - 2) {
		SetRndSeed(towner[talker]._tSeed);
		tq = gossipstart + random_(0, gossipend - gossipstart + 1);
		InitQTextMsg(tq);
		return;
	}

	for (i = 0; i < MAXQUESTS; i++) {
		if (quests[i]._qactive == QUEST_ACTIVE && ((DWORD *)&Qtalklist[talker])[i] != -1 && quests[i]._qlog) {
			if (sn == stextsel) {
				InitQTextMsg(((DWORD *)&Qtalklist[talker])[i]);
			}
			sn += la;
		}
	}
}

void S_TavernEnter()
{
	switch (stextsel) {
	case 12:
		stextlhold = 12;
		talker = 3;
		stextshold = STORE_TAVERN;
		gossipstart = TEXT_OGDEN2;
		gossipend = TEXT_OGDEN10;
		StartStore(STORE_GOSSIP);
		break;
	case 18:
		stextflag = STORE_NONE;
		break;
	}
}

void S_BarmaidEnter()
{
	switch (stextsel) {
	case 12:
		stextlhold = 12;
		talker = 7;
		stextshold = STORE_BARMAID;
		gossipstart = TEXT_GILLIAN2;
		gossipend = TEXT_GILLIAN10;
		StartStore(STORE_GOSSIP);
		break;
	case 18:
		stextflag = STORE_NONE;
		break;
	}
}

void S_DrunkEnter()
{
	switch (stextsel) {
	case 12:
		stextlhold = 12;
		talker = 5;
		stextshold = STORE_DRUNK;
		gossipstart = TEXT_FARNHAM2;
		gossipend = TEXT_FARNHAM13;
		StartStore(STORE_GOSSIP);
		break;
	case 18:
		stextflag = STORE_NONE;
		break;
	}
}

void STextEnter()
{
	if (qtextflag) {
		qtextflag = false;
		if (lvl.type() == DunType::town)
			stream_stop();
	} else {
		PlaySFX(IS_TITLSLCT);
		switch (stextflag) {
		case STORE_SMITH:
			S_SmithEnter();
			break;
		case STORE_SPBUY:
			S_SPBuyEnter();
			break;
		case STORE_SBUY:
			S_SBuyEnter();
			break;
		case STORE_SSELL:
			S_SSellEnter();
			break;
		case STORE_SREPAIR:
			S_SRepairEnter();
			break;
		case STORE_WITCH:
			S_WitchEnter();
			break;
		case STORE_WBUY:
			S_WBuyEnter();
			break;
		case STORE_WSELL:
			S_WSellEnter();
			break;
		case STORE_WRECHARGE:
			S_WRechargeEnter();
			break;
		case STORE_NOMONEY:
		case STORE_NOROOM:
			StartStore(stextshold);
			stextsel = stextlhold;
			stextsval = stextvhold;
			break;
		case STORE_CONFIRM:
			S_ConfirmEnter();
			break;
		case STORE_BOY:
			S_BoyEnter();
			break;
		case STORE_BBOY:
			S_BBuyEnter();
			break;
		case STORE_HEALER:
			S_HealerEnter();
			break;
		case STORE_STORY:
			S_StoryEnter();
			break;
		case STORE_HBUY:
			S_HBuyEnter();
			break;
		case STORE_SIDENTIFY:
			S_SIDEnter();
			break;
		case STORE_GOSSIP:
			S_TalkEnter();
			break;
		case STORE_IDSHOW:
			StartStore(STORE_SIDENTIFY);
			break;
		case STORE_DRUNK:
			S_DrunkEnter();
			break;
		case STORE_TAVERN:
			S_TavernEnter();
			break;
		case STORE_BARMAID:
			S_BarmaidEnter();
			break;
		}
	}
}

void CheckStoreBtn()
{
	int y;

	if (qtextflag) {
		qtextflag = false;
		if (lvl.type() == DunType::town)
			stream_stop();
	} else if (stextsel != -1 && Mouse.y >= 32 && Mouse.y <= 320) {
		if (!stextsize) {
			if (Mouse.x < 344 + PANEL_LEFT || Mouse.x > 616 + PANEL_LEFT)
				return;
		} else {
			if (Mouse.x < 24 + PANEL_LEFT || Mouse.x > 616 + PANEL_LEFT)
				return;
		}
		y = (Mouse.y - 32) / 12;
		if (stextscrl && Mouse.x > 600 + PANEL_LEFT) {
			if (y == 4) {
				if (stextscrlubtn <= 0) {
					STextUp();
					stextscrlubtn = 10;
				} else {
					stextscrlubtn--;
				}
			}
			if (y == 20) {
				if (stextscrldbtn <= 0) {
					STextDown();
					stextscrldbtn = 10;
				} else {
					stextscrldbtn--;
				}
			}
		} else if (y >= 5) {
			if (y >= 23)
				y = 22;
			if (stextscrl && y < 21 && !stext[y]._ssel) {
				if (stext[y - 2]._ssel) {
					y -= 2;
				} else if (stext[y - 1]._ssel) {
					y--;
				}
			}
			if (stext[y]._ssel || stextscrl && y == 22) {
				stextsel = y;
				STextEnter();
			}
		}
	}
}

void ReleaseStoreBtn()
{
	stextscrlubtn = -1;
	stextscrldbtn = -1;
}

DEVILUTION_END_NAMESPACE
