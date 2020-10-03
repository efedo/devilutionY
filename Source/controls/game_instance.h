/**
 * @file control.h
 *
 * Interface of the character and main control panels
 */
#ifndef __GAME_INSTANCE_H__
#define __GAME_INSTANCE_H__

DEVILUTION_BEGIN_NAMESPACE

class GameInstance
{
   public:
	void setMaxPlayers(uint8_t num);
	bool isMultiplayer();
	bool isSingleplayer();
	uint8_t maxPlayers();
   private:
	size_t _maxPlayers = 0;
};

extern GameInstance game;

DEVILUTION_END_NAMESPACE

#endif __GAME_INSTANCE_H__
