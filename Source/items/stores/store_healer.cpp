#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

StoreHealer healerStore;

void StoreHealer::S_Start()
{
	stextsize = false;
	stextscrl = false;
	AddSText(0, 1, true, "Welcome to the", COL_GOLD, false);
	AddSText(0, 3, true, "Healer's home", COL_GOLD, false);
	AddSText(0, 9, true, "Would you like to:", COL_GOLD, false);
	AddSText(0, 12, true, "Talk to Pepin", COL_BLUE, true);
	AddSText(0, 14, true, "Receive healing", COL_WHITE, true);
	AddSText(0, 16, true, "Buy items", COL_WHITE, true);
	AddSText(0, 18, true, "Leave Healer's home", COL_WHITE, true);
	AddSLine(5);
	storenumh = 20;
}

void StoreHealer::S_Enter()
{
	switch (stextsel) {
		case 12:
			stextlhold = 12;
			talker = 1;
			stextshold = StoreTalkId::HEALER;
			gossipstart = TEXT_PEPIN2;
			gossipend = TEXT_PEPIN11;
			StartStore(StoreTalkId::GOSSIP);
			break;
		case 14:
			if (myplr().data._pHitPoints != myplr().data._pMaxHP)
				PlaySFX(IS_CAST8);
			redrawhpflag = true;
			myplr().data._pHitPoints = myplr().data._pMaxHP;
			myplr().data._pHPBase = myplr().data._pMaxHPBase;
			break;
		case 16:
			StartStore(StoreTalkId::HBUY);
			break;
		case 18:
			stextflag = StoreTalkId::NONE;
			break;
	}
}

void StoreHealer::Spawn(int lvl)
{
	int i, nsi, srnd, itype;

	loadPresetAttributes(0, ItemIndex::HEAL, 1);
	healitem[0] = item[0];
	healitem[0]._iCreateInfo = lvl;
	healitem[0]._iStatFlag = true;

	loadPresetAttributes(0, ItemIndex::FULLHEAL, 1);
	healitem[1] = item[0];
	healitem[1]._iCreateInfo = lvl;
	healitem[1]._iStatFlag = true;

	if (plr.isMultiplayer()) {
		loadPresetAttributes(0, ItemIndex::RESURRECT, 1);
		healitem[2] = item[0];
		healitem[2]._iCreateInfo = lvl;
		healitem[2]._iStatFlag = true;

		srnd = 3;
	} else {
		srnd = 2;
	}
	nsi = random_(50, 8) + 10;
	for (i = srnd; i < nsi; i++) {
		item[0]._iSeed = GetRndSeed();
		SetRndSeed(item[0]._iSeed);
		itype = RndItem(lvl) - 1;
		loadPresetAttributes(0, itype, lvl);
		healitem[i] = item[0];
		healitem[i]._iCreateInfo = lvl | 0x4000;
		healitem[i]._iIdentified = true;
		healitem[i]._iStatFlag = StoreStatOk(&healitem[i]);
	}
	for (i = nsi; i < 20; i++) { healitem[i]._itype = ItemType::none; }
	SortItems();
}

int StoreHealer::RndItem(int lvl)
{
	int i, ri;
	int ril[512];

	ri = 0;
	for (i = 1; AllItemsList[i].iLoc != ILOC_INVALID; i++) {
		if (AllItemsList[i].iRnd && HealerItemOk(i) &&
		    lvl >= AllItemsList[i].iMinMLvl) {
			ril[ri] = i;
			ri++;
		}
	}

	return ril[random_(50, ri)] + 1;
}

void StoreHealer::BuyItem()
{
	int idx;
	bool ok;

	idx = stextvhold + ((stextlhold - stextup) >> 2);

	ok = false;
	if (plr.isSingleplayer()) {
		if (idx < 2) ok = true;
	} else {
		if (idx < 3) ok = true;
	}
	if (ok) { myplr().data.HoldItem._iSeed = GetRndSeed(); }

	TakePlrsMoney(myplr().data.HoldItem._iIvalue);
	if (myplr().data.HoldItem._iMagical == ItemQuality::normal)
		myplr().data.HoldItem._iIdentified = false;
	StoreAutoPlace();

	ok = false;
	if (plr.isSingleplayer()) {
		if (idx >= 2) ok = true;
	} else {
		if (idx >= 3) ok = true;
	}
	if (ok) {
		idx = stextvhold + ((stextlhold - stextup) >> 2);
		if (idx == 19) {
			healitem[19]._itype = ItemType::none;
		} else {
			for (; healitem[idx + 1]._itype != ItemType::none; idx++) {
				healitem[idx] = healitem[idx + 1];
			}
			healitem[idx]._itype = ItemType::none;
		}
		myplr().CalcPlrInv(true);
	}
}


