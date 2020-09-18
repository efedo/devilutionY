/**
 * @file missiles.cpp
 *
 * Implementation of missile functionality.
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

int missileactive[MAXMISSILES];
int missileavail[MAXMISSILES];
MissileStruct missile[MAXMISSILES];
ChainStruct chain[MAXMISSILES];
int nummissiles;
BOOL ManashieldFlag;
BOOL MissilePreFlag;
int numchains;

/** Maps from direction to X-offset. */
int XDirAdd[8] = { 1, 0, -1, -1, -1, 0, 1, 1 };
/** Maps from direction to Y-offset. */
int YDirAdd[8] = { 1, 1, 1, 0, -1, -1, -1, 0 };

void GetDamageAmt(int i, int *mind, int *maxd)
{
	int k, sl;

	assert((DWORD)myplr() < MAX_PLRS);
	assert((DWORD)i < 64);
	sl = myplr().data._pSplLvl[i] + myplr().data._pISplLvlAdd;

	switch (i) {
	case SPL_FIREBOLT:
		*mind = (myplr().data._pMagic >> 3) + sl + 1;
		*maxd = (myplr().data._pMagic >> 3) + sl + 10;
		break;
	case SPL_HEAL: /// BUGFIX: healing calculation is unused
		*mind = myplr().data._pLevel + sl + 1;
		if (myplr().data._pClass == PC_WARRIOR) {
			*mind <<= 1;
		}
		if (myplr().data._pClass == PC_ROGUE) {
			*mind += *mind >> 1;
		}
		*maxd = 10;
		for (k = 0; k < myplr().data._pLevel; k++) {
			*maxd += 4;
		}
		for (k = 0; k < sl; k++) {
			*maxd += 6;
		}
		if (myplr().data._pClass == PC_WARRIOR) {
			*maxd <<= 1;
		}
		if (myplr().data._pClass == PC_ROGUE) {
			*maxd += *maxd >> 1;
		}
		*mind = -1;
		*maxd = -1;
		break;
	case SPL_LIGHTNING:
		*mind = 2;
		*maxd = myplr().data._pLevel + 2;
		break;
	case SPL_FLASH:
		*mind = myplr().data._pLevel;
		for (k = 0; k < sl; k++) {
			*mind += *mind >> 3;
		}
		*mind += *mind >> 1;
		*maxd = *mind * 2;
		break;
	case SPL_IDENTIFY:
	case SPL_TOWN:
	case SPL_STONE:
	case SPL_INFRA:
	case SPL_RNDTELEPORT:
	case SPL_MANASHIELD:
	case SPL_DOOMSERP:
	case SPL_BLODRIT:
	case SPL_INVISIBIL:
	case SPL_BLODBOIL:
	case SPL_TELEPORT:
	case SPL_ETHEREALIZE:
	case SPL_REPAIR:
	case SPL_RECHARGE:
	case SPL_DISARM:
	case SPL_RESURRECT:
	case SPL_TELEKINESIS:
	case SPL_BONESPIRIT:
		*mind = -1;
		*maxd = -1;
		break;
	case SPL_FIREWALL:
		*mind = (4 * myplr().data._pLevel + 8) >> 1;
		*maxd = (4 * myplr().data._pLevel + 80) >> 1;
		break;
	case SPL_FIREBALL:
		*mind = 2 * myplr().data._pLevel + 4;
		for (k = 0; k < sl; k++) {
			*mind += *mind >> 3;
		}
		*maxd = 2 * myplr().data._pLevel + 40;
		for (k = 0; k < sl; k++) {
			*maxd += *maxd >> 3;
		}
		break;
	case SPL_GUARDIAN:
		*mind = (myplr().data._pLevel >> 1) + 1;
		for (k = 0; k < sl; k++) {
			*mind += *mind >> 3;
		}
		*maxd = (myplr().data._pLevel >> 1) + 10;
		for (k = 0; k < sl; k++) {
			*maxd += *maxd >> 3;
		}
		break;
	case SPL_CHAIN:
		*mind = 4;
		*maxd = 2 * myplr().data._pLevel + 4;
		break;
	case SPL_WAVE:
		*mind = 6 * (myplr().data._pLevel + 1);
		*maxd = 6 * (myplr().data._pLevel + 10);
		break;
	case SPL_NOVA:
		*mind = (myplr().data._pLevel + 5) >> 1;
		for (k = 0; k < sl; k++) {
			*mind += *mind >> 3;
		}
		*mind *= 5;
		*maxd = (myplr().data._pLevel + 30) >> 1;
		for (k = 0; k < sl; k++) {
			*maxd += *maxd >> 3;
		}
		*maxd *= 5;
		break;
	case SPL_FLAME:
		*mind = 3;
		*maxd = myplr().data._pLevel + 4;
		*maxd += *maxd >> 1;
		break;
	case SPL_GOLEM:
		*mind = 11;
		*maxd = 17;
		break;
	case SPL_APOCA:
		*mind = 0;
		for (k = 0; k < myplr().data._pLevel; k++) {
			*mind += 1;
		}
		*maxd = 0;
		for (k = 0; k < myplr().data._pLevel; k++) {
			*maxd += 6;
		}
		break;
	case SPL_ELEMENT:
		*mind = 2 * myplr().data._pLevel + 4;
		for (k = 0; k < sl; k++) {
			*mind += *mind >> 3;
		}
		/// BUGFIX: add here '*mind >>= 1;'
		*maxd = 2 * myplr().data._pLevel + 40;
		for (k = 0; k < sl; k++) {
			*maxd += *maxd >> 3;
		}
		/// BUGFIX: add here '*maxd >>= 1;'
		break;
	case SPL_CBOLT:
		*mind = 1;
		*maxd = (myplr().data._pMagic >> 2) + 1;
		break;
	case SPL_HBOLT:
		*mind = myplr().data._pLevel + 9;
		*maxd = myplr().data._pLevel + 18;
		break;
	case SPL_HEALOTHER: /// BUGFIX: healing calculation is unused
		*mind = myplr().data._pLevel + sl + 1;
		if (myplr().data._pClass == PC_WARRIOR) {
			*mind <<= 1;
		}
		if (myplr().data._pClass == PC_ROGUE) {
			*mind += *mind >> 1;
		}
		*maxd = 10;
		for (k = 0; k < myplr().data._pLevel; k++) {
			*maxd += 4;
		}
		for (k = 0; k < sl; k++) {
			*maxd += 6;
		}
		if (myplr().data._pClass == PC_WARRIOR) {
			*maxd <<= 1;
		}
		if (myplr().data._pClass == PC_ROGUE) {
			*maxd += *maxd >> 1;
		}
		*mind = -1;
		*maxd = -1;
		break;
	case SPL_FLARE:
		*mind = (myplr().data._pMagic >> 1) + 3 * sl - (myplr().data._pMagic >> 3);
		*maxd = *mind;
		break;
	}
}

BOOL CheckBlock(V2Di f, V2Di t)
{
	int pn;
	BOOL coll;

	coll = FALSE;
	while (f.x != t.x || f.y != t.y) {
		pn = GetDirection(f, t);
		f.x += XDirAdd[pn];
		f.y += YDirAdd[pn];
		if (pieces[grid.at(f).dPiece].nSolidTable)
			coll = TRUE;
	}

	return coll;
}

int FindClosest(V2Di s, int rad)
{
	V2Di t;
	int j, i, mid, cr;
	int CrawlNum[19] = { 0, 3, 12, 45, 94, 159, 240, 337, 450, 579, 724, 885, 1062, 1255, 1464, 1689, 1930, 2187, 2460 };

	if (rad > 19)
		rad = 19;

	for (i = 1; i < rad; i++) {
		cr = CrawlNum[i] + 2;
		for (j = (BYTE)CrawlTable[CrawlNum[i]]; j > 0; j--) {
			t.x = s.x + CrawlTable[cr - 1];
			t.y = s.y + CrawlTable[cr];
			if (t.x > 0 && t.x < MAXDUNX && t.y > 0 && t.y < MAXDUNY) {
				mid = grid.at(t).dMonster;
				if (mid > 0 && !CheckBlock(s, t))
					return mid - 1;
			}
			cr += 2;
		}
	}
	return -1;
}

int GetSpellLevel(int id, int sn)
{
	int result;

	if (id == myplr())
		result = plr[id].data._pISplLvlAdd + plr[id].data._pSplLvl[sn];
	else
		result = 1;

	if (result < 0)
		result = 0;

	return result;
}

