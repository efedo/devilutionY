/**
 * @file monster_instance.cpp
 *
 * Monster instance class
 */
#include "all.h"

namespace dvl {


void Monster::ai_Zombie()
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
			}
			else {
				md = M_GetDir();
				M_CallWalk(md);
			}
		}
	}
	else if (v < 2 * _mint + 10) {
		M_StartAttack();
	}

	if (_mmode == MonsterMode::STAND)
		_mAnimData = MType->Anims[MonstAnim::STAND].Data[int(_mdir)];
}

void Monster::ai_SkelSd()
{
	if (_mmode != MonsterMode::STAND || _msquelch == 0)
		return;

	V2Di m = _m;
	int dist = (m - _menemypos).maxabs();
	Dir md = GetDirection(m, _last);
	_mdir = md;
	if (dist >= 2) {
		if (_mVar1_mmode == MonsterMode::DELAY || (random_(106, 100) >= 35 - 4 * _mint)) {
			M_CallWalk(md);
		}
		else {
			M_StartDelay(15 - 2 * _mint + random_(106, 10));
		}
	}
	else {
		if (_mVar1_mmode == MonsterMode::DELAY || (random_(105, 100) < 2 * _mint + 20)) {
			M_StartAttack();
		}
		else {
			M_StartDelay(2 * (5 - _mint) + random_(105, 10));
		}
	}

	if (_mmode == MonsterMode::STAND)
		_mAnimData = MType->Anims[MonstAnim::STAND].Data[int(md)];
}

bool Monster::ai_Path()
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

void Monster::ai_Snake()
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
		if (dist < 3 && LineClearF1(PosOkMonst, i, _m, f) && _mVar1_mmode != MonsterMode::CHARGE) {
			if (AddMissile(_m, f, md, MissileType::RHINO, pnum, i, 0, 0) != -1) {
				PlayEffect(i, 0);
				grid.at(_m).setActor(i);
				_mmode = MonsterMode::CHARGE;
			}
			// This is all just code to choose a direction
		}
		else if (_mVar1_mmode == MonsterMode::DELAY || random_(106, 100) >= 35 - 2 * _mint) {
			tmp = rotate(md, pattern[_mgoalvar1]);
			_mgoalvar1++;
			if (_mgoalvar1 > 5) _mgoalvar1 = 0;
			tmp = rotate(md, -_mgoalvar2);

			if (int(md) > 0) {
				if (int(md) < 4) {
					md = right(Dir(_mgoalvar2));
					_mgoalvar2 = int(md);
				}
				else if (md == Dir(4)) {
					_mgoalvar2 = int(tmp);
				}
				else {
					md = left(Dir(_mgoalvar2));
					_mgoalvar2 = int(md);
				}
			}
			if (!M_DumbWalk(Dir(_mgoalvar2))) M_CallWalk2(_mdir);
		}
		else {
			M_StartDelay(15 - _mint + random_(106, 10));
		}
	}
	else {
		if (_mVar1_mmode == MonsterMode::DELAY
			|| _mVar1_mmode == MonsterMode::CHARGE
			|| (random_(105, 100) < _mint + 20)) {
			M_StartAttack();
		}
		else
			M_StartDelay(10 - _mint + random_(105, 10));
	}
	if (_mmode == MonsterMode::STAND)
		_mAnimData = MType->Anims[MonstAnim::STAND].Data[int(_mdir)];
}

