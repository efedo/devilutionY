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
bool ManashieldFlag;
bool MissilePreFlag;
int numchains;

void GetDamageAmt(int i, int *mind, int *maxd)
{
	int k, sl;

	assert((DWORD)myplr() < MAX_PLRS);
	assert((DWORD)i < 64);
	sl = myplr().data._pSplLvl[i] + myplr().data._pISplLvlAdd;

	switch (i) {
	case SpellId::FIREBOLT:
		*mind = (myplr().data._pMagic >> 3) + sl + 1;
		*maxd = (myplr().data._pMagic >> 3) + sl + 10;
		break;
	case SpellId::HEAL: /// BUGFIX: healing calculation is unused
		*mind = myplr().data._pLevel + sl + 1;
		if (myplr().data._pClass == PlayerClass::warrior) {
			*mind <<= 1;
		}
		if (myplr().data._pClass == PlayerClass::rogue) {
			*mind += *mind >> 1;
		}
		*maxd = 10;
		for (k = 0; k < myplr().data._pLevel; k++) {
			*maxd += 4;
		}
		for (k = 0; k < sl; k++) {
			*maxd += 6;
		}
		if (myplr().data._pClass == PlayerClass::warrior) {
			*maxd <<= 1;
		}
		if (myplr().data._pClass == PlayerClass::rogue) {
			*maxd += *maxd >> 1;
		}
		*mind = -1;
		*maxd = -1;
		break;
	case SpellId::LIGHTNING:
		*mind = 2;
		*maxd = myplr().data._pLevel + 2;
		break;
	case SpellId::FLASH:
		*mind = myplr().data._pLevel;
		for (k = 0; k < sl; k++) {
			*mind += *mind >> 3;
		}
		*mind += *mind >> 1;
		*maxd = *mind * 2;
		break;
	case SpellId::IDENTIFY:
	case SpellId::TOWN:
	case SpellId::STONE:
	case SpellId::INFRA:
	case SpellId::RNDTELEPORT:
	case SpellId::MANASHIELD:
	case SpellId::DOOMSERP:
	case SpellId::BLODRIT:
	case SpellId::INVISIBIL:
	case SpellId::BLODBOIL:
	case SpellId::TELEPORT:
	case SpellId::ETHEREALIZE:
	case SpellId::REPAIR:
	case SpellId::RECHARGE:
	case SpellId::DISARM:
	case SpellId::RESURRECT:
	case SpellId::TELEKINESIS:
	case SpellId::BONESPIRIT:
		*mind = -1;
		*maxd = -1;
		break;
	case SpellId::FIREWALL:
		*mind = (4 * myplr().data._pLevel + 8) >> 1;
		*maxd = (4 * myplr().data._pLevel + 80) >> 1;
		break;
	case SpellId::FIREBALL:
		*mind = 2 * myplr().data._pLevel + 4;
		for (k = 0; k < sl; k++) {
			*mind += *mind >> 3;
		}
		*maxd = 2 * myplr().data._pLevel + 40;
		for (k = 0; k < sl; k++) {
			*maxd += *maxd >> 3;
		}
		break;
	case SpellId::GUARDIAN:
		*mind = (myplr().data._pLevel >> 1) + 1;
		for (k = 0; k < sl; k++) {
			*mind += *mind >> 3;
		}
		*maxd = (myplr().data._pLevel >> 1) + 10;
		for (k = 0; k < sl; k++) {
			*maxd += *maxd >> 3;
		}
		break;
	case SpellId::CHAIN:
		*mind = 4;
		*maxd = 2 * myplr().data._pLevel + 4;
		break;
	case SpellId::WAVE:
		*mind = 6 * (myplr().data._pLevel + 1);
		*maxd = 6 * (myplr().data._pLevel + 10);
		break;
	case SpellId::NOVA:
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
	case SpellId::FLAME:
		*mind = 3;
		*maxd = myplr().data._pLevel + 4;
		*maxd += *maxd >> 1;
		break;
	case SpellId::GOLEM:
		*mind = 11;
		*maxd = 17;
		break;
	case SpellId::APOCA:
		*mind = 0;
		for (k = 0; k < myplr().data._pLevel; k++) {
			*mind += 1;
		}
		*maxd = 0;
		for (k = 0; k < myplr().data._pLevel; k++) {
			*maxd += 6;
		}
		break;
	case SpellId::ELEMENT:
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
	case SpellId::CBOLT:
		*mind = 1;
		*maxd = (myplr().data._pMagic >> 2) + 1;
		break;
	case SpellId::HBOLT:
		*mind = myplr().data._pLevel + 9;
		*maxd = myplr().data._pLevel + 18;
		break;
	case SpellId::HEALOTHER: /// BUGFIX: healing calculation is unused
		*mind = myplr().data._pLevel + sl + 1;
		if (myplr().data._pClass == PlayerClass::warrior) {
			*mind <<= 1;
		}
		if (myplr().data._pClass == PlayerClass::rogue) {
			*mind += *mind >> 1;
		}
		*maxd = 10;
		for (k = 0; k < myplr().data._pLevel; k++) {
			*maxd += 4;
		}
		for (k = 0; k < sl; k++) {
			*maxd += 6;
		}
		if (myplr().data._pClass == PlayerClass::warrior) {
			*maxd <<= 1;
		}
		if (myplr().data._pClass == PlayerClass::rogue) {
			*maxd += *maxd >> 1;
		}
		*mind = -1;
		*maxd = -1;
		break;
	case SpellId::FLARE:
		*mind = (myplr().data._pMagic >> 1) + 3 * sl - (myplr().data._pMagic >> 3);
		*maxd = *mind;
		break;
	}
}

bool CheckBlock(V2Di f, V2Di t)
{
	bool coll = false;
	while (f != t) {
		Dir pn = GetDirection(f, t);
		f += offset(pn);
		if (grid.at(f).isSolid()) coll = true;
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
		for (j = (uint8_t)CrawlTable[CrawlNum[i]]; j > 0; j--) {
			t.x = s.x + CrawlTable[cr - 1];
			t.y = s.y + CrawlTable[cr];
			if (t.x > 0 && t.x < MAXDUNX && t.y > 0 && t.y < MAXDUNY) {
				mid = grid.at(t).getActor();
				if (mid > 0 && !CheckBlock(s, t))
					return mid - 1;
			}
			cr += 2;
		}
	}
	return -1;
}