int GetDirection8(V2Di t1, V2Di t2)
{
	BYTE Dirs[16][16] = {
		{ 99, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 2, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 2, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 },
		{ 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0 },
		{ 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
		{ 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
	};
	int mx, my, md;
	ALIGN_BY_1 BYTE urtoll[] = { 3, 4, 5 },
	                ultolr[] = { 3, 2, 1 },
	                lrtoul[] = { 7, 6, 5 },
	                lltour[] = { 7, 0, 1 };

	mx = abs(t2.x - t1.x);
	if (mx > 15)
		mx = 15;
	my = abs(t2.y - t1.y);
	if (my > 15)
		my = 15;
	md = Dirs[my][mx];
	if (t1.x > t2.x) {
		if (t1.y > t2.y)
			md = urtoll[md];
		else
			md = ultolr[md];
	} else if (t1.y > t2.y)
		md = lrtoul[md];
	else
		md = lltour[md];
	return md;
}

int GetDirection16(V2Di t1, V2Di t2)
{
	BYTE Dirs[16][16] = {
		{ 99, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 4, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 4, 3, 2, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 4, 3, 3, 2, 2, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0 },
		{ 4, 4, 3, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 4, 4, 3, 3, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 4, 4, 3, 3, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1 },
		{ 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1 },
		{ 4, 4, 4, 3, 3, 3, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1 },
		{ 4, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 1, 1, 1, 1 },
		{ 4, 4, 4, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 1, 1 },
		{ 4, 4, 4, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 1 },
		{ 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2 },
		{ 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2 },
		{ 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2 },
		{ 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2 }
	};
	BYTE urtoll[5] = { 6, 7, 8, 9, 10 };
	BYTE ultolr[5] = { 6, 5, 4, 3, 2 };
	BYTE lltour[5] = { 14, 13, 12, 11, 10 };
	BYTE lrtoul[5] = { 14, 15, 0, 1, 2 };
	int mx, my, md;

	mx = abs(t2.x - t1.x);
	if (mx > 15)
		mx = 15;
	my = abs(t2.y - t1.y);
	if (my > 15)
		my = 15;
	md = Dirs[my][mx];
	if (t1.x > t2.x) {
		if (t1.y > t2.y)
			md = urtoll[md];
		else
			md = ultolr[md];
	} else if (t1.y > t2.y) {
		md = lltour[md];
	} else {
		md = lrtoul[md];
	}
	return md;
}

void DeleteMissile(int mi, int i)
{
	int src;

	if (missile[mi]._mitype == MIS_MANASHIELD) {
		src = missile[mi]._misource;
		if (src == myplr())
			NetSendCmd(TRUE, CMD_REMSHIELD);
		plr[src].data.pManaShield = FALSE;
	}
	missileavail[MAXMISSILES - nummissiles] = mi;
	nummissiles--;
	if (nummissiles > 0 && i != nummissiles)
		missileactive[i] = missileactive[nummissiles];
}

void GetMissileVel(int i, V2Di s, V2Di d, int v)
{
	double dr;
	V2Di dp;

	if (d != s) {
		dp.x = (d.x + s.y - s.x - d.y) * (1 << 21);
		dp.y = (d.y + d.x - s.x - s.y) * (1 << 21);
		dr = sqrt(dp.x * dp.x + dp.y * dp.y);
		missile[i]._mivel.x = (dp.x * (v << 16)) / dr;
		missile[i]._mivel.y = (dp.y * (v << 15)) / dr;
	} else {
		missile[i]._mivel.x = 0;
		missile[i]._mivel.y = 0;
	}
}

void PutMissile(int i)
{
	V2Di pos = missile[i]._mi;
	if (pos.x <= 0 || pos.y <= 0 || pos.x >= MAXDUNX || pos.y >= MAXDUNY)
		missile[i]._miDelFlag = TRUE;
	if (!missile[i]._miDelFlag) {
		grid.at(pos).dFlags |= BFLAG_MISSILE;
		if (grid.at(pos).dMissile == 0)
			grid.at(pos).dMissile = i + 1;
		else
			grid.at(pos).dMissile = -1;
		if (missile[i]._miPreFlag)
			MissilePreFlag = TRUE;
	}
}

void GetMissilePos(int i)
{
	int mx, my, dx, dy, lx, ly;

	mx = missile[i]._mitoff.x >> 16;
	my = missile[i]._mitoff.y >> 16;
	dx = mx + 2 * my;
	dy = 2 * my - mx;
	if (dx < 0) {
		lx = -(-dx / 8);
		dx = -(-dx / 64);
	} else {
		lx = dx / 8;
		dx = dx / 64;
	}
	if (dy < 0) {
		ly = -(-dy / 8);
		dy = -(-dy / 64);
	} else {
		ly = dy / 8;
		dy = dy / 64;
	}
	missile[i]._mi.x = dx + missile[i]._mis.x;
	missile[i]._mi.y = dy + missile[i]._mis.y;
	missile[i]._mioff.x = mx + (dy * 32) - (dx * 32);
	missile[i]._mioff.y = my - (dx * 16) - (dy * 16);
	ChangeLightOff(missile[i]._mlid, { lx - (dx * 8), ly - (dy * 8) });
}

void MoveMissilePos(int i)
{
	V2Di pos, d;

	switch (missile[i]._mimfnum) {
	case DIR_S:
		d = { 1, 1 };
		break;
	case DIR_SW:
		d = { 1, 1 };
		break;
	case DIR_W:
		d = { 0, 1 };
		break;
	case DIR_NW:
		d = { 0, 0 };
		break;
	case DIR_N:
		d = { 0, 0 };
		break;
	case DIR_NE:
		d = { 0, 0 };
		break;
	case DIR_E:
		d = { 1, 0 };
		break;
	case DIR_SE:
		d = { 1, 1 };
		break;
	}

	pos = missile[i]._mi + d;
	if (PosOkMonst(missile[i]._misource, pos)) {
		missile[i]._mi += d;
		missile[i]._mioff.x += (d.y << 5) - (d.x << 5);
		missile[i]._mioff.y -= (d.y << 4) + (d.x << 4);
	}
}

BOOL MonsterTrapHit(int m, int mindam, int maxdam, int dist, int t, BOOLEAN shift)
{
	int hit, hper, dam, mor, mir;
	BOOL resist, ret;

	resist = FALSE;
	if (monsters[m].data.mtalkmsg) {
		return FALSE;
	}
	if (monsters[m].data._mhitpoints >> 6 <= 0) {
		return FALSE;
	}
	if (monsters[m].data.MType->mtype == MT_ILLWEAV && monsters[m].data._mgoal == MGOAL_RETREAT)
		return FALSE;
	if (monsters[m].data._mmode == MM_CHARGE)
		return FALSE;

	mir = missiledata[t].mResist;
	mor = monsters[m].data.mMagicRes;
	if (mor & IMUNE_MAGIC && mir == MISR_MAGIC
	    || mor & IMUNE_FIRE && mir == MISR_FIRE
	    || mor & IMUNE_LIGHTNING && mir == MISR_LIGHTNING) {
		return FALSE;
	}

	if ((mor & RESIST_MAGIC && mir == MISR_MAGIC)
	    || (mor & RESIST_FIRE && mir == MISR_FIRE)
	    || (mor & RESIST_LIGHTNING && mir == MISR_LIGHTNING)) {
		resist = TRUE;
	}

	hit = random_(68, 100);
	hper = 90 - (BYTE)monsters[m].data.mArmorClass - dist;
	if (hper < 5)
		hper = 5;
	if (hper > 95)
		hper = 95;
	if (CheckMonsterHit(m, &ret)) {
		return ret;
	}
#ifdef _DEBUG
	else if (hit < hper || debug_mode_dollar_sign || debug_mode_key_inverted_v || monsters[m].data._mmode == MM_STONE) {
#else
	else if (hit < hper || monsters[m].data._mmode == MM_STONE) {
#endif
		dam = mindam + random_(68, maxdam - mindam + 1);
		if (!shift)
			dam <<= 6;
		if (resist)
			monsters[m].data._mhitpoints -= dam >> 2;
		else
			monsters[m].data._mhitpoints -= dam;
#ifdef _DEBUG
		if (debug_mode_dollar_sign || debug_mode_key_inverted_v)
			monsters[m].data._mhitpoints = 0;
#endif
		if (monsters[m].data._mhitpoints >> 6 <= 0) {
			if (monsters[m].data._mmode == MM_STONE) {
				monsters[m].M_StartKill(-1);
				monsters[m].data._mmode = MM_STONE;
			} else {
				monsters[m].M_StartKill(-1);
			}
		} else {
			if (resist) {
				PlayEffect(m, 1);
			} else if (monsters[m].data._mmode == MM_STONE) {
				if (m > 3)
					monsters[m].M_StartHit(-1, dam);
				monsters[m].data._mmode = MM_STONE;
			} else {
				if (m > 3)
					monsters[m].M_StartHit(-1, dam);
			}
		}
		return TRUE;
	} else {
		return FALSE;
	}
}

BOOL MonsterMHit(int pnum, int m, int mindam, int maxdam, int dist, int t, BOOLEAN shift)
{
	int hit, hper, dam, mor, mir;
	BOOL resist, ret;

	resist = FALSE;
	if (monsters[m].data.mtalkmsg
	    || monsters[m].data._mhitpoints >> 6 <= 0
	    || t == MIS_HBOLT && monsters[m].data.MType->mtype != MT_DIABLO && monsters[m].data.MData->mMonstClass) {
		return FALSE;
	}
	if (monsters[m].data.MType->mtype == MT_ILLWEAV && monsters[m].data._mgoal == MGOAL_RETREAT)
		return FALSE;
	if (monsters[m].data._mmode == MM_CHARGE)
		return FALSE;

	mor = monsters[m].data.mMagicRes;
	mir = missiledata[t].mResist;

	if (mor & IMUNE_MAGIC && mir == MISR_MAGIC
	    || mor & IMUNE_FIRE && mir == MISR_FIRE
	    || mor & IMUNE_LIGHTNING && mir == MISR_LIGHTNING
	    || (mor & IMUNE_ACID) && mir == MISR_ACID)
		return FALSE;

	if (mor & RESIST_MAGIC && mir == MISR_MAGIC
	    || mor & RESIST_FIRE && mir == MISR_FIRE
	    || mor & RESIST_LIGHTNING && mir == MISR_LIGHTNING)
		resist = TRUE;

	hit = random_(69, 100);
	if (!missiledata[t].mType) {
		hper = plr[pnum].data._pDexterity
		    + plr[pnum].data._pIBonusToHit
		    + plr[pnum].data._pLevel
		    - monsters[m].data.mArmorClass
		    - (dist * dist >> 1)
		    + plr[pnum].data._pIEnAc
		    + 50;
		if (plr[pnum].data._pClass == PC_ROGUE)
			hper += 20;
		if (plr[pnum].data._pClass == PC_WARRIOR)
			hper += 10;
	} else {
		hper = plr[pnum].data._pMagic - (monsters[m].data.mLevel << 1) - dist + 50;
		if (plr[pnum].data._pClass == PC_SORCERER)
			hper += 20;
	}
	if (hper < 5)
		hper = 5;
	if (hper > 95)
		hper = 95;
	if (monsters[m].data._mmode == MM_STONE)
		hit = 0;
	if (CheckMonsterHit(m, &ret))
		return ret;
#ifdef _DEBUG
	if (hit >= hper && !debug_mode_key_inverted_v && !debug_mode_dollar_sign)
		return FALSE;
#else
	if (hit >= hper)
		return FALSE;
#endif
	if (t == MIS_BONESPIRIT) {
		dam = monsters[m].data._mhitpoints / 3 >> 6;
	} else {
		dam = mindam + random_(70, maxdam - mindam + 1);
	}
	if (!missiledata[t].mType) {
		dam = plr[pnum].data._pIBonusDamMod + dam * plr[pnum].data._pIBonusDam / 100 + dam;
		if (plr[pnum].data._pClass == PC_ROGUE)
			dam += plr[pnum].data._pDamageMod;
		else
			dam += (plr[pnum].data._pDamageMod >> 1);
	}
	if (!shift)
		dam <<= 6;
	if (resist)
		dam >>= 2;
	if (pnum == myplr())
		monsters[m].data._mhitpoints -= dam;
	if (plr[pnum].data._pIFlags & ISPL_FIRE_ARROWS)
		monsters[m].data._mFlags |= MFLAG_NOHEAL;

	if (monsters[m].data._mhitpoints >> 6 <= 0) {
		if (monsters[m].data._mmode == MM_STONE) {
			monsters[m].M_StartKill(pnum);
			monsters[m].data._mmode = MM_STONE;
		} else {
			monsters[m].M_StartKill(pnum);
		}
	} else {
		if (resist) {
			PlayEffect(m, 1);
		} else if (monsters[m].data._mmode == MM_STONE) {
			if (m > 3)
				monsters[m].M_StartHit(pnum, dam);
			monsters[m].data._mmode = MM_STONE;
		} else {
			if (!missiledata[t].mType && plr[pnum].data._pIFlags & ISPL_KNOCKBACK) {
				monsters[m].M_GetKnockback();
			}
			if (m > 3)
				monsters[m].M_StartHit(pnum, dam);
		}
	}

	if (!monsters[m].data._msquelch) {
		monsters[m].data._msquelch = UCHAR_MAX;
		monsters[m].data._last = plr[pnum].data._p;
	}
	return TRUE;
}

BOOL PlayerMHit(int pnum, int m, int dist, int mind, int maxd, int mtype, BOOLEAN shift, int earflag)
{
	int hit, hper, tac, dam, blk, blkper, resper;

	if (plr[pnum].data._pHitPoints >> 6 <= 0) {
		return FALSE;
	}

	if (plr[pnum].data._pInvincible) {
		return FALSE;
	}

	if (plr[pnum].data._pSpellFlags & 1 && !missiledata[mtype].mType) {
		return FALSE;
	}

	hit = random_(72, 100);
#ifdef _DEBUG
	if (debug_mode_dollar_sign || debug_mode_key_inverted_v)
		hit = 1000;
#endif
	if (!missiledata[mtype].mType) {
		tac = plr[pnum].data._pIAC + plr[pnum].data._pIBonusAC + plr[pnum].data._pDexterity / 5;
		if (m != -1) {
			hper = monsters[m].data.mHit
			    + ((monsters[m].data.mLevel - plr[pnum].data._pLevel) * 2)
			    + 30
			    - (dist << 1) - tac;
		} else {
			hper = 100 - (tac >> 1) - (dist << 1);
		}
	} else {
		if (m != -1) {
			hper = +40 - (plr[pnum].data._pLevel << 1) - (dist << 1) + (monsters[m].data.mLevel << 1);
		} else {
			hper = 40;
		}
	}

	if (hper < 10)
		hper = 10;
	if (level.currlevel == 14 && hper < 20) {
		hper = 20;
	}
	if (level.currlevel == 15 && hper < 25) {
		hper = 25;
	}
	if (level.currlevel == 16 && hper < 30) {
		hper = 30;
	}

	if ((plr[pnum].data._pmode == PM_STAND || plr[pnum].data._pmode == PM_ATTACK) && plr[pnum].data._pBlockFlag) {
		blk = random_(73, 100);
	} else {
		blk = 100;
	}

	if (shift == TRUE)
		blk = 100;
	if (mtype == MIS_ACIDPUD)
		blk = 100;
	if (m != -1)
		blkper = plr[pnum].data._pBaseToBlk + (plr[pnum].data._pLevel << 1) - (monsters[m].data.mLevel << 1) + plr[pnum].data._pDexterity;
	else
		blkper = plr[pnum].data._pBaseToBlk + plr[pnum].data._pDexterity;
	if (blkper < 0)
		blkper = 0;
	if (blkper > 100)
		blkper = 100;

	switch (missiledata[mtype].mResist) {
	case MISR_FIRE:
		resper = plr[pnum].data._pFireResist;
		break;
	case MISR_LIGHTNING:
		resper = plr[pnum].data._pLghtResist;
		break;
	case MISR_MAGIC:
	case MISR_ACID:
		resper = plr[pnum].data._pMagResist;
		break;
	default:
		resper = 0;
		break;
	}

	if (hit < hper) {
		if (mtype == MIS_BONESPIRIT) {
			dam = plr[pnum].data._pHitPoints / 3;
		} else {
			if (shift == FALSE) {

				dam = (mind << 6) + random_(75, (maxd - mind + 1) << 6);
				if (m == -1 && plr[pnum].data._pIFlags & ISPL_ABSHALFTRAP)
					dam >>= 1;
				dam += (plr[pnum].data._pIGetHit * 64);
			} else {
				dam = mind + random_(75, maxd - mind + 1);
				if (m == -1 && plr[pnum].data._pIFlags & ISPL_ABSHALFTRAP)
					dam >>= 1;
				dam += plr[pnum].data._pIGetHit;
			}

			if (dam < 64)
				dam = 64;
		}
		if (resper > 0) {

			dam = dam - dam * resper / 100;
			if (pnum == myplr()) {
				plr[pnum].data._pHitPoints -= dam;
				plr[pnum].data._pHPBase -= dam;
			}
			if (plr[pnum].data._pHitPoints > plr[pnum].data._pMaxHP) {
				plr[pnum].data._pHitPoints = plr[pnum].data._pMaxHP;
				plr[pnum].data._pHPBase = plr[pnum].data._pMaxHPBase;
			}

			if (plr[pnum].data._pHitPoints >> 6 <= 0) {
				plr[pnum].SyncPlrKill(earflag);
			} else {
				if (plr[pnum].data._pClass == PC_WARRIOR) {
					PlaySfxLoc(PS_WARR69, plr[pnum].data._p);
#ifndef SPAWN
				} else if (plr[pnum].data._pClass == PC_ROGUE) {
					PlaySfxLoc(PS_ROGUE69, plr[pnum].data._p);
				} else if (plr[pnum].data._pClass == PC_SORCERER) {
					PlaySfxLoc(PS_MAGE69, plr[pnum].data._p);
#endif
				}
				drawhpflag = TRUE;
			}
		} else {
			if (blk < blkper) {
				if (m != -1) {
					tac = GetDirection(plr[pnum].data._p, monsters[m].data._m);
				} else {
					tac = plr[pnum].data._pdir;
				}
				plr[pnum].StartPlrBlock(tac);
			} else {
				if (pnum == myplr()) {
					plr[pnum].data._pHitPoints -= dam;
					plr[pnum].data._pHPBase -= dam;
				}
				if (plr[pnum].data._pHitPoints > plr[pnum].data._pMaxHP) {
					plr[pnum].data._pHitPoints = plr[pnum].data._pMaxHP;
					plr[pnum].data._pHPBase = plr[pnum].data._pMaxHPBase;
				}
				if (plr[pnum].data._pHitPoints >> 6 <= 0) {
					plr[pnum].SyncPlrKill(earflag);
				} else {
					plr[pnum].StartPlrHit(dam, FALSE);
				}
			}
		}
		return TRUE;
	}
	return FALSE;
}

BOOL Plr2PlrMHit(int pnum, int p, int mindam, int maxdam, int dist, int mtype, BOOLEAN shift)
{
	int tac, resper, dam, blk, blkper, hper, hit;

	if (plr[p].data._pInvincible) {
		return FALSE;
	}

	if (mtype == MIS_HBOLT) {
		return FALSE;
	}

	if (plr[p].data._pSpellFlags & 1 && !missiledata[mtype].mType) {
		return FALSE;
	}

	switch (missiledata[mtype].mResist) {
	case MISR_FIRE:
		resper = plr[p].data._pFireResist;
		break;
	case MISR_LIGHTNING:
		resper = plr[p].data._pLghtResist;
		break;
	case MISR_MAGIC:
	case MISR_ACID:
		resper = plr[p].data._pMagResist;
		break;
	default:
		resper = 0;
		break;
	}
	hper = random_(69, 100);
	if (!missiledata[mtype].mType) {
		hit = plr[pnum].data._pIBonusToHit
		    + plr[pnum].data._pLevel
		    - (dist * dist >> 1)
		    - plr[p].data._pDexterity / 5
		    - plr[p].data._pIBonusAC
		    - plr[p].data._pIAC
		    + plr[pnum].data._pDexterity + 50;
		if (plr[pnum].data._pClass == PC_ROGUE)
			hit += 20;
		if (plr[pnum].data._pClass == PC_WARRIOR)
			hit += 10;
	} else {
		hit = plr[pnum].data._pMagic
		    - (plr[p].data._pLevel << 1)
		    - dist
		    + 50;
		if (plr[pnum].data._pClass == PC_SORCERER)
			hit += 20;
	}
	if (hit < 5)
		hit = 5;
	if (hit > 95)
		hit = 95;
	if (hper < hit) {
		if ((plr[p].data._pmode == PM_STAND || plr[p].data._pmode == PM_ATTACK) && plr[p].data._pBlockFlag) {
			blkper = random_(73, 100);
		} else {
			blkper = 100;
		}
		if (shift == TRUE)
			blkper = 100;
		blk = plr[p].data._pDexterity + plr[p].data._pBaseToBlk + (plr[p].data._pLevel << 1) - (plr[pnum].data._pLevel << 1);

		if (blk < 0) {
			blk = 0;
		}
		if (blk > 100) {
			blk = 100;
		}

		if (mtype == MIS_BONESPIRIT) {
			dam = plr[p].data._pHitPoints / 3;
		} else {
			dam = mindam + random_(70, maxdam - mindam + 1);
			if (!missiledata[mtype].mType)
				dam += plr[pnum].data._pIBonusDamMod + plr[pnum].data._pDamageMod + dam * plr[pnum].data._pIBonusDam / 100;
			if (!shift)
				dam <<= 6;
		}
		if (missiledata[mtype].mType)
			dam >>= 1;
		if (resper > 0) {
			if (pnum == myplr())
				NetSendCmdDamage(TRUE, p, dam - resper * dam / 100);
			if (plr[pnum].data._pClass == PC_WARRIOR) {
				tac = PS_WARR69;
#ifndef SPAWN
			} else if (plr[pnum].data._pClass == PC_ROGUE) {
				tac = PS_ROGUE69;
			} else if (plr[pnum].data._pClass == PC_SORCERER) {
				tac = PS_MAGE69;
#endif
			} else {
				return TRUE;
			}
			PlaySfxLoc(tac, plr[pnum].data._p);
		} else {
			if (blkper < blk) {
				plr[pnum].StartPlrBlock(GetDirection(plr[p].data._p, plr[pnum].data._p));
			} else {
				if (pnum == myplr())
					NetSendCmdDamage(TRUE, p, dam);
				plr[p].StartPlrHit(dam, FALSE);
			}
		}
		return TRUE;
	}
	return FALSE;
}

void CheckMissileCol(int i, int mindam, int maxdam, BOOL shift, V2Di m, BOOLEAN nodel)
{
	int oi;

	if (missile[i]._miAnimType != MFILE_FIREWAL && missile[i]._misource != -1) {
		if (!missile[i]._micaster) {
			if (grid.at(m).dMonster > 0) {
				if (MonsterMHit(
				        missile[i]._misource,
				        grid.at(m).dMonster - 1,
				        mindam,
				        maxdam,
				        missile[i]._midist,
				        missile[i]._mitype,
				        shift)) {
					if (!nodel)
						missile[i]._mirange = 0;
					missile[i]._miHitFlag = TRUE;
				}
			} else {
				if (grid.at(m).dMonster < 0
				    && monsters[-(grid.at(m).dMonster + 1)].data._mmode == MM_STONE
				    && MonsterMHit(
				           missile[i]._misource,
						   -(grid.at(m).dMonster + 1),
				           mindam,
				           maxdam,
				           missile[i]._midist,
				           missile[i]._mitype,
				           shift)) {
					if (!nodel)
						missile[i]._mirange = 0;
					missile[i]._miHitFlag = TRUE;
				}
			}
			if (grid.at(m).dPlayer > 0
			    && grid.at(m).dPlayer - 1 != missile[i]._misource
			    && Plr2PlrMHit(
			           missile[i]._misource,
			        grid.at(m).dPlayer - 1,
			           mindam,
			           maxdam,
			           missile[i]._midist,
			           missile[i]._mitype,
			           shift)) {
				if (!nodel)
					missile[i]._mirange = 0;
				missile[i]._miHitFlag = TRUE;
			}
		} else {
			if (monsters[missile[i]._misource].data._mFlags & MFLAG_TARGETS_MONSTER
			    && grid.at(m).dMonster > 0
			    && monsters[grid.at(m).dMonster - 1].data._mFlags & MFLAG_GOLEM
			    && MonsterTrapHit(grid.at(m).dMonster - 1, mindam, maxdam, missile[i]._midist, missile[i]._mitype, shift)) {
				if (!nodel)
					missile[i]._mirange = 0;
				missile[i]._miHitFlag = TRUE;
			}
			if (grid.at(m).dPlayer > 0
			    && PlayerMHit(
			           grid.at(m).dPlayer - 1,
			           missile[i]._misource,
			           missile[i]._midist,
			           mindam,
			           maxdam,
			           missile[i]._mitype,
			           shift,
			           0)) {
				if (!nodel)
					missile[i]._mirange = 0;
					missile[i]._miHitFlag = TRUE;
			}
		}
	} else {
		if (grid.at(m).dMonster > 0) {
			if (missile[i]._miAnimType == MFILE_FIREWAL) {
				if (MonsterMHit(
				        missile[i]._misource,
				        grid.at(m).dMonster - 1,
				        mindam,
				        maxdam,
				        missile[i]._midist,
				        missile[i]._mitype,
				        shift)) {
					if (!nodel)
						missile[i]._mirange = 0;
					missile[i]._miHitFlag = TRUE;
				}
			} else if (MonsterTrapHit(grid.at(m).dMonster - 1, mindam, maxdam, missile[i]._midist, missile[i]._mitype, shift)) {
				if (!nodel)
					missile[i]._mirange = 0;
				missile[i]._miHitFlag = TRUE;
			}
		}
		if (grid.at(m).dPlayer > 0
		    && PlayerMHit(grid.at(m).dPlayer - 1, -1, missile[i]._midist, mindam, maxdam, missile[i]._mitype, shift, missile[i]._miAnimType == MFILE_FIREWAL)) {
			if (!nodel)
				missile[i]._mirange = 0;
			missile[i]._miHitFlag = TRUE;
		}
	}
	if (grid.at(m).dObject) {
		oi = grid.at(m).dObject > 0 ? grid.at(m).dObject - 1 : -(grid.at(m).dObject + 1);
		if (!object[oi]._oMissFlag) {
			if (object[oi]._oBreak == 1)
				BreakObject(-1, oi);
			if (!nodel)
				missile[i]._mirange = 0;
			missile[i]._miHitFlag = FALSE;
		}
	}
	if (pieces[grid.at(m).dPiece].nMissileTable) {
		if (!nodel)
			missile[i]._mirange = 0;
		missile[i]._miHitFlag = FALSE;
	}
	if (!missile[i]._mirange && missiledata[missile[i]._mitype].miSFX != -1)
		PlaySfxLoc(missiledata[missile[i]._mitype].miSFX, missile[i]._mi);
}

void SetMissAnim(int mi, int animtype)
{
	int dir = missile[mi]._mimfnum;

	if (animtype > MFILE_NULL) {
		animtype = MFILE_NULL;
	}

	missile[mi]._miAnimType = animtype;
	missile[mi]._miAnimFlags = misfiledata[animtype].mFlags;
	missile[mi]._miAnimData = misfiledata[animtype].mAnimData[dir];
	missile[mi]._miAnimDelay = misfiledata[animtype].mAnimDelay[dir];
	missile[mi]._miAnimLen = misfiledata[animtype].mAnimLen[dir];
	missile[mi]._miAnimWidth = misfiledata[animtype].mAnimWidth[dir];
	missile[mi]._miAnimWidth2 = misfiledata[animtype].mAnimWidth2[dir];
	missile[mi]._miAnimCnt = 0;
	missile[mi]._miAnimFrame = 1;
}

void SetMissDir(int mi, int dir)
{
	missile[mi]._mimfnum = dir;
	SetMissAnim(mi, missile[mi]._miAnimType);
}

void LoadMissileGFX(BYTE mi)
{
	char pszName[256];
	int i;
	BYTE *file;
	MisFileData *mfd;

	mfd = &misfiledata[mi];
	if (mfd->mFlags & MFLAG_ALLOW_SPECIAL) {
		sprintf(pszName, "Missiles\\%s.CL2", mfd->mName);
		file = LoadFileInMem(pszName, NULL);
		for (i = 0; i < mfd->mAnimFAmt; i++)
			mfd->mAnimData[i] = CelGetFrameStart(file, i);
	} else if (mfd->mAnimFAmt == 1) {
		sprintf(pszName, "Missiles\\%s.CL2", mfd->mName);
		if (!mfd->mAnimData[0])
			mfd->mAnimData[0] = LoadFileInMem(pszName, NULL);
	} else {
		for (i = 0; i < mfd->mAnimFAmt; i++) {
			sprintf(pszName, "Missiles\\%s%i.CL2", mfd->mName, i + 1);
			if (!mfd->mAnimData[i]) {
				file = LoadFileInMem(pszName, NULL);
				mfd->mAnimData[i] = file;
			}
		}
	}
}

void InitMissileGFX()
{
	int mi;

	for (mi = 0; misfiledata[mi].mAnimFAmt; mi++) {
		if (!(misfiledata[mi].mFlags & MFLAG_HIDDEN))
			LoadMissileGFX(mi);
	}
}

void FreeMissileGFX(int mi)
{
	int i;
	DWORD *p;

	if (misfiledata[mi].mFlags & MFLAG_ALLOW_SPECIAL) {
		if (misfiledata[mi].mAnimData[0]) {
			p = (DWORD *)misfiledata[mi].mAnimData[0];
			p -= misfiledata[mi].mAnimFAmt;
			MemFreeDbg(p);
			misfiledata[mi].mAnimData[0] = NULL;
		}
		return;
	}

	for (i = 0; i < misfiledata[mi].mAnimFAmt; i++) {
		if (misfiledata[mi].mAnimData[i]) {
			MemFreeDbg(misfiledata[mi].mAnimData[i]);
		}
	}
}

void FreeMissiles()
{
	int mi;

	for (mi = 0; misfiledata[mi].mAnimFAmt; mi++) {
		if (!(misfiledata[mi].mFlags & MFLAG_HIDDEN))
			FreeMissileGFX(mi);
	}
}

void FreeMissiles2()
{
	int mi;

	for (mi = 0; misfiledata[mi].mAnimFAmt; mi++) {
		if (misfiledata[mi].mFlags & MFLAG_HIDDEN)
			FreeMissileGFX(mi);
	}
}

void InitMissiles()
{
	int mi, src, i, j;

	myplr().data._pSpellFlags &= ~0x1;
	if (myplr().data._pInfraFlag == TRUE) {
		for (i = 0; i < nummissiles; ++i) {
			mi = missileactive[i];
			if (missile[mi]._mitype == MIS_INFRA) {
				src = missile[mi]._misource;
				if (src == myplr())
					CalcPlrItemVals(src, TRUE);
			}
		}
	}
	nummissiles = 0;
	for (i = 0; i < MAXMISSILES; i++) {
		missileavail[i] = i;
		missileactive[i] = 0;
	}
	numchains = 0;
	for (i = 0; i < MAXMISSILES; i++) {
		chain[i].idx = -1;
		chain[i]._mitype = 0;
		chain[i]._mirange = 0;
	}
	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++) {
			grid[i][j].dFlags &= ~BFLAG_MISSILE;
		}
	}
}

void AddLArrow(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	if (s == d) {
		d += { XDirAdd[midir], YDirAdd[midir] };
	}
	if (!mienemy) {
		if (plr[id].data._pClass == PC_ROGUE)
			GetMissileVel(mi, s, d, (plr[id].data._pLevel >> 2) + 31);
		else if (plr[id].data._pClass == PC_WARRIOR)
			GetMissileVel(mi, s, d, (plr[id].data._pLevel >> 3) + 31);
		else
			GetMissileVel(mi, s, d, 32);
	} else
		GetMissileVel(mi, s, d, 32);

	SetMissDir(mi, GetDirection16(s, d));
	missile[mi]._mirange = 256;
	missile[mi]._miVar1 = s.x;
	missile[mi]._miVar2 = s.y;
	missile[mi]._mlid = AddLight(s, 5);
}

void AddArrow(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	int av;

	if (s == d) {
		d += { XDirAdd[midir], YDirAdd[midir] };
	}
	if (!mienemy) {
		av = 32;
		if (plr[id].data._pIFlags & ISPL_RNDARROWVEL) {
			av = random_(64, 32) + 16;
		}
		if (plr[id].data._pClass == PC_ROGUE)
			av += (plr[id].data._pLevel - 1) >> 2;
		if (plr[id].data._pClass == PC_WARRIOR)
			av += (plr[id].data._pLevel - 1) >> 3;
		GetMissileVel(mi, s, d, av);
	} else {
		GetMissileVel(mi, s, d, 32);
	}
	missile[mi]._miAnimFrame = GetDirection16(s, d) + 1;
	missile[mi]._mirange = 256;
}

void GetVileMissPos(int mi, V2Di d)
{
	V2Di n;
	for (int k = 1; k < 50; k++) {
		for (int j = -k; j <= k; j++) {
			n.y = j + d.y;
			for (int i = -k; i <= k; i++) {
				n.x = i + d.x;
				if (PosOkPlayer(myplr(), n)) {
					missile[mi]._mi = n;
					return;
				}
			}
		}
	}
	missile[mi]._mi = d;
}

void AddRndTeleport(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	int pn, r1, r2, nTries;

	nTries = 0;
	do {
		nTries++;
		if (nTries > 500) {
			r1 = 0;
			r2 = 0;
			break;
		}
		r1 = random_(58, 3) + 4;
		r2 = random_(58, 3) + 4;
		if (random_(58, 2) == 1)
			r1 = -r1;
		if (random_(58, 2) == 1)
			r2 = -r2;

		pn = grid[r1 + s.x][s.y + r2].dPiece;
	} while (pieces[pn].nSolidTable || grid[r1 + s.x][s.y + r2].dObject || grid[r1 + s.x][s.y + r2].dMonster);

	missile[mi]._mirange = 2;
	missile[mi]._miVar1 = 0;
	if (!level.setlevel || level.setlvlnum != SL_VILEBETRAYER) {
		missile[mi]._mi = { s.x + r1, s.y + r2 };
		if (!mienemy)
			UseMana(id, SPL_RNDTELEPORT);
	} else {
		pn = grid.at(d).dObject - 1;
		if (object[pn]._otype == OBJ_MCIRCLE1 || object[pn]._otype == OBJ_MCIRCLE2) {
			missile[mi]._mi = d;
			if (!PosOkPlayer(myplr(), d))
				GetVileMissPos(mi, d);
		}
	}
}

void AddFirebolt(int mi, V2Di s, V2Di d, int midir, char micaster, int id, int dam)
{
	int i, mx, sp;

	if (s == d) {
		d += { XDirAdd[midir], YDirAdd[midir] };
	}
	if (!micaster) {
		for (i = 0; i < nummissiles; i++) {
			mx = missileactive[i];
			if (missile[mx]._mitype == MIS_GUARDIAN && missile[mx]._misource == id && missile[mx]._miVar3 == mi)
				break;
		}
		if (i == nummissiles)
			UseMana(id, SPL_FIREBOLT);
		if (id != -1) {
			sp = 2 * missile[mi]._mispllvl + 16;
			if (sp >= 63)
				sp = 63;
		} else {
			sp = 16;
		}
	} else {
		sp = 26;
	}
	GetMissileVel(mi, s, d, sp);
	SetMissDir(mi, GetDirection16(s, d));
	missile[mi]._mirange = 256;
	missile[mi]._miVar1 = s.x;
	missile[mi]._miVar2 = s.y;
	missile[mi]._mlid = AddLight(s, 8);
}

void AddMagmaball(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	GetMissileVel(mi, s, d, 16);
	missile[mi]._mitoff.x += 3 * missile[mi]._mivel.x;
	missile[mi]._mitoff.y += 3 * missile[mi]._mivel.y;
	GetMissilePos(mi);
	missile[mi]._mirange = 256;
	missile[mi]._miVar1 = s.x;
	missile[mi]._miVar2 = s.y;
	missile[mi]._mlid = AddLight(s, 8);
}

void miss_null_33(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	GetMissileVel(mi, s, d, 16);
	missile[mi]._mirange = 256;
	missile[mi]._miVar1 = s.x;
	missile[mi]._miVar2 = s.y;
	PutMissile(mi);
}

void AddTeleport(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	int i, pn, k, j;
	V2Di t;
	int CrawlNum[6] = { 0, 3, 12, 45, 94, 159 };

	missile[mi]._miDelFlag = TRUE;
	for (i = 0; i < 6; i++) {
		k = CrawlNum[i];
		pn = k + 2;
		for (j = (BYTE)CrawlTable[k]; j > 0; j--) {
			t.x = d.x + CrawlTable[pn - 1];
			t.y = d.y + CrawlTable[pn];
			if (0 < t.x && t.x < MAXDUNX && 0 < t.y && t.y < MAXDUNY) {
				if (!(pieces[grid.at(t).dPiece].nSolidTable | grid.at(t).dMonster | grid.at(t).dObject | grid.at(t).dPlayer)) {
					missile[mi]._mi = t;
					missile[mi]._mis = t;
					missile[mi]._miDelFlag = FALSE;
					i = 6;
					break;
				}
			}
			pn += 2;
		}
	}

	if (!missile[mi]._miDelFlag) {
		UseMana(id, SPL_TELEPORT);
		missile[mi]._mirange = 2;
	}
}

void AddLightball(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	GetMissileVel(mi, s, d, 16);
	missile[mi]._midam = dam;
	missile[mi]._miAnimFrame = random_(63, 8) + 1;
	missile[mi]._mirange = 255;
	if (id < 0) {
		missile[mi]._miVar1 = s.x;
		missile[mi]._miVar2 = s.y;
	} else {
		missile[mi]._miVar1 = plr[id].data._p.x;
		missile[mi]._miVar2 = plr[id].data._p.y;
	}
}

void AddFirewall(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	int i;

	missile[mi]._midam = 16 * (random_(53, 10) + random_(53, 10) + plr[id].data._pLevel + 2) >> 1;
	GetMissileVel(mi, s, d, 16);
	missile[mi]._mirange = 10;
	i = missile[mi]._mispllvl;
	if (i > 0)
		missile[mi]._mirange = 10 * (i + 1);
	missile[mi]._mirange = ((missile[mi]._mirange * plr[id].data._pISplDur >> 3) & 0xFFFFFFF0) + 16 * missile[mi]._mirange;
	missile[mi]._miVar1 = missile[mi]._mirange - missile[mi]._miAnimLen;
	missile[mi]._miVar2 = 0;
}

void AddFireball(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	int i;

	if (s == d) {
		d += { XDirAdd[midir], YDirAdd[midir] };
	}
	if (!mienemy) {
		missile[mi]._midam = 2 * (plr[id].data._pLevel + random_(60, 10) + random_(60, 10)) + 4;
		for (i = missile[mi]._mispllvl; i > 0; i--) {
			missile[mi]._midam += missile[mi]._midam >> 3;
		}
		i = 2 * missile[mi]._mispllvl + 16;
		if (i > 50)
			i = 50;
		UseMana(id, SPL_FIREBALL);
	} else {
		i = 16;
	}
	GetMissileVel(mi, s, d, i);
	SetMissDir(mi, GetDirection16(s, d));
	missile[mi]._mirange = 256;
	missile[mi]._miVar1 = s.x;
	missile[mi]._miVar2 = s.y;
	missile[mi]._miVar3 = 0;
	missile[mi]._miVar4 = s.x;
	missile[mi]._miVar5 = s.y;
	missile[mi]._mlid = AddLight(s, 8);
}

void AddLightctrl(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	if (!dam && !mienemy)
		UseMana(id, SPL_LIGHTNING);
	missile[mi]._miVar1 = s.x;
	missile[mi]._miVar2 = s.y;
	GetMissileVel(mi, s, d, 32);
	missile[mi]._miAnimFrame = random_(52, 8) + 1;
	missile[mi]._mirange = 256;
}

void AddLightning(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	missile[mi]._mis = d;
	if (midir >= 0) {
		missile[mi]._mioff = missile[midir]._mioff;
		missile[mi]._mitoff = missile[midir]._mitoff;
	}
	missile[mi]._miAnimFrame = random_(52, 8) + 1;

	if (midir < 0 || mienemy == 1 || id == -1) {
		if (midir < 0 || id == -1)
			missile[mi]._mirange = 8;
		else
			missile[mi]._mirange = 10;
	} else {
		missile[mi]._mirange = (missile[mi]._mispllvl >> 1) + 6;
	}
	missile[mi]._mlid = AddLight(missile[mi]._mi, 4);
}

void AddMisexp(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	CMonster *mon;

	if (mienemy && id > 0) {
		mon = monsters[id].data.MType;
		if (mon->mtype == MT_SUCCUBUS)
			SetMissAnim(mi, MFILE_FLAREEXP);
		if (mon->mtype == MT_SNOWWICH)
			SetMissAnim(mi, MFILE_SCBSEXPB);
		if (mon->mtype == MT_HLSPWN)
			SetMissAnim(mi, MFILE_SCBSEXPD);
		if (mon->mtype == MT_SOLBRNR)
			SetMissAnim(mi, MFILE_SCBSEXPC);
	}

	missile[mi]._mi = missile[d.x]._mi;
	missile[mi]._mis = missile[d.x]._mis;
	missile[mi]._mioff = missile[d.x]._mioff;
	missile[mi]._mitoff = missile[d.x]._mitoff;
	missile[mi]._mivel = { 0, 0 };
	missile[mi]._mirange = missile[mi]._miAnimLen;
	missile[mi]._miVar1 = 0;
}

void AddWeapexp(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	missile[mi]._mi = s;
	missile[mi]._mis = s;
	missile[mi]._mivel = { 0, 0 };
	missile[mi]._miVar1 = 0;
	missile[mi]._miVar2 = d.x;
	missile[mi]._mimfnum = 0;
	if (d.x == 1)
		SetMissAnim(mi, MFILE_MAGBLOS);
	else
		SetMissAnim(mi, MFILE_MINILTNG);
	missile[mi]._mirange = missile[mi]._miAnimLen - 1;
}

BOOL CheckIfTrig(V2Di t)
{
	for (int i = 0; i < numtrigs; i++) {
		int dist = (trigs[i]._t - t).maxabs();
		if ((t == trigs[i]._t) || (dist < 2))
			return TRUE;
	}
	return FALSE;
}

void AddTown(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	int i, j, k, mx, dp;
	V2Di t;
	int CrawlNum[6] = { 0, 3, 12, 45, 94, 159 };

	if (level.currlevel) {
		missile[mi]._miDelFlag = TRUE;
		for (j = 0; j < 6; j++) {
			k = CrawlNum[j] + 2;
			for (i = (BYTE)CrawlTable[CrawlNum[j]]; i > 0; i--) {
				t.x = d.x + CrawlTable[k - 1];
				t.y = d.y + CrawlTable[k];
				if (t.x > 0 && t.x < MAXDUNX && t.y > 0 && t.y < MAXDUNY) {
					dp = grid.at(t).dPiece;
					if (!(grid.at(t).dMissile | pieces[dp].nSolidTable | pieces[dp].nMissileTable | grid.at(t).dObject | grid.at(t).dPlayer)) {
						if (!CheckIfTrig(t)) {
							missile[mi]._mi = t;
							missile[mi]._mis = t;
							missile[mi]._miDelFlag = FALSE;
							j = 6;
							break;
						}
					}
				}
				k += 2;
			}
		}
	} else {
		t = d;
		missile[mi]._mi = t;
		missile[mi]._mis = t;
		missile[mi]._miDelFlag = FALSE;
	}
	missile[mi]._mirange = 100;
	missile[mi]._miVar1 = missile[mi]._mirange - missile[mi]._miAnimLen;
	missile[mi]._miVar2 = 0;
	for (i = 0; i < nummissiles; i++) {
		mx = missileactive[i];
		if (missile[mx]._mitype == MIS_TOWN && mx != mi && missile[mx]._misource == id)
			missile[mx]._mirange = 0;
	}
	PutMissile(mi);
	if (id == myplr() && !missile[mi]._miDelFlag && level.currlevel) {
		if (!level.setlevel) {
			NetSendCmdLocParam3(TRUE, CMD_ACTIVATEPORTAL, t, level.currlevel, level.leveltype, 0);
		} else {
			NetSendCmdLocParam3(TRUE, CMD_ACTIVATEPORTAL, t, level.setlvlnum, level.leveltype, 1);
		}
	}
}

void AddFlash(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	int i;

	if (!mienemy) {
		if (id != -1) {
			missile[mi]._midam = 0;
			for (i = 0; i <= plr[id].data._pLevel; i++) {
				missile[mi]._midam += random_(55, 20) + 1;
			}
			for (i = missile[mi]._mispllvl; i > 0; i--) {
				missile[mi]._midam += missile[mi]._midam >> 3;
			}
			missile[mi]._midam += missile[mi]._midam >> 1;
			UseMana(id, SPL_FLASH);
		} else {
			missile[mi]._midam = level.currlevel >> 1;
		}
	} else {
		missile[mi]._midam = monsters[id].data.mLevel << 1;
	}
	missile[mi]._mirange = 19;
}

void AddFlash2(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	int i;

	if (!mienemy) {
		if (id != -1) {
			missile[mi]._midam = 0;
			for (i = 0; i <= plr[id].data._pLevel; i++) {
				missile[mi]._midam += random_(56, 2) + 1;
			}
			for (i = missile[mi]._mispllvl; i > 0; i--) {
				missile[mi]._midam += missile[mi]._midam >> 3;
			}
			missile[mi]._midam += missile[mi]._midam >> 1;
		} else {
			missile[mi]._midam = level.currlevel >> 1;
		}
	}
	missile[mi]._miPreFlag = TRUE;
	missile[mi]._mirange = 19;
}

void AddManashield(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	missile[mi]._mirange = 48 * plr[id].data._pLevel;
	missile[mi]._miVar1 = plr[id].data._pHitPoints;
	missile[mi]._miVar2 = plr[id].data._pHPBase;
	missile[mi]._miVar8 = -1;
	if (!mienemy)
		UseMana(id, SPL_MANASHIELD);
	if (id == myplr())
		NetSendCmd(TRUE, CMD_SETSHIELD);
	plr[id].data.pManaShield = TRUE;
}

void AddFiremove(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	missile[mi]._midam = random_(59, 10) + plr[id].data._pLevel + 1;
	GetMissileVel(mi, s, d, 16);
	missile[mi]._mirange = 255;
	missile[mi]._miVar1 = 0;
	missile[mi]._miVar2 = 0;
	missile[mi]._mi.x++;
	missile[mi]._mi.y++;
	missile[mi]._mioff.y -= 32;
}

void AddGuardian(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	V2Di t;
	int i, pn, k, j;
	int CrawlNum[6] = { 0, 3, 12, 45, 94, 159 };

	missile[mi]._midam = random_(62, 10) + (plr[id].data._pLevel >> 1) + 1;
	for (i = missile[mi]._mispllvl; i > 0; i--) {
		missile[mi]._midam += missile[mi]._midam >> 3;
	}

	missile[mi]._miDelFlag = TRUE;
	for (i = 0; i < 6; i++) {
		pn = CrawlNum[i];
		k = pn + 2;
		for (j = (BYTE)CrawlTable[pn]; j > 0; j--) {
			t = { d.x + CrawlTable[k - 1], d.y + CrawlTable[k] };
			pn = grid.at(t).dPiece;
			if (t.x > 0 && t.x < MAXDUNX && t.y > 0 && t.y < MAXDUNY) {
				if (LineClear(s, t)) {
					if (!(grid.at(t).dMonster | pieces[pn].nSolidTable | pieces[pn].nMissileTable | grid.at(t).dObject | grid.at(t).dMissile)) {
						missile[mi]._mi = t;
						missile[mi]._mis = t;
						missile[mi]._miDelFlag = FALSE;
						UseMana(id, SPL_GUARDIAN);
						i = 6;
						break;
					}
				}
			}
			k += 2;
		}
	}

	if (missile[mi]._miDelFlag != TRUE) {
		missile[mi]._misource = id;
		missile[mi]._mlid = AddLight(missile[mi]._mi, 1);
		missile[mi]._mirange = missile[mi]._mispllvl + (plr[id].data._pLevel >> 1);
		missile[mi]._mirange += (missile[mi]._mirange * plr[id].data._pISplDur) >> 7;

		if (missile[mi]._mirange > 30)
			missile[mi]._mirange = 30;
		missile[mi]._mirange <<= 4;
		if (missile[mi]._mirange < 30)
			missile[mi]._mirange = 30;

		missile[mi]._miVar1 = missile[mi]._mirange - missile[mi]._miAnimLen;
		missile[mi]._miVar2 = 0;
		missile[mi]._miVar3 = 1;
	}
}

void AddChain(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	missile[mi]._miVar1 = d.x;
	missile[mi]._miVar2 = d.y;
	missile[mi]._mirange = 1;
	UseMana(id, SPL_CHAIN);
}

void miss_null_11(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	SetMissDir(mi, d.x);
	missile[mi]._midam = 0;
	missile[mi]._miLightFlag = TRUE;
	missile[mi]._mirange = 250;
}

void miss_null_12(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	if (d.x > 3) d.x = 2;
	SetMissDir(mi, d.x);
	missile[mi]._midam = 0;
	missile[mi]._miLightFlag = TRUE;
	missile[mi]._mirange = 250;
}

void miss_null_13(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	if (d.x > 3) d.x = 2;
	SetMissDir(mi, d.x);
	missile[mi]._midam = 0;
	missile[mi]._miLightFlag = TRUE;
	missile[mi]._mirange = missile[mi]._miAnimLen;
}

void AddRhino(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	AnimStruct *anim;

	if (monsters[id].data.MType->mtype < MT_HORNED || monsters[id].data.MType->mtype > MT_OBLORD) {
		if (monsters[id].data.MType->mtype < MT_NSNAKE || monsters[id].data.MType->mtype > MT_GSNAKE) {
			anim = &monsters[id].data.MType->Anims[MA_WALK];
		} else {
			anim = &monsters[id].data.MType->Anims[MA_ATTACK];
		}
	} else {
		anim = &monsters[id].data.MType->Anims[MA_SPECIAL];
	}
	GetMissileVel(mi, s, d, 18);
	missile[mi]._mimfnum = midir;
	missile[mi]._miAnimFlags = 0;
	missile[mi]._miAnimData = anim->Data[midir];
	missile[mi]._miAnimDelay = anim->Rate;
	missile[mi]._miAnimLen = anim->Frames;
	missile[mi]._miAnimWidth = monsters[id].data.MType->width;
	missile[mi]._miAnimWidth2 = monsters[id].data.MType->width2;
	missile[mi]._miAnimAdd = 1;
	if (monsters[id].data.MType->mtype >= MT_NSNAKE && monsters[id].data.MType->mtype <= MT_GSNAKE)
		missile[mi]._miAnimFrame = 7;
	missile[mi]._miVar1 = 0;
	missile[mi]._miVar2 = 0;
	missile[mi]._miLightFlag = TRUE;
	if (monsters[id].data._uniqtype != 0) {
		missile[mi]._miUniqTrans = monsters[id].data._uniqtrans + 1;
		missile[mi]._mlid = monsters[id].data.mlid;
	}
	missile[mi]._mirange = 256;
	PutMissile(mi);
}

void miss_null_32(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	AnimStruct *anim;
	MonsterStruct *mon;

	anim = &monsters[id].data.MType->Anims[MA_WALK];
	GetMissileVel(mi, s, d, 16);
	missile[mi]._mimfnum = midir;
	missile[mi]._miAnimFlags = 0;
	missile[mi]._miAnimData = anim->Data[midir];
	missile[mi]._miAnimDelay = anim->Rate;
	missile[mi]._miAnimLen = anim->Frames;
	missile[mi]._miAnimWidth = monsters[id].data.MType->width;
	missile[mi]._miAnimWidth2 = monsters[id].data.MType->width2;
	missile[mi]._miAnimAdd = 1;
	missile[mi]._miVar1 = 0;
	missile[mi]._miVar2 = 0;
	missile[mi]._miLightFlag = TRUE;
	if (monsters[id].data._uniqtype != 0)
		missile[mi]._miUniqTrans = monsters[id].data._uniqtrans + 1;
	mon = &monsters[id].data;
	grid.at(mon->_m).dMonster = 0;
	missile[mi]._mirange = 256;
	PutMissile(mi);
}

void AddFlare(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	if (s.x == d.x && s.y == d.y) {
		d += { XDirAdd[midir], YDirAdd[midir] };
	}
	GetMissileVel(mi, s, d, 16);
	missile[mi]._mirange = 256;
	missile[mi]._miVar1 = s.x;
	missile[mi]._miVar2 = s.y;
	missile[mi]._mlid = AddLight(s, 8);
	if (!mienemy) {
		UseMana(id, SPL_FLARE);
		plr[id].data._pHitPoints -= 320;
		plr[id].data._pHPBase -= 320;
		drawhpflag = TRUE;
		if (plr[id].data._pHitPoints <= 0)
			plr[id].SyncPlrKill(0);
	} else {
		if (id > 0) {
			if (monsters[id].data.MType->mtype == MT_SUCCUBUS)
				SetMissAnim(mi, MFILE_FLARE);
			if (monsters[id].data.MType->mtype == MT_SNOWWICH)
				SetMissAnim(mi, MFILE_SCUBMISB);
			if (monsters[id].data.MType->mtype == MT_HLSPWN)
				SetMissAnim(mi, MFILE_SCUBMISD);
			if (monsters[id].data.MType->mtype == MT_SOLBRNR)
				SetMissAnim(mi, MFILE_SCUBMISC);
		}
	}
}

void AddAcid(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	GetMissileVel(mi, s, d, 16);
	SetMissDir(mi, GetDirection16(s, d));
	missile[mi]._mirange = 5 * (monsters[id].data._mint + 4);
	missile[mi]._mlid = -1;
	missile[mi]._miVar1 = s.x;
	missile[mi]._miVar2 = s.y;
	PutMissile(mi);
}

void miss_null_1D(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	missile[mi]._midam = dam;
	missile[mi]._mivel = { 0, 0 };
	missile[mi]._mirange = 50;
	missile[mi]._miVar1 = missile[mi]._mirange - missile[mi]._miAnimLen;
	missile[mi]._miVar2 = 0;
}

void AddAcidpud(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	int monst;

	missile[mi]._mivel = {0,0};
	missile[mi]._mioff = {0,0};
	missile[mi]._miLightFlag = TRUE;
	monst = missile[mi]._misource;
	missile[mi]._mirange = random_(50, 15) + 40 * (monsters[monst].data._mint + 1);
	missile[mi]._miPreFlag = TRUE;
}

void AddStone(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	int i, j, k, l, tx, ty, mid;
	int CrawlNum[6] = { 0, 3, 12, 45, 94, 159 };

	missile[mi]._misource = id;
	for (i = 0; i < 6; i++) {
		k = CrawlNum[i];
		l = k + 2;
		for (j = (BYTE)CrawlTable[k]; j > 0; j--) {
			tx = d.x + CrawlTable[l - 1];
			ty = d.y + CrawlTable[l];
			if (tx > 0 && tx < MAXDUNX && ty > 0 && ty < MAXDUNY) {
				mid = grid[tx][ty].dMonster;
				mid = mid > 0 ? mid - 1 : -1 - mid;
				if (mid > 3 && monsters[mid].data._mAi != AI_DIABLO) {
					if (monsters[mid].data._mmode != MM_FADEIN && monsters[mid].data._mmode != MM_FADEOUT && monsters[mid].data._mmode != MM_CHARGE) {
						j = -99;
						i = 6;
						missile[mi]._miVar1 = monsters[mid].data._mmode;
						missile[mi]._miVar2 = mid;
						monsters[mid].data._mmode = MM_STONE;
						break;
					}
				}
			}
			l += 2;
		}
	}

	if (j != -99) {
		missile[mi]._miDelFlag = TRUE;
	} else {
		missile[mi]._mi = tx;
		missile[mi]._mis = missile[mi]._mi;
		missile[mi]._mirange = missile[mi]._mispllvl + 6;
		missile[mi]._mirange += (missile[mi]._mirange * plr[id].data._pISplDur) >> 7;

		if (missile[mi]._mirange > 15)
			missile[mi]._mirange = 15;
		missile[mi]._mirange <<= 4;
		UseMana(id, SPL_STONE);
	}
}

void AddGolem(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	int i;
	int mx;

	missile[mi]._miDelFlag = FALSE;
	for (i = 0; i < nummissiles; i++) {
		mx = missileactive[i];
		if (missile[mx]._mitype == MIS_GOLEM) {
			if (mx != mi && missile[mx]._misource == id) {
				missile[mi]._miDelFlag = TRUE;
				return;
			}
		}
	}
	missile[mi]._miVar1 = s.x;
	missile[mi]._miVar2 = s.y;
	missile[mi]._miVar4 = d.x;
	missile[mi]._miVar5 = d.y;
	if ((monsters[id].data._m.x != 1 || monsters[id].data._m.y) && id == myplr())
		monsters[id].M_StartKill(id);
	UseMana(id, SPL_GOLEM);
}

void AddEtherealize(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	int i;

	missile[mi]._mirange = 16 * plr[id].data._pLevel >> 1;
	for (i = missile[mi]._mispllvl; i > 0; i--) {
		missile[mi]._mirange += missile[mi]._mirange >> 3;
	}
	missile[mi]._mirange += missile[mi]._mirange * plr[id].data._pISplDur >> 7;
	missile[mi]._miVar1 = plr[id].data._pHitPoints;
	missile[mi]._miVar2 = plr[id].data._pHPBase;
	if (!mienemy)
		UseMana(id, SPL_ETHEREALIZE);
}

void miss_null_1F(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	missile[mi]._miDelFlag = TRUE;
}

void miss_null_23(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	missile[mi]._midam = dam;
	missile[mi]._mi = s;
	missile[mi]._mis = s;
	missile[mi]._misource = id;
	if (dam == 1)
		SetMissDir(mi, 0);
	else
		SetMissDir(mi, 1);
	missile[mi]._miLightFlag = TRUE;
	missile[mi]._mirange = missile[mi]._miAnimLen;
}

void AddBoom(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	missile[mi]._mi = d;
	missile[mi]._mis = d;
	missile[mi]._mivel = { 0, 0 };
	missile[mi]._midam = dam;
	missile[mi]._mirange = missile[mi]._miAnimLen;
	missile[mi]._miVar1 = 0;
}

void AddHeal(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	int i;
	int HealAmount;

	HealAmount = (random_(57, 10) + 1) << 6;
	for (i = 0; i < plr[id].data._pLevel; i++) {
		HealAmount += (random_(57, 4) + 1) << 6;
	}
	for (i = 0; i < missile[mi]._mispllvl; i++) {
		HealAmount += (random_(57, 6) + 1) << 6;
	}

	if (plr[id].data._pClass == PC_WARRIOR)
		HealAmount <<= 1;

	if (plr[id].data._pClass == PC_ROGUE)
		HealAmount += HealAmount >> 1;

	plr[id].data._pHitPoints += HealAmount;
	if (plr[id].data._pHitPoints > plr[id].data._pMaxHP)
		plr[id].data._pHitPoints = plr[id].data._pMaxHP;

	plr[id].data._pHPBase += HealAmount;
	if (plr[id].data._pHPBase > plr[id].data._pMaxHPBase)
		plr[id].data._pHPBase = plr[id].data._pMaxHPBase;

	UseMana(id, SPL_HEAL);
	missile[mi]._miDelFlag = TRUE;
	drawhpflag = TRUE;
}

void AddHealOther(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	missile[mi]._miDelFlag = TRUE;
	UseMana(id, SPL_HEALOTHER);
	if (id == myplr()) {
		SetCursor_(CURSOR_HEALOTHER);
		if (sgbControllerActive)
			TryIconCurs();
	}
}

void AddElement(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	int i;

	if (s.x == d.x && s.y == d.y) {
		d += { XDirAdd[midir], YDirAdd[midir] };
	}
	missile[mi]._midam = 2 * (plr[id].data._pLevel + random_(60, 10) + random_(60, 10)) + 4;
	for (i = missile[mi]._mispllvl; i > 0; i--) {
		missile[mi]._midam += missile[mi]._midam >> 3;
	}
	missile[mi]._midam >>= 1;
	GetMissileVel(mi, s, d, 16);
	SetMissDir(mi, GetDirection8(s, d));
	missile[mi]._mirange = 256;
	missile[mi]._miVar1 = s.x;
	missile[mi]._miVar2 = s.y;
	missile[mi]._miVar3 = 0;
	missile[mi]._miVar4 = d.x;
	missile[mi]._miVar5 = d.y;
	missile[mi]._mlid = AddLight(s, 8);
	UseMana(id, SPL_ELEMENT);
}

extern void FocusOnInventory();

void AddIdentify(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	missile[mi]._miDelFlag = TRUE;
	UseMana(id, SPL_IDENTIFY);
	if (id == myplr()) {
		if (sbookflag)
			sbookflag = FALSE;
		if (!invflag) {
			invflag = TRUE;
			if (sgbControllerActive)
				FocusOnInventory();
		}
		SetCursor_(CURSOR_IDENTIFY);
	}
}

void AddFirewallC(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	int i, j, k, pn;
	V2Di t;
	int CrawlNum[6] = { 0, 3, 12, 45, 94, 159 };

	missile[mi]._miDelFlag = TRUE;
	for (i = 0; i < 6; i++) {
		k = CrawlNum[i];
		pn = k + 2;
		for (j = (BYTE)CrawlTable[k]; j > 0; j--) {
			t.x = d.x + CrawlTable[pn - 1];
			t.y = d.y + CrawlTable[pn];
			if (0 < t.x && t.x < MAXDUNX && 0 < t.y && t.y < MAXDUNY) {
				k = grid.at(t).dPiece;
				if (LineClear(s, t)) {
					if ((s.x != t.x || s.y != t.y) && !(pieces[k].nSolidTable | grid.at(t).dObject)) {
						missile[mi]._miVar1 = t.x;
						missile[mi]._miVar2 = t.y;
						missile[mi]._miVar5 = t.x;
						missile[mi]._miVar6 = t.y;
						missile[mi]._miDelFlag = FALSE;
						i = 6;
						break;
					}
				}
			}
			pn += 2;
		}
	}

	if (missile[mi]._miDelFlag != TRUE) {
		missile[mi]._miVar7 = 0;
		missile[mi]._miVar8 = 0;
		missile[mi]._miVar3 = (midir - 2) & 7;
		missile[mi]._miVar4 = (midir + 2) & 7;
		missile[mi]._mirange = 7;
		UseMana(id, SPL_FIREWALL);
	}
}

void AddInfra(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	int i;

	missile[mi]._mirange = 1584;
	for (i = missile[mi]._mispllvl; i > 0; i--) {
		missile[mi]._mirange += missile[mi]._mirange >> 3;
	}
	missile[mi]._mirange += missile[mi]._mirange * plr[id].data._pISplDur >> 7;
	if (!mienemy)
		UseMana(id, SPL_INFRA);
}

void AddWave(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	missile[mi]._miVar1 = d.x;
	missile[mi]._miVar2 = d.y;
	missile[mi]._miVar3 = 0;
	missile[mi]._miVar4 = 0;
	missile[mi]._mirange = 1;
	missile[mi]._miAnimFrame = 4;
	UseMana(id, SPL_WAVE);
}

void AddNova(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	int k;

	missile[mi]._miVar1 = d.x;
	missile[mi]._miVar2 = d.y;
	if (id != -1) {
		missile[mi]._midam = (random_(66, 6) + random_(66, 6) + random_(66, 6) + random_(66, 6) + random_(66, 6));
		missile[mi]._midam += plr[id].data._pLevel + 5;
		missile[mi]._midam >>= 1;
		for (k = missile[mi]._mispllvl; k > 0; k--) {
			missile[mi]._midam += missile[mi]._midam >> 3;
		}
		if (!mienemy)
			UseMana(id, SPL_NOVA);
	} else {
		missile[mi]._midam = ((DWORD)level.currlevel >> 1) + random_(66, 3) + random_(66, 3) + random_(66, 3);
	}
	missile[mi]._mirange = 1;
}

void AddBlodboil(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	missile[mi]._miDelFlag = 1;
}

void AddRepair(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	missile[mi]._miDelFlag = TRUE;
	UseMana(id, SPL_REPAIR);
	if (id == myplr()) {
		if (sbookflag)
			sbookflag = FALSE;
		if (!invflag) {
			invflag = TRUE;
			if (sgbControllerActive)
				FocusOnInventory();
		}
		SetCursor_(CURSOR_REPAIR);
	}
}

void AddRecharge(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	missile[mi]._miDelFlag = TRUE;
	UseMana(id, SPL_RECHARGE);
	if (id == myplr()) {
		if (sbookflag)
			sbookflag = FALSE;
		if (!invflag) {
			invflag = TRUE;
			if (sgbControllerActive)
				FocusOnInventory();
		}
		SetCursor_(CURSOR_RECHARGE);
	}
}

void AddDisarm(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	missile[mi]._miDelFlag = TRUE;
	UseMana(id, SPL_DISARM);
	if (id == myplr()) {
		SetCursor_(CURSOR_DISARM);
		if (sgbControllerActive) {
			if (pcursobj != -1)
				NetSendCmdLocParam1(true, CMD_DISARMXY, cursm, pcursobj);
			else
				SetCursor_(CURSOR_HAND);
		}
	}
}

void AddApoca(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	int i;

	missile[mi]._miVar1 = 8;
	missile[mi]._miVar2 = s.y - missile[mi]._miVar1;
	missile[mi]._miVar3 = missile[mi]._miVar1 + s.y;
	missile[mi]._miVar4 = s.x - missile[mi]._miVar1;
	missile[mi]._miVar5 = missile[mi]._miVar1 + s.x;
	missile[mi]._miVar6 = missile[mi]._miVar4;
	if (missile[mi]._miVar2 <= 0)
		missile[mi]._miVar2 = 1;
	if (missile[mi]._miVar3 >= MAXDUNY)
		missile[mi]._miVar3 = MAXDUNY - 1;
	if (missile[mi]._miVar4 <= 0)
		missile[mi]._miVar4 = 1;
	if (missile[mi]._miVar5 >= MAXDUNX)
		missile[mi]._miVar5 = MAXDUNX - 1;
	for (i = 0; i < plr[id].data._pLevel; i++) {
		missile[mi]._midam += random_(67, 6) + 1;
	}
	missile[mi]._mirange = 255;
	missile[mi]._miDelFlag = FALSE;
	UseMana(id, SPL_APOCA);
}

void AddFlame(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	int i;

	missile[mi]._miVar2 = 0;
	for (i = dam; i > 0; i--) {
		missile[mi]._miVar2 += 5;
	}
	missile[mi]._mis = d;
	missile[mi]._mioff = missile[midir]._mioff;
	missile[mi]._mitoff = missile[midir]._mitoff;
	missile[mi]._mirange = missile[mi]._miVar2 + 20;
	missile[mi]._mlid = AddLight(s, 1);
	if (!mienemy) {
		i = random_(79, plr[id].data._pLevel) + random_(79, 2);
		missile[mi]._midam = 8 * i + 16 + ((8 * i + 16) >> 1);
	} else {
		missile[mi]._midam = monsters[id].data.mMinDamage + random_(77, monsters[id].data.mMaxDamage - monsters[id].data.mMinDamage + 1);
	}
}

void AddFlamec(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	if (s.x == d.x && s.y == d.y) {
		d += { XDirAdd[midir], YDirAdd[midir] };
	}
	GetMissileVel(mi, s, d, 32);
	if (!mienemy)
		UseMana(id, SPL_FLAME);
	missile[mi]._miVar1 = s.x;
	missile[mi]._miVar2 = s.y;
	missile[mi]._miVar3 = 0;
	missile[mi]._mirange = 256;
}

void AddCbolt(int mi, V2Di s, V2Di d, int midir, char micaster, int id, int dam)
{
	assert((DWORD)mi < MAXMISSILES);

	if (micaster == 0) {
		if (id == myplr()) {
			missile[mi]._mirnd = random_(63, 15) + 1;
			missile[mi]._midam = random_(68, plr[id].data._pMagic >> 2) + 1;
		} else {
			missile[mi]._mirnd = random_(63, 15) + 1;
			missile[mi]._midam = random_(68, plr[id].data._pMagic >> 2) + 1;
		}
	} else {
		missile[mi]._mirnd = random_(63, 15) + 1;
		missile[mi]._midam = 15;
	}

	if (s.x == d.x && s.y == d.y) {
		d += { XDirAdd[midir], YDirAdd[midir] };
	}

	missile[mi]._miAnimFrame = random_(63, 8) + 1;
	missile[mi]._mlid = AddLight(s, 5);

	GetMissileVel(mi, s, d, 8);
	missile[mi]._miVar1 = 5;
	missile[mi]._miVar2 = midir;
	missile[mi]._miVar3 = 0;
	missile[mi]._mirange = 256;
}

void AddHbolt(int mi, V2Di s, V2Di d, int midir, char micaster, int id, int dam)
{
	int sp;
	if (s.x == d.x && s.y == d.y) {
		d += { XDirAdd[midir], YDirAdd[midir] };
	}
	if (id != -1) {
		sp = 2 * missile[mi]._mispllvl + 16;
		if (sp >= 63) {
			sp = 63;
		}
	} else {
		sp = 16;
	}
	GetMissileVel(mi, s, d, sp);
	SetMissDir(mi, GetDirection16(s, d));
	missile[mi]._mirange = 256;
	missile[mi]._miVar1 = s.x;
	missile[mi]._miVar2 = s.y;
	missile[mi]._mlid = AddLight(s, 8);
	missile[mi]._midam = random_(69, 10) + plr[id].data._pLevel + 9;
	UseMana(id, SPL_HBOLT);
}

void AddResurrect(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	UseMana(id, SPL_RESURRECT);
	if (id == myplr()) {
		SetCursor_(CURSOR_RESURRECT);
		if (sgbControllerActive)
			TryIconCurs();
	}
	missile[mi]._miDelFlag = TRUE;
}

void AddResurrectBeam(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	missile[mi]._mi = d;
	missile[mi]._mis = missile[mi]._mi;
	missile[mi]._mivel = { 0, 0 };
	missile[mi]._mirange = misfiledata[MFILE_RESSUR1].mAnimLen[0];
}

void AddTelekinesis(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	missile[mi]._miDelFlag = TRUE;
	UseMana(id, SPL_TELEKINESIS);
	if (id == myplr())
		SetCursor_(CURSOR_TELEKINESIS);
}

void AddBoneSpirit(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	if (s.x == d.x && s.y == d.y) {
		d += { XDirAdd[midir], YDirAdd[midir] };
	}
	missile[mi]._midam = 0;
	GetMissileVel(mi, s, d, 16);
	SetMissDir(mi, GetDirection8(s, d));
	missile[mi]._mirange = 256;
	missile[mi]._miVar1 = s.x;
	missile[mi]._miVar2 = s.y;
	missile[mi]._miVar3 = 0;
	missile[mi]._miVar4 = d.x;
	missile[mi]._miVar5 = d.y;
	missile[mi]._mlid = AddLight(s, 8);
	if (!mienemy) {
		UseMana(id, SPL_BONESPIRIT);
		plr[id].data._pHitPoints -= 384;
		plr[id].data._pHPBase -= 384;
		drawhpflag = TRUE;
		if (plr[id].data._pHitPoints <= 0)
			plr[id].SyncPlrKill(0);
	}
}

void AddRportal(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	missile[mi]._mi = s;
	missile[mi]._mis = s;
	missile[mi]._mirange = 100;
	missile[mi]._miVar1 = 100 - missile[mi]._miAnimLen;
	missile[mi]._miVar2 = 0;
	PutMissile(mi);
}

void AddDiabApoca(int mi, V2Di s, V2Di d, int midir, char mienemy, int id, int dam)
{
	for (int pnum = 0; pnum < gbMaxPlayers; pnum++) {
		if (plr[pnum].data.plractive) {
			if (LineClear(s, plr[pnum].data._pfut)) {
				AddMissile({ 0, 0 }, plr[pnum].data._pfut, 0, MIS_BOOM2, mienemy, id, dam, 0);
			}
		}
	}
	missile[mi]._miDelFlag = TRUE;
}

int AddMissile(V2Di s, V2Di d, int midir, int mitype, char micaster, int id, int midam, int spllvl)
{
	int i, mi;

	if (nummissiles >= MAXMISSILES)
		return -1;

	if (mitype == MIS_MANASHIELD && plr[id].data.pManaShield == TRUE) {
		if (level.currlevel != plr[id].data.plrlevel)
			return -1;

		for (i = 0; i < nummissiles; i++) {
			mi = missileactive[i];
			if (missile[mi]._mitype == MIS_MANASHIELD && missile[mi]._misource == id)
				return -1;
		}
	}

	mi = missileavail[0];

	missileavail[0] = missileavail[MAXMISSILES - nummissiles - 1];
	missileactive[nummissiles] = mi;
	nummissiles++;

	missile[mi]._mitype = mitype;
	missile[mi]._micaster = micaster;
	missile[mi]._misource = id;
	missile[mi]._miAnimType = missiledata[mitype].mFileNum;
	missile[mi]._miDrawFlag = missiledata[mitype].mDraw;
	missile[mi]._mispllvl = spllvl;
	missile[mi]._mimfnum = midir;

	if (missile[mi]._miAnimType == MFILE_NONE || misfiledata[missile[mi]._miAnimType].mAnimFAmt < 8)
		SetMissDir(mi, 0);
	else
		SetMissDir(mi, midir);

	missile[mi]._mi = s;
	missile[mi]._mioff = { 0, 0 };
	missile[mi]._mis = s;
	missile[mi]._mitoff = { 0, 0 };
	missile[mi]._miDelFlag = FALSE;
	missile[mi]._miAnimAdd = 1;
	missile[mi]._miLightFlag = FALSE;
	missile[mi]._miPreFlag = FALSE;
	missile[mi]._miUniqTrans = 0;
	missile[mi]._midam = midam;
	missile[mi]._miHitFlag = FALSE;
	missile[mi]._midist = 0;
	missile[mi]._mlid = -1;
	missile[mi]._mirnd = 0;

	if (missiledata[mitype].mlSFX != -1) {
		PlaySfxLoc(missiledata[mitype].mlSFX, missile[mi]._mis);
	}

	missiledata[mitype].mAddProc(mi, s, d, midir, micaster, id, midam);

	return mi;
}

int Sentfire(int i, V2Di s)
{
	int ex = 0;
	if (LineClear(missile[i]._mi, s)) {
		if (grid.at(s).dMonster > 0 && monsters[grid.at(s).dMonster - 1].data._mhitpoints >> 6 > 0 && grid.at(s).dMonster - 1 > 3) {
			int dir = GetDirection(missile[i]._mi, s);
			missile[i]._miVar3 = missileavail[0];
			AddMissile(missile[i]._mi, s, dir, MIS_FIREBOLT, 0, missile[i]._misource, missile[i]._midam, GetSpellLevel(missile[i]._misource, SPL_FIREBOLT));
			ex = -1;
		}
	}
	if (ex == -1) {
		SetMissDir(i, 2);
		missile[i]._miVar2 = 3;
	}

	return ex;
}

void MI_Dummy(int i)
{
	return;
}

void MI_Golem(int i)
{
	int CrawlNum[6] = { 0, 3, 12, 45, 94, 159 };
	V2Di t;
	int dp, l, m, src, k, tid;
	char *ct;

	src = missile[i]._misource;
	if (monsters[src].data._m.x == 1 && !monsters[src].data._m.y) {
		for (l = 0; l < 6; l++) {
			k = CrawlNum[l];
			tid = k + 2;
			for (m = (BYTE)CrawlTable[k]; m > 0; m--) {
				ct = &CrawlTable[tid];
				t.x = missile[i]._miVar4 + *(ct - 1);
				t.y = missile[i]._miVar5 + *ct;
				if (0 < t.x && t.x < MAXDUNX && 0 < t.y && t.y < MAXDUNY) {
					dp = grid.at(t).dPiece;
					if (LineClear({ missile[i]._miVar1, missile[i]._miVar2 }, t)) {
						if (!(grid.at(t).dMonster | pieces[dp].nSolidTable | grid.at(t).dObject)) {
							l = 6;
							monsters[src].SpawnGolum(t, i);
							break;
						}
					}
				}
				tid += 2;
			}
		}
	}
	missile[i]._miDelFlag = TRUE;
}

void MI_SetManashield(int i)
{
	ManashieldFlag = TRUE;
}

void MI_LArrow(int i)
{
	int p, mind, maxd, rst;

	missile[i]._mirange--;
	p = missile[i]._misource;
	if (missile[i]._miAnimType == MFILE_MINILTNG || missile[i]._miAnimType == MFILE_MAGBLOS) {
		ChangeLight(missile[i]._mlid, missile[i]._mi, missile[i]._miAnimFrame + 5);
		rst = missiledata[missile[i]._mitype].mResist;
		if (missile[i]._mitype == MIS_LARROW) {
			if (p != -1) {
				mind = plr[p].data._pILMinDam;
				maxd = plr[p].data._pILMaxDam;
			} else {
				mind = random_(68, 10) + 1 + level.currlevel;
				maxd = random_(68, 10) + 1 + level.currlevel * 2;
			}
			missiledata[MIS_LARROW].mResist = MISR_LIGHTNING;
			CheckMissileCol(i, mind, maxd, FALSE, missile[i]._mi, TRUE);
		}
		if (missile[i]._mitype == MIS_FARROW) {
			if (p != -1) {
				mind = plr[p].data._pIFMinDam;
				maxd = plr[p].data._pIFMaxDam;
			} else {
				mind = random_(68, 10) + 1 + level.currlevel;
				maxd = random_(68, 10) + 1 + level.currlevel * 2;
			}
			missiledata[MIS_FARROW].mResist = MISR_FIRE;
			CheckMissileCol(i, mind, maxd, FALSE, missile[i]._mi, TRUE);
		}
		missiledata[missile[i]._mitype].mResist = rst;
	} else {
		missile[i]._midist++;
		missile[i]._mitoff += missile[i]._mivel;
		GetMissilePos(i);

		if (p != -1) {
			if (!missile[i]._micaster) {
				mind = plr[p].data._pIMinDam;
				maxd = plr[p].data._pIMaxDam;
			} else {
				mind = monsters[p].data.mMinDamage;
				maxd = monsters[p].data.mMaxDamage;
			}
		} else {
			mind = random_(68, 10) + 1 + level.currlevel;
			maxd = random_(68, 10) + 1 + level.currlevel * 2;
		}

		if (missile[i]._mi.x != missile[i]._mis.x || missile[i]._mi.y != missile[i]._mis.y) {
			rst = missiledata[missile[i]._mitype].mResist;
			missiledata[missile[i]._mitype].mResist = 0;
			CheckMissileCol(i, mind, maxd, FALSE, missile[i]._mi, FALSE);
			missiledata[missile[i]._mitype].mResist = rst;
		}
		if (!missile[i]._mirange) {
			missile[i]._mimfnum = 0;
			missile[i]._mitoff -= missile[i]._mivel;
			GetMissilePos(i);
			if (missile[i]._mitype == MIS_LARROW)
				SetMissAnim(i, MFILE_MINILTNG);
			else
				SetMissAnim(i, MFILE_MAGBLOS);
			missile[i]._mirange = missile[i]._miAnimLen - 1;
		} else {
			if (missile[i]._mi.x != missile[i]._miVar1 || missile[i]._mi.y != missile[i]._miVar2) {
				missile[i]._miVar1 = missile[i]._mi.x;
				missile[i]._miVar2 = missile[i]._mi.y;
				ChangeLight(missile[i]._mlid, { missile[i]._miVar1, missile[i]._miVar2 }, 5);
			}
		}
	}
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = TRUE;
		AddUnLight(missile[i]._mlid);
	}
	PutMissile(i);
}

