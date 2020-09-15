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

MonsterCatalog beastiary;
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

void (dvl::MonsterInstance:: *AiProc[])() = {
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

MonsterInstance::MonsterInstance(int new_i, int rd, int mtype, int x, int y)
    : i(new_i)
{
	CMonster *monst = &beastiary[mtype].data;

	data._mdir = rd;
	data._mx = x;
	data._my = y;
	data._mfutx = x;
	data._mfuty = y;
	data._moldx = x;
	data._moldy = y;
	data._mMTidx = mtype;
	data._mmode = MM_STAND;
	data.mName = monst->MData->mName;
	data.MType = monst;
	data.MData = monst->MData;
	data._mAnimData = monst->Anims[MA_STAND].Data[rd];
	data._mAnimDelay = monst->Anims[MA_STAND].Rate;
	data._mAnimCnt = random_(88, data._mAnimDelay - 1);
	data._mAnimLen = monst->Anims[MA_STAND].Frames;
	data._mAnimFrame = random_(88, data._mAnimLen - 1) + 1;

	if (monst->mtype == MT_DIABLO) {
		data._mmaxhp = (random_(88, 1) + 1666) << 6;
	} else {
		data._mmaxhp = (monst->mMinHP + random_(88, monst->mMaxHP - monst->mMinHP + 1)) << 6;
	}

	if (gbMaxPlayers == 1) {
		data._mmaxhp >>= 1;
		if (data._mmaxhp < 64) {
			data._mmaxhp = 64;
		}
	}

	data._mhitpoints = data._mmaxhp;
	data._mAi = monst->MData->mAi;
	data._mint = monst->MData->mInt;
	data._mgoal = MGOAL_NORMAL;
	data._mgoalvar1 = 0;
	data._mgoalvar2 = 0;
	data._mgoalvar3 = 0;
	data.field_18 = 0;
	data._pathcount = 0;
	data._mDelFlag = FALSE;
	data._uniqtype = 0;
	data._msquelch = 0;
	data._mRndSeed = GetRndSeed();
	data._mAISeed = GetRndSeed();
	data.mWhoHit = 0;
	data.mLevel = monst->MData->mLevel;
	data.mExp = monst->MData->mExp;
	data.mHit = monst->MData->mHit;
	data.mMinDamage = monst->MData->mMinDamage;
	data.mMaxDamage = monst->MData->mMaxDamage;
	data.mHit2 = monst->MData->mHit2;
	data.mMinDamage2 = monst->MData->mMinDamage2;
	data.mMaxDamage2 = monst->MData->mMaxDamage2;
	data.mArmorClass = monst->MData->mArmorClass;
	data.mMagicRes = monst->MData->mMagicRes;
	data.leader = 0;
	data.leaderflag = 0;
	data._mFlags = monst->MData->mFlags;
	data.mtalkmsg = 0;

	if (data._mAi == AI_GARG) {
		data._mAnimData = monst->Anims[MA_SPECIAL].Data[rd];
		data._mAnimFrame = 1;
		data._mFlags |= MFLAG_ALLOW_SPECIAL;
		data._mmode = MM_SATTACK;
	}

	if (gnDifficulty == DIFF_NIGHTMARE) {
		data._mmaxhp = 3 * data._mmaxhp + 64;
		data._mhitpoints = data._mmaxhp;
		data.mLevel += 15;
		data.mExp = 2 * (data.mExp + 1000);
		data.mHit += 85;
		data.mMinDamage = 2 * (data.mMinDamage + 2);
		data.mMaxDamage = 2 * (data.mMaxDamage + 2);
		data.mHit2 += 85;
		data.mMinDamage2 = 2 * (data.mMinDamage2 + 2);
		data.mMaxDamage2 = 2 * (data.mMaxDamage2 + 2);
		data.mArmorClass += 50;
	}

	if (gnDifficulty == DIFF_HELL) {
		data._mmaxhp = 4 * data._mmaxhp + 192;
		data._mhitpoints = data._mmaxhp;
		data.mLevel += 30;
		data.mExp = 4 * (data.mExp + 1000);
		data.mHit += 120;
		data.mMinDamage = 4 * data.mMinDamage + 6;
		data.mMaxDamage = 4 * data.mMaxDamage + 6;
		data.mHit2 += 120;
		data.mMinDamage2 = 4 * data.mMinDamage2 + 6;
		data.mMaxDamage2 = 4 * data.mMaxDamage2 + 6;
		data.mArmorClass += 80;
		data.mMagicRes = monst->MData->mMagicRes2;
	}
}

void MonsterType::InitMonsterTRN(BOOL special)
{
	BYTE *f;
	int i, n, j;

	f = data.trans_file;
	for (i = 0; i < 256; i++) {
		if (*f == 255) {
			*f = 0;
		}
		f++;
	}

	n = special ? 6 : 5;
	for (i = 0; i < n; i++) {
		if (i != 1 || data.mtype < MT_COUNSLR || data.mtype > MT_ADVOCATE) {
			for (j = 0; j < 8; j++) {
				Cl2ApplyTrans(
				    data.Anims[i].Data[j],
				    data.trans_file,
				    data.Anims[i].Frames);
			}
		}
	}
}

void MonsterType::InitMonsterGFX()
{
	int mtype, anim, i;
	char strBuff[256];
	BYTE *celBuf;

	mtype = data.mtype;

	for (anim = 0; anim < 6; anim++) {
		if ((animletter[anim] != 's' || monsterdata[mtype].has_special) && monsterdata[mtype].Frames[anim] > 0) {
			sprintf(strBuff, monsterdata[mtype].GraphicType, animletter[anim]);

			celBuf = LoadFileInMem(strBuff, NULL);
			data.Anims[anim].CMem = celBuf;

			if (data.mtype != MT_GOLEM || (animletter[anim] != 's' && animletter[anim] != 'd')) {

				for (i = 0; i < 8; i++) {
					data.Anims[anim].Data[i] = CelGetFrameStart(celBuf, i);
				}
			} else {
				for (i = 0; i < 8; i++) {
					data.Anims[anim].Data[i] = celBuf;
				}
			}
		}

		// TODO: either the AnimStruct members have wrong naming or the MonsterData ones it seems
		data.Anims[anim].Frames = monsterdata[mtype].Frames[anim];
		data.Anims[anim].Rate = monsterdata[mtype].Rate[anim];
	}

	data.width = monsterdata[mtype].width;
	data.width2 = (monsterdata[mtype].width - 64) >> 1;
	data.mMinHP = monsterdata[mtype].mMinHP;
	data.mMaxHP = monsterdata[mtype].mMaxHP;
	data.has_special = monsterdata[mtype].has_special;
	data.mAFNum = monsterdata[mtype].mAFNum;
	data.MData = &monsterdata[mtype];

	if (monsterdata[mtype].has_trans) {
		data.trans_file = LoadFileInMem(monsterdata[mtype].TransFile, NULL);
		InitMonsterTRN(monsterdata[mtype].has_special);
		MemFreeDbg(data.trans_file);
	}

	if (mtype >= MT_NMAGMA && mtype <= MT_WMAGMA && !(MissileFileFlag & 1)) {
		MissileFileFlag |= 1;
		LoadMissileGFX(MFILE_MAGBALL);
	}
	if (mtype >= MT_STORM && mtype <= MT_MAEL && !(MissileFileFlag & 2)) {
		MissileFileFlag |= 2;
		LoadMissileGFX(MFILE_THINLGHT);
	}
	if (mtype == MT_SUCCUBUS && !(MissileFileFlag & 4)) {
		MissileFileFlag |= 4;
		LoadMissileGFX(MFILE_FLARE);
		LoadMissileGFX(MFILE_FLAREEXP);
	}
	if (mtype == MT_SNOWWICH && !(MissileFileFlag & 0x20)) {
		MissileFileFlag |= 0x20;
		LoadMissileGFX(MFILE_SCUBMISB);
		LoadMissileGFX(MFILE_SCBSEXPB);
	}
	if (mtype == MT_HLSPWN && !(MissileFileFlag & 0x40)) {
		MissileFileFlag |= 0x40;
		LoadMissileGFX(MFILE_SCUBMISD);
		LoadMissileGFX(MFILE_SCBSEXPD);
	}
	if (mtype == MT_SOLBRNR && !(MissileFileFlag & 0x80)) {
		MissileFileFlag |= 0x80;
		LoadMissileGFX(MFILE_SCUBMISC);
		LoadMissileGFX(MFILE_SCBSEXPC);
	}
	if (mtype >= MT_INCIN && mtype <= MT_HELLBURN && !(MissileFileFlag & 8)) {
		MissileFileFlag |= 8;
		LoadMissileGFX(MFILE_KRULL);
	}
	if (mtype >= MT_NACID && mtype <= MT_XACID && !(MissileFileFlag & 0x10)) {
		MissileFileFlag |= 0x10;
		LoadMissileGFX(MFILE_ACIDBF);
		LoadMissileGFX(MFILE_ACIDSPLA);
		LoadMissileGFX(MFILE_ACIDPUD);
	}
	if (mtype == MT_DIABLO) {
		LoadMissileGFX(MFILE_FIREPLAR);
	}
}


void MonsterCatalog::InitLevelMonsters()
{
	int i;

	nummtypes = 0;
	monstimgtot = 0;
	MissileFileFlag = 0;

	for (i = 0; i < MAX_LVLMTYPES; i++) {
		types[i].data.mPlaceFlags = 0;
	}

	monsters.ClrAllMonsters();
	nummonsters = 0;
	totalmonsters = MAXMONSTERS;

	for (i = 0; i < MAXMONSTERS; i++) {
		monstactive[i] = i;
	}

	uniquetrans = 0;
}

int MonsterCatalog::AddMonsterType(int type, int placeflag)
{
	BOOL done = FALSE;
	int i;

	for (i = 0; i < nummtypes && !done; i++) {
		done = beastiary[i].data.mtype == type;
	}

	i--;

	if (!done) {
		i = nummtypes;
		nummtypes++;
		types[i].data.mtype = type;
		monstimgtot += monsterdata[type].mImage;
		types[i].InitMonsterGFX();
		InitMonsterSND(i);
	}

	types[i].data.mPlaceFlags |= placeflag;
	return i;
}

void MonsterCatalog::GetLevelMTypes()
{
	int i;

	// this array is merged with skeltypes down below.
	int typelist[MAXMONSTERS];
	int skeltypes[NUM_MTYPES];

	int minl; // min level
	int maxl; // max level
	char mamask;
	const int numskeltypes = 19;

	int nt; // number of types

#ifdef SPAWN
	mamask = 1; // monster availability mask
#else
	mamask = 3; // monster availability mask
#endif

	AddMonsterType(MT_GOLEM, 2);
	if (level.currlevel == 16) {
		AddMonsterType(MT_ADVOCATE, 1);
		AddMonsterType(MT_RBLACK, 1);
		AddMonsterType(MT_DIABLO, 2);
		return;
	}

	if (!level.setlevel) {
		if (QuestStatus(Q_BUTCHER))
			AddMonsterType(MT_CLEAVER, 2);
		if (QuestStatus(Q_GARBUD))
			AddMonsterType(UniqMonst[UMT_GARBUD].mtype, 4);
		if (QuestStatus(Q_ZHAR))
			AddMonsterType(UniqMonst[UMT_ZHAR].mtype, 4);
		if (QuestStatus(Q_LTBANNER))
			AddMonsterType(UniqMonst[UMT_SNOTSPIL].mtype, 4);
		if (QuestStatus(Q_VEIL))
			AddMonsterType(UniqMonst[UMT_LACHDAN].mtype, 4);
		if (QuestStatus(Q_WARLORD))
			AddMonsterType(UniqMonst[UMT_WARLORD].mtype, 4);

		if (gbMaxPlayers != 1 && level.currlevel == quests[Q_SKELKING]._qlevel) {

			AddMonsterType(MT_SKING, 4);

			nt = 0;
			for (i = MT_WSKELAX; i <= MT_WSKELAX + numskeltypes; i++) {
				if (IsSkel(i)) {
					minl = 15 * monsterdata[i].mMinDLvl / 30 + 1;
					maxl = 15 * monsterdata[i].mMaxDLvl / 30 + 1;

					if (level.currlevel >= minl && level.currlevel <= maxl) {
						if (MonstAvailTbl[i] & mamask) {
							skeltypes[nt++] = i;
						}
					}
				}
			}
			AddMonsterType(skeltypes[random_(88, nt)], 1);
		}

		nt = 0;
		for (i = 0; i < NUM_MTYPES; i++) {
			minl = 15 * monsterdata[i].mMinDLvl / 30 + 1;
			maxl = 15 * monsterdata[i].mMaxDLvl / 30 + 1;

			if (level.currlevel >= minl && level.currlevel <= maxl) {
				if (MonstAvailTbl[i] & mamask) {
					typelist[nt++] = i;
				}
			}
		}

		if (monstdebug) {
			for (i = 0; i < debugmonsttypes; i++)
				AddMonsterType(DebugMonsters[i], 1);
		} else {

			while (nt > 0 && nummtypes < MAX_LVLMTYPES && monstimgtot < 4000) {
				for (i = 0; i < nt;) {
					if (monsterdata[typelist[i]].mImage > 4000 - monstimgtot) {
						typelist[i] = typelist[--nt];
						continue;
					}

					i++;
				}

				if (nt != 0) {
					i = random_(88, nt);
					AddMonsterType(typelist[i], 1);
					typelist[i] = typelist[--nt];
				}
			}
		}

	} else {
		if (level.setlvlnum == SL_SKELKING) {
			AddMonsterType(MT_SKING, 4);
		}
	}
}

void MonsterInstance::ClearMVars()
{
	data._mVar1 = 0;
	data._mVar2 = 0;
	data._mVar3 = 0;
	data._mVar4 = 0;
	data._mVar5 = 0;
	data._mVar6 = 0;
	data._mVar7 = 0;
	data._mVar8 = 0;
}

void MonsterManager::ClrAllMonsters()
{
	int i;
	MonsterStruct *Monst;

	for (i = 0; i < MAXMONSTERS; i++) {
		Monst = &list[i].data;
		list[i].ClearMVars();
		Monst->mName = "Invalid Monster";
		Monst->_mgoal = 0;
		Monst->_mmode = MM_STAND;
		Monst->_mVar1 = 0;
		Monst->_mVar2 = 0;
		Monst->_mx = 0;
		Monst->_my = 0;
		Monst->_mfutx = 0;
		Monst->_mfuty = 0;
		Monst->_moldx = 0;
		Monst->_moldy = 0;
		Monst->_mdir = random_(89, 8);
		Monst->_mxvel = 0;
		Monst->_myvel = 0;
		Monst->_mAnimData = NULL;
		Monst->_mAnimDelay = 0;
		Monst->_mAnimCnt = 0;
		Monst->_mAnimLen = 0;
		Monst->_mAnimFrame = 0;
		Monst->_mFlags = 0;
		Monst->_mDelFlag = FALSE;
		Monst->_menemy = random_(89, gbActivePlayers);
		Monst->_menemyx = plr[Monst->_menemy].data._pfutx;
		Monst->_menemyy = plr[Monst->_menemy].data._pfuty;
	}
}

BOOL MonstPlace(int xp, int yp)
{
	char f;

	if (xp < 0 || xp >= MAXDUNX
	    || yp < 0 || yp >= MAXDUNY
	    || grid[xp][yp].dMonster
	    || grid[xp][yp].dPlayer) {
		return FALSE;
	}

	f = grid[xp][yp].dFlags;

	if (f & BFLAG_VISIBLE) {
		return FALSE;
	}

	if (f & BFLAG_POPULATED) {
		return FALSE;
	}

	return !SolidLoc(xp, yp);
}

MonsterInstance &MonsterManager::operator[](size_t n)
{
	if ((DWORD)n >= MAXMONSTERS) {
		app_fatal("Invalid monster %d selection", n);
	}

	//if (list[n].data.MType == NULL) {
	//	app_fatal("Monster %d \"%s\" getting hit by monster: MType NULL", n, monsters[n].data.mName);
	//}

	//if (list[n].data.MData == NULL) {
	//	app_fatal("Monster %d \"%s\" getting hit by monster: MData NULL", n, monsters[n].data.mName);
	//}

	return list[n];
}

void MonsterManager::PlaceMonster(int i, int mtype, int x, int y)
{
	int rd;

	grid[x][y].dMonster = i + 1;
	rd = random_(90, 8);
	list[i] = MonsterInstance(i, rd, mtype, x, y);
}

#ifndef SPAWN
void MonsterManager::PlaceUniqueMonster(int uniqindex, int miniontype, int unpackfilesize)
{
	int xp, yp, x, y, i;
	int uniqtype;
	int count2;
	char filestr[64];
	BOOL zharflag, done;
	UniqMonstStruct *Uniq;
	MonsterStruct *Monst;
	int count;

	//Monst = monster + nummonsters;
	Monst = &monsters[nummonsters].data;
	count = 0;
	Uniq = UniqMonst + uniqindex;

	if ((uniquetrans + 19) << 8 >= LIGHTSIZE) {
		return;
	}

	for (uniqtype = 0; uniqtype < nummtypes; uniqtype++) {
		if (beastiary[uniqtype].data.mtype == UniqMonst[uniqindex].mtype) {
			break;
		}
	}

	while (1) {
		xp = random_(91, 80) + 16;
		yp = random_(91, 80) + 16;
		count2 = 0;
		for (x = xp - 3; x < xp + 3; x++) {
			for (y = yp - 3; y < yp + 3; y++) {
				if (y >= 0 && y < MAXDUNY && x >= 0 && x < MAXDUNX && MonstPlace(x, y)) {
					count2++;
				}
			}
		}

		if (count2 < 9) {
			count++;
			if (count < 1000) {
				continue;
			}
		}

		if (MonstPlace(xp, yp)) {
			break;
		}
	}

	if (uniqindex == UMT_SNOTSPIL) {
		xp = 2 * setpc_x + 24;
		yp = 2 * setpc_y + 28;
	}
	if (uniqindex == UMT_WARLORD) {
		xp = 2 * setpc_x + 22;
		yp = 2 * setpc_y + 23;
	}
	if (uniqindex == UMT_ZHAR) {
		zharflag = TRUE;
		for (i = 0; i < themeCount; i++) {
			if (i == zharlib && zharflag == TRUE) {
				zharflag = FALSE;
				xp = 2 * themeLoc[i].x + 20;
				yp = 2 * themeLoc[i].y + 20;
			}
		}
	}
	if (gbMaxPlayers == 1) {
		if (uniqindex == UMT_LAZURUS) {
			xp = 32;
			yp = 46;
		}
		if (uniqindex == UMT_RED_VEX) {
			xp = 40;
			yp = 45;
		}
		if (uniqindex == UMT_BLACKJADE) {
			xp = 38;
			yp = 49;
		}
		if (uniqindex == UMT_SKELKING) {
			xp = 35;
			yp = 47;
		}
	} else {
		if (uniqindex == UMT_LAZURUS) {
			xp = 2 * setpc_x + 19;
			yp = 2 * setpc_y + 22;
		}
		if (uniqindex == UMT_RED_VEX) {
			xp = 2 * setpc_x + 21;
			yp = 2 * setpc_y + 19;
		}
		if (uniqindex == UMT_BLACKJADE) {
			xp = 2 * setpc_x + 21;
			yp = 2 * setpc_y + 25;
		}
	}
	if (uniqindex == UMT_BUTCHER) {
		done = FALSE;
		for (yp = 0; yp < MAXDUNY && !done; yp++) {
			for (xp = 0; xp < MAXDUNX && !done; xp++) {
				done = grid[xp][yp].dPiece == 367;
			}
		}
	}

	PlaceMonster(nummonsters, uniqtype, xp, yp);
	Monst->_uniqtype = uniqindex + 1;

	if (Uniq->mlevel) {
		Monst->mLevel = 2 * Uniq->mlevel;
	} else {
		Monst->mLevel += 5;
	}

	Monst->mExp *= 2;
	Monst->mName = Uniq->mName;
	Monst->_mmaxhp = Uniq->mmaxhp << 6;

	if (gbMaxPlayers == 1) {
		Monst->_mmaxhp = Monst->_mmaxhp >> 1;
		if (Monst->_mmaxhp < 64) {
			Monst->_mmaxhp = 64;
		}
	}

	Monst->_mhitpoints = Monst->_mmaxhp;
	Monst->_mAi = Uniq->mAi;
	Monst->_mint = Uniq->mint;
	Monst->mMinDamage = Uniq->mMinDamage;
	Monst->mMaxDamage = Uniq->mMaxDamage;
	Monst->mMinDamage2 = Uniq->mMinDamage;
	Monst->mMaxDamage2 = Uniq->mMaxDamage;
	Monst->mMagicRes = Uniq->mMagicRes;
	Monst->mtalkmsg = Uniq->mtalkmsg;
	Monst->mlid = AddLight(Monst->_mx, Monst->_my, 3);

	if (gbMaxPlayers != 1) {
		if (Monst->_mAi == AI_LAZHELP)
			Monst->mtalkmsg = 0;
		if (Monst->_mAi != AI_LAZURUS || quests[Q_BETRAYER]._qvar1 <= 3) {
			if (Monst->mtalkmsg) {
				Monst->_mgoal = MGOAL_INQUIRING;
			}
		} else {
			Monst->_mgoal = MGOAL_NORMAL;
		}
	} else if (Monst->mtalkmsg)
		Monst->_mgoal = MGOAL_INQUIRING;

	if (gnDifficulty == DIFF_NIGHTMARE) {
		Monst->_mmaxhp = 3 * Monst->_mmaxhp + 64;
		Monst->mLevel += 15;
		Monst->_mhitpoints = Monst->_mmaxhp;
		Monst->mExp = 2 * (Monst->mExp + 1000);
		Monst->mMinDamage = 2 * (Monst->mMinDamage + 2);
		Monst->mMaxDamage = 2 * (Monst->mMaxDamage + 2);
		Monst->mMinDamage2 = 2 * (Monst->mMinDamage2 + 2);
		Monst->mMaxDamage2 = 2 * (Monst->mMaxDamage2 + 2);
	}

	if (gnDifficulty == DIFF_HELL) {
		Monst->_mmaxhp = 4 * Monst->_mmaxhp + 192;
		Monst->mLevel += 30;
		Monst->_mhitpoints = Monst->_mmaxhp;
		Monst->mExp = 4 * (Monst->mExp + 1000);
		Monst->mMinDamage = 4 * Monst->mMinDamage + 6;
		Monst->mMaxDamage = 4 * Monst->mMaxDamage + 6;
		Monst->mMinDamage2 = 4 * Monst->mMinDamage2 + 6;
		Monst->mMaxDamage2 = 4 * Monst->mMaxDamage2 + 6;
	}

	sprintf(filestr, "monstTypes\\monstTypes\\%s.TRN", Uniq->mTrnName);
	LoadFileWithMem(filestr, &pLightTbl[256 * (uniquetrans + 19)]);

	Monst->_uniqtrans = uniquetrans++;

	if (Uniq->mUnqAttr & 4) {
		Monst->mHit = Uniq->mUnqVar1;
		Monst->mHit2 = Uniq->mUnqVar1;
	}
	if (Uniq->mUnqAttr & 8) {
		Monst->mArmorClass = Uniq->mUnqVar1;
	}

	nummonsters++;

	if (Uniq->mUnqAttr & 1) {
		PlaceGroup(miniontype, unpackfilesize, Uniq->mUnqAttr, nummonsters - 1);
	}

	if (Monst->_mAi != AI_GARG) {
		Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Data[Monst->_mdir];
		Monst->_mAnimFrame = random_(88, Monst->_mAnimLen - 1) + 1;
		Monst->_mFlags &= ~MFLAG_ALLOW_SPECIAL;
		Monst->_mmode = MM_STAND;
	}
}

void MonsterManager::PlaceQuestMonsters()
{
	int skeltype;
	BYTE *setp;

	if (!level.setlevel) {
		if (QuestStatus(Q_BUTCHER)) {
			PlaceUniqueMonster(UMT_BUTCHER, 0, 0);
		}

		if (level.currlevel == quests[Q_SKELKING]._qlevel && gbMaxPlayers != 1) {
			skeltype = 0;

			for (skeltype = 0; skeltype < nummtypes; skeltype++) {
				if (IsSkel(beastiary[skeltype].data.mtype)) {
					break;
				}
			}

			PlaceUniqueMonster(UMT_SKELKING, skeltype, 30);
		}

		if (QuestStatus(Q_LTBANNER)) {
			setp = LoadFileInMem("Levels\\L1Data\\Banner1.DUN", NULL);
			SetMapMonsters(setp, 2 * setpc_x, 2 * setpc_y);
			mem_free_dbg(setp);
		}
		if (QuestStatus(Q_BLOOD)) {
			setp = LoadFileInMem("Levels\\L2Data\\Blood2.DUN", NULL);
			SetMapMonsters(setp, 2 * setpc_x, 2 * setpc_y);
			mem_free_dbg(setp);
		}
		if (QuestStatus(Q_BLIND)) {
			setp = LoadFileInMem("Levels\\L2Data\\Blind2.DUN", NULL);
			SetMapMonsters(setp, 2 * setpc_x, 2 * setpc_y);
			mem_free_dbg(setp);
		}
		if (QuestStatus(Q_ANVIL)) {
			setp = LoadFileInMem("Levels\\L3Data\\Anvil.DUN", NULL);
			SetMapMonsters(setp, 2 * setpc_x + 2, 2 * setpc_y + 2);
			mem_free_dbg(setp);
		}
		if (QuestStatus(Q_WARLORD)) {
			setp = LoadFileInMem("Levels\\L4Data\\Warlord.DUN", NULL);
			SetMapMonsters(setp, 2 * setpc_x, 2 * setpc_y);
			mem_free_dbg(setp);
			beastiary.AddMonsterType(UniqMonst[UMT_WARLORD].mtype, 1);
		}
		if (QuestStatus(Q_VEIL)) {
			beastiary.AddMonsterType(UniqMonst[UMT_LACHDAN].mtype, 1);
		}
		if (QuestStatus(Q_ZHAR) && zharlib == -1) {
			quests[Q_ZHAR]._qactive = QUEST_NOTAVAIL;
		}

		if (level.currlevel == quests[Q_BETRAYER]._qlevel && gbMaxPlayers != 1) {
			beastiary.AddMonsterType(UniqMonst[UMT_LAZURUS].mtype, 4);
			beastiary.AddMonsterType(UniqMonst[UMT_RED_VEX].mtype, 4);
			PlaceUniqueMonster(UMT_LAZURUS, 0, 0);
			PlaceUniqueMonster(UMT_RED_VEX, 0, 0);
			PlaceUniqueMonster(UMT_BLACKJADE, 0, 0);
			setp = LoadFileInMem("Levels\\L4Data\\Vile1.DUN", NULL);
			SetMapMonsters(setp, 2 * setpc_x, 2 * setpc_y);
			mem_free_dbg(setp);
		}
	} else if (level.setlvlnum == SL_SKELKING) {
		PlaceUniqueMonster(UMT_SKELKING, 0, 0);
	}
}
#endif

void MonsterManager::PlaceGroup(int mtype, int num, int leaderf, int leader)
{
	int placed, try1, try2, j;
	int xp, yp, x1, y1;

	placed = 0;

	for (try1 = 0; try1 < 10; try1++) {
		while (placed) {
			nummonsters--;
			placed--;
			grid[list[nummonsters].data._mx][list[nummonsters].data._my].dMonster = 0;
		}

		if (leaderf & 1) {
			int offset = random_(92, 8);
			x1 = xp = list[leader].data._mx + offset_x[offset];
			y1 = yp = list[leader].data._my + offset_y[offset];
		} else {
			do {
				x1 = xp = random_(93, 80) + 16;
				y1 = yp = random_(93, 80) + 16;
			} while (!MonstPlace(xp, yp));
		}

		if (num + nummonsters > totalmonsters) {
			num = totalmonsters - nummonsters;
		}

		j = 0;
		for (try2 = 0; j < num && try2 < 100; xp += offset_x[random_(94, 8)], yp += offset_x[random_(94, 8)]) { /// BUGFIX: `yp += offset_y`
			if (!MonstPlace(xp, yp)
			    || (grid[xp][yp].dTransVal != grid[x1][y1].dTransVal)
			    || (leaderf & 2) && ((abs(xp - x1) >= 4) || (abs(yp - y1) >= 4))) {
				try2++;
				continue;
			}

			PlaceMonster(nummonsters, mtype, xp, yp);
			if (leaderf & 1) {
				list[nummonsters].data._mmaxhp *= 2;
				list[nummonsters].data._mhitpoints = list[nummonsters].data._mmaxhp;
				list[nummonsters].data._mint = list[leader].data._mint;

				if (leaderf & 2) {
					list[nummonsters].data.leader = leader;
					list[nummonsters].data.leaderflag = 1;
					list[nummonsters].data._mAi = list[leader].data._mAi;
				}

				if (list[nummonsters].data._mAi != AI_GARG) {
					list[nummonsters].data._mAnimData = list[nummonsters].data.MType->Anims[MA_STAND].Data[list[nummonsters].data._mdir];
					list[nummonsters].data._mAnimFrame = random_(88, list[nummonsters].data._mAnimLen - 1) + 1;
					list[nummonsters].data._mFlags &= ~MFLAG_ALLOW_SPECIAL;
					list[nummonsters].data._mmode = MM_STAND;
				}
			}
			nummonsters++;
			placed++;
			j++;
		}

		if (placed >= num) {
			break;
		}
	}

	if (leaderf & 2) {
		list[leader].data.packsize = placed;
	}
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
		AddMonster(1, 0, 0, 0, FALSE);
		AddMonster(1, 0, 0, 0, FALSE);
		AddMonster(1, 0, 0, 0, FALSE);
		AddMonster(1, 0, 0, 0, FALSE);
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
				DoVision(s + trigs[i]._tx, t + trigs[i]._ty, 15, FALSE, FALSE);
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
				DoUnVision(s + trigs[i]._tx, t + trigs[i]._ty, 15);
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
	AddMonster(1, 0, 0, 0, FALSE);
	AddMonster(1, 0, 0, 0, FALSE);
	AddMonster(1, 0, 0, 0, FALSE);
	AddMonster(1, 0, 0, 0, FALSE);
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
				monsters.PlaceMonster(nummonsters++, mtype, i + startx + 16, j + starty + 16);
			}
			lm++;
		}
	}
}
#endif

