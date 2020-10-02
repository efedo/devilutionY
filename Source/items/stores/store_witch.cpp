#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

StoreWitch witchStore;

void StoreWitch::S_Start()
{
	stextsize = false;
	stextscrl = false;
	AddSText(0, 2, true, "Witch's shack", COL_GOLD, false);
	AddSText(0, 9, true, "Would you like to:", COL_GOLD, false);
	AddSText(0, 12, true, "Talk to Adria", COL_BLUE, true);
	AddSText(0, 14, true, "Buy items", COL_WHITE, true);
	AddSText(0, 16, true, "Sell items", COL_WHITE, true);
	AddSText(0, 18, true, "Recharge staves", COL_WHITE, true);
	AddSText(0, 20, true, "Leave the shack", COL_WHITE, true);
	AddSLine(5);
	storenumh = 20;
}

void StoreWitch::S_Enter()
{
	switch (stextsel) {
		case 12:
			stextlhold = 12;
			talker = 6;
			stextshold = STORE_WITCH;
			gossipstart = TEXT_ADRIA2;
			gossipend = TEXT_ADRIA13;
			StartStore(STORE_GOSSIP);
			return;
		case 14:
			StartStore(STORE_WBUY);
			return;
		case 16:
			StartStore(STORE_WSELL);
			return;
		case 18:
			StartStore(STORE_WRECHARGE);
			return;
		case 20:
			stextflag = STORE_NONE;
			break;
	}
}

void StoreWitch::Spawn(int lvl)
{
	int i, iCnt;
	int idata, maxlvl;

	loadPresetAttributes(0, IDI_MANA, 1);

	witchitem[0] = item[0];
	witchitem[0]._iCreateInfo = lvl;
	witchitem[0]._iStatFlag = true;
	loadPresetAttributes(0, IDI_FULLMANA, 1);
	witchitem[1] = item[0];
	witchitem[1]._iCreateInfo = lvl;
	witchitem[1]._iStatFlag = true;
	loadPresetAttributes(0, IDI_PORTAL, 1);
	witchitem[2] = item[0];
	witchitem[2]._iCreateInfo = lvl;
	witchitem[2]._iStatFlag = true;
	iCnt = random_(51, 8) + 10;

	for (i = 3; i < iCnt; i++) {
		do {
			item[0]._iSeed = GetRndSeed();
			SetRndSeed(item[0]._iSeed);
			idata = RndItem(lvl) - 1;
			loadPresetAttributes(0, idata, lvl);
			maxlvl = -1;
			if (random_(51, 100) <= 5) maxlvl = 2 * lvl;
			if (maxlvl == -1 && item[0]._iMiscId == IMISC_STAFF)
				maxlvl = 2 * lvl;
			if (maxlvl != -1) GetItemBonus(0, idata, maxlvl >> 1, maxlvl, true);
		} while (item[0]._iIvalue > 140000);
		witchitem[i] = item[0];
		witchitem[i]._iCreateInfo = lvl | 0x2000;
		witchitem[i]._iIdentified = true;
		WitchBookLevel(i);
		witchitem[i]._iStatFlag = StoreStatOk(&witchitem[i]);
	}

	for (i = iCnt; i < 20; i++) witchitem[i]._itype = ITYPE_NONE;

	SortItems();
}


int StoreWitch::RndItem(int lvl)
{
	int i, ri;
	int ril[512];

	ri = 0;
	for (i = 1; AllItemsList[i].iLoc != ILOC_INVALID; i++) {
		if (AllItemsList[i].iRnd && WitchItemOk(i) &&
		    lvl >= AllItemsList[i].iMinMLvl) {
			ril[ri] = i;
			ri++;
		}
	}

	return ril[random_(51, ri)] + 1;
}

void StoreWitch::BuyItem()
{
	int idx;

	idx = stextvhold + ((stextlhold - stextup) >> 2);

	if (idx < 3) myplr().data.HoldItem._iSeed = GetRndSeed();

	TakePlrsMoney(myplr().data.HoldItem._iIvalue);
	StoreAutoPlace();

	if (idx >= 3) {
		if (idx == 19) {
			witchitem[19]._itype = ITYPE_NONE;
		} else {
			for (; witchitem[idx + 1]._itype != ITYPE_NONE; idx++) {
				witchitem[idx] = witchitem[idx + 1];
			}
			witchitem[idx]._itype = ITYPE_NONE;
		}
	}

	myplr().CalcPlrInv(true);
}