void MI_Arrow(int i)
{
	int p, mind, maxd;

	missile[i]._mirange--;
	missile[i]._midist++;
	missile[i]._mitoff += missile[i]._mivel;
	GetMissilePos(i);
	p = missile[i]._misource;
	if (p != -1) {
		if (!missile[i]._micaster) {
			mind = plr[p].data._pIMinDam;
			maxd = plr[p].data._pIMaxDam;
		} else {
			mind = monsters[p].data.mMinDamage;
			maxd = monsters[p].data.mMaxDamage;
		}
	} else {
		mind = level.currlevel;
		maxd = 2 * level.currlevel;
	}
	if (missile[i]._mi.x != missile[i]._mis.x || missile[i]._mi.y != missile[i]._mis.y)
		CheckMissileCol(i, mind, maxd, FALSE, missile[i]._mi, FALSE);
	if (!missile[i]._mirange)
		missile[i]._miDelFlag = TRUE;
	PutMissile(i);
}

void MI_Firebolt(int i)
{
	V2Di om;
	int d, p;

	missile[i]._mirange--;
	if (missile[i]._mitype != MIS_BONESPIRIT || missile[i]._mimfnum != 8) {
		om = missile[i]._mitoff;
		missile[i]._mitoff += missile[i]._mivel;
		GetMissilePos(i);
		p = missile[i]._misource;
		if (p != -1) {
			if (!missile[i]._micaster) {
				switch (missile[i]._mitype) {
				case MIS_FIREBOLT:
					d = random_(75, 10) + (plr[p].data._pMagic >> 3) + missile[i]._mispllvl + 1;
					break;
				case MIS_FLARE:
					d = 3 * missile[i]._mispllvl - (plr[p].data._pMagic >> 3) + (plr[p].data._pMagic >> 1);
					break;
				case MIS_BONESPIRIT:
					d = 0;
					break;
				}
			} else {
				d = monsters[p].data.mMinDamage + random_(77, monsters[p].data.mMaxDamage - monsters[p].data.mMinDamage + 1);
			}
		} else {
			d = level.currlevel + random_(78, 2 * level.currlevel);
		}
		if (missile[i]._mi.x != missile[i]._mis.x || missile[i]._mi.y != missile[i]._mis.y) {
			CheckMissileCol(i, d, d, FALSE, missile[i]._mi, FALSE);
		}
		if (!missile[i]._mirange) {
			missile[i]._miDelFlag = TRUE;
			missile[i]._mitoff = om;
			GetMissilePos(i);
			switch (missile[i]._mitype) {
			case MIS_FIREBOLT:
			case MIS_MAGMABALL:
				AddMissile(missile[i]._mi, { i, 0 }, missile[i]._mimfnum, MIS_MISEXP, missile[i]._micaster, missile[i]._misource, 0, 0);
				break;
			case MIS_FLARE:
				AddMissile(missile[i]._mi, { i, 0 }, missile[i]._mimfnum, MIS_MISEXP2, missile[i]._micaster, missile[i]._misource, 0, 0);
				break;
			case MIS_ACID:
				AddMissile(missile[i]._mi, { i, 0 }, missile[i]._mimfnum, MIS_MISEXP3, missile[i]._micaster, missile[i]._misource, 0, 0);
				break;
			case MIS_BONESPIRIT:
				SetMissDir(i, 8);
				missile[i]._mirange = 7;
				missile[i]._miDelFlag = FALSE;
				PutMissile(i);
				return;
			}
			if (missile[i]._mlid >= 0)
				AddUnLight(missile[i]._mlid);
			PutMissile(i);
		} else {
			if (missile[i]._mi.x != missile[i]._miVar1 || missile[i]._mi.y != missile[i]._miVar2) {
				missile[i]._miVar1 = missile[i]._mi.x;
				missile[i]._miVar2 = missile[i]._mi.y;
				if (missile[i]._mlid >= 0)
					ChangeLight(missile[i]._mlid, { missile[i]._miVar1, missile[i]._miVar2 }, 8);
			}
			PutMissile(i);
		}
	} else if (!missile[i]._mirange) {
		if (missile[i]._mlid >= 0)
			AddUnLight(missile[i]._mlid);
		missile[i]._miDelFlag = TRUE;
		PlaySfxLoc(LS_BSIMPCT, missile[i]._mi);
		PutMissile(i);
	} else
		PutMissile(i);
}

