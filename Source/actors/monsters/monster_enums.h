// HEADER_GOES_HERE
#ifndef __MONSTER_ENUMS_H__
#define __MONSTER_ENUMS_H__

DEVILUTION_BEGIN_NAMESPACE

enum class MonsterType {
	NZOMBIE = 0x0,
	BZOMBIE = 0x1,
	GZOMBIE = 0x2,
	YZOMBIE = 0x3,
	RFALLSP = 0x4,
	DFALLSP = 0x5,
	YFALLSP = 0x6,
	BFALLSP = 0x7,
	WSKELAX = 0x8,
	TSKELAX = 0x9,
	RSKELAX = 0xA,
	XSKELAX = 0xB,
	RFALLSD = 0xC,
	DFALLSD = 0xD,
	YFALLSD = 0xE,
	BFALLSD = 0xF,
	NSCAV = 0x10,
	BSCAV = 0x11,
	WSCAV = 0x12,
	YSCAV = 0x13,
	WSKELBW = 0x14,
	TSKELBW = 0x15,
	RSKELBW = 0x16,
	XSKELBW = 0x17,
	WSKELSD = 0x18,
	TSKELSD = 0x19,
	RSKELSD = 0x1A,
	XSKELSD = 0x1B,
	INVILORD = 0x1C,
	SNEAK = 0x1D,
	STALKER = 0x1E,
	UNSEEN = 0x1F,
	ILLWEAV = 0x20,
	LRDSAYTR = 0x21,
	NGOATMC = 0x22,
	BGOATMC = 0x23,
	RGOATMC = 0x24,
	GGOATMC = 0x25,
	FIEND = 0x26,
	BLINK = 0x27,
	GLOOM = 0x28,
	FAMILIAR = 0x29,
	NGOATBW = 0x2A,
	BGOATBW = 0x2B,
	RGOATBW = 0x2C,
	GGOATBW = 0x2D,
	NACID = 0x2E,
	RACID = 0x2F,
	BACID = 0x30,
	XACID = 0x31,
	SKING = 0x32,
	CLEAVER = 0x33,
	FAT = 0x34,
	MUDMAN = 0x35,
	TOAD = 0x36,
	FLAYED = 0x37,
	WYRM = 0x38,
	CAVSLUG = 0x39,
	DVLWYRM = 0x3A,
	DEVOUR = 0x3B,
	NMAGMA = 0x3C,
	YMAGMA = 0x3D,
	BMAGMA = 0x3E,
	WMAGMA = 0x3F,
	HORNED = 0x40,
	MUDRUN = 0x41,
	FROSTC = 0x42,
	OBLORD = 0x43,
	BONEDMN = 0x44,
	REDDTH = 0x45,
	LTCHDMN = 0x46,
	UDEDBLRG = 0x47,
	INCIN = 0x48,
	FLAMLRD = 0x49,
	DOOMFIRE = 0x4A,
	HELLBURN = 0x4B,
	STORM = 0x4C,
	RSTORM = 0x4D,
	STORML = 0x4E,
	MAEL = 0x4F,
	BIGFALL = 0x50,
	WINGED = 0x51,
	GARGOYLE = 0x52,
	BLOODCLW = 0x53,
	DEATHW = 0x54,
	MEGA = 0x55,
	GUARD = 0x56,
	VTEXLRD = 0x57,
	BALROG = 0x58,
	NSNAKE = 0x59,
	RSNAKE = 0x5A,
	BSNAKE = 0x5B,
	GSNAKE = 0x5C,
	NBLACK = 0x5D,
	RTBLACK = 0x5E,
	BTBLACK = 0x5F,
	RBLACK = 0x60,
	UNRAV = 0x61,
	HOLOWONE = 0x62,
	PAINMSTR = 0x63,
	REALWEAV = 0x64,
	SUCCUBUS = 0x65,
	SNOWWICH = 0x66,
	HLSPWN = 0x67,
	SOLBRNR = 0x68,
	COUNSLR = 0x69,
	MAGISTR = 0x6A,
	CABALIST = 0x6B,
	ADVOCATE = 0x6C,
	GOLEM = 0x6D,
	DIABLO = 0x6E,
	DARKMAGE = 0x6F,
	NUM_MTYPES = 0x6F,  /// BUGFIX the count is off by one
};

