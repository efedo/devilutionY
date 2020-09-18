#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

BOOL townwarps[3];
BOOL trigflag;
int numtrigs;
TriggerStruct trigs[MAXTRIGGERS];
int TWarpFrom;

int TownDownList[] = { 716, 715, 719, 720, 721, 723, 724, 725, 726, 727, -1 };
int TownWarp1List[] = { 1171, 1172, 1173, 1174, 1175, 1176, 1177, 1178, 1179, 1181, 1183, 1185, -1 };
int L1UpList[] = { 127, 129, 130, 131, 132, 133, 135, 137, 138, 139, 140, -1 };
int L1DownList[] = { 106, 107, 108, 109, 110, 112, 114, 115, 118, -1 };
int L2UpList[] = { 266, 267, -1 };
int L2DownList[] = { 269, 270, 271, 272, -1 };
int L2TWarpUpList[] = { 558, 559, -1 };
int L3UpList[] = { 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, -1 };
int L3DownList[] = { 162, 163, 164, 165, 166, 167, 168, 169, -1 };
int L3TWarpUpList[] = { 548, 549, 550, 551, 552, 553, 554, 555, 556, 557, 558, 559, 560, -1 };
int L4UpList[] = { 82, 83, 90, -1 };
int L4DownList[] = { 120, 130, 131, 132, 133, -1 };
int L4TWarpUpList[] = { 421, 422, 429, -1 };
int L4PentaList[] = { 353, 354, 355, 356, 357, 358, 359, 360, 361, 362, 363, 364, 365, 366, 367, 368, 369, 370, 371, 372, 373, 374, 375, 376, 377, 378, 379, 380, 381, 382, 383, 384, -1 };

#ifndef SPAWN
void InitNoTriggers()
{
	numtrigs = 0;
	trigflag = FALSE;
}
#endif

void InitTownTriggers()
{
	int i;
	numtrigs = 0;
	trigs[numtrigs]._t = { 25, 29 };
	trigs[numtrigs]._tmsg = WM_DIABNEXTLVL;
	numtrigs++;

	#ifndef SPAWN
	if (gbMaxPlayers == MAX_PLRS) {
		for (i = 0; i < sizeof(townwarps) / sizeof(townwarps[0]); i++) {
			townwarps[i] = TRUE;
		}
		trigs[numtrigs]._t = { 49, 21 };
		trigs[numtrigs]._tmsg = WM_DIABTOWNWARP;
		trigs[numtrigs]._tlvl = 5;
		numtrigs++;
		trigs[numtrigs]._t = { 17, 69 };
		trigs[numtrigs]._tmsg = WM_DIABTOWNWARP;
		trigs[numtrigs]._tlvl = 9;
		numtrigs++;
		trigs[numtrigs]._t = { 41, 80 };
		trigs[numtrigs]._tmsg = WM_DIABTOWNWARP;
		trigs[numtrigs]._tlvl = 13;
		numtrigs++;
	} else {
	#endif
		for (i = 0; i < MAX_PLRS - 1; i++) {
			townwarps[i] = FALSE;
		}
		#ifndef SPAWN
		if (myplr().data.pTownWarps & 1) {
			trigs[numtrigs]._t = { 49, 21 };
			trigs[numtrigs]._tmsg = WM_DIABTOWNWARP;
			trigs[numtrigs]._tlvl = 5;
			numtrigs++;
			townwarps[0] = TRUE;
		}
		if (myplr().data.pTownWarps & 2) {
			townwarps[1] = TRUE;
			trigs[numtrigs]._t = { 17, 69 };
			trigs[numtrigs]._tmsg = WM_DIABTOWNWARP;
			trigs[numtrigs]._tlvl = 9;
			numtrigs++;
		}
		if (myplr().data.pTownWarps & 4) {
			townwarps[2] = TRUE;
			trigs[numtrigs]._t = { 41, 80 };
			trigs[numtrigs]._tmsg = WM_DIABTOWNWARP;
			trigs[numtrigs]._tlvl = 13;
			numtrigs++;
		}
	}
#endif

	trigflag = FALSE;
}

