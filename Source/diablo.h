/**
 * @file diablo.h
 *
 * Interface of the main game initialization functions.
 */
#ifndef __DIABLO_H__
#define __DIABLO_H__

DEVILUTION_BEGIN_NAMESPACE

#include <string>

extern SDL_Window *ghMainWnd;
extern DWORD glSeedTbl[NUMLEVELS];
extern int gnLevelTypeTbl[NUMLEVELS];
extern int glEndSeed[NUMLEVELS];
extern int glMid1Seed[NUMLEVELS];
extern int glMid2Seed[NUMLEVELS];
extern int glMid3Seed[NUMLEVELS];
extern V2Di Mouse;
extern bool gbGameLoopStartup;
extern bool gbRunGame;
extern bool gbRunGameResult;
extern bool zoomflag;
extern bool gbProcessPlayers;
extern bool gbLoadGame;
extern HINSTANCE ghInst;
extern int DebugMonsters[10];
extern BOOLEAN cineflag;
extern int force_redraw;
extern bool visiondebug;
/* These are defined in fonts.h */
extern bool was_fonts_init;
extern void FontsCleanup();
/** unused */
extern bool scrollflag;
extern bool light4flag;
extern bool leveldebug;
extern bool monstdebug;
/** unused */
extern bool trigdebug;
extern int setseed;
extern int debugmonsttypes;
extern int PauseMode;
extern char sgbMouseDown;
extern int color_cycle_timer;

void FreeGameMem();
bool StartGame(bool bNewGame, bool bSinglePlayer);
void run_game_loop(unsigned int uMsg);
void start_game(unsigned int uMsg);
void free_game();
int DiabloMain(int argc, char **argv);
void diablo_parse_flags(int argc, char **argv);
void diablo_init_screen();
void diablo_reload_process(HINSTANCE hInstance);
void diablo_quit(int exitStatus);
bool PressEscKey();
LRESULT DisableInputWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT GM_Game(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
bool LeftMouseDown(int wParam);
bool LeftMouseCmd(bool bShift);
bool TryIconCurs();
void LeftMouseUp();
void RightMouseDown();
void j_gmenu_on_mouse_move(LPARAM lParam);
bool PressSysKey(int wParam);
void diablo_hotkey_msg(DWORD dwMsg);
void ReleaseKey(int vkey);
void PressKey(int vkey);
void diablo_pause_game();
void PressChar(int vkey);
void LoadLvlGFX();
void LoadAllGFX();
void CreateLevel(int lvldir);
void LoadGameLevel(bool firstflag, int lvldir);
void game_loop(bool bStartup);
void game_logic();
void timeout_cursor(bool bTimeout);
void diablo_color_cyc_logic();

/* data */

/* rdata */

extern bool fullscreen;
extern int showintrodebug;
#ifdef _DEBUG
extern int questdebug;
extern int debug_mode_key_s;
extern int debug_mode_key_w;
extern int debug_mode_key_inverted_v;
extern int debug_mode_dollar_sign;
extern int debug_mode_key_d;
extern int debug_mode_key_i;
extern int dbgplr;
extern int dbgqst;
extern int dbgmon;
extern int arrowdebug;
#endif
extern int frameflag;
extern int frameend;
extern int framerate;
extern int framestart;
extern bool FriendlyMode;
extern char *spszMsgTbl[4];
extern char *spszMsgHotKeyTbl[4];

DEVILUTION_END_NAMESPACE

#endif /* __DIABLO_H__ */
