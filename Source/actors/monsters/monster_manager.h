/**
 * @file monster_manager.h
 *
 * Monster manager class (manages monster instances, not types)
 */
#ifndef __MONSTER_MANAGER_H__
#define __MONSTER_MANAGER_H__

#include "monster_instance.h"

DEVILUTION_BEGIN_NAMESPACE

class MonsterManager {
public:
	//MonsterManager();
	MonsterInstance &operator[](size_t n);
	void PlaceMonster(int i, int mtype, V2Di pos);
	void PlaceUniqueMonster(int uniqindex, int miniontype, int unpackfilesize);
	void PlaceQuestMonsters();
	void PlaceGroup(int mtype, int num, int leaderf, int leader);
	void DeleteMonster(int i);
	void ClrAllMonsters();

private:
	MonsterInstance list[MAXMONSTERS];
};

extern MonsterManager monsters;

DEVILUTION_END_NAMESPACE

#endif // __MONSTER_MANAGER_H__
