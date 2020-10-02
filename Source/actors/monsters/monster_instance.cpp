/**
 * @file monster_instance.cpp
 *
 * Monster instance class
 */
#include "all.h"
#include "../3rdParty/Storm/Source/storm.h"

DEVILUTION_BEGIN_NAMESPACE

MonsterInstance::MonsterInstance()
	: Actor(ActorType::monster)
{};

MonsterInstance::MonsterInstance(int new_i, Dir rd, int mtype, V2Di pos)
    : Actor(ActorType::monster), i(new_i)
{
	CMonster *monst = &beastiary[mtype].data;

	data._mdir = rd;
	data._m = pos;
	data._mfut = pos;
	data._mold = pos;
	data._mMTidx = mtype;
	data._mmode = MM_STAND;
	data.mName = monst->MData->mName;
	data.MType = monst;
	data.MData = monst->MData;
	data._mAnimData = monst->Anims[MA_STAND].Data[int(rd)];
	data._mAnimDelay = monst->Anims[MA_STAND].Rate;
	data._mAnimCnt = random_(88, data._mAnimDelay - 1);
	data._mAnimLen = monst->Anims[MA_STAND].Frames;
	data._mAnimFrame = random_(88, data._mAnimLen - 1) + 1;

	if (monst->mtype == MT_DIABLO) {
		data._mmaxhp = (random_(88, 1) + 1666) << 6;
	} else {
		data._mmaxhp = (monst->mMinHP + random_(88, monst->mMaxHP - monst->mMinHP + 1)) << 6;
	}

	if (plr.isSingleplayer()) {
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
	data._mDelFlag = false;
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
		data._mAnimData = monst->Anims[MA_SPECIAL].Data[int(rd)];
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

// Plants in stationary position (no velocity or offset) facing specified direction
// Often called before starting a new action
// If no position specified, plants in current position
void MonsterInstance::PlantInPosition(Dir dir, V2Di * pos)
{
	if (pos) data._m = *pos;
	data._moff = { 0, 0 };
	data._mfut = data._m;
	data._mold = data._m;
	if (dir != Dir::NONE) data._mdir = dir;
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

void MonsterInstance::NewMonsterAnim(AnimStruct *anim, Dir md)
{
	data._mAnimData = anim->Data[int(md)];
	data._mAnimLen = anim->Frames;
	data._mAnimCnt = 0;
	data._mAnimFrame = 1;
	data._mAnimDelay = anim->Rate;
	data._mFlags &= ~(MFLAG_LOCK_ANIMATION | MFLAG_ALLOW_SPECIAL);
	data._mdir = md;
}

void MonsterInstance::GetTargetLoc(V2Di &pos, int &dist, Dir &dir)
{
}

bool MonsterInstance::M_Ranged()
{
	char ai = data._mAi;
	return ai == AI_SKELBOW || ai == AI_GOATBOW || ai == AI_SUCC || ai == AI_LAZHELP;
}

bool MonsterInstance::M_Talker()
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
	bool sameroom, bestsameroom;
	//MonsterStruct *Monst;
	uint8_t enemyx, enemyy;

	_menemy = -1;
	best_dist = -1;
	bestsameroom = 0;
	//Monst = monster + i;
	if (!(data._mFlags & MFLAG_GOLEM)) {
		for (pnum = 0; pnum < MAX_PLRS; pnum++) {
			if (!plr[pnum].data.plractive || lvl.currlevel != plr[pnum].data.plrlevel || plr[pnum].data._pLvlChanging || (plr[pnum].data._pHitPoints == 0 && plr.isMultiplayer()))
				continue;
			if (grid.at(data._m).dTransVal == grid.at(plr[pnum].pos()).dTransVal)
				sameroom = true;
			else
				sameroom = false;
			int dist = (data._m - plr[pnum].pos()).maxabs();
			if ((sameroom && !bestsameroom)
			    || ((sameroom || !bestsameroom) && dist < best_dist)
			    || (_menemy == -1)) {
				data._mFlags &= ~MFLAG_TARGETS_MONSTER;
				_menemy = pnum;
				enemyx = plr[pnum].futpos().x;
				enemyy = plr[pnum].futpos().y;
				best_dist = dist;
				bestsameroom = sameroom;
			}
		}
	}
	for (j = 0; j < nummonsters; j++) {
		mi = monstactive[j];
		if (mi == i)
			continue;
		if (monsters[mi].data._m.x == 1 && monsters[mi].data._m.y == 0)
			continue;
		if (monsters[mi].M_Talker() && monsters[mi].data.mtalkmsg)
			continue;

		int dista = (monsters[mi].data._m - data._m).maxabs();
		if (!(data._mFlags & MFLAG_GOLEM)
		        && ((dista >= 2) && !M_Ranged()
		        || (!(data._mFlags & MFLAG_GOLEM) && !(monsters[mi].data._mFlags & MFLAG_GOLEM)))) {
			continue;
		}
		sameroom = grid.at(data._m).dTransVal == grid.at(monsters[mi].data._m).dTransVal;
		int distb = (data._m - monsters[mi].data._m).maxabs();
		if ((sameroom && !bestsameroom)
		    || ((sameroom || !bestsameroom) && distb < best_dist)
		    || (_menemy == -1)) {
			data._mFlags |= MFLAG_TARGETS_MONSTER;
			_menemy = mi;
			enemyx = monsters[mi].data._mfut.x;
			enemyy = monsters[mi].data._mfut.y;
			best_dist = dist;
			bestsameroom = sameroom;
		}
	}
	if (_menemy != -1) {
		data._mFlags &= ~MFLAG_NO_ENEMY;
		data._menemy = _menemy;
		data._menemypos = { enemyx, enemyy };
	} else {
		data._mFlags |= MFLAG_NO_ENEMY;
	}
}

Dir MonsterInstance::M_GetDir()
{
	return GetDirection(data._m, data._menemypos);
}

void MonsterInstance::M_StartStand(Dir md)
{
	ClearMVars();
	if (data.MType->mtype == MT_GOLEM)
		NewMonsterAnim(&data.MType->Anims[MA_WALK], md);
	else
		NewMonsterAnim(&data.MType->Anims[MA_STAND], md);
	data._mVar1 = data._mmode;
	data._mVar2 = 0;
	data._mmode = MM_STAND;
	PlantInPosition(md);
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

void MonsterInstance::M_StartSpStand(Dir md)
{
	NewMonsterAnim(&data.MType->Anims[MA_SPECIAL], md);
	data._mmode = MM_SPSTAND;
	PlantInPosition(md);
}

void MonsterInstance::M_StartWalk(V2Di vel, V2Di add, Dir EndDir)
{
	V2Di f = add + data._m;
	grid.at(f).setActor(i);
	data._mmode = MM_WALK;
	data._mold = data._m;
	data._mfut = f;
	data._mvel = vel;
	data._mVar1 = add.x;
	data._mVar2 = add.y;
	data._mVar3 = int(EndDir);
	data._mdir = EndDir;
	NewMonsterAnim(&data.MType->Anims[MA_WALK], EndDir);
	data._mVar6 = 0;
	data._mVar7 = 0;
	data._mVar8 = 0;
}

void MonsterInstance::M_StartWalk2(V2Di vel, V2Di off, V2Di add, Dir EndDir)
{
	V2Di f = add + data._m;
	grid.at(data._m).setActor(i);
	data._mVar1 = data._m.x;
	data._mVar2 = data._m.y;
	data._mold = data._m;
	data._m = f;
	data._mfut = f;
	grid.at(f).setActor(i);
	if (data._uniqtype != 0) ChangeLightXY(data.mlid, data._m);
	data._moff = off;
	data._mmode = MM_WALK2;
	data._mvel = vel;
	data._mVar3 = int(EndDir);
	data._mdir = EndDir;
	NewMonsterAnim(&data.MType->Anims[MA_WALK], EndDir);
	data._mVar6 = 16 * off.x;
	data._mVar7 = 16 * off.y;
	data._mVar8 = 0;
}

void MonsterInstance::M_StartWalk3(V2Di vel, V2Di off, V2Di add, V2Di map, Dir EndDir)
{
	V2Di f = add + data._m;
	V2Di n = map + data._m;
	if (data._uniqtype != 0) ChangeLightXY(data.mlid, n);
	grid.at(data._m).setActor(i);
	grid.at(f).setActor(i);
	data._mVar4 = n.x;
	data._mVar5 = n.y;
	grid[n.x][n.y].dFlags |= BFLAG_MONSTLR;
	data._mold = data._m;
	data._mfut = f;
	data._moff = off;
	data._mmode = MM_WALK3;
	data._mvel = vel;
	data._mVar1 = f.x;
	data._mVar2 = f.y;
	data._mVar3 = int(EndDir);
	data._mdir = EndDir;
	NewMonsterAnim(&data.MType->Anims[MA_WALK], EndDir);
	data._mVar6 = 16 * off.x;
	data._mVar7 = 16 * off.y;
	data._mVar8 = 0;
}

void MonsterInstance::M_StartAttack()
{
	Dir md = M_GetDir();
	NewMonsterAnim(&data.MType->Anims[MA_ATTACK], md);
	data._mmode = MM_ATTACK;
	PlantInPosition(md);
}

void MonsterInstance::M_StartRAttack(int missile_type, int dam)
{
	Dir md = M_GetDir();
	NewMonsterAnim(&data.MType->Anims[MA_ATTACK], md);
	data._mmode = MM_RATTACK;
	data._mVar1 = missile_type;
	data._mVar2 = dam;
	PlantInPosition(md);
}

void MonsterInstance::M_StartRSpAttack(int missile_type, int dam)
{
	Dir md = M_GetDir();
	NewMonsterAnim(&data.MType->Anims[MA_SPECIAL], md);
	data._mmode = MM_RSPATTACK;
	data._mVar1 = missile_type;
	data._mVar2 = 0;
	data._mVar3 = dam;
	PlantInPosition(md);
}

void MonsterInstance::M_StartSpAttack()
{
	Dir md = M_GetDir();
	NewMonsterAnim(&data.MType->Anims[MA_SPECIAL], md);
	data._mmode = MM_SATTACK;
	PlantInPosition(md);
}

void MonsterInstance::M_StartEat()
{
	NewMonsterAnim(&data.MType->Anims[MA_SPECIAL], data._mdir);
	data._mmode = MM_SATTACK;
	PlantInPosition(data._mdir);
}

void MonsterInstance::M_ClearSquares()
{
	int x, y, mx, my, m1, m2;
	mx = data._mold.x;
	my = data._mold.y;
	m1 = -1 - i;
	m2 = i + 1;

	for (y = my - 1; y <= my + 1; y++) {
		if (y >= 0 && y < MAXDUNY) {
			for (x = mx - 1; x <= mx + 1; x++) {
				if (x >= 0 && x < MAXDUNX && (grid[x][y].getActor() == m1 || grid[x][y].getActor() == m2))
					grid[x][y].clearMonster();
			}
		}
	}

	if (mx + 1 < MAXDUNX) grid[mx + 1][my].dFlags &= ~BFLAG_MONSTLR;
	if (my + 1 < MAXDUNY) grid[mx][my + 1].dFlags &= ~BFLAG_MONSTLR;
}

void MonsterInstance::M_GetKnockback()
{
	Dir d = rotate(data._mdir, -4);
	if (DirOK(d)) {
		M_ClearSquares();
		data._mold += offset(d);
		NewMonsterAnim(&data.MType->Anims[MA_GOTHIT], data._mdir);
		data._mmode = MM_GOTHIT;
		data._moff = { 0, 0 };
		data._m = data._mold;
		data._mfut = data._m;
		// BUGFIX useless assignment
		data._mold = data._m;
		M_ClearSquares();
		grid.at(data._m).setActor(i);
	}
}

void MonsterInstance::M_StartHit(int pnum, int dam)
{
	if (pnum >= 0)
		data.mWhoHit |= 1 << pnum;
	if (pnum == myplr()) {
		delta_monster_hp(i, data._mhitpoints, lvl.currlevel);
		NetSendCmdParam2(false, CMD_MONSTDAMAGE, i, dam);
	}
	PlayEffect(i, 1);
	if (data.MType->mtype >= MT_SNEAK && data.MType->mtype <= MT_ILLWEAV || dam >> 6 >= data.mLevel + 3) {
		if (pnum >= 0) {
			data._mFlags &= ~MFLAG_TARGETS_MONSTER;
			data._menemy = pnum;
			data._menemypos = plr[pnum].futpos();
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
			PlantInPosition(data._mdir, &data._mold);
			M_ClearSquares();
			grid.at(data._m).setActor(i);
		}
	}
}

void MonsterInstance::M_DiabloDeath(bool sendmsg)
{
	int j, k;
	V2Di _mold;
	PlaySFX(USFX_DIABLOD);
	quests[Q_DIABLO]._qactive = QUEST_DONE;
	if (sendmsg) NetSendCmdQuest(true, Q_DIABLO);
	gbProcessPlayers = false;
	sgbSaveSoundOn = gbSoundOn;

	// Kills all monsters?
	for (j = 0; j < nummonsters; j++) {
		k = monstactive[j];
		if (k == i || data._msquelch == 0)
			continue;
		monsters[k].NewMonsterAnim(&monsters[k].data.MType->Anims[MA_DEATH], monsters[k].data._mdir);
		monsters[k].data._mVar1 = 0;
		monsters[k].data._mmode = MM_DEATH;
		_mold = monsters[k].data._mold;
		monsters[k].PlantInPosition(monsters[k].data._mdir, &_mold);
		monsters[k].M_ClearSquares();
		grid.at(monsters[k].data._m).setActor(k);
	}
	AddLight(monsters[k].data._m, 8);
	DoVision(monsters[k].data._m, 8, false, true);

	int dist = (View - monsters[k].data._m).maxabs();
	if (dist > 20) dist = 20;
	j = View.x << 16;
	k = View.y << 16;
	monsters[k].data._mVar3 = j;
	monsters[k].data._mVar4 = k;
	monsters[k].data._mVar5 = (int)((j - (monsters[k].data._m.x << 16)) / (double)dist);
	monsters[k].data._mVar6 = (int)((k - (monsters[k].data._m.y << 16)) / (double)dist);
}

void MonsterInstance::MonstStartKill(int pnum, bool sendmsg)
{
	Dir md;

	//MonsterStruct *Monst;
	if (pnum >= 0) data.mWhoHit |= 1 << pnum;
	if (pnum < MAX_PLRS && i > MAX_PLRS) AddPlrMonstExper(data.mLevel, data.mExp, data.mWhoHit);
	monstkills[data.MType->mtype]++;
	data._mhitpoints = 0;
	SetRndSeed(data._mRndSeed);
	if (QuestStatus(Q_GARBUD) && data.mName == UniqMonst[UMT_GARBUD].mName) {
		CreateTypeItem({ data._m.x + 1, data._m.y + 1 }, true, ITYPE_MACE, IMISC_NONE, true, false);
	} else if (i > MAX_PLRS - 1) { // Golems should not spawn items
		SpawnItem(i, data._m, sendmsg);
	}
	if (data.MType->mtype == MT_DIABLO)
		M_DiabloDeath(true);
	else
		PlayEffect(i, 2);

	if (pnum >= 0)
		md = M_GetDir();
	else
		md = data._mdir;
	data._mdir = md;
	NewMonsterAnim(&data.MType->Anims[MA_DEATH], md);
	data._mmode = MM_DEATH;
	PlantInPosition(data._mdir, &data._mold);
	data._mVar1 = 0;

	M_ClearSquares();
	grid.at(data._m).setActor(i);
	CheckQuestKill(i, sendmsg);
	M_FallenFear(data._m);
	if (data.MType->mtype >= MT_NACID && data.MType->mtype <= MT_XACID)
		AddMissile(data._m, { 0, 0 }, Dir(0), MIS_ACIDPUD, 1, i, data._mint + 1, 0);
}

void MonsterInstance::M2MStartKill(int mid)
{
	Dir md;

	if ((DWORD)i >= MAXMONSTERS) { /// BUGFIX: should check `mid`
		app_fatal("M2MStartKill: Invalid monster (killed) %d", mid);
	}
	if (!data.MType)
		app_fatal("M2MStartKill: Monster %d \"%s\" MType NULL", mid, monsters[mid].data.mName);

	delta_kill_monster(mid, monsters[mid].data._m, lvl.currlevel);
	NetSendCmdLocParam1(false, CMD_MONSTDEATH, monsters[mid].data._m, mid);

	monsters[mid].data.mWhoHit |= 1 << i;
	if (i < MAX_PLRS)
		AddPlrMonstExper(monsters[mid].data.mLevel, monsters[mid].data.mExp, monsters[mid].data.mWhoHit);

	monstkills[monsters[mid].data.MType->mtype]++;
	monsters[mid].data._mhitpoints = 0;
	SetRndSeed(monsters[mid].data._mRndSeed);

	if (mid >= MAX_PLRS)
		SpawnItem(mid, monsters[mid].data._m, true);

	if (monsters[mid].data.MType->mtype == MT_DIABLO)
		monsters[mid].M_DiabloDeath(true);
	else
		PlayEffect(i, 2);

	PlayEffect(mid, 2);

	md = rotate(data._mdir, -4);
	if (monsters[mid].data.MType->mtype == MT_GOLEM) md = Dir(0);

	monsters[mid].data._mdir = md;
	monsters[mid].NewMonsterAnim(&monsters[mid].data.MType->Anims[MA_DEATH], md);
	monsters[mid].data._mmode = MM_DEATH;
	monsters[mid].PlantInPosition(md, &monsters[mid].data._mold);
	monsters[mid].M_ClearSquares();
	grid.at(monsters[mid].data._m).setActor(mid);
	CheckQuestKill(mid, true);
	M_FallenFear(monsters[mid].data._m);
	if (monsters[mid].data.MType->mtype >= MT_NACID && monsters[mid].data.MType->mtype <= MT_XACID)
		AddMissile(monsters[mid].data._m, { 0, 0 }, Dir(0), MIS_ACIDPUD, 1, mid, monsters[mid].data._mint + 1, 0);
}

void MonsterInstance::M_StartKill(int pnum)
{
	if (myplr() == pnum) {
		delta_kill_monster(i, data._m, lvl.currlevel);
		if (i != pnum) {
			NetSendCmdLocParam1(false, CMD_MONSTDEATH, data._m, i);
		} else {
			NetSendCmdLocParam1(false, CMD_KILLGOLEM, data._m, lvl.currlevel);
		}
	}
	MonstStartKill(pnum, true);
}

void MonsterInstance::M_SyncStartKill(V2Di pos, int pnum)
{
	if (data._mhitpoints == 0 || data._mmode == MM_DEATH) {
		return;
	}

	if (grid[pos.x][pos.y].getActor() == 0) {
		M_ClearSquares();
		data._m = pos;
		data._mold = pos;
	}

	if (data._mmode == MM_STONE) {
		MonstStartKill(pnum, false);
		data._mmode = MM_STONE;
	} else {
		MonstStartKill(pnum, false);
	}
}

void MonsterInstance::M_StartFadein(Dir md, bool backwards)
{
	NewMonsterAnim(&data.MType->Anims[MA_SPECIAL], md);
	data._mmode = MM_FADEIN;
	PlantInPosition(md);
	data._mFlags &= ~MFLAG_HIDDEN;
	if (backwards) {
		data._mFlags |= MFLAG_LOCK_ANIMATION;
		data._mAnimFrame = data._mAnimLen;
	}
}

void MonsterInstance::M_StartFadeout(Dir md, bool backwards)
{
	NewMonsterAnim(&data.MType->Anims[MA_SPECIAL], md);
	data._mmode = MM_FADEOUT;
	PlantInPosition(md);
	if (backwards) {
		data._mFlags |= MFLAG_LOCK_ANIMATION;
		data._mAnimFrame = data._mAnimLen;
	}
}

void MonsterInstance::M_StartHeal()
{
	data._mAnimData = data.MType->Anims[MA_SPECIAL].Data[int(data._mdir)];
	data._mAnimFrame = data.MType->Anims[MA_SPECIAL].Frames;
	data._mFlags |= MFLAG_LOCK_ANIMATION;
	data._mmode = MM_HEAL;
	data._mVar1 = data._mmaxhp / (16 * (random_(97, 5) + 4));
}

void MonsterInstance::M_ChangeLightOffset()
{
	int lx, ly, _mxoff, _myoff, sign;

	lx = data._moff.x + 2 * data._moff.y;
	ly = 2 * data._moff.y - data._moff.x;

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
	ChangeLightOff(monsters[i].data.mlid, { _mxoff, _myoff });
}

bool MonsterInstance::M_DoStand()
{
	if (data.MType->mtype == MT_GOLEM)
		data._mAnimData = data.MType->Anims[MA_WALK].Data[int(data._mdir)];
	else
		data._mAnimData = data.MType->Anims[MA_STAND].Data[int(data._mdir)];

	if (data._mAnimFrame == data._mAnimLen)
		M_Enemy();

	data._mVar2++;
	return false;
}

bool MonsterInstance::M_DoWalk()
{
	bool rv = false;
	if (data._mVar8 == data.MType->Anims[MA_WALK].Frames) {
		grid.at(data._m).clearMonster();
		data._m.x += data._mVar1;
		data._m.y += data._mVar2;
		grid.at(data._m).setActor(i);
		if (data._uniqtype != 0) ChangeLightXY(data.mlid, data._m);
		M_StartStand(data._mdir);
		rv = true;
	} else if (!data._mAnimCnt) {
		data._mVar8++;
		data._mVar6 += data._mvel.x;
		data._mVar7 += data._mvel.y;
		data._moff.x = data._mVar6 >> 4;
		data._moff.y = data._mVar7 >> 4;
	}

	if (data._uniqtype != 0) M_ChangeLightOffset();
	return rv;
}

bool MonsterInstance::M_DoWalk2()
{
	bool rv;
	if (data._mVar8 == data.MType->Anims[MA_WALK].Frames) {
		grid[data._mVar1][data._mVar2].clearMonster();
		if (data._uniqtype != 0) ChangeLightXY(data.mlid, data._m);
		M_StartStand(data._mdir);
		rv = true;
	} else {
		if (!data._mAnimCnt) {
			data._mVar8++;
			data._mVar6 += data._mvel.x;
			data._mVar7 += data._mvel.y;
			data._moff.x = data._mVar6 >> 4;
			data._moff.y = data._mVar7 >> 4;
		}
		rv = false;
	}
	if (data._uniqtype != 0) M_ChangeLightOffset();
	return rv;
}

bool MonsterInstance::M_DoWalk3()
{
	bool rv;
	if (data._mVar8 == data.MType->Anims[MA_WALK].Frames) {
		grid.at(data._m).clearMonster();
		data._m = { data._mVar1, data._mVar2 };
		grid[data._mVar4][data._mVar5].dFlags &= ~BFLAG_MONSTLR;
		grid.at(data._m).setActor(i);
		if (data._uniqtype) ChangeLightXY(data.mlid, data._m);
		M_StartStand(data._mdir);
		rv = true;
	} else {
		if (!data._mAnimCnt) {
			data._mVar8++;
			data._mVar6 += data._mvel.x;
			data._mVar7 += data._mvel.y;
			data._moff.x = data._mVar6 >> 4;
			data._moff.y = data._mVar7 >> 4;
		}
		rv = false;
	}
	if (data._uniqtype != 0) M_ChangeLightOffset();
	return rv;
}

void MonsterInstance::M_TryM2MHit(int mid, int hper, int mind, int maxd)
{
	bool ret;

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
	int blk, blkper;
	int dam, mdam;
	V2Di newpos;
	int j, misnum, ms_num, cur_ms_num, new_hp;

	if (data._mFlags & MFLAG_TARGETS_MONSTER) {
		M_TryM2MHit(pnum, Hit, MinDam, MaxDam);
		return;
	}
	if (plr[pnum].data._pHitPoints >> 6 <= 0 || plr[pnum].data._pInvincible || plr[pnum].data._pSpellFlags & 1)
		return;

	int dist = (data._m - plr[pnum].pos()).maxabs();
	if (dist >= 2) return;

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
	if (lvl.currlevel == 14 && hit < 20)
		hit = 20;
	if (lvl.currlevel == 15 && hit < 25)
		hit = 25;
	if (lvl.currlevel == 16 && hit < 30)
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
		plr[pnum].StartPlrBlock(GetDirection(plr[pnum].pos(), data._m));
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
	if (!(data._mFlags & MFLAG_NOLIFESTEAL) && data.MType->mtype == MT_SKING && plr.isMultiplayer())
		data._mhitpoints += dam;
	if (plr[pnum].data._pHitPoints > plr[pnum].data._pMaxHP) {
		plr[pnum].data._pHitPoints = plr[pnum].data._pMaxHP;
		plr[pnum].data._pHPBase = plr[pnum].data._pMaxHPBase;
	}
	if (plr[pnum].data._pHitPoints >> 6 <= 0) {
		plr[pnum].SyncPlrKill(0);
		return;
	}
	plr[pnum].StartPlrHit(dam, false);
	if (data._mFlags & MFLAG_KNOCKBACK) {
		if (plr[pnum].data._pmode != PM_GOTHIT)
			plr[pnum].StartPlrHit(0, true);
		newpos = plr[pnum].pos() + offset(data._mdir);
		if (PosOkPlayer(pnum, newpos)) {
			plr[pnum].changePos(newpos);
			plr[pnum].PlantInPlace(plr[pnum].data._pdir);
			plr[pnum].FixPlrWalkTags();
			plr[pnum].SetPlayerOld();
		}
	}
}

bool MonsterInstance::M_DoAttack()
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
		return true;
	}

	return false;
}

bool MonsterInstance::M_DoRAttack()
{
	int multimissiles, mi;
	if (data._mAnimFrame == data.MData->mAFNum) {
		if (data._mVar1 != -1) {
			if (data._mVar1 != MIS_CBOLT)
				multimissiles = 1;
			else
				multimissiles = 3;
			for (mi = 0; mi < multimissiles; mi++) {
				AddMissile(data._m, data._menemypos, data._mdir, data._mVar1,
				    1, i, data._mVar2, 0);
			}
		}
		PlayEffect(i, 0);
	}

	if (data._mAnimFrame == data._mAnimLen) {
		M_StartStand(data._mdir);
		return true;
	}
	return false;
}

int MonsterInstance::M_DoRSpAttack()
{
	if (data._mAnimFrame == data.MData->mAFNum2 && !data._mAnimCnt) {
		AddMissile(data._m, data._menemypos, data._mdir, data._mVar1,
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
		return true;
	}
	return false;
}

bool MonsterInstance::M_DoSAttack()
{
	if (data._mAnimFrame == data.MData->mAFNum2)
		M_TryH2HHit(data._menemy, data.mHit2, data.mMinDamage2, data.mMaxDamage2);

	if (data._mAnimFrame == data._mAnimLen) {
		M_StartStand(data._mdir);
		return true;
	}
	return false;
}

bool MonsterInstance::M_DoFadein()
{
	if ((!(data._mFlags & MFLAG_LOCK_ANIMATION) || data._mAnimFrame != 1)
	    && (data._mFlags & MFLAG_LOCK_ANIMATION || data._mAnimFrame != data._mAnimLen)) {
		return false;
	}

	M_StartStand(data._mdir);
	data._mFlags &= ~MFLAG_LOCK_ANIMATION;

	return true;
}

bool MonsterInstance::M_DoFadeout()
{
	if ((!(data._mFlags & MFLAG_LOCK_ANIMATION) || data._mAnimFrame != 1)
	    && (data._mFlags & MFLAG_LOCK_ANIMATION || data._mAnimFrame != data._mAnimLen)) {
		return false;
	}

	int mt = data.MType->mtype;
	if (mt < MT_INCIN || mt > MT_HELLBURN) {
		data._mFlags &= ~MFLAG_LOCK_ANIMATION;
		data._mFlags |= MFLAG_HIDDEN;
	} else {
		data._mFlags &= ~MFLAG_LOCK_ANIMATION;
	}

	M_StartStand(data._mdir);

	return true;
}

int MonsterInstance::M_DoHeal()
{
	if (data._mFlags & MFLAG_NOHEAL) {
		data._mFlags &= ~MFLAG_ALLOW_SPECIAL;
		data._mmode = MM_SATTACK;
		return false;
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
	return false;
}

int MonsterInstance::M_DoTalk()
{
	int tren;
	M_StartStand(data._mdir);
	data._mgoal = MGOAL_TALKING;
	if (effect_is_playing(alltext[data.mtalkmsg].sfxnr))
		return false;
	InitQTextMsg(data.mtalkmsg);
	if (data.mName == UniqMonst[UMT_GARBUD].mName) {
		if (data.mtalkmsg == TEXT_GARBUD1)
			quests[Q_GARBUD]._qactive = QUEST_ACTIVE;
		quests[Q_GARBUD]._qlog = true;
		if (data.mtalkmsg == TEXT_GARBUD2 && !(data._mFlags & MFLAG_QUEST_COMPLETE)) {
			SpawnItem(i, { data._m.x + 1, data._m.y + 1 }, true);
			data._mFlags |= MFLAG_QUEST_COMPLETE;
		}
	}
	if (data.mName == UniqMonst[UMT_ZHAR].mName
	    && data.mtalkmsg == TEXT_ZHAR1
	    && !(data._mFlags & MFLAG_QUEST_COMPLETE)) {
		quests[Q_ZHAR]._qactive = QUEST_ACTIVE;
		quests[Q_ZHAR]._qlog = true;
		CreateTypeItem({ data._m.x + 1, data._m.y + 1 }, false, ITYPE_MISC, IMISC_BOOK, true, false);
		data._mFlags |= MFLAG_QUEST_COMPLETE;
	}
	if (data.mName == UniqMonst[UMT_SNOTSPIL].mName) {
		if (data.mtalkmsg == TEXT_BANNER10 && !(data._mFlags & MFLAG_QUEST_COMPLETE)) {
			ObjChangeMap({ lvl.getpc().x, lvl.getpc().y }, { (lvl.getpc().w >> 1) + lvl.getpc().x + 2, (lvl.getpc().h >> 1) + lvl.getpc().y - 2 });
			tren = lvl.TransVal;
			lvl.TransVal = 9;
			DRLG_MRectTrans(lvl.getpc().x, lvl.getpc().y, (lvl.getpc().w >> 1) + lvl.getpc().x + 4, lvl.getpc().y + (lvl.getpc().h >> 1));
			lvl.TransVal = tren;
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
			quests[Q_VEIL]._qlog = true;
		}
		if (data.mtalkmsg == TEXT_VEIL11 && !(data._mFlags & MFLAG_QUEST_COMPLETE)) {
			SpawnUnique(UITEM_STEELVEIL, { data._m.x + 1, data._m.y + 1 });
			data._mFlags |= MFLAG_QUEST_COMPLETE;
		}
	}
	if (data.mName == UniqMonst[UMT_WARLORD].mName)
		quests[Q_WARLORD]._qvar1 = 2;
	if (data.mName == UniqMonst[UMT_LAZURUS].mName && plr.isMultiplayer()) {
		data._msquelch = UINT8_MAX;
		data.mtalkmsg = 0;
		quests[Q_BETRAYER]._qvar1 = 6;
		data._mgoal = MGOAL_NORMAL;
	}
	return false;
}

void MonsterInstance::M_Teleport()
{
	bool tren = false;
	int k, j, x, y, _mx, _my, rx, ry;

	if (data._mmode != MM_STONE) {
		_mx = data._menemypos.x;
		_my = data._menemypos.y;
		rx = 2 * random_(100, 2) - 1;
		ry = 2 * random_(100, 2) - 1;

		for (j = -1; j <= 1 && !tren; j++) {
			for (k = -1; k < 1 && !tren; k++) {
				if (j != 0 || k != 0) {
					x = _mx + rx * j;
					y = _my + ry * k;
					if (y >= 0 && y < MAXDUNY && x >= 0 && x < MAXDUNX && x != data._m.x && y != data._m.y) {
						if (PosOkMonst(i, { x, y }))
							tren = true;
					}
				}
			}
		}
	}

	if (tren) {
		M_ClearSquares();
		grid.at(data._m).clearMonster();
		grid[x][y].setActor(i);
		data._mold = { x, y };
		data._mdir = M_GetDir();
	}
}

bool MonsterInstance::M_DoGotHit()
{
	if (data._mAnimFrame == data._mAnimLen) {
		M_StartStand(data._mdir);
		return true;
	}
	return false;
}

bool MonsterInstance::M_DoDeath()
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
		if (data._uniqtype == 0) AddDead(data._m, data.MType->mdeadval, data._mdir);
		else AddDead(data._m, data._udeadval, data._mdir);

		grid.at(data._m).clearMonster();
		data._mDelFlag = true;

		M_UpdateLeader();
	}
	return false;
}

bool MonsterInstance::M_DoSpStand()
{
	if (data._mAnimFrame == data.MData->mAFNum2)
		PlayEffect(i, 3);

	if (data._mAnimFrame == data._mAnimLen) {
		M_StartStand(data._mdir);
		return true;
	}
	return false;
}

bool MonsterInstance::M_DoDelay()
{
	int mVar2;
	int oFrame;
	data._mAnimData = data.MType->Anims[MA_STAND].Data[int(M_GetDir())];
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
		return true;
	}
	return false;
}

