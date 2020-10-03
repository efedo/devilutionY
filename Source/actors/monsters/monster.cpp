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

//int monstkills[MAXMONSTERS];
//int monstactive[MAXMONSTERS];
int nummonsters;
bool sgbSaveSoundOn;
//MonsterStruct monster[MAXMONSTERS];
int totalmonsters;
//CMonster monstTypes[MAX_LVLMTYPES];
//uint8_t GraphicTable[NUMLEVELS][MAX_LVLMTYPES];
int monstimgtot;
int uniquetrans;
int nummtypes;

const char plr2monst[9] = { 0, 5, 3, 7, 1, 4, 6, 0, 2 };
const uint8_t counsmiss[4] = { MissileType::FIREBOLT, MissileType::CBOLT, MissileType::LIGHTCTRL, MissileType::FIREBALL };

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

void (Monster:: *AiProc[])() = {
	&Monster::MMonstAi::Zombie,
	&Monster::MMonstAi::Fat,
	&Monster::MMonstAi::SkelSd,
	&Monster::MMonstAi::SkelBow,
	&Monster::MMonstAi::Scav,
	&Monster::MMonstAi::Rhino,
	&Monster::MMonstAi::GoatMc,
	&Monster::MMonstAi::GoatBow,
	&Monster::MMonstAi::Fallen,
	&Monster::MMonstAi::Magma,
	&Monster::MMonstAi::SkelKing,
	&Monster::MMonstAi::Bat,
	&Monster::MMonstAi::Garg,
	&Monster::MMonstAi::Cleaver,
	&Monster::MMonstAi::Succ,
	&Monster::MMonstAi::Sneak,
	&Monster::MMonstAi::Storm,
	&Monster::MMonstAi::Fireman,
	&Monster::MMonstAi::Garbud,
	&Monster::MMonstAi::Acid,
	&Monster::MMonstAi::AcidUniq,
	&Monster::MMonstAi::Golum,
	&Monster::MMonstAi::Zhar,
	&Monster::MMonstAi::SnotSpil,
	&Monster::MMonstAi::Snake,
	&Monster::MMonstAi::Counselor,
	&Monster::MMonstAi::Mega,
	&Monster::MMonstAi::Diablo,
	&Monster::MMonstAi::Lazurus,
	&Monster::MMonstAi::Lazhelp,
	&Monster::MMonstAi::Lachdanan,
	&Monster::MMonstAi::Warlord,
};





bool MonstPlace(V2Di p)
{
	if (p.x < 0 || p.x >= MAXDUNX
	    || p.y < 0 || p.y >= MAXDUNY
	    || grid.at(p).getActor()
	    || grid.at(p).isPlayer()) {
		return false;
	}

	char f = grid.at(p).dFlags;
	if (f & DunTileFlag::VISIBLE) return false;
	if (f & DunTileFlag::POPULATED) return false;
	return !grid.at(p).isSolid();
}

