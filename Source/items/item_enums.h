/**
 * @file item_id.h
 *
 * Item id
 */

#ifndef __ITEM_ENUMS_H__
#define __ITEM_ENUMS_H__

DEVILUTION_BEGIN_NAMESPACE

// Item equip type
enum class ItemClass {
	None = 0x0,
	OneHand = 0x1,
	TwoHand = 0x2,
	Armor = 0x3,
	Helm = 0x4,
	Ring = 0x5,
	Amulet = 0x6,
	Unequipable = 0x7,
	Belt = 0x8,
	Invalid = -1,
};

DEVILUTION_END_NAMESPACE

#endif /* __ITEM_ENUMS_H__ */
