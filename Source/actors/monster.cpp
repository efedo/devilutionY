/**
 * @file monster.cpp
 *
 * Implementation of monster functionality, AI, actions, spawning, loading, etc.
 */
#include "all.h"
#include "../3rdParty/Storm/Source/storm.h"

DEVILUTION_BEGIN_NAMESPACE

/** Tracks which missile files are already loaded */
int MissileFileFlag;

// BUGFIX: replace monstkills[MAXMONSTERS] with monstkills[NUM_MTYPES].

Beastiary beastiary;
MonsterManager monsters;

int monstkills[MAXMONSTERS];
int monstactive[MAXMONSTERS];
int nummonsters;
BOOLEAN sgbSaveSoundOn;
//MonsterStruct monster[MAXMONSTERS];
int totalmonsters;
//CMonster monstTypes[MAX_LVLMTYPES];
//BYTE GraphicTable[NUMLEVELS][MAX_LVLMTYPES];
int monstimgtot;
int uniquetrans;
int nummtypes;

const char plr2monst[9] = { 0, 5, 3, 7, 1, 4, 6, 0, 2 };
const BYTE counsmiss[4] = { MIS_FIREBOLT, MIS_CBOLT, MIS_LIGHTCTRL, MIS_FIREBALL };

/* data */

int MWVel[24][3] = {
	{ 256, 512, 1024 },
	{ 128, 256, 512 },
	{ 85, 170, 341 },
	{ 64, 128, 256 },
	{ 51, 102, 204 },
	{ 42, 85, 170 },
	{ 36, 73, 146 },
	{ 32, 64, 128 },
	{ 28, 56, 113 },
	{ 26, 51, 102 },
	{ 23, 46, 93 },
	{ 21, 42, 85 },
	{ 19, 39, 78 },
	{ 18, 36, 73 },
	{ 17, 34, 68 },
	{ 16, 32, 64 },
	{ 15, 30, 60 },
	{ 14, 28, 57 },
	{ 13, 26, 54 },
	{ 12, 25, 51 },
	{ 12, 24, 48 },
	{ 11, 23, 46 },
	{ 11, 22, 44 },
	{ 10, 21, 42 }
};
char animletter[7] = "nwahds";
int left[8] = { 7, 0, 1, 2, 3, 4, 5, 6 };
int right[8] = { 1, 2, 3, 4, 5, 6, 7, 0 };
int opposite[8] = { 4, 5, 6, 7, 0, 1, 2, 3 };
int offset_x[8] = { 1, 0, -1, -1, -1, 0, 1, 1 };
int offset_y[8] = { 1, 1, 1, 0, -1, -1, -1, 0 };

void (MonsterInstance:: *AiProc[])() = {
	&MonsterInstance::MAI_Zombie,
	&MonsterInstance::MAI_Fat,
	&MonsterInstance::MAI_SkelSd,
	&MonsterInstance::MAI_SkelBow,
	&MonsterInstance::MAI_Scav,
	&MonsterInstance::MAI_Rhino,
	&MonsterInstance::MAI_GoatMc,
	&MonsterInstance::MAI_GoatBow,
	&MonsterInstance::MAI_Fallen,
	&MonsterInstance::MAI_Magma,
	&MonsterInstance::MAI_SkelKing,
	&MonsterInstance::MAI_Bat,
	&MonsterInstance::MAI_Garg,
	&MonsterInstance::MAI_Cleaver,
	&MonsterInstance::MAI_Succ,
	&MonsterInstance::MAI_Sneak,
	&MonsterInstance::MAI_Storm,
	&MonsterInstance::MAI_Fireman,
	&MonsterInstance::MAI_Garbud,
	&MonsterInstance::MAI_Acid,
	&MonsterInstance::MAI_AcidUniq,
	&MonsterInstance::MAI_Golum,
	&MonsterInstance::MAI_Zhar,
	&MonsterInstance::MAI_SnotSpil,
	&MonsterInstance::MAI_Snake,
	&MonsterInstance::MAI_Counselor,
	&MonsterInstance::MAI_Mega,
	&MonsterInstance::MAI_Diablo,
	&MonsterInstance::MAI_Lazurus,
	&MonsterInstance::MAI_Lazhelp,
	&MonsterInstance::MAI_Lachdanan,
	&MonsterInstance::MAI_Warlord,
};





BOOL MonstPlace(V2Di p)
{
	if (p.x < 0 || p.x >= MAXDUNX
	    || p.y < 0 || p.y >= MAXDUNY
	    || grid.at(p).dMonster
	    || grid.at(p).dPlayer) {
		return FALSE;
	}

	char f = grid.at(p).dFlags;
	if (f & BFLAG_VISIBLE) return FALSE;
	if (f & BFLAG_POPULATED) return FALSE;
	return !SolidLoc(p);
}



#ifndef SPAWN
void LoadDiabMonsts()
{
	BYTE *lpSetPiece;

	lpSetPiece = LoadFileInMem("Levels\\L4Data\\diab1.DUN", NULL);
	SetMapMonsters(lpSetPiece, 2 * diabquad1x, 2 * diabquad1y);
	mem_free_dbg(lpSetPiece);
	lpSetPiece = LoadFileInMem("Levels\\L4Data\\diab2a.DUN", NULL);
	SetMapMonsters(lpSetPiece, 2 * diabquad2x, 2 * diabquad2y);
	mem_free_dbg(lpSetPiece);
	lpSetPiece = LoadFileInMem("Levels\\L4Data\\diab3a.DUN", NULL);
	SetMapMonsters(lpSetPiece, 2 * diabquad3x, 2 * diabquad3y);
	mem_free_dbg(lpSetPiece);
	lpSetPiece = LoadFileInMem("Levels\\L4Data\\diab4a.DUN", NULL);
	SetMapMonsters(lpSetPiece, 2 * diabquad4x, 2 * diabquad4y);
	mem_free_dbg(lpSetPiece);
}
#endif

void InitMonsters()
{
	int na, nt;
	int i, s, t;
	int numplacemonsters;
	int mtype;
	int numscattypes;
	int scattertypes[NUM_MTYPES];

	numscattypes = 0;
	if (gbMaxPlayers != 1)
		CheckDungeonClear();
	if (!level.setlevel) {
		AddMonster({ 1, 0 }, 0, 0, FALSE);
		AddMonster({ 1, 0 }, 0, 0, FALSE);
		AddMonster({ 1, 0 }, 0, 0, FALSE);
		AddMonster({ 1, 0 }, 0, 0, FALSE);
	}
#ifndef SPAWN
	if (!level.setlevel && level.currlevel == 16)
		LoadDiabMonsts();
#endif
	nt = numtrigs;
	if (level.currlevel == 15)
		nt = 1;
	for (i = 0; i < nt; i++) {
		for (s = -2; s < 2; s++) {
			for (t = -2; t < 2; t++)
				DoVision(s + trigs[i]._t, 15, FALSE, FALSE);
		}
	}
#ifndef SPAWN
	monsters.PlaceQuestMonsters();
#endif
	if (!level.setlevel) {
#ifndef SPAWN
		PlaceUniques();
#endif
		na = 0;
		for (s = 16; s < 96; s++)
			for (t = 16; t < 96; t++)
				if (!SolidLoc(s, t))
					na++;
		numplacemonsters = na / 30;
		if (gbMaxPlayers != 1)
			numplacemonsters += numplacemonsters >> 1;
		if (nummonsters + numplacemonsters > 190)
			numplacemonsters = 190 - nummonsters;
		totalmonsters = nummonsters + numplacemonsters;
		for (i = 0; i < nummtypes; i++) {
			if (beastiary[i].data.mPlaceFlags & 1) {
				scattertypes[numscattypes] = i;
				numscattypes++;
			}
		}
		while (nummonsters < totalmonsters) {
			mtype = scattertypes[random_(95, numscattypes)];
			if (level.currlevel == 1 || random_(95, 2) == 0)
				na = 1;
			else if (level.currlevel == 2)
				na = random_(95, 2) + 2;
			else
				na = random_(95, 3) + 3;
			monsters.PlaceGroup(mtype, na, 0, 0);
		}
	}
	for (i = 0; i < nt; i++) {
		for (s = -2; s < 2; s++) {
			for (t = -2; t < 2; t++)
				DoUnVision({ s + trigs[i]._t.x, t + trigs[i]._t.y }, 15);
		}
	}
}

#ifndef SPAWN
void PlaceUniques()
{
	int u, mt;
	BOOL done;

	for (u = 0; UniqMonst[u].mtype != -1; u++) {
		if (UniqMonst[u].mlevel != level.currlevel)
			continue;
		done = FALSE;
		for (mt = 0; mt < nummtypes; mt++) {
			if (done)
				break;
			done = (beastiary[mt].data.mtype == UniqMonst[u].mtype);
		}
		mt--;
		if (u == UMT_GARBUD && quests[Q_GARBUD]._qactive == QUEST_NOTAVAIL)
			done = FALSE;
		if (u == UMT_ZHAR && quests[Q_ZHAR]._qactive == QUEST_NOTAVAIL)
			done = FALSE;
		if (u == UMT_SNOTSPIL && quests[Q_LTBANNER]._qactive == QUEST_NOTAVAIL)
			done = FALSE;
		if (u == UMT_LACHDAN && quests[Q_VEIL]._qactive == QUEST_NOTAVAIL)
			done = FALSE;
		if (u == UMT_WARLORD && quests[Q_WARLORD]._qactive == QUEST_NOTAVAIL)
			done = FALSE;
		if (done)
			monsters.PlaceUniqueMonster(u, mt, 8);
	}
}

void SetMapMonsters(BYTE *pMap, int startx, int starty)
{
	WORD rw, rh;
	WORD *lm;
	int i, j;
	int mtype;

	beastiary.AddMonsterType(MT_GOLEM, 2);
	AddMonster({ 1, 0 }, 0, 0, FALSE);
	AddMonster({ 1, 0 }, 0, 0, FALSE);
	AddMonster({ 1, 0 }, 0, 0, FALSE);
	AddMonster({ 1, 0 }, 0, 0, FALSE);
	if (level.setlevel && level.setlvlnum == SL_VILEBETRAYER) {
		beastiary.AddMonsterType(UniqMonst[UMT_LAZURUS].mtype, 4);
		beastiary.AddMonsterType(UniqMonst[UMT_RED_VEX].mtype, 4);
		beastiary.AddMonsterType(UniqMonst[UMT_BLACKJADE].mtype, 4);
		monsters.PlaceUniqueMonster(UMT_LAZURUS, 0, 0);
		monsters.PlaceUniqueMonster(UMT_RED_VEX, 0, 0);
		monsters.PlaceUniqueMonster(UMT_BLACKJADE, 0, 0);
	}
	lm = (WORD *)pMap;
	rw = SDL_SwapLE16(*lm);
	lm++;
	rh = SDL_SwapLE16(*lm);
	lm += (rw * rh + 1);
	rw = rw << 1;
	rh = rh << 1;
	lm += rw * rh;

	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*lm) {
				mtype = beastiary.AddMonsterType(MonstConvTbl[SDL_SwapLE16(*lm) - 1], 2);
				monsters.PlaceMonster(nummonsters++, mtype, { i + startx + 16, j + starty + 16 });
			}
			lm++;
		}
	}
}
#endif

int AddMonster(V2Di pos, int dir, int mtype, BOOL InMap)
{
	if (nummonsters < MAXMONSTERS) {
		int i = monstactive[nummonsters++];
		if (InMap) grid.at(pos).dMonster = i + 1;
		monsters[i] = MonsterInstance(i, dir, mtype, pos);
		return i;
	}

	return -1;
}



void M2MStartHit(int mid, int i, int dam)
{
	if ((DWORD)mid >= MAXMONSTERS) {
		app_fatal("Invalid monster %d getting hit by monster", mid);
	}

	if (monsters[mid].data.MType == NULL) {
		app_fatal("Monster %d \"%s\" getting hit by monster: MType NULL", mid, monsters[mid].data.mName);
	}

	if (i >= 0)
		monsters[i].data.mWhoHit |= 1 << i;

	delta_monster_hp(mid, monsters[mid].data._mhitpoints, level.currlevel);
	NetSendCmdParam2(FALSE, CMD_MONSTDAMAGE, mid, dam);
	PlayEffect(mid, 1);

	if (monsters[mid].data.MType->mtype >= MT_SNEAK && monsters[mid].data.MType->mtype <= MT_ILLWEAV || dam >> 6 >= monsters[mid].data.mLevel + 3) {
		if (i >= 0)
			monsters[mid].data._mdir = (monsters[i].data._mdir - 4) & 7;

		if (monsters[mid].data.MType->mtype == MT_BLINK) {
			monsters[mid].M_Teleport();
		} else if (monsters[mid].data.MType->mtype >= MT_NSCAV && monsters[mid].data.MType->mtype <= MT_YSCAV) {
			monsters[mid].data._mgoal = MGOAL_NORMAL;
		}

		if (monsters[mid].data._mmode != MM_STONE) {
			if (monsters[mid].data.MType->mtype != MT_GOLEM) {
				monsters[mid].NewMonsterAnim(&monsters[mid].data.MType->Anims[MA_GOTHIT], monsters[mid].data._mdir);
				monsters[mid].data._mmode = MM_GOTHIT;
			}

			monsters[mid].PlantInPosition(monsters[mid].data._mdir, &monsters[mid].data._mold);
			monsters[mid].M_ClearSquares();
			grid.at(monsters[mid].data._m).dMonster = mid + 1;
		}
	}
}



void MonsterInstance::M_UpdateLeader()
{
	int ma, j;
	for (j = 0; j < nummonsters; j++) {
		ma = monstactive[j];
		if (monsters[ma].data.leaderflag == 1 && monsters[ma].data.leader == i)
			monsters[ma].data.leaderflag = 0;
	}

	if (data.leaderflag == 1) {
		monsters[data.leader].data.packsize--;
	}
}

