/**
 * @file items.cpp
 *
 * Implementation of item functionality.
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

int itemactive[MAXITEMS];
bool uitemflag;
int itemavail[MAXITEMS];
ItemStruct curruitem;
ItemGetRecordStruct itemrecord[MAXITEMS];
ItemStruct item[MAXITEMS + 1];
bool itemhold[3][3];
BYTE *itemanims[ITEMTYPES];
bool UniqueItemFlag[128];
int numitems;
int gnNumGetRecords;

/* data */

BYTE ItemCAnimTbl[] = {
	20, 16, 16, 16, 4, 4, 4, 12, 12, 12,
	12, 12, 12, 12, 12, 21, 21, 25, 12, 28,
	28, 28, 0, 0, 0, 32, 0, 0, 0, 24,
	24, 26, 2, 25, 22, 23, 24, 25, 27, 27,
	29, 0, 0, 0, 12, 12, 12, 12, 12, 0,
	8, 8, 0, 8, 8, 8, 8, 8, 8, 6,
	8, 8, 8, 6, 8, 8, 6, 8, 8, 6,
	6, 6, 8, 8, 8, 5, 9, 13, 13, 13,
	5, 5, 5, 15, 5, 5, 18, 18, 18, 30,
	5, 5, 14, 5, 14, 13, 16, 18, 5, 5,
	7, 1, 3, 17, 1, 15, 10, 14, 3, 11,
	8, 0, 1, 7, 0, 7, 15, 7, 3, 3,
	3, 6, 6, 11, 11, 11, 31, 14, 14, 14,
	6, 6, 7, 3, 8, 14, 0, 14, 14, 0,
	33, 1, 1, 1, 1, 1, 7, 7, 7, 14,
	14, 17, 17, 17, 0, 34, 1, 0, 3, 17,
	8, 8, 6, 1, 3, 3, 11, 3, 4
};
char *ItemDropNames[] = {
	"Armor2",
	"Axe",
	"FBttle",
	"Bow",
	"GoldFlip",
	"Helmut",
	"Mace",
	"Shield",
	"SwrdFlip",
	"Rock",
	"Cleaver",
	"Staff",
	"Ring",
	"CrownF",
	"LArmor",
	"WShield",
	"Scroll",
	"FPlateAr",
	"FBook",
	"Food",
	"FBttleBB",
	"FBttleDY",
	"FBttleOR",
	"FBttleBR",
	"FBttleBL",
	"FBttleBY",
	"FBttleWH",
	"FBttleDB",
	"FEar",
	"FBrain",
	"FMush",
	"Innsign",
	"Bldstn",
	"Fanvil",
	"FLazStaf",
};
BYTE ItemAnimLs[] = {
	15,
	13,
	16,
	13,
	10,
	13,
	13,
	13,
	13,
	10,
	13,
	13,
	13,
	13,
	13,
	13,
	13,
	13,
	13,
	1,
	16,
	16,
	16,
	16,
	16,
	16,
	16,
	16,
	13,
	12,
	12,
	13,
	13,
	13,
	8,
};
int ItemDropSnds[] = {
	IS_FHARM,
	IS_FAXE,
	IS_FPOT,
	IS_FBOW,
	IS_GOLD,
	IS_FCAP,
	IS_FSWOR,
	IS_FSHLD,
	IS_FSWOR,
	IS_FROCK,
	IS_FAXE,
	IS_FSTAF,
	IS_FRING,
	IS_FCAP,
	IS_FLARM,
	IS_FSHLD,
	IS_FSCRL,
	IS_FHARM,
	IS_FBOOK,
	IS_FLARM,
	IS_FPOT,
	IS_FPOT,
	IS_FPOT,
	IS_FPOT,
	IS_FPOT,
	IS_FPOT,
	IS_FPOT,
	IS_FPOT,
	IS_FBODY,
	IS_FBODY,
	IS_FMUSH,
	IS_ISIGN,
	IS_FBLST,
	IS_FANVL,
	IS_FSTAF,
};
int ItemInvSnds[] = {
	IS_IHARM,
	IS_IAXE,
	IS_IPOT,
	IS_IBOW,
	IS_GOLD,
	IS_ICAP,
	IS_ISWORD,
	IS_ISHIEL,
	IS_ISWORD,
	IS_IROCK,
	IS_IAXE,
	IS_ISTAF,
	IS_IRING,
	IS_ICAP,
	IS_ILARM,
	IS_ISHIEL,
	IS_ISCROL,
	IS_IHARM,
	IS_IBOOK,
	IS_IHARM,
	IS_IPOT,
	IS_IPOT,
	IS_IPOT,
	IS_IPOT,
	IS_IPOT,
	IS_IPOT,
	IS_IPOT,
	IS_IPOT,
	IS_IBODY,
	IS_IBODY,
	IS_IMUSH,
	IS_ISIGN,
	IS_IBLST,
	IS_IANVL,
	IS_ISTAF,
};
int idoppely = 16;
int premiumlvladd[6] = { -1, -1, 0, 0, 1, 2 };

Item::Item(Player *new_owner)
    : _owner(new_owner)
{
}

Player& Item::owner() {
	assert(_owner);
	return *_owner;
}

void InitItemGFX()
{
	int i;
	char arglist[64];

	for (i = 0; i < ITEMTYPES; i++) {
		sprintf(arglist, "Items\\%s.CEL", ItemDropNames[i]);
		itemanims[i] = LoadFileInMem(arglist, NULL);
	}
	memset(UniqueItemFlag, 0, sizeof(UniqueItemFlag));
}

bool ItemPlace(V2Di pos)
{
	if (grid.at(pos).isMonster()) return FALSE;
	if (grid.at(pos).isPlayer()) return FALSE;
	if (grid.at(pos).isItem()) return FALSE;
	if (grid.at(pos).isObject()) return FALSE;
	if (grid.at(pos).dFlags & BFLAG_POPULATED) return FALSE;
	if (grid.at(pos).isSolid()) return FALSE;
	return TRUE;
}

void AddInitItems()
{
	V2Di pos;
	int i, j, rnd;

	rnd = random_(11, 3) + 3;
	for (j = 0; j < rnd; j++) {
		i = itemavail[0];
		itemavail[0] = itemavail[MAXITEMS - numitems - 1];
		itemactive[numitems] = i;
		pos.x = random_(12, 80) + 16;
		pos.y = random_(12, 80) + 16;
		while (!ItemPlace(pos)) {
			pos.x = random_(12, 80) + 16;
			pos.y = random_(12, 80) + 16;
		}
		item[i]._i = pos;
		grid.at(pos).setItem(i);
		item[i]._iSeed = GetRndSeed();
		SetRndSeed(item[i]._iSeed);
		if (random_(12, 2))
			GetItemAttrs(i, IDI_HEAL, level.currlevel);
		else
			GetItemAttrs(i, IDI_MANA, level.currlevel);
		item[i]._iCreateInfo = level.currlevel - 0x8000;
		SetupItem(i);
		item[i]._iAnimFrame = item[i]._iAnimLen;
		item[i]._iAnimFlag = FALSE;
		item[i]._iSelFlag = 1;
		DeltaAddItem(i);
		numitems++;
	}
}

void InitItems()
{
	int i;
	long s;

	GetItemAttrs(0, IDI_GOLD, 1);
	golditem = item[0];
	golditem._iStatFlag = TRUE;
	numitems = 0;

	for (i = 0; i < MAXITEMS; i++) {
		item[i]._itype = ITYPE_MISC;
		item[i]._i = { 0, 0 };
		item[i]._iAnimFlag = FALSE;
		item[i]._iSelFlag = 0;
		item[i]._iIdentified = FALSE;
		item[i]._iPostDraw = FALSE;
	}

	for (i = 0; i < MAXITEMS; i++) {
		itemavail[i] = i;
		itemactive[i] = 0;
	}

	if (!level.setlevel) {
		s = GetRndSeed(); /* unused */
		if (QuestStatus(Q_ROCK))
			SpawnRock();
		if (QuestStatus(Q_ANVIL))
			SpawnQuestItem(IDI_ANVIL, { 2 * setpc.x + 27, 2 * setpc.y + 27 }, 0, 1);
		if (level.currlevel > 0 && level.currlevel < 16)
			AddInitItems();
	}

	uitemflag = FALSE;
}

void CalcPlrItemVals(int p, bool Loadgfx)
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
		ItemStruct *itm = &plr[p].data.InvBody[i];
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

		if (plr[p].data.InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_SHIELD && plr[p].data.InvBody[INVLOC_HAND_LEFT]._iStatFlag) {
			maxd = 3;
		}

		if (plr[p].data.InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_SHIELD && plr[p].data.InvBody[INVLOC_HAND_RIGHT]._iStatFlag) {
			maxd = 3;
		}
	}

	plr[p].data._pIMinDam = mind;
	plr[p].data._pIMaxDam = maxd;
	plr[p].data._pIAC = tac;
	plr[p].data._pIBonusDam = bdam;
	plr[p].data._pIBonusToHit = btohit;
	plr[p].data._pIBonusAC = bac;
	plr[p].data._pIFlags = iflgs;
	plr[p].data._pIBonusDamMod = dmod;
	plr[p].data._pIGetHit = ghit;

	if (lrad < 2) {
		lrad = 2;
	}
	if (lrad > 15) {
		lrad = 15;
	}

	if (plr[p].data._pLightRad != lrad && p == myplr()) {
		ChangeLightRadius(plr[p].data._plid, lrad);

		pvid = plr[p].data._pvid;
		if (lrad >= 10) {
			ChangeVisionRadius(pvid, lrad);
		} else {
			ChangeVisionRadius(pvid, 10);
		}

		plr[p].data._pLightRad = lrad;
	}

	plr[p].data._pStrength = sadd + plr[p].data._pBaseStr;
	if (myplr().data._pStrength <= 0) {
		myplr().data._pStrength = 0;
	}

	plr[p].data._pMagic = madd + plr[p].data._pBaseMag;
	if (myplr().data._pMagic <= 0) {
		myplr().data._pMagic = 0;
	}

	plr[p].data._pDexterity = dadd + plr[p].data._pBaseDex;
	if (myplr().data._pDexterity <= 0) {
		myplr().data._pDexterity = 0;
	}

	plr[p].data._pVitality = vadd + plr[p].data._pBaseVit;
	if (myplr().data._pVitality <= 0) {
		myplr().data._pVitality = 0;
	}

	if (plr[p].data._pClass == PC_ROGUE) {
		plr[p].data._pDamageMod = plr[p].data._pLevel * (plr[p].data._pStrength + plr[p].data._pDexterity) / 200;
	} else {
		plr[p].data._pDamageMod = plr[p].data._pLevel * plr[p].data._pStrength / 100;
	}

	plr[p].data._pISpells = spl;

	// check if the current RSplType is a valid/allowed spell
	if (plr[p].data._pRSplType == RSPLTYPE_CHARGES
	    && !(spl & ((unsigned __int64)1 << (plr[p].data._pRSpell - 1)))) {
		plr[p].data._pRSpell = SPL_INVALID;
		plr[p].data._pRSplType = RSPLTYPE_INVALID;
		force_redraw = 255;
	}

	plr[p].data._pISplLvlAdd = spllvladd;
	plr[p].data._pIEnAc = enac;

	if (iflgs & ISPL_ALLRESZERO) {
		// reset resistances to zero if the respective special effect is active
		mr = 0;
		fr = 0;
		lr = 0;
	}

	if (mr > MAXRESIST)
		mr = MAXRESIST;
	plr[p].data._pMagResist = mr;

	if (fr > MAXRESIST)
		fr = MAXRESIST;
	plr[p].data._pFireResist = fr;

	if (lr > MAXRESIST)
		lr = MAXRESIST;
	plr[p].data._pLghtResist = lr;

	if (plr[p].data._pClass == PC_WARRIOR) {
		vadd *= 2;
	}
	if (plr[p].data._pClass == PC_ROGUE) {
		vadd += vadd >> 1;
	}
	ihp += (vadd << 6);

	if (plr[p].data._pClass == PC_SORCERER) {
		madd *= 2;
	}
	if (plr[p].data._pClass == PC_ROGUE) {
		madd += madd >> 1;
	}
	imana += (madd << 6);

	plr[p].data._pHitPoints = ihp + plr[p].data._pHPBase;
	plr[p].data._pMaxHP = ihp + plr[p].data._pMaxHPBase;

	if (p == myplr() && (plr[p].data._pHitPoints >> 6) <= 0) {
		plr[p].SetPlayerHitPoints(0);
	}

	plr[p].data._pMana = imana + plr[p].data._pManaBase;
	plr[p].data._pMaxMana = imana + plr[p].data._pMaxManaBase;

	plr[p].data._pIFMinDam = fmin;
	plr[p].data._pIFMaxDam = fmax;
	plr[p].data._pILMinDam = lmin;
	plr[p].data._pILMaxDam = lmax;

	if (iflgs & ISPL_INFRAVISION) {
		plr[p].data._pInfraFlag = TRUE;
	} else {
		plr[p].data._pInfraFlag = FALSE;
	}

	plr[p].data._pBlockFlag = FALSE;
	plr[p].data._pwtype = WT_MELEE;

	g = 0;

	if (plr[p].data.InvBody[INVLOC_HAND_LEFT]._itype != ITYPE_NONE
	    && plr[p].data.InvBody[INVLOC_HAND_LEFT]._iClass == ICLASS_WEAPON
	    && plr[p].data.InvBody[INVLOC_HAND_LEFT]._iStatFlag) {
		g = plr[p].data.InvBody[INVLOC_HAND_LEFT]._itype;
	}

	if (plr[p].data.InvBody[INVLOC_HAND_RIGHT]._itype != ITYPE_NONE
	    && plr[p].data.InvBody[INVLOC_HAND_RIGHT]._iClass == ICLASS_WEAPON
	    && plr[p].data.InvBody[INVLOC_HAND_RIGHT]._iStatFlag) {
		g = plr[p].data.InvBody[INVLOC_HAND_RIGHT]._itype;
	}

	switch (g) {
	case ITYPE_SWORD:
		g = ANIM_ID_SWORD;
		break;
	case ITYPE_AXE:
		g = ANIM_ID_AXE;
		break;
	case ITYPE_BOW:
		plr[p].data._pwtype = WT_RANGED;
		g = ANIM_ID_BOW;
		break;
	case ITYPE_MACE:
		g = ANIM_ID_MACE;
		break;
	case ITYPE_STAFF:
		g = ANIM_ID_STAFF;
		break;
	}

	if (plr[p].data.InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_SHIELD && plr[p].data.InvBody[INVLOC_HAND_LEFT]._iStatFlag) {
		plr[p].data._pBlockFlag = TRUE;
		g++;
	}
	if (plr[p].data.InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_SHIELD && plr[p].data.InvBody[INVLOC_HAND_RIGHT]._iStatFlag) {
		plr[p].data._pBlockFlag = TRUE;
		g++;
	}

	if (plr[p].data.InvBody[INVLOC_CHEST]._itype == ITYPE_MARMOR && plr[p].data.InvBody[INVLOC_CHEST]._iStatFlag) {
		g += ANIM_ID_MEDIUM_ARMOR;
	}
	if (plr[p].data.InvBody[INVLOC_CHEST]._itype == ITYPE_HARMOR && plr[p].data.InvBody[INVLOC_CHEST]._iStatFlag) {
		g += ANIM_ID_HEAVY_ARMOR;
	}

	if (plr[p].data._pgfxnum != g && Loadgfx) {
		plr[p].data._pgfxnum = g;
		plr[p].data._pGFXLoad = 0;
		plr[p].LoadPlrGFX(PFILE_STAND);
		plr[p].SetPlrAnims();
		d = plr[p].data._pdir;
		assert(plr[p].data._pNAnim[size_t(d)]);
		plr[p].data._pAnimData = plr[p].data._pNAnim[int(d)];
		plr[p].data._pAnimLen = plr[p].data._pNFrames;
		plr[p].data._pAnimFrame = 1;
		plr[p].data._pAnimCnt = 0;
		plr[p].data._pAnimDelay = 3;
		plr[p].data._pAnimWidth = plr[p].data._pNWidth;
		plr[p].data._pAnimWidth2 = (plr[p].data._pNWidth - 64) >> 1;
	} else {
		plr[p].data._pgfxnum = g;
	}

	for (i = 0; i < nummissiles; i++) {
		mi = missileactive[i];
		if (missile[mi]._mitype == MIS_MANASHIELD && missile[mi]._misource == p) {
			missile[mi]._miVar1 = plr[p].data._pHitPoints;
			missile[mi]._miVar2 = plr[p].data._pHPBase;
		}
	}

	drawmanaflag = TRUE;
	drawhpflag = TRUE;
}

void CalcPlrScrolls(int p)
{
	int i, j;

	plr[p].data._pScrlSpells = 0;
	for (i = 0; i < plr[p].data._pNumInv; i++) {
		if (plr[p].data.InvList[i]._itype != ITYPE_NONE && (plr[p].data.InvList[i]._iMiscId == IMISC_SCROLL || plr[p].data.InvList[i]._iMiscId == IMISC_SCROLLT)) {
			if (plr[p].data.InvList[i]._iStatFlag)
				plr[p].data._pScrlSpells |= (__int64)1 << (plr[p].data.InvList[i]._iSpell - 1);
		}
	}

	for (j = 0; j < MAXBELTITEMS; j++) {
		if (plr[p].data.SpdList[j]._itype != ITYPE_NONE && (plr[p].data.SpdList[j]._iMiscId == IMISC_SCROLL || plr[p].data.SpdList[j]._iMiscId == IMISC_SCROLLT)) {
			if (plr[p].data.SpdList[j]._iStatFlag)
				plr[p].data._pScrlSpells |= (__int64)1 << (plr[p].data.SpdList[j]._iSpell - 1);
		}
	}
	if (plr[p].data._pRSplType == RSPLTYPE_SCROLL) {
		if (!(plr[p].data._pScrlSpells & 1 << (plr[p].data._pRSpell - 1))) {
			plr[p].data._pRSpell = SPL_INVALID;
			plr[p].data._pRSplType = RSPLTYPE_INVALID;
			force_redraw = 255;
		}
	}
}

