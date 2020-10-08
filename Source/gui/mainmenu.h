/**
 * @file mainmenu.h
 *
 * Interface of functions for interacting with the main menu.
 */
#ifndef __MAINMENU_H__
#define __MAINMENU_H__

namespace dvl {

//#ifdef __cplusplus
//extern "C" {
//#endif

extern char gszHero[16];

void mainmenu_refresh_music();
void mainmenu_change_name(int arg1, int arg2, int arg3, int arg4, char *name_1, char *name_2);
bool mainmenu_select_hero_dialog(
    const _SNETPROGRAMDATA *client_info,
    const _SNETPLAYERDATA *user_info,
    const _SNETUIDATA *ui_info,
    const _SNETVERSIONDATA *fileinfo,
    DWORD mode,               // 4 chars, e.g. 'IPXN', 'BNET' etc. */
    char *cname, DWORD clen,  // character name will be copied here
    char *cdesc, DWORD cdlen, // character "description" will be copied here (used to advertise games)
    bool *multi);             // new character? - unsure about this
void mainmenu_loop();
bool mainmenu_single_player();
bool mainmenu_init_menu(int type);
bool mainmenu_multi_player();
void mainmenu_play_intro();

/* rdata */

/* data */

extern int menu_music_track_id;

//#ifdef __cplusplus
//}
//#endif

}

#endif /* __MAINMENU_H__ */