void MI_Lightball(int i)
{
	int tx, ty, j, oi;
	char obj;

	tx = missile[i]._miVar1;
	ty = missile[i]._miVar2;
	missile[i]._mirange--;
	missile[i]._mitoff += missile[i]._mivel;
	GetMissilePos(i);
	j = missile[i]._mirange;
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, FALSE, missile[i]._mi, FALSE);
	if (missile[i]._miHitFlag == TRUE)
		missile[i]._mirange = j;
	obj = grid[tx][ty].dObject;
	if (obj && tx == missile[i]._mi.x && ty == missile[i]._mi.y) {
		if (obj > 0) {
			oi = obj - 1;
		} else {
			oi = -1 - obj;
		}
		if (object[oi]._otype == OBJ_SHRINEL || object[oi]._otype == OBJ_SHRINER)
			missile[i]._mirange = j;
	}
	if (!missile[i]._mirange)
		missile[i]._miDelFlag = TRUE;
	PutMissile(i);
}

void mi_null_33(int i)
{
	missile[i]._mirange--;
	missile[i]._mitoff += missile[i]._mivel;
	GetMissilePos(i);
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, FALSE, missile[i]._mi, FALSE);
	if (!missile[i]._mirange)
		missile[i]._miDelFlag = TRUE;
	PutMissile(i);
}