void DoEnding()
{
	BOOL bMusicOn;
	int musicVolume;

	if (gbMaxPlayers > 1) {
		SNetLeaveGame(0x40000004);
	}

	music_stop();

	if (gbMaxPlayers > 1) {
		SDL_Delay(1000);
	}

#ifndef SPAWN
	if (myplr().data._pClass == PC_WARRIOR) {
		play_movie("gendata\\DiabVic2.smk", FALSE);
	} else if (myplr().data._pClass == PC_SORCERER) {
		play_movie("gendata\\DiabVic1.smk", FALSE);
	} else {
		play_movie("gendata\\DiabVic3.smk", FALSE);
	}
	play_movie("gendata\\Diabend.smk", FALSE);

	bMusicOn = gbMusicOn;
	gbMusicOn = TRUE;

	musicVolume = sound_get_or_set_music_volume(1);
	sound_get_or_set_music_volume(0);

	music_start(TMUSIC_L2);
	loop_movie = TRUE;
	play_movie("gendata\\loopdend.smk", TRUE);
	loop_movie = FALSE;
	music_stop();

	sound_get_or_set_music_volume(musicVolume);
	gbMusicOn = bMusicOn;
#endif
}

void PrepDoEnding()
{
	int newKillLevel, i;
	DWORD *killLevel;

	gbSoundOn = sgbSaveSoundOn;
	gbRunGame = FALSE;
	deathflag = FALSE;
	cineflag = TRUE;

	killLevel = &myplr().data.pDiabloKillLevel;
	newKillLevel = gnDifficulty + 1;
	if (*killLevel > newKillLevel)
		newKillLevel = *killLevel;
	myplr().data.pDiabloKillLevel = newKillLevel;

	for (i = 0; i < MAX_PLRS; i++) {
		plr[i].data._pmode = PM_QUIT;
		plr[i].data._pInvincible = TRUE;
		if (gbMaxPlayers > 1) {
			if (plr[i].data._pHitPoints >> 6 == 0)
				plr[i].data._pHitPoints = 64;
			if (plr[i].data._pMana >> 6 == 0)
				plr[i].data._pMana = 64;
		}
	}
}

BOOL MonsterInstance::M_DoDeath()
{
	int var1;
	int x, y;
	data._mVar1++;
	var1 = data._mVar1;
	if (data.MType->mtype == MT_DIABLO) {
		x = data._m.x - View.x;
		if (x < 0)
			x = -1;
		else
			x = x > 0;
		View.x += x;

		y = data._m.y - View.y;
		if (y < 0) {
			y = -1;
		} else {
			y = y > 0;
		}
		View.y += y;

		if (var1 == 140)
			PrepDoEnding();
	} else if (data._mAnimFrame == data._mAnimLen) {
		if (data._uniqtype == 0)
			AddDead(data._m, data.MType->mdeadval, (direction)data._mdir);
		else
			AddDead(data._m, data._udeadval, (direction)data._mdir);

		grid.at(data._m).dMonster = 0;
		data._mDelFlag = TRUE;

		M_UpdateLeader();
	}
	return FALSE;
}

BOOL MonsterInstance::M_DoSpStand()
{
	if (data._mAnimFrame == data.MData->mAFNum2)
		PlayEffect(i, 3);

	if (data._mAnimFrame == data._mAnimLen) {
		M_StartStand(data._mdir);
		return TRUE;
	}
	return FALSE;
}

BOOL MonsterInstance::M_DoDelay()
{
	int mVar2;
	int oFrame;
	data._mAnimData = data.MType->Anims[MA_STAND].Data[M_GetDir()];
	if (data._mAi == AI_LAZURUS) {
		if (data._mVar2 > 8 || data._mVar2 < 0)
			data._mVar2 = 8;
	}

	mVar2 = data._mVar2;
	data._mVar2--;

	if (!mVar2) {
		oFrame = data._mAnimFrame;
		M_StartStand(data._mdir);
		data._mAnimFrame = oFrame;
		return TRUE;
	}
	return FALSE;
}

BOOL MonsterInstance::M_DoStone()
{
	if (!data._mhitpoints) {
		grid.at(data._m).dMonster = 0;
		data._mDelFlag = TRUE;
	}
	return FALSE;
}

void MonsterInstance::M_WalkDir(int md)
{
	int mwi = data.MType->Anims[MA_WALK].Frames - 1;
	switch (md) {
	case DIR_N:
		M_StartWalk({ 0, -MWVel[mwi][1] }, { -1, -1 }, DIR_N);
		break;
	case DIR_NE:
		M_StartWalk({ MWVel[mwi][1], -MWVel[mwi][0] }, { 0, -1 }, DIR_NE);
		break;
	case DIR_E:
		M_StartWalk3({ MWVel[mwi][2], 0 }, { -32, -16 }, { 1, -1 }, { 1, 0 }, DIR_E);
		break;
	case DIR_SE:
		M_StartWalk2({ MWVel[mwi][1], MWVel[mwi][0] }, { -32, -16 }, { 1, 0 }, DIR_SE);
		break;
	case DIR_S:
		M_StartWalk2({ 0, MWVel[mwi][1] }, { 0, -32 }, { 1, 1 }, DIR_S);
		break;
	case DIR_SW:
		M_StartWalk2({ -MWVel[mwi][1], MWVel[mwi][0] }, { 32, -16 }, { 0, 1 }, DIR_SW);
		break;
	case DIR_W:
		M_StartWalk3({ -MWVel[mwi][2], 0 }, { 32, -16 }, { -1, 1 }, { 0, 1 }, DIR_W);
		break;
	case DIR_NW:
		M_StartWalk({ -MWVel[mwi][1], -MWVel[mwi][0] }, { -1, 0 }, DIR_NW);
		break;
	}
}

void MonsterInstance::GroupUnity()
{
	int leader, m, j;
	BOOL clear;
	if (data.leaderflag) {
		leader = data.leader;
		clear = LineClearF(CheckNoSolid, data._m, monsters[leader].data._mfut);
		if (clear || data.leaderflag != 1) {
			V2Di d = abs(data._m - monsters[leader].data._mfut);
			if (clear
			    && data.leaderflag == 2
			    && d.maxdim() < 4) {
				monsters[leader].data.packsize++;
				data.leaderflag = 1;
			}
		} else {
			monsters[leader].data.packsize--;
			data.leaderflag = 2;
		}
	}

	if (data.leaderflag == 1) {
		if (data._msquelch > monsters[leader].data._msquelch) {
			monsters[leader].data._last = data._m;
			monsters[leader].data._msquelch = data._msquelch - 1;
		}
		if (monsters[leader].data._mAi == AI_GARG) {
			if (monsters[leader].data._mFlags & MFLAG_ALLOW_SPECIAL) {
				monsters[leader].data._mmode = MM_SATTACK;
				monsters[leader].data._mFlags &= ~MFLAG_ALLOW_SPECIAL;
			}
		}
		return;
	}
	if (data._uniqtype != 0) {
		if (UniqMonst[data._uniqtype - 1].mUnqAttr & 2) {
			for (j = 0; j < nummonsters; j++) {
				m = monstactive[j];
				if (monsters[m].data.leaderflag == 1 && monsters[m].data.leader == i) {
					if (data._msquelch > monsters[m].data._msquelch) {
						monsters[m].data._last = data._m;
						monsters[m].data._msquelch = data._msquelch - 1;
					}
					if (monsters[m].data._mAi == AI_GARG) {
						if (monsters[m].data._mFlags & MFLAG_ALLOW_SPECIAL) {
							monsters[m].data._mmode = MM_SATTACK;
							monsters[m].data._mFlags &= ~MFLAG_ALLOW_SPECIAL;
						}
					}
				}
			}
		}
	}
}

BOOL MonsterInstance::M_CallWalk(int md)
{
	int mdtemp = md;
	BOOL ok = DirOK(md);
	if (random_(101, 2))
		ok = ok || (md = left[mdtemp], DirOK(md)) || (md = right[mdtemp], DirOK(md));
	else
		ok = ok || (md = right[mdtemp], DirOK(md)) || (md = left[mdtemp], DirOK(md));
	if (random_(102, 2))
		ok = ok
		    || (md = right[right[mdtemp]], DirOK(md))
		    || (md = left[left[mdtemp]], DirOK(md));
	else
		ok = ok
		    || (md = left[left[mdtemp]], DirOK(md))
		    || (md = right[right[mdtemp]], DirOK(md));
	if (ok)
		M_WalkDir(md);
	return ok;
}

BOOL MonsterInstance::M_PathWalk()
{
	char path[25];
	BOOL(* Check)(int, V2Di) = PosOkMonst3;
	if (!(data._mFlags & MFLAG_CAN_OPEN_DOOR))
		Check = PosOkMonst;

	if (FindPath(Check, i, data._m, data._menemypos, path)) {
		M_CallWalk(plr2monst[path[0]]); /* plr2monst is local */
		return TRUE;
	}

	return FALSE;
}

BOOL MonsterInstance::M_CallWalk2(int md)
{
	BOOL ok = DirOK(md); // Can we continue in the same direction
	int mdtemp = md;
	if (random_(101, 2)) { // Randomly go left or right
		ok = ok || (mdtemp = left[md], DirOK(left[md])) || (mdtemp = right[md], DirOK(right[md]));
	} else {
		ok = ok || (mdtemp = right[md], DirOK(right[md])) || (mdtemp = left[md], DirOK(left[md]));
	}

	if (ok)
		M_WalkDir(mdtemp);
	return ok;
}

BOOL MonsterInstance::M_DumbWalk(int md)
{
	BOOL ok = DirOK(md);
	if (ok) M_WalkDir(md);
	return ok;
}

BOOL MonsterInstance::M_RoundWalk(int md, int *dir)
{
	int mdtemp = md;
	BOOL ok = DirOK(md);
	if (*dir)
		md = left[left[md]];
	else
		md = right[right[md]];

	if (!ok) {
		if (*dir) {
			md = right[mdtemp];
			ok = DirOK(md) || (md = right[right[mdtemp]], DirOK(md));
		} else {
			md = left[mdtemp];
			ok = (DirOK(md) || (md = left[left[mdtemp]], DirOK(md)));
		}
	}
	if (ok) {
		M_WalkDir(md);
	} else {
		*dir = !*dir;
		ok = M_CallWalk(opposite[mdtemp]);
	}
	return ok;
}

void MonsterInstance::MAI_Zombie()
{
	if (data._mmode != MM_STAND) {
		return;
	}

	V2Di m = data._m;
	if (!(grid.at(m).dFlags & BFLAG_VISIBLE)) {
		return;
	}

	m = abs(m - data._menemypos);
	int md = data._mdir;
	int v = random_(103, 100);
	if (m.maxdim() >= 2) {
		if (v < 2 * data._mint + 10) {
			if (m.x >= 2 * data._mint + 4 || m.y >= 2 * data._mint + 4) {
				if (random_(104, 100) < 2 * data._mint + 20) {
					md = random_(104, 8);
				}
				M_DumbWalk(md);
			} else {
				md = M_GetDir();
				M_CallWalk(md);
			}
		}
	} else if (v < 2 * data._mint + 10) {
		M_StartAttack();
	}

	if (data._mmode == MM_STAND)
		data._mAnimData = data.MType->Anims[MA_STAND].Data[data._mdir];
}

void MonsterInstance::MAI_SkelSd()
{
	if (data._mmode != MM_STAND || data._msquelch == 0) {
		return;
	}

	V2Di m = data._m;
	V2Di d = abs(m - data._menemypos);
	int md = GetDirection(m, data._last);
	data._mdir = md;
	if (d.maxdim() >= 2) {
		if (data._mVar1 == MM_DELAY || (random_(106, 100) >= 35 - 4 * data._mint)) {
			M_CallWalk(md);
		} else {
			M_StartDelay(15 - 2 * data._mint + random_(106, 10));
		}
	} else {
		if (data._mVar1 == MM_DELAY || (random_(105, 100) < 2 * data._mint + 20)) {
			M_StartAttack();
		} else {
			M_StartDelay(2 * (5 - data._mint) + random_(105, 10));
		}
	}

	if (data._mmode == MM_STAND)
		data._mAnimData = data.MType->Anims[MA_STAND].Data[md];
}

BOOL MonsterInstance::MAI_Path()
{
	if (data.MType->mtype != MT_GOLEM) {
		if (data._msquelch == 0)
			return FALSE;
		if (data._mmode != MM_STAND)
			return FALSE;
		if (data._mgoal != MGOAL_NORMAL && data._mgoal != MGOAL_MOVE && data._mgoal != MGOAL_SHOOT)
			return FALSE;
		if (data._m.x == 1 && data._m.y == 0)
			return FALSE;
	}

	BOOL clear = LineClearF1(PosOkMonst2, i, data._m, data._menemypos);
	if (!clear || data._pathcount >= 5 && data._pathcount < 8) {
		if (data._mFlags & MFLAG_CAN_OPEN_DOOR)
			MonstCheckDoors(i);
		data._pathcount++;
		if (data._pathcount < 5)
			return FALSE;
		if (M_PathWalk())
			return TRUE;
	}

	if (data.MType->mtype != MT_GOLEM)
		data._pathcount = 0;

	return FALSE;
}

