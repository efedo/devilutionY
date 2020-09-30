/**
 * @file gamemenu.cpp
 *
 * Implementation of the in-game menu functions.
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

/** Contains the game menu items of the single player menu. */
TMenuItem sgSingleMenu[] = {
	// clang-format off
	// dwFlags,      pszStr,         fnMenu
	{ GMENU_ENABLED, "Save Game",    &gamemenu_save_game  },
	{ GMENU_ENABLED, "Options",      &gamemenu_options    },
	{ GMENU_ENABLED, "New Game",     &gamemenu_new_game   },
	{ GMENU_ENABLED, "Load Game",    &gamemenu_load_game  },
	{ GMENU_ENABLED, "Quit Diablo",  &gamemenu_quit_game  },
	{ GMENU_ENABLED, NULL,           NULL }
	// clang-format on
};
/** Contains the game menu items of the multi player menu. */
TMenuItem sgMultiMenu[] = {
	// clang-format off
	// dwFlags,      pszStr,            fnMenu
	{ GMENU_ENABLED, "Options",         &gamemenu_options      },
	{ GMENU_ENABLED, "New Game",        &gamemenu_new_game     },
	{ GMENU_ENABLED, "Restart In Town", &gamemenu_restart_town },
	{ GMENU_ENABLED, "Quit Diablo",     &gamemenu_quit_game    },
	{ GMENU_ENABLED, NULL,              NULL                   },
	// clang-format on
};
TMenuItem sgOptionsMenu[] = {
	// clang-format off
	// dwFlags,                     pszStr,          fnMenu
	{ GMENU_ENABLED | GMENU_SLIDER, NULL,            &gamemenu_music_volume  },
	{ GMENU_ENABLED | GMENU_SLIDER, NULL,            &gamemenu_sound_volume  },
	{ GMENU_ENABLED | GMENU_SLIDER, "Gamma",         &gamemenu_gamma         },
	{ GMENU_ENABLED               , NULL,            &gamemenu_color_cycling },
	{ GMENU_ENABLED               , "Previous Menu", &gamemenu_previous      },
	{ GMENU_ENABLED               , NULL,            NULL                    },
	// clang-format on
};
/** Specifies the menu names for music enabled and disabled. */
char *music_toggle_names[] = {
	"Music",
	"Music Disabled",
};
/** Specifies the menu names for sound enabled and disabled. */
char *sound_toggle_names[] = {
	"Sound",
	"Sound Disabled",
};
/** Specifies the menu names for colour cycling disabled and enabled. */
char *color_cycling_toggle_names[] = { "Color Cycling Off", "Color Cycling On" };

void gamemenu_on()
{
	if (plr.isSingleplayer()) {
		gmenu_set_items(sgSingleMenu, gamemenu_update_single);
	} else {
		gmenu_set_items(sgMultiMenu, gamemenu_update_multi);
	}
	PressEscKey();
}

void gamemenu_update_single(TMenuItem *pMenuItems)
{
	bool enable;

	gmenu_enable(&sgSingleMenu[3], gbValidSaveFile);

	enable = false;
	if (myplr().data._pmode != PM_DEATH && !deathflag)
		enable = TRUE;

	gmenu_enable(&sgSingleMenu[0], enable);
}

void gamemenu_update_multi(TMenuItem *pMenuItems)
{
	gmenu_enable(&sgMultiMenu[2], deathflag);
}

void gamemenu_off()
{
	gmenu_set_items(NULL, NULL);
}

void gamemenu_handle_previous()
{
	if (gmenu_is_active())
		gamemenu_off();
	else
		gamemenu_on();
}

void gamemenu_previous(bool bActivate)
{
	gamemenu_on();
}

void gamemenu_new_game(bool bActivate)
{
	int i;

	for (i = 0; i < plr.maxPlayers(); i++) {
		if (plr.isValidPlayer(i)) {
			plr[i].data._pmode = PM_QUIT;
			plr[i].data._pInvincible = TRUE;
		}
	}

	deathflag = false;
	force_redraw = 255;
	scrollrt_draw_game_screen(TRUE);
	gbRunGame = false;
	gamemenu_off();
}

void gamemenu_quit_game(bool bActivate)
{
	gamemenu_new_game(bActivate);
	gbRunGameResult = false;
}

void gamemenu_load_game(bool bActivate)
{
	WNDPROC saveProc = SetWindowProc(DisableInputWndProc);
	gamemenu_off();
	SetCursor_(CURSOR_NONE);
	InitDiabloMsg(EMSG_LOADING);
	force_redraw = 255;
	DrawAndBlit();
	LoadGame(false);
	ClrDiabloMsg();
	PaletteFadeOut(8);
	deathflag = false;
	force_redraw = 255;
	DrawAndBlit();
	PaletteFadeIn(8);
	SetCursor_(CURSOR_HAND);
	interface_msg_pump();
	SetWindowProc(saveProc);
}

