#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

StoreSmith smithStore;

void StoreSmith::S_Start()
{
	stextsize = false;
	stextscrl = false;
	AddSText(0, 1, true, "Welcome to the", COL_GOLD, false);
	AddSText(0, 3, true, "Blacksmith's shop", COL_GOLD, false);
	AddSText(0, 7, true, "Would you like to:", COL_GOLD, false);
	AddSText(0, 10, true, "Talk to Griswold", COL_BLUE, true);
	AddSText(0, 12, true, "Buy basic items", COL_WHITE, true);
	AddSText(0, 14, true, "Buy premium items", COL_WHITE, true);
	AddSText(0, 16, true, "Sell items", COL_WHITE, true);
	AddSText(0, 18, true, "Repair items", COL_WHITE, true);
	AddSText(0, 20, true, "Leave the shop", COL_WHITE, true);
	AddSLine(5);
	storenumh = 20;
}

void StoreSmith::S_Enter()
{
	switch (stextsel) {
		case 10:
			talker = 0;
			stextlhold = 10;
			stextshold = STORE_SMITH;
			gossipstart = TEXT_GRISWOLD2;
			gossipend = TEXT_GRISWOLD13;
			StartStore(STORE_GOSSIP);
			break;
		case 12:
			StartStore(STORE_SBUY);
			break;
		case 14:
			StartStore(STORE_SPBUY);
			break;
		case 16:
			StartStore(STORE_SSELL);
			break;
		case 18:
			StartStore(STORE_SREPAIR);
			break;
		case 20:
			stextflag = STORE_NONE;
			break;
	}
}

void StoreSmith::Spawn(int lvl)
{
	int i, iCnt, idata;

	iCnt = random_(50, SMITH_ITEMS - 10) + 10;
	for (i = 0; i < iCnt; i++) {
		do {
			item[0]._iSeed = GetRndSeed();
			SetRndSeed(item[0]._iSeed);
			idata = RndItem(lvl) - 1;
			loadPresetAttributes(0, idata, lvl);
		} while (item[0]._iIvalue > SMITH_MAX_VALUE);
		smithitem[i] = item[0];
		smithitem[i]._iCreateInfo = lvl | 0x400;
		smithitem[i]._iIdentified = true;
		smithitem[i]._iStatFlag = StoreStatOk(&smithitem[i]);
	}
	for (i = iCnt; i < SMITH_ITEMS; i++) smithitem[i]._itype = ITYPE_NONE;
	SortItems();
}