void MonsterInstance::MAI_Snake()
{
	int md, tmp;

	char pattern[6] = { 1, 1, 0, -1, -1, 0 };
	int pnum = data._menemy;
	if (data._mmode != MM_STAND || data._msquelch == 0)
		return;
	V2Di f = data._menemypos;
	V2Di d = abs(data._m - f);
	md = GetDirection(data._m, data._last);
	data._mdir = md;
	if (d.x >= 2 || d.y >= 2) {
		if (d.x < 3 && d.y < 3 && LineClearF1(PosOkMonst, i, data._m, f) && data._mVar1 != MM_CHARGE) {
			if (AddMissile(data._m, f, md, MIS_RHINO, pnum, i, 0, 0) != -1) {
				PlayEffect(i, 0);
				grid.at(data._m).dMonster = -(i + 1);
				data._mmode = MM_CHARGE;
			}
		} else if (data._mVar1 == MM_DELAY || random_(106, 100) >= 35 - 2 * data._mint) {
			if (md + pattern[data._mgoalvar1] < 0) {
				tmp = md + pattern[data._mgoalvar1] + 8;
			} else {
				tmp = md + pattern[data._mgoalvar1] - 8;
				if (md + pattern[data._mgoalvar1] < 8)
					tmp = md + pattern[data._mgoalvar1];
			}
			data._mgoalvar1++;
			if (data._mgoalvar1 > 5)
				data._mgoalvar1 = 0;
			if (tmp - data._mgoalvar2 < 0) {
				md = tmp - data._mgoalvar2 + 8;
			} else if (tmp - data._mgoalvar2 >= 8) {
				md = tmp - data._mgoalvar2 - 8;
			} else
				md = tmp - data._mgoalvar2;
			if (md > 0) {
				if (md < 4) {
					if (data._mgoalvar2 + 1 < 0) {
						md = data._mgoalvar2 + 9;
					} else if (data._mgoalvar2 + 1 >= 8) {
						md = data._mgoalvar2 - 7;
					} else
						md = data._mgoalvar2 + 1;
					data._mgoalvar2 = md;
				} else if (md == 4) {
					data._mgoalvar2 = tmp;
				} else {
					if (data._mgoalvar2 - 1 < 0) {
						md = data._mgoalvar2 + 7;
					} else if (data._mgoalvar2 - 1 >= 8) {
						md = data._mgoalvar2 - 9;
					} else
						md = data._mgoalvar2 - 1;
					data._mgoalvar2 = md;
				}
			}
			if (!M_DumbWalk(data._mgoalvar2))
				M_CallWalk2(data._mdir);
		} else {
			M_StartDelay(15 - data._mint + random_(106, 10));
		}
	} else {
		if (data._mVar1 == MM_DELAY
		    || data._mVar1 == MM_CHARGE
		    || (random_(105, 100) < data._mint + 20)) {
			M_StartAttack();
		} else
			M_StartDelay(10 - data._mint + random_(105, 10));
	}
	if (data._mmode == MM_STAND)
		data._mAnimData = data.MType->Anims[MA_STAND].Data[data._mdir];
}

void MonsterInstance::MAI_Bat()
{
	int pnum = data._menemy;
	if (data._mmode != MM_STAND || data._msquelch == 0) {
		return;
	}

	V2Di d = abs(data._m - data._menemypos);
	int md = GetDirection(data._m, data._last);
	data._mdir = md;
	int v = random_(107, 100);
	if (data._mgoal == MGOAL_RETREAT) {
		if (!data._mgoalvar1) {
			M_CallWalk(opposite[md]);
			data._mgoalvar1++;
		} else {
			if (random_(108, 2))
				M_CallWalk(left[md]);
			else
				M_CallWalk(right[md]);
			data._mgoal = MGOAL_NORMAL;
		}
		return;
	}

	V2Di f = data._menemypos;
	if (data.MType->mtype == MT_GLOOM
	    && (d.x >= 5 || d.y >= 5)
	    && v < 4 * data._mint + 33
	    && LineClearF1(PosOkMonst, i, data._m, f)) {
		if (AddMissile(data._m, f, md, MIS_RHINO, pnum, i, 0, 0) != -1) {
			grid.at(data._m).dMonster = -(i + 1);
			data._mmode = MM_CHARGE;
		}
	} else if (d.maxdim() >= 2) {
		if (data._mVar2 > 20 && v < data._mint + 13
		    || (data._mVar1 == MM_WALK || data._mVar1 == MM_WALK2 || data._mVar1 == MM_WALK3)
		        && data._mVar2 == 0
		        && v < data._mint + 63) {
			M_CallWalk(md);
		}
	} else if (v < 4 * data._mint + 8) {
		M_StartAttack();
		data._mgoal = MGOAL_RETREAT;
		data._mgoalvar1 = 0;
		if (data.MType->mtype == MT_FAMILIAR) {
			AddMissile(data._menemypos, { data._menemypos.x + 1, 0 }, -1, MIS_LIGHTNING, 1, i, random_(109, 10) + 1, 0);
		}
	}

	if (data._mmode == MM_STAND)
		data._mAnimData = data.MType->Anims[MA_STAND].Data[md];
}

void MonsterInstance::MAI_SkelBow()
{
	BOOL walking = FALSE;
	if (data._mmode != MM_STAND || data._msquelch == 0) {
		return;
	}

	V2Di d = abs(data._m - data._menemypos);
	int md = M_GetDir();
	data._mdir = md;
	int v = random_(110, 100);

	if (d.maxdim() < 4) {
		if (data._mVar2 > 20 && v < 2 * data._mint + 13
		    || (data._mVar1 == MM_WALK || data._mVar1 == MM_WALK2 || data._mVar1 == MM_WALK3)
		        && data._mVar2 == 0
		        && v < 2 * data._mint + 63) {
			walking = M_DumbWalk(opposite[md]);
		}
	}

	V2Di m = data._menemypos;
	if (!walking) {
		if (random_(110, 100) < 2 * data._mint + 3) {
			if (LineClear(data._m, m))
				M_StartRAttack(MIS_ARROW, 4);
		}
	}

	if (data._mmode == MM_STAND)
		data._mAnimData = data.MType->Anims[MA_STAND].Data[md];
}

void MonsterInstance::MAI_Fat()
{
	if (data._mmode != MM_STAND || data._msquelch == 0) {
		return;
	}

	V2Di d = abs(data._m - data._menemypos);
	int md = M_GetDir();
	data._mdir = md;
	int v = random_(111, 100);
	if (d.maxdim() >= 2) {
		if (data._mVar2 > 20 && v < 4 * data._mint + 20
		    || (data._mVar1 == MM_WALK || data._mVar1 == MM_WALK2 || data._mVar1 == MM_WALK3)
		        && data._mVar2 == 0
		        && v < 4 * data._mint + 70) {
			M_CallWalk(md);
		}
	} else if (v < 4 * data._mint + 15) {
		M_StartAttack();
	} else if (v < 4 * data._mint + 20) {
		M_StartSpAttack();
	}

	if (data._mmode == MM_STAND)
		data._mAnimData = data.MType->Anims[MA_STAND].Data[md];
}

void MonsterInstance::MAI_Sneak()
{
	if (data._mmode == MM_STAND) {
		V2Di mb = data._m;
		if (grid.at(mb).dLight != lightmax) {
			V2Di d = abs(mb - data._menemypos);
			int md = M_GetDir();
			int dist = 5 - data._mint;
			if (data._mVar1 == MM_GOTHIT) {
				data._mgoalvar1 = 0;
				data._mgoal = MGOAL_RETREAT;
			} else {
				if (d.maxdim() >= dist + 3 || data._mgoalvar1 > 8) {
					data._mgoalvar1 = 0;
					data._mgoal = MGOAL_NORMAL;
				}
			}
			if (data._mgoal == MGOAL_RETREAT) {
				if (data._mFlags & MFLAG_TARGETS_MONSTER)
					md = GetDirection(data._m, plr[data._menemy].data._powner);
				md = opposite[md];
				if (data.MType->mtype == MT_UNSEEN) {
					if (random_(112, 2))
						md = left[md];
					else
						md = right[md];
				}
			}
			data._mdir = md;
			int v = random_(112, 100);
			if (d.maxdim() < dist && data._mFlags & MFLAG_HIDDEN) {
				M_StartFadein(md, FALSE);
			} else {
				if ((d.maxdim() >= dist + 1) && !(data._mFlags & MFLAG_HIDDEN)) {
					M_StartFadeout(md, TRUE);
				} else {
					if (data._mgoal == MGOAL_RETREAT
					    || (d.maxdim() >= 2) && (data._mVar2 > 20 && v < 4 * data._mint + 14 || (data._mVar1 == MM_WALK || data._mVar1 == MM_WALK2 || data._mVar1 == MM_WALK3) && data._mVar2 == 0 && v < 4 * data._mint + 64)) {
						data._mgoalvar1++;
						M_CallWalk(md);
					}
				}
			}
			if (data._mmode == MM_STAND) {
				if (d.maxdim() >= 2 || v >= 4 * data._mint + 10)
					data._mAnimData = data.MType->Anims[MA_STAND].Data[md];
				else
					M_StartAttack();
			}
		}
	}
}

void MonsterInstance::MAI_Fireman()
{
	int md, pnum;
	if (data._mmode != MM_STAND || data._msquelch == 0)
		return;

	pnum = data._menemy;
	V2Di f = data._menemypos;
	V2Di d = abs(data._m - f);

	md = M_GetDir();
	if (data._mgoal == MGOAL_NORMAL) {
		if (LineClear(data._m, f)
		    && AddMissile(data._m, f, md, MIS_FIREMAN, pnum, i, 0, 0) != -1) {
			data._mmode = MM_CHARGE;
			data._mgoal = MGOAL_SHOOT;
			data._mgoalvar1 = 0;
		}
	} else if (data._mgoal == MGOAL_SHOOT) {
		if (data._mgoalvar1 == 3) {
			data._mgoal = MGOAL_NORMAL;
			M_StartFadeout(md, TRUE);
		} else if (LineClear(data._m, f)) {
			M_StartRAttack(MIS_KRULL, 4);
			data._mgoalvar1++;
		} else {
			M_StartDelay(random_(112, 10) + 5);
			data._mgoalvar1++;
		}
	} else if (data._mgoal == MGOAL_RETREAT) {
		M_StartFadein(md, FALSE);
		data._mgoal = MGOAL_SHOOT;
	}
	data._mdir = md;
	random_(112, 100);
	if (data._mmode != MM_STAND)
		return;

	if (d.maxdim() < 2 && data._mgoal == MGOAL_NORMAL) {
		M_TryH2HHit(data._menemy, data.mHit, data.mMinDamage, data.mMaxDamage);
		data._mgoal = MGOAL_RETREAT;
		if (!M_CallWalk(opposite[md])) {
			M_StartFadein(md, FALSE);
			data._mgoal = MGOAL_SHOOT;
		}
	} else if (!M_CallWalk(md) && (data._mgoal == MGOAL_NORMAL || data._mgoal == MGOAL_RETREAT)) {
		M_StartFadein(md, FALSE);
		data._mgoal = MGOAL_SHOOT;
	}
}

void MonsterInstance::MAI_Fallen()
{
	int m, rad, md;
	if (data._mgoal == MGOAL_SHOOT) {
		if (data._mgoalvar1)
			data._mgoalvar1--;
		else
			data._mgoal = MGOAL_NORMAL;
	}
	if (data._mmode != MM_STAND || data._msquelch == 0) {
		return;
	}

	if (data._mgoal == MGOAL_RETREAT) {
		if (!data._mgoalvar1--) {
			data._mgoal = MGOAL_NORMAL;
			M_StartStand(opposite[data._mdir]);
		}
	}

	if (data._mAnimFrame == data._mAnimLen) {
		if (random_(113, 4)) {
			return;
		}
		if (!(data._mFlags & MFLAG_NOHEAL)) {
			M_StartSpStand(data._mdir);
			rad = 2 * data._mint + 2;
			if (data._mmaxhp - rad >= data._mhitpoints)
				data._mhitpoints = rad + data._mhitpoints;
			else
				data._mhitpoints = data._mmaxhp;
		}
		rad = 2 * data._mint + 4;
		for (int y = -rad; y <= rad; y++) {
			for (int x = -rad; x <= rad; x++) {
				if (y >= 0 && y < MAXDUNY && x >= 0 && x < MAXDUNX) {
					m = grid[x + data._m.x][y + data._m.y].dMonster;
					if (m > 0) {
						m--;
						if (monsters[m].data._mAi == AI_FALLEN) {
							monsters[m].data._mgoal = MGOAL_SHOOT;
							monsters[m].data._mgoalvar1 = 30 * data._mint + 105;
						}
					}
				}
			}
		}
	} else if (data._mgoal == MGOAL_RETREAT) {
		md = data._mdir;
		M_CallWalk(md);
	} else if (data._mgoal == MGOAL_SHOOT) {
		V2Di d = abs(data._m - data._menemypos);
		if (d.maxdim() < 2) {
			M_StartAttack();
		} else {
			md = M_GetDir();
			M_CallWalk(md);
		}
	} else {
		MAI_SkelSd();
	}
}

void MonsterInstance::MAI_Cleaver()
{
	if (data._mmode != MM_STAND || data._msquelch == 0) {
		return;
	}

	V2Di m = data._m;
	V2Di d = abs(m - data._menemypos);

	int md = GetDirection(m, data._last);
	data._mdir = md;

	if (d.maxdim() >= 2)
		M_CallWalk(md);
	else
		M_StartAttack();

	if (data._mmode == MM_STAND)
		data._mAnimData = data.MType->Anims[MA_STAND].Data[md];
}