void InitL1Triggers()
{
	int i, j;

	numtrigs = 0;
	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++) {
			if (grid[i][j].dPiece == 129) {
				trigs[numtrigs]._t = { i, j };
				trigs[numtrigs]._tmsg = WM_DIABPREVLVL;
				numtrigs++;
			}
			if (grid[i][j].dPiece == 115) {
				trigs[numtrigs]._t = { i, j };
				trigs[numtrigs]._tmsg = WM_DIABNEXTLVL;
				numtrigs++;
			}
		}
	}
	trigflag = FALSE;
}

#ifndef SPAWN
void InitL2Triggers()
{
	V2Di p;

	numtrigs = 0;
	for (p.y = 0; p.y < MAXDUNY; p.y++) {
		for (p.x = 0; p.x < MAXDUNX; p.x++) {
			if (grid.at(p).dPiece == 267 && (p != quests[Q_SCHAMB]._qt)) {
				trigs[numtrigs]._t = p;
				trigs[numtrigs]._tmsg = WM_DIABPREVLVL;
				numtrigs++;
			}

			if (grid.at(p).dPiece == 559) {
				trigs[numtrigs]._t = p;
				trigs[numtrigs]._tmsg = WM_DIABTWARPUP;
				trigs[numtrigs]._tlvl = 0;
				numtrigs++;
			}

			if (grid.at(p).dPiece == 271) {
				trigs[numtrigs]._t = p;
				trigs[numtrigs]._tmsg = WM_DIABNEXTLVL;
				numtrigs++;
			}
		}
	}
	trigflag = FALSE;
}

void InitL3Triggers()
{
	int i, j;

	numtrigs = 0;
	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++) {
			if (grid[i][j].dPiece == 171) {
				trigs[numtrigs]._t = { i, j };
				trigs[numtrigs]._tmsg = WM_DIABPREVLVL;
				numtrigs++;
			}

			if (grid[i][j].dPiece == 168) {
				trigs[numtrigs]._t = { i, j };
				trigs[numtrigs]._tmsg = WM_DIABNEXTLVL;
				numtrigs++;
			}

			if (grid[i][j].dPiece == 549) {
				trigs[numtrigs]._t = { i, j };
				trigs[numtrigs]._tmsg = WM_DIABTWARPUP;
				numtrigs++;
			}
		}
	}
	trigflag = FALSE;
}

void InitL4Triggers()
{
	int i, j;

	numtrigs = 0;
	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++) {
			if (grid[i][j].dPiece == 83) {
				trigs[numtrigs]._t = { i, j };
				trigs[numtrigs]._tmsg = WM_DIABPREVLVL;
				numtrigs++;
			}

			if (grid[i][j].dPiece == 422) {
				trigs[numtrigs]._t = { i, j };
				trigs[numtrigs]._tmsg = WM_DIABTWARPUP;
				trigs[numtrigs]._tlvl = 0;
				numtrigs++;
			}

			if (grid[i][j].dPiece == 120) {
				trigs[numtrigs]._t = { i, j };
				trigs[numtrigs]._tmsg = WM_DIABNEXTLVL;
				numtrigs++;
			}
		}
	}

	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++) {
			if (grid[i][j].dPiece == 370 && quests[Q_BETRAYER]._qactive == QUEST_DONE) {
				trigs[numtrigs]._t = { i, j };
				trigs[numtrigs]._tmsg = WM_DIABNEXTLVL;
				numtrigs++;
			}
		}
	}
	trigflag = FALSE;
}

void InitSKingTriggers()
{
	trigflag = FALSE;
	numtrigs = 1;
	trigs[0]._t = { 82, 42 };
	trigs[0]._tmsg = WM_DIABRTNLVL;
}

void InitSChambTriggers()
{
	trigflag = FALSE;
	numtrigs = 1;
	trigs[0]._t = { 70, 39 };
	trigs[0]._tmsg = WM_DIABRTNLVL;
}

void InitPWaterTriggers()
{
	trigflag = FALSE;
	numtrigs = 1;
	trigs[0]._t = { 30, 83 };
	trigs[0]._tmsg = WM_DIABRTNLVL;
}

void InitVPTriggers()
{
	trigflag = FALSE;
	numtrigs = 1;
	trigs[0]._t = { 35, 32 };
	trigs[0]._tmsg = WM_DIABRTNLVL;
}
#endif

