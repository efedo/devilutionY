/**
 * @file inv.cpp
 *
 * Inv display rectangle
 */
#include "all.h"

namespace dvl {


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
//const InvXY InvRect[] = {
//	// clang-format off
//	//  X,   Y
//	{ RIGHT_PANEL + 132,  31 }, // helmet
//	{ RIGHT_PANEL + 160,  31 }, // helmet
//	{ RIGHT_PANEL + 132,  59 }, // helmet
//	{ RIGHT_PANEL + 160,  59 }, // helmet
//	{ RIGHT_PANEL + 45,  205 }, // left ring
//	{ RIGHT_PANEL + 247, 205 }, // right ring
//	{ RIGHT_PANEL + 204,  59 }, // amulet
//	{ RIGHT_PANEL + 17,  104 }, // left hand
//	{ RIGHT_PANEL + 46,  104 }, // left hand
//	{ RIGHT_PANEL + 17,  132 }, // left hand
//	{ RIGHT_PANEL + 46,  132 }, // left hand
//	{ RIGHT_PANEL + 17,  160 }, // left hand
//	{ RIGHT_PANEL + 46,  160 }, // left hand
//	{ RIGHT_PANEL + 247, 104 }, // right hand
//	{ RIGHT_PANEL + 276, 104 }, // right hand
//	{ RIGHT_PANEL + 247, 132 }, // right hand
//	{ RIGHT_PANEL + 276, 132 }, // right hand
//	{ RIGHT_PANEL + 247, 160 }, // right hand
//	{ RIGHT_PANEL + 276, 160 }, // right hand
//	{ RIGHT_PANEL + 132, 104 }, // chest
//	{ RIGHT_PANEL + 160, 104 }, // chest
//	{ RIGHT_PANEL + 132, 132 }, // chest
//	{ RIGHT_PANEL + 160, 132 }, // chest
//	{ RIGHT_PANEL + 132, 160 }, // chest
//	{ RIGHT_PANEL + 160, 160 }, // chest
//	{ RIGHT_PANEL + 17,  250 }, // inv row 1
//	{ RIGHT_PANEL + 46,  250 }, // inv row 1
//	{ RIGHT_PANEL + 74,  250 }, // inv row 1
//	{ RIGHT_PANEL + 103, 250 }, // inv row 1
//	{ RIGHT_PANEL + 131, 250 }, // inv row 1
//	{ RIGHT_PANEL + 160, 250 }, // inv row 1
//	{ RIGHT_PANEL + 189, 250 }, // inv row 1
//	{ RIGHT_PANEL + 218, 250 }, // inv row 1
//	{ RIGHT_PANEL + 247, 250 }, // inv row 1
//	{ RIGHT_PANEL + 276, 250 }, // inv row 1
//	{ RIGHT_PANEL + 17,  279 }, // inv row 2
//	{ RIGHT_PANEL + 46,  279 }, // inv row 2
//	{ RIGHT_PANEL + 74,  279 }, // inv row 2
//	{ RIGHT_PANEL + 103, 279 }, // inv row 2
//	{ RIGHT_PANEL + 131, 279 }, // inv row 2
//	{ RIGHT_PANEL + 160, 279 }, // inv row 2
//	{ RIGHT_PANEL + 189, 279 }, // inv row 2
//	{ RIGHT_PANEL + 218, 279 }, // inv row 2
//	{ RIGHT_PANEL + 247, 279 }, // inv row 2
//	{ RIGHT_PANEL + 276, 279 }, // inv row 2
//	{ RIGHT_PANEL + 17,  308 }, // inv row 3
//	{ RIGHT_PANEL + 46,  308 }, // inv row 3
//	{ RIGHT_PANEL + 74,  308 }, // inv row 3
//	{ RIGHT_PANEL + 103, 308 }, // inv row 3
//	{ RIGHT_PANEL + 131, 308 }, // inv row 3
//	{ RIGHT_PANEL + 160, 308 }, // inv row 3
//	{ RIGHT_PANEL + 189, 308 }, // inv row 3
//	{ RIGHT_PANEL + 218, 308 }, // inv row 3
//	{ RIGHT_PANEL + 247, 308 }, // inv row 3
//	{ RIGHT_PANEL + 276, 308 }, // inv row 3
//	{ RIGHT_PANEL + 17,  336 }, // inv row 4
//	{ RIGHT_PANEL + 46,  336 }, // inv row 4
//	{ RIGHT_PANEL + 74,  336 }, // inv row 4
//	{ RIGHT_PANEL + 103, 336 }, // inv row 4
//	{ RIGHT_PANEL + 131, 336 }, // inv row 4
//	{ RIGHT_PANEL + 160, 336 }, // inv row 4
//	{ RIGHT_PANEL + 189, 336 }, // inv row 4
//	{ RIGHT_PANEL + 218, 336 }, // inv row 4
//	{ RIGHT_PANEL + 247, 336 }, // inv row 4
//	{ RIGHT_PANEL + 276, 336 }, // inv row 4
	//{ PANEL_LEFT + 205, PANEL_TOP + 33 }, // belt
	//{ PANEL_LEFT + 234, PANEL_TOP + 33 }, // belt
	//{ PANEL_LEFT + 263, PANEL_TOP + 33 }, // belt
	//{ PANEL_LEFT + 292, PANEL_TOP + 33 }, // belt
	//{ PANEL_LEFT + 321, PANEL_TOP + 33 }, // belt
	//{ PANEL_LEFT + 350, PANEL_TOP + 33 }, // belt
	//{ PANEL_LEFT + 379, PANEL_TOP + 33 }, // belt
	//{ PANEL_LEFT + 408, PANEL_TOP + 33 }  // belt
	// clang-format on
//};

PlayerInventory::PlayerInventory(Player &new_owner)
    : owner(new_owner)
{

	// Set body slot positions
	// clang-format off
	const int sz = INV_SLOT_SIZE_PX;
	body.head =				{{ 133 + RIGHT_PANEL_X,  59 + SCREEN_Y, 2 * sz, 2 * sz }, BodyLoc::Head};
	body.ring_left =		{{  48 + RIGHT_PANEL_X, 205 + SCREEN_Y, 1 * sz, 1 * sz }, BodyLoc::RingLeft};
	body.right_right =		{{ 249 + RIGHT_PANEL_X, 205 + SCREEN_Y, 1 * sz, 1 * sz }, BodyLoc::RingRight};
	body.amulet =			{{ 205 + RIGHT_PANEL_X,  60 + SCREEN_Y, 1 * sz, 1 * sz }, BodyLoc::Amulet};
	body.hand_left =		{{  17 + RIGHT_PANEL_X, 160 + SCREEN_Y, 2 * sz, 3 * sz }, BodyLoc::HandLeft};
	body.hand_right =		{{ 247 + RIGHT_PANEL_X, 160 + SCREEN_Y, 2 * sz, 3 * sz }, BodyLoc::HandRight};
	body.chest =			{{ 133 + RIGHT_PANEL_X, 160 + SCREEN_Y, 2 * sz, 3 * sz }, BodyLoc::Chest};
	// clang-format on

	// Set inventory slot positions
	int n = 0;
	RECT32 rect = { RIGHT_PANEL + SCREEN_X + 17, 250 + SCREEN_Y - 1, sz, sz };
	for (int j = 0; j < 4; ++j) {
		rect.x = RIGHT_PANEL + SCREEN_X + 17;
		for (int i = 0; i < 10, ++i) {
			bag.grid[n] = { rect, BodyLoc::Bag, { i, j } };
			rect.x += 29;
			++n;
		}
		rect.y += 29;
	}

	// Set belt slot positions
	for (int i = 0; i < MAXBELTITEMS; ++i) {
		belt.slots[i] = { { PANEL_LEFT + SCREEN_X + 205 + 29 * i,
			                  PANEL_TOP + SCREEN_Y - 1 + 33,
			                  sz,
			                  sz },
			BodyLoc::Belt,
			{ i, 0 } }
	} 
}

void PlayerInventory::FreeInvGFX()
{
	MemFreeDbg(pInvCels);
}

void PlayerInventory::InitInv()
{
	if (myplr().data._pClass == PlayerClass::warrior) {
		pInvCels = LoadFileInMem("Data\\Inv\\Inv.CEL", NULL);
	} else if (myplr().data._pClass == PlayerClass::rogue) {
		pInvCels = LoadFileInMem("Data\\Inv\\Inv_rog.CEL", NULL);
	} else if (myplr().data._pClass == PlayerClass::sorceror) {
		pInvCels = LoadFileInMem("Data\\Inv\\Inv_Sor.CEL", NULL);
	}

	invflag = false;
}


}
