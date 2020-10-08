/**
 * @file monster_beastiary.h
 *
 * Monster beastiary class (manages monster types)
 */
#ifndef __MONSTER_BEASTIARY_H__
#define __MONSTER_BEASTIARY_H__

namespace dvl {

class MonsterClass;
class UniqMonstStruct;
class V2Di;
class MonsterData;
enum class Dir;
enum class MonsterType;
enum class UniqueMonsterType;

class Beastiary {
public:
	Beastiary();
	MonsterClass &operator[](size_t n);
	void InitLevelMonsters();
	void AddMonsterType(MonsterType type, int placeflag);
	void GetLevelMTypes();
	uint8_t GraphicTable[NUMLEVELS][MAX_LVLMTYPES]; // Stores all monster graphics
	MonsterClass &getClass(MonsterType);
	const UniqMonstStruct& getUnique(UniqueMonsterType);
	MonsterType convertDunFileMonType(uint8_t);
	MonsterType getRandomActiveSkelType(bool lvl_restrict = false);
	void FreeMonsterSnd(); // Clears all mosnter sounds


	void LoadDiabMonsts();
	void InitMonsters();
	void PlaceUniques();

	void SetMapMonsters(uint8_t* pMap, V2Di start);

	void FreeMonsters();
	int numTypes() { return types.size(); };

	int monstimgtot = 0; // number of monster imgs loaded
private:
	friend class MonsterClass;

	int DebugMonsters[10];

	std::array<MonsterClass, 112> types;

	static const MonsterType MonstConvTbl[]; //158
	static const MonsterData monsterdata[]; //112
	static const uint8_t MonstAvailTbl[]; //112
	static const UniqMonstStruct UniqMonst[]; //97
};

extern Beastiary beastiary;

}

#endif // __MONSTER_BEASTIARY_H__