void Monster::ai_Bat()
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
		}
		else {
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
	}
	else if (dist >= 2) {
		if (_mVar2 > 20 && v < _mint + 13
			|| (_mVar1_mmode == MonsterMode::WALK || _mVar1_mmode == MonsterMode::WALK2 || _mVar1_mmode == MonsterMode::WALK3)
			&& _mVar2 == 0
			&& v < _mint + 63) {
			M_CallWalk(md);
		}
	}
	else if (v < 4 * _mint + 8) {
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

void Monster::ai_SkelBow()
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
			|| (_mVar1_mmode == MonsterMode::WALK || _mVar1_mmode == MonsterMode::WALK2 || _mVar1_mmode == MonsterMode::WALK3)
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

void Monster::ai_Fat()
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
			|| (_mVar1_mmode == MonsterMode::WALK || _mVar1_mmode == MonsterMode::WALK2 || _mVar1_mmode == MonsterMode::WALK3)
			&& _mVar2 == 0
			&& v < 4 * _mint + 70) {
			M_CallWalk(md);
		}
	}
	else if (v < 4 * _mint + 15) {
		M_StartAttack();
	}
	else if (v < 4 * _mint + 20) {
		M_StartSpAttack();
	}

	if (_mmode == MonsterMode::STAND)
		_mAnimData = MType->Anims[MonstAnim::STAND].Data[int(md)];
}

void Monster::ai_Sneak()
{
	if (_mmode != MonsterMode::STAND)
		return;

	V2Di mb = _m;
	if (grid.at(mb).dLight != lightmax) {
		V2Di d = (mb - _menemypos).abs();
		Dir md = M_GetDir();
		int dist = 5 - _mint;
		if (_mVar1_mmode == MonsterMode::GOTHIT) {
			_mgoalvar1 = 0;
			_mgoal = MonsterGoal::RETREAT;
		}
		else {
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
		}
		else {
			if ((d.maxdim() >= dist + 1) && !(_mFlags & MonsterFlag::hidden)) {
				M_StartFadeout(md, true);
			}
			else {
				if (_mgoal == MonsterGoal::RETREAT
					|| (d.maxdim() >= 2) && (_mVar2 > 20 && v < 4 * _mint + 14 || (_mVar1_mmode == MonsterMode::WALK ||
						_mVar1_mmode == MonsterMode::WALK2 || _mVar1_mmode == MonsterMode::WALK3) && _mVar1_mmode == 0 && v < 4 * _mint + 64)) {
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

void Monster::ai_Fireman()
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
	}
	else if (_mgoal == MonsterGoal::SHOOT) {
		if (_mgoalvar1 == 3) {
			_mgoal = MonsterGoal::NORMAL;
			M_StartFadeout(md, true);
		}
		else if (LineClear(_m, f)) {
			M_StartRAttack(MissileType::KRULL, 4);
			_mgoalvar1++;
		}
		else {
			M_StartDelay(random_(112, 10) + 5);
			_mgoalvar1++;
		}
	}
	else if (_mgoal == MonsterGoal::RETREAT) {
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
	}
	else if (!M_CallWalk(md) && (_mgoal == MonsterGoal::NORMAL || _mgoal == MonsterGoal::RETREAT)) {
		M_StartFadein(md, false);
		_mgoal = MonsterGoal::SHOOT;
	}
}

void Monster::ai_Fallen()
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
	}
	else if (_mgoal == MonsterGoal::RETREAT) {
		md = _mdir;
		M_CallWalk(md);
	}
	else if (_mgoal == MonsterGoal::SHOOT) {
		int dist = (_m - _menemypos).maxabs();
		if (dist < 2) {
			M_StartAttack();
		}
		else {
			md = M_GetDir();
			M_CallWalk(md);
		}
	}
	else {
		ai_SkelSd();
	}
}

void Monster::ai_Cleaver()
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

void Monster::ai_Round(bool special)
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
			}
			else if (!M_RoundWalk(md, &_mgoalvar2)) {
				M_StartDelay(random_(125, 10) + 10);
			}
		}
	}
	else
		_mgoal = MonsterGoal::NORMAL;
	if (_mgoal == MonsterGoal::NORMAL) {
		if (dist >= 2) {
			if (_mVar2 > 20 && v < 2 * _mint + 28
				|| (_mVar1_mmode == MonsterMode::WALK || _mVar1_mmode == MonsterMode::WALK2 || _mVar1_mmode == MonsterMode::WALK3)
				&& _mVar2 == 0
				&& v < 2 * _mint + 78) {
				M_CallWalk(md);
			}
		}
		else if (v < 2 * _mint + 23) {
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

void Monster::ai_GoatMc()
{
	ai_Round(true);
}

void Monster::ai_Ranged(MissileType missile_type, bool special)
{
	if (_mmode != MonsterMode::STAND) return;

	if (_msquelch == UINT8_MAX || _mFlags & MonsterFlag::targets_monster) {
		V2Di f = _menemypos;
		int dist = (_m - f).maxabs();
		Dir md = M_GetDir();
		if (_msquelch < UINT8_MAX)
			MonstCheckDoors(i);
		_mdir = md;
		if (_mVar1_mmode == MonsterMode::RATTACK) {
			M_StartDelay(random_(118, 20));
		}
		else if (dist < 4) {
			if (random_(119, 100) < 10 * (_mint + 7))
				M_CallWalk(opposite(md));
		}
		if (_mmode == MonsterMode::STAND) {
			if (LineClear(_m, f)) {
				if (special)
					M_StartRSpAttack(missile_type, 4);
				else
					M_StartRAttack(missile_type, 4);
			}
			else {
				_mAnimData = MType->Anims[MonstAnim::STAND].Data[int(md)];
			}
		}
	}
	else if (_msquelch != 0) {
		Dir md = GetDirection(_m, _last);
		M_CallWalk(md);
	}
}

void Monster::ai_GoatBow()
{
	ai_Ranged(MissileType::ARROW, false);
}

void Monster::ai_Succ()
{
	ai_Ranged(MissileType::FLARE, false);
}

void Monster::ai_AcidUniq()
{
	ai_Ranged(MissileType::ACID, true);
}

void Monster::ai_Scav()
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
		}
		else {
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
				}
				else {
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
		ai_SkelSd();
}

void Monster::ai_Garg()
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

	if (_mhitpoints < _mmaxhp >> 1 && !(_mFlags & MonsterFlag::noheal))
		_mgoal = MonsterGoal::RETREAT;
	if (_mgoal == MonsterGoal::RETREAT) {
		int dist = (_m - _last).maxabs();
		if (dist >= _mint + 2) {
			_mgoal = MonsterGoal::NORMAL;
			M_StartHeal();
		}
		else if (!M_CallWalk(opposite(md))) {
			_mgoal = MonsterGoal::NORMAL;
		}
	}
	ai_Round(false);
}

