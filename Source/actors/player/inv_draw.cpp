/**
 * @file inv.cpp
 *
 * Inv display rectangle
 */
#include "all.h"

namespace dvl {


void PlayerInventory::DrawBodySlot(BodyLoc bodyloc)
{
	InvSlot &slot = getBodySlot(bodyloc);
	RECT32 slotrect = slot.rect;

	Item *item = getBodySlot(bodyloc).item();
	if (!item || item->_itype == ItemType::none)
		return;

	V2Di slotpos = { slotrect.x, slotrect.y };
	DrawInvSlotBack(slotrect);

	int frame = item->_iCurs + Cursor::FIRSTITEM;
	int frame_width = InvItemWidth[frame];
	int color;

	if (pcursinvitem == INVITEM_HEAD) {
		color = ItemColor::white;
		if (item->_iMagical != ItemQuality::normal)
			color = ItemColor::blue;
		if (!item->_iStatFlag)
			color = ItemColor::red;
		CelBlitOutline(color, slotpos, pCursCels, frame, frame_width);
	}

	if (item->_iStatFlag) {
		CelClippedDraw(slotpos, pCursCels, frame, frame_width);
	} else {
		CelDrawLightRed(slotpos, pCursCels, frame, frame_width, 1);
	}
}

void PlayerInventory::DrawInvSlots()
{
	V2Di screen;
	uint8_t *pBuff;

	for (InvSlot &i : getBagArray()) {
		DrawInvSlotBack(i.rect);
	}

	Item *lastItem = 0;
	for (InvSlot &j : getBagArray()) {
		auto rect = j.rect;
		V2Di pos = { rect.x, rect.y };
		Item *item = j.item();
		if (item == lastItem)
			continue; // Skip multiple slots of same item
		lastItem = item;
		int frame = item->_iCurs + Cursor::FIRSTITEM;
		int frame_width = InvItemWidth[frame];
		if (pcursinvitem == INVITEM_INV_FIRST) { // may be incorrect
			int color = ItemColor::white;
			if (item->_iMagical != ItemQuality::normal) {
				color = ItemColor::blue;
			}
			if (!item->_iStatFlag) {
				color = ItemColor::red;
			}
			CelBlitOutline(color, pos, pCursCels, frame, frame_width);
		}

		if (item->_iStatFlag) {
			CelClippedDraw(pos, pCursCels, frame, frame_width);
		} else {
			CelDrawLightRed(pos, pCursCels, frame, frame_width, 1);
		}
	}
}

void PlayerInventory::DrawInvBelt()
{
	int frame, frame_width, color;
	uint8_t fi, ff;

	if (talkflag)
		return;

	DrawPanelBox({ 205, 21, 232, 28 }, { PANEL_X + 205, PANEL_Y + 5 });

	for (int i = 0; i < MAXBELTITEMS; i++) {
		Item *item = getBeltSlot(i).item();
		if (!item)
			continue;
		if (item->_itype == ItemType::none) {
			continue;
		}
		auto rect = _belt.slots[i].rect;
		V2Di pos = { rect.x, rect.y };

		DrawInvSlotBack(rect);
		frame = item->_iCurs + Cursor::FIRSTITEM;
		frame_width = InvItemWidth[frame];

		if (pcursinvitem == i + INVITEM_BELT_FIRST) {
			color = ItemColor::white;
			if (item->_iMagical)
				color = ItemColor::blue;
			if (!item->_iStatFlag)
				color = ItemColor::red;
			if (!sgbControllerActive || gui.invflag)
				CelBlitOutline(color, pos, pCursCels, frame, frame_width);
		}

		if (item->_iStatFlag) {
			CelClippedDraw(pos, pCursCels, frame, frame_width);
		} else {
			CelDrawLightRed(pos, pCursCels, frame, frame_width, 1);
		}

		if (AllItemsList[item->IDidx].iUsable
		    && item->_iStatFlag
		    && item->_itype != ItemType::gold) {
			fi = i + 49;
			ff = fontframe[gbFontTransTbl[fi]];
			PrintChar({ pos.x + 28 - fontkern[ff], pos.y }, ff, 0);
		}
	}
}

void PlayerInventory::DrawInv()
{
	CelDraw(RIGHT_PANEL_X, 351 + SCREEN_Y, pInvCels, 1, SPANEL_WIDTH);

	DrawBodySlot(BodyLoc::Head);
	DrawBodySlot(BodyLoc::RingLeft);
	DrawBodySlot(BodyLoc::RingRight);
	DrawBodySlot(BodyLoc::Amulet);
	DrawBodySlot(BodyLoc::HandLeft);
	DrawBodySlot(BodyLoc::HandRight);
	DrawBodySlot(BodyLoc::Chest);
	DrawInvSlots();
}

void PlayerInventory::DrawInvMsg(char *msg)
{
	DWORD dwTicks;

	int sgdwLastTime; // check name
	dwTicks = SDL_GetTicks();
	if (dwTicks - sgdwLastTime >= 5000) {
		sgdwLastTime = dwTicks;
		ErrorPlrMsg(msg);
	}
}

char PlayerInventory::CheckInvHLight()
{
	InvSlot * slot = GetCursorInvSlot(Mouse);
	if (!slot) return false;
	Item *pi = 0;
	if (!pi) return false;

	infoclr = COL_WHITE;
	PlayerStruct *p = &myplr().data;
	ClearPanel();

	if (pi->_itype == ItemType::none) return -1;

	if (pi->_itype == ItemType::gold) {
		int nGold = pi->_ivalue;
		sprintf(infostr, "%i gold %s", nGold, get_pieces_str(nGold));
	} else {
		if (pi->_iMagical == ItemQuality::magic) {
			infoclr = COL_BLUE;
		} else if (pi->_iMagical == ItemQuality::unique) {
			infoclr = COL_GOLD;
		}
		strcpy(infostr, pi->_iName);
		if (pi->_iIdentified) {
			strcpy(infostr, pi->_iIName);
			pi->PrintDetails();
		} else {
			pi->PrintDur();
		}
	}
	return true;
}

void PlayerInventory::DrawInvSlotBack(RECT32 r)
{
	uint8_t *dst;
	assert(gpBuffer);
	dst = &gpBuffer[r.x + BUFFER_WIDTH * r.y];
	int wdt, hgt;
	uint8_t pix;

	for (hgt = r.h; hgt; hgt--, dst -= BUFFER_WIDTH + r.w) {
		for (wdt = r.w; wdt; wdt--) {
			pix = *dst;
			if (pix >= PAL16_BLUE) {
				if (pix <= PAL16_BLUE + 15)
					pix -= PAL16_BLUE - PAL16_BEIGE;
				else if (pix >= PAL16_GRAY)
					pix -= PAL16_GRAY - PAL16_BEIGE;
			}
			*dst++ = pix;
		}
	}
}

}
