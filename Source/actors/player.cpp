#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

int plr_lframe_size;
int plr_wframe_size;
BYTE plr_gfx_flag = 0;
int plr_aframe_size;
int plr_fframe_size;
int plr_qframe_size;
BOOL deathflag;
int plr_hframe_size;
int plr_bframe_size;
BYTE plr_gfx_bflag = 0;
int plr_sframe_size;
int deathdelay;
int plr_dframe_size;

PlayerManager plr;

Player & myplr()
{
	return plr._local();
}

const char ArmourChar[4] = { 'L', 'M', 'H', 0 };
const char WepChar[10] = { 'N', 'U', 'S', 'D', 'B', 'A', 'M', 'H', 'T', 0 };
const char CharChar[] = {
	'W',
	'R',
	'S',
	0
};

/* data */

int plrxoff[9] = { 0, 2, 0, 2, 1, 0, 1, 2, 1 };
int plryoff[9] = { 0, 2, 2, 0, 1, 1, 0, 1, 2 };
int plrxoff2[9] = { 0, 1, 0, 1, 2, 0, 1, 2, 2 };
int plryoff2[9] = { 0, 0, 1, 1, 0, 2, 2, 1, 2 };

CharacterTypes classes;

int ExpLvlsTbl[MAXCHARLEVEL] = {
	0,
	2000,
	4620,
	8040,
	12489,
	18258,
	25712,
	35309,
	47622,
	63364,
	83419,
	108879,
	141086,
	181683,
	231075,
	313656,
	424067,
	571190,
	766569,
	1025154,
	1366227,
	1814568,
	2401895,
	3168651,
	4166200,
	5459523,
	7130496,
	9281874,
	12042092,
	15571031,
	20066900,
	25774405,
	32994399,
	42095202,
	53525811,
	67831218,
	85670061,
	107834823,
	135274799,
	169122009,
	210720231,
	261657253,
	323800420,
	399335440,
	490808349,
	601170414,
	733825617,
	892680222,
	1082908612,
	1310707109,
	1583495809
};

BYTE fix[9] = { 0, 0, 3, 3, 3, 6, 6, 6, 8 }; /* PM_ChangeLightOff local type */

void SetPlayerGPtrs(BYTE *pData, BYTE **pAnim)
{
	int i;

	for (i = 0; i < 8; i++) {
		pAnim[i] = CelGetFrameStart(pData, i);
	}
}

bool Player::isLocal()
{
	return id() == myplr();
}

void Player::LoadPlrGFX(player_graphic gfxflag)
{
	char prefix[16];
	char pszName[256];
	char *szCel;
	BYTE *pData, *pAnim;
	DWORD i;

	sprintf(prefix, "%c%c%c", CharChar[data._pClass], ArmourChar[data._pgfxnum >> 4], WepChar[data._pgfxnum & 0xF]);
	std::string &cs = classes[data._pClass].name;

	for (i = 1; i <= PFILE_NONDEATH; i <<= 1) {
		if (!(i & gfxflag)) {
			continue;
		}

		switch (i) {
		case PFILE_STAND:
			szCel = "AS";
			if (level.leveltype == DTYPE_TOWN) {
				szCel = "ST";
			}
			pData = data._pNData;
			pAnim = (BYTE *)data._pNAnim;
			break;
		case PFILE_WALK:
			szCel = "AW";
			if (level.leveltype == DTYPE_TOWN) {
				szCel = "WL";
			}
			pData = data._pWData;
			pAnim = (BYTE *)data._pWAnim;
			break;
		case PFILE_ATTACK:
			if (level.leveltype == DTYPE_TOWN) {
				continue;
			}
			szCel = "AT";
			pData = data._pAData;
			pAnim = (BYTE *)data._pAAnim;
			break;
		case PFILE_HIT:
			if (level.leveltype == DTYPE_TOWN) {
				continue;
			}
			szCel = "HT";
			pData = data._pHData;
			pAnim = (BYTE *)data._pHAnim;
			break;
		case PFILE_LIGHTNING:
			if (level.leveltype == DTYPE_TOWN) {
				continue;
			}
			szCel = "LM";
			pData = data._pLData;
			pAnim = (BYTE *)data._pLAnim;
			break;
		case PFILE_FIRE:
			if (level.leveltype == DTYPE_TOWN) {
				continue;
			}
			szCel = "FM";
			pData = data._pFData;
			pAnim = (BYTE *)data._pFAnim;
			break;
		case PFILE_MAGIC:
			if (level.leveltype == DTYPE_TOWN) {
				continue;
			}
			szCel = "QM";
			pData = data._pTData;
			pAnim = (BYTE *)data._pTAnim;
			break;
		case PFILE_DEATH:
			if (data._pgfxnum & 0xF) {
				continue;
			}
			szCel = "DT";
			pData = data._pDData;
			pAnim = (BYTE *)data._pDAnim;
			break;
		case PFILE_BLOCK:
			if (level.leveltype == DTYPE_TOWN) {
				continue;
			}
			if (!data._pBlockFlag) {
				continue;
			}

			szCel = "BL";
			pData = data._pBData;
			pAnim = (BYTE *)data._pBAnim;
			break;
		default:
			app_fatal("PLR:2");
			break;
		}

		sprintf(pszName, "PlrGFX\\%s\\%s\\%s%s.CL2", cs.c_str(), prefix, prefix, szCel);
		LoadFileWithMem(pszName, pData);
		SetPlayerGPtrs((BYTE *)pData, (BYTE **)pAnim);
		data._pGFXLoad |= i;
	}
}

void Player::InitPlayerGFX()
{

	if (data._pHitPoints >> 6 == 0) {
		data._pgfxnum = 0;
		LoadPlrGFX(PFILE_DEATH);
	} else {
		LoadPlrGFX(PFILE_NONDEATH);
	}
}

void Player::InitPlrGFXMem()
{
	if (!(plr_gfx_flag & 0x1)) { //STAND
		plr_gfx_flag |= 0x1;
		if (GetPlrGFXSize("ST") > GetPlrGFXSize("AS")) {
			plr_sframe_size = GetPlrGFXSize("ST"); //TOWN
		} else {
			plr_sframe_size = GetPlrGFXSize("AS"); //DUNGEON
		}
	}
	data._pNData = DiabloAllocPtr(plr_sframe_size);

	if (!(plr_gfx_flag & 0x2)) { //WALK
		plr_gfx_flag |= 0x2;
		if (GetPlrGFXSize("WL") > GetPlrGFXSize("AW")) {
			plr_wframe_size = GetPlrGFXSize("WL"); //TOWN
		} else {
			plr_wframe_size = GetPlrGFXSize("AW"); //DUNGEON
		}
	}
	data._pWData = DiabloAllocPtr(plr_wframe_size);

	if (!(plr_gfx_flag & 0x4)) { //ATTACK
		plr_gfx_flag |= 0x4;
		plr_aframe_size = GetPlrGFXSize("AT");
	}
	data._pAData = DiabloAllocPtr(plr_aframe_size);

	if (!(plr_gfx_flag & 0x8)) { //HIT
		plr_gfx_flag |= 0x8;
		plr_hframe_size = GetPlrGFXSize("HT");
	}
	data._pHData = DiabloAllocPtr(plr_hframe_size);

	if (!(plr_gfx_flag & 0x10)) { //LIGHTNING
		plr_gfx_flag |= 0x10;
		plr_lframe_size = GetPlrGFXSize("LM");
	}
	data._pLData = DiabloAllocPtr(plr_lframe_size);

	if (!(plr_gfx_flag & 0x20)) { //FIRE
		plr_gfx_flag |= 0x20;
		plr_fframe_size = GetPlrGFXSize("FM");
	}
	data._pFData = DiabloAllocPtr(plr_fframe_size);

	if (!(plr_gfx_flag & 0x40)) { //MAGIC
		plr_gfx_flag |= 0x40;
		plr_qframe_size = GetPlrGFXSize("QM");
	}
	data._pTData = DiabloAllocPtr(plr_qframe_size);

	if (!(plr_gfx_flag & 0x80)) { //DEATH
		plr_gfx_flag |= 0x80;
		plr_dframe_size = GetPlrGFXSize("DT");
	}
	data._pDData = DiabloAllocPtr(plr_dframe_size);

	if (!(plr_gfx_bflag & 0x1)) { //BLOCK
		plr_gfx_bflag |= 0x1;
		plr_bframe_size = GetPlrGFXSize("BL");
	}
	data._pBData = DiabloAllocPtr(plr_bframe_size);

	data._pGFXLoad = 0;
}

DWORD GetPlrGFXSize(char *szCel)
{
	int c;
	const char *a, *w;
	DWORD dwSize, dwMaxSize;
	HANDLE hsFile;
	char pszName[256];
	char Type[16];

	dwMaxSize = 0;

	for (c = 0; c < classes.numclasses; c++) {
	//for (c = 0; c < sizeof(ClassStrTbl) / sizeof(*ClassStrTbl); c++) {
#ifdef SPAWN
		if (c != 0)
			continue;
#endif
		for (a = &ArmourChar[0]; *a; a++) {
#ifdef SPAWN
			if (a != &ArmourChar[0])
				break;
#endif
			for (w = &WepChar[0]; *w; w++) {
				if (szCel[0] == 'D' && szCel[1] == 'T' && *w != 'N') {
					continue; //Death has no weapon
				}
				if (szCel[0] == 'B' && szCel[1] == 'L' && (*w != 'U' && *w != 'D' && *w != 'H')) {
					continue; //No block without weapon
				}
				sprintf(Type, "%c%c%c", CharChar[c], *a, *w);
				sprintf(pszName, "PlrGFX\\%s\\%s\\%s%s.CL2", classes[c].name.c_str(), Type, Type, szCel);
				if (WOpenFile(pszName, &hsFile, TRUE)) {
					/// ASSERT: assert(hsFile);
					dwSize = WGetFileSize(hsFile, NULL, pszName);
					WCloseFile(hsFile);
					if (dwMaxSize <= dwSize) {
						dwMaxSize = dwSize;
					}
				}
			}
		}
	}

	return dwMaxSize;
}

void Player::FreePlayerGFX()
{
	//if ((DWORD)pnum >= MAX_PLRS) {
	//	app_fatal("FreePlayerGFX: illegal player %d", pnum);
	//}

	MemFreeDbg(data._pNData);
	MemFreeDbg(data._pWData);
	MemFreeDbg(data._pAData);
	MemFreeDbg(data._pHData);
	MemFreeDbg(data._pLData);
	MemFreeDbg(data._pFData);
	MemFreeDbg(data._pTData);
	MemFreeDbg(data._pDData);
	MemFreeDbg(data._pBData);
	data._pGFXLoad = 0;
}

void Player::NewPlrAnim(BYTE *Peq, int numFrames, int Delay, int width)
{

	data._pAnimData = Peq;
	data._pAnimLen = numFrames;
	data._pAnimFrame = 1;
	data._pAnimCnt = 0;
	data._pAnimDelay = Delay;
	data._pAnimWidth = width;
	data._pAnimWidth2 = (width - 64) >> 1;
}

void Player::ClearPlrPVars()
{
	data._pVar1 = 0;
	data._pVar2 = 0;
	data._pVar3 = 0;
	data._pVar4 = 0;
	data._pVar5 = 0;
	data._pVar6 = 0;
	data._pVar7 = 0;
	data._pVar8 = 0;
}

void Player::SetPlrAnims()
{
	int pc, gn;

	data._pNWidth = 96;
	data._pWWidth = 96;
	data._pAWidth = 128;
	data._pHWidth = 96;
	data._pSWidth = 96;
	data._pDWidth = 128;
	data._pBWidth = 96;

	pc = data._pClass;

	if (level.leveltype == DTYPE_TOWN) {
		data._pNFrames = classes[pc].PlrGFXAnimLens[7];
		data._pWFrames = classes[pc].PlrGFXAnimLens[8];
		data._pDFrames = classes[pc].PlrGFXAnimLens[4];
		data._pSFrames = classes[pc].PlrGFXAnimLens[5];
	} else {
		data._pNFrames = classes[pc].PlrGFXAnimLens[0];
		data._pWFrames = classes[pc].PlrGFXAnimLens[2];
		data._pAFrames = classes[pc].PlrGFXAnimLens[1];
		data._pHFrames = classes[pc].PlrGFXAnimLens[6];
		data._pSFrames = classes[pc].PlrGFXAnimLens[5];
		data._pDFrames = classes[pc].PlrGFXAnimLens[4];
		data._pBFrames = classes[pc].PlrGFXAnimLens[3];
		data._pAFNum = classes[pc].PlrGFXAnimLens[9];
	}
	data._pSFNum = classes[pc].PlrGFXAnimLens[10];

	gn = data._pgfxnum & 0xF;
	if (pc == PC_WARRIOR) {
		if (gn == ANIM_ID_BOW) {
			if (level.leveltype != DTYPE_TOWN) {
				data._pNFrames = 8;
			}
			data._pAWidth = 96;
			data._pAFNum = 11;
		} else if (gn == ANIM_ID_AXE) {
			data._pAFrames = 20;
			data._pAFNum = 10;
		} else if (gn == ANIM_ID_STAFF) {
			data._pAFrames = 16;
			data._pAFNum = 11;
		}
#ifndef SPAWN
	} else if (pc == PC_ROGUE) {
		if (gn == ANIM_ID_AXE) {
			data._pAFrames = 22;
			data._pAFNum = 13;
		} else if (gn == ANIM_ID_BOW) {
			data._pAFrames = 12;
			data._pAFNum = 7;
		} else if (gn == ANIM_ID_STAFF) {
			data._pAFrames = 16;
			data._pAFNum = 11;
		}
	} else if (pc == PC_SORCERER) {
		data._pSWidth = 128;
		if (gn == ANIM_ID_UNARMED) {
			data._pAFrames = 20;
		} else if (gn == ANIM_ID_UNARMED_SHIELD) {
			data._pAFNum = 9;
		} else if (gn == ANIM_ID_BOW) {
			data._pAFrames = 20;
			data._pAFNum = 16;
		} else if (gn == ANIM_ID_AXE) {
			data._pAFrames = 24;
			data._pAFNum = 16;
		}
#endif
	}
}

void Player::ClearPlrRVars()
{
	// TODO: Missing debug assert p != NULL
	data.bReserved[0] = 0;
	data.bReserved[1] = 0;
	data.bReserved[2] = 0;
	data.wReserved[0] = 0;
	data.wReserved[1] = 0;
	data.wReserved[2] = 0;
	data.wReserved[3] = 0;
	data.wReserved[4] = 0;
	data.wReserved[5] = 0;
	data.wReserved[6] = 0;
	data.wReserved[7] = 0;
	data.dwReserved[0] = 0;
	data.dwReserved[1] = 0;
	data.dwReserved[2] = 0;
	data.dwReserved[3] = 0;
	data.dwReserved[4] = 0;
	data.dwReserved[5] = 0;
	data.dwReserved[6] = 0;
}

/**
 * @param c plr_classes value
 */
