/**
 * @file interfac.cpp
 *
 * Implementation of load screens.
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

uint8_t *sgpBackCel;
int sgdwProgress;
int progress_id;

/** The colour used for the progress bar as an index into the palette. */
const uint8_t BarColor[3] = { 138, 43, 254 };
/** The screen position of the top left corner of the progress bar. */
const int BarPos[3][2] = { { 53, 37 }, { 53, 421 }, { 53, 37 } };

void interface_msg_pump()
{
	MSG Msg;

	while (PeekMessage(&Msg)) {
		if (Msg.message != DVL_WM_QUIT) {
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
	}
}

bool IncProgress()
{
	interface_msg_pump();
	sgdwProgress += 23;
	if ((DWORD)sgdwProgress > 534)
		sgdwProgress = 534;
	if (sgpBackCel)
		DrawCutscene();
	return (DWORD)sgdwProgress >= 534;
}

void DrawCutscene()
{
	DWORD i;

	lock_buf(1);
	CelDraw(PANEL_X, 480 + SCREEN_Y - 1, sgpBackCel, 1, 640);

	for (i = 0; i < sgdwProgress; i++) {
		DrawProgress(
		    BarPos[progress_id][0] + i + PANEL_X,
		    BarPos[progress_id][1] + SCREEN_Y,
		    progress_id);
	}

	unlock_buf(1);
	force_redraw = 255;
	scrollrt_draw_game_screen(false);
}

void DrawProgress(int screen_x, int screen_y, int progress_id)
{
	uint8_t *dst;
	int i;

	dst = &gpBuffer[screen_x + BUFFER_WIDTH * screen_y];
	for (i = 0; i < 22; i++) {
		*dst = BarColor[progress_id];
		dst += BUFFER_WIDTH;
	}
}

void ShowProgress(unsigned int uMsg)
{
	WNDPROC saveProc;

	gbSomebodyWonGameKludge = false;
	plrmsg_delay(true);

	assert(ghMainWnd);
	saveProc = SetWindowProc(DisableInputWndProc);

	interface_msg_pump();
	ClearScreenBuffer();
	scrollrt_draw_game_screen(true);
	InitCutscene(uMsg);
	BlackPalette();
	DrawCutscene();
	PaletteFadeIn(8);
	IncProgress();
	sound_init();
	IncProgress();

	switch (uMsg) {
	case WM_DIABLOADGAME:
		IncProgress();
		IncProgress();
		LoadGame(true);
		IncProgress();
		IncProgress();
		break;
	case WM_DIABNEWGAME:
		IncProgress();
		FreeGameMem();
		IncProgress();
		pfile_remove_temp_files();
		IncProgress();
		LoadGameLevel(true, 0);
		IncProgress();
		break;
	case WM_DIABNEXTLVL:
		IncProgress();
		if (plr.isSingleplayer()) {
			SaveLevel();
		} else {
			DeltaSaveLevel();
		}
		IncProgress();
		FreeGameMem();
		lvl.currlevel++;
		lvl.setType(DunType(gnLevelTypeTbl[lvl.currlevel]));
		assert(myplr().data.plrlevel == lvl.currlevel);
		IncProgress();
		LoadGameLevel(false, 0);
		IncProgress();
		break;
	case WM_DIABPREVLVL:
		IncProgress();
		if (plr.isSingleplayer()) {
			SaveLevel();
		} else {
			DeltaSaveLevel();
		}
		IncProgress();
		FreeGameMem();
		lvl.currlevel--;
		lvl.setType(DunType(gnLevelTypeTbl[lvl.currlevel]));
		assert(myplr().data.plrlevel == lvl.currlevel);
		IncProgress();
		LoadGameLevel(false, 1);
		IncProgress();
		break;
	case WM_DIABSETLVL:
		SetReturnLvlPos();
		IncProgress();
		if (plr.isSingleplayer()) {
			SaveLevel();
		} else {
			DeltaSaveLevel();
		}
		IncProgress();
		lvl.setlevel = true;
		lvl.setType(lvl.setlvltype);
		FreeGameMem();
		IncProgress();
		LoadGameLevel(false, 2);
		IncProgress();
		break;
	case WM_DIABRTNLVL:
		IncProgress();
		if (plr.isSingleplayer()) {
			SaveLevel();
		} else {
			DeltaSaveLevel();
		}
		IncProgress();
		lvl.setlevel = false;
		FreeGameMem();
		IncProgress();
		GetReturnLvlPos();
		LoadGameLevel(false, 3);
		IncProgress();
		break;
	case WM_DIABWARPLVL:
		IncProgress();
		if (plr.isSingleplayer()) {
			SaveLevel();
		} else {
			DeltaSaveLevel();
		}
		IncProgress();
		FreeGameMem();
		GetPortalLevel();
		IncProgress();
		LoadGameLevel(false, 5);
		IncProgress();
		break;
	case WM_DIABTOWNWARP:
		IncProgress();
		if (plr.isSingleplayer()) {
			SaveLevel();
		} else {
			DeltaSaveLevel();
		}
		IncProgress();
		FreeGameMem();
		lvl.currlevel = myplr().data.plrlevel;
		lvl.setType(DunType(gnLevelTypeTbl[lvl.currlevel]));
		assert(myplr().data.plrlevel == lvl.currlevel);
		IncProgress();
		LoadGameLevel(false, 6);
		IncProgress();
		break;
	case WM_DIABTWARPUP:
		IncProgress();
		if (plr.isSingleplayer()) {
			SaveLevel();
		} else {
			DeltaSaveLevel();
		}
		IncProgress();
		FreeGameMem();
		lvl.currlevel = myplr().data.plrlevel;
		lvl.setType(DunType(gnLevelTypeTbl[lvl.currlevel]));
		assert(myplr().data.plrlevel == lvl.currlevel);
		IncProgress();
		LoadGameLevel(false, 7);
		IncProgress();
		break;
	case WM_DIABRETOWN:
		IncProgress();
		if (plr.isSingleplayer()) {
			SaveLevel();
		} else {
			DeltaSaveLevel();
		}
		IncProgress();
		FreeGameMem();
		lvl.currlevel = myplr().data.plrlevel;
		lvl.setType(DunType(gnLevelTypeTbl[lvl.currlevel]));
		assert(myplr().data.plrlevel == lvl.currlevel);
		IncProgress();
		LoadGameLevel(false, 0);
		IncProgress();
		break;
	}

	assert(ghMainWnd);

	PaletteFadeOut(8);
	FreeInterface();

	saveProc = SetWindowProc(saveProc);
	assert(saveProc == DisableInputWndProc);

	NetSendCmdLocParam1(true, CMD_PLAYER_JOINLEVEL, myplr().pos(), myplr().data.plrlevel);
	plrmsg_delay(false);
	ResetPal();

	if (gbSomebodyWonGameKludge && myplr().data.plrlevel == 16) {
		PrepDoEnding();
	}

	gbSomebodyWonGameKludge = false;
}

void FreeInterface()
{
	MemFreeDbg(sgpBackCel);
}

void InitCutscene(unsigned int uMsg)
{
	assert(!sgpBackCel);

	switch (uMsg) {
	case WM_DIABNEXTLVL:
		switch (gnLevelTypeTbl[lvl.currlevel]) {
		case 0:
			sgpBackCel = LoadFileInMem("Gendata\\Cuttt.CEL", NULL);
			LoadPalette("Gendata\\Cuttt.pal");
			progress_id = 1;
			break;
		case 1:
			sgpBackCel = LoadFileInMem("Gendata\\Cutl1d.CEL", NULL);
			LoadPalette("Gendata\\Cutl1d.pal");
			progress_id = 0;
			break;
		case 2:
			sgpBackCel = LoadFileInMem("Gendata\\Cut2.CEL", NULL);
			LoadPalette("Gendata\\Cut2.pal");
			progress_id = 2;
			break;
		case 3:
			sgpBackCel = LoadFileInMem("Gendata\\Cut3.CEL", NULL);
			LoadPalette("Gendata\\Cut3.pal");
			progress_id = 1;
			break;
		case 4:
			if (lvl.currlevel < 15) {
				sgpBackCel = LoadFileInMem("Gendata\\Cut4.CEL", NULL);
				LoadPalette("Gendata\\Cut4.pal");
				progress_id = 1;
			} else {
				sgpBackCel = LoadFileInMem("Gendata\\Cutgate.CEL", NULL);
				LoadPalette("Gendata\\Cutgate.pal");
				progress_id = 1;
			}
			break;
		default:
			sgpBackCel = LoadFileInMem("Gendata\\Cutl1d.CEL", NULL);
			LoadPalette("Gendata\\Cutl1d.pal");
			progress_id = 0;
			break;
		}
		break;
	case WM_DIABPREVLVL:
		if (gnLevelTypeTbl[lvl.currlevel - 1] == 0) {
			sgpBackCel = LoadFileInMem("Gendata\\Cuttt.CEL", NULL);
			LoadPalette("Gendata\\Cuttt.pal");
			progress_id = 1;
		} else {
			switch (gnLevelTypeTbl[lvl.currlevel]) {
			case 0:
				sgpBackCel = LoadFileInMem("Gendata\\Cuttt.CEL", NULL);
				LoadPalette("Gendata\\Cuttt.pal");
				progress_id = 1;
				break;
			case 1:
				sgpBackCel = LoadFileInMem("Gendata\\Cutl1d.CEL", NULL);
				LoadPalette("Gendata\\Cutl1d.pal");
				progress_id = 0;
				break;
			case 2:
				sgpBackCel = LoadFileInMem("Gendata\\Cut2.CEL", NULL);
				LoadPalette("Gendata\\Cut2.pal");
				progress_id = 2;
				break;
			case 3:
				sgpBackCel = LoadFileInMem("Gendata\\Cut3.CEL", NULL);
				LoadPalette("Gendata\\Cut3.pal");
				progress_id = 1;
				break;
			case 4:
				sgpBackCel = LoadFileInMem("Gendata\\Cut4.CEL", NULL);
				LoadPalette("Gendata\\Cut4.pal");
				progress_id = 1;
				break;
			default:
				sgpBackCel = LoadFileInMem("Gendata\\Cutl1d.CEL", NULL);
				LoadPalette("Gendata\\Cutl1d.pal");
				progress_id = 0;
				break;
			}
		}
		break;
	case WM_DIABSETLVL:
		if (lvl.setlvlnum == SetLvl::BoneChamb) {
			sgpBackCel = LoadFileInMem("Gendata\\Cut2.CEL", NULL);
			LoadPalette("Gendata\\Cut2.pal");
			progress_id = 2;
		} else if (lvl.setlvlnum == SetLvl::VileBetrayer) {
			sgpBackCel = LoadFileInMem("Gendata\\Cutportr.CEL", NULL);
			LoadPalette("Gendata\\Cutportr.pal");
			progress_id = 1;
		} else {
			sgpBackCel = LoadFileInMem("Gendata\\Cutl1d.CEL", NULL);
			LoadPalette("Gendata\\Cutl1d.pal");
			progress_id = 0;
		}
		break;
	case WM_DIABRTNLVL:
		if (lvl.setlvlnum == SetLvl::BoneChamb) {
			sgpBackCel = LoadFileInMem("Gendata\\Cut2.CEL", NULL);
			LoadPalette("Gendata\\Cut2.pal");
			progress_id = 2;
		} else if (lvl.setlvlnum == SetLvl::VileBetrayer) {
			sgpBackCel = LoadFileInMem("Gendata\\Cutportr.CEL", NULL);
			LoadPalette("Gendata\\Cutportr.pal");
			progress_id = 1;
		} else {
			sgpBackCel = LoadFileInMem("Gendata\\Cutl1d.CEL", NULL);
			LoadPalette("Gendata\\Cutl1d.pal");
			progress_id = 0;
		}
		break;
	case WM_DIABWARPLVL:
		sgpBackCel = LoadFileInMem("Gendata\\Cutportl.CEL", NULL);
		LoadPalette("Gendata\\Cutportl.pal");
		progress_id = 1;
		break;
	case WM_DIABLOADGAME:
		sgpBackCel = LoadFileInMem("Gendata\\Cutstart.CEL", NULL);
		LoadPalette("Gendata\\Cutstart.pal");
		progress_id = 1;
		break;
	case WM_DIABNEWGAME:
		sgpBackCel = LoadFileInMem("Gendata\\Cutstart.CEL", NULL);
		LoadPalette("Gendata\\Cutstart.pal");
		progress_id = 1;
		break;
	case WM_DIABTOWNWARP:
	case WM_DIABTWARPUP:
		switch (gnLevelTypeTbl[myplr().data.plrlevel]) {
		case 0:
			sgpBackCel = LoadFileInMem("Gendata\\Cuttt.CEL", NULL);
			LoadPalette("Gendata\\Cuttt.pal");
			progress_id = 1;
			break;
		case 2:
			sgpBackCel = LoadFileInMem("Gendata\\Cut2.CEL", NULL);
			LoadPalette("Gendata\\Cut2.pal");
			progress_id = 2;
			break;
		case 3:
			sgpBackCel = LoadFileInMem("Gendata\\Cut3.CEL", NULL);
			LoadPalette("Gendata\\Cut3.pal");
			progress_id = 1;
			break;
		case 4:
			sgpBackCel = LoadFileInMem("Gendata\\Cut4.CEL", NULL);
			LoadPalette("Gendata\\Cut4.pal");
			progress_id = 1;
			break;
		}
		break;
	case WM_DIABRETOWN:
		sgpBackCel = LoadFileInMem("Gendata\\Cuttt.CEL", NULL);
		LoadPalette("Gendata\\Cuttt.pal");
		progress_id = 1;
		break;
	default:
		app_fatal("Unknown progress mode");
		break;
	}

	sgdwProgress = 0;
}

DEVILUTION_END_NAMESPACE
