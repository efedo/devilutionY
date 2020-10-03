#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

int GetManaAmount(int id, int sn)
{
	int ma; // mana amount

	// mana adjust
	int adj = 0;

	// spell level
	int sl = plr[id].data._pSplLvl[sn] + plr[id].data._pISplLvlAdd - 1;

	if (sl < 0) {
		sl = 0;
	}

	if (sn == SpellId::FIREBOLT) {
		adj >>= 1;
	} else if (sn == SpellId::RESURRECT && sl > 0) {
		adj = sl * (spelldata[SpellId::RESURRECT].sManaCost / 8);
	} else if (sl > 0) {
		adj = sl * spelldata[sn].sManaAdj;
	}

	if (sn == SpellId::HEAL || sn == SpellId::HEALOTHER) {
		ma = (spelldata[SpellId::HEAL].sManaCost + 2 * plr[id].data._pLevel - adj);
	} else if (spelldata[sn].sManaCost == 255) {
		ma = ((uint8_t)plr[id].data._pMaxManaBase - adj);
	} else {
		ma = (spelldata[sn].sManaCost - adj);
	}

	if (ma < 0)
		ma = 0;
	ma <<= 6;

	if (plr[id].data._pClass == PlayerClass::rogue) {
		ma -= ma >> 2;
	}

	if (spelldata[sn].sMinMana > ma >> 6) {
		ma = spelldata[sn].sMinMana << 6;
	}

	return ma * (100 - plr[id].data._pISplCost) / 100;
}

void UseMana(int id, int sn)
{
	int ma; // mana cost

	if (id == myplr()) {
		switch (plr[id].data._pSplType) {
		case RSpellType::SKILL:
		case RSpellType::INVALID:
			break;
		case RSpellType::SCROLL:
			plr[id].inv.UseScroll();
			break;
		case RSpellType::CHARGES:
			plr[id].inv.UseStaffCharge();
			break;
		case RSpellType::SPELL:
#ifdef _DEBUG
			if (!debug_mode_key_inverted_v) {
#endif
				ma = GetManaAmount(id, sn);
				plr[id].data._pMana -= ma;
				plr[id].data._pManaBase -= ma;
				drawmanaflag = true;
#ifdef _DEBUG
			}
#endif
			break;
		}
	}
}

bool CheckSpell(int id, int sn, char st, bool manaonly)
{
	bool result;

#ifdef _DEBUG
	if (debug_mode_key_inverted_v)
		return true;
#endif

	result = true;
	if (!manaonly && pcurs != CURSOR_HAND) {
		result = false;
	} else {
		if (st != RSpellType::SKILL) {
			if (GetSpellLevel(id, sn) <= 0) {
				result = false;
			} else {
				result = plr[id].data._pMana >= GetManaAmount(id, sn);
			}
		}
	}

	return result;
}

void CastSpell(int id, int spl, V2Di s, V2Di d, int caster, int spllvl)
{
	int i;
	Dir dir; // missile direction

	switch (caster) {
	case 1:
		dir = monsters[id].data._mdir;
		break;
	case 0:
		// caster must be 0 already in this case, but oh well,
		// it's needed to generate the right code
		caster = 0;
		dir = plr[id].data._pdir;

		if (spl == SpellId::FIREWALL) {
			dir = Dir(plr[id].data._pVar3);
		}
		break;
	}

	for (i = 0; spelldata[spl].sMissiles[i] != MissileType::ARROW && i < 3; i++) {
		AddMissile(s, d, dir, spelldata[spl].sMissiles[i], caster, id, 0, spllvl);
	}

	if (spelldata[spl].sMissiles[0] == MissileType::TOWN) {
		UseMana(id, SpellId::TOWN);
	}
	if (spelldata[spl].sMissiles[0] == MissileType::CBOLT) {
		UseMana(id, SpellId::CBOLT);

		for (i = (spllvl >> 1) + 3; i > 0; i--) {
			AddMissile(s, d, dir, MissileType::CBOLT, caster, id, 0, spllvl);
		}
	}
}

