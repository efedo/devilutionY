/**
 * @file items.cpp
 *
 * Implementation of item functionality.
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

Item::Item()
    : Item(0)
{}

Item::Item(Player *new_owner)
    : Item(rand64(), new_owner)
{}

Item::Item(ItemID nid)
    : Item(nid, 0)
{}

Item::Item(ItemID nid, Player *new_owner)
    : _owner(new_owner)
    , id(nid)
{
	_itype = ITYPE_MISC;
	_i = { 0, 0 };
	_iAnimFlag = FALSE;
	_iSelFlag = 0;
	_iIdentified = FALSE;
	_iPostDraw = FALSE;
	items.addItem(id, this);
}

Item::~Item()
{
	items.removeItem(id);
}

Player &Item::owner()
{
	assert(_owner);
	return *_owner;
}

bool Item::GetItemSpace(V2Di pos)
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
	if (!savail)
		return FALSE;

	n = { 0, 0 };
	while (rs > 0) {
		if (itemhold[n.x][n.y])
			rs--;
		if (rs > 0) {
			n.x++;
			if (n.x == 3) {
				n.x = 0;
				n.y++;
				if (n.y == 3)
					n.y = 0;
			}
		}
	}
	n += { pos.x - 1, pos.y - 1 };
	_i = n;
	grid.at(n).setItem(id);

	return TRUE;
}

void Item::GetSuperItemSpace(V2Di pos)
{
	int xx, yy;
	int i, j, k;

	if (!GetItemSpace(pos)) {
		for (k = 2; k < 50; k++) {
			for (j = -k; j <= k; j++) {
				yy = pos.y + j;
				for (i = -k; i <= k; i++) {
					xx = i + pos.x;
					if (ItemSpaceOk(xx, yy)) {
						_i = { xx, yy };
						grid[xx][yy].setItem(id);
						return;
					}
				}
			}
		}
	}
}

void Item::CalcItemValue()
{
	int v = _iVMult1 + _iVMult2;
	if (v > 0) {
		v *= _ivalue;
	}
	if (v < 0) {
		v = _ivalue / v;
	}
	v = _iVAdd1 + _iVAdd2 + v;
	if (v <= 0) {
		v = 1;
	}
	_iIvalue = v;
}

void Item::GetBookSpell(int lvl)
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
	strcat(_iName, spelldata[bs].sNameText);
	strcat(_iIName, spelldata[bs].sNameText);
	_iSpell = bs;
	_iMinMag = spelldata[bs].sMinInt;
	_ivalue += spelldata[bs].sBookCost;
	_iIvalue += spelldata[bs].sBookCost;
	if (spelldata[bs].sType == STYPE_FIRE)
		_iCurs = ICURS_BOOK_RED;
	if (spelldata[bs].sType == STYPE_LIGHTNING)
		_iCurs = ICURS_BOOK_BLUE;
	if (spelldata[bs].sType == STYPE_MAGIC)
		_iCurs = ICURS_BOOK_GREY;
}

void Item::GetStaffPower(int lvl, int bs, bool onlygood)
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
			sprintf(istr, "%s %s", PL_Prefix[preidx].PLName, _iIName);
			strcpy(_iIName, istr);
			_iMagical = ITEM_QUALITY_MAGIC;
			SaveItemPower(
			    PL_Prefix[preidx].PLPower,
			    PL_Prefix[preidx].PLParam1,
			    PL_Prefix[preidx].PLParam2,
			    PL_Prefix[preidx].PLMinVal,
			    PL_Prefix[preidx].PLMaxVal,
			    PL_Prefix[preidx].PLMultVal);
			_iPrePower = PL_Prefix[preidx].PLPower;
		}
	}
	if (!control_WriteStringToBuffer((uint8_t *)_iIName)) {
		strcpy(_iIName, AllItemsList[IDidx].iSName);
		if (preidx != -1) {
			sprintf(istr, "%s %s", PL_Prefix[preidx].PLName, _iIName);
			strcpy(_iIName, istr);
		}
		sprintf(istr, "%s of %s", _iIName, spelldata[bs].sNameText);
		strcpy(_iIName, istr);
		if (_iMagical == ITEM_QUALITY_NORMAL)
			strcpy(_iName, _iIName);
	}
	CalcItemValue();
}

void Item::GetStaffSpell(int lvl, bool onlygood)
{
	int l, rv, s, minc, maxc, v, bs;
	char istr[64];

	if (!random_(17, 4)) {
		GetItemPower(lvl >> 1, lvl, PLT_STAFF, onlygood);
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
		sprintf(istr, "%s of %s", _iName, spelldata[bs].sNameText);
		if (!control_WriteStringToBuffer((uint8_t *)istr))
			sprintf(istr, "Staff of %s", spelldata[bs].sNameText);
		strcpy(_iName, istr);
		strcpy(_iIName, istr);

		minc = spelldata[bs].sStaffMin;
		maxc = spelldata[bs].sStaffMax - minc + 1;
		_iSpell = bs;
		_iCharges = minc + random_(19, maxc);
		_iMaxCharges = _iCharges;

		_iMinMag = spelldata[bs].sMinInt;
		v = _iCharges * spelldata[bs].sStaffCost / 5;
		_ivalue += v;
		_iIvalue += v;
		GetStaffPower(lvl, bs, onlygood);
	}
}

void Item::GetItemAttrs(int idata, int curlvl)
{
	int rndv;

	_itype = AllItemsList[idata].itype;
	_iCurs = AllItemsList[idata].iCurs;
	strcpy(_iName, AllItemsList[idata].iName);
	strcpy(_iIName, AllItemsList[idata].iName);
	_iLoc = AllItemsList[idata].iLoc;
	_iClass = AllItemsList[idata].iClass;
	_iMinDam = AllItemsList[idata].iMinDam;
	_iMaxDam = AllItemsList[idata].iMaxDam;
	_iAC = AllItemsList[idata].iMinAC + random_(20, AllItemsList[idata].iMaxAC - AllItemsList[idata].iMinAC + 1);
	_iFlags = AllItemsList[idata].iFlags;
	_iMiscId = AllItemsList[idata].iMiscId;
	_iSpell = AllItemsList[idata].iSpell;
	_iMagical = ITEM_QUALITY_NORMAL;
	_ivalue = AllItemsList[idata].iValue;
	_iIvalue = AllItemsList[idata].iValue;
	_iVAdd1 = 0;
	_iVMult1 = 0;
	_iVAdd2 = 0;
	_iVMult2 = 0;
	_iPLDam = 0;
	_iPLToHit = 0;
	_iPLAC = 0;
	_iPLStr = 0;
	_iPLMag = 0;
	_iPLDex = 0;
	_iPLVit = 0;
	_iCharges = 0;
	_iMaxCharges = 0;
	_iDurability = AllItemsList[idata].iDurability;
	_iMaxDur = AllItemsList[idata].iDurability;
	_iMinStr = AllItemsList[idata].iMinStr;
	_iMinMag = AllItemsList[idata].iMinMag;
	_iMinDex = AllItemsList[idata].iMinDex;
	_iPLFR = 0;
	_iPLLR = 0;
	_iPLMR = 0;
	IDidx = idata;
	_iPLDamMod = 0;
	_iPLGetHit = 0;
	_iPLLight = 0;
	_iSplLvlAdd = 0;
	_iRequest = FALSE;
	_iFMinDam = 0;
	_iFMaxDam = 0;
	_iLMinDam = 0;
	_iLMaxDam = 0;
	_iPLEnAc = 0;
	_iPLMana = 0;
	_iPLHP = 0;
	_iPrePower = -1;
	_iSufPower = -1;

	if (_iMiscId == IMISC_BOOK)
		GetBookSpell(curlvl);

	if (_itype == ITYPE_GOLD) {
		if (gnDifficulty == DIFF_NORMAL)
			rndv = 5 * lvl.currlevel + random_(21, 10 * lvl.currlevel);
		if (gnDifficulty == DIFF_NIGHTMARE)
			rndv = 5 * (lvl.currlevel + 16) + random_(21, 10 * (lvl.currlevel + 16));
		if (gnDifficulty == DIFF_HELL)
			rndv = 5 * (lvl.currlevel + 32) + random_(21, 10 * (lvl.currlevel + 32));
		if (lvl.type() == DunType::hell)
			rndv += rndv >> 3;
		if (rndv > GOLD_MAX_LIMIT)
			rndv = GOLD_MAX_LIMIT;

		_ivalue = rndv;

		if (rndv >= GOLD_MEDIUM_LIMIT)
			_iCurs = ICURS_GOLD_LARGE;
		else
			_iCurs = (rndv > GOLD_SMALL_LIMIT) + 4;
	}
}


void Item::SaveItemPower(int power, int param1, int param2, int minval, int maxval, int multval)
{
	int r, r2;

	r = RndPL(param1, param2);
	switch (power) {
	case IPL_TOHIT:
		_iPLToHit += r;
		break;
	case IPL_TOHIT_CURSE:
		_iPLToHit -= r;
		break;
	case IPL_DAMP:
		_iPLDam += r;
		break;
	case IPL_DAMP_CURSE:
		_iPLDam -= r;
		break;
	case IPL_TOHIT_DAMP:
		r = RndPL(param1, param2);
		_iPLDam += r;
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
		_iPLToHit += r2;
		break;
	case IPL_TOHIT_DAMP_CURSE:
		_iPLDam -= r;
		if (param1 == 25)
			r2 = RndPL(1, 5);
		if (param1 == 50)
			r2 = RndPL(6, 10);
		_iPLToHit -= r2;
		break;
	case IPL_ACP:
		_iPLAC += r;
		break;
	case IPL_ACP_CURSE:
		_iPLAC -= r;
		break;
	case IPL_SETAC:
		_iAC = r;
		break;
	case IPL_AC_CURSE:
		_iAC -= r;
		break;
	case IPL_FIRERES:
		_iPLFR += r;
		break;
	case IPL_LIGHTRES:
		_iPLLR += r;
		break;
	case IPL_MAGICRES:
		_iPLMR += r;
		break;
	case IPL_ALLRES:
		_iPLFR += r;
		_iPLLR += r;
		_iPLMR += r;
		if (_iPLFR < 0)
			_iPLFR = 0;
		if (_iPLLR < 0)
			_iPLLR = 0;
		if (_iPLMR < 0)
			_iPLMR = 0;
		break;
	case IPL_SPLLVLADD:
		_iSplLvlAdd = r;
		break;
	case IPL_CHARGES:
		_iCharges *= param1;
		_iMaxCharges = _iCharges;
		break;
	case IPL_SPELL:
		_iSpell = param1;
		_iCharges = param1; // BUGFIX: should be param2. This code was correct in v1.04, and the bug was introduced between 1.04 and 1.09b.
		_iMaxCharges = param2;
		break;
	case IPL_FIREDAM:
		_iFlags |= ISPL_FIREDAM;
		_iFMinDam = param1;
		_iFMaxDam = param2;
		break;
	case IPL_LIGHTDAM:
		_iFlags |= ISPL_LIGHTDAM;
		_iLMinDam = param1;
		_iLMaxDam = param2;
		break;
	case IPL_STR:
		_iPLStr += r;
		break;
	case IPL_STR_CURSE:
		_iPLStr -= r;
		break;
	case IPL_MAG:
		_iPLMag += r;
		break;
	case IPL_MAG_CURSE:
		_iPLMag -= r;
		break;
	case IPL_DEX:
		_iPLDex += r;
		break;
	case IPL_DEX_CURSE:
		_iPLDex -= r;
		break;
	case IPL_VIT:
		_iPLVit += r;
		break;
	case IPL_VIT_CURSE:
		_iPLVit -= r;
		break;
	case IPL_ATTRIBS:
		_iPLStr += r;
		_iPLMag += r;
		_iPLDex += r;
		_iPLVit += r;
		break;
	case IPL_ATTRIBS_CURSE:
		_iPLStr -= r;
		_iPLMag -= r;
		_iPLDex -= r;
		_iPLVit -= r;
		break;
	case IPL_GETHIT_CURSE:
		_iPLGetHit += r;
		break;
	case IPL_GETHIT:
		_iPLGetHit -= r;
		break;
	case IPL_LIFE:
		_iPLHP += r << 6;
		break;
	case IPL_LIFE_CURSE:
		_iPLHP -= r << 6;
		break;
	case IPL_MANA:
		_iPLMana += r << 6;
		drawmanaflag = TRUE;
		break;
	case IPL_MANA_CURSE:
		_iPLMana -= r << 6;
		drawmanaflag = TRUE;
		break;
	case IPL_DUR:
		r2 = r * _iMaxDur / 100;
		_iMaxDur += r2;
		_iDurability += r2;
		break;
	case IPL_DUR_CURSE:
		_iMaxDur -= r * _iMaxDur / 100;
		if (_iMaxDur < 1)
			_iMaxDur = 1;
		_iDurability = _iMaxDur;
		break;
	case IPL_INDESTRUCTIBLE:
		_iDurability = DUR_INDESTRUCTIBLE;
		_iMaxDur = DUR_INDESTRUCTIBLE;
		break;
	case IPL_LIGHT:
		_iPLLight += param1;
		break;
	case IPL_LIGHT_CURSE:
		_iPLLight -= param1;
		break;
	case IPL_FIRE_ARROWS:
		_iFlags |= ISPL_FIRE_ARROWS;
		_iFMinDam = param1;
		_iFMaxDam = param2;
		break;
	case IPL_LIGHT_ARROWS:
		_iFlags |= ISPL_LIGHT_ARROWS;
		_iLMinDam = param1;
		_iLMaxDam = param2;
		break;
	case IPL_THORNS:
		_iFlags |= ISPL_THORNS;
		break;
	case IPL_NOMANA:
		_iFlags |= ISPL_NOMANA;
		drawmanaflag = TRUE;
		break;
	case IPL_NOHEALPLR:
		_iFlags |= ISPL_NOHEALPLR;
		break;
	case IPL_ABSHALFTRAP:
		_iFlags |= ISPL_ABSHALFTRAP;
		break;
	case IPL_KNOCKBACK:
		_iFlags |= ISPL_KNOCKBACK;
		break;
	case IPL_3XDAMVDEM:
		_iFlags |= ISPL_3XDAMVDEM;
		break;
	case IPL_ALLRESZERO:
		_iFlags |= ISPL_ALLRESZERO;
		break;
	case IPL_NOHEALMON:
		_iFlags |= ISPL_NOHEALMON;
		break;
	case IPL_STEALMANA:
		if (param1 == 3)
			_iFlags |= ISPL_STEALMANA_3;
		if (param1 == 5)
			_iFlags |= ISPL_STEALMANA_5;
		drawmanaflag = TRUE;
		break;
	case IPL_STEALLIFE:
		if (param1 == 3)
			_iFlags |= ISPL_STEALLIFE_3;
		if (param1 == 5)
			_iFlags |= ISPL_STEALLIFE_5;
		drawhpflag = TRUE;
		break;
	case IPL_TARGAC:
		_iPLEnAc += r;
		break;
	case IPL_FASTATTACK:
		if (param1 == 1)
			_iFlags |= ISPL_QUICKATTACK;
		if (param1 == 2)
			_iFlags |= ISPL_FASTATTACK;
		if (param1 == 3)
			_iFlags |= ISPL_FASTERATTACK;
		if (param1 == 4)
			_iFlags |= ISPL_FASTESTATTACK;
		break;
	case IPL_FASTRECOVER:
		if (param1 == 1)
			_iFlags |= ISPL_FASTRECOVER;
		if (param1 == 2)
			_iFlags |= ISPL_FASTERRECOVER;
		if (param1 == 3)
			_iFlags |= ISPL_FASTESTRECOVER;
		break;
	case IPL_FASTBLOCK:
		_iFlags |= ISPL_FASTBLOCK;
		break;
	case IPL_DAMMOD:
		_iPLDamMod += r;
		break;
	case IPL_RNDARROWVEL:
		_iFlags |= ISPL_RNDARROWVEL;
		break;
	case IPL_SETDAM:
		_iMinDam = param1;
		_iMaxDam = param2;
		break;
	case IPL_SETDUR:
		_iDurability = param1;
		_iMaxDur = param1;
		break;
	case IPL_FASTSWING:
		_iFlags |= ISPL_FASTERATTACK;
		break;
	case IPL_ONEHAND:
		_iLoc = ILOC_ONEHAND;
		break;
	case IPL_DRAINLIFE:
		_iFlags |= ISPL_DRAINLIFE;
		break;
	case IPL_RNDSTEALLIFE:
		_iFlags |= ISPL_RNDSTEALLIFE;
		break;
	case IPL_INFRAVISION:
		_iFlags |= ISPL_INFRAVISION;
		break;
	case IPL_NOMINSTR:
		_iMinStr = 0;
		break;
	case IPL_INVCURS:
		_iCurs = param1;
		break;
	case IPL_ADDACLIFE:
		_iPLHP = (myplr().data._pIBonusAC + myplr().data._pIAC + myplr().data._pDexterity / 5) << 6;
		break;
	case IPL_ADDMANAAC:
		_iAC += (myplr().data._pMaxManaBase >> 6) / 10;
		break;
	case IPL_FIRERESCLVL:
		_iPLFR = 30 - myplr().data._pLevel;
		if (_iPLFR < 0)
			_iPLFR = 0;
		break;
	}
	if (_iVAdd1 || _iVMult1) {
		_iVAdd2 = PLVal(r, param1, param2, minval, maxval);
		_iVMult2 = multval;
	} else {
		_iVAdd1 = PLVal(r, param1, param2, minval, maxval);
		_iVMult1 = multval;
	}
}

void Item::GetItemPower(int minlvl, int maxlvl, int flgs, bool onlygood)
{
	int pre, post, nt, nl, j, preidx, sufidx;
	int l[256];
	char istr[128];
	uint8_t goe;

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
			sprintf(istr, "%s %s", PL_Prefix[preidx].PLName, _iIName);
			strcpy(_iIName, istr);
			_iMagical = ITEM_QUALITY_MAGIC;
			SaveItemPower(
			    PL_Prefix[preidx].PLPower,
			    PL_Prefix[preidx].PLParam1,
			    PL_Prefix[preidx].PLParam2,
			    PL_Prefix[preidx].PLMinVal,
			    PL_Prefix[preidx].PLMaxVal,
			    PL_Prefix[preidx].PLMultVal);
			_iPrePower = PL_Prefix[preidx].PLPower;
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
			sprintf(istr, "%s of %s", _iIName, PL_Suffix[sufidx].PLName);
			strcpy(_iIName, istr);
			_iMagical = ITEM_QUALITY_MAGIC;
			SaveItemPower(
			    PL_Suffix[sufidx].PLPower,
			    PL_Suffix[sufidx].PLParam1,
			    PL_Suffix[sufidx].PLParam2,
			    PL_Suffix[sufidx].PLMinVal,
			    PL_Suffix[sufidx].PLMaxVal,
			    PL_Suffix[sufidx].PLMultVal);
			_iSufPower = PL_Suffix[sufidx].PLPower;
		}
	}
	if (!control_WriteStringToBuffer((uint8_t *)_iIName)) {
		strcpy(_iIName, AllItemsList[IDidx].iSName);
		if (preidx != -1) {
			sprintf(istr, "%s %s", PL_Prefix[preidx].PLName, _iIName);
			strcpy(_iIName, istr);
		}
		if (sufidx != -1) {
			sprintf(istr, "%s of %s", _iIName, PL_Suffix[sufidx].PLName);
			strcpy(_iIName, istr);
		}
	}
	if (preidx != -1 || sufidx != -1)
		CalcItemValue();
}

void Item::GetItemBonus(int idata, int minlvl, int maxlvl, bool onlygood)
{
	if (_iClass != ICLASS_GOLD) {
		if (minlvl > 25)
			minlvl = 25;

		switch (_itype) {
		case ITYPE_SWORD:
		case ITYPE_AXE:
		case ITYPE_MACE:
			GetItemPower(minlvl, maxlvl, PLT_WEAP, onlygood);
			break;
		case ITYPE_BOW:
			GetItemPower(minlvl, maxlvl, PLT_BOW, onlygood);
			break;
		case ITYPE_SHIELD:
			GetItemPower(minlvl, maxlvl, PLT_SHLD, onlygood);
			break;
		case ITYPE_LARMOR:
		case ITYPE_HELM:
		case ITYPE_MARMOR:
		case ITYPE_HARMOR:
			GetItemPower(minlvl, maxlvl, PLT_ARMO, onlygood);
			break;
		case ITYPE_STAFF:
			GetStaffSpell(maxlvl, onlygood);
			break;
		case ITYPE_RING:
		case ITYPE_AMULET:
			GetItemPower(minlvl, maxlvl, PLT_MISC, onlygood);
			break;
		}
	}
}

void Item::SetupItem()
{
	int it = ItemCAnimTbl[_iCurs];
	_iAnimData = itemTypes[it].anims;
	_iAnimLen = ItemAnimLs[it];
	_iAnimWidth = 96;
	_iAnimWidth2 = 16;
	_iIdentified = FALSE;
	_iPostDraw = FALSE;

	if (!myplr().data.pLvlLoad) {
		_iAnimFrame = 1;
		_iAnimFlag = TRUE;
		_iSelFlag = 0;
	} else {
		_iAnimFrame = _iAnimLen;
		_iAnimFlag = FALSE;
		_iSelFlag = 1;
	}
}


int Item::CheckUnique(int lvl, int uper, bool recreate)
{
	int j, idata, numu;
	bool uok[128];

	if (random_(28, 100) > uper)
		return -1;

	numu = 0;
	memset(uok, 0, sizeof(uok));
	for (j = 0; UniqueItemList[j].UIItemId != UITYPE_INVALID; j++) {
		if (UniqueItemList[j].UIItemId == AllItemsList[IDidx].iItemId
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

void Item::GetUniqueItem(int uid)
{
	UniqueItemFlag[uid] = TRUE;
	SaveItemPower(UniqueItemList[uid].UIPower1, UniqueItemList[uid].UIParam1, UniqueItemList[uid].UIParam2, 0, 0, 1);

	if (UniqueItemList[uid].UINumPL > 1)
		SaveItemPower(UniqueItemList[uid].UIPower2, UniqueItemList[uid].UIParam3, UniqueItemList[uid].UIParam4, 0, 0, 1);
	if (UniqueItemList[uid].UINumPL > 2)
		SaveItemPower(UniqueItemList[uid].UIPower3, UniqueItemList[uid].UIParam5, UniqueItemList[uid].UIParam6, 0, 0, 1);
	if (UniqueItemList[uid].UINumPL > 3)
		SaveItemPower(UniqueItemList[uid].UIPower4, UniqueItemList[uid].UIParam7, UniqueItemList[uid].UIParam8, 0, 0, 1);
	if (UniqueItemList[uid].UINumPL > 4)
		SaveItemPower(UniqueItemList[uid].UIPower5, UniqueItemList[uid].UIParam9, UniqueItemList[uid].UIParam10, 0, 0, 1);
	if (UniqueItemList[uid].UINumPL > 5)
		SaveItemPower(UniqueItemList[uid].UIPower6, UniqueItemList[uid].UIParam11, UniqueItemList[uid].UIParam12, 0, 0, 1);

	strcpy(_iIName, UniqueItemList[uid].UIName);
	_iIvalue = UniqueItemList[uid].UIValue;

	if (_iMiscId == IMISC_UNIQUE)
		_iSeed = uid;

	_iUid = uid;
	_iMagical = ITEM_QUALITY_UNIQUE;
	_iCreateInfo |= 0x0200;
}

void Item::RespawnItem(bool FlipFlag)
{
	int it = ItemCAnimTbl[_iCurs];
	_iAnimData = itemTypes[it].anims;
	_iAnimLen = ItemAnimLs[it];
	_iAnimWidth = 96;
	_iAnimWidth2 = 16;
	_iPostDraw = FALSE;
	_iRequest = FALSE;
	if (FlipFlag) {
		_iAnimFrame = 1;
		_iAnimFlag = TRUE;
		_iSelFlag = 0;
	} else {
		_iAnimFrame = _iAnimLen;
		_iAnimFlag = FALSE;
		_iSelFlag = 1;
	}

	if (_iCurs == ICURS_MAGIC_ROCK) {
		_iSelFlag = 1;
		PlaySfxLoc(ItemDropSnds[it], _i);
	}
	if (_iCurs == ICURS_TAVERN_SIGN)
		_iSelFlag = 1;
	if (_iCurs == ICURS_ANVIL_OF_FURY)
		_iSelFlag = 1;
}


void Item::GetItemFrm()
{
	_iAnimData = itemTypes[ItemCAnimTbl[_iCurs]].anims;
}

void Item::GetItemStr()
{
	int nGold;

	if (_itype != ITYPE_GOLD) {
		if (_iIdentified)
			strcpy(infostr, _iIName);
		else
			strcpy(infostr, _iName);

		if (_iMagical == ITEM_QUALITY_MAGIC)
			infoclr = COL_BLUE;
		if (_iMagical == ITEM_QUALITY_UNIQUE)
			infoclr = COL_GOLD;
	} else {
		nGold = _ivalue;
		sprintf(infostr, "%i gold %s", nGold, get_pieces_str(nGold));
	}
}

void Item::ItemRndDur()
{
	if (_iDurability && _iDurability != DUR_INDESTRUCTIBLE)
		_iDurability = random_(0, _iMaxDur >> 1) + (_iMaxDur >> 2) + 1;
}

void Item::SetupAllItems(int idx, int iseed, int lvl, int uper, int onlygood, bool recreate, bool pregen)
{
	int iblvl, uid;

	_iSeed = iseed;
	SetRndSeed(iseed);
	GetItemAttrs(idx, lvl >> 1);
	_iCreateInfo = lvl;

	if (pregen)
		_iCreateInfo = lvl | 0x8000;
	if (onlygood)
		_iCreateInfo |= 0x40;

	if (uper == 15)
		_iCreateInfo |= 0x80;
	else if (uper == 1)
		_iCreateInfo |= 0x0100;

	if (_iMiscId != IMISC_UNIQUE) {
		iblvl = -1;
		if (random_(32, 100) <= 10 || random_(33, 100) <= lvl) {
			iblvl = lvl;
		}
		if (iblvl == -1 && _iMiscId == IMISC_STAFF) {
			iblvl = lvl;
		}
		if (iblvl == -1 && _iMiscId == IMISC_RING) {
			iblvl = lvl;
		}
		if (iblvl == -1 && _iMiscId == IMISC_AMULET) {
			iblvl = lvl;
		}
		if (onlygood)
			iblvl = lvl;
		if (uper == 15)
			iblvl = lvl + 4;
		if (iblvl != -1) {
			uid = CheckUnique(iblvl, uper, recreate);
			if (uid == UITYPE_INVALID) {
				GetItemBonus(idx, iblvl >> 1, iblvl, onlygood);
			} else {
				GetUniqueItem(uid);
				_iCreateInfo |= 0x0200;
			}
		}
		if (_iMagical != ITEM_QUALITY_UNIQUE)
			ItemRndDur();
	} else {
		if (_iLoc != ILOC_UNEQUIPABLE) {
			//uid = CheckUnique(ii, iblvl, uper, recreate);
			//if (uid != UITYPE_INVALID) {
			//	GetUniqueItem(ii, uid);
			//}
			GetUniqueItem(iseed); // BUG: the second argument to GetUniqueItem should be uid.
		}
	}
	SetupItem();
}

void Item::SetupAllUseful(int iseed, int lvl)
{
	int idx;

	_iSeed = iseed;
	SetRndSeed(iseed);

	if (random_(34, 2))
		idx = IDI_HEAL;
	else
		idx = IDI_MANA;

	if (lvl > 1 && !random_(34, 3))
		idx = IDI_PORTAL;

	GetItemAttrs(idx, lvl);
	_iCreateInfo = lvl + 384;
	SetupItem();
}

void Item::RecreateItem(int idx, WORD icreateinfo, int iseed, int ivalue)
{
	int uper, onlygood, recreate;
	bool pregen;

	if (!idx) {
		SetPlrHandItem(this, IDI_GOLD);
		_iSeed = iseed;
		_iCreateInfo = icreateinfo;
		_ivalue = ivalue;
		if (ivalue >= GOLD_MEDIUM_LIMIT)
			_iCurs = ICURS_GOLD_LARGE;
		else if (ivalue <= GOLD_SMALL_LIMIT)
			_iCurs = ICURS_GOLD_SMALL;
		else
			_iCurs = ICURS_GOLD_MEDIUM;
	} else {
		if (!icreateinfo) {
			SetPlrHandItem(this, idx);
			SetPlrHandSeed(this, iseed);
		} else {
			if (icreateinfo & 0x7C00) {
				RecreateTownItem(idx, icreateinfo, iseed, ivalue);
			} else if ((icreateinfo & 0x0180) == 0x0180) {
				SetupAllUseful(iseed, icreateinfo & 0x3F);
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
				SetupAllItems(idx, iseed, icreateinfo & 0x3F, uper, onlygood, recreate, pregen);
			}
		}
	}
}

void Item::RecreateEar(WORD ic, int iseed, int Id, int dur, int mdur, int ch, int mch, int ivalue, int ibuff)
{
	SetPlrHandItem(this, IDI_EAR);
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
	sprintf(_iName, "Ear of %s", tempstr);
	_iCurs = ((ivalue >> 6) & 3) + 19;
	_ivalue = ivalue & 0x3F;
	_iCreateInfo = ic;
	_iSeed = iseed;
}


void Item::RecreateSmithItem(int idx, int lvl, int iseed)
{
	int itype;

	SetRndSeed(iseed);
	itype = RndSmithItem(lvl) - 1;
	GetItemAttrs(itype, lvl);

	_iSeed = iseed;
	_iCreateInfo = lvl | 0x400;
	_iIdentified = TRUE;
}

void Item::RecreatePremiumItem(int idx, int plvl, int iseed)
{
	int itype;

	SetRndSeed(iseed);
	itype = RndPremiumItem(plvl >> 2, plvl) - 1;
	GetItemAttrs(itype, plvl);
	GetItemBonus(itype, plvl >> 1, plvl, TRUE);

	_iSeed = iseed;
	_iCreateInfo = plvl | 0x800;
	_iIdentified = TRUE;
}

void Item::RecreateBoyItem(int idx, int lvl, int iseed)
{
	int itype;

	SetRndSeed(iseed);
	itype = RndBoyItem(lvl) - 1;
	GetItemAttrs(itype, lvl);
	GetItemBonus(itype, lvl, 2 * lvl, TRUE);
	_iSeed = iseed;
	_iCreateInfo = lvl | 0x1000;
	_iIdentified = TRUE;
}

void Item::RecreateWitchItem(int idx, int lvl, int iseed)
{
	int iblvl, itype;

	if (idx == IDI_MANA || idx == IDI_FULLMANA || idx == IDI_PORTAL) {
		GetItemAttrs(idx, lvl);
	} else {
		SetRndSeed(iseed);
		itype = RndWitchItem(lvl) - 1;
		GetItemAttrs(itype, lvl);
		iblvl = -1;
		if (random_(51, 100) <= 5)
			iblvl = 2 * lvl;
		if (iblvl == -1 && _iMiscId == IMISC_STAFF)
			iblvl = 2 * lvl;
		if (iblvl != -1)
			GetItemBonus(itype, iblvl >> 1, iblvl, TRUE);
	}

	_iSeed = iseed;
	_iCreateInfo = lvl | 0x2000;
	_iIdentified = TRUE;
}

void Item::RecreateHealerItem(int idx, int lvl, int iseed)
{
	int itype;

	if (idx == IDI_HEAL || idx == IDI_FULLHEAL || idx == IDI_RESURRECT) {
		GetItemAttrs(idx, lvl);
	} else {
		SetRndSeed(iseed);
		itype = RndHealerItem(lvl) - 1;
		GetItemAttrs(itype, lvl);
	}

	_iSeed = iseed;
	_iCreateInfo = lvl | 0x4000;
	_iIdentified = TRUE;
}

void Item::RecreateTownItem(int idx, WORD icreateinfo, int iseed, int ivalue)
{
	if (icreateinfo & 0x400)
		RecreateSmithItem(idx, icreateinfo & 0x3F, iseed);
	else if (icreateinfo & 0x800)
		RecreatePremiumItem(idx, icreateinfo & 0x3F, iseed);
	else if (icreateinfo & 0x1000)
		RecreateBoyItem(idx, icreateinfo & 0x3F, iseed);
	else if (icreateinfo & 0x2000)
		RecreateWitchItem(idx, icreateinfo & 0x3F, iseed);
	else if (icreateinfo & 0x4000)
		RecreateHealerItem(idx, icreateinfo & 0x3F, iseed);
}


DEVILUTION_END_NAMESPACE