Dir GetDirection8(V2Di t1, V2Di t2)
{
	static uint8_t Dirs[16][16] = {
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
	ALIGN_BY_1 uint8_t urtoll[] = { 3, 4, 5 },
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
	return Dir(md);
}

Dir16 GetDirection16(V2Di t1, V2Di t2)
{
	static uint8_t Dirs[16][16] = {
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
	uint8_t urtoll[5] = { 6, 7, 8, 9, 10 };
	uint8_t ultolr[5] = { 6, 5, 4, 3, 2 };
	uint8_t lltour[5] = { 14, 13, 12, 11, 10 };
	uint8_t lrtoul[5] = { 14, 15, 0, 1, 2 };
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
	return Dir16(md);
}

void DeleteMissile(int mi, int i)
{
	int src;

	if (missile[mi]._mitype == MissileType::MANASHIELD) {
		src = missile[mi]._misource;
		if (src == myplr())
			NetSendCmd(true, CMD_REMSHIELD);
		plr[src].data.pManaShield = false;
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
		missile[i]._miDelFlag = true;
	if (!missile[i]._miDelFlag) {
		grid.at(pos).dFlags |= DunTileFlag::MISSILE;
		grid.at(pos).setMissile(i);
		if (missile[i]._miPreFlag)
			MissilePreFlag = true;
	}
}

void GetMissilePos(int i)
{
	V2Di d, l;
	V2Di m = missile[i]._mitoff >> 16;
	d.x = 2 * m.y + m.x;
	d.y = 2 * m.y - m.x;
	if (d.x < 0) {
		l.x = -(-d.x / 8);
		d.x = -(-d.x / 64);
	} else {
		l.x = d.x / 8;
		d.x = d.x / 64;
	}
	if (d.y < 0) {
		l.y = -(-d.y / 8);
		d.y = -(-d.y / 64);
	} else {
		l.y = d.y / 8;
		d.y = d.y / 64;
	}
	missile[i]._mi = d + missile[i]._mis;
	missile[i]._mioff.x = m.x + (d.y * 32) - (d.x * 32);
	missile[i]._mioff.y = m.y - (d.x * 16) - (d.y * 16);
	ChangeLightOff(missile[i]._mlid, { l.x - (d.x * 8), l.y - (d.y * 8) });
}

void MoveMissilePos(int i)
{
	static V2Di misoffsets[8] = {
		{ 1, 1 },
		{ 1, 1 },
		{ 0, 1 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 1, 0 },
		{ 1, 1 }
	};
	V2Di d = misoffsets[int(missile[i]._mimfnum)];
	V2Di pos = missile[i]._mi + d;
	if (PosOkMonst(missile[i]._misource, pos)) {
		missile[i]._mi += d;
		missile[i]._mioff.x += (d.y << 5) - (d.x << 5);
		missile[i]._mioff.y -= (d.y << 4) + (d.x << 4);
	}
}

bool MonsterTrapHit(int m, int mindam, int maxdam, int dist, int t, bool shift)
{
	int hit, hper, dam, mor, mir;
	bool resist, ret;

	resist = false;
	if (monsters[m].data.mtalkmsg) {
		return false;
	}
	if (monsters[m].data._mhitpoints >> 6 <= 0) {
		return false;
	}
	if (monsters[m].data.MType->mtype == MonsterType::ILLWEAV && monsters[m].data._mgoal == MonsterGoal::RETREAT)
		return false;
	if (monsters[m].data._mmode == MonsterMode::CHARGE)
		return false;

	mir = missiledata[t].mResist;
	mor = monsters[m].data.mMagicRes;
	if (mor & MonsterResist::imune_magic && mir == MissileResistance::MAGIC
	    || mor & MonsterResist::imune_fire && mir == MissileResistance::FIRE
	    || mor & MonsterResist::imune_lightning && mir == MissileResistance::LIGHTNING) {
		return false;
	}

	if ((mor & MonsterResist::resist_magic && mir == MissileResistance::MAGIC)
	    || (mor & MonsterResist::resist_fire && mir == MissileResistance::FIRE)
	    || (mor & MonsterResist::resist_lightning && mir == MissileResistance::LIGHTNING)) {
		resist = true;
	}

	hit = random_(68, 100);
	hper = 90 - (uint8_t)monsters[m].data.mArmorClass - dist;
	if (hper < 5)
		hper = 5;
	if (hper > 95)
		hper = 95;
	if (CheckMonsterHit(m, &ret)) {
		return ret;
	}
	#ifdef _DEBUG
	else if (hit < hper || debug_mode_dollar_sign || debug_mode_key_inverted_v || monsters[m].data._mmode == MonsterMode::STONE) {
	#else
	else if (hit < hper || monsters[m].data._mmode == MonsterMode::STONE) {
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
			if (monsters[m].data._mmode == MonsterMode::STONE) {
				monsters[m].M_StartKill(-1);
				monsters[m].data._mmode = MonsterMode::STONE;
			} else {
				monsters[m].M_StartKill(-1);
			}
		} else {
			if (resist) {
				PlayEffect(m, 1);
			} else if (monsters[m].data._mmode == MonsterMode::STONE) {
				if (m > 3)
					monsters[m].M_StartHit(-1, dam);
				monsters[m].data._mmode = MonsterMode::STONE;
			} else {
				if (m > 3)
					monsters[m].M_StartHit(-1, dam);
			}
		}
		return true;
	} else {
		return false;
	}
}

bool MonsterMHit(int pnum, int m, int mindam, int maxdam, int dist, int t, bool shift)
{
	int hit, hper, dam, mor, mir;
	bool resist, ret;

	resist = false;
	if (monsters[m].data.mtalkmsg
	    || monsters[m].data._mhitpoints >> 6 <= 0
	    || t == MissileType::HBOLT && monsters[m].data.MType->mtype != MonsterType::DIABLO && monsters[m].data.MData->mMonstClass) {
		return false;
	}
	if (monsters[m].data.MType->mtype == MonsterType::ILLWEAV && monsters[m].data._mgoal == MonsterGoal::RETREAT)
		return false;
	if (monsters[m].data._mmode == MonsterMode::CHARGE)
		return false;

	mor = monsters[m].data.mMagicRes;
	mir = missiledata[t].mResist;

	if (mor & MonsterResist::imune_magic && mir == MissileResistance::MAGIC
	    || mor & MonsterResist::imune_fire && mir == MissileResistance::FIRE
	    || mor & MonsterResist::imune_lightning && mir == MissileResistance::LIGHTNING
	    || (mor & MonsterResist::imune_acid) && mir == MissileResistance::ACID)
		return false;

	if (mor & MonsterResist::resist_magic && mir == MissileResistance::MAGIC
	    || mor & MonsterResist::resist_fire && mir == MissileResistance::FIRE
	    || mor & MonsterResist::resist_lightning && mir == MissileResistance::LIGHTNING)
		resist = true;

	hit = random_(69, 100);
	if (!missiledata[t].mType) {
		hper = plr[pnum].data._pDexterity
		    + plr[pnum].data._pIBonusToHit
		    + plr[pnum].data._pLevel
		    - monsters[m].data.mArmorClass
		    - (dist * dist >> 1)
		    + plr[pnum].data._pIEnAc
		    + 50;
		if (plr[pnum].data._pClass == PlayerClass::rogue)
			hper += 20;
		if (plr[pnum].data._pClass == PlayerClass::warrior)
			hper += 10;
	} else {
		hper = plr[pnum].data._pMagic - (monsters[m].data.mLevel << 1) - dist + 50;
		if (plr[pnum].data._pClass == PlayerClass::sorceror)
			hper += 20;
	}
	if (hper < 5)
		hper = 5;
	if (hper > 95)
		hper = 95;
	if (monsters[m].data._mmode == MonsterMode::STONE)
		hit = 0;
	if (CheckMonsterHit(m, &ret))
		return ret;
#ifdef _DEBUG
	if (hit >= hper && !debug_mode_key_inverted_v && !debug_mode_dollar_sign)
		return false;
#else
	if (hit >= hper)
		return false;
#endif
	if (t == MissileType::BONESPIRIT) {
		dam = monsters[m].data._mhitpoints / 3 >> 6;
	} else {
		dam = mindam + random_(70, maxdam - mindam + 1);
	}
	if (!missiledata[t].mType) {
		dam = plr[pnum].data._pIBonusDamMod + dam * plr[pnum].data._pIBonusDam / 100 + dam;
		if (plr[pnum].data._pClass == PlayerClass::rogue)
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
	if (plr[pnum].data._pIFlags & ItemSpecialEffect::FIRE_ARROWS)
		monsters[m].data._mFlags |= MonsterFlag::noheal;

	if (monsters[m].data._mhitpoints >> 6 <= 0) {
		if (monsters[m].data._mmode == MonsterMode::STONE) {
			monsters[m].M_StartKill(pnum);
			monsters[m].data._mmode = MonsterMode::STONE;
		} else {
			monsters[m].M_StartKill(pnum);
		}
	} else {
		if (resist) {
			PlayEffect(m, 1);
		} else if (monsters[m].data._mmode == MonsterMode::STONE) {
			if (m > 3)
				monsters[m].M_StartHit(pnum, dam);
			monsters[m].data._mmode = MonsterMode::STONE;
		} else {
			if (!missiledata[t].mType && plr[pnum].data._pIFlags & ItemSpecialEffect::KNOCKBACK) {
				monsters[m].M_GetKnockback();
			}
			if (m > 3)
				monsters[m].M_StartHit(pnum, dam);
		}
	}

	if (!monsters[m].data._msquelch) {
		monsters[m].data._msquelch = UINT8_MAX;
		monsters[m].data._last = plr[pnum].pos();
	}
	return true;
}

bool PlayerMHit(int pnum, int m, int dist, int mind, int maxd, int mtype, bool shift, int earflag)
{
	int tac, hit, hper, dam, blk, blkper, resper;

	if (plr[pnum].data._pHitPoints >> 6 <= 0) {
		return false;
	}

	if (plr[pnum].data._pInvincible) {
		return false;
	}

	if (plr[pnum].data._pSpellFlags & 1 && !missiledata[mtype].mType) {
		return false;
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
	if (lvl.currlevel == 14 && hper < 20) {
		hper = 20;
	}
	if (lvl.currlevel == 15 && hper < 25) {
		hper = 25;
	}
	if (lvl.currlevel == 16 && hper < 30) {
		hper = 30;
	}

	if ((plr[pnum].data._pmode == PlayerMode::STAND || plr[pnum].data._pmode == PlayerMode::ATTACK) && plr[pnum].data._pBlockFlag) {
		blk = random_(73, 100);
	} else {
		blk = 100;
	}

	if (shift == true)
		blk = 100;
	if (mtype == MissileType::ACIDPUD)
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
	case MissileResistance::FIRE:
		resper = plr[pnum].data._pFireResist;
		break;
	case MissileResistance::LIGHTNING:
		resper = plr[pnum].data._pLghtResist;
		break;
	case MissileResistance::MAGIC:
	case MissileResistance::ACID:
		resper = plr[pnum].data._pMagResist;
		break;
	default:
		resper = 0;
		break;
	}

	if (hit < hper) {
		if (mtype == MissileType::BONESPIRIT) {
			dam = plr[pnum].data._pHitPoints / 3;
		} else {
			if (shift == false) {

				dam = (mind << 6) + random_(75, (maxd - mind + 1) << 6);
				if (m == -1 && plr[pnum].data._pIFlags & ItemSpecialEffect::ABSHALFTRAP)
					dam >>= 1;
				dam += (plr[pnum].data._pIGetHit * 64);
			} else {
				dam = mind + random_(75, maxd - mind + 1);
				if (m == -1 && plr[pnum].data._pIFlags & ItemSpecialEffect::ABSHALFTRAP)
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
				if (plr[pnum].data._pClass == PlayerClass::warrior) {
					PlaySfxLoc(PS_WARR69, plr[pnum].pos());
				} else if (plr[pnum].data._pClass == PlayerClass::rogue) {
					PlaySfxLoc(PS_ROGUE69, plr[pnum].pos());
				} else if (plr[pnum].data._pClass == PlayerClass::sorceror) {
					PlaySfxLoc(PS_MAGE69, plr[pnum].pos());
				}
				drawhpflag = true;
			}
		} else {
			if (blk < blkper) {
				Dir tacb;
				if (m != -1) {
					tacb = GetDirection(plr[pnum].pos(), monsters[m].data._m);
				} else {
					tacb = plr[pnum].data._pdir;
				}
				plr[pnum].StartPlrBlock(tacb);
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
					plr[pnum].StartPlrHit(dam, false);
				}
			}
		}
		return true;
	}
	return false;
}

bool Plr2PlrMHit(int pnum, int p, int mindam, int maxdam, int dist, int mtype, bool shift)
{
	int tac, resper, dam, blk, blkper, hper, hit;

	if (plr[p].data._pInvincible) {
		return false;
	}

	if (mtype == MissileType::HBOLT) {
		return false;
	}

	if (plr[p].data._pSpellFlags & 1 && !missiledata[mtype].mType) {
		return false;
	}

	switch (missiledata[mtype].mResist) {
	case MissileResistance::FIRE:
		resper = plr[p].data._pFireResist;
		break;
	case MissileResistance::LIGHTNING:
		resper = plr[p].data._pLghtResist;
		break;
	case MissileResistance::MAGIC:
	case MissileResistance::ACID:
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
		if (plr[pnum].data._pClass == PlayerClass::rogue)
			hit += 20;
		if (plr[pnum].data._pClass == PlayerClass::warrior)
			hit += 10;
	} else {
		hit = plr[pnum].data._pMagic
		    - (plr[p].data._pLevel << 1)
		    - dist
		    + 50;
		if (plr[pnum].data._pClass == PlayerClass::sorceror)
			hit += 20;
	}
	if (hit < 5)
		hit = 5;
	if (hit > 95)
		hit = 95;
	if (hper < hit) {
		if ((plr[p].data._pmode == PlayerMode::STAND || plr[p].data._pmode == PlayerMode::ATTACK) && plr[p].data._pBlockFlag) {
			blkper = random_(73, 100);
		} else {
			blkper = 100;
		}
		if (shift == true)
			blkper = 100;
		blk = plr[p].data._pDexterity + plr[p].data._pBaseToBlk + (plr[p].data._pLevel << 1) - (plr[pnum].data._pLevel << 1);

		if (blk < 0) {
			blk = 0;
		}
		if (blk > 100) {
			blk = 100;
		}

		if (mtype == MissileType::BONESPIRIT) {
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
				NetSendCmdDamage(true, p, dam - resper * dam / 100);
			if (plr[pnum].data._pClass == PlayerClass::warrior) {
				tac = PS_WARR69;
			} else if (plr[pnum].data._pClass == PlayerClass::rogue) {
				tac = PS_ROGUE69;
			} else if (plr[pnum].data._pClass == PlayerClass::sorceror) {
				tac = PS_MAGE69;
			} else {
				return true;
			}
			PlaySfxLoc(tac, plr[pnum].pos());
		} else {
			if (blkper < blk) {
				plr[pnum].StartPlrBlock(GetDirection(plr[p].pos(), plr[pnum].pos()));
			} else {
				if (pnum == myplr())
					NetSendCmdDamage(true, p, dam);
				plr[p].StartPlrHit(dam, false);
			}
		}
		return true;
	}
	return false;
}

void CheckMissileCol(int i, int mindam, int maxdam, bool shift, V2Di m, bool nodel)
{
	int oi;

	if (missile[i]._miAnimType != MissileGraphic::FIREWAL && missile[i]._misource != -1) {
		if (!missile[i]._micaster) {
			if (grid.at(m).getActor() > 0) {
				if (MonsterMHit(
				        missile[i]._misource,
				        grid.at(m).getActor() - 1,
				        mindam,
				        maxdam,
				        missile[i]._midist,
				        missile[i]._mitype,
				        shift)) {
					if (!nodel)
						missile[i]._mirange = 0;
					missile[i]._miHitFlag = true;
				}
			} else {
				if (grid.at(m).getActor() < 0
				    && monsters[-(grid.at(m).getActor() + 1)].data._mmode == MonsterMode::STONE
				    && MonsterMHit(
				           missile[i]._misource,
						   -(grid.at(m).getActor() + 1),
				           mindam,
				           maxdam,
				           missile[i]._midist,
				           missile[i]._mitype,
				           shift)) {
					if (!nodel)
						missile[i]._mirange = 0;
					missile[i]._miHitFlag = true;
				}
			}
			if (grid.at(m).isPlayer()
			    && grid.at(m).getPlayer() != missile[i]._misource
			    && Plr2PlrMHit(
			           missile[i]._misource,
					   grid.at(m).getPlayer(),
			           mindam,
			           maxdam,
			           missile[i]._midist,
			           missile[i]._mitype,
			           shift)) {
				if (!nodel)
					missile[i]._mirange = 0;
				missile[i]._miHitFlag = true;
			}
		} else {
			if (monsters[missile[i]._misource].data._mFlags & MonsterFlag::targets_monster
			    && grid.at(m).getActor() > 0
			    && monsters[grid.at(m).getActor() - 1].data._mFlags & MonsterFlag::golem
			    && MonsterTrapHit(grid.at(m).getActor() - 1, mindam, maxdam, missile[i]._midist, missile[i]._mitype, shift)) {
				if (!nodel)
					missile[i]._mirange = 0;
				missile[i]._miHitFlag = true;
			}
			if (grid.at(m).isPlayer()
			    && PlayerMHit(
			           grid.at(m).getPlayer(),
			           missile[i]._misource,
			           missile[i]._midist,
			           mindam,
			           maxdam,
			           missile[i]._mitype,
			           shift,
			           0)) {
				if (!nodel)
					missile[i]._mirange = 0;
					missile[i]._miHitFlag = true;
			}
		}
	} else {
		if (grid.at(m).getActor() > 0) {
			if (missile[i]._miAnimType == MissileGraphic::FIREWAL) {
				if (MonsterMHit(
				        missile[i]._misource,
				        grid.at(m).getActor() - 1,
				        mindam,
				        maxdam,
				        missile[i]._midist,
				        missile[i]._mitype,
				        shift)) {
					if (!nodel)
						missile[i]._mirange = 0;
					missile[i]._miHitFlag = true;
				}
			} else if (MonsterTrapHit(grid.at(m).getActor() - 1, mindam, maxdam, missile[i]._midist, missile[i]._mitype, shift)) {
				if (!nodel)
					missile[i]._mirange = 0;
				missile[i]._miHitFlag = true;
			}
		}
		if (grid.at(m).isPlayer()
		    && PlayerMHit(grid.at(m).getPlayer(), -1, missile[i]._midist, mindam, maxdam, missile[i]._mitype, shift, missile[i]._miAnimType == MissileGraphic::FIREWAL)) {
			if (!nodel)
				missile[i]._mirange = 0;
			missile[i]._miHitFlag = true;
		}
	}
	if (grid.at(m).isObject()) {
		oi = grid.at(m).getObject();
		if (!object[oi]._oMissFlag) {
			if (object[oi]._oBreak == 1)
				BreakObject(-1, oi);
			if (!nodel)
				missile[i]._mirange = 0;
			missile[i]._miHitFlag = false;
		}
	}
	if (!grid.at(m).blocksMissile()) {
		if (!nodel)
			missile[i]._mirange = 0;
		missile[i]._miHitFlag = false;
	}
	if (!missile[i]._mirange && missiledata[missile[i]._mitype].miSFX != -1)
		PlaySfxLoc(missiledata[missile[i]._mitype].miSFX, missile[i]._mi);
}

void SetMissAnim(int mi, int animtype)
{
	Dir dir = missile[mi]._mimfnum;

	if (animtype > MissileGraphic::NULL) { animtype = MissileGraphic::NULL; }

	missile[mi]._miAnimType = animtype;
	missile[mi]._miAnimFlags = misfiledata[animtype].mFlags;
	missile[mi]._miAnimData = misfiledata[animtype].mAnimData[int(dir)];
	missile[mi]._miAnimDelay = misfiledata[animtype].mAnimDelay[int(dir)];
	missile[mi]._miAnimLen = misfiledata[animtype].mAnimLen[int(dir)];
	missile[mi]._miAnimWidth = misfiledata[animtype].mAnimWidth[int(dir)];
	missile[mi]._miAnimWidth2 = misfiledata[animtype].mAnimWidth2[int(dir)];
	missile[mi]._miAnimCnt = 0;
	missile[mi]._miAnimFrame = 1;
}

void SetMissDir(int mi, Dir dir)
{
	SetMissDir(mi, dir);
}

void LoadMissileGFX(uint8_t mi)
{
	char pszName[256];
	int i;
	uint8_t *file;
	MisFileData *mfd;

	mfd = &misfiledata[mi];
	if (mfd->mFlags & MonsterFlag::allow_special) {
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
		if (!(misfiledata[mi].mFlags & MonsterFlag::hidden))
			LoadMissileGFX(mi);
	}
}

void FreeMissileGFX(int mi)
{
	int i;
	DWORD *p;

	if (misfiledata[mi].mFlags & MonsterFlag::allow_special) {
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
		if (!(misfiledata[mi].mFlags & MonsterFlag::hidden))
			FreeMissileGFX(mi);
	}
}

void FreeMissiles2()
{
	int mi;

	for (mi = 0; misfiledata[mi].mAnimFAmt; mi++) {
		if (misfiledata[mi].mFlags & MonsterFlag::hidden)
			FreeMissileGFX(mi);
	}
}

void InitMissiles()
{
	int mi, src, i, j;

	myplr().data._pSpellFlags &= ~0x1;
	if (myplr().data._pInfraFlag == true) {
		for (i = 0; i < nummissiles; ++i) {
			mi = missileactive[i];
			if (missile[mi]._mitype == MissileType::INFRA) {
				src = missile[mi]._misource;
				if (src == myplr())
					CalcPlrItemVals(src, true);
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
			grid[i][j].dFlags &= ~DunTileFlag::MISSILE;
		}
	}
}

void AddLArrow(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	if (s == d) d += offset(midir);
	if (!mienemy) {
		if (plr[id].data._pClass == PlayerClass::rogue)
			GetMissileVel(mi, s, d, (plr[id].data._pLevel >> 2) + 31);
		else if (plr[id].data._pClass == PlayerClass::warrior)
			GetMissileVel(mi, s, d, (plr[id].data._pLevel >> 3) + 31);
		else
			GetMissileVel(mi, s, d, 32);
	} else
		GetMissileVel(mi, s, d, 32);

	SetMissDir(mi, GetDirection16(s, d).to8());
	missile[mi]._mirange = 256;
	missile[mi]._miVar1 = s.x;
	missile[mi]._miVar2 = s.y;
	missile[mi]._mlid = AddLight(s, 5);
}

void AddArrow(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	if (s == d) d += offset(midir);
	if (!mienemy) {
		int av = 32;
		if (plr[id].data._pIFlags & ItemSpecialEffect::RNDARROWVEL) {
			av = random_(64, 32) + 16;
		}
		if (plr[id].data._pClass == PlayerClass::rogue)
			av += (plr[id].data._pLevel - 1) >> 2;
		if (plr[id].data._pClass == PlayerClass::warrior)
			av += (plr[id].data._pLevel - 1) >> 3;
		GetMissileVel(mi, s, d, av);
	} else {
		GetMissileVel(mi, s, d, 32);
	}
	missile[mi]._miAnimFrame = int(GetDirection16(s, d)) + 1;
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

void AddRndTeleport(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	int r1, r2, nTries;
	Tile * tile;

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

		tile = &grid[r1 + s.x][s.y + r2];
	} while (tile->isSolid() || tile->isObject() || tile->isActor());

	missile[mi]._mirange = 2;
	missile[mi]._miVar1 = 0;
	if (!lvl.setlevel || lvl.setlvlnum != SetLvl::VileBetrayer) {
		missile[mi]._mi = { s.x + r1, s.y + r2 };
		if (!mienemy)
			UseMana(id, SpellId::RNDTELEPORT);
	} else {
		int pn = grid.at(d).getObject();
		if (object[pn]._otype == ObjectType::MCIRCLE1 || object[pn]._otype == ObjectType::MCIRCLE2) {
			missile[mi]._mi = d;
			if (!PosOkPlayer(myplr(), d))
				GetVileMissPos(mi, d);
		}
	}
}

void AddFirebolt(int mi, V2Di s, V2Di d, Dir midir, char micaster, int id, int dam)
{
	int i, mx, sp;

	if (s == d) {
		d += offset(midir);
	}
	if (!micaster) {
		for (i = 0; i < nummissiles; i++) {
			mx = missileactive[i];
			if (missile[mx]._mitype == MissileType::GUARDIAN && missile[mx]._misource == id && missile[mx]._miVar3 == mi)
				break;
		}
		if (i == nummissiles)
			UseMana(id, SpellId::FIREBOLT);
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
	SetMissDir(mi, GetDirection16(s, d).to8());
	missile[mi]._mirange = 256;
	missile[mi]._miVar1 = s.x;
	missile[mi]._miVar2 = s.y;
	missile[mi]._mlid = AddLight(s, 8);
}

void AddMagmaball(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
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

void miss_null_33(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	GetMissileVel(mi, s, d, 16);
	missile[mi]._mirange = 256;
	missile[mi]._miVar1 = s.x;
	missile[mi]._miVar2 = s.y;
	PutMissile(mi);
}

void AddTeleport(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	int i, pn, k, j;
	V2Di t;
	int CrawlNum[6] = { 0, 3, 12, 45, 94, 159 };

	missile[mi]._miDelFlag = true;
	for (i = 0; i < 6; i++) {
		k = CrawlNum[i];
		pn = k + 2;
		for (j = (uint8_t)CrawlTable[k]; j > 0; j--) {
			t.x = d.x + CrawlTable[pn - 1];
			t.y = d.y + CrawlTable[pn];
			if (0 < t.x && t.x < MAXDUNX && 0 < t.y && t.y < MAXDUNY) {
				if (!(grid.at(t).isSolid() | grid.at(t).isActor() | grid.at(t).isObject() | grid.at(t).isPlayer())) {
					missile[mi]._mi = t;
					missile[mi]._mis = t;
					missile[mi]._miDelFlag = false;
					i = 6;
					break;
				}
			}
			pn += 2;
		}
	}

	if (!missile[mi]._miDelFlag) {
		UseMana(id, SpellId::TELEPORT);
		missile[mi]._mirange = 2;
	}
}

void AddLightball(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	GetMissileVel(mi, s, d, 16);
	missile[mi]._midam = dam;
	missile[mi]._miAnimFrame = random_(63, 8) + 1;
	missile[mi]._mirange = 255;
	if (id < 0) {
		missile[mi]._miVar1 = s.x;
		missile[mi]._miVar2 = s.y;
	} else {
		missile[mi]._miVar1 = plr[id].pos().x;
		missile[mi]._miVar2 = plr[id].pos().y;
	}
}

void AddFirewall(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
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

void AddFireball(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	int i;

	if (s == d) {
		d += offset(midir);
	}
	if (!mienemy) {
		missile[mi]._midam = 2 * (plr[id].data._pLevel + random_(60, 10) + random_(60, 10)) + 4;
		for (i = missile[mi]._mispllvl; i > 0; i--) {
			missile[mi]._midam += missile[mi]._midam >> 3;
		}
		i = 2 * missile[mi]._mispllvl + 16;
		if (i > 50)
			i = 50;
		UseMana(id, SpellId::FIREBALL);
	} else {
		i = 16;
	}
	GetMissileVel(mi, s, d, i);
	SetMissDir(mi, GetDirection16(s, d).to8());
	missile[mi]._mirange = 256;
	missile[mi]._miVar1 = s.x;
	missile[mi]._miVar2 = s.y;
	missile[mi]._miVar3 = 0;
	missile[mi]._miVar4 = s.x;
	missile[mi]._miVar5 = s.y;
	missile[mi]._mlid = AddLight(s, 8);
}

void AddLightctrl(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	if (!dam && !mienemy)
		UseMana(id, SpellId::LIGHTNING);
	missile[mi]._miVar1 = s.x;
	missile[mi]._miVar2 = s.y;
	GetMissileVel(mi, s, d, 32);
	missile[mi]._miAnimFrame = random_(52, 8) + 1;
	missile[mi]._mirange = 256;
}

void AddLightning(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	missile[mi]._mis = d;
	if (int(midir) >= 0) {
		missile[mi]._mioff = missile[int(midir)]._mioff;
		missile[mi]._mitoff = missile[int(midir)]._mitoff;
	}
	missile[mi]._miAnimFrame = random_(52, 8) + 1;

	if (midir == Dir::NONE || mienemy == 1 || id == -1) {
		if (midir == Dir::NONE || id == -1)
			missile[mi]._mirange = 8;
		else
			missile[mi]._mirange = 10;
	} else {
		missile[mi]._mirange = (missile[mi]._mispllvl >> 1) + 6;
	}
	missile[mi]._mlid = AddLight(missile[mi]._mi, 4);
}

void AddMisexp(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	CMonster *mon;

	if (mienemy && id > 0) {
		mon = monsters[id].data.MType;
		if (mon->mtype == MonsterType::SUCCUBUS)
			SetMissAnim(mi, MissileGraphic::FLAREEXP);
		if (mon->mtype == MonsterType::SNOWWICH)
			SetMissAnim(mi, MissileGraphic::SCBSEXPB);
		if (mon->mtype == MonsterType::HLSPWN)
			SetMissAnim(mi, MissileGraphic::SCBSEXPD);
		if (mon->mtype == MonsterType::SOLBRNR)
			SetMissAnim(mi, MissileGraphic::SCBSEXPC);
	}

	missile[mi]._mi = missile[d.x]._mi;
	missile[mi]._mis = missile[d.x]._mis;
	missile[mi]._mioff = missile[d.x]._mioff;
	missile[mi]._mitoff = missile[d.x]._mitoff;
	missile[mi]._mivel = { 0, 0 };
	missile[mi]._mirange = missile[mi]._miAnimLen;
	missile[mi]._miVar1 = 0;
}

void AddWeapexp(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	missile[mi]._mi = s;
	missile[mi]._mis = s;
	missile[mi]._mivel = { 0, 0 };
	missile[mi]._miVar1 = 0;
	missile[mi]._miVar2 = d.x;
	missile[mi]._mimfnum = Dir(0);
	if (d.x == 1)
		SetMissAnim(mi, MissileGraphic::MAGBLOS);
	else
		SetMissAnim(mi, MissileGraphic::MINILTNG);
	missile[mi]._mirange = missile[mi]._miAnimLen - 1;
}

bool CheckIfTrig(V2Di t)
{
	for (int i = 0; i < numtrigs; i++) {
		int dist = (trigs[i]._t - t).maxabs();
		if ((t == trigs[i]._t) || (dist < 2))
			return true;
	}
	return false;
}

void AddTown(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	int i, j, k, mx, dp;
	V2Di t;
	int CrawlNum[6] = { 0, 3, 12, 45, 94, 159 };

	if (lvl.currlevel) {
		missile[mi]._miDelFlag = true;
		for (j = 0; j < 6; j++) {
			k = CrawlNum[j] + 2;
			for (i = (uint8_t)CrawlTable[CrawlNum[j]]; i > 0; i--) {
				t.x = d.x + CrawlTable[k - 1];
				t.y = d.y + CrawlTable[k];
				if (t.x > 0 && t.x < MAXDUNX && t.y > 0 && t.y < MAXDUNY) {
					Tile &tile = grid.at(t);
					if (!(tile.getMissile() | tile.isSolid() | tile.blocksMissile() | tile.isObject() | tile.isPlayer())) {
						if (!CheckIfTrig(t)) {
							missile[mi]._mi = t;
							missile[mi]._mis = t;
							missile[mi]._miDelFlag = false;
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
		missile[mi]._miDelFlag = false;
	}
	missile[mi]._mirange = 100;
	missile[mi]._miVar1 = missile[mi]._mirange - missile[mi]._miAnimLen;
	missile[mi]._miVar2 = 0;
	for (i = 0; i < nummissiles; i++) {
		mx = missileactive[i];
		if (missile[mx]._mitype == MissileType::TOWN && mx != mi && missile[mx]._misource == id)
			missile[mx]._mirange = 0;
	}
	PutMissile(mi);
	if (id == myplr() && !missile[mi]._miDelFlag && lvl.currlevel) {
		if (!lvl.setlevel) {
			NetSendCmdLocParam3(true, CMD_ACTIVATEPORTAL, t, lvl.currlevel, int(lvl.type()), 0);
		} else {
			NetSendCmdLocParam3(true, CMD_ACTIVATEPORTAL, t, int(lvl.setlvlnum), int(lvl.type()), 1);
		}
	}
}

void AddFlash(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
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
			UseMana(id, SpellId::FLASH);
		} else {
			missile[mi]._midam = lvl.currlevel >> 1;
		}
	} else {
		missile[mi]._midam = monsters[id].data.mLevel << 1;
	}
	missile[mi]._mirange = 19;
}

void AddFlash2(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
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
			missile[mi]._midam = lvl.currlevel >> 1;
		}
	}
	missile[mi]._miPreFlag = true;
	missile[mi]._mirange = 19;
}

void AddManashield(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	missile[mi]._mirange = 48 * plr[id].data._pLevel;
	missile[mi]._miVar1 = plr[id].data._pHitPoints;
	missile[mi]._miVar2 = plr[id].data._pHPBase;
	missile[mi]._miVar8 = -1;
	if (!mienemy)
		UseMana(id, SpellId::MANASHIELD);
	if (id == myplr())
		NetSendCmd(true, CMD_SETSHIELD);
	plr[id].data.pManaShield = true;
}

void AddFiremove(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
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

void AddGuardian(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	V2Di t;
	int i, pn, k, j;
	int CrawlNum[6] = { 0, 3, 12, 45, 94, 159 };

	missile[mi]._midam = random_(62, 10) + (plr[id].data._pLevel >> 1) + 1;
	for (i = missile[mi]._mispllvl; i > 0; i--) {
		missile[mi]._midam += missile[mi]._midam >> 3;
	}

	missile[mi]._miDelFlag = true;
	for (i = 0; i < 6; i++) {
		pn = CrawlNum[i];
		k = pn + 2;
		for (j = (uint8_t)CrawlTable[pn]; j > 0; j--) {
			t = { d.x + CrawlTable[k - 1], d.y + CrawlTable[k] };
			if (t.x > 0 && t.x < MAXDUNX && t.y > 0 && t.y < MAXDUNY) {
				if (LineClear(s, t)) {
					const Tile &tile = grid.at(t);
					if (!(tile.isActor() | tile.isSolid() | tile.blocksMissile() | tile.isObject() | tile.isMissile())) {
						missile[mi]._mi = t;
						missile[mi]._mis = t;
						missile[mi]._miDelFlag = false;
						UseMana(id, SpellId::GUARDIAN);
						i = 6;
						break;
					}
				}
			}
			k += 2;
		}
	}

	if (missile[mi]._miDelFlag != true) {
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

void AddChain(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	missile[mi]._miVar1 = d.x;
	missile[mi]._miVar2 = d.y;
	missile[mi]._mirange = 1;
	UseMana(id, SpellId::CHAIN);
}

void miss_null_11(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	SetMissDir(mi, Dir(d.x)); // Bug???
	missile[mi]._midam = 0;
	missile[mi]._miLightFlag = true;
	missile[mi]._mirange = 250;
}

void miss_null_12(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	if (d.x > 3) d.x = 2;
	SetMissDir(mi, Dir(d.x)); // Bug???
	missile[mi]._midam = 0;
	missile[mi]._miLightFlag = true;
	missile[mi]._mirange = 250;
}

void miss_null_13(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	if (d.x > 3) d.x = 2;
	SetMissDir(mi, Dir(d.x)); // Bug???
	missile[mi]._midam = 0;
	missile[mi]._miLightFlag = true;
	missile[mi]._mirange = missile[mi]._miAnimLen;
}

void AddRhino(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	AnimStruct *anim;

	if (monsters[id].data.MType->mtype < MonsterType::HORNED || monsters[id].data.MType->mtype > MonsterType::OBLORD) {
		if (monsters[id].data.MType->mtype < MonsterType::NSNAKE || monsters[id].data.MType->mtype > MonsterType::GSNAKE) {
			anim = &monsters[id].data.MType->Anims[MonstAnim::WALK];
		} else {
			anim = &monsters[id].data.MType->Anims[MonstAnim::ATTACK];
		}
	} else {
		anim = &monsters[id].data.MType->Anims[MonstAnim::SPECIAL];
	}
	GetMissileVel(mi, s, d, 18);
	missile[mi]._mimfnum = Dir(midir);
	missile[mi]._miAnimFlags = 0;
	missile[mi]._miAnimData = anim->Data[int(midir)];
	missile[mi]._miAnimDelay = anim->Rate;
	missile[mi]._miAnimLen = anim->Frames;
	missile[mi]._miAnimWidth = monsters[id].data.MType->width;
	missile[mi]._miAnimWidth2 = monsters[id].data.MType->width2;
	missile[mi]._miAnimAdd = 1;
	if (monsters[id].data.MType->mtype >= MonsterType::NSNAKE && monsters[id].data.MType->mtype <= MonsterType::GSNAKE)
		missile[mi]._miAnimFrame = 7;
	missile[mi]._miVar1 = 0;
	missile[mi]._miVar2 = 0;
	missile[mi]._miLightFlag = true;
	if (monsters[id].data._uniqtype != 0) {
		missile[mi]._miUniqTrans = monsters[id].data._uniqtrans + 1;
		missile[mi]._mlid = monsters[id].data.mlid;
	}
	missile[mi]._mirange = 256;
	PutMissile(mi);
}

void miss_null_32(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	AnimStruct *anim;
	MonsterStruct *mon;

	anim = &monsters[id].data.MType->Anims[MonstAnim::WALK];
	GetMissileVel(mi, s, d, 16);
	missile[mi]._mimfnum = Dir(midir);
	missile[mi]._miAnimFlags = 0;
	missile[mi]._miAnimData = anim->Data[int(midir)];
	missile[mi]._miAnimDelay = anim->Rate;
	missile[mi]._miAnimLen = anim->Frames;
	missile[mi]._miAnimWidth = monsters[id].data.MType->width;
	missile[mi]._miAnimWidth2 = monsters[id].data.MType->width2;
	missile[mi]._miAnimAdd = 1;
	missile[mi]._miVar1 = 0;
	missile[mi]._miVar2 = 0;
	missile[mi]._miLightFlag = true;
	if (monsters[id].data._uniqtype != 0)
		missile[mi]._miUniqTrans = monsters[id].data._uniqtrans + 1;
	mon = &monsters[id].data;
	grid.at(mon->_m).clearMonster();
	missile[mi]._mirange = 256;
	PutMissile(mi);
}

void AddFlare(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	if (s.x == d.x && s.y == d.y) {
		d += offset(midir);
	}
	GetMissileVel(mi, s, d, 16);
	missile[mi]._mirange = 256;
	missile[mi]._miVar1 = s.x;
	missile[mi]._miVar2 = s.y;
	missile[mi]._mlid = AddLight(s, 8);
	if (!mienemy) {
		UseMana(id, SpellId::FLARE);
		plr[id].data._pHitPoints -= 320;
		plr[id].data._pHPBase -= 320;
		drawhpflag = true;
		if (plr[id].data._pHitPoints <= 0)
			plr[id].SyncPlrKill(0);
	} else {
		if (id > 0) {
			if (monsters[id].data.MType->mtype == MonsterType::SUCCUBUS)
				SetMissAnim(mi, MissileGraphic::FLARE);
			if (monsters[id].data.MType->mtype == MonsterType::SNOWWICH)
				SetMissAnim(mi, MissileGraphic::SCUBMISB);
			if (monsters[id].data.MType->mtype == MonsterType::HLSPWN)
				SetMissAnim(mi, MissileGraphic::SCUBMISD);
			if (monsters[id].data.MType->mtype == MonsterType::SOLBRNR)
				SetMissAnim(mi, MissileGraphic::SCUBMISC);
		}
	}
}

void AddAcid(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	GetMissileVel(mi, s, d, 16);
	SetMissDir(mi, GetDirection16(s, d).to8());
	missile[mi]._mirange = 5 * (monsters[id].data._mint + 4);
	missile[mi]._mlid = -1;
	missile[mi]._miVar1 = s.x;
	missile[mi]._miVar2 = s.y;
	PutMissile(mi);
}

void miss_null_1D(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	missile[mi]._midam = dam;
	missile[mi]._mivel = { 0, 0 };
	missile[mi]._mirange = 50;
	missile[mi]._miVar1 = missile[mi]._mirange - missile[mi]._miAnimLen;
	missile[mi]._miVar2 = 0;
}

void AddAcidpud(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	int monst;

	missile[mi]._mivel = {0,0};
	missile[mi]._mioff = {0,0};
	missile[mi]._miLightFlag = true;
	monst = missile[mi]._misource;
	missile[mi]._mirange = random_(50, 15) + 40 * (monsters[monst].data._mint + 1);
	missile[mi]._miPreFlag = true;
}

void AddStone(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	int i, j, k, l, tx, ty, mid;
	int CrawlNum[6] = { 0, 3, 12, 45, 94, 159 };

	missile[mi]._misource = id;
	for (i = 0; i < 6; i++) {
		k = CrawlNum[i];
		l = k + 2;
		for (j = (uint8_t)CrawlTable[k]; j > 0; j--) {
			tx = d.x + CrawlTable[l - 1];
			ty = d.y + CrawlTable[l];
			if (tx > 0 && tx < MAXDUNX && ty > 0 && ty < MAXDUNY) {
				mid = grid[tx][ty].getActor();
				mid = mid > 0 ? mid - 1 : -1 - mid;
				if (mid > 3 && monsters[mid].data._mAi != MonstAi::DIABLO) {
					if (monsters[mid].data._mmode != MonsterMode::FADEIN && monsters[mid].data._mmode != MonsterMode::FADEOUT && monsters[mid].data._mmode != MonsterMode::CHARGE) {
						j = -99;
						i = 6;
						missile[mi]._miVar1 = monsters[mid].data._mmode;
						missile[mi]._miVar2 = mid;
						monsters[mid].data._mmode = MonsterMode::STONE;
						break;
					}
				}
			}
			l += 2;
		}
	}

	if (j != -99) {
		missile[mi]._miDelFlag = true;
	} else {
		missile[mi]._mi = { tx, ty };
		missile[mi]._mis = missile[mi]._mi;
		missile[mi]._mirange = missile[mi]._mispllvl + 6;
		missile[mi]._mirange += (missile[mi]._mirange * plr[id].data._pISplDur) >> 7;

		if (missile[mi]._mirange > 15)
			missile[mi]._mirange = 15;
		missile[mi]._mirange <<= 4;
		UseMana(id, SpellId::STONE);
	}
}

void AddGolem(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	int i;
	int mx;

	missile[mi]._miDelFlag = false;
	for (i = 0; i < nummissiles; i++) {
		mx = missileactive[i];
		if (missile[mx]._mitype == MissileType::GOLEM) {
			if (mx != mi && missile[mx]._misource == id) {
				missile[mi]._miDelFlag = true;
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
	UseMana(id, SpellId::GOLEM);
}

void AddEtherealize(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
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
		UseMana(id, SpellId::ETHEREALIZE);
}

void miss_null_1F(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	missile[mi]._miDelFlag = true;
}

void miss_null_23(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	missile[mi]._midam = dam;
	missile[mi]._mi = s;
	missile[mi]._mis = s;
	missile[mi]._misource = id;
	if (dam == 1)
		SetMissDir(mi, Dir(0));
	else
		SetMissDir(mi, Dir(1));
	missile[mi]._miLightFlag = true;
	missile[mi]._mirange = missile[mi]._miAnimLen;
}

void AddBoom(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	missile[mi]._mi = d;
	missile[mi]._mis = d;
	missile[mi]._mivel = { 0, 0 };
	missile[mi]._midam = dam;
	missile[mi]._mirange = missile[mi]._miAnimLen;
	missile[mi]._miVar1 = 0;
}

void AddHeal(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
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

	if (plr[id].data._pClass == PlayerClass::warrior)
		HealAmount <<= 1;

	if (plr[id].data._pClass == PlayerClass::rogue)
		HealAmount += HealAmount >> 1;

	plr[id].data._pHitPoints = std::min(
		plr[id].data._pHitPoints + HealAmount,
		plr[id].data._pMaxHP);

	plr[id].data._pHPBase = std::min(
	    plr[id].data._pHPBase + HealAmount,
	    plr[id].data._pMaxHPBase);

	UseMana(id, SpellId::HEAL);
	missile[mi]._miDelFlag = true;
	drawhpflag = true;
}

void AddHealOther(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	missile[mi]._miDelFlag = true;
	UseMana(id, SpellId::HEALOTHER);
	if (id == myplr()) {
		SetCursor_(CURSOR_HEALOTHER);
		if (sgbControllerActive)
			TryIconCurs();
	}
}

void AddElement(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	if (s == d) {
		d += offset(midir);
	}
	missile[mi]._midam = 2 * (plr[id].data._pLevel + random_(60, 10) + random_(60, 10)) + 4;
	for (int i = missile[mi]._mispllvl; i > 0; i--) {
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
	UseMana(id, SpellId::ELEMENT);
}

extern void FocusOnInventory();

void AddIdentify(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	missile[mi]._miDelFlag = true;
	UseMana(id, SpellId::IDENTIFY);
	if (id == myplr()) {
		if (sbookflag) sbookflag = false;
		if (!invflag) {
			invflag = true;
			if (sgbControllerActive)
				FocusOnInventory();
		}
		SetCursor_(CURSOR_IDENTIFY);
	}
}

void AddFirewallC(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	int i, j, k, pn;
	V2Di t;
	int CrawlNum[6] = { 0, 3, 12, 45, 94, 159 };

	missile[mi]._miDelFlag = true;
	for (i = 0; i < 6; i++) {
		k = CrawlNum[i];
		pn = k + 2;
		for (j = (uint8_t)CrawlTable[k]; j > 0; j--) {
			t.x = d.x + CrawlTable[pn - 1];
			t.y = d.y + CrawlTable[pn];
			if (0 < t.x && t.x < MAXDUNX && 0 < t.y && t.y < MAXDUNY) {
				if (LineClear(s, t)) {
					if ((s.x != t.x || s.y != t.y) && !(grid.at(t).isSolid() | grid.at(t).isObject())) {
						missile[mi]._miVar1 = t.x;
						missile[mi]._miVar2 = t.y;
						missile[mi]._miVar5 = t.x;
						missile[mi]._miVar6 = t.y;
						missile[mi]._miDelFlag = false;
						i = 6;
						break;
					}
				}
			}
			pn += 2;
		}
	}

	if (missile[mi]._miDelFlag != true) {
		missile[mi]._miVar7 = 0;
		missile[mi]._miVar8 = 0;
		missile[mi]._miVar3 = int(rotate(midir, -2));
		missile[mi]._miVar4 = int(rotate(midir, 2));
		missile[mi]._mirange = 7;
		UseMana(id, SpellId::FIREWALL);
	}
}

void AddInfra(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	missile[mi]._mirange = 1584;
	for (int i = missile[mi]._mispllvl; i > 0; i--) {
		missile[mi]._mirange += missile[mi]._mirange >> 3;
	}
	missile[mi]._mirange += missile[mi]._mirange * plr[id].data._pISplDur >> 7;
	if (!mienemy)
		UseMana(id, SpellId::INFRA);
}

void AddWave(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	missile[mi]._miVar1 = d.x;
	missile[mi]._miVar2 = d.y;
	missile[mi]._miVar3 = 0;
	missile[mi]._miVar4 = 0;
	missile[mi]._mirange = 1;
	missile[mi]._miAnimFrame = 4;
	UseMana(id, SpellId::WAVE);
}

void AddNova(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	missile[mi]._miVar1 = d.x;
	missile[mi]._miVar2 = d.y;
	if (id != -1) {
		missile[mi]._midam = (random_(66, 6) + random_(66, 6) + random_(66, 6) + random_(66, 6) + random_(66, 6));
		missile[mi]._midam += plr[id].data._pLevel + 5;
		missile[mi]._midam >>= 1;
		for (int k = missile[mi]._mispllvl; k > 0; k--) {
			missile[mi]._midam += missile[mi]._midam >> 3;
		}
		if (!mienemy)
			UseMana(id, SpellId::NOVA);
	} else {
		missile[mi]._midam = ((DWORD)lvl.currlevel >> 1) + random_(66, 3) + random_(66, 3) + random_(66, 3);
	}
	missile[mi]._mirange = 1;
}

void AddBlodboil(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	missile[mi]._miDelFlag = 1;
}

void AddRepair(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	missile[mi]._miDelFlag = true;
	UseMana(id, SpellId::REPAIR);
	if (id == myplr()) {
		if (sbookflag) sbookflag = false;
		if (!invflag) {
			invflag = true;
			if (sgbControllerActive)
				FocusOnInventory();
		}
		SetCursor_(CURSOR_REPAIR);
	}
}

void AddRecharge(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	missile[mi]._miDelFlag = true;
	UseMana(id, SpellId::RECHARGE);
	if (id == myplr()) {
		if (sbookflag)
			sbookflag = false;
		if (!invflag) {
			invflag = true;
			if (sgbControllerActive)
				FocusOnInventory();
		}
		SetCursor_(CURSOR_RECHARGE);
	}
}

void AddDisarm(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	missile[mi]._miDelFlag = true;
	UseMana(id, SpellId::DISARM);
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

void AddApoca(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
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
	for (int i = 0; i < plr[id].data._pLevel; i++) {
		missile[mi]._midam += random_(67, 6) + 1;
	}
	missile[mi]._mirange = 255;
	missile[mi]._miDelFlag = false;
	UseMana(id, SpellId::APOCA);
}

void AddFlame(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	int i;
	missile[mi]._miVar2 = 0;
	for (i = dam; i > 0; i--) {
		missile[mi]._miVar2 += 5;
	}
	missile[mi]._mis = d;
	missile[mi]._mioff = missile[int(midir)]._mioff; // Bug???? missile[midir]???
	missile[mi]._mitoff = missile[int(midir)]._mitoff; // Bug???? missile[midir]???
	missile[mi]._mirange = missile[mi]._miVar2 + 20;
	missile[mi]._mlid = AddLight(s, 1);
	if (!mienemy) {
		int j = random_(79, plr[id].data._pLevel) + random_(79, 2);
		missile[mi]._midam = 8 * j + 16 + ((8 * i + 16) >> 1);
	} else {
		missile[mi]._midam = monsters[id].data.mMinDamage + random_(77, monsters[id].data.mMaxDamage - monsters[id].data.mMinDamage + 1);
	}
}

void AddFlamec(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	if (s == d) d += offset(midir);
	GetMissileVel(mi, s, d, 32);
	if (!mienemy) UseMana(id, SpellId::FLAME);
	missile[mi]._miVar1 = s.x;
	missile[mi]._miVar2 = s.y;
	missile[mi]._miVar3 = 0;
	missile[mi]._mirange = 256;
}

void AddCbolt(int mi, V2Di s, V2Di d, Dir midir, char micaster, int id, int dam)
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

	if (s== d) d += offset(midir);

	missile[mi]._miAnimFrame = random_(63, 8) + 1;
	missile[mi]._mlid = AddLight(s, 5);

	GetMissileVel(mi, s, d, 8);
	missile[mi]._miVar1 = 5;
	missile[mi]._miVar2 = int(midir);
	missile[mi]._miVar3 = 0;
	missile[mi]._mirange = 256;
}

void AddHbolt(int mi, V2Di s, V2Di d, Dir midir, char micaster, int id, int dam)
{
	int sp;
	if (s == d) d += offset(midir);
	if (id != -1) {
		sp = 2 * missile[mi]._mispllvl + 16;
		if (sp >= 63) {
			sp = 63;
		}
	} else {
		sp = 16;
	}
	GetMissileVel(mi, s, d, sp);
	SetMissDir(mi, GetDirection16(s, d).to8());
	missile[mi]._mirange = 256;
	missile[mi]._miVar1 = s.x;
	missile[mi]._miVar2 = s.y;
	missile[mi]._mlid = AddLight(s, 8);
	missile[mi]._midam = random_(69, 10) + plr[id].data._pLevel + 9;
	UseMana(id, SpellId::HBOLT);
}

void AddResurrect(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	UseMana(id, SpellId::RESURRECT);
	if (id == myplr()) {
		SetCursor_(CURSOR_RESURRECT);
		if (sgbControllerActive)
			TryIconCurs();
	}
	missile[mi]._miDelFlag = true;
}

void AddResurrectBeam(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	missile[mi]._mi = d;
	missile[mi]._mis = missile[mi]._mi;
	missile[mi]._mivel = { 0, 0 };
	missile[mi]._mirange = misfiledata[MissileGraphic::RESSUR1].mAnimLen[0];
}

void AddTelekinesis(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	missile[mi]._miDelFlag = true;
	UseMana(id, SpellId::TELEKINESIS);
	if (id == myplr())
		SetCursor_(CURSOR_TELEKINESIS);
}

void AddBoneSpirit(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	if (s== d) d += offset(midir);
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
		UseMana(id, SpellId::BONESPIRIT);
		plr[id].data._pHitPoints -= 384;
		plr[id].data._pHPBase -= 384;
		drawhpflag = true;
		if (plr[id].data._pHitPoints <= 0)
			plr[id].SyncPlrKill(0);
	}
}

void AddRportal(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	missile[mi]._mi = s;
	missile[mi]._mis = s;
	missile[mi]._mirange = 100;
	missile[mi]._miVar1 = 100 - missile[mi]._miAnimLen;
	missile[mi]._miVar2 = 0;
	PutMissile(mi);
}

void AddDiabApoca(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam)
{
	for (int pnum = 0; pnum < plr.maxPlayers(); pnum++) {
		if (!plr.isValidPlayer(pnum)) continue;
		if (plr[pnum].data.plractive) {
			if (LineClear(s, plr[pnum].futpos())) {
				AddMissile({ 0, 0 }, plr[pnum].futpos(), Dir(0), MissileType::BOOM2, mienemy, id, dam, 0);
			}
		}
	}
	missile[mi]._miDelFlag = true;
}

int AddMissile(V2Di s, V2Di d, Dir midir, int mitype, char micaster, int id, int midam, int spllvl)
{
	int i, mi;

	if (nummissiles >= MAXMISSILES)
		return -1;

	if (mitype == MissileType::MANASHIELD && plr[id].data.pManaShield == true) {
		if (lvl.currlevel != plr[id].data.plrlevel)
			return -1;

		for (i = 0; i < nummissiles; i++) {
			mi = missileactive[i];
			if (missile[mi]._mitype == MissileType::MANASHIELD && missile[mi]._misource == id)
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

	if (missile[mi]._miAnimType == MissileGraphic::NONE || misfiledata[missile[mi]._miAnimType].mAnimFAmt < 8)
		SetMissDir(mi, Dir(0));
	else
		SetMissDir(mi, midir);

	missile[mi]._mi = s;
	missile[mi]._mioff = { 0, 0 };
	missile[mi]._mis = s;
	missile[mi]._mitoff = { 0, 0 };
	missile[mi]._miDelFlag = false;
	missile[mi]._miAnimAdd = 1;
	missile[mi]._miLightFlag = false;
	missile[mi]._miPreFlag = false;
	missile[mi]._miUniqTrans = 0;
	missile[mi]._midam = midam;
	missile[mi]._miHitFlag = false;
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
		if (grid.at(s).getActor() > 0 && monsters[grid.at(s).getActor() - 1].data._mhitpoints >> 6 > 0 && grid.at(s).getActor() - 1 > 3) {
			Dir dir = GetDirection(missile[i]._mi, s);
			missile[i]._miVar3 = missileavail[0];
			AddMissile(missile[i]._mi, s, dir, MissileType::FIREBOLT, 0, missile[i]._misource, missile[i]._midam, GetSpellLevel(missile[i]._misource, SpellId::FIREBOLT));
			ex = -1;
		}
	}
	if (ex == -1) {
		SetMissDir(i, Dir(2));
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
			for (m = (uint8_t)CrawlTable[k]; m > 0; m--) {
				ct = &CrawlTable[tid];
				t.x = missile[i]._miVar4 + *(ct - 1);
				t.y = missile[i]._miVar5 + *ct;
				if (0 < t.x && t.x < MAXDUNX && 0 < t.y && t.y < MAXDUNY) {
					if (LineClear({ missile[i]._miVar1, missile[i]._miVar2 }, t)) {
						if (!(grid.at(t).isActor() | grid.at(t).isSolid() | grid.at(t).isObject())) {
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
	missile[i]._miDelFlag = true;
}

void MI_SetManashield(int i)
{
	ManashieldFlag = true;
}

void MI_LArrow(int i)
{
	int p, mind, maxd, rst;

	missile[i]._mirange--;
	p = missile[i]._misource;
	if (missile[i]._miAnimType == MissileGraphic::MINILTNG || missile[i]._miAnimType == MissileGraphic::MAGBLOS) {
		ChangeLight(missile[i]._mlid, missile[i]._mi, missile[i]._miAnimFrame + 5);
		rst = missiledata[missile[i]._mitype].mResist;
		if (missile[i]._mitype == MissileType::LARROW) {
			if (p != -1) {
				mind = plr[p].data._pILMinDam;
				maxd = plr[p].data._pILMaxDam;
			} else {
				mind = random_(68, 10) + 1 + lvl.currlevel;
				maxd = random_(68, 10) + 1 + lvl.currlevel * 2;
			}
			missiledata[MissileType::LARROW].mResist = MissileResistance::LIGHTNING;
			CheckMissileCol(i, mind, maxd, false, missile[i]._mi, true);
		}
		if (missile[i]._mitype == MissileType::FARROW) {
			if (p != -1) {
				mind = plr[p].data._pIFMinDam;
				maxd = plr[p].data._pIFMaxDam;
			} else {
				mind = random_(68, 10) + 1 + lvl.currlevel;
				maxd = random_(68, 10) + 1 + lvl.currlevel * 2;
			}
			missiledata[MissileType::FARROW].mResist = MissileResistance::FIRE;
			CheckMissileCol(i, mind, maxd, false, missile[i]._mi, true);
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
			mind = random_(68, 10) + 1 + lvl.currlevel;
			maxd = random_(68, 10) + 1 + lvl.currlevel * 2;
		}

		if (missile[i]._mi.x != missile[i]._mis.x || missile[i]._mi.y != missile[i]._mis.y) {
			rst = missiledata[missile[i]._mitype].mResist;
			missiledata[missile[i]._mitype].mResist = 0;
			CheckMissileCol(i, mind, maxd, false, missile[i]._mi, false);
			missiledata[missile[i]._mitype].mResist = rst;
		}
		if (!missile[i]._mirange) {
			missile[i]._mimfnum = Dir(0);
			missile[i]._mitoff -= missile[i]._mivel;
			GetMissilePos(i);
			if (missile[i]._mitype == MissileType::LARROW)
				SetMissAnim(i, MissileGraphic::MINILTNG);
			else
				SetMissAnim(i, MissileGraphic::MAGBLOS);
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
		missile[i]._miDelFlag = true;
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
		mind = lvl.currlevel;
		maxd = 2 * lvl.currlevel;
	}
	if (missile[i]._mi.x != missile[i]._mis.x || missile[i]._mi.y != missile[i]._mis.y)
		CheckMissileCol(i, mind, maxd, false, missile[i]._mi, false);
	if (!missile[i]._mirange)
		missile[i]._miDelFlag = true;
	PutMissile(i);
}

void MI_Firebolt(int i)
{
	V2Di om;
	int d, p;

	missile[i]._mirange--;
	if (missile[i]._mitype != MissileType::BONESPIRIT || int(missile[i]._mimfnum) != 8) {
		om = missile[i]._mitoff;
		missile[i]._mitoff += missile[i]._mivel;
		GetMissilePos(i);
		p = missile[i]._misource;
		if (p != -1) {
			if (!missile[i]._micaster) {
				switch (missile[i]._mitype) {
				case MissileType::FIREBOLT:
					d = random_(75, 10) + (plr[p].data._pMagic >> 3) + missile[i]._mispllvl + 1;
					break;
				case MissileType::FLARE:
					d = 3 * missile[i]._mispllvl - (plr[p].data._pMagic >> 3) + (plr[p].data._pMagic >> 1);
					break;
				case MissileType::BONESPIRIT:
					d = 0;
					break;
				}
			} else {
				d = monsters[p].data.mMinDamage + random_(77, monsters[p].data.mMaxDamage - monsters[p].data.mMinDamage + 1);
			}
		} else {
			d = lvl.currlevel + random_(78, 2 * lvl.currlevel);
		}
		if (missile[i]._mi.x != missile[i]._mis.x || missile[i]._mi.y != missile[i]._mis.y) {
			CheckMissileCol(i, d, d, false, missile[i]._mi, false);
		}
		if (!missile[i]._mirange) {
			missile[i]._miDelFlag = true;
			missile[i]._mitoff = om;
			GetMissilePos(i);
			switch (missile[i]._mitype) {
			case MissileType::FIREBOLT:
			case MissileType::MAGMABALL:
				AddMissile(missile[i]._mi, { i, 0 }, Dir(int(missile[i]._mimfnum)), MissileType::MISEXP, missile[i]._micaster, missile[i]._misource, 0, 0);
				break;
			case MissileType::FLARE:
				AddMissile(missile[i]._mi, { i, 0 }, Dir(int(missile[i]._mimfnum)), MissileType::MISEXP2, missile[i]._micaster, missile[i]._misource, 0, 0);
				break;
			case MissileType::ACID:
				AddMissile(missile[i]._mi, { i, 0 }, Dir(int(missile[i]._mimfnum)), MissileType::MISEXP3, missile[i]._micaster, missile[i]._misource, 0, 0);
				break;
			case MissileType::BONESPIRIT:
				SetMissDir(i, Dir(8));
				missile[i]._mirange = 7;
				missile[i]._miDelFlag = false;
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
		missile[i]._miDelFlag = true;
		PlaySfxLoc(LS_BSIMPCT, missile[i]._mi);
		PutMissile(i);
	} else
		PutMissile(i);
}

void MI_Lightball(int i)
{
	int j, oi;
	char obj;

	V2Di t = { missile[i]._miVar1, missile[i]._miVar2 };
	missile[i]._mirange--;
	missile[i]._mitoff += missile[i]._mivel;
	GetMissilePos(i);
	j = missile[i]._mirange;
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, false, missile[i]._mi, false);
	if (missile[i]._miHitFlag == true)
		missile[i]._mirange = j;
	obj = grid.at(t).getObject();
	if (obj && t == missile[i]._mi) {

		if (object[oi]._otype == ObjectType::SHRINEL || object[oi]._otype == ObjectType::SHRINER)
			missile[i]._mirange = j;
	}
	if (!missile[i]._mirange)
		missile[i]._miDelFlag = true;
	PutMissile(i);
}

void mi_null_33(int i)
{
	missile[i]._mirange--;
	missile[i]._mitoff += missile[i]._mivel;
	GetMissilePos(i);
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, false, missile[i]._mi, false);
	if (!missile[i]._mirange)
		missile[i]._miDelFlag = true;
	PutMissile(i);
}

void MI_Acidpud(int i)
{
	int range;

	missile[i]._mirange--;
	range = missile[i]._mirange;
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, true, missile[i]._mi, false);
	missile[i]._mirange = range;
	if (!range) {
		if (int(missile[i]._mimfnum)) {
			missile[i]._miDelFlag = true;
		} else {
			SetMissDir(i, Dir(1));
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
		SetMissDir(i, Dir(1));
		missile[i]._miAnimFrame = random_(83, 11) + 1;
	}
	if (missile[i]._mirange == missile[i]._miAnimLen - 1) {
		SetMissDir(i, Dir(0));
		missile[i]._miAnimFrame = 13;
		missile[i]._miAnimAdd = -1;
	}
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, true, missile[i]._mi, true);
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = true;
		AddUnLight(missile[i]._mlid);
	}
	if (int(missile[i]._mimfnum) && missile[i]._mirange && missile[i]._miAnimAdd != -1 && missile[i]._miVar2 < 12) {
		if (!missile[i]._miVar2)
			missile[i]._mlid = AddLight(missile[i]._mi, ExpLight[0]);
		ChangeLight(missile[i]._mlid, missile[i]._mi, ExpLight[missile[i]._miVar2]);
		missile[i]._miVar2++;
	}
	PutMissile(i);
}

void MI_Fireball(int i)
{
	V2Di p, m;
	int id = missile[i]._misource;
	int dam = missile[i]._midam;
	missile[i]._mirange--;

	if (missile[i]._micaster == 0) {
		p = plr[id].pos();
	} else {
		p = monsters[id].data._m;
	}

	if (missile[i]._miAnimType == MissileGraphic::BIGEXP) {
		if (!missile[i]._mirange) {
			missile[i]._miDelFlag = true;
			AddUnLight(missile[i]._mlid);
		}
	} else {
		missile[i]._mitoff += missile[i]._mivel;
		GetMissilePos(i);
		if (missile[i]._mi.x != missile[i]._mis.x || missile[i]._mi.y != missile[i]._mis.y)
			CheckMissileCol(i, dam, dam, false, missile[i]._mi, false);
		if (!missile[i]._mirange) {
			m = missile[i]._mi;
			ChangeLight(missile[i]._mlid, { missile[i]._mi.x, m.y }, missile[i]._miAnimFrame);
			if (!CheckBlock(p, m))
				CheckMissileCol(i, dam, dam, false, m, true);
			if (!CheckBlock(p, { m.x, m.y + 1 }))
				CheckMissileCol(i, dam, dam, false, { m.x, m.y + 1 }, true);
			if (!CheckBlock(p, { m.x, m.y - 1 }))
				CheckMissileCol(i, dam, dam, false, { m.x, m.y - 1 }, true);
			if (!CheckBlock(p, { m.x + 1, m.y }))
				CheckMissileCol(i, dam, dam, false, { m.x + 1, m.y }, true);
			if (!CheckBlock(p, { m.x + 1, m.y - 1 }))
				CheckMissileCol(i, dam, dam, false, { m.x + 1, m.y - 1 }, true);
			if (!CheckBlock(p, { m.x + 1, m.y + 1 }))
				CheckMissileCol(i, dam, dam, false, { m.x + 1, m.y + 1 }, true);
			if (!CheckBlock(p, { m.x - 1, m.y }))
				CheckMissileCol(i, dam, dam, false, { m.x - 1, m.y }, true);
			if (!CheckBlock(p, { m.x - 1, m.y + 1 }))
				CheckMissileCol(i, dam, dam, false, { m.x - 1, m.y + 1 }, true);
			if (!CheckBlock(p, { m.x - 1, m.y - 1 }))
				CheckMissileCol(i, dam, dam, false, { m.x - 1, m.y - 1 }, true);
			if (!lvl.TransList[grid.at(m).dTransVal]
			    || (missile[i]._mivel.x < 0 && ((lvl.TransList[grid[m.x][m.y + 1].dTransVal] && grid[m.x][m.y + 1].isSolid()) ||
					(lvl.TransList[grid[m.x][m.y - 1].dTransVal] && grid[m.x][m.y - 1].isSolid())))) {
				missile[i]._mi.x++;
				missile[i]._mi.y++;
				missile[i]._mioff.y -= 32;
			}
			if (missile[i]._mivel.y > 0
			    && (lvl.TransList[grid[m.x + 1][m.y].dTransVal] && grid[m.x + 1][m.y].isSolid()
			        || lvl.TransList[grid[m.x - 1][m.y].dTransVal] && grid[m.x - 1][m.y].isSolid())) {
				missile[i]._mioff.y -= 32;
			}
			if (missile[i]._mivel.x > 0
			    && (lvl.TransList[grid[m.x][m.y + 1].dTransVal] && grid[m.x][m.y + 1].isSolid()
			        || lvl.TransList[grid[m.x][m.y - 1].dTransVal] && grid[m.x][m.y - 1].isSolid())) {
				missile[i]._mioff.x -= 32;
			}
			missile[i]._mimfnum = Dir(0);
			SetMissAnim(i, MissileGraphic::BIGEXP);
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
		dam = random_(81, lvl.currlevel) + 2 * lvl.currlevel;
	}

	missile[i]._mitoff += missile[i]._mivel;
	GetMissilePos(i);

	m = missile[i]._mi;
	assert((DWORD)m.x < MAXDUNX);
	assert((DWORD)m.y < MAXDUNY);
	pn = grid.at(m).getPiece();
	assert((DWORD)pn <= MAXTILES);

	if (missile[i]._misource == -1) {
		if ((m.x != missile[i]._mis.x || m.y != missile[i]._mis.y) && pieces[pn].missile) {
			missile[i]._mirange = 0;
		}
	} else if (pieces[pn].missile) {
		missile[i]._mirange = 0;
	}
	if (!pieces[pn].missile) {
		if ((m.x != missile[i]._miVar1 || m.y != missile[i]._miVar2) && grid.isValid(m)) {
			if (missile[i]._misource != -1) {
				if (missile[i]._micaster == 1
				    && monsters[missile[i]._misource].data.MType->mtype >= MonsterType::STORM
				    && monsters[missile[i]._misource].data.MType->mtype <= MonsterType::MAEL) {
					AddMissile(
					    m,
					    missile[i]._mis,
					    Dir(i),
					    MissileType::LIGHTNING2,
					    missile[i]._micaster,
					    missile[i]._misource,
					    dam,
					    missile[i]._mispllvl);
				} else {
					AddMissile(
					    m,
					    missile[i]._mis,
					    Dir(i),
					    MissileType::LIGHTNING,
					    missile[i]._micaster,
					    missile[i]._misource,
					    dam,
					    missile[i]._mispllvl);
				}
			} else {
				AddMissile(
				    m,
					missile[i]._mis,
				    Dir(i),
				    MissileType::LIGHTNING,
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
		missile[i]._miDelFlag = true;
	}
}

void MI_Lightning(int i)
{
	int j;

	missile[i]._mirange--;
	j = missile[i]._mirange;
	if (missile[i]._mi.x != missile[i]._mis.x || missile[i]._mi.y != missile[i]._mis.y)
		CheckMissileCol(i, missile[i]._midam, missile[i]._midam, true, missile[i]._mi, false);
	if (missile[i]._miHitFlag == true)
		missile[i]._mirange = j;
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = true;
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
		SetMissDir(i, Dir(1));
	if (lvl.currlevel && int(missile[i]._mimfnum) != 1 && missile[i]._mirange) {
		if (!missile[i]._miVar2)
			missile[i]._mlid = AddLight(missile[i]._mi, 1);
		ChangeLight(missile[i]._mlid, missile[i]._mi, ExpLight[missile[i]._miVar2]);
		missile[i]._miVar2++;
	}

	for (p = 0; p < MAX_PLRS; p++) {
		if (plr[p].data.plractive && lvl.currlevel == plr[p].data.plrlevel && !plr[p].data._pLvlChanging && plr[p].data._pmode == PlayerMode::STAND && plr[p].pos().x == missile[i]._mi.x && plr[p].pos().y == missile[i]._mi.y) {
			plr[p].ClrPlrPath();
			if (p == myplr()) {
				NetSendCmdParam1(true, CMD_WARP, missile[i]._misource);
				plr[p].data._pmode = PlayerMode::NEWLVL;
			}
		}
	}

	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = true;
		AddUnLight(missile[i]._mlid);
	}
	PutMissile(i);
}

void MI_Flash(int i)
{
	if (!missile[i]._micaster) {
		if (missile[i]._misource != -1)
			plr[missile[i]._misource].data._pInvincible = true;
	}
	missile[i]._mirange--;
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, true, { missile[i]._mi.x - 1, missile[i]._mi.y }, true);
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, true, { missile[i]._mi.x, missile[i]._mi.y }, true);
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, true, { missile[i]._mi.x + 1, missile[i]._mi.y }, true);
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, true, { missile[i]._mi.x - 1, missile[i]._mi.y + 1 }, true);
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, true, { missile[i]._mi.x, missile[i]._mi.y + 1 }, true);
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, true, { missile[i]._mi.x + 1, missile[i]._mi.y + 1 }, true);
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = true;
		if (missile[i]._micaster == 0) {
			if (missile[i]._misource != -1)
				plr[missile[i]._misource].data._pInvincible = false;
		}
	}
	PutMissile(i);
}

void MI_Flash2(int i)
{
	if (!missile[i]._micaster) {
		if (missile[i]._misource != -1)
			plr[missile[i]._misource].data._pInvincible = true;
	}
	missile[i]._mirange--;
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, true, { missile[i]._mi.x - 1, missile[i]._mi.y - 1 }, true);
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, true, { missile[i]._mi.x, missile[i]._mi.y - 1 }, true);
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, true, { missile[i]._mi.x + 1, missile[i]._mi.y - 1 }, true);
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = true;
		if (missile[i]._micaster == 0) {
			if (missile[i]._misource != -1)
				plr[missile[i]._misource].data._pInvincible = false;
		}
	}
	PutMissile(i);
}

void MI_Manashield(int i)
{
	int id, diff;

	id = missile[i]._misource;
	missile[i]._mi = plr[id].pos();
	missile[i]._mitoff.x = plr[id].data._poff.x << 16;
	missile[i]._mitoff.y = plr[id].data._poff.y << 16;
	if (plr[id].data._pmode == PlayerMode::WALK3) {
		missile[i]._mis = plr[id].futpos();
	} else {
		missile[i]._mis = plr[id].pos();
	}
	GetMissilePos(i);
	if (plr[id].data._pmode == PlayerMode::WALK3) {
		if (plr[id].data._pdir == Dir::W)
			missile[i]._mi.x++;
		else
			missile[i]._mi.y++;
	}
	if (id != myplr()) {
		if (lvl.currlevel != plr[id].data.plrlevel)
			missile[i]._miDelFlag = true;
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
			drawmanaflag = true;
			drawhpflag = true;

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
				missile[i]._miDelFlag = true;
				if (plr[id].data._pHitPoints < 0)
					plr[id].SetPlayerHitPoints(0);
				if (!(plr[id].data._pHitPoints >> 6) && id == myplr()) {
					plr[id].SyncPlrKill(missile[i]._miVar8);
				}
			}
		}

		if (id == myplr() && !plr[id].data._pHitPoints && !missile[i]._miVar1 && plr[id].data._pmode != PlayerMode::DEATH) {
			missile[i]._mirange = 0;
			missile[i]._miDelFlag = true;
			plr[id].SyncPlrKill(-1);
		}
		missile[i]._miVar1 = plr[id].data._pHitPoints;
		missile[i]._miVar2 = plr[id].data._pHPBase;
		if (missile[i]._mirange == 0) {
			missile[i]._miDelFlag = true;
			NetSendCmd(true, CMD_ENDSHIELD);
		}
	}
	PutMissile(i);
}

void MI_Etherealize(int i)
{
	int src;

	missile[i]._mirange--;
	src = missile[i]._misource;
	missile[i]._mi = plr[src].pos();
	missile[i]._mitoff.x = plr[src].data._poff.x << 16;
	missile[i]._mitoff.y = plr[src].data._poff.y << 16;
	if (plr[src].data._pmode == PlayerMode::WALK3) {
		missile[i]._mis = plr[src].futpos();
	} else {
		missile[i]._mis = plr[src].pos();
	}
	GetMissilePos(i);
	if (plr[src].data._pmode == PlayerMode::WALK3) {
		if (plr[src].data._pdir == Dir::W)
			missile[i]._mi.x++;
		else
			missile[i]._mi.y++;
	}
	plr[src].data._pSpellFlags |= 1;
	if (!missile[i]._mirange || plr[src].data._pHitPoints <= 0) {
		missile[i]._miDelFlag = true;
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
		SetMissDir(i, Dir(1));
		missile[i]._miAnimFrame = random_(82, 11) + 1;
	}
	missile[i]._mitoff += missile[i]._mivel;
	GetMissilePos(i);
	j = missile[i]._mirange;
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, false, missile[i]._mi, false);
	if (missile[i]._miHitFlag == true)
		missile[i]._mirange = j;
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = true;
		AddUnLight(missile[i]._mlid);
	}
	if (int(missile[i]._mimfnum) || !missile[i]._mirange) {
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
	if (missile[i]._mirange == missile[i]._miVar1 || int(missile[i]._mimfnum) == MissileGraphic::GUARD && missile[i]._miVar2 == 0) {
		SetMissDir(i, Dir(1));
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
		SetMissDir(i, Dir(0));
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
		missile[i]._miDelFlag = true;
		AddUnLight(missile[i]._mlid);
	}

	PutMissile(i);
}

void MI_Chain(int i)
{
	V2Di s, t;
	int id, l, n, m, k, rad;
	int CrawlNum[19] = { 0, 3, 12, 45, 94, 159, 240, 337, 450, 579, 724, 885, 1062, 1255, 1464, 1689, 1930, 2187, 2460 };

	id = missile[i]._misource;
	s = missile[i]._mi;
	Dir dir = GetDirection(s, { missile[i]._miVar1, missile[i]._miVar2 });
	AddMissile(s, { missile[i]._miVar1, missile[i]._miVar2 }, dir, MissileType::LIGHTCTRL, 0, id, 1, missile[i]._mispllvl);
	rad = missile[i]._mispllvl + 3;
	if (rad > 19)
		rad = 19;
	for (m = 1; m < rad; m++) {
		k = CrawlNum[m];
		l = k + 2;
		for (n = (uint8_t)CrawlTable[k]; n > 0; n--) {
			t.x = s.x + CrawlTable[l - 1];
			t.y = s.y + CrawlTable[l];
			if (t.x > 0 && t.x < MAXDUNX && t.y > 0 && t.y < MAXDUNY && grid.at(t).getActor() > 0) {
				dir = GetDirection(s, t);
				AddMissile(s, t, dir, MissileType::LIGHTCTRL, 0, id, 1, missile[i]._mispllvl);
			}
			l += 2;
		}
	}
	missile[i]._mirange--;
	if (missile[i]._mirange == 0)
		missile[i]._miDelFlag = true;
}

void mi_null_11(int i)
{
	missile[i]._mirange--;
	if (!missile[i]._mirange)
		missile[i]._miDelFlag = true;
	if (missile[i]._miAnimFrame == missile[i]._miAnimLen)
		missile[i]._miPreFlag = true;
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
		missiledata[missile[i]._mitype].mResist = MissileResistance::FIRE;
	} else {
		mind = plr[id].data._pILMinDam;
		maxd = plr[id].data._pILMaxDam;
		missiledata[missile[i]._mitype].mResist = MissileResistance::LIGHTNING;
	}
	CheckMissileCol(i, mind, maxd, false, missile[i]._mi, false);
	if (!missile[i]._miVar1) {
		missile[i]._mlid = AddLight(missile[i]._mi, 9);
	} else {
		if (missile[i]._mirange)
			ChangeLight(missile[i]._mlid, missile[i]._mi, ExpLight[missile[i]._miVar1]);
	}
	missile[i]._miVar1++;
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = true;
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
		missile[i]._miDelFlag = true;
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
		missile[i]._miDelFlag = true;
		monst = missile[i]._misource;
		dam = missile[i]._mispllvl;
		AddMissile(missile[i]._mi, { i, 0 }, Dir(int(missile[i]._mimfnum)), MissileType::ACIDPUD, 1, monst, (monsters[monst].data.MData->mLevel >= 2) + 1, dam);
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
		missile[i]._miDelFlag = true;
	} else {
		//grid.at(plr[id].pos()).dPlayer = 0;
		PlrClrTrans(plr[id].pos());
		plr[id].changePos(missile[i]._mi);
		plr[id].changeFutPos(plr[id].pos());
		plr[id].data._pold = plr[id].pos();
		PlrDoTrans(plr[id].pos());
		missile[i]._miVar1 = 1;
		//grid.at(plr[id].pos()).dPlayer = id + 1;
		if (lvl.type() != DunType::town) {
			ChangeLightXY(plr[id].data._plid, plr[id].pos());
			ChangeVisionXY(plr[id].data._pvid, plr[id].pos());
		}
		if (id == myplr()) {
			View = plr[id].pos() - ScrollInfo._sd;
		}
	}
}

void MI_Stone(int i)
{
	int m;

	missile[i]._mirange--;
	m = missile[i]._miVar2;
	if (!monsters[m].data._mhitpoints && missile[i]._miAnimType != MissileGraphic::SHATTER1) {
		missile[i]._mimfnum = Dir(0);
		missile[i]._miDrawFlag = true;
		SetMissAnim(i, MissileGraphic::SHATTER1);
		missile[i]._mirange = 11;
	}
	if (monsters[m].data._mmode != MonsterMode::STONE) {
		missile[i]._miDelFlag = true;
		return;
	}

	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = true;
		if (monsters[m].data._mhitpoints > 0) {
			monsters[m].data._mmode = missile[i]._miVar1;
		} else {
			AddDead(monsters[m].data._m, stonendx, monsters[m].data._mdir);
		}
	}
	if (missile[i]._miAnimType == MissileGraphic::SHATTER1)
		PutMissile(i);
}

void MI_Boom(int i)
{
	missile[i]._mirange--;
	if (!missile[i]._miVar1)
		CheckMissileCol(i, missile[i]._midam, missile[i]._midam, false, missile[i]._mi, true);
	if (missile[i]._miHitFlag == true)
		missile[i]._miVar1 = 1;
	if (!missile[i]._mirange)
		missile[i]._miDelFlag = true;
	PutMissile(i);
}

void MI_Rhino(int i)
{
	V2Di mi, mi2, om;

	int monst = missile[i]._misource;
	if (monsters[monst].data._mmode != MonsterMode::CHARGE) {
		missile[i]._miDelFlag = true;
		return;
	}
	GetMissilePos(i);
	mi = missile[i]._mi;
	grid.at(mi).clearMonster();
	if (monsters[monst].data._mAi == MonstAi::SNAKE) {
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
	if (!PosOkMonst(monst, missile[i]._mi) || (monsters[monst].data._mAi == MonstAi::SNAKE && !PosOkMonst(monst, mi2))) {
		MissToMonst(i, mi);
		missile[i]._miDelFlag = true;
		return;
	}
	monsters[monst].data._mfut = om;
	grid.at(om).setActor(monst);
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
	if (!(monsters[src].data._mFlags & MonsterFlag::targets_monster)) {
		c = plr[enemy].pos();
	} else {
		c = monsters[enemy].data._m;
	}
	if ((b.x != a.x || b.y != a.y) && (missile[i]._miVar1 & 1 && (abs(a.x - c.x) >= 4 || abs(a.y - c.y) >= 4) ||
		missile[i]._miVar2 > 1) && PosOkMonst(missile[i]._misource, a)) {
		MissToMonst(i, a);
		missile[i]._miDelFlag = true;
	} else if (!(monsters[src].data._mFlags & MonsterFlag::targets_monster)) {
		j = grid.at(b).isPlayer();
	} else {
		j = grid.at(b).getActor();
	}
	if (!PosOkMissile(b) || j > 0 && !(missile[i]._miVar1 & 1)) {
		missile[i]._mivel.x *= -1;
		missile[i]._mivel.y *= -1;
		missile[i]._mimfnum = opposite(missile[i]._mimfnum);
		missile[i]._miAnimData = monsters[src].data.MType->Anims[MonstAnim::WALK].Data[int(missile[i]._mimfnum)];
		missile[i]._miVar2++;
		if (j > 0)
			missile[i]._miVar1 |= 1;
	}
	MoveMissilePos(i);
	PutMissile(i);
}

void MI_FirewallC(int i)
{
	int id;
	V2Di t;

	id = missile[i]._misource;
	missile[i]._mirange--;
	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = true;
	} else {
		t.x = missile[i]._miVar1 + offset(Dir(missile[i]._miVar3)).x;
		t.y = missile[i]._miVar2 + offset(Dir(missile[i]._miVar3)).y;
		if (!grid[missile[i]._miVar1][missile[i]._miVar2].blocksMissile() && !missile[i]._miVar8 && grid.isValid(t)) {
			AddMissile({ missile[i]._miVar1, missile[i]._miVar2 }, { missile[i]._miVar1, missile[i]._miVar2 }, plr[id].data._pdir, MissileType::FIREWALL, 0, id, 0, missile[i]._mispllvl);
			missile[i]._miVar1 = t.x;
			missile[i]._miVar2 = t.y;
		} else {
			missile[i]._miVar8 = 1;
		}
		t.x = missile[i]._miVar5 + offset(Dir(missile[i]._miVar4)).x;
		t.y = missile[i]._miVar6 + offset(Dir(missile[i]._miVar4)).y;
		if (!grid[missile[i]._miVar5][missile[i]._miVar6].blocksMissile() && !missile[i]._miVar7 && grid.isValid(t)) {
			AddMissile({ missile[i]._miVar5, missile[i]._miVar6 }, { missile[i]._miVar5, missile[i]._miVar6 }, plr[id].data._pdir, MissileType::FIREWALL, 0, id, 0, missile[i]._mispllvl);
			missile[i]._miVar5 = t.x;
			missile[i]._miVar6 = t.y;
		} else {
			missile[i]._miVar7 = 1;
		}
	}
}

void MI_Infra(int i)
{
	missile[i]._mirange--;
	plr[missile[i]._misource].data._pInfraFlag = true;
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = true;
		CalcPlrItemVals(missile[i]._misource, true);
	}
}

void MI_Apoca(int i)
{
	int j, k, id;
	bool exit;

	id = missile[i]._misource;
	exit = false;
	for (j = missile[i]._miVar2; j < missile[i]._miVar3 && !exit; j++) {
		for (k = missile[i]._miVar4; k < missile[i]._miVar5 && !exit; k++) {
			if (grid[k][j].getActor() > 3 && !grid[k][j].isSolid()) {
				AddMissile({ k, j }, { k, j }, plr[id].data._pdir, MissileType::BOOM, 0, id, missile[i]._midam, 0);
				exit = true;
			}
		}
		if (!exit) {
			missile[i]._miVar4 = missile[i]._miVar6;
		}
	}

	if (exit == true) {
		missile[i]._miVar2 = j - 1;
		missile[i]._miVar4 = k;
	} else {
		missile[i]._miDelFlag = true;
	}
}

void MI_Wave(int i)
{
	V2Di s, na, nb, v;
	int j, id, pn;

	bool f1 = false;
	bool f2 = false;
	assert((DWORD)i < MAXMISSILES);

	id = missile[i]._misource;
	s = missile[i]._mi;
	v.x = missile[i]._miVar1;
	v.y = missile[i]._miVar2;
	Dir sd = GetDirection(s, v);
	Dir dira = rotate(sd, -2);
	Dir dirb = rotate(sd, 2);
	na = s + offset(sd);
	pn = grid.at(na).getPiece();
	assert((DWORD)pn <= MAXTILES);
	if (!pieces[pn].missile) {
		AddMissile(na, na + offset(sd), plr[id].data._pdir, MissileType::FIREMOVE, 0, id, 0, missile[i]._mispllvl);
		na += offset(dira);
		nb = s + offset(sd) + offset(dirb);
		for (j = 0; j < (missile[i]._mispllvl >> 1) + 2; j++) {
			pn = grid.at(na).getPiece(); // BUGFIX: dPiece is accessed before check against dungeon size and 0
			assert((DWORD)pn <= MAXTILES);
			if (pieces[pn].missile || f1 || na.x <= 0 || na.x >= MAXDUNX || na.y <= 0 || na.y >= MAXDUNY) {
				f1 = true;
			} else {
				AddMissile(na, na+ offset(sd), plr[id].data._pdir, MissileType::FIREMOVE, 0, id, 0, missile[i]._mispllvl);
				na += offset(dira);
			}
			pn = grid.at(nb).getPiece(); // BUGFIX: dPiece is accessed before check against dungeon size and 0
			assert((DWORD)pn <= MAXTILES);
			if (pieces[pn].missile || f2 || nb.x <= 0 || nb.x >= MAXDUNX || nb.y <= 0 || nb.y >= MAXDUNY) {
				f2 = true;
			} else {
				AddMissile(nb, nb + offset(sd), plr[id].data._pdir, MissileType::FIREMOVE, 0, id, 0, missile[i]._mispllvl);
				nb += offset(dirb);
			}
		}
	}
	missile[i]._mirange--;
	if (missile[i]._mirange == 0)
		missile[i]._miDelFlag = true;
}

void MI_Nova(int i)
{
	int k, id, en, dam;
	V2Di s, s1;
	Dir dir;

	id = missile[i]._misource;
	dam = missile[i]._midam;
	s = missile[i]._mi;
	if (id != -1) {
		en = 0;
		dir = plr[id].data._pdir;
	} else {
		dir = Dir(0);
		en = 1;
	}
	for (k = 0; k < 23; k++) {
		if (s1.x != vCrawlTable[k][6] || s1.y != vCrawlTable[k][7]) {
			AddMissile(s, { s.x + vCrawlTable[k][6], s.y + vCrawlTable[k][7] }, dir, MissileType::LIGHTBALL, en, id, dam, missile[i]._mispllvl);
			AddMissile(s, { s.x - vCrawlTable[k][6], s.y - vCrawlTable[k][7] }, dir, MissileType::LIGHTBALL, en, id, dam, missile[i]._mispllvl);
			AddMissile(s, { s.x - vCrawlTable[k][6], s.y + vCrawlTable[k][7] }, dir, MissileType::LIGHTBALL, en, id, dam, missile[i]._mispllvl);
			AddMissile(s, { s.x + vCrawlTable[k][6], s.y - vCrawlTable[k][7] }, dir, MissileType::LIGHTBALL, en, id, dam, missile[i]._mispllvl);
			s1.x = vCrawlTable[k][6];
			s1.y = vCrawlTable[k][7];
		}
	}
	missile[i]._mirange--;
	if (missile[i]._mirange == 0)
		missile[i]._miDelFlag = true;
}

void MI_Blodboil(int i)
{
	missile[i]._miDelFlag = true;
}

void MI_Flame(int i)
{
	int k;

	missile[i]._mirange--;
	missile[i]._miVar2--;
	k = missile[i]._mirange;
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, true, missile[i]._mi, false);
	if (missile[i]._mirange == 0 && missile[i]._miHitFlag == true)
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
		missile[i]._miDelFlag = true;
		AddUnLight(missile[i]._mlid);
	}
	if (missile[i]._miVar2 <= 0)
		PutMissile(i);
}

void MI_Flamec(int i)
{
	missile[i]._mirange--;
	int src = missile[i]._misource;
	missile[i]._mitoff += missile[i]._mivel;
	GetMissilePos(i);
	if (missile[i]._mi.x != missile[i]._miVar1 || missile[i]._mi.y != missile[i]._miVar2) {
		int id = grid.at(missile[i]._mi).getPiece();
		if (!pieces[id].missile) {
			AddMissile(
			    missile[i]._mi,
			    missile[i]._mis,
			    Dir(i),
			    MissileType::FLAME,
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
		missile[i]._miDelFlag = true;
}

void MI_Cbolt(int i)
{
	Dir md;
	int bpath[16] = { -1, 0, 1, -1, 0, 1, -1, -1, 0, 0, 1, 1, 0, 1, -1, 0 };

	missile[i]._mirange--;
	if (missile[i]._miAnimType != MissileGraphic::LGHNING) {
		if (!missile[i]._miVar3) {
			md = rotate(Dir(missile[i]._miVar2), bpath[missile[i]._mirnd]);
			missile[i]._mirnd = (missile[i]._mirnd + 1) & 0xF;
			GetMissileVel(i, missile[i]._mi, missile[i]._mi + offset(md), 8);
			missile[i]._miVar3 = 16;
		} else {
			missile[i]._miVar3--;
		}
		missile[i]._mitoff += missile[i]._mivel;
		GetMissilePos(i);
		CheckMissileCol(i, missile[i]._midam, missile[i]._midam, false, missile[i]._mi, false);
		if (missile[i]._miHitFlag == true) {
			missile[i]._miVar1 = 8;
			missile[i]._mimfnum = Dir(0);
			missile[i]._mioff = { 0, 0 };
			SetMissAnim(i, MissileGraphic::LGHNING);
			missile[i]._mirange = missile[i]._miAnimLen;
			GetMissilePos(i);
		}
		ChangeLight(missile[i]._mlid, missile[i]._mi, missile[i]._miVar1);
	}
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = true;
		AddUnLight(missile[i]._mlid);
	}
	PutMissile(i);
}

void MI_Hbolt(int i)
{
	int dam;

	missile[i]._mirange--;
	if (missile[i]._miAnimType != MissileGraphic::HOLYEXPL) {
		missile[i]._mitoff += missile[i]._mivel;
		GetMissilePos(i);
		dam = missile[i]._midam;
		if (missile[i]._mi.x != missile[i]._mis.x || missile[i]._mi.y != missile[i]._mis.y) {
			CheckMissileCol(i, dam, dam, false, missile[i]._mi, false);
		}
		if (!missile[i]._mirange) {
			missile[i]._mitoff -= missile[i]._mivel;
			GetMissilePos(i);
			missile[i]._mimfnum = Dir(0);
			SetMissAnim(i, MissileGraphic::HOLYEXPL);
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
			missile[i]._miDelFlag = true;
			AddUnLight(missile[i]._mlid);
		}
	}
	PutMissile(i);
}

void MI_Element(int i)
{
	int mid, dam, id;
	V2Di c, p;

	missile[i]._mirange--;
	dam = missile[i]._midam;
	id = missile[i]._misource;
	if (missile[i]._miAnimType == MissileGraphic::BIGEXP) {
		c = missile[i]._mi;
		p = plr[id].pos();
		ChangeLight(missile[i]._mlid, c, missile[i]._miAnimFrame);
		if (!CheckBlock(p, c))
			CheckMissileCol(i, dam, dam, true, c, true);
		if (!CheckBlock(p, { c.x, c.y + 1 }))
			CheckMissileCol(i, dam, dam, true, { c.x, c.y + 1 }, true);
		if (!CheckBlock(p, { c.x, c.y - 1 }))
			CheckMissileCol(i, dam, dam, true, { c.x, c.y - 1 }, true);
		if (!CheckBlock(p, { c.x + 1, c.y }))
			CheckMissileCol(i, dam, dam, true, { c.x + 1, c.y }, true); /* check x/y */
		if (!CheckBlock(p, { c.x + 1, c.y - 1 }))
			CheckMissileCol(i, dam, dam, true, { c.x + 1, c.y - 1 }, true);
		if (!CheckBlock(p, { c.x + 1, c.y + 1 }))
			CheckMissileCol(i, dam, dam, true, { c.x + 1, c.y + 1 }, true);
		if (!CheckBlock(p, { c.x - 1, c.y }))
			CheckMissileCol(i, dam, dam, true, { c.x - 1, c.y }, true);
		if (!CheckBlock(p, { c.x - 1, c.y + 1 }))
			CheckMissileCol(i, dam, dam, true, { c.x - 1, c.y + 1 }, true);
		if (!CheckBlock(p, { c.x - 1, c.y - 1 }))
			CheckMissileCol(i, dam, dam, true, { c.x - 1, c.y - 1 }, true);
		if (!missile[i]._mirange) {
			missile[i]._miDelFlag = true;
			AddUnLight(missile[i]._mlid);
		}
	} else {
		missile[i]._mitoff += missile[i]._mivel;
		GetMissilePos(i);
		c = missile[i]._mi;
		CheckMissileCol(i, dam, dam, false, c, false);
		if (!missile[i]._miVar3 && c.x == missile[i]._miVar4 && c.y == missile[i]._miVar5)
			missile[i]._miVar3 = 1;
		if (missile[i]._miVar3 == 1) {
			missile[i]._miVar3 = 2;
			missile[i]._mirange = 255;
			mid = FindClosest(c, 19);
			Dir sd;
			if (mid > 0) {
				sd = GetDirection8(c, monsters[mid].data._m);
				SetMissDir(i, sd);
				GetMissileVel(i, c, monsters[mid].data._m, 16);
			} else {
				sd = plr[id].data._pdir;
				SetMissDir(i, sd);
				GetMissileVel(i, c, c + offset(sd), 16);
			}
		}
		if (c.x != missile[i]._miVar1 || c.y != missile[i]._miVar2) {
			missile[i]._miVar1 = c.x;
			missile[i]._miVar2 = c.y;
			ChangeLight(missile[i]._mlid, c, 8);
		}
		if (!missile[i]._mirange) {
			missile[i]._mimfnum = Dir(0);
			SetMissAnim(i, MissileGraphic::BIGEXP);
			missile[i]._mirange = missile[i]._miAnimLen - 1;
		}
	}
	PutMissile(i);
}

void MI_Bonespirit(int i)
{
	int id, mid, dam;
	V2Di c;

	missile[i]._mirange--;
	dam = missile[i]._midam;
	id = missile[i]._misource;
	if (int(missile[i]._mimfnum) == 8) {
		ChangeLight(missile[i]._mlid, missile[i]._mi, missile[i]._miAnimFrame);
		if (!missile[i]._mirange) {
			missile[i]._miDelFlag = true;
			AddUnLight(missile[i]._mlid);
		}
		PutMissile(i);
	} else {
		missile[i]._mitoff += missile[i]._mivel;
		GetMissilePos(i);
		c = missile[i]._mi;
		CheckMissileCol(i, dam, dam, false, c, false);
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
				Dir sd = plr[id].data._pdir;
				SetMissDir(i, sd);
				GetMissileVel(i, c, c + offset(sd), 16);
			}
		}
		if (c.x != missile[i]._miVar1 || c.y != missile[i]._miVar2) {
			missile[i]._miVar1 = c.x;
			missile[i]._miVar2 = c.y;
			ChangeLight(missile[i]._mlid, c, 8);
		}
		if (!missile[i]._mirange) {
			SetMissDir(i, Dir::OMNI);
			missile[i]._mirange = 7;
		}
		PutMissile(i);
	}
}

void MI_ResurrectBeam(int i)
{
	missile[i]._mirange--;
	if (missile[i]._mirange == 0)
		missile[i]._miDelFlag = true;
	PutMissile(i);
}

void MI_Rportal(int i)
{
	int ExpLight[17] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 15, 15 };

	if (missile[i]._mirange > 1)
		missile[i]._mirange--;
	if (missile[i]._mirange == missile[i]._miVar1)
		SetMissDir(i, Dir(1));

	if (lvl.currlevel && int(missile[i]._mimfnum) != 1 && missile[i]._mirange != 0) {
		if (!missile[i]._miVar2)
			missile[i]._mlid = AddLight(missile[i]._mi, 1);
		ChangeLight(missile[i]._mlid, missile[i]._mi, ExpLight[missile[i]._miVar2]);
		missile[i]._miVar2++;
	}
	if (!missile[i]._mirange) {
		missile[i]._miDelFlag = true;
		AddUnLight(missile[i]._mlid);
	}
	PutMissile(i);
}

void ProcessMissiles()
{
	int i, mi;

	for (i = 0; i < nummissiles; i++) {
		grid.at(missile[missileactive[i]]._mi).dFlags &= ~DunTileFlag::MISSILE;
		grid.at(missile[missileactive[i]]._mi).clearMissile();
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

	MissilePreFlag = false;
	ManashieldFlag = false;

	for (i = 0; i < nummissiles; i++) {
		mi = missileactive[i];
		missiledata[missile[mi]._mitype].mProc(missileactive[i]);
		if (!(missile[mi]._miAnimFlags & MonsterFlag::lock_animation)) {
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
			if (missile[missileactive[i]]._mitype == MissileType::MANASHIELD) {
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
		mis->_miAnimData = misfiledata[mis->_miAnimType].mAnimData[int(mis->_mimfnum)];
		if (mis->_mitype == MissileType::RHINO) {
			mon = monsters[mis->_misource].data.MType;
			if (mon->mtype >= MonsterType::HORNED && mon->mtype <= MonsterType::OBLORD) {
				anim = &mon->Anims[MonstAnim::SPECIAL];
			} else {
				if (mon->mtype >= MonsterType::NSNAKE && mon->mtype <= MonsterType::GSNAKE)
					anim = &mon->Anims[MonstAnim::ATTACK];
				else
					anim = &mon->Anims[MonstAnim::WALK];
			}
			missile[mi]._miAnimData = anim->Data[int(mis->_mimfnum)];
		}
	}
}

void ClearMissileSpot(int mi)
{
	grid.at(missile[mi]._mi).dFlags &= ~DunTileFlag::MISSILE;
	grid.at(missile[mi]._mi).clearMissile();
}

DEVILUTION_END_NAMESPACE
