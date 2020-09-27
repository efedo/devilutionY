/**
 * @file inv.cpp
 *
 * Implementation of player inventory.
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

bool invflag;
BYTE *pInvCels;
bool drawsbarflag;
int sgdwLastTime; // check name

/**
 * Maps from inventory slot to screen position. The inventory slots are
 * arranged as follows:
 *                          00 01
 *                          02 03   06
 *              07 08       19 20       13 14
 *              09 10       21 22       15 16
 *              11 12       23 24       17 18
 *                 04                   05
 *              25 26 27 28 29 30 31 32 33 34
 *              35 36 37 38 39 40 41 42 43 44
 *              45 46 47 48 49 50 51 52 53 54
 *              55 56 57 58 59 60 61 62 63 64
 * 65 66 67 68 69 70 71 72
 * @see graphics/inv/inventory.png
 */
const InvXY InvRect[] = {
	// clang-format off
	//  X,   Y
	{ RIGHT_PANEL + 132,  31 }, // helmet
	{ RIGHT_PANEL + 160,  31 }, // helmet
	{ RIGHT_PANEL + 132,  59 }, // helmet
	{ RIGHT_PANEL + 160,  59 }, // helmet
	{ RIGHT_PANEL + 45,  205 }, // left ring
	{ RIGHT_PANEL + 247, 205 }, // right ring
	{ RIGHT_PANEL + 204,  59 }, // amulet
	{ RIGHT_PANEL + 17,  104 }, // left hand
	{ RIGHT_PANEL + 46,  104 }, // left hand
	{ RIGHT_PANEL + 17,  132 }, // left hand
	{ RIGHT_PANEL + 46,  132 }, // left hand
	{ RIGHT_PANEL + 17,  160 }, // left hand
	{ RIGHT_PANEL + 46,  160 }, // left hand
	{ RIGHT_PANEL + 247, 104 }, // right hand
	{ RIGHT_PANEL + 276, 104 }, // right hand
	{ RIGHT_PANEL + 247, 132 }, // right hand
	{ RIGHT_PANEL + 276, 132 }, // right hand
	{ RIGHT_PANEL + 247, 160 }, // right hand
	{ RIGHT_PANEL + 276, 160 }, // right hand
	{ RIGHT_PANEL + 132, 104 }, // chest
	{ RIGHT_PANEL + 160, 104 }, // chest
	{ RIGHT_PANEL + 132, 132 }, // chest
	{ RIGHT_PANEL + 160, 132 }, // chest
	{ RIGHT_PANEL + 132, 160 }, // chest
	{ RIGHT_PANEL + 160, 160 }, // chest
	{ RIGHT_PANEL + 17,  250 }, // inv row 1
	{ RIGHT_PANEL + 46,  250 }, // inv row 1
	{ RIGHT_PANEL + 74,  250 }, // inv row 1
	{ RIGHT_PANEL + 103, 250 }, // inv row 1
	{ RIGHT_PANEL + 131, 250 }, // inv row 1
	{ RIGHT_PANEL + 160, 250 }, // inv row 1
	{ RIGHT_PANEL + 189, 250 }, // inv row 1
	{ RIGHT_PANEL + 218, 250 }, // inv row 1
	{ RIGHT_PANEL + 247, 250 }, // inv row 1
	{ RIGHT_PANEL + 276, 250 }, // inv row 1
	{ RIGHT_PANEL + 17,  279 }, // inv row 2
	{ RIGHT_PANEL + 46,  279 }, // inv row 2
	{ RIGHT_PANEL + 74,  279 }, // inv row 2
	{ RIGHT_PANEL + 103, 279 }, // inv row 2
	{ RIGHT_PANEL + 131, 279 }, // inv row 2
	{ RIGHT_PANEL + 160, 279 }, // inv row 2
	{ RIGHT_PANEL + 189, 279 }, // inv row 2
	{ RIGHT_PANEL + 218, 279 }, // inv row 2
	{ RIGHT_PANEL + 247, 279 }, // inv row 2
	{ RIGHT_PANEL + 276, 279 }, // inv row 2
	{ RIGHT_PANEL + 17,  308 }, // inv row 3
	{ RIGHT_PANEL + 46,  308 }, // inv row 3
	{ RIGHT_PANEL + 74,  308 }, // inv row 3
	{ RIGHT_PANEL + 103, 308 }, // inv row 3
	{ RIGHT_PANEL + 131, 308 }, // inv row 3
	{ RIGHT_PANEL + 160, 308 }, // inv row 3
	{ RIGHT_PANEL + 189, 308 }, // inv row 3
	{ RIGHT_PANEL + 218, 308 }, // inv row 3
	{ RIGHT_PANEL + 247, 308 }, // inv row 3
	{ RIGHT_PANEL + 276, 308 }, // inv row 3
	{ RIGHT_PANEL + 17,  336 }, // inv row 4
	{ RIGHT_PANEL + 46,  336 }, // inv row 4
	{ RIGHT_PANEL + 74,  336 }, // inv row 4
	{ RIGHT_PANEL + 103, 336 }, // inv row 4
	{ RIGHT_PANEL + 131, 336 }, // inv row 4
	{ RIGHT_PANEL + 160, 336 }, // inv row 4
	{ RIGHT_PANEL + 189, 336 }, // inv row 4
	{ RIGHT_PANEL + 218, 336 }, // inv row 4
	{ RIGHT_PANEL + 247, 336 }, // inv row 4
	{ RIGHT_PANEL + 276, 336 }, // inv row 4
	{ PANEL_LEFT + 205, PANEL_TOP + 33 }, // belt
	{ PANEL_LEFT + 234, PANEL_TOP + 33 }, // belt
	{ PANEL_LEFT + 263, PANEL_TOP + 33 }, // belt
	{ PANEL_LEFT + 292, PANEL_TOP + 33 }, // belt
	{ PANEL_LEFT + 321, PANEL_TOP + 33 }, // belt
	{ PANEL_LEFT + 350, PANEL_TOP + 33 }, // belt
	{ PANEL_LEFT + 379, PANEL_TOP + 33 }, // belt
	{ PANEL_LEFT + 408, PANEL_TOP + 33 }  // belt
	// clang-format on
};

/* data */
/** Specifies the starting inventory slots for placement of 2x2 items. */
int AP2x2Tbl[10] = { 8, 28, 6, 26, 4, 24, 2, 22, 0, 20 };

void FreeInvGFX()
{
	MemFreeDbg(pInvCels);
}

void InitInv()
{
	if (myplr().data._pClass == PC_WARRIOR) {
		pInvCels = LoadFileInMem("Data\\Inv\\Inv.CEL", NULL);
	} else if (myplr().data._pClass == PC_ROGUE) {
		pInvCels = LoadFileInMem("Data\\Inv\\Inv_rog.CEL", NULL);
	} else if (myplr().data._pClass == PC_SORCERER) {
		pInvCels = LoadFileInMem("Data\\Inv\\Inv_Sor.CEL", NULL);
	}

	invflag = FALSE;
	drawsbarflag = FALSE;
}

