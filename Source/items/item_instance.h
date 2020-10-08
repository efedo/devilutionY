/**
 * @file items.h
 *
 * Item instance
 */
#ifndef __ITEM_INSTANCE_H__
#define __ITEM_INSTANCE_H__

#include "item_id.h"

namespace dvl {

class Item : public ItemStruct {
public:
	Item(ItemID nid);
	Item(ItemID nid, Player *);
	Item(Player *);
	Item();
	~Item();
	Item(const Item &) = delete;             // no explicit copy constructor
	Item &operator=(const Item &) = delete;  // no explicit copies
	Player &owner();

	// Init
	void SetupItem();
	void SetupAllItems(ItemIndex idx, int iseed, int lvl, int uper, int onlygood,
	                   bool recreate, bool pregen);
	void SetupAllUseful(int iseed, int lvl);
	void loadPresetAttributes(ItemIndex idata, int lvl);
	void GetUniqueItem(UniqueItemType uid);
	void ItemRndDur();  // Generate random durability

	// Basic
	V2Di getSize();

	// Finds grid spaces to drop item; move most of body over to grid
	bool GetItemSpace(V2Di pos);
	void GetSuperItemSpace(V2Di pos);

	// Graphics
	void GetItemFrm();  // Get animation frame
	void RespawnItem(bool FlipFlag);

	// Re-create item based on seed
	void RecreateItem(ItemIndex idx, WORD icreateinfo, int iseed, int ivalue);
	void RecreateTownItem(ItemIndex idx, WORD icreateinfo, int iseed, int ivalue);
	void RecreateSmithItem(ItemIndex idx, int lvl, int iseed);
	void RecreatePremiumItem(ItemIndex idx, int plvl, int iseed);
	void RecreateBoyItem(ItemIndex idx, int lvl, int iseed);
	void RecreateWitchItem(ItemIndex idx, int lvl, int iseed);
	void RecreateHealerItem(ItemIndex idx, int lvl, int iseed);

	// Item maintenance
	void Repair(int lvl);
	void Recharge(int r);

	// Describe item
	void GetItemStr();  // gets item text string
	void PrintMisc();
	void PrintDetails();
	void PrintDur();
	void PrintPower(ItemEffectType plidx);
	void PrintOil();

	bool StoreStatOk();

	// Misc
	void CalcItemValue();
	void GetBookSpell(int lvl);
	void GetStaffPower(int lvl, int bs, bool onlygood);
	void GetStaffSpell(int lvl, bool onlygood);
	void SaveItemPower(ItemEffectType power, int param1, int param2, int minval,
	                   int maxval, int multval);
	void GetItemPower(int minlvl, int maxlvl, ItemAffixFlags flgs, bool onlygood);
	void GetItemBonus(ItemIndex idata, int minlvl, int maxlvl, bool onlygood);
	UniqueItemType CheckUnique(int lvl, int uper, bool recreate);

   private:
	Player *_owner = 0;
	ItemID id; // only used for saving
};

void BubbleSwapItem(ItemStruct *a, ItemStruct *b);

bool StoreStatOk(ItemStruct *h);


}

#endif /* __ITEM_INSTANCE_H__ */
