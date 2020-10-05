/**
 * @file monster_instance.cpp
 *
 * Monster instance class
 */
#include "all.h"
#include "../3rdParty/Storm/Source/storm.h"

DEVILUTION_BEGIN_NAMESPACE

Monster::Monster()
	: Actor(ActorType::monster)
{};

Monster::Monster(Dir rd, int mtype, V2Di pos)
    : Actor(ActorType::monster)
{
	CMonster *monst = &beastiary[mtype].data;

	_mdir = rd;
	_m = pos;
	_mfut = pos;
	_mold = pos;
	_mMTidx = mtype;
	_mmode = MonsterMode::STAND;
	mName = monst->MData->mName;
	MType = monst;
	MData = monst->MData;
	_mAnimData = monst->getAnim(MonstAnim::STAND).Data[int(rd)];
	_mAnimDelay = monst->getAnim(MonstAnim::STAND).Rate;
	_mAnimCnt = random_(88, _mAnimDelay - 1);
	_mAnimLen = monst->getAnim(MonstAnim::STAND).Frames;
	_mAnimFrame = random_(88, _mAnimLen - 1) + 1;

	if (monst->mtype == MonsterType::DIABLO) {
		_mmaxhp = (random_(88, 1) + 1666) << 6;
	} else {
		_mmaxhp = (monst->mMinHP + random_(88, monst->mMaxHP - monst->mMinHP + 1)) << 6;
	}

	if (plr.isSingleplayer()) {
		_mmaxhp >>= 1;
		if (_mmaxhp < 64) {
			_mmaxhp = 64;
		}
	}

	_mhitpoints = _mmaxhp;
	_mAi = monst->MData->mAi;
	_mint = monst->MData->mInt;
	_mgoal = MonsterGoal::NORMAL;
	_mgoalvar1 = 0;
	_mgoalvar2 = 0;
	_mgoalvar3 = 0;
	field_18 = 0;
	_pathcount = 0;
	_mDelFlag = false;
	_uniqtype = 0;
	_msquelch = 0;
	_mRndSeed = GetRndSeed();
	_mAISeed = GetRndSeed();
	mWhoHit = 0;
	mLevel = monst->MData->mLevel;
	mExp = monst->MData->mExp;
	mHit = monst->MData->mHit;
	mMinDamage = monst->MData->mMinDamage;
	mMaxDamage = monst->MData->mMaxDamage;
	mHit2 = monst->MData->mHit2;
	mMinDamage2 = monst->MData->mMinDamage2;
	mMaxDamage2 = monst->MData->mMaxDamage2;
	mArmorClass = monst->MData->mArmorClass;
	mMagicRes = monst->MData->mMagicRes;
	leader = 0;
	leaderflag = 0;
	_mFlags = monst->MData->mFlags;
	mtalkmsg = 0;

	if (_mAi == MonstAi::GARG) {
		_mAnimData = monst->getAnim(MonstAnim::SPECIAL).Data[int(rd)];
		_mAnimFrame = 1;
		_mFlags |= MonsterFlag::allow_special;
		_mmode = MonsterMode::SATTACK;
	}

	if (gnDifficulty == DIFF_NIGHTMARE) {
		_mmaxhp = 3 * _mmaxhp + 64;
		_mhitpoints = _mmaxhp;
		mLevel += 15;
		mExp = 2 * (mExp + 1000);
		mHit += 85;
		mMinDamage = 2 * (mMinDamage + 2);
		mMaxDamage = 2 * (mMaxDamage + 2);
		mHit2 += 85;
		mMinDamage2 = 2 * (mMinDamage2 + 2);
		mMaxDamage2 = 2 * (mMaxDamage2 + 2);
		mArmorClass += 50;
	}

	if (gnDifficulty == DIFF_HELL) {
		_mmaxhp = 4 * _mmaxhp + 192;
		_mhitpoints = _mmaxhp;
		mLevel += 30;
		mExp = 4 * (mExp + 1000);
		mHit += 120;
		mMinDamage = 4 * mMinDamage + 6;
		mMaxDamage = 4 * mMaxDamage + 6;
		mHit2 += 120;
		mMinDamage2 = 4 * mMinDamage2 + 6;
		mMaxDamage2 = 4 * mMaxDamage2 + 6;
		mArmorClass += 80;
		mMagicRes = monst->MData->mMagicRes2;
	}
}

// Plants in stationary position (no velocity or offset) facing specified direction
// Often called before starting a new action
// If no position specified, plants in current position
void Monster::PlantInPosition(Dir dir, V2Di * pos)
{
	if (pos) _m = *pos;
	_moff = { 0, 0 };
	_mfut = _m;
	_mold = _m;
	if (dir != Dir::NONE) _mdir = dir;
}

void Monster::ClearMVars()
{
	_mVar1 = 0;
	_mVar2 = 0;
	_mVar3 = 0;
	_mVar4 = 0;
	_mVar5 = 0;
	_mVar6 = 0;
	_mVar7 = 0;
	_mVar8 = 0;
}

void Monster::NewMonsterAnim(AnimStruct *anim, Dir md)
{
	_mAnimData = anim->Data[int(md)];
	_mAnimLen = anim->Frames;
	_mAnimCnt = 0;
	_mAnimFrame = 1;
	_mAnimDelay = anim->Rate;
	_mFlags &= ~(MonsterFlag::lock_animation | MonsterFlag::allow_special);
	_mdir = md;
}

void Monster::GetTargetLoc(V2Di &pos, int &dist, Dir &dir)
{
}

bool Monster::M_Ranged()
{
	MonstAi ai = _mAi;
	return ai == MonstAi::SKELBOW || ai == MonstAi::GOATBOW || ai == MonstAi::SUCC || ai == MonstAi::LAZHELP;
}

bool Monster::M_Talker()
{
	MonstAi ai = _mAi;
	return ai == MonstAi::LAZURUS
	    || ai == MonstAi::WARLORD
	    || ai == MonstAi::GARBUD
	    || ai == MonstAi::ZHAR
	    || ai == MonstAi::SNOTSPIL
	    || ai == MonstAi::LACHDAN
	    || ai == MonstAi::LAZHELP;
}

void Monster::M_Enemy()
{
	int mi, pnum;
	int dist;
	bool sameroom;
	uint8_t enemyx, enemyy;

	int _menemy = -1;
	int best_dist = -1;
	int bestsameroom = 0;
	//Monst = monster + i;
	if (!(_mFlags & MonsterFlag::golem)) {
		for (pnum = 0; pnum < MAX_PLRS; pnum++) {
			if (!plr[pnum].plractive || lvl.currlevel != plr[pnum].plrlevel || plr[pnum]._pLvlChanging || (plr[pnum]._pHitPoints == 0 && plr.isMultiplayer()))
				continue;
			if (grid.at(_m).dTransVal == grid.at(plr[pnum].pos()).dTransVal)
				sameroom = true;
			else
				sameroom = false;
			int dist = (_m - plr[pnum].pos()).maxabs();
			if ((sameroom && !bestsameroom)
			    || ((sameroom || !bestsameroom) && dist < best_dist)
			    || (_menemy == -1)) {
				_mFlags &= ~MonsterFlag::targets_monster;
				_menemy = pnum;
				enemyx = plr[pnum].futpos().x;
				enemyy = plr[pnum].futpos().y;
				best_dist = dist;
				bestsameroom = sameroom;
			}
		}
	}
	for (int j = 0; j < nummonsters; j++) {
		mi = monstactive[j];
		if (mi == i)
			continue;
		if (monsters[mi]._m.x == 1 && monsters[mi]._m.y == 0)
			continue;
		if (monsters[mi].M_Talker() && monsters[mi].mtalkmsg)
			continue;

		int dista = (monsters[mi]._m - _m).maxabs();
		if (!(_mFlags & MonsterFlag::golem)
		        && ((dista >= 2) && !M_Ranged()
		        || (!(_mFlags & MonsterFlag::golem) && !(monsters[mi]._mFlags & MonsterFlag::golem)))) {
			continue;
		}
		sameroom = grid.at(_m).dTransVal == grid.at(monsters[mi]._m).dTransVal;
		int distb = (_m - monsters[mi]._m).maxabs();
		if ((sameroom && !bestsameroom)
		    || ((sameroom || !bestsameroom) && distb < best_dist)
		    || (_menemy == -1)) {
			_mFlags |= MonsterFlag::targets_monster;
			_menemy = mi;
			enemyx = monsters[mi]._mfut.x;
			enemyy = monsters[mi]._mfut.y;
			best_dist = dist;
			bestsameroom = sameroom;
		}
	}
	if (_menemy != -1) {
		_mFlags &= ~MonsterFlag::no_enemy;
		_menemy = _menemy;
		_menemypos = { enemyx, enemyy };
	} else {
		_mFlags |= MonsterFlag::no_enemy;
	}
}

Dir Monster::M_GetDir()
{
	return GetDirection(_m, _menemypos);
}

void Monster::M_StartStand(Dir md)
{
	ClearMVars();
	if (MType->mtype == MonsterFlag::golem)
		NewMonsterAnim(&MType->getAnim(MonstAnim::WALK), md);
	else
		NewMonsterAnim(&MType->getAnim(MonstAnim::STAND), md);
	_mVar1 = _mmode;
	_mVar2 = 0;
	_mmode = MonsterMode::STAND;
	PlantInPosition(md);
	M_Enemy();
}

void Monster::M_StartDelay(int len)
{
	if (len <= 0) {
		return;
	}

	if (_mAi != MonstAi::LAZURUS) {
		_mVar2 = len;
		_mmode = MonsterMode::DELAY;
	}
}

void Monster::M_StartSpStand(Dir md)
{
	NewMonsterAnim(&MType->getAnim(MonstAnim::SPECIAL), md);
	_mmode = MonsterMode::SPSTAND;
	PlantInPosition(md);
}

void Monster::M_StartWalk(V2Di vel, V2Di add, Dir EndDir)
{
	V2Di f = add + _m;
	grid.at(f).setActor(*this);
	_mmode = MonsterMode::WALK;
	_mold = _m;
	_mfut = f;
	_mvel = vel;
	_mVar1 = add.x;
	_mVar2 = add.y;
	_mVar3 = int(EndDir);
	_mdir = EndDir;
	NewMonsterAnim(&MType->getAnim(MonstAnim::WALK), EndDir);
	_mVar6 = 0;
	_mVar7 = 0;
	_mVar8 = 0;
}

void Monster::M_StartWalk2(V2Di vel, V2Di off, V2Di add, Dir EndDir)
{
	V2Di f = add + _m;
	grid.at(_m).setActor(*this);
	_mVar1 = _m.x;
	_mVar2 = _m.y;
	_mold = _m;
	_m = f;
	_mfut = f;
	grid.at(f).setActor(*this);
	if (_uniqtype != 0) ChangeLightXY(mlid, _m);
	_moff = off;
	_mmode = MonsterMode::WALK2;
	_mvel = vel;
	_mVar3 = int(EndDir);
	_mdir = EndDir;
	NewMonsterAnim(&MType->getAnim(MonstAnim::WALK), EndDir);
	_mVar6 = 16 * off.x;
	_mVar7 = 16 * off.y;
	_mVar8 = 0;
}