bool MonsterInstance::M_DoStone()
{
	if (!data._mhitpoints) {
		grid.at(data._m).clearMonster();
		data._mDelFlag = true;
	}
	return false;
}

void MonsterInstance::M_WalkDir(Dir md)
{
	int mwi = data.MType->Anims[MA_WALK].Frames - 1;
	switch (md) {
	case Dir::N:
		M_StartWalk({ 0, -MWVel[mwi][1] }, { -1, -1 }, Dir::N);
		break;
	case Dir::NE:
		M_StartWalk({ MWVel[mwi][1], -MWVel[mwi][0] }, { 0, -1 }, Dir::NE);
		break;
	case Dir::E:
		M_StartWalk3({ MWVel[mwi][2], 0 }, { -32, -16 }, { 1, -1 }, { 1, 0 }, Dir::E);
		break;
	case Dir::SE:
		M_StartWalk2({ MWVel[mwi][1], MWVel[mwi][0] }, { -32, -16 }, { 1, 0 }, Dir::SE);
		break;
	case Dir::S:
		M_StartWalk2({ 0, MWVel[mwi][1] }, { 0, -32 }, { 1, 1 }, Dir::S);
		break;
	case Dir::SW:
		M_StartWalk2({ -MWVel[mwi][1], MWVel[mwi][0] }, { 32, -16 }, { 0, 1 }, Dir::SW);
		break;
	case Dir::W:
		M_StartWalk3({ -MWVel[mwi][2], 0 }, { 32, -16 }, { -1, 1 }, { 0, 1 }, Dir::W);
		break;
	case Dir::NW:
		M_StartWalk({ -MWVel[mwi][1], -MWVel[mwi][0] }, { -1, 0 }, Dir::NW);
		break;
	}
}