void Player::CreatePlayer(char c)
{
	char val;
	int hp, mana;
	int i;

	ClearPlrRVars();
	SetRndSeed(SDL_GetTicks());

	data._pClass = c;

	val = classes[c].strength;
	if (val < 0) {
		val = 0;
	}
	data._pStrength = val;
	data._pBaseStr = val;

	val = classes[c].magic;
	if (val < 0) {
		val = 0;
	}
	data._pMagic = val;
	data._pBaseMag = val;

	val = classes[c].dexterity;
	if (val < 0) {
		val = 0;
	}
	data._pDexterity = val;
	data._pBaseDex = val;

	val = classes[c].vitality;
	if (val < 0) {
		val = 0;
	}
	data._pVitality = val;
	data._pBaseVit = val;

	data._pStatPts = 0;
	data.pTownWarps = 0;
	data.pDungMsgs = 0;
	data.pLvlLoad = 0;
	data.pDiabloKillLevel = 0;

	if (c == PC_ROGUE) {
		data._pDamageMod = data._pLevel * (data._pStrength + data._pDexterity) / 200;
	} else {
		data._pDamageMod = data._pStrength * data._pLevel / 100;
	}

	data._pBaseToBlk = classes[c].block;

	data._pHitPoints = (val + 10) << 6;
	if (c == PC_WARRIOR) {
		data._pHitPoints *= 2;
	}
	if (c == PC_ROGUE) {
		data._pHitPoints += data._pHitPoints >> 1;
	}

	hp = data._pHitPoints;
	data._pMaxHP = hp;
	data._pHPBase = hp;
	data._pMaxHPBase = hp;

	data._pMana = data._pMagic << 6;
	if (c == PC_SORCERER) {
		data._pMana *= 2;
	}
	if (c == PC_ROGUE) {
		data._pMana += data._pMana >> 1;
	}

	mana = data._pMana;
	data._pMaxMana = mana;
	data._pManaBase = mana;
	data._pMaxManaBase = mana;

	data._pLevel = 1;
	data._pMaxLvl = 1;
	data._pExperience = 0;
	data._pMaxExp = 0;
	data._pNextExper = ExpLvlsTbl[1];
	data._pArmorClass = 0;
	data._pMagResist = 0;
	data._pFireResist = 0;
	data._pLghtResist = 0;
	data._pLightRad = 10;
	data._pInfraFlag = FALSE;

	if (c == PC_WARRIOR) {
		data._pAblSpells = (__int64)1 << (SPL_REPAIR - 1);
#ifndef SPAWN
	} else if (c == PC_ROGUE) {
		data._pAblSpells = (__int64)1 << (SPL_DISARM - 1);
	} else if (c == PC_SORCERER) {
		data._pAblSpells = (__int64)1 << (SPL_RECHARGE - 1);
#endif
	}

	if (c == PC_SORCERER) {
		data._pMemSpells = 1;
	} else {
		data._pMemSpells = 0;
	}

	for (i = 0; i < sizeof(data._pSplLvl) / sizeof(data._pSplLvl[0]); i++) {
		data._pSplLvl[i] = 0;
	}

	data._pSpellFlags = 0;

	if (data._pClass == PC_SORCERER) {
		data._pSplLvl[SPL_FIREBOLT] = 2;
	}

	// interestingly, only the first three hotkeys are reset
	// TODO: BUGFIX: clear all 4 hotkeys instead of 3 (demo leftover)
	for (i = 0; i < 3; i++) {
		data._pSplHotKey[i] = -1;
	}

	if (c == PC_WARRIOR) {
		data._pgfxnum = ANIM_ID_SWORD_SHIELD;
#ifndef SPAWN
	} else if (c == PC_ROGUE) {
		data._pgfxnum = ANIM_ID_BOW;
	} else if (c == PC_SORCERER) {
		data._pgfxnum = ANIM_ID_STAFF;
#endif
	}

	for (i = 0; i < NUMLEVELS; i++) {
		data._pLvlVisited[i] = FALSE;
	}

	for (i = 0; i < 10; i++) {
		data._pSLvlVisited[i] = FALSE;
	}

	data._pLvlChanging = FALSE;
	data.pTownWarps = 0;
	data.pLvlLoad = 0;
	data.pBattleNet = FALSE;
	data.pManaShield = FALSE;

	InitDungMsgs();
	CreatePlrItems(pnum);
	SetRndSeed(0);
}

int Player::CalcStatDiff()
{
	int c;

	c = data._pClass;
	return classes[c].MaxStats[ATTRIB_STR]
	    - data._pBaseStr
	    + classes[c].MaxStats[ATTRIB_MAG]
	    - data._pBaseMag
	    + classes[c].MaxStats[ATTRIB_DEX]
	    - data._pBaseDex
	    + classes[c].MaxStats[ATTRIB_VIT]
	    - data._pBaseVit;
}

void Player::NextPlrLevel()
{
	int hp, mana;

	data._pLevel++;
	data._pMaxLvl++;

	if (CalcStatDiff() < 5) {
		data._pStatPts = CalcStatDiff();
	} else {
		data._pStatPts += 5;
	}

	data._pNextExper = ExpLvlsTbl[data._pLevel];

	hp = data._pClass == PC_SORCERER ? 64 : 128;
	if (gbMaxPlayers == 1) {
		hp++;
	}
	data._pMaxHP += hp;
	data._pHitPoints = data._pMaxHP;
	data._pMaxHPBase += hp;
	data._pHPBase = data._pMaxHPBase;

	if (pnum == myplr()) {
		drawhpflag = TRUE;
	}

	if (data._pClass == PC_WARRIOR)
		mana = 64;
	else
		mana = 128;

	if (gbMaxPlayers == 1) {
		mana++;
	}
	data._pMaxMana += mana;
	data._pMaxManaBase += mana;

	if (!(data._pIFlags & ISPL_NOMANA)) {
		data._pMana = data._pMaxMana;
		data._pManaBase = data._pMaxManaBase;
	}

	if (pnum == myplr()) {
		drawmanaflag = TRUE;
	}

	if (sgbControllerActive)
		FocusOnCharInfo();
}

void Player::AddPlrExperience(int lvl, int exp)
{
	int powerLvlCap, expCap, newLvl, i;

	if (pnum != myplr()) {
		return;
	}

	if ((DWORD)myplr() >= MAX_PLRS) {
		app_fatal("AddPlrExperience: illegal player %d", myplr());
	}

	if (data._pHitPoints <= 0) {
		return;
	}

	// Adjust xp based on difference in level between player and monster
	exp *= 1 + ((double)lvl - data._pLevel) / 10;
	if (exp < 0) {
		exp = 0;
	}

	// Prevent power leveling
	if (gbMaxPlayers > 1) {
		powerLvlCap = data._pLevel < 0 ? 0 : data._pLevel;
		if (powerLvlCap >= 50) {
			powerLvlCap = 50;
		}
		// cap to 1/20 of current levels xp
		if (exp >= ExpLvlsTbl[powerLvlCap] / 20) {
			exp = ExpLvlsTbl[powerLvlCap] / 20;
		}
		// cap to 200 * current level
		expCap = 200 * powerLvlCap;
		if (exp >= expCap) {
			exp = expCap;
		}
	}

	data._pExperience += exp;
	if ((DWORD)data._pExperience > MAXEXP) {
		data._pExperience = MAXEXP;
	}

	if (data._pExperience >= ExpLvlsTbl[49]) {
		data._pLevel = 50;
		return;
	}

	// Increase player level if applicable
	newLvl = 0;
	while (data._pExperience >= ExpLvlsTbl[newLvl]) {
		newLvl++;
	}
	if (newLvl != data._pLevel) {
		for (i = newLvl - data._pLevel; i > 0; i--) {
			NextPlrLevel();
		}
	}

	NetSendCmdParam1(FALSE, CMD_PLRLEVEL, data._pLevel);
}

void AddPlrMonstExper(int lvl, int exp, char pmask)
{
	int totplrs, i, e;

	totplrs = 0;
	for (i = 0; i < MAX_PLRS; i++) {
		if ((1 << i) & pmask) {
			totplrs++;
		}
	}

	if (totplrs) {
		e = exp / totplrs;
		if (pmask & (1 << myplr()))
			myplr().AddPlrExperience(lvl, e);
	}
}

void Player::InitPlayer(BOOL FirstTime)
{
	DWORD i;

	ClearPlrRVars();

	if (FirstTime) {
		data._pRSplType = RSPLTYPE_INVALID;
		data._pRSpell = SPL_INVALID;
		data._pSBkSpell = SPL_INVALID;
		data._pSpell = data._pRSpell;
		data._pSplType = data._pRSplType;
		if ((data._pgfxnum & 0xF) == ANIM_ID_BOW) {
			data._pwtype = WT_RANGED;
		} else {
			data._pwtype = WT_MELEE;
		}
		data.pManaShield = FALSE;
	}

	if (data.plrlevel == level.currlevel || leveldebug) {

		SetPlrAnims();

		data._poff = { 0, 0 };
		data._pvel = { 0, 0 };
		ClearPlrPVars();

		if (data._pHitPoints >> 6 > 0) {
			data._pmode = PM_STAND;
			NewPlrAnim(data._pNAnim[DIR_S], data._pNFrames, 3, data._pNWidth);
			data._pAnimFrame = random_(2, data._pNFrames - 1) + 1;
			data._pAnimCnt = random_(2, 3);
		} else {
			data._pmode = PM_DEATH;
			NewPlrAnim(data._pDAnim[DIR_S], data._pDFrames, 1, data._pDWidth);
			data._pAnimFrame = data._pAnimLen - 1;
			data._pVar8 = 2 * data._pAnimLen;
		}

		data._pdir = DIR_S;

		if (pnum == myplr()) {
			if (!FirstTime || level.currlevel != 0) {
				data._p = View;
			}
			data._ptarg = data._p;
		} else {
			data._ptarg = data._p;
			for (i = 0; i < 8 && !PosOkPlayer(plrxoff2[i] + data._p); i++)
				;
			data._p.x += plrxoff2[i];
			data._p.y += plryoff2[i];
		}

		data._pfut = data._p;
		data.walkpath[0] = WALK_NONE;
		data.destAction = ACTION_NONE;

		if (pnum == myplr()) {
			data._plid = AddLight(data._p, data._pLightRad);
		} else {
			data._plid = -1;
		}
		data._pvid = AddVision(data._p, data._pLightRad, pnum == myplr());
	}

	if (data._pClass == PC_WARRIOR) {
		data._pAblSpells = 1 << (SPL_REPAIR - 1);
	#ifndef SPAWN
	} else if (data._pClass == PC_ROGUE) {
		data._pAblSpells = 1 << (SPL_DISARM - 1);
	} else if (data._pClass == PC_SORCERER) {
		data._pAblSpells = 1 << (SPL_RECHARGE - 1);
	#endif
	}

	#ifdef _DEBUG
	if (debug_mode_dollar_sign && FirstTime) {
		data._pMemSpells |= 1 << (SPL_TELEPORT - 1);
		if (!data._pSplLvl[SPL_TELEPORT]) {
			data._pSplLvl[SPL_TELEPORT] = 1;
		}
	}
	if (debug_mode_key_inverted_v && FirstTime) {
		data._pMemSpells = SPL_INVALID;
	}
	#endif

	data._pNextExper = ExpLvlsTbl[data._pLevel];
	data._pInvincible = FALSE;

	if (pnum == myplr()) {
		deathdelay = 0;
		deathflag = FALSE;
		ScrollInfo._soff = { 0, 0 };
		ScrollInfo._sdir = SDIR_NONE;
	}
}

void InitMultiView()
{
	if ((DWORD)myplr() >= MAX_PLRS) {
		app_fatal("InitPlayer: illegal player %d", myplr());
	}
	View = myplr().data._p;
}

BOOL SolidLoc(V2Di pos)
{
	if (pos.x < 0 || pos.y < 0 || pos.x >= MAXDUNX || pos.y >= MAXDUNY) {
		return FALSE;
	}
	return pieces[grid.at(pos).dPiece].nSolidTable;
}

BOOL Player::PlrDirOK(int dir)
{
	BOOL isOk;
	V2Di p = data._p + offset[dir];

	if (p.x < 0 || !grid.at(p).dPiece || !PosOkPlayer(p)) {
		return FALSE;
	}

	isOk = TRUE;
	if (dir == DIR_E) {
		isOk = !SolidLoc({ p.x, p.y + 1 }) && !(grid[p.x][p.y + 1].dFlags & BFLAG_PLAYERLR);
	}

	if (isOk && dir == DIR_W) {
		isOk = !SolidLoc({ p.x + 1, p.y }) && !(grid[p.x + 1][p.y].dFlags & BFLAG_PLAYERLR);
	}

	return isOk;
}

void PlrClrTrans(V2Di pos)
{
	for (int i = pos.y - 1; i <= pos.y + 1; i++) {
		for (int j = pos.x - 1; j <= pos.x + 1; j++) {
			TransList[grid[j][i].dTransVal] = FALSE;
		}
	}
}

void PlrDoTrans(V2Di pos)
{
	if (level.leveltype != DTYPE_CATHEDRAL && level.leveltype != DTYPE_CATACOMBS) {
		TransList[1] = TRUE;
	} else {
		for (int i = pos.y - 1; i <= pos.y + 1; i++) {
			for (int j = pos.x - 1; j <= pos.x + 1; j++) {
				if (!pieces[grid[j][i].dPiece].nSolidTable && grid[j][i].dTransVal) {
					TransList[grid[j][i].dTransVal] = TRUE;
				}
			}
		}
	}
}

void Player::SetPlayerOld()
{
	data._pold = data._p;
}

void Player::FixPlayerLocation(int bDir)
{
	data._pfut = data._p;
	data._ptarg = data._p;
	data._poff = { 0, 0 };
	data._pdir = bDir;
	if (pnum == myplr()) {
		ScrollInfo._soff = { 0, 0 };
		ScrollInfo._sdir = SDIR_NONE;
		View = data._p;
	}
}

void Player::StartStand(int dir)
{
	if (!data._pInvincible || data._pHitPoints || pnum != myplr()) {
		if (!(data._pGFXLoad & PFILE_STAND)) {
			LoadPlrGFX(PFILE_STAND);
		}

		NewPlrAnim(data._pNAnim[dir], data._pNFrames, 3, data._pNWidth);
		data._pmode = PM_STAND;
		FixPlayerLocation(dir);
		FixPlrWalkTags();
		grid.at(data._p).dPlayer = pnum + 1;
		SetPlayerOld();
	} else {
		SyncPlrKill(-1);
	}
}

void Player::StartWalkStand()
{
	data._pmode = PM_STAND;
	data._pfut = data._p;
	data._poff = { 0, 0 };

	if (pnum == myplr()) {
		ScrollInfo._soff = { 0, 0 };
		ScrollInfo._sdir = SDIR_NONE;
		View = data._p;
	}
}

