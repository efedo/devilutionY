/**
 * @file inv_rect.h
 *
 * Inv display rectangle
 */

#ifndef __INV_RECT_H__
#define __INV_RECT_H__

#include "inv_enums.h"

DEVILUTION_BEGIN_NAMESPACE

struct InvSlot {
	RECT32 rect; // x, y, width, height (in squares)
	BodyLoc bLoc = BodyLoc::Invalid;
	V2Di gridpos;
	std::unique_ptr<Item> item = 0;
};

DEVILUTION_END_NAMESPACE

#endif /* __INV_RECT_H__ */