void Monster::M_StartWalk3(V2Di vel, V2Di off, V2Di add, V2Di map, Dir EndDir)
{
	V2Di f = add + _m;
	V2Di n = map + _m;
	if (_uniqtype != 0) ChangeLightXY(mlid, n);
	grid.at(_m).setActor(*this);
	grid.at(f).setActor(*this);
	_mVar4 = n.x;
	_mVar5 = n.y;
	grid[n.x][n.y].dFlags |= DunTileFlag::MONSTLR;
	_mold = _m;
	_mfut = f;
	_moff = off;
	_mmode = MonsterMode::WALK3;
	_mvel = vel;
	_mVar1 = f.x;
	_mVar2 = f.y;
	_mVar3 = int(EndDir);
	_mdir = EndDir;
	NewMonsterAnim(&MType->getAnim(MonstAnim::WALK), EndDir);
	_mVar6 = 16 * off.x;
	_mVar7 = 16 * off.y;
	_mVar8 = 0;
}

void Monster::M_StartAttack()
{
	Dir md = M_GetDir();
	NewMonsterAnim(&MType->getAnim(MonstAnim::ATTACK), md);
	_mmode = MonsterMode::ATTACK;
	PlantInPosition(md);
}

void Monster::M_StartRAttack(int missile_type, int dam)
{
	Dir md = M_GetDir();
	NewMonsterAnim(&MType->getAnim(MonstAnim::ATTACK), md);
	_mmode = MonsterMode::RATTACK;
	_mVar1 = missile_type;
	_mVar2 = dam;
	PlantInPosition(md);
}

void Monster::M_StartRSpAttack(int missile_type, int dam)
{
	Dir md = M_GetDir();
	NewMonsterAnim(&MType->getAnim(MonstAnim::SPECIAL), md);
	_mmode = MonsterMode::RSPATTACK;
	_mVar1 = missile_type;
	_mVar2 = 0;
	_mVar3 = dam;
	PlantInPosition(md);
}

void Monster::M_StartSpAttack()
{
	Dir md = M_GetDir();
	NewMonsterAnim(&MType->getAnim(MonstAnim::SPECIAL), md);
	_mmode = MonsterMode::SATTACK;
	PlantInPosition(md);
}

void Monster::M_StartEat()
{
	NewMonsterAnim(&MType->getAnim(MonstAnim::SPECIAL), _mdir);
	_mmode = MonsterMode::SATTACK;
	PlantInPosition(_mdir);
}

void Monster::M_ClearSquares()
{
	int x, y, mx, my, m1, m2;
	mx = _mold.x;
	my = _mold.y;

	for (y = my - 1; y <= my + 1; y++) {
		if (y >= 0 && y < MAXDUNY) {
			for (x = mx - 1; x <= mx + 1; x++) {
				if (x >= 0 && x < MAXDUNX && (&grid[x][y].getActor() == this))
					grid[x][y].clearActor(*this);
			}
		}
	}

	if (mx + 1 < MAXDUNX) grid[mx + 1][my].dFlags &= ~DunTileFlag::MONSTLR;
	if (my + 1 < MAXDUNY) grid[mx][my + 1].dFlags &= ~DunTileFlag::MONSTLR;
}

void Monster::M_GetKnockback()
{
	Dir d = rotate(_mdir, -4);
	if (DirOK(d)) {
		M_ClearSquares();
		_mold += offset(d);
		NewMonsterAnim(&MType->getAnim(MonstAnim::GOTHIT), _mdir);
		_mmode = MonsterMode::GOTHIT;
		_moff = { 0, 0 };
		_m = _mold;
		_mfut = _m;
		// BUGFIX useless assignment
		_mold = _m;
		M_ClearSquares();
		grid.at(_m).setActor(*this);
	}
}

void Monster::M_StartHit(int pnum, int dam)
{
	if (pnum >= 0)
		mWhoHit |= 1 << pnum;
	if (pnum == myplr()) {
		delta_monster_hp(i, _mhitpoints, lvl.currlevel);
		NetSendCmdParam2(false, CMD_MONSTDAMAGE, i, dam);
	}
	PlayEffect(i, 1);
	if (MType->mtype >= MonsterType::SNEAK && MType->mtype <= MonsterType::ILLWEAV || dam >> 6 >= mLevel + 3) {
		if (pnum >= 0) {
			_mFlags &= ~MonsterFlag::targets_monster;
			_menemy = pnum;
			_menemypos = plr[pnum].futpos();
			_mdir = M_GetDir();
		}
		if (MType->mtype == MonsterType::BLINK) {
			M_Teleport();
		} else if (MType->mtype >= MonsterType::NSCAV && MType->mtype <= MonsterType::YSCAV) {
			_mgoal = MonsterGoal::NORMAL;
		}
		if (_mmode != MonsterMode::STONE) {
			NewMonsterAnim(&MType->getAnim(MonstAnim::GOTHIT), _mdir);
			_mmode = MonsterMode::GOTHIT;
			PlantInPosition(_mdir, &_mold);
			M_ClearSquares();
			grid.at(_m).setActor(*this);
		}
	}
}

void Monster::M_DiabloDeath(bool sendmsg)
{
	V2Di _mold;
	PlaySFX(USFX_DIABLOD);
	quests[Q_DIABLO]._qactive = QUEST_DONE;
	if (sendmsg) NetSendCmdQuest(true, Q_DIABLO);
	gbProcessPlayers = false;
	sgbSaveSoundOn = gbSoundOn;

	// Kills all monsters?
	Monster *k = 0;
	for (auto &i : monsters._list) {
		k = i.get();
		if (i.get() == this || _msquelch == 0) continue;
		i->NewMonsterAnim(&i->MType->getAnim(MonstAnim::DEATH),
		                  i->_mdir);
		i->_mVar1 = 0;
		i->_mmode = MonsterMode::DEATH;
		_mold = i->_mold;
		i->PlantInPosition(i->_mdir, &_mold);
		i->M_ClearSquares();
		grid.at(i->_m).setActor(*i.get());
	}

	// For some reason only does this for last monster
	AddLight(k->_m, 8);
	DoVision(k->_m, 8, false, true);
	int dist = (View - k->_m).maxabs();
	if (dist > 20) dist = 20;

	// This is some funky shit, suspect it was a mistake

	//j = View.x << 16;
	//k = View.y << 16;
	//monsters[k]._mVar3 = j;
	//monsters[k]._mVar4 = k;
	//monsters[k]._mVar5 = (int)((j - (monsters[k]._m.x << 16)) / (double)dist);
	//monsters[k]._mVar6 = (int)((k - (monsters[k]._m.y << 16)) / (double)dist);
}

void Monster::MonstStartKill(int pnum, bool sendmsg)
{
	Dir md;

	//MonsterStruct *Monst;
	if (pnum >= 0) mWhoHit |= 1 << pnum;
	if (pnum < MAX_PLRS && i > MAX_PLRS) plr.AddPlrMonstExper(mLevel, mExp, mWhoHit);


	monstkills[MType->mtype]++;
	_mhitpoints = 0;
	SetRndSeed(_mRndSeed);
	if (QuestStatus(Q_GARBUD) && mName == UniqMonst[UniqueMonsterType::GARBUD].mName) {
		CreateTypeItem({ _m.x + 1, _m.y + 1 }, true, ItemType::mace, MiscItemId::NONE, true, false);
	} else if (i > MAX_PLRS - 1) { // Golems should not spawn items
		SpawnItem(i, _m, sendmsg);
	}
	if (MType->mtype == MonsterType::DIABLO)
		M_DiabloDeath(true);
	else
		PlayEffect(i, 2);

	if (pnum >= 0)
		md = M_GetDir();
	else
		md = _mdir;
	_mdir = md;
	NewMonsterAnim(&MType->Anims[MonstAnim::DEATH], md);
	_mmode = MonsterMode::DEATH;
	PlantInPosition(_mdir, &_mold);
	_mVar1 = 0;

	M_ClearSquares();
	grid.at(_m).setActor(i);
	CheckQuestKill(i, sendmsg);
	M_FallenFear(_m);
	if (MType->mtype >= MonsterType::NACID && MType->mtype <= MonsterType::XACID)
		AddMissile(_m, { 0, 0 }, Dir(0), MissileType::ACIDPUD, 1, i, _mint + 1, 0);
}

void Monster::M2MStartKill(int mid)
{
	Dir md;

	if ((DWORD)mid >= MAXMONSTERS) {
		app_fatal("M2MStartKill: Invalid monster (killed) %d", mid);
	}
	if (!MType)
		app_fatal("M2MStartKill: Monster %d \"%s\" MType NULL", mid, monsters[mid].mName);

	delta_kill_monster(mid, monsters[mid]._m, lvl.currlevel);
	NetSendCmdLocParam1(false, CMD_MONSTDEATH, monsters[mid]._m, mid);

	monsters[mid].mWhoHit |= 1 << i;
	if (i < MAX_PLRS)
		AddPlrMonstExper(monsters[mid].mLevel, monsters[mid].mExp, monsters[mid].mWhoHit);

	monstkills[monsters[mid].MType->mtype]++;
	monsters[mid]._mhitpoints = 0;
	SetRndSeed(monsters[mid]._mRndSeed);

	if (mid >= MAX_PLRS)
		SpawnItem(mid, monsters[mid]._m, true);

	if (monsters[mid].MType->mtype == MonsterType::DIABLO)
		monsters[mid].M_DiabloDeath(true);
	else
		PlayEffect(i, 2);

	PlayEffect(mid, 2);

	md = rotate(_mdir, -4);
	if (monsters[mid].MType->mtype == MonsterFlag::golem) md = Dir(0);

	monsters[mid]._mdir = md;
	monsters[mid].NewMonsterAnim(&monsters[mid].MType->Anims[MonstAnim::DEATH], md);
	monsters[mid]._mmode = MonsterMode::DEATH;
	monsters[mid].PlantInPosition(md, &monsters[mid]._mold);
	monsters[mid].M_ClearSquares();
	grid.at(monsters[mid]._m).setActor(mid);
	CheckQuestKill(mid, true);
	M_FallenFear(monsters[mid]._m);
	if (monsters[mid].MType->mtype >= MonsterType::NACID && monsters[mid].MType->mtype <= MonsterType::XACID)
		AddMissile(monsters[mid]._m, { 0, 0 }, Dir(0), MissileType::ACIDPUD, 1, mid, monsters[mid]._mint + 1, 0);
}

void Monster::M_StartKill(int pnum)
{
	if (myplr() == pnum) {
		delta_kill_monster(i, _m, lvl.currlevel);
		if (i != pnum) {
			NetSendCmdLocParam1(false, CMD_MONSTDEATH, _m, i);
		} else {
			NetSendCmdLocParam1(false, CMD_KILLGOLEM, _m, lvl.currlevel);
		}
	}
	MonstStartKill(pnum, true);
}

void Monster::M_SyncStartKill(V2Di pos, int pnum)
{
	if (_mhitpoints == 0 || _mmode == MonsterMode::DEATH) {
		return;
	}

	if (grid[pos.x][pos.y].getActor() == 0) {
		M_ClearSquares();
		_m = pos;
		_mold = pos;
	}

	if (_mmode == MonsterMode::STONE) {
		MonstStartKill(pnum, false);
		_mmode = MonsterMode::STONE;
	} else {
		MonstStartKill(pnum, false);
	}
}

void Monster::M_StartFadein(Dir md, bool backwards)
{
	NewMonsterAnim(&MType->Anims[MonstAnim::SPECIAL], md);
	_mmode = MonsterMode::FADEIN;
	PlantInPosition(md);
	_mFlags &= ~MonsterFlag::hidden;
	if (backwards) {
		_mFlags |= MonsterFlag::lock_animation;
		_mAnimFrame = _mAnimLen;
	}
}

