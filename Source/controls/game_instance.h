/**
 * @file control.h
 *
 * Interface of the character and main control panels
 */
#ifndef __GAME_INSTANCE_H__
#define __GAME_INSTANCE_H__

namespace dvl {

class GameInstance
{
public:
	void setMaxPlayers(uint8_t num);
	bool isMultiplayer();
	bool isSingleplayer();
	uint8_t maxPlayers();

	void DoEnding();
	void PrepDoEnding();
private:
	size_t _maxPlayers = 0;
};

extern GameInstance game;

}

#endif __GAME_INSTANCE_H__