void MonsterInstance::MAI_Round(BOOL special)
{
	if (data._mmode == MM_STAND && data._msquelch != 0) {
		V2Di f = data._menemypos;
		V2Di d = abs(data._m - f);
		int dist = d.maxdim();
		int md = GetDirection(data._m, data._last);
		if (data._msquelch < UCHAR_MAX)
			MonstCheckDoors(i);
		int v = random_(114, 100);
		if ((dist >= 2) && data._msquelch == UCHAR_MAX && grid.at(data._m).dTransVal == grid.at(f).dTransVal) {
			if (data._mgoal == MGOAL_MOVE || (dist >= 4) && random_(115, 4) == 0) {
				if (data._mgoal != MGOAL_MOVE) {
					data._mgoalvar1 = 0;
					data._mgoalvar2 = random_(116, 2);
				}
				data._mgoal = MGOAL_MOVE;
				if (data._mgoalvar1++ >= 2 * dist && DirOK(md) || grid.at(data._m).dTransVal != grid.at(f).dTransVal) {
					data._mgoal = MGOAL_NORMAL;
				} else if (!M_RoundWalk(md, &data._mgoalvar2)) {
					M_StartDelay(random_(125, 10) + 10);
				}
			}
		} else
			data._mgoal = MGOAL_NORMAL;
		if (data._mgoal == MGOAL_NORMAL) {
			if (dist >= 2) {
				if (data._mVar2 > 20 && v < 2 * data._mint + 28
				    || (data._mVar1 == MM_WALK || data._mVar1 == MM_WALK2 || data._mVar1 == MM_WALK3)
				        && data._mVar2 == 0
				        && v < 2 * data._mint + 78) {
					M_CallWalk(md);
				}
			} else if (v < 2 * data._mint + 23) {
				data._mdir = md;
				if (special && data._mhitpoints < (data._mmaxhp >> 1) && random_(117, 2) != 0)
					M_StartSpAttack();
				else
					M_StartAttack();
			}
		}
		if (data._mmode == MM_STAND)
			data._mAnimData = data.MType->Anims[MA_STAND].Data[md];
	}
}

void MonsterInstance::MAI_GoatMc()
{
	MAI_Round(TRUE);
}

void MonsterInstance::MAI_Ranged(int missile_type, BOOL special)
{
	if (data._mmode != MM_STAND) {
		return;
	}

	if (data._msquelch == UCHAR_MAX || data._mFlags & MFLAG_TARGETS_MONSTER) {
		V2Di f = data._menemypos;
		V2Di d = abs(data._m - f);
		int md = M_GetDir();
		if (data._msquelch < UCHAR_MAX)
			MonstCheckDoors(i);
		data._mdir = md;
		if (data._mVar1 == MM_RATTACK) {
			M_StartDelay(random_(118, 20));
		} else if (d.maxdim() < 4) {
			if (random_(119, 100) < 10 * (data._mint + 7))
				M_CallWalk(opposite[md]);
		}
		if (data._mmode == MM_STAND) {
			if (LineClear(data._m, f)) {
				if (special)
					M_StartRSpAttack(missile_type, 4);
				else
					M_StartRAttack(missile_type, 4);
			} else {
				data._mAnimData = data.MType->Anims[MA_STAND].Data[md];
			}
		}
	} else if (data._msquelch != 0) {
		int md = GetDirection(data._m, data._last);
		M_CallWalk(md);
	}
}

void MonsterInstance::MAI_GoatBow()
{
	MAI_Ranged(MIS_ARROW, FALSE);
}

void MonsterInstance::MAI_Succ()
{
	MAI_Ranged(MIS_FLARE, FALSE);
}

void MonsterInstance::MAI_AcidUniq()
{
	MAI_Ranged(MIS_ACID, TRUE);
}

void MonsterInstance::MAI_Scav()
{
	int x, y;
	V2Di _m = data._m;
	BOOL done = FALSE;
	if (data._mmode != MM_STAND)
		return;
	if (data._mhitpoints < (data._mmaxhp >> 1) && data._mgoal != MGOAL_HEALING) {
		if (data.leaderflag) {
			monsters[data.leader].data.packsize--;
			data.leaderflag = 0;
		}
		data._mgoal = MGOAL_HEALING;
		data._mgoalvar3 = 10;
	}
	if (data._mgoal == MGOAL_HEALING && data._mgoalvar3 != 0) {
		data._mgoalvar3--;
		if (grid.at(data._m).dDead) {
			M_StartEat();
			if (!(data._mFlags & MFLAG_NOHEAL))
				data._mhitpoints += 64;
			if (data._mhitpoints >= (data._mmaxhp >> 1) + (data._mmaxhp >> 2)) {
				data._mgoal = MGOAL_NORMAL;
				data._mgoalvar1 = 0;
				data._mgoalvar2 = 0;
			}
		} else {
			if (data._mgoalvar1 == 0) {
				if (random_(120, 2) != 0) {
					for (y = -4; y <= 4 && !done; y++) {
						for (x = -4; x <= 4 && !done; x++) {
							// BUGFIX: incorrect check of offset against limits of the dungeon
							if (y < 0 || y >= MAXDUNY || x < 0 || x >= MAXDUNX)
								continue;
							done = grid[data._m.x + x][data._m.y + y].dDead != 0
							    && LineClearF(
							           CheckNoSolid,
							           data._m,
							        { data._m.x + x,
							            data._m.y + y });
						}
					}
					x--;
					y--;
				} else {
					for (y = 4; y >= -4 && !done; y--) {
						for (x = 4; x >= -4 && !done; x--) {
							// BUGFIX: incorrect check of offset against limits of the dungeon
							if (y < 0 || y >= MAXDUNY || x < 0 || x >= MAXDUNX)
								continue;
							done = grid[data._m.x + x][data._m.y + y].dDead != 0
							    && LineClearF(
							           CheckNoSolid,
							           data._m,
							        { data._m.x + x,
							            data._m.y + y });
						}
					}
					x++;
					y++;
				}
				if (done) {
					data._mgoalvar1 = x + data._m.x + 1;
					data._mgoalvar2 = y + data._m.y + 1;
				}
			}
			if (data._mgoalvar1) {
				x = data._mgoalvar1 - 1;
				y = data._mgoalvar2 - 1;
				data._mdir = GetDirection(data._m, { x, y });
				M_CallWalk(data._mdir);
			}
		}
	}
	if (data._mmode == MM_STAND) MAI_SkelSd();
}

void MonsterInstance::MAI_Garg()
{
	int md = M_GetDir();
	if (data._msquelch != 0 && data._mFlags & MFLAG_ALLOW_SPECIAL) {
		M_Enemy();
		V2Di d = abs(data._m - data._last);
		if (d.maxdim() < data._mint + 2) {
			data._mFlags &= ~MFLAG_ALLOW_SPECIAL;
		}
		return;
	}

	if (data._mmode != MM_STAND || data._msquelch == 0) {
		return;
	}

	if (data._mhitpoints<data._mmaxhp>> 1 && !(data._mFlags & MFLAG_NOHEAL))
		data._mgoal = MGOAL_RETREAT;
	if (data._mgoal == MGOAL_RETREAT) {
		V2Di d = abs(data._m - data._last);
		if (d.maxdim() >= data._mint + 2) {
			data._mgoal = MGOAL_NORMAL;
			M_StartHeal();
		} else if (!M_CallWalk(opposite[md])) {
			data._mgoal = MGOAL_NORMAL;
		}
	}
	MAI_Round(FALSE);
}

void MonsterInstance::MAI_RoundRanged(int missile_type, BOOL checkdoors, int dam, int lessmissiles)
{
	if (data._mmode == MM_STAND && data._msquelch != 0) {
		V2Di f = data._menemypos;
		V2Di d = data._m - f;
		int dist = d.maxdim();
		int md = GetDirection(data._m, data._last);
		if (checkdoors && data._msquelch < UCHAR_MAX)
			MonstCheckDoors(i);
		int v = random_(121, 10000);
		if ((dist >= 2) && data._msquelch == UCHAR_MAX && grid.at(data._m).dTransVal == grid.at(f).dTransVal) {
			if (data._mgoal == MGOAL_MOVE || ((dist >= 3) && random_(122, 4 << lessmissiles) == 0)) {
				if (data._mgoal != MGOAL_MOVE) {
					data._mgoalvar1 = 0;
					data._mgoalvar2 = random_(123, 2);
				}
				data._mgoal = MGOAL_MOVE;
				if (data._mgoalvar1++ >= 2 * dist && DirOK(md)) {
					data._mgoal = MGOAL_NORMAL;
				} else if (v<500 * (data._mint + 1)>> lessmissiles
				    && (LineClear(data._m, f))) {
					M_StartRSpAttack(missile_type, dam);
				} else {
					M_RoundWalk(md, &data._mgoalvar2);
				}
			}
		} else {
			data._mgoal = MGOAL_NORMAL;
		}
		if (data._mgoal == MGOAL_NORMAL) {
			if (((dist >= 3) && v < ((500 * (data._mint + 2)) >> lessmissiles)
			        || v < ((500 * (data._mint + 1)) >> lessmissiles))
			    && LineClear(data._m, f)) {
				M_StartRSpAttack(missile_type, dam);
			} else if (dist >= 2) {
				v = random_(124, 100);
				if (v < 1000 * (data._mint + 5)
				    || (data._mVar1 == MM_WALK || data._mVar1 == MM_WALK2 || data._mVar1 == MM_WALK3) && data._mVar2 == 0 && v < 1000 * (data._mint + 8)) {
					M_CallWalk(md);
				}
			} else if (v < 1000 * (data._mint + 6)) {
				data._mdir = md;
				M_StartAttack();
			}
		}
		if (data._mmode == MM_STAND) {
			M_StartDelay(random_(125, 10) + 5);
		}
	}
}

void MonsterInstance::MAI_Magma()
{
	MAI_RoundRanged(MIS_MAGMABALL, TRUE, 4, 0);
}

void MonsterInstance::MAI_Storm()
{
	MAI_RoundRanged(MIS_LIGHTCTRL2, TRUE, 4, 0);
}

void MonsterInstance::MAI_Acid()
{
	MAI_RoundRanged(MIS_ACID, FALSE, 4, 1);
}

void MonsterInstance::MAI_Diablo()
{
	MAI_RoundRanged(MIS_DIABAPOCA, FALSE, 40, 0);
}

void MonsterInstance::MAI_RR2(int mistype, int dam)
{
	int mx, my, fx, fy;
	int dist, v, md;
	mx = data._mx - data._menemyx;
	my = data._my - data._menemyy;
	if (abs(mx) >= 5 || abs(my) >= 5) {
		MAI_SkelSd();
		return;
	}

	if (data._mmode == MM_STAND && data._msquelch != 0) {
		fx = data._menemyx;
		fy = data._menemyy;
		mx = data._mx - fx;
		my = data._my - fy;
		md = GetDirection(data._mx, data._my, data._lastx, data._lasty);
		if (data._msquelch < UCHAR_MAX)
			MonstCheckDoors(i);
		v = random_(121, 100);
		if ((abs(mx) >= 2 || abs(my) >= 2) && data._msquelch == UCHAR_MAX && grid[data._mx][data._my].dTransVal == grid[fx][fy].dTransVal) {
			if (data._mgoal == MGOAL_MOVE || (abs(mx) >= 3 || abs(my) >= 3)) {
				if (data._mgoal != MGOAL_MOVE) {
					data._mgoalvar1 = 0;
					data._mgoalvar2 = random_(123, 2);
				}
				data._mgoal = MGOAL_MOVE;
				data._mgoalvar3 = 4;
				if (abs(mx) > abs(my)) {
					dist = abs(mx);
				} else {
					dist = abs(my);
				}
				if (data._mgoalvar1++ < 2 * dist || !DirOK(md)) {
					if (v < 5 * (data._mint + 16))
						M_RoundWalk(md, &data._mgoalvar2);
				} else
					data._mgoal = MGOAL_NORMAL;
			}
		} else
			data._mgoal = MGOAL_NORMAL;
		if (data._mgoal == MGOAL_NORMAL) {
			if (((abs(mx) >= 3 || abs(my) >= 3) && v < 5 * (data._mint + 2) || v < 5 * (data._mint + 1) || data._mgoalvar3 == 4) && LineClear(data._mx, data._my, fx, fy)) {
				M_StartRSpAttack(mistype, dam);
			} else if (abs(mx) >= 2 || abs(my) >= 2) {
				v = random_(124, 100);
				if (v < 2 * (5 * data._mint + 25)
				    || (data._mVar1 == MM_WALK || data._mVar1 == MM_WALK2 || data._mVar1 == MM_WALK3)
				        && data._mVar2 == 0
				        && v < 2 * (5 * data._mint + 40)) {
					M_CallWalk(md);
				}
			} else {
				if (random_(124, 100) < 10 * (data._mint + 4)) {
					data._mdir = md;
					if (random_(124, 2) != 0)
						M_StartAttack();
					else
						M_StartRSpAttack(mistype, dam);
				}
			}
			data._mgoalvar3 = 1;
		}
		if (data._mmode == MM_STAND) {
			M_StartDelay(random_(125, 10) + 5);
		}
	}
}

void MonsterInstance::MAI_Mega()
{
	MAI_RR2(MIS_FLAMEC, 0);
}

void MonsterInstance::MAI_Golum()
{
	int mx, my, _mex, _mey;
	int md, j, k, _menemy;
	BOOL have_enemy, ok;
	if (data._mx == 1 && data._my == 0) {
		return;
	}

	if (data._mmode == MM_DEATH
	    || data._mmode == MM_SPSTAND
	    || (data._mmode >= MM_WALK && data._mmode <= MM_WALK3)) {
		return;
	}

	if (!(data._mFlags & MFLAG_TARGETS_MONSTER))
		M_Enemy();

	have_enemy = !(data._mFlags & MFLAG_NO_ENEMY);

	if (data._mmode == MM_ATTACK) return;

	_menemy = data._menemy;

	mx = data._mx;
	my = data._my;
	_mex = mx - monsters[_menemy].data._mfutx;
	_mey = my - monsters[_menemy].data._mfuty;
	md = GetDirection(mx, my, monsters[_menemy].data._mx, monsters[_menemy].data._my);
	data._mdir = md;
	if (abs(_mex) >= 2 || abs(_mey) >= 2) {
		if (have_enemy && MAI_Path())
			return;
	} else if (have_enemy) {
		_menemy = data._menemy;
		data._menemyx = monsters[_menemy].data._mx;
		data._menemyy = monsters[_menemy].data._my;
		if (monsters[_menemy].data._msquelch == 0) {
			monsters[_menemy].data._msquelch = UCHAR_MAX;
			monsters[data._menemy].data._lastx = data._mx;
			monsters[data._menemy].data._lasty = data._my;
			for (j = 0; j < 5; j++) {
				for (k = 0; k < 5; k++) {
					_menemy = grid[data._mx + k - 2][data._my + j - 2].dMonster;
					if (_menemy > 0)
						monsters[_menemy].data._msquelch = UCHAR_MAX;
				}
			}
		}
		M_StartAttack();
		return;
	}

	data._pathcount++;
	if (data._pathcount > 8)
		data._pathcount = 5;

	ok = M_CallWalk(plr[i].data._pdir);
	if (!ok) {
		md = (md - 1) & 7;
		for (j = 0; j < 8 && !ok; j++) {
			md = (md + 1) & 7;
			ok = DirOK(md);
		}
		if (!ok) {
			return;
		}
		M_WalkDir(md);
	}
}

