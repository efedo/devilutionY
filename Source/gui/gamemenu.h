/**
 * @file gamemenu.h
 *
 * Interface of the in-game menu functions.
 */
#ifndef __GAMEMENU_H__
#define __GAMEMENU_H__

DEVILUTION_BEGIN_NAMESPACE

//#ifdef __cplusplus
//extern "C" {
//#endif

void gamemenu_on();
void gamemenu_update_single(TMenuItem *pMenuItems);
void gamemenu_update_multi(TMenuItem *pMenuItems);
void gamemenu_off();
void gamemenu_handle_previous();
void gamemenu_previous(bool bActivate);
void gamemenu_new_game(bool bActivate);
void gamemenu_quit_game(bool bActivate);
void gamemenu_load_game(bool bActivate);
void gamemenu_save_game(bool bActivate);
void gamemenu_restart_town(bool bActivate);
void gamemenu_options(bool bActivate);
void gamemenu_get_music();
void gamemenu_sound_music_toggle(char **names, TMenuItem *menu_item, int gamma);
void gamemenu_get_sound();
void gamemenu_get_color_cycling();
void gamemenu_get_gamma();
void gamemenu_music_volume(bool bActivate);
int gamemenu_slider_music_sound(TMenuItem *menu_item);
void gamemenu_sound_volume(bool bActivate);
void gamemenu_gamma(bool bActivate);
int gamemenu_slider_gamma();
void gamemenu_color_cycling(bool bActivate);

/* rdata */
extern char *music_toggle_names[];
extern char *sound_toggle_names[];
extern char *color_cycling_toggle_names[];

//#ifdef __cplusplus
//}
//#endif

DEVILUTION_END_NAMESPACE

#endif /* __GAMEMENU_H__ */