void MonsterManager::DeleteMonster(int i)
{
	int temp;

	nummonsters--;
	temp = monstactive[nummonsters];
	monstactive[nummonsters] = monstactive[i];
	monstactive[i] = temp;
}

int AddMonster(int x, int y, int dir, int mtype, BOOL InMap)
{
	if (nummonsters < MAXMONSTERS) {
		int i = monstactive[nummonsters++];
		if (InMap)
			grid[x][y].dMonster = i + 1;
		monsters[i] = MonsterInstance(i, dir, mtype, x, y);
		return i;
	}

	return -1;
}

void MonsterInstance::NewMonsterAnim(AnimStruct *anim, int md)
{
	data._mAnimData = anim->Data[md];
	data._mAnimLen = anim->Frames;
	data._mAnimCnt = 0;
	data._mAnimFrame = 1;
	data._mAnimDelay = anim->Rate;
	data._mFlags &= ~(MFLAG_LOCK_ANIMATION | MFLAG_ALLOW_SPECIAL);
	data._mdir = md;
}

BOOL MonsterInstance::M_Ranged()
{
	char ai = data._mAi;
	return ai == AI_SKELBOW || ai == AI_GOATBOW || ai == AI_SUCC || ai == AI_LAZHELP;
}

BOOL MonsterInstance::M_Talker()
{
	char ai = data._mAi;
	return ai == AI_LAZURUS
	    || ai == AI_WARLORD
	    || ai == AI_GARBUD
	    || ai == AI_ZHAR
	    || ai == AI_SNOTSPIL
	    || ai == AI_LACHDAN
	    || ai == AI_LAZHELP;
}