void StoreWitch::S_StartBuy()
{
	int i;

	stextsize = true;
	stextscrl = true;
	stextsval = 0;
	stextsmax = 20;
	sprintf(tempstr, "I have these items for sale :           Your gold : %i",
	        myplr().data._pGold);
	AddSText(0, 1, true, tempstr, COL_GOLD, false);
	AddSLine(3);
	AddSLine(21);
	S_ScrollBuy(stextsval);
	AddSText(0, 22, true, "Back", COL_WHITE, false);
	OffsetSTextY(22, 6);

	storenumh = 0;
	for (i = 0; witchitem[i]._itype != ITYPE_NONE; i++) { storenumh++; }
	stextsmax = storenumh - 4;
	if (stextsmax < 0) stextsmax = 0;
}

void StoreWitch::S_BuyEnter()
{
	int i, idx;
	bool done;

	if (stextsel == 22) {
		StartStore(STORE_WITCH);
		stextsel = 14;
	} else {
		stextlhold = stextsel;
		stextvhold = stextsval;
		stextshold = STORE_WBUY;
		idx = stextsval + ((stextsel - stextup) >> 2);

		if (myplr().data._pGold < witchitem[idx]._iIvalue) {
			StartStore(STORE_NOMONEY);
		} else {
			myplr().data.HoldItem = witchitem[idx];
			SetCursor_(myplr().data.HoldItem._iCurs + CURSOR_FIRSTITEM);
			done = false;

			for (i = 0; i < 40 && !done; i++) {
				done = myplr().inv.stashHeldSpecial(
				    i, {cursW / 28, cursH / 28}, false);
			}

			if (done)
				StartStore(STORE_CONFIRM);
			else
				StartStore(STORE_NOROOM);

			SetCursor_(CURSOR_HAND);
		}
	}
}

void StoreWitch::S_ScrollBuy(int idx)
{
	int l, ls;
	char iclr;

	ls = idx;
	ClearSText(5, 21);
	stextup = 5;

	for (l = 5; l < 20; l += 4) {
		if (witchitem[ls]._itype != ITYPE_NONE) {
			iclr = COL_WHITE;
			if (witchitem[ls]._iMagical) { iclr = COL_BLUE; }

			if (!witchitem[ls]._iStatFlag) { iclr = COL_RED; }

			if (witchitem[ls]._iMagical) {
				AddSText(20, l, false, witchitem[ls]._iIName, iclr, true);
			} else {
				AddSText(20, l, false, witchitem[ls]._iName, iclr, true);
			}

			AddSTextVal(l, witchitem[ls]._iIvalue);
			PrintStoreItem(&witchitem[ls], l + 1, iclr);
			stextdown = l;
			ls++;
		}
	}

	if (!stext[stextsel]._ssel && stextsel != 22) stextsel = stextdown;
}

void StoreWitch::SortItems()
{
	int j, k;
	bool sorted;

	j = 3;
	while (witchitem[j + 1]._itype != ITYPE_NONE) { j++; }

	sorted = false;
	while (j > 3 && !sorted) {
		sorted = true;
		for (k = 3; k < j; k++) {
			if (witchitem[k].IDidx > witchitem[k + 1].IDidx) {
				BubbleSwapItem(&witchitem[k], &witchitem[k + 1]);
				sorted = false;
			}
		}
		j--;
	}
}

bool StoreWitch::ItemOk(int i)
{
	bool rv = false;
	if (AllItemsList[i].itype == ITYPE_MISC) rv = true;
	if (AllItemsList[i].itype == ITYPE_STAFF) rv = true;
	if (AllItemsList[i].iMiscId == IMISC_MANA) rv = false;
	if (AllItemsList[i].iMiscId == IMISC_FULLMANA) rv = false;
	if (AllItemsList[i].iSpell == SPL_TOWN) rv = false;
	if (AllItemsList[i].iMiscId == IMISC_FULLHEAL) rv = false;
	if (AllItemsList[i].iMiscId == IMISC_HEAL) rv = false;
	if (AllItemsList[i].iSpell == SPL_RESURRECT && plr.isSingleplayer())
		rv = false;
	if (AllItemsList[i].iSpell == SPL_HEALOTHER && plr.isSingleplayer())
		rv = false;

	return rv;
}

