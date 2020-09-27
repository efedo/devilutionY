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
	PlayerManager()
	{
	}

	Player &operator[](uint8_t p)
	{
		if (p >= MAX_PLRS || p >= list.size() || !list[p]) {
			app_fatal("Illegal player %d", p);
		}
		return *list[p];
	}

	void setMaxPlayers(uint8_t num)
	{
		list.resize(num);
		_maxPlayers = num;
	}

	bool isMultiplayer()
	{
		return (_maxPlayers > 1);
	}

	bool isSingleplayer()
	{
		return (_maxPlayers <= 1);
	}

	void addPlayer(uint8_t num)
	{
		assert(num < _maxPlayers);
		assert(!list[num]); // Player already exists
		list[num] = std::make_unique<Player>(num);
	}

	void removePlayer(uint8_t num)
	{
		assert(num < _maxPlayers);
		assert(list[num]); // Player does not exist
		list[num].reset();
	}

	uint8_t maxPlayers()
	{
		return list.size();
	}

	void setLocal(uint8_t newlocal)
	{
		myplr = newlocal;
	}

	bool isValidPlayer(int num)
	{
		if (num < 0)
			return false;
		if (num >= list.size())
			return false;
		if (!list[num])
			return false;
		return true;
	}

private:
	friend Player &myplr();

	auto &_local()
	{
		return operator[](myplr);
	}

	std::vector<std::unique_ptr<Player>> list;
	int myplr;
	size_t _maxPlayers = 0;
};

extern PlayerManager plr;

DEVILUTION_END_NAMESPACE

#endif /* __PLAYER_MANAGER_H__ */