void MI_Acidpud(int i)
{
	int range;

	missile[i]._mirange--;
	range = missile[i]._mirange;
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, TRUE, missile[i]._mi, FALSE);
	missile[i]._mirange = range;
	if (!range) {
		if (missile[i]._mimfnum) {
			missile[i]._miDelFlag = TRUE;
		} else {
			SetMissDir(i, 1);
			missile[i]._mirange = missile[i]._miAnimLen;
		}
	}
	PutMissile(i);
}

void MI_Firewall(int i)
{
	int ExpLight[14] = { 2, 3, 4, 5, 5, 6, 7, 8, 9, 10, 11, 12, 12 };

	missile[i]._mirange--;
	if (missile[i]._mirange == missile[i]._miVar1) {
		SetMissDir(i, 1);
		missile[i]._miAnimFrame = random_(83, 11) + 1;
	}
	if (missile[i]._mirange == missile[i]._miAnimLen - 1) {
		SetMissDir(i, 0);
		missile[i]._miAnimFrame = 13;
		missile[i]._miAnimAdd = -1;
	}
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, TRUE, missile[i]._mi, TRUE);
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = TRUE;
		AddUnLight(missile[i]._mlid);
	}
	if (missile[i]._mimfnum && missile[i]._mirange && missile[i]._miAnimAdd != -1 && missile[i]._miVar2 < 12) {
		if (!missile[i]._miVar2)
			missile[i]._mlid = AddLight(missile[i]._mi, ExpLight[0]);
		ChangeLight(missile[i]._mlid, missile[i]._mi, ExpLight[missile[i]._miVar2]);
		missile[i]._miVar2++;
	}
	PutMissile(i);
}