void LoadDiabMonsts()
{
	uint8_t *lpSetPiece;

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

void InitMonsters()
{
	int na, nt;
	int i, s, t;
	int numplacemonsters;
	int mtype;
	int numscattypes;
	int scattertypes[NUM_MTYPES];

	numscattypes = 0;
	if (plr.isMultiplayer())
		CheckDungeonClear();
	if (!lvl.setlevel) {
		AddMonster({ 1, 0 }, Dir(0), 0, false);
		AddMonster({ 1, 0 }, Dir(0), 0, false);
		AddMonster({ 1, 0 }, Dir(0), 0, false);
		AddMonster({ 1, 0 }, Dir(0), 0, false);
	}
	if (!lvl.setlevel && lvl.currlevel == 16)
		LoadDiabMonsts();
	nt = numtrigs;
	if (lvl.currlevel == 15)
		nt = 1;
	for (i = 0; i < nt; i++) {
		for (s = -2; s < 2; s++) {
			for (t = -2; t < 2; t++)
				DoVision({ s + trigs[i]._t.x, t + trigs[i]._t.y }, 15, false, false);
		}
	}
	monsters.PlaceQuestMonsters();
	if (!lvl.setlevel) {
		PlaceUniques();
		na = 0;
		for (s = 16; s < 96; s++)
			for (t = 16; t < 96; t++)
				if (!grid.at({ s, t }).isSolid())
					na++;
		numplacemonsters = na / 30;
		if (plr.isMultiplayer())
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
			if (lvl.currlevel == 1 || random_(95, 2) == 0) {
				na = 1;
			} else if (lvl.currlevel == 2) {
				na = random_(95, 2) + 2;
			} else {
				na = random_(95, 3) + 3;
			}
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

void PlaceUniques()
{
	int u, mt;
	bool done;

	for (u = 0; UniqMonst[u].mtype != -1; u++) {
		if (UniqMonst[u].mlevel != lvl.currlevel)
			continue;
		done = false;
		for (mt = 0; mt < nummtypes; mt++) {
			if (done)
				break;
			done = (beastiary[mt].data.mtype == UniqMonst[u].mtype);
		}
		mt--;
		if (u == UniqueMonsterType::GARBUD && quests[Q_GARBUD]._qactive == QUEST_NOTAVAIL)
			done = false;
		if (u == UniqueMonsterType::ZHAR && quests[Q_ZHAR]._qactive == QUEST_NOTAVAIL)
			done = false;
		if (u == UniqueMonsterType::SNOTSPIL && quests[Q_LTBANNER]._qactive == QUEST_NOTAVAIL)
			done = false;
		if (u == UniqueMonsterType::LACHDAN && quests[Q_VEIL]._qactive == QUEST_NOTAVAIL)
			done = false;
		if (u == UniqueMonsterType::WARLORD && quests[Q_WARLORD]._qactive == QUEST_NOTAVAIL)
			done = false;
		if (done)
			monsters.PlaceUniqueMonster(u, mt, 8);
	}
}

void SetMapMonsters(uint8_t *pMap, V2Di start)
{
	WORD rw, rh;
	WORD *lm;
	int i, j;
	int mtype;

	beastiary.AddMonsterType(MonsterFlag::golem, 2);
	AddMonster({ 1, 0 }, Dir(0), 0, false);
	AddMonster({ 1, 0 }, Dir(0), 0, false);
	AddMonster({ 1, 0 }, Dir(0), 0, false);
	AddMonster({ 1, 0 }, Dir(0), 0, false);
	if (lvl.setlevel && lvl.setlvlnum == SetLvl::VileBetrayer) {
		beastiary.AddMonsterType(UniqMonst[UniqueMonsterType::LAZURUS].mtype, 4);
		beastiary.AddMonsterType(UniqMonst[UniqueMonsterType::RED_VEX].mtype, 4);
		beastiary.AddMonsterType(UniqMonst[UniqueMonsterType::BLACKJADE].mtype, 4);
		monsters.PlaceUniqueMonster(UniqueMonsterType::LAZURUS, 0, 0);
		monsters.PlaceUniqueMonster(UniqueMonsterType::RED_VEX, 0, 0);
		monsters.PlaceUniqueMonster(UniqueMonsterType::BLACKJADE, 0, 0);
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

int AddMonster(V2Di pos, Dir dir, int mtype, bool InMap)
{
	if (nummonsters < MAXMONSTERS) {
		int i = monstactive[nummonsters++];
		if (InMap) grid.at(pos).setActor(i);
		monsters[i] = Monster(i, dir, mtype, pos);
		return i;
	}
	return -1;
}



void M2MStartHit(int mid, int i, int dam)
{
	if ((DWORD)mid >= MAXMONSTERS) app_fatal("Invalid monster %d getting hit by monster", mid);
	if (monsters[mid].data.MType == NULL) app_fatal("Monster %d \"%s\" getting hit by monster: MType NULL", mid, monsters[mid].data.mName);
	if (i >= 0) monsters[i].data.mWhoHit |= 1 << i;

	delta_monster_hp(mid, monsters[mid].data._mhitpoints, lvl.currlevel);
	NetSendCmdParam2(false, CMD_MONSTDAMAGE, mid, dam);
	PlayEffect(mid, 1);

	if (monsters[mid].data.MType->mtype >= MonsterType::SNEAK && monsters[mid].data.MType->mtype <= MonsterType::ILLWEAV || dam >> 6 >= monsters[mid].data.mLevel + 3) {
		if (i >= 0)
			monsters[mid].data._mdir = rotate(monsters[i].data._mdir, -4);

		if (monsters[mid].data.MType->mtype == MonsterType::BLINK) {
			monsters[mid].M_Teleport();
		} else if (monsters[mid].data.MType->mtype >= MonsterType::NSCAV && monsters[mid].data.MType->mtype <= MonsterType::YSCAV) {
			monsters[mid].data._mgoal = MonsterGoal::NORMAL;
		}

		if (monsters[mid].data._mmode != MonsterMode::STONE) {
			if (monsters[mid].data.MType->mtype != MonsterFlag::golem) {
				monsters[mid].NewMonsterAnim(&monsters[mid].data.MType->Anims[MonstAnim::GOTHIT], monsters[mid].data._mdir);
				monsters[mid].data._mmode = MonsterMode::GOTHIT;
			}

			monsters[mid].PlantInPosition(monsters[mid].data._mdir, &monsters[mid].data._mold);
			monsters[mid].M_ClearSquares();
			grid.at(monsters[mid].data._m).setActor(mid);
		}
	}
}



void Monster::M_UpdateLeader()
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
	if (plr.isMultiplayer()) SNetLeaveGame(0x40000004);

	music_stop();

	if (plr.isMultiplayer()) SDL_Delay(1000);

	if (myplr().data._pClass == PlayerClass::warrior) {
		play_movie("gendata\\DiabVic2.smk", false);
	} else if (myplr().data._pClass == PlayerClass::sorceror) {
		play_movie("gendata\\DiabVic1.smk", false);
	} else {
		play_movie("gendata\\DiabVic3.smk", false);
	}
	play_movie("gendata\\Diabend.smk", false);

	bool bMusicOn = gbMusicOn;
	gbMusicOn = true;

	int musicVolume = sound_get_or_set_music_volume(1);
	sound_get_or_set_music_volume(0);

	music_start(TMUSIC_L2);
	loop_movie = true;
	play_movie("gendata\\loopdend.smk", true);
	loop_movie = false;
	music_stop();

	sound_get_or_set_music_volume(musicVolume);
	gbMusicOn = bMusicOn;
}

void PrepDoEnding()
{
	int newKillLevel, i;
	DWORD *killLevel;

	gbSoundOn = sgbSaveSoundOn;
	gbRunGame = false;
	deathflag = false;
	cineflag = true;

	killLevel = &myplr().data.pDiabloKillLevel;
	newKillLevel = gnDifficulty + 1;
	if (*killLevel > newKillLevel)
		newKillLevel = *killLevel;
	myplr().data.pDiabloKillLevel = newKillLevel;

	for (i = 0; i < MAX_PLRS; i++) {
		plr[i].data._pmode = PlayerMode::QUIT;
		plr[i].data._pInvincible = true;
		if (plr.isMultiplayer()) {
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
			monsters[i].data._mDelFlag = false;
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
	bool raflag;
	MonsterStruct *Monst;

	DeleteMonsterList();

	assert((DWORD)nummonsters <= MAXMONSTERS);
	for (i = 0; i < nummonsters; i++) {
		mi = monstactive[i];
		Monst = &monsters[mi].data;
		raflag = false;
		if (plr.isMultiplayer()) {
			SetRndSeed(Monst->_mAISeed);
			Monst->_mAISeed = GetRndSeed();
		}
		if (!(Monst->_mFlags & MonsterFlag::noheal) && Monst->_mhitpoints < Monst->_mmaxhp && Monst->_mhitpoints >> 6 > 0) {
			if (Monst->mLevel > 1) {
				Monst->_mhitpoints += Monst->mLevel >> 1;
			} else {
				Monst->_mhitpoints += Monst->mLevel;
			}
		}
		m = Monst->_m;
		if (grid.at(m).dFlags & DunTileFlag::VISIBLE && Monst->_msquelch == 0) {
			if (Monst->MType->mtype == MonsterType::CLEAVER) {
				PlaySFX(USFX_CLEAVER);
			}
		}
		if (Monst->_mFlags & MonsterFlag::targets_monster) {
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
			Monst->_menemypos = plr[Monst->_menemy].futpos();
			if (grid.at(m).dFlags & DunTileFlag::VISIBLE) {
				Monst->_msquelch = 255;
				Monst->_last = plr[Monst->_menemy].futpos();
			} else if (Monst->_msquelch != 0 && Monst->_mAi != MonsterType::DIABLO) { /// BUGFIX: change '_mAi' to 'MType->mtype'
				Monst->_msquelch--;
			}
		}
		do {
			if (!(Monst->_mFlags & MonsterFlag::search)) {
				std::invoke(AiProc[Monst->_mAi], &monsters[mi]);
				//AiProc[Monst->_mAi](monsters[mi]);
			} else if (!monsters[mi].MMonstAi::Path()) {
				//AiProc[Monst->_mAi](monsters[mi]);
				std::invoke(AiProc[Monst->_mAi], &monsters[mi]);
			}
			switch (Monst->_mmode) {
			case MonsterMode::STAND:
				raflag = monsters[mi].M_DoStand();
				break;
			case MonsterMode::WALK:
				raflag = monsters[mi].M_DoWalk();
				break;
			case MonsterMode::WALK2:
				raflag = monsters[mi].M_DoWalk2();
				break;
			case MonsterMode::WALK3:
				raflag = monsters[mi].M_DoWalk3();
				break;
			case MonsterMode::ATTACK:
				raflag = monsters[mi].M_DoAttack();
				break;
			case MonsterMode::GOTHIT:
				raflag = monsters[mi].M_DoGotHit();
				break;
			case MonsterMode::DEATH:
				raflag = monsters[mi].M_DoDeath();
				break;
			case MonsterMode::SATTACK:
				raflag = monsters[mi].M_DoSAttack();
				break;
			case MonsterMode::FADEIN:
				raflag = monsters[mi].M_DoFadein();
				break;
			case MonsterMode::FADEOUT:
				raflag = monsters[mi].M_DoFadeout();
				break;
			case MonsterMode::RATTACK:
				raflag = monsters[mi].M_DoRAttack();
				break;
			case MonsterMode::SPSTAND:
				raflag = monsters[mi].M_DoSpStand();
				break;
			case MonsterMode::RSPATTACK:
				raflag = monsters[mi].M_DoRSpAttack();
				break;
			case MonsterMode::DELAY:
				raflag = monsters[mi].M_DoDelay();
				break;
			case MonsterMode::CHARGE:
				raflag = false;
				break;
			case MonsterMode::STONE:
				raflag = monsters[mi].M_DoStone();
				break;
			case MonsterMode::HEAL:
				raflag = monsters[mi].M_DoHeal();
				break;
			case MonsterMode::TALK:
				raflag = monsters[mi].M_DoTalk();
				break;
			}
			if (raflag) {
				monsters[mi].GroupUnity();
			}
		} while (raflag);
		if (Monst->_mmode != MonsterMode::STONE) {
			Monst->_mAnimCnt++;
			if (!(Monst->_mFlags & MonsterFlag::allow_special) && Monst->_mAnimCnt >= Monst->_mAnimDelay) {
				Monst->_mAnimCnt = 0;
				if (Monst->_mFlags & MonsterFlag::lock_animation) {
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

bool Monster::DirOK(Dir mdir)
{
	V2Di p;
	int mcount, mi;
	V2Di f = data._m + offset(mdir);
	if (f.y < 0 || f.y >= MAXDUNY || f.x < 0 || f.x >= MAXDUNX || !PosOkMonst(i, f))
		return false;
	if (mdir == Dir::E) {
		if (grid.at({ f.x, f.y + 1 }).isSolid() || grid[f.x][f.y + 1].dFlags & DunTileFlag::MONSTLR)
			return false;
	}
	if (mdir == Dir::W) {
		if (grid.at({ f.x + 1, f.y }).isSolid() || grid[f.x + 1][f.y].dFlags & DunTileFlag::MONSTLR)
			return false;
	}
	if (mdir == Dir::N) {
		if (grid.at({ f.x + 1, f.y }).isSolid() || grid.at({ f.x, f.y + 1 }).isSolid())
			return false;
	}
	if (mdir == Dir::S)
		if (grid.at({ f.x - 1, f.y }).isSolid() || grid.at({ f.x, f.y - 1 }).isSolid())
			return false;
	if (data.leaderflag == 1) {
		int dist = (f - monsters[data.leader].data._mfut).maxabs();
		if (dist >= 4) {
			return false;
		}
		return true;
	}
	if (data._uniqtype == 0 || !(UniqMonst[data._uniqtype - 1].mUnqAttr & 2))
		return true;
	mcount = 0;
	for (int x = f.x - 3; x <= f.x + 3; x++) {
		for (int y = f.y - 3; y <= f.y + 3; y++) {
			if (y < 0 || y >= MAXDUNY || x < 0 || x >= MAXDUNX)
				continue;
			mi = grid[x][y].getActor();
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

bool PosOkMissile(V2Di pos)
{
	return !grid.at(pos).blocksMissile() && !(grid.at(pos).dFlags & DunTileFlag::MONSTLR);
}

bool CheckNoSolid(V2Di pos)
{
	return pieces[grid.at(pos).getPiece()].solid == false;
}

bool LineClearF(bool (*Clear)(V2Di), V2Di p1, V2Di p2)
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

bool LineClear(V2Di p1, V2Di p2)
{
	return LineClearF(PosOkMissile, p1, p2);
}

bool LineClearF1(bool (*Clear)(int, V2Di), int monst, V2Di p1, V2Di p2)
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

void Monster::SyncMonsterAnim()
{
	MonsterData *MData;
	Dir _mdir;
	data.MType->mtype = beastiary[data._mMTidx].data.mtype;
	MData = beastiary[data._mMTidx].data.MData;
	data.MData = MData;
	if (data._uniqtype != 0)
		data.mName = UniqMonst[data._uniqtype - 1].mName;
	else
		data.mName = MData->mName;
	_mdir = data._mdir;

	switch (data._mmode) {
	case MonsterMode::WALK:
	case MonsterMode::WALK2:
	case MonsterMode::WALK3:
		data._mAnimData = data.MType->Anims[MonstAnim::WALK].Data[int(_mdir)];
		return;
	case MonsterMode::ATTACK:
	case MonsterMode::RATTACK:
		data._mAnimData = data.MType->Anims[MonstAnim::ATTACK].Data[int(_mdir)];
		return;
	case MonsterMode::GOTHIT:
		data._mAnimData = data.MType->Anims[MonstAnim::GOTHIT].Data[int(_mdir)];
		return;
	case MonsterMode::DEATH:
		data._mAnimData = data.MType->Anims[MonstAnim::DEATH].Data[int(_mdir)];
		return;
	case MonsterMode::SATTACK:
	case MonsterMode::FADEIN:
	case MonsterMode::FADEOUT:
		data._mAnimData = data.MType->Anims[MonstAnim::SPECIAL].Data[int(_mdir)];
		return;
	case MonsterMode::SPSTAND:
	case MonsterMode::RSPATTACK:
		data._mAnimData = data.MType->Anims[MonstAnim::SPECIAL].Data[int(_mdir)];
		return;
	case MonsterMode::HEAL:
		data._mAnimData = data.MType->Anims[MonstAnim::SPECIAL].Data[int(_mdir)];
		return;
	case MonsterMode::STAND:
		data._mAnimData = data.MType->Anims[MonstAnim::STAND].Data[int(_mdir)];
		return;
	case MonsterMode::DELAY:
		data._mAnimData = data.MType->Anims[MonstAnim::STAND].Data[int(_mdir)];
		return;
	case MonsterMode::TALK:
		data._mAnimData = data.MType->Anims[MonstAnim::STAND].Data[int(_mdir)];
		return;
	case MonsterMode::CHARGE:
		data._mAnimData = data.MType->Anims[MonstAnim::ATTACK].Data[int(_mdir)];
		data._mAnimFrame = 1;
		data._mAnimLen = data.MType->Anims[MonstAnim::ATTACK].Frames;
		break;
	default:
		data._mAnimData = data.MType->Anims[MonstAnim::STAND].Data[int(_mdir)];
		data._mAnimFrame = 1;
		data._mAnimLen = data.MType->Anims[MonstAnim::STAND].Frames;
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
		case MonsterType::RFALLSP:
		case MonsterType::RFALLSD:
			rundist = 7;
			break;
		case MonsterType::DFALLSP:
		case MonsterType::DFALLSD:
			rundist = 5;
			break;
		case MonsterType::YFALLSP:
		case MonsterType::YFALLSD:
			rundist = 3;
			break;
		case MonsterType::BFALLSP:
		case MonsterType::BFALLSD:
			rundist = 2;
			break;
		}
		aitype = monsters[mi].data._mAi;
		if (aitype == MonstAi::FALLEN
		    && rundist
		    && ::abs(pos.x - monsters[mi].data._m.x) < 5
		    && ::abs(pos.y - monsters[mi].data._m.y) < 5
		    && monsters[mi].data._mhitpoints >> 6 > 0) {
			monsters[mi].data._mgoal = MonsterGoal::RETREAT;
			monsters[mi].data._mgoalvar1 = rundist;
			monsters[mi].data._mdir = GetDirection(pos, monsters[i].data._m);
		}
	}
}

void PrintMonstHistory(int mt)
{
	int minHP, maxHP, res;

	sprintf(tempstr, "Total kills : %i", monstkills[mt]);
	AddPanelString(tempstr, true);
	if (monstkills[mt] >= 30) {
		minHP = monsterdata[mt].mMinHP;
		maxHP = monsterdata[mt].mMaxHP;
		if (plr.isSingleplayer()) {
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
		AddPanelString(tempstr, true);
	}
	if (monstkills[mt] >= 15) {
		if (gnDifficulty != DIFF_HELL)
			res = monsterdata[mt].mMagicRes;
		else
			res = monsterdata[mt].mMagicRes2;
		res = res & (MonsterResist::resist_magic | MonsterResist::resist_fire | MonsterResist::resist_lightning | MonsterResist::imune_magic | MonsterResist::imune_fire | MonsterResist::imune_lightning);
		if (!res) {
			strcpy(tempstr, "No magic resistance");
			AddPanelString(tempstr, true);
		} else {
			if (res & (MonsterResist::resist_magic | MonsterResist::resist_fire | MonsterResist::resist_lightning)) {
				strcpy(tempstr, "Resists : ");
				if (res & MonsterResist::resist_magic)
					strcat(tempstr, "Magic ");
				if (res & MonsterResist::resist_fire)
					strcat(tempstr, "Fire ");
				if (res & MonsterResist::resist_lightning)
					strcat(tempstr, "Lightning ");
				tempstr[strlen(tempstr) - 1] = '\0';
				AddPanelString(tempstr, true);
			}
			if (res & (MonsterResist::imune_magic | MonsterResist::imune_fire | MonsterResist::imune_lightning)) {
				strcpy(tempstr, "Immune : ");
				if (res & MonsterResist::imune_magic)
					strcat(tempstr, "Magic ");
				if (res & MonsterResist::imune_fire)
					strcat(tempstr, "Fire ");
				if (res & MonsterResist::imune_lightning)
					strcat(tempstr, "Lightning ");
				tempstr[strlen(tempstr) - 1] = '\0';
				AddPanelString(tempstr, true);
			}
		}
	}
	pinfoflag = true;
}

void PrintUniqueHistory()
{
	int res = monsters[pcursmonst].data.mMagicRes & (MonsterResist::resist_magic | MonsterResist::resist_fire | MonsterResist::resist_lightning | MonsterResist::imune_magic | MonsterResist::imune_fire | MonsterResist::imune_lightning);
	if (!res) {
		strcpy(tempstr, "No resistances");
		AddPanelString(tempstr, true);
		strcpy(tempstr, "No Immunities");
	} else {
		if (res & (MonsterResist::resist_magic | MonsterResist::resist_fire | MonsterResist::resist_lightning))
			strcpy(tempstr, "Some Magic Resistances");
		else
			strcpy(tempstr, "No resistances");
		AddPanelString(tempstr, true);
		if (res & (MonsterResist::imune_magic | MonsterResist::imune_fire | MonsterResist::imune_lightning)) {
			strcpy(tempstr, "Some Magic Immunities");
		} else {
			strcpy(tempstr, "No Immunities");
		}
	}
	AddPanelString(tempstr, true);
	pinfoflag = true;
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
	grid.at(pos).setActor(m);
	Monst->_mdir = Dir(Miss->_mimfnum);
	Monst->_m = pos;
	monsters[m].M_StartStand(Monst->_mdir);
	if (Monst->MType->mtype < MonsterType::INCIN || Monst->MType->mtype > MonsterType::HELLBURN) {
		if (!(Monst->_mFlags & MonsterFlag::targets_monster))
			monsters[m].M_StartHit(-1, 0);
		else
			M2MStartHit(m, -1, 0);
	} else {
		monsters[m].M_StartFadein(Monst->_mdir, false);
	}

	if (!(Monst->_mFlags & MonsterFlag::targets_monster)) {
		pnum = grid.at(old).getPlayer();
		if (pnum > 0) {
			if (Monst->MType->mtype != MonsterType::GLOOM && (Monst->MType->mtype < MonsterType::INCIN || Monst->MType->mtype > MonsterType::HELLBURN)) {
				monsters[m].M_TryH2HHit(pnum, 500, Monst->mMinDamage2, Monst->mMaxDamage2);
				if (pnum == myplr() && (Monst->MType->mtype < MonsterType::NSNAKE || Monst->MType->mtype > MonsterType::GSNAKE)) {
					if (plr[pnum].data._pmode != 7 && plr[pnum].data._pmode != 8)
						plr[pnum].StartPlrHit(0, true);
					n = old + offset(Monst->_mdir);
					if (PosOkPlayer(pnum, n)) {
						plr[pnum].PlantInPlace(plr[pnum].data._pdir);
						plr[pnum].FixPlrWalkTags();
						plr[pnum].changePos(n);
						plr[pnum].SetPlayerOld();
					}
				}
			}
		}
	} else {
		if (grid.at(old).getActor() > 0) {
			if (Monst->MType->mtype != MonsterType::GLOOM && (Monst->MType->mtype < MonsterType::INCIN || Monst->MType->mtype > MonsterType::HELLBURN)) {
				monsters[m].M_TryM2MHit(grid.at(old).getActor() - 1, 500, Monst->mMinDamage2, Monst->mMaxDamage2);
				if (Monst->MType->mtype < MonsterType::NSNAKE || Monst->MType->mtype > MonsterType::GSNAKE) {
					n = old + offset(Monst->_mdir);
					if (PosOkMonst(grid.at(old).getActor() - 1, n)) {
						m = grid.at(old).getActor();
						grid.at(n).setActor(m);
						grid.at(old).clearMonster();
						m--;
						monsters[m].data._m = n;
						monsters[m].data._mfut = n;
					}
				}
			}
		}
	}
}

bool PosOkMonst(int i, V2Di pos)
{
	int oi, mi, j;
	bool ret, fire;

	fire = false;
	ret = !grid.at(pos).isSolid() && !grid.at(pos).isPlayer() && !grid.at(pos).isActor();
	if (ret && grid.at(pos).isObject()) {
		oi = grid.at(pos).getObject();
		if (object[oi]._oSolidFlag)
			ret = false;
	}

	if (ret && grid.at(pos).getMissile() && i >= 0) {
		mi = grid.at(pos).getMissile();
		if (mi > 0) {
			if (missile[mi - 1]._mitype == MissileType::FIREWALL) { // BUGFIX: Change 'mi' to 'mi - 1' (fixed)
				fire = true;
			} else {
				for (j = 0; j < nummissiles; j++) {
					if (missile[missileactive[j]]._mitype == MissileType::FIREWALL)
						fire = true;
				}
			}
		}
		if (fire && (!(monsters[i].data.mMagicRes & MonsterResist::imune_fire) || monsters[i].data.MType->mtype == MonsterType::DIABLO))
			ret = false;
	}
	return ret;
}

bool PosOkMonst2(int i, V2Di pos)
{
	int oi, mi, j;
	bool ret, fire;

	fire = false;
	ret = !grid.at(pos).isSolid();
	if (ret && grid.at(pos).isObject()) {
		oi = grid.at(pos).getObject();
		if (object[oi]._oSolidFlag)
			ret = false;
	}

	if (ret && grid.at(pos).getMissile() && i >= 0) {
		mi = grid.at(pos).getMissile();
		if (mi > 0) {
			if (missile[mi - 1]._mitype == MissileType::FIREWALL) { // BUGFIX: Change 'mi' to 'mi - 1' (fixed)
				fire = true;
			} else {
				for (j = 0; j < nummissiles; j++) {
					if (missile[missileactive[j]]._mitype == MissileType::FIREWALL)
						fire = true;
				}
			}
		}
		if (fire && (!(monsters[i].data.mMagicRes & MonsterResist::imune_fire) || monsters[i].data.MType->mtype == MonsterType::DIABLO))
			ret = false;
	}

	return ret;
}

bool PosOkMonst3(int i, V2Di pos)
{
	int j, oi, objtype, mi;
	bool ret, fire, isdoor;

	fire = false;
	ret = true;
	isdoor = false;

	if (ret && grid.at(pos).isObject()) {
		oi = grid.at(pos).getObject();
		objtype = object[oi]._otype;
		isdoor = objtype == ObjectType::L1LDOOR || objtype == ObjectType::L1RDOOR
		    || objtype == ObjectType::L2LDOOR || objtype == ObjectType::L2RDOOR
		    || objtype == ObjectType::L3LDOOR || objtype == ObjectType::L3RDOOR;
		if (object[oi]._oSolidFlag && !isdoor) {
			ret = false;
		}
	}
	if (ret) {
		ret = (!grid.at(pos).isSolid() || isdoor) && !grid.at(pos).isPlayer() && grid.at(pos).getActor() == 0;
	}
	if (ret && grid.at(pos).getMissile() != 0 && i >= 0) {
		mi = grid.at(pos).getMissile();
		if (mi > 0) {
			if (missile[mi - 1]._mitype == MissileType::FIREWALL) { // BUGFIX: Change 'mi' to 'mi - 1' (fixed)
				fire = true;
			} else {
				for (j = 0; j < nummissiles; j++) {
					mi = missileactive[j];
					if (missile[mi]._mitype == MissileType::FIREWALL) {
						fire = true;
					}
				}
			}
		}
		if (fire && (!(monsters[i].data.mMagicRes & MonsterResist::imune_fire) || monsters[i].data.MType->mtype == MonsterType::DIABLO)) {
			ret = false;
		}
	}

	return ret;
}

bool IsSkel(int mt)
{
	return mt >= MonsterType::WSKELAX && mt <= MonsterType::XSKELAX
	    || mt >= MonsterType::WSKELBW && mt <= MonsterType::XSKELBW
	    || mt >= MonsterType::WSKELSD && mt <= MonsterType::XSKELSD;
}

bool IsGoat(int mt)
{
	return mt >= MonsterType::NGOATMC && mt <= MonsterType::GGOATMC
	    || mt >= MonsterType::NGOATBW && mt <= MonsterType::GGOATBW;
}

int M_SpawnSkel(V2Di pos, Dir dir)
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
		int skel = AddMonster(pos, dir, i - 1, true);
		if (skel != -1) monsters[skel].M_StartSpStand(dir);
		return skel;
	}
	return -1;
}

void Monster::ActivateSpawn(V2Di pos, Dir dir)
{
	grid.at(pos).setActor(i);
	data._m = pos;
	data._mfut = pos;
	data._mold = pos;
	M_StartSpStand(dir);
}

bool SpawnSkeleton(int ii, V2Di pos)
{
	V2Di d, n;
	Dir dir;
	int j, k, rs;
	bool savail;
	int monstok[3][3];

	if (ii == -1) return false;

	if (PosOkMonst(-1, pos)) {
		dir = GetDirection(pos, pos); // ??? Gimmick to get random dir?
		monsters[ii].ActivateSpawn(pos, dir);
		return true;
	}

	savail = false;
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
		return false;
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

	d = pos + n - V2Di(1, 1);
	dir = GetDirection(d, pos);
	monsters[ii].ActivateSpawn(d, dir);
	return true;
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
		skel = AddMonster({ 0, 0 }, Dir(0), i - 1, false);
		if (skel != -1)
			monsters[skel].M_StartStand(Dir(0));
		return skel;
	}
	return -1;
}

void Monster::TalktoMonster()
{
	int pnum, itm;
	pnum = data._menemy;
	data._mmode = MonsterMode::TALK;
	if (data._mAi == MonstAi::SNOTSPIL || data._mAi == MonstAi::LACHDAN) {
		if (QuestStatus(Q_LTBANNER) && quests[Q_LTBANNER]._qvar1 == 2 && HasItem(pnum, ItemIndex::BANNER, &itm)) {
			plr[pnum].inventory.RemoveInvItem(itm);
			quests[Q_LTBANNER]._qactive = QUEST_DONE;
			data.mtalkmsg = TEXT_BANNER12;
			data._mgoal = MonsterGoal::INQUIRING;
		}
		if (QuestStatus(Q_VEIL) && data.mtalkmsg >= TEXT_VEIL9) {
			if (HasItem(pnum, ItemIndex::GLDNELIX, &itm)) {
				plr[pnum].inventory.RemoveInvItem(itm);
				data.mtalkmsg = TEXT_VEIL11;
				data._mgoal = MonsterGoal::INQUIRING;
			}
		}
	}
}

void Monster::SpawnGolum(V2Di pos, int mi)
{
	grid.at(pos).setActor(i);
	data._m = pos;
	data._mfut = pos;
	data._mold = pos;
	data._pathcount = 0;
	data._mFlags |= MonsterFlag::golem;
	data.mArmorClass = 25;
	data._mmaxhp = 2 * (320 * missile[mi]._mispllvl + plr[i].data._pMaxMana / 3);
	data._mhitpoints = data._mmaxhp;
	data.mHit = 5 * (missile[mi]._mispllvl + 8) + 2 * plr[i].data._pLevel;
	data.mMinDamage = 2 * (missile[mi]._mispllvl + 4);
	data.mMaxDamage = 2 * (missile[mi]._mispllvl + 8);
	M_StartSpStand(Dir(0));
	M_Enemy();
	if (i == myplr()) {
		NetSendCmdGolem(
		    data._m,
		    int(data._mdir),
		    data._menemy,
		    data._mhitpoints,
		    lvl.currlevel);
	}
}

bool CanTalkToMonst(int m)
{
	if ((DWORD)m >= MAXMONSTERS) {
		app_fatal("CanTalkToMonst: Invalid monster %d", m);
	}

	if (monsters[m].data._mgoal == MonsterGoal::INQUIRING) {
		return true;
	}

	return monsters[m].data._mgoal == MonsterGoal::TALKING;
}

bool CheckMonsterHit(int m, bool *ret)
{
	if ((DWORD)m >= MAXMONSTERS) {
		app_fatal("CheckMonsterHit: Invalid monster %d", m);
	}

	if (monsters[m].data._mAi == MonstAi::GARG && monsters[m].data._mFlags & MonsterFlag::allow_special) {
		monsters[m].data._mFlags &= ~MonsterFlag::allow_special;
		monsters[m].data._mmode = MonsterMode::SATTACK;
		*ret = true;
		return true;
	}

	if (monsters[m].data.MType->mtype >= MonsterType::COUNSLR && monsters[m].data.MType->mtype <= MonsterType::ADVOCATE) {
		if (monsters[m].data._mgoal != MonsterGoal::NORMAL) {
			*ret = false;
			return true;
		}
	}

	return false;
}

int encode_enemy(int m)
{
	if (monsters[m].data._mFlags & MonsterFlag::targets_monster)
		return monsters[m].data._menemy + MAX_PLRS;
	else
		return monsters[m].data._menemy;
}

void decode_enemy(int m, int enemy)
{
	if (enemy < MAX_PLRS) {
		monsters[m].data._mFlags &= ~MonsterFlag::targets_monster;
		monsters[m].data._menemy = enemy;
		monsters[m].data._menemypos = plr[enemy].futpos();
	} else {
		monsters[m].data._mFlags |= MonsterFlag::targets_monster;
		enemy -= MAX_PLRS;
		monsters[m].data._menemy = enemy;
		monsters[m].data._menemypos = monsters[enemy].data._mfut;
	}
}

DEVILUTION_END_NAMESPACE