static void PlacePlayer(int pnum)
{
	V2Di n, p;
	int max, min, x, y;
	DWORD i;
	bool done;

	if (plr[pnum].data.plrlevel == lvl.currlevel) {
		for (i = 0; i < 8; i++) {
			n = plr[pnum].pos() + plroff2[i];

			if (PosOkPlayer(pnum, n)) {
				break;
			}
		}

		if (!PosOkPlayer(pnum, n)) {
			done = false;

			for (max = 1, min = -1; min > -50 && !done; max++, min--) {
				for (y = min; y <= max && !done; y++) {
					n.y = plr[pnum].pos().y + y;
					for (x = min; x <= max && !done; x++) {
						n.x = plr[pnum].pos().x + x;
						if (PosOkPlayer(pnum, n)) {
							done = true;
						}
					}
				}
			}
		}

		plr[pnum].changePos(n);
		if (pnum == myplr()) View = n;
	}
}

/**
 * @param pnum player index
 * @param rid target player index
 */
void DoResurrect(int pnum, int rid)
{
	int hp;

	if ((char)rid != -1) {
		AddMissile(plr[rid].pos(), plr[rid].pos(), Dir(0), MissileType::RESURRECTBEAM, 0, pnum, 0, 0);
	}

	if (pnum == myplr()) {
		NewCursor(CURSOR_HAND);
	}

	if ((char)rid != -1 && plr[rid].data._pHitPoints == 0) {
		if (rid == myplr()) {
			deathflag = false;
			gamemenu_off();
			drawhpflag = true;
			drawmanaflag = true;
		}

		plr[rid].ClrPlrPath();
		plr[rid].data.destAction = DestinationAction::NONE;
		plr[rid].data._pInvincible = false;
		PlacePlayer(rid);

		hp = 640;
		if (plr[rid].data._pMaxHPBase < 640) {
			hp = plr[rid].data._pMaxHPBase;
		}
		plr[rid].SetPlayerHitPoints(hp);

		plr[rid].data._pHPBase = plr[rid].data._pHitPoints + (plr[rid].data._pMaxHPBase - plr[rid].data._pMaxHP);
		plr[rid].data._pMana = 0;
		plr[rid].data._pManaBase = plr[rid].data._pMana + (plr[rid].data._pMaxManaBase - plr[rid].data._pMaxMana);

		plr[rid].CalcPlrInv(true);

		if (plr[rid].data.plrlevel == lvl.currlevel) {
			plr[rid].StartStand(plr[rid].data._pdir);
		} else {
			plr[rid].data._pmode = PlayerMode::STAND;
		}
	}
}

void DoHealOther(int pnum, int rid)
{
	int i, j, hp;

	if (pnum == myplr()) {
		NewCursor(CURSOR_HAND);
	}

	if ((char)rid != -1 && (plr[rid].data._pHitPoints >> 6) > 0) {
		hp = (random_(57, 10) + 1) << 6;

		for (i = 0; i < plr[pnum].data._pLevel; i++) {
			hp += (random_(57, 4) + 1) << 6;
		}

		for (j = 0; j < GetSpellLevel(pnum, SpellId::HEALOTHER); ++j) {
			hp += (random_(57, 6) + 1) << 6;
		}

		if (plr[pnum].data._pClass == PlayerClass::warrior) {
			hp <<= 1;
		}

		if (plr[pnum].data._pClass == PlayerClass::rogue) {
			hp += hp >> 1;
		}

		plr[rid].data._pHitPoints += hp;

		if (plr[rid].data._pHitPoints > plr[rid].data._pMaxHP) {
			plr[rid].data._pHitPoints = plr[rid].data._pMaxHP;
		}

		plr[rid].data._pHPBase += hp;

		if (plr[rid].data._pHPBase > plr[rid].data._pMaxHPBase) {
			plr[rid].data._pHPBase = plr[rid].data._pMaxHPBase;
		}

		drawhpflag = true;
	}
}

DEVILUTION_END_NAMESPACE
