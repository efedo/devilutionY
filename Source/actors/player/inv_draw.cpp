/**
 * @file inv.cpp
 *
 * Inv display rectangle
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE


void PlayerInventory::DrawBodySlot(BodyLoc bodyloc)
{
	InvSlot &slot = getBodySlot(bodyloc);
	RECT32 slotrect = slot.rect;

	Item *item = getBodySlot(bodyloc).item();
	if (!item || item->_itype == ITYPE_NONE)
		return;

	V2Di slotpos = { slotrect.x, slotrect.y };
	DrawInvSlotBack(slotrect);

	int frame = item->_iCurs + CURSOR_FIRSTITEM;
	int frame_width = InvItemWidth[frame];
	int color;

	if (pcursinvitem == INVITEM_HEAD) {
		color = ICOL_WHITE;
		if (item->_iMagical != ITEM_QUALITY_NORMAL)
			color = ICOL_BLUE;
		if (!item->_iStatFlag)
			color = ICOL_RED;
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
		int frame = item->_iCurs + CURSOR_FIRSTITEM;
		int frame_width = InvItemWidth[frame];
		if (pcursinvitem == INVITEM_INV_FIRST) { // may be incorrect
			int color = ICOL_WHITE;
			if (item->_iMagical != ITEM_QUALITY_NORMAL) {
				color = ICOL_BLUE;
			}
			if (!item->_iStatFlag) {
				color = ICOL_RED;
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
		if (item->_itype == ITYPE_NONE) {
			continue;
		}
		auto rect = _belt.slots[i].rect;
		V2Di pos = { rect.x, rect.y };

		DrawInvSlotBack(rect);
		frame = item->_iCurs + CURSOR_FIRSTITEM;
		frame_width = InvItemWidth[frame];

		if (pcursinvitem == i + INVITEM_BELT_FIRST) {
			color = ICOL_WHITE;
			if (item->_iMagical)
				color = ICOL_BLUE;
			if (!item->_iStatFlag)
				color = ICOL_RED;
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
		    && item->_itype != ITYPE_GOLD) {
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

	if (pi->_itype == ITYPE_NONE) return -1;

	if (pi->_itype == ITYPE_GOLD) {
		int nGold = pi->_ivalue;
		sprintf(infostr, "%i gold %s", nGold, get_pieces_str(nGold));
	} else {
		if (pi->_iMagical == ITEM_QUALITY_MAGIC) {
			infoclr = COL_BLUE;
		} else if (pi->_iMagical == ITEM_QUALITY_UNIQUE) {
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

DEVILUTION_END_NAMESPACE