void MI_Fireball(int i)
{
	int dam, id;
	V2Di p, m;

	id = missile[i]._misource;
	dam = missile[i]._midam;
	missile[i]._mirange--;

	if (missile[i]._micaster == 0) {
		p = plr[id].data._p;
	} else {
		p = monsters[id].data._m;
	}

	if (missile[i]._miAnimType == MFILE_BIGEXP) {
		if (!missile[i]._mirange) {
			missile[i]._miDelFlag = TRUE;
			AddUnLight(missile[i]._mlid);
		}
	} else {
		missile[i]._mitoff += missile[i]._mivel;
		GetMissilePos(i);
		if (missile[i]._mi.x != missile[i]._mis.x || missile[i]._mi.y != missile[i]._mis.y)
			CheckMissileCol(i, dam, dam, FALSE, missile[i]._mi, FALSE);
		if (!missile[i]._mirange) {
			m = missile[i]._mi;
			ChangeLight(missile[i]._mlid, { missile[i]._mi.x, m.y }, missile[i]._miAnimFrame);
			if (!CheckBlock(p, m))
				CheckMissileCol(i, dam, dam, FALSE, m, TRUE);
			if (!CheckBlock(p, { m.x, m.y + 1 }))
				CheckMissileCol(i, dam, dam, FALSE, { m.x, m.y + 1 }, TRUE);
			if (!CheckBlock(p, { m.x, m.y - 1 }))
				CheckMissileCol(i, dam, dam, FALSE, { m.x, m.y - 1 }, TRUE);
			if (!CheckBlock(p, { m.x + 1, m.y }))
				CheckMissileCol(i, dam, dam, FALSE, { m.x + 1, m.y }, TRUE);
			if (!CheckBlock(p, { m.x + 1, m.y - 1 }))
				CheckMissileCol(i, dam, dam, FALSE, { m.x + 1, m.y - 1 }, TRUE);
			if (!CheckBlock(p, { m.x + 1, m.y + 1 }))
				CheckMissileCol(i, dam, dam, FALSE, { m.x + 1, m.y + 1 }, TRUE);
			if (!CheckBlock(p, { m.x - 1, m.y }))
				CheckMissileCol(i, dam, dam, FALSE, { m.x - 1, m.y }, TRUE);
			if (!CheckBlock(p, { m.x - 1, m.y + 1 }))
				CheckMissileCol(i, dam, dam, FALSE, { m.x - 1, m.y + 1 }, TRUE);
			if (!CheckBlock(p, { m.x - 1, m.y - 1 }))
				CheckMissileCol(i, dam, dam, FALSE, { m.x - 1, m.y - 1 }, TRUE);
			if (!TransList[grid.at(m).dTransVal]
			    || (missile[i]._mivel.x < 0 && ((TransList[grid[m.x][m.y + 1].dTransVal] && pieces[grid[m.x][m.y + 1].dPiece].nSolidTable) ||
					(TransList[grid[m.x][m.y - 1].dTransVal] && pieces[grid[m.x][m.y - 1].dPiece].nSolidTable)))) {
				missile[i]._mi.x++;
				missile[i]._mi.y++;
				missile[i]._mioff.y -= 32;
			}
			if (missile[i]._mivel.y > 0
			    && (TransList[grid[m.x + 1][m.y].dTransVal] && pieces[grid[m.x + 1][m.y].dPiece].nSolidTable
			        || TransList[grid[m.x - 1][m.y].dTransVal] && pieces[grid[m.x - 1][m.y].dPiece].nSolidTable)) {
				missile[i]._mioff.y -= 32;
			}
			if (missile[i]._mivel.x > 0
			    && (TransList[grid[m.x][m.y + 1].dTransVal] && pieces[grid[m.x][m.y + 1].dPiece].nSolidTable
			        || TransList[grid[m.x][m.y - 1].dTransVal] && pieces[grid[m.x][m.y - 1].dPiece].nSolidTable)) {
				missile[i]._mioff.x -= 32;
			}
			missile[i]._mimfnum = 0;
			SetMissAnim(i, MFILE_BIGEXP);
			missile[i]._mirange = missile[i]._miAnimLen - 1;
		} else if (missile[i]._mi.x != missile[i]._miVar1 || missile[i]._mi.y != missile[i]._miVar2) {
			missile[i]._miVar1 = missile[i]._mi.x;
			missile[i]._miVar2 = missile[i]._mi.y;
			ChangeLight(missile[i]._mlid, { missile[i]._miVar1, missile[i]._miVar2 }, 8);
		}
	}

	PutMissile(i);
}

void MI_Lightctrl(int i)
{
	int pn, dam, p;
	V2Di m;

	assert((DWORD)i < MAXMISSILES);
	missile[i]._mirange--;

	p = missile[i]._misource;
	if (p != -1) {
		if (missile[i]._micaster == 0) {
			dam = (random_(79, 2) + random_(79, plr[p].data._pLevel) + 2) << 6;
		} else {
			dam = 2 * (monsters[p].data.mMinDamage + random_(80, monsters[p].data.mMaxDamage - monsters[p].data.mMinDamage + 1));
		}
	} else {
		dam = random_(81, level.currlevel) + 2 * level.currlevel;
	}

	missile[i]._mitoff += missile[i]._mivel;
	GetMissilePos(i);

	m = missile[i]._mi;
	assert((DWORD)m.x < MAXDUNX);
	assert((DWORD)m.y < MAXDUNY);
	pn = grid.at(m).dPiece;
	assert((DWORD)pn <= MAXTILES);

	if (missile[i]._misource == -1) {
		if ((m.x != missile[i]._mis.x || m.y != missile[i]._mis.y) && pieces[pn].nMissileTable) {
			missile[i]._mirange = 0;
		}
	} else if (pieces[pn].nMissileTable) {
		missile[i]._mirange = 0;
	}
	if (!pieces[pn].nMissileTable) {
		if ((m.x != missile[i]._miVar1 || m.y != missile[i]._miVar2) && m.x > 0 && m.y > 0 && m.x < MAXDUNX && m.y < MAXDUNY) {
			if (missile[i]._misource != -1) {
				if (missile[i]._micaster == 1
				    && monsters[missile[i]._misource].data.MType->mtype >= MT_STORM
				    && monsters[missile[i]._misource].data.MType->mtype <= MT_MAEL) {
					AddMissile(
					    m,
					    missile[i]._mis,
					    i,
					    MIS_LIGHTNING2,
					    missile[i]._micaster,
					    missile[i]._misource,
					    dam,
					    missile[i]._mispllvl);
				} else {
					AddMissile(
					    m,
					    missile[i]._mis,
					    i,
					    MIS_LIGHTNING,
					    missile[i]._micaster,
					    missile[i]._misource,
					    dam,
					    missile[i]._mispllvl);
				}
			} else {
				AddMissile(
				    m,
					missile[i]._mis,
				    i,
				    MIS_LIGHTNING,
				    missile[i]._micaster,
				    missile[i]._misource,
				    dam,
				    missile[i]._mispllvl);
			}
			missile[i]._miVar1 = missile[i]._mi.x;
			missile[i]._miVar2 = missile[i]._mi.y;
		}
	}
	if (missile[i]._mirange == 0 || m.x <= 0 || m.y <= 0 || m.x >= MAXDUNX || m.y > MAXDUNY) {
		missile[i]._miDelFlag = TRUE;
	}
}

void MI_Lightning(int i)
{
	int j;

	missile[i]._mirange--;
	j = missile[i]._mirange;
	if (missile[i]._mi.x != missile[i]._mis.x || missile[i]._mi.y != missile[i]._mis.y)
		CheckMissileCol(i, missile[i]._midam, missile[i]._midam, TRUE, missile[i]._mi, FALSE);
	if (missile[i]._miHitFlag == TRUE)
		missile[i]._mirange = j;
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = TRUE;
		AddUnLight(missile[i]._mlid);
	}
	PutMissile(i);
}

void MI_Town(int i)
{
	int ExpLight[17] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 15, 15 };
	int p;

	if (missile[i]._mirange > 1)
		missile[i]._mirange--;
	if (missile[i]._mirange == missile[i]._miVar1)
		SetMissDir(i, 1);
	if (level.currlevel && missile[i]._mimfnum != 1 && missile[i]._mirange) {
		if (!missile[i]._miVar2)
			missile[i]._mlid = AddLight(missile[i]._mi, 1);
		ChangeLight(missile[i]._mlid, missile[i]._mi, ExpLight[missile[i]._miVar2]);
		missile[i]._miVar2++;
	}

	for (p = 0; p < MAX_PLRS; p++) {
		if (plr[p].data.plractive && level.currlevel == plr[p].data.plrlevel && !plr[p].data._pLvlChanging && plr[p].data._pmode == PM_STAND && plr[p].data._p.x == missile[i]._mi.x && plr[p].data._p.y == missile[i]._mi.y) {
			plr[p].ClrPlrPath();
			if (p == myplr()) {
				NetSendCmdParam1(TRUE, CMD_WARP, missile[i]._misource);
				plr[p].data._pmode = PM_NEWLVL;
			}
		}
	}

	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = TRUE;
		AddUnLight(missile[i]._mlid);
	}
	PutMissile(i);
}

void MI_Flash(int i)
{
	if (!missile[i]._micaster) {
		if (missile[i]._misource != -1)
			plr[missile[i]._misource].data._pInvincible = TRUE;
	}
	missile[i]._mirange--;
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, TRUE, { missile[i]._mi.x - 1, missile[i]._mi.y }, TRUE);
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, TRUE, { missile[i]._mi.x, missile[i]._mi.y }, TRUE);
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, TRUE, { missile[i]._mi.x + 1, missile[i]._mi.y }, TRUE);
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, TRUE, { missile[i]._mi.x - 1, missile[i]._mi.y + 1 }, TRUE);
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, TRUE, { missile[i]._mi.x, missile[i]._mi.y + 1 }, TRUE);
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, TRUE, { missile[i]._mi.x + 1, missile[i]._mi.y + 1 }, TRUE);
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = TRUE;
		if (missile[i]._micaster == 0) {
			if (missile[i]._misource != -1)
				plr[missile[i]._misource].data._pInvincible = FALSE;
		}
	}
	PutMissile(i);
}

void MI_Flash2(int i)
{
	if (!missile[i]._micaster) {
		if (missile[i]._misource != -1)
			plr[missile[i]._misource].data._pInvincible = TRUE;
	}
	missile[i]._mirange--;
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, TRUE, { missile[i]._mi.x - 1, missile[i]._mi.y - 1 }, TRUE);
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, TRUE, { missile[i]._mi.x, missile[i]._mi.y - 1 }, TRUE);
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, TRUE, { missile[i]._mi.x + 1, missile[i]._mi.y - 1 }, TRUE);
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = TRUE;
		if (missile[i]._micaster == 0) {
			if (missile[i]._misource != -1)
				plr[missile[i]._misource].data._pInvincible = FALSE;
		}
	}
	PutMissile(i);
}

void MI_Manashield(int i)
{
	int id, diff;

	id = missile[i]._misource;
	missile[i]._mi = plr[id].data._p;
	missile[i]._mitoff.x = plr[id].data._poff.x << 16;
	missile[i]._mitoff.y = plr[id].data._poff.y << 16;
	if (plr[id].data._pmode == PM_WALK3) {
		missile[i]._mis = plr[id].data._pfut;
	} else {
		missile[i]._mis = plr[id].data._p;
	}
	GetMissilePos(i);
	if (plr[id].data._pmode == PM_WALK3) {
		if (plr[id].data._pdir == DIR_W)
			missile[i]._mi.x++;
		else
			missile[i]._mi.y++;
	}
	if (id != myplr()) {
		if (level.currlevel != plr[id].data.plrlevel)
			missile[i]._miDelFlag = TRUE;
	} else {
		if (plr[id].data._pMana <= 0 || !plr[id].data.plractive)
			missile[i]._mirange = 0;
		if (plr[id].data._pHitPoints < missile[i]._miVar1) {
			diff = missile[i]._miVar1 - plr[id].data._pHitPoints;
			if (missile[i]._mispllvl > 0) {
				diff += diff / -3;
			}

			if (diff < 0)
				diff = 0;
			drawmanaflag = TRUE;
			drawhpflag = TRUE;

			if (plr[id].data._pMana >= diff) {
				plr[id].data._pHitPoints = missile[i]._miVar1;
				plr[id].data._pHPBase = missile[i]._miVar2;
				plr[id].data._pMana -= diff;
				plr[id].data._pManaBase -= diff;
			} else {
				plr[id].data._pHitPoints = plr[id].data._pMana + missile[i]._miVar1 - diff;
				plr[id].data._pHPBase = plr[id].data._pMana + missile[i]._miVar2 - diff;
				plr[id].data._pMana = 0;
				plr[id].data._pManaBase = plr[id].data._pMaxManaBase - plr[id].data._pMaxMana;
				missile[i]._mirange = 0;
				missile[i]._miDelFlag = TRUE;
				if (plr[id].data._pHitPoints < 0)
					plr[id].SetPlayerHitPoints(0);
				if (!(plr[id].data._pHitPoints >> 6) && id == myplr()) {
					plr[id].SyncPlrKill(missile[i]._miVar8);
				}
			}
		}

		if (id == myplr() && !plr[id].data._pHitPoints && !missile[i]._miVar1 && plr[id].data._pmode != PM_DEATH) {
			missile[i]._mirange = 0;
			missile[i]._miDelFlag = TRUE;
			plr[id].SyncPlrKill(-1);
		}
		missile[i]._miVar1 = plr[id].data._pHitPoints;
		missile[i]._miVar2 = plr[id].data._pHPBase;
		if (missile[i]._mirange == 0) {
			missile[i]._miDelFlag = TRUE;
			NetSendCmd(TRUE, CMD_ENDSHIELD);
		}
	}
	PutMissile(i);
}