void Monster::ai_RoundRanged(MissileType missile_type, bool checkdoors, int dam, int lessmissiles)
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
			}
			else if (v < 500 * (_mint + 1) >> lessmissiles
				&& (LineClear(_m, f))) {
				M_StartRSpAttack(missile_type, dam);
			}
			else {
				M_RoundWalk(md, &_mgoalvar2);
			}
		}
	}
	else {
		_mgoal = MonsterGoal::NORMAL;
	}
	if (_mgoal == MonsterGoal::NORMAL) {
		if (((dist >= 3) && v < ((500 * (_mint + 2)) >> lessmissiles)
			|| v < ((500 * (_mint + 1)) >> lessmissiles))
			&& LineClear(_m, f)) {
			M_StartRSpAttack(missile_type, dam);
		}
		else if (dist >= 2) {
			v = random_(124, 100);
			if (v < 1000 * (_mint + 5)
				|| (_mVar1_mmode == MonsterMode::WALK || _mVar1_mmode == MonsterMode::WALK2 || _mVar1_mmode == MonsterMode::WALK3) && _mVar2 == 0 && v < 1000 * (_mint + 8)) {
				M_CallWalk(md);
			}
		}
		else if (v < 1000 * (_mint + 6)) {
			_mdir = md;
			M_StartAttack();
		}
	}
	if (_mmode == MonsterMode::STAND) {
		M_StartDelay(random_(125, 10) + 5);
	}
}

