/**
 * @file inv.cpp
 *
 * Implementation of player inventory.
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

uint8_t *pInvCels = 0;
int users = 0;

Item *const PlayerInventory::findBagItemType(int itemType)
{
	for (InvSlot &i : getBagArray()) {
		Item *item = i.item();
		if (!item) continue;
		if (item->_itype == itemType) return item;
	}
	return 0;
}

Item * PlayerInventory::GetGoldSeed()
{
	Item *item = findBagItemType(ItemType::gold);

	if (!item) {
		InvSlot *slot = getEmptyBagSlot();
		if (slot) {
			slot->takeItem(std::make_unique<Item>());
			return slot->item();
		}
	}
	return 0;
}

bool PlayerInventory::stashHeldIntoBag(bool checkOnly)
{
	// Find empty spot
	const int numrows = 10;
	const int numcols = 4;
	bool goodpos = false;
	int i, j;

	// If gold, handle specially (eventually will encompass all stackables)
	if (getHeldItem()->_itype == ItemType::gold) {
		Item *item = GetGoldSeed();
		if (item) {
			if (!checkOnly) AddHeldGoldToStack(item);
			return true;
		}
		return false;
	}

	Item *item = getBagSlot(i, j).item();
	const V2Di size = {cursW / 28, cursH / 28};  // Very shit way of doing this

	for (i = 0; !goodpos && i <= numrows - size.x; ++i) {
		for (j = 0; !goodpos && j <= numcols - size.y; ++j) {
			if (getBagSlot(i, j).item()) {
				continue;
			} else {
				// Check if required number of spots left and right are also empty
				goodpos = true;
				for (int k = 0; goodpos && k < size.x; ++k) {
					for (int l = 0; goodpos && l < size.y; ++l) {
						if (getBagSlot(i, j).item()) {
							goodpos = false;
						}
					}
				}
			}
		}
	}
	if (!goodpos || checkOnly) return goodpos;

	// Stash in bag
	SwapHeldItem(getBagSlot(i, j));
	for (int k = 0; k < size.x; ++k) {
		for (int l = 0; l < size.y; ++l) {
			getBagSlot(i + k, j + l).filled = true;
		}
	}
	return goodpos;
}

bool PlayerInventory::stashHeldIntoBelt(bool checkOnly)
{
	for (int i = 0; i < MAXBELTITEMS; i++) {
		if (!getBeltSlot(i).item()) {
			if (!checkOnly) {
				SwapHeldItem(getBeltSlot(i));
			}
			return true;
		}
	}
	return false;
}

void PlayerInventory::AddHeldGoldToStack(Item * item)
{
	assert(item && item->_itype == ItemType::gold);
	Item *held = getHeldItem();
	if (!held) return;
	item->_ivalue += held->_ivalue;
	if (item->_ivalue >= GOLD_MEDIUM_LIMIT) {
		item->_iCurs = ItemCursor::GOLD_LARGE;
	} else if (item->_ivalue >= GOLD_SMALL_LIMIT) {
		item->_iCurs = ItemCursor::GOLD_MEDIUM;
	} else {
		item->_iCurs = ItemCursor::GOLD_SMALL; }
	owner.data._pGold = CalculateGold();
}

bool PlayerInventory::tryEquipHeld(bool checkOnly)
{
	if (tryPasteSlot(getBodySlot(BodyLoc::HandLeft), checkOnly, false)) return true;
	if (tryPasteSlot(getBodySlot(BodyLoc::HandRight), checkOnly, false)) return true;
	if (tryPasteSlot(getBodySlot(BodyLoc::Head), checkOnly, false)) return true;
	if (tryPasteSlot(getBodySlot(BodyLoc::Chest), checkOnly, false)) return true;
	if (tryPasteSlot(getBodySlot(BodyLoc::Amulet), checkOnly, false)) return true;
	if (tryPasteSlot(getBodySlot(BodyLoc::RingLeft), checkOnly, false)) return true;
	if (tryPasteSlot(getBodySlot(BodyLoc::RingRight), checkOnly, false)) return true;
	return false;
}

bool PlayerInventory::stashHeld(bool checkOnly)
{
	bool success = false;
	if (!success) success = tryEquipHeld(checkOnly);
	if (!success) success = stashHeldIntoBelt(checkOnly);
	if (!success) success = stashHeldIntoBag(checkOnly);
	return success;
}

InvSlot * PlayerInventory::GetCursorInvSlot(V2Di pos)
{
	InvSlot *invslot = 0;
	auto _over = [&](InvSlot &slot) -> bool {
		RECT32 &rect = slot.rect;
		if ((pos.x >= rect.x && pos.x <= (rect.x + rect.w)) &&
		    (pos.y >= rect.y && pos.y <= (rect.y + rect.h))) {
			invslot = &slot;
			return true;
		}
	};

	// Check over body slots
	if (_over(getBodySlot(BodyLoc::Head))) return invslot;
	if (_over(getBodySlot(BodyLoc::HandLeft))) return invslot;
	if (_over(getBodySlot(BodyLoc::HandRight))) return invslot;
	if (_over(getBodySlot(BodyLoc::Chest))) return invslot;
	if (_over(getBodySlot(BodyLoc::Amulet))) return invslot;
	if (_over(getBodySlot(BodyLoc::RingLeft))) return invslot;
	if (_over(getBodySlot(BodyLoc::RingRight))) return invslot;

	// Check over belt slots
	for (InvSlot &i : getBeltArray()) {
		if (_over(i)) return invslot;
	}

	// Check over bag slots
	for (InvSlot &i : getBagArray()) {
		if (_over(i)) return invslot;
	}
	return 0;
}

// Try pasting at the given position
bool PlayerInventory::tryPasteSlot(InvSlot &slot, bool checkOnly, bool forceSwap)
{
	const Item *const held = getHeldItem();
	if (!held) return false;  // Not holding an item
	if (slot.item() && !forceSwap) return false; // Item already present and not swapping
	bool success = false;
	const BodyLoc bodyloc = slot.bLoc;
	const ItemSlot itemClass = held->_iLoc;

	// Handle simple cases
	auto _simplePaste = [&](BodyLoc loc, ItemSlot cls) {
		if (!success && bodyloc == loc && itemClass == cls) {
			if (!checkOnly) {
				NetSendCmdChItem(false, loc);
				SwapHeldItem(slot);
			}
			success = true;
		}
	};

	_simplePaste(BodyLoc::Head, ItemSlot::Helm);
	_simplePaste(BodyLoc::RingLeft, ItemSlot::Ring);
	_simplePaste(BodyLoc::RingRight, ItemSlot::Ring);
	_simplePaste(BodyLoc::Chest, ItemSlot::Armor);
	_simplePaste(BodyLoc::Amulet, ItemSlot::Amulet);

	// Add to weapons slot
	if (!success && (bodyloc == BodyLoc::HandRight || bodyloc == BodyLoc::HandLeft)) {

		Item *left = getBodySlot(BodyLoc::HandLeft).item();
		Item *right = getBodySlot(BodyLoc::HandLeft).item();

		// One-handed
		if (itemClass == ItemSlot::OneHand) {
			if (!left || forceSwap) {
				if (!checkOnly) {
					SwapHeldItem(getBodySlot(BodyLoc::HandLeft));
					NetSendCmdChItem(false, BodyLoc::HandLeft);
				}
				success = true;
			}
		}

		// Two-handed
		if (!success && itemClass == ItemSlot::TwoHand) {
			if (!left || !right) {  // At least one hand empty
				if ((!left && !right) || forceSwap) {
					if (!checkOnly) {
						if (right) {  // Previous item in right hand, swap hands
							          // first
							SwapSlots(getBodySlot(BodyLoc::HandLeft),
							          getBodySlot(BodyLoc::HandRight));
						}
						SwapHeldItem(getBodySlot(BodyLoc::HandLeft));
						NetSendCmdChItem(false, BodyLoc::HandLeft);
						NetSendCmdChItem(false, BodyLoc::HandRight);
					}
					success = true;
				}
			}
		}

		// Shield
		if (!success && held->_iClass == ItemType::shield) {
			// swap with right unless two-handed in left
			if (left->_iLoc != ItemSlot::TwoHand) {
				if (!right || forceSwap) {
					if (!checkOnly) {
						SwapHeldItem(getBodySlot(BodyLoc::HandRight));
						NetSendCmdChItem(false, BodyLoc::HandRight);
					}
					success = true;
				}
			}
		}
	}

	// Add item to belt
	if (!success && bodyloc == BodyLoc::Belt) {
		bool valid = true;
		const V2Di size = {cursW / 28, cursH / 28};  // Very shit way of doing this
		if (size.maxabs() != 1) valid = false;  // Too big
		if (!AllItemsList[getHeldItem()->IDidx].iUsable)
			valid = false;                                       // Not usable
		if (!getHeldItem()->_iStatFlag) valid = false;           // ??
		if (getHeldItem()->_itype == ItemType::gold) valid = false;  // Gold
		if (valid) {
			if (!checkOnly) { SwapHeldItem(slot); }
			success = true;
		}
	}

	// Add item to bag
	if (!success && bodyloc == BodyLoc::Bag) {
		if (held->_itype == ItemType::gold) {
			if (!slot.item()) {  // If over empty slot, make new pile
				slot.takeItem(std::make_unique<Item>());
				slot.item()->_itype = ItemType::gold;
			}
			if (slot.item()->_itype = ItemType::gold) {
				AddHeldGoldToStack(slot.item());
				success = true;
			}
		} else {  // Not gold, place item in inventory
		}
	}
	return false;
}

bool PlayerInventory::tryPaste(V2Di pos)
{
	if (!getHeldItem()) return false;
	InvSlot * slotptr = GetCursorInvSlot(pos);
	if (!slotptr) return false;  // Not over slot
	InvSlot & slot = *slotptr;
	if (tryPasteSlot(slot, false, true)) {
		owner.CalcPlrInv(true);
		owner.CalcPlrStaff();
		if (owner.isMyPlr()) PlaySFX(ItemInvSnds[ItemCAnimTbl[getHeldItem()->_iCurs]]);
		return true;
	}
	if (owner.data._pClass == PC_WARRIOR)
		PlaySFX(PS_WARR13);
	else if (owner.data._pClass == PC_ROGUE)
		PlaySFX(PS_ROGUE13);
	else if (owner.data._pClass == PC_SORCERER)
		PlaySFX(PS_MAGE13);
	//if (owner.data._pClass == PC_WARRIOR) {
	//	PlaySFX(random_(0, 3) + PS_WARR14);
	//} else if (owner.data._pClass == PC_ROGUE) {
	//	PlaySFX(random_(0, 3) + PS_ROGUE14);
	//} else if (owner.data._pClass == PC_SORCERER) {
	//	PlaySFX(random_(0, 3) + PS_MAGE14);
	//}
	return false;
}
bool PlayerInventory::tryCut(V2Di pos)
{
	if (getHeldItem()) return false;                 // Already holding an item
	if (owner.data._pmode > PM_WALK3) return false;  // Doing action, can't pick up
	InvSlot *slot = GetCursorInvSlot(pos);		  	 // Get cursor inventory slot
	if (!slot) return false;                         // Not over a slot
	tryCutSlot(*slot);
}

bool PlayerInventory::tryCutSlot(InvSlot &slot)
{
	if (!slot.item()) return;                 // Slot is empty
	// Handle simple cases
	auto _simpleBodyCut = [&](BodyLoc loc) {
		if (slot.bLoc == loc) {
			NetSendCmdDelItem(false, loc);
			SwapHeldItem(slot);
		}
	};

	_simpleBodyCut(BodyLoc::Head);
	_simpleBodyCut(BodyLoc::RingLeft);
	_simpleBodyCut(BodyLoc::RingRight);
	_simpleBodyCut(BodyLoc::Amulet);
	_simpleBodyCut(BodyLoc::HandLeft);
	_simpleBodyCut(BodyLoc::HandRight);
	_simpleBodyCut(BodyLoc::Chest);

	if (slot.bLoc == BodyLoc::Belt || slot.bLoc == BodyLoc::Bag) {
		// Need to worry about size somewhere
		SwapHeldItem(slot);
	}

	if (getHeldItem()->_itype != ItemType::none) {
		if (getHeldItem()->_itype == ItemType::gold) {
			owner.data._pGold = CalculateGold();
		}
		owner.CalcPlrInv(true);
		CheckItemStats();
		if (owner.isMyPlr()) {
			PlaySFX(IS_IGRAB);
			SetCursor_(getHeldItem()->_iCurs + CURSOR_FIRSTITEM);
			//SetCursorPos(pos.x - (cursW >> 1), Mouse.y - (cursH >> 1));
		}
	}
}

void PlayerInventory::CheckInvItem()
{
	if (pcurs >= CURSOR_FIRSTITEM) {
		tryPaste(Mouse);
	} else {
		tryCut(Mouse);
	}
}

void PlayerInventory::destroyItem(InvSlot & slot)
{
	slot.destroyItem();
	owner.CalcPlrInv(true);
	owner.CalcPlrScrolls();
}

void PlayerInventory::CheckInvScrn()
{
	if (Mouse.x > 190 + PANEL_LEFT && Mouse.x < 437 + PANEL_LEFT
	    && Mouse.y > PANEL_TOP && Mouse.y < 33 + PANEL_TOP) {
		CheckInvItem();
	}
}

void PlayerInventory::CheckItemStats()
{
	PlayerStruct *p = &owner.data;
	getHeldItem()->_iStatFlag = false;
	if (p->_pStrength >= getHeldItem()->_iMinStr
	    && p->_pMagic >= getHeldItem()->_iMinMag
	    && p->_pDexterity >= getHeldItem()->_iMinDex) {
		getHeldItem()->_iStatFlag = true;
	}
}

void PlayerInventory::CheckBookLevel()
{
	if (getHeldItem()->_iMiscId == MiscItemId::BOOK) {
		getHeldItem()->_iMinMag = spelldata[getHeldItem()->_iSpell].sMinInt;
		int slvl = owner.data._pSplLvl[getHeldItem()->_iSpell];
		while (slvl != 0) {
			getHeldItem()->_iMinMag += 20 * getHeldItem()->_iMinMag / 100;
			slvl--;
			if (getHeldItem()->_iMinMag + 20 * getHeldItem()->_iMinMag / 100 > 255) {
				getHeldItem()->_iMinMag = -1;
				slvl = 0;
			}
		}
	}
}

void PlayerInventory::CheckQuestItem()
{
	if (getHeldItem()->IDidx == ItemIndex::OPTAMULET)
		quests[Q_BLIND]._qactive = QUEST_DONE;
	if (getHeldItem()->IDidx == ItemIndex::MUSHROOM && quests[Q_MUSHROOM]._qactive == QUEST_ACTIVE && quests[Q_MUSHROOM]._qvar1 == QS_MUSHSPAWNED) {
		sfxdelay = 10;
		if (owner.data._pClass == PC_WARRIOR) { // BUGFIX: Voice for this quest might be wrong in MP
			sfxdnum = PS_WARR95;
		} else if (owner.data._pClass == PC_ROGUE) {
			sfxdnum = PS_ROGUE95;
		} else if (owner.data._pClass == PC_SORCERER) {
			sfxdnum = PS_MAGE95;
		}
		quests[Q_MUSHROOM]._qvar1 = QS_MUSHPICKED;
	}
	if (getHeldItem()->IDidx == ItemIndex::ANVIL) {
		if (quests[Q_ANVIL]._qactive == QUEST_INIT) {
			quests[Q_ANVIL]._qactive = QUEST_ACTIVE;
			quests[Q_ANVIL]._qvar1 = 1;
		}
		if (quests[Q_ANVIL]._qlog == true) {
			sfxdelay = 10;
			if (myplr().data._pClass == PC_WARRIOR) {
				sfxdnum = PS_WARR89;
			} else if (myplr().data._pClass == PC_ROGUE) {
				sfxdnum = PS_ROGUE89;
			} else if (myplr().data._pClass == PC_SORCERER) {
				sfxdnum = PS_MAGE89;
			}
		}
	}
	if (getHeldItem()->IDidx == ItemIndex::GLDNELIX) {
		sfxdelay = 30;
		if (myplr().data._pClass == PC_WARRIOR) {
			sfxdnum = PS_WARR88;
		} else if (myplr().data._pClass == PC_ROGUE) {
			sfxdnum = PS_ROGUE88;
		} else if (myplr().data._pClass == PC_SORCERER) {
			sfxdnum = PS_MAGE88;
		}
	}
	if (getHeldItem()->IDidx == ItemIndex::ROCK) {
		if (quests[Q_ROCK]._qactive == QUEST_INIT) {
			quests[Q_ROCK]._qactive = QUEST_ACTIVE;
			quests[Q_ROCK]._qvar1 = 1;
		}
		if (quests[Q_ROCK]._qlog == true) {
			sfxdelay = 10;
			if (myplr().data._pClass == PC_WARRIOR) {
				sfxdnum = PS_WARR87;
			} else if (myplr().data._pClass == PC_ROGUE) {
				sfxdnum = PS_ROGUE87;
			} else if (myplr().data._pClass == PC_SORCERER) {
				sfxdnum = PS_MAGE87;
			}
		}
	}
	if (getHeldItem()->IDidx == ItemIndex::ARMOFVAL) {
		quests[Q_BLOOD]._qactive = QUEST_DONE;
		sfxdelay = 20;
		if (myplr().data._pClass == PC_WARRIOR) {
			sfxdnum = PS_WARR91;
		} else if (myplr().data._pClass == PC_ROGUE) {
			sfxdnum = PS_ROGUE91;
		} else if (myplr().data._pClass == PC_SORCERER) {
			sfxdnum = PS_MAGE91;
		}
	}
}

// Gets item from ground and puts in hand
void PlayerInventory::PickupItem(V2Di pos)
{
	if (grid.at(pos).isItem()) {
		if (owner.isMyPlr() && pcurs >= CURSOR_FIRSTITEM)
			NetSendCmdPItem(true, CMD_SYNCPUTITEM, myplr().pos().x, myplr().pos().y); //CmdPItem = player-held item?
		grid.at(pos).getItem()->_iCreateInfo &= ~0x8000;
		SwapHeldItem(grid.at(pos)._getItemPtr());
		pcursitem = 0;
		SetCursor_(getHeldItem()->_iCurs + CURSOR_FIRSTITEM);
	}
}

// Gets item from ground
void PlayerInventory::PickupAndStashItem(V2Di pos)
{
	PickupItem(pos);
	stashHeld(false);
	// NetSendCmdPItem(true, CMD_RESPAWNITEM, item._i.x, item._i.y);
}

//int PlayerInventory::FindGetItem(int idx, WORD ci, int iseed)
//{
//	int i, ii;
//	i = 0;
//	if (numitems <= 0)
//		return -1;
//
//	while (1) {
//		ii = itemactive[i];
//		if (item[ii].IDidx == idx && item[ii]._iSeed == iseed && item[ii]._iCreateInfo == ci)
//			break;
//
//		i++;
//
//		if (i >= numitems)
//			return -1;
//	}
//	return ii;
//}

//void PlayerInventory::SyncGetItem(V2Di pos, int idx, WORD ci, int iseed)
//{
//	int x = pos.x;
//	int y = pos.y;
//	int i, ii;
//
//	if (grid[x][y].isItem()) {
//		ii = grid[x][y].getItem();
//		if (item[ii].IDidx == idx
//		    && item[ii]._iSeed == iseed
//		    && item[ii]._iCreateInfo == ci) {
//			FindGetItem(idx, ci, iseed);
//		} else {
//			ii = FindGetItem(idx, ci, iseed);
//		}
//	} else {
//		ii = FindGetItem(idx, ci, iseed);
//	}
//
//	if (ii != -1) {
//		grid[item[ii]._i.x][item[ii]._i.y].clearItem();
//		i = 0;
//		while (i < numitems) {
//			if (itemactive[i] == ii) {
//				DeleteItem(itemactive[i], i);
//				FindGetItem(idx, ci, iseed);
//				/// ASSERT: assert(FindGetItem(idx,ci,iseed) == -1);
//				FindGetItem(idx, ci, iseed); /* todo: replace with above */
//				i = 0;
//			} else {
//				i++;
//			}
//		}
//		/// ASSERT: assert(FindGetItem(idx, ci, iseed) == -1);
//		FindGetItem(idx, ci, iseed); /* todo: replace with above */
//	}
//}

