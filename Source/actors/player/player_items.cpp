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

	for (InvSlot &i : inv.getBagArray()) {
		Item *itm = i.item();
		if (!itm) continue;
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

	const Item * const left = inv.getBodySlot(BodyLoc::HandLeft).item();
	const Item * const right = inv.getBodySlot(BodyLoc::HandRight).item();
	if (mind == 0 && maxd == 0) {
		mind = 1;
		maxd = 1;

		if (left && left->_itype == ITYPE_SHIELD && left->_iStatFlag) {
			maxd = 3;
		}

		if (right && right->_itype == ITYPE_SHIELD && right->_iStatFlag) {
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

	if (data._pLightRad != lrad && pnum == myplr()) {
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

	if (pnum == myplr() && (data._pHitPoints >> 6) <= 0) {
		SetPlayerHitPoints(0);
	}

	data._pMana = imana + data._pManaBase;
	data._pMaxMana = imana + data._pMaxManaBase;

	data._pIFMinDam = fmin;
	data._pIFMaxDam = fmax;
	data._pILMinDam = lmin;
	data._pILMaxDam = lmax;

	if (iflgs & ISPL_INFRAVISION) {
		data._pInfraFlag = true;
	} else {
		data._pInfraFlag = false;
	}

	data._pBlockFlag = false;
	data._pwtype = WT_MELEE;

	g = 0;

	if (left && left->_itype != ITYPE_NONE && left->_iClass == ICLASS_WEAPON &&
	    left->_iStatFlag) {
		g = left->_itype;
	}

	if (right && right->_itype != ITYPE_NONE &&
	    right->_iClass == ICLASS_WEAPON && right->_iStatFlag) {
		g = right->_itype;
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

	if (left && left->_itype == ITYPE_SHIELD && left->_iStatFlag) {
		data._pBlockFlag = true;
		g++;
	}
	if (right && right->_itype == ITYPE_SHIELD && right->_iStatFlag) {
		data._pBlockFlag = true;
		g++;
	}

	const Item * chest = inv.getBodySlot(BodyLoc::Chest).item();

	if (chest && chest->_itype == ITYPE_MARMOR && chest->_iStatFlag) {
		g += ANIM_ID_MEDIUM_ARMOR;
	}
	if (chest && chest->_itype == ITYPE_HARMOR && chest->_iStatFlag) {
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
		if (missile[mi]._mitype == MIS_MANASHIELD && missile[mi]._misource == pnum) {
			missile[mi]._miVar1 = data._pHitPoints;
			missile[mi]._miVar2 = data._pHPBase;
		}
	}

	drawmanaflag = true;
	drawhpflag = true;
}

void Player::CalcPlrScrolls()
{
	data._pScrlSpells = 0;
	for (InvSlot &i : inv.getBagArray()) {
		Item *item = i.item();
		if (item && item->_itype != ITYPE_NONE &&
		    (item->_iMiscId == IMISC_SCROLL ||
		     item->_iMiscId == IMISC_SCROLLT)) {
			if (item->_iStatFlag)
				data._pScrlSpells |= (__int64)1 << (item->_iSpell - 1);
		}
	}

	for (int j = 0; j < MAXBELTITEMS; j++) {
		Item *item = inv.getBeltSlot(j).item();
		if (item && item->_itype != ITYPE_NONE &&
		    (item->_iMiscId == IMISC_SCROLL ||
		     item->_iMiscId == IMISC_SCROLLT)) {
			if (item->_iStatFlag)
				data._pScrlSpells |= (__int64)1 << (item->_iSpell - 1);
		}
	}
	if (data._pRSplType == RSPLTYPE_SCROLL) {
		if (!(data._pScrlSpells & 1 << (data._pRSpell - 1))) {
			data._pRSpell = SPL_INVALID;
			data._pRSplType = RSPLTYPE_INVALID;
			force_redraw = 255;
		}
	}
}

void Player::CalcPlrStaff()
{
	data._pISpells = 0;
	const Item *const left = inv.getBodySlot(BodyLoc::HandLeft).item();
	if (!left) return;

	if (left->_itype != ITYPE_NONE && left->_iStatFlag && left->_iCharges > 0) {
		data._pISpells |= (__int64)1 << (left->_iSpell - 1);
	}

	if (left->_itype == ITYPE_STAFF && left->_iSpell != 0 && left->_iCharges > 0) {
		data._pRSpell = left->_iSpell;
		data._pRSplType = RSPLTYPE_CHARGES;
		force_redraw = 255;
	}
}

void Player::CalcSelfItems()
{
	PlayerStruct *p = &plr[pnum].data;
	int sa = 0;
	int ma = 0;
	int da = 0;
	for (InvSlot &i : inv.getBagArray()) {
		Item *item = i.item();
		if (!item) continue;
		if (item->_itype != ITYPE_NONE) {
			item->_iStatFlag = true;
			if (item->_iIdentified) {
				sa += item->_iPLStr;
				ma += item->_iPLMag;
				da += item->_iPLDex;
			}
		}
	}
	bool changeflag;
	do {
		changeflag = false;
		for (InvSlot &i : inv.getBagArray()) {
			Item *item = i.item();
			if (!item) continue;
			if (item->_itype != ITYPE_NONE && item->_iStatFlag) {
				int sf = true;
				if (sa + p->_pBaseStr < item->_iMinStr)
					sf = false;
				if (ma + p->_pBaseMag < item->_iMinMag)
					sf = false;
				if (da + p->_pBaseDex < item->_iMinDex)
					sf = false;
				if (!sf) {
					changeflag = true;
					item->_iStatFlag = false;
					if (item->_iIdentified) {
						sa -= item->_iPLStr;
						ma -= item->_iPLMag;
						da -= item->_iPLDex;
					}
				}
			}
		}
	} while (changeflag);
}

void Player::CalcPlrItemMin()
{
	PlayerStruct *p = &plr[pnum].data;

	for (auto &i : inv.getBagArray()) {
		Item *item = i.item();
		if (!item) continue;
		item->_iStatFlag = ItemMinStats(p, item);
	}

	for (auto &i : inv.getBeltArray()) {
		Item *item = i.item();
		if (item && item->_itype != ITYPE_NONE) {
			item->_iStatFlag = ItemMinStats(p, item);
		}
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

	for (auto &i : inv.getBagArray()) {
		Item *item = i.item();
		if (!item) continue;

		if (item->_itype == ITYPE_MISC && item->_iMiscId == IMISC_BOOK) {
			item->_iMinMag = spelldata[item->_iSpell].sMinInt;
			slvl = data._pSplLvl[item->_iSpell];

			while (slvl != 0) {
				item->_iMinMag += 20 * item->_iMinMag / 100;
				slvl--;
				if (item->_iMinMag + 20 * item->_iMinMag / 100 > 255) {
					item->_iMinMag = 255;
					slvl = 0;
				}
			}
			item->_iStatFlag = ItemMinStats(&data, item);
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
	for (InvSlot &i : inv.getBodyArray()) {
		if (i.item()) i.destroyItem();
	}

	for (InvSlot &i : inv.getBagArray()) {
		if (i.item()) i.destroyItem();
	}

	for (InvSlot &i : inv.getBeltArray()) {
		if (i.item()) i.destroyItem();
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
			inventory.AutoPlace(0, { 1, 3 }, true);
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
		for (i = 0; i < MAXINVITEMS; i++) {
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
		drawhpflag = true;
		break;
	case IMISC_FULLHEAL:
		data._pHitPoints = data._pMaxHP;
		data._pHPBase = data._pMaxHPBase;
		drawhpflag = true;
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
			drawmanaflag = true;
		}
		break;
	case IMISC_FULLMANA:
		if (!(data._pIFlags & ISPL_NOMANA)) {
			data._pMana = data._pMaxMana;
			data._pManaBase = data._pMaxManaBase;
			drawmanaflag = true;
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
		drawhpflag = true;
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
			drawmanaflag = true;
		}
		break;
	case IMISC_FULLREJUV:
		data._pHitPoints = data._pMaxHP;
		data._pHPBase = data._pMaxHPBase;
		drawhpflag = true;
		if (!(data._pIFlags & ISPL_NOMANA)) {
			data._pMana = data._pMaxMana;
			data._pManaBase = data._pMaxManaBase;
			drawmanaflag = true;
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
				NetSendCmdLoc(true, CMD_NOVA, cursm);
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
		drawmanaflag = true;
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
	Item *item = inv.getBodySlot(loc).item();
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






void Player::CreateRndUseful(V2Di pos, bool sendmsg)
{
	Item &item = items.createNewItem();
	item.GetSuperItemSpace(pos);
	item.SetupAllUseful(GetRndSeed(), lvl.currlevel);
	if (sendmsg) { NetSendCmdDItem(false, item); }
}



void Player::CheckIdentify(int cii)
{
	ItemStruct *pi;

	if (cii >= MAXINVITEMS)
		pi = &plr[pnum].data.InvList[cii - MAXINVITEMS];
	else
		pi = &plr[pnum].data.InvBody[cii];

	pi->_iIdentified = true;
	CalcPlrInv(pnum, true);

	if (pnum == myplr()) SetCursor_(CURSOR_HAND);
}

void Player::DoRepair(int cii)
{
	ItemStruct *pi;
	Player &p = plr[pnum];
	PlaySfxLoc(IS_REPAIR, p.pos());

	if (cii >= MAXINVITEMS) {
		pi = &p.data.InvList[cii - MAXINVITEMS];
	} else {
		pi = &p.data.InvBody[cii];
	}

	RepairItem(pi, p.data._pLevel);
	CalcPlrInv(pnum, true);

	if (pnum == myplr()) SetCursor_(CURSOR_HAND);
}

void Player::DoRecharge(int cii)
{
	PlayerStruct *p;
	ItemStruct *pi;
	int r;

	p = &plr[pnum].data;
	if (cii >= MAXINVITEMS) {
		pi = &p->InvList[cii - MAXINVITEMS];
	} else {
		pi = &p->InvBody[cii];
	}
	if (pi->_itype == ITYPE_STAFF && pi->_iSpell) {
		r = spelldata[pi->_iSpell].sBookLvl;
		r = random_(38, p->_pLevel / r) + 1;
		RechargeItem(pi, r);
		CalcPlrInv(pnum, true);
	}

	if (pnum == myplr()) SetCursor_(CURSOR_HAND);
}

DEVILUTION_END_NAMESPACE
