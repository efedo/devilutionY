// HEADER_GOES_HERE
#ifndef __PLAYER_ENUMS_H__
#define __PLAYER_ENUMS_H__

DEVILUTION_BEGIN_NAMESPACE

typedef enum AnimWeaponId {
	unarmed = 0x00,
	unarmed_shield = 0x01,
	sword = 0x02,
	sword_shield = 0x03,
	bow = 0x04,
	axe = 0x05,
	mace = 0x06,
	mace_shield = 0x07,
	staff = 0x08
};

typedef enum AnimArmorId {
	light = 0x00,
	medium = 0x10,
	heavy = 0x20
};

DEVILUTION_END_NAMESPACE

#endif /* __PLAYER_ENUMS_H__ */