void MonsterInstance::GroupUnity()
{
	int leader, m, j;
	bool clear;
	if (data.leaderflag) {
		leader = data.leader;
		clear = LineClearF(CheckNoSolid, data._m, monsters[leader].data._mfut);
		if (clear || data.leaderflag != 1) {
			int dist = (data._m - monsters[leader].data._mfut).maxabs();
			if (clear
			    && data.leaderflag == 2
			    && dist < 4) {
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

bool MonsterInstance::M_CallWalk(Dir md)
{
	const Dir mdtemp = md;
	bool ok = DirOK(Dir(md));

	bool test = (md = left(mdtemp), DirOK(md));

	if (random_(101, 2))
		ok = ok || (md = left(mdtemp), DirOK(md)) || (md = right(mdtemp), DirOK(md));
	else
		ok = ok || (md = right(mdtemp), DirOK(md)) || (md = left(mdtemp), DirOK(md));
	if (random_(102, 2))
		ok = ok
		    || (md = right(right(mdtemp)), DirOK(md))
		    || (md = left(left(mdtemp)), DirOK(md));
	else
		ok = ok
		    || (md = left(left(mdtemp)), DirOK(md))
		    || (md = right(right(mdtemp)), DirOK(md));
	if (ok)
		M_WalkDir(md);
	return ok;
}

bool MonsterInstance::M_PathWalk()
{
	//char path[25];
	std::queue<PathNode> path;

	bool (*Check)
	(int, V2Di) = PosOkMonst3;
	if (!(data._mFlags & MFLAG_CAN_OPEN_DOOR))
		Check = PosOkMonst;

	int dist = pathfinder.FindPath(Check, i, data._m, data._menemypos, path);
	if (dist > 1) {
		M_CallWalk(GetDirection(data._m, path.front().pos())); /* plr2monst is local */
		return true;
	}

	return false;
}

bool MonsterInstance::M_CallWalk2(Dir md)
{
	bool ok = DirOK(md); // Can we continue in the same direction
	Dir mdtemp = md;
	if (random_(101, 2)) { // Randomly go left or right
		ok = ok || (mdtemp = left(md), DirOK(left(md))) || (mdtemp = right(md), DirOK(right(md)));
	} else {
		ok = ok || (mdtemp = right(md), DirOK(right(md))) || (mdtemp = left(md), DirOK(left(md)));
	}

	if (ok) M_WalkDir(mdtemp);
	return ok;
}

bool MonsterInstance::M_DumbWalk(Dir md)
{
	bool ok = DirOK(md);
	if (ok) M_WalkDir(md);
	return ok;
}

bool MonsterInstance::M_RoundWalk(Dir md, int *dir)
{
	Dir mdtemp = md;
	bool ok = DirOK(md);
	if (*dir)
		md = left(left(md));
	else
		md = right(right(md));

	if (!ok) {
		if (*dir) {
			md = right(mdtemp);
			ok = DirOK(md) || (md = right(right(mdtemp)), DirOK(md));
		} else {
			md = left(mdtemp);
			ok = (DirOK(md) || (md = left(left(mdtemp)), DirOK(md)));
		}
	}
	if (ok) {
		M_WalkDir(md);
	} else {
		*dir = !*dir;
		ok = M_CallWalk(opposite(mdtemp));
	}
	return ok;
}

void MonsterInstance::MAI_Zombie()
{
	if (data._mmode != MM_STAND)
		return;

	V2Di m = data._m;
	if (!(grid.at(m).dFlags & BFLAG_VISIBLE)) {
		return;
	}
	int dist = (m - data._menemypos).maxabs();
	Dir md = data._mdir;
	int v = random_(103, 100);
	if (dist >= 2) {
		if (v < 2 * data._mint + 10) {
			if (dist >= 2 * data._mint + 4) {
				if (random_(104, 100) < 2 * data._mint + 20) {
					md = Dir(random_(104, 8));
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
		data._mAnimData = data.MType->Anims[MA_STAND].Data[int(data._mdir)];
}

void MonsterInstance::MAI_SkelSd()
{
	if (data._mmode != MM_STAND || data._msquelch == 0)
		return;

	V2Di m = data._m;
	int dist = (m - data._menemypos).maxabs();
	Dir md = GetDirection(m, data._last);
	data._mdir = md;
	if (dist >= 2) {
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
		data._mAnimData = data.MType->Anims[MA_STAND].Data[int(md)];
}

bool MonsterInstance::MAI_Path()
{
	if (data.MType->mtype != MT_GOLEM) {
		if (data._msquelch == 0)
			return false;
		if (data._mmode != MM_STAND)
			return false;
		if (data._mgoal != MGOAL_NORMAL && data._mgoal != MGOAL_MOVE && data._mgoal != MGOAL_SHOOT)
			return false;
		if (data._m.x == 1 && data._m.y == 0)
			return false;
	}

	bool clear = LineClearF1(PosOkMonst2, i, data._m, data._menemypos);
	if (!clear || data._pathcount >= 5 && data._pathcount < 8) {
		if (data._mFlags & MFLAG_CAN_OPEN_DOOR) MonstCheckDoors(i);
		data._pathcount++;
		if (data._pathcount < 5) return false;
		if (M_PathWalk()) return true;
	}

	if (data.MType->mtype != MT_GOLEM)
		data._pathcount = 0;

	return false;
}

void MonsterInstance::MAI_Snake()
{
	if (data._mmode != MM_STAND || data._msquelch == 0) return;

	Dir tmp;
	char pattern[6] = { 1, 1, 0, -1, -1, 0 };
	int pnum = data._menemy;

	V2Di f = data._menemypos;
	int dist = (data._m - f).maxabs();
	Dir md = GetDirection(data._m, data._last);
	data._mdir = md;

	if (dist >= 2) {
		if (dist < 3 && LineClearF1(PosOkMonst, i, data._m, f) && data._mVar1 != MM_CHARGE) {
			if (AddMissile(data._m, f, md, MIS_RHINO, pnum, i, 0, 0) != -1) {
				PlayEffect(i, 0);
				grid.at(data._m).setActor(i);
				data._mmode = MM_CHARGE;
			}
		// This is all just code to choose a direction
		} else if (data._mVar1 == MM_DELAY || random_(106, 100) >= 35 - 2 * data._mint) {
			tmp = rotate(md, pattern[data._mgoalvar1]);
			data._mgoalvar1++;
			if (data._mgoalvar1 > 5) data._mgoalvar1 = 0;
			tmp = rotate(md, -data._mgoalvar2);

			if (int(md) > 0) {
				if (int(md) < 4) {
					md = right(Dir(data._mgoalvar2));
					data._mgoalvar2 = int(md);
				} else if (md == Dir(4)) {
					data._mgoalvar2 = int(tmp);
				} else {
					md = left(Dir(data._mgoalvar2));
					data._mgoalvar2 = int(md);
				}
			}
			if (!M_DumbWalk(Dir(data._mgoalvar2))) M_CallWalk2(data._mdir);
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
		data._mAnimData = data.MType->Anims[MA_STAND].Data[int(data._mdir)];
}

void MonsterInstance::MAI_Bat()
{
	if (data._mmode != MM_STAND || data._msquelch == 0) return;

	int pnum = data._menemy;
	V2Di f = data._menemypos;
	int dist = (data._m - f).maxabs();
	Dir md = GetDirection(data._m, data._last);
	data._mdir = md;
	int v = random_(107, 100);

	if (data._mgoal == MGOAL_RETREAT) {
		if (!data._mgoalvar1) {
			M_CallWalk(opposite(md));
			data._mgoalvar1++;
		} else {
			if (random_(108, 2)) M_CallWalk(left(md));
			else M_CallWalk(right(md));
			data._mgoal = MGOAL_NORMAL;
		}
		return;
	}

	if (data.MType->mtype == MT_GLOOM
	    && (dist >= 5)
	    && v < 4 * data._mint + 33
	    && LineClearF1(PosOkMonst, i, data._m, f)) {
		if (AddMissile(data._m, f, md, MIS_RHINO, pnum, i, 0, 0) != -1) {
			grid.at(data._m).setActor(i);
			data._mmode = MM_CHARGE;
		}
	} else if (dist >= 2) {
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
			AddMissile(data._menemypos, { data._menemypos.x + 1, 0 }, Dir(-1), MIS_LIGHTNING, 1, i, random_(109, 10) + 1, 0);
		}
	}

	if (data._mmode == MM_STAND)
		data._mAnimData = data.MType->Anims[MA_STAND].Data[int(md)];
}

void MonsterInstance::MAI_SkelBow()
{
	if (data._mmode != MM_STAND || data._msquelch == 0) return;

	bool walking = false;
	V2Di f = data._menemypos;
	int dist = (data._m - f).maxabs();
	Dir md = GetDirection(data._m, data._last);
	data._mdir = md;

	int v = random_(110, 100);

	if (dist < 4) {
		if (data._mVar2 > 20 && v < 2 * data._mint + 13
		    || (data._mVar1 == MM_WALK || data._mVar1 == MM_WALK2 || data._mVar1 == MM_WALK3)
		        && data._mVar2 == 0
		        && v < 2 * data._mint + 63) {
			walking = M_DumbWalk(opposite(md));
		}
	}

	V2Di m = data._menemypos;
	if (!walking) {
		if (random_(110, 100) < 2 * data._mint + 3) {
			if (LineClear(data._m, m))
				M_StartRAttack(MIS_ARROW, 4);
		}
	}

	if (data._mmode == MM_STAND) data._mAnimData = data.MType->Anims[MA_STAND].Data[int(md)];
}

void MonsterInstance::MAI_Fat()
{
	if (data._mmode != MM_STAND || data._msquelch == 0)
		return;

	V2Di f = data._menemypos;
	int dist = (data._m - f).maxabs();
	Dir md = GetDirection(data._m, data._last);
	data._mdir = md;
	int v = random_(111, 100);
	if (dist >= 2) {
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
		data._mAnimData = data.MType->Anims[MA_STAND].Data[int(md)];
}

void MonsterInstance::MAI_Sneak()
{
	if (data._mmode != MM_STAND)
		return;

	V2Di mb = data._m;
	if (grid.at(mb).dLight != lightmax) {
		V2Di d = (mb - data._menemypos).abs();
		Dir md = M_GetDir();
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
			md = opposite(md);
			if (data.MType->mtype == MT_UNSEEN) {
				if (random_(112, 2))
					md = left(md);
				else
					md = right(md);
			}
		}
		data._mdir = md;
		int v = random_(112, 100);
		if (d.maxdim() < dist && data._mFlags & MFLAG_HIDDEN) {
			M_StartFadein(md, false);
		} else {
			if ((d.maxdim() >= dist + 1) && !(data._mFlags & MFLAG_HIDDEN)) {
				M_StartFadeout(md, true);
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
				data._mAnimData = data.MType->Anims[MA_STAND].Data[int(md)];
			else
				M_StartAttack();
		}
	}
}

void MonsterInstance::MAI_Fireman()
{
	if (data._mmode != MM_STAND || data._msquelch == 0) return;

	int pnum = data._menemy;
	V2Di f = data._menemypos;
	int dist = (data._m - f).maxabs();
	Dir md = M_GetDir();
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
			M_StartFadeout(md, true);
		} else if (LineClear(data._m, f)) {
			M_StartRAttack(MIS_KRULL, 4);
			data._mgoalvar1++;
		} else {
			M_StartDelay(random_(112, 10) + 5);
			data._mgoalvar1++;
		}
	} else if (data._mgoal == MGOAL_RETREAT) {
		M_StartFadein(md, false);
		data._mgoal = MGOAL_SHOOT;
	}
	data._mdir = md;
	random_(112, 100);
	if (data._mmode != MM_STAND)
		return;

	if (dist < 2 && data._mgoal == MGOAL_NORMAL) {
		M_TryH2HHit(data._menemy, data.mHit, data.mMinDamage, data.mMaxDamage);
		data._mgoal = MGOAL_RETREAT;
		if (!M_CallWalk(opposite(md))) {
			M_StartFadein(md, false);
			data._mgoal = MGOAL_SHOOT;
		}
	} else if (!M_CallWalk(md) && (data._mgoal == MGOAL_NORMAL || data._mgoal == MGOAL_RETREAT)) {
		M_StartFadein(md, false);
		data._mgoal = MGOAL_SHOOT;
	}
}

void MonsterInstance::MAI_Fallen()
{
	int m, rad;
	Dir md;

	if (data._mgoal == MGOAL_SHOOT) {
		if (data._mgoalvar1)
			data._mgoalvar1--;
		else
			data._mgoal = MGOAL_NORMAL;
	}

	if (data._mmode != MM_STAND || data._msquelch == 0) return;
	if (data._mgoal == MGOAL_RETREAT) {
		if (!data._mgoalvar1--) {
			data._mgoal = MGOAL_NORMAL;
			M_StartStand(opposite(data._mdir));
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
					m = grid[x + data._m.x][y + data._m.y].getActor();
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
		int dist = (data._m - data._menemypos).maxabs();
		if (dist < 2) {
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
	if (data._mmode != MM_STAND || data._msquelch == 0) return;

	V2Di f = data._menemypos;
	int dist = (data._m - f).maxabs();
	Dir md = GetDirection(data._m, data._last);
	data._mdir = md;

	if (dist >= 2) M_CallWalk(md);
	else M_StartAttack();

	if (data._mmode == MM_STAND)
		data._mAnimData = data.MType->Anims[MA_STAND].Data[int(md)];
}

void MonsterInstance::MAI_Round(bool special)
{
	if (data._mmode != MM_STAND || data._msquelch == 0) return;

	V2Di f = data._menemypos;
	int dist = (data._m - f).maxabs();
	Dir md = GetDirection(data._m, data._last);
	if (data._msquelch < UINT8_MAX)
		MonstCheckDoors(i);
	int v = random_(114, 100);
	if ((dist >= 2) && data._msquelch == UINT8_MAX && grid.at(data._m).dTransVal == grid.at(f).dTransVal) {
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
		data._mAnimData = data.MType->Anims[MA_STAND].Data[int(md)];
}

void MonsterInstance::MAI_GoatMc()
{
	MAI_Round(true);
}

void MonsterInstance::MAI_Ranged(int missile_type, bool special)
{
	if (data._mmode != MM_STAND) return;

	if (data._msquelch == UINT8_MAX || data._mFlags & MFLAG_TARGETS_MONSTER) {
		V2Di f = data._menemypos;
		int dist = (data._m - f).maxabs();
		Dir md = M_GetDir();
		if (data._msquelch < UINT8_MAX)
			MonstCheckDoors(i);
		data._mdir = md;
		if (data._mVar1 == MM_RATTACK) {
			M_StartDelay(random_(118, 20));
		} else if (dist < 4) {
			if (random_(119, 100) < 10 * (data._mint + 7))
				M_CallWalk(opposite(md));
		}
		if (data._mmode == MM_STAND) {
			if (LineClear(data._m, f)) {
				if (special)
					M_StartRSpAttack(missile_type, 4);
				else
					M_StartRAttack(missile_type, 4);
			} else {
				data._mAnimData = data.MType->Anims[MA_STAND].Data[int(md)];
			}
		}
	} else if (data._msquelch != 0) {
		Dir md = GetDirection(data._m, data._last);
		M_CallWalk(md);
	}
}

void MonsterInstance::MAI_GoatBow()
{
	MAI_Ranged(MIS_ARROW, false);
}

void MonsterInstance::MAI_Succ()
{
	MAI_Ranged(MIS_FLARE, false);
}

void MonsterInstance::MAI_AcidUniq()
{
	MAI_Ranged(MIS_ACID, true);
}

void MonsterInstance::MAI_Scav()
{
	int x, y;
	V2Di _m = data._m;
	bool done = false;
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
	if (data._mmode == MM_STAND)
		MAI_SkelSd();
}

void MonsterInstance::MAI_Garg()
{
	Dir md = M_GetDir();
	if (data._msquelch != 0 && data._mFlags & MFLAG_ALLOW_SPECIAL) {
		M_Enemy();
		int dist = (data._m - data._last).maxabs();
		if (dist < data._mint + 2) {
			data._mFlags &= ~MFLAG_ALLOW_SPECIAL;
		}
		return;
	}

	if (data._mmode != MM_STAND || data._msquelch == 0) return;

	if (data._mhitpoints<data._mmaxhp >> 1 && !(data._mFlags & MFLAG_NOHEAL))
		data._mgoal = MGOAL_RETREAT;
	if (data._mgoal == MGOAL_RETREAT) {
		int dist = (data._m - data._last).maxabs();
		if (dist >= data._mint + 2) {
			data._mgoal = MGOAL_NORMAL;
			M_StartHeal();
		} else if (!M_CallWalk(opposite(md))) {
			data._mgoal = MGOAL_NORMAL;
		}
	}
	MAI_Round(false);
}

void MonsterInstance::MAI_RoundRanged(int missile_type, bool checkdoors, int dam, int lessmissiles)
{
	if (data._mmode != MM_STAND || data._msquelch == 0)
		return;

	V2Di f = data._menemypos;
	int dist = (data._m - f).maxabs();
	Dir md = GetDirection(data._m, data._last);
	if (checkdoors && data._msquelch < UINT8_MAX)
		MonstCheckDoors(i);
	int v = random_(121, 10000);
	if ((dist >= 2) && data._msquelch == UINT8_MAX && grid.at(data._m).dTransVal == grid.at(f).dTransVal) {
		if (data._mgoal == MGOAL_MOVE || ((dist >= 3) && random_(122, 4 << lessmissiles) == 0)) {
			if (data._mgoal != MGOAL_MOVE) {
				data._mgoalvar1 = 0;
				data._mgoalvar2 = random_(123, 2);
			}
			data._mgoal = MGOAL_MOVE;
			if (data._mgoalvar1++ >= 2 * dist && DirOK(md)) {
				data._mgoal = MGOAL_NORMAL;
			} else if (v < 500 * (data._mint + 1) >> lessmissiles
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

void MonsterInstance::MAI_Magma()
{
	MAI_RoundRanged(MIS_MAGMABALL, true, 4, 0);
}

void MonsterInstance::MAI_Storm()
{
	MAI_RoundRanged(MIS_LIGHTCTRL2, true, 4, 0);
}

void MonsterInstance::MAI_Acid()
{
	MAI_RoundRanged(MIS_ACID, false, 4, 1);
}

void MonsterInstance::MAI_Diablo()
{
	MAI_RoundRanged(MIS_DIABAPOCA, false, 40, 0);
}

void MonsterInstance::MAI_RR2(int mistype, int dam)
{
	V2Di f = data._menemypos;
	int dist = (data._m - f).maxabs();
	if (dist >= 5) {
		MAI_SkelSd();
		return;
	}

	if (data._mmode != MM_STAND || data._msquelch == 0) return;

	Dir md = GetDirection(data._m, data._last);
	if (data._msquelch < UINT8_MAX)
		MonstCheckDoors(i);
	int v = random_(121, 100);
	if ((dist >= 2) && data._msquelch == UINT8_MAX && grid.at(data._m).dTransVal == grid.at(f).dTransVal) {
		if (data._mgoal == MGOAL_MOVE || (dist >= 3)) {
			if (data._mgoal != MGOAL_MOVE) {
				data._mgoalvar1 = 0;
				data._mgoalvar2 = random_(123, 2);
			}
			data._mgoal = MGOAL_MOVE;
			data._mgoalvar3 = 4;
			if (data._mgoalvar1++ < 2 * dist || !DirOK(md)) {
				if (v < 5 * (data._mint + 16))
					M_RoundWalk(md, &data._mgoalvar2);
			} else
				data._mgoal = MGOAL_NORMAL;
		}
	} else
		data._mgoal = MGOAL_NORMAL;
	if (data._mgoal == MGOAL_NORMAL) {
		if (((dist >= 3) && v < 5 * (data._mint + 2) || v < 5 * (data._mint + 1) || data._mgoalvar3 == 4) && LineClear(data._m, f)) {
			M_StartRSpAttack(mistype, dam);
		} else if (dist >= 2) {
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

void MonsterInstance::MAI_Mega()
{
	MAI_RR2(MIS_FLAMEC, 0);
}

void MonsterInstance::MAI_Golum()
{
	int j, k, _menemy;
	Dir md;
	bool have_enemy, ok;
	if (data._m.x == 1 && data._m.y == 0) {
		return;
	}

	if (data._mmode == MM_DEATH
	    || data._mmode == MM_SPSTAND
	    || (data._mmode >= MM_WALK && data._mmode <= MM_WALK3)) {
		return;
	}

	if (!(data._mFlags & MFLAG_TARGETS_MONSTER)) M_Enemy();

	have_enemy = !(data._mFlags & MFLAG_NO_ENEMY);

	if (data._mmode == MM_ATTACK)
		return;
	_menemy = data._menemy;
	V2Di m = data._m;
	int dist = (m - monsters[_menemy].data._mfut).maxabs();
	md = GetDirection(m, monsters[_menemy].data._m);
	data._mdir = md;
	if (dist >= 2) {
		if (have_enemy && MAI_Path()) return;
	} else if (have_enemy) {
		_menemy = data._menemy;
		data._menemypos = monsters[_menemy].data._m;
		if (monsters[_menemy].data._msquelch == 0) {
			monsters[_menemy].data._msquelch = UINT8_MAX;
			monsters[data._menemy].data._last = data._m;
			for (j = 0; j < 5; j++) {
				for (k = 0; k < 5; k++) {
					_menemy = grid[data._m.x + k - 2][data._m.y + j - 2].getActor();
					if (_menemy > 0)
						monsters[_menemy].data._msquelch = UINT8_MAX;
				}
			}
		}
		M_StartAttack();
		return;
	}

	data._pathcount++;
	if (data._pathcount > 8) data._pathcount = 5;

	ok = M_CallWalk(plr[i].data._pdir);
	if (!ok) {
		md = rotate(md, -1);
		for (j = 0; j < 8 && !ok; j++) {
			md = rotate(md, 1);
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
	if (data._mmode != MM_STAND || data._msquelch == 0) return;

	V2Di f = data._menemypos;
	int dist = (data._m - f).maxabs();
	Dir md = GetDirection(data._m, data._last);
	if (data._msquelch < UINT8_MAX) MonstCheckDoors(i);
	int v = random_(126, 100);
	if ((dist >= 2) && data._msquelch == UINT8_MAX && grid.at(data._m).dTransVal == grid.at(f).dTransVal) {
		if (data._mgoal == MGOAL_MOVE || (dist >= 3) && random_(127, 4) == 0) {
			if (data._mgoal != MGOAL_MOVE) {
				data._mgoalvar1 = 0;
				data._mgoalvar2 = random_(128, 2);
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
		if (plr.isSingleplayer()
		    && ((dist >= 3) && v < 4 * data._mint + 35 || v < 6)
		    && LineClear(data._m, f)) {
			V2Di n = data._m + offset(md);
			if (PosOkMonst(i, n) && nummonsters < MAXMONSTERS) {
				M_SpawnSkel(n, md);
				M_StartSpStand(md);
			}
		} else {
			if (dist >= 2) {
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
	if (data._mmode == MM_STAND) data._mAnimData = data.MType->Anims[MA_STAND].Data[int(md)];
}

void MonsterInstance::MAI_Rhino()
{
	if (data._mmode != MM_STAND || data._msquelch == 0) return;

	V2Di f = data._menemypos;
	int dist = (data._m - f).maxabs();
	Dir md = GetDirection(data._m, data._last);
	if (data._msquelch < UINT8_MAX)
		MonstCheckDoors(i);
	int v = random_(131, 100);
	if (dist >= 2) {
		if (data._mgoal == MGOAL_MOVE || (dist >= 5) && random_(132, 4) != 0) {
			if (data._mgoal != MGOAL_MOVE) {
				data._mgoalvar1 = 0;
				data._mgoalvar2 = random_(133, 2);
			}
			data._mgoal = MGOAL_MOVE;
			if (data._mgoalvar1++ >= 2 * dist || grid.at(data._m).dTransVal != grid.at(f).dTransVal) {
				data._mgoal = MGOAL_NORMAL;
			} else if (!M_RoundWalk(md, &data._mgoalvar2)) {
				M_StartDelay(random_(125, 10) + 10);
			}
		}
	} else
		data._mgoal = MGOAL_NORMAL;
	if (data._mgoal == MGOAL_NORMAL) {
		if ((dist >= 5)
		    && v < 2 * data._mint + 43
		    && LineClearF1(PosOkMonst, i, data._m, f)) {
			if (AddMissile(data._m, f, md, MIS_RHINO, data._menemy, i, 0, 0) != -1) {
				if (data.MData->snd_special)
					PlayEffect(i, 3);
				data._mmode = MM_CHARGE;
				grid.at(data._m).setActor(i);
			}
		} else {
			if (dist >= 2) {
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
		data._mAnimData = data.MType->Anims[MA_STAND].Data[int(data._mdir)];
}

void MonsterInstance::MAI_Counselor()
{
	if (data._mmode != MM_STAND || data._msquelch == 0) return;

	V2Di f = data._menemypos;
	int dist = (data._m - f).maxabs();
	Dir md = GetDirection(data._m, data._last);
	if (data._msquelch < UINT8_MAX)
		MonstCheckDoors(i);
	int v = random_(121, 100);
	if (data._mgoal == MGOAL_RETREAT) {
		if (data._mgoalvar1++ <= 3)
			M_CallWalk(opposite(md));
		else {
			data._mgoal = MGOAL_NORMAL;
			M_StartFadein(md, true);
		}
	} else if (data._mgoal == MGOAL_MOVE) {
		if ((dist >= 2) && data._msquelch == UINT8_MAX && grid.at(data._m).dTransVal == grid.at(f).dTransVal) {
			if (data._mgoalvar1++ < 2 * dist || !DirOK(md)) {
				M_RoundWalk(md, &data._mgoalvar2);
			} else {
				data._mgoal = MGOAL_NORMAL;
				M_StartFadein(md, true);
			}
		} else {
			data._mgoal = MGOAL_NORMAL;
			M_StartFadein(md, true);
		}
	} else if (data._mgoal == MGOAL_NORMAL) {
		if (dist >= 2) {
			if (v < 5 * (data._mint + 10) && LineClear(data._m, f)) {
				M_StartRAttack(counsmiss[data._mint], data.mMinDamage + random_(77, data.mMaxDamage - data.mMinDamage + 1));
			} else if (random_(124, 100) < 30) {
				data._mgoal = MGOAL_MOVE;
				data._mgoalvar1 = 0;
				M_StartFadeout(md, false);
			} else
				M_StartDelay(random_(105, 10) + 2 * (5 - data._mint));
		} else {
			data._mdir = md;
			if (data._mhitpoints < (data._mmaxhp >> 1)) {
				data._mgoal = MGOAL_RETREAT;
				data._mgoalvar1 = 0;
				M_StartFadeout(md, false);
			} else if (data._mVar1 == MM_DELAY
			    || random_(105, 100) < 2 * data._mint + 20) {
				M_StartRAttack(-1, 0);
				AddMissile(data._m, { 0, 0 }, data._mdir, MIS_FLASH, 1, i, 4, 0);
				AddMissile(data._m, { 0, 0 }, data._mdir, MIS_FLASH2, 1, i, 4, 0);
			} else
				M_StartDelay(random_(105, 10) + 2 * (5 - data._mint));
		}
	}
	if (data._mmode == MM_STAND) {
		M_StartDelay(random_(125, 10) + 5);
	}
}

void MonsterInstance::MAI_Garbud()
{
	if (data._mmode != MM_STAND)
		return;

	V2Di _m = data._m;
	Dir md = M_GetDir();

	if (data.mtalkmsg < TEXT_GARBUD4
	    && data.mtalkmsg > TEXT_DOOM10
	    && !(grid.at(_m).dFlags & BFLAG_VISIBLE)
	    && data._mgoal == MGOAL_TALKING) {
		data._mgoal = MGOAL_INQUIRING;
		data.mtalkmsg++;
	}

	if (grid.at(_m).dFlags & BFLAG_VISIBLE) {
		if (data.mtalkmsg == TEXT_GARBUD4) {
			if (!effect_is_playing(USFX_GARBUD4) && data._mgoal == MGOAL_TALKING) {
				data._mgoal = MGOAL_NORMAL;
				data._msquelch = UINT8_MAX;
				data.mtalkmsg = 0;
			}
		}
	}

	if (data._mgoal == MGOAL_NORMAL || data._mgoal == MGOAL_MOVE)
		MAI_Round(true);

	data._mdir = md;

	if (data._mmode == MM_STAND)
		data._mAnimData = data.MType->Anims[MA_STAND].Data[int(md)];
}

void MonsterInstance::MAI_Zhar()
{
	if (data._mmode != MM_STAND) {
		return;
	}

	V2Di m = data._m;
	Dir md = M_GetDir();
	if (data.mtalkmsg == TEXT_ZHAR1 && !(grid.at(m).dFlags & BFLAG_VISIBLE) && data._mgoal == MGOAL_TALKING) {
		data.mtalkmsg = TEXT_ZHAR2;
		data._mgoal = MGOAL_INQUIRING;
	}

	if (grid.at(m).dFlags & BFLAG_VISIBLE) {
		V2Di _m = data._m - data._menemypos;
		if (data.mtalkmsg == TEXT_ZHAR2) {
			if (!effect_is_playing(USFX_ZHAR2) && data._mgoal == MGOAL_TALKING) {
				data._msquelch = UINT8_MAX;
				data.mtalkmsg = 0;
				data._mgoal = MGOAL_NORMAL;
			}
		}
	}

	if (data._mgoal == MGOAL_NORMAL || data._mgoal == MGOAL_RETREAT || data._mgoal == MGOAL_MOVE)
		MAI_Counselor();

	data._mdir = md;

	if (data._mmode == MM_STAND)
		data._mAnimData = data.MType->Anims[MA_STAND].Data[int(md)];
}

void MonsterInstance::MAI_SnotSpil()
{
	if (data._mmode != MM_STAND) return;

	V2Di m = data._m;
	Dir md = M_GetDir();

	if (data.mtalkmsg == TEXT_BANNER10 && !(grid.at(m).dFlags & BFLAG_VISIBLE) && data._mgoal == MGOAL_TALKING) {
		data.mtalkmsg = TEXT_BANNER11;
		data._mgoal = MGOAL_INQUIRING;
	}

	if (data.mtalkmsg == TEXT_BANNER11 && quests[Q_LTBANNER]._qvar1 == 3) {
		data.mtalkmsg = 0;
		data._mgoal = MGOAL_NORMAL;
	}

	if (grid.at(m).dFlags & BFLAG_VISIBLE) {
		if (data.mtalkmsg == TEXT_BANNER12) {
			if (!effect_is_playing(USFX_SNOT3) && data._mgoal == MGOAL_TALKING) {
				ObjChangeMap({ lvl.getpc().x, lvl.getpc().y }, { lvl.getpc().x + lvl.getpc().w + 1, lvl.getpc().y + lvl.getpc().h + 1 });
				quests[Q_LTBANNER]._qvar1 = 3;
				RedoPlayerVision();
				data._msquelch = UINT8_MAX;
				data.mtalkmsg = 0;
				data._mgoal = MGOAL_NORMAL;
			}
		}
		if (quests[Q_LTBANNER]._qvar1 == 3) {
			if (data._mgoal == MGOAL_NORMAL || data._mgoal == MGOAL_SHOOT)
				MAI_Fallen();
		}
	}

	data._mdir = md;

	if (data._mmode == MM_STAND)
		data._mAnimData = data.MType->Anims[MA_STAND].Data[int(md)];
}

void MonsterInstance::MAI_Lazurus()
{
	if (data._mmode != MM_STAND)
		return;

	V2Di m = data._m;
	Dir md = M_GetDir();
	if (grid.at(m).dFlags & BFLAG_VISIBLE) {
		if (plr.isSingleplayer()) {
			if (data.mtalkmsg == TEXT_VILE13 && data._mgoal == MGOAL_INQUIRING && myplr().pos().x == TEXT_VILE13 && myplr().pos().y == 46) {
				PlayInGameMovie("gendata\\fprst3.smk");
				data._mmode = MM_TALK;
				quests[Q_BETRAYER]._qvar1 = 5;
			}

			if (data.mtalkmsg == TEXT_VILE13 && !effect_is_playing(USFX_LAZ1) && data._mgoal == MGOAL_TALKING) {
				ObjChangeMapResync({ 1, 18 }, { 20, 24 });
				RedoPlayerVision();
				data._msquelch = UINT8_MAX;
				data.mtalkmsg = 0;
				quests[Q_BETRAYER]._qvar1 = 6;
				data._mgoal = MGOAL_NORMAL;
			}
		}

		if (plr.isMultiplayer() && data.mtalkmsg == TEXT_VILE13 && data._mgoal == MGOAL_INQUIRING && quests[Q_BETRAYER]._qvar1 <= 3) {
			data._mmode = MM_TALK;
		}
	}

	if (data._mgoal == MGOAL_NORMAL || data._mgoal == MGOAL_RETREAT || data._mgoal == MGOAL_MOVE) {
		data.mtalkmsg = 0;
		MAI_Counselor();
	}

	data._mdir = md;
	if (data._mmode == MM_STAND || data._mmode == MM_TALK)
		data._mAnimData = data.MType->Anims[MA_STAND].Data[int(md)];
}

void MonsterInstance::MAI_Lazhelp()
{
	if (data._mmode != MM_STAND)
		return;

	V2Di _m = data._m;
	Dir md = M_GetDir();

	if (grid.at(_m).dFlags & BFLAG_VISIBLE) {
		if (plr.isSingleplayer()) {
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
		data._mAnimData = data.MType->Anims[MA_STAND].Data[int(md)];
}

void MonsterInstance::MAI_Lachdanan()
{
	if (data._mmode != MM_STAND)
		return;

	V2Di _m = data._m;
	Dir md = M_GetDir();
	if (data.mtalkmsg == TEXT_VEIL9 && !(grid.at(_m).dFlags & BFLAG_VISIBLE) && data._mgoal == MGOAL_TALKING) {
		data.mtalkmsg = TEXT_VEIL10;
		data._mgoal = MGOAL_INQUIRING;
	}

	if (grid.at(_m).dFlags & BFLAG_VISIBLE) {
		if (data.mtalkmsg == TEXT_VEIL11) {
			if (!effect_is_playing(USFX_LACH3) && data._mgoal == MGOAL_TALKING) {
				data.mtalkmsg = 0;
				quests[Q_VEIL]._qactive = QUEST_DONE;
				M_StartKill(-1);
			}
		}
	}

	data._mdir = md;

	if (data._mmode == MM_STAND) data._mAnimData = data.MType->Anims[MA_STAND].Data[int(md)];
}

void MonsterInstance::MAI_Warlord()
{
	if (data._mmode != MM_STAND)
		return;

	V2Di m = data._m;
	Dir md = M_GetDir();
	if (grid.at(m).dFlags & BFLAG_VISIBLE) {
		if (data.mtalkmsg == TEXT_WARLRD9 && data._mgoal == MGOAL_INQUIRING)
			data._mmode = MM_TALK;
		if (data.mtalkmsg == TEXT_WARLRD9 && !effect_is_playing(USFX_WARLRD1) && data._mgoal == MGOAL_TALKING) {
			data._msquelch = UINT8_MAX;
			data.mtalkmsg = 0;
			data._mgoal = MGOAL_NORMAL;
		}
	}

	if (data._mgoal == MGOAL_NORMAL)
		MAI_SkelSd();

	data._mdir = md;

	if (data._mmode == MM_STAND || data._mmode == MM_TALK)
		data._mAnimData = data.MType->Anims[MA_STAND].Data[int(data._mdir)];
}

DEVILUTION_END_NAMESPACE