void MI_Etherealize(int i)
{
	int src;

	missile[i]._mirange--;
	src = missile[i]._misource;
	missile[i]._mi = plr[src].data._p;
	missile[i]._mitoff.x = plr[src].data._poff.x << 16;
	missile[i]._mitoff.y = plr[src].data._poff.y << 16;
	if (plr[src].data._pmode == PM_WALK3) {
		missile[i]._mis = plr[src].data._pfut;
	} else {
		missile[i]._mis = plr[src].data._p;
	}
	GetMissilePos(i);
	if (plr[src].data._pmode == PM_WALK3) {
		if (plr[src].data._pdir == DIR_W)
			missile[i]._mi.x++;
		else
			missile[i]._mi.y++;
	}
	plr[src].data._pSpellFlags |= 1;
	if (!missile[i]._mirange || plr[src].data._pHitPoints <= 0) {
		missile[i]._miDelFlag = TRUE;
		plr[src].data._pSpellFlags &= ~0x1;
	}
	PutMissile(i);
}

void MI_Firemove(int i)
{
	int j;
	int ExpLight[14] = { 2, 3, 4, 5, 5, 6, 7, 8, 9, 10, 11, 12, 12 };

	missile[i]._mi.x--;
	missile[i]._mi.y--;
	missile[i]._mioff.y += 32;
	missile[i]._miVar1++;
	if (missile[i]._miVar1 == missile[i]._miAnimLen) {
		SetMissDir(i, 1);
		missile[i]._miAnimFrame = random_(82, 11) + 1;
	}
	missile[i]._mitoff += missile[i]._mivel;
	GetMissilePos(i);
	j = missile[i]._mirange;
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, FALSE, missile[i]._mi, FALSE);
	if (missile[i]._miHitFlag == TRUE)
		missile[i]._mirange = j;
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = TRUE;
		AddUnLight(missile[i]._mlid);
	}
	if (missile[i]._mimfnum || !missile[i]._mirange) {
		if (missile[i]._mi.x != missile[i]._miVar3 || missile[i]._mi.y != missile[i]._miVar4) {
			missile[i]._miVar3 = missile[i]._mi.x;
			missile[i]._miVar4 = missile[i]._mi.y;
			ChangeLight(missile[i]._mlid, { missile[i]._miVar3, missile[i]._miVar4 }, 8);
		}
	} else {
		if (!missile[i]._miVar2)
			missile[i]._mlid = AddLight(missile[i]._mi, ExpLight[0]);
		ChangeLight(missile[i]._mlid, missile[i]._mi, ExpLight[missile[i]._miVar2]);
		missile[i]._miVar2++;
	}
	missile[i]._mi.x++;
	missile[i]._mi.y++;
	missile[i]._mioff.y -= 32;
	PutMissile(i);
}

void MI_Guardian(int i)
{
	int j, k, ex;
	V2Di s, s1;

	assert((DWORD)i < MAXMISSILES);

	missile[i]._mirange--;

	if (missile[i]._miVar2 > 0) {
		missile[i]._miVar2--;
	}
	if (missile[i]._mirange == missile[i]._miVar1 || missile[i]._mimfnum == MFILE_GUARD && missile[i]._miVar2 == 0) {
		SetMissDir(i, 1);
	}

	if (!(missile[i]._mirange % 16)) {
		ex = 0;
		for (j = 0; j < 23 && ex != -1; j++) {
			for (k = 10; k >= 0 && ex != -1 && (vCrawlTable[j][k] != 0 || vCrawlTable[j][k + 1] != 0); k -= 2) {
				if (s1.x == vCrawlTable[j][k] && s1.y == vCrawlTable[j][k + 1]) {
					continue;
				}
				s.x = missile[i]._mi.x + vCrawlTable[j][k];
				s.y = missile[i]._mi.y + vCrawlTable[j][k + 1];
				ex = Sentfire(i, s);
				if (ex == -1) {
					break;
				}
				s.x = missile[i]._mi.x - vCrawlTable[j][k];
				s.y = missile[i]._mi.y - vCrawlTable[j][k + 1];
				ex = Sentfire(i, s);
				if (ex == -1) {
					break;
				}
				s.x = missile[i]._mi.x + vCrawlTable[j][k];
				s.y = missile[i]._mi.y - vCrawlTable[j][k + 1];
				ex = Sentfire(i, s);
				if (ex == -1) {
					break;
				}
				s.x = missile[i]._mi.x - vCrawlTable[j][k];
				s.y = missile[i]._mi.y + vCrawlTable[j][k + 1];
				ex = Sentfire(i, s);
				if (ex == -1) {
					break;
				}
				s1.x = vCrawlTable[j][k];
				s1.y = vCrawlTable[j][k + 1];
			}
		}
	}

	if (missile[i]._mirange == 14) {
		SetMissDir(i, 0);
		missile[i]._miAnimFrame = 15;
		missile[i]._miAnimAdd = -1;
	}

	missile[i]._miVar3 += missile[i]._miAnimAdd;

	if (missile[i]._miVar3 > 15) {
		missile[i]._miVar3 = 15;
	} else if (missile[i]._miVar3 > 0) {
		ChangeLight(missile[i]._mlid, missile[i]._mi, missile[i]._miVar3);
	}

	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = TRUE;
		AddUnLight(missile[i]._mlid);
	}

	PutMissile(i);
}

void MI_Chain(int i)
{
	V2Di s, t;
	int id, l, n, m, k, rad, dir;
	int CrawlNum[19] = { 0, 3, 12, 45, 94, 159, 240, 337, 450, 579, 724, 885, 1062, 1255, 1464, 1689, 1930, 2187, 2460 };

	id = missile[i]._misource;
	s = missile[i]._mi;
	dir = GetDirection(s, { missile[i]._miVar1, missile[i]._miVar2 });
	AddMissile(s, { missile[i]._miVar1, missile[i]._miVar2 }, dir, MIS_LIGHTCTRL, 0, id, 1, missile[i]._mispllvl);
	rad = missile[i]._mispllvl + 3;
	if (rad > 19)
		rad = 19;
	for (m = 1; m < rad; m++) {
		k = CrawlNum[m];
		l = k + 2;
		for (n = (BYTE)CrawlTable[k]; n > 0; n--) {
			t.x = s.x + CrawlTable[l - 1];
			t.y = s.y + CrawlTable[l];
			if (t.x > 0 && t.x < MAXDUNX && t.y > 0 && t.y < MAXDUNY && grid.at(t).dMonster > 0) {
				dir = GetDirection(s, t);
				AddMissile(s, t, dir, MIS_LIGHTCTRL, 0, id, 1, missile[i]._mispllvl);
			}
			l += 2;
		}
	}
	missile[i]._mirange--;
	if (missile[i]._mirange == 0)
		missile[i]._miDelFlag = TRUE;
}

void mi_null_11(int i)
{
	missile[i]._mirange--;
	if (!missile[i]._mirange)
		missile[i]._miDelFlag = TRUE;
	if (missile[i]._miAnimFrame == missile[i]._miAnimLen)
		missile[i]._miPreFlag = TRUE;
	PutMissile(i);
}

void MI_Weapexp(int i)
{
	int id, mind, maxd;
	int ExpLight[10] = { 9, 10, 11, 12, 11, 10, 8, 6, 4, 2 };

	missile[i]._mirange--;
	id = missile[i]._misource;
	if (missile[i]._miVar2 == 1) {
		mind = plr[id].data._pIFMinDam;
		maxd = plr[id].data._pIFMaxDam;
		missiledata[missile[i]._mitype].mResist = MISR_FIRE;
	} else {
		mind = plr[id].data._pILMinDam;
		maxd = plr[id].data._pILMaxDam;
		missiledata[missile[i]._mitype].mResist = MISR_LIGHTNING;
	}
	CheckMissileCol(i, mind, maxd, FALSE, missile[i]._mi, FALSE);
	if (!missile[i]._miVar1) {
		missile[i]._mlid = AddLight(missile[i]._mi, 9);
	} else {
		if (missile[i]._mirange)
			ChangeLight(missile[i]._mlid, missile[i]._mi, ExpLight[missile[i]._miVar1]);
	}
	missile[i]._miVar1++;
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = TRUE;
		AddUnLight(missile[i]._mlid);
	} else {
		PutMissile(i);
	}
}

void MI_Misexp(int i)
{
	int ExpLight[10] = { 9, 10, 11, 12, 11, 10, 8, 6, 4, 2 };

	missile[i]._mirange--;
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = TRUE;
		AddUnLight(missile[i]._mlid);
	} else {
		if (!missile[i]._miVar1)
			missile[i]._mlid = AddLight(missile[i]._mi, 9);
		else
			ChangeLight(missile[i]._mlid, missile[i]._mi, ExpLight[missile[i]._miVar1]);
		missile[i]._miVar1++;
		PutMissile(i);
	}
}

void MI_Acidsplat(int i)
{
	int monst, dam;

	if (missile[i]._mirange == missile[i]._miAnimLen) {
		missile[i]._mi.x++;
		missile[i]._mi.y++;
		missile[i]._mioff.y -= 32;
	}
	missile[i]._mirange--;
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = TRUE;
		monst = missile[i]._misource;
		dam = missile[i]._mispllvl;
		AddMissile(missile[i]._mi, { i, 0 }, missile[i]._mimfnum, MIS_ACIDPUD, 1, monst, (monsters[monst].data.MData->mLevel >= 2) + 1, dam);
	} else {
		PutMissile(i);
	}
}

void MI_Teleport(int i)
{
	int id;

	id = missile[i]._misource;
	missile[i]._mirange--;
	if (missile[i]._mirange <= 0) {
		missile[i]._miDelFlag = TRUE;
	} else {
		grid.at(plr[id].data._p).dPlayer = 0;
		PlrClrTrans(plr[id].data._p);
		plr[id].data._p = missile[i]._mi;
		plr[id].data._pfut = plr[id].data._p;
		plr[id].data._pold = plr[id].data._p;
		PlrDoTrans(plr[id].data._p);
		missile[i]._miVar1 = 1;
		grid.at(plr[id].data._p).dPlayer = id + 1;
		if (level.leveltype != DTYPE_TOWN) {
			ChangeLightXY(plr[id].data._plid, plr[id].data._p);
			ChangeVisionXY(plr[id].data._pvid, plr[id].data._p);
		}
		if (id == myplr()) {
			View = plr[id].data._p - ScrollInfo._sd;
		}
	}
}

void MI_Stone(int i)
{
	int m;

	missile[i]._mirange--;
	m = missile[i]._miVar2;
	if (!monsters[m].data._mhitpoints && missile[i]._miAnimType != MFILE_SHATTER1) {
		missile[i]._mimfnum = 0;
		missile[i]._miDrawFlag = TRUE;
		SetMissAnim(i, MFILE_SHATTER1);
		missile[i]._mirange = 11;
	}
	if (monsters[m].data._mmode != MM_STONE) {
		missile[i]._miDelFlag = TRUE;
		return;
	}

	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = TRUE;
		if (monsters[m].data._mhitpoints > 0) {
			monsters[m].data._mmode = missile[i]._miVar1;
		} else {
			AddDead(monsters[m].data._m, stonendx, (direction)monsters[m].data._mdir);
		}
	}
	if (missile[i]._miAnimType == MFILE_SHATTER1)
		PutMissile(i);
}

void MI_Boom(int i)
{
	missile[i]._mirange--;
	if (!missile[i]._miVar1)
		CheckMissileCol(i, missile[i]._midam, missile[i]._midam, FALSE, missile[i]._mi, TRUE);
	if (missile[i]._miHitFlag == TRUE)
		missile[i]._miVar1 = 1;
	if (!missile[i]._mirange)
		missile[i]._miDelFlag = TRUE;
	PutMissile(i);
}

void MI_Rhino(int i)
{
	V2Di mi, mi2, om;

	int monst = missile[i]._misource;
	if (monsters[monst].data._mmode != MM_CHARGE) {
		missile[i]._miDelFlag = TRUE;
		return;
	}
	GetMissilePos(i);
	mi = missile[i]._mi;
	grid.at(mi).dMonster = 0;
	if (monsters[monst].data._mAi == AI_SNAKE) {
		missile[i]._mitoff.x += 2 * missile[i]._mivel.x;
		missile[i]._mitoff.y += 2 * missile[i]._mivel.y;
		GetMissilePos(i);
		mi2 = missile[i]._mi;
		missile[i]._mitoff -= missile[i]._mivel;
	} else {
		missile[i]._mitoff += missile[i]._mivel;
	}
	GetMissilePos(i);
	om = missile[i]._mi;
	if (!PosOkMonst(monst, missile[i]._mi) || (monsters[monst].data._mAi == AI_SNAKE && !PosOkMonst(monst, mi2))) {
		MissToMonst(i, mi);
		missile[i]._miDelFlag = TRUE;
		return;
	}
	monsters[monst].data._mfut = om;
	grid.at(om).dMonster = -1 - monst;
	monsters[monst].data._m = om;
	monsters[monst].data._mfut.y = om.y;
	monsters[monst].data._mold.y = om.y;
	if (monsters[monst].data._uniqtype != 0)
		ChangeLightXY(missile[i]._mlid, om);
	MoveMissilePos(i);
	PutMissile(i);
}

void mi_null_32(int i)
{
	int src, enemy, j;
	V2Di a, b, c;

	GetMissilePos(i);
	a = missile[i]._mi;
	missile[i]._mitoff += missile[i]._mivel;
	GetMissilePos(i);
	src = missile[i]._misource;
	b = missile[i]._mi;
	enemy = monsters[src].data._menemy;
	if (!(monsters[src].data._mFlags & MFLAG_TARGETS_MONSTER)) {
		c = plr[enemy].data._p;
	} else {
		c = monsters[enemy].data._m;
	}
	if ((b.x != a.x || b.y != a.y) && (missile[i]._miVar1 & 1 && (abs(a.x - c.x) >= 4 || abs(a.y - c.y) >= 4) ||
		missile[i]._miVar2 > 1) && PosOkMonst(missile[i]._misource, a)) {
		MissToMonst(i, a);
		missile[i]._miDelFlag = TRUE;
	} else if (!(monsters[src].data._mFlags & MFLAG_TARGETS_MONSTER)) {
		j = grid.at(b).dPlayer;
	} else {
		j = grid.at(b).dMonster;
	}
	if (!PosOkMissile(b) || j > 0 && !(missile[i]._miVar1 & 1)) {
		missile[i]._mivel.x *= -1;
		missile[i]._mivel.y *= -1;
		missile[i]._mimfnum = opposite[missile[i]._mimfnum];
		missile[i]._miAnimData = monsters[src].data.MType->Anims[MA_WALK].Data[missile[i]._mimfnum];
		missile[i]._miVar2++;
		if (j > 0)
			missile[i]._miVar1 |= 1;
	}
	MoveMissilePos(i);
	PutMissile(i);
}

void MI_FirewallC(int i)
{
	int tx, ty, id;

	id = missile[i]._misource;
	missile[i]._mirange--;
	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = TRUE;
	} else {
		tx = missile[i]._miVar1 + XDirAdd[missile[i]._miVar3];
		ty = missile[i]._miVar2 + YDirAdd[missile[i]._miVar3];
		if (!pieces[grid[missile[i]._miVar1][missile[i]._miVar2].dPiece].nMissileTable && !missile[i]._miVar8 && tx > 0 && tx < MAXDUNX && ty > 0 && ty < MAXDUNY) {
			AddMissile({ missile[i]._miVar1, missile[i]._miVar2 }, { missile[i]._miVar1, missile[i]._miVar2 }, plr[id].data._pdir, MIS_FIREWALL, 0, id, 0, missile[i]._mispllvl);
			missile[i]._miVar1 = tx;
			missile[i]._miVar2 = ty;
		} else {
			missile[i]._miVar8 = 1;
		}
		tx = missile[i]._miVar5 + XDirAdd[missile[i]._miVar4];
		ty = missile[i]._miVar6 + YDirAdd[missile[i]._miVar4];
		if (!pieces[grid[missile[i]._miVar5][missile[i]._miVar6].dPiece].nMissileTable && !missile[i]._miVar7 && tx > 0 && tx < MAXDUNX && ty > 0 && ty < MAXDUNY) {
			AddMissile({ missile[i]._miVar5, missile[i]._miVar6 }, { missile[i]._miVar5, missile[i]._miVar6 }, plr[id].data._pdir, MIS_FIREWALL, 0, id, 0, missile[i]._mispllvl);
			missile[i]._miVar5 = tx;
			missile[i]._miVar6 = ty;
		} else {
			missile[i]._miVar7 = 1;
		}
	}
}

void MI_Infra(int i)
{
	missile[i]._mirange--;
	plr[missile[i]._misource].data._pInfraFlag = TRUE;
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = TRUE;
		CalcPlrItemVals(missile[i]._misource, TRUE);
	}
}

void MI_Apoca(int i)
{
	int j, k, id;
	BOOL exit;

	id = missile[i]._misource;
	exit = FALSE;
	for (j = missile[i]._miVar2; j < missile[i]._miVar3 && !exit; j++) {
		for (k = missile[i]._miVar4; k < missile[i]._miVar5 && !exit; k++) {
			if (grid[k][j].dMonster > 3 && !pieces[grid[k][j].dPiece].nSolidTable) {
				AddMissile({ k, j }, { k, j }, plr[id].data._pdir, MIS_BOOM, 0, id, missile[i]._midam, 0);
				exit = TRUE;
			}
		}
		if (!exit) {
			missile[i]._miVar4 = missile[i]._miVar6;
		}
	}

	if (exit == TRUE) {
		missile[i]._miVar2 = j - 1;
		missile[i]._miVar4 = k;
	} else {
		missile[i]._miDelFlag = TRUE;
	}
}