void Player::PM_ChangeLightOff()
{
	V2Di p, mul, lv, off;
	const LightListStruct *l;

	// check if issue is upstream
	if (data._plid == -1) return;

	l = &LightList[data._plid];
	p.x = 2 * data._poff.y + data._poff.x;
	p.y = 2 * data._poff.y - data._poff.x;
	if (p.x < 0) {
		mul.x = -1;
		p.x = -p.x;
	} else {
		mul.x = 1;
	}
	if (p.y < 0) {
		mul.y = -1;
		p.y = -p.y;
	} else {
		mul.y = 1;
	}

	p.x = (p.x >> 3) * mul.x;
	p.y = (p.y >> 3) * mul.y;
	lv.x = p.x + (l->_l.x << 3);
	lv.y = p.y + (l->_l.y << 3);
	off.x = l->_off.x + (l->_l.x << 3);
	off.y = l->_off.y + (l->_l.y << 3);

	if (abs(lv.x - off.x) < 3 && abs(lv.y - off.y) < 3)
		return;

	ChangeLightOff(data._plid, p);
}

void Player::PM_ChangeOffset()
{
	V2Di p;
	data._pVar8++;
	p.x = data._pVar6 / 256;
	p.y = data._pVar7 / 256;

	data._pVar6 += data._pvel.x;
	data._pVar7 += data._pvel.y;
	data._poff.x = data._pVar6 / 256;
	data._poff.y = data._pVar7 / 256;

	p.x -= data._pVar6 >> 8;
	p.y -= data._pVar7 >> 8;

	if (pnum == myplr() && ScrollInfo._sdir) {
		ScrollInfo._soff += p;
	}

	PM_ChangeLightOff();
}

void Player::StartWalk(V2Di vel, V2Di add, int EndDir, int sdir)
{
	if (data._pInvincible && !data._pHitPoints && pnum == myplr()) {
		SyncPlrKill(-1);
		return;
	}

	SetPlayerOld();
	V2Di p = add + data._p;
	if (!PlrDirOK(EndDir)) return;
	data._pfut = p;

	if (pnum == myplr()) {
		ScrollInfo._sd = data._p - View;
	}

	grid.at(p).dPlayer = -(pnum + 1);
	data._pmode = PM_WALK;
	data._pvel = vel;
	data._poff = { 0, 0 };
	data._pVar1 = add.x;
	data._pVar2 = add.y;
	data._pVar3 = EndDir;

	if (!(data._pGFXLoad & PFILE_WALK)) {
		LoadPlrGFX(PFILE_WALK);
	}

	NewPlrAnim(data._pWAnim[EndDir], data._pWFrames, 0, data._pWWidth);

	data._pdir = EndDir;
	data._pVar6 = 0;
	data._pVar7 = 0;
	data._pVar8 = 0;

	if (pnum != myplr()) {
		return;
	}

	int dist = (ScrollInfo._sd).maxabs();
	if (zoomflag) {

		if (dist >= 3) {
			ScrollInfo._sdir = SDIR_NONE;
		} else {
			ScrollInfo._sdir = sdir;
		}
	} else if (dist >= 2) {
		ScrollInfo._sdir = SDIR_NONE;
	} else {
		ScrollInfo._sdir = sdir;
	}
}

#if defined(__clang__) || defined(__GNUC__)
__attribute__((no_sanitize("shift-base")))
#endif
void Player::StartWalk2(V2Di vel, V2Di off, V2Di add, int EndDir, int sdir)
{
	V2Di p = add + data._p;
	if (data._pInvincible && !data._pHitPoints && pnum == myplr()) {
		SyncPlrKill(-1);
		return;
	}

	SetPlayerOld();
	if (!PlrDirOK(EndDir)) {
		return;
	}

	data._pfut = p;

	if (pnum == myplr()) {
		ScrollInfo._sd = data._p - View;
	}

	grid.at(data._p).dPlayer = -1 - pnum;
	data._pVar1 = data._p.x;
	data._pVar2 = data._p.y;
	data._p = p;
	grid.at(data._p).dPlayer = pnum + 1;
	data._poff = off;

	ChangeLightXY(data._plid, data._p);
	PM_ChangeLightOff();

	data._pmode = PM_WALK2;
	data._pvel = vel;
	data._pVar6 = off.x * 256;
	data._pVar7 = off.y * 256;
	data._pVar3 = EndDir;

	if (!(data._pGFXLoad & PFILE_WALK)) {
		LoadPlrGFX(PFILE_WALK);
	}
	NewPlrAnim(data._pWAnim[EndDir], data._pWFrames, 0, data._pWWidth);

	data._pdir = EndDir;
	data._pVar8 = 0;

	if (pnum != myplr()) {
		return;
	}

	int dist = (ScrollInfo._sd).maxabs();
	if (zoomflag) {
		if (dist >= 3) {
			ScrollInfo._sdir = SDIR_NONE;
		} else {
			ScrollInfo._sdir = sdir;
		}
	} else if (dist >= 2) {
		ScrollInfo._sdir = SDIR_NONE;
	} else {
		ScrollInfo._sdir = sdir;
	}
}

#if defined(__clang__) || defined(__GNUC__)
__attribute__((no_sanitize("shift-base")))
#endif
void Player::StartWalk3(V2Di vel, V2Di off, V2Di add, V2Di map, int EndDir, int sdir)
{
	V2Di p, n;

	if (data._pInvincible && !data._pHitPoints && pnum == myplr()) {
		SyncPlrKill(-1);
		return;
	}

	SetPlayerOld();
	p = add + data._p;
	n = map + data._p;

	if (!PlrDirOK(EndDir)) {
		return;
	}

	data._pfut = p;

	if (pnum == myplr()) {
		ScrollInfo._sd = data._p - View;
	}

	grid.at(data._p).dPlayer = -1 - pnum;
	grid.at(p).dPlayer = -1 - pnum;
	data._pVar4 = n.x;
	data._pVar5 = n.y;
	grid.at(n).dFlags |= BFLAG_PLAYERLR;
	data._poff = off;

	if (level.leveltype != DTYPE_TOWN) {
		ChangeLightXY(data._plid, n);
		PM_ChangeLightOff();
	}

	data._pmode = PM_WALK3;
	data._pvel = vel;
	data._pVar1 = p.x;
	data._pVar2 = p.y;
	data._pVar6 = off.x * 256;
	data._pVar7 = off.y * 256;
	data._pVar3 = EndDir;

	if (!(data._pGFXLoad & PFILE_WALK)) {
		LoadPlrGFX(PFILE_WALK);
	}
	NewPlrAnim(data._pWAnim[EndDir], data._pWFrames, 0, data._pWWidth);

	data._pdir = EndDir;
	data._pVar8 = 0;

	if (pnum != myplr()) {
		return;
	}
	int dist = (ScrollInfo._sd).maxabs();
	if (zoomflag) {
		if (dist >= 3) {
			ScrollInfo._sdir = SDIR_NONE;
		} else {
			ScrollInfo._sdir = sdir;
		}
	} else if (dist >= 2) {
		ScrollInfo._sdir = SDIR_NONE;
	} else {
		ScrollInfo._sdir = sdir;
	}
}

void Player::StartAttack(int d)
{

	if (data._pInvincible && !data._pHitPoints && pnum == myplr()) {
		SyncPlrKill(-1);
		return;
	}

	if (!(data._pGFXLoad & PFILE_ATTACK)) {
		LoadPlrGFX(PFILE_ATTACK);
	}

	NewPlrAnim(data._pAAnim[d], data._pAFrames, 0, data._pAWidth);
	data._pmode = PM_ATTACK;
	FixPlayerLocation(d);
	SetPlayerOld();
}

void Player::StartRangeAttack(int d, V2Di c)
{
	if (data._pInvincible && !data._pHitPoints && pnum == myplr()) {
		SyncPlrKill(-1);
		return;
	}

	if (!(data._pGFXLoad & PFILE_ATTACK)) {
		LoadPlrGFX(PFILE_ATTACK);
	}
	NewPlrAnim(data._pAAnim[d], data._pAFrames, 0, data._pAWidth);

	data._pmode = PM_RATTACK;
	FixPlayerLocation(d);
	SetPlayerOld();
	data._pVar1 = c.x;
	data._pVar2 = c.y;
}

void Player::StartPlrBlock(int dir)
{

	if (data._pInvincible && !data._pHitPoints && pnum == myplr()) {
		SyncPlrKill(-1);
		return;
	}

	PlaySfxLoc(IS_ISWORD, data._p);

	if (!(data._pGFXLoad & PFILE_BLOCK)) {
		LoadPlrGFX(PFILE_BLOCK);
	}
	NewPlrAnim(data._pBAnim[dir], data._pBFrames, 2, data._pBWidth);

	data._pmode = PM_BLOCK;
	FixPlayerLocation(dir);
	SetPlayerOld();
}

void Player::StartSpell(int d, V2Di c)
{
	if (data._pInvincible && !data._pHitPoints && pnum == myplr()) {
		SyncPlrKill(-1);
		return;
	}

	if (level.leveltype != DTYPE_TOWN) {
		switch (spelldata[data._pSpell].sType) {
		case STYPE_FIRE:
			if (!(data._pGFXLoad & PFILE_FIRE)) {
				LoadPlrGFX(PFILE_FIRE);
			}
			NewPlrAnim(data._pFAnim[d], data._pSFrames, 0, data._pSWidth);
			break;
		case STYPE_LIGHTNING:
			if (!(data._pGFXLoad & PFILE_LIGHTNING)) {
				LoadPlrGFX(PFILE_LIGHTNING);
			}
			NewPlrAnim(data._pLAnim[d], data._pSFrames, 0, data._pSWidth);
			break;
		case STYPE_MAGIC:
			if (!(data._pGFXLoad & PFILE_MAGIC)) {
				LoadPlrGFX(PFILE_MAGIC);
			}
			NewPlrAnim(data._pTAnim[d], data._pSFrames, 0, data._pSWidth);
			break;
		}
	}

	PlaySfxLoc(spelldata[data._pSpell].sSFX, data._p);

	data._pmode = PM_SPELL;

	FixPlayerLocation(d);
	SetPlayerOld();

	data._pVar1 = c.x;
	data._pVar2 = c.y;
	data._pVar4 = GetSpellLevel(pnum, data._pSpell);
	data._pVar8 = 1;
}

void Player::FixPlrWalkTags()
{
	int pp, pn;
	V2Di d, n;

	pp = pnum + 1;
	pn = -(pnum + 1);
	d = data._pold;
	for (int y = d.y - 1; y <= d.y + 1; y++) {
		for (int x = d.x - 1; x <= d.x + 1; x++) {
			if (x >= 0 && x < MAXDUNX && y >= 0 && y < MAXDUNY && (grid[x][y].dPlayer == pp || grid[x][y].dPlayer == pn)) {
				grid[x][y].dPlayer = 0;
			}
		}
	}

	if (d.x >= 0 && d.x < MAXDUNX - 1 && d.y >= 0 && d.y < MAXDUNY - 1) {
		grid[d.x + 1][d.y].dFlags &= ~BFLAG_PLAYERLR;
		grid[d.x][d.y + 1].dFlags &= ~BFLAG_PLAYERLR;
	}
}

void Player::RemovePlrFromMap()
{
	int x, y;
	int pp, pn;

	pp = pnum + 1;
	pn = -(pnum + 1);

	for (y = 1; y < MAXDUNY; y++)
		for (x = 1; x < MAXDUNX; x++)
			if (grid[x][y - 1].dPlayer == pn || grid[x - 1][y].dPlayer == pn)
				if (grid[x][y].dFlags & BFLAG_PLAYERLR)
					grid[x][y].dFlags &= ~BFLAG_PLAYERLR;

	for (y = 0; y < MAXDUNY; y++)
		for (x = 0; x < MAXDUNX; x++)
			if (grid[x][y].dPlayer == pp || grid[x][y].dPlayer == pn)
				grid[x][y].dPlayer = 0;
}

void Player::StartPlrHit(int dam, BOOL forcehit)
{
	int pd;

	if (data._pInvincible && !data._pHitPoints && pnum == myplr()) {
		SyncPlrKill(-1);
		return;
	}

	if (data._pClass == PC_WARRIOR) {
		PlaySfxLoc(PS_WARR69, data._p);
#ifndef SPAWN
	} else if (data._pClass == PC_ROGUE) {
		PlaySfxLoc(PS_ROGUE69, data._p);
	} else if (data._pClass == PC_SORCERER) {
		PlaySfxLoc(PS_MAGE69, data._p);
#endif
	}

	drawhpflag = TRUE;
	if (dam >> 6 >= data._pLevel || forcehit) {
		pd = data._pdir;

		if (!(data._pGFXLoad & PFILE_HIT)) {
			LoadPlrGFX(PFILE_HIT);
		}
		NewPlrAnim(data._pHAnim[pd], data._pHFrames, 0, data._pHWidth);

		data._pmode = PM_GOTHIT;
		FixPlayerLocation(pd);
		data._pVar8 = 1;
		FixPlrWalkTags();
		grid.at(data._p).dPlayer = pnum + 1;
		SetPlayerOld();
	}
}

void RespawnDeadItem(ItemStruct *itm, V2Di pos)
{
	int ii;
	if (numitems >= MAXITEMS) return;

	if (FindGetItem(itm->IDidx, itm->_iCreateInfo, itm->_iSeed) >= 0) {
		DrawInvMsg("A duplicate item has been detected.  Destroying duplicate...");
		SyncGetItem(pos, itm->IDidx, itm->_iCreateInfo, itm->_iSeed);
	}

	ii = itemavail[0];
	grid.at(pos).dItem = ii + 1;
	itemavail[0] = itemavail[MAXITEMS - numitems - 1];
	itemactive[numitems] = ii;
	item[ii] = *itm;
	item[ii]._i = pos;
	RespawnItem(ii, TRUE);
	numitems++;
	itm->_itype = ITYPE_NONE;
}