void gamemenu_save_game(bool bActivate)
{
	if (pcurs != CURSOR_HAND) {
		return;
	}

	if (myplr().data._pmode == PM_DEATH || deathflag) {
		gamemenu_off();
		return;
	}

	WNDPROC saveProc = SetWindowProc(DisableInputWndProc);
	SetCursor_(CURSOR_NONE);
	gamemenu_off();
	InitDiabloMsg(EMSG_SAVING);
	force_redraw = 255;
	DrawAndBlit();
	SaveGame();
	ClrDiabloMsg();
	force_redraw = 255;
	SetCursor_(CURSOR_HAND);
	interface_msg_pump();
	SetWindowProc(saveProc);
}

void gamemenu_restart_town(bool bActivate)
{
	NetSendCmd(TRUE, CMD_RETOWN);
}

void gamemenu_options(bool bActivate)
{
	gamemenu_get_music();
	gamemenu_get_sound();
	gamemenu_get_gamma();
	gamemenu_get_color_cycling();
	gmenu_set_items(sgOptionsMenu, NULL);
}

void gamemenu_get_music()
{
	gamemenu_sound_music_toggle(music_toggle_names, sgOptionsMenu, sound_get_or_set_music_volume(1));
}

void gamemenu_sound_music_toggle(char **names, TMenuItem *menu_item, int volume)
{
	if (gbSndInited) {
		menu_item->dwFlags |= GMENU_ENABLED | GMENU_SLIDER;
		menu_item->pszStr = *names;
		gmenu_slider_steps(menu_item, 17);
		gmenu_slider_set(menu_item, VOLUME_MIN, VOLUME_MAX, volume);
		return;
	}

	menu_item->dwFlags &= ~(GMENU_ENABLED | GMENU_SLIDER);
	menu_item->pszStr = names[1];
}

void gamemenu_get_sound()
{
	gamemenu_sound_music_toggle(sound_toggle_names, &sgOptionsMenu[1], sound_get_or_set_sound_volume(1));
}

void gamemenu_get_color_cycling()
{
	sgOptionsMenu[3].pszStr = color_cycling_toggle_names[palette_get_color_cycling() & 1];
}

void gamemenu_get_gamma()
{
	gmenu_slider_steps(&sgOptionsMenu[2], 15);
	gmenu_slider_set(&sgOptionsMenu[2], 30, 100, UpdateGamma(0));
}

void gamemenu_music_volume(bool bActivate)
{
	int volume;

	if (bActivate) {
		if (gbMusicOn) {
			gbMusicOn = false;
			music_stop();
			sound_get_or_set_music_volume(VOLUME_MIN);
		} else {
			gbMusicOn = TRUE;
			sound_get_or_set_music_volume(VOLUME_MAX);
			music_start(int(lvl.type()));
		}
	} else {
		volume = gamemenu_slider_music_sound(&sgOptionsMenu[0]);
		sound_get_or_set_music_volume(volume);

		if (volume == VOLUME_MIN) {
			if (gbMusicOn) {
				gbMusicOn = false;
				music_stop();
			}
		} else if (!gbMusicOn) {
			gbMusicOn = TRUE;
			music_start(int(lvl.type()));
		}
	}
	gamemenu_get_music();
}

int gamemenu_slider_music_sound(TMenuItem *menu_item)
{
	return gmenu_slider_get(menu_item, VOLUME_MIN, VOLUME_MAX);
}

void gamemenu_sound_volume(bool bActivate)
{
	int volume;
	if (bActivate) {
		if (gbSoundOn) {
			gbSoundOn = false;
			sound_stop();
			sound_get_or_set_sound_volume(VOLUME_MIN);
		} else {
			gbSoundOn = TRUE;
			sound_get_or_set_sound_volume(VOLUME_MAX);
		}
	} else {
		volume = gamemenu_slider_music_sound(&sgOptionsMenu[1]);
		sound_get_or_set_sound_volume(volume);
		if (volume == VOLUME_MIN) {
			if (gbSoundOn) {
				gbSoundOn = false;
				sound_stop();
			}
		} else if (!gbSoundOn) {
			gbSoundOn = TRUE;
		}
	}
	PlaySFX(IS_TITLEMOV);
	gamemenu_get_sound();
}

void gamemenu_gamma(bool bActivate)
{
	int gamma;
	if (bActivate) {
		gamma = UpdateGamma(0);
		if (gamma == 30)
			gamma = 100;
		else
			gamma = 30;
	} else {
		gamma = gamemenu_slider_gamma();
	}

	UpdateGamma(gamma);
	gamemenu_get_gamma();
}

int gamemenu_slider_gamma()
{
	return gmenu_slider_get(&sgOptionsMenu[2], 30, 100);
}

void gamemenu_color_cycling(bool bActivate)
{
	bool color_cycling;

	color_cycling = palette_set_color_cycling(palette_get_color_cycling() == 0);
	sgOptionsMenu[3].pszStr = color_cycling_toggle_names[color_cycling & 1];
}

DEVILUTION_END_NAMESPACE
