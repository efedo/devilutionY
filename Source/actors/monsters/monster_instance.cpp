/**
 * @file monster_instance.cpp
 *
 * Monster instance class
 */
#include "all.h"
#include "../3rdParty/Storm/Source/storm.h"

namespace dvl {

Monster::Monster()
	: Actor(ActorType::monster)
{};

Monster::Monster(Dir rd, MonsterType mtype, V2Di pos)
    : Actor(ActorType::monster)
{
	CMonster *monst = &beastiary.getClass(mtype).data;
	data._mdir = rd;
	data._m = pos;
	data._mfut = pos;
	data._mold = pos;
	data._mMTidx = mtype;
	data.mName = monst->MData->mName;
	data.MType = monst;
	data.MData = monst->MData;
	data._mAnimData = monst->getAnim(MonstAnim::STAND, rd);
	data._mAnimDelay = monst->getAnim(MonstAnim::STAND).Rate;
	data._mAnimCnt = random_(88, data._mAnimDelay - 1);
	data._mAnimLen = monst->getAnim(MonstAnim::STAND).Frames;
	data._mAnimFrame = random_(88, data._mAnimLen - 1) + 1;

	if (monst->mtype == MonsterType::DIABLO) {
		data._mmaxhp = (random_(88, 1) + 1666) << 6;
	} else {
		data._mmaxhp = (monst->mMinHP + random_(88, monst->mMaxHP - monst->mMinHP + 1)) << 6;
	}

	if (game.isSingleplayer()) {
		data._mmaxhp >>= 1;
		if (data._mmaxhp < 64) data._mmaxhp = 64;
	}

	data._mhitpoints = data._mmaxhp;
	data._mAi = monst->MData->mAi;
	data._mint = monst->MData->mInt;
	data._mRndSeed = GetRndSeed();
	data._mAISeed = GetRndSeed();
	data.mWhoHit.clear();
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
	data._mFlags = monst->MData->mFlags;

	if (data._mAi == MonstAi::GARG) {
		data._mAnimData = monst->getAnim(MonstAnim::SPECIAL, rd);
		data._mAnimFrame = 1;
		data._mFlags |= MonsterFlag::allow_special;
		data._mmode = MonsterMode::SATTACK;
	}

	if (gnDifficulty == Difficulty::NIGHTMARE) {
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

	if (gnDifficulty == Difficulty::HELL) {
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
void Monster::PlantInPosition(Dir dir, V2Di * pos)
{
	if (pos) data._m = *pos;
	data._moff = { 0, 0 };
	data._mfut = data._m;
	data._mold = data._m;
	if (dir != Dir::NONE) data._mdir = dir;
}

void Monster::ClearMVars()
{
	data._mVar1 = 0;
	data._mVar1_mmode = MonsterMode::STAND;
	data._mVar1_mistype = MissileType::ARROW;
	data._mVar2 = 0;
	data._mVar3 = 0;
	data._mVar4 = 0;
	data._mVar5 = 0;
	data._mVar6 = 0;
	data._mVar7 = 0;
	data._mVar8 = 0;
}

void Monster::NewMonsterAnim(AnimStruct *anim, Dir md)
{
	data._mAnimData = anim->Data[int(md)];
	data._mAnimLen = anim->Frames;
	data._mAnimCnt = 0;
	data._mAnimFrame = 1;
	data._mAnimDelay = anim->Rate;
	data._mFlags &= ~(MonsterFlag::lock_animation | MonsterFlag::allow_special);
	data._mdir = md;
}

void Monster::GetTargetLoc(V2Di &pos, int &dist, Dir &dir)
{
	throw;
}

bool Monster::M_Ranged()
{
	MonstAi ai = data._mAi;
	return ai == MonstAi::SKELBOW || ai == MonstAi::GOATBOW || ai == MonstAi::SUCC || ai == MonstAi::LAZHELP;
}

bool Monster::M_Talker()
{
	MonstAi ai = data._mAi;
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
	int dist;
	bool sameroom;
	V2Di enemy_pos;
	ActorId _menemy = ActorId::getInvalid();
	int best_dist = -1;
	int bestsameroom = 0;
	//Monst = monster + i;
	if (!(data._mFlags & MonsterFlag::golem)) {
		for (Player * player: actors.getPlayerList()) {
			if (!player->plractive || lvl.currlevel != player->plrlevel || player->_pLvlChanging || (player->_hp == 0 && game.isMultiplayer()))
				continue;
			if (grid.at(data._m).dTransVal == grid.at(player->pos()).dTransVal)
				sameroom = true;
			else
				sameroom = false;
			int dist = (data._m - player->pos()).maxabs();
			if ((sameroom && !bestsameroom)
			    || ((sameroom || !bestsameroom) && dist < best_dist)
			    || (_menemy == ActorId::getInvalid())) {
				data._mFlags &= ~MonsterFlag::targets_monster;
				_menemy = player->id();
				enemy_pos.x = player->futpos().x;
				enemy_pos.y = player->futpos().y;
				best_dist = dist;
				bestsameroom = sameroom;
			}
		}
	}
	for (Monster * monster : actors.getMonsterList()) {
		if (monster == this) continue;
		if (monster->data._m.x == 1 && monster->data._m.y == 0) continue;
		if (monster->M_Talker() && monster->data.mtalkmsg)
			continue;

		int dista = (monster->data._m - data._m).maxabs();
		if (!(data._mFlags & MonsterFlag::golem)
		        && ((dista >= 2) && !M_Ranged()
		        || (!(data._mFlags & MonsterFlag::golem) && !(monster->data._mFlags & MonsterFlag::golem)))) {
			continue;
		}
		sameroom = grid.at(data._m).dTransVal == grid.at(monster->data._m).dTransVal;
		int distb = (data._m - monster->data._m).maxabs();
		if ((sameroom && !bestsameroom)
		    || ((sameroom || !bestsameroom) && distb < best_dist)
		    || (_menemy == ActorId::getInvalid())) {
			data._mFlags |= MonsterFlag::targets_monster;
			_menemy = monster->id();
			enemy_pos.x = monster->data._mfut.x;
			enemy_pos.y = monster->data._mfut.y;
			best_dist = dist;
			bestsameroom = sameroom;
		}
	}
	if (_menemy != ActorId::getInvalid()) {
		data._mFlags &= ~MonsterFlag::no_enemy;
		_menemy = _menemy;
		data._menemypos = enemy_pos;
	} else {
		data._mFlags |= MonsterFlag::no_enemy;
	}
}

Dir Monster::M_GetDir()
{
	return GetDirection(data._m, data._menemypos);
}

void Monster::M_StartStand(Dir md)
{
	ClearMVars();
	if (data.MType->mtype == MonsterType::GOLEM)
		NewMonsterAnim(&data.MType->getAnim(MonstAnim::WALK), md);
	else
		NewMonsterAnim(&data.MType->getAnim(MonstAnim::STAND), md);
	data._mVar1_mmode = data._mmode;
	data._mVar2 = 0;
	data._mmode = MonsterMode::STAND;
	PlantInPosition(md);
	M_Enemy();
}

void Monster::M_StartDelay(int len)
{
	if (len <= 0) return;
	if (data._mAi != MonstAi::LAZURUS) {
		data._mVar2 = len;
		data._mmode = MonsterMode::DELAY;
	}
}

void Monster::M_StartSpStand(Dir md)
{
	NewMonsterAnim(&data.MType->getAnim(MonstAnim::SPECIAL), md);
	data._mmode = MonsterMode::SPSTAND;
	PlantInPosition(md);
}

void Monster::M_StartWalk(V2Di vel, V2Di add, Dir EndDir)
{
	V2Di f = add + data._m;
	grid.at(f).setActor(*this);
	data._mmode = MonsterMode::WALK;
	data._mold = data._m;
	data._mfut = f;
	data._mvel = vel;
	data._mVar1 = add.x;
	data._mVar2 = add.y;
	data._mVar3 = int(EndDir);
	data._mdir = EndDir;
	NewMonsterAnim(&data.MType->getAnim(MonstAnim::WALK), EndDir);
	data._mVar6 = 0;
	data._mVar7 = 0;
	data._mVar8 = 0;
}

void Monster::M_StartWalk2(V2Di vel, V2Di off, V2Di add, Dir EndDir)
{
	V2Di f = add + data._m;
	grid.at(data._m).setActor(*this);
	data._mVar1 = data._m.x;
	data._mVar2 = data._m.y;
	data._mold = data._m;
	data._m = f;
	data._mfut = f;
	grid.at(f).setActor(*this);
	if (data._uniqtype != UniqueMonsterType::INVALID) ChangeLightXY(data.mlid, data._m);
	data._moff = off;
	data._mmode = MonsterMode::WALK2;
	data._mvel = vel;
	data._mVar3 = int(EndDir);
	data._mdir = EndDir;
	NewMonsterAnim(&data.MType->getAnim(MonstAnim::WALK), EndDir);
	data._mVar6 = 16 * off.x;
	data._mVar7 = 16 * off.y;
	data._mVar8 = 0;
}

void Monster::M_StartWalk3(V2Di vel, V2Di off, V2Di add, V2Di map, Dir EndDir)
{
	V2Di f = add + data._m;
	V2Di n = map + data._m;
	if (data._uniqtype != UniqueMonsterType::INVALID) ChangeLightXY(data.mlid, n);
	grid.at(data._m).setActor(*this);
	grid.at(f).setActor(*this);
	data._mVar4 = n.x;
	data._mVar5 = n.y;
	grid[n.x][n.y].dFlags |= DunTileFlag::MONSTLR;
	data._mold = data._m;
	data._mfut = f;
	data._moff = off;
	data._mmode = MonsterMode::WALK3;
	data._mvel = vel;
	data._mVar1 = f.x;
	data._mVar2 = f.y;
	data._mVar3 = int(EndDir);
	data._mdir = EndDir;
	NewMonsterAnim(&data.MType->getAnim(MonstAnim::WALK), EndDir);
	data._mVar6 = 16 * off.x;
	data._mVar7 = 16 * off.y;
	data._mVar8 = 0;
}

void Monster::M_StartAttack()
{
	Dir md = M_GetDir();
	NewMonsterAnim(&data.MType->getAnim(MonstAnim::ATTACK), md);
	data._mmode = MonsterMode::ATTACK;
	PlantInPosition(md);
}

void Monster::M_StartRAttack(MissileType missile_type, int dam)
{
	Dir md = M_GetDir();
	NewMonsterAnim(&data.MType->getAnim(MonstAnim::ATTACK), md);
	data._mmode = MonsterMode::RATTACK;
	data._mVar1_mistype = missile_type;
	data._mVar2 = dam;
	PlantInPosition(md);
}

void Monster::M_StartRSpAttack(MissileType missile_type, int dam)
{
	Dir md = M_GetDir();
	NewMonsterAnim(&data.MType->getAnim(MonstAnim::SPECIAL), md);
	data._mmode = MonsterMode::RSPATTACK;
	data._mVar1_mistype = missile_type;
	data._mVar2 = 0;
	data._mVar3 = dam;
	PlantInPosition(md);
}

void Monster::M_StartSpAttack()
{
	Dir md = M_GetDir();
	NewMonsterAnim(&data.MType->getAnim(MonstAnim::SPECIAL), md);
	data._mmode = MonsterMode::SATTACK;
	PlantInPosition(md);
}

void Monster::M_StartEat()
{
	NewMonsterAnim(&data.MType->getAnim(MonstAnim::SPECIAL), data._mdir);
	data._mmode = MonsterMode::SATTACK;
	PlantInPosition(data._mdir);
}

void Monster::M_ClearSquares()
{
	V2Di m = data._mold;
	for (int y = m.y - 1; y <= m.y + 1; y++) {
		if (y >= 0 && y < MAXDUNY) {
			for (int x = m.x - 1; x <= m.x + 1; x++) {
				if (x >= 0 && x < MAXDUNX && (&grid[x][y].getActor() == this))
					grid[x][y].clearActor(*this);
			}
		}
	}
	if (m.x + 1 < MAXDUNX) grid[m.x + 1][m.y].dFlags &= ~DunTileFlag::MONSTLR;
	if (m.y + 1 < MAXDUNY) grid[m.x][m.y + 1].dFlags &= ~DunTileFlag::MONSTLR;
}

void Monster::M_GetKnockback()
{
	Dir d = rotate(data._mdir, -4);
	if (DirOK(d)) {
		M_ClearSquares();
		data._mold += offset(d);
		NewMonsterAnim(&data.MType->getAnim(MonstAnim::GOTHIT), data._mdir);
		data._mmode = MonsterMode::GOTHIT;
		data._moff = { 0, 0 };
		data._m = data._mold;
		data._mfut = data._m;
		// BUGFIX useless assignment
		data._mold = data._m;
		M_ClearSquares();
		grid.at(data._m).setActor(*this);
	}
}

void Monster::startHitByPlayer(ActorId pnum, int dam)
{
	if (pnum.isValid()) data.mWhoHit.insert(pnum);
	if (pnum == myplr()) {
		delta_monster_hp(i, data._mhitpoints, lvl.currlevel);
		NetSendCmdParam2(false, Cmd::MONSTDAMAGE, i, dam);
	}
	PlayEffect(1);
	if (data.MType->mtype >= MonsterType::SNEAK && data.MType->mtype <= MonsterType::ILLWEAV || dam >> 6 >= data.mLevel + 3) {
		if (pnum.isValid()) {
			data._mFlags &= ~MonsterFlag::targets_monster;
			data._menemy = pnum;
			data._menemypos = plr[pnum].futpos();
			data._mdir = M_GetDir();
		}
		if (data.MType->mtype == MonsterType::BLINK) {
			M_Teleport();
		} else if (data.MType->mtype >= MonsterType::NSCAV && MType->mtype <= MonsterType::YSCAV) {
			data._mgoal = MonsterGoal::NORMAL;
		}
		if (data._mmode != MonsterMode::STONE) {
			NewMonsterAnim(&data.MType->getAnim(MonstAnim::GOTHIT), data._mdir);
			data._mmode = MonsterMode::GOTHIT;
			PlantInPosition(data._mdir, &data._mold);
			M_ClearSquares();
			grid.at(data._m).setActor(*this);
		}
	}
}

void Monster::M_DiabloDeath(bool sendmsg)
{
	V2Di _mold;
	PlaySFX(USFX_DIABLOD);
	quests[QuestId::diablo]._qactive = QuestState::done;
	if (sendmsg) NetSendCmdQuest(true, QuestId::diablo);
	gbProcessPlayers = false;
	sgbSaveSoundOn = gbSoundOn;

	// Kills all monsters?
	Monster *k = 0;

	for (auto i : actors.getMonsterList()) {
		k = i;
		if (i == this || _msquelch == 0) continue;
		i->NewMonsterAnim(&i->MType->getAnim(MonstAnim::DEATH),
		                  i->_mdir);
		i->_mVar1 = 0;
		i->_mmode = MonsterMode::DEATH;
		_mold = i->_mold;
		i->PlantInPosition(i->_mdir, &_mold);
		i->M_ClearSquares();
		grid.at(i->_m).setActor(*i);
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

void Monster::MonstStartKill(ActorId pnum, bool sendmsg)
{
	Dir md;

	//MonsterStruct *Monst;
	if (pnum >= 0) mWhoHit |= 1 << pnum;
	if (pnum < MAX_PLRS && i > MAX_PLRS) plr.AddPlrMonstExper(mLevel, mExp, mWhoHit);


	monstkills[MType->mtype]++;
	_mhitpoints = 0;
	SetRndSeed(_mRndSeed);
	if (QuestStatus(QuestId::garbud) && mName == UniqMonst[UniqueMonsterType::GARBUD].mName) {
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
	NewMonsterAnim(&MType->getAnim(MonstAnim::DEATH), md);
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

void Monster::M2MStartKill(ActorId killed)
{
	Dir md;

	Monster * victim = actors.getMonster(killed);

	if (!victim) {
		app_fatal("M2MStartKill: Invalid monster (killed) %d", *victim);
	}

	delta_kill_monster(*victim, victim->_m, lvl.currlevel);
	NetSendCmdLocParam1(false, int(Cmd::MONSTDEATH), victim->_m, size_t(victim->id()));

	victim->mWhoHit |= 1 << i;
	if (i < MAX_PLRS) actors.AddPlrMonstExper(victim->mLevel, victim->mExp, victim->mWhoHit);

	beastiary.getClass(victim->MType->mtype).kills++;
	victim->_mhitpoints = 0;
	SetRndSeed(victim->_mRndSeed);

	if (mid >= MAX_PLRS)
		SpawnItem(mid, victim->_m, true);

	if (victim->MType->mtype == MonsterType::DIABLO)
		victim->M_DiabloDeath(true);
	else
		PlayEffect(i, 2);

	PlayEffect(mid, 2);

	md = rotate(_mdir, -4);
	if (victim->MType->mtype == MonsterType::GOLEM) md = Dir(0);

	victim->_mdir = md;
	victim->NewMonsterAnim(&victim->MType->getAnim(MonstAnim::DEATH), md);
	victim->_mmode = MonsterMode::DEATH;
	victim->PlantInPosition(md, &victim->_mold);
	victim->M_ClearSquares();
	grid.at(victim->_m).setActor(mid);
	CheckQuestKill(mid, true);
	M_FallenFear(victim->_m);
	if (victim->MType->mtype >= MonsterType::NACID && victim->MType->mtype <= MonsterType::XACID)
		AddMissile(victim->_m, { 0, 0 }, Dir(0), MissileType::ACIDPUD, 1, mid, victim->_mint + 1, 0);
}

void Monster::startKillByPlayer(ActorId pnum)
{
	if (myplr() == pnum) {
		delta_kill_monster(id(), _m, lvl.currlevel);
		if (id() != pnum) {
			NetSendCmdLocParam1(false, Cmd::MONSTDEATH, _m, i);
		} else {
			NetSendCmdLocParam1(false, Cmd::KILLGOLEM, _m, lvl.currlevel);
		}
	}
	MonstStartKill(pnum, true);
}

void Monster::M_SyncStartKill(V2Di pos, ActorId pnum)
{
	if (_mhitpoints == 0 || _mmode == MonsterMode::DEATH) return;

	if (grid.at(pos).isActor()) {
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
	NewMonsterAnim(&MType->getAnim(MonstAnim::SPECIAL), md);
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
	NewMonsterAnim(&MType->getAnim(MonstAnim::SPECIAL), md);
	_mmode = MonsterMode::FADEOUT;
	PlantInPosition(md);
	if (backwards) {
		_mFlags |= MonsterFlag::lock_animation;
		_mAnimFrame = _mAnimLen;
	}
}

void Monster::M_StartHeal()
{
	_mAnimData = MType->getAnim(MonstAnim::SPECIAL, _mdir);
	_mAnimFrame = MType->getAnim(MonstAnim::SPECIAL).Frames;
	_mFlags |= MonsterFlag::lock_animation;
	_mmode = MonsterMode::HEAL;
	_mVar1 = _mmaxhp / (16 * (random_(97, 5) + 4));
}

void Monster::M_ChangeLightOffset()
{
	V2Di moff;
	int lx = _moff.x + 2 * _moff.y;
	int ly = 2 * _moff.y - _moff.x;

	int sign;
	if (lx < 0) {
		sign = -1;
		lx = -lx;
	} else {
		sign = 1;
	}

	moff.x = sign * (lx >> 3);
	if (ly < 0) {
		moff.y = -1;
		ly = -ly;
	} else {
		moff.y = 1;
	}

	moff.y *= (ly >> 3);
	ChangeLightOff(mlid, moff);
}

bool Monster::M_DoStand()
{
	if (MType->mtype == MonsterType::GOLEM)
		_mAnimData = MType->getAnim(MonstAnim::WALK, _mdir);
	else
		_mAnimData = MType->getAnim(MonstAnim::STAND, _mdir);

	if (_mAnimFrame == _mAnimLen)
		M_Enemy();

	_mVar2++;
	return false;
}

bool Monster::M_DoWalk()
{
	bool rv = false;
	if (_mVar8 == MType->getAnim(MonstAnim::WALK).Frames) {
		grid.at(_m).clearActor(*this);
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
	if (_mVar8 == MType->getAnim(MonstAnim::WALK).Frames) {
		grid[_mVar1][_mVar2].clearActor(*this);
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
	if (_mVar8 == MType->getAnim(MonstAnim::WALK).Frames) {
		grid.at(_m).clearActor(*this);
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
	if (player._pIFlags & ItemSpecialEffectFlag::THORNS) { // Player has thorns
		mdam = (random_(99, 3) + 1);
		_mhitpoints -= mdam;
		if (_mhitpoints <= 0)
			startKillByPlayer(pnum);
		else
			startHitByPlayer(pnum, mdam);
	}
	if (!(_mFlags & MonsterFlag::nolifesteal) && MType->mtype == MonsterType::SKING && game.isMultiplayer())
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
			quests[QuestId::garbud]._qactive = QuestState::active;
		quests[QuestId::garbud]._qlog = true;
		if (mtalkmsg == TEXT_GARBUD2 && !(_mFlags & MonsterFlag::quest_complete)) {
			SpawnItem(i, { _m.x + 1, _m.y + 1 }, true);
			_mFlags |= MonsterFlag::quest_complete;
		}
	}
	if (mName == UniqMonst[UniqueMonsterType::ZHAR].mName
	    && mtalkmsg == TEXT_ZHAR1
	    && !(_mFlags & MonsterFlag::quest_complete)) {
		quests[QuestId::zhar]._qactive = QuestState::active;
		quests[QuestId::zhar]._qlog = true;
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
			quests[QuestId::ltbanner]._qvar1 = 2;
			if (quests[QuestId::ltbanner]._qactive == QuestState::init)
				quests[QuestId::ltbanner]._qactive = QuestState::active;
			_mFlags |= MonsterFlag::quest_complete;
		}
		if (quests[QuestId::ltbanner]._qvar1 < 2) {
			sprintf(tempstr, "SS Talk = %i, Flags = %i", mtalkmsg, _mFlags);
			app_fatal(tempstr);
		}
	}
	if (mName == UniqMonst[UniqueMonsterType::LACHDAN].mName) {
		if (mtalkmsg == TEXT_VEIL9) {
			quests[QuestId::veil]._qactive = QuestState::active;
			quests[QuestId::veil]._qlog = true;
		}
		if (mtalkmsg == TEXT_VEIL11 && !(_mFlags & MonsterFlag::quest_complete)) {
			SpawnUnique(UniqueItemId::STEELVEIL, { _m.x + 1, _m.y + 1 });
			_mFlags |= MonsterFlag::quest_complete;
		}
	}
	if (mName == UniqMonst[UniqueMonsterType::WARLORD].mName)
		quests[QuestId::warlord]._qvar1 = 2;
	if (mName == UniqMonst[UniqueMonsterType::LAZURUS].mName && game.isMultiplayer()) {
		_msquelch = UINT8_MAX;
		mtalkmsg = 0;
		quests[QuestId::betrayer]._qvar1 = 6;
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
		grid.at(_m).clearActor(*this);
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

		grid.at(_m).clearActor(*this);
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
	_mAnimData = MType->getAnim(MonstAnim::STAND, M_GetDir());
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
		grid.at(_m).clearActor(*this);
		_mDelFlag = true;
	}
	return false;
}

void Monster::M_WalkDir(Dir md)
{
	int mwi = MType->getAnim(MonstAnim::WALK).Frames - 1;
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


void Monster::SyncMonsterAnim()
{
	MType->mtype = beastiary.getClass(_mMTidx).data.mtype;
	MData = beastiary.getClass(_mMTidx).data.MData;
	if (_uniqtype != 0)
		mName = UniqMonst[data._uniqtype - 1].mName;
	else
		mName = MData->mName;

	switch (data._mmode) {
	case MonsterMode::WALK:
	case MonsterMode::WALK2:
	case MonsterMode::WALK3:
		data._mAnimData = data.MType->getAnim(MonstAnim::WALK, _mdir);
		return;
	case MonsterMode::ATTACK:
	case MonsterMode::RATTACK:
		data._mAnimData = data.MType->getAnim(MonstAnim::ATTACK, _mdir);
		return;
	case MonsterMode::GOTHIT:
		data._mAnimData = data.MType->getAnim(MonstAnim::GOTHIT, _mdir);
		return;
	case MonsterMode::DEATH:
		data._mAnimData = data.MType->getAnim(MonstAnim::DEATH, _mdir);
		return;
	case MonsterMode::SATTACK:
	case MonsterMode::FADEIN:
	case MonsterMode::FADEOUT:
		data._mAnimData = data.MType->getAnim(MonstAnim::SPECIAL, _mdir);
		return;
	case MonsterMode::SPSTAND:
	case MonsterMode::RSPATTACK:
		data._mAnimData = data.MType->getAnim(MonstAnim::SPECIAL, _mdir);
		return;
	case MonsterMode::HEAL:
		data._mAnimData = data.MType->getAnim(MonstAnim::SPECIAL, _mdir);
		return;
	case MonsterMode::STAND:
		data._mAnimData = data.MType->getAnim(MonstAnim::STAND, _mdir);
		return;
	case MonsterMode::DELAY:
		data._mAnimData = data.MType->getAnim(MonstAnim::STAND, _mdir);
		return;
	case MonsterMode::TALK:
		data._mAnimData = data.MType->getAnim(MonstAnim::STAND, _mdir);
		return;
	case MonsterMode::CHARGE:
		data._mAnimData = data.MType->getAnim(MonstAnim::ATTACK, _mdir);
		data._mAnimFrame = 1;
		data._mAnimLen = data.MType->getAnim(MonstAnim::ATTACK).Frames;
		break;
	default:
		data._mAnimData = data.MType->getAnim(MonstAnim::STAND, _mdir);
		data._mAnimFrame = 1;
		data._mAnimLen = data.MType->getAnim(MonstAnim::STAND).Frames;
		break;
	}
}

void Monster::PlayEffect(int mode)
{
	if (myplr().pLvlLoad) return;
	int sndIdx = random_(164, 2);
	if (!gbSndInited || !gbSoundOn || gbBufferMsgs) return;
	MonsterType mi = _mMTidx;
	TSnd* snd = beastiary.getClass(mi).data.Snds[mode][sndIdx];
	if (!snd || snd_playing(snd)) return;
	int lVolume, lPan;
	if (!calc_snd_position(_m, &lVolume, &lPan)) return;
	snd_play_snd(snd, lVolume, lPan);
}

bool Monster::PosOkMonst(V2Di pos)
{
	bool fire = false;
	bool ret = !grid.at(pos).isSolid() && !grid.at(pos).isPlayer() && !grid.at(pos).isActor();
	if (ret && grid.at(pos).isObject()) {
		int oi = grid.at(pos).getObject();
		if (object[oi]._oSolidFlag) ret = false;
	}

	if (ret && grid.at(pos).getMissile() && i >= 0) {
		int mi = grid.at(pos).getMissile();
		if (mi > 0) {
			if (missile[mi - 1]._mitype == MissileType::FIREWALL) { // BUGFIX: Change 'mi' to 'mi - 1' (fixed)
				fire = true;
			}
			else {
				for (int j = 0; j < nummissiles; j++) {
					if (missile[missileactive[j]]._mitype == MissileType::FIREWALL)
						fire = true;
				}
			}
		}
		if (fire && (!(data.mMagicRes & MonsterResistFlag::imune_fire) || data.MType->mtype == MonsterType::DIABLO))
			ret = false;
	}
	return ret;
}

bool Monster::PosOkMonst2(V2Di pos)
{
	bool fire = false;
	bool ret = !grid.at(pos).isSolid();
	if (ret && grid.at(pos).isObject()) {
		int oi = grid.at(pos).getObject();
		if (object[oi]._oSolidFlag) ret = false;
	}

	if (ret && grid.at(pos).getMissile()) {
		int mi = grid.at(pos).getMissile();
		if (mi > 0) {
			if (missile[mi - 1]._mitype == MissileType::FIREWALL) { // BUGFIX: Change 'mi' to 'mi - 1' (fixed)
				fire = true;
			}
			else {
				for (int j = 0; j < nummissiles; j++) {
					if (missile[missileactive[j]]._mitype == MissileType::FIREWALL)
						fire = true;
				}
			}
		}
		
		if (fire && (!(data.mMagicRes & MonsterResistFlag::imune_fire) || data.MType->mtype == MonsterType::DIABLO))
			ret = false;
	}
	return ret;
}

bool Monster::PosOkMonst3(V2Di pos)
{
	bool fire = false;
	bool ret = true;
	bool isdoor = false;
	if (ret && grid.at(pos).isObject()) {
		int oi = grid.at(pos).getObject();
		ObjectType objtype = object[oi]._otype;
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
		int mi = grid.at(pos).getMissile();
		if (mi > 0) {
			if (missile[mi - 1]._mitype == MissileType::FIREWALL) { // BUGFIX: Change 'mi' to 'mi - 1' (fixed)
				fire = true;
			}
			else {
				for (int j = 0; j < nummissiles; j++) {
					mi = missileactive[j];
					if (missile[mi]._mitype == MissileType::FIREWALL) {
						fire = true;
					}
				}
			}
		}
		if (fire && (!(data.mMagicRes & MonsterResistFlag::imune_fire) || data.MType->mtype == MonsterType::DIABLO)) {
			ret = false;
		}
	}
	return ret;
}

bool Monster::CanTalkToMonst()
{
	if (data._mgoal == MonsterGoal::INQUIRING) {
		return true;
	}

	return data._mgoal == MonsterGoal::TALKING;
}

bool Monster::CheckMonsterHit(bool* ret)
{
	if (data._mAi == MonstAi::GARG && data._mFlags & MonsterFlag::allow_special) {
		data._mFlags &= ~MonsterFlag::allow_special;
		data._mmode = MonsterMode::SATTACK;
		*ret = true;
		return true;
	}

	if (data.MType->mtype >= MonsterType::COUNSLR && data.MType->mtype <= MonsterType::ADVOCATE) {
		if (data._mgoal != MonsterGoal::NORMAL) {
			*ret = false;
			return true;
		}
	}
	return false;
}

int Monster::encode_enemy()
{
	if (data._mFlags & MonsterFlag::targets_monster)
		return data._menemy + MAX_PLRS;
	else
		return data._menemy;
}

void Monster::decode_enemy(int enemy)
{
	if (enemy < MAX_PLRS) {
		data._mFlags &= ~MonsterFlag::targets_monster;
		data._menemy = enemy;
		data._menemypos = plr[enemy].futpos();
	}
	else {
		data._mFlags |= MonsterFlag::targets_monster;
		enemy -= MAX_PLRS;
		data._menemy = enemy;
		data._menemypos = monsters[enemy].data._mfut;
	}
}


void Monster::ActivateSpawn(V2Di pos, Dir dir)
{
	grid.at(pos).setActor(i);
	data._m = pos;
	data._mfut = pos;
	data._mold = pos;
	M_StartSpStand(dir);
}


void Monster::TalktoMonster()
{
	int pnum, itm;
	pnum = data._menemy;
	data._mmode = MonsterMode::TALK;
	if (data._mAi == MonstAi::SNOTSPIL || data._mAi == MonstAi::LACHDAN) {
		if (QuestStatus(QuestId::ltbanner) && quests[QuestId::ltbanner]._qvar1 == 2 && HasItem(pnum, ItemIndex::BANNER, &itm)) {
			plr[pnum].inventory.RemoveInvItem(itm);
			quests[QuestId::ltbanner]._qactive = QuestState::done;
			data.mtalkmsg = TEXT_BANNER12;
			data._mgoal = MonsterGoal::INQUIRING;
		}
		if (QuestStatus(QuestId::veil) && data.mtalkmsg >= TEXT_VEIL9) {
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

}