// this enum contains indexes from UniqMonst array for special unique monsters
// (usually quest related)
// matches position in UniqMonst data table
enum class UniqueMonsterType {
	GARBUD = 0,
	SKELKING = 1,
	ZHAR = 2,
	SNOTSPIL = 3,
	LAZURUS = 4,
	RED_VEX = 5,
	BLACKJADE = 6,
	LACHDAN = 7,
	WARLORD = 8,
	BUTCHER = 9,
};

using MonsterFlags = int;

//enum class MonsterFlag {
namespace MonsterFlag
{
	constexpr int hidden = 0x01;
	constexpr int lock_animation = 0x02;
    constexpr int allow_special = 0x04;
    constexpr int noheal = 0x08;
    constexpr int targets_monster = 0x10;
    constexpr int golem = 0x20;
    constexpr int quest_complete = 0x40;
    constexpr int knockback = 0x80;
    constexpr int search = 0x100;
    constexpr int can_open_door = 0x200;
    constexpr int no_enemy = 0x400;
	constexpr int unused = 0x800;
    constexpr int nolifesteal = 0x1000;
};

enum class MonsterGoal {
	None = 0,
	NORMAL = 1,
	RETREAT = 2,
	HEALING = 3,
	MOVE = 4,
	SHOOT = 5,
	INQUIRING = 6,
	TALKING = 7,
};

enum class MonsterResist {
	resist_magic = 0x01,
	resist_fire = 0x02,
	resist_lightning = 0x04,
	imune_magic = 0x08,
	imune_fire = 0x10,
	imune_lightning = 0x20,
	null_40 = 0x40,
	imune_acid = 0x80,
};

enum class MonsterMode {
	STAND = 0,
	WALK = 1,
	WALK2 = 2,
	WALK3 = 3,
	ATTACK = 4,
	GOTHIT = 5,
	DEATH = 6,
	SATTACK = 7,
	FADEIN = 8,
	FADEOUT = 9,
	RATTACK = 10,
	SPSTAND = 11,
	RSPATTACK = 12,
	DELAY = 13,
	CHARGE = 14,
	STONE = 15,
	HEAL = 16,
	TALK = 17,
};

enum class MonstAnim {
	STAND = 0,
	WALK = 1,
	ATTACK = 2,
	GOTHIT = 3,
	DEATH = 4,
	SPECIAL = 5,
};



enum class MonstAi {
	ZOMBIE = 0,
	FAT = 1,
	SKELSD = 2,
	SKELBOW = 3,
	SCAV = 4,
	RHINO = 5,
	GOATMC = 6,
	GOATBOW = 7,
	FALLEN = 8,
	MAGMA = 9,
	SKELKING = 10,
	BAT = 11,
	GARG = 12,
	CLEAVER = 13,
	SUCC = 14,
	SNEAK = 15,
	STORM = 16,
	FIREMAN = 17,
	GARBUD = 18,
	ACID = 19,
	ACIDUNIQ = 20,
	GOLUM = 21,
	ZHAR = 22,
	SNOTSPIL = 23,
	SNAKE = 24,
	COUNSLR = 25,
	MEGA = 26,
	DIABLO = 27,
	LAZURUS = 28,
	LAZHELP = 29,
	LACHDAN = 30,
	WARLORD = 31,
};

enum class MonstClassId {
	undead = 0,
	demon = 1,
	animal = 2,
};



DEVILUTION_END_NAMESPACE

#endif /* __MONSTER_ENUMS_H__ */