void MonsterInstance::MAI_SkelKing()
{
	int mx, my, fx, fy, nx, ny;
	int dist, v, md;
	if (data._mmode == MM_STAND && data._msquelch != 0) {
		fx = data._menemyx;
		fy = data._menemyy;
		mx = data._mx - fx;
		my = data._my - fy;
		md = GetDirection(data._mx, data._my, data._lastx, data._lasty);
		if (data._msquelch < UCHAR_MAX)
			MonstCheckDoors(i);
		v = random_(126, 100);
		if ((abs(mx) >= 2 || abs(my) >= 2) && data._msquelch == UCHAR_MAX && grid[data._mx][data._my].dTransVal == grid[fx][fy].dTransVal) {
			if (data._mgoal == MGOAL_MOVE || (abs(mx) >= 3 || abs(my) >= 3) && random_(127, 4) == 0) {
				if (data._mgoal != MGOAL_MOVE) {
					data._mgoalvar1 = 0;
					data._mgoalvar2 = random_(128, 2);
				}
				data._mgoal = MGOAL_MOVE;
				if (abs(mx) > abs(my)) {
					dist = abs(mx);
				} else {
					dist = abs(my);
				}
				if (data._mgoalvar1++ >= 2 * dist && DirOK(md) || grid[data._mx][data._my].dTransVal != grid[fx][fy].dTransVal) {
					data._mgoal = MGOAL_NORMAL;
				} else if (!M_RoundWalk(md, &data._mgoalvar2)) {
					M_StartDelay(random_(125, 10) + 10);
				}
			}
		} else
			data._mgoal = MGOAL_NORMAL;
		if (data._mgoal == MGOAL_NORMAL) {
			if (gbMaxPlayers == 1
			    && ((abs(mx) >= 3 || abs(my) >= 3) && v < 4 * data._mint + 35 || v < 6)
			    && LineClear(data._mx, data._my, fx, fy)) {
				nx = data._mx + offset_x[md];
				ny = data._my + offset_y[md];
				if (PosOkMonst(i, nx, ny) && nummonsters < MAXMONSTERS) {
					M_SpawnSkel(nx, ny, md);
					M_StartSpStand(md);
				}
			} else {
				if (abs(mx) >= 2 || abs(my) >= 2) {
					v = random_(129, 100);
					if (v >= data._mint + 25
					    && (data._mVar1 != MM_WALK && data._mVar1 != MM_WALK2 && data._mVar1 != MM_WALK3 || data._mVar2 != 0 || (v >= data._mint + 75))) {
						M_StartDelay(random_(130, 10) + 10);
					} else {
						M_CallWalk(md);
					}
				} else if (v < data._mint + 20) {
					data._mdir = md;
					M_StartAttack();
				}
			}
		}
		if (data._mmode == MM_STAND)
			data._mAnimData = data.MType->Anims[MA_STAND].Data[md];
	}
}

void MonsterInstance::MAI_Rhino()
{
	int mx, my, fx, fy;
	int v, dist, md;
	if (data._mmode == MM_STAND && data._msquelch != 0) {
		fx = data._menemyx;
		fy = data._menemyy;
		mx = data._mx - fx;
		my = data._my - fy;
		md = GetDirection(data._mx, data._my, data._lastx, data._lasty);
		if (data._msquelch < UCHAR_MAX)
			MonstCheckDoors(i);
		v = random_(131, 100);
		if (abs(mx) >= 2 || abs(my) >= 2) {
			if (data._mgoal == MGOAL_MOVE || (abs(mx) >= 5 || abs(my) >= 5) && random_(132, 4) != 0) {
				if (data._mgoal != MGOAL_MOVE) {
					data._mgoalvar1 = 0;
					data._mgoalvar2 = random_(133, 2);
				}
				data._mgoal = MGOAL_MOVE;
				if (abs(mx) > abs(my)) {
					dist = abs(mx);
				} else {
					dist = abs(my);
				}
				if (data._mgoalvar1++ >= 2 * dist || grid[data._mx][data._my].dTransVal != grid[fx][fy].dTransVal) {
					data._mgoal = MGOAL_NORMAL;
				} else if (!M_RoundWalk(md, &data._mgoalvar2)) {
					M_StartDelay(random_(125, 10) + 10);
				}
			}
		} else
			data._mgoal = MGOAL_NORMAL;
		if (data._mgoal == MGOAL_NORMAL) {
			if ((abs(mx) >= 5 || abs(my) >= 5)
			    && v < 2 * data._mint + 43
			    && LineClearF1(PosOkMonst, i, data._mx, data._my, fx, fy)) {
				if (AddMissile(data._mx, data._my, fx, fy, md, MIS_RHINO, data._menemy, i, 0, 0) != -1) {
					if (data.MData->snd_special)
						PlayEffect(i, 3);
					data._mmode = MM_CHARGE;
					grid[data._mx][data._my].dMonster = -1 - i;
				}
			} else {
				if (abs(mx) >= 2 || abs(my) >= 2) {
					v = random_(134, 100);
					if (v >= 2 * data._mint + 33
					    && (data._mVar1 != MM_WALK && data._mVar1 != MM_WALK2 && data._mVar1 != MM_WALK3
					           || data._mVar2
					           || v >= 2 * data._mint + 83)) {
						M_StartDelay(random_(135, 10) + 10);
					} else {
						M_CallWalk(md);
					}
				} else if (v < 2 * data._mint + 28) {
					data._mdir = md;
					M_StartAttack();
				}
			}
		}
		if (data._mmode == MM_STAND)
			data._mAnimData = data.MType->Anims[MA_STAND].Data[data._mdir];
	}
}

void MonsterInstance::MAI_Counselor()
{
	int mx, my, fx, fy;
	int dist, md, v;
	if (data._mmode == MM_STAND && data._msquelch != 0) {
		fx = data._menemyx;
		fy = data._menemyy;
		mx = data._mx - fx;
		my = data._my - fy;
		md = GetDirection(data._mx, data._my, data._lastx, data._lasty);
		if (data._msquelch < UCHAR_MAX)
			MonstCheckDoors(i);
		v = random_(121, 100);
		if (data._mgoal == MGOAL_RETREAT) {
			if (data._mgoalvar1++ <= 3)
				M_CallWalk(opposite[md]);
			else {
				data._mgoal = MGOAL_NORMAL;
				M_StartFadein(md, TRUE);
			}
		} else if (data._mgoal == MGOAL_MOVE) {
			if (abs(mx) > abs(my))
				dist = abs(mx);
			else
				dist = abs(my);
			if ((abs(mx) >= 2 || abs(my) >= 2) && data._msquelch == UCHAR_MAX && grid[data._mx][data._my].dTransVal == grid[fx][fy].dTransVal) {
				if (data._mgoalvar1++ < 2 * dist || !DirOK(md)) {
					M_RoundWalk(md, &data._mgoalvar2);
				} else {
					data._mgoal = MGOAL_NORMAL;
					M_StartFadein(md, TRUE);
				}
			} else {
				data._mgoal = MGOAL_NORMAL;
				M_StartFadein(md, TRUE);
			}
		} else if (data._mgoal == MGOAL_NORMAL) {
			if (abs(mx) >= 2 || abs(my) >= 2) {
				if (v < 5 * (data._mint + 10) && LineClear(data._mx, data._my, fx, fy)) {
					M_StartRAttack(counsmiss[data._mint], data.mMinDamage + random_(77, data.mMaxDamage - data.mMinDamage + 1));
				} else if (random_(124, 100) < 30) {
					data._mgoal = MGOAL_MOVE;
					data._mgoalvar1 = 0;
					M_StartFadeout(md, FALSE);
				} else
					M_StartDelay(random_(105, 10) + 2 * (5 - data._mint));
			} else {
				data._mdir = md;
				if (data._mhitpoints < (data._mmaxhp >> 1)) {
					data._mgoal = MGOAL_RETREAT;
					data._mgoalvar1 = 0;
					M_StartFadeout(md, FALSE);
				} else if (data._mVar1 == MM_DELAY
				    || random_(105, 100) < 2 * data._mint + 20) {
					M_StartRAttack(-1, 0);
					AddMissile(data._mx, data._my, 0, 0, data._mdir, MIS_FLASH, 1, i, 4, 0);
					AddMissile(data._mx, data._my, 0, 0, data._mdir, MIS_FLASH2, 1, i, 4, 0);
				} else
					M_StartDelay(random_(105, 10) + 2 * (5 - data._mint));
			}
		}
		if (data._mmode == MM_STAND) {
			M_StartDelay(random_(125, 10) + 5);
		}
	}
}

void MonsterInstance::MAI_Garbud()
{
	int _mx, _my, md;
	if (data._mmode != MM_STAND) {
		return;
	}

	_mx = data._mx;
	_my = data._my;
	md = M_GetDir();

	if (data.mtalkmsg < TEXT_GARBUD4
	    && data.mtalkmsg > TEXT_DOOM10
	    && !(grid[_mx][_my].dFlags & BFLAG_VISIBLE)
	    && data._mgoal == MGOAL_TALKING) {
		data._mgoal = MGOAL_INQUIRING;
		data.mtalkmsg++;
	}

	if (grid[_mx][_my].dFlags & BFLAG_VISIBLE) {
#ifndef SPAWN
		if (data.mtalkmsg == TEXT_GARBUD4) {
			if (!effect_is_playing(USFX_GARBUD4) && data._mgoal == MGOAL_TALKING) {
				data._mgoal = MGOAL_NORMAL;
				data._msquelch = UCHAR_MAX;
				data.mtalkmsg = 0;
			}
		}
#endif
	}

	if (data._mgoal == MGOAL_NORMAL || data._mgoal == MGOAL_MOVE)
		MAI_Round(TRUE);

	data._mdir = md;

	if (data._mmode == MM_STAND)
		data._mAnimData = data.MType->Anims[MA_STAND].Data[md];
}

void MonsterInstance::MAI_Zhar()
{
	int mx, my, _mx, _my, md;
	if (data._mmode != MM_STAND) {
		return;
	}

	mx = data._mx;
	my = data._my;
	md = M_GetDir();
	if (data.mtalkmsg == TEXT_ZHAR1 && !(grid[mx][my].dFlags & BFLAG_VISIBLE) && data._mgoal == MGOAL_TALKING) {
		data.mtalkmsg = TEXT_ZHAR2;
		data._mgoal = MGOAL_INQUIRING;
	}

	if (grid[mx][my].dFlags & BFLAG_VISIBLE) {
		_mx = data._mx - data._menemyx;
		_my = data._my - data._menemyy;
		if (abs(_mx) > abs(_my))
			abs(_mx);
		else
			abs(_my);
#ifndef SPAWN
		if (data.mtalkmsg == TEXT_ZHAR2) {
			if (!effect_is_playing(USFX_ZHAR2) && data._mgoal == MGOAL_TALKING) {
				data._msquelch = UCHAR_MAX;
				data.mtalkmsg = 0;
				data._mgoal = MGOAL_NORMAL;
			}
		}
#endif
	}

	if (data._mgoal == MGOAL_NORMAL || data._mgoal == MGOAL_RETREAT || data._mgoal == MGOAL_MOVE)
		MAI_Counselor();

	data._mdir = md;

	if (data._mmode == MM_STAND)
		data._mAnimData = data.MType->Anims[MA_STAND].Data[md];
}

void MonsterInstance::MAI_SnotSpil()
{
	int mx, my, md;
	if (data._mmode != MM_STAND) {
		return;
	}

	mx = data._mx;
	my = data._my;
	md = M_GetDir();

	if (data.mtalkmsg == TEXT_BANNER10 && !(grid[mx][my].dFlags & BFLAG_VISIBLE) && data._mgoal == MGOAL_TALKING) {
		data.mtalkmsg = TEXT_BANNER11;
		data._mgoal = MGOAL_INQUIRING;
	}

	if (data.mtalkmsg == TEXT_BANNER11 && quests[Q_LTBANNER]._qvar1 == 3) {
		data.mtalkmsg = 0;
		data._mgoal = MGOAL_NORMAL;
	}

	if (grid[mx][my].dFlags & BFLAG_VISIBLE) {
#ifndef SPAWN
		if (data.mtalkmsg == TEXT_BANNER12) {
			if (!effect_is_playing(USFX_SNOT3) && data._mgoal == MGOAL_TALKING) {
				ObjChangeMap(setpc.x, setpc.y, setpc.x + setpc_w + 1, setpc.y + setpc_h + 1);
				quests[Q_LTBANNER]._qvar1 = 3;
				RedoPlayerVision();
				data._msquelch = UCHAR_MAX;
				data.mtalkmsg = 0;
				data._mgoal = MGOAL_NORMAL;
			}
		}
#endif
		if (quests[Q_LTBANNER]._qvar1 == 3) {
			if (data._mgoal == MGOAL_NORMAL || data._mgoal == MGOAL_SHOOT)
				MAI_Fallen();
		}
	}

	data._mdir = md;

	if (data._mmode == MM_STAND)
		data._mAnimData = data.MType->Anims[MA_STAND].Data[md];
}

