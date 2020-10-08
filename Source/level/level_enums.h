// HEADER_GOES_HERE
#ifndef __LEVEL_ENUMS_H__
#define __LEVEL_ENUMS_H__

namespace dvl {

enum class SetLvl {
	None = 0,
	// SL_BUTCHCHAMB = 0x0,
	SkelKing = 0x1,
	BoneChamb = 0x2,
	Maze = 0x3,
	PoisonWater = 0x4,
	VileBetrayer = 0x5,
};

enum class DunType {
	town = 0x0,
	cathedral = 0x1,
	catacombs = 0x2,
	caves = 0x3,
	hell = 0x4,
	none = 0xFF,
};

enum class Shrine {
	MYSTERIOUS = 0,
	HIDDEN = 1,
	GLOOMY = 2,
	WEIRD = 3,
	MAGICAL = 4,
	STONE = 5,
	RELIGIOUS = 6,
	ENCHANTED = 7,
	THAUMATURGIC = 8,
	FASCINATING = 9,
	CRYPTIC = 10,
	MAGICAL2 = 11,
	ELDRITCH = 12,
	EERIE = 13,
	DIVINE = 14,
	HOLY = 15,
	SACRED = 16,
	SPIRITUAL = 17,
	SPOOKY = 18,
	ABANDONED = 19,
	CREEPY = 20,
	QUIET = 21,
	SECLUDED = 22,
	ORNATE = 23,
	GLIMMERING = 24,
	TAINTED = 25,
	NUM_SHRINETYPE
};

using DunTileFlags = int;
namespace DunTileFlag {
	constexpr int MISSILE = 0x01;
	constexpr int VISIBLE = 0x02;
	constexpr int DEAD_PLAYER = 0x04;
	constexpr int POPULATED = 0x08;
	constexpr int MONSTLR = 0x10;
	constexpr int PLAYERLR = 0x20;
	constexpr int LIT = 0x40;
	constexpr int EXPLORED = 0x80;
};

}

#endif /* __LEVEL_ENUMS_H__ */
