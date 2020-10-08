/**
 * @file mainmenu.cpp
 *
 * Implementation of functions for interacting with the main menu.
 */
#include "all.h"
#include "../3rdParty/Storm/Source/storm.h"
#include "../DiabloUI/diabloui.h"

namespace dvl {

char gszHero[16];

/* data */

/** The active music track id for the main menu. */
int menu_music_track_id = TMUSIC_INTRO;

void mainmenu_refresh_music()
{
	music_start(menu_music_track_id);
	do {
		menu_music_track_id++;
		if (menu_music_track_id == NUM_MUSIC)
			menu_music_track_id = TMUSIC_TOWN;
	} while (menu_music_track_id == TMUSIC_TOWN || menu_music_track_id == TMUSIC_L1);
}

void mainmenu_change_name(int arg1, int arg2, int arg3, int arg4, char *name_1, char *name_2)
{
	if (UiValidPlayerName(name_2))
		pfile_rename_hero(name_1, name_2);
}

bool mainmenu_select_hero_dialog(
    const _SNETPROGRAMDATA *client_info,
    const _SNETPLAYERDATA *user_info,
    const _SNETUIDATA *ui_info,
    const _SNETVERSIONDATA *fileinfo,
    DWORD mode,
    char *cname, DWORD clen,
    char *cdesc, DWORD cdlen,
    bool *multi)
{
	bool hero_is_created = true;
	int dlgresult = 0;
	if (game.isSingleplayer()) {
		if (!UiSelHeroSingDialog(
		        pfile_ui_set_hero_infos,
		        pfile_ui_save_create,
		        pfile_delete_save,
		        pfile_ui_set_class_stats,
		        &dlgresult,
		        gszHero,
		        &gnDifficulty))
			app_fatal("Unable to display SelHeroSing");

		if (dlgresult == SELHERO_CONTINUE)
			gbLoadGame = true;
		else
			gbLoadGame = false;

	} else if (!UiSelHeroMultDialog(
	               pfile_ui_set_hero_infos,
	               pfile_ui_save_create,
	               pfile_delete_save,
	               pfile_ui_set_class_stats,
	               &dlgresult,
	               &hero_is_created,
	               gszHero)) {
		app_fatal("Can't load multiplayer dialog");
	}
	if (dlgresult == SELHERO_PREVIOUS) {
		SErrSetLastError(1223);
		return false;
	}

	pfile_create_player_description(cdesc, cdlen);
	if (multi) {
		if (mode == 'BNET')
			*multi = hero_is_created || !myplr().data.pBattleNet;
		else
			*multi = hero_is_created;
	}
	if (cname && clen)
		SStrCopy(cname, gszHero, clen);

	return true;
}

void mainmenu_loop()
{
	bool done;
	int menu;

	done = false;
	mainmenu_refresh_music();

	do {
		menu = 0;
		if (!UiMainMenuDialog(gszProductName, &menu, effects_play_sound, 30))
			app_fatal("Unable to display mainmenu");

		switch (menu) {
		case MAINMENU_SINGLE_PLAYER:
			if (!mainmenu_single_player())
				done = true;
			break;
		case MAINMENU_MULTIPLAYER:
			if (!mainmenu_multi_player())
				done = true;
			break;
		case MAINMENU_REPLAY_INTRO:
		case MAINMENU_ATTRACT_MODE:
			if (gbActive)
				mainmenu_play_intro();
			break;
		case MAINMENU_SHOW_CREDITS:
			UiCreditsDialog(16);
			break;
		case MAINMENU_EXIT_DIABLO:
			done = true;
			break;
		}
	} while (!done);

	music_stop();
}

bool mainmenu_single_player()
{
	plr.setMaxPlayers(1);
	return mainmenu_init_menu(SELHERO_NEW_DUNGEON);
}

bool mainmenu_init_menu(int type)
{
	bool success;

	if (type == SELHERO_PREVIOUS)
		return true;

	music_stop();

	success = StartGame(type != SELHERO_CONTINUE, type != SELHERO_CONNECT);
	if (success)
		mainmenu_refresh_music();

	return success;
}

bool mainmenu_multi_player()
{
	plr.setMaxPlayers(MAX_PLRS);
	return mainmenu_init_menu(SELHERO_CONNECT);
}

void mainmenu_play_intro()
{
	music_stop();
	play_movie("gendata\\diablo1.smk", true);
	mainmenu_refresh_music();
}

}