void Monster::M_StartFadeout(Dir md, bool backwards)
{
	NewMonsterAnim(&MType->Anims[MonstAnim::SPECIAL], md);
	_mmode = MonsterMode::FADEOUT;
	PlantInPosition(md);
	if (backwards) {
		_mFlags |= MonsterFlag::lock_animation;
		_mAnimFrame = _mAnimLen;
	}
}

void Monster::M_StartHeal()
{
	_mAnimData = MType->Anims[MonstAnim::SPECIAL].Data[int(_mdir)];
	_mAnimFrame = MType->Anims[MonstAnim::SPECIAL].Frames;
	_mFlags |= MonsterFlag::lock_animation;
	_mmode = MonsterMode::HEAL;
	_mVar1 = _mmaxhp / (16 * (random_(97, 5) + 4));
}

void Monster::M_ChangeLightOffset()
{
	int lx, ly, _mxoff, _myoff, sign;

	lx = _moff.x + 2 * _moff.y;
	ly = 2 * _moff.y - _moff.x;

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
	ChangeLightOff(monsters[i].mlid, { _mxoff, _myoff });
}

bool Monster::M_DoStand()
{
	if (MType->mtype == MonsterFlag::golem)
		_mAnimData = MType->Anims[MonstAnim::WALK].Data[int(_mdir)];
	else
		_mAnimData = MType->Anims[MonstAnim::STAND].Data[int(_mdir)];

	if (_mAnimFrame == _mAnimLen)
		M_Enemy();

	_mVar2++;
	return false;
}

bool Monster::M_DoWalk()
{
	bool rv = false;
	if (_mVar8 == MType->Anims[MonstAnim::WALK].Frames) {
		grid.at(_m).clearMonster();
		_m.x += _mVar1;
		_m.y += _mVar2;
		grid.at(_m).setActor(i);
		if (_uniqtype != 0) ChangeLightXY(mlid, _m);
		M_StartStand(_mdir);
		rv = true;
	} else if (!_mAnimCnt) {
		_mVar8++;
		_mVar6 += _mvel.x;
		_mVar7 += _mvel.y;
		_moff.x = _mVar6 >> 4;
		_moff.y = _mVar7 >> 4;
	}

	if (_uniqtype != 0) M_ChangeLightOffset();
	return rv;
}

bool Monster::M_DoWalk2()
{
	bool rv;
	if (_mVar8 == MType->Anims[MonstAnim::WALK].Frames) {
		grid[_mVar1][_mVar2].clearMonster();
		if (_uniqtype != 0) ChangeLightXY(mlid, _m);
		M_StartStand(_mdir);
		rv = true;
	} else {
		if (!_mAnimCnt) {
			_mVar8++;
			_mVar6 += _mvel.x;
			_mVar7 += _mvel.y;
			_moff.x = _mVar6 >> 4;
			_moff.y = _mVar7 >> 4;
		}
		rv = false;
	}
	if (_uniqtype != 0) M_ChangeLightOffset();
	return rv;
}

bool Monster::M_DoWalk3()
{
	bool rv;
	if (_mVar8 == MType->Anims[MonstAnim::WALK].Frames) {
		grid.at(_m).clearMonster();
		_m = { _mVar1, _mVar2 };
		grid[_mVar4][_mVar5].dFlags &= ~DunTileFlag::MONSTLR;
		grid.at(_m).setActor(i);
		if (_uniqtype) ChangeLightXY(mlid, _m);
		M_StartStand(_mdir);
		rv = true;
	} else {
		if (!_mAnimCnt) {
			_mVar8++;
			_mVar6 += _mvel.x;
			_mVar7 += _mvel.y;
			_moff.x = _mVar6 >> 4;
			_moff.y = _mVar7 >> 4;
		}
		rv = false;
	}
	if (_uniqtype != 0) M_ChangeLightOffset();
	return rv;
}

void Monster::M_TryM2MHit(int mid, int hper, int mind, int maxd)
{
	bool ret;

	if ((DWORD)mid >= MAXMONSTERS) {
		app_fatal("M_TryM2MHit: Invalid monster %d", mid);
	}
	if (monsters[mid].MType == NULL)
		app_fatal("M_TryM2MHit: Monster %d \"%s\" MType NULL", mid, monsters[mid].mName);

	if (monsters[mid]._mhitpoints >> 6 > 0 && (monsters[mid].MType->mtype != MonsterType::ILLWEAV || monsters[mid]._mgoal != MonsterGoal::RETREAT)) {
		int hit = random_(4, 100);
		if (monsters[mid]._mmode == MonsterMode::STONE)
			hit = 0;
		if (!CheckMonsterHit(mid, &ret) && hit < hper) {
			int dam = (mind + random_(5, maxd - mind + 1)) << 6;
			monsters[mid]._mhitpoints -= dam;
			if (monsters[mid]._mhitpoints >> 6 <= 0) {
				if (monsters[mid]._mmode == MonsterMode::STONE) {
					M2MStartKill(mid);
					monsters[mid]._mmode = MonsterMode::STONE;
				} else {
					M2MStartKill(mid);
				}
			} else {
				if (monsters[mid]._mmode == MonsterMode::STONE) {
					M2MStartHit(mid, i, dam);
					monsters[mid]._mmode = MonsterMode::STONE;
				} else {
					M2MStartHit(mid, i, dam);
				}
			}
		}
	}
}

void Monster::M_TryH2HHit(ActorId pnum, int Hit, int MinDam, int MaxDam)
{
	if (!actors.isValidPlayer(pnum)) return;
	Player& player = *actors.getPlayer(pnum);

	V2Di newpos;
	int j, misnum, ms_num, cur_ms_num, new_hp;

	if (_mFlags & MonsterFlag::targets_monster) {
		M_TryM2MHit(pnum, Hit, MinDam, MaxDam);
		return;
	}
	if (player._hp <= 0 || player._pInvincible || player._pSpellFlags & 1)
		return;

	int dist = (_m - player.pos()).maxabs();
	if (dist >= 2) return;

	int hper = random_(98, 100);
	#ifdef _DEBUG
	if (debug_mode_dollar_sign || debug_mode_key_inverted_v)
		hper = 1000;
	#endif
	int hit = Hit
	    + 2 * (mLevel - player._pLevel)
	    + 30
	    - player._pIBonusAC
	    - player._pIAC
	    - player._dexterity / 5;
	if (hit < 15) hit = 15;
	if (lvl.currlevel == 14 && hit < 20) hit = 20;
	if (lvl.currlevel == 15 && hit < 25) hit = 25;
	if (lvl.currlevel == 16 && hit < 30) hit = 30;

	int blk, blkper;
	if ((player._pmode == PlayerMode::STAND || player._pmode == PlayerMode::ATTACK) && player._pBlockFlag) {
		blkper = random_(98, 100);
	} else {
		blkper = 100;
	}
	blk = player._dexterity
	    + player._pBaseToBlk
	    - (mLevel << 1)
	    + (player._pLevel << 1);
	if (blk < 0) blk = 0;
	if (blk > 100) blk = 100;
	if (hper >= hit) return;
	if (blkper < blk) {
		player.StartPlrBlock(GetDirection(player.pos(), _m));
		return;
	}
	if (MType->mtype == MonsterType::YZOMBIE && player.isMyPlr()) {
		ms_num = -1;
		cur_ms_num = -1;
		for (j = 0; j < nummissiles; j++) {
			misnum = missileactive[j];
			if (missile[misnum]._mitype != MissileType::MANASHIELD)
				continue;
			if (missile[misnum]._misource == pnum)
				cur_ms_num = misnum;
			else
				ms_num = misnum;
		}
		//if (player._pMaxHP > 64) {
		//	new_hp = player._pMaxHP - 64;
		//	player._pMaxHP = new_hp;
		//	if (player._pHitPoints > new_hp) {
		//		player._pHitPoints = new_hp;
		//		if (cur_ms_num >= 0)
		//			missile[cur_ms_num]._miVar1 = new_hp;
		//	}
		//	new_hp = player._pMaxHPBase - 64;
		//	player._pMaxHPBase = new_hp;
		//	if (player._pHPBase > new_hp) {
		//		player._pHPBase = new_hp;
		//		if (cur_ms_num >= 0)
		//			missile[cur_ms_num]._miVar2 = new_hp;
		//	}
		//}
	}

	int mdam;
	int dam = (MinDam ) + random_(99, (MaxDam - MinDam + 1));
	dam += (player._pIGetHit);
	if (player.isMyPlr()) {
		player._pHitPoints -= dam;
		player._pHPBase -= dam;
	}
	if (player._pIFlags & ItemSpecialEffect::THORNS) { // Player has thorns
		mdam = (random_(99, 3) + 1);
		_mhitpoints -= mdam;
		if (_mhitpoints <= 0)
			M_StartKill(pnum);
		else
			M_StartHit(pnum, mdam);
	}
	if (!(_mFlags & MonsterFlag::nolifesteal) && MType->mtype == MonsterType::SKING && plr.isMultiplayer())
		_mhitpoints += dam;

	player._hp.check();
	if (player._hp <= 0) {
		player.SyncPlrKill();
		return;
	}
	player.StartPlrHit(dam, false);
	if (_mFlags & MonsterFlag::knockback) {
		if (player._pmode != PlayerMode::GOTHIT)
			player.StartPlrHit(0, true);
		newpos = player.pos() + offset(_mdir);
		if (PosOkPlayer(pnum, newpos)) {
			player.changePos(newpos);
			player.PlantInPlace(player._pdir);
			player.FixPlrWalkTags();
			player.SetPlayerOld();
		}
	}
}

bool Monster::M_DoAttack()
{
	if (MType == NULL)
		app_fatal("M_DoAttack: Monster %d \"%s\" MType NULL", i, mName);
	if (MType == NULL) // BUGFIX: should check MData
		app_fatal("M_DoAttack: Monster %d \"%s\" MData NULL", i, mName);

	if (_mAnimFrame == MData->mAFNum) {
		M_TryH2HHit(_menemy, mHit, mMinDamage, mMaxDamage);
		if (_mAi != MonstAi::SNAKE)
			PlayEffect(i, 0);
	}
	if (MType->mtype >= MonsterType::NMAGMA && MType->mtype <= MonsterType::WMAGMA && _mAnimFrame == 9) {
		M_TryH2HHit(_menemy, mHit + 10, mMinDamage - 2, mMaxDamage - 2);
		PlayEffect(i, 0);
	}
	if (MType->mtype >= MonsterType::STORM && MType->mtype <= MonsterType::MAEL && _mAnimFrame == 13) {
		M_TryH2HHit(_menemy, mHit - 20, mMinDamage + 4, mMaxDamage + 4);
		PlayEffect(i, 0);
	}
	if (_mAi == MonstAi::SNAKE && _mAnimFrame == 1)
		PlayEffect(i, 0);
	if (_mAnimFrame == _mAnimLen) {
		M_StartStand(_mdir);
		return true;
	}

	return false;
}

bool Monster::M_DoRAttack()
{
	int multimissiles, mi;
	if (_mAnimFrame == MData->mAFNum) {
		if (_mVar1 != -1) {
			if (_mVar1 != MissileType::CBOLT)
				multimissiles = 1;
			else
				multimissiles = 3;
			for (mi = 0; mi < multimissiles; mi++) {
				AddMissile(_m, _menemypos, _mdir, _mVar1,
				    1, i, _mVar2, 0);
			}
		}
		PlayEffect(i, 0);
	}

	if (_mAnimFrame == _mAnimLen) {
		M_StartStand(_mdir);
		return true;
	}
	return false;
}