void Monster::ai_Magma()
{
	ai_RoundRanged(MissileType::MAGMABALL, true, 4, 0);
}

void Monster::ai_Storm()
{
	ai_RoundRanged(MissileType::LIGHTCTRL2, true, 4, 0);
}

void Monster::ai_Acid()
{
	ai_RoundRanged(MissileType::ACID, false, 4, 1);
}

void Monster::ai_Diablo()
{
	ai_RoundRanged(MissileType::DIABAPOCA, false, 40, 0);
}

void Monster::ai_RR2(MissileType mistype, int dam)
{
	V2Di f = _menemypos;
	int dist = (_m - f).maxabs();
	if (dist >= 5) {
		ai_SkelSd();
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
			}
			else
				_mgoal = MonsterGoal::NORMAL;
		}
	}
	else
		_mgoal = MonsterGoal::NORMAL;
	if (_mgoal == MonsterGoal::NORMAL) {
		if (((dist >= 3) && v < 5 * (_mint + 2) || v < 5 * (_mint + 1) || _mgoalvar3 == 4) && LineClear(_m, f)) {
			M_StartRSpAttack(mistype, dam);
		}
		else if (dist >= 2) {
			v = random_(124, 100);
			if (v < 2 * (5 * _mint + 25)
				|| (_mVar1_mmode == MonsterMode::WALK || _mVar1_mmode == MonsterMode::WALK2 || _mVar1_mmode == MonsterMode::WALK3)
				&& _mVar2 == 0
				&& v < 2 * (5 * _mint + 40)) {
				M_CallWalk(md);
			}
		}
		else {
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

void Monster::ai_Mega()
{
	ai_RR2(MissileType::FLAMEC, 0);
}

void Monster::ai_Golum()
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
	}
	else if (have_enemy) {
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

void Monster::ai_SkelKing()
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
			}
			else if (!M_RoundWalk(md, &_mgoalvar2)) {
				M_StartDelay(random_(125, 10) + 10);
			}
		}
	}
	else
		_mgoal = MonsterGoal::NORMAL;
	if (_mgoal == MonsterGoal::NORMAL) {
		if (game.isSingleplayer()
			&& ((dist >= 3) && v < 4 * _mint + 35 || v < 6)
			&& LineClear(_m, f)) {
			V2Di n = _m + offset(md);
			if (PosOkMonst(i, n) && nummonsters < MAXMONSTERS) {
				M_SpawnSkel(n, md);
				M_StartSpStand(md);
			}
		}
		else {
			if (dist >= 2) {
				v = random_(129, 100);
				if (v >= _mint + 25
					&& (_mVar1_mmode != MonsterMode::WALK && _mVar1_mmode != MonsterMode::WALK2 && _mVar1_mmode != MonsterMode::WALK3 || _mVar2 != 0 || (v >= _mint + 75))) {
					M_StartDelay(random_(130, 10) + 10);
				}
				else {
					M_CallWalk(md);
				}
			}
			else if (v < _mint + 20) {
				_mdir = md;
				M_StartAttack();
			}
		}
	}
	if (_mmode == MonsterMode::STAND) _mAnimData = MType->Anims[MonstAnim::STAND].Data[int(md)];
}

void Monster::ai_Rhino()
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
			}
			else if (!M_RoundWalk(md, &_mgoalvar2)) {
				M_StartDelay(random_(125, 10) + 10);
			}
		}
	}
	else
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
		}
		else {
			if (dist >= 2) {
				v = random_(134, 100);
				if (v >= 2 * _mint + 33
					&& (_mVar1_mmode != MonsterMode::WALK && _mVar1_mmode != MonsterMode::WALK2 && _mVar1_mmode != MonsterMode::WALK3
						|| _mVar2
						|| v >= 2 * _mint + 83)) {
					M_StartDelay(random_(135, 10) + 10);
				}
				else {
					M_CallWalk(md);
				}
			}
			else if (v < 2 * _mint + 28) {
				_mdir = md;
				M_StartAttack();
			}
		}
	}
	if (_mmode == MonsterMode::STAND)
		_mAnimData = MType->Anims[MonstAnim::STAND].Data[int(_mdir)];
}

