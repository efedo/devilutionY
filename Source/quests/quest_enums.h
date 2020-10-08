//HEADER_GOES_HERE
#ifndef __QUEST_ENUMS_H__
#define __QUEST_ENUMS_H__

namespace dvl {

enum class QuestId {
	rock = 0x0,
	mushroom = 0x1,
	garbud = 0x2,
	zhar = 0x3,
	veil = 0x4,
	diablo = 0x5,
	butcher = 0x6,
	ltbanner = 0x7,
	blind = 0x8,
	blood = 0x9,
	anvil = 0xA,
	warlord = 0xB,
	skelking = 0xC,
	pwater = 0xD,
	schamb = 0xE,
	betrayer = 0xF,
	invalid = -1,
};

enum class QuestState {
	not_available = 0,
	init = 1,
	active = 2,
	done = 3
};

// Eliminate
enum class QuestMushState {
	init = 0,
	tomespawned = 1,
	tomegiven = 2,
	mushspawned = 3,
	mushpicked = 4,
	mushgiven = 5,
	brainspawned = 6,
	braingiven = 7,
};

}

#endif // __QUEST_ENUMS_H__