int Monster::M_DoRSpAttack()
{
	if (_mAnimFrame == MData->mAFNum2 && !_mAnimCnt) {
		AddMissile(_m, _menemypos, _mdir, _mVar1,
		    1, i, _mVar3, 0);
		PlayEffect(i, 3);
	}

	if (_mAi == MonstAi::MEGA && _mAnimFrame == 3) {
		int hadV2 = _mVar2;
		_mVar2++;
		if (hadV2 == 0) {
			_mFlags |= MonsterFlag::allow_special;
		} else if (_mVar2 == 15) {
			_mFlags &= ~MonsterFlag::allow_special;
		}
	}

	if (_mAnimFrame == _mAnimLen) {
		M_StartStand(_mdir);
		return true;
	}
	return false;
}

bool Monster::M_DoSAttack()
{
	if (_mAnimFrame == MData->mAFNum2)
		M_TryH2HHit(_menemy, mHit2, mMinDamage2, mMaxDamage2);

	if (_mAnimFrame == _mAnimLen) {
		M_StartStand(_mdir);
		return true;
	}
	return false;
}

bool Monster::M_DoFadein()
{
	if ((!(_mFlags & MonsterFlag::lock_animation) || _mAnimFrame != 1)
	    && (_mFlags & MonsterFlag::lock_animation || _mAnimFrame != _mAnimLen)) {
		return false;
	}

	M_StartStand(_mdir);
	_mFlags &= ~MonsterFlag::lock_animation;

	return true;
}

bool Monster::M_DoFadeout()
{
	if ((!(_mFlags & MonsterFlag::lock_animation) || _mAnimFrame != 1)
	    && (_mFlags & MonsterFlag::lock_animation || _mAnimFrame != _mAnimLen)) {
		return false;
	}

	int mt = MType->mtype;
	if (mt < MonsterType::INCIN || mt > MonsterType::HELLBURN) {
		_mFlags &= ~MonsterFlag::lock_animation;
		_mFlags |= MonsterFlag::hidden;
	} else {
		_mFlags &= ~MonsterFlag::lock_animation;
	}

	M_StartStand(_mdir);

	return true;
}

int Monster::M_DoHeal()
{
	if (_mFlags & MonsterFlag::noheal) {
		_mFlags &= ~MonsterFlag::allow_special;
		_mmode = MonsterMode::SATTACK;
		return false;
	}

	if (_mAnimFrame == 1) {
		_mFlags &= ~MonsterFlag::lock_animation;
		_mFlags |= MonsterFlag::allow_special;
		if (_mVar1 + _mhitpoints < _mmaxhp) {
			_mhitpoints = _mVar1 + _mhitpoints;
		} else {
			_mhitpoints = _mmaxhp;
			_mFlags &= ~MonsterFlag::allow_special;
			_mmode = MonsterMode::SATTACK;
		}
	}
	return false;
}

int Monster::M_DoTalk()
{
	int tren;
	M_StartStand(_mdir);
	_mgoal = MonsterGoal::TALKING;
	if (effect_is_playing(alltext[mtalkmsg].sfxnr))
		return false;
	InitQTextMsg(mtalkmsg);
	if (mName == UniqMonst[UniqueMonsterType::GARBUD].mName) {
		if (mtalkmsg == TEXT_GARBUD1)
			quests[Q_GARBUD]._qactive = QUEST_ACTIVE;
		quests[Q_GARBUD]._qlog = true;
		if (mtalkmsg == TEXT_GARBUD2 && !(_mFlags & MonsterFlag::quest_complete)) {
			SpawnItem(i, { _m.x + 1, _m.y + 1 }, true);
			_mFlags |= MonsterFlag::quest_complete;
		}
	}
	if (mName == UniqMonst[UniqueMonsterType::ZHAR].mName
	    && mtalkmsg == TEXT_ZHAR1
	    && !(_mFlags & MonsterFlag::quest_complete)) {
		quests[Q_ZHAR]._qactive = QUEST_ACTIVE;
		quests[Q_ZHAR]._qlog = true;
		CreateTypeItem({ _m.x + 1, _m.y + 1 }, false, ItemType::misc, MiscItemId::BOOK, true, false);
		_mFlags |= MonsterFlag::quest_complete;
	}
	if (mName == UniqMonst[UniqueMonsterType::SNOTSPIL].mName) {
		if (mtalkmsg == TEXT_BANNER10 && !(_mFlags & MonsterFlag::quest_complete)) {
			ObjChangeMap({ lvl.getpc().x, lvl.getpc().y }, { (lvl.getpc().w >> 1) + lvl.getpc().x + 2, (lvl.getpc().h >> 1) + lvl.getpc().y - 2 });
			tren = lvl.TransVal;
			lvl.TransVal = 9;
			DRLG_MRectTrans(lvl.getpc().x, lvl.getpc().y, (lvl.getpc().w >> 1) + lvl.getpc().x + 4, lvl.getpc().y + (lvl.getpc().h >> 1));
			lvl.TransVal = tren;
			quests[Q_LTBANNER]._qvar1 = 2;
			if (quests[Q_LTBANNER]._qactive == QUEST_INIT)
				quests[Q_LTBANNER]._qactive = QUEST_ACTIVE;
			_mFlags |= MonsterFlag::quest_complete;
		}
		if (quests[Q_LTBANNER]._qvar1 < 2) {
			sprintf(tempstr, "SS Talk = %i, Flags = %i", mtalkmsg, _mFlags);
			app_fatal(tempstr);
		}
	}
	if (mName == UniqMonst[UniqueMonsterType::LACHDAN].mName) {
		if (mtalkmsg == TEXT_VEIL9) {
			quests[Q_VEIL]._qactive = QUEST_ACTIVE;
			quests[Q_VEIL]._qlog = true;
		}
		if (mtalkmsg == TEXT_VEIL11 && !(_mFlags & MonsterFlag::quest_complete)) {
			SpawnUnique(UniqueItemId::STEELVEIL, { _m.x + 1, _m.y + 1 });
			_mFlags |= MonsterFlag::quest_complete;
		}
	}
	if (mName == UniqMonst[UniqueMonsterType::WARLORD].mName)
		quests[Q_WARLORD]._qvar1 = 2;
	if (mName == UniqMonst[UniqueMonsterType::LAZURUS].mName && plr.isMultiplayer()) {
		_msquelch = UINT8_MAX;
		mtalkmsg = 0;
		quests[Q_BETRAYER]._qvar1 = 6;
		_mgoal = MonsterGoal::NORMAL;
	}
	return false;
}

void Monster::M_Teleport()
{
	bool tren = false;
	int k, j, x, y, _mx, _my, rx, ry;

	if (_mmode != MonsterMode::STONE) {
		_mx = _menemypos.x;
		_my = _menemypos.y;
		rx = 2 * random_(100, 2) - 1;
		ry = 2 * random_(100, 2) - 1;

		for (j = -1; j <= 1 && !tren; j++) {
			for (k = -1; k < 1 && !tren; k++) {
				if (j != 0 || k != 0) {
					x = _mx + rx * j;
					y = _my + ry * k;
					if (y >= 0 && y < MAXDUNY && x >= 0 && x < MAXDUNX && x != _m.x && y != _m.y) {
						if (PosOkMonst(i, { x, y }))
							tren = true;
					}
				}
			}
		}
	}

	if (tren) {
		M_ClearSquares();
		grid.at(_m).clearMonster();
		grid[x][y].setActor(i);
		_mold = { x, y };
		_mdir = M_GetDir();
	}
}

bool Monster::M_DoGotHit()
{
	if (_mAnimFrame == _mAnimLen) {
		M_StartStand(_mdir);
		return true;
	}
	return false;
}

bool Monster::M_DoDeath()
{
	int var1;
	int x, y;
	_mVar1++;
	var1 = _mVar1;
	if (MType->mtype == MonsterType::DIABLO) {
		x = _m.x - View.x;
		if (x < 0)
			x = -1;
		else
			x = x > 0;
		View.x += x;

		y = _m.y - View.y;
		if (y < 0) {
			y = -1;
		} else {
			y = y > 0;
		}
		View.y += y;

		if (var1 == 140)
			PrepDoEnding();
	} else if (_mAnimFrame == _mAnimLen) {
		if (_uniqtype == 0) AddDead(_m, MType->mdeadval, _mdir);
		else AddDead(_m, _udeadval, _mdir);

		grid.at(_m).clearMonster();
		_mDelFlag = true;

		M_UpdateLeader();
	}
	return false;
}

bool Monster::M_DoSpStand()
{
	if (_mAnimFrame == MData->mAFNum2)
		PlayEffect(i, 3);

	if (_mAnimFrame == _mAnimLen) {
		M_StartStand(_mdir);
		return true;
	}
	return false;
}

bool Monster::M_DoDelay()
{
	int mVar2;
	int oFrame;
	_mAnimData = MType->Anims[MonstAnim::STAND].Data[int(M_GetDir())];
	if (_mAi == MonstAi::LAZURUS) {
		if (_mVar2 > 8 || _mVar2 < 0)
			_mVar2 = 8;
	}

	mVar2 = _mVar2;
	_mVar2--;

	if (!mVar2) {
		oFrame = _mAnimFrame;
		M_StartStand(_mdir);
		_mAnimFrame = oFrame;
		return true;
	}
	return false;
}

bool Monster::M_DoStone()
{
	if (!_mhitpoints) {
		grid.at(_m).clearMonster();
		_mDelFlag = true;
	}
	return false;
}

