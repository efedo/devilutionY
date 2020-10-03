/**
 * @file monster_manager.h
 *
 * Monster manager class (manages monster instances, not types)
 */
#ifndef __ACTOR_MANAGER_H__
#define __ACTOR_MANAGER_H__

#include <vector>
#include <set>

DEVILUTION_BEGIN_NAMESPACE

class Actor;
class Player;
class Monster;
class Npc;
class ActorId;

class ActorManager {
public:
	~ActorManager();
	void clearAll();

	Actor *getActor(ActorId &);
	Player *getPlayer(ActorId &);
	Monster *getMonster(ActorId &);
	Npc *getNpc(ActorId &);

	void _getFirstEmpty();

	// Monster functions
	void PlaceMonster(ActorId id, int mtype, V2Di pos);
	void PlaceUniqueMonster(UniqueMonsterType uniqindex, int miniontype,
	                     int unpackfilesize);
	void PlaceQuestMonsters();
	void PlaceGroup(int mtype, int num, int leaderf, int leader);
	void clearAllMonsters();

	// Player functions
	void setLocal(ActorId newlocal);
	bool isValidPlayer(ActorId);
	void SetPlayerGPtrs(uint8_t *pData, uint8_t **pAnim);
	void AddPlrMonstExper(int lvl, int exp,
	                      char pmask);  // Distributes to party
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
	ActorId myplr;
};

extern ActorManager actors;

DEVILUTION_END_NAMESPACE

#endif // __ACTOR_MANAGER_H__
