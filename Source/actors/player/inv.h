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

DEVILUTION_BEGIN_NAMESPACE

class Player;
class ItemID;

class PlayerInventory {
public:
	// Init
	PlayerInventory(Player &);
	void InitInv();
	void FreeInvGFX();

	// Misc
	InvSlot * GetCursorInvSlot(V2Di pos);

	// Draw
	void DrawInvSlotBack(RECT32 r);
	void DrawBodySlot(BodyLoc bodyloc);
	void DrawInvSlots();
	void DrawInvBelt();
	void DrawInv();
	void DrawInvMsg(char *msg);
	char CheckInvHLight();

	// Place into inventory
	bool AutoPlaceBag(V2Di size, bool saveflag);		// Place in bag
	bool AutoPlaceBelt(bool saveflag);					// Place in belt
	bool AutoPlaceSpecial(V2Di size, bool saveflag);	// Place anywhere
	bool AutoPlaceGold();

	// Pick up item
	void PickupItem(ItemID id);
	void AutoPickUpItem(Item &item);

	// Drop item
	bool GetDropPos(V2Di & pos); // returns valid pos
	Item *DropItem(V2Di pos);
	Item *DropItemSync(V2Di pos, int idx, WORD icreateinfo, int iseed, int Id, int dur, int mdur, int ch, int mch, int ivalue, DWORD ibuff);
	bool DropItemBeforeTrig();

	// Cut and paste from inventory
	void CheckInvPaste(V2Di pos);
	void CheckInvCut(V2Di pos);

	// Access inventory slots
	InvSlot & getBodySlot(BodyLoc) const;
	InvSlot & getBeltSlot(uint8_t) const;
	InvSlot & getBagSlot(uint8_t) const;
	InvSlot * getEmptyBagSlot() const;
	InvSlot * getHeldSlot() const;

	// Access inventory items
	Item * const getBodyItem(BodyLoc) const;
	Item * const getBeltItem(uint8_t) const;
	Item * const getBagItem(uint8_t) const;
	Item * const getBagItem(uint8_t, uint8_t) const;
	Item * const getHeldItem() const;
	//yy = 10 * (i / 10); //xx = i % 10;


	// Modify inventory items
	void removeItem(InvSlot &);
	void removeBeltItem(uint8_t);
	void removeBagItem(uint8_t);

	void setItem(const InvSlot &, const Item * const);
	void setBodyItem(BodyLoc, const Item *const);
	void setBagItem(uint8_t, uint8_t, const Item * const);
	void setBeltItem(uint8_t, const Item * const);
	void setHeldItem(const Item *const);

	// Find Items
	Item *const findBagItemType(int);

	// Misc
	bool TryArmWeapon();
	void CheckItemStats();
	void CheckBookLevel();
	void CheckQuestItem();
	bool UseInvItem(InvSlot &);
	void UseStaffCharge();
	bool UseStaff();
	bool UseScroll();
	void RemoveScroll();
	int CalculateGold();
	int SwapItem(Item *a, Item *b);
	void CheckInvItem();
	void CheckInvScrn();
	//int FindGetItem(int idx, WORD ci, int iseed);
	//void SyncGetItem(V2Di pos, int idx, WORD ci, int iseed);
	void DoTelekinesis();
private:
	Player & owner;

	static bool invflag; // Inventory is open flag???
	static uint8_t *pInvCels;
	static int users;

	// Data
	struct {
		InvSlot head;
		InvSlot ring_left;
		InvSlot ring_right;
		InvSlot amulet;
		InvSlot hand_left;
		InvSlot hand_right;
		InvSlot chest;
		// InvSlot belt; (for DII)
	} body;

	struct {
		std::array<InvSlot, MAXBELTITEMS> slots;
	} belt;

	struct {
		std::array<InvSlot, NUM_INV_GRID_ELEM> grid;
	} bag;

	struct {
		InvSlot item;
	} hand;
};

DEVILUTION_END_NAMESPACE

#endif /* __INV_H__ */