void InvDrawSlotBack(RECT32 r)
{
	BYTE *dst;
	assert(gpBuffer);
	dst = &gpBuffer[r.x + BUFFER_WIDTH * r.y];
	int wdt, hgt;
	BYTE pix;

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

void DrawInv()
{
	bool invtest[NUM_INV_GRID_ELEM];
	int frame, frame_width, color, i, j, ii;
	V2Di screen;
	BYTE *pBuff;

	CelDraw(RIGHT_PANEL_X, 351 + SCREEN_Y, pInvCels, 1, SPANEL_WIDTH);

	if (myplr().data.InvBody[INVLOC_HEAD]._itype != ITYPE_NONE) {
		InvDrawSlotBack({ RIGHT_PANEL_X + 133, 59 + SCREEN_Y, 2 * INV_SLOT_SIZE_PX, 2 * INV_SLOT_SIZE_PX });

		frame = myplr().data.InvBody[INVLOC_HEAD]._iCurs + CURSOR_FIRSTITEM;
		frame_width = InvItemWidth[frame];

		if (pcursinvitem == INVITEM_HEAD) {
			color = ICOL_WHITE;
			if (myplr().data.InvBody[INVLOC_HEAD]._iMagical != ITEM_QUALITY_NORMAL) {
				color = ICOL_BLUE;
			}
			if (!myplr().data.InvBody[INVLOC_HEAD]._iStatFlag) {
				color = ICOL_RED;
			}
			CelBlitOutline(color, { RIGHT_PANEL_X + 133, 59 + SCREEN_Y }, pCursCels, frame, frame_width);
		}

		if (myplr().data.InvBody[INVLOC_HEAD]._iStatFlag) {
			CelClippedDraw({ RIGHT_PANEL_X + 133, 59 + SCREEN_Y }, pCursCels, frame, frame_width);
		} else {
			CelDrawLightRed({ RIGHT_PANEL_X + 133, 59 + SCREEN_Y }, pCursCels, frame, frame_width, 1);
		}
	}

	if (myplr().data.InvBody[INVLOC_RING_LEFT]._itype != ITYPE_NONE) {
		InvDrawSlotBack({ RIGHT_PANEL_X + 48, 205 + SCREEN_Y, INV_SLOT_SIZE_PX, INV_SLOT_SIZE_PX });

		frame = myplr().data.InvBody[INVLOC_RING_LEFT]._iCurs + CURSOR_FIRSTITEM;
		frame_width = InvItemWidth[frame];

		if (pcursinvitem == INVITEM_RING_LEFT) {
			color = ICOL_WHITE;
			if (myplr().data.InvBody[INVLOC_RING_LEFT]._iMagical != ITEM_QUALITY_NORMAL) {
				color = ICOL_BLUE;
			}
			if (!myplr().data.InvBody[INVLOC_RING_LEFT]._iStatFlag) {
				color = ICOL_RED;
			}
			CelBlitOutline(color, { RIGHT_PANEL_X + 48, 205 + SCREEN_Y }, pCursCels, frame, frame_width);
		}

		if (myplr().data.InvBody[INVLOC_RING_LEFT]._iStatFlag) {
			CelClippedDraw({ RIGHT_PANEL_X + 48, 205 + SCREEN_Y }, pCursCels, frame, frame_width);
		} else {
			CelDrawLightRed({ RIGHT_PANEL_X + 48, 205 + SCREEN_Y }, pCursCels, frame, frame_width, 1);
		}
	}

	if (myplr().data.InvBody[INVLOC_RING_RIGHT]._itype != ITYPE_NONE) {
		InvDrawSlotBack({ RIGHT_PANEL_X + 249, 205 + SCREEN_Y, INV_SLOT_SIZE_PX, INV_SLOT_SIZE_PX });

		frame = myplr().data.InvBody[INVLOC_RING_RIGHT]._iCurs + CURSOR_FIRSTITEM;
		frame_width = InvItemWidth[frame];

		if (pcursinvitem == INVITEM_RING_RIGHT) {
			color = ICOL_WHITE;
			if (myplr().data.InvBody[INVLOC_RING_RIGHT]._iMagical != ITEM_QUALITY_NORMAL) {
				color = ICOL_BLUE;
			}
			if (!myplr().data.InvBody[INVLOC_RING_RIGHT]._iStatFlag) {
				color = ICOL_RED;
			}
			CelBlitOutline(color, { RIGHT_PANEL_X + 249, 205 + SCREEN_Y }, pCursCels, frame, frame_width);
		}

		if (myplr().data.InvBody[INVLOC_RING_RIGHT]._iStatFlag) {
			CelClippedDraw({ RIGHT_PANEL_X + 249, 205 + SCREEN_Y }, pCursCels, frame, frame_width);
		} else {
			CelDrawLightRed({ RIGHT_PANEL_X + 249, 205 + SCREEN_Y }, pCursCels, frame, frame_width, 1);
		}
	}

	if (myplr().data.InvBody[INVLOC_AMULET]._itype != ITYPE_NONE) {
		InvDrawSlotBack({ RIGHT_PANEL_X + 205, 60 + SCREEN_Y, INV_SLOT_SIZE_PX, INV_SLOT_SIZE_PX });

		frame = myplr().data.InvBody[INVLOC_AMULET]._iCurs + CURSOR_FIRSTITEM;
		frame_width = InvItemWidth[frame];

		if (pcursinvitem == INVITEM_AMULET) {
			color = ICOL_WHITE;
			if (myplr().data.InvBody[INVLOC_AMULET]._iMagical != ITEM_QUALITY_NORMAL) {
				color = ICOL_BLUE;
			}
			if (!myplr().data.InvBody[INVLOC_AMULET]._iStatFlag) {
				color = ICOL_RED;
			}
			CelBlitOutline(color, { RIGHT_PANEL_X + 205, 60 + SCREEN_Y }, pCursCels, frame, frame_width);
		}

		if (myplr().data.InvBody[INVLOC_AMULET]._iStatFlag) {
			CelClippedDraw({ RIGHT_PANEL_X + 205, 60 + SCREEN_Y }, pCursCels, frame, frame_width);
		} else {
			CelDrawLightRed({ RIGHT_PANEL_X + 205, 60 + SCREEN_Y }, pCursCels, frame, frame_width, 1);
		}
	}

	if (myplr().data.InvBody[INVLOC_HAND_LEFT]._itype != ITYPE_NONE) {
		InvDrawSlotBack({ RIGHT_PANEL_X + 17, 160 + SCREEN_Y, 2 * INV_SLOT_SIZE_PX, 3 * INV_SLOT_SIZE_PX });

		frame = myplr().data.InvBody[INVLOC_HAND_LEFT]._iCurs + CURSOR_FIRSTITEM;
		frame_width = InvItemWidth[frame];
		// calc item offsets for weapons smaller than 2x3 slots
		screen.x = frame_width == INV_SLOT_SIZE_PX ? (RIGHT_PANEL_X + 31) : (RIGHT_PANEL_X + 17);
		screen.y = InvItemHeight[frame] == (3 * INV_SLOT_SIZE_PX) ? (160 + SCREEN_Y) : (146 + SCREEN_Y);

		if (pcursinvitem == INVITEM_HAND_LEFT) {
			color = ICOL_WHITE;
			if (myplr().data.InvBody[INVLOC_HAND_LEFT]._iMagical != ITEM_QUALITY_NORMAL) {
				color = ICOL_BLUE;
			}
			if (!myplr().data.InvBody[INVLOC_HAND_LEFT]._iStatFlag) {
				color = ICOL_RED;
			}
			CelBlitOutline(color, screen, pCursCels, frame, frame_width);
		}

		if (myplr().data.InvBody[INVLOC_HAND_LEFT]._iStatFlag) {
			CelClippedDraw(screen, pCursCels, frame, frame_width);
		} else {
			CelDrawLightRed(screen, pCursCels, frame, frame_width, 1);
		}

		if (myplr().data.InvBody[INVLOC_HAND_LEFT]._iLoc == ILOC_TWOHAND) {
			InvDrawSlotBack({ RIGHT_PANEL_X + 247, 160 + SCREEN_Y, 2 * INV_SLOT_SIZE_PX, 3 * INV_SLOT_SIZE_PX });
			light_table_index = 0;

			pBuff = frame_width == INV_SLOT_SIZE_PX
			    ? &gpBuffer[SCREENXY(RIGHT_PANEL_X + 197, SCREEN_Y)]
			    : &gpBuffer[SCREENXY(RIGHT_PANEL_X + 183, SCREEN_Y)];
			CelClippedBlitLightTrans(pBuff, pCursCels, frame, frame_width, true);
		}
	}
	if (myplr().data.InvBody[INVLOC_HAND_RIGHT]._itype != ITYPE_NONE) {
		InvDrawSlotBack({ RIGHT_PANEL_X + 247, 160 + SCREEN_Y, 2 * INV_SLOT_SIZE_PX, 3 * INV_SLOT_SIZE_PX });

		frame = myplr().data.InvBody[INVLOC_HAND_RIGHT]._iCurs + CURSOR_FIRSTITEM;
		frame_width = InvItemWidth[frame];
		// calc item offsets for weapons smaller than 2x3 slots
		screen.x = frame_width == INV_SLOT_SIZE_PX ? (RIGHT_PANEL_X + 261) : (RIGHT_PANEL_X + 249);
		screen.y = InvItemHeight[frame] == 3 * INV_SLOT_SIZE_PX ? (160 + SCREEN_Y) : (146 + SCREEN_Y);

		if (pcursinvitem == INVITEM_HAND_RIGHT) {
			color = ICOL_WHITE;
			if (myplr().data.InvBody[INVLOC_HAND_RIGHT]._iMagical != ITEM_QUALITY_NORMAL) {
				color = ICOL_BLUE;
			}
			if (!myplr().data.InvBody[INVLOC_HAND_RIGHT]._iStatFlag) {
				color = ICOL_RED;
			}
			CelBlitOutline(color, screen, pCursCels, frame, frame_width);
		}

		if (myplr().data.InvBody[INVLOC_HAND_RIGHT]._iStatFlag) {
			CelClippedDraw(screen, pCursCels, frame, frame_width);
		} else {
			CelDrawLightRed(screen, pCursCels, frame, frame_width, 1);
		}
	}

	if (myplr().data.InvBody[INVLOC_CHEST]._itype != ITYPE_NONE) {
		InvDrawSlotBack({ RIGHT_PANEL_X + 133, 160 + SCREEN_Y, 2 * INV_SLOT_SIZE_PX, 3 * INV_SLOT_SIZE_PX });

		frame = myplr().data.InvBody[INVLOC_CHEST]._iCurs + CURSOR_FIRSTITEM;
		frame_width = InvItemWidth[frame];

		if (pcursinvitem == INVITEM_CHEST) {
			color = ICOL_WHITE;
			if (myplr().data.InvBody[INVLOC_CHEST]._iMagical != ITEM_QUALITY_NORMAL) {
				color = ICOL_BLUE;
			}
			if (!myplr().data.InvBody[INVLOC_CHEST]._iStatFlag) {
				color = ICOL_RED;
			}
			CelBlitOutline(color, { RIGHT_PANEL_X + 133, 160 + SCREEN_Y }, pCursCels, frame, frame_width);
		}

		if (myplr().data.InvBody[INVLOC_CHEST]._iStatFlag) {
			CelClippedDraw({ RIGHT_PANEL_X + 133, 160 + SCREEN_Y }, pCursCels, frame, frame_width);
		} else {
			CelDrawLightRed({ RIGHT_PANEL_X + 133, 160 + SCREEN_Y }, pCursCels, frame, frame_width, 1);
		}
	}

	for (i = 0; i < NUM_INV_GRID_ELEM; i++) {
		invtest[i] = FALSE;
		if (myplr().data.InvGrid[i] != 0) {
			InvDrawSlotBack(
			    {	InvRect[i + SLOTXY_INV_FIRST].X + SCREEN_X,
			        InvRect[i + SLOTXY_INV_FIRST].Y + SCREEN_Y - 1,
			        INV_SLOT_SIZE_PX,
			        INV_SLOT_SIZE_PX });
		}
	}

	for (j = 0; j < NUM_INV_GRID_ELEM; j++) {
		if (myplr().data.InvGrid[j] > 0) // first slot of an item
		{
			ii = myplr().data.InvGrid[j] - 1;

			invtest[j] = TRUE;

			frame = myplr().data.InvList[ii]._iCurs + CURSOR_FIRSTITEM;
			frame_width = InvItemWidth[frame];
			if (pcursinvitem == ii + INVITEM_INV_FIRST) {
				color = ICOL_WHITE;
				if (myplr().data.InvList[ii]._iMagical != ITEM_QUALITY_NORMAL) {
					color = ICOL_BLUE;
				}
				if (!myplr().data.InvList[ii]._iStatFlag) {
					color = ICOL_RED;
				}
				CelBlitOutline(
				    color,
				    { InvRect[j + SLOTXY_INV_FIRST].X + SCREEN_X,
				        InvRect[j + SLOTXY_INV_FIRST].Y + SCREEN_Y - 1 },
				    pCursCels, frame, frame_width);
			}

			if (myplr().data.InvList[ii]._iStatFlag) {
				CelClippedDraw(
				    { InvRect[j + SLOTXY_INV_FIRST].X + SCREEN_X,
				        InvRect[j + SLOTXY_INV_FIRST].Y + SCREEN_Y - 1 },
				    pCursCels, frame, frame_width);
			} else {
				CelDrawLightRed(
				    { InvRect[j + SLOTXY_INV_FIRST].X + SCREEN_X,
				        InvRect[j + SLOTXY_INV_FIRST].Y + SCREEN_Y - 1 },
				    pCursCels, frame, frame_width, 1);
			}
		}
	}
}

void DrawInvBelt()
{
	int i, frame, frame_width, color;
	BYTE fi, ff;

	if (talkflag) {
		return;
	}

	DrawPanelBox({ 205, 21, 232, 28 }, { PANEL_X + 205, PANEL_Y + 5 });

	for (i = 0; i < MAXBELTITEMS; i++) {
		if (myplr().data.SpdList[i]._itype == ITYPE_NONE) {
			continue;
		}

		InvDrawSlotBack({ InvRect[i + SLOTXY_BELT_FIRST].X + SCREEN_X, InvRect[i + SLOTXY_BELT_FIRST].Y + SCREEN_Y - 1, 28, 28 });
		frame = myplr().data.SpdList[i]._iCurs + CURSOR_FIRSTITEM;
		frame_width = InvItemWidth[frame];

		if (pcursinvitem == i + INVITEM_BELT_FIRST) {
			color = ICOL_WHITE;
			if (myplr().data.SpdList[i]._iMagical)
				color = ICOL_BLUE;
			if (!myplr().data.SpdList[i]._iStatFlag)
				color = ICOL_RED;
			if (!sgbControllerActive || invflag)
				CelBlitOutline(color, { InvRect[i + SLOTXY_BELT_FIRST].X + SCREEN_X, InvRect[i + SLOTXY_BELT_FIRST].Y + SCREEN_Y - 1 }, pCursCels, frame, frame_width);
		}

		if (myplr().data.SpdList[i]._iStatFlag) {
			CelClippedDraw({InvRect[i + SLOTXY_BELT_FIRST].X + SCREEN_X, InvRect[i + SLOTXY_BELT_FIRST].Y + SCREEN_Y - 1}, pCursCels, frame, frame_width);
		} else {
			CelDrawLightRed({ InvRect[i + SLOTXY_BELT_FIRST].X + SCREEN_X, InvRect[i + SLOTXY_BELT_FIRST].Y + SCREEN_Y - 1 }, pCursCels, frame, frame_width, 1);
		}

		if (AllItemsList[myplr().data.SpdList[i].IDidx].iUsable
		    && myplr().data.SpdList[i]._iStatFlag
		    && myplr().data.SpdList[i]._itype != ITYPE_GOLD) {
			fi = i + 49;
			ff = fontframe[gbFontTransTbl[fi]];
			PrintChar({ InvRect[i + SLOTXY_BELT_FIRST].X + SCREEN_X + 28 - fontkern[ff], InvRect[i + SLOTXY_BELT_FIRST].Y + SCREEN_Y - 1 }, ff, 0);
		}
	}
}

PlayerInventory::PlayerInventory(Player &new_owner)
    : owner(new_owner)
{
} 


bool PlayerInventory::AutoPlace(int ii, V2Di size, bool saveflag)
{
	int i, j, xx, yy;
	bool done;

	done = TRUE;
	yy = 10 * (ii / 10);
	if (yy < 0) {
		yy = 0;
	}
	for (j = 0; j < size.y && done; j++) {
		if (yy >= 40) {
			done = FALSE;
		}
		xx = ii % 10;
		if (xx < 0) {
			xx = 0;
		}
		for (i = 0; i < size.x && done; i++) {
			if (xx >= 10) {
				done = FALSE;
			} else {
				done = owner.data.InvGrid[xx + yy] == 0;
			}
			xx++;
		}
		yy += 10;
	}
	if (done && saveflag) {
		owner.data.InvList[owner.data._pNumInv] = owner.data.HoldItem;
		owner.data._pNumInv++;
		yy = 10 * (ii / 10);
		if (yy < 0) {
			yy = 0;
		}
		for (j = 0; j < size.y; j++) {
			xx = ii % 10;
			if (xx < 0) {
				xx = 0;
			}
			for (i = 0; i < size.x; i++) {
				if (i != 0 || j != size.y - 1) {
					owner.data.InvGrid[xx + yy] = -owner.data._pNumInv;
				} else {
					owner.data.InvGrid[xx + yy] = owner.data._pNumInv;
				}
				xx++;
			}
			yy += 10;
		}
		CalcPlrScrolls(owner.id());
	}
	return done;
}

bool PlayerInventory::SpecialAutoPlace(int ii, V2Di pos, bool saveflag)
{
	int sx = pos.x;
	int sy = pos.y;

	int i, j, xx, yy;
	bool done;

	done = TRUE;
	yy = 10 * (ii / 10);
	if (yy < 0) {
		yy = 0;
	}
	for (j = 0; j < sy && done; j++) {
		if (yy >= 40) {
			done = FALSE;
		}
		xx = ii % 10;
		if (xx < 0) {
			xx = 0;
		}
		for (i = 0; i < sx && done; i++) {
			if (xx >= 10) {
				done = FALSE;
			} else {
				done = owner.data.InvGrid[xx + yy] == 0;
			}
			xx++;
		}
		yy += 10;
	}
	if (!done) {
		if (sx > 1 || sy > 1) {
			done = FALSE;
		} else {
			for (i = 0; i < MAXBELTITEMS; i++) {
				if (owner.data.SpdList[i]._itype == ITYPE_NONE) {
					done = TRUE;
					break;
				}
			}
		}
	}
	if (done && saveflag) {
		owner.data.InvList[owner.data._pNumInv] = owner.data.HoldItem;
		owner.data._pNumInv++;
		yy = 10 * (ii / 10);
		if (yy < 0) {
			yy = 0;
		}
		for (j = 0; j < sy; j++) {
			xx = ii % 10;
			if (xx < 0) {
				xx = 0;
			}
			for (i = 0; i < sx; i++) {
				if (i != 0 || j != sy - 1) {
					owner.data.InvGrid[xx + yy] = -owner.data._pNumInv;
				} else {
					owner.data.InvGrid[xx + yy] = owner.data._pNumInv;
				}
				xx++;
			}
			yy += 10;
		}
		CalcPlrScrolls(owner.id());
	}
	return done;
}

bool PlayerInventory::GoldAutoPlace()
{
	bool done;
	int i, ii;
	int xx, yy;

	done = FALSE;
	for (i = 0; i < owner.data._pNumInv && !done; i++) {
		if (owner.data.InvList[i]._itype == ITYPE_GOLD) {
			if (owner.data.HoldItem._ivalue + owner.data.InvList[i]._ivalue <= GOLD_MAX_LIMIT) {
				owner.data.InvList[i]._ivalue = owner.data.HoldItem._ivalue + owner.data.InvList[i]._ivalue;
				if (owner.data.InvList[i]._ivalue >= GOLD_MEDIUM_LIMIT)
					owner.data.InvList[i]._iCurs = ICURS_GOLD_LARGE;
				else if (owner.data.InvList[i]._ivalue <= GOLD_SMALL_LIMIT)
					owner.data.InvList[i]._iCurs = ICURS_GOLD_SMALL;
				else
					owner.data.InvList[i]._iCurs = ICURS_GOLD_MEDIUM;
				owner.data._pGold = CalculateGold();
				done = TRUE;
			}
		}
	}
	if (done)
		return done;

	for (i = 0; i < owner.data._pNumInv && !done; i++) {
		if (owner.data.InvList[i]._itype == ITYPE_GOLD && owner.data.InvList[i]._ivalue < GOLD_MAX_LIMIT) {
			if (owner.data.HoldItem._ivalue + owner.data.InvList[i]._ivalue <= GOLD_MAX_LIMIT) {
				owner.data.InvList[i]._ivalue = owner.data.HoldItem._ivalue + owner.data.InvList[i]._ivalue;
				if (owner.data.InvList[i]._ivalue >= GOLD_MEDIUM_LIMIT)
					owner.data.InvList[i]._iCurs = ICURS_GOLD_LARGE;
				else if (owner.data.InvList[i]._ivalue <= GOLD_SMALL_LIMIT)
					owner.data.InvList[i]._iCurs = ICURS_GOLD_SMALL;
				else
					owner.data.InvList[i]._iCurs = ICURS_GOLD_MEDIUM;
				owner.data._pGold = CalculateGold();
				done = TRUE;
			}
		}
	}
	if (done)
		return done;

	for (i = 39; i >= 0 && !done; i--) {
		yy = 10 * (i / 10);
		xx = i % 10;
		if (!owner.data.InvGrid[xx + yy]) {
			ii = owner.data._pNumInv;
			owner.data.InvList[ii] = owner.data.HoldItem;
			owner.data._pNumInv = owner.data._pNumInv + 1;
			owner.data.InvGrid[xx + yy] = owner.data._pNumInv;
			if (owner.data.HoldItem._ivalue >= GOLD_MEDIUM_LIMIT)
				owner.data.InvList[ii]._iCurs = ICURS_GOLD_LARGE;
			else if (owner.data.HoldItem._ivalue <= GOLD_SMALL_LIMIT)
				owner.data.InvList[ii]._iCurs = ICURS_GOLD_SMALL;
			else
				owner.data.InvList[ii]._iCurs = ICURS_GOLD_MEDIUM;
			owner.data._pGold = CalculateGold();
			done = TRUE;
		}
	}

	return done;
}

bool PlayerInventory::WeaponAutoPlace()
{
	if (owner.data.HoldItem._iLoc != ILOC_TWOHAND) {
		if (owner.data.InvBody[INVLOC_HAND_LEFT]._itype != ITYPE_NONE && owner.data.InvBody[INVLOC_HAND_LEFT]._iClass == ICLASS_WEAPON)
			return FALSE;
		if (owner.data.InvBody[INVLOC_HAND_RIGHT]._itype != ITYPE_NONE && owner.data.InvBody[INVLOC_HAND_RIGHT]._iClass == ICLASS_WEAPON)
			return FALSE;
		if (owner.data.InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_NONE) {
			NetSendCmdChItem(TRUE, INVLOC_HAND_LEFT);
			owner.data.InvBody[INVLOC_HAND_LEFT] = owner.data.HoldItem;
			return TRUE;
		}
		if (owner.data.InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_NONE && owner.data.InvBody[INVLOC_HAND_LEFT]._iLoc != ILOC_TWOHAND) {
			NetSendCmdChItem(TRUE, INVLOC_HAND_RIGHT);
			owner.data.InvBody[INVLOC_HAND_RIGHT] = owner.data.HoldItem;
			return TRUE;
		}
	} else if (owner.data.InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_NONE && owner.data.InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_NONE) {
		NetSendCmdChItem(TRUE, INVLOC_HAND_LEFT);
		owner.data.InvBody[INVLOC_HAND_LEFT] = owner.data.HoldItem;
		return TRUE;
	}

	return FALSE;
}

int SwapItem(ItemStruct *a, ItemStruct *b)
{
	ItemStruct h;

	h = *a;
	*a = *b;
	*b = h;

	return h._iCurs + CURSOR_FIRSTITEM;
}

void PlayerInventory::CheckInvPaste(V2Di pos)
{
	int mx = pos.x;
	int my = pos.y;

	int r, sx, sy;
	int i, j, xx, yy, ii;
	bool done, done2h;
	int il, cn, it, iv, ig, gt;
	ItemStruct tempitem;

	SetICursor(owner.data.HoldItem._iCurs + CURSOR_FIRSTITEM);
	i = mx + (icursW >> 1);
	j = my + (icursH >> 1);
	sx = icursW28;
	sy = icursH28;
	done = FALSE;
	for (r = 0; r < sizeof(InvRect) / sizeof(InvRect[0]) && !done; r++) {
		if (i >= InvRect[r].X && i < InvRect[r].X + INV_SLOT_SIZE_PX) {
			if (j >= InvRect[r].Y - INV_SLOT_SIZE_PX - 1 && j < InvRect[r].Y) {
				done = TRUE;
				r--;
			}
		}
		if (r == 24) {
			if ((sx & 1) == 0)
				i -= 14;
			if ((sy & 1) == 0) {
				j -= 14;
			}
		}
		if (r == 64 && (sy & 1) == 0)
			j += 14;
	}
	if (!done)
		return;
	il = ILOC_UNEQUIPABLE;
	if (r >= 0 && r <= 3)
		il = ILOC_HELM;
	if (r >= 4 && r <= 5)
		il = ILOC_RING;
	if (r == 6)
		il = ILOC_AMULET;
	if (r >= 7 && r <= 18)
		il = ILOC_ONEHAND;
	if (r >= 19 && r <= 24)
		il = ILOC_ARMOR;
	if (r >= 65 && r <= 72)
		il = ILOC_BELT;
	done = FALSE;
	if (owner.data.HoldItem._iLoc == il)
		done = TRUE;
	if (il == ILOC_ONEHAND && owner.data.HoldItem._iLoc == ILOC_TWOHAND) {
		il = ILOC_TWOHAND;
		done = TRUE;
	}
	if (owner.data.HoldItem._iLoc == ILOC_UNEQUIPABLE && il == ILOC_BELT) {
		if (sx == 1 && sy == 1) {
			done = TRUE;
			if (!AllItemsList[owner.data.HoldItem.IDidx].iUsable)
				done = FALSE;
			if (!owner.data.HoldItem._iStatFlag)
				done = FALSE;
			if (owner.data.HoldItem._itype == ITYPE_GOLD)
				done = FALSE;
		}
	}

	if (il == ILOC_UNEQUIPABLE) {
		done = TRUE;
		it = 0;
		ii = r - 25;
		if (owner.data.HoldItem._itype == ITYPE_GOLD) {
			yy = 10 * (ii / 10);
			xx = ii % 10;
			if (owner.data.InvGrid[xx + yy] != 0) {
				iv = owner.data.InvGrid[xx + yy];
				if (iv > 0) {
					if (owner.data.InvList[iv - 1]._itype != ITYPE_GOLD) {
						it = iv;
					}
				} else {
					it = -iv;
				}
			}
		} else {
			yy = 10 * ((ii / 10) - ((sy - 1) >> 1));
			if (yy < 0)
				yy = 0;
			for (j = 0; j < sy && done; j++) {
				if (yy >= NUM_INV_GRID_ELEM)
					done = FALSE;
				xx = (ii % 10) - ((sx - 1) >> 1);
				if (xx < 0)
					xx = 0;
				for (i = 0; i < sx && done; i++) {
					if (xx >= 10) {
						done = FALSE;
					} else {
						if (owner.data.InvGrid[xx + yy] != 0) {
							iv = owner.data.InvGrid[xx + yy];
							if (iv < 0)
								iv = -iv;
							if (it != 0) {
								if (it != iv)
									done = FALSE;
							} else
								it = iv;
						}
					}
					xx++;
				}
				yy += 10;
			}
		}
	}

	if (!done)
		return;

	if (il != ILOC_UNEQUIPABLE && il != ILOC_BELT && !owner.data.HoldItem._iStatFlag) {
		done = FALSE;
		if (owner.data._pClass == PC_WARRIOR)
			PlaySFX(PS_WARR13);
		else if (owner.data._pClass == PC_ROGUE)
			PlaySFX(PS_ROGUE13);
		else if (owner.data._pClass == PC_SORCERER)
			PlaySFX(PS_MAGE13);
	}

	if (!done)
		return;

	if (owner.isMyPlr())
		PlaySFX(ItemInvSnds[ItemCAnimTbl[owner.data.HoldItem._iCurs]]);

	cn = CURSOR_HAND;
	switch (il) {
	case ILOC_HELM:
		NetSendCmdChItem(FALSE, INVLOC_HEAD);
		if (owner.data.InvBody[INVLOC_HEAD]._itype == ITYPE_NONE)
			owner.data.InvBody[INVLOC_HEAD] = owner.data.HoldItem;
		else
			cn = SwapItem(&owner.data.InvBody[INVLOC_HEAD], &owner.data.HoldItem);
		break;
	case ILOC_RING:
		if (r == 4) {
			NetSendCmdChItem(FALSE, INVLOC_RING_LEFT);
			if (owner.data.InvBody[INVLOC_RING_LEFT]._itype == ITYPE_NONE)
				owner.data.InvBody[INVLOC_RING_LEFT] = owner.data.HoldItem;
			else
				cn = SwapItem(&owner.data.InvBody[INVLOC_RING_LEFT], &owner.data.HoldItem);
		} else {
			NetSendCmdChItem(FALSE, INVLOC_RING_RIGHT);
			if (owner.data.InvBody[INVLOC_RING_RIGHT]._itype == ITYPE_NONE)
				owner.data.InvBody[INVLOC_RING_RIGHT] = owner.data.HoldItem;
			else
				cn = SwapItem(&owner.data.InvBody[INVLOC_RING_RIGHT], &owner.data.HoldItem);
		}
		break;
	case ILOC_AMULET:
		NetSendCmdChItem(FALSE, INVLOC_AMULET);
		if (owner.data.InvBody[INVLOC_AMULET]._itype == ITYPE_NONE)
			owner.data.InvBody[INVLOC_AMULET] = owner.data.HoldItem;
		else
			cn = SwapItem(&owner.data.InvBody[INVLOC_AMULET], &owner.data.HoldItem);
		break;
	case ILOC_ONEHAND:
		if (r <= 12) {
			if (owner.data.InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_NONE) {
				if (owner.data.InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_NONE || owner.data.InvBody[INVLOC_HAND_RIGHT]._iClass != owner.data.HoldItem._iClass) {
					NetSendCmdChItem(FALSE, INVLOC_HAND_LEFT);
					owner.data.InvBody[INVLOC_HAND_LEFT] = owner.data.HoldItem;
				} else {
					NetSendCmdChItem(FALSE, INVLOC_HAND_RIGHT);
					cn = SwapItem(&owner.data.InvBody[INVLOC_HAND_RIGHT], &owner.data.HoldItem);
				}
				break;
			}
			if (owner.data.InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_NONE || owner.data.InvBody[INVLOC_HAND_RIGHT]._iClass != owner.data.HoldItem._iClass) {
				NetSendCmdChItem(FALSE, INVLOC_HAND_LEFT);
				cn = SwapItem(&owner.data.InvBody[INVLOC_HAND_LEFT], &owner.data.HoldItem);
				break;
			}

			NetSendCmdChItem(FALSE, INVLOC_HAND_RIGHT);
			cn = SwapItem(&owner.data.InvBody[INVLOC_HAND_RIGHT], &owner.data.HoldItem);
			break;
		}
		if (owner.data.InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_NONE) {
			if (owner.data.InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_NONE || owner.data.InvBody[INVLOC_HAND_LEFT]._iLoc != ILOC_TWOHAND) {
				if (owner.data.InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_NONE || owner.data.InvBody[INVLOC_HAND_LEFT]._iClass != owner.data.HoldItem._iClass) {
					NetSendCmdChItem(FALSE, INVLOC_HAND_RIGHT);
					owner.data.InvBody[INVLOC_HAND_RIGHT] = owner.data.HoldItem;
					break;
				}
				NetSendCmdChItem(FALSE, INVLOC_HAND_LEFT);
				cn = SwapItem(&owner.data.InvBody[INVLOC_HAND_LEFT], &owner.data.HoldItem);
				break;
			}
			NetSendCmdDelItem(FALSE, INVLOC_HAND_LEFT);
			NetSendCmdChItem(FALSE, INVLOC_HAND_RIGHT);
			SwapItem(&owner.data.InvBody[INVLOC_HAND_RIGHT], &owner.data.InvBody[INVLOC_HAND_LEFT]);
			cn = SwapItem(&owner.data.InvBody[INVLOC_HAND_RIGHT], &owner.data.HoldItem);
			break;
		}

		if (owner.data.InvBody[INVLOC_HAND_LEFT]._itype != ITYPE_NONE && owner.data.InvBody[INVLOC_HAND_LEFT]._iClass == owner.data.HoldItem._iClass) {
			NetSendCmdChItem(FALSE, INVLOC_HAND_LEFT);
			cn = SwapItem(&owner.data.InvBody[INVLOC_HAND_LEFT], &owner.data.HoldItem);
			break;
		}
		NetSendCmdChItem(FALSE, INVLOC_HAND_RIGHT);
		cn = SwapItem(&owner.data.InvBody[INVLOC_HAND_RIGHT], &owner.data.HoldItem);
		break;
	case ILOC_TWOHAND:
		NetSendCmdDelItem(FALSE, INVLOC_HAND_RIGHT);
		if (owner.data.InvBody[INVLOC_HAND_LEFT]._itype != ITYPE_NONE && owner.data.InvBody[INVLOC_HAND_RIGHT]._itype != ITYPE_NONE) {
			tempitem = owner.data.HoldItem;
			if (owner.data.InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_SHIELD)
				owner.data.HoldItem = owner.data.InvBody[INVLOC_HAND_RIGHT];
			else
				owner.data.HoldItem = owner.data.InvBody[INVLOC_HAND_LEFT];
			if (owner.isMyPlr())
				SetCursor_(owner.data.HoldItem._iCurs + CURSOR_FIRSTITEM);
			else
				SetICursor(owner.data.HoldItem._iCurs + CURSOR_FIRSTITEM);
			done2h = FALSE;
			for (i = 0; i < NUM_INV_GRID_ELEM && !done2h; i++)
				done2h = AutoPlace(i, { icursW28, icursH28 }, TRUE);
			owner.data.HoldItem = tempitem;
			if (owner.isMyPlr())
				SetCursor_(owner.data.HoldItem._iCurs + CURSOR_FIRSTITEM);
			else
				SetICursor(owner.data.HoldItem._iCurs + CURSOR_FIRSTITEM);
			if (!done2h)
				return;

			if (owner.data.InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_SHIELD)
				owner.data.InvBody[INVLOC_HAND_RIGHT]._itype = ITYPE_NONE;
			else
				owner.data.InvBody[INVLOC_HAND_LEFT]._itype = ITYPE_NONE;
		}

		if (owner.data.InvBody[INVLOC_HAND_LEFT]._itype != ITYPE_NONE || owner.data.InvBody[INVLOC_HAND_RIGHT]._itype != ITYPE_NONE) {
			NetSendCmdChItem(FALSE, INVLOC_HAND_LEFT);
			if (owner.data.InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_NONE)
				SwapItem(&owner.data.InvBody[INVLOC_HAND_LEFT], &owner.data.InvBody[INVLOC_HAND_RIGHT]);
			cn = SwapItem(&owner.data.InvBody[INVLOC_HAND_LEFT], &owner.data.HoldItem);
		} else {
			NetSendCmdChItem(FALSE, INVLOC_HAND_LEFT);
			owner.data.InvBody[INVLOC_HAND_LEFT] = owner.data.HoldItem;
		}
		if (owner.data.InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_STAFF && owner.data.InvBody[INVLOC_HAND_LEFT]._iSpell != 0 && owner.data.InvBody[INVLOC_HAND_LEFT]._iCharges > 0) {
			owner.data._pRSpell = owner.data.InvBody[INVLOC_HAND_LEFT]._iSpell;
			owner.data._pRSplType = RSPLTYPE_CHARGES;
			force_redraw = 255;
		}
		break;
	case ILOC_ARMOR:
		NetSendCmdChItem(FALSE, INVLOC_CHEST);
		if (owner.data.InvBody[INVLOC_CHEST]._itype == ITYPE_NONE)
			owner.data.InvBody[INVLOC_CHEST] = owner.data.HoldItem;
		else
			cn = SwapItem(&owner.data.InvBody[INVLOC_CHEST], &owner.data.HoldItem);
		break;
	case ILOC_UNEQUIPABLE:
		if (owner.data.HoldItem._itype == ITYPE_GOLD && it == 0) {
			ii = r - 25;
			yy = 10 * (ii / 10);
			xx = ii % 10;
			if (owner.data.InvGrid[yy + xx] > 0) {
				il = owner.data.InvGrid[yy + xx];
				il--;
				gt = owner.data.InvList[il]._ivalue;
				ig = owner.data.HoldItem._ivalue + gt;
				if (ig <= GOLD_MAX_LIMIT) {
					owner.data.InvList[il]._ivalue = ig;
					owner.data._pGold += owner.data.HoldItem._ivalue;
					if (ig >= GOLD_MEDIUM_LIMIT)
						owner.data.InvList[il]._iCurs = ICURS_GOLD_LARGE;
					else if (ig <= GOLD_SMALL_LIMIT)
						owner.data.InvList[il]._iCurs = ICURS_GOLD_SMALL;
					else
						owner.data.InvList[il]._iCurs = ICURS_GOLD_MEDIUM;
				} else {
					ig = GOLD_MAX_LIMIT - gt;
					owner.data._pGold += ig;
					owner.data.HoldItem._ivalue -= ig;
					owner.data.InvList[il]._ivalue = GOLD_MAX_LIMIT;
					owner.data.InvList[il]._iCurs = ICURS_GOLD_LARGE;
					// BUGFIX: incorrect values here are leftover from beta
					if (owner.data.HoldItem._ivalue >= GOLD_MEDIUM_LIMIT)
						cn = 18;
					else if (owner.data.HoldItem._ivalue <= GOLD_SMALL_LIMIT)
						cn = 16;
					else
						cn = 17;
				}
			} else {
				il = owner.data._pNumInv;
				owner.data.InvList[il] = owner.data.HoldItem;
				owner.data._pNumInv++;
				owner.data.InvGrid[yy + xx] = owner.data._pNumInv;
				owner.data._pGold += owner.data.HoldItem._ivalue;
				if (owner.data.HoldItem._ivalue <= GOLD_MAX_LIMIT) {
					if (owner.data.HoldItem._ivalue >= GOLD_MEDIUM_LIMIT)
						owner.data.InvList[il]._iCurs = ICURS_GOLD_LARGE;
					else if (owner.data.HoldItem._ivalue <= GOLD_SMALL_LIMIT)
						owner.data.InvList[il]._iCurs = ICURS_GOLD_SMALL;
					else
						owner.data.InvList[il]._iCurs = ICURS_GOLD_MEDIUM;
				}
			}
		} else {
			if (it == 0) {
				owner.data.InvList[owner.data._pNumInv] = owner.data.HoldItem;
				owner.data._pNumInv++;
				it = owner.data._pNumInv;
			} else {
				il = it - 1;
				if (owner.data.HoldItem._itype == ITYPE_GOLD)
					owner.data._pGold += owner.data.HoldItem._ivalue;
				cn = SwapItem(&owner.data.InvList[il], &owner.data.HoldItem);
				if (owner.data.HoldItem._itype == ITYPE_GOLD)
					owner.data._pGold = CalculateGold();
				for (i = 0; i < NUM_INV_GRID_ELEM; i++) {
					if (owner.data.InvGrid[i] == it)
						owner.data.InvGrid[i] = 0;
					if (owner.data.InvGrid[i] == -it)
						owner.data.InvGrid[i] = 0;
				}
			}
			ii = r - 25;
			yy = 10 * (ii / 10 - ((sy - 1) >> 1));
			if (yy < 0)
				yy = 0;
			for (j = 0; j < sy; j++) {
				xx = (ii % 10 - ((sx - 1) >> 1));
				if (xx < 0)
					xx = 0;
				for (i = 0; i < sx; i++) {
					if (i != 0 || j != sy - 1)
						owner.data.InvGrid[xx + yy] = -it;
					else
						owner.data.InvGrid[xx + yy] = it;
					xx++;
				}
				yy += 10;
			}
		}
		break;
	case ILOC_BELT:
		ii = r - 65;
		if (owner.data.HoldItem._itype == ITYPE_GOLD) {
			if (owner.data.SpdList[ii]._itype != ITYPE_NONE) {
				if (owner.data.SpdList[ii]._itype == ITYPE_GOLD) {
					i = owner.data.HoldItem._ivalue + owner.data.SpdList[ii]._ivalue;
					if (i <= GOLD_MAX_LIMIT) {
						owner.data.SpdList[ii]._ivalue += owner.data.HoldItem._ivalue;
						owner.data._pGold += owner.data.HoldItem._ivalue;
						if (i >= GOLD_MEDIUM_LIMIT)
							owner.data.SpdList[ii]._iCurs = ICURS_GOLD_LARGE;
						else if (i <= GOLD_SMALL_LIMIT)
							owner.data.SpdList[ii]._iCurs = ICURS_GOLD_SMALL;
						else
							owner.data.SpdList[ii]._iCurs = ICURS_GOLD_MEDIUM;
					} else {
						i = GOLD_MAX_LIMIT - owner.data.SpdList[ii]._ivalue;
						owner.data._pGold += i;
						owner.data.HoldItem._ivalue -= i;
						owner.data.SpdList[ii]._ivalue = GOLD_MAX_LIMIT;
						owner.data.SpdList[ii]._iCurs = ICURS_GOLD_LARGE;

						// BUGFIX: incorrect values here are leftover from beta
						if (owner.data.HoldItem._ivalue >= GOLD_MEDIUM_LIMIT)
							cn = 18;
						else if (owner.data.HoldItem._ivalue <= GOLD_SMALL_LIMIT)
							cn = 16;
						else
							cn = 17;
					}
				} else {
					owner.data._pGold += owner.data.HoldItem._ivalue;
					cn = SwapItem(&owner.data.SpdList[ii], &owner.data.HoldItem);
				}
			} else {
				owner.data.SpdList[ii] = owner.data.HoldItem;
				owner.data._pGold += owner.data.HoldItem._ivalue;
			}
		} else if (owner.data.SpdList[ii]._itype == ITYPE_NONE) {
			owner.data.SpdList[ii] = owner.data.HoldItem;
		} else {
			cn = SwapItem(&owner.data.SpdList[ii], &owner.data.HoldItem);
			if (owner.data.HoldItem._itype == ITYPE_GOLD)
				owner.data._pGold = CalculateGold();
		}
		drawsbarflag = TRUE;
		break;
	}
	CalcPlrInv(owner.id(), TRUE);
	if (owner.isMyPlr()) {
		if (cn == 1)
			SetCursorPos(Mouse.x + (cursW >> 1), Mouse.y + (cursH >> 1));
		SetCursor_(cn);
	}
}

void PlayerInventory::CheckInvSwap(BYTE bLoc, int idx, WORD wCI, int seed, bool bId)
{
	PlayerStruct *p;

	RecreateItem(MAXITEMS, idx, wCI, seed, 0);

	p = &owner.data;
	p->HoldItem = item[MAXITEMS];

	if (bId) {
		p->HoldItem._iIdentified = TRUE;
	}

	if (bLoc < NUM_INVLOC) {
		p->InvBody[bLoc] = p->HoldItem;

		if (bLoc == INVLOC_HAND_LEFT && p->HoldItem._iLoc == ILOC_TWOHAND) {
			p->InvBody[INVLOC_HAND_RIGHT]._itype = ITYPE_NONE;
		} else if (bLoc == INVLOC_HAND_RIGHT && p->HoldItem._iLoc == ILOC_TWOHAND) {
			p->InvBody[INVLOC_HAND_LEFT]._itype = ITYPE_NONE;
		}
	}

	CalcPlrInv(owner.id(), TRUE);
}

void PlayerInventory::CheckInvCut(V2Di pos)
{
	int mx = pos.x;
	int my = pos.y;

	int r;
	bool done;
	char ii;
	int iv, i, j, offs, ig;
	Player & owner = myplr();

	if (myplr().data._pmode > PM_WALK3) {
		return;
	}

	if (dropGoldFlag) {
		dropGoldFlag = FALSE;
		dropGoldValue = 0;
	}

	done = FALSE;

	for (r = 0; (DWORD)r < NUM_XY_SLOTS && !done; r++) {
		// check which inventory rectangle the mouse is in, if any
		if (mx >= InvRect[r].X
		    && mx < InvRect[r].X + (INV_SLOT_SIZE_PX + 1)
		    && my >= InvRect[r].Y - (INV_SLOT_SIZE_PX + 1)
		    && my < InvRect[r].Y) {
			done = TRUE;
			r--;
		}
	}

	if (!done) {
		// not on an inventory slot rectangle
		return;
	}

	owner.data.HoldItem._itype = ITYPE_NONE;

	if (
	    r >= SLOTXY_HEAD_FIRST
	    && r <= SLOTXY_HEAD_LAST
	    && owner.data.InvBody[INVLOC_HEAD]._itype != ITYPE_NONE) {
		NetSendCmdDelItem(FALSE, INVLOC_HEAD);
		owner.data.HoldItem = owner.data.InvBody[INVLOC_HEAD];
		owner.data.InvBody[INVLOC_HEAD]._itype = ITYPE_NONE;
	}

	if (
	    r == SLOTXY_RING_LEFT
	    && owner.data.InvBody[INVLOC_RING_LEFT]._itype != ITYPE_NONE) {
		NetSendCmdDelItem(FALSE, INVLOC_RING_LEFT);
		owner.data.HoldItem = owner.data.InvBody[INVLOC_RING_LEFT];
		owner.data.InvBody[INVLOC_RING_LEFT]._itype = ITYPE_NONE;
	}

	if (
	    r == SLOTXY_RING_RIGHT
	    && owner.data.InvBody[INVLOC_RING_RIGHT]._itype != ITYPE_NONE) {
		NetSendCmdDelItem(FALSE, INVLOC_RING_RIGHT);
		owner.data.HoldItem = owner.data.InvBody[INVLOC_RING_RIGHT];
		owner.data.InvBody[INVLOC_RING_RIGHT]._itype = ITYPE_NONE;
	}

	if (
	    r == SLOTXY_AMULET
	    && owner.data.InvBody[INVLOC_AMULET]._itype != ITYPE_NONE) {
		NetSendCmdDelItem(FALSE, INVLOC_AMULET);
		owner.data.HoldItem = owner.data.InvBody[INVLOC_AMULET];
		owner.data.InvBody[INVLOC_AMULET]._itype = ITYPE_NONE;
	}

	if (
	    r >= SLOTXY_HAND_LEFT_FIRST
	    && r <= SLOTXY_HAND_LEFT_LAST
	    && owner.data.InvBody[INVLOC_HAND_LEFT]._itype != ITYPE_NONE) {
		NetSendCmdDelItem(FALSE, INVLOC_HAND_LEFT);
		owner.data.HoldItem = owner.data.InvBody[INVLOC_HAND_LEFT];
		owner.data.InvBody[INVLOC_HAND_LEFT]._itype = ITYPE_NONE;
	}

	if (
	    r >= SLOTXY_HAND_RIGHT_FIRST
	    && r <= SLOTXY_HAND_RIGHT_LAST
	    && owner.data.InvBody[INVLOC_HAND_RIGHT]._itype != ITYPE_NONE) {
		NetSendCmdDelItem(FALSE, INVLOC_HAND_RIGHT);
		owner.data.HoldItem = owner.data.InvBody[INVLOC_HAND_RIGHT];
		owner.data.InvBody[INVLOC_HAND_RIGHT]._itype = ITYPE_NONE;
	}

	if (
	    r >= SLOTXY_CHEST_FIRST
	    && r <= SLOTXY_CHEST_LAST
	    && owner.data.InvBody[INVLOC_CHEST]._itype != ITYPE_NONE) {
		NetSendCmdDelItem(FALSE, INVLOC_CHEST);
		owner.data.HoldItem = owner.data.InvBody[INVLOC_CHEST];
		owner.data.InvBody[INVLOC_CHEST]._itype = ITYPE_NONE;
	}

	if (r >= SLOTXY_INV_FIRST && r <= SLOTXY_INV_LAST) {
		ig = r - SLOTXY_INV_FIRST;
		ii = owner.data.InvGrid[ig];
		if (ii != 0) {
			iv = ii;
			if (ii <= 0) {
				iv = -ii;
			}

			for (i = 0; i < NUM_INV_GRID_ELEM; i++) {
				if (owner.data.InvGrid[i] == iv || owner.data.InvGrid[i] == -iv) {
					owner.data.InvGrid[i] = 0;
				}
			}

			iv--;

			owner.data.HoldItem = owner.data.InvList[iv];
			owner.data._pNumInv--;

			if (owner.data._pNumInv > 0 && owner.data._pNumInv != iv) {
				owner.data.InvList[iv] = owner.data.InvList[owner.data._pNumInv];

				for (j = 0; j < NUM_INV_GRID_ELEM; j++) {
					if (owner.data.InvGrid[j] == owner.data._pNumInv + 1) {
						owner.data.InvGrid[j] = iv + 1;
					}
					if (owner.data.InvGrid[j] == -(owner.data._pNumInv + 1)) {
						owner.data.InvGrid[j] = -iv - 1;
					}
				}
			}
		}
	}

	if (r >= SLOTXY_BELT_FIRST) {
		offs = r - SLOTXY_BELT_FIRST;
		if (owner.data.SpdList[offs]._itype != ITYPE_NONE) {
			owner.data.HoldItem = owner.data.SpdList[offs];
			owner.data.SpdList[offs]._itype = ITYPE_NONE;
			drawsbarflag = TRUE;
		}
	}

	if (owner.data.HoldItem._itype != ITYPE_NONE) {
		if (owner.data.HoldItem._itype == ITYPE_GOLD) {
			owner.data._pGold = owner.inventory.CalculateGold();
		}

		CalcPlrInv(owner.id(), TRUE);
		owner.inventory.CheckItemStats();

		if (owner.isMyPlr()) {
			PlaySFX(IS_IGRAB);
			SetCursor_(owner.data.HoldItem._iCurs + CURSOR_FIRSTITEM);
			SetCursorPos(mx - (cursW >> 1), Mouse.y - (cursH >> 1));
		}
	}
}

void PlayerInventory::inv_update_rem_item(BYTE iv)
{
	if (iv < NUM_INVLOC) {
		owner.data.InvBody[iv]._itype = ITYPE_NONE;
	}

	if (owner.data._pmode != PM_DEATH) {
		CalcPlrInv(owner.id(), TRUE);
	} else {
		CalcPlrInv(owner.id(), FALSE);
	}
}

void PlayerInventory::RemoveInvItem(int iv)
{
	int i, j;

	iv++;

	for (i = 0; i < NUM_INV_GRID_ELEM; i++) {
		if (owner.data.InvGrid[i] == iv || owner.data.InvGrid[i] == -iv) {
			owner.data.InvGrid[i] = 0;
		}
	}

	iv--;
	owner.data._pNumInv--;

	if (owner.data._pNumInv > 0 && owner.data._pNumInv != iv) {
		owner.data.InvList[iv] = owner.data.InvList[owner.data._pNumInv];

		for (j = 0; j < NUM_INV_GRID_ELEM; j++) {
			if (owner.data.InvGrid[j] == owner.data._pNumInv + 1) {
				owner.data.InvGrid[j] = iv + 1;
			}
			if (owner.data.InvGrid[j] == -(owner.data._pNumInv + 1)) {
				owner.data.InvGrid[j] = -(iv + 1);
			}
		}
	}

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

void PlayerInventory::RemoveSpdBarItem(int iv)
{
	owner.data.SpdList[iv]._itype = ITYPE_NONE;

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
		}
	}
	force_redraw = 255;
}