void Monster::ai_Counselor()
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
	}
	else if (_mgoal == MonsterGoal::MOVE) {
		if ((dist >= 2) && _msquelch == UINT8_MAX && grid.at(_m).dTransVal == grid.at(f).dTransVal) {
			if (_mgoalvar1++ < 2 * dist || !DirOK(md)) {
				M_RoundWalk(md, &_mgoalvar2);
			}
			else {
				_mgoal = MonsterGoal::NORMAL;
				M_StartFadein(md, true);
			}
		}
		else {
			_mgoal = MonsterGoal::NORMAL;
			M_StartFadein(md, true);
		}
	}
	else if (_mgoal == MonsterGoal::NORMAL) {
		if (dist >= 2) {
			if (v < 5 * (_mint + 10) && LineClear(_m, f)) {
				M_StartRAttack(counsmiss[_mint], mMinDamage + random_(77, mMaxDamage - mMinDamage + 1));
			}
			else if (random_(124, 100) < 30) {
				_mgoal = MonsterGoal::MOVE;
				_mgoalvar1 = 0;
				M_StartFadeout(md, false);
			}
			else
				M_StartDelay(random_(105, 10) + 2 * (5 - _mint));
		}
		else {
			_mdir = md;
			if (_mhitpoints < (_mmaxhp >> 1)) {
				_mgoal = MonsterGoal::RETREAT;
				_mgoalvar1 = 0;
				M_StartFadeout(md, false);
			}
			else if (_mVar1_mmode == MonsterMode::DELAY
				|| random_(105, 100) < 2 * _mint + 20) {
				M_StartRAttack(-1, 0);
				AddMissile(_m, { 0, 0 }, _mdir, MissileType::FLASH, 1, i, 4, 0);
				AddMissile(_m, { 0, 0 }, _mdir, MissileType::FLASH2, 1, i, 4, 0);
			}
			else
				M_StartDelay(random_(105, 10) + 2 * (5 - _mint));
		}
	}
	if (_mmode == MonsterMode::STAND) {
		M_StartDelay(random_(125, 10) + 5);
	}
}

void Monster::ai_Garbud()
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
		ai_Round(true);

	_mdir = md;

	if (_mmode == MonsterMode::STAND)
		_mAnimData = MType->Anims[MonstAnim::STAND].Data[int(md)];
}

void Monster::ai_Zhar()
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
		ai_Counselor();

	_mdir = md;

	if (_mmode == MonsterMode::STAND)
		_mAnimData = MType->Anims[MonstAnim::STAND].Data[int(md)];
}

void Monster::ai_SnotSpil()
{
	if (_mmode != MonsterMode::STAND) return;

	V2Di m = _m;
	Dir md = M_GetDir();

	if (mtalkmsg == TEXT_BANNER10 && !(grid.at(m).dFlags & DunTileFlag::VISIBLE) && _mgoal == MonsterGoal::TALKING) {
		mtalkmsg = TEXT_BANNER11;
		_mgoal = MonsterGoal::INQUIRING;
	}

	if (mtalkmsg == TEXT_BANNER11 && quests[QuestId::ltbanner]._qvar1 == 3) {
		mtalkmsg = 0;
		_mgoal = MonsterGoal::NORMAL;
	}

	if (grid.at(m).dFlags & DunTileFlag::VISIBLE) {
		if (mtalkmsg == TEXT_BANNER12) {
			if (!effect_is_playing(USFX_SNOT3) && _mgoal == MonsterGoal::TALKING) {
				ObjChangeMap({ lvl.getpc().x, lvl.getpc().y }, { lvl.getpc().x + lvl.getpc().w + 1, lvl.getpc().y + lvl.getpc().h + 1 });
				quests[QuestId::ltbanner]._qvar1 = 3;
				RedoPlayerVision();
				_msquelch = UINT8_MAX;
				mtalkmsg = 0;
				_mgoal = MonsterGoal::NORMAL;
			}
		}
		if (quests[QuestId::ltbanner]._qvar1 == 3) {
			if (_mgoal == MonsterGoal::NORMAL || _mgoal == MonsterGoal::SHOOT)
				ai_Fallen();
		}
	}

	_mdir = md;

	if (_mmode == MonsterMode::STAND)
		_mAnimData = MType->Anims[MonstAnim::STAND].Data[int(md)];
}

