/**
 * @file items.h
 *
 * Item instance
 */
#ifndef __ITEM_INSTANCE_H__
#define __ITEM_INSTANCE_H__

#include "item_id.h"

DEVILUTION_BEGIN_NAMESPACE

class Item : public ItemStruct {
public:
	Item(ItemID nid);
	Item(ItemID nid, Player *);
	Item(Player *);
	Item();
	~Item();

	Item(const Item &) = delete; // no explicit copy constructor
	Item &operator=(const Item &) = delete; // no explicit copies

	Player &owner();
	ItemID id;

	bool GetItemSpace(V2Di pos);
	void GetSuperItemSpace(V2Di pos);
	void CalcItemValue();

	void GetBookSpell(int lvl);
	void GetStaffPower(int lvl, int bs, bool onlygood);
	void GetStaffSpell(int lvl, bool onlygood);
	void GetItemAttrs(int idata, int lvl);
	void SaveItemPower(int power, int param1, int param2, int minval, int maxval, int multval);
	void GetItemPower(int minlvl, int maxlvl, int flgs, bool onlygood);
	void GetItemBonus(int idata, int minlvl, int maxlvl, bool onlygood);
	void SetupItem();
	int CheckUnique(int lvl, int uper, bool recreate);
	void GetUniqueItem(int uid);
	void RespawnItem(bool FlipFlag);
	void GetItemFrm();
	void GetItemStr();

	void ItemRndDur();
	void SetupAllItems(int idx, int iseed, int lvl, int uper, int onlygood, bool recreate, bool pregen);
	void SetupAllUseful(int iseed, int lvl);

	void RecreateItem(int idx, WORD icreateinfo, int iseed, int ivalue);
	void RecreateEar(WORD ic, int iseed, int Id, int dur, int mdur, int ch, int mch, int ivalue, int ibuff);
	void RecreateSmithItem(int idx, int lvl, int iseed);
	void RecreatePremiumItem(int idx, int plvl, int iseed);
	void RecreateBoyItem(int idx, int lvl, int iseed);
	void RecreateWitchItem(int idx, int lvl, int iseed);
	void RecreateHealerItem(int idx, int lvl, int iseed);
	void RecreateTownItem(int idx, WORD icreateinfo, int iseed, int ivalue);
private:
	Player *_owner = 0;
};

DEVILUTION_END_NAMESPACE

#endif /* __ITEM_INSTANCE_H__ */