void MonsterInstance::M_Enemy()
{
	int j;
	int mi, pnum;
	int dist, best_dist;
	int _menemy;
	BOOL sameroom, bestsameroom;
	//MonsterStruct *Monst;
	BYTE enemyx, enemyy;

	_menemy = -1;
	best_dist = -1;
	bestsameroom = 0;
	//Monst = monster + i;
	if (!(data._mFlags & MFLAG_GOLEM)) {
		for (pnum = 0; pnum < MAX_PLRS; pnum++) {
			if (!plr[pnum].data.plractive || level.currlevel != plr[pnum].data.plrlevel || plr[pnum].data._pLvlChanging || (plr[pnum].data._pHitPoints == 0 && gbMaxPlayers != 1))
				continue;
			if (grid[data._mx][data._my].dTransVal == grid[plr[pnum].data._px][plr[pnum].data._py].dTransVal)
				sameroom = TRUE;
			else
				sameroom = FALSE;
			if (abs(data._mx - plr[pnum].data._px) > abs(data._my - plr[pnum].data._py))
				dist = data._mx - plr[pnum].data._px;
			else
				dist = data._my - plr[pnum].data._py;
			dist = abs(dist);
			if ((sameroom && !bestsameroom)
			    || ((sameroom || !bestsameroom) && dist < best_dist)
			    || (_menemy == -1)) {
				data._mFlags &= ~MFLAG_TARGETS_MONSTER;
				_menemy = pnum;
				enemyx = plr[pnum].data._pfutx;
				enemyy = plr[pnum].data._pfuty;
				best_dist = dist;
				bestsameroom = sameroom;
			}
		}
	}
	for (j = 0; j < nummonsters; j++) {
		mi = monstactive[j];
		if (mi == i)
			continue;
		if (monsters[mi].data._mx == 1 && monsters[mi].data._my == 0)
			continue;
		if (monsters[mi].M_Talker() && monsters[mi].data.mtalkmsg)
			continue;
		if (!(data._mFlags & MFLAG_GOLEM)
		    && ((abs(monsters[mi].data._mx - data._mx) >= 2 || abs(monsters[mi].data._my - data._my) >= 2) && !M_Ranged()
		        || (!(data._mFlags & MFLAG_GOLEM) && !(monsters[mi].data._mFlags & MFLAG_GOLEM)))) {
			continue;
		}
		sameroom = grid[data._mx][data._my].dTransVal == grid[monsters[mi].data._mx][monsters[mi].data._my].dTransVal;
		if (abs(data._mx - monsters[mi].data._mx) > abs(data._my - monsters[mi].data._my))
			dist = data._mx - monsters[mi].data._mx;
		else
			dist = data._my - monsters[mi].data._my;
		dist = abs(dist);
		if ((sameroom && !bestsameroom)
		    || ((sameroom || !bestsameroom) && dist < best_dist)
		    || (_menemy == -1)) {
			data._mFlags |= MFLAG_TARGETS_MONSTER;
			_menemy = mi;
			enemyx = monsters[mi].data._mfutx;
			enemyy = monsters[mi].data._mfuty;
			best_dist = dist;
			bestsameroom = sameroom;
		}
	}
	if (_menemy != -1) {
		data._mFlags &= ~MFLAG_NO_ENEMY;
		data._menemy = _menemy;
		data._menemyx = enemyx;
		data._menemyy = enemyy;
	} else {
		data._mFlags |= MFLAG_NO_ENEMY;
	}
}

int MonsterInstance::M_GetDir()
{
	return GetDirection(data._mx, data._my, data._menemyx, data._menemyy);
}

void MonsterInstance::M_StartStand(int md)
{
	ClearMVars();
	if (data.MType->mtype == MT_GOLEM)
		NewMonsterAnim(&data.MType->Anims[MA_WALK], md);
	else
		NewMonsterAnim(&data.MType->Anims[MA_STAND], md);
	data._mVar1 = data._mmode;
	data._mVar2 = 0;
	data._mmode = MM_STAND;
	data._mxoff = 0;
	data._myoff = 0;
	data._mfutx = data._mx;
	data._mfuty = data._my;
	data._moldx = data._mx;
	data._moldy = data._my;
	data._mdir = md;
	M_Enemy();
}

void MonsterInstance::M_StartDelay(int len)
{
	if (len <= 0) {
		return;
	}

	if (data._mAi != AI_LAZURUS) {
		data._mVar2 = len;
		data._mmode = MM_DELAY;
	}
}

void MonsterInstance::M_StartSpStand(int md)
{
	NewMonsterAnim(&data.MType->Anims[MA_SPECIAL], md);
	data._mmode = MM_SPSTAND;
	data._mxoff = 0;
	data._myoff = 0;
	data._mfutx = data._mx;
	data._mfuty = data._my;
	data._moldx = data._mx;
	data._moldy = data._my;
	data._mdir = md;
}

void MonsterInstance::M_StartWalk(int xvel, int yvel, int xadd, int yadd, int EndDir)
{
	int fx = xadd + data._mx;
	int fy = yadd + data._my;

	grid[fx][fy].dMonster = -(i + 1);
	data._mmode = MM_WALK;
	data._moldx = data._mx;
	data._moldy = data._my;
	data._mfutx = fx;
	data._mfuty = fy;
	data._mxvel = xvel;
	data._myvel = yvel;
	data._mVar1 = xadd;
	data._mVar2 = yadd;
	data._mVar3 = EndDir;
	data._mdir = EndDir;
	NewMonsterAnim(&data.MType->Anims[MA_WALK], EndDir);
	data._mVar6 = 0;
	data._mVar7 = 0;
	data._mVar8 = 0;
}

void MonsterInstance::M_StartWalk2(int xvel, int yvel, int xoff, int yoff, int xadd, int yadd, int EndDir)
{
	int fx = xadd + data._mx;
	int fy = yadd + data._my;

	grid[data._mx][data._my].dMonster = -(i + 1);
	data._mVar1 = data._mx;
	data._mVar2 = data._my;
	data._moldx = data._mx;
	data._moldy = data._my;
	data._mx = fx;
	data._my = fy;
	data._mfutx = fx;
	data._mfuty = fy;
	grid[fx][fy].dMonster = i + 1;
	if (data._uniqtype != 0)
		ChangeLightXY(data.mlid, data._mx, data._my);
	data._mxoff = xoff;
	data._myoff = yoff;
	data._mmode = MM_WALK2;
	data._mxvel = xvel;
	data._myvel = yvel;
	data._mVar3 = EndDir;
	data._mdir = EndDir;
	NewMonsterAnim(&data.MType->Anims[MA_WALK], EndDir);
	data._mVar6 = 16 * xoff;
	data._mVar7 = 16 * yoff;
	data._mVar8 = 0;
}

void MonsterInstance::M_StartWalk3(int xvel, int yvel, int xoff, int yoff, int xadd, int yadd, int mapx, int mapy, int EndDir)
{
	int fx = xadd + data._mx;
	int fy = yadd + data._my;
	int x = mapx + data._mx;
	int y = mapy + data._my;

	if (data._uniqtype != 0)
		ChangeLightXY(data.mlid, x, y);

	grid[data._mx][data._my].dMonster = -(i + 1);
	grid[fx][fy].dMonster = -(i + 1);
	data._mVar4 = x;
	data._mVar5 = y;
	grid[x][y].dFlags |= BFLAG_MONSTLR;
	data._moldx = data._mx;
	data._moldy = data._my;
	data._mfutx = fx;
	data._mfuty = fy;
	data._mxoff = xoff;
	data._myoff = yoff;
	data._mmode = MM_WALK3;
	data._mxvel = xvel;
	data._myvel = yvel;
	data._mVar1 = fx;
	data._mVar2 = fy;
	data._mVar3 = EndDir;
	data._mdir = EndDir;
	NewMonsterAnim(&data.MType->Anims[MA_WALK], EndDir);
	data._mVar6 = 16 * xoff;
	data._mVar7 = 16 * yoff;
	data._mVar8 = 0;
}