void StoreWitch::S_StartSell()
{
	int i;
	bool sellok;

	stextsize = true;
	sellok = false;
	storenumh = 0;

	for (i = 0; i < 48; i++) storehold[i]._itype = ITYPE_NONE;

	for (i = 0; i < myplr().data._pNumInv; i++) {
		if (WitchSellOk(i)) {
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

	for (i = 0; i < MAXBELTITEMS; i++) {
		if (myplr().data.SpdList[i]._itype != ITYPE_NONE &&
		    WitchSellOk(-(i + 1))) {
			sellok = true;
			storehold[storenumh] = myplr().data.SpdList[i];

			if (storehold[storenumh]._iMagical != ITEM_QUALITY_NORMAL &&
			    storehold[storenumh]._iIdentified)
				storehold[storenumh]._ivalue = storehold[storenumh]._iIvalue;

			if (!(storehold[storenumh]._ivalue >>= 2))
				storehold[storenumh]._ivalue = 1;

			storehold[storenumh]._iIvalue = storehold[storenumh]._ivalue;
			storehidx[storenumh++] = -(i + 1);
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

void StoreWitch::S_SellEnter()
{
	int idx;

	if (stextsel == 22) {
		StartStore(STORE_WITCH);
		stextsel = 16;
	} else {
		stextlhold = stextsel;
		idx = stextsval + ((stextsel - stextup) >> 2);
		stextshold = STORE_WSELL;
		stextvhold = stextsval;
		myplr().data.HoldItem = storehold[idx];
		if (StoreGoldFit(idx))
			StartStore(STORE_CONFIRM);
		else
			StartStore(STORE_NOROOM);
	}
}

bool StoreWitch::SellOk(int i)
{
	bool rv;
	ItemStruct *pI;

	rv = false;

	if (i >= 0)
		pI = &myplr().data.InvList[i];
	else
		pI = &myplr().data.SpdList[-(i + 1)];

	if (pI->_itype == ITYPE_MISC) rv = true;
	if (pI->_itype == ITYPE_STAFF) rv = true;
	if (pI->IDidx >= IDI_FIRSTQUEST && pI->IDidx <= IDI_LASTQUEST) rv = false;
	if (pI->IDidx == IDI_LAZSTAFF) rv = false;
	return rv;
}


void StoreWitch::RechargeItem()
{
	int i, idx;

	TakePlrsMoney(myplr().data.HoldItem._iIvalue);

	idx = stextvhold + ((stextlhold - stextup) >> 2);
	storehold[idx]._iCharges = storehold[idx]._iMaxCharges;

	i = storehidx[idx];
	if (i < 0)
		myplr().data.InvBody[INVLOC_HAND_LEFT]._iCharges =
		    myplr().data.InvBody[INVLOC_HAND_LEFT]._iMaxCharges;
	else
		myplr().data.InvList[i]._iCharges =
		    myplr().data.InvList[i]._iMaxCharges;

	CalcPlrInv(myplr(), true);
}

void StoreWitch::S_RechargeEnter()
{
	int idx;

	if (stextsel == 22) {
		StartStore(STORE_WITCH);
		stextsel = 18;
	} else {
		stextshold = STORE_WRECHARGE;
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

bool StoreWitch::RechargeOk(int i)
{
	bool rv;

	rv = false;
	if (myplr().data.InvList[i]._itype == ITYPE_STAFF &&
	    myplr().data.InvList[i]._iCharges !=
	        myplr().data.InvList[i]._iMaxCharges) {
		rv = true;
	}
	return rv;
}

void StoreWitch::AddStoreHoldRecharge(ItemStruct itm, int i)
{
	storehold[storenumh] = itm;
	storehold[storenumh]._ivalue += spelldata[itm._iSpell].sStaffCost;
	storehold[storenumh]._ivalue = storehold[storenumh]._ivalue *
	                                   (100 *
	                                    (storehold[storenumh]._iMaxCharges -
	                                     storehold[storenumh]._iCharges) /
	                                    storehold[storenumh]._iMaxCharges) /
	                                   100 >>
	                               1;
	storehold[storenumh]._iIvalue = storehold[storenumh]._ivalue;
	storehidx[storenumh] = i;
	storenumh++;
}

void StoreWitch::S_StartRecharge()
{
	int i;
	bool rechargeok;

	stextsize = true;
	rechargeok = false;
	storenumh = 0;

	for (i = 0; i < 48; i++) { storehold[i]._itype = ITYPE_NONE; }

	if (myplr().data.InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_STAFF &&
	    myplr().data.InvBody[INVLOC_HAND_LEFT]._iCharges !=
	        myplr().data.InvBody[INVLOC_HAND_LEFT]._iMaxCharges) {
		rechargeok = true;
		AddStoreHoldRecharge(myplr().data.InvBody[INVLOC_HAND_LEFT], -1);
	}

	for (i = 0; i < myplr().data._pNumInv; i++) {
		if (WitchRechargeOk(i)) {
			rechargeok = true;
			AddStoreHoldRecharge(myplr().data.InvList[i], i);
		}
	}

	if (!rechargeok) {
		stextscrl = false;
		sprintf(tempstr,
		        "You have nothing to recharge.            Your gold : %i",
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
		sprintf(tempstr, "Recharge which item?            Your gold : %i",
		        myplr().data._pGold);
		AddSText(0, 1, true, tempstr, COL_GOLD, false);
		AddSLine(3);
		AddSLine(21);
		S_ScrollSSell(stextsval);
		AddSText(0, 22, true, "Back", COL_WHITE, true);
		OffsetSTextY(22, 6);
	}
}




DEVILUTION_END_NAMESPACE