void MonsterInstance::MAI_Lazurus()
{
	int mx, my, md;
	if (data._mmode != MM_STAND) {
		return;
	}

	mx = data._mx;
	my = data._my;
	md = M_GetDir();
	if (grid[mx][my].dFlags & BFLAG_VISIBLE) {
		if (gbMaxPlayers == 1) {
			if (data.mtalkmsg == TEXT_VILE13 && data._mgoal == MGOAL_INQUIRING && myplr().data._px == TEXT_VILE13 && myplr().data._py == 46) {
				PlayInGameMovie("gendata\\fprst3.smk");
				data._mmode = MM_TALK;
				quests[Q_BETRAYER]._qvar1 = 5;
			}

#ifndef SPAWN
			if (data.mtalkmsg == TEXT_VILE13 && !effect_is_playing(USFX_LAZ1) && data._mgoal == MGOAL_TALKING) {
				ObjChangeMapResync(1, 18, 20, 24);
				RedoPlayerVision();
				data._msquelch = UCHAR_MAX;
				data.mtalkmsg = 0;
				quests[Q_BETRAYER]._qvar1 = 6;
				data._mgoal = MGOAL_NORMAL;
			}
#endif
		}

		if (gbMaxPlayers != 1 && data.mtalkmsg == TEXT_VILE13 && data._mgoal == MGOAL_INQUIRING && quests[Q_BETRAYER]._qvar1 <= 3) {
			data._mmode = MM_TALK;
		}
	}

	if (data._mgoal == MGOAL_NORMAL || data._mgoal == MGOAL_RETREAT || data._mgoal == MGOAL_MOVE) {
		data.mtalkmsg = 0;
		MAI_Counselor();
	}

	data._mdir = md;
	if (data._mmode == MM_STAND || data._mmode == MM_TALK)
		data._mAnimData = data.MType->Anims[MA_STAND].Data[md];
}

void MonsterInstance::MAI_Lazhelp()
{
	int _mx, _my;
	volatile int md; // BUGFIX: very questionable volatile
	if (data._mmode != MM_STAND)
		return;

	_mx = data._mx;
	_my = data._my;
	md = M_GetDir();

	if (grid[_mx][_my].dFlags & BFLAG_VISIBLE) {
		if (gbMaxPlayers == 1) {
			if (quests[Q_BETRAYER]._qvar1 <= 5) {
				data._mgoal = MGOAL_INQUIRING;
			} else {
				data.mtalkmsg = 0;
				data._mgoal = MGOAL_NORMAL;
			}
		} else
			data._mgoal = MGOAL_NORMAL;
	}
	if (data._mgoal == MGOAL_NORMAL)
		MAI_Succ();
	data._mdir = md;
	if (data._mmode == MM_STAND)
		data._mAnimData = data.MType->Anims[MA_STAND].Data[md];
}

void MonsterInstance::MAI_Lachdanan()
{
	int _mx, _my, md;
	if (data._mmode != MM_STAND) {
		return;
	}

	_mx = data._mx;
	_my = data._my;
	md = M_GetDir();
#ifndef SPAWN
	if (data.mtalkmsg == TEXT_VEIL9 && !(grid[_mx][_my].dFlags & BFLAG_VISIBLE) && data._mgoal == MGOAL_TALKING) {
		data.mtalkmsg = TEXT_VEIL10;
		data._mgoal = MGOAL_INQUIRING;
	}

	if (grid[_mx][_my].dFlags & BFLAG_VISIBLE) {
		if (data.mtalkmsg == TEXT_VEIL11) {
			if (!effect_is_playing(USFX_LACH3) && data._mgoal == MGOAL_TALKING) {
				data.mtalkmsg = 0;
				quests[Q_VEIL]._qactive = QUEST_DONE;
				M_StartKill(-1);
			}
		}
	}
#endif

	data._mdir = md;

	if (data._mmode == MM_STAND)
		data._mAnimData = data.MType->Anims[MA_STAND].Data[md];
}

void MonsterInstance::MAI_Warlord()
{
	int mx, my, md;
	//if ((DWORD)i >= MAXMONSTERS)

	if (data._mmode != MM_STAND) {
		return;
	}

	mx = data._mx;
	my = data._my;
	md = M_GetDir();
	if (grid[mx][my].dFlags & BFLAG_VISIBLE) {
		if (data.mtalkmsg == TEXT_WARLRD9 && data._mgoal == MGOAL_INQUIRING)
			data._mmode = MM_TALK;
#ifndef SPAWN
		if (data.mtalkmsg == TEXT_WARLRD9 && !effect_is_playing(USFX_WARLRD1) && data._mgoal == MGOAL_TALKING) {
			data._msquelch = UCHAR_MAX;
			data.mtalkmsg = 0;
			data._mgoal = MGOAL_NORMAL;
		}
#endif
	}

	if (data._mgoal == MGOAL_NORMAL)
		MAI_SkelSd();

	data._mdir = md;

	if (data._mmode == MM_STAND || data._mmode == MM_TALK)
		data._mAnimData = data.MType->Anims[MA_STAND].Data[data._mdir];
}

void DeleteMonsterList()
{
	int i;
	for (i = 0; i < MAX_PLRS; i++) {
		if (monsters[i].data._mDelFlag) {
			monsters[i].data._mx = 1;
			monsters[i].data._my = 0;
			monsters[i].data._mfutx = 0;
			monsters[i].data._mfuty = 0;
			monsters[i].data._moldx = 0;
			monsters[i].data._moldy = 0;
			monsters[i].data._mDelFlag = FALSE;
		}
	}

	i = MAX_PLRS;
	while (i < nummonsters) {
		if (monsters[monstactive[i]].data._mDelFlag) {
			monsters.DeleteMonster(i);
			i = 0; // TODO: check if this should be MAX_PLRS.
		} else {
			i++;
		}
	}
}

void ProcessMonsters()
{
	int i, mi, mx, my, _menemy;
	BOOL raflag;
	MonsterStruct *Monst;

	DeleteMonsterList();

	assert((DWORD)nummonsters <= MAXMONSTERS);
	for (i = 0; i < nummonsters; i++) {
		mi = monstactive[i];
		Monst = &monsters[mi].data;
		raflag = FALSE;
		if (gbMaxPlayers > 1) {
			SetRndSeed(Monst->_mAISeed);
			Monst->_mAISeed = GetRndSeed();
		}
		if (!(Monst->_mFlags & MFLAG_NOHEAL) && Monst->_mhitpoints < Monst->_mmaxhp && Monst->_mhitpoints >> 6 > 0) {
			if (Monst->mLevel > 1) {
				Monst->_mhitpoints += Monst->mLevel >> 1;
			} else {
				Monst->_mhitpoints += Monst->mLevel;
			}
		}
		mx = Monst->_mx;
		my = Monst->_my;
#ifndef SPAWN
		if (grid[mx][my].dFlags & BFLAG_VISIBLE && Monst->_msquelch == 0) {
			if (Monst->MType->mtype == MT_CLEAVER) {
				PlaySFX(USFX_CLEAVER);
			}
		}
#endif
		if (Monst->_mFlags & MFLAG_TARGETS_MONSTER) {
			_menemy = Monst->_menemy;
			if ((DWORD)_menemy >= MAXMONSTERS) {
				app_fatal("Illegal enemy monster %d for monster \"%s\"", _menemy, Monst->mName);
			}
			Monst->_lastx = monsters[Monst->_menemy].data._mfutx;
			Monst->_menemyx = Monst->_lastx;
			Monst->_lasty = monsters[Monst->_menemy].data._mfuty;
			Monst->_menemyy = Monst->_lasty;
		} else {
			_menemy = Monst->_menemy;
			if ((DWORD)_menemy >= MAX_PLRS) {
				app_fatal("Illegal enemy player %d for monster \"%s\"", _menemy, Monst->mName);
			}
			Monst->_menemyx = plr[Monst->_menemy].data._pfutx;
			Monst->_menemyy = plr[Monst->_menemy].data._pfuty;
			if (grid[mx][my].dFlags & BFLAG_VISIBLE) {
				Monst->_msquelch = 255;
				Monst->_lastx = plr[Monst->_menemy].data._pfutx;
				Monst->_lasty = plr[Monst->_menemy].data._pfuty;
			} else if (Monst->_msquelch != 0 && Monst->_mAi != MT_DIABLO) { /// BUGFIX: change '_mAi' to 'MType->mtype'
				Monst->_msquelch--;
			}
		}
		do {
			if (!(Monst->_mFlags & MFLAG_SEARCH)) {
				std::invoke(AiProc[Monst->_mAi], &monsters[mi]);
				//AiProc[Monst->_mAi](monsters[mi]);
			} else if (!monsters[mi].MAI_Path()) {
				//AiProc[Monst->_mAi](monsters[mi]);
				std::invoke(AiProc[Monst->_mAi], &monsters[mi]);
			}
			switch (Monst->_mmode) {
			case MM_STAND:
				raflag = monsters[mi].M_DoStand();
				break;
			case MM_WALK:
				raflag = monsters[mi].M_DoWalk();
				break;
			case MM_WALK2:
				raflag = monsters[mi].M_DoWalk2();
				break;
			case MM_WALK3:
				raflag = monsters[mi].M_DoWalk3();
				break;
			case MM_ATTACK:
				raflag = monsters[mi].M_DoAttack();
				break;
			case MM_GOTHIT:
				raflag = monsters[mi].M_DoGotHit();
				break;
			case MM_DEATH:
				raflag = monsters[mi].M_DoDeath();
				break;
			case MM_SATTACK:
				raflag = monsters[mi].M_DoSAttack();
				break;
			case MM_FADEIN:
				raflag = monsters[mi].M_DoFadein();
				break;
			case MM_FADEOUT:
				raflag = monsters[mi].M_DoFadeout();
				break;
			case MM_RATTACK:
				raflag = monsters[mi].M_DoRAttack();
				break;
			case MM_SPSTAND:
				raflag = monsters[mi].M_DoSpStand();
				break;
			case MM_RSPATTACK:
				raflag = monsters[mi].M_DoRSpAttack();
				break;
			case MM_DELAY:
				raflag = monsters[mi].M_DoDelay();
				break;
			case MM_CHARGE:
				raflag = FALSE;
				break;
			case MM_STONE:
				raflag = monsters[mi].M_DoStone();
				break;
			case MM_HEAL:
				raflag = monsters[mi].M_DoHeal();
				break;
			case MM_TALK:
				raflag = monsters[mi].M_DoTalk();
				break;
			}
			if (raflag) {
				monsters[mi].GroupUnity();
			}
		} while (raflag);
		if (Monst->_mmode != MM_STONE) {
			Monst->_mAnimCnt++;
			if (!(Monst->_mFlags & MFLAG_ALLOW_SPECIAL) && Monst->_mAnimCnt >= Monst->_mAnimDelay) {
				Monst->_mAnimCnt = 0;
				if (Monst->_mFlags & MFLAG_LOCK_ANIMATION) {
					Monst->_mAnimFrame--;
					if (Monst->_mAnimFrame == 0) {
						Monst->_mAnimFrame = Monst->_mAnimLen;
					}
				} else {
					Monst->_mAnimFrame++;
					if (Monst->_mAnimFrame > Monst->_mAnimLen) {
						Monst->_mAnimFrame = 1;
					}
				}
			}
		}
	}

	DeleteMonsterList();
}

void FreeMonsters()
{
	int mtype;
	int i, j;

	for (i = 0; i < nummtypes; i++) {
		mtype = beastiary[i].data.mtype;
		for (j = 0; j < 6; j++) {
			if (animletter[j] != 's' || monsterdata[mtype].has_special) {
				MemFreeDbg(beastiary[i].data.Anims[j].CMem);
			}
		}
	}

	FreeMissiles2();
}

BOOL MonsterInstance::DirOK(int mdir)
{
	int fx, fy;
	int x, y;
	int mcount, mi;

	fx = data._mx + offset_x[mdir];
	fy = data._my + offset_y[mdir];
	if (fy < 0 || fy >= MAXDUNY || fx < 0 || fx >= MAXDUNX || !PosOkMonst(i, fx, fy))
		return FALSE;
	if (mdir == DIR_E) {
		if (SolidLoc(fx, fy + 1) || grid[fx][fy + 1].dFlags & BFLAG_MONSTLR)
			return FALSE;
	}
	if (mdir == DIR_W) {
		if (SolidLoc(fx + 1, fy) || grid[fx + 1][fy].dFlags & BFLAG_MONSTLR)
			return FALSE;
	}
	if (mdir == DIR_N) {
		if (SolidLoc(fx + 1, fy) || SolidLoc(fx, fy + 1))
			return FALSE;
	}
	if (mdir == DIR_S)
		if (SolidLoc(fx - 1, fy) || SolidLoc(fx, fy - 1))
			return FALSE;
	if (data.leaderflag == 1) {
		if (abs(fx - monsters[data.leader].data._mfutx) >= 4
		    || abs(fy - monsters[data.leader].data._mfuty) >= 4) {
			return FALSE;
		}
		return TRUE;
	}
	if (data._uniqtype == 0 || !(UniqMonst[data._uniqtype - 1].mUnqAttr & 2))
		return TRUE;
	mcount = 0;
	for (x = fx - 3; x <= fx + 3; x++) {
		for (y = fy - 3; y <= fy + 3; y++) {
			if (y < 0 || y >= MAXDUNY || x < 0 || x >= MAXDUNX)
				continue;
			mi = grid[x][y].dMonster;
			if (mi < 0)
				mi = -mi;
			if (mi != 0)
				mi--;
			if (monsters[mi].data.leaderflag == 1
			    && monsters[mi].data.leader == i
			    && monsters[mi].data._mfutx == x
			    && monsters[mi].data._mfuty == y) {
				mcount++;
			}
		}
	}
	return mcount == data.packsize;
}