#if defined(__clang__) || defined(__GNUC__)
__attribute__((no_sanitize("shift-base")))
#endif
void Player::StartPlayerKill(int earflag)
{
	BOOL diablolevel;
	int i, pdd;
	ItemStruct ear;
	ItemStruct *pi;

	if (data._pHitPoints <= 0 && data._pmode == PM_DEATH) {
		return;
	}

	if (myplr() == pnum) {
		NetSendCmdParam1(TRUE, CMD_PLRDEAD, earflag);
	}

	diablolevel = gbMaxPlayers > 1 && data.plrlevel == 16;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("StartPlayerKill: illegal player %d", pnum);
	}

	if (data._pClass == PC_WARRIOR) {
		PlaySfxLoc(PS_DEAD, data._p); // BUGFIX: should use `PS_WARR71` like other classes
	#ifndef SPAWN
	} else if (data._pClass == PC_ROGUE) {
		PlaySfxLoc(PS_ROGUE71, data._p);
	} else if (data._pClass == PC_SORCERER) {
		PlaySfxLoc(PS_MAGE71, data._p);
	#endif
	}

	if (data._pgfxnum) {
		data._pgfxnum = 0;
		data._pGFXLoad = 0;
		SetPlrAnims();
	}

	if (!(data._pGFXLoad & PFILE_DEATH)) {
		LoadPlrGFX(PFILE_DEATH);
	}

	NewPlrAnim(data._pDAnim[data._pdir], data._pDFrames, 1, data._pDWidth);

	data._pBlockFlag = FALSE;
	data._pmode = PM_DEATH;
	data._pInvincible = TRUE;
	SetPlayerHitPoints(0);
	data._pVar8 = 1;

	if (pnum != myplr() && !earflag && !diablolevel) {
		for (i = 0; i < NUM_INVLOC; i++) {
			data.InvBody[i]._itype = ITYPE_NONE;
		}
		CalcPlrInv(pnum, FALSE);
	}

	if (data.plrlevel == level.currlevel) {
		FixPlayerLocation(data._pdir);
		RemovePlrFromMap();
		grid.at(data._p).dFlags |= BFLAG_DEAD_PLAYER;
		SetPlayerOld();

		if (pnum == myplr()) {
			drawhpflag = TRUE;
			deathdelay = 30;

			if (pcurs >= CURSOR_FIRSTITEM) {
				PlrDeadItem(&data.HoldItem, { 0, 0 });
				SetCursor_(CURSOR_HAND);
			}

			if (!diablolevel) {
				DropHalfPlayersGold();
				if (earflag != -1) {
					if (earflag != 0) {
						SetPlrHandItem(&ear, IDI_EAR);
						sprintf(ear._iName, "Ear of %s", data._pName);
						if (data._pClass == PC_SORCERER) {
							ear._iCurs = ICURS_EAR_SORCEROR;
						} else if (data._pClass == PC_WARRIOR) {
							ear._iCurs = ICURS_EAR_WARRIOR;
						} else if (data._pClass == PC_ROGUE) {
							ear._iCurs = ICURS_EAR_ROGUE;
						}

						ear._iCreateInfo = data._pName[0] << 8 | data._pName[1];
						ear._iSeed = data._pName[2] << 24 | data._pName[3] << 16 | data._pName[4] << 8 | data._pName[5];
						ear._ivalue = data._pLevel;

						if (FindGetItem(IDI_EAR, ear._iCreateInfo, ear._iSeed) == -1) {
							PlrDeadItem(&ear, { 0, 0 });
						}
					} else {
						pi = &data.InvBody[0];
						i = NUM_INVLOC;
						while (i--) {
							pdd = (i + data._pdir) & 7;
							PlrDeadItem(pi, offset[pdd]);
							pi++;
						}

						CalcPlrInv(pnum, FALSE);
					}
				}
			}
		}
	}
	SetPlayerHitPoints(0);
}

void Player::PlrDeadItem(ItemStruct *itm, V2Di n)
{
	if (itm->_itype == ITYPE_NONE) return;

	V2Di p = n + data._p;
	if ((n.x || n.y) && ItemSpaceOk(p.x, p.y)) {
		RespawnDeadItem(itm, p);
		data.HoldItem = *itm;
		NetSendCmdPItem(FALSE, CMD_RESPAWNITEM, p.x, p.y);
		return;
	}

	for (int k = 1; k < 50; k++) {
		for (int j = -k; j <= k; j++) {
			p.y = j + data._p.y;
			for (int i = -k; i <= k; i++) {
				p.x = i + data._p.x;
				if (ItemSpaceOk(p.x, p.y)) {
					RespawnDeadItem(itm, p);
					data.HoldItem = *itm;
					NetSendCmdPItem(FALSE, CMD_RESPAWNITEM, p.x, p.y);
					return;
				}
			}
		}
	}
}

void Player::DropHalfPlayersGold()
{
	int i, hGold;

	hGold = data._pGold >> 1;
	for (i = 0; i < MAXBELTITEMS && hGold > 0; i++) {
		if (data.SpdList[i]._itype == ITYPE_GOLD && data.SpdList[i]._ivalue != GOLD_MAX_LIMIT) {
			if (hGold < data.SpdList[i]._ivalue) {
				data.SpdList[i]._ivalue -= hGold;
				SetSpdbarGoldCurs(pnum, i);
				SetPlrHandItem(&data.HoldItem, IDI_GOLD);
				GetGoldSeed(pnum, &data.HoldItem);
				SetPlrHandGoldCurs(&data.HoldItem);
				data.HoldItem._ivalue = hGold;
				PlrDeadItem(&data.HoldItem, { 0, 0 });
				hGold = 0;
			} else {
				hGold -= data.SpdList[i]._ivalue;
				plr[pnum].inventory.RemoveSpdBarItem(i);
				SetPlrHandItem(&data.HoldItem, IDI_GOLD);
				GetGoldSeed(pnum, &data.HoldItem);
				SetPlrHandGoldCurs(&data.HoldItem);
				data.HoldItem._ivalue = data.SpdList[i]._ivalue;
				PlrDeadItem(&data.HoldItem, { 0, 0 });
				i = -1;
			}
		}
	}
	if (hGold > 0) {
		for (i = 0; i < MAXBELTITEMS && hGold > 0; i++) {
			if (data.SpdList[i]._itype == ITYPE_GOLD) {
				if (hGold < data.SpdList[i]._ivalue) {
					data.SpdList[i]._ivalue -= hGold;
					SetSpdbarGoldCurs(pnum, i);
					SetPlrHandItem(&data.HoldItem, IDI_GOLD);
					GetGoldSeed(pnum, &data.HoldItem);
					SetPlrHandGoldCurs(&data.HoldItem);
					data.HoldItem._ivalue = hGold;
					PlrDeadItem(&data.HoldItem, { 0, 0 });
					hGold = 0;
				} else {
					hGold -= data.SpdList[i]._ivalue;
					plr[pnum].inventory.RemoveSpdBarItem(i);
					SetPlrHandItem(&data.HoldItem, IDI_GOLD);
					GetGoldSeed(pnum, &data.HoldItem);
					SetPlrHandGoldCurs(&data.HoldItem);
					data.HoldItem._ivalue = data.SpdList[i]._ivalue;
					PlrDeadItem(&data.HoldItem, { 0, 0 });
					i = -1;
				}
			}
		}
	}
	force_redraw = 255;
	if (hGold > 0) {
		for (i = 0; i < data._pNumInv && hGold > 0; i++) {
			if (data.InvList[i]._itype == ITYPE_GOLD && data.InvList[i]._ivalue != GOLD_MAX_LIMIT) {
				if (hGold < data.InvList[i]._ivalue) {
					data.InvList[i]._ivalue -= hGold;
					SetGoldCurs(pnum, i);
					SetPlrHandItem(&data.HoldItem, IDI_GOLD);
					GetGoldSeed(pnum, &data.HoldItem);
					SetPlrHandGoldCurs(&data.HoldItem);
					data.HoldItem._ivalue = hGold;
					PlrDeadItem(&data.HoldItem, { 0, 0 });
					hGold = 0;
				} else {
					hGold -= data.InvList[i]._ivalue;
					plr[pnum].inventory.RemoveInvItem(i);
					SetPlrHandItem(&data.HoldItem, IDI_GOLD);
					GetGoldSeed(pnum, &data.HoldItem);
					SetPlrHandGoldCurs(&data.HoldItem);
					data.HoldItem._ivalue = data.InvList[i]._ivalue;
					PlrDeadItem(&data.HoldItem, { 0, 0 });
					i = -1;
				}
			}
		}
	}
	if (hGold > 0) {
		for (i = 0; i < data._pNumInv && hGold > 0; i++) {
			if (data.InvList[i]._itype == ITYPE_GOLD) {
				if (hGold < data.InvList[i]._ivalue) {
					data.InvList[i]._ivalue -= hGold;
					SetGoldCurs(pnum, i);
					SetPlrHandItem(&data.HoldItem, IDI_GOLD);
					GetGoldSeed(pnum, &data.HoldItem);
					SetPlrHandGoldCurs(&data.HoldItem);
					data.HoldItem._ivalue = hGold;
					PlrDeadItem(&data.HoldItem, { 0, 0 });
					hGold = 0;
				} else {
					hGold -= data.InvList[i]._ivalue;
					plr[pnum].inventory.RemoveInvItem(i);
					SetPlrHandItem(&data.HoldItem, IDI_GOLD);
					GetGoldSeed(pnum, &data.HoldItem);
					SetPlrHandGoldCurs(&data.HoldItem);
					data.HoldItem._ivalue = data.InvList[i]._ivalue;
					PlrDeadItem(&data.HoldItem, { 0, 0 });
					i = -1;
				}
			}
		}
	}
	data._pGold = plr[pnum].inventory.CalculateGold();
}

void Player::SyncPlrKill(int earflag)
{
	int ma, i;

	if (data._pHitPoints == 0 && level.currlevel == 0) {
		SetPlayerHitPoints(64);
		return;
	}

	for (i = 0; i < nummissiles; i++) {
		ma = missileactive[i];
		if (missile[ma]._mitype == MIS_MANASHIELD && missile[ma]._misource == pnum && missile[ma]._miDelFlag == FALSE) {
			if (earflag != -1) {
				missile[ma]._miVar8 = earflag;
			}

			return;
		}
	}

	SetPlayerHitPoints(0);
	StartPlayerKill(earflag);
}

void Player::RemovePlrMissiles()
{
	int i, am;
	V2Di m;

	if (level.currlevel != 0 && pnum == myplr() && (monsters[myplr()].data._m.x != 1 || monsters[myplr()].data._m.y != 0)) {
		monsters[myplr()].M_StartKill(myplr());
		AddDead(monsters[myplr()].data._m, (monsters[myplr()].data.MType)->mdeadval, monsters[myplr()].data._mdir);
		m = monsters[myplr()].data._m;
		grid.at(m).dMonster = 0;
		monsters[myplr()].data._mDelFlag = TRUE;
		DeleteMonsterList();
	}

	for (i = 0; i < nummissiles; i++) {
		am = missileactive[i];
		if (missile[am]._mitype == MIS_STONE && missile[am]._misource == pnum) {
			monsters[missile[am]._miVar2].data._mmode = missile[am]._miVar1;
		}
		if (missile[am]._mitype == MIS_MANASHIELD && missile[am]._misource == pnum) {
			ClearMissileSpot(am);
			DeleteMissile(am, i);
		}
		if (missile[am]._mitype == MIS_ETHEREALIZE && missile[am]._misource == pnum) {
			ClearMissileSpot(am);
			DeleteMissile(am, i);
		}
	}
}

void Player::InitLevelChange()
{
	RemovePlrMissiles();
	if (pnum == myplr() && qtextflag) {
		qtextflag = FALSE;
		stream_stop();
	}

	RemovePlrFromMap();
	SetPlayerOld();
	if (pnum == myplr()) {
		grid.at(data._p).dPlayer = myplr() + 1;
	} else {
		data._pLvlVisited[data.plrlevel] = TRUE;
	}

	ClrPlrPath();
	data.destAction = ACTION_NONE;
	data._pLvlChanging = TRUE;

	if (pnum == myplr()) {
		data.pLvlLoad = 10;
	}
}

#if defined(__clang__) || defined(__GNUC__)
__attribute__((no_sanitize("shift-base")))
#endif
void Player::StartNewLvl(int fom, int lvl)
{
	InitLevelChange();

	switch (fom) {
	case WM_DIABNEXTLVL:
	case WM_DIABPREVLVL:
		data.plrlevel = lvl;
		break;
	case WM_DIABRTNLVL:
	case WM_DIABTOWNWARP:
		data.plrlevel = lvl;
		break;
	case WM_DIABSETLVL:
		level.setlvlnum = lvl;
		break;
	case WM_DIABTWARPUP:
		data.pTownWarps |= 1 << (level.leveltype - 2);
		data.plrlevel = lvl;
		break;
	case WM_DIABRETOWN:
		break;
	default:
		app_fatal("StartNewLvl");
		break;
	}

	if (pnum == myplr()) {
		data._pmode = PM_NEWLVL;
		data._pInvincible = TRUE;
		PostMessage(fom, 0, 0);
		if (gbMaxPlayers > 1) {
			NetSendCmdParam2(TRUE, CMD_NEWLVL, fom, lvl);
		}
	}
}
void Player::RestartTownLvl()
{
	InitLevelChange();

	data.plrlevel = 0;
	data._pInvincible = FALSE;

	SetPlayerHitPoints(64);

	data._pMana = 0;
	data._pManaBase = data._pMana - (data._pMaxMana - data._pMaxManaBase);

	CalcPlrInv(pnum, FALSE);

	if (pnum == myplr()) {
		data._pmode = PM_NEWLVL;
		data._pInvincible = TRUE;
		PostMessage(WM_DIABRETOWN, 0, 0);
	}
}

void Player::StartWarpLvl(int pidx)
{
	InitLevelChange();

	if (gbMaxPlayers != 1) {
		if (data.plrlevel != 0) {
			data.plrlevel = 0;
		} else {
			data.plrlevel = portal[pidx].level;
		}
	}

	if (pnum == myplr()) {
		SetCurrentPortal(pidx);
		data._pmode = PM_NEWLVL;
		data._pInvincible = TRUE;
		PostMessage(WM_DIABWARPLVL, 0, 0);
	}
}

BOOL Player::PM_DoStand()
{
	return FALSE;
}

BOOL Player::PM_DoWalk()
{
	int anim_len;
	BOOL rv;

	if (data._pAnimFrame == 3
	    || (data._pWFrames == 8 && data._pAnimFrame == 7)
	    || (data._pWFrames != 8 && data._pAnimFrame == 4)) {
		PlaySfxLoc(PS_WALK1, data._p);
	}

	anim_len = 8;
	if (level.currlevel != 0) {
		anim_len = classes[data._pClass].AnimLenFromClass;
	}

	if (data._pVar8 == anim_len) {
		grid.at(data._p).dPlayer = 0;
		data._p.x += data._pVar1;
		data._p.y += data._pVar2;
		grid.at(data._p).dPlayer = pnum + 1;

		if (level.leveltype != DTYPE_TOWN) {
			ChangeLightXY(data._plid, data._p);
			ChangeVisionXY(data._pvid, data._p);
		}

		if (pnum == myplr() && ScrollInfo._sdir) {
			View = data._p - ScrollInfo._sd;
		}

		if (data.walkpath[0] != WALK_NONE) {
			StartWalkStand();
		} else {
			StartStand(data._pVar3);
		}

		ClearPlrPVars();

		if (level.leveltype != DTYPE_TOWN) {
			ChangeLightOff(data._plid, { 0, 0 });
		}
		rv = TRUE;
	} else {
		PM_ChangeOffset();
		rv = FALSE;
	}

	return rv;
}

BOOL Player::PM_DoWalk2()
{
	int anim_len;
	BOOL rv;

	if (data._pAnimFrame == 3
	    || (data._pWFrames == 8 && data._pAnimFrame == 7)
	    || (data._pWFrames != 8 && data._pAnimFrame == 4)) {
		PlaySfxLoc(PS_WALK1, data._p);
	}

	anim_len = 8;
	if (level.currlevel != 0) {
		anim_len = classes[data._pClass].AnimLenFromClass;
	}

	if (data._pVar8 == anim_len) {
		grid[data._pVar1][data._pVar2].dPlayer = 0;

		if (level.leveltype != DTYPE_TOWN) {
			ChangeLightXY(data._plid, data._p);
			ChangeVisionXY(data._pvid, data._p);
		}

		if (pnum == myplr() && ScrollInfo._sdir) {
			View = data._p - ScrollInfo._sd;
		}

		if (data.walkpath[0] != WALK_NONE) {
			StartWalkStand();
		} else {
			StartStand(data._pVar3);
		}

		ClearPlrPVars();
		if (level.leveltype != DTYPE_TOWN) {
			ChangeLightOff(data._plid, { 0, 0 });
		}
		rv = TRUE;
	} else {
		PM_ChangeOffset();
		rv = FALSE;
	}

	return rv;
}