void CalcPlrStaff(int p)
{
	plr[p].data._pISpells = 0;
	if (plr[p].data.InvBody[INVLOC_HAND_LEFT]._itype != ITYPE_NONE
	    && plr[p].data.InvBody[INVLOC_HAND_LEFT]._iStatFlag
	    && plr[p].data.InvBody[INVLOC_HAND_LEFT]._iCharges > 0) {
		plr[p].data._pISpells |= (__int64)1 << (plr[p].data.InvBody[INVLOC_HAND_LEFT]._iSpell - 1);
	}
}

void CalcSelfItems(int pnum)
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
		changeflag = FALSE;
		for (i = 0; i < NUM_INVLOC; i++) {
			if (p->InvBody[i]._itype != ITYPE_NONE && p->InvBody[i]._iStatFlag) {
				sf = TRUE;
				if (sa + p->_pBaseStr < p->InvBody[i]._iMinStr)
					sf = FALSE;
				if (ma + p->_pBaseMag < p->InvBody[i]._iMinMag)
					sf = FALSE;
				if (da + p->_pBaseDex < p->InvBody[i]._iMinDex)
					sf = FALSE;
				if (!sf) {
					changeflag = TRUE;
					p->InvBody[i]._iStatFlag = FALSE;
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

void CalcPlrItemMin(int pnum)
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

bool ItemMinStats(PlayerStruct *p, ItemStruct *x)
{
	if (p->_pMagic < x->_iMinMag)
		return FALSE;

	if (p->_pStrength < x->_iMinStr)
		return FALSE;

	if (p->_pDexterity < x->_iMinDex)
		return FALSE;

	return TRUE;
}

void CalcPlrBookVals(int p)
{
	int i, slvl;

	if (level.currlevel == 0) {
		for (i = 1; witchitem[i]._itype != ITYPE_NONE; i++) {
			WitchBookLevel(i);
			witchitem[i]._iStatFlag = StoreStatOk(&witchitem[i]);
		}
	}

	for (i = 0; i < plr[p].data._pNumInv; i++) {
		if (plr[p].data.InvList[i]._itype == ITYPE_MISC && plr[p].data.InvList[i]._iMiscId == IMISC_BOOK) {
			plr[p].data.InvList[i]._iMinMag = spelldata[plr[p].data.InvList[i]._iSpell].sMinInt;
			slvl = plr[p].data._pSplLvl[plr[p].data.InvList[i]._iSpell];

			while (slvl != 0) {
				plr[p].data.InvList[i]._iMinMag += 20 * plr[p].data.InvList[i]._iMinMag / 100;
				slvl--;
				if (plr[p].data.InvList[i]._iMinMag + 20 * plr[p].data.InvList[i]._iMinMag / 100 > 255) {
					plr[p].data.InvList[i]._iMinMag = 255;
					slvl = 0;
				}
			}
			plr[p].data.InvList[i]._iStatFlag = ItemMinStats(&plr[p].data, &plr[p].data.InvList[i]);
		}
	}
}

void CalcPlrInv(int p, bool Loadgfx)
{
	CalcPlrItemMin(p);
	CalcSelfItems(p);
	CalcPlrItemVals(p, Loadgfx);
	CalcPlrItemMin(p);
	if (p == myplr()) {
		CalcPlrBookVals(p);
		CalcPlrScrolls(p);
		CalcPlrStaff(p);
		if (p == myplr() && level.currlevel == 0)
			RecalcStoreStats();
	}
}

void SetPlrHandItem(ItemStruct *h, int idata)
{
	ItemDataStruct *pAllItem;

	pAllItem = &AllItemsList[idata];

	// zero-initialize struct
	memset(h, 0, sizeof(*h));

	h->_itype = pAllItem->itype;
	h->_iCurs = pAllItem->iCurs;
	strcpy(h->_iName, pAllItem->iName);
	strcpy(h->_iIName, pAllItem->iName);
	h->_iLoc = pAllItem->iLoc;
	h->_iClass = pAllItem->iClass;
	h->_iMinDam = pAllItem->iMinDam;
	h->_iMaxDam = pAllItem->iMaxDam;
	h->_iAC = pAllItem->iMinAC;
	h->_iMiscId = pAllItem->iMiscId;
	h->_iSpell = pAllItem->iSpell;

	if (pAllItem->iMiscId == IMISC_STAFF) {
		h->_iCharges = 40;
	}

	h->_iMaxCharges = h->_iCharges;
	h->_iDurability = pAllItem->iDurability;
	h->_iMaxDur = pAllItem->iDurability;
	h->_iMinStr = pAllItem->iMinStr;
	h->_iMinMag = pAllItem->iMinMag;
	h->_iMinDex = pAllItem->iMinDex;
	h->_ivalue = pAllItem->iValue;
	h->_iIvalue = pAllItem->iValue;
	h->_iPrePower = -1;
	h->_iSufPower = -1;
	h->_iMagical = ITEM_QUALITY_NORMAL;
	h->IDidx = idata;
}

void GetPlrHandSeed(ItemStruct *h)
{
	h->_iSeed = GetRndSeed();
}

void GetGoldSeed(int pnum, ItemStruct *h)
{
	int i, ii, s;
	bool doneflag;

	do {
		doneflag = TRUE;
		s = GetRndSeed();
		for (i = 0; i < numitems; i++) {
			ii = itemactive[i];
			if (item[ii]._iSeed == s)
				doneflag = FALSE;
		}
		if (pnum == myplr()) {
			for (i = 0; i < plr[pnum].data._pNumInv; i++) {
				if (plr[pnum].data.InvList[i]._iSeed == s)
					doneflag = FALSE;
			}
		}
	} while (!doneflag);

	h->_iSeed = s;
}

void SetPlrHandSeed(ItemStruct *h, int iseed)
{
	h->_iSeed = iseed;
}

void SetPlrHandGoldCurs(ItemStruct *h)
{
	if (h->_ivalue >= GOLD_MEDIUM_LIMIT)
		h->_iCurs = ICURS_GOLD_LARGE;
	else if (h->_ivalue <= GOLD_SMALL_LIMIT)
		h->_iCurs = ICURS_GOLD_SMALL;
	else
		h->_iCurs = ICURS_GOLD_MEDIUM;
}

void CreatePlrItems(int p)
{
	int i;
	ItemStruct *pi = plr[p].data.InvBody;

	for (i = NUM_INVLOC; i != 0; i--) {
		pi->_itype = ITYPE_NONE;
		pi++;
	}

	// converting this to a for loop creates a `rep stosd` instruction,
	// so this probably actually was a memset
	memset(&plr[p].data.InvGrid, 0, sizeof(plr[p].data.InvGrid));

	pi = plr[p].data.InvList;
	for (i = NUM_INV_GRID_ELEM; i != 0; i--) {
		pi->_itype = ITYPE_NONE;
		pi++;
	}

	plr[p].data._pNumInv = 0;

	pi = &plr[p].data.SpdList[0];
	for (i = MAXBELTITEMS; i != 0; i--) {
		pi->_itype = ITYPE_NONE;
		pi++;
	}

	switch (plr[p].data._pClass) {
	case PC_WARRIOR:
		SetPlrHandItem(&plr[p].data.InvBody[INVLOC_HAND_LEFT], IDI_WARRIOR);
		GetPlrHandSeed(&plr[p].data.InvBody[INVLOC_HAND_LEFT]);

		SetPlrHandItem(&plr[p].data.InvBody[INVLOC_HAND_RIGHT], IDI_WARRSHLD);
		GetPlrHandSeed(&plr[p].data.InvBody[INVLOC_HAND_RIGHT]);

#ifdef _DEBUG
		if (!debug_mode_key_w) {
#endif
			SetPlrHandItem(&plr[p].data.HoldItem, IDI_WARRCLUB);
			GetPlrHandSeed(&plr[p].data.HoldItem);
			plr[p].inventory.AutoPlace(0, { 1, 3 }, TRUE);
#ifdef _DEBUG
		}
#endif

		SetPlrHandItem(&plr[p].data.SpdList[0], IDI_HEAL);
		GetPlrHandSeed(&plr[p].data.SpdList[0]);

		SetPlrHandItem(&plr[p].data.SpdList[1], IDI_HEAL);
		GetPlrHandSeed(&plr[p].data.SpdList[1]);
		break;
	case PC_ROGUE:
		SetPlrHandItem(&plr[p].data.InvBody[INVLOC_HAND_LEFT], IDI_ROGUE);
		GetPlrHandSeed(&plr[p].data.InvBody[INVLOC_HAND_LEFT]);

		SetPlrHandItem(&plr[p].data.SpdList[0], IDI_HEAL);
		GetPlrHandSeed(&plr[p].data.SpdList[0]);

		SetPlrHandItem(&plr[p].data.SpdList[1], IDI_HEAL);
		GetPlrHandSeed(&plr[p].data.SpdList[1]);
		break;
	case PC_SORCERER:
		SetPlrHandItem(&plr[p].data.InvBody[INVLOC_HAND_LEFT], IDI_SORCEROR);
		GetPlrHandSeed(&plr[p].data.InvBody[INVLOC_HAND_LEFT]);

		SetPlrHandItem(&plr[p].data.SpdList[0], IDI_MANA);
		GetPlrHandSeed(&plr[p].data.SpdList[0]);

		SetPlrHandItem(&plr[p].data.SpdList[1], IDI_MANA);
		GetPlrHandSeed(&plr[p].data.SpdList[1]);
		break;
	}

	SetPlrHandItem(&plr[p].data.HoldItem, IDI_GOLD);
	GetPlrHandSeed(&plr[p].data.HoldItem);

#ifdef _DEBUG
	if (!debug_mode_key_w) {
#endif
		plr[p].data.HoldItem._ivalue = 100;
		plr[p].data.HoldItem._iCurs = ICURS_GOLD_SMALL;
		plr[p].data._pGold = plr[p].data.HoldItem._ivalue;
		plr[p].data.InvList[plr[p].data._pNumInv++] = plr[p].data.HoldItem;
		plr[p].data.InvGrid[30] = plr[p].data._pNumInv;
#ifdef _DEBUG
	} else {
		plr[p].data.HoldItem._ivalue = GOLD_MAX_LIMIT;
		plr[p].data.HoldItem._iCurs = ICURS_GOLD_LARGE;
		plr[p].data._pGold = plr[p].data.HoldItem._ivalue * 40;
		for (i = 0; i < NUM_INV_GRID_ELEM; i++) {
			GetPlrHandSeed(&plr[p].data.HoldItem);
			plr[p].data.InvList[plr[p].data._pNumInv++] = plr[p].data.HoldItem;
			plr[p].data.InvGrid[i] = plr[p].data._pNumInv;
		}
	}
#endif

	CalcPlrItemVals(p, FALSE);
}

bool ItemSpaceOk(int i, int j)
{
	int oi;

	if (!grid.isValid({ i, j }))
		return FALSE;

	if (grid[i][j].isMonster())
		return FALSE;

	if (grid[i][j].isPlayer())
		return FALSE;

	if (grid[i][j].isItem())
		return FALSE;

	if (grid[i][j].isObject()) {
		oi = grid[i][j].getObject();
		if (object[oi]._oSolidFlag)
			return FALSE;
	}

	if (grid[i + 1][j + 1].isObject() && object[grid[i + 1][j + 1].getObject()]._oSelFlag != 0)
		return FALSE;

	if (grid[i + 1][j + 1].isObject() && object[-(grid[i + 1][j + 1].getObject())]._oSelFlag != 0)
		return FALSE;

	if (grid[i + 1][j].isObject()
	    && grid[i][j + 1].isObject()
	    && object[grid[i + 1][j].getObject()]._oSelFlag != 0
	    && object[grid[i][j + 1].getObject()]._oSelFlag != 0) {
		return FALSE;
	}

	return !grid[i][j].isSolid();
}

bool GetItemSpace(V2Di pos, char inum)
{
	int i, j, rs;
	V2Di n;
	bool savail;

	n.y = 0;
	for (j = pos.y - 1; j <= pos.y + 1; j++) {
		n.x = 0;
		for (i = pos.x - 1; i <= pos.x + 1; i++) {
			itemhold[n.x][n.y] = ItemSpaceOk(i, j);
			n.x++;
		}
		n.y++;
	}

	savail = FALSE;
	for (j = 0; j < 3; j++) {
		for (i = 0; i < 3; i++) {
			if (itemhold[i][j])
				savail = TRUE;
		}
	}

	rs = random_(13, 15) + 1;
	if (!savail) return FALSE;

	n = { 0, 0 };
	while (rs > 0) {
		if (itemhold[n.x][n.y])
			rs--;
		if (rs > 0) {
			n.x++;
			if (n.x == 3) {
				n.x = 0;
				n.y++;
				if (n.y == 3) n.y = 0;
			}
		}
	}
	n += { pos.x - 1, pos.y - 1 };
	item[inum]._i = n;
	grid.at(n).setItem(inum);

	return TRUE;
}

void GetSuperItemSpace(V2Di pos, char inum)
{
	int xx, yy;
	int i, j, k;

	if (!GetItemSpace(pos, inum)) {
		for (k = 2; k < 50; k++) {
			for (j = -k; j <= k; j++) {
				yy = pos.y + j;
				for (i = -k; i <= k; i++) {
					xx = i + pos.x;
					if (ItemSpaceOk(xx, yy)) {
						item[inum]._i = { xx, yy };
						grid[xx][yy].setItem(inum);
						return;
					}
				}
			}
		}
	}
}

void GetSuperItemLoc(V2Di pos, V2Di & r)
{
	for (int k = 1; k < 50; k++) {
		for (int j = -k; j <= k; j++) {
			r.y = pos.y + j;
			for (int i = -k; i <= k; i++) {
				r.x = i + pos.x;
				if (ItemSpaceOk(r.x, r.y)) {
					return;
				}
			}
		}
	}
}

void CalcItemValue(int i)
{
	int v;

	v = item[i]._iVMult1 + item[i]._iVMult2;
	if (v > 0) {
		v *= item[i]._ivalue;
	}
	if (v < 0) {
		v = item[i]._ivalue / v;
	}
	v = item[i]._iVAdd1 + item[i]._iVAdd2 + v;
	if (v <= 0) {
		v = 1;
	}
	item[i]._iIvalue = v;
}

void GetBookSpell(int i, int lvl)
{
	int bs;
	if (lvl == 0)
		lvl = 1;
	int rv = random_(14, MAX_SPELLS) + 1;

	int s = SPL_FIREBOLT;
	while (rv > 0) {
		if (spelldata[s].sBookLvl != -1 && lvl >= spelldata[s].sBookLvl) {
			rv--;
			bs = s;
		}
		s++;
		if (plr.isSingleplayer()) {
			if (s == SPL_RESURRECT)
				s = SPL_TELEKINESIS;
		}
		if (plr.isSingleplayer()) {
			if (s == SPL_HEALOTHER)
				s = SPL_FLARE;
		}
		if (s == MAX_SPELLS)
			s = 1;
	}
	strcat(item[i]._iName, spelldata[bs].sNameText);
	strcat(item[i]._iIName, spelldata[bs].sNameText);
	item[i]._iSpell = bs;
	item[i]._iMinMag = spelldata[bs].sMinInt;
	item[i]._ivalue += spelldata[bs].sBookCost;
	item[i]._iIvalue += spelldata[bs].sBookCost;
	if (spelldata[bs].sType == STYPE_FIRE)
		item[i]._iCurs = ICURS_BOOK_RED;
	if (spelldata[bs].sType == STYPE_LIGHTNING)
		item[i]._iCurs = ICURS_BOOK_BLUE;
	if (spelldata[bs].sType == STYPE_MAGIC)
		item[i]._iCurs = ICURS_BOOK_GREY;
}

void GetStaffPower(int i, int lvl, int bs, bool onlygood)
{
	int l[256];
	char istr[128];
	int nl, j, preidx;
	bool addok;
	int tmp;

	tmp = random_(15, 10);
	preidx = -1;
	if (tmp == 0 || onlygood) {
		nl = 0;
		for (j = 0; PL_Prefix[j].PLPower != -1; j++) {
			if (PL_Prefix[j].PLIType & PLT_STAFF && PL_Prefix[j].PLMinLvl <= lvl) {
				addok = TRUE;
				if (onlygood && !PL_Prefix[j].PLOk)
					addok = FALSE;
				if (addok) {
					l[nl] = j;
					nl++;
					if (PL_Prefix[j].PLDouble) {
						l[nl] = j;
						nl++;
					}
				}
			}
		}
		if (nl != 0) {
			preidx = l[random_(16, nl)];
			sprintf(istr, "%s %s", PL_Prefix[preidx].PLName, item[i]._iIName);
			strcpy(item[i]._iIName, istr);
			item[i]._iMagical = ITEM_QUALITY_MAGIC;
			SaveItemPower(
			    i,
			    PL_Prefix[preidx].PLPower,
			    PL_Prefix[preidx].PLParam1,
			    PL_Prefix[preidx].PLParam2,
			    PL_Prefix[preidx].PLMinVal,
			    PL_Prefix[preidx].PLMaxVal,
			    PL_Prefix[preidx].PLMultVal);
			item[i]._iPrePower = PL_Prefix[preidx].PLPower;
		}
	}
	if (!control_WriteStringToBuffer((BYTE *)item[i]._iIName)) {
		strcpy(item[i]._iIName, AllItemsList[item[i].IDidx].iSName);
		if (preidx != -1) {
			sprintf(istr, "%s %s", PL_Prefix[preidx].PLName, item[i]._iIName);
			strcpy(item[i]._iIName, istr);
		}
		sprintf(istr, "%s of %s", item[i]._iIName, spelldata[bs].sNameText);
		strcpy(item[i]._iIName, istr);
		if (item[i]._iMagical == ITEM_QUALITY_NORMAL)
			strcpy(item[i]._iName, item[i]._iIName);
	}
	CalcItemValue(i);
}

void GetStaffSpell(int i, int lvl, bool onlygood)
{
	int l, rv, s, minc, maxc, v, bs;
	char istr[64];

	if (!random_(17, 4)) {
		GetItemPower(i, lvl >> 1, lvl, PLT_STAFF, onlygood);
	} else {
		l = lvl >> 1;
		if (l == 0)
			l = 1;
		rv = random_(18, MAX_SPELLS) + 1;
		s = SPL_FIREBOLT;
		while (rv > 0) {
			if (spelldata[s].sStaffLvl != -1 && l >= spelldata[s].sStaffLvl) {
				rv--;
				bs = s;
			}
			s++;
			if (plr.isSingleplayer() && s == SPL_RESURRECT)
				s = SPL_TELEKINESIS;
			if (plr.isSingleplayer() && s == SPL_HEALOTHER)
				s = SPL_FLARE;
			if (s == MAX_SPELLS)
				s = SPL_FIREBOLT;
		}
		sprintf(istr, "%s of %s", item[i]._iName, spelldata[bs].sNameText);
		if (!control_WriteStringToBuffer((BYTE *)istr))
			sprintf(istr, "Staff of %s", spelldata[bs].sNameText);
		strcpy(item[i]._iName, istr);
		strcpy(item[i]._iIName, istr);

		minc = spelldata[bs].sStaffMin;
		maxc = spelldata[bs].sStaffMax - minc + 1;
		item[i]._iSpell = bs;
		item[i]._iCharges = minc + random_(19, maxc);
		item[i]._iMaxCharges = item[i]._iCharges;

		item[i]._iMinMag = spelldata[bs].sMinInt;
		v = item[i]._iCharges * spelldata[bs].sStaffCost / 5;
		item[i]._ivalue += v;
		item[i]._iIvalue += v;
		GetStaffPower(i, lvl, bs, onlygood);
	}
}

void GetItemAttrs(int i, int idata, int lvl)
{
	int rndv;

	item[i]._itype = AllItemsList[idata].itype;
	item[i]._iCurs = AllItemsList[idata].iCurs;
	strcpy(item[i]._iName, AllItemsList[idata].iName);
	strcpy(item[i]._iIName, AllItemsList[idata].iName);
	item[i]._iLoc = AllItemsList[idata].iLoc;
	item[i]._iClass = AllItemsList[idata].iClass;
	item[i]._iMinDam = AllItemsList[idata].iMinDam;
	item[i]._iMaxDam = AllItemsList[idata].iMaxDam;
	item[i]._iAC = AllItemsList[idata].iMinAC + random_(20, AllItemsList[idata].iMaxAC - AllItemsList[idata].iMinAC + 1);
	item[i]._iFlags = AllItemsList[idata].iFlags;
	item[i]._iMiscId = AllItemsList[idata].iMiscId;
	item[i]._iSpell = AllItemsList[idata].iSpell;
	item[i]._iMagical = ITEM_QUALITY_NORMAL;
	item[i]._ivalue = AllItemsList[idata].iValue;
	item[i]._iIvalue = AllItemsList[idata].iValue;
	item[i]._iVAdd1 = 0;
	item[i]._iVMult1 = 0;
	item[i]._iVAdd2 = 0;
	item[i]._iVMult2 = 0;
	item[i]._iPLDam = 0;
	item[i]._iPLToHit = 0;
	item[i]._iPLAC = 0;
	item[i]._iPLStr = 0;
	item[i]._iPLMag = 0;
	item[i]._iPLDex = 0;
	item[i]._iPLVit = 0;
	item[i]._iCharges = 0;
	item[i]._iMaxCharges = 0;
	item[i]._iDurability = AllItemsList[idata].iDurability;
	item[i]._iMaxDur = AllItemsList[idata].iDurability;
	item[i]._iMinStr = AllItemsList[idata].iMinStr;
	item[i]._iMinMag = AllItemsList[idata].iMinMag;
	item[i]._iMinDex = AllItemsList[idata].iMinDex;
	item[i]._iPLFR = 0;
	item[i]._iPLLR = 0;
	item[i]._iPLMR = 0;
	item[i].IDidx = idata;
	item[i]._iPLDamMod = 0;
	item[i]._iPLGetHit = 0;
	item[i]._iPLLight = 0;
	item[i]._iSplLvlAdd = 0;
	item[i]._iRequest = FALSE;
	item[i]._iFMinDam = 0;
	item[i]._iFMaxDam = 0;
	item[i]._iLMinDam = 0;
	item[i]._iLMaxDam = 0;
	item[i]._iPLEnAc = 0;
	item[i]._iPLMana = 0;
	item[i]._iPLHP = 0;
	item[i]._iPrePower = -1;
	item[i]._iSufPower = -1;

	if (item[i]._iMiscId == IMISC_BOOK)
		GetBookSpell(i, lvl);

	if (item[i]._itype == ITYPE_GOLD) {
		if (gnDifficulty == DIFF_NORMAL)
			rndv = 5 * level.currlevel + random_(21, 10 * level.currlevel);
		if (gnDifficulty == DIFF_NIGHTMARE)
			rndv = 5 * (level.currlevel + 16) + random_(21, 10 * (level.currlevel + 16));
		if (gnDifficulty == DIFF_HELL)
			rndv = 5 * (level.currlevel + 32) + random_(21, 10 * (level.currlevel + 32));
		if (level.leveltype == DTYPE_HELL)
			rndv += rndv >> 3;
		if (rndv > GOLD_MAX_LIMIT)
			rndv = GOLD_MAX_LIMIT;

		item[i]._ivalue = rndv;

		if (rndv >= GOLD_MEDIUM_LIMIT)
			item[i]._iCurs = ICURS_GOLD_LARGE;
		else
			item[i]._iCurs = (rndv > GOLD_SMALL_LIMIT) + 4;
	}
}

int RndPL(int param1, int param2)
{
	return param1 + random_(22, param2 - param1 + 1);
}

int PLVal(int pv, int p1, int p2, int minv, int maxv)
{
	if (p1 == p2)
		return minv;
	if (minv == maxv)
		return minv;
	return minv + (maxv - minv) * (100 * (pv - p1) / (p2 - p1)) / 100;
}

void SaveItemPower(int i, int power, int param1, int param2, int minval, int maxval, int multval)
{
	int r, r2;

	r = RndPL(param1, param2);
	switch (power) {
	case IPL_TOHIT:
		item[i]._iPLToHit += r;
		break;
	case IPL_TOHIT_CURSE:
		item[i]._iPLToHit -= r;
		break;
	case IPL_DAMP:
		item[i]._iPLDam += r;
		break;
	case IPL_DAMP_CURSE:
		item[i]._iPLDam -= r;
		break;
	case IPL_TOHIT_DAMP:
		r = RndPL(param1, param2);
		item[i]._iPLDam += r;
		if (param1 == 20)
			r2 = RndPL(1, 5);
		if (param1 == 36)
			r2 = RndPL(6, 10);
		if (param1 == 51)
			r2 = RndPL(11, 15);
		if (param1 == 66)
			r2 = RndPL(16, 20);
		if (param1 == 81)
			r2 = RndPL(21, 30);
		if (param1 == 96)
			r2 = RndPL(31, 40);
		if (param1 == 111)
			r2 = RndPL(41, 50);
		if (param1 == 126)
			r2 = RndPL(51, 75);
		if (param1 == 151)
			r2 = RndPL(76, 100);
		item[i]._iPLToHit += r2;
		break;
	case IPL_TOHIT_DAMP_CURSE:
		item[i]._iPLDam -= r;
		if (param1 == 25)
			r2 = RndPL(1, 5);
		if (param1 == 50)
			r2 = RndPL(6, 10);
		item[i]._iPLToHit -= r2;
		break;
	case IPL_ACP:
		item[i]._iPLAC += r;
		break;
	case IPL_ACP_CURSE:
		item[i]._iPLAC -= r;
		break;
	case IPL_SETAC:
		item[i]._iAC = r;
		break;
	case IPL_AC_CURSE:
		item[i]._iAC -= r;
		break;
	case IPL_FIRERES:
		item[i]._iPLFR += r;
		break;
	case IPL_LIGHTRES:
		item[i]._iPLLR += r;
		break;
	case IPL_MAGICRES:
		item[i]._iPLMR += r;
		break;
	case IPL_ALLRES:
		item[i]._iPLFR += r;
		item[i]._iPLLR += r;
		item[i]._iPLMR += r;
		if (item[i]._iPLFR < 0)
			item[i]._iPLFR = 0;
		if (item[i]._iPLLR < 0)
			item[i]._iPLLR = 0;
		if (item[i]._iPLMR < 0)
			item[i]._iPLMR = 0;
		break;
	case IPL_SPLLVLADD:
		item[i]._iSplLvlAdd = r;
		break;
	case IPL_CHARGES:
		item[i]._iCharges *= param1;
		item[i]._iMaxCharges = item[i]._iCharges;
		break;
	case IPL_SPELL:
		item[i]._iSpell = param1;
		item[i]._iCharges = param1; // BUGFIX: should be param2. This code was correct in v1.04, and the bug was introduced between 1.04 and 1.09b.
		item[i]._iMaxCharges = param2;
		break;
	case IPL_FIREDAM:
		item[i]._iFlags |= ISPL_FIREDAM;
		item[i]._iFMinDam = param1;
		item[i]._iFMaxDam = param2;
		break;
	case IPL_LIGHTDAM:
		item[i]._iFlags |= ISPL_LIGHTDAM;
		item[i]._iLMinDam = param1;
		item[i]._iLMaxDam = param2;
		break;
	case IPL_STR:
		item[i]._iPLStr += r;
		break;
	case IPL_STR_CURSE:
		item[i]._iPLStr -= r;
		break;
	case IPL_MAG:
		item[i]._iPLMag += r;
		break;
	case IPL_MAG_CURSE:
		item[i]._iPLMag -= r;
		break;
	case IPL_DEX:
		item[i]._iPLDex += r;
		break;
	case IPL_DEX_CURSE:
		item[i]._iPLDex -= r;
		break;
	case IPL_VIT:
		item[i]._iPLVit += r;
		break;
	case IPL_VIT_CURSE:
		item[i]._iPLVit -= r;
		break;
	case IPL_ATTRIBS:
		item[i]._iPLStr += r;
		item[i]._iPLMag += r;
		item[i]._iPLDex += r;
		item[i]._iPLVit += r;
		break;
	case IPL_ATTRIBS_CURSE:
		item[i]._iPLStr -= r;
		item[i]._iPLMag -= r;
		item[i]._iPLDex -= r;
		item[i]._iPLVit -= r;
		break;
	case IPL_GETHIT_CURSE:
		item[i]._iPLGetHit += r;
		break;
	case IPL_GETHIT:
		item[i]._iPLGetHit -= r;
		break;
	case IPL_LIFE:
		item[i]._iPLHP += r << 6;
		break;
	case IPL_LIFE_CURSE:
		item[i]._iPLHP -= r << 6;
		break;
	case IPL_MANA:
		item[i]._iPLMana += r << 6;
		drawmanaflag = TRUE;
		break;
	case IPL_MANA_CURSE:
		item[i]._iPLMana -= r << 6;
		drawmanaflag = TRUE;
		break;
	case IPL_DUR:
		r2 = r * item[i]._iMaxDur / 100;
		item[i]._iMaxDur += r2;
		item[i]._iDurability += r2;
		break;
	case IPL_DUR_CURSE:
		item[i]._iMaxDur -= r * item[i]._iMaxDur / 100;
		if (item[i]._iMaxDur < 1)
			item[i]._iMaxDur = 1;
		item[i]._iDurability = item[i]._iMaxDur;
		break;
	case IPL_INDESTRUCTIBLE:
		item[i]._iDurability = DUR_INDESTRUCTIBLE;
		item[i]._iMaxDur = DUR_INDESTRUCTIBLE;
		break;
	case IPL_LIGHT:
		item[i]._iPLLight += param1;
		break;
	case IPL_LIGHT_CURSE:
		item[i]._iPLLight -= param1;
		break;
	case IPL_FIRE_ARROWS:
		item[i]._iFlags |= ISPL_FIRE_ARROWS;
		item[i]._iFMinDam = param1;
		item[i]._iFMaxDam = param2;
		break;
	case IPL_LIGHT_ARROWS:
		item[i]._iFlags |= ISPL_LIGHT_ARROWS;
		item[i]._iLMinDam = param1;
		item[i]._iLMaxDam = param2;
		break;
	case IPL_THORNS:
		item[i]._iFlags |= ISPL_THORNS;
		break;
	case IPL_NOMANA:
		item[i]._iFlags |= ISPL_NOMANA;
		drawmanaflag = TRUE;
		break;
	case IPL_NOHEALPLR:
		item[i]._iFlags |= ISPL_NOHEALPLR;
		break;
	case IPL_ABSHALFTRAP:
		item[i]._iFlags |= ISPL_ABSHALFTRAP;
		break;
	case IPL_KNOCKBACK:
		item[i]._iFlags |= ISPL_KNOCKBACK;
		break;
	case IPL_3XDAMVDEM:
		item[i]._iFlags |= ISPL_3XDAMVDEM;
		break;
	case IPL_ALLRESZERO:
		item[i]._iFlags |= ISPL_ALLRESZERO;
		break;
	case IPL_NOHEALMON:
		item[i]._iFlags |= ISPL_NOHEALMON;
		break;
	case IPL_STEALMANA:
		if (param1 == 3)
			item[i]._iFlags |= ISPL_STEALMANA_3;
		if (param1 == 5)
			item[i]._iFlags |= ISPL_STEALMANA_5;
		drawmanaflag = TRUE;
		break;
	case IPL_STEALLIFE:
		if (param1 == 3)
			item[i]._iFlags |= ISPL_STEALLIFE_3;
		if (param1 == 5)
			item[i]._iFlags |= ISPL_STEALLIFE_5;
		drawhpflag = TRUE;
		break;
	case IPL_TARGAC:
		item[i]._iPLEnAc += r;
		break;
	case IPL_FASTATTACK:
		if (param1 == 1)
			item[i]._iFlags |= ISPL_QUICKATTACK;
		if (param1 == 2)
			item[i]._iFlags |= ISPL_FASTATTACK;
		if (param1 == 3)
			item[i]._iFlags |= ISPL_FASTERATTACK;
		if (param1 == 4)
			item[i]._iFlags |= ISPL_FASTESTATTACK;
		break;
	case IPL_FASTRECOVER:
		if (param1 == 1)
			item[i]._iFlags |= ISPL_FASTRECOVER;
		if (param1 == 2)
			item[i]._iFlags |= ISPL_FASTERRECOVER;
		if (param1 == 3)
			item[i]._iFlags |= ISPL_FASTESTRECOVER;
		break;
	case IPL_FASTBLOCK:
		item[i]._iFlags |= ISPL_FASTBLOCK;
		break;
	case IPL_DAMMOD:
		item[i]._iPLDamMod += r;
		break;
	case IPL_RNDARROWVEL:
		item[i]._iFlags |= ISPL_RNDARROWVEL;
		break;
	case IPL_SETDAM:
		item[i]._iMinDam = param1;
		item[i]._iMaxDam = param2;
		break;
	case IPL_SETDUR:
		item[i]._iDurability = param1;
		item[i]._iMaxDur = param1;
		break;
	case IPL_FASTSWING:
		item[i]._iFlags |= ISPL_FASTERATTACK;
		break;
	case IPL_ONEHAND:
		item[i]._iLoc = ILOC_ONEHAND;
		break;
	case IPL_DRAINLIFE:
		item[i]._iFlags |= ISPL_DRAINLIFE;
		break;
	case IPL_RNDSTEALLIFE:
		item[i]._iFlags |= ISPL_RNDSTEALLIFE;
		break;
	case IPL_INFRAVISION:
		item[i]._iFlags |= ISPL_INFRAVISION;
		break;
	case IPL_NOMINSTR:
		item[i]._iMinStr = 0;
		break;
	case IPL_INVCURS:
		item[i]._iCurs = param1;
		break;
	case IPL_ADDACLIFE:
		item[i]._iPLHP = (myplr().data._pIBonusAC + myplr().data._pIAC + myplr().data._pDexterity / 5) << 6;
		break;
	case IPL_ADDMANAAC:
		item[i]._iAC += (myplr().data._pMaxManaBase >> 6) / 10;
		break;
	case IPL_FIRERESCLVL:
		item[i]._iPLFR = 30 - myplr().data._pLevel;
		if (item[i]._iPLFR < 0)
			item[i]._iPLFR = 0;
		break;
	}
	if (item[i]._iVAdd1 || item[i]._iVMult1) {
		item[i]._iVAdd2 = PLVal(r, param1, param2, minval, maxval);
		item[i]._iVMult2 = multval;
	} else {
		item[i]._iVAdd1 = PLVal(r, param1, param2, minval, maxval);
		item[i]._iVMult1 = multval;
	}
}

void GetItemPower(int i, int minlvl, int maxlvl, int flgs, bool onlygood)
{
	int pre, post, nt, nl, j, preidx, sufidx;
	int l[256];
	char istr[128];
	BYTE goe;

	pre = random_(23, 4);
	post = random_(23, 3);
	if (pre != 0 && post == 0) {
		if (random_(23, 2))
			post = 1;
		else
			pre = 0;
	}
	preidx = -1;
	sufidx = -1;
	goe = 0;
	if (!onlygood && random_(0, 3))
		onlygood = TRUE;
	if (!pre) {
		nt = 0;
		for (j = 0; PL_Prefix[j].PLPower != -1; j++) {
			if (flgs & PL_Prefix[j].PLIType) {
				if (PL_Prefix[j].PLMinLvl >= minlvl && PL_Prefix[j].PLMinLvl <= maxlvl && (!onlygood || PL_Prefix[j].PLOk) && (flgs != 256 || PL_Prefix[j].PLPower != 15)) {
					l[nt] = j;
					nt++;
					if (PL_Prefix[j].PLDouble) {
						l[nt] = j;
						nt++;
					}
				}
			}
		}
		if (nt != 0) {
			preidx = l[random_(23, nt)];
			sprintf(istr, "%s %s", PL_Prefix[preidx].PLName, item[i]._iIName);
			strcpy(item[i]._iIName, istr);
			item[i]._iMagical = ITEM_QUALITY_MAGIC;
			SaveItemPower(
			    i,
			    PL_Prefix[preidx].PLPower,
			    PL_Prefix[preidx].PLParam1,
			    PL_Prefix[preidx].PLParam2,
			    PL_Prefix[preidx].PLMinVal,
			    PL_Prefix[preidx].PLMaxVal,
			    PL_Prefix[preidx].PLMultVal);
			item[i]._iPrePower = PL_Prefix[preidx].PLPower;
			goe = PL_Prefix[preidx].PLGOE;
		}
	}
	if (post != 0) {
		nl = 0;
		for (j = 0; PL_Suffix[j].PLPower != -1; j++) {
			if (PL_Suffix[j].PLIType & flgs
			    && PL_Suffix[j].PLMinLvl >= minlvl && PL_Suffix[j].PLMinLvl <= maxlvl
			    && (goe | PL_Suffix[j].PLGOE) != 0x11
			    && (!onlygood || PL_Suffix[j].PLOk)) {
				l[nl] = j;
				nl++;
			}
		}
		if (nl != 0) {
			sufidx = l[random_(23, nl)];
			sprintf(istr, "%s of %s", item[i]._iIName, PL_Suffix[sufidx].PLName);
			strcpy(item[i]._iIName, istr);
			item[i]._iMagical = ITEM_QUALITY_MAGIC;
			SaveItemPower(
			    i,
			    PL_Suffix[sufidx].PLPower,
			    PL_Suffix[sufidx].PLParam1,
			    PL_Suffix[sufidx].PLParam2,
			    PL_Suffix[sufidx].PLMinVal,
			    PL_Suffix[sufidx].PLMaxVal,
			    PL_Suffix[sufidx].PLMultVal);
			item[i]._iSufPower = PL_Suffix[sufidx].PLPower;
		}
	}
	if (!control_WriteStringToBuffer((BYTE *)item[i]._iIName)) {
		strcpy(item[i]._iIName, AllItemsList[item[i].IDidx].iSName);
		if (preidx != -1) {
			sprintf(istr, "%s %s", PL_Prefix[preidx].PLName, item[i]._iIName);
			strcpy(item[i]._iIName, istr);
		}
		if (sufidx != -1) {
			sprintf(istr, "%s of %s", item[i]._iIName, PL_Suffix[sufidx].PLName);
			strcpy(item[i]._iIName, istr);
		}
	}
	if (preidx != -1 || sufidx != -1)
		CalcItemValue(i);
}

void GetItemBonus(int i, int idata, int minlvl, int maxlvl, bool onlygood)
{
	if (item[i]._iClass != ICLASS_GOLD) {
		if (minlvl > 25)
			minlvl = 25;

		switch (item[i]._itype) {
		case ITYPE_SWORD:
		case ITYPE_AXE:
		case ITYPE_MACE:
			GetItemPower(i, minlvl, maxlvl, PLT_WEAP, onlygood);
			break;
		case ITYPE_BOW:
			GetItemPower(i, minlvl, maxlvl, PLT_BOW, onlygood);
			break;
		case ITYPE_SHIELD:
			GetItemPower(i, minlvl, maxlvl, PLT_SHLD, onlygood);
			break;
		case ITYPE_LARMOR:
		case ITYPE_HELM:
		case ITYPE_MARMOR:
		case ITYPE_HARMOR:
			GetItemPower(i, minlvl, maxlvl, PLT_ARMO, onlygood);
			break;
		case ITYPE_STAFF:
			GetStaffSpell(i, maxlvl, onlygood);
			break;
		case ITYPE_RING:
		case ITYPE_AMULET:
			GetItemPower(i, minlvl, maxlvl, PLT_MISC, onlygood);
			break;
		}
	}
}

void SetupItem(int i)
{
	int it;

	it = ItemCAnimTbl[item[i]._iCurs];
	item[i]._iAnimData = itemanims[it];
	item[i]._iAnimLen = ItemAnimLs[it];
	item[i]._iAnimWidth = 96;
	item[i]._iAnimWidth2 = 16;
	item[i]._iIdentified = FALSE;
	item[i]._iPostDraw = FALSE;

	if (!myplr().data.pLvlLoad) {
		item[i]._iAnimFrame = 1;
		item[i]._iAnimFlag = TRUE;
		item[i]._iSelFlag = 0;
	} else {
		item[i]._iAnimFrame = item[i]._iAnimLen;
		item[i]._iAnimFlag = FALSE;
		item[i]._iSelFlag = 1;
	}
}

int RndItem(int m)
{
	int i, ri;
	int ril[512];

	if ((monsters[m].data.MData->mTreasure & 0x8000) != 0)
		return -1 - (monsters[m].data.MData->mTreasure & 0xFFF);

	if (monsters[m].data.MData->mTreasure & 0x4000)
		return 0;

	if (random_(24, 100) > 40)
		return 0;

	if (random_(24, 100) > 25)
		return 1;

	ri = 0;
	for (i = 0; AllItemsList[i].iLoc != ILOC_INVALID; i++) {
		if (AllItemsList[i].iRnd == 2 && monsters[m].data.mLevel >= AllItemsList[i].iMinMLvl) {
			ril[ri] = i;
			ri++;
		}
		if (AllItemsList[i].iRnd && monsters[m].data.mLevel >= AllItemsList[i].iMinMLvl) {
			ril[ri] = i;
			ri++;
		}
		if (AllItemsList[i].iSpell == SPL_RESURRECT && plr.isSingleplayer())
			ri--;
		if (AllItemsList[i].iSpell == SPL_HEALOTHER && plr.isSingleplayer())
			ri--;
	}

	return ril[random_(24, ri)] + 1;
}

int RndUItem(int m)
{
	int i, ri;
	int ril[512];
	bool okflag;

	if (m != -1 && (monsters[m].data.MData->mTreasure & 0x8000) != 0 && plr.isSingleplayer())
		return -1 - (monsters[m].data.MData->mTreasure & 0xFFF);

	ri = 0;
	for (i = 0; AllItemsList[i].iLoc != ILOC_INVALID; i++) {
		okflag = TRUE;
		if (!AllItemsList[i].iRnd)
			okflag = FALSE;
		if (m != -1) {
			if (monsters[m].data.mLevel < AllItemsList[i].iMinMLvl)
				okflag = FALSE;
		} else {
			if (2 * level.currlevel < AllItemsList[i].iMinMLvl)
				okflag = FALSE;
		}
		if (AllItemsList[i].itype == ITYPE_MISC)
			okflag = FALSE;
		if (AllItemsList[i].itype == ITYPE_GOLD)
			okflag = FALSE;
		if (AllItemsList[i].itype == ITYPE_MEAT)
			okflag = FALSE;
		if (AllItemsList[i].iMiscId == IMISC_BOOK)
			okflag = TRUE;
		if (AllItemsList[i].iSpell == SPL_RESURRECT && plr.isSingleplayer())
			okflag = FALSE;
		if (AllItemsList[i].iSpell == SPL_HEALOTHER && plr.isSingleplayer())
			okflag = FALSE;
		if (okflag) {
			ril[ri] = i;
			ri++;
		}
	}

	return ril[random_(25, ri)];
}

int RndAllItems()
{
	int i, ri;
	int ril[512];

	if (random_(26, 100) > 25)
		return 0;

	ri = 0;
	for (i = 0; AllItemsList[i].iLoc != ILOC_INVALID; i++) {
		if (AllItemsList[i].iRnd && 2 * level.currlevel >= AllItemsList[i].iMinMLvl) {
			ril[ri] = i;
			ri++;
		}
		if (AllItemsList[i].iSpell == SPL_RESURRECT && plr.isSingleplayer())
			ri--;
		if (AllItemsList[i].iSpell == SPL_HEALOTHER && plr.isSingleplayer())
			ri--;
	}

	return ril[random_(26, ri)];
}

int RndTypeItems(int itype, int imid)
{
	int i, ri;
	bool okflag;
	int ril[512];

	ri = 0;
	for (i = 0; AllItemsList[i].iLoc != ILOC_INVALID; i++) {
		okflag = TRUE;
		if (!AllItemsList[i].iRnd)
			okflag = FALSE;
		if (level.currlevel << 1 < AllItemsList[i].iMinMLvl)
			okflag = FALSE;
		if (AllItemsList[i].itype != itype)
			okflag = FALSE;
		if (imid != -1 && AllItemsList[i].iMiscId != imid)
			okflag = FALSE;
		if (okflag) {
			ril[ri] = i;
			ri++;
		}
	}

	return ril[random_(27, ri)];
}

int CheckUnique(int i, int lvl, int uper, bool recreate)
{
	int j, idata, numu;
	BOOLEAN uok[128];

	if (random_(28, 100) > uper)
		return -1;

	numu = 0;
	memset(uok, 0, sizeof(uok));
	for (j = 0; UniqueItemList[j].UIItemId != UITYPE_INVALID; j++) {
		if (UniqueItemList[j].UIItemId == AllItemsList[item[i].IDidx].iItemId
		    && lvl >= UniqueItemList[j].UIMinLvl
		    && (recreate || !UniqueItemFlag[j] || plr.isMultiplayer())) {
			uok[j] = TRUE;
			numu++;
		}
	}

	if (numu == 0)
		return -1;

	random_(29, 10); /// BUGFIX: unused, last unique in array always gets chosen
	idata = 0;
	while (numu > 0) {
		if (uok[idata])
			numu--;
		if (numu > 0) {
			idata++;
			if (idata == 128)
				idata = 0;
		}
	}

	return idata;
}

void GetUniqueItem(int i, int uid)
{
	UniqueItemFlag[uid] = TRUE;
	SaveItemPower(i, UniqueItemList[uid].UIPower1, UniqueItemList[uid].UIParam1, UniqueItemList[uid].UIParam2, 0, 0, 1);

	if (UniqueItemList[uid].UINumPL > 1)
		SaveItemPower(i, UniqueItemList[uid].UIPower2, UniqueItemList[uid].UIParam3, UniqueItemList[uid].UIParam4, 0, 0, 1);
	if (UniqueItemList[uid].UINumPL > 2)
		SaveItemPower(i, UniqueItemList[uid].UIPower3, UniqueItemList[uid].UIParam5, UniqueItemList[uid].UIParam6, 0, 0, 1);
	if (UniqueItemList[uid].UINumPL > 3)
		SaveItemPower(i, UniqueItemList[uid].UIPower4, UniqueItemList[uid].UIParam7, UniqueItemList[uid].UIParam8, 0, 0, 1);
	if (UniqueItemList[uid].UINumPL > 4)
		SaveItemPower(i, UniqueItemList[uid].UIPower5, UniqueItemList[uid].UIParam9, UniqueItemList[uid].UIParam10, 0, 0, 1);
	if (UniqueItemList[uid].UINumPL > 5)
		SaveItemPower(i, UniqueItemList[uid].UIPower6, UniqueItemList[uid].UIParam11, UniqueItemList[uid].UIParam12, 0, 0, 1);

	strcpy(item[i]._iIName, UniqueItemList[uid].UIName);
	item[i]._iIvalue = UniqueItemList[uid].UIValue;

	if (item[i]._iMiscId == IMISC_UNIQUE)
		item[i]._iSeed = uid;

	item[i]._iUid = uid;
	item[i]._iMagical = ITEM_QUALITY_UNIQUE;
	item[i]._iCreateInfo |= 0x0200;
}

void SpawnUnique(int uid, V2Di pos)
{
	if (numitems >= MAXITEMS) return;

	int ii = itemavail[0];
	GetSuperItemSpace(pos, ii);
	itemavail[0] = itemavail[MAXITEMS - numitems - 1];
	itemactive[numitems] = ii;

	int itype = 0;
	while (AllItemsList[itype].iItemId != UniqueItemList[uid].UIItemId) {
		itype++;
	}

	GetItemAttrs(ii, itype, level.currlevel);
	GetUniqueItem(ii, uid);
	SetupItem(ii);
	numitems++;
}

void ItemRndDur(int ii)
{
	if (item[ii]._iDurability && item[ii]._iDurability != DUR_INDESTRUCTIBLE)
		item[ii]._iDurability = random_(0, item[ii]._iMaxDur >> 1) + (item[ii]._iMaxDur >> 2) + 1;
}

void SetupAllItems(int ii, int idx, int iseed, int lvl, int uper, int onlygood, bool recreate, bool pregen)
{
	int iblvl, uid;

	item[ii]._iSeed = iseed;
	SetRndSeed(iseed);
	GetItemAttrs(ii, idx, lvl >> 1);
	item[ii]._iCreateInfo = lvl;

	if (pregen)
		item[ii]._iCreateInfo = lvl | 0x8000;
	if (onlygood)
		item[ii]._iCreateInfo |= 0x40;

	if (uper == 15)
		item[ii]._iCreateInfo |= 0x80;
	else if (uper == 1)
		item[ii]._iCreateInfo |= 0x0100;

	if (item[ii]._iMiscId != IMISC_UNIQUE) {
		iblvl = -1;
		if (random_(32, 100) <= 10 || random_(33, 100) <= lvl) {
			iblvl = lvl;
		}
		if (iblvl == -1 && item[ii]._iMiscId == IMISC_STAFF) {
			iblvl = lvl;
		}
		if (iblvl == -1 && item[ii]._iMiscId == IMISC_RING) {
			iblvl = lvl;
		}
		if (iblvl == -1 && item[ii]._iMiscId == IMISC_AMULET) {
			iblvl = lvl;
		}
		if (onlygood)
			iblvl = lvl;
		if (uper == 15)
			iblvl = lvl + 4;
		if (iblvl != -1) {
			uid = CheckUnique(ii, iblvl, uper, recreate);
			if (uid == UITYPE_INVALID) {
				GetItemBonus(ii, idx, iblvl >> 1, iblvl, onlygood);
			} else {
				GetUniqueItem(ii, uid);
				item[ii]._iCreateInfo |= 0x0200;
			}
		}
		if (item[ii]._iMagical != ITEM_QUALITY_UNIQUE)
			ItemRndDur(ii);
	} else {
		if (item[ii]._iLoc != ILOC_UNEQUIPABLE) {
			//uid = CheckUnique(ii, iblvl, uper, recreate);
			//if (uid != UITYPE_INVALID) {
			//	GetUniqueItem(ii, uid);
			//}
			GetUniqueItem(ii, iseed); // BUG: the second argument to GetUniqueItem should be uid.
		}
	}
	SetupItem(ii);
}

void SpawnItem(int m, V2Di pos, bool sendmsg)
{
	int ii, onlygood, idx;

	if (monsters[m].data._uniqtype || ((monsters[m].data.MData->mTreasure & 0x8000) && plr.isMultiplayer())) {
		idx = RndUItem(m);
		if (idx < 0) {
			SpawnUnique(-(idx + 1), pos);
			return;
		}
		onlygood = 1;
	} else if (quests[Q_MUSHROOM]._qactive != QUEST_ACTIVE || quests[Q_MUSHROOM]._qvar1 != QS_MUSHGIVEN) {
		idx = RndItem(m);
		if (!idx)
			return;
		if (idx > 0) {
			idx--;
			onlygood = 0;
		} else {
			SpawnUnique(-(idx + 1), pos);
			return;
		}
	} else {
		idx = IDI_BRAIN;
		quests[Q_MUSHROOM]._qvar1 = QS_BRAINSPAWNED;
	}

	if (numitems < MAXITEMS) {
		ii = itemavail[0];
		GetSuperItemSpace(pos, ii);
		itemavail[0] = itemavail[MAXITEMS - numitems - 1];
		itemactive[numitems] = ii;
		if (monsters[m].data._uniqtype) {
			SetupAllItems(ii, idx, GetRndSeed(), monsters[m].data.MData->mLevel, 15, onlygood, FALSE, FALSE);
		} else {
			SetupAllItems(ii, idx, GetRndSeed(), monsters[m].data.MData->mLevel, 1, onlygood, FALSE, FALSE);
		}
		numitems++;
		if (sendmsg)
			NetSendCmdDItem(FALSE, ii);
	}
}

void CreateItem(int uid, V2Di pos)
{
	int ii, idx;

	if (numitems < MAXITEMS) {
		ii = itemavail[0];
		GetSuperItemSpace(pos, ii);
		idx = 0;
		itemavail[0] = itemavail[MAXITEMS - numitems - 1];
		itemactive[numitems] = ii;

		while (AllItemsList[idx].iItemId != UniqueItemList[uid].UIItemId) {
			idx++;
		}

		GetItemAttrs(ii, idx, level.currlevel);
		GetUniqueItem(ii, uid);
		SetupItem(ii);
		item[ii]._iMagical = ITEM_QUALITY_UNIQUE;
		numitems++;
	}
}

void CreateRndItem(V2Di pos, bool onlygood, bool sendmsg, bool delta)
{
	int idx, ii;

	if (onlygood)
		idx = RndUItem(-1);
	else
		idx = RndAllItems();

	if (numitems < MAXITEMS) {
		ii = itemavail[0];
		GetSuperItemSpace(pos, ii);
		itemavail[0] = itemavail[MAXITEMS - numitems - 1];
		itemactive[numitems] = ii;
		SetupAllItems(ii, idx, GetRndSeed(), 2 * level.currlevel, 1, onlygood, FALSE, delta);
		if (sendmsg)
			NetSendCmdDItem(FALSE, ii);
		if (delta)
			DeltaAddItem(ii);
		numitems++;
	}
}

void SetupAllUseful(int ii, int iseed, int lvl)
{
	int idx;

	item[ii]._iSeed = iseed;
	SetRndSeed(iseed);

	if (random_(34, 2))
		idx = IDI_HEAL;
	else
		idx = IDI_MANA;

	if (lvl > 1 && !random_(34, 3))
		idx = IDI_PORTAL;

	GetItemAttrs(ii, idx, lvl);
	item[ii]._iCreateInfo = lvl + 384;
	SetupItem(ii);
}

void CreateRndUseful(int pnum, V2Di pos, bool sendmsg)
{
	int ii;

	if (numitems < MAXITEMS) {
		ii = itemavail[0];
		GetSuperItemSpace(pos, ii);
		itemavail[0] = itemavail[MAXITEMS - numitems - 1];
		itemactive[numitems] = ii;
		SetupAllUseful(ii, GetRndSeed(), level.currlevel);
		if (sendmsg) {
			NetSendCmdDItem(FALSE, ii);
		}
		numitems++;
	}
}

void CreateTypeItem(V2Di pos, bool onlygood, int itype, int imisc, bool sendmsg, bool delta)
{
	int idx, ii;

	if (itype != ITYPE_GOLD)
		idx = RndTypeItems(itype, imisc);
	else
		idx = 0;

	if (numitems < MAXITEMS) {
		ii = itemavail[0];
		GetSuperItemSpace(pos, ii);
		itemavail[0] = itemavail[MAXITEMS - numitems - 1];
		itemactive[numitems] = ii;
		SetupAllItems(ii, idx, GetRndSeed(), 2 * level.currlevel, 1, onlygood, FALSE, delta);
		if (sendmsg) NetSendCmdDItem(FALSE, ii);
		if (delta) DeltaAddItem(ii);
		numitems++;
	}
}

void RecreateItem(int ii, int idx, WORD icreateinfo, int iseed, int ivalue)
{
	int uper, onlygood, recreate;
	bool pregen;

	if (!idx) {
		SetPlrHandItem(&item[ii], IDI_GOLD);
		item[ii]._iSeed = iseed;
		item[ii]._iCreateInfo = icreateinfo;
		item[ii]._ivalue = ivalue;
		if (ivalue >= GOLD_MEDIUM_LIMIT)
			item[ii]._iCurs = ICURS_GOLD_LARGE;
		else if (ivalue <= GOLD_SMALL_LIMIT)
			item[ii]._iCurs = ICURS_GOLD_SMALL;
		else
			item[ii]._iCurs = ICURS_GOLD_MEDIUM;
	} else {
		if (!icreateinfo) {
			SetPlrHandItem(&item[ii], idx);
			SetPlrHandSeed(&item[ii], iseed);
		} else {
			if (icreateinfo & 0x7C00) {
				RecreateTownItem(ii, idx, icreateinfo, iseed, ivalue);
			} else if ((icreateinfo & 0x0180) == 0x0180) {
				SetupAllUseful(ii, iseed, icreateinfo & 0x3F);
			} else {
				uper = 0;
				onlygood = 0;
				recreate = 0;
				pregen = FALSE;
				if (icreateinfo & 0x0100)
					uper = 1;
				if (icreateinfo & 0x80)
					uper = 15;
				if (icreateinfo & 0x40)
					onlygood = 1;
				if (icreateinfo & 0x0200)
					recreate = 1;
				if (icreateinfo & 0x8000)
					pregen = TRUE;
				SetupAllItems(ii, idx, iseed, icreateinfo & 0x3F, uper, onlygood, recreate, pregen);
			}
		}
	}
}

void RecreateEar(int ii, WORD ic, int iseed, int Id, int dur, int mdur, int ch, int mch, int ivalue, int ibuff)
{
	SetPlrHandItem(&item[ii], IDI_EAR);
	tempstr[0] = (ic >> 8) & 0x7F;
	tempstr[1] = ic & 0x7F;
	tempstr[2] = (iseed >> 24) & 0x7F;
	tempstr[3] = (iseed >> 16) & 0x7F;
	tempstr[4] = (iseed >> 8) & 0x7F;
	tempstr[5] = iseed & 0x7F;
	tempstr[6] = Id & 0x7F;
	tempstr[7] = dur & 0x7F;
	tempstr[8] = mdur & 0x7F;
	tempstr[9] = ch & 0x7F;
	tempstr[10] = mch & 0x7F;
	tempstr[11] = (ivalue >> 8) & 0x7F;
	tempstr[12] = (ibuff >> 24) & 0x7F;
	tempstr[13] = (ibuff >> 16) & 0x7F;
	tempstr[14] = (ibuff >> 8) & 0x7F;
	tempstr[15] = ibuff & 0x7F;
	tempstr[16] = '\0';
	sprintf(item[ii]._iName, "Ear of %s", tempstr);
	item[ii]._iCurs = ((ivalue >> 6) & 3) + 19;
	item[ii]._ivalue = ivalue & 0x3F;
	item[ii]._iCreateInfo = ic;
	item[ii]._iSeed = iseed;
}

void SpawnQuestItem(int itemid, V2Di pos, int randarea, int selflag)
{
	bool failed;
	int i, j, tries;

	if (randarea) {
		tries = 0;
		while (1) {
			tries++;
			if (tries > 1000 && randarea > 1)
				randarea--;
			pos.x = random_(0, MAXDUNX);
			pos.y = random_(0, MAXDUNY);
			failed = FALSE;
			for (i = 0; i < randarea && !failed; i++) {
				for (j = 0; j < randarea && !failed; j++) {
					failed = !ItemSpaceOk(i + pos.x, j + pos.y);
				}
			}
			if (!failed)
				break;
		}
	}

	if (numitems < MAXITEMS) {
		i = itemavail[0];
		itemavail[0] = itemavail[MAXITEMS - numitems - 1];
		itemactive[numitems] = i;
		item[i]._i = pos;
		grid.at(pos).setItem(i);
		GetItemAttrs(i, itemid, level.currlevel);
		SetupItem(i);
		item[i]._iPostDraw = TRUE;
		if (selflag) {
			item[i]._iSelFlag = selflag;
			item[i]._iAnimFrame = item[i]._iAnimLen;
			item[i]._iAnimFlag = FALSE;
		}
		numitems++;
	}
}

void SpawnRock()
{
	int i, ii;
	V2Di n;
	int ostand = FALSE;
	for (i = 0; i < nobjects && !ostand; i++) {
		ii = objectactive[i];
		ostand = object[ii]._otype == OBJ_STAND;
	}
	if (ostand) {
		i = itemavail[0];
		itemavail[0] = itemavail[127 - numitems - 1];
		itemactive[numitems] = i;
		n = object[ii]._o;
		item[i]._i = n;
		grid[n.x][item[i]._i.y].setItem(i);
		GetItemAttrs(i, IDI_ROCK, level.currlevel);
		SetupItem(i);
		item[i]._iSelFlag = 2;
		item[i]._iPostDraw = TRUE;
		item[i]._iAnimFrame = 11;
		numitems++;
	}
}

void RespawnItem(int i, bool FlipFlag)
{
	int it = ItemCAnimTbl[item[i]._iCurs];
	item[i]._iAnimData = itemanims[it];
	item[i]._iAnimLen = ItemAnimLs[it];
	item[i]._iAnimWidth = 96;
	item[i]._iAnimWidth2 = 16;
	item[i]._iPostDraw = FALSE;
	item[i]._iRequest = FALSE;
	if (FlipFlag) {
		item[i]._iAnimFrame = 1;
		item[i]._iAnimFlag = TRUE;
		item[i]._iSelFlag = 0;
	} else {
		item[i]._iAnimFrame = item[i]._iAnimLen;
		item[i]._iAnimFlag = FALSE;
		item[i]._iSelFlag = 1;
	}

	if (item[i]._iCurs == ICURS_MAGIC_ROCK) {
		item[i]._iSelFlag = 1;
		PlaySfxLoc(ItemDropSnds[it], item[i]._i);
	}
	if (item[i]._iCurs == ICURS_TAVERN_SIGN)
		item[i]._iSelFlag = 1;
	if (item[i]._iCurs == ICURS_ANVIL_OF_FURY)
		item[i]._iSelFlag = 1;
}

void DeleteItem(int ii, int i)
{
	itemavail[MAXITEMS - numitems] = ii;
	numitems--;
	if (numitems > 0 && i != numitems)
		itemactive[i] = itemactive[numitems];
}

void ItemDoppel()
{
	int idoppelx;
	ItemStruct *i;

	if (plr.isMultiplayer()) {
		for (idoppelx = 16; idoppelx < 96; idoppelx++) {
			if (grid[idoppelx][idoppely].isItem()) {
				i = &item[grid[idoppelx][idoppely].getItem()];
				if (i->_i.x != idoppelx || i->_i.y != idoppely)
					grid[idoppelx][idoppely].clearItem();
			}
		}
		idoppely++;
		if (idoppely == 96)
			idoppely = 16;
	}
}

void ProcessItems()
{
	int i, ii;

	for (i = 0; i < numitems; i++) {
		ii = itemactive[i];
		if (item[ii]._iAnimFlag) {
			item[ii]._iAnimFrame++;
			if (item[ii]._iCurs == ICURS_MAGIC_ROCK) {
				if (item[ii]._iSelFlag == 1 && item[ii]._iAnimFrame == 11)
					item[ii]._iAnimFrame = 1;
				if (item[ii]._iSelFlag == 2 && item[ii]._iAnimFrame == 21)
					item[ii]._iAnimFrame = 11;
			} else {
				if (item[ii]._iAnimFrame == item[ii]._iAnimLen >> 1)
					PlaySfxLoc(ItemDropSnds[ItemCAnimTbl[item[ii]._iCurs]], item[ii]._i);

				if (item[ii]._iAnimFrame >= item[ii]._iAnimLen) {
					item[ii]._iAnimFrame = item[ii]._iAnimLen;
					item[ii]._iAnimFlag = FALSE;
					item[ii]._iSelFlag = 1;
				}
			}
		}
	}
	ItemDoppel();
}

void FreeItemGFX()
{
	int i;

	for (i = 0; i < ITEMTYPES; i++) {
		MemFreeDbg(itemanims[i]);
	}
}

void GetItemFrm(int i)
{
	item[i]._iAnimData = itemanims[ItemCAnimTbl[item[i]._iCurs]];
}

void GetItemStr(int i)
{
	int nGold;

	if (item[i]._itype != ITYPE_GOLD) {
		if (item[i]._iIdentified)
			strcpy(infostr, item[i]._iIName);
		else
			strcpy(infostr, item[i]._iName);

		if (item[i]._iMagical == ITEM_QUALITY_MAGIC)
			infoclr = COL_BLUE;
		if (item[i]._iMagical == ITEM_QUALITY_UNIQUE)
			infoclr = COL_GOLD;
	} else {
		nGold = item[i]._ivalue;
		sprintf(infostr, "%i gold %s", nGold, get_pieces_str(nGold));
	}
}

void CheckIdentify(int pnum, int cii)
{
	ItemStruct *pi;

	if (cii >= NUM_INVLOC)
		pi = &plr[pnum].data.InvList[cii - NUM_INVLOC];
	else
		pi = &plr[pnum].data.InvBody[cii];

	pi->_iIdentified = TRUE;
	CalcPlrInv(pnum, TRUE);

	if (pnum == myplr())
		SetCursor_(CURSOR_HAND);
}

void DoRepair(int pnum, int cii)
{
	ItemStruct *pi;
	Player & p = plr[pnum];
	PlaySfxLoc(IS_REPAIR, p.pos());

	if (cii >= NUM_INVLOC) {
		pi = &p.data.InvList[cii - NUM_INVLOC];
	} else {
		pi = &p.data.InvBody[cii];
	}

	RepairItem(pi, p.data._pLevel);
	CalcPlrInv(pnum, TRUE);

	if (pnum == myplr()) SetCursor_(CURSOR_HAND);
}

void RepairItem(ItemStruct *i, int lvl)
{
	int rep, d;

	if (i->_iDurability == i->_iMaxDur) {
		return;
	}

	if (i->_iMaxDur <= 0) {
		i->_itype = ITYPE_NONE;
		return;
	}

	rep = 0;
	do {
		rep += lvl + random_(37, lvl);
		d = i->_iMaxDur / (lvl + 9);
		if (d < 1)
			d = 1;
		i->_iMaxDur = i->_iMaxDur - d;
		if (!i->_iMaxDur) {
			i->_itype = ITYPE_NONE;
			return;
		}
	} while (rep + i->_iDurability < i->_iMaxDur);

	i->_iDurability += rep;
	if (i->_iDurability > i->_iMaxDur)
		i->_iDurability = i->_iMaxDur;
}

void DoRecharge(int pnum, int cii)
{
	PlayerStruct *p;
	ItemStruct *pi;
	int r;

	p = &plr[pnum].data;
	if (cii >= NUM_INVLOC) {
		pi = &p->InvList[cii - NUM_INVLOC];
	} else {
		pi = &p->InvBody[cii];
	}
	if (pi->_itype == ITYPE_STAFF && pi->_iSpell) {
		r = spelldata[pi->_iSpell].sBookLvl;
		r = random_(38, p->_pLevel / r) + 1;
		RechargeItem(pi, r);
		CalcPlrInv(pnum, TRUE);
	}

	if (pnum == myplr())
		SetCursor_(CURSOR_HAND);
}

void RechargeItem(ItemStruct *i, int r)
{
	while (i->_iCharges != i->_iMaxCharges) {
		i->_iMaxCharges--;
		if (i->_iMaxCharges == 0) {
			break;
		}
		i->_iCharges += r;
		if (i->_iCharges >= i->_iMaxCharges) {
			if (i->_iCharges > i->_iMaxCharges)
				i->_iCharges = i->_iMaxCharges;
			return;
		}
	}
}

void PrintItemOil(char IDidx)
{
	switch (IDidx) {
	case IMISC_FULLHEAL:
		strcpy(tempstr, "fully recover life");
		AddPanelString(tempstr, TRUE);
		break;
	case IMISC_HEAL:
		strcpy(tempstr, "recover partial life");
		AddPanelString(tempstr, TRUE);
		break;
	case IMISC_OLDHEAL:
		strcpy(tempstr, "recover life");
		AddPanelString(tempstr, TRUE);
		break;
	case IMISC_DEADHEAL:
		strcpy(tempstr, "deadly heal");
		AddPanelString(tempstr, TRUE);
		break;
	case IMISC_MANA:
		strcpy(tempstr, "recover mana");
		AddPanelString(tempstr, TRUE);
		break;
	case IMISC_FULLMANA:
		strcpy(tempstr, "fully recover mana");
		AddPanelString(tempstr, TRUE);
		break;
	case IMISC_ELIXSTR:
		strcpy(tempstr, "increase strength");
		AddPanelString(tempstr, TRUE);
		break;
	case IMISC_ELIXMAG:
		strcpy(tempstr, "increase magic");
		AddPanelString(tempstr, TRUE);
		break;
	case IMISC_ELIXDEX:
		strcpy(tempstr, "increase dexterity");
		AddPanelString(tempstr, TRUE);
		break;
	case IMISC_ELIXVIT:
		strcpy(tempstr, "increase vitality");
		AddPanelString(tempstr, TRUE);
		break;
	case IMISC_ELIXWEAK:
		strcpy(tempstr, "decrease strength");
		AddPanelString(tempstr, TRUE);
		break;
	case IMISC_ELIXDIS:
		strcpy(tempstr, "decrease strength");
		AddPanelString(tempstr, TRUE);
		break;
	case IMISC_ELIXCLUM:
		strcpy(tempstr, "decrease dexterity");
		AddPanelString(tempstr, TRUE);
		break;
	case IMISC_ELIXSICK:
		strcpy(tempstr, "decrease vitality");
		AddPanelString(tempstr, TRUE);
		break;
	case IMISC_REJUV:
		strcpy(tempstr, "recover life and mana");
		AddPanelString(tempstr, TRUE);
		break;
	case IMISC_FULLREJUV:
		strcpy(tempstr, "fully recover life and mana");
		AddPanelString(tempstr, TRUE);
		break;
	}
}

void PrintItemPower(char plidx, ItemStruct *x)
{
	switch (plidx) {
	case IPL_TOHIT:
	case IPL_TOHIT_CURSE:
		sprintf(tempstr, "chance to hit : %+i%%", x->_iPLToHit);
		break;
	case IPL_DAMP:
	case IPL_DAMP_CURSE:
		sprintf(tempstr, "%+i%% damage", x->_iPLDam);
		break;
	case IPL_TOHIT_DAMP:
	case IPL_TOHIT_DAMP_CURSE:
		sprintf(tempstr, "to hit: %+i%%, %+i%% damage", x->_iPLToHit, x->_iPLDam);
		break;
	case IPL_ACP:
	case IPL_ACP_CURSE:
		sprintf(tempstr, "%+i%% armor", x->_iPLAC);
		break;
	case IPL_SETAC:
		sprintf(tempstr, "armor class: %i", x->_iAC);
		break;
	case IPL_AC_CURSE:
		sprintf(tempstr, "armor class: %i", x->_iAC);
		break;
	case IPL_FIRERES:
		if (x->_iPLFR < 75)
			sprintf(tempstr, "Resist Fire : %+i%%", x->_iPLFR);
		if (x->_iPLFR >= 75)
			sprintf(tempstr, "Resist Fire : 75%% MAX");
		break;
	case IPL_LIGHTRES:
		if (x->_iPLLR < 75)
			sprintf(tempstr, "Resist Lightning : %+i%%", x->_iPLLR);
		if (x->_iPLLR >= 75)
			sprintf(tempstr, "Resist Lightning : 75%% MAX");
		break;
	case IPL_MAGICRES:
		if (x->_iPLMR < 75)
			sprintf(tempstr, "Resist Magic : %+i%%", x->_iPLMR);
		if (x->_iPLMR >= 75)
			sprintf(tempstr, "Resist Magic : 75%% MAX");
		break;
	case IPL_ALLRES:
		if (x->_iPLFR < 75)
			sprintf(tempstr, "Resist All : %+i%%", x->_iPLFR);
		if (x->_iPLFR >= 75)
			sprintf(tempstr, "Resist All : 75%% MAX");
		break;
	case IPL_SPLLVLADD:
		if (x->_iSplLvlAdd == 1)
			strcpy(tempstr, "spells are increased 1 level");
		if (x->_iSplLvlAdd == 2)
			strcpy(tempstr, "spells are increased 2 levels");
		if (x->_iSplLvlAdd < 1)
			strcpy(tempstr, "spells are decreased 1 level");
		break;
	case IPL_CHARGES:
		strcpy(tempstr, "Extra charges");
		break;
	case IPL_SPELL:
		sprintf(tempstr, "%i %s charges", x->_iMaxCharges, spelldata[x->_iSpell].sNameText);
		break;
	case IPL_FIREDAM:
		sprintf(tempstr, "Fire hit damage: %i-%i", x->_iFMinDam, x->_iFMaxDam);
		break;
	case IPL_LIGHTDAM:
		sprintf(tempstr, "Lightning hit damage: %i-%i", x->_iLMinDam, x->_iLMaxDam);
		break;
	case IPL_STR:
	case IPL_STR_CURSE:
		sprintf(tempstr, "%+i to strength", x->_iPLStr);
		break;
	case IPL_MAG:
	case IPL_MAG_CURSE:
		sprintf(tempstr, "%+i to magic", x->_iPLMag);
		break;
	case IPL_DEX:
	case IPL_DEX_CURSE:
		sprintf(tempstr, "%+i to dexterity", x->_iPLDex);
		break;
	case IPL_VIT:
	case IPL_VIT_CURSE:
		sprintf(tempstr, "%+i to vitality", x->_iPLVit);
		break;
	case IPL_ATTRIBS:
	case IPL_ATTRIBS_CURSE:
		sprintf(tempstr, "%+i to all attributes", x->_iPLStr);
		break;
	case IPL_GETHIT_CURSE:
	case IPL_GETHIT:
		sprintf(tempstr, "%+i damage from enemies", x->_iPLGetHit);
		break;
	case IPL_LIFE:
	case IPL_LIFE_CURSE:
		sprintf(tempstr, "Hit Points : %+i", x->_iPLHP >> 6);
		break;
	case IPL_MANA:
	case IPL_MANA_CURSE:
		sprintf(tempstr, "Mana : %+i", x->_iPLMana >> 6);
		break;
	case IPL_DUR:
		strcpy(tempstr, "high durability");
		break;
	case IPL_DUR_CURSE:
		strcpy(tempstr, "decreased durability");
		break;
	case IPL_INDESTRUCTIBLE:
		strcpy(tempstr, "indestructible");
		break;
	case IPL_LIGHT:
		sprintf(tempstr, "+%i%% light radius", 10 * x->_iPLLight);
		break;
	case IPL_LIGHT_CURSE:
		sprintf(tempstr, "-%i%% light radius", -10 * x->_iPLLight);
		break;
	case IPL_FIRE_ARROWS:
		sprintf(tempstr, "fire arrows damage: %i-%i", x->_iFMinDam, x->_iFMaxDam);
		break;
	case IPL_LIGHT_ARROWS:
		sprintf(tempstr, "lightning arrows damage %i-%i", x->_iLMinDam, x->_iLMaxDam);
		break;
	case IPL_THORNS:
		strcpy(tempstr, "attacker takes 1-3 damage");
		break;
	case IPL_NOMANA:
		strcpy(tempstr, "user loses all mana");
		break;
	case IPL_NOHEALPLR:
		strcpy(tempstr, "you can't heal");
		break;
	case IPL_ABSHALFTRAP:
		strcpy(tempstr, "absorbs half of trap damage");
		break;
	case IPL_KNOCKBACK:
		strcpy(tempstr, "knocks target back");
		break;
	case IPL_3XDAMVDEM:
		strcpy(tempstr, "+200% damage vs. demons");
		break;
	case IPL_ALLRESZERO:
		strcpy(tempstr, "All Resistance equals 0");
		break;
	case IPL_NOHEALMON:
		strcpy(tempstr, "hit monster doesn't heal");
		break;
	case IPL_STEALMANA:
		if (x->_iFlags & ISPL_STEALMANA_3)
			strcpy(tempstr, "hit steals 3% mana");
		if (x->_iFlags & ISPL_STEALMANA_5)
			strcpy(tempstr, "hit steals 5% mana");
		break;
	case IPL_STEALLIFE:
		if (x->_iFlags & ISPL_STEALLIFE_3)
			strcpy(tempstr, "hit steals 3% life");
		if (x->_iFlags & ISPL_STEALLIFE_5)
			strcpy(tempstr, "hit steals 5% life");
		break;
	case IPL_TARGAC:
		strcpy(tempstr, "damages target's armor");
		break;
	case IPL_FASTATTACK:
		if (x->_iFlags & ISPL_QUICKATTACK)
			strcpy(tempstr, "quick attack");
		if (x->_iFlags & ISPL_FASTATTACK)
			strcpy(tempstr, "fast attack");
		if (x->_iFlags & ISPL_FASTERATTACK)
			strcpy(tempstr, "faster attack");
		if (x->_iFlags & ISPL_FASTESTATTACK)
			strcpy(tempstr, "fastest attack");
		break;
	case IPL_FASTRECOVER:
		if (x->_iFlags & ISPL_FASTRECOVER)
			strcpy(tempstr, "fast hit recovery");
		if (x->_iFlags & ISPL_FASTERRECOVER)
			strcpy(tempstr, "faster hit recovery");
		if (x->_iFlags & ISPL_FASTESTRECOVER)
			strcpy(tempstr, "fastest hit recovery");
		break;
	case IPL_FASTBLOCK:
		strcpy(tempstr, "fast block");
		break;
	case IPL_DAMMOD:
		sprintf(tempstr, "adds %i points to damage", x->_iPLDamMod);
		break;
	case IPL_RNDARROWVEL:
		strcpy(tempstr, "fires random speed arrows");
		break;
	case IPL_SETDAM:
		sprintf(tempstr, "unusual item damage");
		break;
	case IPL_SETDUR:
		strcpy(tempstr, "altered durability");
		break;
	case IPL_FASTSWING:
		strcpy(tempstr, "Faster attack swing");
		break;
	case IPL_ONEHAND:
		strcpy(tempstr, "one handed sword");
		break;
	case IPL_DRAINLIFE:
		strcpy(tempstr, "constantly lose hit points");
		break;
	case IPL_RNDSTEALLIFE:
		strcpy(tempstr, "life stealing");
		break;
	case IPL_NOMINSTR:
		strcpy(tempstr, "no strength requirement");
		break;
	case IPL_INFRAVISION:
		strcpy(tempstr, "see with infravision");
		break;
	case IPL_INVCURS:
		strcpy(tempstr, " ");
		break;
	case IPL_ADDACLIFE:
		strcpy(tempstr, "Armor class added to life");
		break;
	case IPL_ADDMANAAC:
		strcpy(tempstr, "10% of mana added to armor");
		break;
	case IPL_FIRERESCLVL:
		if (x->_iPLFR <= 0)
			sprintf(tempstr, " ");
		else if (x->_iPLFR >= 1)
			sprintf(tempstr, "Resist Fire : %+i%%", x->_iPLFR);
		break;
	default:
		strcpy(tempstr, "Another ability (NW)");
		break;
	}
}

void DrawUTextBack()
{
	CelDraw(RIGHT_PANEL_X - SPANEL_WIDTH + 24, SCREEN_Y + 327, pSTextBoxCels, 1, 271);
	trans_rect(RIGHT_PANEL - SPANEL_WIDTH + 27, 28, 265, 297);
}

void PrintUString(V2Di pos, bool cjustflag, char *str, int col)
{
	int len, width, i, k;
	BYTE c;
	V2Di s;
	s = { pos.x + 96, pos.y * 12 + 204 };
	len = strlen(str);
	k = 0;
	if (cjustflag) {
		width = 0;
		for (i = 0; i < len; i++)
			width += fontkern[fontframe[gbFontTransTbl[(BYTE)str[i]]]] + 1;
		if (width < 257)
			k = (257 - width) >> 1;
		s.x += k;
	}

	for (i = 0; i < len; i++) {
		c = fontframe[gbFontTransTbl[(BYTE)str[i]]];
		k += fontkern[c] + 1;
		if (c && k <= 257) {
			PrintChar(s, c, col);
		}
		s.x += fontkern[c] + 1;
	}
}

void DrawULine(int y)
{
	assert(gpBuffer);

	int i;
	BYTE *src, *dst;

	src = &gpBuffer[SCREENXY(26 + RIGHT_PANEL - SPANEL_WIDTH, 25)];
	dst = &gpBuffer[BUFFER_WIDTH * (y * 12 + 198) + 26 + RIGHT_PANEL_X - SPANEL_WIDTH];

	for (i = 0; i < 3; i++, src += BUFFER_WIDTH, dst += BUFFER_WIDTH)
		memcpy(dst, src, 266);
}

void DrawUniqueInfo()
{
	int uid, y;

	if ((!chrflag && !questlog) || SCREEN_WIDTH >= SPANEL_WIDTH * 3) {
		uid = curruitem._iUid;
		DrawUTextBack();
		PrintUString({ 0 + RIGHT_PANEL - SPANEL_WIDTH, 2 }, TRUE, UniqueItemList[uid].UIName, 3);
		DrawULine(5);
		PrintItemPower(UniqueItemList[uid].UIPower1, &curruitem);
		y = 6 - UniqueItemList[uid].UINumPL + 8;
		PrintUString({ 0 + RIGHT_PANEL - SPANEL_WIDTH, y }, TRUE, tempstr, 0);
		if (UniqueItemList[uid].UINumPL > 1) {
			PrintItemPower(UniqueItemList[uid].UIPower2, &curruitem);
			PrintUString({ 0 + RIGHT_PANEL - SPANEL_WIDTH, y + 2 }, TRUE, tempstr, 0);
		}
		if (UniqueItemList[uid].UINumPL > 2) {
			PrintItemPower(UniqueItemList[uid].UIPower3, &curruitem);
			PrintUString({ 0 + RIGHT_PANEL - SPANEL_WIDTH, y + 4 }, TRUE, tempstr, 0);
		}
		if (UniqueItemList[uid].UINumPL > 3) {
			PrintItemPower(UniqueItemList[uid].UIPower4, &curruitem);
			PrintUString({ 0 + RIGHT_PANEL - SPANEL_WIDTH, y + 6 }, TRUE, tempstr, 0);
		}
		if (UniqueItemList[uid].UINumPL > 4) {
			PrintItemPower(UniqueItemList[uid].UIPower5, &curruitem);
			PrintUString({ 0 + RIGHT_PANEL - SPANEL_WIDTH, y + 8 }, TRUE, tempstr, 0);
		}
		if (UniqueItemList[uid].UINumPL > 5) {
			PrintItemPower(UniqueItemList[uid].UIPower6, &curruitem);
			PrintUString({ 0 + RIGHT_PANEL - SPANEL_WIDTH, y + 10 }, TRUE, tempstr, 0);
		}
	}
}

void PrintItemMisc(ItemStruct *x)
{
	if (x->_iMiscId == IMISC_SCROLL) {
		strcpy(tempstr, "Right-click to read");
		AddPanelString(tempstr, TRUE);
	}
	if (x->_iMiscId == IMISC_SCROLLT) {
		strcpy(tempstr, "Right-click to read, then");
		AddPanelString(tempstr, TRUE);
		strcpy(tempstr, "left-click to target");
		AddPanelString(tempstr, TRUE);
	}
	if (x->_iMiscId >= IMISC_USEFIRST && x->_iMiscId <= IMISC_USELAST) {
		PrintItemOil(x->_iMiscId);
		strcpy(tempstr, "Right click to use");
		AddPanelString(tempstr, TRUE);
	}
	if (x->_iMiscId == IMISC_BOOK) {
		strcpy(tempstr, "Right click to read");
		AddPanelString(tempstr, TRUE);
	}
	if (x->_iMiscId == IMISC_MAPOFDOOM) {
		strcpy(tempstr, "Right click to view");
		AddPanelString(tempstr, TRUE);
	}
	if (x->_iMiscId == IMISC_EAR) {
		sprintf(tempstr, "Level : %i", x->_ivalue);
		AddPanelString(tempstr, TRUE);
	}
}

void PrintItemDetails(ItemStruct *x)
{
	if (x->_iClass == ICLASS_WEAPON) {
		if (x->_iMaxDur == DUR_INDESTRUCTIBLE)
			sprintf(tempstr, "damage: %i-%i  Indestructible", x->_iMinDam, x->_iMaxDam);
		else
			sprintf(tempstr, "damage: %i-%i  Dur: %i/%i", x->_iMinDam, x->_iMaxDam, x->_iDurability, x->_iMaxDur);
		AddPanelString(tempstr, TRUE);
	}
	if (x->_iClass == ICLASS_ARMOR) {
		if (x->_iMaxDur == DUR_INDESTRUCTIBLE)
			sprintf(tempstr, "armor: %i  Indestructible", x->_iAC);
		else
			sprintf(tempstr, "armor: %i  Dur: %i/%i", x->_iAC, x->_iDurability, x->_iMaxDur);
		AddPanelString(tempstr, TRUE);
	}
	if (x->_iMiscId == IMISC_STAFF && x->_iMaxCharges) {
		sprintf(tempstr, "dam: %i-%i  Dur: %i/%i", x->_iMinDam, x->_iMaxDam, x->_iDurability, x->_iMaxDur);
		sprintf(tempstr, "Charges: %i/%i", x->_iCharges, x->_iMaxCharges);
		AddPanelString(tempstr, TRUE);
	}
	if (x->_iPrePower != -1) {
		PrintItemPower(x->_iPrePower, x);
		AddPanelString(tempstr, TRUE);
	}
	if (x->_iSufPower != -1) {
		PrintItemPower(x->_iSufPower, x);
		AddPanelString(tempstr, TRUE);
	}
	if (x->_iMagical == ITEM_QUALITY_UNIQUE) {
		AddPanelString("unique item", TRUE);
		uitemflag = TRUE;
		curruitem = *x;
	}
	PrintItemMisc(x);
	if (x->_iMinMag + x->_iMinDex + x->_iMinStr) {
		strcpy(tempstr, "Required:");
		if (x->_iMinStr)
			sprintf(tempstr, "%s %i Str", tempstr, x->_iMinStr);
		if (x->_iMinMag)
			sprintf(tempstr, "%s %i Mag", tempstr, x->_iMinMag);
		if (x->_iMinDex)
			sprintf(tempstr, "%s %i Dex", tempstr, x->_iMinDex);
		AddPanelString(tempstr, TRUE);
	}
	pinfoflag = TRUE;
}

void PrintItemDur(ItemStruct *x)
{
	if (x->_iClass == ICLASS_WEAPON) {
		if (x->_iMaxDur == DUR_INDESTRUCTIBLE)
			sprintf(tempstr, "damage: %i-%i  Indestructible", x->_iMinDam, x->_iMaxDam);
		else
			sprintf(tempstr, "damage: %i-%i  Dur: %i/%i", x->_iMinDam, x->_iMaxDam, x->_iDurability, x->_iMaxDur);
		AddPanelString(tempstr, TRUE);
		if (x->_iMiscId == IMISC_STAFF && x->_iMaxCharges) {
			sprintf(tempstr, "Charges: %i/%i", x->_iCharges, x->_iMaxCharges);
			AddPanelString(tempstr, TRUE);
		}
		if (x->_iMagical != ITEM_QUALITY_NORMAL)
			AddPanelString("Not Identified", TRUE);
	}
	if (x->_iClass == ICLASS_ARMOR) {
		if (x->_iMaxDur == DUR_INDESTRUCTIBLE)
			sprintf(tempstr, "armor: %i  Indestructible", x->_iAC);
		else
			sprintf(tempstr, "armor: %i  Dur: %i/%i", x->_iAC, x->_iDurability, x->_iMaxDur);
		AddPanelString(tempstr, TRUE);
		if (x->_iMagical != ITEM_QUALITY_NORMAL)
			AddPanelString("Not Identified", TRUE);
		if (x->_iMiscId == IMISC_STAFF && x->_iMaxCharges) {
			sprintf(tempstr, "Charges: %i/%i", x->_iCharges, x->_iMaxCharges);
			AddPanelString(tempstr, TRUE);
		}
	}
	if (x->_itype == ITYPE_RING || x->_itype == ITYPE_AMULET)
		AddPanelString("Not Identified", TRUE);
	PrintItemMisc(x);
	if (x->_iMinMag + x->_iMinDex + x->_iMinStr) {
		strcpy(tempstr, "Required:");
		if (x->_iMinStr)
			sprintf(tempstr, "%s %i Str", tempstr, x->_iMinStr);
		if (x->_iMinMag)
			sprintf(tempstr, "%s %i Mag", tempstr, x->_iMinMag);
		if (x->_iMinDex)
			sprintf(tempstr, "%s %i Dex", tempstr, x->_iMinDex);
		AddPanelString(tempstr, TRUE);
	}
	pinfoflag = TRUE;
}

void UseItem(int p, int Mid, int spl)
{
	int l, j;

	switch (Mid) {
	case IMISC_HEAL:
	case IMISC_MEAT:
		j = plr[p].data._pMaxHP >> 8;
		l = ((j >> 1) + random_(39, j)) << 6;
		if (plr[p].data._pClass == PC_WARRIOR)
			l *= 2;
		if (plr[p].data._pClass == PC_ROGUE)
			l += l >> 1;
		plr[p].data._pHitPoints += l;
		if (plr[p].data._pHitPoints > plr[p].data._pMaxHP)
			plr[p].data._pHitPoints = plr[p].data._pMaxHP;
		plr[p].data._pHPBase += l;
		if (plr[p].data._pHPBase > plr[p].data._pMaxHPBase)
			plr[p].data._pHPBase = plr[p].data._pMaxHPBase;
		drawhpflag = TRUE;
		break;
	case IMISC_FULLHEAL:
		plr[p].data._pHitPoints = plr[p].data._pMaxHP;
		plr[p].data._pHPBase = plr[p].data._pMaxHPBase;
		drawhpflag = TRUE;
		break;
	case IMISC_MANA:
		j = plr[p].data._pMaxMana >> 8;
		l = ((j >> 1) + random_(40, j)) << 6;
		if (plr[p].data._pClass == PC_SORCERER)
			l *= 2;
		if (plr[p].data._pClass == PC_ROGUE)
			l += l >> 1;
		if (!(plr[p].data._pIFlags & ISPL_NOMANA)) {
			plr[p].data._pMana += l;
			if (plr[p].data._pMana > plr[p].data._pMaxMana)
				plr[p].data._pMana = plr[p].data._pMaxMana;
			plr[p].data._pManaBase += l;
			if (plr[p].data._pManaBase > plr[p].data._pMaxManaBase)
				plr[p].data._pManaBase = plr[p].data._pMaxManaBase;
			drawmanaflag = TRUE;
		}
		break;
	case IMISC_FULLMANA:
		if (!(plr[p].data._pIFlags & ISPL_NOMANA)) {
			plr[p].data._pMana = plr[p].data._pMaxMana;
			plr[p].data._pManaBase = plr[p].data._pMaxManaBase;
			drawmanaflag = TRUE;
		}
		break;
	case IMISC_ELIXSTR:
		plr[p].ModifyPlrStr(1);
		break;
	case IMISC_ELIXMAG:
		plr[p].ModifyPlrMag(1);
		break;
	case IMISC_ELIXDEX:
		plr[p].ModifyPlrDex(1);
		break;
	case IMISC_ELIXVIT:
		plr[p].ModifyPlrVit(1);
		break;
	case IMISC_REJUV:
		j = plr[p].data._pMaxHP >> 8;
		l = ((j >> 1) + random_(39, j)) << 6;
		if (plr[p].data._pClass == PC_WARRIOR)
			l *= 2;
		if (plr[p].data._pClass == PC_ROGUE)
			l += l >> 1;
		plr[p].data._pHitPoints += l;
		if (plr[p].data._pHitPoints > plr[p].data._pMaxHP)
			plr[p].data._pHitPoints = plr[p].data._pMaxHP;
		plr[p].data._pHPBase += l;
		if (plr[p].data._pHPBase > plr[p].data._pMaxHPBase)
			plr[p].data._pHPBase = plr[p].data._pMaxHPBase;
		drawhpflag = TRUE;
		j = plr[p].data._pMaxMana >> 8;
		l = ((j >> 1) + random_(40, j)) << 6;
		if (plr[p].data._pClass == PC_SORCERER)
			l *= 2;
		if (plr[p].data._pClass == PC_ROGUE)
			l += l >> 1;
		if (!(plr[p].data._pIFlags & ISPL_NOMANA)) {
			plr[p].data._pMana += l;
			if (plr[p].data._pMana > plr[p].data._pMaxMana)
				plr[p].data._pMana = plr[p].data._pMaxMana;
			plr[p].data._pManaBase += l;
			if (plr[p].data._pManaBase > plr[p].data._pMaxManaBase)
				plr[p].data._pManaBase = plr[p].data._pMaxManaBase;
			drawmanaflag = TRUE;
		}
		break;
	case IMISC_FULLREJUV:
		plr[p].data._pHitPoints = plr[p].data._pMaxHP;
		plr[p].data._pHPBase = plr[p].data._pMaxHPBase;
		drawhpflag = TRUE;
		if (!(plr[p].data._pIFlags & ISPL_NOMANA)) {
			plr[p].data._pMana = plr[p].data._pMaxMana;
			plr[p].data._pManaBase = plr[p].data._pMaxManaBase;
			drawmanaflag = TRUE;
		}
		break;
	case IMISC_SCROLL:
		if (spelldata[spl].sTargeted) {
			plr[p].data._pTSpell = spl;
			plr[p].data._pTSplType = RSPLTYPE_INVALID;
			if (p == myplr())
				NewCursor(CURSOR_TELEPORT);
		} else {
			plr[p].ClrPlrPath();
			plr[p].data._pSpell = spl;
			plr[p].data._pSplType = RSPLTYPE_INVALID;
			plr[p].data._pSplFrom = 3;
			plr[p].data.destAction = ACTION_SPELL;
			plr[p].data.destParam1 = cursm.x;
			plr[p].data.destParam2 = cursm.y;
			if (p == myplr() && spl == SPL_NOVA)
				NetSendCmdLoc(TRUE, CMD_NOVA, cursm);
		}
		break;
	case IMISC_SCROLLT:
		if (spelldata[spl].sTargeted) {
			plr[p].data._pTSpell = spl;
			plr[p].data._pTSplType = RSPLTYPE_INVALID;
			if (p == myplr())
				NewCursor(CURSOR_TELEPORT);
		} else {
			plr[p].ClrPlrPath();
			plr[p].data._pSpell = spl;
			plr[p].data._pSplType = RSPLTYPE_INVALID;
			plr[p].data._pSplFrom = 3;
			plr[p].data.destAction = ACTION_SPELL;
			plr[p].data.destParam1 = cursm.x;
			plr[p].data.destParam2 = cursm.y;
		}
		break;
	case IMISC_BOOK:
		plr[p].data._pMemSpells |= (__int64)1 << (spl - 1);
		if (plr[p].data._pSplLvl[spl] < 15)
			plr[p].data._pSplLvl[spl]++;
		plr[p].data._pMana += spelldata[spl].sManaCost << 6;
		if (plr[p].data._pMana > plr[p].data._pMaxMana)
			plr[p].data._pMana = plr[p].data._pMaxMana;
		plr[p].data._pManaBase += spelldata[spl].sManaCost << 6;
		if (plr[p].data._pManaBase > plr[p].data._pMaxManaBase)
			plr[p].data._pManaBase = plr[p].data._pMaxManaBase;
		if (p == myplr())
			CalcPlrBookVals(p);
		drawmanaflag = TRUE;
		break;
	case IMISC_MAPOFDOOM:
		doom_init();
		break;
	case IMISC_SPECELIX:
		plr[p].ModifyPlrStr(3);
		plr[p].ModifyPlrMag(3);
		plr[p].ModifyPlrDex(3);
		plr[p].ModifyPlrVit(3);
		break;
	}
}

bool StoreStatOk(ItemStruct *h)
{
	bool sf;

	sf = TRUE;
	if (myplr().data._pStrength < h->_iMinStr)
		sf = FALSE;
	if (myplr().data._pMagic < h->_iMinMag)
		sf = FALSE;
	if (myplr().data._pDexterity < h->_iMinDex)
		sf = FALSE;

	return sf;
}

bool SmithItemOk(int i)
{
	bool rv;

	rv = TRUE;
	if (AllItemsList[i].itype == ITYPE_MISC)
		rv = FALSE;
	if (AllItemsList[i].itype == ITYPE_GOLD)
		rv = FALSE;
	if (AllItemsList[i].itype == ITYPE_MEAT)
		rv = FALSE;
	if (AllItemsList[i].itype == ITYPE_STAFF)
		rv = FALSE;
	if (AllItemsList[i].itype == ITYPE_RING)
		rv = FALSE;
	if (AllItemsList[i].itype == ITYPE_AMULET)
		rv = FALSE;

	return rv;
}

int RndSmithItem(int lvl)
{
	int i, ri;
	int ril[512];

	ri = 0;
	for (i = 1; AllItemsList[i].iLoc != ILOC_INVALID; i++) {
		if (AllItemsList[i].iRnd && SmithItemOk(i) && lvl >= AllItemsList[i].iMinMLvl) {
			ril[ri] = i;
			ri++;
			if (AllItemsList[i].iRnd == 2) {
				ril[ri] = i;
				ri++;
			}
		}
	}

	return ril[random_(50, ri)] + 1;
}

void BubbleSwapItem(ItemStruct *a, ItemStruct *b)
{
	ItemStruct h;

	h = *a;
	*a = *b;
	*b = h;
}

void SortSmith()
{
	int j, k;
	bool sorted;

	j = 0;
	while (smithitem[j + 1]._itype != ITYPE_NONE) {
		j++;
	}

	sorted = FALSE;
	while (j > 0 && !sorted) {
		sorted = TRUE;
		for (k = 0; k < j; k++) {
			if (smithitem[k].IDidx > smithitem[k + 1].IDidx) {
				BubbleSwapItem(&smithitem[k], &smithitem[k + 1]);
				sorted = FALSE;
			}
		}
		j--;
	}
}

void SpawnSmith(int lvl)
{
	int i, iCnt, idata;

	iCnt = random_(50, SMITH_ITEMS - 10) + 10;
	for (i = 0; i < iCnt; i++) {
		do {
			item[0]._iSeed = GetRndSeed();
			SetRndSeed(item[0]._iSeed);
			idata = RndSmithItem(lvl) - 1;
			GetItemAttrs(0, idata, lvl);
		} while (item[0]._iIvalue > SMITH_MAX_VALUE);
		smithitem[i] = item[0];
		smithitem[i]._iCreateInfo = lvl | 0x400;
		smithitem[i]._iIdentified = TRUE;
		smithitem[i]._iStatFlag = StoreStatOk(&smithitem[i]);
	}
	for (i = iCnt; i < SMITH_ITEMS; i++)
		smithitem[i]._itype = ITYPE_NONE;

	SortSmith();
}

bool PremiumItemOk(int i)
{
	bool rv;

	rv = TRUE;
	if (AllItemsList[i].itype == ITYPE_MISC)
		rv = FALSE;
	if (AllItemsList[i].itype == ITYPE_GOLD)
		rv = FALSE;
	if (AllItemsList[i].itype == ITYPE_MEAT)
		rv = FALSE;
	if (AllItemsList[i].itype == ITYPE_STAFF)
		rv = FALSE;

	if (plr.isMultiplayer()) {
		if (AllItemsList[i].itype == ITYPE_RING)
			rv = FALSE;
		if (AllItemsList[i].itype == ITYPE_AMULET)
			rv = FALSE;
	}

	return rv;
}

int RndPremiumItem(int minlvl, int maxlvl)
{
	int i, ri;
	int ril[512];

	ri = 0;
	for (i = 1; AllItemsList[i].iLoc != ILOC_INVALID; i++) {
		if (AllItemsList[i].iRnd) {
			if (PremiumItemOk(i)) {
				if (AllItemsList[i].iMinMLvl >= minlvl && AllItemsList[i].iMinMLvl <= maxlvl) {
					ril[ri] = i;
					ri++;
				}
			}
		}
	}

	return ril[random_(50, ri)] + 1;
}

void SpawnOnePremium(int i, int plvl)
{
	int itype;
	ItemStruct holditem;

	holditem = item[0];
	if (plvl > 30)
		plvl = 30;
	if (plvl < 1)
		plvl = 1;
	do {
		item[0]._iSeed = GetRndSeed();
		SetRndSeed(item[0]._iSeed);
		itype = RndPremiumItem(plvl >> 2, plvl) - 1;
		GetItemAttrs(0, itype, plvl);
		GetItemBonus(0, itype, plvl >> 1, plvl, TRUE);
	} while (item[0]._iIvalue > SMITH_MAX_PREMIUM_VALUE);
	premiumitem[i] = item[0];
	premiumitem[i]._iCreateInfo = plvl | 0x800;
	premiumitem[i]._iIdentified = TRUE;
	premiumitem[i]._iStatFlag = StoreStatOk(&premiumitem[i]);
	item[0] = holditem;
}

void SpawnPremium(int lvl)
{
	int i;

	if (numpremium < SMITH_PREMIUM_ITEMS) {
		for (i = 0; i < SMITH_PREMIUM_ITEMS; i++) {
			if (premiumitem[i]._itype == ITYPE_NONE)
				SpawnOnePremium(i, premiumlevel + premiumlvladd[i]);
		}
		numpremium = SMITH_PREMIUM_ITEMS;
	}
	while (premiumlevel < lvl) {
		premiumlevel++;
		premiumitem[0] = premiumitem[2];
		premiumitem[1] = premiumitem[3];
		premiumitem[2] = premiumitem[4];
		SpawnOnePremium(3, premiumlevel + premiumlvladd[3]);
		premiumitem[4] = premiumitem[5];
		SpawnOnePremium(5, premiumlevel + premiumlvladd[5]);
	}
}

bool WitchItemOk(int i)
{
	bool rv;

	rv = FALSE;
	if (AllItemsList[i].itype == ITYPE_MISC)
		rv = TRUE;
	if (AllItemsList[i].itype == ITYPE_STAFF)
		rv = TRUE;
	if (AllItemsList[i].iMiscId == IMISC_MANA)
		rv = FALSE;
	if (AllItemsList[i].iMiscId == IMISC_FULLMANA)
		rv = FALSE;
	if (AllItemsList[i].iSpell == SPL_TOWN)
		rv = FALSE;
	if (AllItemsList[i].iMiscId == IMISC_FULLHEAL)
		rv = FALSE;
	if (AllItemsList[i].iMiscId == IMISC_HEAL)
		rv = FALSE;
	if (AllItemsList[i].iSpell == SPL_RESURRECT && plr.isSingleplayer())
		rv = FALSE;
	if (AllItemsList[i].iSpell == SPL_HEALOTHER && plr.isSingleplayer())
		rv = FALSE;

	return rv;
}

int RndWitchItem(int lvl)
{
	int i, ri;
	int ril[512];

	ri = 0;
	for (i = 1; AllItemsList[i].iLoc != ILOC_INVALID; i++) {
		if (AllItemsList[i].iRnd && WitchItemOk(i) && lvl >= AllItemsList[i].iMinMLvl) {
			ril[ri] = i;
			ri++;
		}
	}

	return ril[random_(51, ri)] + 1;
}

void SortWitch()
{
	int j, k;
	bool sorted;

	j = 3;
	while (witchitem[j + 1]._itype != ITYPE_NONE) {
		j++;
	}

	sorted = FALSE;
	while (j > 3 && !sorted) {
		sorted = TRUE;
		for (k = 3; k < j; k++) {
			if (witchitem[k].IDidx > witchitem[k + 1].IDidx) {
				BubbleSwapItem(&witchitem[k], &witchitem[k + 1]);
				sorted = FALSE;
			}
		}
		j--;
	}
}

void WitchBookLevel(int ii)
{
	int slvl;

	if (witchitem[ii]._iMiscId == IMISC_BOOK) {
		witchitem[ii]._iMinMag = spelldata[witchitem[ii]._iSpell].sMinInt;
		slvl = myplr().data._pSplLvl[witchitem[ii]._iSpell];
		while (slvl) {
			witchitem[ii]._iMinMag += 20 * witchitem[ii]._iMinMag / 100;
			slvl--;
			if (witchitem[ii]._iMinMag + 20 * witchitem[ii]._iMinMag / 100 > 255) {
				witchitem[ii]._iMinMag = 255;
				slvl = 0;
			}
		}
	}
}

void SpawnWitch(int lvl)
{
	int i, iCnt;
	int idata, maxlvl;

	GetItemAttrs(0, IDI_MANA, 1);
	witchitem[0] = item[0];
	witchitem[0]._iCreateInfo = lvl;
	witchitem[0]._iStatFlag = TRUE;
	GetItemAttrs(0, IDI_FULLMANA, 1);
	witchitem[1] = item[0];
	witchitem[1]._iCreateInfo = lvl;
	witchitem[1]._iStatFlag = TRUE;
	GetItemAttrs(0, IDI_PORTAL, 1);
	witchitem[2] = item[0];
	witchitem[2]._iCreateInfo = lvl;
	witchitem[2]._iStatFlag = TRUE;
	iCnt = random_(51, 8) + 10;

	for (i = 3; i < iCnt; i++) {
		do {
			item[0]._iSeed = GetRndSeed();
			SetRndSeed(item[0]._iSeed);
			idata = RndWitchItem(lvl) - 1;
			GetItemAttrs(0, idata, lvl);
			maxlvl = -1;
			if (random_(51, 100) <= 5)
				maxlvl = 2 * lvl;
			if (maxlvl == -1 && item[0]._iMiscId == IMISC_STAFF)
				maxlvl = 2 * lvl;
			if (maxlvl != -1)
				GetItemBonus(0, idata, maxlvl >> 1, maxlvl, TRUE);
		} while (item[0]._iIvalue > 140000);
		witchitem[i] = item[0];
		witchitem[i]._iCreateInfo = lvl | 0x2000;
		witchitem[i]._iIdentified = TRUE;
		WitchBookLevel(i);
		witchitem[i]._iStatFlag = StoreStatOk(&witchitem[i]);
	}

	for (i = iCnt; i < 20; i++)
		witchitem[i]._itype = ITYPE_NONE;

	SortWitch();
}

int RndBoyItem(int lvl)
{
	int i, ri;
	int ril[512];

	ri = 0;
	for (i = 1; AllItemsList[i].iLoc != ILOC_INVALID; i++) {
		if (AllItemsList[i].iRnd && PremiumItemOk(i) && lvl >= AllItemsList[i].iMinMLvl) {
			ril[ri] = i;
			ri++;
		}
	}

	return ril[random_(49, ri)] + 1;
}

void SpawnBoy(int lvl)
{
	int itype;

	if (boylevel<lvl>> 1 || boyitem._itype == ITYPE_NONE) {
		do {
			item[0]._iSeed = GetRndSeed();
			SetRndSeed(item[0]._iSeed);
			itype = RndBoyItem(lvl) - 1;
			GetItemAttrs(0, itype, lvl);
			GetItemBonus(0, itype, lvl, 2 * lvl, TRUE);
		} while (item[0]._iIvalue > 90000);
		boyitem = item[0];
		boyitem._iCreateInfo = lvl | 0x1000;
		boyitem._iIdentified = TRUE;
		boyitem._iStatFlag = StoreStatOk(&boyitem);
		boylevel = lvl >> 1;
	}
}

bool HealerItemOk(int i)
{
	bool result;

	result = FALSE;
	if (AllItemsList[i].itype != ITYPE_MISC)
		return FALSE;

	if (AllItemsList[i].iMiscId == IMISC_SCROLL && AllItemsList[i].iSpell == SPL_HEAL)
		result = TRUE;
	if (AllItemsList[i].iMiscId == IMISC_SCROLLT && AllItemsList[i].iSpell == SPL_RESURRECT && plr.isMultiplayer())
		result = FALSE;
	if (AllItemsList[i].iMiscId == IMISC_SCROLLT && AllItemsList[i].iSpell == SPL_HEALOTHER && plr.isMultiplayer())
		result = TRUE;

	if (plr.isSingleplayer()) {
		if (AllItemsList[i].iMiscId == IMISC_ELIXSTR)
			result = TRUE;
		if (AllItemsList[i].iMiscId == IMISC_ELIXMAG)
			result = TRUE;
		if (AllItemsList[i].iMiscId == IMISC_ELIXDEX)
			result = TRUE;
		if (AllItemsList[i].iMiscId == IMISC_ELIXVIT)
			result = TRUE;
	}

	if (AllItemsList[i].iMiscId == IMISC_FULLHEAL) // BUGFIX this is a duplicate with the wrong case
		result = TRUE;

	if (AllItemsList[i].iMiscId == IMISC_REJUV)
		result = TRUE;
	if (AllItemsList[i].iMiscId == IMISC_FULLREJUV)
		result = TRUE;
	if (AllItemsList[i].iMiscId == IMISC_HEAL)
		result = FALSE;
	if (AllItemsList[i].iMiscId == IMISC_FULLHEAL)
		result = FALSE;
	if (AllItemsList[i].iMiscId == IMISC_MANA)
		result = FALSE;
	if (AllItemsList[i].iMiscId == IMISC_FULLMANA)
		result = FALSE;

	return result;
}

int RndHealerItem(int lvl)
{
	int i, ri;
	int ril[512];

	ri = 0;
	for (i = 1; AllItemsList[i].iLoc != ILOC_INVALID; i++) {
		if (AllItemsList[i].iRnd && HealerItemOk(i) && lvl >= AllItemsList[i].iMinMLvl) {
			ril[ri] = i;
			ri++;
		}
	}

	return ril[random_(50, ri)] + 1;
}

void SortHealer()
{
	int j, k;
	bool sorted;

	j = 2;
	while (healitem[j + 1]._itype != ITYPE_NONE) {
		j++;
	}

	sorted = FALSE;
	while (j > 2 && !sorted) {
		sorted = TRUE;
		for (k = 2; k < j; k++) {
			if (healitem[k].IDidx > healitem[k + 1].IDidx) {
				BubbleSwapItem(&healitem[k], &healitem[k + 1]);
				sorted = FALSE;
			}
		}
		j--;
	}
}

void SpawnHealer(int lvl)
{
	int i, nsi, srnd, itype;

	GetItemAttrs(0, IDI_HEAL, 1);
	healitem[0] = item[0];
	healitem[0]._iCreateInfo = lvl;
	healitem[0]._iStatFlag = TRUE;

	GetItemAttrs(0, IDI_FULLHEAL, 1);
	healitem[1] = item[0];
	healitem[1]._iCreateInfo = lvl;
	healitem[1]._iStatFlag = TRUE;

	if (plr.isMultiplayer()) {
		GetItemAttrs(0, IDI_RESURRECT, 1);
		healitem[2] = item[0];
		healitem[2]._iCreateInfo = lvl;
		healitem[2]._iStatFlag = TRUE;

		srnd = 3;
	} else {
		srnd = 2;
	}
	nsi = random_(50, 8) + 10;
	for (i = srnd; i < nsi; i++) {
		item[0]._iSeed = GetRndSeed();
		SetRndSeed(item[0]._iSeed);
		itype = RndHealerItem(lvl) - 1;
		GetItemAttrs(0, itype, lvl);
		healitem[i] = item[0];
		healitem[i]._iCreateInfo = lvl | 0x4000;
		healitem[i]._iIdentified = TRUE;
		healitem[i]._iStatFlag = StoreStatOk(&healitem[i]);
	}
	for (i = nsi; i < 20; i++) {
		healitem[i]._itype = ITYPE_NONE;
	}
	SortHealer();
}

void SpawnStoreGold()
{
	GetItemAttrs(0, IDI_GOLD, 1);
	golditem = item[0];
	golditem._iStatFlag = TRUE;
}

void RecreateSmithItem(int ii, int idx, int lvl, int iseed)
{
	int itype;

	SetRndSeed(iseed);
	itype = RndSmithItem(lvl) - 1;
	GetItemAttrs(ii, itype, lvl);

	item[ii]._iSeed = iseed;
	item[ii]._iCreateInfo = lvl | 0x400;
	item[ii]._iIdentified = TRUE;
}

void RecreatePremiumItem(int ii, int idx, int plvl, int iseed)
{
	int itype;

	SetRndSeed(iseed);
	itype = RndPremiumItem(plvl >> 2, plvl) - 1;
	GetItemAttrs(ii, itype, plvl);
	GetItemBonus(ii, itype, plvl >> 1, plvl, TRUE);

	item[ii]._iSeed = iseed;
	item[ii]._iCreateInfo = plvl | 0x800;
	item[ii]._iIdentified = TRUE;
}

void RecreateBoyItem(int ii, int idx, int lvl, int iseed)
{
	int itype;

	SetRndSeed(iseed);
	itype = RndBoyItem(lvl) - 1;
	GetItemAttrs(ii, itype, lvl);
	GetItemBonus(ii, itype, lvl, 2 * lvl, TRUE);
	item[ii]._iSeed = iseed;
	item[ii]._iCreateInfo = lvl | 0x1000;
	item[ii]._iIdentified = TRUE;
}

void RecreateWitchItem(int ii, int idx, int lvl, int iseed)
{
	int iblvl, itype;

	if (idx == IDI_MANA || idx == IDI_FULLMANA || idx == IDI_PORTAL) {
		GetItemAttrs(ii, idx, lvl);
	} else {
		SetRndSeed(iseed);
		itype = RndWitchItem(lvl) - 1;
		GetItemAttrs(ii, itype, lvl);
		iblvl = -1;
		if (random_(51, 100) <= 5)
			iblvl = 2 * lvl;
		if (iblvl == -1 && item[ii]._iMiscId == IMISC_STAFF)
			iblvl = 2 * lvl;
		if (iblvl != -1)
			GetItemBonus(ii, itype, iblvl >> 1, iblvl, TRUE);
	}

	item[ii]._iSeed = iseed;
	item[ii]._iCreateInfo = lvl | 0x2000;
	item[ii]._iIdentified = TRUE;
}

void RecreateHealerItem(int ii, int idx, int lvl, int iseed)
{
	int itype;

	if (idx == IDI_HEAL || idx == IDI_FULLHEAL || idx == IDI_RESURRECT) {
		GetItemAttrs(ii, idx, lvl);
	} else {
		SetRndSeed(iseed);
		itype = RndHealerItem(lvl) - 1;
		GetItemAttrs(ii, itype, lvl);
	}

	item[ii]._iSeed = iseed;
	item[ii]._iCreateInfo = lvl | 0x4000;
	item[ii]._iIdentified = TRUE;
}

void RecreateTownItem(int ii, int idx, WORD icreateinfo, int iseed, int ivalue)
{
	if (icreateinfo & 0x400)
		RecreateSmithItem(ii, idx, icreateinfo & 0x3F, iseed);
	else if (icreateinfo & 0x800)
		RecreatePremiumItem(ii, idx, icreateinfo & 0x3F, iseed);
	else if (icreateinfo & 0x1000)
		RecreateBoyItem(ii, idx, icreateinfo & 0x3F, iseed);
	else if (icreateinfo & 0x2000)
		RecreateWitchItem(ii, idx, icreateinfo & 0x3F, iseed);
	else if (icreateinfo & 0x4000)
		RecreateHealerItem(ii, idx, icreateinfo & 0x3F, iseed);
}

void RecalcStoreStats()
{
	int i;

	for (i = 0; i < SMITH_ITEMS; i++) {
		if (smithitem[i]._itype != ITYPE_NONE) {
			smithitem[i]._iStatFlag = StoreStatOk(&smithitem[i]);
		}
	}
	for (i = 0; i < SMITH_PREMIUM_ITEMS; i++) {
		if (premiumitem[i]._itype != ITYPE_NONE) {
			premiumitem[i]._iStatFlag = StoreStatOk(&premiumitem[i]);
		}
	}
	for (i = 0; i < 20; i++) {
		if (witchitem[i]._itype != ITYPE_NONE) {
			witchitem[i]._iStatFlag = StoreStatOk(&witchitem[i]);
		}
	}
	for (i = 0; i < 20; i++) {
		if (healitem[i]._itype != ITYPE_NONE) {
			healitem[i]._iStatFlag = StoreStatOk(&healitem[i]);
		}
	}
	boyitem._iStatFlag = StoreStatOk(&boyitem);
}

int ItemNoFlippy()
{
	int r;

	r = itemactive[numitems - 1];
	item[r]._iAnimFrame = item[r]._iAnimLen;
	item[r]._iAnimFlag = FALSE;
	item[r]._iSelFlag = 1;

	return r;
}

void CreateSpellBook(V2Di pos, int ispell, bool sendmsg, bool delta)
{
	int ii, idx;
	bool done;

	done = FALSE;
	idx = RndTypeItems(ITYPE_MISC, IMISC_BOOK);
	if (numitems < MAXITEMS) {
		ii = itemavail[0];
		GetSuperItemSpace(pos, ii);
		itemavail[0] = itemavail[MAXITEMS - numitems - 1];
		itemactive[numitems] = ii;
		while (!done) {
			SetupAllItems(ii, idx, GetRndSeed(), 2 * level.currlevel, 1, TRUE, FALSE, delta);
			if (item[ii]._iMiscId == IMISC_BOOK && item[ii]._iSpell == ispell)
				done = TRUE;
		}
		if (sendmsg)
			NetSendCmdDItem(FALSE, ii);
		if (delta)
			DeltaAddItem(ii);
		numitems++;
	}
}

void CreateMagicArmor(V2Di pos, int imisc, int icurs, bool sendmsg, bool delta)
{
	int ii, idx;
	bool done;

	done = FALSE;
	if (numitems < MAXITEMS) {
		ii = itemavail[0];
		GetSuperItemSpace(pos, ii);
		itemavail[0] = itemavail[MAXITEMS - numitems - 1];
		itemactive[numitems] = ii;
		idx = RndTypeItems(imisc, IMISC_NONE);
		while (!done) {
			SetupAllItems(ii, idx, GetRndSeed(), 2 * level.currlevel, 1, TRUE, FALSE, delta);
			if (item[ii]._iCurs == icurs)
				done = TRUE;
			else
				idx = RndTypeItems(imisc, IMISC_NONE);
		}
		if (sendmsg)
			NetSendCmdDItem(FALSE, ii);
		if (delta)
			DeltaAddItem(ii);
		numitems++;
	}
}

void CreateMagicWeapon(V2Di pos, int imisc, int icurs, bool sendmsg, bool delta)
{
	int ii, idx;
	bool done;

	done = FALSE;
	if (numitems < MAXITEMS) {
		ii = itemavail[0];
		GetSuperItemSpace(pos, ii);
		itemavail[0] = itemavail[MAXITEMS - numitems - 1];
		itemactive[numitems] = ii;
		idx = RndTypeItems(imisc, IMISC_NONE);
		while (!done) {
			SetupAllItems(ii, idx, GetRndSeed(), 2 * level.currlevel, 1, TRUE, FALSE, delta);
			if (item[ii]._iCurs == icurs)
				done = TRUE;
			else
				idx = RndTypeItems(imisc, IMISC_NONE);
		}
		if (sendmsg)
			NetSendCmdDItem(FALSE, ii);
		if (delta)
			DeltaAddItem(ii);
		numitems++;
	}
}

bool GetItemRecord(int nSeed, WORD wCI, int nIndex)
{
	int i;
	DWORD dwTicks;

	dwTicks = SDL_GetTicks();

	for (i = 0; i < gnNumGetRecords; i++) {
		if (dwTicks - itemrecord[i].dwTimestamp > 6000) {
			NextItemRecord(i);
			i--;
		} else if (nSeed == itemrecord[i].nSeed && wCI == itemrecord[i].wCI && nIndex == itemrecord[i].nIndex) {
			return FALSE;
		}
	}

	return TRUE;
}

void NextItemRecord(int i)
{
	gnNumGetRecords--;

	if (gnNumGetRecords == 0) {
		return;
	}

	itemrecord[i].dwTimestamp = itemrecord[gnNumGetRecords].dwTimestamp;
	itemrecord[i].nSeed = itemrecord[gnNumGetRecords].nSeed;
	itemrecord[i].wCI = itemrecord[gnNumGetRecords].wCI;
	itemrecord[i].nIndex = itemrecord[gnNumGetRecords].nIndex;
}

void SetItemRecord(int nSeed, WORD wCI, int nIndex)
{
	DWORD dwTicks;

	dwTicks = SDL_GetTicks();

	if (gnNumGetRecords == MAXITEMS) {
		return;
	}

	itemrecord[gnNumGetRecords].dwTimestamp = dwTicks;
	itemrecord[gnNumGetRecords].nSeed = nSeed;
	itemrecord[gnNumGetRecords].wCI = wCI;
	itemrecord[gnNumGetRecords].nIndex = nIndex;
	gnNumGetRecords++;
}

void PutItemRecord(int nSeed, WORD wCI, int nIndex)
{
	int i;
	DWORD dwTicks;

	dwTicks = SDL_GetTicks();

	for (i = 0; i < gnNumGetRecords; i++) {
		if (dwTicks - itemrecord[i].dwTimestamp > 6000) {
			NextItemRecord(i);
			i--;
		} else if (nSeed == itemrecord[i].nSeed && wCI == itemrecord[i].wCI && nIndex == itemrecord[i].nIndex) {
			NextItemRecord(i);
			break;
		}
	}
}

DEVILUTION_END_NAMESPACE