void MI_Wave(int i)
{
	int sd, dira, dirb;
	V2Di s, na, nb, v;
	int j, id, pn;

	bool f1 = FALSE;
	bool f2 = FALSE;
	assert((DWORD)i < MAXMISSILES);

	id = missile[i]._misource;
	s = missile[i]._mi;
	v.x = missile[i]._miVar1;
	v.y = missile[i]._miVar2;
	sd = GetDirection(s, v);
	dira = (sd - 2) & 7;
	dirb = (sd + 2) & 7;
	na.x = s.x + XDirAdd[sd];
	na.y = s.y + YDirAdd[sd];
	pn = grid.at(na).dPiece;
	assert((DWORD)pn <= MAXTILES);
	if (!pieces[pn].nMissileTable) {
		AddMissile(na, { na.x + XDirAdd[sd], na.y + YDirAdd[sd] }, plr[id].data._pdir, MIS_FIREMOVE, 0, id, 0, missile[i]._mispllvl);
		na += { XDirAdd[dira], YDirAdd[dira] };
		nb.x = s.x + XDirAdd[sd] + XDirAdd[dirb];
		nb.y = s.y + YDirAdd[sd] + YDirAdd[dirb];
		for (j = 0; j < (missile[i]._mispllvl >> 1) + 2; j++) {
			pn = grid.at(na).dPiece; // BUGFIX: dPiece is accessed before check against dungeon size and 0
			assert((DWORD)pn <= MAXTILES);
			if (pieces[pn].nMissileTable || f1 || na.x <= 0 || na.x >= MAXDUNX || na.y <= 0 || na.y >= MAXDUNY) {
				f1 = TRUE;
			} else {
				AddMissile(na, { na.x + XDirAdd[sd], na.y + YDirAdd[sd] }, plr[id].data._pdir, MIS_FIREMOVE, 0, id, 0, missile[i]._mispllvl);
				na += { XDirAdd[dira], YDirAdd[dira] };
			}
			pn = grid.at(nb).dPiece; // BUGFIX: dPiece is accessed before check against dungeon size and 0
			assert((DWORD)pn <= MAXTILES);
			if (pieces[pn].nMissileTable || f2 || nb.x <= 0 || nb.x >= MAXDUNX || nb.y <= 0 || nb.y >= MAXDUNY) {
				f2 = TRUE;
			} else {
				AddMissile(nb, { nb.x + XDirAdd[sd], nb.y + YDirAdd[sd] }, plr[id].data._pdir, MIS_FIREMOVE, 0, id, 0, missile[i]._mispllvl);
				nb += { XDirAdd[dirb], YDirAdd[dirb] };
			}
		}
	}
	missile[i]._mirange--;
	if (missile[i]._mirange == 0)
		missile[i]._miDelFlag = TRUE;
}

void MI_Nova(int i)
{
	int k, id, dir, en, dam;
	V2Di s, s1;

	id = missile[i]._misource;
	dam = missile[i]._midam;
	s = missile[i]._mi;
	if (id != -1) {
		en = 0;
		dir = plr[id].data._pdir;
	} else {
		dir = 0;
		en = 1;
	}
	for (k = 0; k < 23; k++) {
		if (s1.x != vCrawlTable[k][6] || s1.y != vCrawlTable[k][7]) {
			AddMissile(s, { s.x + vCrawlTable[k][6], s.y + vCrawlTable[k][7] }, dir, MIS_LIGHTBALL, en, id, dam, missile[i]._mispllvl);
			AddMissile(s, { s.x - vCrawlTable[k][6], s.y - vCrawlTable[k][7] }, dir, MIS_LIGHTBALL, en, id, dam, missile[i]._mispllvl);
			AddMissile(s, { s.x - vCrawlTable[k][6], s.y + vCrawlTable[k][7] }, dir, MIS_LIGHTBALL, en, id, dam, missile[i]._mispllvl);
			AddMissile(s, { s.x + vCrawlTable[k][6], s.y - vCrawlTable[k][7] }, dir, MIS_LIGHTBALL, en, id, dam, missile[i]._mispllvl);
			s1.x = vCrawlTable[k][6];
			s1.y = vCrawlTable[k][7];
		}
	}
	missile[i]._mirange--;
	if (missile[i]._mirange == 0)
		missile[i]._miDelFlag = TRUE;
}

void MI_Blodboil(int i)
{
	missile[i]._miDelFlag = TRUE;
}

void MI_Flame(int i)
{
	int k;

	missile[i]._mirange--;
	missile[i]._miVar2--;
	k = missile[i]._mirange;
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, TRUE, missile[i]._mi, FALSE);
	if (missile[i]._mirange == 0 && missile[i]._miHitFlag == TRUE)
		missile[i]._mirange = k;
	if (!missile[i]._miVar2)
		missile[i]._miAnimFrame = 20;
	if (missile[i]._miVar2 <= 0) {
		k = missile[i]._miAnimFrame;
		if (k > 11)
			k = 24 - k;
		ChangeLight(missile[i]._mlid, missile[i]._mi, k);
	}
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = TRUE;
		AddUnLight(missile[i]._mlid);
	}
	if (missile[i]._miVar2 <= 0)
		PutMissile(i);
}

void MI_Flamec(int i)
{
	int id, src;

	missile[i]._mirange--;
	src = missile[i]._misource;
	missile[i]._mitoff += missile[i]._mivel;
	GetMissilePos(i);
	if (missile[i]._mi.x != missile[i]._miVar1 || missile[i]._mi.y != missile[i]._miVar2) {
		id = grid.at(missile[i]._mi).dPiece;
		if (!pieces[id].nMissileTable) {
			AddMissile(
			    missile[i]._mi,
			    missile[i]._mis,
			    i,
			    MIS_FLAME,
			    missile[i]._micaster,
			    src,
			    missile[i]._miVar3,
			    missile[i]._mispllvl);
		} else {
			missile[i]._mirange = 0;
		}
		missile[i]._miVar1 = missile[i]._mi.x;
		missile[i]._miVar2 = missile[i]._mi.y;
		missile[i]._miVar3++;
	}
	if (!missile[i]._mirange || missile[i]._miVar3 == 3)
		missile[i]._miDelFlag = TRUE;
}

void MI_Cbolt(int i)
{
	int md;
	int bpath[16] = { -1, 0, 1, -1, 0, 1, -1, -1, 0, 0, 1, 1, 0, 1, -1, 0 };

	missile[i]._mirange--;
	if (missile[i]._miAnimType != MFILE_LGHNING) {
		if (!missile[i]._miVar3) {
			md = (missile[i]._miVar2 + bpath[missile[i]._mirnd]) & 7;
			missile[i]._mirnd = (missile[i]._mirnd + 1) & 0xF;
			GetMissileVel(i, missile[i]._mi, { missile[i]._mi.x + XDirAdd[md], missile[i]._mi.y + YDirAdd[md] }, 8);
			missile[i]._miVar3 = 16;
		} else {
			missile[i]._miVar3--;
		}
		missile[i]._mitoff += missile[i]._mivel;
		GetMissilePos(i);
		CheckMissileCol(i, missile[i]._midam, missile[i]._midam, FALSE, missile[i]._mi, FALSE);
		if (missile[i]._miHitFlag == TRUE) {
			missile[i]._miVar1 = 8;
			missile[i]._mimfnum = 0;
			missile[i]._mioff = { 0, 0 };
			SetMissAnim(i, MFILE_LGHNING);
			missile[i]._mirange = missile[i]._miAnimLen;
			GetMissilePos(i);
		}
		ChangeLight(missile[i]._mlid, missile[i]._mi, missile[i]._miVar1);
	}
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = TRUE;
		AddUnLight(missile[i]._mlid);
	}
	PutMissile(i);
}

void MI_Hbolt(int i)
{
	int dam;

	missile[i]._mirange--;
	if (missile[i]._miAnimType != MFILE_HOLYEXPL) {
		missile[i]._mitoff += missile[i]._mivel;
		GetMissilePos(i);
		dam = missile[i]._midam;
		if (missile[i]._mi.x != missile[i]._mis.x || missile[i]._mi.y != missile[i]._mis.y) {
			CheckMissileCol(i, dam, dam, FALSE, missile[i]._mi, FALSE);
		}
		if (!missile[i]._mirange) {
			missile[i]._mitoff -= missile[i]._mivel;
			GetMissilePos(i);
			missile[i]._mimfnum = 0;
			SetMissAnim(i, MFILE_HOLYEXPL);
			missile[i]._mirange = missile[i]._miAnimLen - 1;
		} else {
			if (missile[i]._mi.x != missile[i]._miVar1 || missile[i]._mi.y != missile[i]._miVar2) {
				missile[i]._miVar1 = missile[i]._mi.x;
				missile[i]._miVar2 = missile[i]._mi.y;
				ChangeLight(missile[i]._mlid, { missile[i]._miVar1, missile[i]._miVar2 }, 8);
			}
		}
	} else {
		ChangeLight(missile[i]._mlid, missile[i]._mi, missile[i]._miAnimFrame + 7);
		if (!missile[i]._mirange) {
			missile[i]._miDelFlag = TRUE;
			AddUnLight(missile[i]._mlid);
		}
	}
	PutMissile(i);
}

void MI_Element(int i)
{
	int mid, sd, dam, id;
	V2Di c, p;

	missile[i]._mirange--;
	dam = missile[i]._midam;
	id = missile[i]._misource;
	if (missile[i]._miAnimType == MFILE_BIGEXP) {
		c = missile[i]._mi;
		p = plr[id].data._p;
		ChangeLight(missile[i]._mlid, c, missile[i]._miAnimFrame);
		if (!CheckBlock(p, c))
			CheckMissileCol(i, dam, dam, TRUE, c, TRUE);
		if (!CheckBlock(p, { c.x, c.y + 1 }))
			CheckMissileCol(i, dam, dam, TRUE, { c.x, c.y + 1 }, TRUE);
		if (!CheckBlock(p, { c.x, c.y - 1 }))
			CheckMissileCol(i, dam, dam, TRUE, { c.x, c.y - 1 }, TRUE);
		if (!CheckBlock(p, { c.x + 1, c.y }))
			CheckMissileCol(i, dam, dam, TRUE, { c.x + 1, c.y }, TRUE); /* check x/y */
		if (!CheckBlock(p, { c.x + 1, c.y - 1 }))
			CheckMissileCol(i, dam, dam, TRUE, { c.x + 1, c.y - 1 }, TRUE);
		if (!CheckBlock(p, { c.x + 1, c.y + 1 }))
			CheckMissileCol(i, dam, dam, TRUE, { c.x + 1, c.y + 1 }, TRUE);
		if (!CheckBlock(p, { c.x - 1, c.y }))
			CheckMissileCol(i, dam, dam, TRUE, { c.x - 1, c.y }, TRUE);
		if (!CheckBlock(p, { c.x - 1, c.y + 1 }))
			CheckMissileCol(i, dam, dam, TRUE, { c.x - 1, c.y + 1 }, TRUE);
		if (!CheckBlock(p, { c.x - 1, c.y - 1 }))
			CheckMissileCol(i, dam, dam, TRUE, { c.x - 1, c.y - 1 }, TRUE);
		if (!missile[i]._mirange) {
			missile[i]._miDelFlag = TRUE;
			AddUnLight(missile[i]._mlid);
		}
	} else {
		missile[i]._mitoff += missile[i]._mivel;
		GetMissilePos(i);
		c = missile[i]._mi;
		CheckMissileCol(i, dam, dam, FALSE, c, FALSE);
		if (!missile[i]._miVar3 && c.x == missile[i]._miVar4 && c.y == missile[i]._miVar5)
			missile[i]._miVar3 = 1;
		if (missile[i]._miVar3 == 1) {
			missile[i]._miVar3 = 2;
			missile[i]._mirange = 255;
			mid = FindClosest(c, 19);
			if (mid > 0) {
				sd = GetDirection8(c, monsters[mid].data._m);
				SetMissDir(i, sd);
				GetMissileVel(i, c, monsters[mid].data._m, 16);
			} else {
				sd = plr[id].data._pdir;
				SetMissDir(i, sd);
				GetMissileVel(i, c, { c.x + XDirAdd[sd], c.y + YDirAdd[sd] }, 16);
			}
		}
		if (c.x != missile[i]._miVar1 || c.y != missile[i]._miVar2) {
			missile[i]._miVar1 = c.x;
			missile[i]._miVar2 = c.y;
			ChangeLight(missile[i]._mlid, c, 8);
		}
		if (!missile[i]._mirange) {
			missile[i]._mimfnum = 0;
			SetMissAnim(i, MFILE_BIGEXP);
			missile[i]._mirange = missile[i]._miAnimLen - 1;
		}
	}
	PutMissile(i);
}

void MI_Bonespirit(int i)
{
	int id, mid, sd, dam;
	V2Di c;

	missile[i]._mirange--;
	dam = missile[i]._midam;
	id = missile[i]._misource;
	if (missile[i]._mimfnum == 8) {
		ChangeLight(missile[i]._mlid, missile[i]._mi, missile[i]._miAnimFrame);
		if (!missile[i]._mirange) {
			missile[i]._miDelFlag = TRUE;
			AddUnLight(missile[i]._mlid);
		}
		PutMissile(i);
	} else {
		missile[i]._mitoff += missile[i]._mivel;
		GetMissilePos(i);
		c = missile[i]._mi;
		CheckMissileCol(i, dam, dam, FALSE, c, FALSE);
		if (missile[i]._miVar3 == 0 && c.x == missile[i]._miVar4 && c.y == missile[i]._miVar5)
			missile[i]._miVar3 = 1;
		if (missile[i]._miVar3 == 1) {
			missile[i]._miVar3 = 2;
			missile[i]._mirange = 255;
			mid = FindClosest(c, 19);
			if (mid > 0) {
				missile[i]._midam = monsters[mid].data._mhitpoints >> 7;
				SetMissDir(i, GetDirection8(c, monsters[mid].data._m));
				GetMissileVel(i, c, monsters[mid].data._m, 16);
			} else {
				sd = plr[id].data._pdir;
				SetMissDir(i, sd);
				GetMissileVel(i, c, { c.x + XDirAdd[sd], c.y + YDirAdd[sd] }, 16);
			}
		}
		if (c.x != missile[i]._miVar1 || c.y != missile[i]._miVar2) {
			missile[i]._miVar1 = c.x;
			missile[i]._miVar2 = c.y;
			ChangeLight(missile[i]._mlid, c, 8);
		}
		if (!missile[i]._mirange) {
			SetMissDir(i, DIR_OMNI);
			missile[i]._mirange = 7;
		}
		PutMissile(i);
	}
}

void MI_ResurrectBeam(int i)
{
	missile[i]._mirange--;
	if (missile[i]._mirange == 0)
		missile[i]._miDelFlag = TRUE;
	PutMissile(i);
}

void MI_Rportal(int i)
{
	int ExpLight[17] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 15, 15 };

	if (missile[i]._mirange > 1)
		missile[i]._mirange--;
	if (missile[i]._mirange == missile[i]._miVar1)
		SetMissDir(i, 1);

	if (level.currlevel && missile[i]._mimfnum != 1 && missile[i]._mirange != 0) {
		if (!missile[i]._miVar2)
			missile[i]._mlid = AddLight(missile[i]._mi, 1);
		ChangeLight(missile[i]._mlid, missile[i]._mi, ExpLight[missile[i]._miVar2]);
		missile[i]._miVar2++;
	}
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = TRUE;
		AddUnLight(missile[i]._mlid);
	}
	PutMissile(i);
}

void ProcessMissiles()
{
	int i, mi;

	for (i = 0; i < nummissiles; i++) {
		grid.at(missile[missileactive[i]]._mi).dFlags &= ~BFLAG_MISSILE;
		grid.at(missile[missileactive[i]]._mi).dMissile = 0;
	}

	i = 0;
	while (i < nummissiles) {
		if (missile[missileactive[i]]._miDelFlag) {
			DeleteMissile(missileactive[i], i);
			i = 0;
		} else {
			i++;
		}
	}

	MissilePreFlag = FALSE;
	ManashieldFlag = FALSE;

	for (i = 0; i < nummissiles; i++) {
		mi = missileactive[i];
		missiledata[missile[mi]._mitype].mProc(missileactive[i]);
		if (!(missile[mi]._miAnimFlags & MFLAG_LOCK_ANIMATION)) {
			missile[mi]._miAnimCnt++;
			if (missile[mi]._miAnimCnt >= missile[mi]._miAnimDelay) {
				missile[mi]._miAnimCnt = 0;
				missile[mi]._miAnimFrame += missile[mi]._miAnimAdd;
				if (missile[mi]._miAnimFrame > missile[mi]._miAnimLen)
					missile[mi]._miAnimFrame = 1;
				if (missile[mi]._miAnimFrame < 1)
					missile[mi]._miAnimFrame = missile[mi]._miAnimLen;
			}
		}
	}

	if (ManashieldFlag) {
		for (i = 0; i < nummissiles; i++) {
			if (missile[missileactive[i]]._mitype == MIS_MANASHIELD) {
				MI_Manashield(missileactive[i]);
			}
		}
	}

	i = 0;
	while (i < nummissiles) {
		if (missile[missileactive[i]]._miDelFlag) {
			DeleteMissile(missileactive[i], i);
			i = 0;
		} else {
			i++;
		}
	}
}

void missiles_process_charge()
{
	CMonster *mon;
	AnimStruct *anim;
	MissileStruct *mis;
	int i, mi;

	for (i = 0; i < nummissiles; i++) {
		mi = missileactive[i];
		mis = &missile[mi];
		mis->_miAnimData = misfiledata[mis->_miAnimType].mAnimData[mis->_mimfnum];
		if (mis->_mitype == MIS_RHINO) {
			mon = monsters[mis->_misource].data.MType;
			if (mon->mtype >= MT_HORNED && mon->mtype <= MT_OBLORD) {
				anim = &mon->Anims[MA_SPECIAL];
			} else {
				if (mon->mtype >= MT_NSNAKE && mon->mtype <= MT_GSNAKE)
					anim = &mon->Anims[MA_ATTACK];
				else
					anim = &mon->Anims[MA_WALK];
			}
			missile[mi]._miAnimData = anim->Data[mis->_mimfnum];
		}
	}
}

void ClearMissileSpot(int mi)
{
	grid.at(missile[mi]._mi).dFlags &= ~BFLAG_MISSILE;
	grid.at(missile[mi]._mi).dMissile = 0;
}

DEVILUTION_END_NAMESPACE
