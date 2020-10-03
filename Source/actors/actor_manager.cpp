/**
 * @file monster_manager.cpp
 *
 * Monster manager class (manages monster instances, not types)
 */
#include "all.h"


DEVILUTION_BEGIN_NAMESPACE

ActorManager::~ActorManager()
{
	clearAll();
}

void ActorManager::clearAll()
{
	_actors.clear();
	_players.clear();
	_monsters.clear();
	_npcs.clear();
}

Actor *ActorManager::getActor(ActorId & id)
{
	if (id < _actors.size()) return _actors[id].get();
	return 0;
}

Player *ActorManager::getPlayer(ActorId & id)
{
	if (id < _actors.size() && _actors[id].get()->type == ActorType::player) {
		return static_cast<Player *>(_actors[id].get());
	}
	return 0;
}

Monster *ActorManager::getMonster(ActorId &id)
{
	if (id < _actors.size() && _actors.at(id).get()->type == ActorType::monster) {
		return static_cast<Monster *>(_actors[id].get());
	}
	return 0;
}

Npc *ActorManager::getNpc(ActorId &id)
{
	if (id < _actors.size() && _actors.at(id).get()->type == ActorType::npc) {
		return static_cast<Npc *>(_actors[id].get());
	}
	return 0;
}

// Sets _firstempty to the first empty spot
void ActorManager::_getFirstEmpty()
{
	const size_t actors_size = _actors.size();
	if (_firstempty == std::numeric_limits<ActorId>::max()) app_fatal("Too many actors");  // too many actors
	assert(_firstempty < actors_size);
	for (size_t i = _firstempty; i != actors_size; ++i) {
		if (_actors[_firstempty].get() == 0) { return; }
	}
	_actors.push_back(std::unique_ptr<Actor>());
	_firstempty = _actors.size();
}

ActorId ActorManager::_registerActor(Actor * newptr)
{
	_getFirstEmpty();
	assert(!_actors[_firstempty]);
	assert(newptr);
	_actors[_firstempty].reset(newptr);

	if (newptr->type == ActorType::player) {
		assert(_players.size() <= _maxPlayers);
		_players.insert(static_cast<Player*>(newptr));
	} else if (newptr->type == ActorType::monster) {
		_monsters.insert(static_cast<Monster *>(newptr));
	} else if (newptr->type == ActorType::npc) {
		_npcs.insert(static_cast<Npc *>(newptr));
	}
	ActorId tmpid;
	tmpid.id = _firstempty;
	return tmpid;
}

void ActorManager::_unregisterActor(ActorId id)
{
	assert(id < _actors.size());
	Actor * actor = _actors[id].get();
	assert(actor);
	if (actor->type == ActorType::player) {
		if (!_players.count(static_cast<Player *>(actor))) return;
		_players.erase(static_cast<Player *>(actor));
	} else if (actor->type == ActorType::monster) {
		if (!_monsters.count(static_cast<Monster *>(actor))) return;
		_monsters.erase(static_cast<Monster *>(actor));
	} else if (actor->type == ActorType::npc) {
		if (!_npcs.count(static_cast<Npc *>(actor))) return;
		_npcs.erase(static_cast<Npc *>(actor));
	}
	_actors[id].reset();
	if (id < _firstempty) _firstempty = id;
}

DEVILUTION_END_NAMESPACE