void MonsterInstance::M_StartAttack()
{
	int md = M_GetDir();
	NewMonsterAnim(&data.MType->Anims[MA_ATTACK], md);
	data._mmode = MM_ATTACK;
	data._mxoff = 0;
	data._myoff = 0;
	data._mfutx = data._mx;
	data._mfuty = data._my;
	data._moldx = data._mx;
	data._moldy = data._my;
	data._mdir = md;
}

void MonsterInstance::M_StartRAttack(int missile_type, int dam)
{
	int md = M_GetDir();
	NewMonsterAnim(&data.MType->Anims[MA_ATTACK], md);
	data._mmode = MM_RATTACK;
	data._mVar1 = missile_type;
	data._mVar2 = dam;
	data._mxoff = 0;
	data._myoff = 0;
	data._mfutx = data._mx;
	data._mfuty = data._my;
	data._moldx = data._mx;
	data._moldy = data._my;
	data._mdir = md;
}

void MonsterInstance::M_StartRSpAttack(int missile_type, int dam)
{
	int md = M_GetDir();
	NewMonsterAnim(&data.MType->Anims[MA_SPECIAL], md);
	data._mmode = MM_RSPATTACK;
	data._mVar1 = missile_type;
	data._mVar2 = 0;
	data._mVar3 = dam;
	data._mxoff = 0;
	data._myoff = 0;
	data._mfutx = data._mx;
	data._mfuty = data._my;
	data._moldx = data._mx;
	data._moldy = data._my;
	data._mdir = md;
}

void MonsterInstance::M_StartSpAttack()
{
	int md = M_GetDir();
	NewMonsterAnim(&data.MType->Anims[MA_SPECIAL], md);
	data._mmode = MM_SATTACK;
	data._mxoff = 0;
	data._myoff = 0;
	data._mfutx = data._mx;
	data._mfuty = data._my;
	data._moldx = data._mx;
	data._moldy = data._my;
	data._mdir = md;
}

void MonsterInstance::M_StartEat()
{
	NewMonsterAnim(&data.MType->Anims[MA_SPECIAL], data._mdir);
	data._mmode = MM_SATTACK;
	data._mxoff = 0;
	data._myoff = 0;
	data._mfutx = data._mx;
	data._mfuty = data._my;
	data._moldx = data._mx;
	data._moldy = data._my;
}

void MonsterInstance::M_ClearSquares()
{
	int x, y, mx, my, m1, m2;
	mx = data._moldx;
	my = data._moldy;
	m1 = -1 - i;
	m2 = i + 1;

	for (y = my - 1; y <= my + 1; y++) {
		if (y >= 0 && y < MAXDUNY) {
			for (x = mx - 1; x <= mx + 1; x++) {
				if (x >= 0 && x < MAXDUNX && (grid[x][y].dMonster == m1 || grid[x][y].dMonster == m2))
					grid[x][y].dMonster = 0;
			}
		}
	}

	if (mx + 1 < MAXDUNX)
		grid[mx + 1][my].dFlags &= ~BFLAG_MONSTLR;
	if (my + 1 < MAXDUNY)
		grid[mx][my + 1].dFlags &= ~BFLAG_MONSTLR;
}

void MonsterInstance::M_GetKnockback()
{
	int d = (data._mdir - 4) & 7;
	if (DirOK(d)) {
		M_ClearSquares();
		data._moldx += offset_x[d];
		data._moldy += offset_y[d];
		NewMonsterAnim(&data.MType->Anims[MA_GOTHIT], data._mdir);
		data._mmode = MM_GOTHIT;
		data._mxoff = 0;
		data._myoff = 0;
		data._mx = data._moldx;
		data._my = data._moldy;
		data._mfutx = data._mx;
		data._mfuty = data._my;
		// BUGFIX useless assignment
		data._moldx = data._mx;
		data._moldy = data._my;
		M_ClearSquares();
		grid[data._mx][data._my].dMonster = i + 1;
	}
}

void MonsterInstance::M_StartHit(int pnum, int dam)
{
	if (pnum >= 0)
		data.mWhoHit |= 1 << pnum;
	if (pnum == myplr()) {
		delta_monster_hp(i, data._mhitpoints, level.currlevel);
		NetSendCmdParam2(FALSE, CMD_MONSTDAMAGE, i, dam);
	}
	PlayEffect(i, 1);
	if (data.MType->mtype >= MT_SNEAK && data.MType->mtype <= MT_ILLWEAV || dam >> 6 >= data.mLevel + 3) {
		if (pnum >= 0) {
			data._mFlags &= ~MFLAG_TARGETS_MONSTER;
			data._menemy = pnum;
			data._menemyx = plr[pnum].data._pfutx;
			data._menemyy = plr[pnum].data._pfuty;
			data._mdir = M_GetDir();
		}
		if (data.MType->mtype == MT_BLINK) {
			M_Teleport();
		} else if (data.MType->mtype >= MT_NSCAV && data.MType->mtype <= MT_YSCAV) {
			data._mgoal = MGOAL_NORMAL;
		}
		if (data._mmode != MM_STONE) {
			NewMonsterAnim(&data.MType->Anims[MA_GOTHIT], data._mdir);
			data._mmode = MM_GOTHIT;
			data._mxoff = 0;
			data._myoff = 0;
			data._mx = data._moldx;
			data._my = data._moldy;
			data._mfutx = data._moldx;
			data._mfuty = data._moldy;
			M_ClearSquares();
			grid[data._mx][data._my].dMonster = i + 1;
		}
	}
}

void MonsterInstance::M_DiabloDeath(BOOL sendmsg)
{
	int dist;
	int j, k;
	int _moldx, _moldy;

	#ifndef SPAWN
	PlaySFX(USFX_DIABLOD);
	#endif
	quests[Q_DIABLO]._qactive = QUEST_DONE;
	if (sendmsg)
		NetSendCmdQuest(TRUE, Q_DIABLO);
	gbProcessPlayers = FALSE;
	sgbSaveSoundOn = gbSoundOn;
	for (j = 0; j < nummonsters; j++) {
		k = monstactive[j];
		if (k == i || data._msquelch == 0)
			continue;

		monsters[k].NewMonsterAnim(&monsters[k].data.MType->Anims[MA_DEATH], monsters[k].data._mdir);
		monsters[k].data._mxoff = 0;
		monsters[k].data._myoff = 0;
		monsters[k].data._mVar1 = 0;
		_moldx = monsters[k].data._moldx;
		_moldy = monsters[k].data._moldy;
		monsters[k].data._my = _moldy;
		monsters[k].data._mfuty = _moldy;
		monsters[k].data._mmode = MM_DEATH;
		monsters[k].data._mx = _moldx;
		monsters[k].data._mfutx = _moldx;
		monsters[k].M_ClearSquares();
		grid[monsters[k].data._mx][monsters[k].data._my].dMonster = k + 1;
	}
	AddLight(monsters[k].data._mx, monsters[k].data._my, 8);
	DoVision(monsters[k].data._mx, monsters[k].data._my, 8, FALSE, TRUE);
	if (abs(ViewX - monsters[k].data._mx) > abs(ViewY - monsters[k].data._my))
		dist = abs(ViewX - monsters[k].data._mx);
	else
		dist = abs(ViewY - monsters[k].data._my);
	if (dist > 20)
		dist = 20;
	j = ViewX << 16;
	k = ViewY << 16;
	monsters[k].data._mVar3 = j;
	monsters[k].data._mVar4 = k;
	monsters[k].data._mVar5 = (int)((j - (monsters[k].data._mx << 16)) / (double)dist);
	monsters[k].data._mVar6 = (int)((k - (monsters[k].data._my << 16)) / (double)dist);
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

			monsters[mid].data._mxoff = 0;
			monsters[mid].data._myoff = 0;
			monsters[mid].data._mx = monsters[mid].data._moldx;
			monsters[mid].data._my = monsters[mid].data._moldy;
			monsters[mid].data._mfutx = monsters[mid].data._moldx;
			monsters[mid].data._mfuty = monsters[mid].data._moldy;
			monsters[mid].M_ClearSquares();
			grid[monsters[mid].data._mx][monsters[mid].data._my].dMonster = mid + 1;
		}
	}
}

void MonsterInstance::MonstStartKill(int pnum, BOOL sendmsg)
{
	int md;

	//MonsterStruct *Monst;
	if (pnum >= 0)
		data.mWhoHit |= 1 << pnum;
	if (pnum < MAX_PLRS && i > MAX_PLRS)
		AddPlrMonstExper(data.mLevel, data.mExp, data.mWhoHit);
	monstkills[data.MType->mtype]++;
	data._mhitpoints = 0;
	SetRndSeed(data._mRndSeed);
	if (QuestStatus(Q_GARBUD) && data.mName == UniqMonst[UMT_GARBUD].mName) {
		CreateTypeItem(data._mx + 1, data._my + 1, TRUE, ITYPE_MACE, IMISC_NONE, TRUE, FALSE);
	} else if (i > MAX_PLRS - 1) { // Golems should not spawn items
		SpawnItem(i, data._mx, data._my, sendmsg);
	}
	if (data.MType->mtype == MT_DIABLO)
		M_DiabloDeath(TRUE);
	else
		PlayEffect(i, 2);

	if (pnum >= 0)
		md = M_GetDir();
	else
		md = data._mdir;
	data._mdir = md;
	NewMonsterAnim(&data.MType->Anims[MA_DEATH], md);
	data._mmode = MM_DEATH;
	data._mxoff = 0;
	data._myoff = 0;
	data._mVar1 = 0;
	data._mx = data._moldx;
	data._my = data._moldy;
	data._mfutx = data._moldx;
	data._mfuty = data._moldy;
	M_ClearSquares();
	grid[data._mx][data._my].dMonster = i + 1;
	CheckQuestKill(i, sendmsg);
	M_FallenFear(data._mx, data._my);
	if (data.MType->mtype >= MT_NACID && data.MType->mtype <= MT_XACID)
		AddMissile(data._mx, data._my, 0, 0, 0, MIS_ACIDPUD, 1, i, data._mint + 1, 0);
}

void MonsterInstance::M2MStartKill(int mid)
{
	int md;

	if ((DWORD)i >= MAXMONSTERS) { /// BUGFIX: should check `mid`
		app_fatal("M2MStartKill: Invalid monster (killed) %d", mid);
	}
	if (!data.MType)
		app_fatal("M2MStartKill: Monster %d \"%s\" MType NULL", mid, monsters[mid].data.mName);

	delta_kill_monster(mid, monsters[mid].data._mx, monsters[mid].data._my, level.currlevel);
	NetSendCmdLocParam1(FALSE, CMD_MONSTDEATH, monsters[mid].data._mx, monsters[mid].data._my, mid);

	monsters[mid].data.mWhoHit |= 1 << i;
	if (i < MAX_PLRS)
		AddPlrMonstExper(monsters[mid].data.mLevel, monsters[mid].data.mExp, monsters[mid].data.mWhoHit);

	monstkills[monsters[mid].data.MType->mtype]++;
	monsters[mid].data._mhitpoints = 0;
	SetRndSeed(monsters[mid].data._mRndSeed);

	if (mid >= MAX_PLRS)
		SpawnItem(mid, monsters[mid].data._mx, monsters[mid].data._my, TRUE);

	if (monsters[mid].data.MType->mtype == MT_DIABLO)
		monsters[mid].M_DiabloDeath(TRUE);
	else
		PlayEffect(i, 2);

	PlayEffect(mid, 2);

	md = (data._mdir - 4) & 7;
	if (monsters[mid].data.MType->mtype == MT_GOLEM)
		md = 0;

	monsters[mid].data._mdir = md;
	monsters[mid].NewMonsterAnim(&monsters[mid].data.MType->Anims[MA_DEATH], md);
	monsters[mid].data._mmode = MM_DEATH;
	monsters[mid].data._mxoff = 0;
	monsters[mid].data._myoff = 0;
	monsters[mid].data._mx = monsters[mid].data._moldx;
	monsters[mid].data._my = monsters[mid].data._moldy;
	monsters[mid].data._mfutx = monsters[mid].data._moldx;
	monsters[mid].data._mfuty = monsters[mid].data._moldy;
	monsters[mid].M_ClearSquares();
	grid[monsters[mid].data._mx][monsters[mid].data._my].dMonster = mid + 1;
	CheckQuestKill(mid, TRUE);
	M_FallenFear(monsters[mid].data._mx, monsters[mid].data._my);
	if (monsters[mid].data.MType->mtype >= MT_NACID && monsters[mid].data.MType->mtype <= MT_XACID)
		AddMissile(monsters[mid].data._mx, monsters[mid].data._my, 0, 0, 0, MIS_ACIDPUD, 1, mid, monsters[mid].data._mint + 1, 0);
}

