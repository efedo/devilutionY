#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

const char Player::ArmourChar[4] = { 'L', 'M', 'H', 0 };
const char Player::WepChar[10] = { 'N', 'U', 'S', 'D', 'B', 'A', 'M', 'H', 'T', 0 };
const char Player::CharChar[] = { 'W', 'R', 'S', 0 };

V2Di Player::plroff[9] = {
	{ 0, 0 },
	{ 2, 2 },
	{ 0, 2 },
	{ 2, 0 },
	{ 1, 1 },
	{ 0, 1 },
	{ 1, 0 },
	{ 2, 1 },
	{ 1, 2 }
};

V2Di Player::plroff2[9] = {
	{ 0, 0 },
	{ 1, 0 },
	{ 0, 1 },
	{ 1, 1 },
	{ 2, 0 },
	{ 0, 2 },
	{ 1, 2 },
	{ 2, 1 },
	{ 2, 2 }
};

int Player::ExpLvlsTbl[MAXCHARLEVEL] = {
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

uint8_t Player::fix[9] = {0, 0, 3, 3, 3, 6, 6, 6, 8}; /* PM_ChangeLightOff local type */

CharacterTypes classes;

Player &myplr()
{
	return actors._localPlr();
}


Player::Player(ActorId id) : Actor(ActorType::player), inv(*this)
{
	memset(&data, 0, sizeof(data)); // 0 out data struct
}

Player::~Player()
{
}

bool Player::isMyPlr()
{
	return id() == myplr().id();
}

Item *Player::heldItem() const
{
	return inv.getHeldItem();
}

// Used to assign pos when map not fully set up
void Player::_changePosOffMap(const V2Di newpos)
{
	data._pos = newpos;
	data._posdraw = newpos;
}

// Changes current position
void Player::changePos(const V2Di newpos)
{
	changeFutPos(newpos);
	advancePos();
}

// Used to assign future pos when map not fully set up
void Player::_changeFutPosOffMap(const V2Di newpos)
{
	data._posfut = newpos;
}

// Changes future position
void Player::changeFutPos(const V2Di newpos)
{
	if (newpos == data._posfut) return;
	assert(PosOkPlayer(newpos));

	// remove from old future position unless same as current position
	if (data._posfut != data._pos)
		grid.at(data._posfut).clearActor(*this);
	// add to new position unless same as current position
	if (newpos != data._posfut)
		grid.at(newpos).setActor(*this);
	data._posfut = newpos;
}

// Changes draw position
void Player::changeDrawPos(const V2Di newpos)
{
	if (newpos == data._posdraw) return;
	grid.at(drawpos()).clearActorDraw(*this);
	data._posdraw = newpos;
	grid.at(drawpos()).setActorDraw(*this);
}

// Makes current position = future position
void Player::advancePos()
{
	if (data._posfut == data._pos) return;
	grid.at(pos()).clearActor(*this);
	if (futpos() != drawpos()) grid.at(drawpos()).clearActorDraw(*this);
	data._pos = futpos();
	data._posdraw = pos();
	grid.at(pos()).setActorDraw(*this);
}

// Adds the player to the map (upon level change/game entry)
void Player::addToMap()
{
	grid.at(pos()).setActor(*this);
	grid.at(drawpos()).setActorDraw(*this);
	grid.at(futpos()).setActor(*this);
}

V2Di Player::pos() const
{
	return data._pos;
}

V2Di Player::futpos() const
{
	return data._posfut;
}

V2Di Player::drawpos() const
{
	return data._posdraw;
}

void Player::LoadPlrGFX(player_graphic gfxflag)
{
	char prefix[16];
	char pszName[256];
	char *szCel;
	uint8_t *pData, *pAnim;
	DWORD i;

	sprintf(prefix, "%c%c%c", CharChar[int(data._pClass)], ArmourChar[int(data._pgfxnum) >> 4], WepChar[int(data._pgfxnum) & 0xF]);
	std::string &cs = classes[int(data._pClass)].name;

	for (i = 1; i <= PlayerGraphicFile::NONDEATH; i <<= 1) {
		if (!(i & gfxflag)) {
			continue;
		}

		switch (i) {
		case PlayerGraphicFile::STAND:
			szCel = "AS";
			if (lvl.type() == DunType::town) {
				szCel = "ST";
			}
			pData = data._pNData;
			pAnim = (uint8_t *)data._pNAnim;
			break;
		case PlayerGraphicFile::WALK:
			szCel = "AW";
			if (lvl.type() == DunType::town) {
				szCel = "WL";
			}
			pData = data._pWData;
			pAnim = (uint8_t *)data._pWAnim;
			break;
		case PlayerGraphicFile::ATTACK:
			if (lvl.type() == DunType::town) {
				continue;
			}
			szCel = "AT";
			pData = data._pAData;
			pAnim = (uint8_t *)data._pAAnim;
			break;
		case PlayerGraphicFile::HIT:
			if (lvl.type() == DunType::town) {
				continue;
			}
			szCel = "HT";
			pData = data._pHData;
			pAnim = (uint8_t *)data._pHAnim;
			break;
		case PlayerGraphicFile::LIGHTNING:
			if (lvl.type() == DunType::town) {
				continue;
			}
			szCel = "LM";
			pData = data._pLData;
			pAnim = (uint8_t *)data._pLAnim;
			break;
		case PlayerGraphicFile::FIRE:
			if (lvl.type() == DunType::town) {
				continue;
			}
			szCel = "FM";
			pData = data._pFData;
			pAnim = (uint8_t *)data._pFAnim;
			break;
		case PlayerGraphicFile::MAGIC:
			if (lvl.type() == DunType::town) {
				continue;
			}
			szCel = "QM";
			pData = data._pTData;
			pAnim = (uint8_t *)data._pTAnim;
			break;
		case PlayerGraphicFile::DEATH:
			if (data._pgfxnum & 0xF) {
				continue;
			}
			szCel = "DT";
			pData = data._pDData;
			pAnim = (uint8_t *)data._pDAnim;
			break;
		case PlayerGraphicFile::BLOCK:
			if (lvl.type() == DunType::town) {
				continue;
			}
			if (!data._pBlockFlag) {
				continue;
			}

			szCel = "BL";
			pData = data._pBData;
			pAnim = (uint8_t *)data._pBAnim;
			break;
		default:
			app_fatal("PLR:2");
			break;
		}

		sprintf(pszName, "PlrGFX\\%s\\%s\\%s%s.CL2", cs.c_str(), prefix, prefix, szCel);
		LoadFileWithMem(pszName, pData);
		actors.SetPlayerGPtrs((uint8_t *)pData, (uint8_t **)pAnim);
		data._pGFXLoad |= i;
	}
}

void Player::InitPlayerGFX()
{
	if (data._pHitPoints >> 6 == 0) {
		data._pgfxnum = AnimWeaponId::unarmed;
		LoadPlrGFX(PlayerGraphicFile::DEATH);
	} else {
		LoadPlrGFX(PlayerGraphicFile::NONDEATH);
	}
}

void Player::InitPlrGFXMem()
{
	if (!(plr_gfx_flag & 0x1)) { //STAND
		plr_gfx_flag |= 0x1;
		if (GetPlrGFXSize("ST") > GetPlrGFXSize("AS")) {
			plr_sframe_size = GetPlrGFXSize("ST");  // TOWN
		} else {
			plr_sframe_size = GetPlrGFXSize("AS");  // DUNGEON
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

DWORD Player::GetPlrGFXSize(char *szCel)
{
	int c;
	const char *a, *w;
	DWORD dwSize, dwMaxSize;
	HANDLE hsFile;
	char pszName[256];
	char Type[16];

	dwMaxSize = 0;

	for (c = 0; c < classes.numclasses; c++) {
		for (a = &ArmourChar[0]; *a; a++) {
			for (w = &WepChar[0]; *w; w++) {
				if (szCel[0] == 'D' && szCel[1] == 'T' && *w != 'N') {
					continue;  // Death has no weapon
				}
				if (szCel[0] == 'B' && szCel[1] == 'L' &&
				    (*w != 'U' && *w != 'D' && *w != 'H')) {
					continue;  // No block without weapon
				}
				sprintf(Type, "%c%c%c", CharChar[c], *a, *w);
				sprintf(pszName, "PlrGFX\\%s\\%s\\%s%s.CL2",
				        classes[c].name.c_str(), Type, Type, szCel);
				if (WOpenFile(pszName, &hsFile, true)) {
					/// ASSERT: assert(hsFile);
					dwSize = WGetFileSize(hsFile, NULL, pszName);
					WCloseFile(hsFile);
					if (dwMaxSize <= dwSize) { dwMaxSize = dwSize; }
				}
			}
		}
	}

	return dwMaxSize;
}


void Player::FreePlayerGFX()
{
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

void Player::NewPlrAnim(uint8_t *Peq, int numFrames, int Delay, int width)
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
	data._pNWidth = 96;
	data._pWWidth = 96;
	data._pAWidth = 128;
	data._pHWidth = 96;
	data._pSWidth = 96;
	data._pDWidth = 128;
	data._pBWidth = 96;

	PlayerClass pc = data._pClass;

	if (lvl.type() == DunType::town) {
		data._pNFrames = classes.get(pc).PlrGFXAnimLens[7];
		data._pWFrames = classes.get(pc).PlrGFXAnimLens[8];
		data._pDFrames = classes.get(pc).PlrGFXAnimLens[4];
		data._pSFrames = classes.get(pc).PlrGFXAnimLens[5];
	} else {
		data._pNFrames = classes.get(pc).PlrGFXAnimLens[0];
		data._pWFrames = classes.get(pc).PlrGFXAnimLens[2];
		data._pAFrames = classes.get(pc).PlrGFXAnimLens[1];
		data._pHFrames = classes.get(pc).PlrGFXAnimLens[6];
		data._pSFrames = classes.get(pc).PlrGFXAnimLens[5];
		data._pDFrames = classes.get(pc).PlrGFXAnimLens[4];
		data._pBFrames = classes.get(pc).PlrGFXAnimLens[3];
		data._pAFNum = classes.get(pc).PlrGFXAnimLens[9];
	}
	data._pSFNum = classes.get(pc).PlrGFXAnimLens[10];

	AnimWeaponId gn = AnimWeaponId(data._pgfxnum & 0xF);
	if (pc == PlayerClass::warrior) {
		if (gn == AnimWeaponId::bow) {
			if (lvl.type() != DunType::town) {
				data._pNFrames = 8;
			}
			data._pAWidth = 96;
			data._pAFNum = 11;
		} else if (gn == AnimWeaponId::axe) {
			data._pAFrames = 20;
			data._pAFNum = 10;
		} else if (gn == AnimWeaponId::staff) {
			data._pAFrames = 16;
			data._pAFNum = 11;
		}
	} else if (pc == PlayerClass::rogue) {
		if (gn == AnimWeaponId::axe) {
			data._pAFrames = 22;
			data._pAFNum = 13;
		} else if (gn == AnimWeaponId::bow) {
			data._pAFrames = 12;
			data._pAFNum = 7;
		} else if (gn == AnimWeaponId::staff) {
			data._pAFrames = 16;
			data._pAFNum = 11;
		}
	} else if (pc == PlayerClass::sorceror) {
		data._pSWidth = 128;
		if (gn == AnimWeaponId::unarmed) {
			data._pAFrames = 20;
		} else if (gn == AnimWeaponId::unarmed_shield) {
			data._pAFNum = 9;
		} else if (gn == AnimWeaponId::bow) {
			data._pAFrames = 20;
			data._pAFNum = 16;
		} else if (gn == AnimWeaponId::axe) {
			data._pAFrames = 24;
			data._pAFNum = 16;
		}
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
void Player::CreatePlayer(PlayerClass c)
{
	char val;
	int hp, mana;
	int i;

	ClearPlrRVars();
	SetRndSeed(SDL_GetTicks());

	data._pClass = c;

	val = classes.get(c).strength;
	if (val < 0) {
		val = 0;
	}
	data._pStrength = val;
	data._pBaseStr = val;

	val = classes.get(c).magic;
	if (val < 0) {
		val = 0;
	}
	data._pMagic = val;
	data._pBaseMag = val;

	val = classes.get(c).dexterity;
	if (val < 0) {
		val = 0;
	}
	data._pDexterity = val;
	data._pBaseDex = val;

	val = classes.get(c).vitality;
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

	if (c == PlayerClass::rogue) {
		data._pDamageMod = data._pLevel * (data._pStrength + data._pDexterity) / 200;
	} else {
		data._pDamageMod = data._pStrength * data._pLevel / 100;
	}

	data._pBaseToBlk = classes.get(c).block;

	data._pHitPoints = (val + 10) << 6;
	if (c == PlayerClass::warrior) {
		data._pHitPoints *= 2;
	}
	if (c == PlayerClass::rogue) {
		data._pHitPoints += data._pHitPoints >> 1;
	}

	hp = data._pHitPoints;
	data._pMaxHP = hp;
	data._pHPBase = hp;
	data._pMaxHPBase = hp;

	data._pMana = data._pMagic << 6;
	if (c == PlayerClass::sorceror) {
		data._pMana *= 2;
	}
	if (c == PlayerClass::rogue) {
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
	data._pInfraFlag = false;

	if (c == PlayerClass::warrior) {
		data._pAblSpells = (__int64)1 << (SpellId::REPAIR - 1);
	} else if (c == PlayerClass::rogue) {
		data._pAblSpells = (__int64)1 << (SpellId::DISARM - 1);
	} else if (c == PlayerClass::sorceror) {
		data._pAblSpells = (__int64)1 << (SpellId::RECHARGE - 1);
	}

	if (c == PlayerClass::sorceror) {
		data._pMemSpells = 1;
	} else {
		data._pMemSpells = 0;
	}

	for (i = 0; i < sizeof(data._pSplLvl) / sizeof(data._pSplLvl[0]); i++) {
		data._pSplLvl[i] = 0;
	}

	data._pSpellFlags = 0;

	if (data._pClass == PlayerClass::sorceror) {
		data._pSplLvl[SpellId::FIREBOLT] = 2;
	}

	// interestingly, only the first three hotkeys are reset
	// TODO: BUGFIX: clear all 4 hotkeys instead of 3 (demo leftover)
	for (i = 0; i < 3; i++) {
		data._pSplHotKey[i] = -1;
	}

	if (c == PlayerClass::warrior) {
		data._pgfxnum = AnimWeaponId::sword_shield;
	} else if (c == PlayerClass::rogue) {
		data._pgfxnum = AnimWeaponId::bow;
	} else if (c == PlayerClass::sorceror) {
		data._pgfxnum = AnimWeaponId::staff;
	}

	for (i = 0; i < NUMLEVELS; i++) {
		data._pLvlVisited[i] = false;
	}

	for (i = 0; i < 10; i++) {
		data._pSetLvlVisited[i] = false;
	}

	data._pLvlChanging = false;
	data.pTownWarps = 0;
	data.pLvlLoad = 0;
	data.pBattleNet = false;
	data.pManaShield = false;

	InitDungMsgs();
	CreatePlrItems();
	SetRndSeed(0);
}

int Player::CalcStatDiff()
{
	PlayerClass c = data._pClass;
	return classes.get(c).MaxStats[AttributeId::STR]
	    - data._pBaseStr +
	       classes.get(c).MaxStats[AttributeId::MAG]
	    - data._pBaseMag +
	       classes.get(c).MaxStats[AttributeId::DEX]
	    - data._pBaseDex +
	       classes.get(c).MaxStats[AttributeId::VIT]
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

	hp = data._pClass == PlayerClass::sorceror ? 64 : 128;
	if (actors.isSingleplayer()) {
		hp++;
	}
	data._pMaxHP += hp;
	data._pHitPoints = data._pMaxHP;
	data._pMaxHPBase += hp;
	data._pHPBase = data._pMaxHPBase;

	if (isMyPlr()) {
		drawhpflag = true;
	}

	if (data._pClass == PlayerClass::warrior)
		mana = 64;
	else
		mana = 128;

	if (actors.isSingleplayer()) {
		mana++;
	}
	data._pMaxMana += mana;
	data._pMaxManaBase += mana;

	if (!(data._pIFlags & ItemSpecialEffect::NOMANA)) {
		data._pMana = data._pMaxMana;
		data._pManaBase = data._pMaxManaBase;
	}

	if (isMyPlr()) {
		drawmanaflag = true;
	}

	if (sgbControllerActive)
		FocusOnCharInfo();
}

void Player::AddPlrExperience(int lvl, int exp)
{
	int powerLvlCap, expCap, newLvl, i;

	if (!isMyPlr()) {
		return;
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
	if (actors.isMultiplayer()) {
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

	NetSendCmdParam1(false, CMD_PLRLEVEL, data._pLevel);
}


void Player::InitPlayer(bool FirstTime)
{
	DWORD i;

	ClearPlrRVars();

	if (FirstTime) {
		data._pRSplType = RSpellType::INVALID;
		data._pRSpell = SpellId::INVALID;
		data._pSBkSpell = SpellId::INVALID;
		data._pSpell = data._pRSpell;
		data._pSplType = data._pRSplType;
		if (AnimWeaponId(data._pgfxnum & 0xF) == AnimWeaponId::bow) {
			data._pwtype = PlayerWeaponType::ranged;
		} else {
			data._pwtype = PlayerWeaponType::melee;
		}
		data.pManaShield = false;
	}

	if (data.plrlevel == lvl.currlevel || leveldebug) {

		SetPlrAnims();

		data._poff = { 0, 0 };
		data._pvel = { 0, 0 };
		ClearPlrPVars();

		if (data._pHitPoints >> 6 > 0) {
			data._pmode = PlayerMode::STAND;
			NewPlrAnim(data._pNAnim[int(Dir::S)], data._pNFrames, 3, data._pNWidth);
			data._pAnimFrame = random_(2, data._pNFrames - 1) + 1;
			data._pAnimCnt = random_(2, 3);
		} else {
			data._pmode = PlayerMode::DEATH;
			NewPlrAnim(data._pDAnim[int(Dir::S)], data._pDFrames, 1, data._pDWidth);
			data._pAnimFrame = data._pAnimLen - 1;
			data._pVar8 = 2 * data._pAnimLen;
		}

		data._pdir = Dir::S;

		if (pnum == myplr()) {
			if (!FirstTime || lvl.currlevel != 0) {
				changePos(View);
			}
			data._pathtarg = pos();
		} else {
			data._pathtarg = pos();

			for (i = 0; i < 8 && !PosOkPlayer(plroff2[i] + pos()); i++)
				;
			changePos(pos() + plroff2[i]);
		}

		changeFutPos(pos());
		ClrPlrPath();
		data.destAction = DestinationAction::NONE;

		if (pnum == myplr()) {
			data._plid = AddLight(pos(), data._pLightRad);
		} else {
			data._plid = -1;
		}
		data._pvid = AddVision(pos(), data._pLightRad, pnum == myplr());
	}

	if (data._pClass == PlayerClass::warrior) {
		data._pAblSpells = 1 << (SpellId::REPAIR - 1);
	} else if (data._pClass == PlayerClass::rogue) {
		data._pAblSpells = 1 << (SpellId::DISARM - 1);
	} else if (data._pClass == PlayerClass::sorceror) {
		data._pAblSpells = 1 << (SpellId::RECHARGE - 1);
	}

	#ifdef _DEBUG
	if (debug_mode_dollar_sign && FirstTime) {
		data._pMemSpells |= 1 << (SpellId::TELEPORT - 1);
		if (!data._pSplLvl[SpellId::TELEPORT]) {
			data._pSplLvl[SpellId::TELEPORT] = 1;
		}
	}
	if (debug_mode_key_inverted_v && FirstTime) {
		data._pMemSpells = SpellId::INVALID;
	}
	#endif

	data._pNextExper = ExpLvlsTbl[data._pLevel];
	data._pInvincible = false;

	if (isMyPlr()) {
		deathdelay = 0;
		deathflag = false;
		ScrollInfo._soff = { 0, 0 };
		ScrollInfo._sdir = ScrollDir::NONE;
	}
}

void InitMultiView()
{
	if ((DWORD)myplr() >= MAX_PLRS) {
		app_fatal("InitPlayer: illegal player %d", myplr());
	}
	View = myplr().pos();
}

bool Player::PlrDirOK(Dir dir)
{
	V2Di p = pos() + offset(dir);

	if (p.x < 0 || !grid.at(p).isPiece() || !PosOkPlayer(p)) return false;

	bool isOk = true;
	if (dir == Dir::E) {
		isOk = !grid.at({ p.x, p.y + 1 }).isSolid() && !(grid[p.x][p.y + 1].dFlags & DunTileFlag::PLAYERLR);
	}

	if (isOk && dir == Dir::W) {
		isOk = !grid.at({ p.x + 1, p.y }).isSolid() && !(grid[p.x + 1][p.y].dFlags & DunTileFlag::PLAYERLR);
	}

	return isOk;
}

void Player::SetPlayerOld()
{
	data._pold = pos();
}

void Player::PlantInPlace(Dir bDir)
{
	changeFutPos(pos());
	data._pathtarg = pos();
	data._poff = { 0, 0 };
	data._pdir = bDir;
	if (pnum == myplr()) {
		ScrollInfo._soff = { 0, 0 };
		ScrollInfo._sdir = ScrollDir::NONE;
		View = pos();
	}
}

void Player::StartStand(Dir dir)
{
	if (!data._pInvincible || data._pHitPoints || pnum != myplr()) {
		if (!(data._pGFXLoad & PlayerGraphicFile::STAND)) {
			LoadPlrGFX(PlayerGraphicFile::STAND);
		}

		NewPlrAnim(data._pNAnim[int(dir)], data._pNFrames, 3, data._pNWidth);
		data._pmode = PlayerMode::STAND;
		PlantInPlace(dir);
		FixPlrWalkTags();
		SetPlayerOld();
	} else {
		SyncPlrKill(-1);
	}
}

void Player::StartWalkStand()
{
	data._pmode = PlayerMode::STAND;
	changeFutPos(pos());
	data._poff = { 0, 0 };

	if (pnum == myplr()) {
		ScrollInfo._soff = { 0, 0 };
		ScrollInfo._sdir = ScrollDir::NONE;
		View = pos();
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

	if (abs(lv.x - off.x) < 3 && abs(lv.y - off.y) < 3) return;

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

	if (pnum == myplr() && int(ScrollInfo._sdir)) {
		ScrollInfo._soff += p;
	}

	PM_ChangeLightOff();
}

void Player::StartWalk(V2Di vel, V2Di add, Dir EndDir, ScrollDir sdir)
{
	if (data._pInvincible && !data._pHitPoints && pnum == myplr()) {
		SyncPlrKill(-1);
		return;
	}

	SetPlayerOld();
	V2Di p = add + pos();
	if (!PlrDirOK(EndDir)) return;
	changeFutPos(p);

	if (pnum == myplr()) ScrollInfo._sd = pos() - View;

	data._pmode = PlayerMode::WALK;
	data._pvel = vel;
	data._poff = { 0, 0 };
	//data._pVar1 = add.x;
	//data._pVar2 = add.y;
	data._pVar3 = int(EndDir);

	if (!(data._pGFXLoad & PlayerGraphicFile::WALK)) {
		LoadPlrGFX(PlayerGraphicFile::WALK);
	}

	NewPlrAnim(data._pWAnim[int(EndDir)], data._pWFrames, 0, data._pWWidth);

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
			ScrollInfo._sdir = ScrollDir::NONE;
		} else {
			ScrollInfo._sdir = sdir;
		}
	} else if (dist >= 2) {
		ScrollInfo._sdir = ScrollDir::NONE;
	} else {
		ScrollInfo._sdir = sdir;
	}
}

#if defined(__clang__) || defined(__GNUC__)
__attribute__((no_sanitize("shift-base")))
#endif
void
Player::StartWalk2(V2Di vel, V2Di off, V2Di add, Dir EndDir, ScrollDir sdir)
{
	if (data._pInvincible && !data._pHitPoints && pnum == myplr()) {
		SyncPlrKill(-1);
		return;
	}

	SetPlayerOld();
	V2Di p = add + pos();
	if (!PlrDirOK(EndDir)) return;
	changeFutPos(p);
	changeDrawPos(p);
	if (pnum == myplr()) ScrollInfo._sd = pos() - View;
	//data._pVar1 = pos().x;
	//data._pVar2 = pos().y;
	data._poff = off;

	ChangeLightXY(data._plid, pos());
	PM_ChangeLightOff();

	data._pmode = PlayerMode::WALK2;
	data._pvel = vel;
	data._pVar6 = off.x * 256;
	data._pVar7 = off.y * 256;
	data._pVar3 = int(EndDir);

	if (!(data._pGFXLoad & PlayerGraphicFile::WALK)) LoadPlrGFX(PlayerGraphicFile::WALK);
	NewPlrAnim(data._pWAnim[int(EndDir)], data._pWFrames, 0, data._pWWidth);

	data._pdir = EndDir;
	data._pVar8 = 0;

	if (pnum != myplr()) return;

	int dist = (ScrollInfo._sd).maxabs();
	if (zoomflag) {
		if (dist >= 3) {
			ScrollInfo._sdir = ScrollDir::NONE;
		} else {
			ScrollInfo._sdir = sdir;
		}
	} else if (dist >= 2) {
		ScrollInfo._sdir = ScrollDir::NONE;
	} else {
		ScrollInfo._sdir = sdir;
	}
}

#if defined(__clang__) || defined(__GNUC__)
__attribute__((no_sanitize("shift-base")))
#endif
void
Player::StartWalk3(V2Di vel, V2Di off, V2Di add, V2Di map, Dir EndDir, ScrollDir sdir)
{
	V2Di p, n;

	if (data._pInvincible && !data._pHitPoints && pnum == myplr()) {
		SyncPlrKill(-1);
		return;
	}

	SetPlayerOld();
	p = add + pos();
	n = map + pos();

	if (!PlrDirOK(EndDir)) return;

	changeFutPos(p);
	changeDrawPos(n);

	if (pnum == myplr()) {
		ScrollInfo._sd = pos() - View;
	}

	//data._pVar4 = n.x;
	//data._pVar5 = n.y;
	grid.at(n).dFlags |= DunTileFlag::PLAYERLR;
	data._poff = off;

	if (lvl.type() != DunType::town) {
		ChangeLightXY(data._plid, n);
		PM_ChangeLightOff();
	}

	data._pmode = PlayerMode::WALK3;
	data._pvel = vel;
	//data._pVar1 = p.x;
	//data._pVar2 = p.y;
	data._pVar6 = off.x * 256;
	data._pVar7 = off.y * 256;
	data._pVar3 = int(EndDir);

	if (!(data._pGFXLoad & PlayerGraphicFile::WALK)) {
		LoadPlrGFX(PlayerGraphicFile::WALK);
	}
	NewPlrAnim(data._pWAnim[int(EndDir)], data._pWFrames, 0, data._pWWidth);

	data._pdir = EndDir;
	data._pVar8 = 0;

	if (pnum != myplr()) {
		return;
	}
	int dist = (ScrollInfo._sd).maxabs();
	if (zoomflag) {
		if (dist >= 3) {
			ScrollInfo._sdir = ScrollDir::NONE;
		} else {
			ScrollInfo._sdir = sdir;
		}
	} else if (dist >= 2) {
		ScrollInfo._sdir = ScrollDir::NONE;
	} else {
		ScrollInfo._sdir = sdir;
	}
}

void Player::StartAttack(Dir d)
{

	if (data._pInvincible && !data._pHitPoints && pnum == myplr()) {
		SyncPlrKill(-1);
		return;
	}

	if (!(data._pGFXLoad & PlayerGraphicFile::ATTACK)) {
		LoadPlrGFX(PlayerGraphicFile::ATTACK);
	}

	NewPlrAnim(data._pAAnim[int(d)], data._pAFrames, 0, data._pAWidth);
	data._pmode = PlayerMode::ATTACK;
	PlantInPlace(d);
	SetPlayerOld();
}

void Player::StartRangeAttack(Dir d, V2Di c)
{
	if (data._pInvincible && !data._pHitPoints && pnum == myplr()) {
		SyncPlrKill(-1);
		return;
	}

	if (!(data._pGFXLoad & PlayerGraphicFile::ATTACK)) {
		LoadPlrGFX(PlayerGraphicFile::ATTACK);
	}
	NewPlrAnim(data._pAAnim[int(d)], data._pAFrames, 0, data._pAWidth);

	data._pmode = PlayerMode::RATTACK;
	PlantInPlace(d);
	SetPlayerOld();
	data._pVar1 = c.x;
	data._pVar2 = c.y;
}

void Player::StartPlrBlock(Dir dir)
{

	if (data._pInvincible && !data._pHitPoints && pnum == myplr()) {
		SyncPlrKill(-1);
		return;
	}

	PlaySfxLoc(IS_ISWORD, pos());

	if (!(data._pGFXLoad & PlayerGraphicFile::BLOCK)) {
		LoadPlrGFX(PlayerGraphicFile::BLOCK);
	}
	NewPlrAnim(data._pBAnim[int(dir)], data._pBFrames, 2, data._pBWidth);

	data._pmode = PlayerMode::BLOCK;
	PlantInPlace(dir);
	SetPlayerOld();
}

void Player::StartSpell(Dir d, V2Di c)
{
	if (data._pInvincible && !data._pHitPoints && pnum == myplr()) {
		SyncPlrKill(-1);
		return;
	}

	if (lvl.type() != DunType::town) {
		switch (spelldata[data._pSpell].sType) {
		case MagicType::FIRE:
			if (!(data._pGFXLoad & PlayerGraphicFile::FIRE)) {
				LoadPlrGFX(PlayerGraphicFile::FIRE);
			}
			NewPlrAnim(data._pFAnim[int(d)], data._pSFrames, 0, data._pSWidth);
			break;
		case MagicType::LIGHTNING:
			if (!(data._pGFXLoad & PlayerGraphicFile::LIGHTNING)) {
				LoadPlrGFX(PlayerGraphicFile::LIGHTNING);
			}
			NewPlrAnim(data._pLAnim[int(d)], data._pSFrames, 0, data._pSWidth);
			break;
		case MagicType::MAGIC:
			if (!(data._pGFXLoad & PlayerGraphicFile::MAGIC)) {
				LoadPlrGFX(PlayerGraphicFile::MAGIC);
			}
			NewPlrAnim(data._pTAnim[int(d)], data._pSFrames, 0, data._pSWidth);
			break;
		}
	}

	PlaySfxLoc(spelldata[data._pSpell].sSFX, pos());

	data._pmode = PlayerMode::SPELL;

	PlantInPlace(d);
	SetPlayerOld();

	data._pVar1 = c.x;
	data._pVar2 = c.y;
	data._pVar4 = GetSpellLevel(pnum, data._pSpell);
	data._pVar8 = 1;
}

void Player::FixPlrWalkTags()
{
	V2Di d = data._pold;

	//// Covers 9-squares around player position
	//V2Di i;
	//for (i.y = d.y - 1; i.y <= d.y + 1; i.y++) {
	//	for (i.x = d.x - 1; i.x <= d.x + 1; i.x++) {
	//		if (grid.isValid(i) && grid.at(i).isPlayer() && grid.at(i).getPlayer() == pnum) {
	//			throw; // Should have made it so that you no longer have errant player positions
	//		}
	//	}
	//}

	if (grid.isValid(d)) {
		grid[d.x + 1][d.y].dFlags &= ~DunTileFlag::PLAYERLR;
		grid[d.x][d.y + 1].dFlags &= ~DunTileFlag::PLAYERLR;
	}
}

void Player::RemoveFromMap()
{
	// Check if at pos and remove
	if (grid.at(pos()).isPlayer()) {
		if (grid.at(pos()).getPlayer() == pnum) {
			grid.at(pos()).clearActor(*this);
		}
	}

	// Check if futpos and remove
	if (grid.at(futpos()).isPlayer()) {
		if (grid.at(futpos()).getPlayer() == pnum) {
			grid.at(futpos()).clearActor(*this);
		}
	}

	// Check if drawpos is set and remove
	if (grid.at(drawpos()).isPlayerDraw()) {
		if (grid.at(drawpos()).getPlayer() == pnum) {
			grid.at(drawpos()).clearActor(*this);
		}
	}

	//V2Di i;
	//for (i.y = 1; i.y < MAXDUNY; i.y++)
	//	for (i.x = 1; i.x < MAXDUNX; i.x++)
	//		if (grid.at(i).isPlayer() && grid.at(i).getPlayer() == pnum)
	//			if (grid.at(i).dFlags & DunTileFlag::PLAYERLR)
	//				grid.at(i).dFlags &= ~DunTileFlag::PLAYERLR;

	//for (i.y = 0; i.y < MAXDUNY; i.y++)
	//	for (i.x = 0; i.x < MAXDUNX; i.x++)
	//		if (grid.at(i).isPlayer() && grid.at(i).getPlayer() == pnum)
	//			throw; // Should have made it so that you no longer have errant player positions
}

void Player::StartPlrHit(int dam, bool forcehit)
{
	Dir pd;

	if (data._pInvincible && !data._pHitPoints && pnum == myplr()) {
		SyncPlrKill(-1);
		return;
	}

	if (data._pClass == PlayerClass::warrior) {
		PlaySfxLoc(PS_WARR69, pos());
	} else if (data._pClass == PlayerClass::rogue) {
		PlaySfxLoc(PS_ROGUE69, pos());
	} else if (data._pClass == PlayerClass::sorceror) {
		PlaySfxLoc(PS_MAGE69, pos());
	}

	drawhpflag = true;
	if (dam >> 6 >= data._pLevel || forcehit) {
		pd = data._pdir;

		if (!(data._pGFXLoad & PlayerGraphicFile::HIT)) {
			LoadPlrGFX(PlayerGraphicFile::HIT);
		}
		NewPlrAnim(data._pHAnim[int(pd)], data._pHFrames, 0, data._pHWidth);
		data._pmode = PlayerMode::GOTHIT;
		PlantInPlace(pd);
		data._pVar8 = 1;
		FixPlrWalkTags();
		SetPlayerOld();
	}
}

void RespawnDeadItem(Item &item, V2Di pos)
{
	grid.at(pos).setItem(item.id);
	item._i = pos;
	item.RespawnItem(true);
	item._itype = ItemType::none;
}

#if defined(__clang__) || defined(__GNUC__)
__attribute__((no_sanitize("shift-base")))
#endif
void Player::StartPlayerKill()
{
	bool diablolevel;
	int i;
	Dir pdd;
	Item * pi;

	if (data._pHitPoints <= 0 && data._pmode == PlayerMode::DEATH) {
		return;
	}

	if (myplr() == pnum) {
		bool earflag = false;
		NetSendCmdParam1(true, CMD_PLRDEAD, earflag);
	}

	diablolevel = plr.isMultiplayer() && data.plrlevel == 16;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("StartPlayerKill: illegal player %d", pnum);
	}

	if (data._pClass == PlayerClass::warrior) {
		PlaySfxLoc(PS_DEAD, pos()); // BUGFIX: should use `PS_WARR71` like other classes
	} else if (data._pClass == PlayerClass::rogue) {
		PlaySfxLoc(PS_ROGUE71, pos());
	} else if (data._pClass == PlayerClass::sorceror) {
		PlaySfxLoc(PS_MAGE71, pos());
	}

	if (data._pgfxnum) {
		data._pgfxnum = 0;
		data._pGFXLoad = 0;
		SetPlrAnims();
	}

	if (!(data._pGFXLoad & PlayerGraphicFile::DEATH)) {
		LoadPlrGFX(PlayerGraphicFile::DEATH);
	}

	NewPlrAnim(data._pDAnim[int(data._pdir)], data._pDFrames, 1, data._pDWidth);

	data._pBlockFlag = false;
	data._pmode = PlayerMode::DEATH;
	data._pInvincible = true;
	SetPlayerHitPoints(0);
	data._pVar8 = 1;

	if (pnum != myplr() && !diablolevel) {
		for (InvSlot &i : inv.getBagArray()) {
			if (!i.item()) continue;
			i.item()->_itype = ItemType::none;
		}
		CalcPlrInv(false);
	}

	if (data.plrlevel == lvl.currlevel) {
		PlantInPlace(data._pdir);
		RemoveFromMap();
		grid.at(pos()).dFlags |= DunTileFlag::DEAD_PLAYER;
		SetPlayerOld();

		if (pnum == myplr()) {
			drawhpflag = true;
			deathdelay = 30;

			if (pcurs >= CURSOR_FIRSTITEM) {
				assert(data.HoldItem);
				PlrDeadItem(*data.HoldItem, { 0, 0 });
				SetCursor_(CURSOR_HAND);
			}

			if (!diablolevel) {
				DropHalfPlayersGold();
				pi = data.InvBody[0];
				i = MAXINVITEMS;
				while (i--) {
					pdd = rotate(data._pdir, i);
					assert(pi);
					PlrDeadItem(*pi, offset(pdd));
					pi++;
				}
				CalcPlrInv(pnum, false);
			}
		}
	}
	SetPlayerHitPoints(0);
}

void Player::PlrDeadItem(Item & item, V2Di n)
{
	if (item._itype == ItemType::none) return;

	V2Di p = n + pos();
	if ((n.x || n.y) && ItemSpaceOk(p.x, p.y)) {
		RespawnDeadItem(item, p);
		data.HoldItem = &item;
		NetSendCmdPItem(false, CMD_RESPAWNITEM, p.x, p.y);
		return;
	}

	for (int k = 1; k < 50; k++) {
		for (int j = -k; j <= k; j++) {
			p.y = j + pos().y;
			for (int i = -k; i <= k; i++) {
				p.x = i + pos().x;
				if (ItemSpaceOk(p.x, p.y)) {
					RespawnDeadItem(item, p);
					data.HoldItem = &item;
					NetSendCmdPItem(false, CMD_RESPAWNITEM, p.x, p.y);
					return;
				}
			}
		}
	}
}

void Player::DropHalfPlayersGold()
{
	const int hGold = data._pGold >> 1;
	// Go through all piles of gold in inventory and take half
	data._pGold = inv.CalculateGold();
}

void Player::SyncPlrKill()
{
	if (data._pHitPoints == 0 && lvl.currlevel == 0) {
		SetPlayerHitPoints(64);
		return;
	}

	for (int i = 0; i < nummissiles; i++) {
		int ma = missileactive[i];
		if (missile[ma]._mitype == MissileType::MANASHIELD && missile[ma]._misource == pnum && missile[ma]._miDelFlag == false) {
			return;
		}
	}

	SetPlayerHitPoints(0);
	StartPlayerKill();
}

void Player::RemovePlrMissiles()
{
	int i, am;
	V2Di m;

	if (lvl.currlevel != 0 && pnum == myplr() && (monsters[myplr()].data._m.x != 1 || monsters[myplr()].data._m.y != 0)) {
		monsters[myplr()].M_StartKill(myplr());
		AddDead(monsters[myplr()].data._m, (monsters[myplr()].data.MType)->mdeadval, monsters[myplr()].data._mdir);
		m = monsters[myplr()].data._m;
		grid.at(m).clearMonster();
		monsters[myplr()].data._mDelFlag = true;
		DeleteMonsterList();
	}

	for (i = 0; i < nummissiles; i++) {
		am = missileactive[i];
		if (missile[am]._mitype == MissileType::STONE && missile[am]._misource == pnum) {
			monsters[missile[am]._miVar2].data._mmode = missile[am]._miVar1;
		}
		if (missile[am]._mitype == MissileType::MANASHIELD && missile[am]._misource == pnum) {
			ClearMissileSpot(am);
			DeleteMissile(am, i);
		}
		if (missile[am]._mitype == MissileType::ETHEREALIZE && missile[am]._misource == pnum) {
			ClearMissileSpot(am);
			DeleteMissile(am, i);
		}
	}
}

void Player::InitLevelChange()
{
	RemovePlrMissiles();
	if (pnum == myplr() && qtextflag) {
		qtextflag = false;
		stream_stop();
	}

	RemoveFromMap();
	SetPlayerOld();
	if (pnum == myplr()) {
		addToMap();
	} else {
		data._pLvlVisited[data.plrlevel] = true;
	}

	ClrPlrPath();
	data.destAction = DestinationAction::NONE;
	data._pLvlChanging = true;

	if (pnum == myplr()) {
		data.pLvlLoad = 10;
	}
}

#if defined(__clang__) || defined(__GNUC__)
__attribute__((no_sanitize("shift-base")))
#endif
void Player::StartNewLvl(int fom, int curlvl)
{
	InitLevelChange();

	switch (fom) {
	case WM_DIABNEXTLVL:
	case WM_DIABPREVLVL:
		data.plrlevel = curlvl;
		break;
	case WM_DIABRTNLVL:
	case WM_DIABTOWNWARP:
		data.plrlevel = curlvl;
		break;
	case WM_DIABSETLVL:
		lvl.setlvlnum = SetLvl(curlvl);
		break;
	case WM_DIABTWARPUP:
		data.pTownWarps |= 1 << (int(lvl.type()) - 2);
		data.plrlevel = curlvl;
		break;
	case WM_DIABRETOWN:
		break;
	default:
		app_fatal("StartNewLvl");
		break;
	}

	if (pnum == myplr()) {
		data._pmode = PlayerMode::NEWLVL;
		data._pInvincible = true;
		PostMessage(fom, 0, 0);
		if (plr.isMultiplayer()) {
			NetSendCmdParam2(true, CMD_NEWLVL, fom, curlvl);
		}
	}
}
void Player::RestartTownLvl()
{
	InitLevelChange();

	data.plrlevel = 0;
	data._pInvincible = false;

	SetPlayerHitPoints(64);

	data._pMana = 0;
	data._pManaBase = data._pMana - (data._pMaxMana - data._pMaxManaBase);

	CalcPlrInv(false);

	if (pnum == myplr()) {
		data._pmode = PlayerMode::NEWLVL;
		data._pInvincible = true;
		PostMessage(WM_DIABRETOWN, 0, 0);
	}
}

void Player::StartWarpLvl(int pidx)
{
	InitLevelChange();

	if (plr.isMultiplayer()) {
		if (data.plrlevel != 0) {
			data.plrlevel = 0;
		} else {
			data.plrlevel = portal[pidx].level;
		}
	}

	if (pnum == myplr()) {
		SetCurrentPortal(pidx);
		data._pmode = PlayerMode::NEWLVL;
		data._pInvincible = true;
		PostMessage(WM_DIABWARPLVL, 0, 0);
	}
}

bool Player::PM_DoStand()
{
	return false;
}

void Player::StartWalk(Dir walkdir)
{
	int xvel3;
	V2Di vel;
	if (lvl.currlevel != 0) {
		xvel3 = classes[data._pClass].PWVel[0];
		vel.x = classes[data._pClass].PWVel[1];
		vel.y = classes[data._pClass].PWVel[2];
	} else {
		xvel3 = 2048;
		vel = { 1024, 512 };
	}

	switch (walkdir) {
	case Dir::N:
			StartWalk({ 0, -vel.x }, { -1, -1 }, Dir::N, ScrollDir::N);
			break;
		case Dir::NE:
			StartWalk({ vel.x, -vel.y }, { 0, -1 }, Dir::NE, ScrollDir::NE);
			break;
		case Dir::E:
			StartWalk3({ xvel3, 0 }, { -32, -16 }, { 1, -1 }, { 1, 0 }, Dir::E, ScrollDir::E);
			break;
		case Dir::SE:
			StartWalk2(vel, { -32, -16 }, { 1, 0 }, Dir::SE, ScrollDir::SE);
			break;
		case Dir::S:
			StartWalk2({ 0, vel.x }, { 0, -32 }, { 1, 1 }, Dir::S, ScrollDir::S);
			break;
		case Dir::SW:
			StartWalk2({ -vel.x, vel.y }, { 32, -16 }, { 0, 1 }, Dir::SW, ScrollDir::SW);
			break;
		case Dir::W:
			StartWalk3({ -xvel3, 0 }, { 32, -16 }, { -1, 1 }, { 0, 1 }, Dir::W, ScrollDir::W);
			break;
		case Dir::NW:
			StartWalk({ -vel.x, -vel.y }, { -1, 0 }, Dir::NW, ScrollDir::NW);
			break;
	}
}


bool Player::PM_DoWalk()
{
	if (data._pAnimFrame == 3
	    || (data._pWFrames == 8 && data._pAnimFrame == 7)
	    || (data._pWFrames != 8 && data._pAnimFrame == 4)) {
		PlaySfxLoc(PS_WALK1, pos());
	}

	int anim_len = 8;
	if (lvl.currlevel != 0) {
		anim_len = classes[data._pClass].AnimLenFromClass;
	}

	if (data._pVar8 == anim_len) { // end of anim
		advancePos();

		if (lvl.type() != DunType::town) {
			ChangeLightXY(data._plid, pos());
			ChangeVisionXY(data._pvid, pos());
		}

		if (pnum == myplr() && int(ScrollInfo._sdir)) {
			View = pos() - ScrollInfo._sd;
		}

		if (!data.wkpath.empty()) {
			StartWalkStand();
		} else {
			StartStand(Dir(data._pVar3));
		}

		ClearPlrPVars();

		if (lvl.type() != DunType::town) {
			ChangeLightOff(data._plid, { 0, 0 });
		}
		return true;
	} else {
		PM_ChangeOffset();
		return false;
	}
}

bool Player::PM_DoWalk2()
{
	if (data._pAnimFrame == 3
	    || (data._pWFrames == 8 && data._pAnimFrame == 7)
	    || (data._pWFrames != 8 && data._pAnimFrame == 4)) {
		PlaySfxLoc(PS_WALK1, pos());
	}

	int anim_len = 8;
	if (lvl.currlevel != 0) {
		anim_len = classes[data._pClass].AnimLenFromClass;
	}

	if (data._pVar8 == anim_len) { // end of anim
		advancePos();

		if (lvl.type() != DunType::town) {
			ChangeLightXY(data._plid, pos());
			ChangeVisionXY(data._pvid, pos());
		}

		if (pnum == myplr() && int(ScrollInfo._sdir)) {
			View = pos() - ScrollInfo._sd;
		}

		if (!data.wkpath.empty()) {
			StartWalkStand();
		} else {
			StartStand(Dir(data._pVar3));
		}

		ClearPlrPVars();
		if (lvl.type() != DunType::town) {
			ChangeLightOff(data._plid, { 0, 0 });
		}
		return true;
	} else {
		PM_ChangeOffset();
		return false;
	}
}

bool Player::PM_DoWalk3()
{
	if (data._pAnimFrame == 3
	    || (data._pWFrames == 8 && data._pAnimFrame == 7)
	    || (data._pWFrames != 8 && data._pAnimFrame == 4)) {
		PlaySfxLoc(PS_WALK1, pos());
	}

	int anim_len = 8;
	if (lvl.currlevel != 0) {
		anim_len = classes[data._pClass].AnimLenFromClass;
	}

	if (data._pVar8 == anim_len) { // end of anim
		grid[data._pVar4][data._pVar5].dFlags &= ~DunTileFlag::PLAYERLR; // makes old pos have lr flag
		advancePos();

		if (lvl.type() != DunType::town) {
			ChangeLightXY(data._plid, pos());
			ChangeVisionXY(data._pvid, pos());
		}

		if (pnum == myplr() && int(ScrollInfo._sdir)) {
			View = pos() - ScrollInfo._sd;
		}

		if (!data.wkpath.empty()) {
			StartWalkStand();
		} else {
			StartStand(Dir(data._pVar3));
		}

		ClearPlrPVars();

		if (lvl.type() != DunType::town) {
			ChangeLightOff(data._plid, { 0, 0 });
		}
		return true;
	} else {
		PM_ChangeOffset();
		return false;
	}
}

bool Player::PlrHitMonst(int m)
{
	bool rv, ret;
	int hit, hper, mind, maxd, ddp, dam, skdam, phanditype, tmac;

	if ((DWORD)m >= MAXMONSTERS) {
		app_fatal("PlrHitMonst: illegal monster %d", m);
	}

	if ((monsters[m].data._mhitpoints >> 6) <= 0) {
		return false;
	}

	if (monsters[m].data.MType->mtype == MonsterType::ILLWEAV && monsters[m].data._mgoal == MonsterGoal::RETREAT) {
		return false;
	}

	if (monsters[m].data._mmode == MonsterMode::CHARGE) {
		return false;
	}

	rv = false;

	hit = random_(4, 100);
	if (monsters[m].data._mmode == MonsterMode::STONE) {
		hit = 0;
	}

	tmac = monsters[m].data.mArmorClass - data._pIEnAc;
	hper = (data._pDexterity >> 1) + data._pLevel + 50 - tmac;
	if (data._pClass == PlayerClass::warrior) {
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
		if (data._pClass == PlayerClass::warrior) {
			ddp = data._pLevel;
			if (random_(6, 100) < ddp) {
				dam *= 2;
			}
		}

		phanditype = ItemType::none;
		if (data.InvBody[INVLOC_HAND_LEFT]._itype == ItemType::sword || data.InvBody[INVLOC_HAND_RIGHT]._itype == ItemType::sword) {
			phanditype = ItemType::sword;
		}
		if (data.InvBody[INVLOC_HAND_LEFT]._itype == ItemType::mace || data.InvBody[INVLOC_HAND_RIGHT]._itype == ItemType::mace) {
			phanditype = ItemType::mace;
		}

		switch (monsters[m].data.MData->mMonstClass) {
		case MonstClassId::undead:
			if (phanditype == ItemType::sword) {
				dam -= dam >> 1;
			}
			if (phanditype == ItemType::mace) {
				dam += dam >> 1;
			}
			break;
		case MonstClassId::animal:
			if (phanditype == ItemType::mace) {
				dam -= dam >> 1;
			}
			if (phanditype == ItemType::sword) {
				dam += dam >> 1;
			}
			break;
		}

		if (data._pIFlags & ItemSpecialEffect::T3XDAMVDEM && monsters[m].data.MData->mMonstClass == MonstClassId::demon) {
			dam *= 3;
		}

		dam <<= 6;
		if (pnum == myplr()) {
			monsters[m].data._mhitpoints -= dam;
		}

		if (data._pIFlags & ItemSpecialEffect::RNDSTEALLIFE) {
			skdam = random_(7, dam >> 3);
			data._pHitPoints += skdam;
			if (data._pHitPoints > data._pMaxHP) {
				data._pHitPoints = data._pMaxHP;
			}
			data._pHPBase += skdam;
			if (data._pHPBase > data._pMaxHPBase) {
				data._pHPBase = data._pMaxHPBase;
			}
			drawhpflag = true;
		}
		if (data._pIFlags & (ItemSpecialEffect::STEALMANA_3 | ItemSpecialEffect::STEALMANA_5) && !(data._pIFlags & ItemSpecialEffect::NOMANA)) {
			if (data._pIFlags & ItemSpecialEffect::STEALMANA_3) {
				skdam = 3 * dam / 100;
			}
			if (data._pIFlags & ItemSpecialEffect::STEALMANA_5) {
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
			drawmanaflag = true;
		}
		if (data._pIFlags & (ItemSpecialEffect::STEALLIFE_3 | ItemSpecialEffect::STEALLIFE_5)) {
			if (data._pIFlags & ItemSpecialEffect::STEALLIFE_3) {
				skdam = 3 * dam / 100;
			}
			if (data._pIFlags & ItemSpecialEffect::STEALLIFE_5) {
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
			drawhpflag = true;
		}
		if (data._pIFlags & ItemSpecialEffect::NOHEALPLR) {
			monsters[m].data._mFlags |= MonsterFlag::noheal;
		}
#ifdef _DEBUG
		if (debug_mode_dollar_sign || debug_mode_key_inverted_v) {
			monsters[m].data._mhitpoints = 0; /* double check */
		}
#endif
		if ((monsters[m].data._mhitpoints >> 6) <= 0) {
			if (monsters[m].data._mmode == MonsterMode::STONE) {
				monsters[m].M_StartKill(pnum);
				monsters[m].data._mmode = MonsterMode::STONE;
			} else {
				monsters[m].M_StartKill(pnum);
			}
		} else {
			if (monsters[m].data._mmode == MonsterMode::STONE) {
				monsters[m].M_StartHit(pnum, dam);
				monsters[m].data._mmode = MonsterMode::STONE;
			} else {
				if (data._pIFlags & ItemSpecialEffect::KNOCKBACK) {
					monsters[m].M_GetKnockback();
				}
				monsters[m].M_StartHit(pnum, dam);
			}
		}
		rv = true;
	}

	return rv;
}

bool Player::PlrHitPlr(char p)
{
	bool rv;
	int hit, hper, blk, blkper, mind, maxd, dam, lvl, skdam, tac;
	Dir dir;

	if ((DWORD)p >= MAX_PLRS) {
		app_fatal("PlrHitPlr: illegal target player %d", p);
	}

	rv = false;

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

	if (data._pClass == PlayerClass::warrior) {
		hper += 20;
	}
	hper += data._pIBonusToHit;
	if (hper < 5) {
		hper = 5;
	}
	if (hper > 95) {
		hper = 95;
	}

	if ((plr[p].data._pmode == PlayerMode::STAND || plr[p].data._pmode == PlayerMode::ATTACK) && plr[p].data._pBlockFlag) {
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
			dir = GetDirection(plr[p].pos(), pos());
			plr[p].StartPlrBlock(dir);
		} else {
			mind = data._pIMinDam;
			maxd = random_(5, data._pIMaxDam - mind + 1);
			dam = maxd + mind;
			dam += data._pDamageMod + data._pIBonusDamMod + dam * data._pIBonusDam / 100;

			if (data._pClass == PlayerClass::warrior) {
				lvl = data._pLevel;
				if (random_(6, 100) < lvl) {
					dam *= 2;
				}
			}
			skdam = dam << 6;
			if (data._pIFlags & ItemSpecialEffect::RNDSTEALLIFE) {
				tac = random_(7, skdam >> 3);
				data._pHitPoints += tac;
				if (data._pHitPoints > data._pMaxHP) {
					data._pHitPoints = data._pMaxHP;
				}
				data._pHPBase += tac;
				if (data._pHPBase > data._pMaxHPBase) {
					data._pHPBase = data._pMaxHPBase;
				}
				drawhpflag = true;
			}
			if (pnum == myplr()) {
				NetSendCmdDamage(true, p, skdam);
			}
			plr[p].StartPlrHit(skdam, false);
		}

		rv = true;
	}

	return rv;
}

bool Player::PlrHitObj(V2Di m)
{
	int oi = grid.at(m).getObject();
	if (object[oi]._oBreak == 1) {
		BreakObject(pnum, oi);
		return true;
	}
	return false;
}

bool Player::PM_DoAttack()
{
	Dir dir;
	int frame, m;
	bool didhit;
	V2Di d;
	frame = data._pAnimFrame;
	if (data._pIFlags & ItemSpecialEffect::QUICKATTACK && frame == 1) {
		data._pAnimFrame++;
	}
	if (data._pIFlags & ItemSpecialEffect::FASTATTACK && (frame == 1 || frame == 3)) {
		data._pAnimFrame++;
	}
	if (data._pIFlags & ItemSpecialEffect::FASTERATTACK && (frame == 1 || frame == 3 || frame == 5)) {
		data._pAnimFrame++;
	}
	if (data._pIFlags & ItemSpecialEffect::FASTESTATTACK && (frame == 1 || frame == 4)) {
		data._pAnimFrame += 2;
	}
	if (data._pAnimFrame == data._pAFNum - 1) {
		PlaySfxLoc(PS_SWING, pos());
	}

	if (data._pAnimFrame == data._pAFNum) {
		dir = data._pdir;
		d = pos() + offset(dir);
		if (grid.at(d).getActor()) {
			if (grid.at(d).getActor() > 0) {
				m = grid.at(d).getActor() - 1;
			} else {
				m = -(grid.at(d).getActor() + 1);
			}
			if (CanTalkToMonst(m)) {
				data._pVar1 = 0;
				return false;
			}
		}

		if (data._pIFlags & ItemSpecialEffect::FIREDAM) {
			AddMissile(d, { 1, 0 }, Dir(0), MissileType::WEAPEXP, 0, pnum, 0, 0);
		}
		if (data._pIFlags & ItemSpecialEffect::LIGHTDAM) {
			AddMissile(d, { 2, 0 }, Dir(0), MissileType::WEAPEXP, 0, pnum, 0, 0);
		}

		didhit = false;
		if (grid.at(d).getActor()) {
			m = grid.at(d).getActor();
			if (grid.at(d).getActor() > 0) {
				m = grid.at(d).getActor() - 1;
			} else {
				m = -(grid.at(d).getActor() + 1);
			}
			didhit = PlrHitMonst(m);
		} else if (grid.at(d).isPlayer() && !FriendlyMode) {
			uint8_t p = grid.at(d).getPlayer();
			didhit = PlrHitPlr(p);
		} else if (grid.at(d).isObject()) {
			didhit = PlrHitObj(d);
		}

		if (didhit && WeaponDur(30)) {
			StartStand(data._pdir);
			ClearPlrPVars();
			return true;
		}
	}

	if (data._pAnimFrame == data._pAFrames) {
		StartStand(data._pdir);
		ClearPlrPVars();
		return true;
	} else {
		return false;
	}
}

bool Player::PM_DoRangeAttack()
{
	int origFrame, mistype;

	origFrame = data._pAnimFrame;
	if (data._pIFlags & ItemSpecialEffect::QUICKATTACK && origFrame == 1) {
		data._pAnimFrame++;
	}
	if (data._pIFlags & ItemSpecialEffect::FASTATTACK && (origFrame == 1 || origFrame == 3)) {
		data._pAnimFrame++;
	}

	if (data._pAnimFrame == data._pAFNum) {
		mistype = MissileType::ARROW;
		if (data._pIFlags & ItemSpecialEffect::FIRE_ARROWS) {
			mistype = MissileType::FARROW;
		}
		if (data._pIFlags & ItemSpecialEffect::LIGHT_ARROWS) {
			mistype = MissileType::LARROW;
		}
		AddMissile(
		    pos(),
		    { data._pVar1, data._pVar2 },
		    data._pdir,
		    mistype,
		    0,
		    pnum,
		    4,
		    0);

		PlaySfxLoc(PS_BFIRE, pos());

		if (WeaponDur(40)) {
			StartStand(data._pdir);
			ClearPlrPVars();
			return true;
		}
	}

	if (data._pAnimFrame >= data._pAFrames) {
		StartStand(data._pdir);
		ClearPlrPVars();
		return true;
	} else {
		return false;
	}
}

bool Player::PM_DoBlock()
{
	if (data._pIFlags & ItemSpecialEffect::FASTBLOCK && data._pAnimFrame != 1) {
		data._pAnimFrame = data._pBFrames;
	}

	if (data._pAnimFrame >= data._pBFrames) {
		StartStand(data._pdir);
		ClearPlrPVars();

		if (!random_(3, 10)) {
			ShieldDur();
		}
		return true;
	}

	return false;
}

bool Player::PM_DoSpell()
{
	if (data._pVar8 == data._pSFNum) {
		CastSpell(
		    pnum,
		    data._pSpell,
		    pos(),
		    { data._pVar1, data._pVar2 },
		    0,
		    data._pVar4);

		if (!data._pSplFrom) {
			if (data._pRSplType == RSpellType::SCROLL) {
				if (!(data._pScrlSpells
				        & (unsigned __int64)1 << (data._pRSpell - 1))) {
					data._pRSpell = SpellId::INVALID;
					data._pRSplType = RSpellType::INVALID;
					force_redraw = 255;
				}
			}

			if (data._pRSplType == RSpellType::CHARGES) {
				if (!(data._pISpells
				        & (unsigned __int64)1 << (data._pRSpell - 1))) {
					data._pRSpell = SpellId::INVALID;
					data._pRSplType = RSpellType::INVALID;
					force_redraw = 255;
				}
			}
		}
	}

	data._pVar8++;

	if (lvl.type() == DunType::town) {
		if (data._pVar8 > data._pSFrames) {
			StartWalkStand();
			ClearPlrPVars();
			return true;
		}
	} else if (data._pAnimFrame == data._pSFrames) {
		StartStand(data._pdir);
		ClearPlrPVars();
		return true;
	}

	return false;
}

bool Player::PM_DoGotHit()
{
	int frame;

	frame = data._pAnimFrame;
	if (data._pIFlags & ItemSpecialEffect::FASTRECOVER && frame == 3) {
		data._pAnimFrame++;
	}
	if (data._pIFlags & ItemSpecialEffect::FASTERRECOVER && (frame == 3 || frame == 5)) {
		data._pAnimFrame++;
	}
	if (data._pIFlags & ItemSpecialEffect::FASTESTRECOVER && (frame == 1 || frame == 3 || frame == 5)) {
		data._pAnimFrame++;
	}

	if (data._pAnimFrame >= data._pHFrames) {
		StartStand(data._pdir);
		ClearPlrPVars();
		if (random_(3, 4)) {
			ArmorDur();
		}

		return true;
	}

	return false;
}



bool Player::PM_DoDeath()
{
	if (data._pVar8 >= 2 * data._pDFrames) {
		if (deathdelay > 1 && isMyPlr()) {
			deathdelay--;
			if (deathdelay == 1) {
				deathflag = true;
				if (actors.isSingleplayer()) {
					gamemenu_on();
				}
			}
		}

		data._pAnimDelay = 10000;
		data._pAnimFrame = data._pAnimLen;
		grid.at(pos()).dFlags |= DunTileFlag::DEAD_PLAYER;
	}

	if (data._pVar8 < 100) {
		data._pVar8++;
	}

	return false;
}

bool Player::PM_DoNewLvl()
{
	return false;
}

void Player::CheckNewPath()
{
	int i;
	Dir d;
	V2Di n, vel;
	int xvel3;

	if (data.destAction == DestinationAction::ATTACKMON) {
		MakePlrPath(monsters[data.destParam1].data._mfut, false);
	}

	if (data.destAction == DestinationAction::ATTACKPLR) {
		MakePlrPath(plr[data.destParam1].futpos(), false);
	}

	if (!data.wkpath.empty()) {
		if (data._pmode == PlayerMode::STAND) {
			if (isMyPlr()) {
				if (data.destAction == DestinationAction::ATTACKMON || data.destAction == DestinationAction::ATTACKPLR) {
					i = data.destParam1;

					if (data.destAction == DestinationAction::ATTACKMON) {
						n = (futpos() - monsters[i].data._mfut).abs();
						d = GetDirection(futpos(), monsters[i].data._mfut);
					} else {
						n = (futpos() - plr[i].futpos()).abs();
						d = GetDirection(futpos(), plr[i].futpos());
					}

					if (n.x < 2 && n.y < 2) {
						ClrPlrPath();
						if (monsters[i].data.mtalkmsg && monsters[i].data.mtalkmsg != TEXT_VILE14) {
							monsters[i].TalktoMonster();
						} else {
							StartAttack(d);
						}
						data.destAction = DestinationAction::NONE;
					}
				}
			}

			const V2Di nextpos = data.wkpath.front().pos();
			if (pos() != nextpos) {
				Dir dir = GetDirection(pos(), nextpos);
				data._pdir = dir;
				StartWalk(dir);
				data.wkpath.pop();
			}

			if (data._pmode == PlayerMode::STAND) {
				StartStand(data._pdir);
				data.destAction = DestinationAction::NONE;
			}
		}

		return;
	}
	if (data.destAction == DestinationAction::NONE) {
		return;
	}

	if (data._pmode == PlayerMode::STAND) {
		switch (data.destAction) {
		case DestinationAction::ATTACK:
			d = GetDirection(pos(), { data.destParam1, data.destParam2 });
			StartAttack(d);
			break;
		case DestinationAction::ATTACKMON:
			i = data.destParam1;
			n = (pos() - monsters[i].data._mfut).abs();
			if (n.x <= 1 && n.y <= 1) {
				d = GetDirection(futpos(), monsters[i].data._mfut);
				if (monsters[i].data.mtalkmsg && monsters[i].data.mtalkmsg != TEXT_VILE14) {
					monsters[i].TalktoMonster();
				} else {
					StartAttack(d);
				}
			}
			break;
		case DestinationAction::ATTACKPLR:
			i = data.destParam1;
			n = (pos() - plr[i].futpos()).abs();
			if (n.x <= 1 && n.y <= 1) {
				d = GetDirection(futpos(), plr[i].futpos());
				StartAttack(d);
			}
			break;
		case DestinationAction::RATTACK:
			d = GetDirection(pos(), { data.destParam1, data.destParam2 });
			StartRangeAttack(d, { data.destParam1, data.destParam2 });
			break;
		case DestinationAction::RATTACKMON:
			i = data.destParam1;
			d = GetDirection(futpos(), monsters[i].data._mfut);
			if (monsters[i].data.mtalkmsg && monsters[i].data.mtalkmsg != TEXT_VILE14) {
				monsters[i].TalktoMonster();
			} else {
				StartRangeAttack(d, monsters[i].data._mfut);
			}
			break;
		case DestinationAction::RATTACKPLR:
			i = data.destParam1;
			d = GetDirection(futpos(), plr[i].futpos());
			StartRangeAttack(d, plr[i].futpos());
			break;
		case DestinationAction::SPELL:
			d = GetDirection(pos(), { data.destParam1, data.destParam2 });
			StartSpell(d, { data.destParam1, data.destParam2 });
			data._pVar4 = data.destParam3;
			break;
		case DestinationAction::SPELLWALL:
			StartSpell(Dir(data.destParam3), { data.destParam1, data.destParam2 });
			data._pVar3 = data.destParam3;
			data._pVar4 = data.destParam4;
			break;
		case DestinationAction::SPELLMON:
			i = data.destParam1;
			d = GetDirection(pos(), monsters[i].data._mfut);
			StartSpell(d, monsters[i].data._mfut);
			data._pVar4 = data.destParam2;
			break;
		case DestinationAction::SPELLPLR:
			i = data.destParam1;
			d = GetDirection(pos(), plr[i].futpos());
			StartSpell(d, plr[i].futpos());
			data._pVar4 = data.destParam2;
			break;
		case DestinationAction::OPERATE:
			i = data.destParam1;
			n = (pos() - object[i]._o).abs();
			if (n.y > 1 && grid[object[i]._o.x][object[i]._o.y - 1].getObject() == i) {
				n.y = abs(pos().y - object[i]._o.y + 1);
			}
			if (n.x <= 1 && n.y <= 1) {
				if (object[i]._oBreak == 1) {
					d = GetDirection(pos(), object[i]._o);
					StartAttack(d);
				} else {
					OperateObject(pnum, i, false);
				}
			}
			break;
		case DestinationAction::DISARM:
			i = data.destParam1;
			n = (pos() - object[i]._o).abs();
			if (n.y > 1 && grid[object[i]._o.x][object[i]._o.y - 1].getObject() == i) {
				n.y = abs(pos().y - object[i]._o.y + 1);
			}
			if (n.x <= 1 && n.y <= 1) {
				if (object[i]._oBreak == 1) {
					d = GetDirection(pos(), object[i]._o);
					StartAttack(d);
				} else {
					TryDisarm(pnum, i);
					OperateObject(pnum, i, false);
				}
			}
			break;
		case DestinationAction::OPERATETK:
			i = data.destParam1;
			if (object[i]._oBreak != 1) {
				OperateObject(pnum, i, true);
			}
			break;
		case DestinationAction::PICKUPITEM:
			if (pnum == myplr()) {
				i = data.destParam1;
				n = (pos() - item[i]._i).abs();
				if (n.x <= 1 && n.y <= 1 && pcurs == CURSOR_HAND && !item[i]._iRequest) {
					NetSendCmdGItem(true, CMD_REQUESTGITEM, myplr(), myplr(), i);
					item[i]._iRequest = true;
				}
			}
			break;
		case DestinationAction::PICKUPAITEM:
			if (pnum == myplr()) {
				i = data.destParam1;
				n = (pos() - item[i]._i).abs();
				if (n.x <= 1 && n.y <= 1 && pcurs == CURSOR_HAND) {
					NetSendCmdGItem(true, CMD_REQUESTAGITEM, myplr(), myplr(), i);
				}
			}
			break;
		case DestinationAction::TALK:
			if (pnum == myplr()) {
				TalkToTowner(pnum, data.destParam1);
			}
			break;
		}

		PlantInPlace(data._pdir);
		data.destAction = DestinationAction::NONE;

		return;
	}

	if (data._pmode == PlayerMode::ATTACK && data._pAnimFrame > myplr().data._pAFNum) {
		if (data.destAction == DestinationAction::ATTACK) {
			d = GetDirection(futpos(), { data.destParam1, data.destParam2 });
			StartAttack(d);
			data.destAction = DestinationAction::NONE;
		} else if (data.destAction == DestinationAction::ATTACKMON) {
			i = data.destParam1;
			n = (pos() - monsters[i].data._mfut).abs();
			if (n.x <= 1 && n.y <= 1) {
				d = GetDirection(futpos(), monsters[i].data._mfut);
				StartAttack(d);
			}
			data.destAction = DestinationAction::NONE;
		} else if (data.destAction == DestinationAction::ATTACKPLR) {
			i = data.destParam1;
			n = (pos() - plr[i].futpos()).abs();
			if (n.x <= 1 && n.y <= 1) {
				d = GetDirection(futpos(), plr[i].futpos());
				StartAttack(d);
			}
			data.destAction = DestinationAction::NONE;
		} else if (data.destAction == DestinationAction::OPERATE) {
			i = data.destParam1;
			n = (pos() - object[i]._o).abs();
			if (n.y > 1 && grid[object[i]._o.x][object[i]._o.y - 1].getObject() == i) {
				n.y = abs(pos().y - object[i]._o.y + 1);
			}
			if (n.x <= 1 && n.y <= 1) {
				if (object[i]._oBreak == 1) {
					d = GetDirection(pos(), object[i]._o);
					StartAttack(d);
				} else {
					OperateObject(pnum, i, false);
				}
			}
		}
	}

	if (data._pmode == PlayerMode::RATTACK && data._pAnimFrame > myplr().data._pAFNum) {
		if (data.destAction == DestinationAction::RATTACK) {
			d = GetDirection(pos(), { data.destParam1, data.destParam2 });
			StartRangeAttack(d, { data.destParam1, data.destParam2 });
			data.destAction = DestinationAction::NONE;
		} else if (data.destAction == DestinationAction::RATTACKMON) {
			i = data.destParam1;
			d = GetDirection(pos(), monsters[i].data._mfut);
			StartRangeAttack(d, monsters[i].data._mfut);
			data.destAction = DestinationAction::NONE;
		} else if (data.destAction == DestinationAction::RATTACKPLR) {
			i = data.destParam1;
			d = GetDirection(pos(), plr[i].pos());
			StartRangeAttack(d, plr[i].pos());
			data.destAction = DestinationAction::NONE;
		}
	}

	if (data._pmode == PlayerMode::SPELL && data._pAnimFrame > data._pSFNum) {
		if (data.destAction == DestinationAction::SPELL) {
			d = GetDirection(pos(), { data.destParam1, data.destParam2 });
			StartSpell(d, { data.destParam1, data.destParam2 });
			data.destAction = DestinationAction::NONE;
		} else if (data.destAction == DestinationAction::SPELLMON) {
			i = data.destParam1;
			d = GetDirection(pos(), monsters[i].data._mfut);
			StartSpell(d, monsters[i].data._mfut);
			data.destAction = DestinationAction::NONE;
		} else if (data.destAction == DestinationAction::SPELLPLR) {
			i = data.destParam1;
			d = GetDirection(pos(), plr[i].pos());
			StartSpell(d, plr[i].pos());
			data.destAction = DestinationAction::NONE;
		}
	}
}

bool Player::PlrDeathModeOK()
{
	if (!isMyPlr()) { return true; }
	if (data._pmode == PlayerMode::DEATH) {
		return true;
	} else if (data._pmode == PlayerMode::QUIT) {
		return true;
	} else if (data._pmode == PlayerMode::NEWLVL) {
		return true;
	}
	return false;
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
		if (data.InvList[i]._itype == ItemType::gold) {
			if (data.InvList[i]._ivalue > GOLD_MAX_LIMIT) {
				data.InvList[i]._ivalue = GOLD_MAX_LIMIT;
			}
			gt += data.InvList[i]._ivalue;
		}
	}
	if (gt != data._pGold)
		data._pGold = gt;

	pc = data._pClass;
	if (data._pBaseStr > classes[pc].MaxStats[AttributeId::STR]) {
		data._pBaseStr = classes[pc].MaxStats[AttributeId::STR];
	}
	if (data._pBaseMag > classes[pc].MaxStats[AttributeId::MAG]) {
		data._pBaseMag = classes[pc].MaxStats[AttributeId::MAG];
	}
	if (data._pBaseDex > classes[pc].MaxStats[AttributeId::DEX]) {
		data._pBaseDex = classes[pc].MaxStats[AttributeId::DEX];
	}
	if (data._pBaseVit > classes[pc].MaxStats[AttributeId::VIT]) {
		data._pBaseVit = classes[pc].MaxStats[AttributeId::VIT];
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

void Player::ProcessPlayer()
{
	int pnum;
	bool tplayer;

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
		if (data.plractive && lvl.currlevel == data.plrlevel && (isMyPlr() || !data._pLvlChanging)) {
			CheckCheatStats();

			if (!PlrDeathModeOK(pnum) && (data._pHitPoints >> 6) <= 0) {
				SyncPlrKill();
			}

			if (isMyPlr()) {
				if ((data._pIFlags & ItemSpecialEffect::DRAINLIFE) && lvl.currlevel != 0) {
					data._pHitPoints -= 4;
					data._pHPBase -= 4;
					if ((data._pHitPoints >> 6) <= 0) {
						SyncPlrKill();
					}
					drawhpflag = true;
				}
				if (data._pIFlags & ItemSpecialEffect::NOMANA && data._pManaBase > 0) {
					data._pManaBase -= data._pMana;
					data._pMana = 0;
					drawmanaflag = true;
				}
			}

			tplayer = false;
			do {
				switch (data._pmode) {
				case PlayerMode::STAND:
					tplayer = PM_DoStand();
					break;
				case PlayerMode::WALK:
					tplayer = PM_DoWalk();
					break;
				case PlayerMode::WALK2:
					tplayer = PM_DoWalk2();
					break;
				case PlayerMode::WALK3:
					tplayer = PM_DoWalk3();
					break;
				case PlayerMode::ATTACK:
					tplayer = PM_DoAttack();
					break;
				case PlayerMode::RATTACK:
					tplayer = PM_DoRangeAttack();
					break;
				case PlayerMode::BLOCK:
					tplayer = PM_DoBlock();
					break;
				case PlayerMode::SPELL:
					tplayer = PM_DoSpell();
					break;
				case PlayerMode::GOTHIT:
					tplayer = PM_DoGotHit();
					break;
				case PlayerMode::DEATH:
					tplayer = PM_DoDeath();
					break;
				case PlayerMode::NEWLVL:
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
	data.wkpath = std::queue<PathNode>();
}


bool Player::PosOkPlayer(V2Di pos)
{
	if (!grid.isValid(pos)) return false;
	if (!grid.at(pos).isPiece()) return false;
	if (grid.at(pos).isSolid()) return false;

	if (grid.at(pos).isPlayer()) { // is another living player
		int p = grid.at(pos).getPlayer();
		if (p != pnum && data._pHitPoints) return false;
	}

	if (grid.at(pos).isActor()) { // is another living monster
		if (lvl.currlevel == 0) return false;
		if ((monsters[grid.at(pos).getActor()].data._mhitpoints >> 6) > 0) return false;
	}

	if (grid.at(pos).isObject()) {
		char bv = grid.at(pos).getObject();
		if (object[bv]._oSolidFlag) return false;
	}
	return true;
}

void Player::MakePlrPath(V2Di p, bool endspace)
{
	data._pathtarg = p;
	if (futpos() == p) return; // already moving there
	int dist = pathfinder.FindPath(::dvl::PosOkPlayer, pnum, futpos(), p, data.wkpath);
	if (!data.wkpath.empty()) {
		data._pathtarg = data.wkpath.front().pos();
		data.wkpath.pop(); // pops the front, which should be the starting location
	}
}

void Player::CheckPlrSpell()
{
	bool addflag;
	int rspell, sl;
	Dir sd;

	if (!isMyPlr()) return;

	rspell = data._pRSpell;
	if (rspell == SpellId::INVALID) {
		if (data._pClass == PlayerClass::warrior) {
			PlaySFX(PS_WARR34);
		} else if (data._pClass == PlayerClass::rogue) {
			PlaySFX(PS_ROGUE34);
		} else if (data._pClass == PlayerClass::sorceror) {
			PlaySFX(PS_MAGE34);
		}
		return;
	}

	if (lvl.type() == DunType::town && !spelldata[rspell].sTownSpell) {
		if (data._pClass == PlayerClass::warrior) {
			PlaySFX(PS_WARR27);
		} else if (data._pClass == PlayerClass::rogue) {
			PlaySFX(PS_ROGUE27);
		} else if (data._pClass == PlayerClass::sorceror) {
			PlaySFX(PS_MAGE27);
		}
		return;
	}

	if (!sgbControllerActive) {
		if (pcurs != CURSOR_HAND
		    || (Mouse.y >= PANEL_TOP && Mouse.x >= PANEL_LEFT && Mouse.x <= RIGHT_PANEL)     // inside main panel
		    || ((chrflag || gui.questlog) && Mouse.x < SPANEL_WIDTH && Mouse.y < SPANEL_HEIGHT) // inside left panel
		    || ((invflag || gui.sbookflag) && Mouse.x > RIGHT_PANEL && Mouse.y < SPANEL_HEIGHT) // inside right panel
		        && rspell != SpellId::HEAL
		        && rspell != SpellId::IDENTIFY
		        && rspell != SpellId::REPAIR
		        && rspell != SpellId::INFRA
		        && rspell != SpellId::RECHARGE) {
			return;
		}
	}

	addflag = false;
	switch (data._pRSplType) {
	case RSpellType::SKILL:
	case RSpellType::SPELL:
		addflag = CheckSpell(rspell, data._pRSplType, false);
		break;
	case RSpellType::SCROLL:
		addflag = UseScroll();
		break;
	case RSpellType::CHARGES:
		addflag = UseStaff();
		break;
	}

	if (addflag) {
		if (data._pRSpell == SpellId::FIREWALL) {
			sd = GetDirection(pos(), cursm);
			sl = GetSpellLevel(data._pRSpell);
			NetSendCmdLocParam3(true, CMD_SPELLXYD, cursm, data._pRSpell, int(sd), sl);
		} else if (pcursmonst != -1) {
			sl = GetSpellLevel(data._pRSpell);
			NetSendCmdParam3(true, CMD_SPELLID, pcursmonst, data._pRSpell, sl);
		} else if (pcursplr != -1) {
			sl = GetSpellLevel(data._pRSpell);
			NetSendCmdParam3(true, CMD_SPELLPID, pcursplr, data._pRSpell, sl);
		} else { //145
			sl = GetSpellLevel(data._pRSpell);
			NetSendCmdLocParam2(true, CMD_SPELLXY, cursm, data._pRSpell, sl);
		}
		return;
	}

	if (data._pRSplType == RSpellType::SPELL) {
		if (data._pClass == PlayerClass::warrior) {
			PlaySFX(PS_WARR35);
		} else if (data._pClass == PlayerClass::rogue) {
			PlaySFX(PS_ROGUE35);
		} else if (data._pClass == PlayerClass::sorceror) {
			PlaySFX(PS_MAGE35);
		}
	}
}

int Player::GetSpellLevel(int sn)
{
	int result;
	if (isMyPlr())
		result = data._pISplLvlAdd + data._pSplLvl[sn];
	else
		result = 1;
	if (result < 0) result = 0;
	return result;
}

void Player::SyncPlrAnim()
{
	int sType;
	Dir dir = data._pdir;
	switch (data._pmode) {
	case PlayerMode::STAND:
		data._pAnimData = data._pNAnim[int(dir)];
		break;
	case PlayerMode::WALK:
	case PlayerMode::WALK2:
	case PlayerMode::WALK3:
		data._pAnimData = data._pWAnim[int(dir)];
		break;
	case PlayerMode::ATTACK:
		data._pAnimData = data._pAAnim[int(dir)];
		break;
	case PlayerMode::RATTACK:
		data._pAnimData = data._pAAnim[int(dir)];
		break;
	case PlayerMode::BLOCK:
		data._pAnimData = data._pBAnim[int(dir)];
		break;
	case PlayerMode::SPELL:
		if (isMyPlr())
			sType = spelldata[data._pSpell].sType;
		else
			sType = MagicType::FIRE;
		if (sType == MagicType::FIRE)
			data._pAnimData = data._pFAnim[int(dir)];
		if (sType == MagicType::LIGHTNING)
			data._pAnimData = data._pLAnim[int(dir)];
		if (sType == MagicType::MAGIC)
			data._pAnimData = data._pTAnim[int(dir)];
		break;
	case PlayerMode::GOTHIT:
		data._pAnimData = data._pHAnim[int(dir)];
		break;
	case PlayerMode::NEWLVL:
		data._pAnimData = data._pNAnim[int(dir)];
		break;
	case PlayerMode::DEATH:
		data._pAnimData = data._pDAnim[int(dir)];
		break;
	case PlayerMode::QUIT:
		data._pAnimData = data._pNAnim[int(dir)];
		break;
	default:
		app_fatal("SyncPlrAnim");
		break;
	}
}

void Player::SyncInitPlrPos()
{
	V2Di p;
	int xx, yy, range;
	bool posOk;

	data._pathtarg = pos();

	if (actors.isSingleplayer() || data.plrlevel != lvl.currlevel) {
		return;
	}

	for (int i = 0; i < 8; i++) {
		p = pos() + plroff2[i];
		if (PosOkPlayer(p)) {
			break;
		}
	}

	if (!PosOkPlayer(p)) {
		posOk = false;
		for (range = 1; range < 50 && !posOk; range++) {
			for (yy = -range; yy <= range && !posOk; yy++) {
				p.y = yy + pos().y;
				for (xx = -range; xx <= range && !posOk; xx++) {
					p.x = xx + pos().x;
					if (PosOkPlayer(p) && !PosOkPortal(lvl.currlevel, p)) {
						posOk = true;
					}
				}
			}
		}
	}

	_changePosOffMap(p);
	addToMap();

	if (isMyPlr()) {
		changeFutPos(p);
		data._pathtarg = p;
		View = p;
	}
}

void Player::SyncInitPlr()
{
	SetPlrAnims();
	SyncInitPlrPos();
}

void Player::CheckStats()
{
	PlayerClass c;
	if (data._pClass == PlayerClass::warrior) {
		c = PlayerClass::warrior;
	} else if (data._pClass == PlayerClass::rogue) {
		c = PlayerClass::rogue;
	} else if (data._pClass == PlayerClass::sorceror) {
		c = PlayerClass::sorceror;
	}

	for (int i = 0; i < 4; i++) {
		switch (i) {
		case AttributeId::STR:
			if (data._pBaseStr > classes.get(pc).MaxStats[AttributeId::STR]) {
				data._pBaseStr = classes.get(pc).MaxStats[AttributeId::STR];
			} else if (data._pBaseStr < 0) {
				data._pBaseStr = 0;
			}
			break;
		case AttributeId::MAG:
			if (data._pBaseMag > classes.get(pc).MaxStats[AttributeId::MAG]) {
				data._pBaseMag = classes.get(pc).MaxStats[AttributeId::MAG];
			} else if (data._pBaseMag < 0) {
				data._pBaseMag = 0;
			}
			break;
		case AttributeId::DEX:
			if (data._pBaseDex > classes.get(pc).MaxStats[AttributeId::DEX]) {
				data._pBaseDex = classes.get(pc).MaxStats[AttributeId::DEX];
			} else if (data._pBaseDex < 0) {
				data._pBaseDex = 0;
			}
			break;
		case AttributeId::VIT:
			if (data._pBaseVit > classes.get(pc).MaxStats[AttributeId::VIT]) {
				data._pBaseVit = classes.get(pc).MaxStats[AttributeId::VIT];
			} else if (data._pBaseVit < 0) {
				data._pBaseVit = 0;
			}
			break;
		}
	}
}

void Player::ModifyPlrStr(int l)
{
	int max = classes[int(data._pClass)].MaxStats[AttributeId::STR];
	if (data._pBaseStr + l > max) {
		l = max - data._pBaseStr;
	}

	data._pStrength += l;
	data._pBaseStr += l;

	if (data._pClass == PlayerClass::rogue) {
		data._pDamageMod = data._pLevel * (data._pStrength + data._pDexterity) / 200;
	} else {
		data._pDamageMod = data._pLevel * data._pStrength / 100;
	}

	CalcPlrInv(true);

	if (pnum == myplr()) {
		NetSendCmdParam1(false, CMD_SETSTR, data._pBaseStr); //60
	}
}

void Player::ModifyPlrMag(int l)
{
	int max = classes[int(data._pClass)].MaxStats[AttributeId::MAG];
	if (data._pBaseMag + l > max) {
		l = max - data._pBaseMag;
	}

	data._pMagic += l;
	data._pBaseMag += l;

	int ms = l << 6;
	if (data._pClass == PlayerClass::sorceror) {
		ms <<= 1;
	}

	data._pMaxManaBase += ms;
	data._pMaxMana += ms;
	if (!(data._pIFlags & ItemSpecialEffect::NOMANA)) {
		data._pManaBase += ms;
		data._pMana += ms;
	}

	CalcPlrInv(true);

	if (pnum == myplr()) {
		NetSendCmdParam1(false, CMD_SETMAG, data._pBaseMag);
	}
}

void Player::ModifyPlrDex(int l)
{
	int max = classes[data._pClass].MaxStats[AttributeId::DEX];
	if (data._pBaseDex + l > max) {
		l = max - data._pBaseDex;
	}

	data._pDexterity += l;
	data._pBaseDex += l;
	CalcPlrInv(true);

	if (data._pClass == PlayerClass::rogue) {
		data._pDamageMod = data._pLevel * (data._pDexterity + data._pStrength) / 200;
	}

	if (isMyPlr()) {
		NetSendCmdParam1(false, CMD_SETDEX, data._pBaseDex);
	}
}

void Player::ModifyPlrVit(int l)
{
	int max = classes[int(data._pClass)].MaxStats[AttributeId::VIT];
	if (data._pBaseVit + l > max) {
		l = max - data._pBaseVit;
	}

	data._pVitality += l;
	data._pBaseVit += l;

	int ms = l << 6;
	if (data._pClass == PlayerClass::warrior) { ms <<= 1; }

	data._pHPBase += ms;
	data._pMaxHPBase += ms;
	data._pHitPoints += ms;
	data._pMaxHP += ms;

	CalcPlrInv(true);

	if (isMyPlr()) {
		NetSendCmdParam1(false, CMD_SETVIT, data._pBaseVit);
	}
}

void Player::SetPlayerHitPoints(int val)
{
	data._pHitPoints = val;
	data._pHPBase = val + data._pMaxHPBase - data._pMaxHP;

	if (isMyPlr()) {
		drawhpflag = true;
	}
}

void Player::SetPlrStr(int v)
{
	int dm;
	data._pBaseStr = v;
	CalcPlrInv(true);
	if (data._pClass == PlayerClass::rogue) {
		dm = data._pLevel * (data._pStrength + data._pDexterity) / 200;
	} else {
		dm = data._pLevel * data._pStrength / 100;
	}
	data._pDamageMod = dm;
}

void Player::SetPlrMag(int v)
{
	data._pBaseMag = v;
	int m = v << 6;
	if (data._pClass == PlayerClass::sorceror) {
		m <<= 1;
	}
	data._pMaxManaBase = m;
	data._pMaxMana = m;
	CalcPlrInv(true);
}

void Player::SetPlrDex(int v)
{
	int dm;
	data._pBaseDex = v;
	CalcPlrInv(true);

	if (data._pClass == PlayerClass::rogue) {
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
	if (data._pClass == PlayerClass::warrior) {
		hp <<= 1;
	}

	data._pHPBase = hp;
	data._pMaxHPBase = hp;
	CalcPlrInv(true);
}

void Player::InitDungMsgs()
{
	data.pDungMsgs = 0;
}

void PlayDungMsgs()
{
	if (lvl.currlevel == 1 && !myplr().data._pLvlVisited[1] && actors.isSingleplayer() && !(myplr().data.pDungMsgs & DMSG_CATHEDRAL)) {
		sfxdelay = 40;
		if (myplr().data._pClass == PlayerClass::warrior) {
			sfxdnum = PS_WARR97;
		} else if (myplr().data._pClass == PlayerClass::rogue) {
			sfxdnum = PS_ROGUE97;
		} else if (myplr().data._pClass == PlayerClass::sorceror) {
			sfxdnum = PS_MAGE97;
		}
		myplr().data.pDungMsgs = myplr().data.pDungMsgs | DMSG_CATHEDRAL;
	} else if (lvl.currlevel == 5 && !myplr().data._pLvlVisited[5] && plr.isSingleplayer() && !(myplr().data.pDungMsgs & DMSG_CATACOMBS)) {
		sfxdelay = 40;
		if (myplr().data._pClass == PlayerClass::warrior) {
			sfxdnum = PS_WARR96B;
		} else if (myplr().data._pClass == PlayerClass::rogue) {
			sfxdnum = PS_ROGUE96;
		} else if (myplr().data._pClass == PlayerClass::sorceror) {
			sfxdnum = PS_MAGE96;
		}
		myplr().data.pDungMsgs |= DMSG_CATACOMBS;
	} else if (lvl.currlevel == 9 && !myplr().data._pLvlVisited[9] && plr.isSingleplayer() && !(myplr().data.pDungMsgs & DMSG_CAVES)) {
		sfxdelay = 40;
		if (myplr().data._pClass == PlayerClass::warrior) {
			sfxdnum = PS_WARR98;
		} else if (myplr().data._pClass == PlayerClass::rogue) {
			sfxdnum = PS_ROGUE98;
		} else if (myplr().data._pClass == PlayerClass::sorceror) {
			sfxdnum = PS_MAGE98;
		}
		myplr().data.pDungMsgs |= DMSG_CAVES;
	} else if (lvl.currlevel == 13 && !myplr().data._pLvlVisited[13] && plr.isSingleplayer() && !(myplr().data.pDungMsgs & DMSG_HELL)) {
		sfxdelay = 40;
		if (myplr().data._pClass == PlayerClass::warrior) {
			sfxdnum = PS_WARR99;
		} else if (myplr().data._pClass == PlayerClass::rogue) {
			sfxdnum = PS_ROGUE99;
		} else if (myplr().data._pClass == PlayerClass::sorceror) {
			sfxdnum = PS_MAGE99;
		}
		myplr().data.pDungMsgs |= DMSG_HELL;
	} else if (lvl.currlevel == 16 && !myplr().data._pLvlVisited[15] && plr.isSingleplayer() && !(myplr().data.pDungMsgs & DMSG_DIABLO)) { // BUGFIX: _pLvlVisited should check 16 or this message will never play
		sfxdelay = 40;
		if (myplr().data._pClass == PlayerClass::warrior || myplr().data._pClass == PlayerClass::rogue || myplr().data._pClass == PlayerClass::sorceror) {
			sfxdnum = PS_DIABLVLINT;
		}
		myplr().data.pDungMsgs |= DMSG_DIABLO;
	} else {
		sfxdelay = 0;
	}
}

Item *Player::HasItem(int item) // takes IDI enum, e.g. ItemIndex::LAZSTAFF
{
	for (*i = 0; *i < data._pNumInv; (*i)++) {
		if (data.InvList[*i].IDidx == item)
			return &data.InvList[*i];
	}
	return 0;
}

DEVILUTION_END_NAMESPACE