void CheckInvItem()
{
	if (pcurs >= CURSOR_FIRSTITEM) {
		myplr().inventory.CheckInvPaste(Mouse);
	} else {
		myplr().inventory.CheckInvCut(Mouse);
	}
}

/**
 * Check for interactions with belt
 */
void CheckInvScrn()
{
	if (Mouse.x > 190 + PANEL_LEFT && Mouse.x < 437 + PANEL_LEFT
	    && Mouse.y > PANEL_TOP && Mouse.y < 33 + PANEL_TOP) {
		CheckInvItem();
	}
}

void PlayerInventory::CheckItemStats()
{
	PlayerStruct *p = &owner.data;

	p->HoldItem._iStatFlag = FALSE;

	if (p->_pStrength >= p->HoldItem._iMinStr
	    && p->_pMagic >= p->HoldItem._iMinMag
	    && p->_pDexterity >= p->HoldItem._iMinDex) {
		p->HoldItem._iStatFlag = TRUE;
	}
}

void PlayerInventory::CheckBookLevel()
{
	int slvl;

	if (owner.data.HoldItem._iMiscId == IMISC_BOOK) {
		owner.data.HoldItem._iMinMag = spelldata[owner.data.HoldItem._iSpell].sMinInt;
		slvl = owner.data._pSplLvl[owner.data.HoldItem._iSpell];
		while (slvl != 0) {
			owner.data.HoldItem._iMinMag += 20 * owner.data.HoldItem._iMinMag / 100;
			slvl--;
			if (owner.data.HoldItem._iMinMag + 20 * owner.data.HoldItem._iMinMag / 100 > 255) {
				owner.data.HoldItem._iMinMag = -1;
				slvl = 0;
			}
		}
	}
}