BOOL ForceTownTrig()
{
	int i, j, k, l;

	for (i = 0; TownDownList[i] != -1; i++) {
		if (grid.at(cursm).dPiece == TownDownList[i]) {
			strcpy(infostr, "Down to dungeon");
			cursm = { 25, 29 };
			return TRUE;
		}
	}

	if (townwarps[0]) {
		for (j = 0; TownWarp1List[j] != -1; j++) {
			if (grid.at(cursm).dPiece == TownWarp1List[j]) {
				strcpy(infostr, "Down to catacombs");
				cursm = { 49, 21 };
				return TRUE;
			}
		}
	}

	if (townwarps[1]) {
		for (k = 1199; k <= 1220; k++) {
			if (grid.at(cursm).dPiece == k) {
				strcpy(infostr, "Down to caves");
				cursm = { 17, 69 };
				return TRUE;
			}
		}
	}

	if (townwarps[2]) {
		for (l = 1240; l <= 1255; l++) {
			if (grid.at(cursm).dPiece == l) {
				strcpy(infostr, "Down to hell");
				cursm = { 41, 80 };
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL ForceL1Trig()
{
	int i, j;

	for (i = 0; L1UpList[i] != -1; i++) {
		if (grid.at(cursm).dPiece == L1UpList[i]) {
			if (level.currlevel > 1)
				sprintf(infostr, "Up to level %i", level.currlevel - 1);
			else
				strcpy(infostr, "Up to town");
			for (j = 0; j < numtrigs; j++) {
				if (trigs[j]._tmsg == WM_DIABPREVLVL) {
					cursm = trigs[j]._t;
					return TRUE;
				}
			}
		}
	}

	for (i = 0; L1DownList[i] != -1; i++) {
		if (grid.at(cursm).dPiece == L1DownList[i]) {
			sprintf(infostr, "Down to level %i", level.currlevel + 1);
			for (j = 0; j < numtrigs; j++) {
				if (trigs[j]._tmsg == WM_DIABNEXTLVL) {
					cursm = trigs[j]._t;
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

BOOL ForceL2Trig()
{
	int i, j, dx, dy;

	for (i = 0; L2UpList[i] != -1; i++) {
		if (grid.at(cursm).dPiece == L2UpList[i]) {
			for (j = 0; j < numtrigs; j++) {
				if (trigs[j]._tmsg == WM_DIABPREVLVL) {
					int dist = (trigs[j]._t - cursm).maxabs();
					if (dist < 4) {
						sprintf(infostr, "Up to level %i", level.currlevel - 1);
						cursm = trigs[j]._t;
						return TRUE;
					}
				}
			}
		}
	}

	for (i = 0; L2DownList[i] != -1; i++) {
		if (grid.at(cursm).dPiece == L2DownList[i]) {
			sprintf(infostr, "Down to level %i", level.currlevel + 1);
			for (j = 0; j < numtrigs; j++) {
				if (trigs[j]._tmsg == WM_DIABNEXTLVL) {
					cursm = trigs[j]._t;
					return TRUE;
				}
			}
		}
	}

	if (level.currlevel == 5) {
		for (i = 0; L2TWarpUpList[i] != -1; i++) {
			if (grid.at(cursm).dPiece == L2TWarpUpList[i]) {
				for (j = 0; j < numtrigs; j++) {
					if (trigs[j]._tmsg == WM_DIABTWARPUP) {
						int dist = (trigs[j]._t - cursm).maxabs();
						if (dist < 4) {
							strcpy(infostr, "Up to town");
							cursm = trigs[j]._t;
							return TRUE;
						}
					}
				}
			}
		}
	}

	return FALSE;
}

BOOL ForceL3Trig()
{
	int i, j, dx, dy;
	for (i = 0; L3UpList[i] != -1; ++i) {
		if (grid.at(cursm).dPiece == L3UpList[i]) {
			sprintf(infostr, "Up to level %i", level.currlevel - 1);
			for (j = 0; j < numtrigs; j++) {
				if (trigs[j]._tmsg == WM_DIABPREVLVL) {
					cursm = trigs[j]._t;
					return TRUE;
				}
			}
		}
	}

	for (i = 0; L3DownList[i] != -1; i++) {
		if (grid.at(cursm).dPiece == L3DownList[i]
		    || grid[cursm.x + 1][cursm.y].dPiece == L3DownList[i]
		    || grid[cursm.x + 2][cursm.y].dPiece == L3DownList[i]) {
			sprintf(infostr, "Down to level %i", level.currlevel + 1);
			for (j = 0; j < numtrigs; j++) {
				if (trigs[j]._tmsg == WM_DIABNEXTLVL) {
					cursm = trigs[j]._t;
					return TRUE;
				}
			}
		}
	}

	if (level.currlevel == 9) {
		for (i = 0; L3TWarpUpList[i] != -1; i++) {
			if (grid.at(cursm).dPiece == L3TWarpUpList[i]) {
				for (j = 0; j < numtrigs; j++) {
					if (trigs[j]._tmsg == WM_DIABTWARPUP) {
						int dist = (trigs[j]._t - cursm).maxabs();
						if (dist < 4) {
							strcpy(infostr, "Up to town");
							cursm = trigs[j]._t;
							return TRUE;
						}
					}
				}
			}
		}
	}

	return FALSE;
}

BOOL ForceL4Trig()
{
	int i, j, dx, dy;

	for (i = 0; L4UpList[i] != -1; ++i) {
		if (grid.at(cursm).dPiece == L4UpList[i]) {
			sprintf(infostr, "Up to level %i", level.currlevel - 1);
			for (j = 0; j < numtrigs; j++) {
				if (trigs[j]._tmsg == WM_DIABPREVLVL) {
					cursm = trigs[j]._t;
					return TRUE;
				}
			}
		}
	}

	for (i = 0; L4DownList[i] != -1; i++) {
		if (grid.at(cursm).dPiece == L4DownList[i]) {
			sprintf(infostr, "Down to level %i", level.currlevel + 1);
			for (j = 0; j < numtrigs; j++) {
				if (trigs[j]._tmsg == WM_DIABNEXTLVL) {
					cursm = trigs[j]._t;
					return TRUE;
				}
			}
		}
	}

	if (level.currlevel == 13) {
		for (i = 0; L4TWarpUpList[i] != -1; i++) {
			if (grid.at(cursm).dPiece == L4TWarpUpList[i]) {
				for (j = 0; j < numtrigs; j++) {
					if (trigs[j]._tmsg == WM_DIABTWARPUP) {
						int dist = (trigs[j]._t - cursm).maxabs();
						if (dist < 4) {
							strcpy(infostr, "Up to town");
							cursm = trigs[j]._t;
							return TRUE;
						}
					}
				}
			}
		}
	}

	if (level.currlevel == 15) {
		for (i = 0; L4PentaList[i] != -1; i++) {
			if (grid.at(cursm).dPiece == L4PentaList[i]) {
				strcpy(infostr, "Down to Diablo");
				for (j = 0; j < numtrigs; j++) {
					if (trigs[j]._tmsg == WM_DIABNEXTLVL) {
						cursm = trigs[j]._t;
						return TRUE;
					}
				}
			}
		}
	}

	return FALSE;
}

void Freeupstairs()
{
	int i, tx, ty, xx, yy;

	for (i = 0; i < numtrigs; i++) {
		tx = trigs[i]._t.x;
		ty = trigs[i]._t.y;

		for (yy = -2; yy <= 2; yy++) {
			for (xx = -2; xx <= 2; xx++) {
				grid[tx + xx][ty + yy].dFlags |= BFLAG_POPULATED;
			}
		}
	}
}

BOOL ForceSKingTrig()
{
	for (int i = 0; L1UpList[i] != -1; i++) {
		if (grid.at(cursm).dPiece == L1UpList[i]) {
			sprintf(infostr, "Back to Level %i", quests[Q_SKELKING]._qlevel);
			cursm = trigs[0]._t;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL ForceSChambTrig()
{
	for (int i = 0; L2DownList[i] != -1; i++) {
		if (grid.at(cursm).dPiece == L2DownList[i]) {
			sprintf(infostr, "Back to Level %i", quests[Q_SCHAMB]._qlevel);
			cursm = trigs[0]._t;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL ForcePWaterTrig()
{
	for (int i = 0; L3DownList[i] != -1; i++) {
		if (grid.at(cursm).dPiece == L3DownList[i]) {
			sprintf(infostr, "Back to Level %i", quests[Q_PWATER]._qlevel);
			cursm = trigs[0]._t;
			return TRUE;
		}
	}

	return FALSE;
}

void CheckTrigForce()
{
	trigflag = FALSE;

	if (!sgbControllerActive && Mouse.y > PANEL_TOP - 1) {
		return;
	}

	if (!level.setlevel) {
		switch (level.leveltype) {
		case DTYPE_TOWN:
			trigflag = ForceTownTrig();
			break;
		case DTYPE_CATHEDRAL:
			trigflag = ForceL1Trig();
			break;
		case DTYPE_CATACOMBS:
			trigflag = ForceL2Trig();
			break;
		case DTYPE_CAVES:
			trigflag = ForceL3Trig();
			break;
		case DTYPE_HELL:
			trigflag = ForceL4Trig();
			break;
		}
		if (level.leveltype != DTYPE_TOWN && !trigflag) {
			trigflag = ForceQuests();
		}
	} else {
		switch (level.setlvlnum) {
		case SL_SKELKING:
			trigflag = ForceSKingTrig();
			break;
		case SL_BONECHAMB:
			trigflag = ForceSChambTrig();
			break;
		case SL_POISONWATER:
			trigflag = ForcePWaterTrig();
			break;
		}
	}

	if (trigflag) {
		ClearPanel();
	}
}

void CheckTriggers()
{
	int x, y, i;
	BOOL abort;
	char abortflag;

	if (myplr().data._pmode != PM_STAND)
		return;

	for (i = 0; i < numtrigs; i++) {
		if (myplr().data._p != trigs[i]._t) {
			continue;
		}

		switch (trigs[i]._tmsg) {
		case WM_DIABNEXTLVL:
#ifdef SPAWN
			if (level.currlevel >= 2) {
				NetSendCmdLoc(TRUE, CMD_WALKXY, myplr().data._px, myplr().data._py + 1);
				PlaySFX(PS_WARR18);
				InitDiabloMsg(EMSG_NOT_IN_SHAREWARE);
			} else {
#endif
				if (pcurs >= CURSOR_FIRSTITEM && DropItemBeforeTrig())
					return;
				myplr().StartNewLvl(trigs[i]._tmsg, level.currlevel + 1);
#ifdef SPAWN
			}
#endif
			break;
		case WM_DIABPREVLVL:
			if (pcurs >= CURSOR_FIRSTITEM && DropItemBeforeTrig())
				return;
			myplr().StartNewLvl(trigs[i]._tmsg, level.currlevel - 1);
			break;
		case WM_DIABRTNLVL:
			myplr().StartNewLvl(trigs[i]._tmsg, ReturnLvl);
			break;
		case WM_DIABTOWNWARP:
			if (gbMaxPlayers != 1) {
				abort = FALSE;

				if (trigs[i]._tlvl == 5 && myplr().data._pLevel < 8) {
					abort = TRUE;
					x = myplr().data._p.x;
					y = myplr().data._p.y + 1;
					abortflag = EMSG_REQUIRES_LVL_8;
				}

				if (trigs[i]._tlvl == 9 && myplr().data._pLevel < 13) {
					abort = TRUE;
					x = myplr().data._p.x + 1;
					y = myplr().data._p.y;
					abortflag = EMSG_REQUIRES_LVL_13;
				}

				if (trigs[i]._tlvl == 13 && myplr().data._pLevel < 17) {
					abort = TRUE;
					x = myplr().data._p.x;
					y = myplr().data._p.y + 1;
					abortflag = EMSG_REQUIRES_LVL_17;
				}

				if (abort) {
					if (myplr().data._pClass == PC_WARRIOR) {
						PlaySFX(PS_WARR43);
#ifndef SPAWN
					} else if (myplr().data._pClass == PC_ROGUE) {
						PlaySFX(PS_ROGUE43);
					} else if (myplr().data._pClass == PC_SORCERER) {
						PlaySFX(PS_MAGE43);
#endif
					}

					InitDiabloMsg(abortflag);
					NetSendCmdLoc(TRUE, CMD_WALKXY, { x, y });
					return;
				}
			}

			myplr().StartNewLvl(trigs[i]._tmsg, trigs[i]._tlvl);
			break;
		case WM_DIABTWARPUP:
			TWarpFrom = level.currlevel;
			myplr().StartNewLvl(trigs[i]._tmsg, 0);
			break;
		default:
			app_fatal("Unknown trigger msg");
			break;
		}
	}
}

DEVILUTION_END_NAMESPACE
