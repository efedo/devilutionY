/**
 * @file inv.cpp
 *
 * Inv display rectangle
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

void Player::CalcPlrItemVals(bool Loadgfx)
{
	int pvid;
	Dir d;

	int mind = 0; // min damage
	int maxd = 0; // max damage
	int tac = 0;  // accuracy

	int g;
	int i;
	int mi;

	int bdam = 0;   // bonus damage
	int btohit = 0; // bonus chance to hit
	int bac = 0;    // bonus accuracy

	int iflgs = ISPL_NONE; // item_special_effect flags

	int sadd = 0; // added strength
	int madd = 0; // added magic
	int dadd = 0; // added dexterity
	int vadd = 0; // added vitality

	unsigned __int64 spl = 0; // bitarray for all enabled/active spells

	int fr = 0; // fire resistance
	int lr = 0; // lightning resistance
	int mr = 0; // magic resistance

	int dmod = 0; // bonus damage mod?
	int ghit = 0; // increased damage from enemies

	int lrad = 10; // light radius

	int ihp = 0;   // increased HP
	int imana = 0; // increased mana

	int spllvladd = 0; // increased spell level
	int enac = 0;      // enhanced accuracy

	int fmin = 0; // minimum fire damage
	int fmax = 0; // maximum fire damage
	int lmin = 0; // minimum lightning damage
	int lmax = 0; // maximum lightning damage

	for (i = 0; i < NUM_INVLOC; i++) {
		ItemStruct *itm = &data.InvBody[i];
		if (itm->_itype != ITYPE_NONE && itm->_iStatFlag) {

			tac += itm->_iAC;
			mind += itm->_iMinDam;
			maxd += itm->_iMaxDam;

			if (itm->_iSpell != SPL_NULL) {
				spl |= (unsigned __int64)1 << (itm->_iSpell - 1);
			}

			if (itm->_iMagical == ITEM_QUALITY_NORMAL || itm->_iIdentified) {
				bdam += itm->_iPLDam;
				btohit += itm->_iPLToHit;
				if (itm->_iPLAC) {
					int tmpac = itm->_iPLAC * itm->_iAC / 100;
					if (tmpac == 0)
						tmpac = 1;
					bac += tmpac;
				}
				iflgs |= itm->_iFlags;
				sadd += itm->_iPLStr;
				madd += itm->_iPLMag;
				dadd += itm->_iPLDex;
				vadd += itm->_iPLVit;
				fr += itm->_iPLFR;
				lr += itm->_iPLLR;
				mr += itm->_iPLMR;
				dmod += itm->_iPLDamMod;
				ghit += itm->_iPLGetHit;
				lrad += itm->_iPLLight;
				ihp += itm->_iPLHP;
				imana += itm->_iPLMana;
				spllvladd += itm->_iSplLvlAdd;
				enac += itm->_iPLEnAc;
				fmin += itm->_iFMinDam;
				fmax += itm->_iFMaxDam;
				lmin += itm->_iLMinDam;
				lmax += itm->_iLMaxDam;
			}
		}
	}

	if (mind == 0 && maxd == 0) {
		mind = 1;
		maxd = 1;

		if (inv.getBodyItem(BodyLoc::HandRight))

		if (data.InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_SHIELD && data.InvBody[INVLOC_HAND_LEFT]._iStatFlag) {
			maxd = 3;
		}

		if (data.InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_SHIELD && data.InvBody[INVLOC_HAND_RIGHT]._iStatFlag) {
			maxd = 3;
		}
	}

	data._pIMinDam = mind;
	data._pIMaxDam = maxd;
	data._pIAC = tac;
	data._pIBonusDam = bdam;
	data._pIBonusToHit = btohit;
	data._pIBonusAC = bac;
	data._pIFlags = iflgs;
	data._pIBonusDamMod = dmod;
	data._pIGetHit = ghit;

	if (lrad < 2) {
		lrad = 2;
	}
	if (lrad > 15) {
		lrad = 15;
	}

	if (data._pLightRad != lrad && p == myplr()) {
		ChangeLightRadius(data._plid, lrad);

		pvid = data._pvid;
		if (lrad >= 10) {
			ChangeVisionRadius(pvid, lrad);
		} else {
			ChangeVisionRadius(pvid, 10);
		}

		data._pLightRad = lrad;
	}

	data._pStrength = sadd + data._pBaseStr;
	if (myplr().data._pStrength <= 0) {
		myplr().data._pStrength = 0;
	}

	data._pMagic = madd + data._pBaseMag;
	if (myplr().data._pMagic <= 0) {
		myplr().data._pMagic = 0;
	}

	data._pDexterity = dadd + data._pBaseDex;
	if (myplr().data._pDexterity <= 0) {
		myplr().data._pDexterity = 0;
	}

	data._pVitality = vadd + data._pBaseVit;
	if (myplr().data._pVitality <= 0) {
		myplr().data._pVitality = 0;
	}

	if (data._pClass == PC_ROGUE) {
		data._pDamageMod = data._pLevel * (data._pStrength + data._pDexterity) / 200;
	} else {
		data._pDamageMod = data._pLevel * data._pStrength / 100;
	}

	data._pISpells = spl;

	// check if the current RSplType is a valid/allowed spell
	if (data._pRSplType == RSPLTYPE_CHARGES
	    && !(spl & ((unsigned __int64)1 << (data._pRSpell - 1)))) {
		data._pRSpell = SPL_INVALID;
		data._pRSplType = RSPLTYPE_INVALID;
		force_redraw = 255;
	}

	data._pISplLvlAdd = spllvladd;
	data._pIEnAc = enac;

	if (iflgs & ISPL_ALLRESZERO) {
		// reset resistances to zero if the respective special effect is active
		mr = 0;
		fr = 0;
		lr = 0;
	}

	if (mr > MAXRESIST)
		mr = MAXRESIST;
	data._pMagResist = mr;

	if (fr > MAXRESIST)
		fr = MAXRESIST;
	data._pFireResist = fr;

	if (lr > MAXRESIST)
		lr = MAXRESIST;
	data._pLghtResist = lr;

	if (data._pClass == PC_WARRIOR) {
		vadd *= 2;
	}
	if (data._pClass == PC_ROGUE) {
		vadd += vadd >> 1;
	}
	ihp += (vadd << 6);

	if (data._pClass == PC_SORCERER) {
		madd *= 2;
	}
	if (data._pClass == PC_ROGUE) {
		madd += madd >> 1;
	}
	imana += (madd << 6);

	data._pHitPoints = ihp + data._pHPBase;
	data._pMaxHP = ihp + data._pMaxHPBase;

	if (p == myplr() && (data._pHitPoints >> 6) <= 0) {
		SetPlayerHitPoints(0);
	}

	data._pMana = imana + data._pManaBase;
	data._pMaxMana = imana + data._pMaxManaBase;

	data._pIFMinDam = fmin;
	data._pIFMaxDam = fmax;
	data._pILMinDam = lmin;
	data._pILMaxDam = lmax;

	if (iflgs & ISPL_INFRAVISION) {
		data._pInfraFlag = TRUE;
	} else {
		data._pInfraFlag = false;
	}

	data._pBlockFlag = false;
	data._pwtype = WT_MELEE;

	g = 0;

	if (data.InvBody[INVLOC_HAND_LEFT]._itype != ITYPE_NONE
	    && data.InvBody[INVLOC_HAND_LEFT]._iClass == ICLASS_WEAPON
	    && data.InvBody[INVLOC_HAND_LEFT]._iStatFlag) {
		g = data.InvBody[INVLOC_HAND_LEFT]._itype;
	}

	if (data.InvBody[INVLOC_HAND_RIGHT]._itype != ITYPE_NONE
	    && data.InvBody[INVLOC_HAND_RIGHT]._iClass == ICLASS_WEAPON
	    && data.InvBody[INVLOC_HAND_RIGHT]._iStatFlag) {
		g = data.InvBody[INVLOC_HAND_RIGHT]._itype;
	}

	switch (g) {
	case ITYPE_SWORD:
		g = ANIM_ID_SWORD;
		break;
	case ITYPE_AXE:
		g = ANIM_ID_AXE;
		break;
	case ITYPE_BOW:
		data._pwtype = WT_RANGED;
		g = ANIM_ID_BOW;
		break;
	case ITYPE_MACE:
		g = ANIM_ID_MACE;
		break;
	case ITYPE_STAFF:
		g = ANIM_ID_STAFF;
		break;
	}

	if (data.InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_SHIELD && data.InvBody[INVLOC_HAND_LEFT]._iStatFlag) {
		data._pBlockFlag = TRUE;
		g++;
	}
	if (data.InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_SHIELD && data.InvBody[INVLOC_HAND_RIGHT]._iStatFlag) {
		data._pBlockFlag = TRUE;
		g++;
	}

	if (data.InvBody[INVLOC_CHEST]._itype == ITYPE_MARMOR && data.InvBody[INVLOC_CHEST]._iStatFlag) {
		g += ANIM_ID_MEDIUM_ARMOR;
	}
	if (data.InvBody[INVLOC_CHEST]._itype == ITYPE_HARMOR && data.InvBody[INVLOC_CHEST]._iStatFlag) {
		g += ANIM_ID_HEAVY_ARMOR;
	}

	if (data._pgfxnum != g && Loadgfx) {
		data._pgfxnum = g;
		data._pGFXLoad = 0;
		LoadPlrGFX(PFILE_STAND);
		SetPlrAnims();
		d = data._pdir;
		assert(data._pNAnim[size_t(d)]);
		data._pAnimData = data._pNAnim[int(d)];
		data._pAnimLen = data._pNFrames;
		data._pAnimFrame = 1;
		data._pAnimCnt = 0;
		data._pAnimDelay = 3;
		data._pAnimWidth = data._pNWidth;
		data._pAnimWidth2 = (data._pNWidth - 64) >> 1;
	} else {
		data._pgfxnum = g;
	}

	for (i = 0; i < nummissiles; i++) {
		mi = missileactive[i];
		if (missile[mi]._mitype == MIS_MANASHIELD && missile[mi]._misource == p) {
			missile[mi]._miVar1 = data._pHitPoints;
			missile[mi]._miVar2 = data._pHPBase;
		}
	}

	drawmanaflag = TRUE;
	drawhpflag = TRUE;
}

void Player::CalcPlrScrolls()
{
	int i, j;

	data._pScrlSpells = 0;
	for (i = 0; i < data._pNumInv; i++) {
		if (data.InvList[i]._itype != ITYPE_NONE && (data.InvList[i]._iMiscId == IMISC_SCROLL || data.InvList[i]._iMiscId == IMISC_SCROLLT)) {
			if (data.InvList[i]._iStatFlag)
				data._pScrlSpells |= (__int64)1 << (data.InvList[i]._iSpell - 1);
		}
	}

	for (j = 0; j < MAXBELTITEMS; j++) {
		if (data.SpdList[j]._itype != ITYPE_NONE && (data.SpdList[j]._iMiscId == IMISC_SCROLL || data.SpdList[j]._iMiscId == IMISC_SCROLLT)) {
			if (data.SpdList[j]._iStatFlag)
				data._pScrlSpells |= (__int64)1 << (data.SpdList[j]._iSpell - 1);
		}
	}
	if (data._pRSplType == RSPLTYPE_SCROLL) {
		if (!(data._pScrlSpells & 1 << (data._pRSpell - 1))) {
			data._pRSpell = SPL_INVALID;
			data._pRSplType = RSPLTYPE_INVALID;
			force_redraw = 255;
		}
	}

	//	if (owner.data._pRSplType == RSPLTYPE_SCROLL) {
	//	if (owner.data._pRSpell != SPL_INVALID) {
	//		// BUGFIX: Cast the literal `1` to `unsigned __int64` to make that
	//		// bitshift 64bit this causes the last 4 skills to not reset
	//		// correctly after use
	//		if (!(owner.data._pScrlSpells & (1 << (owner.data._pRSpell - 1)))) {
	//			owner.data._pRSpell = SPL_INVALID;
	//		}

	//		force_redraw = 255;
	//	}
	//}

}

void Player::CalcPlrStaff()
{
	data._pISpells = 0;
	if (data.InvBody[INVLOC_HAND_LEFT]._itype != ITYPE_NONE
	    && data.InvBody[INVLOC_HAND_LEFT]._iStatFlag
	    && data.InvBody[INVLOC_HAND_LEFT]._iCharges > 0) {
		data._pISpells |= (__int64)1 << (data.InvBody[INVLOC_HAND_LEFT]._iSpell - 1);
	}

		//// Update staff spells etc.
	// if (owner.data.InvBody[INVLOC_HAND_LEFT]->_itype == ITYPE_STAFF &&
	// owner.data.InvBody[INVLOC_HAND_LEFT]->_iSpell != 0 &&
	// owner.data.InvBody[INVLOC_HAND_LEFT]->_iCharges > 0) { 	owner.data._pRSpell
	//= owner.data.InvBody[INVLOC_HAND_LEFT]->_iSpell; 	owner.data._pRSplType =
	//RSPLTYPE_CHARGES; 	force_redraw = 255;
	//}
}

void Player::CalcSelfItems()
{
	int i;
	PlayerStruct *p;
	bool sf, changeflag;
	int sa, ma, da;

	p = &plr[pnum].data;

	sa = 0;
	ma = 0;
	da = 0;
	for (i = 0; i < NUM_INVLOC; i++) {
		if (p->InvBody[i]._itype != ITYPE_NONE) {
			p->InvBody[i]._iStatFlag = TRUE;
			if (p->InvBody[i]._iIdentified) {
				sa += p->InvBody[i]._iPLStr;
				ma += p->InvBody[i]._iPLMag;
				da += p->InvBody[i]._iPLDex;
			}
		}
	}
	do {
		changeflag = false;
		for (i = 0; i < NUM_INVLOC; i++) {
			if (p->InvBody[i]._itype != ITYPE_NONE && p->InvBody[i]._iStatFlag) {
				sf = TRUE;
				if (sa + p->_pBaseStr < p->InvBody[i]._iMinStr)
					sf = false;
				if (ma + p->_pBaseMag < p->InvBody[i]._iMinMag)
					sf = false;
				if (da + p->_pBaseDex < p->InvBody[i]._iMinDex)
					sf = false;
				if (!sf) {
					changeflag = TRUE;
					p->InvBody[i]._iStatFlag = false;
					if (p->InvBody[i]._iIdentified) {
						sa -= p->InvBody[i]._iPLStr;
						ma -= p->InvBody[i]._iPLMag;
						da -= p->InvBody[i]._iPLDex;
					}
				}
			}
		}
	} while (changeflag);
}

void Player::CalcPlrItemMin()
{
	PlayerStruct *p;
	ItemStruct *pi;
	int i;

	p = &plr[pnum].data;
	pi = p->InvList;
	i = p->_pNumInv;

	while (i--) {
		pi->_iStatFlag = ItemMinStats(p, pi);
		pi++;
	}

	pi = p->SpdList;
	for (i = MAXBELTITEMS; i != 0; i--) {
		if (pi->_itype != ITYPE_NONE) {
			pi->_iStatFlag = ItemMinStats(p, pi);
		}
		pi++;
	}
}


void Player::CalcPlrBookVals()
{
	int i, slvl;

	if (lvl.currlevel == 0) {
		for (i = 1; witchitem[i]._itype != ITYPE_NONE; i++) {
			WitchBookLevel(i);
			witchitem[i]._iStatFlag = StoreStatOk(&witchitem[i]);
		}
	}

	for (i = 0; i < data._pNumInv; i++) {
		if (data.InvList[i]._itype == ITYPE_MISC && data.InvList[i]._iMiscId == IMISC_BOOK) {
			data.InvList[i]._iMinMag = spelldata[data.InvList[i]._iSpell].sMinInt;
			slvl = data._pSplLvl[data.InvList[i]._iSpell];

			while (slvl != 0) {
				data.InvList[i]._iMinMag += 20 * data.InvList[i]._iMinMag / 100;
				slvl--;
				if (data.InvList[i]._iMinMag + 20 * data.InvList[i]._iMinMag / 100 > 255) {
					data.InvList[i]._iMinMag = 255;
					slvl = 0;
				}
			}
			data.InvList[i]._iStatFlag = ItemMinStats(&data, &data.InvList[i]);
		}
	}
}

void Player::CalcPlrInv(bool Loadgfx)
{
	CalcPlrItemMin();
	CalcSelfItems();
	CalcPlrItemVals(Loadgfx);
	CalcPlrItemMin();
	if (pnum == myplr()) {
		CalcPlrBookVals();
		CalcPlrScrolls();
		CalcPlrStaff();
		if (lvl.currlevel == 0)
			RecalcStoreStats();
	}
}


void Player::CreatePlrItems()
{
	int i;
	ItemStruct *pi = data.InvBody;

	for (i = NUM_INVLOC; i != 0; i--) {
		pi->_itype = ITYPE_NONE;
		pi++;
	}

	// converting this to a for loop creates a `rep stosd` instruction,
	// so this probably actually was a memset
	memset(&data.InvGrid, 0, sizeof(data.InvGrid));

	pi = data.InvList;
	for (i = NUM_INV_GRID_ELEM; i != 0; i--) {
		pi->_itype = ITYPE_NONE;
		pi++;
	}

	data._pNumInv = 0;

	pi = &data.SpdList[0];
	for (i = MAXBELTITEMS; i != 0; i--) {
		pi->_itype = ITYPE_NONE;
		pi++;
	}

	switch (data._pClass) {
	case PC_WARRIOR:
		SetPlrHandItem(&data.InvBody[INVLOC_HAND_LEFT], IDI_WARRIOR);
		GetPlrHandSeed(&data.InvBody[INVLOC_HAND_LEFT]);

		SetPlrHandItem(&data.InvBody[INVLOC_HAND_RIGHT], IDI_WARRSHLD);
		GetPlrHandSeed(&data.InvBody[INVLOC_HAND_RIGHT]);

#ifdef _DEBUG
		if (!debug_mode_key_w) {
#endif
			SetPlrHandItem(&data.HoldItem, IDI_WARRCLUB);
			GetPlrHandSeed(&data.HoldItem);
			inventory.AutoPlace(0, { 1, 3 }, TRUE);
#ifdef _DEBUG
		}
#endif

		SetPlrHandItem(&data.SpdList[0], IDI_HEAL);
		GetPlrHandSeed(&data.SpdList[0]);

		SetPlrHandItem(&data.SpdList[1], IDI_HEAL);
		GetPlrHandSeed(&data.SpdList[1]);
		break;
	case PC_ROGUE:
		SetPlrHandItem(&data.InvBody[INVLOC_HAND_LEFT], IDI_ROGUE);
		GetPlrHandSeed(&data.InvBody[INVLOC_HAND_LEFT]);

		SetPlrHandItem(&data.SpdList[0], IDI_HEAL);
		GetPlrHandSeed(&data.SpdList[0]);

		SetPlrHandItem(&data.SpdList[1], IDI_HEAL);
		GetPlrHandSeed(&data.SpdList[1]);
		break;
	case PC_SORCERER:
		SetPlrHandItem(&data.InvBody[INVLOC_HAND_LEFT], IDI_SORCEROR);
		GetPlrHandSeed(&data.InvBody[INVLOC_HAND_LEFT]);

		SetPlrHandItem(&data.SpdList[0], IDI_MANA);
		GetPlrHandSeed(&data.SpdList[0]);

		SetPlrHandItem(&data.SpdList[1], IDI_MANA);
		GetPlrHandSeed(&data.SpdList[1]);
		break;
	}

	SetPlrHandItem(&data.HoldItem, IDI_GOLD);
	GetPlrHandSeed(&data.HoldItem);

#ifdef _DEBUG
	if (!debug_mode_key_w) {
#endif
		data.HoldItem._ivalue = 100;
		data.HoldItem._iCurs = ICURS_GOLD_SMALL;
		data._pGold = data.HoldItem._ivalue;
		data.InvList[data._pNumInv++] = data.HoldItem;
		data.InvGrid[30] = data._pNumInv;
#ifdef _DEBUG
	} else {
		data.HoldItem._ivalue = GOLD_MAX_LIMIT;
		data.HoldItem._iCurs = ICURS_GOLD_LARGE;
		data._pGold = data.HoldItem._ivalue * 40;
		for (i = 0; i < NUM_INV_GRID_ELEM; i++) {
			GetPlrHandSeed(&data.HoldItem);
			data.InvList[data._pNumInv++] = data.HoldItem;
			data.InvGrid[i] = data._pNumInv;
		}
	}
#endif

	CalcPlrItemVals(p, false);
}

void Player::UseItem(int Mid, int spl)
{
	int l, j;

	switch (Mid) {
	case IMISC_HEAL:
	case IMISC_MEAT:
		j = data._pMaxHP >> 8;
		l = ((j >> 1) + random_(39, j)) << 6;
		if (data._pClass == PC_WARRIOR)
			l *= 2;
		if (data._pClass == PC_ROGUE)
			l += l >> 1;
		data._pHitPoints += l;
		if (data._pHitPoints > data._pMaxHP)
			data._pHitPoints = data._pMaxHP;
		data._pHPBase += l;
		if (data._pHPBase > data._pMaxHPBase)
			data._pHPBase = data._pMaxHPBase;
		drawhpflag = TRUE;
		break;
	case IMISC_FULLHEAL:
		data._pHitPoints = data._pMaxHP;
		data._pHPBase = data._pMaxHPBase;
		drawhpflag = TRUE;
		break;
	case IMISC_MANA:
		j = data._pMaxMana >> 8;
		l = ((j >> 1) + random_(40, j)) << 6;
		if (data._pClass == PC_SORCERER)
			l *= 2;
		if (data._pClass == PC_ROGUE)
			l += l >> 1;
		if (!(data._pIFlags & ISPL_NOMANA)) {
			data._pMana += l;
			if (data._pMana > data._pMaxMana)
				data._pMana = data._pMaxMana;
			data._pManaBase += l;
			if (data._pManaBase > data._pMaxManaBase)
				data._pManaBase = data._pMaxManaBase;
			drawmanaflag = TRUE;
		}
		break;
	case IMISC_FULLMANA:
		if (!(data._pIFlags & ISPL_NOMANA)) {
			data._pMana = data._pMaxMana;
			data._pManaBase = data._pMaxManaBase;
			drawmanaflag = TRUE;
		}
		break;
	case IMISC_ELIXSTR:
		ModifyPlrStr(1);
		break;
	case IMISC_ELIXMAG:
		ModifyPlrMag(1);
		break;
	case IMISC_ELIXDEX:
		ModifyPlrDex(1);
		break;
	case IMISC_ELIXVIT:
		ModifyPlrVit(1);
		break;
	case IMISC_REJUV:
		j = data._pMaxHP >> 8;
		l = ((j >> 1) + random_(39, j)) << 6;
		if (data._pClass == PC_WARRIOR)
			l *= 2;
		if (data._pClass == PC_ROGUE)
			l += l >> 1;
		data._pHitPoints += l;
		if (data._pHitPoints > data._pMaxHP)
			data._pHitPoints = data._pMaxHP;
		data._pHPBase += l;
		if (data._pHPBase > data._pMaxHPBase)
			data._pHPBase = data._pMaxHPBase;
		drawhpflag = TRUE;
		j = data._pMaxMana >> 8;
		l = ((j >> 1) + random_(40, j)) << 6;
		if (data._pClass == PC_SORCERER)
			l *= 2;
		if (data._pClass == PC_ROGUE)
			l += l >> 1;
		if (!(data._pIFlags & ISPL_NOMANA)) {
			data._pMana += l;
			if (data._pMana > data._pMaxMana)
				data._pMana = data._pMaxMana;
			data._pManaBase += l;
			if (data._pManaBase > data._pMaxManaBase)
				data._pManaBase = data._pMaxManaBase;
			drawmanaflag = TRUE;
		}
		break;
	case IMISC_FULLREJUV:
		data._pHitPoints = data._pMaxHP;
		data._pHPBase = data._pMaxHPBase;
		drawhpflag = TRUE;
		if (!(data._pIFlags & ISPL_NOMANA)) {
			data._pMana = data._pMaxMana;
			data._pManaBase = data._pMaxManaBase;
			drawmanaflag = TRUE;
		}
		break;
	case IMISC_SCROLL:
		if (spelldata[spl].sTargeted) {
			data._pTSpell = spl;
			data._pTSplType = RSPLTYPE_INVALID;
			if (pnum == myplr())
				NewCursor(CURSOR_TELEPORT);
		} else {
			ClrPlrPath();
			data._pSpell = spl;
			data._pSplType = RSPLTYPE_INVALID;
			data._pSplFrom = 3;
			data.destAction = ACTION_SPELL;
			data.destParam1 = cursm.x;
			data.destParam2 = cursm.y;
			if (pnum == myplr() && spl == SPL_NOVA)
				NetSendCmdLoc(TRUE, CMD_NOVA, cursm);
		}
		break;
	case IMISC_SCROLLT:
		if (spelldata[spl].sTargeted) {
			data._pTSpell = spl;
			data._pTSplType = RSPLTYPE_INVALID;
			if (pnum == myplr())
				NewCursor(CURSOR_TELEPORT);
		} else {
			ClrPlrPath();
			data._pSpell = spl;
			data._pSplType = RSPLTYPE_INVALID;
			data._pSplFrom = 3;
			data.destAction = ACTION_SPELL;
			data.destParam1 = cursm.x;
			data.destParam2 = cursm.y;
		}
		break;
	case IMISC_BOOK:
		data._pMemSpells |= (__int64)1 << (spl - 1);
		if (data._pSplLvl[spl] < 15)
			data._pSplLvl[spl]++;
		data._pMana += spelldata[spl].sManaCost << 6;
		if (data._pMana > data._pMaxMana)
			data._pMana = data._pMaxMana;
		data._pManaBase += spelldata[spl].sManaCost << 6;
		if (data._pManaBase > data._pMaxManaBase)
			data._pManaBase = data._pMaxManaBase;
		if (pnum == myplr())
			CalcPlrBookVals();
		drawmanaflag = TRUE;
		break;
	case IMISC_MAPOFDOOM:
		doom_init();
		break;
	case IMISC_SPECELIX:
		ModifyPlrStr(3);
		ModifyPlrMag(3);
		ModifyPlrDex(3);
		ModifyPlrVit(3);
		break;
	}
}

bool Player::DurReduce(BodyLoc loc, int itmclass, int itmtype)
{
	Item *item = inv.getBodyItem(loc);
	if (!item) return false;
	if (itmclass != ICLASS_NONE && item->_iClass != itmclass) return false;
	if (itmtype != ITYPE_NONE && item->_itype != itmtype) return false;
	if (item->_iDurability == DUR_INDESTRUCTIBLE) return false;

	item->_iDurability--;
	if (item->_iDurability == 0) {
		NetSendCmdDelItem(true, loc);
		inv.getBodySlot(loc).destroyItem();
		CalcPlrInv(true);
	}
	return true;
}

bool Player::WeaponDur(int durrnd)
{
	if (pnum != myplr()) return false;
	if (random_(3, durrnd) != 0) return false;
	if (DurReduce(BodyLoc::HandLeft, ICLASS_WEAPON, ITYPE_NONE)) return true;
	if (DurReduce(BodyLoc::HandRight, ICLASS_WEAPON, ITYPE_NONE)) return true;
	if (DurReduce(BodyLoc::HandLeft, ICLASS_NONE, ITYPE_SHIELD)) return true;
	if (DurReduce(BodyLoc::HandLeft, ICLASS_NONE, ITYPE_SHIELD)) return true;
	return false;
}

void Player::ShieldDur()
{
	if (pnum != myplr()) return;
	if (DurReduce(BodyLoc::HandLeft, ICLASS_NONE, ITYPE_SHIELD)) return;
	if (DurReduce(BodyLoc::HandLeft, ICLASS_NONE, ITYPE_SHIELD)) return;
}

void Player::ArmorDur()
{
	if (pnum != myplr()) return;
	if (DurReduce(BodyLoc::Head, ICLASS_NONE, ITYPE_NONE)) return;
	if (DurReduce(BodyLoc::Chest, ICLASS_NONE, ITYPE_NONE)) return;
}


DEVILUTION_END_NAMESPACE