void PlayerInventory::CheckQuestItem()
{
	if (owner.data.HoldItem.IDidx == IDI_OPTAMULET)
		quests[Q_BLIND]._qactive = QUEST_DONE;
	if (owner.data.HoldItem.IDidx == IDI_MUSHROOM && quests[Q_MUSHROOM]._qactive == QUEST_ACTIVE && quests[Q_MUSHROOM]._qvar1 == QS_MUSHSPAWNED) {
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
	if (owner.data.HoldItem.IDidx == IDI_ANVIL) {
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
	if (owner.data.HoldItem.IDidx == IDI_GLDNELIX) {
		sfxdelay = 30;
		if (myplr().data._pClass == PC_WARRIOR) {
			sfxdnum = PS_WARR88;
		} else if (myplr().data._pClass == PC_ROGUE) {
			sfxdnum = PS_ROGUE88;
		} else if (myplr().data._pClass == PC_SORCERER) {
			sfxdnum = PS_MAGE88;
		}
	}
	if (owner.data.HoldItem.IDidx == IDI_ROCK) {
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
	if (owner.data.HoldItem.IDidx == IDI_ARMOFVAL) {
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

void PlayerInventory::InvGetItem(int ii)
{
	int i;

	if (dropGoldFlag) {
		dropGoldFlag = FALSE;
		dropGoldValue = 0;
	}

	if (grid[item[ii]._i.x][item[ii]._i.y].isItem()) {
		if (owner.isMyPlr() && pcurs >= CURSOR_FIRSTITEM)
			NetSendCmdPItem(TRUE, CMD_SYNCPUTITEM, myplr().pos().x, myplr().pos().y);
		item[ii]._iCreateInfo &= ~0x8000;
		owner.data.HoldItem = item[ii];
		CheckQuestItem();
		CheckBookLevel();
		CheckItemStats();
		grid[item[ii]._i.x][item[ii]._i.y].clearItem();
		i = 0;
		while (i < numitems) {
			if (itemactive[i] == ii) {
				DeleteItem(itemactive[i], i);
				i = 0;
			} else {
				i++;
			}
		}
		pcursitem = -1;
		SetCursor_(owner.data.HoldItem._iCurs + CURSOR_FIRSTITEM);
	}
}

void PlayerInventory::AutoGetItem(int ii)
{
	int i, idx;
	int w, h;
	bool done;

	if (pcurs != CURSOR_HAND) {
		return;
	}

	if (dropGoldFlag) {
		dropGoldFlag = FALSE;
		dropGoldValue = 0;
	}

	if (ii != MAXITEMS) {
		if (grid[item[ii]._i.x][item[ii]._i.y].isItem())
			return;
	}

	item[ii]._iCreateInfo &= 0x7FFF;
	owner.data.HoldItem = item[ii]; /// BUGFIX: overwrites cursor item, allowing for belt dupe bug
	CheckQuestItem();
	CheckBookLevel();
	CheckItemStats();
	SetICursor(owner.data.HoldItem._iCurs + CURSOR_FIRSTITEM);
	if (owner.data.HoldItem._itype == ITYPE_GOLD) {
		done = GoldAutoPlace();
	} else {
		done = FALSE;
		if (((owner.data._pgfxnum & 0xF) == ANIM_ID_UNARMED || (owner.data._pgfxnum & 0xF) == ANIM_ID_UNARMED_SHIELD) && owner.data._pmode <= PM_WALK3) {
			if (owner.data.HoldItem._iStatFlag) {
				if (owner.data.HoldItem._iClass == ICLASS_WEAPON) {
					done = WeaponAutoPlace();
					if (done)
						CalcPlrInv(owner.id(), TRUE);
				}
			}
		}
		if (!done) {
			w = icursW28;
			h = icursH28;
			if (w == 1 && h == 1) {
				idx = owner.data.HoldItem.IDidx;
				if (owner.data.HoldItem._iStatFlag && AllItemsList[idx].iUsable) {
					for (i = 0; i < 8 && !done; i++) {
						if (owner.data.SpdList[i]._itype == ITYPE_NONE) {
							owner.data.SpdList[i] = owner.data.HoldItem;
							CalcPlrScrolls(owner.id());
							drawsbarflag = TRUE;
							done = TRUE;
						}
					}
				}
				for (i = 30; i <= 39 && !done; i++) {
					done = AutoPlace(i, { w, h }, TRUE);
				}
				for (i = 20; i <= 29 && !done; i++) {
					done = AutoPlace(i, { w, h }, TRUE);
				}
				for (i = 10; i <= 19 && !done; i++) {
					done = AutoPlace(i, { w, h }, TRUE);
				}
				for (i = 0; i <= 9 && !done; i++) {
					done = AutoPlace(i, { w, h }, TRUE);
				}
			}
			if (w == 1 && h == 2) {
				for (i = 29; i >= 20 && !done; i--) {
					done = AutoPlace(i, { w, h }, TRUE);
				}
				for (i = 9; i >= 0 && !done; i--) {
					done = AutoPlace(i, { w, h }, TRUE);
				}
				for (i = 19; i >= 10 && !done; i--) {
					done = AutoPlace(i, { w, h }, TRUE);
				}
			}
			if (w == 1 && h == 3) {
				for (i = 0; i < 20 && !done; i++) {
					done = AutoPlace(i, { w, h }, TRUE);
				}
			}
			if (w == 2 && h == 2) {
				for (i = 0; i < 10 && !done; i++) {
					done = AutoPlace(AP2x2Tbl[i], { w, h }, TRUE);
				}
				for (i = 21; i < 29 && !done; i += 2) {
					done = AutoPlace(i, { w, h }, TRUE);
				}
				for (i = 1; i < 9 && !done; i += 2) {
					done = AutoPlace(i, { w, h }, TRUE);
				}
				for (i = 10; i < 19 && !done; i++) {
					done = AutoPlace(i, { w, h }, TRUE);
				}
			}
			if (w == 2 && h == 3) {
				for (i = 0; i < 9 && !done; i++) {
					done = AutoPlace(i, { w, h }, TRUE);
				}
				for (i = 10; i < 19 && !done; i++) {
					done = AutoPlace(i, { w, h }, TRUE);
				}
			}
		}
	}
	if (done) {
		grid[item[ii]._i.x][item[ii]._i.y].clearItem();
		i = 0;
		while (i < numitems) {
			if (itemactive[i] == ii) {
				DeleteItem(itemactive[i], i);
				i = 0;
			} else {
				i++;
			}
		}
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
		owner.data.HoldItem = item[ii];
		RespawnItem(ii, TRUE);
		NetSendCmdPItem(TRUE, CMD_RESPAWNITEM, item[ii]._i.x, item[ii]._i.y);
		owner.data.HoldItem._itype = ITYPE_NONE;
	}
}

int FindGetItem(int idx, WORD ci, int iseed)
{
	int i, ii;
	i = 0;
	if (numitems <= 0)
		return -1;

	while (1) {
		ii = itemactive[i];
		if (item[ii].IDidx == idx && item[ii]._iSeed == iseed && item[ii]._iCreateInfo == ci)
			break;

		i++;

		if (i >= numitems)
			return -1;
	}
	return ii;
}

void SyncGetItem(V2Di pos, int idx, WORD ci, int iseed)
{
	int x = pos.x;
	int y = pos.y;
	int i, ii;

	if (grid[x][y].isItem()) {
		ii = grid[x][y].getItem();
		if (item[ii].IDidx == idx
		    && item[ii]._iSeed == iseed
		    && item[ii]._iCreateInfo == ci) {
			FindGetItem(idx, ci, iseed);
		} else {
			ii = FindGetItem(idx, ci, iseed);
		}
	} else {
		ii = FindGetItem(idx, ci, iseed);
	}

	if (ii != -1) {
		grid[item[ii]._i.x][item[ii]._i.y].clearItem();
		i = 0;
		while (i < numitems) {
			if (itemactive[i] == ii) {
				DeleteItem(itemactive[i], i);
				FindGetItem(idx, ci, iseed);
				/// ASSERT: assert(FindGetItem(idx,ci,iseed) == -1);
				FindGetItem(idx, ci, iseed); /* todo: replace with above */
				i = 0;
			} else {
				i++;
			}
		}
		/// ASSERT: assert(FindGetItem(idx, ci, iseed) == -1);
		FindGetItem(idx, ci, iseed); /* todo: replace with above */
	}
}

bool CanPut(V2Di p)
{
	if (grid.at(p).isItem())
		return FALSE;
	if (grid.at(p).isSolid())
		return FALSE;

	if (grid.at(p).isObject()) {
		uint8_t oi = grid.at(p).getObject();
		if (object[oi]._oSolidFlag) return FALSE;
	}

	V2Di f = p + V2Di(1, 1);
	if (grid.at(f).isObject()) {
		uint8_t oi = grid.at(f).getObject();
		if (object[oi]._oSelFlag != 0) return FALSE;
	}

	V2Di g = p + V2Di(1, 0);
	if (grid.at(g).isObject()) {
		uint8_t oi = grid.at(g).getObject();
		if (object[oi]._oSelFlag != 0) return FALSE;
	}

	if (level.currlevel == 0 && !grid.at(p).isMonster()) return FALSE;
	if (level.currlevel == 0 && !grid.at(f).isMonster()) return FALSE;

	return TRUE;
}

bool TryInvPut()
{
	if (numitems >= 127) return FALSE;
	Dir dir = GetDirection(myplr().pos(), cursm);
	V2Di off = offset(dir);
	if (CanPut(myplr().pos() + off)) {
		return TRUE;
	}

	//dir = (dir - 1) & 7;
	if (CanPut(myplr().pos() + off)) {
		return TRUE;
	}

	//dir = (dir + 2) & 7;
	if (CanPut(myplr().pos() + off)) {
		return TRUE;
	}
	return CanPut(myplr().pos());
}

void DrawInvMsg(char *msg)
{
	DWORD dwTicks;

	dwTicks = SDL_GetTicks();
	if (dwTicks - sgdwLastTime >= 5000) {
		sgdwLastTime = dwTicks;
		ErrorPlrMsg(msg);
	}
}

// EF Added
int PlayerInventory::_PrepPutItem(V2Di &pos)
{
	int i, j, l;
	V2Di n, p;
	if (numitems >= 127) return -1;

	Dir d2 = GetDirection(owner.pos(), pos);
	V2Di off = offset(d2);
	n = pos - owner.pos();
	if (abs(n.x) > 1 || abs(n.y) > 1) {
		pos = owner.pos() + off;
	}
	if (!CanPut(pos)) {
		//d = (d - 1) & 7;
		pos = owner.pos() + off;
		if (!CanPut(pos)) {
			//d = (d + 2) & 7;
			pos = owner.pos() + off;
			if (!CanPut(pos)) {
				for (l = 1; l < 50; l++) {
					for (j = -l; j <= l; j++) {
						p.y = j + owner.pos().y;
						for (i = -l; i <= l; i++) {
							p.x = i + owner.pos().x;
							if (CanPut(p)) {
								pos = p;
								goto valid;
							}
						}
					}
				}
				return -1;
			}
		}
	}
	valid:
	int ii = itemavail[0];
	grid[pos.x][pos.y].setItem(ii);
	itemavail[0] = itemavail[MAXITEMS - (numitems + 1)];
	itemactive[numitems] = ii;
	return ii;
}

int PlayerInventory::InvPutItem(V2Di pos)
{
	if (FindGetItem(owner.data.HoldItem.IDidx, owner.data.HoldItem._iCreateInfo, owner.data.HoldItem._iSeed) != -1) {
		DrawInvMsg("A duplicate item has been detected.  Destroying duplicate...");
		SyncGetItem(pos, owner.data.HoldItem.IDidx, owner.data.HoldItem._iCreateInfo, owner.data.HoldItem._iSeed);
	}
	int ii = _PrepPutItem(pos);
	if (ii < 0) return -1;

	item[ii] = owner.data.HoldItem;
	item[ii]._i = pos;
	RespawnItem(ii, TRUE);
	numitems++;
	NewCursor(CURSOR_HAND);
	return ii;
}

int PlayerInventory::SyncPutItem(V2Di pos, int idx, WORD icreateinfo, int iseed, int Id, int dur, int mdur, int ch, int mch, int ivalue, DWORD ibuff)
{
	if (FindGetItem(idx, icreateinfo, iseed) != -1) {
		DrawInvMsg("A duplicate item has been detected from another player.");
		SyncGetItem(pos, idx, icreateinfo, iseed);
	}

	int ii = _PrepPutItem(pos);
	if (ii < 0) return -1;

	if (idx == IDI_EAR) {
		RecreateEar(ii, icreateinfo, iseed, Id, dur, mdur, ch, mch, ivalue, ibuff);
	} else {
		RecreateItem(ii, idx, icreateinfo, iseed, ivalue);
		if (Id)
			item[ii]._iIdentified = TRUE;
		item[ii]._iDurability = dur;
		item[ii]._iMaxDur = mdur;
		item[ii]._iCharges = ch;
		item[ii]._iMaxCharges = mch;
	}

	item[ii]._i = pos;
	RespawnItem(ii, TRUE);
	numitems++;
	return ii;
}

char CheckInvHLight()
{
	int r, ii, nGold;
	ItemStruct *pi;
	PlayerStruct *p;
	char rv;

	for (r = 0; (DWORD)r < 73; r++) {
		if (Mouse.x >= InvRect[r].X
		    && Mouse.x < InvRect[r].X + 29
		    && Mouse.y >= InvRect[r].Y - 29
		    && Mouse.y < InvRect[r].Y) {
			break;
		}
	}

	if ((DWORD)r >= 73)
		return -1;

	rv = -1;
	infoclr = COL_WHITE;
	pi = NULL;
	p = &myplr().data;
	ClearPanel();
	if (r >= 0 && r <= 3) {
		rv = INVLOC_HEAD;
		pi = &p->InvBody[rv];
	} else if (r == 4) {
		rv = INVLOC_RING_LEFT;
		pi = &p->InvBody[rv];
	} else if (r == 5) {
		rv = INVLOC_RING_RIGHT;
		pi = &p->InvBody[rv];
	} else if (r == 6) {
		rv = INVLOC_AMULET;
		pi = &p->InvBody[rv];
	} else if (r >= 7 && r <= 12) {
		rv = INVLOC_HAND_LEFT;
		pi = &p->InvBody[rv];
	} else if (r >= 13 && r <= 18) {
		pi = &p->InvBody[INVLOC_HAND_LEFT];
		if (pi->_itype == ITYPE_NONE || pi->_iLoc != ILOC_TWOHAND) {
			rv = INVLOC_HAND_RIGHT;
			pi = &p->InvBody[rv];
		} else {
			rv = INVLOC_HAND_LEFT;
		}
	} else if (r >= 19 && r <= 24) {
		rv = INVLOC_CHEST;
		pi = &p->InvBody[rv];
	} else if (r >= 25 && r <= 64) {
		r = abs(p->InvGrid[r - 25]);
		if (r == 0)
			return -1;
		ii = r - 1;
		rv = ii + 7;
		pi = &p->InvList[ii];
	} else if (r >= 65) {
		r -= 65;
		drawsbarflag = TRUE;
		pi = &p->SpdList[r];
		if (pi->_itype == ITYPE_NONE)
			return -1;
		rv = r + 47;
	}

	if (pi->_itype == ITYPE_NONE)
		return -1;

	if (pi->_itype == ITYPE_GOLD) {
		nGold = pi->_ivalue;
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
			PrintItemDetails(pi);
		} else {
			PrintItemDur(pi);
		}
	}

	return rv;
}

void PlayerInventory::RemoveScroll()
{
	int i;

	for (i = 0; i < owner.data._pNumInv; i++) {
		if (owner.data.InvList[i]._itype != ITYPE_NONE
		    && (owner.data.InvList[i]._iMiscId == IMISC_SCROLL || owner.data.InvList[i]._iMiscId == IMISC_SCROLLT)
		    && owner.data.InvList[i]._iSpell == owner.data._pRSpell) {
			RemoveInvItem(i);
			CalcPlrScrolls(owner.id());
			return;
		}
	}
	for (i = 0; i < MAXBELTITEMS; i++) {
		if (owner.data.SpdList[i]._itype != ITYPE_NONE
		    && (owner.data.SpdList[i]._iMiscId == IMISC_SCROLL || owner.data.SpdList[i]._iMiscId == IMISC_SCROLLT)
		    && owner.data.SpdList[i]._iSpell == owner.data._pRSpell) {
			RemoveSpdBarItem(i);
			CalcPlrScrolls(owner.id());
			return;
		}
	}
}

bool UseScroll()
{
	int i;

	if (pcurs != CURSOR_HAND)
		return FALSE;
	if (level.leveltype == DTYPE_TOWN && !spelldata[myplr().data._pRSpell].sTownSpell)
		return FALSE;

	for (i = 0; i < myplr().data._pNumInv; i++) {
		if (myplr().data.InvList[i]._itype != ITYPE_NONE
		    && (myplr().data.InvList[i]._iMiscId == IMISC_SCROLL || myplr().data.InvList[i]._iMiscId == IMISC_SCROLLT)
		    && myplr().data.InvList[i]._iSpell == myplr().data._pRSpell) {
			return TRUE;
		}
	}
	for (i = 0; i < MAXBELTITEMS; i++) {
		if (myplr().data.SpdList[i]._itype != ITYPE_NONE
		    && (myplr().data.SpdList[i]._iMiscId == IMISC_SCROLL || myplr().data.SpdList[i]._iMiscId == IMISC_SCROLLT)
		    && myplr().data.SpdList[i]._iSpell == myplr().data._pRSpell) {
			return TRUE;
		}
	}

	return FALSE;
}

void PlayerInventory::UseStaffCharge()
{
	if (owner.data.InvBody[INVLOC_HAND_LEFT]._itype != ITYPE_NONE
	    && owner.data.InvBody[INVLOC_HAND_LEFT]._iMiscId == IMISC_STAFF
	    && owner.data.InvBody[INVLOC_HAND_LEFT]._iSpell == owner.data._pRSpell
	    && owner.data.InvBody[INVLOC_HAND_LEFT]._iCharges > 0) {
		owner.data.InvBody[INVLOC_HAND_LEFT]._iCharges--;
		CalcPlrStaff(owner.id());
	}
}

bool UseStaff()
{
	if (pcurs == CURSOR_HAND) {
		if (myplr().data.InvBody[INVLOC_HAND_LEFT]._itype != ITYPE_NONE
		    && myplr().data.InvBody[INVLOC_HAND_LEFT]._iMiscId == IMISC_STAFF
		    && myplr().data.InvBody[INVLOC_HAND_LEFT]._iSpell == myplr().data._pRSpell
		    && myplr().data.InvBody[INVLOC_HAND_LEFT]._iCharges > 0) {
			return TRUE;
		}
	}

	return FALSE;
}

void StartGoldDrop()
{
	initialDropGoldIndex = pcursinvitem;
	if (pcursinvitem <= INVITEM_INV_LAST)
		initialDropGoldValue = myplr().data.InvList[pcursinvitem - INVITEM_INV_FIRST]._ivalue;
	else
		initialDropGoldValue = myplr().data.SpdList[pcursinvitem - INVITEM_BELT_FIRST]._ivalue;
	dropGoldFlag = TRUE;
	dropGoldValue = 0;
	if (talkflag)
		control_reset_talk();
}

bool PlayerInventory::UseInvItem(int cii)
{
	int c, idata;
	ItemStruct *Item;
	bool speedlist;

	if (owner.data._pInvincible && !owner.data._pHitPoints && owner.isMyPlr())
		return TRUE;
	if (pcurs != CURSOR_HAND)
		return TRUE;
	if (stextflag != STORE_NONE)
		return TRUE;
	if (cii <= INVITEM_HAND_RIGHT)
		return FALSE;

	if (cii <= INVITEM_INV_LAST) {
		c = cii - INVITEM_INV_FIRST;
		Item = &owner.data.InvList[c];
		speedlist = FALSE;
	} else {
		if (talkflag)
			return TRUE;
		c = cii - INVITEM_BELT_FIRST;
		Item = &owner.data.SpdList[c];
		speedlist = TRUE;
	}

	switch (Item->IDidx) {
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

	if (!AllItemsList[Item->IDidx].iUsable)
		return FALSE;

	if (!Item->_iStatFlag) {
		if (owner.data._pClass == PC_WARRIOR) {
			PlaySFX(PS_WARR13);
		} else if (owner.data._pClass == PC_ROGUE) {
			PlaySFX(PS_ROGUE13);
		} else if (owner.data._pClass == PC_SORCERER) {
			PlaySFX(PS_MAGE13);
		}
		return TRUE;
	}

	if (Item->_iMiscId == IMISC_NONE && Item->_itype == ITYPE_GOLD) {
		StartGoldDrop();
		return TRUE;
	}

	if (dropGoldFlag) {
		dropGoldFlag = FALSE;
		dropGoldValue = 0;
	}

	if (Item->_iMiscId == IMISC_SCROLL && level.currlevel == 0 && !spelldata[Item->_iSpell].sTownSpell) {
		return TRUE;
	}

	if (Item->_iMiscId == IMISC_SCROLLT && level.currlevel == 0 && !spelldata[Item->_iSpell].sTownSpell) {
		return TRUE;
	}

	idata = ItemCAnimTbl[Item->_iCurs];
	if (Item->_iMiscId == IMISC_BOOK)
		PlaySFX(IS_RBOOK);
	else if (owner.isMyPlr())
		PlaySFX(ItemInvSnds[idata]);

	UseItem(owner.id(), Item->_iMiscId, Item->_iSpell);

	if (speedlist) {
		RemoveSpdBarItem(c);
		return TRUE;
	} else {
		if (owner.data.InvList[c]._iMiscId == IMISC_MAPOFDOOM)
			return TRUE;
		RemoveInvItem(c);
	}

	return TRUE;
}

void DoTelekinesis()
{
	if (pcursobj != -1)
		NetSendCmdParam1(TRUE, CMD_OPOBJT, pcursobj);
	if (pcursitem != -1)
		NetSendCmdGItem(TRUE, CMD_REQUESTAGITEM, myplr(), myplr(), pcursitem);
	if (pcursmonst != -1 && !monsters[pcursmonst].M_Talker() && monsters[pcursmonst].data.mtalkmsg == 0)
		NetSendCmdParam1(TRUE, CMD_KNOCKBACK, pcursmonst);
	NewCursor(CURSOR_HAND);
}

int PlayerInventory::CalculateGold()
{
	int i, gold;

	gold = 0;
	for (i = 0; i < MAXBELTITEMS; i++) {
		if (owner.data.SpdList[i]._itype == ITYPE_GOLD) {
			gold += owner.data.SpdList[i]._ivalue;
			force_redraw = 255;
		}
	}
	for (i = 0; i < owner.data._pNumInv; i++) {
		if (owner.data.InvList[i]._itype == ITYPE_GOLD)
			gold += owner.data.InvList[i]._ivalue;
	}
	assert(gold >= 0);
	return gold;
}

bool DropItemBeforeTrig()
{
	if (TryInvPut()) {
		NetSendCmdPItem(TRUE, CMD_PUTITEM, cursm.x, cursm.y);
		NewCursor(CURSOR_HAND);
		return TRUE;
	}

	return FALSE;
}

DEVILUTION_END_NAMESPACE
