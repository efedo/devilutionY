/**
 * @file inv.cpp
 *
 * Implementation of player inventory.
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

bool PlayerInventory::invflag = false; // Inventory is open flag???
uint8_t *pInvCels = 0;
int users = 0;

bool PlayerInventory::AutoPlaceBag(V2Di size, bool saveflag)
{
	// Find empty spot
	const int numrows = 10;
	const int numcols = 4;
	bool goodpos = false;
	int i, j;
	for (i = 0; !goodpos && i <= numrows - size.x; ++i) {
		for (j = 0; !goodpos && j <= numcols - size.y; ++j) {
			if (getBagItem(i, j)) {
				continue;
			} else {
				// Check if required number of spots left and right are also empty
				goodpos = true;
				for (int k = 0; goodpos && k < size.x; ++k) {
					for (int l = 0; goodpos && l < size.y; ++l) {
						if (getBagItem(i, j)) {
							goodpos = false;
						}
					}
				}
			}
		}
	}

	// Place
	if (!goodpos || !saveflag)
		return goodpos;
	for (int k = 0; k < size.x; ++k) {
		for (int l = 0; l < size.y; ++l) {
			setBagItem(i + k, j + l, getHeldItem());
		}
	}
	return goodpos;
}

bool PlayerInventory::AutoPlaceBelt(bool saveflag)
{
	for (int i = 0; i < MAXBELTITEMS; i++) {
		if (!getBeltItem(i)) {
			if (saveflag) {
				setBeltItem(i, getHeldItem());
			}
			return true;
		}
	}
	return false;
}

bool PlayerInventory::AutoPlaceSpecial(V2Di size, bool saveflag)
{
	if (size.maxdim() <= 1) { // fits in belt
		if (AutoPlaceBelt(saveflag))
			return true;
	}
	return AutoPlaceBag(size, saveflag);
}

Item * const PlayerInventory::findBagItemType(int itemType)
{
	for (int i = 0; i < NUM_INV_GRID_ELEM; i++) {
		Item * item = getBagItem(i);
		if (!item) continue;
		if (item->_itype == itemType) return item;
	}
	return 0;
}

bool PlayerInventory::AutoPlaceGold()
{
	bool done = false;
	Item * item = findBagItemType(ITYPE_GOLD);

	if (!item) {
		InvSlot * slot = getEmptyBagSlot();
		if (slot) slot->item.reset(new Item);
	}
	
	if (item) {
		item->_ivalue += getHeldSlot()->item->_ivalue;
		if (item->_ivalue >= GOLD_MEDIUM_LIMIT)
			item->_iCurs = ICURS_GOLD_LARGE;
		else if (item->_ivalue >= GOLD_SMALL_LIMIT)
			item->_iCurs = ICURS_GOLD_MEDIUM;
		else item->_iCurs = ICURS_GOLD_SMALL;
		owner.data._pGold = CalculateGold();
		done = true;
	}
	return done;
}



bool PlayerInventory::TryArmWeapon()
{
	Item *item = getHeldItem();
	if (!item) return false;

	// Already armed
	Item * left = getBodyItem(BodyLoc::HandLeft);
	Item * right = getBodyItem(BodyLoc::HandRight);
	if (left && left->_iClass == ICLASS_WEAPON) return false;
	if (right && right->_iClass == ICLASS_WEAPON) return false;

	if (item->_iLoc != ItemClass::TwoHand) { // Single-handed
		if (!left) {
			NetSendCmdChItem(true, BodyLoc::HandLeft);
			setBodyItem(BodyLoc::HandLeft, item);
			return true;
		}

		if (!right && (!left || left->_iLoc != ItemClass::TwoHand)) {
			NetSendCmdChItem(true, BodyLoc::HandRight);
			setBodyItem(BodyLoc::HandRight, item);
			return true;
		}
	} else if (!left && !right) { // Two-handed and unarmed
		NetSendCmdChItem(TRUE, BodyLoc::HandLeft);
		setBodyItem(BodyLoc::HandLeft, item);
		return true;
	}
	return false;
}

int SwapItem(Item * a, Item * b)
{
	assert(a);
	Item * aold = a;
	std::swap(a, b);
	return aold->_iCurs + CURSOR_FIRSTITEM;
}

InvSlot * PlayerInventory::GetCursorInvSlot(V2Di pos)
{
	int i, j;
	int r, sx, sy;
	bool done;
	SetICursor(getHeldItem()->_iCurs + CURSOR_FIRSTITEM);
	i = pos.x + (icursW >> 1);
	j = pos.y + (icursH >> 1);
	sx = icursW28;
	sy = icursH28;

	// Check over body slot


	// Check over belt slot

	// Check over bag slot



	return 0;
}

void PlayerInventory::CheckInvPaste(V2Di pos)
{
	bool done = false;
	int xx, yy, ii;
	bool done2h;
	int cn, it, iv, ig, gt;
	ItemStruct tempitem;

	if (!getHeldItem())
		return;

	InvSlot * slotptr = GetCursorInvSlot(pos);
	if (!slotptr) return; // Not over slot
	InvSlot & slot = *slotptr;
	const BodyLoc bodyLoc = slot.bLoc;
	const ItemClass itemClass = getHeldItem()->_iLoc;
	const V2Di size = { cursW / 28, cursH / 28 }; // Very shit way of doing this

	// Handle simple cases
	auto _simplePaste = [&](ItemClass cls, BodyLoc loc) {
		if (!done && itemClass == cls && bodyLoc == loc) {
			NetSendCmdChItem(false, loc);
			if (!getBodyItem(loc)) {
				setBodyItem(loc, getHeldItem());
			} else {
				cn = SwapItem(getBodyItem(loc), getHeldItem());
			}
			done = true;
		}
	};

	_simplePaste(ItemClass::Helm, BodyLoc::Head);
	_simplePaste(ItemClass::Ring, BodyLoc::RingLeft);
	_simplePaste(ItemClass::Ring, BodyLoc::RingRight);
	_simplePaste(ItemClass::Armor, BodyLoc::Chest);
	_simplePaste(ItemClass::Amulet, BodyLoc::Amulet);
	_simplePaste(ItemClass::OneHand, BodyLoc::HandLeft);
	_simplePaste(ItemClass::OneHand, BodyLoc::HandRight);

	// Two-handed weapon to weapon slot
	if (!done && itemClass == ItemClass::TwoHand) {
		if (bodyLoc == BodyLoc::HandRight || bodyLoc == BodyLoc::HandLeft) {
			if (!getBodyItem(BodyLoc::HandLeft) || !getBodyItem(BodyLoc::HandRight)) {
				if (!getBodyItem(BodyLoc::HandLeft)) {     // Nothing in left hand
					if (getBodyItem(BodyLoc::HandRight)) { // If there was something in right hand, swap it left
						std::swap(body.hand_left.item, body.hand_right.item);
					}
				}
				if (!getBodyItem(BodyLoc::HandLeft)) {
					NetSendCmdChItem(false, BodyLoc::HandLeft);
					NetSendCmdChItem(false, BodyLoc::HandRight);
					setBodyItem(BodyLoc::HandLeft, getHeldItem());
				} else {
					NetSendCmdChItem(false, BodyLoc::HandLeft);
					NetSendCmdChItem(false, BodyLoc::HandRight);
					cn = SwapItem(getBodyItem(BodyLoc::HandLeft), getHeldItem());
				}
				done = true;
			}
		}
	}

	// Belt anything
	if (!done && bodyLoc == BodyLoc::Belt) {
		if (size.maxabs() == 1) {
			done = true;
			if (!AllItemsList[getHeldItem()->IDidx].iUsable)
				done = false;
			if (!getHeldItem()->_iStatFlag)
				done = false;
			if (getHeldItem()->_itype == ITYPE_GOLD)
				done = false;
		}
		if (done) {
			if (!slot.item) {
				slot.item = getHeldItem();
			} else {
				cn = SwapItem(slot.item, getHeldItem());
			}
		}
	}

	if (!done && bodyLoc == BodyLoc::Bag) {
		if (getHeldItem()->_itype == ITYPE_GOLD) {
			if (!slot.item) { // If over empty slot, make new pile
				slot.item = &items.createNewItem();
				slot.item->_itype = ITYPE_GOLD;
			}
			if (slot.item->_itype == ITYPE_GOLD) {
				slot.item->_ivalue += getHeldSlot()->item->_ivalue;
				if (slot.item->_ivalue >= GOLD_MEDIUM_LIMIT)
					slot.item->_iCurs = ICURS_GOLD_LARGE;
				else if (slot.item->_ivalue >= GOLD_SMALL_LIMIT)
					slot.item->_iCurs = ICURS_GOLD_MEDIUM;
				else
					slot.item->_iCurs = ICURS_GOLD_SMALL;
				owner.data._pGold = CalculateGold();
				done = true;
			}
		} else { // Not gold, place item in inventory
			
		}
	}

	CalcPlrInv(owner.id(), TRUE);

	//// Update staff spells etc.
	//if (owner.data.InvBody[INVLOC_HAND_LEFT]->_itype == ITYPE_STAFF && owner.data.InvBody[INVLOC_HAND_LEFT]->_iSpell != 0 && owner.data.InvBody[INVLOC_HAND_LEFT]->_iCharges > 0) {
	//	owner.data._pRSpell = owner.data.InvBody[INVLOC_HAND_LEFT]->_iSpell;
	//	owner.data._pRSplType = RSPLTYPE_CHARGES;
	//	force_redraw = 255;
	//}

	if (!done) {
		done = FALSE;
		if (owner.data._pClass == PC_WARRIOR)
			PlaySFX(PS_WARR13);
		else if (owner.data._pClass == PC_ROGUE)
			PlaySFX(PS_ROGUE13);
		else if (owner.data._pClass == PC_SORCERER)
			PlaySFX(PS_MAGE13);
	}

	if (owner.isMyPlr()) PlaySFX(ItemInvSnds[ItemCAnimTbl[getHeldItem()->_iCurs]]);
}

void PlayerInventory::CheckInvCut(V2Di pos)
{
	if (getHeldItem()) return; // Already holding an item
	if (owner.data._pmode > PM_WALK3) return; // Doing action, can't pick up
	InvSlot * slot = GetCursorInvSlot(pos); // Get cursor inventory slot
	if (!slot) return; // Not over a slot
	if (!slot->item) return; // Slot is empty

	// Handle simple cases
	auto _simpleCut = [&](BodyLoc loc) {
		if (slot->bLoc == loc) {
			NetSendCmdDelItem(false, loc);
			setHeldItem(slot->item);
			slot->item = 0;
		}
	};

	_simpleCut(BodyLoc::Head);
	_simpleCut(BodyLoc::RingLeft);
	_simpleCut(BodyLoc::RingRight);
	_simpleCut(BodyLoc::Amulet);
	_simpleCut(BodyLoc::HandLeft);
	_simpleCut(BodyLoc::HandRight);
	_simpleCut(BodyLoc::Chest);

	if (slot->bLoc == BodyLoc::Belt) {
		setHeldItem(slot->item);
		slot->item = 0;
	}

	if (slot->bLoc == BodyLoc::Bag) {
		setHeldItem(slot->item);
		slot->item = 0;
	}

	if (getHeldItem()->_itype != ITYPE_NONE) {
		if (getHeldItem()->_itype == ITYPE_GOLD) {
			owner.data._pGold = CalculateGold();
		}

		CalcPlrInv(owner.id(), TRUE);
		CheckItemStats();

		if (owner.isMyPlr()) {
			PlaySFX(IS_IGRAB);
			SetCursor_(getHeldItem()->_iCurs + CURSOR_FIRSTITEM);
			//SetCursorPos(pos.x - (cursW >> 1), Mouse.y - (cursH >> 1));
		}
	}
}

void PlayerInventory::removeItem(InvSlot & slot)
{
	slot.item = 0;
	CalcPlrInv(owner.id(), true);
	CalcPlrScrolls(owner.id());

	if (owner.data._pRSplType == RSPLTYPE_SCROLL) {
		if (owner.data._pRSpell != SPL_INVALID) {
			// BUGFIX: Cast the literal `1` to `unsigned __int64` to make that bitshift 64bit
			// this causes the last 4 skills to not reset correctly after use
			if (!(
			        owner.data._pScrlSpells
			        & (1 << (owner.data._pRSpell - 1)))) {
				owner.data._pRSpell = SPL_INVALID;
			}

			force_redraw = 255;
		}
	}
}

void PlayerInventory::removeBeltItem(uint8_t i)
{
	if (i >= MAXBELTITEMS) return;
	removeItem(getBeltSlot(i));
}

void PlayerInventory::CheckInvItem()
{
	if (pcurs >= CURSOR_FIRSTITEM) {
		CheckInvPaste(Mouse);
	} else {
		CheckInvCut(Mouse);
	}
}

/**
 * Check for interactions with belt
 */
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
	if (getHeldItem()->_iMiscId == IMISC_BOOK) {
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
	if (getHeldItem()->IDidx == IDI_OPTAMULET)
		quests[Q_BLIND]._qactive = QUEST_DONE;
	if (getHeldItem()->IDidx == IDI_MUSHROOM && quests[Q_MUSHROOM]._qactive == QUEST_ACTIVE && quests[Q_MUSHROOM]._qvar1 == QS_MUSHSPAWNED) {
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
	if (getHeldItem()->IDidx == IDI_ANVIL) {
		if (quests[Q_ANVIL]._qactive == QUEST_INIT) {
			quests[Q_ANVIL]._qactive = QUEST_ACTIVE;
			quests[Q_ANVIL]._qvar1 = 1;
		}
		if (quests[Q_ANVIL]._qlog == TRUE) {
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
	if (getHeldItem()->IDidx == IDI_GLDNELIX) {
		sfxdelay = 30;
		if (myplr().data._pClass == PC_WARRIOR) {
			sfxdnum = PS_WARR88;
		} else if (myplr().data._pClass == PC_ROGUE) {
			sfxdnum = PS_ROGUE88;
		} else if (myplr().data._pClass == PC_SORCERER) {
			sfxdnum = PS_MAGE88;
		}
	}
	if (getHeldItem()->IDidx == IDI_ROCK) {
		if (quests[Q_ROCK]._qactive == QUEST_INIT) {
			quests[Q_ROCK]._qactive = QUEST_ACTIVE;
			quests[Q_ROCK]._qvar1 = 1;
		}
		if (quests[Q_ROCK]._qlog == TRUE) {
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
	if (getHeldItem()->IDidx == IDI_ARMOFVAL) {
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

// Gets item from ground
void PlayerInventory::PickupItem(ItemID id)
{
	int i;

	if (!items.exists(id)) return;
	Item & item = items.get(id);
	if (grid.at(item._i).isItem()) {
		if (owner.isMyPlr() && pcurs >= CURSOR_FIRSTITEM)
			NetSendCmdPItem(TRUE, CMD_SYNCPUTITEM, myplr().pos().x, myplr().pos().y);
		item._iCreateInfo &= ~0x8000;
		setHeldItem(&item);
		CheckQuestItem();
		CheckBookLevel();
		CheckItemStats();
		grid.at(item._i).clearItem();
		i = 0;
		pcursitem = 0;
		SetCursor_(getHeldItem()->_iCurs + CURSOR_FIRSTITEM);
	}
}

// Gets item from ground
void PlayerInventory::AutoPickUpItem(Item &item)
{
	int i, idx;
	int w, h;
	bool done;

	if (pcurs != CURSOR_HAND) return;

	if (!grid.at(item._i).isItem()) return;

	item._iCreateInfo &= 0x7FFF;
	setHeldItem(&item); /// BUGFIX: overwrites cursor item, allowing for belt dupe bug
	CheckQuestItem();
	CheckBookLevel();
	CheckItemStats();
	SetICursor(getHeldItem()->_iCurs + CURSOR_FIRSTITEM);
	if (getHeldItem()->_itype == ITYPE_GOLD) {
		done = AutoPlaceGold();
	} else {
		done = FALSE;
		if (((owner.data._pgfxnum & 0xF) == ANIM_ID_UNARMED || (owner.data._pgfxnum & 0xF) == ANIM_ID_UNARMED_SHIELD) && owner.data._pmode <= PM_WALK3) {
			if (getHeldItem()->_iStatFlag) {
				if (getHeldItem()->_iClass == ICLASS_WEAPON) {
					done = TryArmWeapon();
					if (done)
						CalcPlrInv(owner.id(), TRUE);
				}
			}
		}
		if (!done) {
			w = icursW28;
			h = icursH28;
			if (w == 1 && h == 1) {
				idx = getHeldItem()->IDidx;
				if (getHeldItem()->_iStatFlag && AllItemsList[idx].iUsable) {
					done = AutoPlaceBelt(true);
				}
			}
			if (!done) done = AutoPlaceBag({ w, h }, true);
		}
	}
	if (done) {
		grid.at(item._i).clearItem();
	} else {
		if (owner.isMyPlr()) {
			if (owner.data._pClass == PC_WARRIOR) {
				PlaySFX(random_(0, 3) + PS_WARR14);
			} else if (owner.data._pClass == PC_ROGUE) {
				PlaySFX(random_(0, 3) + PS_ROGUE14);
			} else if (owner.data._pClass == PC_SORCERER) {
				PlaySFX(random_(0, 3) + PS_MAGE14);
			}
		}
		setHeldItem(&item);
		item.RespawnItem(TRUE);
		NetSendCmdPItem(TRUE, CMD_RESPAWNITEM, item._i.x, item._i.y);
		getHeldItem()->_itype = ITYPE_NONE;
	}
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
			return TRUE;
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
		pos = owner.pos() + off;
	}
	if (!GetDropPos(pos)) return;
	held->_i = pos;
	held->RespawnItem(TRUE);
	grid.at(pos).setItem(held->id);
	NewCursor(CURSOR_HAND);
	return held;
	// Should release at some point?
}

// For when a non-local player drops an item
Item * PlayerInventory::DropItemSync(V2Di pos, int idx, WORD icreateinfo, int iseed, int Id, int dur, int mdur, int ch, int mch, int ivalue, DWORD ibuff)
{
	Item & item = items.createNewItem();
	if (idx == IDI_EAR) {
		item.RecreateEar(icreateinfo, iseed, Id, dur, mdur, ch, mch, ivalue, ibuff);
	} else {
		item.RecreateItem(idx, icreateinfo, iseed, ivalue);
		if (Id) item._iIdentified = TRUE;
		item._iDurability = dur;
		item._iMaxDur = mdur;
		item._iCharges = ch;
		item._iMaxCharges = mch;
	}

	item._i = pos;
	item.RespawnItem(TRUE);
	grid.at(pos).setItem(item.id);
	return &item;
}

void PlayerInventory::RemoveScroll()
{
	for (int i = 0; i < NUM_INV_GRID_ELEM; i++) {
		Item *item = getBagItem(i);
		if (!item) continue;
		if (item->_itype != ITYPE_NONE
		    && (item->_iMiscId == IMISC_SCROLL || item->_iMiscId == IMISC_SCROLLT)
		    && item->_iSpell == owner.data._pRSpell) {
			removeBagItem(i);
			CalcPlrScrolls(owner.id());
			return;
		}
	}
	for (int i = 0; i < MAXBELTITEMS; i++) {
		Item *item = getBeltItem(i);
		if (!item) continue;
		if (item->_itype != ITYPE_NONE
		    && (item->_iMiscId == IMISC_SCROLL || item->_iMiscId == IMISC_SCROLLT)
		    && item->_iSpell == owner.data._pRSpell) {
			removeBeltItem(i);
			CalcPlrScrolls(owner.id());
			return;
		}
	}
}

bool PlayerInventory::UseScroll()
{
	if (pcurs != CURSOR_HAND) return FALSE;
	if (lvl.type() == DunType::town && !spelldata[myplr().data._pRSpell].sTownSpell)
		return FALSE;

	for (int i = 0; i < NUM_INV_GRID_ELEM; i++) {
		Item *item = getBagItem(i);
		if (!item) continue;
		if (item->_itype != ITYPE_NONE
		    && (item->_iMiscId == IMISC_SCROLL || item->_iMiscId == IMISC_SCROLLT)
		    && item->_iSpell == myplr().data._pRSpell) {
			return TRUE;
		}
	}
	for (int i = 0; i < MAXBELTITEMS; i++) {
		Item *item = getBeltItem(i);
		if (!item) continue;
		if (item->_itype != ITYPE_NONE
		    && (item->_iMiscId == IMISC_SCROLL || item->_iMiscId == IMISC_SCROLLT)
		    && item->_iSpell == myplr().data._pRSpell) {
			return TRUE;
		}
	}
	return FALSE;
}

void PlayerInventory::UseStaffCharge()
{
	if (getBodyItem(BodyLoc::HandLeft)->_itype != ITYPE_NONE
	    && getBodyItem(BodyLoc::HandLeft)->_iMiscId == IMISC_STAFF
	    && getBodyItem(BodyLoc::HandLeft)->_iSpell == owner.data._pRSpell
	    && getBodyItem(BodyLoc::HandLeft)->_iCharges > 0) {
		getBodyItem(BodyLoc::HandLeft)->_iCharges--;
		CalcPlrStaff(owner.id());
	}
}

bool PlayerInventory::UseStaff()
{
	if (pcurs == CURSOR_HAND) {
		if (getBodyItem(BodyLoc::HandLeft)->_itype != ITYPE_NONE
		    && getBodyItem(BodyLoc::HandLeft)->_iMiscId == IMISC_STAFF
		    && getBodyItem(BodyLoc::HandLeft)->_iSpell == myplr().data._pRSpell
		    && getBodyItem(BodyLoc::HandLeft)->_iCharges > 0) {
			return TRUE;
		}
	}
	return FALSE;
}

bool PlayerInventory::UseInvItem(InvSlot & slot)
{
	Item *item = slot.item;
	if (!item) return;

	if (owner.data._pInvincible && !owner.data._pHitPoints && owner.isMyPlr())
		return TRUE;
	if (pcurs != CURSOR_HAND)
		return TRUE;
	if (stextflag != STORE_NONE)
		return TRUE;
	if (talkflag) return TRUE;

	switch (item->IDidx) {
	case IDI_MUSHROOM:
		sfxdelay = 10;
		if (owner.data._pClass == PC_WARRIOR) {
			sfxdnum = PS_WARR95;
		} else if (owner.data._pClass == PC_ROGUE) {
			sfxdnum = PS_ROGUE95;
		} else if (owner.data._pClass == PC_SORCERER) {
			sfxdnum = PS_MAGE95;
		}
		return TRUE;
	case IDI_FUNGALTM:
		PlaySFX(IS_IBOOK);
		sfxdelay = 10;
		if (owner.data._pClass == PC_WARRIOR) {
			sfxdnum = PS_WARR29;
		} else if (owner.data._pClass == PC_ROGUE) {
			sfxdnum = PS_ROGUE29;
		} else if (owner.data._pClass == PC_SORCERER) {
			sfxdnum = PS_MAGE29;
		}
		return TRUE;
	}

	if (!AllItemsList[item->IDidx].iUsable)
		return FALSE;

	if (!item->_iStatFlag) {
		if (owner.data._pClass == PC_WARRIOR) {
			PlaySFX(PS_WARR13);
		} else if (owner.data._pClass == PC_ROGUE) {
			PlaySFX(PS_ROGUE13);
		} else if (owner.data._pClass == PC_SORCERER) {
			PlaySFX(PS_MAGE13);
		}
		return TRUE;
	}

	if (item->_iMiscId == IMISC_SCROLL && lvl.currlevel == 0 && !spelldata[item->_iSpell].sTownSpell) {
		return TRUE;
	}

	if (item->_iMiscId == IMISC_SCROLLT && lvl.currlevel == 0 && !spelldata[item->_iSpell].sTownSpell) {
		return TRUE;
	}

	int idata = ItemCAnimTbl[item->_iCurs];
	if (item->_iMiscId == IMISC_BOOK)
		PlaySFX(IS_RBOOK);
	else if (owner.isMyPlr())
		PlaySFX(ItemInvSnds[idata]);

	UseItem(owner.id(), item->_iMiscId, item->_iSpell);

	removeItem(slot);
	items.destroyItem(item->id);
	return true;
}

void PlayerInventory::DoTelekinesis()
{
	if (pcursobj != -1)
		NetSendCmdParam1(TRUE, CMD_OPOBJT, pcursobj);
	if (pcursitem)
		NetSendCmdGItem(TRUE, CMD_REQUESTAGITEM, myplr(), myplr(), *pcursitem);
	if (pcursmonst != -1 && !monsters[pcursmonst].M_Talker() && monsters[pcursmonst].data.mtalkmsg == 0)
		NetSendCmdParam1(TRUE, CMD_KNOCKBACK, pcursmonst);
	NewCursor(CURSOR_HAND);
}

int PlayerInventory::CalculateGold()
{
	int gold = 0;
	for (int i = 0; i < NUM_INV_GRID_ELEM; i++) {
		if (!getBagItem(i)) continue;
		if (getBagItem(i)->_itype == ITYPE_GOLD)
			gold += getBagItem(i)->_ivalue;
	}
	assert(gold >= 0);
	return gold;
}

bool PlayerInventory::DropItemBeforeTrig()
{
	if (GetDropPos(owner.pos()))
	{
		NetSendCmdPItem(TRUE, CMD_PUTITEM, cursm.x, cursm.y);
		NewCursor(CURSOR_HAND);
		return TRUE;
	}
	return FALSE;
}

DEVILUTION_END_NAMESPACE