void Monster::ai_Lazurus()
{
	if (_mmode != MonsterMode::STAND)
		return;

	V2Di m = _m;
	Dir md = M_GetDir();
	if (grid.at(m).dFlags & DunTileFlag::VISIBLE) {
		if (game.isSingleplayer()) {
			if (mtalkmsg == TEXT_VILE13 && _mgoal == MonsterGoal::INQUIRING && myplr().pos().x == TEXT_VILE13 && myplr().pos().y == 46) {
				PlayInGameMovie("gendata\\fprst3.smk");
				_mmode = MonsterMode::TALK;
				quests[QuestId::betrayer]._qvar1 = 5;
			}

			if (mtalkmsg == TEXT_VILE13 && !effect_is_playing(USFX_LAZ1) && _mgoal == MonsterGoal::TALKING) {
				ObjChangeMapResync({ 1, 18 }, { 20, 24 });
				RedoPlayerVision();
				_msquelch = UINT8_MAX;
				mtalkmsg = 0;
				quests[QuestId::betrayer]._qvar1 = 6;
				_mgoal = MonsterGoal::NORMAL;
			}
		}

		if (game.isMultiplayer() && mtalkmsg == TEXT_VILE13 && _mgoal == MonsterGoal::INQUIRING && quests[QuestId::betrayer]._qvar1 <= 3) {
			_mmode = MonsterMode::TALK;
		}
	}

	if (_mgoal == MonsterGoal::NORMAL || _mgoal == MonsterGoal::RETREAT || _mgoal == MonsterGoal::MOVE) {
		mtalkmsg = 0;
		ai_Counselor();
	}

	_mdir = md;
	if (_mmode == MonsterMode::STAND || _mmode == MonsterMode::TALK)
		_mAnimData = MType->Anims[MonstAnim::STAND].Data[int(md)];
}

void Monster::ai_Lazhelp()
{
	if (_mmode != MonsterMode::STAND)
		return;

	V2Di _m = _m;
	Dir md = M_GetDir();

	if (grid.at(_m).dFlags & DunTileFlag::VISIBLE) {
		if (game.isSingleplayer()) {
			if (quests[QuestId::betrayer]._qvar1 <= 5) {
				_mgoal = MonsterGoal::INQUIRING;
			}
			else {
				mtalkmsg = 0;
				_mgoal = MonsterGoal::NORMAL;
			}
		}
		else
			_mgoal = MonsterGoal::NORMAL;
	}
	if (_mgoal == MonsterGoal::NORMAL)
		ai_Succ();
	_mdir = md;
	if (_mmode == MonsterMode::STAND)
		_mAnimData = MType->Anims[MonstAnim::STAND].Data[int(md)];
}

void Monster::ai_Lachdanan()
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
				quests[QuestId::veil]._qactive = QuestState::done;
				startKillByPlayer(-1);
			}
		}
	}

	_mdir = md;

	if (_mmode == MonsterMode::STAND) _mAnimData = MType->Anims[MonstAnim::STAND].Data[int(md)];
}

void Monster::ai_Warlord()
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
		ai_SkelSd();

	_mdir = md;

	if (_mmode == MonsterMode::STAND || _mmode == MonsterMode::TALK)
		_mAnimData = MType->Anims[MonstAnim::STAND].Data[int(_mdir)];
}


}
