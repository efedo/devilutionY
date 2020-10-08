/**
 * @file inv.h
 *
 * Interface of player inventory.
 */
#ifndef __INV_H__
#define __INV_H__

#include "items/item_enums.h"
#include "inv_enums.h"
#include "inv_slot.h"

namespace dvl {

class Player;
class ItemID;

class PlayerInventory {
public:
	// Init
	PlayerInventory(Player &);
	void InitInv();
	void FreeInvGFX();

	// Draw
	void DrawInvSlotBack(RECT32 r);
	void DrawBodySlot(BodyLoc bodyloc);
	void DrawInvSlots();
	void DrawInvBelt();
	void DrawInv();
	void DrawInvMsg(char *msg);
	char CheckInvHLight();

	// Cut and paste from inventory
	InvSlot *GetCursorInvSlot(V2Di pos);
	bool tryPasteSlot(InvSlot &slot, bool checkOnly, bool forceSwap);
	bool tryCutSlot(InvSlot &slot);
	bool tryPaste(V2Di pos);
	bool tryCut(V2Di pos);

	// Place into inventory (from HAND)
	Item * GetGoldSeed(); // Eventually will generalize this to all stackables
	void AddHeldGoldToStack(Item *item);  // Adds gold in hand to specified stack; Eventually will generalize this to all stackables
	bool tryEquipHeld(bool checkOnly);
	bool stashHeldIntoBag(bool checkOnly);		// Place in bag
	bool stashHeldIntoBelt(bool checkOnly);            // Place in belt
	bool stashHeld(bool checkOnly);  // Automatically tries to stash item from hand

	// Pick up item from ground
	void PickupItem(V2Di pos);
	void PickupAndStashItem(V2Di pos);

	// Drop item onto ground
	bool GetDropPos(V2Di & pos); // returns valid pos if available
	Item *DropItem(V2Di pos);
	Item *DropItemSync(V2Di pos, int idx, WORD icreateinfo, int iseed, int Id, int dur, int mdur, int ch, int mch, int ivalue, DWORD ibuff);
	bool DropItemBeforeTrig();

	// Access inventory slots
	InvSlot & getBodySlot(BodyLoc) const; // Enforce body only
	InvSlot & getBeltSlot(uint8_t) const;
	InvSlot & getBagSlot(uint8_t) const;
	InvSlot & getBagSlot(uint8_t, uint8_t) const; //yy = 10 * (i / 10); //xx = i % 10;
	InvSlot * getEmptyBagSlot() const;
	InvSlot & getHeldSlot() const;

	Item * getHeldItem() const; // FOr now, only funtion to directly return an item (based on freq. of usage)


	auto &getBodyArray()
	{
		return _body.slots;
	}

	auto & getBeltArray()
	{
		return _belt.slots;
	}

	auto &getBagArray()
	{
		return _bag.slots;
	}

	// Modify inventory items
	int SwapHeldItem(InvSlot &a);
	int SwapHeldItem(std::unique_ptr<Item> & a);
		/*CheckQuestItem();
		CheckBookLevel();
		CheckItemStats();*/

	//	CheckQuestItem();
	//CheckBookLevel();
	//CheckItemStats();
	int SwapSlots(InvSlot &a, InvSlot &b);  // Not for held items!... check!

	void destroyItem(InvSlot &);

	// Find Items
	Item *const findBagItemType(int);

	// Misc
	void CheckItemStats();
	void CheckBookLevel();
	void CheckQuestItem();
	bool UseInvItem(InvSlot &);
	bool UseStaff(bool checkOnly);
	bool UseScroll(bool checkOnly);
	int CalculateGold();
	void CheckInvItem();
	void CheckInvScrn();
	//int FindGetItem(int idx, WORD ci, int iseed);
	//void SyncGetItem(V2Di pos, int idx, WORD ci, int iseed);
	void DoTelekinesis();
private:
	Player & owner;

	static uint8_t *pInvCels;
	static int users;

	// Data
	struct {
		std::array<InvSlot, 7> slots;
		// InvSlot belt; (for DII)
	} _body;

	struct {
		std::array<InvSlot, MAXBELTITEMS> slots;
	} _belt;

	struct {
		std::array<InvSlot, MAXINVITEMS> slots;
	} _bag;

	struct {
		InvSlot item;
	} _hand;
};

}

#endif /* __INV_H__ */