BOOL Player::PM_DoWalk3()
{
	int anim_len;
	BOOL rv;

	if (data._pAnimFrame == 3
	    || (data._pWFrames == 8 && data._pAnimFrame == 7)
	    || (data._pWFrames != 8 && data._pAnimFrame == 4)) {
		PlaySfxLoc(PS_WALK1, data._p);
	}

	anim_len = 8;
	if (level.currlevel != 0) {
		anim_len = classes[data._pClass].AnimLenFromClass;
	}

	if (data._pVar8 == anim_len) {
		grid.at(data._p).dPlayer = 0;
		grid[data._pVar4][data._pVar5].dFlags &= ~BFLAG_PLAYERLR;
		data._p.x = data._pVar1;
		data._p.y = data._pVar2;
		grid.at(data._p).dPlayer = pnum + 1;

		if (level.leveltype != DTYPE_TOWN) {
			ChangeLightXY(data._plid, data._p);
			ChangeVisionXY(data._pvid, data._p);
		}

		if (pnum == myplr() && ScrollInfo._sdir) {
			View = data._p - ScrollInfo._sd;
		}

		if (data.walkpath[0] != WALK_NONE) {
			StartWalkStand();
		} else {
			StartStand(data._pVar3);
		}

		ClearPlrPVars();

		if (level.leveltype != DTYPE_TOWN) {
			ChangeLightOff(data._plid, { 0, 0 });
		}
		rv = TRUE;
	} else {
		PM_ChangeOffset();
		rv = FALSE;
	}

	return rv;
}

BOOL Player::WeaponDur(int durrnd)
{
	if (pnum != myplr()) {
		return FALSE;
	}

	if (random_(3, durrnd) != 0) {
		return FALSE;
	}

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("WeaponDur: illegal player %d", pnum);
	}

	if (data.InvBody[INVLOC_HAND_LEFT]._itype != ITYPE_NONE && data.InvBody[INVLOC_HAND_LEFT]._iClass == ICLASS_WEAPON) {
		if (data.InvBody[INVLOC_HAND_LEFT]._iDurability == DUR_INDESTRUCTIBLE) {
			return FALSE;
		}

		data.InvBody[INVLOC_HAND_LEFT]._iDurability--;
		if (data.InvBody[INVLOC_HAND_LEFT]._iDurability == 0) {
			NetSendCmdDelItem(TRUE, INVLOC_HAND_LEFT);
			data.InvBody[INVLOC_HAND_LEFT]._itype = ITYPE_NONE;
			CalcPlrInv(pnum, TRUE);
			return TRUE;
		}
	}

	if (data.InvBody[INVLOC_HAND_RIGHT]._itype != ITYPE_NONE && data.InvBody[INVLOC_HAND_RIGHT]._iClass == ICLASS_WEAPON) {
		if (data.InvBody[INVLOC_HAND_RIGHT]._iDurability == DUR_INDESTRUCTIBLE) {
			return FALSE;
		}

		data.InvBody[INVLOC_HAND_RIGHT]._iDurability--;
		if (data.InvBody[INVLOC_HAND_RIGHT]._iDurability == 0) {
			NetSendCmdDelItem(TRUE, INVLOC_HAND_RIGHT);
			data.InvBody[INVLOC_HAND_RIGHT]._itype = ITYPE_NONE;
			CalcPlrInv(pnum, TRUE);
			return TRUE;
		}
	}

	if (data.InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_NONE && data.InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_SHIELD) {
		if (data.InvBody[INVLOC_HAND_RIGHT]._iDurability == DUR_INDESTRUCTIBLE) {
			return FALSE;
		}

		data.InvBody[INVLOC_HAND_RIGHT]._iDurability--;
		if (data.InvBody[INVLOC_HAND_RIGHT]._iDurability == 0) {
			NetSendCmdDelItem(TRUE, INVLOC_HAND_RIGHT);
			data.InvBody[INVLOC_HAND_RIGHT]._itype = ITYPE_NONE;
			CalcPlrInv(pnum, TRUE);
			return TRUE;
		}
	}

	if (data.InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_NONE && data.InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_SHIELD) {
		if (data.InvBody[INVLOC_HAND_LEFT]._iDurability == DUR_INDESTRUCTIBLE) {
			return FALSE;
		}

		data.InvBody[INVLOC_HAND_LEFT]._iDurability--;
		if (data.InvBody[INVLOC_HAND_LEFT]._iDurability == 0) {
			NetSendCmdDelItem(TRUE, INVLOC_HAND_LEFT);
			data.InvBody[INVLOC_HAND_LEFT]._itype = ITYPE_NONE;
			CalcPlrInv(pnum, TRUE);
			return TRUE;
		}
	}

	return FALSE;
}

BOOL Player::PlrHitMonst(int m)
{
	BOOL rv, ret;
	int hit, hper, mind, maxd, ddp, dam, skdam, phanditype, tmac;

	if ((DWORD)m >= MAXMONSTERS) {
		app_fatal("PlrHitMonst: illegal monster %d", m);
	}

	if ((monsters[m].data._mhitpoints >> 6) <= 0) {
		return FALSE;
	}

	if (monsters[m].data.MType->mtype == MT_ILLWEAV && monsters[m].data._mgoal == MGOAL_RETREAT) {
		return FALSE;
	}

	if (monsters[m].data._mmode == MM_CHARGE) {
		return FALSE;
	}

	rv = FALSE;

	hit = random_(4, 100);
	if (monsters[m].data._mmode == MM_STONE) {
		hit = 0;
	}

	tmac = monsters[m].data.mArmorClass - data._pIEnAc;
	hper = (data._pDexterity >> 1) + data._pLevel + 50 - tmac;
	if (data._pClass == PC_WARRIOR) {
		hper += 20;
	}
	hper += data._pIBonusToHit;
	if (hper < 5) {
		hper = 5;
	}
	if (hper > 95) {
		hper = 95;
	}

	if (CheckMonsterHit(m, &ret)) {
		return ret;
	}
#ifdef _DEBUG
	if (hit < hper || debug_mode_key_inverted_v || debug_mode_dollar_sign) {
#else
	if (hit < hper) {
#endif
		mind = data._pIMinDam;
		maxd = data._pIMaxDam;
		dam = random_(5, maxd - mind + 1) + mind;
		dam += dam * data._pIBonusDam / 100;
		dam += data._pDamageMod + data._pIBonusDamMod;
		if (data._pClass == PC_WARRIOR) {
			ddp = data._pLevel;
			if (random_(6, 100) < ddp) {
				dam *= 2;
			}
		}

		phanditype = ITYPE_NONE;
		if (data.InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_SWORD || data.InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_SWORD) {
			phanditype = ITYPE_SWORD;
		}
		if (data.InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_MACE || data.InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_MACE) {
			phanditype = ITYPE_MACE;
		}

		switch (monsters[m].data.MData->mMonstClass) {
		case MC_UNDEAD:
			if (phanditype == ITYPE_SWORD) {
				dam -= dam >> 1;
			}
			if (phanditype == ITYPE_MACE) {
				dam += dam >> 1;
			}
			break;
		case MC_ANIMAL:
			if (phanditype == ITYPE_MACE) {
				dam -= dam >> 1;
			}
			if (phanditype == ITYPE_SWORD) {
				dam += dam >> 1;
			}
			break;
		}

		if (data._pIFlags & ISPL_3XDAMVDEM && monsters[m].data.MData->mMonstClass == MC_DEMON) {
			dam *= 3;
		}

		dam <<= 6;
		if (pnum == myplr()) {
			monsters[m].data._mhitpoints -= dam;
		}

		if (data._pIFlags & ISPL_RNDSTEALLIFE) {
			skdam = random_(7, dam >> 3);
			data._pHitPoints += skdam;
			if (data._pHitPoints > data._pMaxHP) {
				data._pHitPoints = data._pMaxHP;
			}
			data._pHPBase += skdam;
			if (data._pHPBase > data._pMaxHPBase) {
				data._pHPBase = data._pMaxHPBase;
			}
			drawhpflag = TRUE;
		}
		if (data._pIFlags & (ISPL_STEALMANA_3 | ISPL_STEALMANA_5) && !(data._pIFlags & ISPL_NOMANA)) {
			if (data._pIFlags & ISPL_STEALMANA_3) {
				skdam = 3 * dam / 100;
			}
			if (data._pIFlags & ISPL_STEALMANA_5) {
				skdam = 5 * dam / 100;
			}
			data._pMana += skdam;
			if (data._pMana > data._pMaxMana) {
				data._pMana = data._pMaxMana;
			}
			data._pManaBase += skdam;
			if (data._pManaBase > data._pMaxManaBase) {
				data._pManaBase = data._pMaxManaBase;
			}
			drawmanaflag = TRUE;
		}
		if (data._pIFlags & (ISPL_STEALLIFE_3 | ISPL_STEALLIFE_5)) {
			if (data._pIFlags & ISPL_STEALLIFE_3) {
				skdam = 3 * dam / 100;
			}
			if (data._pIFlags & ISPL_STEALLIFE_5) {
				skdam = 5 * dam / 100;
			}
			data._pHitPoints += skdam;
			if (data._pHitPoints > data._pMaxHP) {
				data._pHitPoints = data._pMaxHP;
			}
			data._pHPBase += skdam;
			if (data._pHPBase > data._pMaxHPBase) {
				data._pHPBase = data._pMaxHPBase;
			}
			drawhpflag = TRUE;
		}
		if (data._pIFlags & ISPL_NOHEALPLR) {
			monsters[m].data._mFlags |= MFLAG_NOHEAL;
		}
#ifdef _DEBUG
		if (debug_mode_dollar_sign || debug_mode_key_inverted_v) {
			monsters[m].data._mhitpoints = 0; /* double check */
		}
#endif
		if ((monsters[m].data._mhitpoints >> 6) <= 0) {
			if (monsters[m].data._mmode == MM_STONE) {
				monsters[m].M_StartKill(pnum);
				monsters[m].data._mmode = MM_STONE;
			} else {
				monsters[m].M_StartKill(pnum);
			}
		} else {
			if (monsters[m].data._mmode == MM_STONE) {
				monsters[m].M_StartHit(pnum, dam);
				monsters[m].data._mmode = MM_STONE;
			} else {
				if (data._pIFlags & ISPL_KNOCKBACK) {
					monsters[m].M_GetKnockback();
				}
				monsters[m].M_StartHit(pnum, dam);
			}
		}
		rv = TRUE;
	}

	return rv;
}

BOOL Player::PlrHitPlr(char p)
{
	BOOL rv;
	int hit, hper, blk, blkper, dir, mind, maxd, dam, lvl, skdam, tac;

	if ((DWORD)p >= MAX_PLRS) {
		app_fatal("PlrHitPlr: illegal target player %d", p);
	}

	rv = FALSE;

	if (plr[p].data._pInvincible) {
		return rv;
	}

	if (plr[p].data._pSpellFlags & 1) {
		return rv;
	}

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("PlrHitPlr: illegal attacking player %d", pnum);
	}

	hit = random_(4, 100);

	hper = (data._pDexterity >> 1) + data._pLevel + 50 - (plr[p].data._pIBonusAC + plr[p].data._pIAC + plr[p].data._pDexterity / 5);

	if (data._pClass == PC_WARRIOR) {
		hper += 20;
	}
	hper += data._pIBonusToHit;
	if (hper < 5) {
		hper = 5;
	}
	if (hper > 95) {
		hper = 95;
	}

	if ((plr[p].data._pmode == PM_STAND || plr[p].data._pmode == PM_ATTACK) && plr[p].data._pBlockFlag) {
		blk = random_(5, 100);
	} else {
		blk = 100;
	}

	blkper = plr[p].data._pDexterity + plr[p].data._pBaseToBlk + (plr[p].data._pLevel << 1) - (data._pLevel << 1);
	if (blkper < 0) {
		blkper = 0;
	}
	if (blkper > 100) {
		blkper = 100;
	}

	if (hit < hper) {
		if (blk < blkper) {
			dir = GetDirection(plr[p].data._p, data._p);
			plr[p].StartPlrBlock(dir);
		} else {
			mind = data._pIMinDam;
			maxd = random_(5, data._pIMaxDam - mind + 1);
			dam = maxd + mind;
			dam += data._pDamageMod + data._pIBonusDamMod + dam * data._pIBonusDam / 100;

			if (data._pClass == PC_WARRIOR) {
				lvl = data._pLevel;
				if (random_(6, 100) < lvl) {
					dam *= 2;
				}
			}
			skdam = dam << 6;
			if (data._pIFlags & ISPL_RNDSTEALLIFE) {
				tac = random_(7, skdam >> 3);
				data._pHitPoints += tac;
				if (data._pHitPoints > data._pMaxHP) {
					data._pHitPoints = data._pMaxHP;
				}
				data._pHPBase += tac;
				if (data._pHPBase > data._pMaxHPBase) {
					data._pHPBase = data._pMaxHPBase;
				}
				drawhpflag = TRUE;
			}
			if (pnum == myplr()) {
				NetSendCmdDamage(TRUE, p, skdam);
			}
			plr[p].StartPlrHit(skdam, FALSE);
		}

		rv = TRUE;
	}

	return rv;
}

BOOL Player::PlrHitObj(V2Di m)
{
	int oi;

	if (grid.at(m).dObject > 0) {
		oi = grid.at(m).dObject - 1;
	} else {
		oi = -grid.at(m).dObject - 1;
	}

	if (object[oi]._oBreak == 1) {
		BreakObject(pnum, oi);
		return TRUE;
	}
	return FALSE;
}