bool PlayerInventory::GetDropPos(V2Di & pos)
{
	Dir dir = GetDirection(pos, cursm);
	for (int i = 0; i < 8; ++i) {
		V2Di off = offset(dir);
		if (grid.at(myplr().pos() + offset(dir)).canPutItem()) {
			return true;
		}
		dir = right(dir);
	}
	return grid.at(myplr().pos()).canPutItem();
}

Item * PlayerInventory::DropItem(V2Di pos)
{
	Item * held = getHeldItem();
	if (!held) return 0;
	Dir d2 = GetDirection(owner.pos(), pos);
	V2Di off = offset(d2);
	V2Di n = pos - owner.pos();
	if (abs(n.x) > 1 || abs(n.y) > 1) {
		pos = owner.pos() + off; // Use new pos
	}
	if (!GetDropPos(pos)) return;
	held->_i = pos;
	held->RespawnItem(true);
	SwapHeldItem(grid.at(pos)._getItemPtr());
	NewCursor(CURSOR_HAND);
	return held;
	// Should release at some point?
}

// For when a non-local player drops an item
// Note: destroys item that was in that position before, hope you wanted to do that!
Item * PlayerInventory::DropItemSync(V2Di pos, int idx, WORD icreateinfo, int iseed, int Id, int dur, int mdur, int ch, int mch, int ivalue, DWORD ibuff)
{
	auto item = std::make_unique<Item>();
	item->RecreateItem(idx, icreateinfo, iseed, ivalue);
	if (Id) item->_iIdentified = true;
	item->_iDurability = dur;
	item->_iMaxDur = mdur;
	item->_iCharges = ch;
	item->_iMaxCharges = mch;
	item->_i = pos;
	item->RespawnItem(true);
	grid.at(pos).swapItem(item);
	return grid.at(pos).getItem();
	assert(!item); // Destroyed an item already on the ground
}

