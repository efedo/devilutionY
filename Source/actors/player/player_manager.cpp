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

void PlayerManager::SetPlayerGPtrs(uint8_t *pData, uint8_t **pAnim)
{
	for (int i = 0; i < 8; i++) { pAnim[i] = CelGetFrameStart(pData, i); }
}

DWORD PlayerManager::GetPlrGFXSize(char *szCel)
{
	int c;
	const char *a, *w;
	DWORD dwSize, dwMaxSize;
	HANDLE hsFile;
	char pszName[256];
	char Type[16];

	dwMaxSize = 0;

	for (c = 0; c < classes.numclasses; c++) {
		// for (c = 0; c < sizeof(ClassStrTbl) / sizeof(*ClassStrTbl); c++) {
		for (a = &ArmourChar[0]; *a; a++) {
			for (w = &WepChar[0]; *w; w++) {
				if (szCel[0] == 'D' && szCel[1] == 'T' && *w != 'N') {
					continue;  // Death has no weapon
				}
				if (szCel[0] == 'B' && szCel[1] == 'L' &&
				    (*w != 'U' && *w != 'D' && *w != 'H')) {
					continue;  // No block without weapon
				}
				sprintf(Type, "%c%c%c", CharChar[c], *a, *w);
				sprintf(pszName, "PlrGFX\\%s\\%s\\%s%s.CL2",
				        classes[c].name.c_str(), Type, Type, szCel);
				if (WOpenFile(pszName, &hsFile, true)) {
					/// ASSERT: assert(hsFile);
					dwSize = WGetFileSize(hsFile, NULL, pszName);
					WCloseFile(hsFile);
					if (dwMaxSize <= dwSize) { dwMaxSize = dwSize; }
				}
			}
		}
	}

	return dwMaxSize;
}

void PlayerManager::AddPlrMonstExper(int lvl, int exp, char pmask)
{
	int totplrs, i, e;

	totplrs = 0;
	for (i = 0; i < MAX_PLRS; i++) {
		if ((1 << i) & pmask) { totplrs++; }
	}

	if (totplrs) {
		e = exp / totplrs;
		if (pmask & (1 << myplr())) myplr().AddPlrExperience(lvl, e);
	}
}


DEVILUTION_END_NAMESPACE
