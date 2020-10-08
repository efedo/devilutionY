/**
 * @file monster_manager.cpp
 *
 * Monster manager class (manages monster instances, not types)
 */
#include "all.h"


namespace dvl {

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

bool ActorManager::isActor(ActorId id) const
{
	const size_t ind = size_t(id);
	if (ind < _actors.size() && _actors[ind].get()) return true;
	return false;
}

bool ActorManager::isPlayer(ActorId id) const
{
	const size_t ind = size_t(id);
	if (ind < _actors.size() && _actors[ind].get()->type == ActorType::player) return true;
	return false;
}

bool ActorManager::isMonster(ActorId id) const
{
	const size_t ind = size_t(id);
	if (ind < _actors.size() && _actors[ind].get()->type == ActorType::monster) return true;
	return false;
}

bool ActorManager::isNpc(ActorId id) const
{
	const size_t ind = size_t(id);
	if (ind < _actors.size() && _actors[ind].get()->type == ActorType::npc) return true;
	return false;
}

Actor & ActorManager::getActor(ActorId id) const
{
	assert(isActor(id));
	return *_actors[size_t(id)].get();
}

Player & ActorManager::getPlayer(ActorId id) const
{
	assert(isPlayer(id));
	return *static_cast<Player*>(_actors[size_t(id)].get());
}

Monster & ActorManager::getMonster(ActorId id) const
{
	assert(isMonster(id));
	return *static_cast<Monster*>(_actors[size_t(id)].get());
}

Npc & ActorManager::getNpc(ActorId id) const
{
	assert(isPlayer(id));
	return *static_cast<Npc*>(_actors[size_t(id)].get());
}

// Sets _firstempty to the first empty spot
void ActorManager::_getFirstEmpty()
{
	const size_t actors_size = _actors.size();
	if (_firstempty == std::numeric_limits<ActorId::BaseT>::max()) app_fatal("Too many actors");  // too many actors
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
		assert(_players.size() <= game.maxPlayers());
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
	const size_t ind = size_t(id);
	assert(ind < _actors.size());
	Actor * actor = _actors[ind].get();
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
	_actors[ind].reset();
	if (ind < _firstempty) _firstempty = ind;
}

}
