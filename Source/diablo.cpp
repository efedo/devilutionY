/**
 * @file diablo.cpp
 *
 * Implementation of the main game initialization functions.
 */
#include "all.h"
#include "../3rdParty/Storm/Source/storm.h"
#include "../DiabloUI/diabloui.h"
#include <config.h>

DEVILUTION_BEGIN_NAMESPACE

SDL_Window *ghMainWnd;
DWORD glSeedTbl[NUMLEVELS];
int gnLevelTypeTbl[NUMLEVELS];
int glEndSeed[NUMLEVELS];
int glMid1Seed[NUMLEVELS];
int glMid2Seed[NUMLEVELS];
int glMid3Seed[NUMLEVELS];
V2Di Mouse;
bool gbGameLoopStartup;
bool gbRunGame;
bool gbRunGameResult;
bool zoomflag;
bool gbProcessPlayers;
bool gbLoadGame;
int DebugMonsters[10];
bool cineflag;
int force_redraw;
bool visiondebug;
/** unused */
bool scrollflag;
bool light4flag;
bool leveldebug;
bool monstdebug;
/** unused */
bool trigdebug;
int setseed;
int debugmonsttypes;
int PauseMode;
int sgnTimeoutCurs;
char sgbMouseDown;
int color_cycle_timer;

/* rdata */

/**
 * Specifies whether to give the game exclusive access to the
 * screen, as needed for efficient rendering in fullscreen mode.
 */
bool fullscreen = true;
int showintrodebug = 1;
#ifdef _DEBUG
int questdebug = -1;
int debug_mode_key_s;
int debug_mode_key_w;
int debug_mode_key_inverted_v;
int debug_mode_dollar_sign;
int debug_mode_key_d;
int debug_mode_key_i;
int dbgplr;
int dbgqst;
int dbgmon;
int arrowdebug;
#endif
int frameflag;
int frameend;
int framerate;
int framestart;
/** Specifies whether players are in non-PvP mode. */
bool FriendlyMode = true;
/** Default quick messages */
char *spszMsgTbl[4] = {
	"I need help! Come Here!",
	"Follow me.",
	"Here's something for you.",
	"Now you DIE!"
};
/** INI files variable names for quick message keys */
char *spszMsgHotKeyTbl[4] = { "F9", "F10", "F11", "F12" };

/** To know if these things have been done when we get to the diablo_deinit() function */
bool was_archives_init = false;
bool was_ui_init = false;
bool was_snd_init = false;
bool was_sfx_init = false;

void FreeGameMem()
{
	music_stop();

	FreeMissiles();
	FreeMonsters();
	FreeObjectGFX();
	FreeMonsterSnd();
	FreeTownerGFX();
}

bool StartGame(bool bNewGame, bool bSinglePlayer)
{
	bool fExitProgram;
	unsigned int uMsg;

	gbSelectProvider = true;

	do {
		fExitProgram = false;
		gbLoadGame = false;

		if (!NetInit(bSinglePlayer, &fExitProgram)) {
			gbRunGameResult = !fExitProgram;
			break;
		}

		gbSelectProvider = false;

		if (bNewGame || !gbValidSaveFile) {
			InitLevels();
			InitQuests();
			InitPortals();
			myplr().InitDungMsgs();
		}
		if (!gbValidSaveFile || !gbLoadGame) {
			uMsg = WM_DIABNEWGAME;
		} else {
			uMsg = WM_DIABLOADGAME;
		}
		run_game_loop(uMsg);
		NetClose();
		pfile_create_player_description(0, 0);
	} while (gbRunGameResult);

	SNetDestroy();
	return gbRunGameResult;
}

// Controller support: Actions to run after updating the cursor state.
// Defined in SourceX/controls/plctrls.cpp.
extern void finish_simulated_mouse_clicks(int current_mouse_x, int current_mouse_y);
extern void plrctrls_after_check_curs_move();

static bool ProcessInput()
{
	if (PauseMode == 2) {
		return false;
	}
	if (plr.isSingleplayer() && gmenu_is_active()) {
		force_redraw |= 1;
		return false;
	}

	if (!gmenu_is_active() && sgnTimeoutCurs == CURSOR_NONE) {
#ifndef USE_SDL1
		finish_simulated_mouse_clicks(Mouse.x, Mouse.y);
#endif
		CheckCursMove();
		plrctrls_after_check_curs_move();
		track_process();
	}

	return true;
}