void MonsterInstance::M_StartKill(int pnum)
{

	if (myplr() == pnum) {
		delta_kill_monster(i, data._mx, data._my, level.currlevel);
		if (i != pnum) {
			NetSendCmdLocParam1(FALSE, CMD_MONSTDEATH, data._mx, data._my, i);
		} else {
			NetSendCmdLocParam1(FALSE, CMD_KILLGOLEM, data._mx, data._my, level.currlevel);
		}
	}

	MonstStartKill(pnum, TRUE);
}

void MonsterInstance::M_SyncStartKill(int x, int y, int pnum)
{
	if (data._mhitpoints == 0 || data._mmode == MM_DEATH) {
		return;
	}

	if (grid[x][y].dMonster == 0) {
		M_ClearSquares();
		data._mx = x;
		data._my = y;
		data._moldx = x;
		data._moldy = y;
	}

	if (data._mmode == MM_STONE) {
		MonstStartKill(pnum, FALSE);
		data._mmode = MM_STONE;
	} else {
		MonstStartKill(pnum, FALSE);
	}
}

void MonsterInstance::M_StartFadein(int md, BOOL backwards)
{
	NewMonsterAnim(&data.MType->Anims[MA_SPECIAL], md);
	data._mmode = MM_FADEIN;
	data._mxoff = 0;
	data._myoff = 0;
	data._mfutx = data._mx;
	data._mfuty = data._my;
	data._moldx = data._mx;
	data._moldy = data._my;
	data._mdir = md;
	data._mFlags &= ~MFLAG_HIDDEN;
	if (backwards) {
		data._mFlags |= MFLAG_LOCK_ANIMATION;
		data._mAnimFrame = data._mAnimLen;
	}
}

void MonsterInstance::M_StartFadeout(int md, BOOL backwards)
{
	NewMonsterAnim(&data.MType->Anims[MA_SPECIAL], md);
	data._mmode = MM_FADEOUT;
	data._mxoff = 0;
	data._myoff = 0;
	data._mfutx = data._mx;
	data._mfuty = data._my;
	data._moldx = data._mx;
	data._moldy = data._my;
	data._mdir = md;
	if (backwards) {
		data._mFlags |= MFLAG_LOCK_ANIMATION;
		data._mAnimFrame = data._mAnimLen;
	}
}

void MonsterInstance::M_StartHeal()
{
	data._mAnimData = data.MType->Anims[MA_SPECIAL].Data[data._mdir];
	data._mAnimFrame = data.MType->Anims[MA_SPECIAL].Frames;
	data._mFlags |= MFLAG_LOCK_ANIMATION;
	data._mmode = MM_HEAL;
	data._mVar1 = data._mmaxhp / (16 * (random_(97, 5) + 4));
}

void MonsterInstance::M_ChangeLightOffset()
{
	int lx, ly, _mxoff, _myoff, sign;

	lx = data._mxoff + 2 * data._myoff;
	ly = 2 * data._myoff - data._mxoff;

	if (lx < 0) {
		sign = -1;
		lx = -lx;
	} else {
		sign = 1;
	}

	_mxoff = sign * (lx >> 3);
	if (ly < 0) {
		_myoff = -1;
		ly = -ly;
	} else {
		_myoff = 1;
	}

	_myoff *= (ly >> 3);
	ChangeLightOff(monsters[i].data.mlid, _mxoff, _myoff);
}

BOOL MonsterInstance::M_DoStand()
{
	if (data.MType->mtype == MT_GOLEM)
		data._mAnimData = data.MType->Anims[MA_WALK].Data[data._mdir];
	else
		data._mAnimData = data.MType->Anims[MA_STAND].Data[data._mdir];

	if (data._mAnimFrame == data._mAnimLen)
		M_Enemy();

	data._mVar2++;
	return FALSE;
}

BOOL MonsterInstance::M_DoWalk()
{
	BOOL rv = FALSE;
	if (data._mVar8 == data.MType->Anims[MA_WALK].Frames) {
		grid[data._mx][data._my].dMonster = 0;
		data._mx += data._mVar1;
		data._my += data._mVar2;
		grid[data._mx][data._my].dMonster = i + 1;
		if (data._uniqtype != 0)
			ChangeLightXY(data.mlid, data._mx, data._my);
		M_StartStand(data._mdir);
		rv = TRUE;
	} else if (!data._mAnimCnt) {
		data._mVar8++;
		data._mVar6 += data._mxvel;
		data._mVar7 += data._myvel;
		data._mxoff = data._mVar6 >> 4;
		data._myoff = data._mVar7 >> 4;
	}

	if (data._uniqtype != 0)
		M_ChangeLightOffset();

	return rv;
}

BOOL MonsterInstance::M_DoWalk2()
{
	BOOL rv;
	if (data._mVar8 == data.MType->Anims[MA_WALK].Frames) {
		grid[data._mVar1][data._mVar2].dMonster = 0;
		if (data._uniqtype != 0)
			ChangeLightXY(data.mlid, data._mx, data._my);
		M_StartStand(data._mdir);
		rv = TRUE;
	} else {
		if (!data._mAnimCnt) {
			data._mVar8++;
			data._mVar6 += data._mxvel;
			data._mVar7 += data._myvel;
			data._mxoff = data._mVar6 >> 4;
			data._myoff = data._mVar7 >> 4;
		}
		rv = FALSE;
	}
	if (data._uniqtype != 0)
		M_ChangeLightOffset();

	return rv;
}

BOOL MonsterInstance::M_DoWalk3()
{
	BOOL rv;
	if (data._mVar8 == data.MType->Anims[MA_WALK].Frames) {
		grid[data._mx][data._my].dMonster = 0;
		data._mx = data._mVar1;
		data._my = data._mVar2;
		grid[data._mVar4][data._mVar5].dFlags &= ~BFLAG_MONSTLR;
		grid[data._mx][data._my].dMonster = i + 1;
		if (data._uniqtype)
			ChangeLightXY(data.mlid, data._mx, data._my);
		M_StartStand(data._mdir);
		rv = TRUE;
	} else {
		if (!data._mAnimCnt) {
			data._mVar8++;
			data._mVar6 += data._mxvel;
			data._mVar7 += data._myvel;
			data._mxoff = data._mVar6 >> 4;
			data._myoff = data._mVar7 >> 4;
		}
		rv = FALSE;
	}
	if (data._uniqtype != 0)
		M_ChangeLightOffset();

	return rv;
}

void MonsterInstance::M_TryM2MHit(int mid, int hper, int mind, int maxd)
{
	BOOL ret;

	if ((DWORD)mid >= MAXMONSTERS) {
		app_fatal("M_TryM2MHit: Invalid monster %d", mid);
	}
	if (monsters[mid].data.MType == NULL)
		app_fatal("M_TryM2MHit: Monster %d \"%s\" MType NULL", mid, monsters[mid].data.mName);

	if (monsters[mid].data._mhitpoints >> 6 > 0 && (monsters[mid].data.MType->mtype != MT_ILLWEAV || monsters[mid].data._mgoal != MGOAL_RETREAT)) {
		int hit = random_(4, 100);
		if (monsters[mid].data._mmode == MM_STONE)
			hit = 0;
		if (!CheckMonsterHit(mid, &ret) && hit < hper) {
			int dam = (mind + random_(5, maxd - mind + 1)) << 6;
			monsters[mid].data._mhitpoints -= dam;
			if (monsters[mid].data._mhitpoints >> 6 <= 0) {
				if (monsters[mid].data._mmode == MM_STONE) {
					M2MStartKill(mid);
					monsters[mid].data._mmode = MM_STONE;
				} else {
					M2MStartKill(mid);
				}
			} else {
				if (monsters[mid].data._mmode == MM_STONE) {
					M2MStartHit(mid, i, dam);
					monsters[mid].data._mmode = MM_STONE;
				} else {
					M2MStartHit(mid, i, dam);
				}
			}
		}
	}
}

