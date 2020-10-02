//HEADER_GOES_HERE
#ifndef __PLAYER_MANAGER_H__
#define __PLAYER_MANAGER_H__

#include <string>
#include <array>
#include <vector>

DEVILUTION_BEGIN_NAMESPACE

class PlayerManager
{
public:
	PlayerManager();
	Player &operator[](uint8_t p);
	void setMaxPlayers(uint8_t num);
	bool isMultiplayer();
	bool isSingleplayer();
	void addPlayer(uint8_t num);
	void removePlayer(uint8_t num);
	uint8_t maxPlayers();
	void setLocal(uint8_t newlocal);
	bool isValidPlayer(int num);
	void SetPlayerGPtrs(uint8_t *pData, uint8_t **pAnim);
	DWORD GetPlrGFXSize(char *szCel);
	void AddPlrMonstExper(int lvl, int exp,
	                      char pmask);  // Distributes to party

   private:
	friend Player &myplr();
	Player &_local();
	std::vector<std::unique_ptr<Player>> list;
	int myplr;
	size_t _maxPlayers = 0;
};


extern PlayerManager plr;

DEVILUTION_END_NAMESPACE

#endif /* __PLAYER_MANAGER_H__ */