BOOL PosOkMissile(V2Di pos)
{
	return !pieces[grid.at(pos).dPiece].nMissileTable && !(grid.at(pos).dFlags & BFLAG_MONSTLR);
}

BOOL CheckNoSolid(V2Di pos)
{
	return pieces[grid.at(pos).dPiece].nSolidTable == FALSE;
}

BOOL LineClearF(BOOL (*Clear)(V2Di), V2Di p1, V2Di p2)
{
	int d;
	int xincD, yincD, dincD, dincH;
	int tmp;

	V2Di org = p1;
	V2Di dd = p2 - p1;

	if (abs(dd.x) > abs(dd.y)) {
		if (dd.x < 0) {
			tmp = p1.x;
			p1.x = p2.x;
			p2.x = tmp;
			tmp = p1.y;
			p1.y = p2.y;
			p2.y = tmp;
			dd.x = -dd.x;
			dd.y = -dd.y;
		}
		if (dd.y > 0) {
			d = 2 * dd.y - dd.x;
			dincH = 2 * (dd.y - dd.x);
			dincD = 2 * dd.y;
			yincD = 1;
		} else {
			d = 2 * dd.y + dd.x;
			dincH = 2 * (dd.x + dd.y);
			dincD = 2 * dd.y;
			yincD = -1;
		}
		while (p1.x != p2.x || p1.y != p2.y) {
			if ((d <= 0) ^ (yincD < 0)) {
				d += dincD;
			} else {
				d += dincH;
				p1.y += yincD;
			}
			p1.x++;
			if ((p1 != org) && !Clear(p1))
				break;
		}
	} else {
		if (dd.y < 0) {
			tmp = p1.y;
			p1.y = p2.y;
			p2.y = tmp;
			tmp = p1.x;
			p1.x = p2.x;
			p2.x = tmp;
			dd.y = -dd.y;
			dd.x = -dd.x;
		}
		if (dd.x > 0) {
			d = 2 * dd.x - dd.y;
			dincH = 2 * (dd.x - dd.y);
			dincD = 2 * dd.x;
			xincD = 1;
		} else {
			d = 2 * dd.x + dd.y;
			dincH = 2 * (dd.y + dd.x);
			dincD = 2 * dd.x;
			xincD = -1;
		}
		while (p1.y != p2.y || p1.x != p2.x) {
			if ((d <= 0) ^ (xincD < 0)) {
				d += dincD;
			} else {
				d += dincH;
				p1.x += xincD;
			}
			p1.y++;
			if ((p1 != org) && !Clear(p1))
				break;
		}
	}
	return p1.x == p2.x && p1.y == p2.y;
}

BOOL LineClear(V2Di p1, V2Di p2)
{
	return LineClearF(PosOkMissile, p1, p2);
}

BOOL LineClearF1(BOOL (*Clear)(int, V2Di), int monst, V2Di p1, V2Di p2)
{
	V2Di org = p1;
	V2Di dd = p2 - p1;
	int d;
	int xincD, yincD, dincD, dincH;
	int tmp;

	org = p1;
	dd = p2 - p1;
	if (abs(dd.x) > abs(dd.y)) {
		if (dd.x < 0) {
			tmp = p1.x;
			p1.x = p2.x;
			p2.x = tmp;
			tmp = p1.y;
			p1.y = p2.y;
			p2.y = tmp;
			dd.x = -dd.x;
			dd.y = -dd.y;
		}
		if (dd.y > 0) {
			d = 2 * dd.y - dd.x;
			dincH = 2 * (dd.y - dd.x);
			dincD = 2 * dd.y;
			yincD = 1;
		} else {
			d = 2 * dd.y + dd.x;
			dincH = 2 * (dd.x + dd.y);
			dincD = 2 * dd.y;
			yincD = -1;
		}
		while (p1.x != p2.x || p1.y != p2.y) {
			if ((d <= 0) ^ (yincD < 0)) {
				d += dincD;
			} else {
				d += dincH;
				p1.y += yincD;
			}
			p1.x++;
			if ((p1.x != xorg || p1.y != yorg) && !Clear(monst, p1.x, p1.y))
				break;
		}
	} else {
		if (dd.y < 0) {
			tmp = p1.y;
			p1.y = p2.y;
			p2.y = tmp;
			tmp = p1.x;
			p1.x = p2.x;
			p2.x = tmp;
			dd.y = -dd.y;
			dd.x = -dd.x;
		}
		if (dd.x > 0) {
			d = 2 * dd.x - dd.y;
			dincH = 2 * (dd.x - dd.y);
			dincD = 2 * dd.x;
			xincD = 1;
		} else {
			d = 2 * dd.x + dd.y;
			dincH = 2 * (dd.y + dd.x);
			dincD = 2 * dd.x;
			xincD = -1;
		}
		while (p1 != p2) {
			if ((d <= 0) ^ (xincD < 0)) {
				d += dincD;
			} else {
				d += dincH;
				p1.x += xincD;
			}
			p1.y++;
			if ((p1 != org) && !Clear(monst, p1))
				break;
		}
	}
	return p1.x == p2.x && p1.y == p2.y;
}

void MonsterInstance::SyncMonsterAnim()
{
	MonsterData *MData;
	int _mdir;
	data.MType->mtype = beastiary[data._mMTidx].data.mtype;
	MData = beastiary[data._mMTidx].data.MData;
	data.MData = MData;
	if (data._uniqtype != 0)
		data.mName = UniqMonst[data._uniqtype - 1].mName;
	else
		data.mName = MData->mName;
	_mdir = data._mdir;

	switch (data._mmode) {
	case MM_WALK:
	case MM_WALK2:
	case MM_WALK3:
		data._mAnimData = data.MType->Anims[MA_WALK].Data[_mdir];
		return;
	case MM_ATTACK:
	case MM_RATTACK:
		data._mAnimData = data.MType->Anims[MA_ATTACK].Data[_mdir];
		return;
	case MM_GOTHIT:
		data._mAnimData = data.MType->Anims[MA_GOTHIT].Data[_mdir];
		return;
	case MM_DEATH:
		data._mAnimData = data.MType->Anims[MA_DEATH].Data[_mdir];
		return;
	case MM_SATTACK:
	case MM_FADEIN:
	case MM_FADEOUT:
		data._mAnimData = data.MType->Anims[MA_SPECIAL].Data[_mdir];
		return;
	case MM_SPSTAND:
	case MM_RSPATTACK:
		data._mAnimData = data.MType->Anims[MA_SPECIAL].Data[_mdir];
		return;
	case MM_HEAL:
		data._mAnimData = data.MType->Anims[MA_SPECIAL].Data[_mdir];
		return;
	case MM_STAND:
		data._mAnimData = data.MType->Anims[MA_STAND].Data[_mdir];
		return;
	case MM_DELAY:
		data._mAnimData = data.MType->Anims[MA_STAND].Data[_mdir];
		return;
	case MM_TALK:
		data._mAnimData = data.MType->Anims[MA_STAND].Data[_mdir];
		return;
	case MM_CHARGE:
		data._mAnimData = data.MType->Anims[MA_ATTACK].Data[_mdir];
		data._mAnimFrame = 1;
		data._mAnimLen = data.MType->Anims[MA_ATTACK].Frames;
		break;
	default:
		data._mAnimData = data.MType->Anims[MA_STAND].Data[_mdir];
		data._mAnimFrame = 1;
		data._mAnimLen = data.MType->Anims[MA_STAND].Frames;
		break;
	}
}

void M_FallenFear(V2Di pos)
{
	int i, mi, rundist, aitype;

	for (i = 0; i < nummonsters; i++) {
		rundist = 0;
		mi = monstactive[i];

		switch (monsters[mi].data.MType->mtype) {
		case MT_RFALLSP:
		case MT_RFALLSD:
			rundist = 7;
			break;
		case MT_DFALLSP:
		case MT_DFALLSD:
			rundist = 5;
			break;
		case MT_YFALLSP:
		case MT_YFALLSD:
			rundist = 3;
			break;
		case MT_BFALLSP:
		case MT_BFALLSD:
			rundist = 2;
			break;
		}
		aitype = monsters[mi].data._mAi;
		if (aitype == AI_FALLEN
		    && rundist
		    && abs(pos.x - monsters[mi].data._m.x) < 5
		    && abs(pos.y - monsters[mi].data._m.y) < 5
		    && monsters[mi].data._mhitpoints >> 6 > 0) {
			monsters[mi].data._mgoal = MGOAL_RETREAT;
			monsters[mi].data._mgoalvar1 = rundist;
			monsters[mi].data._mdir = GetDirection(pos, monsters[i].data._m);
		}
	}
}

void PrintMonstHistory(int mt)
{
	int minHP, maxHP, res;

	sprintf(tempstr, "Total kills : %i", monstkills[mt]);
	AddPanelString(tempstr, TRUE);
	if (monstkills[mt] >= 30) {
		minHP = monsterdata[mt].mMinHP;
		maxHP = monsterdata[mt].mMaxHP;
		if (gbMaxPlayers == 1) {
			minHP = monsterdata[mt].mMinHP >> 1;
			maxHP = monsterdata[mt].mMaxHP >> 1;
		}
		if (minHP < 1)
			minHP = 1;
		if (maxHP < 1)
			maxHP = 1;
		if (gnDifficulty == DIFF_NIGHTMARE) {
			minHP = 3 * minHP + 1;
			maxHP = 3 * maxHP + 1;
		}
		if (gnDifficulty == DIFF_HELL) {
			minHP = 4 * minHP + 3;
			maxHP = 4 * maxHP + 3;
		}
		sprintf(tempstr, "Hit Points : %i-%i", minHP, maxHP);
		AddPanelString(tempstr, TRUE);
	}
	if (monstkills[mt] >= 15) {
		if (gnDifficulty != DIFF_HELL)
			res = monsterdata[mt].mMagicRes;
		else
			res = monsterdata[mt].mMagicRes2;
		res = res & (RESIST_MAGIC | RESIST_FIRE | RESIST_LIGHTNING | IMUNE_MAGIC | IMUNE_FIRE | IMUNE_LIGHTNING);
		if (!res) {
			strcpy(tempstr, "No magic resistance");
			AddPanelString(tempstr, TRUE);
		} else {
			if (res & (RESIST_MAGIC | RESIST_FIRE | RESIST_LIGHTNING)) {
				strcpy(tempstr, "Resists : ");
				if (res & RESIST_MAGIC)
					strcat(tempstr, "Magic ");
				if (res & RESIST_FIRE)
					strcat(tempstr, "Fire ");
				if (res & RESIST_LIGHTNING)
					strcat(tempstr, "Lightning ");
				tempstr[strlen(tempstr) - 1] = '\0';
				AddPanelString(tempstr, TRUE);
			}
			if (res & (IMUNE_MAGIC | IMUNE_FIRE | IMUNE_LIGHTNING)) {
				strcpy(tempstr, "Immune : ");
				if (res & IMUNE_MAGIC)
					strcat(tempstr, "Magic ");
				if (res & IMUNE_FIRE)
					strcat(tempstr, "Fire ");
				if (res & IMUNE_LIGHTNING)
					strcat(tempstr, "Lightning ");
				tempstr[strlen(tempstr) - 1] = '\0';
				AddPanelString(tempstr, TRUE);
			}
		}
	}
	pinfoflag = TRUE;
}

void PrintUniqueHistory()
{
	int res = monsters[pcursmonst].data.mMagicRes & (RESIST_MAGIC | RESIST_FIRE | RESIST_LIGHTNING | IMUNE_MAGIC | IMUNE_FIRE | IMUNE_LIGHTNING);
	if (!res) {
		strcpy(tempstr, "No resistances");
		AddPanelString(tempstr, TRUE);
		strcpy(tempstr, "No Immunities");
	} else {
		if (res & (RESIST_MAGIC | RESIST_FIRE | RESIST_LIGHTNING))
			strcpy(tempstr, "Some Magic Resistances");
		else
			strcpy(tempstr, "No resistances");
		AddPanelString(tempstr, TRUE);
		if (res & (IMUNE_MAGIC | IMUNE_FIRE | IMUNE_LIGHTNING)) {
			strcpy(tempstr, "Some Magic Immunities");
		} else {
			strcpy(tempstr, "No Immunities");
		}
	}
	AddPanelString(tempstr, TRUE);
	pinfoflag = TRUE;
}