void run_game_loop(unsigned int uMsg)
{
	WNDPROC saveProc;
	MSG msg;

	nthread_ignore_mutex(true);
	start_game(uMsg);
	assert(ghMainWnd);
	saveProc = SetWindowProc(GM_Game);
	control_update_life_mana();
	run_delta_info();
	gbRunGame = true;
	gbProcessPlayers = true;
	gbRunGameResult = true;
	force_redraw = 255;
	DrawAndBlit();
	PaletteFadeIn(8);
	force_redraw = 255;
	gbGameLoopStartup = true;
	nthread_ignore_mutex(false);

	// Main game loop
	while (gbRunGame) {
		while (PeekMessage(&msg)) {
			if (msg.message == DVL_WM_QUIT) {
				gbRunGameResult = false;
				gbRunGame = false;
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (!gbRunGame)
			break;
		if (!nthread_has_500ms_passed(false)) {
			ProcessInput();
			DrawAndBlit();
			continue;
		}
		diablo_color_cyc_logic();
		multi_process_network_packets();
		game_loop(gbGameLoopStartup);
		gbGameLoopStartup = false;
		DrawAndBlit();
	}

	if (plr.isMultiplayer()) {
		pfile_write_hero();
	}

	pfile_flush_W();
	PaletteFadeOut(8);
	NewCursor(CURSOR_NONE);
	ClearScreenBuffer();
	force_redraw = 255;
	scrollrt_draw_game_screen(true);
	saveProc = SetWindowProc(saveProc);
	assert(saveProc == GM_Game);
	free_game();

	if (cineflag) {
		cineflag = false;
		DoEnding();
	}
}

void start_game(unsigned int uMsg)
{
	zoomflag = true;
	cineflag = false;
	InitCursor();
	InitLightTable();
	LoadDebugGFX();
	assert(ghMainWnd);
	music_stop();
	ShowProgress(uMsg);
	gmenu_init_menu();
	InitLevelCursor();
	sgnTimeoutCurs = CURSOR_NONE;
	sgbMouseDown = 0;
	track_repeat_walk(false);
}

void free_game()
{
	FreeControlPan();
	FreeInvGFX();
	FreeGMenu();
	FreeQuestText();
	FreeStoreMem();

	for (int i = 0; i < plr.maxPlayers(); i++) {
		if (!plr.isValidPlayer(i))
			continue;
		plr[i].FreePlayerGFX();
	}

	FreeItemGFX();
	FreeCursor();
	FreeLightTable();
	FreeDebugGFX();
	FreeGameMem();
}

void diablo_init()
{
	init_create_window();
	SFileEnableDirectAccess(true);
	init_archives();
	was_archives_init = true;
	UiInitialize();
	was_ui_init = true;
	ReadOnlyTest();
	InitHash();
	diablo_init_screen();
	snd_init(NULL);
	was_snd_init = true;
	sound_init();
	was_sfx_init = true;
}

void diablo_splash()
{
	if (!showintrodebug) return;
	play_movie("gendata\\logo.smk", true);
	UiTitleDialog();
}

void diablo_deinit()
{
	if (was_sfx_init)
		effects_cleanup_sfx();
	if (was_snd_init)
		sound_cleanup();
	if (was_ui_init)
		UiDestroy();
	if (was_archives_init)
		init_cleanup();
	if (was_window_init)
		dx_cleanup(); // Cleanup SDL surfaces stuff, so we have to do it before SDL_Quit().
	if (was_fonts_init)
		FontsCleanup();
	if (SDL_WasInit(SDL_INIT_EVERYTHING & ~SDL_INIT_HAPTIC))
		SDL_Quit();
}

void diablo_quit(int exitStatus)
{
	diablo_deinit();
	exit(exitStatus);
}

int DiabloMain(int argc, char **argv)
{
	fullscreen = false;
	diablo_parse_flags(argc, argv);
	diablo_init();
	diablo_splash();
	mainmenu_loop();
	diablo_deinit();
	return 0;
}

static void print_help_and_exit()
{
	printf("Options:\n");
	printf("    %-20s %-30s\n", "-h, --help", "Print this message and exit");
	printf("    %-20s %-30s\n", "--version", "Print the version and exit");
	printf("    %-20s %-30s\n", "--data-dir", "Specify the folder of diabdat.mpq");
	printf("    %-20s %-30s\n", "--save-dir", "Specify the folder of save files");
	printf("    %-20s %-30s\n", "-n", "Skip startup videos");
	printf("    %-20s %-30s\n", "-f", "Display frames per second");
	printf("    %-20s %-30s\n", "-x", "Run in windowed mode");
#ifdef _DEBUG
	printf("\nDebug options:\n");
	printf("    %-20s %-30s\n", "-d", "Increaased item drops");
	printf("    %-20s %-30s\n", "-w", "Enable cheats");
	printf("    %-20s %-30s\n", "-$", "Enable god mode");
	printf("    %-20s %-30s\n", "-^", "Enable god mode and debug tools");
	//printf("    %-20s %-30s\n", "-b", "Enable item drop log");
	printf("    %-20s %-30s\n", "-v", "Highlight visibility");
	printf("    %-20s %-30s\n", "-i", "Ignore network timeout");
	//printf("    %-20s %-30s\n", "-j <##>", "Init trigger at level");
	printf("    %-20s %-30s\n", "-l <##> <##>", "Start in level as type");
	printf("    %-20s %-30s\n", "-m <##>", "Add debug monster, up to 10 allowed");
	printf("    %-20s %-30s\n", "-q <#>", "Force a certain quest");
	printf("    %-20s %-30s\n", "-r <##########>", "Set map seed");
	printf("    %-20s %-30s\n", "-t <##>", "Set current quest level");
#endif
	printf("\nReport bugs at https://github.com/diasurgical/devilutionX/\n");
	diablo_quit(0);
}

void diablo_parse_flags(int argc, char **argv)
{
	for (int i = 1; i < argc; i++) {
		if (strcasecmp("-h", argv[i]) == 0 || strcasecmp("--help", argv[i]) == 0) {
			print_help_and_exit();
		} else if (strcasecmp("--version", argv[i]) == 0) {
			printf("%s v%s\n", PROJECT_NAME, PROJECT_VERSION);
			diablo_quit(0);
		} else if (strcasecmp("--data-dir", argv[i]) == 0) {
			basePath = argv[++i];
#ifdef _WIN32
			if (basePath.back() != '\\')
				basePath += '\\';
#else
			if (basePath.back() != '/')
				basePath += '/';
#endif
		} else if (strcasecmp("--save-dir", argv[i]) == 0) {
			prefPath = argv[++i];
#ifdef _WIN32
			if (prefPath.back() != '\\')
				prefPath += '\\';
#else
			if (prefPath.back() != '/')
				prefPath += '/';
#endif
		} else if (strcasecmp("-n", argv[i]) == 0) {
			showintrodebug = false;
		} else if (strcasecmp("-f", argv[i]) == 0) {
			EnableFrameCount();
		} else if (strcasecmp("-x", argv[i]) == 0) {
			fullscreen = false;
#ifdef _DEBUG
		} else if (strcasecmp("-^", argv[i]) == 0) {
			debug_mode_key_inverted_v = true;
		} else if (strcasecmp("-$", argv[i]) == 0) {
			debug_mode_dollar_sign = true;
			/*
		} else if (strcasecmp("-b", argv[i]) == 0) {
			debug_mode_key_b = 1;
		*/
		} else if (strcasecmp("-d", argv[i]) == 0) {
			debug_mode_key_d = true;
		} else if (strcasecmp("-i", argv[i]) == 0) {
			debug_mode_key_i = true;
			/*
		} else if (strcasecmp("-j", argv[i]) == 0) {
			debug_mode_key_J_trigger = argv[++i];
		*/
		} else if (strcasecmp("-l", argv[i]) == 0) {
			lvl.setlevel = false;
			leveldebug = true;
			lvl.setType(DunType(SDL_atoi(argv[++i])));
			lvl.currlevel = SDL_atoi(argv[++i]);
			plr[0].data.plrlevel = lvl.currlevel;
		} else if (strcasecmp("-m", argv[i]) == 0) {
			monstdebug = true;
			DebugMonsters[debugmonsttypes++] = SDL_atoi(argv[++i]);
		} else if (strcasecmp("-q", argv[i]) == 0) {
			questdebug = SDL_atoi(argv[++i]);
		} else if (strcasecmp("-r", argv[i]) == 0) {
			setseed = SDL_atoi(argv[++i]);
		} else if (strcasecmp("-s", argv[i]) == 0) {
			debug_mode_key_s = true;
		} else if (strcasecmp("-t", argv[i]) == 0) {
			leveldebug = true;
			lvl.setlevel = true;
			lvl.setlvlnum = SetLvl(SDL_atoi(argv[++i]));
		} else if (strcasecmp("-v", argv[i]) == 0) {
			visiondebug = true;
		} else if (strcasecmp("-w", argv[i]) == 0) {
			debug_mode_key_w = true;
#endif
		} else {
			printf("unrecognized option '%s'\n", argv[i]);
			print_help_and_exit();
		}
	}
}

void diablo_init_screen()
{
	Mouse.x = SCREEN_WIDTH / 2;
	Mouse.y = SCREEN_HEIGHT / 2;
	if (!sgbControllerActive)
		SetCursorPos(Mouse.x, Mouse.y);
	ScrollInfo._sd.x = 0;
	ScrollInfo._sd.y = 0;
	ScrollInfo._soff.x = 0;
	ScrollInfo._soff.y = 0;
	ScrollInfo._sdir = ScrollDir::NONE;

	ClrDiabloMsg();
}

bool PressEscKey()
{
	bool rv = false;

	if (doomflag) {
		doom_close();
		rv = true;
	}
	if (helpflag) {
		helpflag = false;
		rv = true;
	}

	if (qtextflag) {
		qtextflag = false;
		stream_stop();
		rv = true;
	} else if (stextflag) {
		STextESC();
		rv = true;
	}

	if (msgflag) {
		msgdelay = 0;
		rv = true;
	}
	if (talkflag) {
		control_reset_talk();
		rv = true;
	}
	if (dropGoldFlag) {
		control_drop_gold(DVL_VK_ESCAPE);
		rv = true;
	}
	if (spselflag) {
		spselflag = false;
		rv = true;
	}

	return rv;
}

static void GetMousePos(LPARAM lParam)
{
	Mouse.x = (short)(lParam & 0xffff);
	Mouse.y = (short)((lParam >> 16) & 0xffff);
}

LRESULT DisableInputWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case DVL_WM_KEYDOWN:
	case DVL_WM_KEYUP:
	case DVL_WM_CHAR:
	case DVL_WM_SYSKEYDOWN:
	case DVL_WM_SYSCOMMAND:
	case DVL_WM_MOUSEMOVE:
		GetMousePos(lParam);
		return 0;
	case DVL_WM_LBUTTONDOWN:
		if (sgbMouseDown != 0)
			return 0;
		sgbMouseDown = 1;
		return 0;
	case DVL_WM_LBUTTONUP:
		if (sgbMouseDown != 1)
			return 0;
		sgbMouseDown = 0;
		return 0;
	case DVL_WM_RBUTTONDOWN:
		if (sgbMouseDown != 0)
			return 0;
		sgbMouseDown = 2;
		return 0;
	case DVL_WM_RBUTTONUP:
		if (sgbMouseDown != 2)
			return 0;
		sgbMouseDown = 0;
		return 0;
	case DVL_WM_CAPTURECHANGED:
		if (hWnd == (HWND)lParam)
			return 0;
		sgbMouseDown = 0;
		return 0;
	}

	return MainWndProc(hWnd, uMsg, wParam, lParam);
}

