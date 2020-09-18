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

	if (sn == SPL_FIREBOLT) {
		adj >>= 1;
	} else if (sn == SPL_RESURRECT && sl > 0) {
		adj = sl * (spelldata[SPL_RESURRECT].sManaCost / 8);
	} else if (sl > 0) {
		adj = sl * spelldata[sn].sManaAdj;
	}

	if (sn == SPL_HEAL || sn == SPL_HEALOTHER) {
		ma = (spelldata[SPL_HEAL].sManaCost + 2 * plr[id].data._pLevel - adj);
	} else if (spelldata[sn].sManaCost == 255) {
		ma = ((BYTE)plr[id].data._pMaxManaBase - adj);
	} else {
		ma = (spelldata[sn].sManaCost - adj);
	}

	if (ma < 0)
		ma = 0;
	ma <<= 6;

	if (plr[id].data._pClass == PC_ROGUE) {
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
		case RSPLTYPE_SKILL:
		case RSPLTYPE_INVALID:
			break;
		case RSPLTYPE_SCROLL:
			plr[id].inventory.RemoveScroll();
			break;
		case RSPLTYPE_CHARGES:
			plr[id].inventory.UseStaffCharge();
			break;
		case RSPLTYPE_SPELL:
#ifdef _DEBUG
			if (!debug_mode_key_inverted_v) {
#endif
				ma = GetManaAmount(id, sn);
				plr[id].data._pMana -= ma;
				plr[id].data._pManaBase -= ma;
				drawmanaflag = TRUE;
#ifdef _DEBUG
			}
#endif
			break;
		}
	}
}

BOOL CheckSpell(int id, int sn, char st, BOOL manaonly)
{
	BOOL result;

#ifdef _DEBUG
	if (debug_mode_key_inverted_v)
		return TRUE;
#endif

	result = TRUE;
	if (!manaonly && pcurs != CURSOR_HAND) {
		result = FALSE;
	} else {
		if (st != RSPLTYPE_SKILL) {
			if (GetSpellLevel(id, sn) <= 0) {
				result = FALSE;
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
	int dir; // missile direction

	switch (caster) {
	case 1:
		dir = monsters[id].data._mdir;
		break;
	case 0:
		// caster must be 0 already in this case, but oh well,
		// it's needed to generate the right code
		caster = 0;
		dir = plr[id].data._pdir;

		if (spl == SPL_FIREWALL) {
			dir = plr[id].data._pVar3;
		}
		break;
	}

	for (i = 0; spelldata[spl].sMissiles[i] != MIS_ARROW && i < 3; i++) {
		AddMissile(s, d, dir, spelldata[spl].sMissiles[i], caster, id, 0, spllvl);
	}

	if (spelldata[spl].sMissiles[0] == MIS_TOWN) {
		UseMana(id, SPL_TOWN);
	}
	if (spelldata[spl].sMissiles[0] == MIS_CBOLT) {
		UseMana(id, SPL_CBOLT);

		for (i = (spllvl >> 1) + 3; i > 0; i--) {
			AddMissile(s, d, dir, MIS_CBOLT, caster, id, 0, spllvl);
		}
	}
}

static void PlacePlayer(int pnum)
{
	int nx, ny, max, min, x, y;
	DWORD i;
	BOOL done;

	if (plr[pnum].data.plrlevel == level.currlevel) {
		for (i = 0; i < 8; i++) {
			nx = plr[pnum].data._p.x + plrxoff2[i];
			ny = plr[pnum].data._p.y + plryoff2[i];

			if (PosOkPlayer(pnum, { nx, ny })) {
				break;
			}
		}

		if (!PosOkPlayer(pnum, { nx, ny })) {
			done = FALSE;

			for (max = 1, min = -1; min > -50 && !done; max++, min--) {
				for (y = min; y <= max && !done; y++) {
					ny = plr[pnum].data._p.y + y;
					for (x = min; x <= max && !done; x++) {
						nx = plr[pnum].data._p.x + x;
						if (PosOkPlayer(pnum, { nx, ny })) {
							done = TRUE;
						}
					}
				}
			}
		}

		plr[pnum].data._p = { nx, ny };
		grid[nx][ny].dPlayer = pnum + 1;

		if (pnum == myplr()) {
			View = { nx, ny };
		}
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
		AddMissile(plr[rid].data._p, plr[rid].data._p, 0, MIS_RESURRECTBEAM, 0, pnum, 0, 0);
	}

	if (pnum == myplr()) {
		NewCursor(CURSOR_HAND);
	}

	if ((char)rid != -1 && plr[rid].data._pHitPoints == 0) {
		if (rid == myplr()) {
			deathflag = FALSE;
			gamemenu_off();
			drawhpflag = TRUE;
			drawmanaflag = TRUE;
		}

		plr[rid].ClrPlrPath();
		plr[rid].data.destAction = ACTION_NONE;
		plr[rid].data._pInvincible = FALSE;
		PlacePlayer(rid);

		hp = 640;
		if (plr[rid].data._pMaxHPBase < 640) {
			hp = plr[rid].data._pMaxHPBase;
		}
		plr[rid].SetPlayerHitPoints(hp);

		plr[rid].data._pHPBase = plr[rid].data._pHitPoints + (plr[rid].data._pMaxHPBase - plr[rid].data._pMaxHP);
		plr[rid].data._pMana = 0;
		plr[rid].data._pManaBase = plr[rid].data._pMana + (plr[rid].data._pMaxManaBase - plr[rid].data._pMaxMana);

		CalcPlrInv(rid, TRUE);

		if (plr[rid].data.plrlevel == level.currlevel) {
			plr[rid].StartStand(plr[rid].data._pdir);
		} else {
			plr[rid].data._pmode = PM_STAND;
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

		for (j = 0; j < GetSpellLevel(pnum, SPL_HEALOTHER); ++j) {
			hp += (random_(57, 6) + 1) << 6;
		}

		if (plr[pnum].data._pClass == PC_WARRIOR) {
			hp <<= 1;
		}

		if (plr[pnum].data._pClass == PC_ROGUE) {
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

		drawhpflag = TRUE;
	}
}

DEVILUTION_END_NAMESPACE
