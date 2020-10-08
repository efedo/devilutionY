// HEADER_GOES_HERE
#ifndef __PLAYER_ENUMS_H__
#define __PLAYER_ENUMS_H__

namespace dvl {

enum class AnimWeaponId {
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

enum class AnimArmorId {
	light = 0x00,
	medium = 0x10,
	heavy = 0x20
};

enum class PlayerClass {
	warrior = 0x0,
	rogue = 0x1,
	sorceror = 0x2
};

enum class UiClass {
	warrior = 0x0,
	rogue = 0x1,
	sorceror = 0x2,
};

enum class DestinationAction {
	WALK = -2,  // Automatic walk when using gamepad
	NONE = -1,
	ATTACK = 9,
	RATTACK = 10,
	SPELL = 12,
	OPERATE = 13,
	DISARM = 14,
	PICKUPITEM = 15,   // put item in hand (inventory screen open)
	PICKUPAITEM = 16,  // put item in inventory
	TALK = 17,
	OPERATETK = 18,  // operate via telekinesis
	ATTACKMON = 20,
	ATTACKPLR = 21,
	RATTACKMON = 22,
	RATTACKPLR = 23,
	SPELLMON = 24,
	SPELLPLR = 25,
	SPELLWALL = 26,
};

enum class PlayerWeaponType {
	melee = 0,
	ranged = 1,
};

using PlayerGraphicFileFlags = int;

namespace PlayerGraphicFileFlag {
	constexpr int STAND = 1 << 0;
	constexpr int WALK = 1 << 1;
	constexpr int ATTACK = 1 << 2;
	constexpr int HIT = 1 << 3;
	constexpr int LIGHTNING = 1 << 4;
	constexpr int FIRE = 1 << 5;
	constexpr int MAGIC = 1 << 6;
	constexpr int DEATH = 1 << 7;
	constexpr int BLOCK = 1 << 8;
	// everything except DEATH
	// 0b1_0111_1111
	constexpr int NONDEATH = 0x17F;
};

enum class PlayerMode {
	STAND = 0,
	WALK = 1,
	WALK2 = 2,
	WALK3 = 3,
	ATTACK = 4,
	RATTACK = 5,
	BLOCK = 6,
	GOTHIT = 7,
	DEATH = 8,
	SPELL = 9,
	NEWLVL = 10,
	QUIT = 11,
};

enum class AttributeId {
	STR = 0,
	MAG = 1,
	DEX = 2,
	VIT = 3,
};

enum class RSpellType {
	SKILL = 0x0,
	SPELL = 0x1,
	SCROLL = 0x2,
	CHARGES = 0x3,
	INVALID = 0x4,
};

}

#endif /* __PLAYER_ENUMS_H__ */
