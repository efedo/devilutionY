/**
 * @file cursor.cpp
 *
 * Implementation of cursor tracking functionality.
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

GameInstance game;

void GameInstance::setMaxPlayers(uint8_t num)
{
	_maxPlayers = num;
}

bool GameInstance::isMultiplayer()
{
	return (_maxPlayers > 1);
}

bool GameInstance::isSingleplayer()
{
	return (_maxPlayers <= 1);
}

uint8_t GameInstance::maxPlayers()
{
	return _maxPlayers;
}

DEVILUTION_END_NAMESPACE