BOOL Player::PM_DoAttack()
{
	int frame, dir, m;
	BOOL didhit;
	V2Di d;
	frame = data._pAnimFrame;
	if (data._pIFlags & ISPL_QUICKATTACK && frame == 1) {
		data._pAnimFrame++;
	}
	if (data._pIFlags & ISPL_FASTATTACK && (frame == 1 || frame == 3)) {
		data._pAnimFrame++;
	}
	if (data._pIFlags & ISPL_FASTERATTACK && (frame == 1 || frame == 3 || frame == 5)) {
		data._pAnimFrame++;
	}
	if (data._pIFlags & ISPL_FASTESTATTACK && (frame == 1 || frame == 4)) {
		data._pAnimFrame += 2;
	}
	if (data._pAnimFrame == data._pAFNum - 1) {
		PlaySfxLoc(PS_SWING, data._p);
	}

	if (data._pAnimFrame == data._pAFNum) {
		dir = data._pdir;
		d = data._p + offset[dir];
		if (grid.at(d).dMonster) {
			if (grid.at(d).dMonster > 0) {
				m = grid.at(d).dMonster - 1;
			} else {
				m = -(grid.at(d).dMonster + 1);
			}
			if (CanTalkToMonst(m)) {
				data._pVar1 = 0;
				return FALSE;
			}
		}

		if (data._pIFlags & ISPL_FIREDAM) {
			AddMissile(d, { 1, 0 }, 0, MIS_WEAPEXP, 0, pnum, 0, 0);
		}
		if (data._pIFlags & ISPL_LIGHTDAM) {
			AddMissile(d, { 2, 0 }, 0, MIS_WEAPEXP, 0, pnum, 0, 0);
		}

		didhit = FALSE;
		if (grid.at(d).dMonster) {
			m = grid.at(d).dMonster;
			if (grid.at(d).dMonster > 0) {
				m = grid.at(d).dMonster - 1;
			} else {
				m = -(grid.at(d).dMonster + 1);
			}
			didhit = PlrHitMonst(m);
		} else if (grid.at(d).dPlayer && !FriendlyMode) {
			BYTE p = grid.at(d).dPlayer;
			if (grid.at(d).dPlayer > 0) {
				p = grid.at(d).dPlayer - 1;
			} else {
				p = -(grid.at(d).dPlayer + 1);
			}
			didhit = PlrHitPlr(p);
		} else if (grid.at(d).dObject > 0) {
			didhit = PlrHitObj(d);
		}

		if (didhit && WeaponDur(30)) {
			StartStand(data._pdir);
			ClearPlrPVars();
			return TRUE;
		}
	}

	if (data._pAnimFrame == data._pAFrames) {
		StartStand(data._pdir);
		ClearPlrPVars();
		return TRUE;
	} else {
		return FALSE;
	}
}

BOOL Player::PM_DoRangeAttack()
{
	int origFrame, mistype;

	origFrame = data._pAnimFrame;
	if (data._pIFlags & ISPL_QUICKATTACK && origFrame == 1) {
		data._pAnimFrame++;
	}
	if (data._pIFlags & ISPL_FASTATTACK && (origFrame == 1 || origFrame == 3)) {
		data._pAnimFrame++;
	}

	if (data._pAnimFrame == data._pAFNum) {
		mistype = MIS_ARROW;
		if (data._pIFlags & ISPL_FIRE_ARROWS) {
			mistype = MIS_FARROW;
		}
		if (data._pIFlags & ISPL_LIGHT_ARROWS) {
			mistype = MIS_LARROW;
		}
		AddMissile(
		    data._p,
		    { data._pVar1, data._pVar2 },
		    data._pdir,
		    mistype,
		    0,
		    pnum,
		    4,
		    0);

		PlaySfxLoc(PS_BFIRE, data._p);

		if (WeaponDur(40)) {
			StartStand(data._pdir);
			ClearPlrPVars();
			return TRUE;
		}
	}

	if (data._pAnimFrame >= data._pAFrames) {
		StartStand(data._pdir);
		ClearPlrPVars();
		return TRUE;
	} else {
		return FALSE;
	}
}

void Player::ShieldDur()
{
	if (pnum != myplr()) {
		return;
	}

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("ShieldDur: illegal player %d", pnum);
	}

	if (data.InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_SHIELD) {
		if (data.InvBody[INVLOC_HAND_LEFT]._iDurability == DUR_INDESTRUCTIBLE) {
			return;
		}

		data.InvBody[INVLOC_HAND_LEFT]._iDurability--;
		if (data.InvBody[INVLOC_HAND_LEFT]._iDurability == 0) {
			NetSendCmdDelItem(TRUE, INVLOC_HAND_LEFT);
			data.InvBody[INVLOC_HAND_LEFT]._itype = ITYPE_NONE;
			CalcPlrInv(pnum, TRUE);
		}
	}

	if (data.InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_SHIELD) {
		if (data.InvBody[INVLOC_HAND_RIGHT]._iDurability != DUR_INDESTRUCTIBLE) {
			data.InvBody[INVLOC_HAND_RIGHT]._iDurability--;
			if (data.InvBody[INVLOC_HAND_RIGHT]._iDurability == 0) {
				NetSendCmdDelItem(TRUE, INVLOC_HAND_RIGHT);
				data.InvBody[INVLOC_HAND_RIGHT]._itype = ITYPE_NONE;
				CalcPlrInv(pnum, TRUE);
			}
		}
	}
}

BOOL Player::PM_DoBlock()
{
	if (data._pIFlags & ISPL_FASTBLOCK && data._pAnimFrame != 1) {
		data._pAnimFrame = data._pBFrames;
	}

	if (data._pAnimFrame >= data._pBFrames) {
		StartStand(data._pdir);
		ClearPlrPVars();

		if (!random_(3, 10)) {
			ShieldDur();
		}
		return TRUE;
	}

	return FALSE;
}

BOOL Player::PM_DoSpell()
{
	if (data._pVar8 == data._pSFNum) {
		CastSpell(
		    pnum,
		    data._pSpell,
		    data._p,
		    { data._pVar1, data._pVar2 },
		    0,
		    data._pVar4);

		if (!data._pSplFrom) {
			if (data._pRSplType == RSPLTYPE_SCROLL) {
				if (!(data._pScrlSpells
				        & (unsigned __int64)1 << (data._pRSpell - 1))) {
					data._pRSpell = SPL_INVALID;
					data._pRSplType = RSPLTYPE_INVALID;
					force_redraw = 255;
				}
			}

			if (data._pRSplType == RSPLTYPE_CHARGES) {
				if (!(data._pISpells
				        & (unsigned __int64)1 << (data._pRSpell - 1))) {
					data._pRSpell = SPL_INVALID;
					data._pRSplType = RSPLTYPE_INVALID;
					force_redraw = 255;
				}
			}
		}
	}

	data._pVar8++;

	if (level.leveltype == DTYPE_TOWN) {
		if (data._pVar8 > data._pSFrames) {
			StartWalkStand();
			ClearPlrPVars();
			return TRUE;
		}
	} else if (data._pAnimFrame == data._pSFrames) {
		StartStand(data._pdir);
		ClearPlrPVars();
		return TRUE;
	}

	return FALSE;
}

BOOL Player::PM_DoGotHit()
{
	int frame;

	frame = data._pAnimFrame;
	if (data._pIFlags & ISPL_FASTRECOVER && frame == 3) {
		data._pAnimFrame++;
	}
	if (data._pIFlags & ISPL_FASTERRECOVER && (frame == 3 || frame == 5)) {
		data._pAnimFrame++;
	}
	if (data._pIFlags & ISPL_FASTESTRECOVER && (frame == 1 || frame == 3 || frame == 5)) {
		data._pAnimFrame++;
	}

	if (data._pAnimFrame >= data._pHFrames) {
		StartStand(data._pdir);
		ClearPlrPVars();
		if (random_(3, 4)) {
			ArmorDur();
		}

		return TRUE;
	}

	return FALSE;
}

void Player::ArmorDur()
{
	int a;
	ItemStruct *pi;
	PlayerStruct *p;

	if (pnum != myplr()) {
		return;
	}

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("ArmorDur: illegal player %d", pnum);
	}

	p = &data;
	if (p->InvBody[INVLOC_CHEST]._itype == ITYPE_NONE && p->InvBody[INVLOC_HEAD]._itype == ITYPE_NONE) {
		return;
	}

	a = random_(8, 3);
	if (p->InvBody[INVLOC_CHEST]._itype != ITYPE_NONE && p->InvBody[INVLOC_HEAD]._itype == ITYPE_NONE) {
		a = 1;
	}
	if (p->InvBody[INVLOC_CHEST]._itype == ITYPE_NONE && p->InvBody[INVLOC_HEAD]._itype != ITYPE_NONE) {
		a = 0;
	}

	if (a != 0) {
		pi = &p->InvBody[INVLOC_CHEST];
	} else {
		pi = &p->InvBody[INVLOC_HEAD];
	}
	if (pi->_iDurability == DUR_INDESTRUCTIBLE) {
		return;
	}

	pi->_iDurability--;
	if (pi->_iDurability != 0) {
		return;
	}

	if (a != 0) {
		NetSendCmdDelItem(TRUE, INVLOC_CHEST);
	} else {
		NetSendCmdDelItem(TRUE, INVLOC_HEAD);
	}
	pi->_itype = ITYPE_NONE;
	CalcPlrInv(pnum, TRUE);
}

BOOL Player::PM_DoDeath()
{
	if (data._pVar8 >= 2 * data._pDFrames) {
		if (deathdelay > 1 && pnum == myplr()) {
			deathdelay--;
			if (deathdelay == 1) {
				deathflag = TRUE;
				if (gbMaxPlayers == 1) {
					gamemenu_on();
				}
			}
		}

		data._pAnimDelay = 10000;
		data._pAnimFrame = data._pAnimLen;
		grid.at(data._p).dFlags |= BFLAG_DEAD_PLAYER;
	}

	if (data._pVar8 < 100) {
		data._pVar8++;
	}

	return FALSE;
}

BOOL Player::PM_DoNewLvl()
{
	return FALSE;
}

void Player::CheckNewPath()
{
	int i, d;
	V2Di n, vel;
	int xvel3;

	if (data.destAction == ACTION_ATTACKMON) {
		MakePlrPath(monsters[data.destParam1].data._mfut, FALSE);
	}

	if (data.destAction == ACTION_ATTACKPLR) {
		MakePlrPath(plr[data.destParam1].data._pfut, FALSE);
	}

	if (data.walkpath[0] != WALK_NONE) {
		if (data._pmode == PM_STAND) {
			if (pnum == myplr()) {
				if (data.destAction == ACTION_ATTACKMON || data.destAction == ACTION_ATTACKPLR) {
					i = data.destParam1;

					if (data.destAction == ACTION_ATTACKMON) {
						n = (data._pfut - monsters[i].data._mfut).abs();
						d = GetDirection(data._pfut, monsters[i].data._mfut);
					} else {
						n = (data._pfut - plr[i].data._pfut).abs();
						d = GetDirection(data._pfut, plr[i].data._pfut);
					}

					if (n.x < 2 && n.y < 2) {
						ClrPlrPath();
						if (monsters[i].data.mtalkmsg && monsters[i].data.mtalkmsg != TEXT_VILE14) {
							monsters[i].TalktoMonster();
						} else {
							StartAttack(d);
						}
						data.destAction = ACTION_NONE;
					}
				}
			}

			if (level.currlevel != 0) {
				xvel3 = classes[data._pClass].PWVel[0];
				vel.x = classes[data._pClass].PWVel[1];
				vel.y = classes[data._pClass].PWVel[2];
			} else {
				xvel3 = 2048;
				vel.x = 1024;
				vel.y = 512;
			}

			switch (data.walkpath[0]) {
			case WALK_N:
				StartWalk({ 0, -vel.x }, { -1, -1 }, DIR_N, SDIR_N);
				break;
			case WALK_NE:
				StartWalk({ vel.x, -vel.y }, { 0, -1 }, DIR_NE, SDIR_NE);
				break;
			case WALK_E:
				StartWalk3({ xvel3, 0 }, { -32, -16 }, { 1, -1 }, { 1, 0 }, DIR_E, SDIR_E);
				break;
			case WALK_SE:
				StartWalk2(vel, { -32, -16 }, { 1, 0 }, DIR_SE, SDIR_SE);
				break;
			case WALK_S:
				StartWalk2({ 0, vel.x }, { 0, -32 }, { 1, 1 }, DIR_S, SDIR_S);
				break;
			case WALK_SW:
				StartWalk2({ -vel.x, vel.y }, { 32, -16 }, { 0, 1 }, DIR_SW, SDIR_SW);
				break;
			case WALK_W:
				StartWalk3({ -xvel3, 0 }, { 32, -16 }, { -1, 1 }, { 0, 1 }, DIR_W, SDIR_W);
				break;
			case WALK_NW:
				StartWalk({ -vel.x, -vel.y }, { -1, 0 }, DIR_NW, SDIR_NW);
				break;
			}

			for (i = 1; i < MAX_PATH_LENGTH; i++) {
				data.walkpath[i - 1] = data.walkpath[i];
			}

			data.walkpath[MAX_PATH_LENGTH - 1] = WALK_NONE;

			if (data._pmode == PM_STAND) {
				StartStand(data._pdir);
				data.destAction = ACTION_NONE;
			}
		}

		return;
	}
	if (data.destAction == ACTION_NONE) {
		return;
	}

	if (data._pmode == PM_STAND) {
		switch (data.destAction) {
		case ACTION_ATTACK:
			d = GetDirection(data._p, { data.destParam1, data.destParam2 });
			StartAttack(d);
			break;
		case ACTION_ATTACKMON:
			i = data.destParam1;
			n = (data._p - monsters[i].data._mfut).abs();
			if (n.x <= 1 && n.y <= 1) {
				d = GetDirection(data._pfut, monsters[i].data._mfut);
				if (monsters[i].data.mtalkmsg && monsters[i].data.mtalkmsg != TEXT_VILE14) {
					monsters[i].TalktoMonster();
				} else {
					StartAttack(d);
				}
			}
			break;
		case ACTION_ATTACKPLR:
			i = data.destParam1;
			n = (data._p - plr[i].data._pfut).abs();
			if (n.x <= 1 && n.y <= 1) {
				d = GetDirection(data._pfut, plr[i].data._pfut);
				StartAttack(d);
			}
			break;
		case ACTION_RATTACK:
			d = GetDirection(data._p, { data.destParam1, data.destParam2 });
			StartRangeAttack(d, { data.destParam1, data.destParam2 });
			break;
		case ACTION_RATTACKMON:
			i = data.destParam1;
			d = GetDirection(data._pfut, monsters[i].data._mfut);
			if (monsters[i].data.mtalkmsg && monsters[i].data.mtalkmsg != TEXT_VILE14) {
				monsters[i].TalktoMonster();
			} else {
				StartRangeAttack(d, monsters[i].data._mfut);
			}
			break;
		case ACTION_RATTACKPLR:
			i = data.destParam1;
			d = GetDirection(data._pfut, plr[i].data._pfut);
			StartRangeAttack(d, plr[i].data._pfut);
			break;
		case ACTION_SPELL:
			d = GetDirection(data._p, { data.destParam1, data.destParam2 });
			StartSpell(d, { data.destParam1, data.destParam2 });
			data._pVar4 = data.destParam3;
			break;
		case ACTION_SPELLWALL:
			StartSpell(data.destParam3, { data.destParam1, data.destParam2 });
			data._pVar3 = data.destParam3;
			data._pVar4 = data.destParam4;
			break;
		case ACTION_SPELLMON:
			i = data.destParam1;
			d = GetDirection(data._p, monsters[i].data._mfut);
			StartSpell(d, monsters[i].data._mfut);
			data._pVar4 = data.destParam2;
			break;
		case ACTION_SPELLPLR:
			i = data.destParam1;
			d = GetDirection(data._p, plr[i].data._pfut);
			StartSpell(d, plr[i].data._pfut);
			data._pVar4 = data.destParam2;
			break;
		case ACTION_OPERATE:
			i = data.destParam1;
			n = (data._p - object[i]._o).abs();
			if (n.y > 1 && grid[object[i]._o.x][object[i]._o.y - 1].dObject == -1 - i) {
				n.y = abs(data._p.y - object[i]._o.y + 1);
			}
			if (n.x <= 1 && n.y <= 1) {
				if (object[i]._oBreak == 1) {
					d = GetDirection(data._p, object[i]._o);
					StartAttack(d);
				} else {
					OperateObject(pnum, i, FALSE);
				}
			}
			break;
		case ACTION_DISARM:
			i = data.destParam1;
			n = (data._p - object[i]._o).abs();
			if (n.y > 1 && grid[object[i]._o.x][object[i]._o.y - 1].dObject == -1 - i) {
				n.y = abs(data._p.y - object[i]._o.y + 1);
			}
			if (n.x <= 1 && n.y <= 1) {
				if (object[i]._oBreak == 1) {
					d = GetDirection(data._p, object[i]._o);
					StartAttack(d);
				} else {
					TryDisarm(pnum, i);
					OperateObject(pnum, i, FALSE);
				}
			}
			break;
		case ACTION_OPERATETK:
			i = data.destParam1;
			if (object[i]._oBreak != 1) {
				OperateObject(pnum, i, TRUE);
			}
			break;
		case ACTION_PICKUPITEM:
			if (pnum == myplr()) {
				i = data.destParam1;
				n = (data._p - item[i]._i).abs();
				if (n.x <= 1 && n.y <= 1 && pcurs == CURSOR_HAND && !item[i]._iRequest) {
					NetSendCmdGItem(TRUE, CMD_REQUESTGITEM, myplr(), myplr(), i);
					item[i]._iRequest = TRUE;
				}
			}
			break;
		case ACTION_PICKUPAITEM:
			if (pnum == myplr()) {
				i = data.destParam1;
				n = (data._p - item[i]._i).abs();
				if (n.x <= 1 && n.y <= 1 && pcurs == CURSOR_HAND) {
					NetSendCmdGItem(TRUE, CMD_REQUESTAGITEM, myplr(), myplr(), i);
				}
			}
			break;
		case ACTION_TALK:
			if (pnum == myplr()) {
				TalkToTowner(pnum, data.destParam1);
			}
			break;
		}

		FixPlayerLocation(data._pdir);
		data.destAction = ACTION_NONE;

		return;
	}

	if (data._pmode == PM_ATTACK && data._pAnimFrame > myplr().data._pAFNum) {
		if (data.destAction == ACTION_ATTACK) {
			d = GetDirection(data._pfut, { data.destParam1, data.destParam2 });
			StartAttack(d);
			data.destAction = ACTION_NONE;
		} else if (data.destAction == ACTION_ATTACKMON) {
			i = data.destParam1;
			n = (data._p - monsters[i].data._mfut).abs();
			if (n.x <= 1 && n.y <= 1) {
				d = GetDirection(data._pfut, monsters[i].data._mfut);
				StartAttack(d);
			}
			data.destAction = ACTION_NONE;
		} else if (data.destAction == ACTION_ATTACKPLR) {
			i = data.destParam1;
			n = (data._p - plr[i].data._pfut).abs();
			if (n.x <= 1 && n.y <= 1) {
				d = GetDirection(data._pfut, plr[i].data._pfut);
				StartAttack(d);
			}
			data.destAction = ACTION_NONE;
		} else if (data.destAction == ACTION_OPERATE) {
			i = data.destParam1;
			n = (data._p - object[i]._o).abs();
			if (n.y > 1 && grid[object[i]._o.x][object[i]._o.y - 1].dObject == -1 - i) {
				n.y = abs(data._p.y - object[i]._o.y + 1);
			}
			if (n.x <= 1 && n.y <= 1) {
				if (object[i]._oBreak == 1) {
					d = GetDirection(data._p, object[i]._o);
					StartAttack(d);
				} else {
					OperateObject(pnum, i, FALSE);
				}
			}
		}
	}

	if (data._pmode == PM_RATTACK && data._pAnimFrame > myplr().data._pAFNum) {
		if (data.destAction == ACTION_RATTACK) {
			d = GetDirection(data._p, { data.destParam1, data.destParam2 });
			StartRangeAttack(d, { data.destParam1, data.destParam2 });
			data.destAction = ACTION_NONE;
		} else if (data.destAction == ACTION_RATTACKMON) {
			i = data.destParam1;
			d = GetDirection(data._p, monsters[i].data._mfut);
			StartRangeAttack(d, monsters[i].data._mfut);
			data.destAction = ACTION_NONE;
		} else if (data.destAction == ACTION_RATTACKPLR) {
			i = data.destParam1;
			d = GetDirection(data._p, plr[i].data._pfut);
			StartRangeAttack(d, plr[i].data._pfut);
			data.destAction = ACTION_NONE;
		}
	}

	if (data._pmode == PM_SPELL && data._pAnimFrame > data._pSFNum) {
		if (data.destAction == ACTION_SPELL) {
			d = GetDirection(data._p, { data.destParam1, data.destParam2 });
			StartSpell(d, { data.destParam1, data.destParam2 });
			data.destAction = ACTION_NONE;
		} else if (data.destAction == ACTION_SPELLMON) {
			i = data.destParam1;
			d = GetDirection(data._p, monsters[i].data._mfut);
			StartSpell(d, monsters[i].data._mfut);
			data.destAction = ACTION_NONE;
		} else if (data.destAction == ACTION_SPELLPLR) {
			i = data.destParam1;
			d = GetDirection(data._p, plr[i].data._pfut);
			StartSpell(d, plr[i].data._pfut);
			data.destAction = ACTION_NONE;
		}
	}
}

