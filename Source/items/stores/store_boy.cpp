#include "all.h"

namespace dvl {

extern StoreBoy boyStore;

void StoreBoy::S_Start()
{
	stextsize = false;
	stextscrl = false;
	AddSText(0, 2, true, "Wirt the Peg-legged boy", COL_GOLD, false);
	AddSLine(5);
	if (boyitem._itype != ItemType::none) {
		AddSText(0, 8, true, "Talk to Wirt", COL_BLUE, true);
		AddSText(0, 12, true, "I have something for sale,", COL_GOLD, false);
		AddSText(0, 14, true, "but it will cost 50 gold", COL_GOLD, false);
		AddSText(0, 16, true, "just to take a look. ", COL_GOLD, false);
		AddSText(0, 18, true, "What have you got?", COL_WHITE, true);
		AddSText(0, 20, true, "Say goodbye", COL_WHITE, true);
	} else {
		AddSText(0, 12, true, "Talk to Wirt", COL_BLUE, true);
		AddSText(0, 18, true, "Say goodbye", COL_WHITE, true);
	}
}

void StoreBoy::S_Enter()
{
	if (boyitem._itype != ItemType::none && stextsel == 18) {
		if (myplr().data._pGold < 50) {
			stextshold = StoreTalkId::BOY;
			stextlhold = 18;
			stextvhold = stextsval;
			StartStore(StoreTalkId::NOMONEY);
		} else {
			TakePlrsMoney(50);
			StartStore(StoreTalkId::BBOY);
		}
	} else if (stextsel == 8 && boyitem._itype != ItemType::none ||
	           stextsel == 12 && boyitem._itype == ItemType::none) {
		talker = 8;
		stextshold = StoreTalkId::BOY;
		stextlhold = stextsel;
		gossipstart = TEXT_WIRT2;
		gossipend = TEXT_WIRT12;
		StartStore(StoreTalkId::GOSSIP);
	} else {
		stextflag = StoreTalkId::NONE;
	}
}

void StoreBoy::Spawn(int lvl)
{
	int itype;

	if (boylevel < lvl >> 1 || boyitem._itype == ItemType::none) {
		do {
			item[0]._iSeed = GetRndSeed();
			SetRndSeed(item[0]._iSeed);
			itype = RndItem(lvl) - 1;
			loadPresetAttributes(0, itype, lvl);
			GetItemBonus(0, itype, lvl, 2 * lvl, true);
		} while (item[0]._iIvalue > 90000);
		boyitem = item[0];
		boyitem._iCreateInfo = lvl | 0x1000;
		boyitem._iIdentified = true;
		boyitem._iStatFlag = StoreStatOk(&boyitem);
		boylevel = lvl >> 1;
	}
}

int StoreBoy::RndItem(int lvl)
{
	int i, ri;
	int ril[512];

	ri = 0;
	for (i = 1; AllItemsList[i].iLoc != ILOC_INVALID; i++) {
		if (AllItemsList[i].iRnd && PremiumItemOk(i) &&
		    lvl >= AllItemsList[i].iMinMLvl) {
			ril[ri] = i;
			ri++;
		}
	}

	return ril[random_(49, ri)] + 1;
}

void StoreBoy::BuyItem()
{
	TakePlrsMoney(myplr().data.HoldItem._iIvalue);
	StoreAutoPlace();
	boyitem._itype = ItemType::none;
	stextshold = StoreTalkId::BOY;
	myplr().CalcPlrInv(true);
}

void StoreBoy::S_StartBuy()
{
	int iclr;

	stextsize = true;
	stextscrl = false;
	sprintf(tempstr, "I have this item for sale :           Your gold : %i",
	        myplr().data._pGold);
	AddSText(0, 1, true, tempstr, COL_GOLD, false);
	AddSLine(3);
	AddSLine(21);
	iclr = COL_WHITE;

	if (boyitem._iMagical != ItemQuality::normal) iclr = COL_BLUE;
	if (!boyitem._iStatFlag) iclr = COL_RED;
	if (boyitem._iMagical != ItemQuality::normal)
		AddSText(20, 10, false, boyitem._iIName, iclr, true);
	else
		AddSText(20, 10, false, boyitem._iName, iclr, true);

	AddSTextVal(10, boyitem._iIvalue + (boyitem._iIvalue >> 1));
	PrintStoreItem(&boyitem, 11, iclr);
	AddSText(0, 22, true, "Leave", COL_WHITE, true);
	OffsetSTextY(22, 6);
}

void StoreBoy::S_BuyEnter()
{
	bool done;
	int i;

	if (stextsel == 10) {
		stextshold = StoreTalkId::BBOY;
		stextvhold = stextsval;
		stextlhold = 10;
		if (myplr().data._pGold < boyitem._iIvalue + (boyitem._iIvalue >> 1)) {
			StartStore(StoreTalkId::NOMONEY);
		} else {
			myplr().data.HoldItem = boyitem;
			myplr().data.HoldItem._iIvalue +=
			    myplr().data.HoldItem._iIvalue >> 1;
			SetCursor_(myplr().data.HoldItem._iCurs + Cursor::FIRSTITEM);
			done = false;
			for (i = 0; i < 40 && !done; i++) {
				done = myplr().inv.AutoPlace(i, {cursW / 28, cursH / 28},
				                                   false);
			}
			if (done)
				StartStore(StoreTalkId::CONFIRM);
			else
				StartStore(StoreTalkId::NOROOM);
			SetCursor_(Cursor::HAND);
		}
	} else {
		stextflag = StoreTalkId::NONE;
	}
}

}