void MonsterInstance::M_TryH2HHit(int pnum, int Hit, int MinDam, int MaxDam)
{
	int hit, hper;
	int dx, dy;
	int blk, blkper;
	int dam, mdam;
	int newx, newy;
	int j, misnum, ms_num, cur_ms_num, new_hp;

	if (data._mFlags & MFLAG_TARGETS_MONSTER) {
		M_TryM2MHit(pnum, Hit, MinDam, MaxDam);
		return;
	}
	if (plr[pnum].data._pHitPoints >> 6 <= 0 || plr[pnum].data._pInvincible || plr[pnum].data._pSpellFlags & 1)
		return;
	dx = abs(data._mx - plr[pnum].data._px);
	dy = abs(data._my - plr[pnum].data._py);
	if (dx >= 2 || dy >= 2)
		return;

	hper = random_(98, 100);
#ifdef _DEBUG
	if (debug_mode_dollar_sign || debug_mode_key_inverted_v)
		hper = 1000;
#endif
	hit = Hit
	    + 2 * (data.mLevel - plr[pnum].data._pLevel)
	    + 30
	    - plr[pnum].data._pIBonusAC
	    - plr[pnum].data._pIAC
	    - plr[pnum].data._pDexterity / 5;
	if (hit < 15)
		hit = 15;
	if (level.currlevel == 14 && hit < 20)
		hit = 20;
	if (level.currlevel == 15 && hit < 25)
		hit = 25;
	if (level.currlevel == 16 && hit < 30)
		hit = 30;
	if ((plr[pnum].data._pmode == PM_STAND || plr[pnum].data._pmode == PM_ATTACK) && plr[pnum].data._pBlockFlag) {
		blkper = random_(98, 100);
	} else {
		blkper = 100;
	}
	blk = plr[pnum].data._pDexterity
	    + plr[pnum].data._pBaseToBlk
	    - (data.mLevel << 1)
	    + (plr[pnum].data._pLevel << 1);
	if (blk < 0)
		blk = 0;
	if (blk > 100)
		blk = 100;
	if (hper >= hit)
		return;
	if (blkper < blk) {
		plr[pnum].StartPlrBlock(GetDirection(plr[pnum].data._px, plr[pnum].data._py, data._mx, data._my));
		return;
	}
	if (data.MType->mtype == MT_YZOMBIE && pnum == myplr()) {
		ms_num = -1;
		cur_ms_num = -1;
		for (j = 0; j < nummissiles; j++) {
			misnum = missileactive[j];
			if (missile[misnum]._mitype != MIS_MANASHIELD)
				continue;
			if (missile[misnum]._misource == pnum)
				cur_ms_num = misnum;
			else
				ms_num = misnum;
		}
		if (plr[pnum].data._pMaxHP > 64) {
			if (plr[pnum].data._pMaxHPBase > 64) {
				new_hp = plr[pnum].data._pMaxHP - 64;
				plr[pnum].data._pMaxHP = new_hp;
				if (plr[pnum].data._pHitPoints > new_hp) {
					plr[pnum].data._pHitPoints = new_hp;
					if (cur_ms_num >= 0)
						missile[cur_ms_num]._miVar1 = new_hp;
				}
				new_hp = plr[pnum].data._pMaxHPBase - 64;
				plr[pnum].data._pMaxHPBase = new_hp;
				if (plr[pnum].data._pHPBase > new_hp) {
					plr[pnum].data._pHPBase = new_hp;
					if (cur_ms_num >= 0)
						missile[cur_ms_num]._miVar2 = new_hp;
				}
			}
		}
	}
	dam = (MinDam << 6) + random_(99, (MaxDam - MinDam + 1) << 6);
	dam += (plr[pnum].data._pIGetHit << 6);
	if (dam < 64)
		dam = 64;
	if (pnum == myplr()) {
		plr[pnum].data._pHitPoints -= dam;
		plr[pnum].data._pHPBase -= dam;
	}
	if (plr[pnum].data._pIFlags & ISPL_THORNS) {
		mdam = (random_(99, 3) + 1) << 6;
		data._mhitpoints -= mdam;
		if (data._mhitpoints >> 6 <= 0)
			M_StartKill(pnum);
		else
			M_StartHit(pnum, mdam);
	}
	if (!(data._mFlags & MFLAG_NOLIFESTEAL) && data.MType->mtype == MT_SKING && gbMaxPlayers != 1)
		data._mhitpoints += dam;
	if (plr[pnum].data._pHitPoints > plr[pnum].data._pMaxHP) {
		plr[pnum].data._pHitPoints = plr[pnum].data._pMaxHP;
		plr[pnum].data._pHPBase = plr[pnum].data._pMaxHPBase;
	}
	if (plr[pnum].data._pHitPoints >> 6 <= 0) {
		plr[pnum].SyncPlrKill(0);
		return;
	}
	plr[pnum].StartPlrHit(dam, FALSE);
	if (data._mFlags & MFLAG_KNOCKBACK) {
		if (plr[pnum].data._pmode != PM_GOTHIT)
			plr[pnum].StartPlrHit(0, TRUE);
		newx = plr[pnum].data._px + offset_x[data._mdir];
		newy = plr[pnum].data._py + offset_y[data._mdir];
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

BOOL MonsterInstance::M_DoAttack()
{
	if (data.MType == NULL)
		app_fatal("M_DoAttack: Monster %d \"%s\" MType NULL", i, data.mName);
	if (data.MType == NULL) // BUGFIX: should check MData
		app_fatal("M_DoAttack: Monster %d \"%s\" MData NULL", i, data.mName);

	if (data._mAnimFrame == data.MData->mAFNum) {
		M_TryH2HHit(data._menemy, data.mHit, data.mMinDamage, data.mMaxDamage);
		if (data._mAi != AI_SNAKE)
			PlayEffect(i, 0);
	}
	if (data.MType->mtype >= MT_NMAGMA && data.MType->mtype <= MT_WMAGMA && data._mAnimFrame == 9) {
		M_TryH2HHit(data._menemy, data.mHit + 10, data.mMinDamage - 2, data.mMaxDamage - 2);
		PlayEffect(i, 0);
	}
	if (data.MType->mtype >= MT_STORM && data.MType->mtype <= MT_MAEL && data._mAnimFrame == 13) {
		M_TryH2HHit(data._menemy, data.mHit - 20, data.mMinDamage + 4, data.mMaxDamage + 4);
		PlayEffect(i, 0);
	}
	if (data._mAi == AI_SNAKE && data._mAnimFrame == 1)
		PlayEffect(i, 0);
	if (data._mAnimFrame == data._mAnimLen) {
		M_StartStand(data._mdir);
		return TRUE;
	}

	return FALSE;
}

BOOL MonsterInstance::M_DoRAttack()
{
	int multimissiles, mi;
	if (data._mAnimFrame == data.MData->mAFNum) {
		if (data._mVar1 != -1) {
			if (data._mVar1 != MIS_CBOLT)
				multimissiles = 1;
			else
				multimissiles = 3;
			for (mi = 0; mi < multimissiles; mi++) {
				AddMissile(
				    data._mx,
				    data._my,
				    data._menemyx,
				    data._menemyy,
				    data._mdir,
				    data._mVar1,
				    1,
				    i,
				    data._mVar2,
				    0);
			}
		}
		PlayEffect(i, 0);
	}

	if (data._mAnimFrame == data._mAnimLen) {
		M_StartStand(data._mdir);
		return TRUE;
	}

	return FALSE;
}

int MonsterInstance::M_DoRSpAttack()
{
	if (data._mAnimFrame == data.MData->mAFNum2 && !data._mAnimCnt) {
		AddMissile(data._mx, data._my, data._menemyx, data._menemyy, data._mdir, data._mVar1,
		    1, i, data._mVar3, 0);
		PlayEffect(i, 3);
	}

	if (data._mAi == AI_MEGA && data._mAnimFrame == 3) {
		int hadV2 = data._mVar2;
		data._mVar2++;
		if (hadV2 == 0) {
			data._mFlags |= MFLAG_ALLOW_SPECIAL;
		} else if (data._mVar2 == 15) {
			data._mFlags &= ~MFLAG_ALLOW_SPECIAL;
		}
	}

	if (data._mAnimFrame == data._mAnimLen) {
		M_StartStand(data._mdir);
		return TRUE;
	}

	return FALSE;
}

BOOL MonsterInstance::M_DoSAttack()
{
	if (data._mAnimFrame == data.MData->mAFNum2)
		M_TryH2HHit(data._menemy, data.mHit2, data.mMinDamage2, data.mMaxDamage2);

	if (data._mAnimFrame == data._mAnimLen) {
		M_StartStand(data._mdir);
		return TRUE;
	}
	return FALSE;
}

BOOL MonsterInstance::M_DoFadein()
{
	if ((!(data._mFlags & MFLAG_LOCK_ANIMATION) || data._mAnimFrame != 1)
	    && (data._mFlags & MFLAG_LOCK_ANIMATION || data._mAnimFrame != data._mAnimLen)) {
		return FALSE;
	}

	M_StartStand(data._mdir);
	data._mFlags &= ~MFLAG_LOCK_ANIMATION;

	return TRUE;
}

BOOL MonsterInstance::M_DoFadeout()
{
	if ((!(data._mFlags & MFLAG_LOCK_ANIMATION) || data._mAnimFrame != 1)
	    && (data._mFlags & MFLAG_LOCK_ANIMATION || data._mAnimFrame != data._mAnimLen)) {
		return FALSE;
	}

	int mt = data.MType->mtype;
	if (mt < MT_INCIN || mt > MT_HELLBURN) {
		data._mFlags &= ~MFLAG_LOCK_ANIMATION;
		data._mFlags |= MFLAG_HIDDEN;
	} else {
		data._mFlags &= ~MFLAG_LOCK_ANIMATION;
	}

	M_StartStand(data._mdir);

	return TRUE;
}

int MonsterInstance::M_DoHeal()
{
	if (data._mFlags & MFLAG_NOHEAL) {
		data._mFlags &= ~MFLAG_ALLOW_SPECIAL;
		data._mmode = MM_SATTACK;
		return FALSE;
	}

	if (data._mAnimFrame == 1) {
		data._mFlags &= ~MFLAG_LOCK_ANIMATION;
		data._mFlags |= MFLAG_ALLOW_SPECIAL;
		if (data._mVar1 + data._mhitpoints < data._mmaxhp) {
			data._mhitpoints = data._mVar1 + data._mhitpoints;
		} else {
			data._mhitpoints = data._mmaxhp;
			data._mFlags &= ~MFLAG_ALLOW_SPECIAL;
			data._mmode = MM_SATTACK;
		}
	}
	return FALSE;
}

int MonsterInstance::M_DoTalk()
{
	int tren;
	M_StartStand(data._mdir);
	data._mgoal = MGOAL_TALKING;
	if (effect_is_playing(alltext[data.mtalkmsg].sfxnr))
		return FALSE;
	InitQTextMsg(data.mtalkmsg);
	if (data.mName == UniqMonst[UMT_GARBUD].mName) {
		if (data.mtalkmsg == TEXT_GARBUD1)
			quests[Q_GARBUD]._qactive = QUEST_ACTIVE;
		quests[Q_GARBUD]._qlog = TRUE;
		if (data.mtalkmsg == TEXT_GARBUD2 && !(data._mFlags & MFLAG_QUEST_COMPLETE)) {
			SpawnItem(i, data._mx + 1, data._my + 1, TRUE);
			data._mFlags |= MFLAG_QUEST_COMPLETE;
		}
	}
	if (data.mName == UniqMonst[UMT_ZHAR].mName
	    && data.mtalkmsg == TEXT_ZHAR1
	    && !(data._mFlags & MFLAG_QUEST_COMPLETE)) {
		quests[Q_ZHAR]._qactive = QUEST_ACTIVE;
		quests[Q_ZHAR]._qlog = TRUE;
		CreateTypeItem(data._mx + 1, data._my + 1, FALSE, ITYPE_MISC, IMISC_BOOK, TRUE, FALSE);
		data._mFlags |= MFLAG_QUEST_COMPLETE;
	}
	if (data.mName == UniqMonst[UMT_SNOTSPIL].mName) {
		if (data.mtalkmsg == TEXT_BANNER10 && !(data._mFlags & MFLAG_QUEST_COMPLETE)) {
			ObjChangeMap(setpc_x, setpc_y, (setpc_w >> 1) + setpc_x + 2, (setpc_h >> 1) + setpc_y - 2);
			tren = TransVal;
			TransVal = 9;
			DRLG_MRectTrans(setpc_x, setpc_y, (setpc_w >> 1) + setpc_x + 4, setpc_y + (setpc_h >> 1));
			TransVal = tren;
			quests[Q_LTBANNER]._qvar1 = 2;
			if (quests[Q_LTBANNER]._qactive == QUEST_INIT)
				quests[Q_LTBANNER]._qactive = QUEST_ACTIVE;
			data._mFlags |= MFLAG_QUEST_COMPLETE;
		}
		if (quests[Q_LTBANNER]._qvar1 < 2) {
			sprintf(tempstr, "SS Talk = %i, Flags = %i", data.mtalkmsg, data._mFlags);
			app_fatal(tempstr);
		}
	}
	if (data.mName == UniqMonst[UMT_LACHDAN].mName) {
		if (data.mtalkmsg == TEXT_VEIL9) {
			quests[Q_VEIL]._qactive = QUEST_ACTIVE;
			quests[Q_VEIL]._qlog = TRUE;
		}
		if (data.mtalkmsg == TEXT_VEIL11 && !(data._mFlags & MFLAG_QUEST_COMPLETE)) {
			SpawnUnique(UITEM_STEELVEIL, data._mx + 1, data._my + 1);
			data._mFlags |= MFLAG_QUEST_COMPLETE;
		}
	}
	if (data.mName == UniqMonst[UMT_WARLORD].mName)
		quests[Q_WARLORD]._qvar1 = 2;
	if (data.mName == UniqMonst[UMT_LAZURUS].mName && gbMaxPlayers != 1) {
		data._msquelch = UCHAR_MAX;
		data.mtalkmsg = 0;
		quests[Q_BETRAYER]._qvar1 = 6;
		data._mgoal = MGOAL_NORMAL;
	}
	return FALSE;
}

void MonsterInstance::M_Teleport()
{
	BOOL tren = FALSE;
	int k, j, x, y, _mx, _my, rx, ry;

	if (data._mmode != MM_STONE) {
		_mx = data._menemyx;
		_my = data._menemyy;
		rx = 2 * random_(100, 2) - 1;
		ry = 2 * random_(100, 2) - 1;

		for (j = -1; j <= 1 && !tren; j++) {
			for (k = -1; k < 1 && !tren; k++) {
				if (j != 0 || k != 0) {
					x = _mx + rx * j;
					y = _my + ry * k;
					if (y >= 0 && y < MAXDUNY && x >= 0 && x < MAXDUNX && x != data._mx && y != data._my) {
						if (PosOkMonst(i, x, y))
							tren = TRUE;
					}
				}
			}
		}
	}

	if (tren) {
		M_ClearSquares();
		grid[data._mx][data._my].dMonster = 0;
		grid[x][y].dMonster = i + 1;
		data._moldx = x;
		data._moldy = y;
		data._mdir = M_GetDir();
	}
}

BOOL MonsterInstance::M_DoGotHit()
{
	if (data._mAnimFrame == data._mAnimLen) {
		M_StartStand(data._mdir);
		return TRUE;
	}
	return FALSE;
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
		x = data._mx - ViewX;
		if (x < 0)
			x = -1;
		else
			x = x > 0;
		ViewX += x;

		y = data._my - ViewY;
		if (y < 0) {
			y = -1;
		} else {
			y = y > 0;
		}
		ViewY += y;

		if (var1 == 140)
			PrepDoEnding();
	} else if (data._mAnimFrame == data._mAnimLen) {
		if (data._uniqtype == 0)
			AddDead(data._mx, data._my, data.MType->mdeadval, (direction)data._mdir);
		else
			AddDead(data._mx, data._my, data._udeadval, (direction)data._mdir);

		grid[data._mx][data._my].dMonster = 0;
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
		grid[data._mx][data._my].dMonster = 0;
		data._mDelFlag = TRUE;
	}
	return FALSE;
}

void MonsterInstance::M_WalkDir(int md)
{
	int mwi = data.MType->Anims[MA_WALK].Frames - 1;
	switch (md) {
	case DIR_N:
		M_StartWalk(0, -MWVel[mwi][1], -1, -1, DIR_N);
		break;
	case DIR_NE:
		M_StartWalk(MWVel[mwi][1], -MWVel[mwi][0], 0, -1, DIR_NE);
		break;
	case DIR_E:
		M_StartWalk3(MWVel[mwi][2], 0, -32, -16, 1, -1, 1, 0, DIR_E);
		break;
	case DIR_SE:
		M_StartWalk2(MWVel[mwi][1], MWVel[mwi][0], -32, -16, 1, 0, DIR_SE);
		break;
	case DIR_S:
		M_StartWalk2(0, MWVel[mwi][1], 0, -32, 1, 1, DIR_S);
		break;
	case DIR_SW:
		M_StartWalk2(-MWVel[mwi][1], MWVel[mwi][0], 32, -16, 0, 1, DIR_SW);
		break;
	case DIR_W:
		M_StartWalk3(-MWVel[mwi][2], 0, 32, -16, -1, 1, 0, 1, DIR_W);
		break;
	case DIR_NW:
		M_StartWalk(-MWVel[mwi][1], -MWVel[mwi][0], -1, 0, DIR_NW);
		break;
	}
}

void MonsterInstance::GroupUnity()
{
	int leader, m, j;
	BOOL clear;
	if (data.leaderflag) {
		leader = data.leader;
		clear = LineClearF(CheckNoSolid, data._mx, data._my, monsters[leader].data._mfutx, monsters[leader].data._mfuty);
		if (clear || data.leaderflag != 1) {
			if (clear
			    && data.leaderflag == 2
			    && abs(data._mx - monsters[leader].data._mfutx) < 4
			    && abs(data._my - monsters[leader].data._mfuty) < 4) {
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
			monsters[leader].data._lastx = data._mx;
			monsters[leader].data._lasty = data._my;
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
						monsters[m].data._lastx = data._mx;
						monsters[m].data._lasty = data._my;
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
	BOOL(* Check)(int, int, int) = PosOkMonst3;
	if (!(data._mFlags & MFLAG_CAN_OPEN_DOOR))
		Check = PosOkMonst;

	if (FindPath(Check, i, data._mx, data._my, data._menemyx, data._menemyy, path)) {
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
	int mx, my;
	int md, v;
	if (data._mmode != MM_STAND) {
		return;
	}

	mx = data._mx;
	my = data._my;
	if (!(grid[mx][my].dFlags & BFLAG_VISIBLE)) {
		return;
	}

	mx = mx - data._menemyx;
	my = my - data._menemyy;
	md = data._mdir;
	v = random_(103, 100);
	if (abs(mx) >= 2 || abs(my) >= 2) {
		if (v < 2 * data._mint + 10) {
			if (abs(mx) >= 2 * data._mint + 4 || abs(my) >= 2 * data._mint + 4) {
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
	int mx, my, x, y, md;
	if (data._mmode != MM_STAND || data._msquelch == 0) {
		return;
	}

	mx = data._mx;
	my = data._my;
	x = mx - data._menemyx;
	y = my - data._menemyy;
	md = GetDirection(mx, my, data._lastx, data._lasty);
	data._mdir = md;
	if (abs(x) >= 2 || abs(y) >= 2) {
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
	BOOL clear;
	if (data.MType->mtype != MT_GOLEM) {
		if (data._msquelch == 0)
			return FALSE;
		if (data._mmode != MM_STAND)
			return FALSE;
		if (data._mgoal != MGOAL_NORMAL && data._mgoal != MGOAL_MOVE && data._mgoal != MGOAL_SHOOT)
			return FALSE;
		if (data._mx == 1 && data._my == 0)
			return FALSE;
	}

	clear = LineClearF1(PosOkMonst2, i, data._mx, data._my, data._menemyx, data._menemyy);
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
	int fx, fy, mx, my, md, tmp;

	char pattern[6] = { 1, 1, 0, -1, -1, 0 };
	int pnum = data._menemy;
	if (data._mmode != MM_STAND || data._msquelch == 0)
		return;
	fx = data._menemyx;
	fy = data._menemyy;
	mx = data._mx - fx;
	my = data._my - fy;
	md = GetDirection(data._mx, data._my, data._lastx, data._lasty);
	data._mdir = md;
	if (abs(mx) >= 2 || abs(my) >= 2) {
		if (abs(mx) < 3 && abs(my) < 3 && LineClearF1(PosOkMonst, i, data._mx, data._my, fx, fy) && data._mVar1 != MM_CHARGE) {
			if (AddMissile(data._mx, data._my, fx, fy, md, MIS_RHINO, pnum, i, 0, 0) != -1) {
				PlayEffect(i, 0);
				grid[data._mx][data._my].dMonster = -(i + 1);
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
	int md, v, pnum;
	int fx, fy, xd, yd;
	pnum = data._menemy;
	if (data._mmode != MM_STAND || data._msquelch == 0) {
		return;
	}

	xd = data._mx - data._menemyx;
	yd = data._my - data._menemyy;
	md = GetDirection(data._mx, data._my, data._lastx, data._lasty);
	data._mdir = md;
	v = random_(107, 100);
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

	fx = data._menemyx;
	fy = data._menemyy;
	if (data.MType->mtype == MT_GLOOM
	    && (abs(xd) >= 5 || abs(yd) >= 5)
	    && v < 4 * data._mint + 33
	    && LineClearF1(PosOkMonst, i, data._mx, data._my, fx, fy)) {
		if (AddMissile(data._mx, data._my, fx, fy, md, MIS_RHINO, pnum, i, 0, 0) != -1) {
			grid[data._mx][data._my].dMonster = -(i + 1);
			data._mmode = MM_CHARGE;
		}
	} else if (abs(xd) >= 2 || abs(yd) >= 2) {
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
			AddMissile(data._menemyx, data._menemyy, data._menemyx + 1, 0, -1, MIS_LIGHTNING, 1, i, random_(109, 10) + 1, 0);
		}
	}

	if (data._mmode == MM_STAND)
		data._mAnimData = data.MType->Anims[MA_STAND].Data[md];
}

void MonsterInstance::MAI_SkelBow()
{
	int mx, my, md, v;
	BOOL walking = FALSE;
	if (data._mmode != MM_STAND || data._msquelch == 0) {
		return;
	}

	mx = data._mx - data._menemyx;
	my = data._my - data._menemyy;

	md = M_GetDir();
	data._mdir = md;
	v = random_(110, 100);

	if (abs(mx) < 4 && abs(my) < 4) {
		if (data._mVar2 > 20 && v < 2 * data._mint + 13
		    || (data._mVar1 == MM_WALK || data._mVar1 == MM_WALK2 || data._mVar1 == MM_WALK3)
		        && data._mVar2 == 0
		        && v < 2 * data._mint + 63) {
			walking = M_DumbWalk(opposite[md]);
		}
	}

	mx = data._menemyx;
	my = data._menemyy;
	if (!walking) {
		if (random_(110, 100) < 2 * data._mint + 3) {
			if (LineClear(data._mx, data._my, mx, my))
				M_StartRAttack(MIS_ARROW, 4);
		}
	}

	if (data._mmode == MM_STAND)
		data._mAnimData = data.MType->Anims[MA_STAND].Data[md];
}

void MonsterInstance::MAI_Fat()
{
	int mx, my, md, v;
	if (data._mmode != MM_STAND || data._msquelch == 0) {
		return;
	}

	mx = data._mx - data._menemyx;
	my = data._my - data._menemyy;
	md = M_GetDir();
	data._mdir = md;
	v = random_(111, 100);
	if (abs(mx) >= 2 || abs(my) >= 2) {
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
	int mx, my, md;
	int dist, v;
	if (data._mmode == MM_STAND) {
		mx = data._mx;
		my = data._my;
		if (grid[mx][my].dLight != lightmax) {
			mx -= data._menemyx;
			my -= data._menemyy;

			md = M_GetDir();
			dist = 5 - data._mint;
			if (data._mVar1 == MM_GOTHIT) {
				data._mgoalvar1 = 0;
				data._mgoal = MGOAL_RETREAT;
			} else {
				if (abs(mx) >= dist + 3 || abs(my) >= dist + 3 || data._mgoalvar1 > 8) {
					data._mgoalvar1 = 0;
					data._mgoal = MGOAL_NORMAL;
				}
			}
			if (data._mgoal == MGOAL_RETREAT) {
				if (data._mFlags & MFLAG_TARGETS_MONSTER)
					md = GetDirection(data._mx, data._my, plr[data._menemy].data._pownerx, plr[data._menemy].data._pownery);
				md = opposite[md];
				if (data.MType->mtype == MT_UNSEEN) {
					if (random_(112, 2))
						md = left[md];
					else
						md = right[md];
				}
			}
			data._mdir = md;
			v = random_(112, 100);
			if (abs(mx) < dist && abs(my) < dist && data._mFlags & MFLAG_HIDDEN) {
				M_StartFadein(md, FALSE);
			} else {
				if ((abs(mx) >= dist + 1 || abs(my) >= dist + 1) && !(data._mFlags & MFLAG_HIDDEN)) {
					M_StartFadeout(md, TRUE);
				} else {
					if (data._mgoal == MGOAL_RETREAT
					    || (abs(mx) >= 2 || abs(my) >= 2) && (data._mVar2 > 20 && v < 4 * data._mint + 14 || (data._mVar1 == MM_WALK || data._mVar1 == MM_WALK2 || data._mVar1 == MM_WALK3) && data._mVar2 == 0 && v < 4 * data._mint + 64)) {
						data._mgoalvar1++;
						M_CallWalk(md);
					}
				}
			}
			if (data._mmode == MM_STAND) {
				if (abs(mx) >= 2 || abs(my) >= 2 || v >= 4 * data._mint + 10)
					data._mAnimData = data.MType->Anims[MA_STAND].Data[md];
				else
					M_StartAttack();
			}
		}
	}
}

void MonsterInstance::MAI_Fireman()
{
	int xd, yd;
	int md, pnum;
	int fx, fy;
	if (data._mmode != MM_STAND || data._msquelch == 0)
		return;

	pnum = data._menemy;
	fx = data._menemyx;
	fy = data._menemyy;
	xd = data._mx - fx;
	yd = data._my - fy;

	md = M_GetDir();
	if (data._mgoal == MGOAL_NORMAL) {
		if (LineClear(data._mx, data._my, fx, fy)
		    && AddMissile(data._mx, data._my, fx, fy, md, MIS_FIREMAN, pnum, i, 0, 0) != -1) {
			data._mmode = MM_CHARGE;
			data._mgoal = MGOAL_SHOOT;
			data._mgoalvar1 = 0;
		}
	} else if (data._mgoal == MGOAL_SHOOT) {
		if (data._mgoalvar1 == 3) {
			data._mgoal = MGOAL_NORMAL;
			M_StartFadeout(md, TRUE);
		} else if (LineClear(data._mx, data._my, fx, fy)) {
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

	if (abs(xd) < 2 && abs(yd) < 2 && data._mgoal == MGOAL_NORMAL) {
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
	int x, y, xpos, ypos;
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
		for (y = -rad; y <= rad; y++) {
			for (x = -rad; x <= rad; x++) {
				if (y >= 0 && y < MAXDUNY && x >= 0 && x < MAXDUNX) {
					m = grid[x + data._mx][y + data._my].dMonster;
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
		xpos = data._mx - data._menemyx;
		ypos = data._my - data._menemyy;
		if (abs(xpos) < 2 && abs(ypos) < 2) {
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
	int x, y, mx, my, md;
	if (data._mmode != MM_STAND || data._msquelch == 0) {
		return;
	}

	mx = data._mx;
	my = data._my;
	x = mx - data._menemyx;
	y = my - data._menemyy;

	md = GetDirection(mx, my, data._lastx, data._lasty);
	data._mdir = md;

	if (abs(x) >= 2 || abs(y) >= 2)
		M_CallWalk(md);
	else
		M_StartAttack();

	if (data._mmode == MM_STAND)
		data._mAnimData = data.MType->Anims[MA_STAND].Data[md];
}

void MonsterInstance::MAI_Round(BOOL special)
{
	int fx, fy;
	int mx, my, md;
	int dist, v;

	if (data._mmode == MM_STAND && data._msquelch != 0) {
		fy = data._menemyy;
		fx = data._menemyx;
		mx = data._mx - fx;
		my = data._my - fy;
		md = GetDirection(data._mx, data._my, data._lastx, data._lasty);
		if (data._msquelch < UCHAR_MAX)
			MonstCheckDoors(i);
		v = random_(114, 100);
		if ((abs(mx) >= 2 || abs(my) >= 2) && data._msquelch == UCHAR_MAX && grid[data._mx][data._my].dTransVal == grid[fx][fy].dTransVal) {
			if (data._mgoal == MGOAL_MOVE || (abs(mx) >= 4 || abs(my) >= 4) && random_(115, 4) == 0) {
				if (data._mgoal != MGOAL_MOVE) {
					data._mgoalvar1 = 0;
					data._mgoalvar2 = random_(116, 2);
				}
				data._mgoal = MGOAL_MOVE;
				if (abs(mx) > abs(my))
					dist = abs(mx);
				else
					dist = abs(my);
				if (data._mgoalvar1++ >= 2 * dist && DirOK(md) || grid[data._mx][data._my].dTransVal != grid[fx][fy].dTransVal) {
					data._mgoal = MGOAL_NORMAL;
				} else if (!M_RoundWalk(md, &data._mgoalvar2)) {
					M_StartDelay(random_(125, 10) + 10);
				}
			}
		} else
			data._mgoal = MGOAL_NORMAL;
		if (data._mgoal == MGOAL_NORMAL) {
			if (abs(mx) >= 2 || abs(my) >= 2) {
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
	int md;
	int fx, fy, mx, my;
	if (data._mmode != MM_STAND) {
		return;
	}

	if (data._msquelch == UCHAR_MAX || data._mFlags & MFLAG_TARGETS_MONSTER) {
		fx = data._menemyx;
		fy = data._menemyy;
		mx = data._mx - fx;
		my = data._my - fy;
		md = M_GetDir();
		if (data._msquelch < UCHAR_MAX)
			MonstCheckDoors(i);
		data._mdir = md;
		if (data._mVar1 == MM_RATTACK) {
			M_StartDelay(random_(118, 20));
		} else if (abs(mx) < 4 && abs(my) < 4) {
			if (random_(119, 100) < 10 * (data._mint + 7))
				M_CallWalk(opposite[md]);
		}
		if (data._mmode == MM_STAND) {
			if (LineClear(data._mx, data._my, fx, fy)) {
				if (special)
					M_StartRSpAttack(missile_type, 4);
				else
					M_StartRAttack(missile_type, 4);
			} else {
				data._mAnimData = data.MType->Anims[MA_STAND].Data[md];
			}
		}
	} else if (data._msquelch != 0) {
		fx = data._lastx;
		fy = data._lasty;
		md = GetDirection(data._mx, data._my, fx, fy);
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
	BOOL done;
	int x, y;
	int _mx = data._mx;
	int _my = data._my;
	done = FALSE;
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
		if (grid[data._mx][data._my].dDead) {
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
							done = grid[data._mx + x][data._my + y].dDead != 0
							    && LineClearF(
							           CheckNoSolid,
							           data._mx,
							           data._my,
							           data._mx + x,
							           data._my + y);
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
							done = grid[data._mx + x][data._my + y].dDead != 0
							    && LineClearF(
							           CheckNoSolid,
							           data._mx,
							           data._my,
							           data._mx + x,
							           data._my + y);
						}
					}
					x++;
					y++;
				}
				if (done) {
					data._mgoalvar1 = x + data._mx + 1;
					data._mgoalvar2 = y + data._my + 1;
				}
			}
			if (data._mgoalvar1) {
				x = data._mgoalvar1 - 1;
				y = data._mgoalvar2 - 1;
				data._mdir = GetDirection(data._mx, data._my, x, y);
				M_CallWalk(data._mdir);
			}
		}
	}
	if (data._mmode == MM_STAND) MAI_SkelSd();
}

void MonsterInstance::MAI_Garg()
{
	int mx, my, dx, dy, md;
	dx = data._mx - data._lastx;
	dy = data._my - data._lasty;
	md = M_GetDir();
	if (data._msquelch != 0 && data._mFlags & MFLAG_ALLOW_SPECIAL) {
		M_Enemy();
		mx = data._mx - data._menemyx;
		my = data._my - data._menemyy;
		if (abs(mx) < data._mint + 2 && abs(my) < data._mint + 2) {
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
		if (abs(dx) >= data._mint + 2 || abs(dy) >= data._mint + 2) {
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
	int mx, my;
	int fx, fy;
	int md, dist, v;
	if (data._mmode == MM_STAND && data._msquelch != 0) {
		fx = data._menemyx;
		fy = data._menemyy;
		mx = data._mx - fx;
		my = data._my - fy;
		md = GetDirection(data._mx, data._my, data._lastx, data._lasty);
		if (checkdoors && data._msquelch < UCHAR_MAX)
			MonstCheckDoors(i);
		v = random_(121, 10000);
		if ((abs(mx) >= 2 || abs(my) >= 2) && data._msquelch == UCHAR_MAX && grid[data._mx][data._my].dTransVal == grid[fx][fy].dTransVal) {
			if (data._mgoal == MGOAL_MOVE || ((abs(mx) >= 3 || abs(my) >= 3) && random_(122, 4 << lessmissiles) == 0)) {
				if (data._mgoal != MGOAL_MOVE) {
					data._mgoalvar1 = 0;
					data._mgoalvar2 = random_(123, 2);
				}
				data._mgoal = MGOAL_MOVE;
				if (abs(mx) > abs(my)) {
					dist = abs(mx);
				} else {
					dist = abs(my);
				}
				if (data._mgoalvar1++ >= 2 * dist && DirOK(md)) {
					data._mgoal = MGOAL_NORMAL;
				} else if (v<500 * (data._mint + 1)>> lessmissiles
				    && (LineClear(data._mx, data._my, fx, fy))) {
					M_StartRSpAttack(missile_type, dam);
				} else {
					M_RoundWalk(md, &data._mgoalvar2);
				}
			}
		} else {
			data._mgoal = MGOAL_NORMAL;
		}
		if (data._mgoal == MGOAL_NORMAL) {
			if (((abs(mx) >= 3 || abs(my) >= 3) && v < ((500 * (data._mint + 2)) >> lessmissiles)
			        || v < ((500 * (data._mint + 1)) >> lessmissiles))
			    && LineClear(data._mx, data._my, fx, fy)) {
				M_StartRSpAttack(missile_type, dam);
			} else if (abs(mx) >= 2 || abs(my) >= 2) {
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
				ObjChangeMap(setpc_x, setpc_y, setpc_x + setpc_w + 1, setpc_y + setpc_h + 1);
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

BOOL PosOkMissile(int x, int y)
{
	return !pieces[grid[x][y].dPiece].nMissileTable && !(grid[x][y].dFlags & BFLAG_MONSTLR);
}

BOOL CheckNoSolid(int x, int y)
{
	return pieces[grid[x][y].dPiece].nSolidTable == FALSE;
}

BOOL LineClearF(BOOL (*Clear)(int, int), int x1, int y1, int x2, int y2)
{
	int xorg, yorg;
	int dx, dy;
	int d;
	int xincD, yincD, dincD, dincH;
	int tmp;

	xorg = x1;
	yorg = y1;
	dx = x2 - x1;
	dy = y2 - y1;
	if (abs(dx) > abs(dy)) {
		if (dx < 0) {
			tmp = x1;
			x1 = x2;
			x2 = tmp;
			tmp = y1;
			y1 = y2;
			y2 = tmp;
			dx = -dx;
			dy = -dy;
		}
		if (dy > 0) {
			d = 2 * dy - dx;
			dincH = 2 * (dy - dx);
			dincD = 2 * dy;
			yincD = 1;
		} else {
			d = 2 * dy + dx;
			dincH = 2 * (dx + dy);
			dincD = 2 * dy;
			yincD = -1;
		}
		while (x1 != x2 || y1 != y2) {
			if ((d <= 0) ^ (yincD < 0)) {
				d += dincD;
			} else {
				d += dincH;
				y1 += yincD;
			}
			x1++;
			if ((x1 != xorg || y1 != yorg) && !Clear(x1, y1))
				break;
		}
	} else {
		if (dy < 0) {
			tmp = y1;
			y1 = y2;
			y2 = tmp;
			tmp = x1;
			x1 = x2;
			x2 = tmp;
			dy = -dy;
			dx = -dx;
		}
		if (dx > 0) {
			d = 2 * dx - dy;
			dincH = 2 * (dx - dy);
			dincD = 2 * dx;
			xincD = 1;
		} else {
			d = 2 * dx + dy;
			dincH = 2 * (dy + dx);
			dincD = 2 * dx;
			xincD = -1;
		}
		while (y1 != y2 || x1 != x2) {
			if ((d <= 0) ^ (xincD < 0)) {
				d += dincD;
			} else {
				d += dincH;
				x1 += xincD;
			}
			y1++;
			if ((y1 != yorg || x1 != xorg) && !Clear(x1, y1))
				break;
		}
	}
	return x1 == x2 && y1 == y2;
}

BOOL LineClear(int x1, int y1, int x2, int y2)
{
	return LineClearF(PosOkMissile, x1, y1, x2, y2);
}

BOOL LineClearF1(BOOL (*Clear)(int, int, int), int monst, int x1, int y1, int x2, int y2)
{
	int xorg, yorg;
	int dx, dy;
	int d;
	int xincD, yincD, dincD, dincH;
	int tmp;

	xorg = x1;
	yorg = y1;
	dx = x2 - x1;
	dy = y2 - y1;
	if (abs(dx) > abs(dy)) {
		if (dx < 0) {
			tmp = x1;
			x1 = x2;
			x2 = tmp;
			tmp = y1;
			y1 = y2;
			y2 = tmp;
			dx = -dx;
			dy = -dy;
		}
		if (dy > 0) {
			d = 2 * dy - dx;
			dincH = 2 * (dy - dx);
			dincD = 2 * dy;
			yincD = 1;
		} else {
			d = 2 * dy + dx;
			dincH = 2 * (dx + dy);
			dincD = 2 * dy;
			yincD = -1;
		}
		while (x1 != x2 || y1 != y2) {
			if ((d <= 0) ^ (yincD < 0)) {
				d += dincD;
			} else {
				d += dincH;
				y1 += yincD;
			}
			x1++;
			if ((x1 != xorg || y1 != yorg) && !Clear(monst, x1, y1))
				break;
		}
	} else {
		if (dy < 0) {
			tmp = y1;
			y1 = y2;
			y2 = tmp;
			tmp = x1;
			x1 = x2;
			x2 = tmp;
			dy = -dy;
			dx = -dx;
		}
		if (dx > 0) {
			d = 2 * dx - dy;
			dincH = 2 * (dx - dy);
			dincD = 2 * dx;
			xincD = 1;
		} else {
			d = 2 * dx + dy;
			dincH = 2 * (dy + dx);
			dincD = 2 * dx;
			xincD = -1;
		}
		while (y1 != y2 || x1 != x2) {
			if ((d <= 0) ^ (xincD < 0)) {
				d += dincD;
			} else {
				d += dincH;
				x1 += xincD;
			}
			y1++;
			if ((y1 != yorg || x1 != xorg) && !Clear(monst, x1, y1))
				break;
		}
	}
	return x1 == x2 && y1 == y2;
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

void M_FallenFear(int x, int y)
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
		    && abs(x - monsters[mi].data._mx) < 5
		    && abs(y - monsters[mi].data._my) < 5
		    && monsters[mi].data._mhitpoints >> 6 > 0) {
			monsters[mi].data._mgoal = MGOAL_RETREAT;
			monsters[mi].data._mgoalvar1 = rundist;
			monsters[mi].data._mdir = GetDirection(x, y, monsters[i].data._mx, monsters[i].data._my);
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

void MissToMonst(int i, int x, int y)
{
	int oldx, oldy;
	int newx, newy;
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
	oldx = Miss->_mix;
	oldy = Miss->_miy;
	grid[x][y].dMonster = m + 1;
	Monst->_mdir = Miss->_mimfnum;
	Monst->_mx = x;
	Monst->_my = y;
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
		pnum = grid[oldx][oldy].dPlayer - 1;
		if (grid[oldx][oldy].dPlayer > 0) {
			if (Monst->MType->mtype != MT_GLOOM && (Monst->MType->mtype < MT_INCIN || Monst->MType->mtype > MT_HELLBURN)) {
				monsters[m].M_TryH2HHit(grid[oldx][oldy].dPlayer - 1, 500, Monst->mMinDamage2, Monst->mMaxDamage2);
				if (pnum == grid[oldx][oldy].dPlayer - 1 && (Monst->MType->mtype < MT_NSNAKE || Monst->MType->mtype > MT_GSNAKE)) {
					if (plr[pnum].data._pmode != 7 && plr[pnum].data._pmode != 8)
						plr[pnum].StartPlrHit(0, TRUE);
					newx = oldx + offset_x[Monst->_mdir];
					newy = oldy + offset_y[Monst->_mdir];
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

BOOL PosOkMonst(int i, int x, int y)
{
	int oi, mi, j;
	BOOL ret, fire;

	fire = FALSE;
	ret = !SolidLoc(x, y) && !grid[x][y].dPlayer && !grid[x][y].dMonster;
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

BOOL PosOkMonst2(int i, int x, int y)
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

BOOL PosOkMonst3(int i, int x, int y)
{
	int j, oi, objtype, mi;
	BOOL ret, fire, isdoor;

	fire = FALSE;
	ret = TRUE;
	isdoor = FALSE;

	if (ret && grid[x][y].dObject != 0) {
		oi = grid[x][y].dObject > 0 ? grid[x][y].dObject - 1 : -(grid[x][y].dObject + 1);
		objtype = object[oi]._otype;
		isdoor = objtype == OBJ_L1LDOOR || objtype == OBJ_L1RDOOR
		    || objtype == OBJ_L2LDOOR || objtype == OBJ_L2RDOOR
		    || objtype == OBJ_L3LDOOR || objtype == OBJ_L3RDOOR;
		if (object[oi]._oSolidFlag && !isdoor) {
			ret = FALSE;
		}
	}
	if (ret) {
		ret = (!SolidLoc(x, y) || isdoor) && grid[x][y].dPlayer == 0 && grid[x][y].dMonster == 0;
	}
	if (ret && grid[x][y].dMissile != 0 && i >= 0) {
		mi = grid[x][y].dMissile;
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

int M_SpawnSkel(int x, int y, int dir)
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
		skel = AddMonster(x, y, dir, i - 1, TRUE);
		if (skel != -1)
			monsters[skel].M_StartSpStand(dir);

		return skel;
	}

	return -1;
}

void MonsterInstance::ActivateSpawn(int x, int y, int dir)
{
	grid[x][y].dMonster = i + 1;
	data._mx = x;
	data._my = y;
	data._mfutx = x;
	data._mfuty = y;
	data._moldx = x;
	data._moldy = y;
	M_StartSpStand(dir);
}

BOOL SpawnSkeleton(int ii, int x, int y)
{
	int dx, dy, xx, yy, dir, j, k, rs;
	BOOL savail;
	int monstok[3][3];

	if (ii == -1)
		return FALSE;

	if (PosOkMonst(-1, x, y)) {
		dir = GetDirection(x, y, x, y);
		monsters[ii].ActivateSpawn(x, y, dir);
		return TRUE;
	}

	savail = FALSE;
	yy = 0;
	for (j = y - 1; j <= y + 1; j++) {
		xx = 0;
		for (k = x - 1; k <= x + 1; k++) {
			monstok[xx][yy] = PosOkMonst(-1, k, j);
			savail |= monstok[xx][yy];
			xx++;
		}
		yy++;
	}
	if (!savail) {
		return FALSE;
	}

	rs = random_(137, 15) + 1;
	xx = 0;
	yy = 0;
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

	dx = x - 1 + xx;
	dy = y - 1 + yy;
	dir = GetDirection(dx, dy, x, y);
	monsters[ii].ActivateSpawn(dx, dy, dir);

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
		skel = AddMonster(0, 0, 0, i - 1, FALSE);
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

void MonsterInstance::SpawnGolum(int x, int y, int mi)
{
	grid[x][y].dMonster = i + 1;
	data._mx = x;
	data._my = y;
	data._mfutx = x;
	data._mfuty = y;
	data._moldx = x;
	data._moldy = y;
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
		    data._mx,
		    data._my,
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
