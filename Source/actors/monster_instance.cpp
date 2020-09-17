/**
 * @file monster_instance.cpp
 *
 * Monster instance class
 */
#include "all.h"
#include "../3rdParty/Storm/Source/storm.h"

DEVILUTION_BEGIN_NAMESPACE

MonsterInstance::MonsterInstance(int new_i, int rd, int mtype, V2Di pos)
    : i(new_i)
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

// Plants in stationary position (no velocity or offset) facing specified direction
// Often called before starting a new action
// If no position specified, plants in current position
void MonsterInstance::PlantInPosition(int dir, V2Di * pos)
{
	if (pos) data._m = *pos;
	data._moff = { 0, 0 };
	data._mfut = data._m;
	data._mold = data._m;
	if (dir != std::numeric_limits<int>::max()) data._mdir = dir;
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
			if (grid.at(data._m).dTransVal == grid.at(plr[pnum].data._p).dTransVal)
				sameroom = TRUE;
			else
				sameroom = FALSE;
			if (abs(data._m.x - plr[pnum].data._p.x) > abs(data._m.y - plr[pnum].data._p.y))
				dist = data._m.x - plr[pnum].data._p.x;
			else
				dist = data._m.y - plr[pnum].data._p.y;
			dist = abs(dist);
			if ((sameroom && !bestsameroom)
			    || ((sameroom || !bestsameroom) && dist < best_dist)
			    || (_menemy == -1)) {
				data._mFlags &= ~MFLAG_TARGETS_MONSTER;
				_menemy = pnum;
				enemyx = plr[pnum].data._pfut.x;
				enemyy = plr[pnum].data._pfut.y;
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
		if (!(data._mFlags & MFLAG_GOLEM)
		    && ((abs(monsters[mi].data._m.x - data._m.x) >= 2 || abs(monsters[mi].data._m.y - data._m.y) >= 2) && !M_Ranged()
		        || (!(data._mFlags & MFLAG_GOLEM) && !(monsters[mi].data._mFlags & MFLAG_GOLEM)))) {
			continue;
		}
		sameroom = grid.at(data._m).dTransVal == grid.at(monsters[mi].data._m).dTransVal;
		if (abs(data._m.x - monsters[mi].data._m.x) > abs(data._m.y - monsters[mi].data._m.y))
			dist = data._m.x - monsters[mi].data._m.x;
		else
			dist = data._m.y - monsters[mi].data._m.y;
		dist = abs(dist);
		if ((sameroom && !bestsameroom)
		    || ((sameroom || !bestsameroom) && dist < best_dist)
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

int MonsterInstance::M_GetDir()
{
	return GetDirection(data._m, data._menemypos);
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

void MonsterInstance::M_StartSpStand(int md)
{
	NewMonsterAnim(&data.MType->Anims[MA_SPECIAL], md);
	data._mmode = MM_SPSTAND;
	PlantInPosition(md);
}

void MonsterInstance::M_StartWalk(V2Di vel, V2Di add, int EndDir)
{
	V2Di f = add + data._m;
	grid[f.x][f.y].dMonster = -(i + 1);
	data._mmode = MM_WALK;
	data._mold = data._m;
	data._mfut = f;
	data._mvel = vel;
	data._mVar1 = add.x;
	data._mVar2 = add.y;
	data._mVar3 = EndDir;
	data._mdir = EndDir;
	NewMonsterAnim(&data.MType->Anims[MA_WALK], EndDir);
	data._mVar6 = 0;
	data._mVar7 = 0;
	data._mVar8 = 0;
}

void MonsterInstance::M_StartWalk2(V2Di vel, V2Di off, V2Di add, int EndDir)
{
	V2Di f = add + data._m;
	grid[data._m.x][data._m.y].dMonster = -(i + 1);
	data._mVar1 = data._m.x;
	data._mVar2 = data._m.y;
	data._mold = data._m;
	data._m = f;
	data._mfut = f;
	grid[f.x][f.y].dMonster = i + 1;
	if (data._uniqtype != 0)
		ChangeLightXY(data.mlid, data._m);
	data._moff = off;
	data._mmode = MM_WALK2;
	data._mvel = vel;
	data._mVar3 = EndDir;
	data._mdir = EndDir;
	NewMonsterAnim(&data.MType->Anims[MA_WALK], EndDir);
	data._mVar6 = 16 * off.x;
	data._mVar7 = 16 * off.y;
	data._mVar8 = 0;
}

void MonsterInstance::M_StartWalk3(V2Di vel, V2Di off, V2Di add, V2Di map, int EndDir)
{
	V2Di f = add + data._m;
	V2Di n = map + data._m;

	if (data._uniqtype != 0)
		ChangeLightXY(data.mlid, n);

	grid[data._m.x][data._m.y].dMonster = -(i + 1);
	grid[f.x][f.y].dMonster = -(i + 1);
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
	data._mVar3 = EndDir;
	data._mdir = EndDir;
	NewMonsterAnim(&data.MType->Anims[MA_WALK], EndDir);
	data._mVar6 = 16 * off.x;
	data._mVar7 = 16 * off.y;
	data._mVar8 = 0;
}

void MonsterInstance::M_StartAttack()
{
	int md = M_GetDir();
	NewMonsterAnim(&data.MType->Anims[MA_ATTACK], md);
	data._mmode = MM_ATTACK;
	PlantInPosition(md);
}

void MonsterInstance::M_StartRAttack(int missile_type, int dam)
{
	int md = M_GetDir();
	NewMonsterAnim(&data.MType->Anims[MA_ATTACK], md);
	data._mmode = MM_RATTACK;
	data._mVar1 = missile_type;
	data._mVar2 = dam;
	PlantInPosition(md);
}

void MonsterInstance::M_StartRSpAttack(int missile_type, int dam)
{
	int md = M_GetDir();
	NewMonsterAnim(&data.MType->Anims[MA_SPECIAL], md);
	data._mmode = MM_RSPATTACK;
	data._mVar1 = missile_type;
	data._mVar2 = 0;
	data._mVar3 = dam;
	PlantInPosition(md);
}

void MonsterInstance::M_StartSpAttack()
{
	int md = M_GetDir();
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
		data._mold.x += offset_x[d];
		data._mold.y += offset_y[d];
		NewMonsterAnim(&data.MType->Anims[MA_GOTHIT], data._mdir);
		data._mmode = MM_GOTHIT;
		data._moff = { 0, 0 };
		data._m = data._mold;
		data._mfut = data._m;
		// BUGFIX useless assignment
		data._mold = data._m;
		M_ClearSquares();
		grid.at(data._m).dMonster = i + 1;
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
			data._menemypos = plr[pnum].data._pfut;
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
			grid.at(data._m).dMonster = i + 1;
		}
	}
}

void MonsterInstance::M_DiabloDeath(BOOL sendmsg)
{
	int dist;
	int j, k;
	V2Di _mold;

#ifndef SPAWN
	PlaySFX(USFX_DIABLOD);
#endif
	quests[Q_DIABLO]._qactive = QUEST_DONE;
	if (sendmsg)
		NetSendCmdQuest(TRUE, Q_DIABLO);
	gbProcessPlayers = FALSE;
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
		grid.at(monsters[k].data._m).dMonster = k + 1;
	}
	AddLight(monsters[k].data._m, 8);
	DoVision(monsters[k].data._m, 8, FALSE, TRUE);
	if (abs(View.x - monsters[k].data._m.x) > abs(View.y - monsters[k].data._m.y))
		dist = abs(View.x - monsters[k].data._m.x);
	else
		dist = abs(View.y - monsters[k].data._m.y);
	if (dist > 20)
		dist = 20;
	j = View.x << 16;
	k = View.y << 16;
	monsters[k].data._mVar3 = j;
	monsters[k].data._mVar4 = k;
	monsters[k].data._mVar5 = (int)((j - (monsters[k].data._m.x << 16)) / (double)dist);
	monsters[k].data._mVar6 = (int)((k - (monsters[k].data._m.y << 16)) / (double)dist);
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
		CreateTypeItem({ data._m.x + 1, data._m.y + 1 }, TRUE, ITYPE_MACE, IMISC_NONE, TRUE, FALSE);
	} else if (i > MAX_PLRS - 1) { // Golems should not spawn items
		SpawnItem(i, data._m, sendmsg);
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
	PlantInPosition(data._mdir, &data._mold);
	data._mVar1 = 0;

	M_ClearSquares();
	grid.at(data._m).dMonster = i + 1;
	CheckQuestKill(i, sendmsg);
	M_FallenFear(data._m);
	if (data.MType->mtype >= MT_NACID && data.MType->mtype <= MT_XACID)
		AddMissile(data._m, { 0, 0 }, 0, MIS_ACIDPUD, 1, i, data._mint + 1, 0);
}

void MonsterInstance::M2MStartKill(int mid)
{
	int md;

	if ((DWORD)i >= MAXMONSTERS) { /// BUGFIX: should check `mid`
		app_fatal("M2MStartKill: Invalid monster (killed) %d", mid);
	}
	if (!data.MType)
		app_fatal("M2MStartKill: Monster %d \"%s\" MType NULL", mid, monsters[mid].data.mName);

	delta_kill_monster(mid, monsters[mid].data._m, level.currlevel);
	NetSendCmdLocParam1(FALSE, CMD_MONSTDEATH, monsters[mid].data._m, mid);

	monsters[mid].data.mWhoHit |= 1 << i;
	if (i < MAX_PLRS)
		AddPlrMonstExper(monsters[mid].data.mLevel, monsters[mid].data.mExp, monsters[mid].data.mWhoHit);

	monstkills[monsters[mid].data.MType->mtype]++;
	monsters[mid].data._mhitpoints = 0;
	SetRndSeed(monsters[mid].data._mRndSeed);

	if (mid >= MAX_PLRS)
		SpawnItem(mid, monsters[mid].data._m, TRUE);

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
	monsters[mid].PlantInPosition(md, &monsters[mid].data._mold);
	monsters[mid].M_ClearSquares();
	grid.at(monsters[mid].data._m).dMonster = mid + 1;
	CheckQuestKill(mid, TRUE);
	M_FallenFear(monsters[mid].data._m);
	if (monsters[mid].data.MType->mtype >= MT_NACID && monsters[mid].data.MType->mtype <= MT_XACID)
		AddMissile(monsters[mid].data._m, { 0, 0 }, 0, MIS_ACIDPUD, 1, mid, monsters[mid].data._mint + 1, 0);
}

void MonsterInstance::M_StartKill(int pnum)
{
	if (myplr() == pnum) {
		delta_kill_monster(i, data._m, level.currlevel);
		if (i != pnum) {
			NetSendCmdLocParam1(FALSE, CMD_MONSTDEATH, data._m, i);
		} else {
			NetSendCmdLocParam1(FALSE, CMD_KILLGOLEM, data._m, level.currlevel);
		}
	}
	MonstStartKill(pnum, TRUE);
}

void MonsterInstance::M_SyncStartKill(V2Di pos, int pnum)
{
	if (data._mhitpoints == 0 || data._mmode == MM_DEATH) {
		return;
	}

	if (grid[pos.x][pos.y].dMonster == 0) {
		M_ClearSquares();
		data._m = pos;
		data._mold = pos;
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
	PlantInPosition(md);
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
	PlantInPosition(md);
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
		grid.at(data._m).dMonster = 0;
		data._m.x += data._mVar1;
		data._m.y += data._mVar2;
		grid.at(data._m).dMonster = i + 1;
		if (data._uniqtype != 0)
			ChangeLightXY(data.mlid, data._m);
		M_StartStand(data._mdir);
		rv = TRUE;
	} else if (!data._mAnimCnt) {
		data._mVar8++;
		data._mVar6 += data._mvel.x;
		data._mVar7 += data._mvel.y;
		data._moff.x = data._mVar6 >> 4;
		data._moff.y = data._mVar7 >> 4;
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
			ChangeLightXY(data.mlid, data._m);
		M_StartStand(data._mdir);
		rv = TRUE;
	} else {
		if (!data._mAnimCnt) {
			data._mVar8++;
			data._mVar6 += data._mvel.x;
			data._mVar7 += data._mvel.y;
			data._moff.x = data._mVar6 >> 4;
			data._moff.y = data._mVar7 >> 4;
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
		grid.at(data._m).dMonster = 0;
		data._m = { data._mVar1, data._mVar2 };
		grid[data._mVar4][data._mVar5].dFlags &= ~BFLAG_MONSTLR;
		grid.at(data._m).dMonster = i + 1;
		if (data._uniqtype)
			ChangeLightXY(data.mlid, data._m);
		M_StartStand(data._mdir);
		rv = TRUE;
	} else {
		if (!data._mAnimCnt) {
			data._mVar8++;
			data._mVar6 += data._mvel.x;
			data._mVar7 += data._mvel.y;
			data._moff.x = data._mVar6 >> 4;
			data._moff.y = data._mVar7 >> 4;
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
	int blk, blkper;
	int dam, mdam;
	V2Di d, newpos;
	int j, misnum, ms_num, cur_ms_num, new_hp;

	if (data._mFlags & MFLAG_TARGETS_MONSTER) {
		M_TryM2MHit(pnum, Hit, MinDam, MaxDam);
		return;
	}
	if (plr[pnum].data._pHitPoints >> 6 <= 0 || plr[pnum].data._pInvincible || plr[pnum].data._pSpellFlags & 1)
		return;
	d.x = abs(data._m.x - plr[pnum].data._p.x);
	d.y = abs(data._m.y - plr[pnum].data._p.y);
	if (d.x >= 2 || d.y >= 2)
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
		plr[pnum].StartPlrBlock(GetDirection(plr[pnum].data._p, data._m));
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
		newpos.x = plr[pnum].data._p.x + offset_x[data._mdir];
		newpos.y = plr[pnum].data._p.y + offset_y[data._mdir];
		if (PosOkPlayer(pnum, newpos)) {
			plr[pnum].data._p = newpos;
			plr[pnum].FixPlayerLocation(plr[pnum].data._pdir);
			plr[pnum].FixPlrWalkTags();
			grid.at(newpos).dPlayer = pnum + 1;
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
				AddMissile(data._m, data._menemypos, data._mdir, data._mVar1,
				    1, i, data._mVar2, 0);
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
			SpawnItem(i, { data._m.x + 1, data._m.y + 1 }, TRUE);
			data._mFlags |= MFLAG_QUEST_COMPLETE;
		}
	}
	if (data.mName == UniqMonst[UMT_ZHAR].mName
	    && data.mtalkmsg == TEXT_ZHAR1
	    && !(data._mFlags & MFLAG_QUEST_COMPLETE)) {
		quests[Q_ZHAR]._qactive = QUEST_ACTIVE;
		quests[Q_ZHAR]._qlog = TRUE;
		CreateTypeItem({ data._m.x + 1, data._m.y + 1 }, FALSE, ITYPE_MISC, IMISC_BOOK, TRUE, FALSE);
		data._mFlags |= MFLAG_QUEST_COMPLETE;
	}
	if (data.mName == UniqMonst[UMT_SNOTSPIL].mName) {
		if (data.mtalkmsg == TEXT_BANNER10 && !(data._mFlags & MFLAG_QUEST_COMPLETE)) {
			ObjChangeMap(setpc.x, setpc.y, (setpc.w >> 1) + setpc.x + 2, (setpc.h >> 1) + setpc.y - 2);
			tren = TransVal;
			TransVal = 9;
			DRLG_MRectTrans(setpc.x, setpc.y, (setpc.w >> 1) + setpc.x + 4, setpc.y + (setpc.h >> 1));
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
			SpawnUnique(UITEM_STEELVEIL, { data._m.x + 1, data._m.y + 1 });
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
							tren = TRUE;
					}
				}
			}
		}
	}

	if (tren) {
		M_ClearSquares();
		grid.at(data._m).dMonster = 0;
		grid[x][y].dMonster = i + 1;
		data._mold = { x, y };
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

DEVILUTION_END_NAMESPACE