int StoreSmith::RndItem(int lvl)
{
	int i, ri;
	int ril[512];

	ri = 0;
	for (i = 1; AllItemsList[i].iLoc != ILOC_INVALID; i++) {
		if (AllItemsList[i].iRnd && SmithItemOk(i) &&
		    lvl >= AllItemsList[i].iMinMLvl) {
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

void StoreSmith::SpawnPremium(int lvl)
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

void StoreSmith::SpawnOnePremium(int i, int plvl)
{
	int itype;
	ItemStruct holditem;

	holditem = item[0];
	if (plvl > 30) plvl = 30;
	if (plvl < 1) plvl = 1;
	do {
		item[0]._iSeed = GetRndSeed();
		SetRndSeed(item[0]._iSeed);
		itype = RndPremiumItem(plvl >> 2, plvl) - 1;
		loadPresetAttributes(0, itype, plvl);
		GetItemBonus(0, itype, plvl >> 1, plvl, true);
	} while (item[0]._iIvalue > SMITH_MAX_PREMIUM_VALUE);
	premiumitem[i] = item[0];
	premiumitem[i]._iCreateInfo = plvl | 0x800;
	premiumitem[i]._iIdentified = true;
	premiumitem[i]._iStatFlag = StoreStatOk(&premiumitem[i]);
	item[0] = holditem;
}

int StoreSmith::RndPremiumItem(int minlvl, int maxlvl)
{
	int i, ri;
	int ril[512];

	ri = 0;
	for (i = 1; AllItemsList[i].iLoc != ILOC_INVALID; i++) {
		if (AllItemsList[i].iRnd) {
			if (PremiumItemOk(i)) {
				if (AllItemsList[i].iMinMLvl >= minlvl &&
				    AllItemsList[i].iMinMLvl <= maxlvl) {
					ril[ri] = i;
					ri++;
				}
			}
		}
	}
	return ril[random_(50, ri)] + 1;
}

void StoreSmith::BuyItem()
{
	TakePlrsMoney(myplr().data.HoldItem._iIvalue);
	if (myplr().data.HoldItem._iMagical == ITEM_QUALITY_NORMAL)
		myplr().data.HoldItem._iIdentified = false;
	StoreAutoPlace();
	int idx = stextvhold + ((stextlhold - stextup) >> 2);
	if (idx == SMITH_ITEMS - 1) {
		smithitem[SMITH_ITEMS - 1]._itype = ITYPE_NONE;
	} else {
		for (; smithitem[idx + 1]._itype != ITYPE_NONE; idx++) {
			smithitem[idx] = smithitem[idx + 1];
		}
		smithitem[idx]._itype = ITYPE_NONE;
	}
	myplr().CalcPlrInv(true);
}

void StoreSmith::S_StartBuy()
{
	int i;

	stextsize = true;
	stextscrl = true;
	stextsval = 0;
	sprintf(tempstr, "I have these items for sale :           Your gold : %i",
	        myplr().data._pGold);
	AddSText(0, 1, true, tempstr, COL_GOLD, false);
	AddSLine(3);
	AddSLine(21);
	S_ScrollBuy(stextsval);
	AddSText(0, 22, true, "Back", COL_WHITE, false);
	OffsetSTextY(22, 6);
	storenumh = 0;
	for (i = 0; smithitem[i]._itype != ITYPE_NONE; i++) { storenumh++; }

	stextsmax = storenumh - 4;
	if (stextsmax < 0) stextsmax = 0;
}

void StoreSmith::S_BuyEnter()
{
	int idx, i;
	bool done;

	if (stextsel == 22) {
		StartStore(STORE_SMITH);
		stextsel = 12;
	} else {
		stextlhold = stextsel;
		stextvhold = stextsval;
		stextshold = STORE_SBUY;
		idx = stextsval + ((stextsel - stextup) >> 2);
		if (myplr().data._pGold < smithitem[idx]._iIvalue) {
			StartStore(STORE_NOMONEY);
		} else {
			myplr().data.HoldItem = smithitem[idx];
			SetCursor_(myplr().data.HoldItem._iCurs + CURSOR_FIRSTITEM);
			done = false;

			for (i = 0; i < 40 && !done; i++) {
				done = myplr().inv.AutoPlace(i, {cursW / 28, cursH / 28},
				                                   false);
			}
			if (done)
				StartStore(STORE_CONFIRM);
			else
				StartStore(STORE_NOROOM);
			SetCursor_(CURSOR_HAND);
		}
	}
}

void StoreSmith::S_ScrollBuy(int idx)
{
	int l, ls;
	char iclr;

	ls = idx;
	ClearSText(5, 21);
	stextup = 5;

	for (l = 5; l < 20; l += 4) {
		if (smithitem[ls]._itype != ITYPE_NONE) {
			iclr = COL_WHITE;
			if (smithitem[ls]._iMagical) { iclr = COL_BLUE; }

			if (!smithitem[ls]._iStatFlag) { iclr = COL_RED; }

			if (smithitem[ls]._iMagical) {
				AddSText(20, l, false, smithitem[ls]._iIName, iclr, true);
			} else {
				AddSText(20, l, false, smithitem[ls]._iName, iclr, true);
			}

			AddSTextVal(l, smithitem[ls]._iIvalue);
			PrintStoreItem(&smithitem[ls], l + 1, iclr);
			stextdown = l;
			ls++;
		}
	}

	if (!stext[stextsel]._ssel && stextsel != 22) stextsel = stextdown;
}

void StoreSmith::SortItems()
{
	int j, k;
	bool sorted;

	j = 0;
	while (smithitem[j + 1]._itype != ITYPE_NONE) { j++; }

	sorted = false;
	while (j > 0 && !sorted) {
		sorted = true;
		for (k = 0; k < j; k++) {
			if (smithitem[k].IDidx > smithitem[k + 1].IDidx) {
				BubbleSwapItem(&smithitem[k], &smithitem[k + 1]);
				sorted = false;
			}
		}
		j--;
	}
}

void StoreSmith::BuyPremiumItem()
{
	int i, xx, idx;

	TakePlrsMoney(myplr().data.HoldItem._iIvalue);
	if (myplr().data.HoldItem._iMagical == ITEM_QUALITY_NORMAL)
		myplr().data.HoldItem._iIdentified = false;
	StoreAutoPlace();

	idx = stextvhold + ((stextlhold - stextup) >> 2);
	xx = 0;
	for (i = 0; idx >= 0; i++) {
		if (premiumitem[i]._itype != ITYPE_NONE) {
			idx--;
			xx = i;
		}
	}

	premiumitem[xx]._itype = ITYPE_NONE;
	numpremium--;
	SpawnPremium(myplr().data._pLevel);
}

void StoreSmith::S_PremiumBuyEnter()
{
	int i, idx, xx;
	bool done;

	if (stextsel == 22) {
		StartStore(STORE_SMITH);
		stextsel = 14;
	} else {
		stextshold = STORE_SPBUY;
		stextlhold = stextsel;
		stextvhold = stextsval;
		xx = stextsval + ((stextsel - stextup) >> 2);
		idx = 0;
		for (i = 0; xx >= 0; i++) {
			if (premiumitem[i]._itype != ITYPE_NONE) {
				xx--;
				idx = i;
			}
		}
		if (myplr().data._pGold < premiumitem[idx]._iIvalue) {
			StartStore(STORE_NOMONEY);
		} else {
			myplr().data.HoldItem = premiumitem[idx];
			SetCursor_(myplr().data.HoldItem._iCurs + CURSOR_FIRSTITEM);
			done = false;
			for (i = 0; i < 40 && !done; i++) {
				done = myplr().inventory.AutoPlace(i, {cursW / 28, cursH / 28},
				                                   false);
			}
			if (done)
				StartStore(STORE_CONFIRM);
			else
				StartStore(STORE_NOROOM);
			SetCursor_(CURSOR_HAND);
		}
	}
}


bool StoreSmith::S_StartPremiumBuy()
{
	int i;

	storenumh = 0;
	for (i = 0; i < 6; i++) {
		if (premiumitem[i]._itype != ITYPE_NONE) storenumh++;
	}
	if (!storenumh) {
		StartStore(STORE_SMITH);
		stextsel = 14;
		return false;
	}

	stextsize = true;
	stextscrl = true;
	stextsval = 0;

	sprintf(tempstr, "I have these premium items for sale :   Your gold : %i",
	        myplr().data._pGold);
	AddSText(0, 1, true, tempstr, COL_GOLD, false);
	AddSLine(3);
	AddSLine(21);
	AddSText(0, 22, true, "Back", COL_WHITE, false);
	OffsetSTextY(22, 6);

	stextsmax = storenumh - 4;
	if (stextsmax < 0) stextsmax = 0;

	S_ScrollSPBuy(stextsval);

	return true;
}

void StoreSmith::S_ScrollPremiumBuy(int idx)
{
	int l, boughtitems;
	char iclr;

	ClearSText(5, 21);
	boughtitems = idx;

	stextup = 5;
	for (idx = 0; boughtitems; idx++) {
		if (premiumitem[idx]._itype != ITYPE_NONE) boughtitems--;
	}

	for (l = 5; l < 20 && idx < 6; l += 4) {
		if (premiumitem[idx]._itype != ITYPE_NONE) {
			iclr = COL_WHITE;
			if (premiumitem[idx]._iMagical) iclr = COL_BLUE;
			if (!premiumitem[idx]._iStatFlag) iclr = COL_RED;
			AddSText(20, l, false, premiumitem[idx]._iIName, iclr, true);
			AddSTextVal(l, premiumitem[idx]._iIvalue);
			PrintStoreItem(&premiumitem[idx], l + 1, iclr);
			stextdown = l;
		} else {
			l -= 4;
		}
		idx++;
	}
	if (!stext[stextsel]._ssel && stextsel != 22) stextsel = stextdown;
}



bool StoreSmith::ItemOk(int i)
{
	bool rv;

	rv = true;
	if (AllItemsList[i].itype == ITYPE_MISC) rv = false;
	if (AllItemsList[i].itype == ITYPE_GOLD) rv = false;
	if (AllItemsList[i].itype == ITYPE_MEAT) rv = false;
	if (AllItemsList[i].itype == ITYPE_STAFF) rv = false;
	if (AllItemsList[i].itype == ITYPE_RING) rv = false;
	if (AllItemsList[i].itype == ITYPE_AMULET) rv = false;

	return rv;
}

bool StoreSmith::PremiumItemOk(int i)
{
	bool rv = true;
	if (AllItemsList[i].itype == ITYPE_MISC) rv = false;
	if (AllItemsList[i].itype == ITYPE_GOLD) rv = false;
	if (AllItemsList[i].itype == ITYPE_MEAT) rv = false;
	if (AllItemsList[i].itype == ITYPE_STAFF) rv = false;

	if (plr.isMultiplayer()) {
		if (AllItemsList[i].itype == ITYPE_RING) rv = false;
		if (AllItemsList[i].itype == ITYPE_AMULET) rv = false;
	}
	return rv;
}

void StoreSmith::S_StartSell()
{
	int i;
	bool sellok;

	stextsize = true;
	sellok = false;
	storenumh = 0;

	for (i = 0; i < 48; i++) storehold[i]._itype = ITYPE_NONE;

	for (i = 0; i < myplr().data._pNumInv; i++) {
		if (SmithSellOk(i)) {
			sellok = true;
			storehold[storenumh] = myplr().data.InvList[i];

			if (storehold[storenumh]._iMagical != ITEM_QUALITY_NORMAL &&
			    storehold[storenumh]._iIdentified)
				storehold[storenumh]._ivalue = storehold[storenumh]._iIvalue;

			if (!(storehold[storenumh]._ivalue >>= 2))
				storehold[storenumh]._ivalue = 1;

			storehold[storenumh]._iIvalue = storehold[storenumh]._ivalue;
			storehidx[storenumh++] = i;
		}
	}

	if (!sellok) {
		stextscrl = false;
		sprintf(tempstr, "You have nothing I want.            Your gold : %i",
		        myplr().data._pGold);
		AddSText(0, 1, true, tempstr, COL_GOLD, false);
		AddSLine(3);
		AddSLine(21);
		AddSText(0, 22, true, "Back", COL_WHITE, true);
		OffsetSTextY(22, 6);
	} else {
		stextscrl = true;
		stextsval = 0;
		stextsmax = myplr().data._pNumInv;
		sprintf(tempstr, "Which item is for sale?            Your gold : %i",
		        myplr().data._pGold);
		AddSText(0, 1, true, tempstr, COL_GOLD, false);
		AddSLine(3);
		AddSLine(21);
		S_ScrollSSell(stextsval);
		AddSText(0, 22, true, "Back", COL_WHITE, true);
		OffsetSTextY(22, 6);
	}
}

void StoreSmith::S_SellEnter()
{
	int idx;

	if (stextsel == 22) {
		StartStore(STORE_SMITH);
		stextsel = 16;
	} else {
		stextlhold = stextsel;
		idx = stextsval + ((stextsel - stextup) >> 2);
		stextshold = STORE_SSELL;
		stextvhold = stextsval;
		myplr().data.HoldItem = storehold[idx];

		if (StoreGoldFit(idx))
			StartStore(STORE_CONFIRM);
		else
			StartStore(STORE_NOROOM);
	}
}

bool StoreSmith::SellOk(int i)
{
	if (myplr().data.InvList[i]._itype == ITYPE_NONE) return false;
	if (myplr().data.InvList[i]._itype == ITYPE_MISC) return false;
	if (myplr().data.InvList[i]._itype == ITYPE_GOLD) return false;
	if (myplr().data.InvList[i]._itype == ITYPE_MEAT) return false;
	if (myplr().data.InvList[i]._itype == ITYPE_STAFF) return false;
	if (myplr().data.InvList[i].IDidx == IDI_LAZSTAFF) return false;

	return true;
}

void StoreSmith::S_ScrollSell(int idx)
{
	int l;
	char iclr;

	ClearSText(5, 21);
	stextup = 5;

	for (l = 5; l < 20; l += 4) {
		if (idx >= storenumh) break;
		if (storehold[idx]._itype != ITYPE_NONE) {
			iclr = COL_WHITE;
			if (storehold[idx]._iMagical) { iclr = COL_BLUE; }

			if (!storehold[idx]._iStatFlag) { iclr = COL_RED; }

			if (storehold[idx]._iMagical && storehold[idx]._iIdentified) {
				AddSText(20, l, false, storehold[idx]._iIName, iclr, true);
				AddSTextVal(l, storehold[idx]._iIvalue);
			} else {
				AddSText(20, l, false, storehold[idx]._iName, iclr, true);
				AddSTextVal(l, storehold[idx]._ivalue);
			}

			PrintStoreItem(&storehold[idx], l + 1, iclr);
			stextdown = l;
		}
		idx++;
	}

	stextsmax = storenumh - 4;
	if (stextsmax < 0) stextsmax = 0;
}

void StoreSmith::RepairItem()
{
	int i, idx;

	TakePlrsMoney(myplr().data.HoldItem._iIvalue);

	idx = stextvhold + ((stextlhold - stextup) >> 2);
	storehold[idx]._iDurability = storehold[idx]._iMaxDur;

	i = storehidx[idx];
	if (i < 0) {
		if (i == -1)
			myplr().data.InvBody[INVLOC_HEAD]._iDurability =
			    myplr().data.InvBody[INVLOC_HEAD]._iMaxDur;
		if (i == -2)
			myplr().data.InvBody[INVLOC_CHEST]._iDurability =
			    myplr().data.InvBody[INVLOC_CHEST]._iMaxDur;
		if (i == -3)
			myplr().data.InvBody[INVLOC_HAND_LEFT]._iDurability =
			    myplr().data.InvBody[INVLOC_HAND_LEFT]._iMaxDur;
		if (i == -4)
			myplr().data.InvBody[INVLOC_HAND_RIGHT]._iDurability =
			    myplr().data.InvBody[INVLOC_HAND_RIGHT]._iMaxDur;
	} else {
		myplr().data.InvList[i]._iDurability = myplr().data.InvList[i]._iMaxDur;
	}
}

void StoreSmith::S_RepairEnter()
{
	int idx;

	if (stextsel == 22) {
		StartStore(STORE_SMITH);
		stextsel = 18;
	} else {
		stextshold = STORE_SREPAIR;
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


bool StoreSmith::RepairOk(int i)
{
	if (myplr().data.InvList[i]._itype == ITYPE_NONE) return false;
	if (myplr().data.InvList[i]._itype == ITYPE_MISC) return false;
	if (myplr().data.InvList[i]._itype == ITYPE_GOLD) return false;
	if (myplr().data.InvList[i]._itype == ITYPE_MEAT) return false;
	if (myplr().data.InvList[i]._iDurability ==
	    myplr().data.InvList[i]._iMaxDur)
		return false;

	return true;
}

void StoreSmith::S_StartRepair()
{
	bool repairok;
	int i;

	stextsize = true;
	repairok = false;
	storenumh = 0;
	for (i = 0; i < 48; i++) storehold[i]._itype = ITYPE_NONE;
	if (myplr().data.InvBody[INVLOC_HEAD]._itype != ITYPE_NONE &&
	    myplr().data.InvBody[INVLOC_HEAD]._iDurability !=
	        myplr().data.InvBody[INVLOC_HEAD]._iMaxDur) {
		repairok = true;
		AddStoreHoldRepair(myplr().data.InvBody, -1);
	}
	if (myplr().data.InvBody[INVLOC_CHEST]._itype != ITYPE_NONE &&
	    myplr().data.InvBody[INVLOC_CHEST]._iDurability !=
	        myplr().data.InvBody[INVLOC_CHEST]._iMaxDur) {
		repairok = true;
		AddStoreHoldRepair(&myplr().data.InvBody[INVLOC_CHEST], -2);
	}
	if (myplr().data.InvBody[INVLOC_HAND_LEFT]._itype != ITYPE_NONE &&
	    myplr().data.InvBody[INVLOC_HAND_LEFT]._iDurability !=
	        myplr().data.InvBody[INVLOC_HAND_LEFT]._iMaxDur) {
		repairok = true;
		AddStoreHoldRepair(&myplr().data.InvBody[INVLOC_HAND_LEFT], -3);
	}
	if (myplr().data.InvBody[INVLOC_HAND_RIGHT]._itype != ITYPE_NONE &&
	    myplr().data.InvBody[INVLOC_HAND_RIGHT]._iDurability !=
	        myplr().data.InvBody[INVLOC_HAND_RIGHT]._iMaxDur) {
		repairok = true;
		AddStoreHoldRepair(&myplr().data.InvBody[INVLOC_HAND_RIGHT], -4);
	}
	for (i = 0; i < myplr().data._pNumInv; i++) {
		if (SmithRepairOk(i)) {
			repairok = true;
			AddStoreHoldRepair(&myplr().data.InvList[i], i);
		}
	}
	if (!repairok) {
		stextscrl = false;
		sprintf(tempstr,
		        "You have nothing to repair.            Your gold : %i",
		        myplr().data._pGold);
		AddSText(0, 1, true, tempstr, COL_GOLD, false);
		AddSLine(3);
		AddSLine(21);
		AddSText(0, 22, true, "Back", COL_WHITE, true);
		OffsetSTextY(22, 6);
		return;
	}

	stextscrl = true;
	stextsval = 0;
	stextsmax = myplr().data._pNumInv;
	sprintf(tempstr, "Repair which item?            Your gold : %i",
	        myplr().data._pGold);
	AddSText(0, 1, true, tempstr, COL_GOLD, false);
	AddSLine(3);
	AddSLine(21);
	S_ScrollSSell(stextsval);
	AddSText(0, 22, true, "Back", COL_WHITE, true);
	OffsetSTextY(22, 6);
}



DEVILUTION_END_NAMESPACE
