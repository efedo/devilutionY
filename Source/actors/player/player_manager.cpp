#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

PlayerManager plr;

PlayerManager::PlayerManager()
{
}

Player &PlayerManager::operator[](uint8_t p)
{
	if (p >= MAX_PLRS || p >= list.size() || !list[p]) {
		app_fatal("Illegal player %d", p);
	}
	return *list[p];
}

void PlayerManager::setMaxPlayers(uint8_t num)
{
	list.resize(num);
	_maxPlayers = num;
}

bool PlayerManager::isMultiplayer()
{
	return (_maxPlayers > 1);
}

bool PlayerManager::isSingleplayer()
{
	return (_maxPlayers <= 1);
}

void PlayerManager::addPlayer(uint8_t num)
{
	assert(num < _maxPlayers);
	assert(!list[num]); // Player already exists
	list[num] = std::make_unique<Player>(num);
}

void PlayerManager::removePlayer(uint8_t num)
{
	assert(num < _maxPlayers);
	assert(list[num]); // Player does not exist
	list[num].reset();
}

uint8_t PlayerManager::maxPlayers()
{
	return list.size();
}

void PlayerManager::setLocal(uint8_t newlocal)
{
	myplr = newlocal;
}

bool PlayerManager::isValidPlayer(int num)
{
	if (num < 0)
		return false;
	if (num >= list.size())
		return false;
	if (!list[num])
		return false;
	return true;
}

Player & PlayerManager::_local()
{
	return operator[](myplr);
}

DEVILUTION_END_NAMESPACE