LRESULT GM_Game(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case DVL_WM_KEYDOWN:
		PressKey(wParam);
		return 0;
	case DVL_WM_KEYUP:
		ReleaseKey(wParam);
		return 0;
	case DVL_WM_CHAR:
		PressChar(wParam);
		return 0;
	case DVL_WM_SYSKEYDOWN:
		if (PressSysKey(wParam))
			return 0;
		break;
	case DVL_WM_SYSCOMMAND:
		if (wParam == DVL_SC_CLOSE) {
			gbRunGame = false;
			gbRunGameResult = false;
			return 0;
		}
		break;
	case DVL_WM_MOUSEMOVE:
		GetMousePos(lParam);
		gmenu_on_mouse_move();
		return 0;
	case DVL_WM_LBUTTONDOWN:
		GetMousePos(lParam);
		if (sgbMouseDown == 0) {
			sgbMouseDown = 1;
			track_repeat_walk(LeftMouseDown(wParam));
		}
		return 0;
	case DVL_WM_LBUTTONUP:
		GetMousePos(lParam);
		if (sgbMouseDown == 1) {
			sgbMouseDown = 0;
			LeftMouseUp();
			track_repeat_walk(false);
		}
		return 0;
	case DVL_WM_RBUTTONDOWN:
		GetMousePos(lParam);
		if (sgbMouseDown == 0) {
			sgbMouseDown = 2;
			RightMouseDown();
		}
		return 0;
	case DVL_WM_RBUTTONUP:
		GetMousePos(lParam);
		if (sgbMouseDown == 2) {
			sgbMouseDown = 0;
		}
		return 0;
	case DVL_WM_CAPTURECHANGED:
		if (hWnd != (HWND)lParam) {
			sgbMouseDown = 0;
			track_repeat_walk(false);
		}
		break;
	case WM_DIABNEXTLVL:
	case WM_DIABPREVLVL:
	case WM_DIABRTNLVL:
	case WM_DIABSETLVL:
	case WM_DIABWARPLVL:
	case WM_DIABTOWNWARP:
	case WM_DIABTWARPUP:
	case WM_DIABRETOWN:
		if (plr.isMultiplayer())
			pfile_write_hero();
		nthread_ignore_mutex(true);
		PaletteFadeOut(8);
		sound_stop();
		music_stop();
		track_repeat_walk(false);
		sgbMouseDown = 0;
		ShowProgress(uMsg);
		force_redraw = 255;
		DrawAndBlit();
		if (gbRunGame)
			PaletteFadeIn(8);
		nthread_ignore_mutex(false);
		gbGameLoopStartup = true;
		return 0;
	}

	return MainWndProc(hWnd, uMsg, wParam, lParam);
}

