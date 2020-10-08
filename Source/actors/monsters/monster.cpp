/**
 * @file monster.cpp
 *
 * Implementation of monster functionality, AI, actions, spawning, loading, etc.
 */
#include "all.h"
#include "../3rdParty/Storm/Source/storm.h"

namespace dvl {

int MissileFileFlag; /** Tracks which missile files are already loaded */
bool sgbSaveSoundOn;
int uniquetrans;
const char plr2monst[9] = { 0, 5, 3, 7, 1, 4, 6, 0, 2 };
const MissileType counsmiss[4] = { MissileType::FIREBOLT, MissileType::CBOLT, MissileType::LIGHTCTRL, MissileType::FIREBALL };

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

void (Monster:: *AiProc[])() = {
	&Monster::ai_Zombie,
	&Monster::ai_Fat,
	&Monster::ai_SkelSd,
	&Monster::ai_SkelBow,
	&Monster::ai_Scav,
	&Monster::ai_Rhino,
	&Monster::ai_GoatMc,
	&Monster::ai_GoatBow,
	&Monster::ai_Fallen,
	&Monster::ai_Magma,
	&Monster::ai_SkelKing,
	&Monster::ai_Bat,
	&Monster::ai_Garg,
	&Monster::ai_Cleaver,
	&Monster::ai_Succ,
	&Monster::ai_Sneak,
	&Monster::ai_Storm,
	&Monster::ai_Fireman,
	&Monster::ai_Garbud,
	&Monster::ai_Acid,
	&Monster::ai_AcidUniq,
	&Monster::ai_Golum,
	&Monster::ai_Zhar,
	&Monster::ai_SnotSpil,
	&Monster::ai_Snake,
	&Monster::ai_Counselor,
	&Monster::ai_Mega,
	&Monster::ai_Diablo,
	&Monster::ai_Lazurus,
	&Monster::ai_Lazhelp,
	&Monster::ai_Lachdanan,
	&Monster::ai_Warlord,
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

void M2MStartHit(int mid, int i, int dam)
{
	if ((DWORD)mid >= MAXMONSTERS) app_fatal("Invalid monster %d getting hit by monster", mid);
	if (monsters[mid].data.MType == NULL) app_fatal("Monster %d \"%s\" getting hit by monster: MType NULL", mid, monsters[mid].data.mName);
	if (i >= 0) monsters[i].data.mWhoHit |= 1 << i;

	delta_monster_hp(mid, monsters[mid].data._mhitpoints, lvl.currlevel);
	NetSendCmdParam2(false, Cmd::MONSTDAMAGE, mid, dam);
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

void PrintUniqueHistory()
{
	int res = monsters[pcursmonst].data.mMagicRes & (MonsterResistFlag::resist_magic | MonsterResistFlag::resist_fire | MonsterResistFlag::resist_lightning | MonsterResistFlag::imune_magic | MonsterResistFlag::imune_fire | MonsterResistFlag::imune_lightning);
	if (!res) {
		strcpy(tempstr, "No resistances");
		AddPanelString(tempstr, true);
		strcpy(tempstr, "No Immunities");
	} else {
		if (res & (MonsterResistFlag::resist_magic | MonsterResistFlag::resist_fire | MonsterResistFlag::resist_lightning))
			strcpy(tempstr, "Some Magic Resistances");
		else
			strcpy(tempstr, "No resistances");
		AddPanelString(tempstr, true);
		if (res & (MonsterResistFlag::imune_magic | MonsterResistFlag::imune_fire | MonsterResistFlag::imune_lightning)) {
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
			monsters[m].startHitByPlayer(-1, 0);
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

}
