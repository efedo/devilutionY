/**
 * @file inv_enums.h
 *
 * Inv enums
 */

#ifndef __INV_ENUMS_H__
#define __INV_ENUMS_H__

namespace dvl {

// Logical equipment locations
enum class BodyLoc {
	Head = 0,
	RingLeft = 1,
	RingRight = 2,
	Amulet = 3,
	HandLeft = 4,
	HandRight = 5,
	Chest = 6,
	Belt = 7,
	Bag = 8,
	InHand = 9,
	Invalid = -1
};

// Maps from slot map to
typedef enum inv_item {
	INVITEM_HEAD = 0,
	INVITEM_RING_LEFT = 1,
	INVITEM_RING_RIGHT = 2,
	INVITEM_AMULET = 3,
	INVITEM_HAND_LEFT = 4,
	INVITEM_HAND_RIGHT = 5,
	INVITEM_CHEST = 6,
	INVITEM_INV_FIRST = 7,
	INVITEM_INV_LAST = 46,
	INVITEM_BELT_FIRST = 47,
	INVITEM_BELT_LAST = 54,
	NUM_INVELEM
} inv_item;

// identifiers for each of the inventory squares
// see https://github.com/sanctuary/graphics/blob/master/inventory.png
typedef enum inv_xy_slot {
	SLOTXY_HEAD_FIRST = 0,
	SLOTXY_HEAD_LAST = 3,
	SLOTXY_RING_LEFT = 4,
	SLOTXY_RING_RIGHT = 5,
	SLOTXY_AMULET = 6,
	SLOTXY_HAND_LEFT_FIRST = 7,
	SLOTXY_HAND_LEFT_LAST = 12,
	SLOTXY_HAND_RIGHT_FIRST = 13,
	SLOTXY_HAND_RIGHT_LAST = 18,
	SLOTXY_CHEST_FIRST = 19,
	SLOTXY_CHEST_LAST = 24,

	// regular inventory
	SLOTXY_INV_FIRST = 25,
	SLOTXY_INV_LAST = 64,

	// belt items
	SLOTXY_BELT_FIRST = 65,
	SLOTXY_BELT_LAST = 72,
	NUM_XY_SLOTS = 73
} inv_xy_slot;

}

#endif /* __INV_ENUMS_H__ */