bool LeftMouseDown(int wParam)
{
	if (!gmenu_left_mouse(true) && !control_check_talk_btn() && sgnTimeoutCurs == CURSOR_NONE) {
		if (deathflag) {
			control_check_btn_press();
		} else if (PauseMode != 2) {
			if (doomflag) {
				doom_close();
			} else if (spselflag) {
				SetSpell();
			} else if (stextflag != StoreTalkId::NONE) {
				CheckStoreBtn();
			} else if (Mouse.y < PANEL_TOP || Mouse.x < PANEL_LEFT || Mouse.x >= PANEL_LEFT + PANEL_WIDTH) {
				if (!gmenu_is_active() && !TryIconCurs()) {
					if (questlog && Mouse.x > 32 && Mouse.x < 288 && Mouse.y > 32 && Mouse.y < 308) {
						QuestlogESC();
					} else if (qtextflag) {
						qtextflag = false;
						stream_stop();
					} else if (chrflag && Mouse.x < SPANEL_WIDTH && Mouse.y < SPANEL_HEIGHT) {
						CheckChrBtns();
					} else if (invflag && Mouse.x > RIGHT_PANEL && Mouse.y < SPANEL_HEIGHT) {
						if (!dropGoldFlag)
							CheckInvItem();
					} else if (sbookflag && Mouse.x > RIGHT_PANEL && Mouse.y < SPANEL_HEIGHT) {
						CheckSBook();
					} else if (pcurs >= CURSOR_FIRSTITEM) {
						if (CanDropAdjacent()) {
							NetSendCmdPItem(true, CMD_PUTITEM, cursm.x, cursm.y);
							NewCursor(CURSOR_HAND);
						}
					} else {
						if (myplr().data._pStatPts != 0 && !spselflag)
							CheckLvlBtn();
						if (!lvlbtndown)
							return LeftMouseCmd(wParam == DVL_MK_SHIFT + DVL_MK_LBUTTON);
					}
				}
			} else {
				if (!talkflag && !dropGoldFlag && !gmenu_is_active())
					CheckInvScrn();
				DoPanBtn();
				if (pcurs > CURSOR_HAND && pcurs < CURSOR_FIRSTITEM)
					NewCursor(CURSOR_HAND);
			}
		}
	}

	return false;
}

bool LeftMouseCmd(bool bShift)
{
	bool bNear;

	assert(Mouse.y < PANEL_TOP || Mouse.x < PANEL_LEFT || Mouse.x >= PANEL_LEFT + PANEL_WIDTH);

	if (lvl.type() == DunType::town) {
		if (pcursitem != -1 && pcurs == CURSOR_HAND)
			NetSendCmdLocParam1(true, invflag ? CMD_GOTOGETITEM : CMD_GOTOAGETITEM, cursm, pcursitem);
		if (pcursmonst != -1)
			NetSendCmdLocParam1(true, CMD_TALKXY, cursm, pcursmonst);
		if (pcursitem == -1 && pcursmonst == -1 && pcursplr == -1)
			return true;
	} else {
		bNear = (myplr().pos() - cursm).abs().maxdim() < 2;
		if (pcursitem != -1 && pcurs == CURSOR_HAND && !bShift) {
			NetSendCmdLocParam1(true, invflag ? CMD_GOTOGETITEM : CMD_GOTOAGETITEM, cursm, pcursitem);
		} else if (pcursobj != -1 && (!bShift || bNear && object[pcursobj]._oBreak == 1)) {
			NetSendCmdLocParam1(true, pcurs == CURSOR_DISARM ? CMD_DISARMXY : CMD_OPOBJXY, cursm, pcursobj);
		} else if (myplr().data._pwtype == WT_RANGED) {
			if (bShift) {
				NetSendCmdLoc(true, CMD_RATTACKXY, cursm);
			} else if (pcursmonst != -1) {
				if (CanTalkToMonst(pcursmonst)) {
					NetSendCmdParam1(true, CMD_ATTACKID, pcursmonst);
				} else {
					NetSendCmdParam1(true, CMD_RATTACKID, pcursmonst);
				}
			} else if (pcursplr != -1 && !FriendlyMode) {
				NetSendCmdParam1(true, CMD_RATTACKPID, pcursplr);
			}
		} else {
			if (bShift) {
				if (pcursmonst != -1) {
					if (CanTalkToMonst(pcursmonst)) {
						NetSendCmdParam1(true, CMD_ATTACKID, pcursmonst);
					} else {
						NetSendCmdLoc(true, CMD_SATTACKXY, cursm);
					}
				} else {
					NetSendCmdLoc(true, CMD_SATTACKXY, cursm);
				}
			} else if (pcursmonst != -1) {
				NetSendCmdParam1(true, CMD_ATTACKID, pcursmonst);
			} else if (pcursplr != -1 && !FriendlyMode) {
				NetSendCmdParam1(true, CMD_ATTACKPID, pcursplr);
			}
		}
		if (!bShift && pcursitem == -1 && pcursobj == -1 && pcursmonst == -1 && pcursplr == -1)
			return true;
	}

	return false;
}

bool TryIconCurs()
{
	if (pcurs == CURSOR_RESURRECT) {
		NetSendCmdParam1(true, CMD_RESURRECT, pcursplr);
		return true;
	} else if (pcurs == CURSOR_HEALOTHER) {
		NetSendCmdParam1(true, CMD_HEALOTHER, pcursplr);
		return true;
	} else if (pcurs == CURSOR_TELEKINESIS) {
		DoTelekinesis();
		return true;
	} else if (pcurs == CURSOR_IDENTIFY) {
		if (pcursinvitem != -1) {
			CheckIdentify(myplr(), pcursinvitem);
		} else {
			NewCursor(CURSOR_HAND);
		}
		return true;
	} else if (pcurs == CURSOR_REPAIR) {
		if (pcursinvitem != -1) {
			DoRepair(myplr(), pcursinvitem);
		} else {
			NewCursor(CURSOR_HAND);
		}
		return true;
	} else if (pcurs == CURSOR_RECHARGE) {
		if (pcursinvitem != -1) {
			DoRecharge(myplr(), pcursinvitem);
		} else {
			NewCursor(CURSOR_HAND);
		}
		return true;
	} else if (pcurs == CURSOR_TELEPORT) {
		if (pcursmonst != -1) {
			NetSendCmdParam3(true, CMD_TSPELLID, pcursmonst, myplr().data._pTSpell, GetSpellLevel(myplr(), myplr().data._pTSpell));
		} else if (pcursplr != -1) {
			NetSendCmdParam3(true, CMD_TSPELLPID, pcursplr, myplr().data._pTSpell, GetSpellLevel(myplr(), myplr().data._pTSpell));
		} else {
			NetSendCmdLocParam2(true, CMD_TSPELLXY, cursm, myplr().data._pTSpell, GetSpellLevel(myplr(), myplr().data._pTSpell));
		}
		NewCursor(CURSOR_HAND);
		return true;
	} else if (pcurs == CURSOR_DISARM && pcursobj == -1) {
		NewCursor(CURSOR_HAND);
		return true;
	}

	return false;
}

void LeftMouseUp()
{
	gmenu_left_mouse(false);
	control_release_talk_btn();
	if (panbtndown)
		CheckBtnUp();
	if (chrbtnactive)
		ReleaseChrBtns();
	if (lvlbtndown)
		ReleaseLvlBtn();
	if (stextflag != StoreTalkId::NONE)
		ReleaseStoreBtn();
}