bool PlayerInventory::UseScroll(bool checkOnly)
{
	if (pcurs != CURSOR_HAND) return false;
	if (lvl.type() == DunType::town && !spelldata[myplr().data._pRSpell].sTownSpell)
		return false;

	for (InvSlot &i : getBagArray()) {
		Item *item = i.item();
		if (!item) continue;
		if (item->_itype != ItemType::none &&
		    (item->_iMiscId == MiscItemId::SCROLL ||
		     item->_iMiscId == MiscItemId::SCROLLT) &&
		    item->_iSpell == myplr().data._pRSpell) {
			if (!checkOnly) {
				destroyItem(i);
				owner.CalcPlrScrolls();
			}
			return true;
		}
	}

	for (int i = 0; i < MAXBELTITEMS; i++) {
		Item *item = getBeltSlot(i).item();
		if (!item) continue;
		if (item->_itype != ItemType::none
		    && (item->_iMiscId == MiscItemId::SCROLL || item->_iMiscId == MiscItemId::SCROLLT)
		    && item->_iSpell == myplr().data._pRSpell) {
			if (!checkOnly) {
				destroyItem(getBeltSlot(i));
				owner.CalcPlrScrolls();
			}
			return true;
		}
	}
	return false;
}

bool PlayerInventory::UseStaff(bool checkOnly)
{
	if (pcurs == CURSOR_HAND) {
		Item * item = getBodySlot(BodyLoc::HandLeft).item();
		if (item
			&& item->_itype != ItemType::none
			&& item->_iMiscId == MiscItemId::STAFF
			&& item->_iSpell == myplr().data._pRSpell
			&& item->_iCharges > 0) {
			if (!checkOnly) {
				item->_iCharges--;
				owner.CalcPlrStaff();
			}
			return true;
		}
	}
	return false;
}

