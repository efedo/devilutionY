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
V2Di offset[8] = {
	{  1,  1 },
	{  0,  1 },
	{ -1,  1 },
	{ -1,  0 },
	{ -1, -1 },
	{  0, -1 },
	{  1, -1 },
	{  1,  0 },
};

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
	SetMapMonsters(lpSetPiece, { 2 * diabquad1x, 2 * diabquad1y });
	mem_free_dbg(lpSetPiece);
	lpSetPiece = LoadFileInMem("Levels\\L4Data\\diab2a.DUN", NULL);
	SetMapMonsters(lpSetPiece, { 2 * diabquad2x, 2 * diabquad2y });
	mem_free_dbg(lpSetPiece);
	lpSetPiece = LoadFileInMem("Levels\\L4Data\\diab3a.DUN", NULL);
	SetMapMonsters(lpSetPiece, { 2 * diabquad3x, 2 * diabquad3y });
	mem_free_dbg(lpSetPiece);
	lpSetPiece = LoadFileInMem("Levels\\L4Data\\diab4a.DUN", NULL);
	SetMapMonsters(lpSetPiece, { 2 * diabquad4x, 2 * diabquad4y });
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
				if (!SolidLoc({ s, t }))
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

void SetMapMonsters(BYTE *pMap, V2Di start)
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
				monsters.PlaceMonster(nummonsters++, mtype, { i + start.x + 16, j + start.y + 16 });
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



void DeleteMonsterList()
{
	int i;
	for (i = 0; i < MAX_PLRS; i++) {
		if (monsters[i].data._mDelFlag) {
			monsters[i].data._m = { 1, 0 };
			monsters[i].data._mfut = { 0, 0 };
			monsters[i].data._mold = { 0, 0 };
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
	int i, mi, _menemy;
	V2Di m;
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
		m = Monst->_m;
		#ifndef SPAWN
		if (grid.at(m).dFlags & BFLAG_VISIBLE && Monst->_msquelch == 0) {
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
			Monst->_last = monsters[Monst->_menemy].data._mfut;
			Monst->_menemypos = Monst->_last;
		} else {
			_menemy = Monst->_menemy;
			if ((DWORD)_menemy >= MAX_PLRS) {
				app_fatal("Illegal enemy player %d for monster \"%s\"", _menemy, Monst->mName);
			}
			Monst->_menemypos = plr[Monst->_menemy].data._pfut;
			if (grid.at(m).dFlags & BFLAG_VISIBLE) {
				Monst->_msquelch = 255;
				Monst->_last = plr[Monst->_menemy].data._pfut;
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
	V2Di p;
	int mcount, mi;
	V2Di f = data._m + offset[mdir];
	if (f.y < 0 || f.y >= MAXDUNY || f.x < 0 || f.x >= MAXDUNX || !PosOkMonst(i, f))
		return FALSE;
	if (mdir == DIR_E) {
		if (SolidLoc({ f.x, f.y + 1 }) || grid[f.x][f.y + 1].dFlags & BFLAG_MONSTLR)
			return FALSE;
	}
	if (mdir == DIR_W) {
		if (SolidLoc({ f.x + 1, f.y }) || grid[f.x + 1][f.y].dFlags & BFLAG_MONSTLR)
			return FALSE;
	}
	if (mdir == DIR_N) {
		if (SolidLoc({ f.x + 1, f.y }) || SolidLoc({ f.x, f.y + 1 }))
			return FALSE;
	}
	if (mdir == DIR_S)
		if (SolidLoc({ f.x - 1, f.y }) || SolidLoc({ f.x, f.y - 1 }))
			return FALSE;
	if (data.leaderflag == 1) {
		int dist = (f - monsters[data.leader].data._mfut).maxabs();
		if (dist >= 4) {
			return FALSE;
		}
		return TRUE;
	}
	if (data._uniqtype == 0 || !(UniqMonst[data._uniqtype - 1].mUnqAttr & 2))
		return TRUE;
	mcount = 0;
	for (int x = f.x - 3; x <= f.x + 3; x++) {
		for (int y = f.y - 3; y <= f.y + 3; y++) {
			if (y < 0 || y >= MAXDUNY || x < 0 || x >= MAXDUNX)
				continue;
			mi = grid[x][y].dMonster;
			if (mi < 0)
				mi = -mi;
			if (mi != 0)
				mi--;
			if (monsters[mi].data.leaderflag == 1
			    && monsters[mi].data.leader == i
			    && monsters[mi].data._mfut.x == x
			    && monsters[mi].data._mfut.y == y) {
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

	if (::abs(dd.x) > ::abs(dd.y)) {
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
	if (::abs(dd.x) > ::abs(dd.y)) {
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
			if ((p1.x != org.x || p1.y != org.y) && !Clear(monst, p1))
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
		    && ::abs(pos.x - monsters[mi].data._m.x) < 5
		    && ::abs(pos.y - monsters[mi].data._m.y) < 5
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
					n = old + offset[Monst->_mdir];
					if (PosOkPlayer(pnum, n)) {
						plr[pnum].data._p = n;
						plr[pnum].FixPlayerLocation(plr[pnum].data._pdir);
						plr[pnum].FixPlrWalkTags();
						grid.at(n).dPlayer = pnum + 1;
						plr[pnum].SetPlayerOld();
					}
				}
			}
		}
	} else {
		if (grid.at(old).dMonster > 0) {
			if (Monst->MType->mtype != MT_GLOOM && (Monst->MType->mtype < MT_INCIN || Monst->MType->mtype > MT_HELLBURN)) {
				monsters[m].M_TryM2MHit(grid.at(old).dMonster - 1, 500, Monst->mMinDamage2, Monst->mMaxDamage2);
				if (Monst->MType->mtype < MT_NSNAKE || Monst->MType->mtype > MT_GSNAKE) {
					n = old + offset[Monst->_mdir];
					if (PosOkMonst(grid.at(old).dMonster - 1, n)) {
						m = grid.at(old).dMonster;
						grid.at(n).dMonster = m;
						grid.at(old).dMonster = 0;
						m--;
						monsters[m].data._m = n;
						monsters[m].data._mfut = n;
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
	ret = !SolidLoc(pos);
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
		if (monstok[n.x][n.y])
			rs--;
		if (rs > 0) {
			n.x++;
			if (n.x == 3) {
				n.x = 0;
				n.y++;
				if (n.y == 3)
					n.y = 0;
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
		monsters[m].data._menemypos = plr[enemy].data._pfut;
	} else {
		monsters[m].data._mFlags |= MFLAG_TARGETS_MONSTER;
		enemy -= MAX_PLRS;
		monsters[m].data._menemy = enemy;
		monsters[m].data._menemypos = monsters[enemy].data._mfut;
	}
}

DEVILUTION_END_NAMESPACE