void Monster::M_WalkDir(Dir md)
{
	int mwi = MType->Anims[MonstAnim::WALK].Frames - 1;
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

void Monster::GroupUnity()
{
	int leader, m, j;
	bool clear;
	if (leaderflag) {
		leader = leader;
		clear = LineClearF(CheckNoSolid, _m, monsters[leader]._mfut);
		if (clear || leaderflag != 1) {
			int dist = (_m - monsters[leader]._mfut).maxabs();
			if (clear
			    && leaderflag == 2
			    && dist < 4) {
				monsters[leader].packsize++;
				leaderflag = 1;
			}
		} else {
			monsters[leader].packsize--;
			leaderflag = 2;
		}
	}

	if (leaderflag == 1) {
		if (_msquelch > monsters[leader]._msquelch) {
			monsters[leader]._last = _m;
			monsters[leader]._msquelch = _msquelch - 1;
		}
		if (monsters[leader]._mAi == MonstAi::GARG) {
			if (monsters[leader]._mFlags & MonsterFlag::allow_special) {
				monsters[leader]._mmode = MonsterMode::SATTACK;
				monsters[leader]._mFlags &= ~MonsterFlag::allow_special;
			}
		}
		return;
	}
	if (_uniqtype != 0) {
		if (UniqMonst[_uniqtype - 1].mUnqAttr & 2) {
			for (j = 0; j < nummonsters; j++) {
				m = monstactive[j];
				if (monsters[m].leaderflag == 1 && monsters[m].leader == i) {
					if (_msquelch > monsters[m]._msquelch) {
						monsters[m]._last = _m;
						monsters[m]._msquelch = _msquelch - 1;
					}
					if (monsters[m]._mAi == MonstAi::GARG) {
						if (monsters[m]._mFlags & MonsterFlag::allow_special) {
							monsters[m]._mmode = MonsterMode::SATTACK;
							monsters[m]._mFlags &= ~MonsterFlag::allow_special;
						}
					}
				}
			}
		}
	}
}

bool Monster::M_CallWalk(Dir md)
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

bool Monster::M_PathWalk()
{
	//char path[25];
	std::queue<PathNode> path;

	bool (*Check)
	(int, V2Di) = PosOkMonst3;
	if (!(_mFlags & MonsterFlag::can_open_door))
		Check = PosOkMonst;

	int dist = pathfinder.FindPath(Check, i, _m, _menemypos, path);
	if (dist > 1) {
		M_CallWalk(GetDirection(_m, path.front().pos())); /* plr2monst is local */
		return true;
	}

	return false;
}

bool Monster::M_CallWalk2(Dir md)
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

bool Monster::M_DumbWalk(Dir md)
{
	bool ok = DirOK(md);
	if (ok) M_WalkDir(md);
	return ok;
}

bool Monster::M_RoundWalk(Dir md, int *dir)
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

void Monster::MAI::Zombie()
{
	if (_mmode != MonsterMode::STAND)
		return;

	V2Di m = _m;
	if (!(grid.at(m).dFlags & DunTileFlag::VISIBLE)) {
		return;
	}
	int dist = (m - _menemypos).maxabs();
	Dir md = _mdir;
	int v = random_(103, 100);
	if (dist >= 2) {
		if (v < 2 * _mint + 10) {
			if (dist >= 2 * _mint + 4) {
				if (random_(104, 100) < 2 * _mint + 20) {
					md = Dir(random_(104, 8));
				}
				M_DumbWalk(md);
			} else {
				md = M_GetDir();
				M_CallWalk(md);
			}
		}
	} else if (v < 2 * _mint + 10) {
		M_StartAttack();
	}

	if (_mmode == MonsterMode::STAND)
		_mAnimData = MType->Anims[MonstAnim::STAND].Data[int(_mdir)];
}

void Monster::MAI::SkelSd()
{
	if (_mmode != MonsterMode::STAND || _msquelch == 0)
		return;

	V2Di m = _m;
	int dist = (m - _menemypos).maxabs();
	Dir md = GetDirection(m, _last);
	_mdir = md;
	if (dist >= 2) {
		if (_mVar1 == MonsterMode::DELAY || (random_(106, 100) >= 35 - 4 * _mint)) {
			M_CallWalk(md);
		} else {
			M_StartDelay(15 - 2 * _mint + random_(106, 10));
		}
	} else {
		if (_mVar1 == MonsterMode::DELAY || (random_(105, 100) < 2 * _mint + 20)) {
			M_StartAttack();
		} else {
			M_StartDelay(2 * (5 - _mint) + random_(105, 10));
		}
	}

	if (_mmode == MonsterMode::STAND)
		_mAnimData = MType->Anims[MonstAnim::STAND].Data[int(md)];
}

bool Monster::MAI::Path()
{
	if (MType->mtype != MonsterFlag::golem) {
		if (_msquelch == 0)
			return false;
		if (_mmode != MonsterMode::STAND)
			return false;
		if (_mgoal != MonsterGoal::NORMAL && _mgoal != MonsterGoal::MOVE && _mgoal != MonsterGoal::SHOOT)
			return false;
		if (_m.x == 1 && _m.y == 0)
			return false;
	}

	bool clear = LineClearF1(PosOkMonst2, i, _m, _menemypos);
	if (!clear || _pathcount >= 5 && _pathcount < 8) {
		if (_mFlags & MonsterFlag::can_open_door) MonstCheckDoors(i);
		_pathcount++;
		if (_pathcount < 5) return false;
		if (M_PathWalk()) return true;
	}

	if (MType->mtype != MonsterFlag::golem)
		_pathcount = 0;

	return false;
}

void Monster::MAI::Snake()
{
	if (_mmode != MonsterMode::STAND || _msquelch == 0) return;

	Dir tmp;
	char pattern[6] = { 1, 1, 0, -1, -1, 0 };
	int pnum = _menemy;

	V2Di f = _menemypos;
	int dist = (_m - f).maxabs();
	Dir md = GetDirection(_m, _last);
	_mdir = md;

	if (dist >= 2) {
		if (dist < 3 && LineClearF1(PosOkMonst, i, _m, f) && _mVar1 != MonsterMode::CHARGE) {
			if (AddMissile(_m, f, md, MissileType::RHINO, pnum, i, 0, 0) != -1) {
				PlayEffect(i, 0);
				grid.at(_m).setActor(i);
				_mmode = MonsterMode::CHARGE;
			}
		// This is all just code to choose a direction
		} else if (_mVar1 == MonsterMode::DELAY || random_(106, 100) >= 35 - 2 * _mint) {
			tmp = rotate(md, pattern[_mgoalvar1]);
			_mgoalvar1++;
			if (_mgoalvar1 > 5) _mgoalvar1 = 0;
			tmp = rotate(md, -_mgoalvar2);

			if (int(md) > 0) {
				if (int(md) < 4) {
					md = right(Dir(_mgoalvar2));
					_mgoalvar2 = int(md);
				} else if (md == Dir(4)) {
					_mgoalvar2 = int(tmp);
				} else {
					md = left(Dir(_mgoalvar2));
					_mgoalvar2 = int(md);
				}
			}
			if (!M_DumbWalk(Dir(_mgoalvar2))) M_CallWalk2(_mdir);
		} else {
			M_StartDelay(15 - _mint + random_(106, 10));
		}
	} else {
		if (_mVar1 == MonsterMode::DELAY
		    || _mVar1 == MonsterMode::CHARGE
		    || (random_(105, 100) < _mint + 20)) {
			M_StartAttack();
		} else
			M_StartDelay(10 - _mint + random_(105, 10));
	}
	if (_mmode == MonsterMode::STAND)
		_mAnimData = MType->Anims[MonstAnim::STAND].Data[int(_mdir)];
}

void Monster::MAI::Bat()
{
	if (_mmode != MonsterMode::STAND || _msquelch == 0) return;

	int pnum = _menemy;
	V2Di f = _menemypos;
	int dist = (_m - f).maxabs();
	Dir md = GetDirection(_m, _last);
	_mdir = md;
	int v = random_(107, 100);

	if (_mgoal == MonsterGoal::RETREAT) {
		if (!_mgoalvar1) {
			M_CallWalk(opposite(md));
			_mgoalvar1++;
		} else {
			if (random_(108, 2)) M_CallWalk(left(md));
			else M_CallWalk(right(md));
			_mgoal = MonsterGoal::NORMAL;
		}
		return;
	}

	if (MType->mtype == MonsterType::GLOOM
	    && (dist >= 5)
	    && v < 4 * _mint + 33
	    && LineClearF1(PosOkMonst, i, _m, f)) {
		if (AddMissile(_m, f, md, MissileType::RHINO, pnum, i, 0, 0) != -1) {
			grid.at(_m).setActor(i);
			_mmode = MonsterMode::CHARGE;
		}
	} else if (dist >= 2) {
		if (_mVar2 > 20 && v < _mint + 13
		    || (_mVar1 == MonsterMode::WALK || _mVar1 == MonsterMode::WALK2 || _mVar1 == MonsterMode::WALK3)
		        && _mVar2 == 0
		        && v < _mint + 63) {
			M_CallWalk(md);
		}
	} else if (v < 4 * _mint + 8) {
		M_StartAttack();
		_mgoal = MonsterGoal::RETREAT;
		_mgoalvar1 = 0;
		if (MType->mtype == MonsterType::FAMILIAR) {
			AddMissile(_menemypos, { _menemypos.x + 1, 0 }, Dir(-1), MissileType::LIGHTNING, 1, i, random_(109, 10) + 1, 0);
		}
	}

	if (_mmode == MonsterMode::STAND)
		_mAnimData = MType->Anims[MonstAnim::STAND].Data[int(md)];
}

void Monster::MAI::SkelBow()
{
	if (_mmode != MonsterMode::STAND || _msquelch == 0) return;

	bool walking = false;
	V2Di f = _menemypos;
	int dist = (_m - f).maxabs();
	Dir md = GetDirection(_m, _last);
	_mdir = md;

	int v = random_(110, 100);

	if (dist < 4) {
		if (_mVar2 > 20 && v < 2 * _mint + 13
		    || (_mVar1 == MonsterMode::WALK || _mVar1 == MonsterMode::WALK2 || _mVar1 == MonsterMode::WALK3)
		        && _mVar2 == 0
		        && v < 2 * _mint + 63) {
			walking = M_DumbWalk(opposite(md));
		}
	}

	V2Di m = _menemypos;
	if (!walking) {
		if (random_(110, 100) < 2 * _mint + 3) {
			if (LineClear(_m, m))
				M_StartRAttack(MissileType::ARROW, 4);
		}
	}

	if (_mmode == MonsterMode::STAND) _mAnimData = MType->Anims[MonstAnim::STAND].Data[int(md)];
}

void Monster::MAI::Fat()
{
	if (_mmode != MonsterMode::STAND || _msquelch == 0)
		return;

	V2Di f = _menemypos;
	int dist = (_m - f).maxabs();
	Dir md = GetDirection(_m, _last);
	_mdir = md;
	int v = random_(111, 100);
	if (dist >= 2) {
		if (_mVar2 > 20 && v < 4 * _mint + 20
		    || (_mVar1 == MonsterMode::WALK || _mVar1 == MonsterMode::WALK2 || _mVar1 == MonsterMode::WALK3)
		        && _mVar2 == 0
		        && v < 4 * _mint + 70) {
			M_CallWalk(md);
		}
	} else if (v < 4 * _mint + 15) {
		M_StartAttack();
	} else if (v < 4 * _mint + 20) {
		M_StartSpAttack();
	}

	if (_mmode == MonsterMode::STAND)
		_mAnimData = MType->Anims[MonstAnim::STAND].Data[int(md)];
}

void Monster::MAI::Sneak()
{
	if (_mmode != MonsterMode::STAND)
		return;

	V2Di mb = _m;
	if (grid.at(mb).dLight != lightmax) {
		V2Di d = (mb - _menemypos).abs();
		Dir md = M_GetDir();
		int dist = 5 - _mint;
		if (_mVar1 == MonsterMode::GOTHIT) {
			_mgoalvar1 = 0;
			_mgoal = MonsterGoal::RETREAT;
		} else {
			if (d.maxdim() >= dist + 3 || _mgoalvar1 > 8) {
				_mgoalvar1 = 0;
				_mgoal = MonsterGoal::NORMAL;
			}
		}
		if (_mgoal == MonsterGoal::RETREAT) {
			if (_mFlags & MonsterFlag::targets_monster)
				md = GetDirection(_m, plr[_menemy]._powner);
			md = opposite(md);
			if (MType->mtype == MonsterType::UNSEEN) {
				if (random_(112, 2))
					md = left(md);
				else
					md = right(md);
			}
		}
		_mdir = md;
		int v = random_(112, 100);
		if (d.maxdim() < dist && _mFlags & MonsterFlag::hidden) {
			M_StartFadein(md, false);
		} else {
			if ((d.maxdim() >= dist + 1) && !(_mFlags & MonsterFlag::hidden)) {
				M_StartFadeout(md, true);
			} else {
				if (_mgoal == MonsterGoal::RETREAT
				    || (d.maxdim() >= 2) && (_mVar2 > 20 && v < 4 * _mint + 14 || (_mVar1 == MonsterMode::WALK || _mVar1 == MonsterMode::WALK2 || _mVar1 == MonsterMode::WALK3) && _mVar2 == 0 && v < 4 * _mint + 64)) {
					_mgoalvar1++;
					M_CallWalk(md);
				}
			}
		}
		if (_mmode == MonsterMode::STAND) {
			if (d.maxdim() >= 2 || v >= 4 * _mint + 10)
				_mAnimData = MType->Anims[MonstAnim::STAND].Data[int(md)];
			else
				M_StartAttack();
		}
	}
}

void Monster::MAI::Fireman()
{
	if (_mmode != MonsterMode::STAND || _msquelch == 0) return;

	int pnum = _menemy;
	V2Di f = _menemypos;
	int dist = (_m - f).maxabs();
	Dir md = M_GetDir();
	if (_mgoal == MonsterGoal::NORMAL) {
		if (LineClear(_m, f)
		    && AddMissile(_m, f, md, MissileType::FIREMAN, pnum, i, 0, 0) != -1) {
			_mmode = MonsterMode::CHARGE;
			_mgoal = MonsterGoal::SHOOT;
			_mgoalvar1 = 0;
		}
	} else if (_mgoal == MonsterGoal::SHOOT) {
		if (_mgoalvar1 == 3) {
			_mgoal = MonsterGoal::NORMAL;
			M_StartFadeout(md, true);
		} else if (LineClear(_m, f)) {
			M_StartRAttack(MissileType::KRULL, 4);
			_mgoalvar1++;
		} else {
			M_StartDelay(random_(112, 10) + 5);
			_mgoalvar1++;
		}
	} else if (_mgoal == MonsterGoal::RETREAT) {
		M_StartFadein(md, false);
		_mgoal = MonsterGoal::SHOOT;
	}
	_mdir = md;
	random_(112, 100);
	if (_mmode != MonsterMode::STAND)
		return;

	if (dist < 2 && _mgoal == MonsterGoal::NORMAL) {
		M_TryH2HHit(_menemy, mHit, mMinDamage, mMaxDamage);
		_mgoal = MonsterGoal::RETREAT;
		if (!M_CallWalk(opposite(md))) {
			M_StartFadein(md, false);
			_mgoal = MonsterGoal::SHOOT;
		}
	} else if (!M_CallWalk(md) && (_mgoal == MonsterGoal::NORMAL || _mgoal == MonsterGoal::RETREAT)) {
		M_StartFadein(md, false);
		_mgoal = MonsterGoal::SHOOT;
	}
}

void Monster::MAI::Fallen()
{
	int m, rad;
	Dir md;

	if (_mgoal == MonsterGoal::SHOOT) {
		if (_mgoalvar1)
			_mgoalvar1--;
		else
			_mgoal = MonsterGoal::NORMAL;
	}

	if (_mmode != MonsterMode::STAND || _msquelch == 0) return;
	if (_mgoal == MonsterGoal::RETREAT) {
		if (!_mgoalvar1--) {
			_mgoal = MonsterGoal::NORMAL;
			M_StartStand(opposite(_mdir));
		}
	}

	if (_mAnimFrame == _mAnimLen) {
		if (random_(113, 4)) {
			return;
		}
		if (!(_mFlags & MonsterFlag::noheal)) {
			M_StartSpStand(_mdir);
			rad = 2 * _mint + 2;
			if (_mmaxhp - rad >= _mhitpoints)
				_mhitpoints = rad + _mhitpoints;
			else
				_mhitpoints = _mmaxhp;
		}
		rad = 2 * _mint + 4;
		for (int y = -rad; y <= rad; y++) {
			for (int x = -rad; x <= rad; x++) {
				if (y >= 0 && y < MAXDUNY && x >= 0 && x < MAXDUNX) {
					m = grid[x + _m.x][y + _m.y].getActor();
					if (m > 0) {
						m--;
						if (monsters[m]._mAi == MonstAi::FALLEN) {
							monsters[m]._mgoal = MonsterGoal::SHOOT;
							monsters[m]._mgoalvar1 = 30 * _mint + 105;
						}
					}
				}
			}
		}
	} else if (_mgoal == MonsterGoal::RETREAT) {
		md = _mdir;
		M_CallWalk(md);
	} else if (_mgoal == MonsterGoal::SHOOT) {
		int dist = (_m - _menemypos).maxabs();
		if (dist < 2) {
			M_StartAttack();
		} else {
			md = M_GetDir();
			M_CallWalk(md);
		}
	} else {
		SkelSd();
	}
}

void Monster::MAI::Cleaver()
{
	if (_mmode != MonsterMode::STAND || _msquelch == 0) return;

	V2Di f = _menemypos;
	int dist = (_m - f).maxabs();
	Dir md = GetDirection(_m, _last);
	_mdir = md;

	if (dist >= 2) M_CallWalk(md);
	else M_StartAttack();

	if (_mmode == MonsterMode::STAND)
		_mAnimData = MType->Anims[MonstAnim::STAND].Data[int(md)];
}

void Monster::MAI::Round(bool special)
{
	if (_mmode != MonsterMode::STAND || _msquelch == 0) return;

	V2Di f = _menemypos;
	int dist = (_m - f).maxabs();
	Dir md = GetDirection(_m, _last);
	if (_msquelch < UINT8_MAX)
		MonstCheckDoors(i);
	int v = random_(114, 100);
	if ((dist >= 2) && _msquelch == UINT8_MAX && grid.at(_m).dTransVal == grid.at(f).dTransVal) {
		if (_mgoal == MonsterGoal::MOVE || (dist >= 4) && random_(115, 4) == 0) {
			if (_mgoal != MonsterGoal::MOVE) {
				_mgoalvar1 = 0;
				_mgoalvar2 = random_(116, 2);
			}
			_mgoal = MonsterGoal::MOVE;
			if (_mgoalvar1++ >= 2 * dist && DirOK(md) || grid.at(_m).dTransVal != grid.at(f).dTransVal) {
				_mgoal = MonsterGoal::NORMAL;
			} else if (!M_RoundWalk(md, &_mgoalvar2)) {
				M_StartDelay(random_(125, 10) + 10);
			}
		}
	} else
		_mgoal = MonsterGoal::NORMAL;
	if (_mgoal == MonsterGoal::NORMAL) {
		if (dist >= 2) {
			if (_mVar2 > 20 && v < 2 * _mint + 28
			    || (_mVar1 == MonsterMode::WALK || _mVar1 == MonsterMode::WALK2 || _mVar1 == MonsterMode::WALK3)
			        && _mVar2 == 0
			        && v < 2 * _mint + 78) {
				M_CallWalk(md);
			}
		} else if (v < 2 * _mint + 23) {
			_mdir = md;
			if (special && _mhitpoints < (_mmaxhp >> 1) && random_(117, 2) != 0)
				M_StartSpAttack();
			else
				M_StartAttack();
		}
	}
	if (_mmode == MonsterMode::STAND)
		_mAnimData = MType->Anims[MonstAnim::STAND].Data[int(md)];
}

void Monster::MAI::GoatMc()
{
	Round(true);
}

void Monster::MAI::Ranged(int missile_type, bool special)
{
	if (_mmode != MonsterMode::STAND) return;

	if (_msquelch == UINT8_MAX || _mFlags & MonsterFlag::targets_monster) {
		V2Di f = _menemypos;
		int dist = (_m - f).maxabs();
		Dir md = M_GetDir();
		if (_msquelch < UINT8_MAX)
			MonstCheckDoors(i);
		_mdir = md;
		if (_mVar1 == MonsterMode::RATTACK) {
			M_StartDelay(random_(118, 20));
		} else if (dist < 4) {
			if (random_(119, 100) < 10 * (_mint + 7))
				M_CallWalk(opposite(md));
		}
		if (_mmode == MonsterMode::STAND) {
			if (LineClear(_m, f)) {
				if (special)
					M_StartRSpAttack(missile_type, 4);
				else
					M_StartRAttack(missile_type, 4);
			} else {
				_mAnimData = MType->Anims[MonstAnim::STAND].Data[int(md)];
			}
		}
	} else if (_msquelch != 0) {
		Dir md = GetDirection(_m, _last);
		M_CallWalk(md);
	}
}

void Monster::MAI::GoatBow()
{
	Ranged(MissileType::ARROW, false);
}

void Monster::MAI::Succ()
{
	Ranged(MissileType::FLARE, false);
}

void Monster::MAI::AcidUniq()
{
	Ranged(MissileType::ACID, true);
}

void Monster::MAI::Scav()
{
	int x, y;
	V2Di _m = _m;
	bool done = false;
	if (_mmode != MonsterMode::STAND)
		return;
	if (_mhitpoints < (_mmaxhp >> 1) && _mgoal != MonsterGoal::HEALING) {
		if (leaderflag) {
			monsters[leader].packsize--;
			leaderflag = 0;
		}
		_mgoal = MonsterGoal::HEALING;
		_mgoalvar3 = 10;
	}
	if (_mgoal == MonsterGoal::HEALING && _mgoalvar3 != 0) {
		_mgoalvar3--;
		if (grid.at(_m).dDead) {
			M_StartEat();
			if (!(_mFlags & MonsterFlag::noheal))
				_mhitpoints += 64;
			if (_mhitpoints >= (_mmaxhp >> 1) + (_mmaxhp >> 2)) {
				_mgoal = MonsterGoal::NORMAL;
				_mgoalvar1 = 0;
				_mgoalvar2 = 0;
			}
		} else {
			if (_mgoalvar1 == 0) {
				if (random_(120, 2) != 0) {
					for (y = -4; y <= 4 && !done; y++) {
						for (x = -4; x <= 4 && !done; x++) {
							// BUGFIX: incorrect check of offset against limits of the dungeon
							if (y < 0 || y >= MAXDUNY || x < 0 || x >= MAXDUNX)
								continue;
							done = grid[_m.x + x][_m.y + y].dDead != 0
							    && LineClearF(
							        CheckNoSolid,
							        _m,
							        { _m.x + x,
							            _m.y + y });
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
							done = grid[_m.x + x][_m.y + y].dDead != 0
							    && LineClearF(
							        CheckNoSolid,
							        _m,
							        { _m.x + x,
							            _m.y + y });
						}
					}
					x++;
					y++;
				}
				if (done) {
					_mgoalvar1 = x + _m.x + 1;
					_mgoalvar2 = y + _m.y + 1;
				}
			}
			if (_mgoalvar1) {
				x = _mgoalvar1 - 1;
				y = _mgoalvar2 - 1;
				_mdir = GetDirection(_m, { x, y });
				M_CallWalk(_mdir);
			}
		}
	}
	if (_mmode == MonsterMode::STAND)
		SkelSd();
}

void Monster::MAI::Garg()
{
	Dir md = M_GetDir();
	if (_msquelch != 0 && _mFlags & MonsterFlag::allow_special) {
		M_Enemy();
		int dist = (_m - _last).maxabs();
		if (dist < _mint + 2) {
			_mFlags &= ~MonsterFlag::allow_special;
		}
		return;
	}

	if (_mmode != MonsterMode::STAND || _msquelch == 0) return;

	if (_mhitpoints<_mmaxhp >> 1 && !(_mFlags & MonsterFlag::noheal))
		_mgoal = MonsterGoal::RETREAT;
	if (_mgoal == MonsterGoal::RETREAT) {
		int dist = (_m - _last).maxabs();
		if (dist >= _mint + 2) {
			_mgoal = MonsterGoal::NORMAL;
			M_StartHeal();
		} else if (!M_CallWalk(opposite(md))) {
			_mgoal = MonsterGoal::NORMAL;
		}
	}
	Round(false);
}

void Monster::MAI::RoundRanged(int missile_type, bool checkdoors, int dam, int lessmissiles)
{
	if (_mmode != MonsterMode::STAND || _msquelch == 0)
		return;

	V2Di f = _menemypos;
	int dist = (_m - f).maxabs();
	Dir md = GetDirection(_m, _last);
	if (checkdoors && _msquelch < UINT8_MAX)
		MonstCheckDoors(i);
	int v = random_(121, 10000);
	if ((dist >= 2) && _msquelch == UINT8_MAX && grid.at(_m).dTransVal == grid.at(f).dTransVal) {
		if (_mgoal == MonsterGoal::MOVE || ((dist >= 3) && random_(122, 4 << lessmissiles) == 0)) {
			if (_mgoal != MonsterGoal::MOVE) {
				_mgoalvar1 = 0;
				_mgoalvar2 = random_(123, 2);
			}
			_mgoal = MonsterGoal::MOVE;
			if (_mgoalvar1++ >= 2 * dist && DirOK(md)) {
				_mgoal = MonsterGoal::NORMAL;
			} else if (v < 500 * (_mint + 1) >> lessmissiles
			    && (LineClear(_m, f))) {
				M_StartRSpAttack(missile_type, dam);
			} else {
				M_RoundWalk(md, &_mgoalvar2);
			}
		}
	} else {
		_mgoal = MonsterGoal::NORMAL;
	}
	if (_mgoal == MonsterGoal::NORMAL) {
		if (((dist >= 3) && v < ((500 * (_mint + 2)) >> lessmissiles)
		        || v < ((500 * (_mint + 1)) >> lessmissiles))
		    && LineClear(_m, f)) {
			M_StartRSpAttack(missile_type, dam);
		} else if (dist >= 2) {
			v = random_(124, 100);
			if (v < 1000 * (_mint + 5)
			    || (_mVar1 == MonsterMode::WALK || _mVar1 == MonsterMode::WALK2 || _mVar1 == MonsterMode::WALK3) && _mVar2 == 0 && v < 1000 * (_mint + 8)) {
				M_CallWalk(md);
			}
		} else if (v < 1000 * (_mint + 6)) {
			_mdir = md;
			M_StartAttack();
		}
	}
	if (_mmode == MonsterMode::STAND) {
		M_StartDelay(random_(125, 10) + 5);
	}
}

void Monster::MAI::Magma()
{
	RoundRanged(MissileType::MAGMABALL, true, 4, 0);
}

void Monster::MAI::Storm()
{
	RoundRanged(MissileType::LIGHTCTRL2, true, 4, 0);
}

void Monster::MAI::Acid()
{
	RoundRanged(MissileType::ACID, false, 4, 1);
}

void Monster::MAI::Diablo()
{
	RoundRanged(MissileType::DIABAPOCA, false, 40, 0);
}

void Monster::MAI_RR2(int mistype, int dam)
{
	V2Di f = _menemypos;
	int dist = (_m - f).maxabs();
	if (dist >= 5) {
		SkelSd();
		return;
	}

	if (_mmode != MonsterMode::STAND || _msquelch == 0) return;

	Dir md = GetDirection(_m, _last);
	if (_msquelch < UINT8_MAX)
		MonstCheckDoors(i);
	int v = random_(121, 100);
	if ((dist >= 2) && _msquelch == UINT8_MAX && grid.at(_m).dTransVal == grid.at(f).dTransVal) {
		if (_mgoal == MonsterGoal::MOVE || (dist >= 3)) {
			if (_mgoal != MonsterGoal::MOVE) {
				_mgoalvar1 = 0;
				_mgoalvar2 = random_(123, 2);
			}
			_mgoal = MonsterGoal::MOVE;
			_mgoalvar3 = 4;
			if (_mgoalvar1++ < 2 * dist || !DirOK(md)) {
				if (v < 5 * (_mint + 16))
					M_RoundWalk(md, &_mgoalvar2);
			} else
				_mgoal = MonsterGoal::NORMAL;
		}
	} else
		_mgoal = MonsterGoal::NORMAL;
	if (_mgoal == MonsterGoal::NORMAL) {
		if (((dist >= 3) && v < 5 * (_mint + 2) || v < 5 * (_mint + 1) || _mgoalvar3 == 4) && LineClear(_m, f)) {
			M_StartRSpAttack(mistype, dam);
		} else if (dist >= 2) {
			v = random_(124, 100);
			if (v < 2 * (5 * _mint + 25)
				|| (_mVar1 == MonsterMode::WALK || _mVar1 == MonsterMode::WALK2 || _mVar1 == MonsterMode::WALK3)
				    && _mVar2 == 0
				    && v < 2 * (5 * _mint + 40)) {
				M_CallWalk(md);
			}
		} else {
			if (random_(124, 100) < 10 * (_mint + 4)) {
				_mdir = md;
				if (random_(124, 2) != 0)
					M_StartAttack();
				else
					M_StartRSpAttack(mistype, dam);
			}
		}
		_mgoalvar3 = 1;
	}
	if (_mmode == MonsterMode::STAND) {
		M_StartDelay(random_(125, 10) + 5);
	}
}

void Monster::MAI::Mega()
{
	MAI_RR2(MissileType::FLAMEC, 0);
}

void Monster::MAI::Golum()
{
	int j, k, _menemy;
	Dir md;
	bool have_enemy, ok;
	if (_m.x == 1 && _m.y == 0) {
		return;
	}

	if (_mmode == MonsterMode::DEATH
	    || _mmode == MonsterMode::SPSTAND
	    || (_mmode >= MonsterMode::WALK && _mmode <= MonsterMode::WALK3)) {
		return;
	}

	if (!(_mFlags & MonsterFlag::targets_monster)) M_Enemy();

	have_enemy = !(_mFlags & MonsterFlag::no_enemy);

	if (_mmode == MonsterMode::ATTACK)
		return;
	_menemy = _menemy;
	V2Di m = _m;
	int dist = (m - monsters[_menemy]._mfut).maxabs();
	md = GetDirection(m, monsters[_menemy]._m);
	_mdir = md;
	if (dist >= 2) {
		if (have_enemy && Path()) return;
	} else if (have_enemy) {
		_menemy = _menemy;
		_menemypos = monsters[_menemy]._m;
		if (monsters[_menemy]._msquelch == 0) {
			monsters[_menemy]._msquelch = UINT8_MAX;
			monsters[_menemy]._last = _m;
			for (j = 0; j < 5; j++) {
				for (k = 0; k < 5; k++) {
					_menemy = grid[_m.x + k - 2][_m.y + j - 2].getActor();
					if (_menemy > 0)
						monsters[_menemy]._msquelch = UINT8_MAX;
				}
			}
		}
		M_StartAttack();
		return;
	}

	_pathcount++;
	if (_pathcount > 8) _pathcount = 5;

	ok = M_CallWalk(plr[i]._pdir);
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

void Monster::MAI::SkelKing()
{
	if (_mmode != MonsterMode::STAND || _msquelch == 0) return;

	V2Di f = _menemypos;
	int dist = (_m - f).maxabs();
	Dir md = GetDirection(_m, _last);
	if (_msquelch < UINT8_MAX) MonstCheckDoors(i);
	int v = random_(126, 100);
	if ((dist >= 2) && _msquelch == UINT8_MAX && grid.at(_m).dTransVal == grid.at(f).dTransVal) {
		if (_mgoal == MonsterGoal::MOVE || (dist >= 3) && random_(127, 4) == 0) {
			if (_mgoal != MonsterGoal::MOVE) {
				_mgoalvar1 = 0;
				_mgoalvar2 = random_(128, 2);
			}
			_mgoal = MonsterGoal::MOVE;
			if (_mgoalvar1++ >= 2 * dist && DirOK(md) || grid.at(_m).dTransVal != grid.at(f).dTransVal) {
				_mgoal = MonsterGoal::NORMAL;
			} else if (!M_RoundWalk(md, &_mgoalvar2)) {
				M_StartDelay(random_(125, 10) + 10);
			}
		}
	} else
		_mgoal = MonsterGoal::NORMAL;
	if (_mgoal == MonsterGoal::NORMAL) {
		if (plr.isSingleplayer()
		    && ((dist >= 3) && v < 4 * _mint + 35 || v < 6)
		    && LineClear(_m, f)) {
			V2Di n = _m + offset(md);
			if (PosOkMonst(i, n) && nummonsters < MAXMONSTERS) {
				M_SpawnSkel(n, md);
				M_StartSpStand(md);
			}
		} else {
			if (dist >= 2) {
				v = random_(129, 100);
				if (v >= _mint + 25
				    && (_mVar1 != MonsterMode::WALK && _mVar1 != MonsterMode::WALK2 && _mVar1 != MonsterMode::WALK3 || _mVar2 != 0 || (v >= _mint + 75))) {
					M_StartDelay(random_(130, 10) + 10);
				} else {
					M_CallWalk(md);
				}
			} else if (v < _mint + 20) {
				_mdir = md;
				M_StartAttack();
			}
		}
	}
	if (_mmode == MonsterMode::STAND) _mAnimData = MType->Anims[MonstAnim::STAND].Data[int(md)];
}

void Monster::MAI::Rhino()
{
	if (_mmode != MonsterMode::STAND || _msquelch == 0) return;

	V2Di f = _menemypos;
	int dist = (_m - f).maxabs();
	Dir md = GetDirection(_m, _last);
	if (_msquelch < UINT8_MAX)
		MonstCheckDoors(i);
	int v = random_(131, 100);
	if (dist >= 2) {
		if (_mgoal == MonsterGoal::MOVE || (dist >= 5) && random_(132, 4) != 0) {
			if (_mgoal != MonsterGoal::MOVE) {
				_mgoalvar1 = 0;
				_mgoalvar2 = random_(133, 2);
			}
			_mgoal = MonsterGoal::MOVE;
			if (_mgoalvar1++ >= 2 * dist || grid.at(_m).dTransVal != grid.at(f).dTransVal) {
				_mgoal = MonsterGoal::NORMAL;
			} else if (!M_RoundWalk(md, &_mgoalvar2)) {
				M_StartDelay(random_(125, 10) + 10);
			}
		}
	} else
		_mgoal = MonsterGoal::NORMAL;
	if (_mgoal == MonsterGoal::NORMAL) {
		if ((dist >= 5)
		    && v < 2 * _mint + 43
		    && LineClearF1(PosOkMonst, i, _m, f)) {
			if (AddMissile(_m, f, md, MissileType::RHINO, _menemy, i, 0, 0) != -1) {
				if (MData->snd_special)
					PlayEffect(i, 3);
				_mmode = MonsterMode::CHARGE;
				grid.at(_m).setActor(i);
			}
		} else {
			if (dist >= 2) {
				v = random_(134, 100);
				if (v >= 2 * _mint + 33
				    && (_mVar1 != MonsterMode::WALK && _mVar1 != MonsterMode::WALK2 && _mVar1 != MonsterMode::WALK3
				        || _mVar2
				        || v >= 2 * _mint + 83)) {
					M_StartDelay(random_(135, 10) + 10);
				} else {
					M_CallWalk(md);
				}
			} else if (v < 2 * _mint + 28) {
				_mdir = md;
				M_StartAttack();
			}
		}
	}
	if (_mmode == MonsterMode::STAND)
		_mAnimData = MType->Anims[MonstAnim::STAND].Data[int(_mdir)];
}

void Monster::MAI::Counselor()
{
	if (_mmode != MonsterMode::STAND || _msquelch == 0) return;

	V2Di f = _menemypos;
	int dist = (_m - f).maxabs();
	Dir md = GetDirection(_m, _last);
	if (_msquelch < UINT8_MAX)
		MonstCheckDoors(i);
	int v = random_(121, 100);
	if (_mgoal == MonsterGoal::RETREAT) {
		if (_mgoalvar1++ <= 3)
			M_CallWalk(opposite(md));
		else {
			_mgoal = MonsterGoal::NORMAL;
			M_StartFadein(md, true);
		}
	} else if (_mgoal == MonsterGoal::MOVE) {
		if ((dist >= 2) && _msquelch == UINT8_MAX && grid.at(_m).dTransVal == grid.at(f).dTransVal) {
			if (_mgoalvar1++ < 2 * dist || !DirOK(md)) {
				M_RoundWalk(md, &_mgoalvar2);
			} else {
				_mgoal = MonsterGoal::NORMAL;
				M_StartFadein(md, true);
			}
		} else {
			_mgoal = MonsterGoal::NORMAL;
			M_StartFadein(md, true);
		}
	} else if (_mgoal == MonsterGoal::NORMAL) {
		if (dist >= 2) {
			if (v < 5 * (_mint + 10) && LineClear(_m, f)) {
				M_StartRAttack(counsmiss[_mint], mMinDamage + random_(77, mMaxDamage - mMinDamage + 1));
			} else if (random_(124, 100) < 30) {
				_mgoal = MonsterGoal::MOVE;
				_mgoalvar1 = 0;
				M_StartFadeout(md, false);
			} else
				M_StartDelay(random_(105, 10) + 2 * (5 - _mint));
		} else {
			_mdir = md;
			if (_mhitpoints < (_mmaxhp >> 1)) {
				_mgoal = MonsterGoal::RETREAT;
				_mgoalvar1 = 0;
				M_StartFadeout(md, false);
			} else if (_mVar1 == MonsterMode::DELAY
			    || random_(105, 100) < 2 * _mint + 20) {
				M_StartRAttack(-1, 0);
				AddMissile(_m, { 0, 0 }, _mdir, MissileType::FLASH, 1, i, 4, 0);
				AddMissile(_m, { 0, 0 }, _mdir, MissileType::FLASH2, 1, i, 4, 0);
			} else
				M_StartDelay(random_(105, 10) + 2 * (5 - _mint));
		}
	}
	if (_mmode == MonsterMode::STAND) {
		M_StartDelay(random_(125, 10) + 5);
	}
}

void Monster::MAI::Garbud()
{
	if (_mmode != MonsterMode::STAND)
		return;

	V2Di _m = _m;
	Dir md = M_GetDir();

	if (mtalkmsg < TEXT_GARBUD4
	    && mtalkmsg > TEXT_DOOM10
	    && !(grid.at(_m).dFlags & DunTileFlag::VISIBLE)
	    && _mgoal == MonsterGoal::TALKING) {
		_mgoal = MonsterGoal::INQUIRING;
		mtalkmsg++;
	}

	if (grid.at(_m).dFlags & DunTileFlag::VISIBLE) {
		if (mtalkmsg == TEXT_GARBUD4) {
			if (!effect_is_playing(USFX_GARBUD4) && _mgoal == MonsterGoal::TALKING) {
				_mgoal = MonsterGoal::NORMAL;
				_msquelch = UINT8_MAX;
				mtalkmsg = 0;
			}
		}
	}

	if (_mgoal == MonsterGoal::NORMAL || _mgoal == MonsterGoal::MOVE)
		Round(true);

	_mdir = md;

	if (_mmode == MonsterMode::STAND)
		_mAnimData = MType->Anims[MonstAnim::STAND].Data[int(md)];
}

void Monster::MAI::Zhar()
{
	if (_mmode != MonsterMode::STAND) {
		return;
	}

	V2Di m = _m;
	Dir md = M_GetDir();
	if (mtalkmsg == TEXT_ZHAR1 && !(grid.at(m).dFlags & DunTileFlag::VISIBLE) && _mgoal == MonsterGoal::TALKING) {
		mtalkmsg = TEXT_ZHAR2;
		_mgoal = MonsterGoal::INQUIRING;
	}

	if (grid.at(m).dFlags & DunTileFlag::VISIBLE) {
		V2Di _m = _m - _menemypos;
		if (mtalkmsg == TEXT_ZHAR2) {
			if (!effect_is_playing(USFX_ZHAR2) && _mgoal == MonsterGoal::TALKING) {
				_msquelch = UINT8_MAX;
				mtalkmsg = 0;
				_mgoal = MonsterGoal::NORMAL;
			}
		}
	}

	if (_mgoal == MonsterGoal::NORMAL || _mgoal == MonsterGoal::RETREAT || _mgoal == MonsterGoal::MOVE)
		Counselor();

	_mdir = md;

	if (_mmode == MonsterMode::STAND)
		_mAnimData = MType->Anims[MonstAnim::STAND].Data[int(md)];
}

void Monster::MAI::SnotSpil()
{
	if (_mmode != MonsterMode::STAND) return;

	V2Di m = _m;
	Dir md = M_GetDir();

	if (mtalkmsg == TEXT_BANNER10 && !(grid.at(m).dFlags & DunTileFlag::VISIBLE) && _mgoal == MonsterGoal::TALKING) {
		mtalkmsg = TEXT_BANNER11;
		_mgoal = MonsterGoal::INQUIRING;
	}

	if (mtalkmsg == TEXT_BANNER11 && quests[Q_LTBANNER]._qvar1 == 3) {
		mtalkmsg = 0;
		_mgoal = MonsterGoal::NORMAL;
	}

	if (grid.at(m).dFlags & DunTileFlag::VISIBLE) {
		if (mtalkmsg == TEXT_BANNER12) {
			if (!effect_is_playing(USFX_SNOT3) && _mgoal == MonsterGoal::TALKING) {
				ObjChangeMap({ lvl.getpc().x, lvl.getpc().y }, { lvl.getpc().x + lvl.getpc().w + 1, lvl.getpc().y + lvl.getpc().h + 1 });
				quests[Q_LTBANNER]._qvar1 = 3;
				RedoPlayerVision();
				_msquelch = UINT8_MAX;
				mtalkmsg = 0;
				_mgoal = MonsterGoal::NORMAL;
			}
		}
		if (quests[Q_LTBANNER]._qvar1 == 3) {
			if (_mgoal == MonsterGoal::NORMAL || _mgoal == MonsterGoal::SHOOT)
				Fallen();
		}
	}

	_mdir = md;

	if (_mmode == MonsterMode::STAND)
		_mAnimData = MType->Anims[MonstAnim::STAND].Data[int(md)];
}

void Monster::MAI::Lazurus()
{
	if (_mmode != MonsterMode::STAND)
		return;

	V2Di m = _m;
	Dir md = M_GetDir();
	if (grid.at(m).dFlags & DunTileFlag::VISIBLE) {
		if (plr.isSingleplayer()) {
			if (mtalkmsg == TEXT_VILE13 && _mgoal == MonsterGoal::INQUIRING && myplr().pos().x == TEXT_VILE13 && myplr().pos().y == 46) {
				PlayInGameMovie("gendata\\fprst3.smk");
				_mmode = MonsterMode::TALK;
				quests[Q_BETRAYER]._qvar1 = 5;
			}

			if (mtalkmsg == TEXT_VILE13 && !effect_is_playing(USFX_LAZ1) && _mgoal == MonsterGoal::TALKING) {
				ObjChangeMapResync({ 1, 18 }, { 20, 24 });
				RedoPlayerVision();
				_msquelch = UINT8_MAX;
				mtalkmsg = 0;
				quests[Q_BETRAYER]._qvar1 = 6;
				_mgoal = MonsterGoal::NORMAL;
			}
		}

		if (plr.isMultiplayer() && mtalkmsg == TEXT_VILE13 && _mgoal == MonsterGoal::INQUIRING && quests[Q_BETRAYER]._qvar1 <= 3) {
			_mmode = MonsterMode::TALK;
		}
	}

	if (_mgoal == MonsterGoal::NORMAL || _mgoal == MonsterGoal::RETREAT || _mgoal == MonsterGoal::MOVE) {
		mtalkmsg = 0;
		Counselor();
	}

	_mdir = md;
	if (_mmode == MonsterMode::STAND || _mmode == MonsterMode::TALK)
		_mAnimData = MType->Anims[MonstAnim::STAND].Data[int(md)];
}

void Monster::MAI::Lazhelp()
{
	if (_mmode != MonsterMode::STAND)
		return;

	V2Di _m = _m;
	Dir md = M_GetDir();

	if (grid.at(_m).dFlags & DunTileFlag::VISIBLE) {
		if (plr.isSingleplayer()) {
			if (quests[Q_BETRAYER]._qvar1 <= 5) {
				_mgoal = MonsterGoal::INQUIRING;
			} else {
				mtalkmsg = 0;
				_mgoal = MonsterGoal::NORMAL;
			}
		} else
			_mgoal = MonsterGoal::NORMAL;
	}
	if (_mgoal == MonsterGoal::NORMAL)
		Succ();
	_mdir = md;
	if (_mmode == MonsterMode::STAND)
		_mAnimData = MType->Anims[MonstAnim::STAND].Data[int(md)];
}

void Monster::MAI::Lachdanan()
{
	if (_mmode != MonsterMode::STAND)
		return;

	V2Di _m = _m;
	Dir md = M_GetDir();
	if (mtalkmsg == TEXT_VEIL9 && !(grid.at(_m).dFlags & DunTileFlag::VISIBLE) && _mgoal == MonsterGoal::TALKING) {
		mtalkmsg = TEXT_VEIL10;
		_mgoal = MonsterGoal::INQUIRING;
	}

	if (grid.at(_m).dFlags & DunTileFlag::VISIBLE) {
		if (mtalkmsg == TEXT_VEIL11) {
			if (!effect_is_playing(USFX_LACH3) && _mgoal == MonsterGoal::TALKING) {
				mtalkmsg = 0;
				quests[Q_VEIL]._qactive = QUEST_DONE;
				M_StartKill(-1);
			}
		}
	}

	_mdir = md;

	if (_mmode == MonsterMode::STAND) _mAnimData = MType->Anims[MonstAnim::STAND].Data[int(md)];
}

void Monster::MAI::Warlord()
{
	if (_mmode != MonsterMode::STAND)
		return;

	V2Di m = _m;
	Dir md = M_GetDir();
	if (grid.at(m).dFlags & DunTileFlag::VISIBLE) {
		if (mtalkmsg == TEXT_WARLRD9 && _mgoal == MonsterGoal::INQUIRING)
			_mmode = MonsterMode::TALK;
		if (mtalkmsg == TEXT_WARLRD9 && !effect_is_playing(USFX_WARLRD1) && _mgoal == MonsterGoal::TALKING) {
			_msquelch = UINT8_MAX;
			mtalkmsg = 0;
			_mgoal = MonsterGoal::NORMAL;
		}
	}

	if (_mgoal == MonsterGoal::NORMAL)
		SkelSd();

	_mdir = md;

	if (_mmode == MonsterMode::STAND || _mmode == MonsterMode::TALK)
		_mAnimData = MType->Anims[MonstAnim::STAND].Data[int(_mdir)];
}

DEVILUTION_END_NAMESPACE