bool PlayerInventory::UseInvItem(InvSlot & slot)
{
	Item *item = slot.item();
	if (!item) return;

	if (owner.data._pInvincible && !owner.data._pHitPoints && owner.isMyPlr())
		return true;
	if (pcurs != CURSOR_HAND)
		return true;
	if (stextflag != StoreTalkId::NONE)
		return true;
	if (talkflag) return true;

	switch (item->IDidx) {
	case ItemIndex::MUSHROOM:
		sfxdelay = 10;
		if (owner.data._pClass == PC_WARRIOR) {
			sfxdnum = PS_WARR95;
		} else if (owner.data._pClass == PC_ROGUE) {
			sfxdnum = PS_ROGUE95;
		} else if (owner.data._pClass == PC_SORCERER) {
			sfxdnum = PS_MAGE95;
		}
		return true;
	case ItemIndex::FUNGALTM:
		PlaySFX(IS_IBOOK);
		sfxdelay = 10;
		if (owner.data._pClass == PC_WARRIOR) {
			sfxdnum = PS_WARR29;
		} else if (owner.data._pClass == PC_ROGUE) {
			sfxdnum = PS_ROGUE29;
		} else if (owner.data._pClass == PC_SORCERER) {
			sfxdnum = PS_MAGE29;
		}
		return true;
	}

	if (!AllItemsList[item->IDidx].iUsable)
		return false;

	if (!item->_iStatFlag) {
		if (owner.data._pClass == PC_WARRIOR) {
			PlaySFX(PS_WARR13);
		} else if (owner.data._pClass == PC_ROGUE) {
			PlaySFX(PS_ROGUE13);
		} else if (owner.data._pClass == PC_SORCERER) {
			PlaySFX(PS_MAGE13);
		}
		return true;
	}

	if (item->_iMiscId == MiscItemId::SCROLL && lvl.currlevel == 0 && !spelldata[item->_iSpell].sTownSpell) {
		return true;
	}

	if (item->_iMiscId == MiscItemId::SCROLLT && lvl.currlevel == 0 && !spelldata[item->_iSpell].sTownSpell) {
		return true;
	}

	int idata = ItemCAnimTbl[item->_iCurs];
	if (item->_iMiscId == MiscItemId::BOOK)
		PlaySFX(IS_RBOOK);
	else if (owner.isMyPlr())
		PlaySFX(ItemInvSnds[idata]);

	owner.UseItem(item->_iMiscId, item->_iSpell);
	destroyItem(slot);
	return true;
}

void PlayerInventory::DoTelekinesis()
{
	if (pcursobj != -1)
		NetSendCmdParam1(true, CMD_OPOBJT, pcursobj);
	if (pcursitem)
		NetSendCmdGItem(true, CMD_REQUESTAGITEM, myplr(), myplr(), *pcursitem);
	if (pcursmonst != -1 && !monsters[pcursmonst].M_Talker() && monsters[pcursmonst].data.mtalkmsg == 0)
		NetSendCmdParam1(true, CMD_KNOCKBACK, pcursmonst);
	NewCursor(CURSOR_HAND);
}

int PlayerInventory::CalculateGold()
{
	int gold = 0;
	for (InvSlot &i : getBagArray()) {
		if (!i.item()) continue;
		if (i.item()->_itype == ItemType::gold)
			gold += i.item()->_ivalue;
	}
	assert(gold >= 0);
	return gold;
}

bool PlayerInventory::DropItemBeforeTrig()
{
	if (GetDropPos(owner.pos()))
	{
		NetSendCmdPItem(true, CMD_PUTITEM, cursm.x, cursm.y);
		NewCursor(CURSOR_HAND);
		return true;
	}
	return false;
}

DEVILUTION_END_NAMESPACE
