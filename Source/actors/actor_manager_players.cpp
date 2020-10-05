#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

void ActorManager::setLocal(ActorId newlocal)
{
	_myplr = newlocal;
}

bool ActorManager::isValidPlayer(ActorId id)
{
	if (id >= _actors.size()) return false;
	if (!_actors[id].get()) return false;
	if (_actors[id].get()->type != ActorType::player) return false;
	return true;
}

Player & ActorManager::_localPlr()
{
	try {
		return *static_cast<Player *>(_actors[_myplr].get());
	} catch (...) {
		std::cerr << "Could not obtain myplr";
	}
}

void ActorManager::SetPlayerGPtrs(uint8_t *pData, uint8_t **pAnim)
{
	for (int i = 0; i < 8; i++) { pAnim[i] = CelGetFrameStart(pData, i); }
}


void ActorManager::AddPlrMonstExper(int lvl, int exp, char pmask)
{
	int totplrs, i, e;

	totplrs = 0;
	for (i = 0; i < MAX_PLRS; i++) {
		if ((1 << i) & pmask) { totplrs++; }
	}

	if (totplrs) {
		e = exp / totplrs;
		if (pmask & (1 << myplr().id())) myplr().AddPlrExperience(lvl, e);
	}
}

Player* ActorManager::getRandomPlayer() {

}


DEVILUTION_END_NAMESPACE