BOOL PlrDeathModeOK(int p)
{
	if (p != myplr()) {
		return TRUE;
	}

	if ((DWORD)p >= MAX_PLRS) {
		app_fatal("PlrDeathModeOK: illegal player %d", p);
	}

	if (plr[p].data._pmode == PM_DEATH) {
		return TRUE;
	} else if (plr[p].data._pmode == PM_QUIT) {
		return TRUE;
	} else if (plr[p].data._pmode == PM_NEWLVL) {
		return TRUE;
	}

	return FALSE;
}

void ValidatePlayer()
{
	if ((DWORD)myplr() >= MAX_PLRS) {
		app_fatal("ValidatePlayer: illegal player %d", myplr());
	}
	myplr().ValidatePlayer();
}

void Player::ValidatePlayer()
{
	int gt, pc, i, b;
	__int64 msk = 0;

	if (data._pLevel > MAXCHARLEVEL - 1)
		data._pLevel = MAXCHARLEVEL - 1;
	if (data._pExperience > data._pNextExper)
		data._pExperience = data._pNextExper;

	gt = 0;
	for (i = 0; i < data._pNumInv; i++) {
		if (data.InvList[i]._itype == ITYPE_GOLD) {
			if (data.InvList[i]._ivalue > GOLD_MAX_LIMIT) {
				data.InvList[i]._ivalue = GOLD_MAX_LIMIT;
			}
			gt += data.InvList[i]._ivalue;
		}
	}
	if (gt != data._pGold)
		data._pGold = gt;

	pc = data._pClass;
	if (data._pBaseStr > classes[pc].MaxStats[ATTRIB_STR]) {
		data._pBaseStr = classes[pc].MaxStats[ATTRIB_STR];
	}
	if (data._pBaseMag > classes[pc].MaxStats[ATTRIB_MAG]) {
		data._pBaseMag = classes[pc].MaxStats[ATTRIB_MAG];
	}
	if (data._pBaseDex > classes[pc].MaxStats[ATTRIB_DEX]) {
		data._pBaseDex = classes[pc].MaxStats[ATTRIB_DEX];
	}
	if (data._pBaseVit > classes[pc].MaxStats[ATTRIB_VIT]) {
		data._pBaseVit = classes[pc].MaxStats[ATTRIB_VIT];
	}

	for (b = 1; b < MAX_SPELLS; b++) {
		if (spelldata[b].sBookLvl != -1) {
			msk |= (__int64)1 << (b - 1);
			if (data._pSplLvl[b] > 15)
				data._pSplLvl[b] = 15;
		}
	}

	data._pMemSpells &= msk;
}

void ProcessPlayers()
{
	if ((DWORD)myplr() >= MAX_PLRS) {
		app_fatal("ProcessPlayers: illegal player %d", myplr());
	}
	myplr().ProcessPlayer();
}

void Player::ProcessPlayer()
{
	int pnum;
	BOOL tplayer;

	if (data.pLvlLoad > 0) {
		data.pLvlLoad--;
	}

	if (sfxdelay > 0) {
		sfxdelay--;
		if (sfxdelay == 0) {
			PlaySFX(sfxdnum);
		}
	}

	ValidatePlayer();

	for (pnum = 0; pnum < MAX_PLRS; pnum++) {
		if (data.plractive && level.currlevel == data.plrlevel && (pnum == myplr() || !data._pLvlChanging)) {
			CheckCheatStats();

			if (!PlrDeathModeOK(pnum) && (data._pHitPoints >> 6) <= 0) {
				SyncPlrKill(-1);
			}

			if (pnum == myplr()) {
				if ((data._pIFlags & ISPL_DRAINLIFE) && level.currlevel != 0) {
					data._pHitPoints -= 4;
					data._pHPBase -= 4;
					if ((data._pHitPoints >> 6) <= 0) {
						SyncPlrKill(0);
					}
					drawhpflag = TRUE;
				}
				if (data._pIFlags & ISPL_NOMANA && data._pManaBase > 0) {
					data._pManaBase -= data._pMana;
					data._pMana = 0;
					drawmanaflag = TRUE;
				}
			}

			tplayer = FALSE;
			do {
				switch (data._pmode) {
				case PM_STAND:
					tplayer = PM_DoStand();
					break;
				case PM_WALK:
					tplayer = PM_DoWalk();
					break;
				case PM_WALK2:
					tplayer = PM_DoWalk2();
					break;
				case PM_WALK3:
					tplayer = PM_DoWalk3();
					break;
				case PM_ATTACK:
					tplayer = PM_DoAttack();
					break;
				case PM_RATTACK:
					tplayer = PM_DoRangeAttack();
					break;
				case PM_BLOCK:
					tplayer = PM_DoBlock();
					break;
				case PM_SPELL:
					tplayer = PM_DoSpell();
					break;
				case PM_GOTHIT:
					tplayer = PM_DoGotHit();
					break;
				case PM_DEATH:
					tplayer = PM_DoDeath();
					break;
				case PM_NEWLVL:
					tplayer = PM_DoNewLvl();
					break;
				}
				CheckNewPath();
			} while (tplayer);

			data._pAnimCnt++;
			if (data._pAnimCnt > data._pAnimDelay) {
				data._pAnimCnt = 0;
				data._pAnimFrame++;
				if (data._pAnimFrame > data._pAnimLen) {
					data._pAnimFrame = 1;
				}
			}
		}
	}
}

void Player::CheckCheatStats()
{
	if (data._pStrength > 750) {
		data._pStrength = 750;
	}

	if (data._pDexterity > 750) {
		data._pDexterity = 750;
	}

	if (data._pMagic > 750) {
		data._pMagic = 750;
	}

	if (data._pVitality > 750) {
		data._pVitality = 750;
	}

	if (data._pHitPoints > 128000) {
		data._pHitPoints = 128000;
	}

	if (data._pMana > 128000) {
		data._pMana = 128000;
	}
}

void Player::ClrPlrPath()
{
	memset(data.walkpath, WALK_NONE, sizeof(data.walkpath));
}

BOOL PosOkPlayer(int pnum, V2Di pos)
{
	return plr[pnum].PosOkPlayer(pos);
}

BOOL Player::PosOkPlayer(V2Di pos)
{
	int x = pos.x;
	int y = pos.y;
	BOOL PosOK;
	DWORD p;
	char bv;

	PosOK = FALSE;
	if (x >= 0 && x < MAXDUNX && y >= 0 && y < MAXDUNY && !SolidLoc({ x, y }) && grid[x][y].dPiece) {

		if (grid[x][y].dPlayer) {
			if (grid[x][y].dPlayer > 0) {
				p = grid[x][y].dPlayer - 1;
			} else {
				p = -(grid[x][y].dPlayer + 1);
			}
			if (p != pnum && p < MAX_PLRS && plr[p].data._pHitPoints) {
				return FALSE;
			}
		}

		if (grid[x][y].dMonster) {
			if (level.currlevel == 0) {
				return FALSE;
			}
			if (grid[x][y].dMonster <= 0) {
				return FALSE;
			}
			if ((monsters[grid[x][y].dMonster - 1].data._mhitpoints >> 6) > 0) {
				return FALSE;
			}
		}

		if (grid[x][y].dObject) {
			if (grid[x][y].dObject > 0) {
				bv = grid[x][y].dObject - 1;
			} else {
				bv = -(grid[x][y].dObject + 1);
			}
			if (object[bv]._oSolidFlag) {
				return FALSE;
			}
		}

		PosOK = TRUE;
	}

	if (!PosOK)
		return FALSE;
	return TRUE;
}

void Player::MakePlrPath(V2Di p, BOOL endspace)
{
	data._ptarg = p;
	if (data._pfut == p) {
		return;
	}

	int path = FindPath(::dvl::PosOkPlayer, pnum, data._pfut, p, data.walkpath);
	if (!path) return;

	if (!endspace) {
		path--;

		switch (data.walkpath[path]) {
		case WALK_NE:
			p.y++;
			break;
		case WALK_NW:
			p.x++;
			break;
		case WALK_SE:
			p.x--;
			break;
		case WALK_SW:
			p.y--;
			break;
		case WALK_N:
			p.x++;
			p.y++;
			break;
		case WALK_E:
			p.x--;
			p.y++;
			break;
		case WALK_S:
			p.x--;
			p.y--;
			break;
		case WALK_W:
			p.x++;
			p.y--;
			break;
		}

		data._ptarg = p;
	}

	data.walkpath[path] = WALK_NONE;
}

