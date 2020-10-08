/**
 * @file cursor.cpp
 *
 * Implementation of cursor tracking functionality.
 */
#include "all.h"

namespace dvl {

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


void GameInstance::DoEnding()
{
	if (game.isMultiplayer()) SNetLeaveGame(0x40000004);

	music_stop();

	if (game.isMultiplayer()) SDL_Delay(1000);

	if (myplr().data._pClass == PlayerClass::warrior) {
		play_movie("gendata\\DiabVic2.smk", false);
	}
	else if (myplr().data._pClass == PlayerClass::sorceror) {
		play_movie("gendata\\DiabVic1.smk", false);
	}
	else {
		play_movie("gendata\\DiabVic3.smk", false);
	}
	play_movie("gendata\\Diabend.smk", false);

	bool bMusicOn = gbMusicOn;
	gbMusicOn = true;

	int musicVolume = sound_get_or_set_music_volume(1);
	sound_get_or_set_music_volume(0);

	music_start(TMUSIC_L2);
	loop_movie = true;
	play_movie("gendata\\loopdend.smk", true);
	loop_movie = false;
	music_stop();

	sound_get_or_set_music_volume(musicVolume);
	gbMusicOn = bMusicOn;
}

void GameInstance::PrepDoEnding()
{
	int newKillLevel, i;
	DWORD* killLevel;

	gbSoundOn = sgbSaveSoundOn;
	gbRunGame = false;
	deathflag = false;
	cineflag = true;

	killLevel = &myplr().data.pDiabloKillLevel;
	newKillLevel = gnDifficulty + 1;
	if (*killLevel > newKillLevel)
		newKillLevel = *killLevel;
	myplr().data.pDiabloKillLevel = newKillLevel;

	for (i = 0; i < MAX_PLRS; i++) {
		plr[i].data._pmode = PlayerMode::QUIT;
		plr[i].data._pInvincible = true;
	}
}
}
