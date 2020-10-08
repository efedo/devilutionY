/**
 * @file monster_manager.h
 *
 * Monster manager class (manages monster instances, not types)
 */
#ifndef __ACTOR_MANAGER_H__
#define __ACTOR_MANAGER_H__

#include <vector>
#include <set>

namespace dvl {

class Actor;
class Player;
class Monster;
class Npc;
class ActorId;

class ActorManager {
public:
	~ActorManager();
	void clearAll();
	void clearAllPlayers();
	void clearAllMonsters();

	bool isActor(ActorId) const;
	bool isPlayer(ActorId) const;
	bool isMonster(ActorId) const;
	bool isNpc(ActorId) const;

	Actor & getActor(ActorId) const;
	Player & getPlayer(ActorId) const;
	Monster & getMonster(ActorId) const;
	Npc & getNpc(ActorId) const;

	void _getFirstEmpty();

	// Monster functions
	Monster & PlaceMonster(MonsterType mtype, V2Di pos);
	Monster & AddMonster(MonsterType mtype, V2Di pos, Dir dir, bool InMap);
	Monster & PlaceUniqueMonster(UniqueMonsterType uniqindex, MonsterType miniontype,
	                     int unpackfilesize);
	void PlaceQuestMonsters();
	void PlaceGroup(MonsterType mtype, int num, int leaderf, Monster & leader);

	Monster * M_SpawnSkel(V2Di pos, Dir dir);
	bool SpawnSkeleton(int ii, V2Di pos);
	Monster * PreSpawnSkeleton();
	void DeleteMonsterList();
	void ProcessMonsters();

	// Player functions
	void setLocal(ActorId newlocal);
	bool isValidPlayer(ActorId);
	void SetPlayerGPtrs(uint8_t *pData, uint8_t **pAnim);
	void AddPlrMonstExper(int lvl, int exp,
	                      char pmask);  // Distributes to party
	Player* getRandomPlayer();

	auto& getPlayerList() { return _players; };
	auto& getMonsterList() { return _monsters; };
	auto& getNpcList() { return _npcs; };

	int numPlayers() { return _players.size(); };
	int numMonsters() { return _monsters.size(); };
	int numNpcs() { return _npcs.size(); };

   private:
	friend class Actor;
	ActorId _registerActor(Actor *);
	void _unregisterActor(ActorId);

	std::vector<std::unique_ptr<Actor>> _actors;  // _owns_ actors
	size_t _firstempty = std::numeric_limits<size_t>::max();
	std::set<Player *> _players; // player x-ref (for loops);
	std::set<Monster *> _monsters; // monster x-ref (for loops);
	std::set<Npc *> _npcs;  // monster x-ref (for loops);

	// player specific
	friend Player &myplr();
	Player &_localPlr();
	ActorId _myplr;
};

extern ActorManager actors;

}

#endif // __ACTOR_MANAGER_H__
