/**
 * @file monster_type.h
 *
 * Monster type class
 */
#ifndef __MONSTER_TYPE_H__
#define __MONSTER_TYPE_H__

DEVILUTION_BEGIN_NAMESPACE

class MonsterType {
public:
	MonsterType(int new_monst = 0)
	{
	}
	void InitMonsterTRN(BOOL special);
	void InitMonsterGFX();
	CMonster data;
};

DEVILUTION_END_NAMESPACE

#endif // __MONSTER_TYPE_H__