void StoreHealer::S_StartBuy()
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
	for (i = 0; healitem[i]._itype != ItemType::none; i++) { storenumh++; }
	stextsmax = storenumh - 4;
	if (stextsmax < 0) stextsmax = 0;
}

void StoreHealer::S_BuyEnter()
{
	int i, idx;
	bool done;

	if (stextsel == 22) {
		StartStore(StoreTalkId::HEALER);
		stextsel = 16;
	} else {
		stextlhold = stextsel;
		stextvhold = stextsval;
		stextshold = StoreTalkId::HBUY;
		idx = stextsval + ((stextsel - stextup) >> 2);
		if (myplr().data._pGold < healitem[idx]._iIvalue) {
			StartStore(StoreTalkId::NOMONEY);
		} else {
			myplr().data.HoldItem = healitem[idx];
			SetCursor_(myplr().data.HoldItem._iCurs + CURSOR_FIRSTITEM);
			done = false;
			i = 0;
			for (i = 0; i < 40 && !done; i++) {
				done = myplr().inv.stashHeldSpecial(
				    i, {cursW / 28, cursH / 28}, false);
			}
			if (done)
				StartStore(StoreTalkId::CONFIRM);
			else
				StartStore(StoreTalkId::NOROOM);
			SetCursor_(CURSOR_HAND);
		}
	}
}

void StoreHealer::S_ScrollBuy(int idx)
{
	int l;
	char iclr;

	ClearSText(5, 21);
	stextup = 5;
	for (l = 5; l < 20; l += 4) {
		if (healitem[idx]._itype != ItemType::none) {
			iclr = COL_WHITE;
			if (!healitem[idx]._iStatFlag) { iclr = COL_RED; }

			AddSText(20, l, false, healitem[idx]._iName, iclr, true);
			AddSTextVal(l, healitem[idx]._iIvalue);
			PrintStoreItem(&healitem[idx], l + 1, iclr);
			stextdown = l;
			idx++;
		}
	}

	if (!stext[stextsel]._ssel && stextsel != 22) stextsel = stextdown;
}

void StoreHealer::SortItems()
{
	int j, k;
	bool sorted;

	j = 2;
	while (healitem[j + 1]._itype != ItemType::none) { j++; }

	sorted = false;
	while (j > 2 && !sorted) {
		sorted = true;
		for (k = 2; k < j; k++) {
			if (healitem[k].IDidx > healitem[k + 1].IDidx) {
				BubbleSwapItem(&healitem[k], &healitem[k + 1]);
				sorted = false;
			}
		}
		j--;
	}
}

bool StoreHealer::ItemOk(int i)
{
	bool result;

	result = false;
	if (AllItemsList[i].itype != ItemType::misc) return false;

	if (AllItemsList[i].iMiscId == MiscItemId::SCROLL &&
	    AllItemsList[i].iSpell == SpellId::HEAL)
		result = true;
	if (AllItemsList[i].iMiscId == MiscItemId::SCROLLT &&
	    AllItemsList[i].iSpell == SpellId::RESURRECT && plr.isMultiplayer())
		result = false;
	if (AllItemsList[i].iMiscId == MiscItemId::SCROLLT &&
	    AllItemsList[i].iSpell == SpellId::HEALOTHER && plr.isMultiplayer())
		result = true;

	if (plr.isSingleplayer()) {
		if (AllItemsList[i].iMiscId == MiscItemId::ELIXSTR) result = true;
		if (AllItemsList[i].iMiscId == MiscItemId::ELIXMAG) result = true;
		if (AllItemsList[i].iMiscId == MiscItemId::ELIXDEX) result = true;
		if (AllItemsList[i].iMiscId == MiscItemId::ELIXVIT) result = true;
	}

	if (AllItemsList[i].iMiscId ==
	    MiscItemId::FULLHEAL)  // BUGFIX this is a duplicate with the wrong case
		result = true;

	if (AllItemsList[i].iMiscId == MiscItemId::REJUV) result = true;
	if (AllItemsList[i].iMiscId == MiscItemId::FULLREJUV) result = true;
	if (AllItemsList[i].iMiscId == MiscItemId::HEAL) result = false;
	if (AllItemsList[i].iMiscId == MiscItemId::FULLHEAL) result = false;
	if (AllItemsList[i].iMiscId == MiscItemId::MANA) result = false;
	if (AllItemsList[i].iMiscId == MiscItemId::FULLMANA) result = false;

	return result;
}


DEVILUTION_END_NAMESPACE
