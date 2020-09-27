/**
 * @file monster_beastiary.h
 *
 * Monster beastiary class (manages monster types)
 */
#ifndef __MONSTER_BEASTIARY_H__
#define __MONSTER_BEASTIARY_H__

DEVILUTION_BEGIN_NAMESPACE

class Beastiary {
public:
	Beastiary();
	MonsterType &operator[](size_t n);
	void InitLevelMonsters();
	int AddMonsterType(int type, int placeflag);
	void GetLevelMTypes();
	BYTE GraphicTable[NUMLEVELS][MAX_LVLMTYPES]; // Stores all monster graphics
private:
	MonsterType types[MAX_LVLMTYPES];
};

extern Beastiary beastiary;

DEVILUTION_END_NAMESPACE

#endif // __MONSTER_BEASTIARY_H__
