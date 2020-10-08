/**
 * @file monster_type.h
 *
 * Monster type class
 */
#ifndef __MONSTER_TYPE_H__
#define __MONSTER_TYPE_H__

namespace dvl {

class MonsterClass {
public:
	MonsterClass(int new_monst = 0)
	{}
	int kills = 0;
	void InitMonsterTRN(bool special);
	void InitMonsterGFX();
	CMonster data;
	CMonster vdata();
	const MonsterData cdata();
	char animLetter(MonstAnim);
	void InitMonsterSND();

	bool IsSkel();
	bool IsGoat();
	void PrintMonstHistory();
private:
	static const char animletter[7];
};

}

#endif // __MONSTER_TYPE_H__
