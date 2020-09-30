/**
 * @file inv_rect.h
 *
 * Inv display rectangle
 */

#ifndef __INV_RECT_H__
#define __INV_RECT_H__

#include "inv_enums.h"

DEVILUTION_BEGIN_NAMESPACE

class InvSlot {
public:
	RECT32 rect; // screen rect
	BodyLoc bLoc = BodyLoc::Invalid;
	V2Di gridpos;
	Item *item();
	void destroyItem(); // remember to unblock adjacent slots
	void takeItem(std::unique_ptr<Item> &ptr);
	bool filled = false;
protected:
	std::unique_ptr<Item> _item = 0;
};

DEVILUTION_END_NAMESPACE

#endif /* __INV_RECT_H__ */