void RightMouseDown()
{
	if (!gmenu_is_active() && sgnTimeoutCurs == CURSOR_NONE && PauseMode != 2 && !myplr().data._pInvincible) {
		if (doomflag) {
			doom_close();
		} else if (stextflag == StoreTalkId::NONE) {
			if (spselflag) {
				SetSpell();
			} else if (Mouse.y >= SPANEL_HEIGHT
			    || (!sbookflag || Mouse.x <= RIGHT_PANEL)
			        && !TryIconCurs()
			        && (pcursinvitem == -1 || !myplr().inventory.UseInvItem(pcursinvitem))) {
				if (pcurs == CURSOR_HAND) {
					if (pcursinvitem == -1 || !myplr().inventory.UseInvItem(pcursinvitem))
						CheckPlrSpell();
				} else if (pcurs > CURSOR_HAND && pcurs < CURSOR_FIRSTITEM) {
					NewCursor(CURSOR_HAND);
				}
			}
		}
	}
}

bool PressSysKey(int wParam)
{
	if (gmenu_is_active() || wParam != DVL_VK_F10)
		return false;
	diablo_hotkey_msg(1);
	return true;
}

void diablo_hotkey_msg(DWORD dwMsg)
{
	char szMsg[MAX_SEND_STR_LEN];

	if (plr.isSingleplayer()) {
		return;
	}

	assert(dwMsg < sizeof(spszMsgTbl) / sizeof(spszMsgTbl[0]));
	if (!getIniValue("NetMsg", spszMsgHotKeyTbl[dwMsg], szMsg, MAX_SEND_STR_LEN)) {
		snprintf(szMsg, MAX_SEND_STR_LEN, "%s", spszMsgTbl[dwMsg]);
		setIniValue("NetMsg", spszMsgHotKeyTbl[dwMsg], szMsg);
	}

	NetSendCmdString(-1, szMsg);
}

void ReleaseKey(int vkey)
{
	if (vkey == DVL_VK_SNAPSHOT)
		CaptureScreen();
}

void PressKey(int vkey)
{
	if (gmenu_presskeys(vkey) || control_presskeys(vkey)) {
		return;
	}

	if (deathflag) {
		if (sgnTimeoutCurs != CURSOR_NONE) {
			return;
		}
		if (vkey == DVL_VK_F9) {
			diablo_hotkey_msg(0);
		}
		if (vkey == DVL_VK_F10) {
			diablo_hotkey_msg(1);
		}
		if (vkey == DVL_VK_F11) {
			diablo_hotkey_msg(2);
		}
		if (vkey == DVL_VK_F12) {
			diablo_hotkey_msg(3);
		}
		if (vkey == DVL_VK_RETURN) {
			if (GetAsyncKeyState(DVL_VK_MENU) & 0x8000)
				dx_reinit();
			else
				control_type_message();
		}
		if (vkey != DVL_VK_ESCAPE) {
			return;
		}
	}
	if (vkey == DVL_VK_ESCAPE) {
		if (!PressEscKey()) {
			track_repeat_walk(false);
			gamemenu_on();
		}
		return;
	}

	if (sgnTimeoutCurs != CURSOR_NONE || dropGoldFlag) {
		return;
	}
	if (vkey == DVL_VK_PAUSE) {
		diablo_pause_game();
		return;
	}
	if (PauseMode == 2) {
		if ((vkey == DVL_VK_RETURN) && (GetAsyncKeyState(DVL_VK_MENU) & 0x8000))
			dx_reinit();
		return;
	}

	if (vkey == DVL_VK_RETURN) {
		if (GetAsyncKeyState(DVL_VK_MENU) & 0x8000) {
			dx_reinit();
		} else if (stextflag) {
			STextEnter();
		} else if (questlog) {
			QuestlogEnter();
		} else {
			control_type_message();
		}
	} else if (vkey == DVL_VK_F1) {
		if (helpflag) {
			helpflag = false;
		} else if (stextflag != StoreTalkId::NONE) {
			ClearPanel();
			AddPanelString("No help available", true); /// BUGFIX: message isn't displayed
			AddPanelString("while in stores", true);
			track_repeat_walk(false);
		} else {
			invflag = false;
			chrflag = false;
			sbookflag = false;
			spselflag = false;
			if (qtextflag && lvl.type() == DunType::town) {
				qtextflag = false;
				stream_stop();
			}
			questlog = false;
			automap.disable();
			msgdelay = 0;
			gamemenu_off();
			DisplayHelp();
			doom_close();
		}
	}
#ifdef _DEBUG
	else if (vkey == DVL_VK_F2) {
	}
#endif
#ifdef _DEBUG
	else if (vkey == DVL_VK_F3) {
		if (pcursitem != -1) {
			sprintf(
			    tempstr,
			    "IDX = %i  :  Seed = %i  :  CF = %i",
			    item[pcursitem].IDidx,
			    item[pcursitem]._iSeed,
			    item[pcursitem]._iCreateInfo);
			NetSendCmdString(1 << myplr(), tempstr);
		}
		sprintf(tempstr, "Numitems : %i", item.num);
		NetSendCmdString(1 << myplr(), tempstr);
	}
#endif
#ifdef _DEBUG
	else if (vkey == DVL_VK_F4) {
		PrintDebugQuest();
	}
#endif
	else if (vkey == DVL_VK_F5) {
		if (spselflag) {
			SetSpeedSpell(0);
			return;
		}
		ToggleSpell(0);
		return;
	} else if (vkey == DVL_VK_F6) {
		if (spselflag) {
			SetSpeedSpell(1);
			return;
		}
		ToggleSpell(1);
		return;
	} else if (vkey == DVL_VK_F7) {
		if (spselflag) {
			SetSpeedSpell(2);
			return;
		}
		ToggleSpell(2);
		return;
	} else if (vkey == DVL_VK_F8) {
		if (spselflag) {
			SetSpeedSpell(3);
			return;
		}
		ToggleSpell(3);
		return;
	} else if (vkey == DVL_VK_F9) {
		diablo_hotkey_msg(0);
	} else if (vkey == DVL_VK_F10) {
		diablo_hotkey_msg(1);
	} else if (vkey == DVL_VK_F11) {
		diablo_hotkey_msg(2);
	} else if (vkey == DVL_VK_F12) {
		diablo_hotkey_msg(3);
	} else if (vkey == DVL_VK_UP) {
		if (stextflag) {
			STextUp();
		} else if (questlog) {
			QuestlogUp();
		} else if (helpflag) {
			HelpScrollUp();
		} else if (automap.enabled()) {
			automap.up();
		} else {
			myplr().StartWalk(Dir::N);
		}
	} else if (vkey == DVL_VK_DOWN) {
		if (stextflag) {
			STextDown();
		} else if (questlog) {
			QuestlogDown();
		} else if (helpflag) {
			HelpScrollDown();
		} else if (automap.enabled()) {
			automap.down();
		} else {
			myplr().StartWalk(Dir::S);
		}
	} else if (vkey == DVL_VK_PRIOR) {
		if (stextflag) {
			STextPrior();
		}
	} else if (vkey == DVL_VK_NEXT) {
		if (stextflag) {
			STextNext();
		}
	} else if (vkey == DVL_VK_LEFT) {
		if (automap.enabled() && !talkflag) {
			automap.left();
		} else {
			myplr().StartWalk(Dir::W);
		}
	} else if (vkey == DVL_VK_RIGHT) {
		if (automap.enabled() && !talkflag) {
			automap.right();
		} else {
			myplr().StartWalk(Dir::E);
		}
	} else if (vkey == DVL_VK_TAB) {
		DoAutoMap();
	} else if (vkey == DVL_VK_SPACE) {
		if (!chrflag && invflag && Mouse.x < 480 && Mouse.y < PANEL_TOP && PANELS_COVER) {
			SetCursorPos(Mouse.x + 160, Mouse.y);
		}
		if (!invflag && chrflag && Mouse.x > 160 && Mouse.y < PANEL_TOP && PANELS_COVER) {
			SetCursorPos(Mouse.x - 160, Mouse.y);
		}
		helpflag = false;
		invflag = false;
		chrflag = false;
		sbookflag = false;
		spselflag = false;
		if (qtextflag && lvl.type() == DunType::town) {
			qtextflag = false;
			stream_stop();
		}
		questlog = false;
		automap.disable();
		msgdelay = 0;
		gamemenu_off();
		doom_close();
	}
}