void CheckPlrSpell()
{
	BOOL addflag;
	int rspell, sd, sl;

	if ((DWORD)myplr() >= MAX_PLRS) {
		app_fatal("CheckPlrSpell: illegal player %d", myplr());
	}

	rspell = myplr().data._pRSpell;
	if (rspell == SPL_INVALID) {
		if (myplr().data._pClass == PC_WARRIOR) {
			PlaySFX(PS_WARR34);
#ifndef SPAWN
		} else if (myplr().data._pClass == PC_ROGUE) {
			PlaySFX(PS_ROGUE34);
		} else if (myplr().data._pClass == PC_SORCERER) {
			PlaySFX(PS_MAGE34);
#endif
		}
		return;
	}

	if (level.leveltype == DTYPE_TOWN && !spelldata[rspell].sTownSpell) {
		if (myplr().data._pClass == PC_WARRIOR) {
			PlaySFX(PS_WARR27);
#ifndef SPAWN
		} else if (myplr().data._pClass == PC_ROGUE) {
			PlaySFX(PS_ROGUE27);
		} else if (myplr().data._pClass == PC_SORCERER) {
			PlaySFX(PS_MAGE27);
#endif
		}
		return;
	}

	if (!sgbControllerActive) {
		if (pcurs != CURSOR_HAND
		    || (Mouse.y >= PANEL_TOP && Mouse.x >= PANEL_LEFT && Mouse.x <= RIGHT_PANEL)     // inside main panel
		    || ((chrflag || questlog) && Mouse.x < SPANEL_WIDTH && Mouse.y < SPANEL_HEIGHT) // inside left panel
		    || ((invflag || sbookflag) && Mouse.x > RIGHT_PANEL && Mouse.y < SPANEL_HEIGHT) // inside right panel
		        && rspell != SPL_HEAL
		        && rspell != SPL_IDENTIFY
		        && rspell != SPL_REPAIR
		        && rspell != SPL_INFRA
		        && rspell != SPL_RECHARGE) {
			return;
		}
	}

	addflag = FALSE;
	switch (myplr().data._pRSplType) {
	case RSPLTYPE_SKILL:
	case RSPLTYPE_SPELL:
		addflag = CheckSpell(myplr(), rspell, myplr().data._pRSplType, FALSE);
		break;
	case RSPLTYPE_SCROLL:
		addflag = UseScroll();
		break;
	case RSPLTYPE_CHARGES:
		addflag = UseStaff();
		break;
	}

	if (addflag) {
		if (myplr().data._pRSpell == SPL_FIREWALL) {
			sd = GetDirection(myplr().data._p, cursm);
			sl = GetSpellLevel(myplr(), myplr().data._pRSpell);
			NetSendCmdLocParam3(TRUE, CMD_SPELLXYD, cursm, myplr().data._pRSpell, sd, sl);
		} else if (pcursmonst != -1) {
			sl = GetSpellLevel(myplr(), myplr().data._pRSpell);
			NetSendCmdParam3(TRUE, CMD_SPELLID, pcursmonst, myplr().data._pRSpell, sl);
		} else if (pcursplr != -1) {
			sl = GetSpellLevel(myplr(), myplr().data._pRSpell);
			NetSendCmdParam3(TRUE, CMD_SPELLPID, pcursplr, myplr().data._pRSpell, sl);
		} else { //145
			sl = GetSpellLevel(myplr(), myplr().data._pRSpell);
			NetSendCmdLocParam2(TRUE, CMD_SPELLXY, cursm, myplr().data._pRSpell, sl);
		}
		return;
	}

	if (myplr().data._pRSplType == RSPLTYPE_SPELL) {
		if (myplr().data._pClass == PC_WARRIOR) {
			PlaySFX(PS_WARR35);
#ifndef SPAWN
		} else if (myplr().data._pClass == PC_ROGUE) {
			PlaySFX(PS_ROGUE35);
		} else if (myplr().data._pClass == PC_SORCERER) {
			PlaySFX(PS_MAGE35);
#endif
		}
	}
}

void Player::SyncPlrAnim()
{
	int dir, sType;
	dir = data._pdir;
	switch (data._pmode) {
	case PM_STAND:
		data._pAnimData = data._pNAnim[dir];
		break;
	case PM_WALK:
	case PM_WALK2:
	case PM_WALK3:
		data._pAnimData = data._pWAnim[dir];
		break;
	case PM_ATTACK:
		data._pAnimData = data._pAAnim[dir];
		break;
	case PM_RATTACK:
		data._pAnimData = data._pAAnim[dir];
		break;
	case PM_BLOCK:
		data._pAnimData = data._pBAnim[dir];
		break;
	case PM_SPELL:
		if (pnum == myplr())
			sType = spelldata[data._pSpell].sType;
		else
			sType = STYPE_FIRE;
		if (sType == STYPE_FIRE)
			data._pAnimData = data._pFAnim[dir];
		if (sType == STYPE_LIGHTNING)
			data._pAnimData = data._pLAnim[dir];
		if (sType == STYPE_MAGIC)
			data._pAnimData = data._pTAnim[dir];
		break;
	case PM_GOTHIT:
		data._pAnimData = data._pHAnim[dir];
		break;
	case PM_NEWLVL:
		data._pAnimData = data._pNAnim[dir];
		break;
	case PM_DEATH:
		data._pAnimData = data._pDAnim[dir];
		break;
	case PM_QUIT:
		data._pAnimData = data._pNAnim[dir];
		break;
	default:
		app_fatal("SyncPlrAnim");
		break;
	}
}

void Player::SyncInitPlrPos()
{
	int x, y, xx, yy, range;
	DWORD i;
	BOOL posOk;

	data._ptarg = data._p;

	if (gbMaxPlayers == 1 || data.plrlevel != level.currlevel) {
		return;
	}

	for (i = 0; i < 8; i++) {
		x = data._p.x + plrxoff2[i];
		y = data._p.y + plryoff2[i];
		if (PosOkPlayer({ x, y })) {
			break;
		}
	}

	if (!PosOkPlayer({ x, y })) {
		posOk = FALSE;
		for (range = 1; range < 50 && !posOk; range++) {
			for (yy = -range; yy <= range && !posOk; yy++) {
				y = yy + data._p.y;
				for (xx = -range; xx <= range && !posOk; xx++) {
					x = xx + data._p.x;
					if (PosOkPlayer({ x, y }) && !PosOkPortal(level.currlevel, { x, y })) {
						posOk = TRUE;
					}
				}
			}
		}
	}

	data._p = { x, y };
	grid[x][y].dPlayer = pnum + 1;

	if (pnum == myplr()) {
		data._pfut = { x, y };
		data._ptarg = { x, y };
		View = { x, y };
	}
}

void Player::SyncInitPlr()
{
	SetPlrAnims();
	SyncInitPlrPos();
}

void Player::CheckStats()
{
	int c, i;

	if (data._pClass == PC_WARRIOR) {
		c = PC_WARRIOR;
	} else if (data._pClass == PC_ROGUE) {
		c = PC_ROGUE;
	} else if (data._pClass == PC_SORCERER) {
		c = PC_SORCERER;
	}

	for (i = 0; i < 4; i++) {
		switch (i) {
		case ATTRIB_STR:
			if (data._pBaseStr > classes[c].MaxStats[ATTRIB_STR]) {
				data._pBaseStr = classes[c].MaxStats[ATTRIB_STR];
			} else if (data._pBaseStr < 0) {
				data._pBaseStr = 0;
			}
			break;
		case ATTRIB_MAG:
			if (data._pBaseMag > classes[c].MaxStats[ATTRIB_MAG]) {
				data._pBaseMag = classes[c].MaxStats[ATTRIB_MAG];
			} else if (data._pBaseMag < 0) {
				data._pBaseMag = 0;
			}
			break;
		case ATTRIB_DEX:
			if (data._pBaseDex > classes[c].MaxStats[ATTRIB_DEX]) {
				data._pBaseDex = classes[c].MaxStats[ATTRIB_DEX];
			} else if (data._pBaseDex < 0) {
				data._pBaseDex = 0;
			}
			break;
		case ATTRIB_VIT:
			if (data._pBaseVit > classes[c].MaxStats[ATTRIB_VIT]) {
				data._pBaseVit = classes[c].MaxStats[ATTRIB_VIT];
			} else if (data._pBaseVit < 0) {
				data._pBaseVit = 0;
			}
			break;
		}
	}
}

void Player::ModifyPlrStr(int l)
{
	int max;

	max = classes[data._pClass].MaxStats[ATTRIB_STR];
	if (data._pBaseStr + l > max) {
		l = max - data._pBaseStr;
	}

	data._pStrength += l;
	data._pBaseStr += l;

	if (data._pClass == PC_ROGUE) {
		data._pDamageMod = data._pLevel * (data._pStrength + data._pDexterity) / 200;
	} else {
		data._pDamageMod = data._pLevel * data._pStrength / 100;
	}

	CalcPlrInv(pnum, TRUE);

	if (pnum == myplr()) {
		NetSendCmdParam1(FALSE, CMD_SETSTR, data._pBaseStr); //60
	}
}

void Player::ModifyPlrMag(int l)
{
	int max, ms;

	max = classes[data._pClass].MaxStats[ATTRIB_MAG];
	if (data._pBaseMag + l > max) {
		l = max - data._pBaseMag;
	}

	data._pMagic += l;
	data._pBaseMag += l;

	ms = l << 6;
	if (data._pClass == PC_SORCERER) {
		ms <<= 1;
	}

	data._pMaxManaBase += ms;
	data._pMaxMana += ms;
	if (!(data._pIFlags & ISPL_NOMANA)) {
		data._pManaBase += ms;
		data._pMana += ms;
	}

	CalcPlrInv(pnum, TRUE);

	if (pnum == myplr()) {
		NetSendCmdParam1(FALSE, CMD_SETMAG, data._pBaseMag);
	}
}

void Player::ModifyPlrDex(int l)
{
	int max;

	max = classes[data._pClass].MaxStats[ATTRIB_DEX];
	if (data._pBaseDex + l > max) {
		l = max - data._pBaseDex;
	}

	data._pDexterity += l;
	data._pBaseDex += l;
	CalcPlrInv(pnum, TRUE);

	if (data._pClass == PC_ROGUE) {
		data._pDamageMod = data._pLevel * (data._pDexterity + data._pStrength) / 200;
	}

	if (pnum == myplr()) {
		NetSendCmdParam1(FALSE, CMD_SETDEX, data._pBaseDex);
	}
}

void Player::ModifyPlrVit(int l)
{
	int max, ms;

	max = classes[data._pClass].MaxStats[ATTRIB_VIT];
	if (data._pBaseVit + l > max) {
		l = max - data._pBaseVit;
	}

	data._pVitality += l;
	data._pBaseVit += l;

	ms = l << 6;
	if (data._pClass == PC_WARRIOR) {
		ms <<= 1;
	}

	data._pHPBase += ms;
	data._pMaxHPBase += ms;
	data._pHitPoints += ms;
	data._pMaxHP += ms;

	CalcPlrInv(pnum, TRUE);

	if (pnum == myplr()) {
		NetSendCmdParam1(FALSE, CMD_SETVIT, data._pBaseVit);
	}
}

void Player::SetPlayerHitPoints(int val)
{
	data._pHitPoints = val;
	data._pHPBase = val + data._pMaxHPBase - data._pMaxHP;

	if (pnum == myplr()) {
		drawhpflag = TRUE;
	}
}

void Player::SetPlrStr(int v)
{
	int dm;
	data._pBaseStr = v;
	CalcPlrInv(pnum, TRUE);

	if (data._pClass == PC_ROGUE) {
		dm = data._pLevel * (data._pStrength + data._pDexterity) / 200;
	} else {
		dm = data._pLevel * data._pStrength / 100;
	}

	data._pDamageMod = dm;
}

void Player::SetPlrMag(int v)
{
	int m;

	data._pBaseMag = v;

	m = v << 6;
	if (data._pClass == PC_SORCERER) {
		m <<= 1;
	}

	data._pMaxManaBase = m;
	data._pMaxMana = m;
	CalcPlrInv(pnum, TRUE);
}

void Player::SetPlrDex(int v)
{
	int dm;

	data._pBaseDex = v;
	CalcPlrInv(pnum, TRUE);

	if (data._pClass == PC_ROGUE) {
		dm = data._pLevel * (data._pStrength + data._pDexterity) / 200;
	} else {
		dm = data._pStrength * data._pLevel / 100;
	}

	data._pDamageMod = dm;
}

void Player::SetPlrVit(int v)
{
	int hp;

	data._pBaseVit = v;

	hp = v << 6;
	if (data._pClass == PC_WARRIOR) {
		hp <<= 1;
	}

	data._pHPBase = hp;
	data._pMaxHPBase = hp;
	CalcPlrInv(id(), TRUE);
}

void Player::InitDungMsgs()
{
	data.pDungMsgs = 0;
}

void PlayDungMsgs()
{
	if ((DWORD)myplr() >= MAX_PLRS) {
		app_fatal("PlayDungMsgs: illegal player %d", myplr());
	}

	if (level.currlevel == 1 && !myplr().data._pLvlVisited[1] && gbMaxPlayers == 1 && !(myplr().data.pDungMsgs & DMSG_CATHEDRAL)) {
		sfxdelay = 40;
		if (myplr().data._pClass == PC_WARRIOR) {
			sfxdnum = PS_WARR97;
#ifndef SPAWN
		} else if (myplr().data._pClass == PC_ROGUE) {
			sfxdnum = PS_ROGUE97;
		} else if (myplr().data._pClass == PC_SORCERER) {
			sfxdnum = PS_MAGE97;
#endif
		}
		myplr().data.pDungMsgs = myplr().data.pDungMsgs | DMSG_CATHEDRAL;
	} else if (level.currlevel == 5 && !myplr().data._pLvlVisited[5] && gbMaxPlayers == 1 && !(myplr().data.pDungMsgs & DMSG_CATACOMBS)) {
		sfxdelay = 40;
		if (myplr().data._pClass == PC_WARRIOR) {
			sfxdnum = PS_WARR96B;
#ifndef SPAWN
		} else if (myplr().data._pClass == PC_ROGUE) {
			sfxdnum = PS_ROGUE96;
		} else if (myplr().data._pClass == PC_SORCERER) {
			sfxdnum = PS_MAGE96;
#endif
		}
		myplr().data.pDungMsgs |= DMSG_CATACOMBS;
	} else if (level.currlevel == 9 && !myplr().data._pLvlVisited[9] && gbMaxPlayers == 1 && !(myplr().data.pDungMsgs & DMSG_CAVES)) {
		sfxdelay = 40;
		if (myplr().data._pClass == PC_WARRIOR) {
			sfxdnum = PS_WARR98;
#ifndef SPAWN
		} else if (myplr().data._pClass == PC_ROGUE) {
			sfxdnum = PS_ROGUE98;
		} else if (myplr().data._pClass == PC_SORCERER) {
			sfxdnum = PS_MAGE98;
#endif
		}
		myplr().data.pDungMsgs |= DMSG_CAVES;
	} else if (level.currlevel == 13 && !myplr().data._pLvlVisited[13] && gbMaxPlayers == 1 && !(myplr().data.pDungMsgs & DMSG_HELL)) {
		sfxdelay = 40;
		if (myplr().data._pClass == PC_WARRIOR) {
			sfxdnum = PS_WARR99;
#ifndef SPAWN
		} else if (myplr().data._pClass == PC_ROGUE) {
			sfxdnum = PS_ROGUE99;
		} else if (myplr().data._pClass == PC_SORCERER) {
			sfxdnum = PS_MAGE99;
#endif
		}
		myplr().data.pDungMsgs |= DMSG_HELL;
	} else if (level.currlevel == 16 && !myplr().data._pLvlVisited[15] && gbMaxPlayers == 1 && !(myplr().data.pDungMsgs & DMSG_DIABLO)) { // BUGFIX: _pLvlVisited should check 16 or this message will never play
		sfxdelay = 40;
#ifndef SPAWN
		if (myplr().data._pClass == PC_WARRIOR || myplr().data._pClass == PC_ROGUE || myplr().data._pClass == PC_SORCERER) {
			sfxdnum = PS_DIABLVLINT;
		}
#endif
		myplr().data.pDungMsgs |= DMSG_DIABLO;
	} else {
		sfxdelay = 0;
	}
}

DEVILUTION_END_NAMESPACE