void MissToMonst(int i, V2Di pos)
{
	V2Di old, n;
	int m, pnum;
	MissileStruct *Miss;
	MonsterStruct *Monst;

	if ((DWORD)i >= MAXMISSILES)
		app_fatal("MissToMonst: Invalid missile %d", i);

	Miss = &missile[i];
	m = Miss->_misource;

	if ((DWORD)m >= MAXMONSTERS)
		app_fatal("MissToMonst: Invalid monster %d", m);

	Monst = &monsters[m].data;
	old = Miss->_mi;
	grid[pos.x][pos.y].dMonster = m + 1;
	Monst->_mdir = Miss->_mimfnum;
	Monst->_m = pos;
	monsters[m].M_StartStand(Monst->_mdir);
	if (Monst->MType->mtype < MT_INCIN || Monst->MType->mtype > MT_HELLBURN) {
		if (!(Monst->_mFlags & MFLAG_TARGETS_MONSTER))
			monsters[m].M_StartHit(-1, 0);
		else
			M2MStartHit(m, -1, 0);
	} else {
		monsters[m].M_StartFadein(Monst->_mdir, FALSE);
	}

	if (!(Monst->_mFlags & MFLAG_TARGETS_MONSTER)) {
		pnum = grid[old.x][old.y].dPlayer - 1;
		if (grid[old.x][old.y].dPlayer > 0) {
			if (Monst->MType->mtype != MT_GLOOM && (Monst->MType->mtype < MT_INCIN || Monst->MType->mtype > MT_HELLBURN)) {
				monsters[m].M_TryH2HHit(grid[old.x][old.y].dPlayer - 1, 500, Monst->mMinDamage2, Monst->mMaxDamage2);
				if (pnum == grid[old.x][old.y].dPlayer - 1 && (Monst->MType->mtype < MT_NSNAKE || Monst->MType->mtype > MT_GSNAKE)) {
					if (plr[pnum].data._pmode != 7 && plr[pnum].data._pmode != 8)
						plr[pnum].StartPlrHit(0, TRUE);
					n.x = old.x + offset_x[Monst->_mdir];
					n.y = old.y + offset_y[Monst->_mdir];
					if (PosOkPlayer(pnum, newx, newy)) {
						plr[pnum].data._px = newx;
						plr[pnum].data._py = newy;
						plr[pnum].FixPlayerLocation(plr[pnum].data._pdir);
						plr[pnum].FixPlrWalkTags();
						grid[newx][newy].dPlayer = pnum + 1;
						plr[pnum].SetPlayerOld();
					}
				}
			}
		}
	} else {
		if (grid[oldx][oldy].dMonster > 0) {
			if (Monst->MType->mtype != MT_GLOOM && (Monst->MType->mtype < MT_INCIN || Monst->MType->mtype > MT_HELLBURN)) {
				monsters[m].M_TryM2MHit(grid[oldx][oldy].dMonster - 1, 500, Monst->mMinDamage2, Monst->mMaxDamage2);
				if (Monst->MType->mtype < MT_NSNAKE || Monst->MType->mtype > MT_GSNAKE) {
					newx = oldx + offset_x[Monst->_mdir];
					newy = oldy + offset_y[Monst->_mdir];
					if (PosOkMonst(grid[oldx][oldy].dMonster - 1, newx, newy)) {
						m = grid[oldx][oldy].dMonster;
						grid[newx][newy].dMonster = m;
						grid[oldx][oldy].dMonster = 0;
						m--;
						monsters[m].data._mx = newx;
						monsters[m].data._mfutx = newx;
						monsters[m].data._my = newy;
						monsters[m].data._mfuty = newy;
					}
				}
			}
		}
	}
}

BOOL PosOkMonst(int i, V2Di pos)
{
	int oi, mi, j;
	BOOL ret, fire;

	fire = FALSE;
	ret = !SolidLoc(pos) && !grid.at(pos).dPlayer && !grid.at(pos).dMonster;
	if (ret && grid.at(pos).dObject) {
		oi = grid.at(pos).dObject > 0 ? grid.at(pos).dObject - 1 : -(grid.at(pos).dObject + 1);
		if (object[oi]._oSolidFlag)
			ret = FALSE;
	}

	if (ret && grid.at(pos).dMissile && i >= 0) {
		mi = grid.at(pos).dMissile;
		if (mi > 0) {
			if (missile[mi - 1]._mitype == MIS_FIREWALL) { // BUGFIX: Change 'mi' to 'mi - 1' (fixed)
				fire = TRUE;
			} else {
				for (j = 0; j < nummissiles; j++) {
					if (missile[missileactive[j]]._mitype == MIS_FIREWALL)
						fire = TRUE;
				}
			}
		}
		if (fire && (!(monsters[i].data.mMagicRes & IMUNE_FIRE) || monsters[i].data.MType->mtype == MT_DIABLO))
			ret = FALSE;
	}
	return ret;
}

BOOL PosOkMonst2(int i, V2Di pos)
{
	int oi, mi, j;
	BOOL ret, fire;

	fire = FALSE;
	ret = !SolidLoc(x, y);
	if (ret && grid[x][y].dObject) {
		oi = grid[x][y].dObject > 0 ? grid[x][y].dObject - 1 : -(grid[x][y].dObject + 1);
		if (object[oi]._oSolidFlag)
			ret = FALSE;
	}

	if (ret && grid[x][y].dMissile && i >= 0) {
		mi = grid[x][y].dMissile;
		if (mi > 0) {
			if (missile[mi - 1]._mitype == MIS_FIREWALL) { // BUGFIX: Change 'mi' to 'mi - 1' (fixed)
				fire = TRUE;
			} else {
				for (j = 0; j < nummissiles; j++) {
					if (missile[missileactive[j]]._mitype == MIS_FIREWALL)
						fire = TRUE;
				}
			}
		}
		if (fire && (!(monsters[i].data.mMagicRes & IMUNE_FIRE) || monsters[i].data.MType->mtype == MT_DIABLO))
			ret = FALSE;
	}

	return ret;
}

BOOL PosOkMonst3(int i, V2Di pos)
{
	int j, oi, objtype, mi;
	BOOL ret, fire, isdoor;

	fire = FALSE;
	ret = TRUE;
	isdoor = FALSE;

	if (ret && grid.at(pos).dObject != 0) {
		oi = grid.at(pos).dObject > 0 ? grid.at(pos).dObject - 1 : -(grid.at(pos).dObject + 1);
		objtype = object[oi]._otype;
		isdoor = objtype == OBJ_L1LDOOR || objtype == OBJ_L1RDOOR
		    || objtype == OBJ_L2LDOOR || objtype == OBJ_L2RDOOR
		    || objtype == OBJ_L3LDOOR || objtype == OBJ_L3RDOOR;
		if (object[oi]._oSolidFlag && !isdoor) {
			ret = FALSE;
		}
	}
	if (ret) {
		ret = (!SolidLoc(pos) || isdoor) && grid.at(pos).dPlayer == 0 && grid.at(pos).dMonster == 0;
	}
	if (ret && grid.at(pos).dMissile != 0 && i >= 0) {
		mi = grid.at(pos).dMissile;
		if (mi > 0) {
			if (missile[mi - 1]._mitype == MIS_FIREWALL) { // BUGFIX: Change 'mi' to 'mi - 1' (fixed)
				fire = TRUE;
			} else {
				for (j = 0; j < nummissiles; j++) {
					mi = missileactive[j];
					if (missile[mi]._mitype == MIS_FIREWALL) {
						fire = TRUE;
					}
				}
			}
		}
		if (fire && (!(monsters[i].data.mMagicRes & IMUNE_FIRE) || monsters[i].data.MType->mtype == MT_DIABLO)) {
			ret = FALSE;
		}
	}

	return ret;
}

BOOL IsSkel(int mt)
{
	return mt >= MT_WSKELAX && mt <= MT_XSKELAX
	    || mt >= MT_WSKELBW && mt <= MT_XSKELBW
	    || mt >= MT_WSKELSD && mt <= MT_XSKELSD;
}

BOOL IsGoat(int mt)
{
	return mt >= MT_NGOATMC && mt <= MT_GGOATMC
	    || mt >= MT_NGOATBW && mt <= MT_GGOATBW;
}

int M_SpawnSkel(V2Di pos, int dir)
{
	int i, j = 0;
	for (i = 0; i < nummtypes; i++) {
		if (IsSkel(beastiary[i].data.mtype)) j++;
	}

	if (j) {
		int skeltypes = random_(136, j);
		j = 0;
		for (i = 0; i < nummtypes && j <= skeltypes; i++) {
			if (IsSkel(beastiary[i].data.mtype)) j++;
		}
		int skel = AddMonster(pos, dir, i - 1, TRUE);
		if (skel != -1) monsters[skel].M_StartSpStand(dir);
		return skel;
	}
	return -1;
}

void MonsterInstance::ActivateSpawn(V2Di pos, int dir)
{
	grid[pos.x][pos.y].dMonster = i + 1;
	data._m = pos;
	data._mfut = pos;
	data._mold = pos;
	M_StartSpStand(dir);
}

BOOL SpawnSkeleton(int ii, V2Di pos)
{
	V2Di d, n;
	int dir, j, k, rs;
	BOOL savail;
	int monstok[3][3];

	if (ii == -1) return FALSE;

	if (PosOkMonst(-1, pos)) {
		dir = GetDirection(pos, pos); // ??? Gimmick to get random dir?
		monsters[ii].ActivateSpawn(pos, dir);
		return TRUE;
	}

	savail = FALSE;
	n.y = 0;
	for (j = pos.y - 1; j <= pos.y + 1; j++) {
		n.x = 0;
		for (k = pos.x - 1; k <= pos.x + 1; k++) {
			monstok[n.x][n.y] = PosOkMonst(-1, { k, j });
			savail |= monstok[n.x][n.y];
			n.x++;
		}
		n.y++;
	}
	if (!savail) {
		return FALSE;
	}

	rs = random_(137, 15) + 1;
	n = { 0, 0 };
	while (rs > 0) {
		if (monstok[xx][yy])
			rs--;
		if (rs > 0) {
			xx++;
			if (xx == 3) {
				xx = 0;
				yy++;
				if (yy == 3)
					yy = 0;
			}
		}
	}

	d.x = pos.x - 1 + n.x;
	d.y = pos.y - 1 + n.y;
	dir = GetDirection(d, pos);
	monsters[ii].ActivateSpawn(d, dir);
	return TRUE;
}

int PreSpawnSkeleton()
{
	int i, j, skeltypes, skel;
	j = 0;

	for (i = 0; i < nummtypes; i++) {
		if (IsSkel(beastiary[i].data.mtype))
			j++;
	}

	if (j) {
		skeltypes = random_(136, j);
		j = 0;
		for (i = 0; i < nummtypes && j <= skeltypes; i++) {
			if (IsSkel(beastiary[i].data.mtype))
				j++;
		}
		skel = AddMonster({ 0, 0 }, 0, i - 1, FALSE);
		if (skel != -1)
			monsters[skel].M_StartStand(0);
		return skel;
	}
	return -1;
}

void MonsterInstance::TalktoMonster()
{
	int pnum, itm;
	pnum = data._menemy;
	data._mmode = MM_TALK;
	if (data._mAi == AI_SNOTSPIL || data._mAi == AI_LACHDAN) {
		if (QuestStatus(Q_LTBANNER) && quests[Q_LTBANNER]._qvar1 == 2 && PlrHasItem(pnum, IDI_BANNER, &itm)) {
			plr[pnum].inventory.RemoveInvItem(itm);
			quests[Q_LTBANNER]._qactive = QUEST_DONE;
			data.mtalkmsg = TEXT_BANNER12;
			data._mgoal = MGOAL_INQUIRING;
		}
		if (QuestStatus(Q_VEIL) && data.mtalkmsg >= TEXT_VEIL9) {
			if (PlrHasItem(pnum, IDI_GLDNELIX, &itm)) {
				plr[pnum].inventory.RemoveInvItem(itm);
				data.mtalkmsg = TEXT_VEIL11;
				data._mgoal = MGOAL_INQUIRING;
			}
		}
	}
}

void MonsterInstance::SpawnGolum(V2Di pos, int mi)
{
	grid[pos.x][pos.y].dMonster = i + 1;
	data._m = pos;
	data._mfut = pos;
	data._mold = pos;
	data._pathcount = 0;
	data._mFlags |= MFLAG_GOLEM;
	data.mArmorClass = 25;
	data._mmaxhp = 2 * (320 * missile[mi]._mispllvl + plr[i].data._pMaxMana / 3);
	data._mhitpoints = data._mmaxhp;
	data.mHit = 5 * (missile[mi]._mispllvl + 8) + 2 * plr[i].data._pLevel;
	data.mMinDamage = 2 * (missile[mi]._mispllvl + 4);
	data.mMaxDamage = 2 * (missile[mi]._mispllvl + 8);
	M_StartSpStand(0);
	M_Enemy();
	if (i == myplr()) {
		NetSendCmdGolem(
		    data._m,
		    data._mdir,
		    data._menemy,
		    data._mhitpoints,
		    level.currlevel);
	}
}

BOOL CanTalkToMonst(int m)
{
	if ((DWORD)m >= MAXMONSTERS) {
		app_fatal("CanTalkToMonst: Invalid monster %d", m);
	}

	if (monsters[m].data._mgoal == MGOAL_INQUIRING) {
		return TRUE;
	}

	return monsters[m].data._mgoal == MGOAL_TALKING;
}

BOOL CheckMonsterHit(int m, BOOL *ret)
{
	if ((DWORD)m >= MAXMONSTERS) {
		app_fatal("CheckMonsterHit: Invalid monster %d", m);
	}

	if (monsters[m].data._mAi == AI_GARG && monsters[m].data._mFlags & MFLAG_ALLOW_SPECIAL) {
		monsters[m].data._mFlags &= ~MFLAG_ALLOW_SPECIAL;
		monsters[m].data._mmode = MM_SATTACK;
		*ret = TRUE;
		return TRUE;
	}

	if (monsters[m].data.MType->mtype >= MT_COUNSLR && monsters[m].data.MType->mtype <= MT_ADVOCATE) {
		if (monsters[m].data._mgoal != MGOAL_NORMAL) {
			*ret = FALSE;
			return TRUE;
		}
	}

	return FALSE;
}

int encode_enemy(int m)
{
	if (monsters[m].data._mFlags & MFLAG_TARGETS_MONSTER)
		return monsters[m].data._menemy + MAX_PLRS;
	else
		return monsters[m].data._menemy;
}

void decode_enemy(int m, int enemy)
{
	if (enemy < MAX_PLRS) {
		monsters[m].data._mFlags &= ~MFLAG_TARGETS_MONSTER;
		monsters[m].data._menemy = enemy;
		monsters[m].data._menemyx = plr[enemy].data._pfutx;
		monsters[m].data._menemyy = plr[enemy].data._pfuty;
	} else {
		monsters[m].data._mFlags |= MFLAG_TARGETS_MONSTER;
		enemy -= MAX_PLRS;
		monsters[m].data._menemy = enemy;
		monsters[m].data._menemyx = monsters[enemy].data._mfutx;
		monsters[m].data._menemyy = monsters[enemy].data._mfuty;
	}
}

DEVILUTION_END_NAMESPACE