void diablo_pause_game()
{
	if (plr.isSingleplayer()) {
		if (PauseMode) {
			PauseMode = 0;
		} else {
			PauseMode = 2;
			sound_stop();
			track_repeat_walk(false);
		}
		force_redraw = 255;
	}
}

/**
 * @internal `return` must be used instead of `break` to be bin exact as C++
 */
void PressChar(int vkey)
{
	if (gmenu_is_active() || control_talk_last_key(vkey) || sgnTimeoutCurs != CURSOR_NONE || deathflag) {
		return;
	}
	if ((char)vkey == 'p' || (char)vkey == 'P') {
		diablo_pause_game();
		return;
	}
	if (PauseMode == 2) {
		return;
	}
	if (doomflag) {
		doom_close();
		return;
	}
	if (dropGoldFlag) {
		control_drop_gold(vkey);
		return;
	}

	switch (vkey) {
	case 'G':
	case 'g':
		DecreaseGamma();
		return;
	case 'F':
	case 'f':
		IncreaseGamma();
		return;
	case 'I':
	case 'i':
		if (stextflag == StoreTalkId::NONE) {
			sbookflag = false;
			invflag = !invflag;
			if (!invflag || chrflag) {
				if (Mouse.x < 480 && Mouse.y < PANEL_TOP && PANELS_COVER) {
					SetCursorPos(Mouse.x + 160, Mouse.y);
				}
			} else {
				if (Mouse.x > 160 && Mouse.y < PANEL_TOP && PANELS_COVER) {
					SetCursorPos(Mouse.x - 160, Mouse.y);
				}
			}
		}
		return;
	case 'C':
	case 'c':
		if (stextflag == StoreTalkId::NONE) {
			questlog = false;
			chrflag = !chrflag;
			if (!chrflag || invflag) {
				if (Mouse.x > 160 && Mouse.y < PANEL_TOP && PANELS_COVER) {
					SetCursorPos(Mouse.x - 160, Mouse.y);
				}
			} else {
				if (Mouse.x < 480 && Mouse.y < PANEL_TOP && PANELS_COVER) {
					SetCursorPos(Mouse.x + 160, Mouse.y);
				}
			}
		}
		return;
	case 'Q':
	case 'q':
		if (stextflag == StoreTalkId::NONE) {
			chrflag = false;
			if (!questlog) {
				StartQuestlog();
			} else {
				questlog = false;
			}
		}
		return;
	case 'Z':
	case 'z':
		zoomflag = !zoomflag;
		return;
	case 'S':
	case 's':
		if (stextflag == StoreTalkId::NONE) {
			invflag = false;
			if (!spselflag) {
				DoSpeedBook();
			} else {
				spselflag = false;
			}
			track_repeat_walk(false);
		}
		return;
	case 'B':
	case 'b':
		if (stextflag == StoreTalkId::NONE) {
			invflag = false;
			sbookflag = !sbookflag;
		}
		return;
	case '+':
	case '=':
		if (automap.enabled()) {
			automap.zoomIn();
		}
		return;
	case '-':
	case '_':
		if (automap.enabled()) {
			automap.zoomOut();
		}
		return;
	case 'v':
		NetSendCmdString(1 << myplr(), gszProductName);
		return;
	case 'V':
		NetSendCmdString(1 << myplr(), gszVersionNumber);
		return;
	case '!':
	case '1':
		if (myplr().data.SpdList[0]._itype != ItemType::none && myplr().data.SpdList[0]._itype != ItemType::gold) {
			myplr().inventory.UseInvItem(INVITEM_BELT_FIRST);
		}
		return;
	case '@':
	case '2':
		if (myplr().data.SpdList[1]._itype != ItemType::none && myplr().data.SpdList[1]._itype != ItemType::gold) {
			myplr().inventory.UseInvItem(INVITEM_BELT_FIRST + 1);
		}
		return;
	case '#':
	case '3':
		if (myplr().data.SpdList[2]._itype != ItemType::none && myplr().data.SpdList[2]._itype != ItemType::gold) {
			myplr().inventory.UseInvItem(INVITEM_BELT_FIRST + 2);
		}
		return;
	case '$':
	case '4':
		if (myplr().data.SpdList[3]._itype != ItemType::none && myplr().data.SpdList[3]._itype != ItemType::gold) {
			myplr().inventory.UseInvItem(INVITEM_BELT_FIRST + 3);
		}
		return;
	case '%':
	case '5':
		if (myplr().data.SpdList[4]._itype != ItemType::none && myplr().data.SpdList[4]._itype != ItemType::gold) {
			myplr().inventory.UseInvItem(INVITEM_BELT_FIRST + 4);
		}
		return;
	case '^':
	case '6':
		if (myplr().data.SpdList[5]._itype != ItemType::none && myplr().data.SpdList[5]._itype != ItemType::gold) {
			myplr().inventory.UseInvItem(INVITEM_BELT_FIRST + 5);
		}
		return;
	case '&':
	case '7':
		if (myplr().data.SpdList[6]._itype != ItemType::none && myplr().data.SpdList[6]._itype != ItemType::gold) {
			myplr().inventory.UseInvItem(INVITEM_BELT_FIRST + 6);
		}
		return;
	case '*':
	case '8':
#ifdef _DEBUG
		if (debug_mode_key_inverted_v || debug_mode_key_w) {
			NetSendCmd(true, CMD_CHEAT_EXPERIENCE);
			return;
		}
#endif
		if (myplr().data.SpdList[7]._itype != ItemType::none && myplr().data.SpdList[7]._itype != ItemType::gold) {
			myplr().inventory.UseInvItem(INVITEM_BELT_FIRST + 7);
		}
		return;
#ifdef _DEBUG
	case ')':
	case '0':
		if (debug_mode_key_inverted_v) {
			if (arrowdebug > 2) {
				arrowdebug = 0;
			}
			if (arrowdebug == 0) {
				myplr().data._pIFlags &= ~ItemSpecialEffect::FIRE_ARROWS;
				myplr().data._pIFlags &= ~ItemSpecialEffect::LIGHT_ARROWS;
			}
			if (arrowdebug == 1) {
				myplr().data._pIFlags |= ItemSpecialEffect::FIRE_ARROWS;
			}
			if (arrowdebug == 2) {
				myplr().data._pIFlags |= ItemSpecialEffect::LIGHT_ARROWS;
			}
			arrowdebug++;
		}
		return;
	case ':':
		if (lvl.currlevel == 0 && debug_mode_key_w) {
			SetAllSpellsCheat();
		}
		return;
	case '[':
		if (lvl.currlevel == 0 && debug_mode_key_w) {
			TakeGoldCheat();
		}
		return;
	case ']':
		if (lvl.currlevel == 0 && debug_mode_key_w) {
			MaxSpellsCheat();
		}
		return;
	case 'a':
		if (debug_mode_key_inverted_v) {
			spelldata[SPL_TELEPORT].sTownSpell = 1;
			myplr().data._pSplLvl[myplr().data._pSpell]++;
		}
		return;
	case 'D':
		PrintDebugPlayer(true);
		return;
	case 'd':
		PrintDebugPlayer(false);
		return;
	case 'L':
	case 'l':
		if (debug_mode_key_inverted_v) {
			ToggleLighting();
		}
		return;
	case 'M':
		NextDebugMonster();
		return;
	case 'm':
		GetDebugMonster();
		return;
	case 'R':
	case 'r':
		sprintf(tempstr, "seed = %i", glSeedTbl[lvl.currlevel]);
		NetSendCmdString(1 << myplr(), tempstr);
		sprintf(tempstr, "Mid1 = %i : Mid2 = %i : Mid3 = %i", glMid1Seed[lvl.currlevel], glMid2Seed[lvl.currlevel], glMid3Seed[lvl.currlevel]);
		NetSendCmdString(1 << myplr(), tempstr);
		sprintf(tempstr, "End = %i", glEndSeed[lvl.currlevel]);
		NetSendCmdString(1 << myplr(), tempstr);
		return;
	case 'T':
	case 't':
		if (debug_mode_key_inverted_v) {
			sprintf(tempstr, "PX = %i  PY = %i", myplr().pos().x, myplr().pos().y);
			NetSendCmdString(1 << myplr(), tempstr);
			sprintf(tempstr, "CX = %i  CY = %i  DP = %i", cursm.x, cursm.y, dgrid[cursm.x][cursm.y].dungeon);
			NetSendCmdString(1 << myplr(), tempstr);
		}
		return;
	case '|':
		if (lvl.currlevel == 0 && debug_mode_key_w) {
			GiveGoldCheat();
		}
		return;
	case '~':
		if (lvl.currlevel == 0 && debug_mode_key_w) {
			StoresCheat();
		}
		return;
#endif
	}
}

void LoadLvlGFX()
{
	lvl.loadGFX();
}

void LoadAllGFX()
{
	IncProgress();
	IncProgress();
	InitObjectGFX();
	IncProgress();
	InitMissileGFX();
	IncProgress();
}

/**
 * @param lvldir method of entry
 */
void CreateLevel(int lvldir)
{
	lvl.create(lvldir);
}

void LoadGameLevel(bool firstflag, int lvldir)
{
	int i, j;
	bool visited;

	if (setseed)
		glSeedTbl[lvl.currlevel] = setseed;

	music_stop();
	NewCursor(CURSOR_HAND);
	SetRndSeed(glSeedTbl[lvl.currlevel]);
	IncProgress();
	MakeLightTable();
	LoadLvlGFX();
	IncProgress();

	if (firstflag) {
		InitInv();
		InitItemGFX();
		InitQuestText();

		for (i = 0; i < plr.maxPlayers(); i++) {
			if (!plr.isValidPlayer(i)) continue;
			plr[i].InitPlrGFXMem();
		}

		InitStores();
		automap.initOnce();
		InitHelp();
	}

	SetRndSeed(glSeedTbl[lvl.currlevel]);

	if (lvl.type() == DunType::town)
		SetupTownStores();

	IncProgress();
	automap.init();

	if (lvl.type() != DunType::town && lvldir != 4) {
		InitLighting();
		InitVision();
	}

	beastiary.InitLevelMonsters();
	IncProgress();

	if (!lvl.setlevel) {
		CreateLevel(lvldir);
		IncProgress();
		pieces.FillSolidBlockTbls(lvl.type());
		SetRndSeed(glSeedTbl[lvl.currlevel]);

		if (lvl.type() != DunType::town) {
			beastiary.GetLevelMTypes();
			InitThemes();
			LoadAllGFX();
		} else {
			IncProgress();
			IncProgress();
			InitMissileGFX();
			IncProgress();
			IncProgress();
		}

		IncProgress();

		if (lvldir == 3)
			GetReturnLvlPos();
		if (lvldir == 5)
			GetPortalLvlPos();

		IncProgress();

		for (i = 0; i < MAX_PLRS; i++) {
			if (!plr.isValidPlayer(i)) continue;
			if (plr[i].data.plractive && lvl.currlevel == plr[i].data.plrlevel) {
				plr[i].InitPlayerGFX();
				if (lvldir != 4)
					plr[i].InitPlayer(firstflag);
			}
		}

		PlayDungMsgs();
		InitMultiView();
		IncProgress();

		visited = false;
		for (i = 0; i < plr.maxPlayers(); i++) {
			if (!plr.isValidPlayer(i)) continue;
			if (plr[i].data.plractive)
				visited = visited || plr[i].data._pLvlVisited[lvl.currlevel];
		}

		SetRndSeed(glSeedTbl[lvl.currlevel]);

		if (lvl.type() != DunType::town) {
			if (firstflag || lvldir == 4 || !myplr().data._pLvlVisited[lvl.currlevel] || plr.isMultiplayer()) {
				HoldThemeRooms();
				glMid1Seed[lvl.currlevel] = GetRndSeed();
				InitMonsters();
				glMid2Seed[lvl.currlevel] = GetRndSeed();
				IncProgress();
				InitObjects();
				InitItems();
				CreateThemeRooms();
				IncProgress();
				glMid3Seed[lvl.currlevel] = GetRndSeed();
				InitMissiles();
				InitDead();
				glEndSeed[lvl.currlevel] = GetRndSeed();

				if (plr.isMultiplayer())
					DeltaLoadLevel();

				IncProgress();
				SavePreLighting();
			} else {
				InitMonsters();
				InitMissiles();
				InitDead();
				IncProgress();
				LoadLevel();
				IncProgress();
			}
		} else {
			for (i = 0; i < MAXDUNX; i++) {
				for (j = 0; j < MAXDUNY; j++)
					grid[i][j].dFlags |= BFLAG_LIT;
			}

			InitTowners();
			InitItems();
			InitMissiles();
			IncProgress();

			if (!firstflag && lvldir != 4 && myplr().data._pLvlVisited[lvl.currlevel] && plr.isSingleplayer())
				LoadLevel();
			if (plr.isMultiplayer())
				DeltaLoadLevel();

			IncProgress();
		}
		if (plr.isSingleplayer())
			ResyncQuests();
		else
			ResyncMPQuests();
	} else {
		LoadSetMap();
		IncProgress();
		beastiary.GetLevelMTypes();
		IncProgress();
		InitMonsters();
		IncProgress();
		InitMissileGFX();
		IncProgress();
		InitDead();
		IncProgress();
		pieces.FillSolidBlockTbls(lvl.type());
		IncProgress();

		if (lvldir == 5)
			GetPortalLvlPos();
		IncProgress();

		for (i = 0; i < MAX_PLRS; i++) {
			if (plr[i].data.plractive && lvl.currlevel == plr[i].data.plrlevel) {
				plr[i].InitPlayerGFX();
				if (lvldir != 4)
					plr[i].InitPlayer(firstflag);
			}
		}
		IncProgress();

		InitMultiView();
		IncProgress();

		if (firstflag || lvldir == 4 || !myplr().data._pSetLvlVisited[int(lvl.setlvlnum)]) {
			InitItems();
			SavePreLighting();
		} else {
			LoadLevel();
		}

		InitMissiles();
		IncProgress();
	}

	SyncPortals();

	for (i = 0; i < MAX_PLRS; i++) {
		if (!plr.isValidPlayer(i)) continue;
		if (plr[i].data.plractive && plr[i].data.plrlevel == lvl.currlevel && (!plr[i].data._pLvlChanging || i == myplr())) {
			if (plr[i].data._pHitPoints > 0) {
				if (plr.isSingleplayer())
					plr[i].addToMap();
				else
					plr[i].SyncInitPlrPos();
			} else {
				grid.at(plr[i].pos()).dFlags |= BFLAG_DEAD_PLAYER;
			}
		}
	}

	SetDungeonMicros();

	InitLightMax();
	IncProgress();
	IncProgress();

	if (firstflag) {
		InitControlPan();
	}
	IncProgress();
	if (lvl.type() != DunType::town) {
		ProcessLightList();
		ProcessVisionList();
	}

	music_start(int(lvl.type()));

	while (!IncProgress())
		;

	if (lvl.setlevel && lvl.setlvlnum == SetLvl::SkelKing && quests[Q_SKELKING]._qactive == QUEST_ACTIVE)
		PlaySFX(USFX_SKING1);
}

void game_loop(bool bStartup)
{
	int i;

	i = bStartup ? 60 : 3;

	while (i--) {
		if (!multi_handle_delta()) {
			timeout_cursor(true);
			break;
		} else {
			timeout_cursor(false);
			game_logic();
		}
		if (!gbRunGame || plr.isSingleplayer() || !nthread_has_500ms_passed(true))
			break;
	}
}

// Controller support:
extern void plrctrls_after_game_logic();

// Runs once per main game loop
void game_logic()
{
	if (!ProcessInput()) {
		return;
	}
	if (gbProcessPlayers) {
		ProcessPlayers();
	}
	if (lvl.type() != DunType::town) {
		ProcessMonsters();
		ProcessObjects();
		ProcessMissiles();
		ProcessItems();
		ProcessLightList();
		ProcessVisionList();
	} else {
		ProcessTowners();
		ProcessItems();
		ProcessMissiles();
	}

#ifdef _DEBUG
	if (debug_mode_key_inverted_v && GetAsyncKeyState(DVL_VK_SHIFT) & 0x8000) {
		ScrollView();
	}
#endif

	sound_update();
	ClearPlrMsg();
	CheckTriggers();
	CheckQuests();
	force_redraw |= 1;
	pfile_update(false);

	plrctrls_after_game_logic();
}

void timeout_cursor(bool bTimeout)
{
	if (bTimeout) {
		if (sgnTimeoutCurs == CURSOR_NONE && !sgbMouseDown) {
			sgnTimeoutCurs = pcurs;
			multi_net_ping();
			ClearPanel();
			AddPanelString("-- Network timeout --", true);
			AddPanelString("-- Waiting for players --", true);
			NewCursor(CURSOR_HOURGLASS);
			force_redraw = 255;
		}
		scrollrt_draw_game_screen(true);
	} else if (sgnTimeoutCurs != CURSOR_NONE) {
		NewCursor(sgnTimeoutCurs);
		sgnTimeoutCurs = CURSOR_NONE;
		ClearPanel();
		force_redraw = 255;
	}
}

void diablo_color_cyc_logic()
{
	if (!palette_get_color_cycling())
		return;

	if (lvl.type() == DunType::hell) {
		lighting_color_cycling();
	} else if (lvl.type() == DunType::caves) {
		palette_update_caves();
	}
}

DEVILUTION_END_NAMESPACE
