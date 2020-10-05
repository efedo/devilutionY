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
	// Set attribute limits (applies when using cheat codes)
	_strength.setActMax(750);
	_dexterity.setActMax(750);
	_magic.setActMax(750);
	_vitality.setActMax(750);
	_hp.setActMax(128000);
	_mana.setActMax(128000);
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
	_pos = newpos;
	_posdraw = newpos;
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
	_posfut = newpos;
}

// Changes future position
void Player::changeFutPos(const V2Di newpos)
{
	if (newpos == _posfut) return;
	assert(PosOkPlayer(newpos));

	// remove from old future position unless same as current position
	if (_posfut != _pos)
		grid.at(_posfut).clearActor(*this);
	// add to new position unless same as current position
	if (newpos != _posfut)
		grid.at(newpos).setActor(*this);
	_posfut = newpos;
}

// Changes draw position
void Player::changeDrawPos(const V2Di newpos)
{
	if (newpos == _posdraw) return;
	grid.at(drawpos()).clearActorDraw(*this);
	_posdraw = newpos;
	grid.at(drawpos()).setActorDraw(*this);
}

// Makes current position = future position
void Player::advancePos()
{
	if (_posfut == _pos) return;
	grid.at(pos()).clearActor(*this);
	if (futpos() != drawpos()) grid.at(drawpos()).clearActorDraw(*this);
	_pos = futpos();
	_posdraw = pos();
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
	return _pos;
}

V2Di Player::futpos() const
{
	return _posfut;
}

V2Di Player::drawpos() const
{
	return _posdraw;
}

void Player::LoadPlrGFX(PlayerGraphicFileFlags gfxflag)
{
	char prefix[16];
	char pszName[256];
	char *szCel;
	uint8_t *pData, *pAnim;
	DWORD i;

	sprintf(prefix, "%c%c%c", CharChar[int(_pClass)], ArmourChar[int(_pgfxnum) >> 4], WepChar[int(_pgfxnum) & 0xF]);
	std::string &cs = classes.get(_pClass).name;

	for (i = 1; i <= PlayerGraphicFileFlag::NONDEATH; i <<= 1) {
		if (!(i & gfxflag)) {
			continue;
		}

		switch (gfxflag) {
		case PlayerGraphicFileFlag::STAND:
			szCel = "AS";
			if (lvl.type() == DunType::town) {
				szCel = "ST";
			}
			pData = _pNData;
			pAnim = (uint8_t *)_pNAnim;
			break;
		case PlayerGraphicFileFlag::WALK:
			szCel = "AW";
			if (lvl.type() == DunType::town) {
				szCel = "WL";
			}
			pData = _pWData;
			pAnim = (uint8_t *)_pWAnim;
			break;
		case PlayerGraphicFileFlag::ATTACK:
			if (lvl.type() == DunType::town) {
				continue;
			}
			szCel = "AT";
			pData = _pAData;
			pAnim = (uint8_t *)_pAAnim;
			break;
		case PlayerGraphicFileFlag::HIT:
			if (lvl.type() == DunType::town) {
				continue;
			}
			szCel = "HT";
			pData = _pHData;
			pAnim = (uint8_t *)_pHAnim;
			break;
		case PlayerGraphicFileFlag::LIGHTNING:
			if (lvl.type() == DunType::town) {
				continue;
			}
			szCel = "LM";
			pData = _pLData;
			pAnim = (uint8_t *)_pLAnim;
			break;
		case PlayerGraphicFileFlag::FIRE:
			if (lvl.type() == DunType::town) {
				continue;
			}
			szCel = "FM";
			pData = _pFData;
			pAnim = (uint8_t *)_pFAnim;
			break;
		case PlayerGraphicFileFlag::MAGIC:
			if (lvl.type() == DunType::town) {
				continue;
			}
			szCel = "QM";
			pData = _pTData;
			pAnim = (uint8_t *)_pTAnim;
			break;
		case PlayerGraphicFileFlag::DEATH:
			if (_pgfxnum & 0xF) {
				continue;
			}
			szCel = "DT";
			pData = _pDData;
			pAnim = (uint8_t *)_pDAnim;
			break;
		case PlayerGraphicFileFlag::BLOCK:
			if (lvl.type() == DunType::town) {
				continue;
			}
			if (!_pBlockFlag) {
				continue;
			}

			szCel = "BL";
			pData = _pBData;
			pAnim = (uint8_t *)_pBAnim;
			break;
		default:
			app_fatal("PLR:2");
			break;
		}

		sprintf(pszName, "PlrGFX\\%s\\%s\\%s%s.CL2", cs.c_str(), prefix, prefix, szCel);
		LoadFileWithMem(pszName, pData);
		actors.SetPlayerGPtrs((uint8_t *)pData, (uint8_t **)pAnim);
		_pGFXLoad |= i;
	}
}

void Player::InitPlayerGFX()
{
	if (_hp >> 6 == 0) {
		_pgfxnum = AnimWeaponId::unarmed;
		LoadPlrGFX(PlayerGraphicFileFlag::DEATH);
	} else {
		LoadPlrGFX(PlayerGraphicFileFlag::NONDEATH);
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
	_pNData = DiabloAllocPtr(plr_sframe_size);

	if (!(plr_gfx_flag & 0x2)) { //WALK
		plr_gfx_flag |= 0x2;
		if (GetPlrGFXSize("WL") > GetPlrGFXSize("AW")) {
			plr_wframe_size = GetPlrGFXSize("WL"); //TOWN
		} else {
			plr_wframe_size = GetPlrGFXSize("AW"); //DUNGEON
		}
	}
	_pWData = DiabloAllocPtr(plr_wframe_size);

	if (!(plr_gfx_flag & 0x4)) { //ATTACK
		plr_gfx_flag |= 0x4;
		plr_aframe_size = GetPlrGFXSize("AT");
	}
	_pAData = DiabloAllocPtr(plr_aframe_size);

	if (!(plr_gfx_flag & 0x8)) { //HIT
		plr_gfx_flag |= 0x8;
		plr_hframe_size = GetPlrGFXSize("HT");
	}
	_pHData = DiabloAllocPtr(plr_hframe_size);

	if (!(plr_gfx_flag & 0x10)) { //LIGHTNING
		plr_gfx_flag |= 0x10;
		plr_lframe_size = GetPlrGFXSize("LM");
	}
	_pLData = DiabloAllocPtr(plr_lframe_size);

	if (!(plr_gfx_flag & 0x20)) { //FIRE
		plr_gfx_flag |= 0x20;
		plr_fframe_size = GetPlrGFXSize("FM");
	}
	_pFData = DiabloAllocPtr(plr_fframe_size);

	if (!(plr_gfx_flag & 0x40)) { //MAGIC
		plr_gfx_flag |= 0x40;
		plr_qframe_size = GetPlrGFXSize("QM");
	}
	_pTData = DiabloAllocPtr(plr_qframe_size);

	if (!(plr_gfx_flag & 0x80)) { //DEATH
		plr_gfx_flag |= 0x80;
		plr_dframe_size = GetPlrGFXSize("DT");
	}
	_pDData = DiabloAllocPtr(plr_dframe_size);

	if (!(plr_gfx_bflag & 0x1)) { //BLOCK
		plr_gfx_bflag |= 0x1;
		plr_bframe_size = GetPlrGFXSize("BL");
	}
	_pBData = DiabloAllocPtr(plr_bframe_size);

	_pGFXLoad = 0;
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
				        classes.get(PlayerClass(c)).name.c_str(), Type, Type, szCel);
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
	MemFreeDbg(_pNData);
	MemFreeDbg(_pWData);
	MemFreeDbg(_pAData);
	MemFreeDbg(_pHData);
	MemFreeDbg(_pLData);
	MemFreeDbg(_pFData);
	MemFreeDbg(_pTData);
	MemFreeDbg(_pDData);
	MemFreeDbg(_pBData);
	_pGFXLoad = 0;
}

void Player::NewPlrAnim(uint8_t *Peq, int numFrames, int Delay, int width)
{
	_pAnimData = Peq;
	_pAnimLen = numFrames;
	_pAnimFrame = 1;
	_pAnimCnt = 0;
	_pAnimDelay = Delay;
	_pAnimWidth = width;
	_pAnimWidth2 = (width - 64) >> 1;
}

void Player::ClearPlrPVars()
{
	_pVar1 = 0;
	_pVar2 = 0;
	_pVar3 = 0;
	_pVar4 = 0;
	_pVar5 = 0;
	_pVar6 = 0;
	_pVar7 = 0;
	_pVar8 = 0;
}

void Player::SetPlrAnims()
{
	_pNWidth = 96;
	_pWWidth = 96;
	_pAWidth = 128;
	_pHWidth = 96;
	_pSWidth = 96;
	_pDWidth = 128;
	_pBWidth = 96;

	PlayerClass pc = _pClass;

	if (lvl.type() == DunType::town) {
		_pNFrames = classes.get(pc).PlrGFXAnimLens[7];
		_pWFrames = classes.get(pc).PlrGFXAnimLens[8];
		_pDFrames = classes.get(pc).PlrGFXAnimLens[4];
		_pSFrames = classes.get(pc).PlrGFXAnimLens[5];
	} else {
		_pNFrames = classes.get(pc).PlrGFXAnimLens[0];
		_pWFrames = classes.get(pc).PlrGFXAnimLens[2];
		_pAFrames = classes.get(pc).PlrGFXAnimLens[1];
		_pHFrames = classes.get(pc).PlrGFXAnimLens[6];
		_pSFrames = classes.get(pc).PlrGFXAnimLens[5];
		_pDFrames = classes.get(pc).PlrGFXAnimLens[4];
		_pBFrames = classes.get(pc).PlrGFXAnimLens[3];
		_pAFNum = classes.get(pc).PlrGFXAnimLens[9];
	}
	_pSFNum = classes.get(pc).PlrGFXAnimLens[10];

	AnimWeaponId gn = AnimWeaponId(_pgfxnum & 0xF);
	if (pc == PlayerClass::warrior) {
		if (gn == AnimWeaponId::bow) {
			if (lvl.type() != DunType::town) {
				_pNFrames = 8;
			}
			_pAWidth = 96;
			_pAFNum = 11;
		} else if (gn == AnimWeaponId::axe) {
			_pAFrames = 20;
			_pAFNum = 10;
		} else if (gn == AnimWeaponId::staff) {
			_pAFrames = 16;
			_pAFNum = 11;
		}
	} else if (pc == PlayerClass::rogue) {
		if (gn == AnimWeaponId::axe) {
			_pAFrames = 22;
			_pAFNum = 13;
		} else if (gn == AnimWeaponId::bow) {
			_pAFrames = 12;
			_pAFNum = 7;
		} else if (gn == AnimWeaponId::staff) {
			_pAFrames = 16;
			_pAFNum = 11;
		}
	} else if (pc == PlayerClass::sorceror) {
		_pSWidth = 128;
		if (gn == AnimWeaponId::unarmed) {
			_pAFrames = 20;
		} else if (gn == AnimWeaponId::unarmed_shield) {
			_pAFNum = 9;
		} else if (gn == AnimWeaponId::bow) {
			_pAFrames = 20;
			_pAFNum = 16;
		} else if (gn == AnimWeaponId::axe) {
			_pAFrames = 24;
			_pAFNum = 16;
		}
	}
}

void Player::ClearPlrRVars()
{
	// TODO: Missing debug assert p != NULL
	bReserved[0] = 0;
	bReserved[1] = 0;
	bReserved[2] = 0;
	wReserved[0] = 0;
	wReserved[1] = 0;
	wReserved[2] = 0;
	wReserved[3] = 0;
	wReserved[4] = 0;
	wReserved[5] = 0;
	wReserved[6] = 0;
	wReserved[7] = 0;
	dwReserved[0] = 0;
	dwReserved[1] = 0;
	dwReserved[2] = 0;
	dwReserved[3] = 0;
	dwReserved[4] = 0;
	dwReserved[5] = 0;
	dwReserved[6] = 0;
}

/**
 * @param c plr_classes value
 */
void Player::CreatePlayer(PlayerClass c)
{
	ClearPlrRVars();
	SetRndSeed(SDL_GetTicks());

	_pClass = c;
	this->SetPlrStr(classes.get(c).strength);
	this->SetPlrDex(classes.get(c).dexterity);
	this->SetPlrMag(classes.get(c).magic);
	this->SetPlrVit(classes.get(c).vitality);

	_pStatPts = 0;

	pTownWarps = 0;
	pDungMsgs = 0;
	pLvlLoad = 0;
	pDiabloKillLevel = 0;

	_pBaseToBlk = classes.get(c).block;

	_pLevel = 1;
	_pMaxLvl = 1;
	_pExperience = 0;
	_pMaxExp = 0;
	_pNextExper = ExpLvlsTbl[1];
	_pArmorClass = 0;
	_pMagResist = 0;
	_pFireResist = 0;
	_pLghtResist = 0;
	_pLightRad = 10;
	_pInfraFlag = false;

	if (c == PlayerClass::warrior) {
		_spellBook.entry(SpellId::REPAIR).ability_level = 1;
	} else if (c == PlayerClass::rogue) {
		_spellBook.entry(SpellId::DISARM).ability_level = 1;
	} else if (c == PlayerClass::sorceror) {
		_spellBook.entry(SpellId::RECHARGE).ability_level = 1;
	}

	_pSpellFlags = 0;

	if (_pClass == PlayerClass::sorceror) {
		_spellBook.entry(SpellId::FIREBOLT).memory_level = 2;
	}

	// interestingly, only the first three hotkeys are reset
	// TODO: BUGFIX: clear all 4 hotkeys instead of 3 (demo leftover)
	for (i = 0; i < 4; i++) {
		_pSplHotKey[i] = -1;
	}

	if (c == PlayerClass::warrior) {
		_pgfxnum = AnimWeaponId::sword_shield;
	} else if (c == PlayerClass::rogue) {
		_pgfxnum = AnimWeaponId::bow;
	} else if (c == PlayerClass::sorceror) {
		_pgfxnum = AnimWeaponId::staff;
	}

	for (int i = 0; i < NUMLEVELS; i++) {
		_pLvlVisited[i] = false;
	}

	for (int i = 0; i < 10; i++) {
		_pSetLvlVisited[i] = false;
	}

	_pLvlChanging = false;
	pTownWarps = 0;
	pLvlLoad = 0;
	pBattleNet = false;
	pManaShield = false;

	InitDungMsgs();
	CreatePlrItems();
	SetRndSeed(0);
}

// Determines how many points can still be assigned
int Player::CalcStatDiff()
{
	return (_strength.statRoom() + _dexterity.statRoom() + _magic.statRoom() + _vitality.statRoom());
}

void Player::NextPlrLevel()
{
	_pLevel++;
	_pMaxLvl++;

	if (CalcStatDiff() < 5) {
		_pStatPts = CalcStatDiff();
	} else {
		_pStatPts += 5;
	}

	_pNextExper = ExpLvlsTbl[_pLevel];

	if (sgbControllerActive) FocusOnCharInfo();
}

void Player::AddPlrExperience(int lvl, int exp)
{
	int powerLvlCap, expCap, newLvl, i;

	if (!isMyPlr()) return;

	if (_hp <= 0) return;

	// Adjust xp based on difference in level between player and monster
	exp *= 1 + ((double)lvl - _pLevel) / 10;
	if (exp < 0) exp = 0;

	// Prevent power leveling
	if (game.isMultiplayer()) {
		powerLvlCap = _pLevel < 0 ? 0 : _pLevel;
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

	_pExperience += exp;
	if ((DWORD)_pExperience > MAXEXP) {
		_pExperience = MAXEXP;
	}

	if (_pExperience >= ExpLvlsTbl[49]) {
		_pLevel = 50;
		return;
	}

	// Increase player level if applicable
	newLvl = 0;
	while (_pExperience >= ExpLvlsTbl[newLvl]) {
		newLvl++;
	}
	if (newLvl != _pLevel) {
		for (i = newLvl - _pLevel; i > 0; i--) {
			NextPlrLevel();
		}
	}

	NetSendCmdParam1(false, CMD_PLRLEVEL, _pLevel);
}


void Player::InitPlayer(bool FirstTime)
{
	DWORD i;

	ClearPlrRVars();

	if (FirstTime) {
		_pRSplType = RSpellType::INVALID;
		_pRSpell = SpellId::INVALID;
		_pSBkSpell = SpellId::INVALID;
		_pSpell = _pRSpell;
		_pSplType = _pRSplType;
		if (AnimWeaponId(_pgfxnum & 0xF) == AnimWeaponId::bow) {
			_pwtype = PlayerWeaponType::ranged;
		} else {
			_pwtype = PlayerWeaponType::melee;
		}
		pManaShield = false;
	}

	if (plrlevel == lvl.currlevel || leveldebug) {

		SetPlrAnims();

		_poff = { 0, 0 };
		_pvel = { 0, 0 };
		ClearPlrPVars();

		if (_hp > 0) {
			_pmode = PlayerMode::STAND;
			NewPlrAnim(_pNAnim[int(Dir::S)], _pNFrames, 3, _pNWidth);
			_pAnimFrame = random_(2, _pNFrames - 1) + 1;
			_pAnimCnt = random_(2, 3);
		} else {
			_pmode = PlayerMode::DEATH;
			NewPlrAnim(_pDAnim[int(Dir::S)], _pDFrames, 1, _pDWidth);
			_pAnimFrame = _pAnimLen - 1;
			_pVar8 = 2 * _pAnimLen;
		}

		_pdir = Dir::S;

		if (pnum == myplr()) {
			if (!FirstTime || lvl.currlevel != 0) {
				changePos(View);
			}
			_pathtarg = pos();
		} else {
			_pathtarg = pos();

			for (i = 0; i < 8 && !PosOkPlayer(plroff2[i] + pos()); i++)
				;
			changePos(pos() + plroff2[i]);
		}

		changeFutPos(pos());
		ClrPlrPath();
		destAction = DestinationAction::NONE;

		if (pnum == myplr()) {
			_plid = AddLight(pos(), _pLightRad);
		} else {
			_plid = -1;
		}
		_pvid = AddVision(pos(), _pLightRad, pnum == myplr());
	}

	if (_pClass == PlayerClass::warrior) {
		_pAbilitySpells = SpellId::REPAIR;
	} else if (_pClass == PlayerClass::rogue) {
		_pAbilitySpells = SpellId::DISARM;
	} else if (_pClass == PlayerClass::sorceror) {
		_pAbilitySpells = SpellId::RECHARGE;
	}

	#ifdef _DEBUG
	if (debug_mode_dollar_sign && FirstTime) {
		_pMemorySpells |= 1 << (SpellId::TELEPORT - 1);
		if (!_pSplLvl[SpellId::TELEPORT]) {
			_pSplLvl[SpellId::TELEPORT] = 1;
		}
	}
	if (debug_mode_key_inverted_v && FirstTime) {
		_pMemorySpells = SpellId::INVALID;
	}
	#endif

	_pNextExper = ExpLvlsTbl[_pLevel];
	_pInvincible = false;

	if (isMyPlr()) {
		deathdelay = 0;
		deathflag = false;
		ScrollInfo._soff = { 0, 0 };
		ScrollInfo._sdir = ScrollDir::NONE;
	}
}

void Player::InitMultiView()
{
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
	_pold = pos();
}

void Player::PlantInPlace(Dir bDir)
{
	changeFutPos(pos());
	_pathtarg = pos();
	_poff = { 0, 0 };
	_pdir = bDir;
	if (isMyPlr()) {
		ScrollInfo._soff = { 0, 0 };
		ScrollInfo._sdir = ScrollDir::NONE;
		View = pos();
	}
}

// Checks if the player should die and if so, starts death action
bool Player::_checkDie() {
	if (_pInvincible && _hp <= 0 && isMyPlr()) {
		SyncPlrKill();
		return true;
	}
	return false;
}

void Player::StartStand(Dir dir)
{
	if (_checkDie()) return;
	if (!(_pGFXLoad & PlayerGraphicFileFlag::STAND)) {
		LoadPlrGFX(PlayerGraphicFileFlag::STAND);
	}

	NewPlrAnim(_pNAnim[int(dir)], _pNFrames, 3, _pNWidth);
	_pmode = PlayerMode::STAND;
	PlantInPlace(dir);
	FixPlrWalkTags();
	SetPlayerOld();
}

void Player::StartWalkStand()
{
	_pmode = PlayerMode::STAND;
	changeFutPos(pos());
	_poff = { 0, 0 };

	if (isMyPlr()) {
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
	if (_plid == -1) return;

	l = &LightList[_plid];
	p.x = 2 * _poff.y + _poff.x;
	p.y = 2 * _poff.y - _poff.x;
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

	ChangeLightOff(_plid, p);
}

void Player::PM_ChangeOffset()
{
	V2Di p;
	_pVar8++;
	p.x = _pVar6 / 256;
	p.y = _pVar7 / 256;

	_pVar6 += _pvel.x;
	_pVar7 += _pvel.y;
	_poff.x = _pVar6 / 256;
	_poff.y = _pVar7 / 256;

	p.x -= _pVar6 >> 8;
	p.y -= _pVar7 >> 8;

	if (isMyPlr() && int(ScrollInfo._sdir)) {
		ScrollInfo._soff += p;
	}

	PM_ChangeLightOff();
}

void Player::StartWalk(V2Di vel, V2Di add, Dir EndDir, ScrollDir sdir)
{
	if (_checkDie()) return;
	SetPlayerOld();
	V2Di p = add + pos();
	if (!PlrDirOK(EndDir)) return;
	changeFutPos(p);

	if (isMyPlr()) ScrollInfo._sd = pos() - View;

	_pmode = PlayerMode::WALK;
	_pvel = vel;
	_poff = { 0, 0 };
	//_pVar1 = add.x;
	//_pVar2 = add.y;
	_pVar3 = int(EndDir);

	if (!(_pGFXLoad & PlayerGraphicFileFlag::WALK)) {
		LoadPlrGFX(PlayerGraphicFileFlag::WALK);
	}

	NewPlrAnim(_pWAnim[int(EndDir)], _pWFrames, 0, _pWWidth);

	_pdir = EndDir;
	_pVar6 = 0;
	_pVar7 = 0;
	_pVar8 = 0;

	if (!isMyPlr()) return;

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
	if (_checkDie()) return;
	SetPlayerOld();
	V2Di p = add + pos();
	if (!PlrDirOK(EndDir)) return;
	changeFutPos(p);
	changeDrawPos(p);
	if (isMyPlr()) ScrollInfo._sd = pos() - View;
	//_pVar1 = pos().x;
	//_pVar2 = pos().y;
	_poff = off;

	ChangeLightXY(_plid, pos());
	PM_ChangeLightOff();

	_pmode = PlayerMode::WALK2;
	_pvel = vel;
	_pVar6 = off.x * 256;
	_pVar7 = off.y * 256;
	_pVar3 = int(EndDir);

	if (!(_pGFXLoad & PlayerGraphicFileFlag::WALK)) LoadPlrGFX(PlayerGraphicFileFlag::WALK);
	NewPlrAnim(_pWAnim[int(EndDir)], _pWFrames, 0, _pWWidth);

	_pdir = EndDir;
	_pVar8 = 0;

	if (!isMyPlr()) return;

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
	if (_checkDie()) return;
	SetPlayerOld();
	V2Di p = add + pos();
	V2Di n = map + pos();

	if (!PlrDirOK(EndDir)) return;

	changeFutPos(p);
	changeDrawPos(n);

	if (isMyPlr()) {
		ScrollInfo._sd = pos() - View;
	}

	//_pVar4 = n.x;
	//_pVar5 = n.y;
	grid.at(n).dFlags |= DunTileFlag::PLAYERLR;
	_poff = off;

	if (lvl.type() != DunType::town) {
		ChangeLightXY(_plid, n);
		PM_ChangeLightOff();
	}

	_pmode = PlayerMode::WALK3;
	_pvel = vel;
	//_pVar1 = p.x;
	//_pVar2 = p.y;
	_pVar6 = off.x * 256;
	_pVar7 = off.y * 256;
	_pVar3 = int(EndDir);

	if (!(_pGFXLoad & PlayerGraphicFileFlag::WALK)) {
		LoadPlrGFX(PlayerGraphicFileFlag::WALK);
	}
	NewPlrAnim(_pWAnim[int(EndDir)], _pWFrames, 0, _pWWidth);

	_pdir = EndDir;
	_pVar8 = 0;

	if (!isMyPlr()) return;

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
	if (_checkDie()) return;

	if (!(_pGFXLoad & PlayerGraphicFileFlag::ATTACK)) {
		LoadPlrGFX(PlayerGraphicFileFlag::ATTACK);
	}

	NewPlrAnim(_pAAnim[int(d)], _pAFrames, 0, _pAWidth);
	_pmode = PlayerMode::ATTACK;
	PlantInPlace(d);
	SetPlayerOld();
}

void Player::StartRangeAttack(Dir d, V2Di c)
{
	if (_checkDie()) return;

	if (!(_pGFXLoad & PlayerGraphicFileFlag::ATTACK)) {
		LoadPlrGFX(PlayerGraphicFileFlag::ATTACK);
	}
	NewPlrAnim(_pAAnim[int(d)], _pAFrames, 0, _pAWidth);

	_pmode = PlayerMode::RATTACK;
	PlantInPlace(d);
	SetPlayerOld();
	_pVar1 = c.x;
	_pVar2 = c.y;
}

void Player::StartPlrBlock(Dir dir)
{
	if (_checkDie()) return;

	PlaySfxLoc(IS_ISWORD, pos());

	if (!(_pGFXLoad & PlayerGraphicFileFlag::BLOCK)) {
		LoadPlrGFX(PlayerGraphicFileFlag::BLOCK);
	}
	NewPlrAnim(_pBAnim[int(dir)], _pBFrames, 2, _pBWidth);

	_pmode = PlayerMode::BLOCK;
	PlantInPlace(dir);
	SetPlayerOld();
}

void Player::StartSpell(Dir d, V2Di c)
{
	if (_checkDie()) return;

	if (lvl.type() != DunType::town) {
		switch (spelldata[_pSpell].sType) {
		case MagicType::FIRE:
			if (!(_pGFXLoad & PlayerGraphicFileFlag::FIRE)) {
				LoadPlrGFX(PlayerGraphicFileFlag::FIRE);
			}
			NewPlrAnim(_pFAnim[int(d)], _pSFrames, 0, _pSWidth);
			break;
		case MagicType::LIGHTNING:
			if (!(_pGFXLoad & PlayerGraphicFileFlag::LIGHTNING)) {
				LoadPlrGFX(PlayerGraphicFileFlag::LIGHTNING);
			}
			NewPlrAnim(_pLAnim[int(d)], _pSFrames, 0, _pSWidth);
			break;
		case MagicType::MAGIC:
			if (!(_pGFXLoad & PlayerGraphicFileFlag::MAGIC)) {
				LoadPlrGFX(PlayerGraphicFileFlag::MAGIC);
			}
			NewPlrAnim(_pTAnim[int(d)], _pSFrames, 0, _pSWidth);
			break;
		}
	}

	PlaySfxLoc(spelldata[_pSpell].sSFX, pos());

	_pmode = PlayerMode::SPELL;

	PlantInPlace(d);
	SetPlayerOld();

	_pVar1 = c.x;
	_pVar2 = c.y;
	_pVar4 = GetSpellLevel(pnum, _pSpell);
	_pVar8 = 1;
}

void Player::FixPlrWalkTags()
{
	V2Di d = _pold;

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

	if (_checkDie()) return;

	if (_pClass == PlayerClass::warrior) {
		PlaySfxLoc(PS_WARR69, pos());
	} else if (_pClass == PlayerClass::rogue) {
		PlaySfxLoc(PS_ROGUE69, pos());
	} else if (_pClass == PlayerClass::sorceror) {
		PlaySfxLoc(PS_MAGE69, pos());
	}

	gui.setRedrawHp();
	if (dam >> 6 >= _pLevel || forcehit) {
		pd = _pdir;

		if (!(_pGFXLoad & PlayerGraphicFileFlag::HIT)) {
			LoadPlrGFX(PlayerGraphicFileFlag::HIT);
		}
		NewPlrAnim(_pHAnim[int(pd)], _pHFrames, 0, _pHWidth);
		_pmode = PlayerMode::GOTHIT;
		PlantInPlace(pd);
		_pVar8 = 1;
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
	int i;
	Dir pdd;
	Item * pi;

	if (_hp <= 0 && _pmode == PlayerMode::DEATH) return; // already dying?

	if (isMyPlr()) {
		bool earflag = false;
		NetSendCmdParam1(true, CMD_PLRDEAD, earflag);
	}

	bool diablolevel = game.isMultiplayer() && plrlevel == 16;

	if (_pClass == PlayerClass::warrior) {
		PlaySfxLoc(PS_DEAD, pos()); // BUGFIX: should use `PS_WARR71` like other classes
	} else if (_pClass == PlayerClass::rogue) {
		PlaySfxLoc(PS_ROGUE71, pos());
	} else if (_pClass == PlayerClass::sorceror) {
		PlaySfxLoc(PS_MAGE71, pos());
	}

	if (_pgfxnum) {
		_pgfxnum = 0;
		_pGFXLoad = 0;
		SetPlrAnims();
	}

	if (!(_pGFXLoad & PlayerGraphicFileFlag::DEATH)) {
		LoadPlrGFX(PlayerGraphicFileFlag::DEATH);
	}

	NewPlrAnim(_pDAnim[int(_pdir)], _pDFrames, 1, _pDWidth);

	_pBlockFlag = false;
	_pmode = PlayerMode::DEATH;
	_pInvincible = true;
	SetPlayerHitPoints(0);
	_pVar8 = 1;

	if (pnum != myplr() && !diablolevel) {
		for (InvSlot &i : inv.getBagArray()) {
			if (!i.item()) continue;
			i.item()->_itype = ItemType::none;
		}
		CalcPlrInv(false);
	}

	if (plrlevel == lvl.currlevel) {
		PlantInPlace(_pdir);
		RemoveFromMap();
		grid.at(pos()).dFlags |= DunTileFlag::DEAD_PLAYER;
		SetPlayerOld();

		if (pnum == myplr()) {
			redrawhpflag = true;
			deathdelay = 30;

			if (pcurs >= CURSOR_FIRSTITEM) {
				assert(HoldItem);
				PlrDeadItem(*HoldItem, { 0, 0 });
				SetCursor_(CURSOR_HAND);
			}

			if (!diablolevel) {
				DropHalfPlayersGold();
				pi = InvBody[0];
				i = MAXINVITEMS;
				while (i--) {
					pdd = rotate(_pdir, i);
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
		HoldItem = &item;
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
					HoldItem = &item;
					NetSendCmdPItem(false, CMD_RESPAWNITEM, p.x, p.y);
					return;
				}
			}
		}
	}
}

void Player::DropHalfPlayersGold()
{
	const int hGold = _pGold >> 1;
	// Go through all piles of gold in inventory and take half
	_pGold = inv.CalculateGold();
}

void Player::SyncPlrKill()
{
	for (int i = 0; i < nummissiles; i++) {
		int ma = missileactive[i];
		if (missile[ma]._mitype == MissileType::MANASHIELD && missile[ma]._misource == id() && missile[ma]._miDelFlag == false) {
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

	if (lvl.currlevel != 0 && pnum == myplr() && (monsters[myplr()]._m.x != 1 || monsters[myplr()]._m.y != 0)) {
		monsters[myplr()].M_StartKill(myplr());
		AddDead(monsters[myplr()]._m, (monsters[myplr()].MType)->mdeadval, monsters[myplr()]._mdir);
		m = monsters[myplr()]._m;
		grid.at(m).clearMonster();
		monsters[myplr()]._mDelFlag = true;
		DeleteMonsterList();
	}

	for (i = 0; i < nummissiles; i++) {
		am = missileactive[i];
		if (missile[am]._mitype == MissileType::STONE && missile[am]._misource == pnum) {
			monsters[missile[am]._miVar2]._mmode = missile[am]._miVar1;
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
		_pLvlVisited[plrlevel] = true;
	}

	ClrPlrPath();
	destAction = DestinationAction::NONE;
	_pLvlChanging = true;

	if (pnum == myplr()) {
		pLvlLoad = 10;
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
		plrlevel = curlvl;
		break;
	case WM_DIABRTNLVL:
	case WM_DIABTOWNWARP:
		plrlevel = curlvl;
		break;
	case WM_DIABSETLVL:
		lvl.setlvlnum = SetLvl(curlvl);
		break;
	case WM_DIABTWARPUP:
		pTownWarps |= 1 << (int(lvl.type()) - 2);
		plrlevel = curlvl;
		break;
	case WM_DIABRETOWN:
		break;
	default:
		app_fatal("StartNewLvl");
		break;
	}

	if (pnum == myplr()) {
		_pmode = PlayerMode::NEWLVL;
		_pInvincible = true;
		PostMessage(fom, 0, 0);
		if (plr.isMultiplayer()) {
			NetSendCmdParam2(true, CMD_NEWLVL, fom, curlvl);
		}
	}
}
void Player::RestartTownLvl()
{
	InitLevelChange();

	plrlevel = 0;
	_pInvincible = false;

	SetPlayerHitPoints(64);

	_pMana = 0;
	_pManaBase = _pMana - (_pMaxMana - _pMaxManaBase);

	CalcPlrInv(false);

	if (pnum == myplr()) {
		_pmode = PlayerMode::NEWLVL;
		_pInvincible = true;
		PostMessage(WM_DIABRETOWN, 0, 0);
	}
}

void Player::StartWarpLvl(int pidx)
{
	InitLevelChange();

	if (game.isMultiplayer()) {
		if (plrlevel != 0) {
			plrlevel = 0;
		} else {
			plrlevel = portal[pidx].level;
		}
	}

	if (isMyPlr()) {
		SetCurrentPortal(pidx);
		_pmode = PlayerMode::NEWLVL;
		_pInvincible = true;
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
		xvel3 = classes[_pClass].PWVel[0];
		vel.x = classes[_pClass].PWVel[1];
		vel.y = classes[_pClass].PWVel[2];
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
	if (_pAnimFrame == 3
	    || (_pWFrames == 8 && _pAnimFrame == 7)
	    || (_pWFrames != 8 && _pAnimFrame == 4)) {
		PlaySfxLoc(PS_WALK1, pos());
	}

	int anim_len = 8;
	if (lvl.currlevel != 0) {
		anim_len = classes[_pClass].AnimLenFromClass;
	}

	if (_pVar8 == anim_len) { // end of anim
		advancePos();

		if (lvl.type() != DunType::town) {
			ChangeLightXY(_plid, pos());
			ChangeVisionXY(_pvid, pos());
		}

		if (isMyPlr() && int(ScrollInfo._sdir)) {
			View = pos() - ScrollInfo._sd;
		}

		if (!wkpath.empty()) {
			StartWalkStand();
		} else {
			StartStand(Dir(_pVar3));
		}

		ClearPlrPVars();

		if (lvl.type() != DunType::town) {
			ChangeLightOff(_plid, { 0, 0 });
		}
		return true;
	} else {
		PM_ChangeOffset();
		return false;
	}
}

bool Player::PM_DoWalk2()
{
	if (_pAnimFrame == 3
	    || (_pWFrames == 8 && _pAnimFrame == 7)
	    || (_pWFrames != 8 && _pAnimFrame == 4)) {
		PlaySfxLoc(PS_WALK1, pos());
	}

	int anim_len = 8;
	if (lvl.currlevel != 0) {
		anim_len = classes[_pClass].AnimLenFromClass;
	}

	if (_pVar8 == anim_len) { // end of anim
		advancePos();

		if (lvl.type() != DunType::town) {
			ChangeLightXY(_plid, pos());
			ChangeVisionXY(_pvid, pos());
		}

		if (isMyPlr() && int(ScrollInfo._sdir)) {
			View = pos() - ScrollInfo._sd;
		}

		if (!wkpath.empty()) {
			StartWalkStand();
		} else {
			StartStand(Dir(_pVar3));
		}

		ClearPlrPVars();
		if (lvl.type() != DunType::town) {
			ChangeLightOff(_plid, { 0, 0 });
		}
		return true;
	} else {
		PM_ChangeOffset();
		return false;
	}
}

bool Player::PM_DoWalk3()
{
	if (_pAnimFrame == 3
	    || (_pWFrames == 8 && _pAnimFrame == 7)
	    || (_pWFrames != 8 && _pAnimFrame == 4)) {
		PlaySfxLoc(PS_WALK1, pos());
	}

	int anim_len = 8;
	if (lvl.currlevel != 0) {
		anim_len = classes[_pClass].AnimLenFromClass;
	}

	if (_pVar8 == anim_len) { // end of anim
		grid[_pVar4][_pVar5].dFlags &= ~DunTileFlag::PLAYERLR; // makes old pos have lr flag
		advancePos();

		if (lvl.type() != DunType::town) {
			ChangeLightXY(_plid, pos());
			ChangeVisionXY(_pvid, pos());
		}

		if (isMyPlr() && int(ScrollInfo._sdir)) {
			View = pos() - ScrollInfo._sd;
		}

		if (!wkpath.empty()) {
			StartWalkStand();
		} else {
			StartStand(Dir(_pVar3));
		}

		ClearPlrPVars();

		if (lvl.type() != DunType::town) {
			ChangeLightOff(_plid, { 0, 0 });
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
	int hit, hper, mind, maxd, ddp, dam, phanditype, tmac;

	if ((monsters[m]._mhitpoints >> 6) <= 0) {
		return false;
	}

	if (monsters[m].MType->mtype == MonsterType::ILLWEAV && monsters[m]._mgoal == MonsterGoal::RETREAT) {
		return false;
	}

	if (monsters[m]._mmode == MonsterMode::CHARGE) {
		return false;
	}

	rv = false;

	hit = random_(4, 100);
	if (monsters[m]._mmode == MonsterMode::STONE) {
		hit = 0;
	}

	tmac = monsters[m].mArmorClass - _pIEnAc;
	hper = (_dexterity >> 1) + _pLevel + 50 - tmac;
	if (_pClass == PlayerClass::warrior) {
		hper += 20;
	}
	hper += _pIBonusToHit;
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
		mind = _pIMinDam;
		maxd = _pIMaxDam;
		dam = random_(5, maxd - mind + 1) + mind;
		dam += dam * _pIBonusDam / 100;
		dam += _pDamageMod + _pIBonusDamMod;
		if (_pClass == PlayerClass::warrior) {
			ddp = _pLevel;
			if (random_(6, 100) < ddp) {
				dam *= 2;
			}
		}

		phanditype = ItemType::none;
		if (InvBody[INVLOC_HAND_LEFT]._itype == ItemType::sword || InvBody[INVLOC_HAND_RIGHT]._itype == ItemType::sword) {
			phanditype = ItemType::sword;
		}
		if (InvBody[INVLOC_HAND_LEFT]._itype == ItemType::mace || InvBody[INVLOC_HAND_RIGHT]._itype == ItemType::mace) {
			phanditype = ItemType::mace;
		}

		switch (monsters[m].MData->mMonstClass) {
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

		if (_pIFlags & ItemSpecialEffect::T3XDAMVDEM && monsters[m].MData->mMonstClass == MonstClassId::demon) {
			dam *= 3;
		}

		if (isMyPlr()) {
			monsters[m]._mhitpoints -= dam;
		}

		int skdam;
		if (_pIFlags & ItemSpecialEffect::RNDSTEALLIFE) {
			skdam = random_(7, dam >> 3);
			this->heal(skdam);
		}
		if (_pIFlags & (ItemSpecialEffect::STEALMANA_3 | ItemSpecialEffect::STEALMANA_5) && !(_pIFlags & ItemSpecialEffect::NOMANA)) {
			if (_pIFlags & ItemSpecialEffect::STEALMANA_3) {
				skdam = 3 * dam / 100;
			}
			if (_pIFlags & ItemSpecialEffect::STEALMANA_5) {
				skdam = 5 * dam / 100;
			}
			this->restoreMana(skdam);
		}
		if (_pIFlags & (ItemSpecialEffect::STEALLIFE_3 | ItemSpecialEffect::STEALLIFE_5)) {
			if (_pIFlags & ItemSpecialEffect::STEALLIFE_3) {
				skdam = 3 * dam / 100;
			}
			if (_pIFlags & ItemSpecialEffect::STEALLIFE_5) {
				skdam = 5 * dam / 100;
			}
			this->heal(skdam);
		}
		if (_pIFlags & ItemSpecialEffect::NOHEALPLR) {
			monsters[m]._mFlags |= MonsterFlag::noheal;
		}
#ifdef _DEBUG
		if (debug_mode_dollar_sign || debug_mode_key_inverted_v) {
			monsters[m]._mhitpoints = 0; /* double check */
		}
#endif
		if ((monsters[m]._mhitpoints >> 6) <= 0) {
			if (monsters[m]._mmode == MonsterMode::STONE) {
				monsters[m].M_StartKill(pnum);
				monsters[m]._mmode = MonsterMode::STONE;
			} else {
				monsters[m].M_StartKill(pnum);
			}
		} else {
			if (monsters[m]._mmode == MonsterMode::STONE) {
				monsters[m].M_StartHit(pnum, dam);
				monsters[m]._mmode = MonsterMode::STONE;
			} else {
				if (_pIFlags & ItemSpecialEffect::KNOCKBACK) {
					monsters[m].M_GetKnockback();
				}
				monsters[m].M_StartHit(pnum, dam);
			}
		}
		rv = true;
	}
	return rv;
}

bool Player::PlrHitPlr(Player & target)
{
	int blk, blkper, mind, maxd, dam, lvl, skdam, tac;
	Dir dir;

	bool rv = false;
	if (target._pInvincible) return rv;
	if (target._pSpellFlags & 1) return rv;

	int hit = random_(4, 100);
	int hper = (_dexterity >> 1) + _pLevel + 50 - (target._pIBonusAC + target._pIAC + target._dexterity / 5);

	if (_pClass == PlayerClass::warrior) {
		hper += 20;
	}
	hper += _pIBonusToHit;
	if (hper < 5) hper = 5;
	if (hper > 95) hper = 95;

	if ((target._pmode == PlayerMode::STAND || target._pmode == PlayerMode::ATTACK) && target._pBlockFlag) {
		blk = random_(5, 100);
	} else {
		blk = 100;
	}

	blkper = target._dexterity + target._pBaseToBlk + (target._pLevel << 1) - (_pLevel << 1);
	if (blkper < 0) blkper = 0;
	if (blkper > 100) blkper = 100;

	if (hit < hper) {
		if (blk < blkper) {
			dir = GetDirection(target.pos(), pos());
			target.StartPlrBlock(dir);
		} else {
			mind = _pIMinDam;
			maxd = random_(5, _pIMaxDam - mind + 1);
			dam = maxd + mind;
			dam += _pDamageMod + _pIBonusDamMod + dam * _pIBonusDam / 100;

			if (_pClass == PlayerClass::warrior) {
				lvl = _pLevel;
				if (random_(6, 100) < lvl) {
					dam *= 2;
				}
			}
			skdam = dam;// << 6;
			if (_pIFlags & ItemSpecialEffect::RNDSTEALLIFE) {
				tac = random_(7, skdam >> 3);
				this->heal(tac);
			}
			if (isMyPlr()) NetSendCmdDamage(true, target.id(), skdam);
			target.StartPlrHit(skdam, false);
		}
		rv = true;
	}
	return rv;
}

bool Player::PlrHitObj(V2Di m)
{
	int oi = grid.at(m).getObject();
	if (object[oi]._oBreak == 1) {
		BreakObject(id(), oi);
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
	frame = _pAnimFrame;
	if (_pIFlags & ItemSpecialEffect::QUICKATTACK && frame == 1) {
		_pAnimFrame++;
	}
	if (_pIFlags & ItemSpecialEffect::FASTATTACK && (frame == 1 || frame == 3)) {
		_pAnimFrame++;
	}
	if (_pIFlags & ItemSpecialEffect::FASTERATTACK && (frame == 1 || frame == 3 || frame == 5)) {
		_pAnimFrame++;
	}
	if (_pIFlags & ItemSpecialEffect::FASTESTATTACK && (frame == 1 || frame == 4)) {
		_pAnimFrame += 2;
	}
	if (_pAnimFrame == _pAFNum - 1) {
		PlaySfxLoc(PS_SWING, pos());
	}

	if (_pAnimFrame == _pAFNum) {
		dir = _pdir;
		d = pos() + offset(dir);
		if (grid.at(d).getActor()) {
			if (grid.at(d).getActor() > 0) {
				m = grid.at(d).getActor() - 1;
			} else {
				m = -(grid.at(d).getActor() + 1);
			}
			if (CanTalkToMonst(m)) {
				_pVar1 = 0;
				return false;
			}
		}

		if (_pIFlags & ItemSpecialEffect::FIREDAM) {
			AddMissile(d, { 1, 0 }, Dir(0), MissileType::WEAPEXP, 0, pnum, 0, 0);
		}
		if (_pIFlags & ItemSpecialEffect::LIGHTDAM) {
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
			StartStand(_pdir);
			ClearPlrPVars();
			return true;
		}
	}

	if (_pAnimFrame == _pAFrames) {
		StartStand(_pdir);
		ClearPlrPVars();
		return true;
	} else {
		return false;
	}
}

bool Player::PM_DoRangeAttack()
{
	int origFrame;
	MissileType mistype;

	origFrame = _pAnimFrame;
	if (_pIFlags & ItemSpecialEffect::QUICKATTACK && origFrame == 1) {
		_pAnimFrame++;
	}
	if (_pIFlags & ItemSpecialEffect::FASTATTACK && (origFrame == 1 || origFrame == 3)) {
		_pAnimFrame++;
	}

	if (_pAnimFrame == _pAFNum) {
		mistype = MissileType::ARROW;
		if (_pIFlags & ItemSpecialEffect::FIRE_ARROWS) {
			mistype = MissileType::FARROW;
		}
		if (_pIFlags & ItemSpecialEffect::LIGHT_ARROWS) {
			mistype = MissileType::LARROW;
		}
		AddMissile(
		    pos(),
		    { _pVar1, _pVar2 },
		    _pdir,
		    mistype,
		    0,
		    pnum,
		    4,
		    0);

		PlaySfxLoc(PS_BFIRE, pos());

		if (WeaponDur(40)) {
			StartStand(_pdir);
			ClearPlrPVars();
			return true;
		}
	}

	if (_pAnimFrame >= _pAFrames) {
		StartStand(_pdir);
		ClearPlrPVars();
		return true;
	} else {
		return false;
	}
}

bool Player::PM_DoBlock()
{
	if (_pIFlags & ItemSpecialEffect::FASTBLOCK && _pAnimFrame != 1) {
		_pAnimFrame = _pBFrames;
	}

	if (_pAnimFrame >= _pBFrames) {
		StartStand(_pdir);
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
	if (_pVar8 == _pSFNum) {
		CastSpell(
		    pnum,
		    _pSpell,
		    pos(),
		    { _pVar1, _pVar2 },
		    0,
		    _pVar4);

		if (!_pSplFrom) {
			if (_pRSplType == RSpellType::SCROLL) {
				if (!(_pScrollSpells
				        & (unsigned __int64)1 << (_pRSpell - 1))) {
					_pRSpell = SpellId::INVALID;
					_pRSplType = RSpellType::INVALID;
					force_redraw = 255;
				}
			}

			if (_pRSplType == RSpellType::CHARGES) {
				if (!(_pISpells
				        & (unsigned __int64)1 << (_pRSpell - 1))) {
					_pRSpell = SpellId::INVALID;
					_pRSplType = RSpellType::INVALID;
					force_redraw = 255;
				}
			}
		}
	}

	_pVar8++;

	if (lvl.type() == DunType::town) {
		if (_pVar8 > _pSFrames) {
			StartWalkStand();
			ClearPlrPVars();
			return true;
		}
	} else if (_pAnimFrame == _pSFrames) {
		StartStand(_pdir);
		ClearPlrPVars();
		return true;
	}

	return false;
}

bool Player::PM_DoGotHit()
{
	int frame;

	frame = _pAnimFrame;
	if (_pIFlags & ItemSpecialEffect::FASTRECOVER && frame == 3) {
		_pAnimFrame++;
	}
	if (_pIFlags & ItemSpecialEffect::FASTERRECOVER && (frame == 3 || frame == 5)) {
		_pAnimFrame++;
	}
	if (_pIFlags & ItemSpecialEffect::FASTESTRECOVER && (frame == 1 || frame == 3 || frame == 5)) {
		_pAnimFrame++;
	}

	if (_pAnimFrame >= _pHFrames) {
		StartStand(_pdir);
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
	if (_pVar8 >= 2 * _pDFrames) {
		if (deathdelay > 1 && isMyPlr()) {
			deathdelay--;
			if (deathdelay == 1) {
				deathflag = true;
				if (actors.isSingleplayer()) {
					gamemenu_on();
				}
			}
		}

		_pAnimDelay = 10000;
		_pAnimFrame = _pAnimLen;
		grid.at(pos()).dFlags |= DunTileFlag::DEAD_PLAYER;
	}

	if (_pVar8 < 100) {
		_pVar8++;
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

	if (destAction == DestinationAction::ATTACKMON) {
		MakePlrPath(monsters[destParam1]._mfut, false);
	}

	if (destAction == DestinationAction::ATTACKPLR) {
		MakePlrPath(plr[destParam1].futpos(), false);
	}

	if (!wkpath.empty()) {
		if (_pmode == PlayerMode::STAND) {
			if (isMyPlr()) {
				if (destAction == DestinationAction::ATTACKMON || destAction == DestinationAction::ATTACKPLR) {
					i = destParam1;

					if (destAction == DestinationAction::ATTACKMON) {
						n = (futpos() - monsters[i]._mfut).abs();
						d = GetDirection(futpos(), monsters[i]._mfut);
					} else {
						n = (futpos() - plr[i].futpos()).abs();
						d = GetDirection(futpos(), plr[i].futpos());
					}

					if (n.x < 2 && n.y < 2) {
						ClrPlrPath();
						if (monsters[i].mtalkmsg && monsters[i].mtalkmsg != TEXT_VILE14) {
							monsters[i].TalktoMonster();
						} else {
							StartAttack(d);
						}
						destAction = DestinationAction::NONE;
					}
				}
			}

			const V2Di nextpos = wkpath.front().pos();
			if (pos() != nextpos) {
				Dir dir = GetDirection(pos(), nextpos);
				_pdir = dir;
				StartWalk(dir);
				wkpath.pop();
			}

			if (_pmode == PlayerMode::STAND) {
				StartStand(_pdir);
				destAction = DestinationAction::NONE;
			}
		}

		return;
	}
	if (destAction == DestinationAction::NONE) {
		return;
	}

	if (_pmode == PlayerMode::STAND) {
		switch (destAction) {
		case DestinationAction::ATTACK:
			d = GetDirection(pos(), { destParam1, destParam2 });
			StartAttack(d);
			break;
		case DestinationAction::ATTACKMON:
			i = destParam1;
			n = (pos() - monsters[i]._mfut).abs();
			if (n.x <= 1 && n.y <= 1) {
				d = GetDirection(futpos(), monsters[i]._mfut);
				if (monsters[i].mtalkmsg && monsters[i].mtalkmsg != TEXT_VILE14) {
					monsters[i].TalktoMonster();
				} else {
					StartAttack(d);
				}
			}
			break;
		case DestinationAction::ATTACKPLR:
			i = destParam1;
			n = (pos() - plr[i].futpos()).abs();
			if (n.x <= 1 && n.y <= 1) {
				d = GetDirection(futpos(), plr[i].futpos());
				StartAttack(d);
			}
			break;
		case DestinationAction::RATTACK:
			d = GetDirection(pos(), { destParam1, destParam2 });
			StartRangeAttack(d, { destParam1, destParam2 });
			break;
		case DestinationAction::RATTACKMON:
			i = destParam1;
			d = GetDirection(futpos(), monsters[i]._mfut);
			if (monsters[i].mtalkmsg && monsters[i].mtalkmsg != TEXT_VILE14) {
				monsters[i].TalktoMonster();
			} else {
				StartRangeAttack(d, monsters[i]._mfut);
			}
			break;
		case DestinationAction::RATTACKPLR:
			i = destParam1;
			d = GetDirection(futpos(), plr[i].futpos());
			StartRangeAttack(d, plr[i].futpos());
			break;
		case DestinationAction::SPELL:
			d = GetDirection(pos(), { destParam1, destParam2 });
			StartSpell(d, { destParam1, destParam2 });
			_pVar4 = destParam3;
			break;
		case DestinationAction::SPELLWALL:
			StartSpell(Dir(destParam3), { destParam1, destParam2 });
			_pVar3 = destParam3;
			_pVar4 = destParam4;
			break;
		case DestinationAction::SPELLMON:
			i = destParam1;
			d = GetDirection(pos(), monsters[i]._mfut);
			StartSpell(d, monsters[i]._mfut);
			_pVar4 = destParam2;
			break;
		case DestinationAction::SPELLPLR:
			i = destParam1;
			d = GetDirection(pos(), plr[i].futpos());
			StartSpell(d, plr[i].futpos());
			_pVar4 = destParam2;
			break;
		case DestinationAction::OPERATE:
			i = destParam1;
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
			i = destParam1;
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
			i = destParam1;
			if (object[i]._oBreak != 1) {
				OperateObject(pnum, i, true);
			}
			break;
		case DestinationAction::PICKUPITEM:
			if (pnum == myplr()) {
				i = destParam1;
				n = (pos() - item[i]._i).abs();
				if (n.x <= 1 && n.y <= 1 && pcurs == CURSOR_HAND && !item[i]._iRequest) {
					NetSendCmdGItem(true, CMD_REQUESTGITEM, myplr(), myplr(), i);
					item[i]._iRequest = true;
				}
			}
			break;
		case DestinationAction::PICKUPAITEM:
			if (pnum == myplr()) {
				i = destParam1;
				n = (pos() - item[i]._i).abs();
				if (n.x <= 1 && n.y <= 1 && pcurs == CURSOR_HAND) {
					NetSendCmdGItem(true, CMD_REQUESTAGITEM, myplr(), myplr(), i);
				}
			}
			break;
		case DestinationAction::TALK:
			if (pnum == myplr()) {
				TalkToTowner(pnum, destParam1);
			}
			break;
		}

		PlantInPlace(_pdir);
		destAction = DestinationAction::NONE;

		return;
	}

	if (_pmode == PlayerMode::ATTACK && _pAnimFrame > myplr()._pAFNum) {
		if (destAction == DestinationAction::ATTACK) {
			d = GetDirection(futpos(), { destParam1, destParam2 });
			StartAttack(d);
			destAction = DestinationAction::NONE;
		} else if (destAction == DestinationAction::ATTACKMON) {
			i = destParam1;
			n = (pos() - monsters[i]._mfut).abs();
			if (n.x <= 1 && n.y <= 1) {
				d = GetDirection(futpos(), monsters[i]._mfut);
				StartAttack(d);
			}
			destAction = DestinationAction::NONE;
		} else if (destAction == DestinationAction::ATTACKPLR) {
			i = destParam1;
			n = (pos() - plr[i].futpos()).abs();
			if (n.x <= 1 && n.y <= 1) {
				d = GetDirection(futpos(), plr[i].futpos());
				StartAttack(d);
			}
			destAction = DestinationAction::NONE;
		} else if (destAction == DestinationAction::OPERATE) {
			i = destParam1;
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

	if (_pmode == PlayerMode::RATTACK && _pAnimFrame > myplr()._pAFNum) {
		if (destAction == DestinationAction::RATTACK) {
			d = GetDirection(pos(), { destParam1, destParam2 });
			StartRangeAttack(d, { destParam1, destParam2 });
			destAction = DestinationAction::NONE;
		} else if (destAction == DestinationAction::RATTACKMON) {
			i = destParam1;
			d = GetDirection(pos(), monsters[i]._mfut);
			StartRangeAttack(d, monsters[i]._mfut);
			destAction = DestinationAction::NONE;
		} else if (destAction == DestinationAction::RATTACKPLR) {
			i = destParam1;
			d = GetDirection(pos(), plr[i].pos());
			StartRangeAttack(d, plr[i].pos());
			destAction = DestinationAction::NONE;
		}
	}

	if (_pmode == PlayerMode::SPELL && _pAnimFrame > _pSFNum) {
		if (destAction == DestinationAction::SPELL) {
			d = GetDirection(pos(), { destParam1, destParam2 });
			StartSpell(d, { destParam1, destParam2 });
			destAction = DestinationAction::NONE;
		} else if (destAction == DestinationAction::SPELLMON) {
			i = destParam1;
			d = GetDirection(pos(), monsters[i]._mfut);
			StartSpell(d, monsters[i]._mfut);
			destAction = DestinationAction::NONE;
		} else if (destAction == DestinationAction::SPELLPLR) {
			i = destParam1;
			d = GetDirection(pos(), plr[i].pos());
			StartSpell(d, plr[i].pos());
			destAction = DestinationAction::NONE;
		}
	}
}

bool Player::PlrDeathModeOK()
{
	if (!isMyPlr()) { return true; }
	if (_pmode == PlayerMode::DEATH) {
		return true;
	} else if (_pmode == PlayerMode::QUIT) {
		return true;
	} else if (_pmode == PlayerMode::NEWLVL) {
		return true;
	}
	return false;
}

void Player::ValidatePlayer()
{
	int gt, pc, i, b;
	__int64 msk = 0;

	if (_pLevel > MAXCHARLEVEL - 1)
		_pLevel = MAXCHARLEVEL - 1;
	if (_pExperience > _pNextExper)
		_pExperience = _pNextExper;

	gt = 0;
	for (i = 0; i < _pNumInv; i++) {
		if (InvList[i]._itype == ItemType::gold) {
			if (InvList[i]._ivalue > GOLD_MAX_LIMIT) {
				InvList[i]._ivalue = GOLD_MAX_LIMIT;
			}
			gt += InvList[i]._ivalue;
		}
	}
	if (gt != _pGold)
		_pGold = gt;

	pc = _pClass;
	if (_pBaseStr > classes[pc].MaxStats[AttributeId::STR]) {
		_pBaseStr = classes[pc].MaxStats[AttributeId::STR];
	}
	if (_pBaseMag > classes[pc].MaxStats[AttributeId::MAG]) {
		_pBaseMag = classes[pc].MaxStats[AttributeId::MAG];
	}
	if (_pBaseDex > classes[pc].MaxStats[AttributeId::DEX]) {
		_pBaseDex = classes[pc].MaxStats[AttributeId::DEX];
	}
	if (_pBaseVit > classes[pc].MaxStats[AttributeId::VIT]) {
		_pBaseVit = classes[pc].MaxStats[AttributeId::VIT];
	}

	for (b = 1; b < MAX_SPELLS; b++) {
		if (spelldata[b].sBookLvl != -1) {
			msk |= (__int64)1 << (b - 1);
			if (_pSplLvl[b] > 15)
				_pSplLvl[b] = 15;
		}
	}

	_pMemorySpells &= msk;
}

void Player::ProcessPlayer()
{
	int pnum;
	bool tplayer;

	if (pLvlLoad > 0) {
		pLvlLoad--;
	}

	if (sfxdelay > 0) {
		sfxdelay--;
		if (sfxdelay == 0) {
			PlaySFX(sfxdnum);
		}
	}

	ValidatePlayer();

	for (pnum = 0; pnum < MAX_PLRS; pnum++) {
		if (plractive && lvl.currlevel == plrlevel && (isMyPlr() || !_pLvlChanging)) {
			CheckCheatStats();

			if (!PlrDeathModeOK(pnum) && (_pHitPoints >> 6) <= 0) {
				SyncPlrKill();
			}

			if (isMyPlr()) {
				if ((_pIFlags & ItemSpecialEffect::DRAINLIFE) && lvl.currlevel != 0) {
					this->damage(4);
					this->_checkDie();
				}
				if (_pIFlags & ItemSpecialEffect::NOMANA && _mana > 0) {
					this->useMana(_mana - 0);
				}
			}

			tplayer = false;
			do {
				switch (_pmode) {
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

			_pAnimCnt++;
			if (_pAnimCnt > _pAnimDelay) {
				_pAnimCnt = 0;
				_pAnimFrame++;
				if (_pAnimFrame > _pAnimLen) {
					_pAnimFrame = 1;
				}
			}
		}
	}
}

void Player::CheckCheatStats()
{
	// No longer necessary, attributes check themselves
	return;
}

void Player::ClrPlrPath()
{
	wkpath = std::queue<PathNode>();
}

bool Player::PosOkPlayer(V2Di pos)
{
	if (!grid.isValid(pos)) return false;
	if (!grid.at(pos).isPiece()) return false;
	if (grid.at(pos).isSolid()) return false;

	if (grid.at(pos).isPlayer()) { // is another living player
		if (&grid.at(pos).getPlayer() != this) return false;
		if (_hp) return false;
	}

	if (grid.at(pos).isActor()) { // is another living creature
		if (lvl.currlevel == 0) return false;
		if ((grid.at(pos).isMonster() && grid.at(pos).getMonster()._mhitpoints >> 6) > 0) return false;
	}

	if (grid.at(pos).isObject()) {
		char bv = grid.at(pos).getObject();
		if (object[bv]._oSolidFlag) return false;
	}
	return true;
}

void Player::MakePlrPath(V2Di p, bool endspace)
{
	_pathtarg = p;
	if (futpos() == p) return; // already moving there
	int dist = pathfinder.FindPath(::dvl::PosOkPlayer, pnum, futpos(), p, wkpath);
	if (!wkpath.empty()) {
		_pathtarg = wkpath.front().pos();
		wkpath.pop(); // pops the front, which should be the starting location
	}
}

void Player::CheckPlrSpell()
{
	bool addflag;
	int sl;
	Dir sd;

	if (!isMyPlr()) return;

	SpellId rspell = _pRSpell;
	if (rspell == SpellId::INVALID) {
		if (_pClass == PlayerClass::warrior) {
			PlaySFX(PS_WARR34);
		} else if (_pClass == PlayerClass::rogue) {
			PlaySFX(PS_ROGUE34);
		} else if (_pClass == PlayerClass::sorceror) {
			PlaySFX(PS_MAGE34);
		}
		return;
	}

	if (lvl.type() == DunType::town && !spelldata[rspell].sTownSpell) {
		if (_pClass == PlayerClass::warrior) {
			PlaySFX(PS_WARR27);
		} else if (_pClass == PlayerClass::rogue) {
			PlaySFX(PS_ROGUE27);
		} else if (_pClass == PlayerClass::sorceror) {
			PlaySFX(PS_MAGE27);
		}
		return;
	}

	if (!sgbControllerActive) {
		if (pcurs != CURSOR_HAND
		    || (Mouse.y >= PANEL_TOP && Mouse.x >= PANEL_LEFT && Mouse.x <= RIGHT_PANEL)     // inside main panel
		    || ((gui.panels.characterUp() || gui.panels.questLogUp()) && Mouse.x < SPANEL_WIDTH && Mouse.y < SPANEL_HEIGHT) // inside left panel
		    || ((gui.panels.inventoryUp() || gui.panels.spellBookUp()) && Mouse.x > RIGHT_PANEL && Mouse.y < SPANEL_HEIGHT) // inside right panel
		        && rspell != SpellId::HEAL
		        && rspell != SpellId::IDENTIFY
		        && rspell != SpellId::REPAIR
		        && rspell != SpellId::INFRA
		        && rspell != SpellId::RECHARGE) {
			return;
		}
	}

	addflag = false;
	switch (_pRSplType) {
	case RSpellType::SKILL:
	case RSpellType::SPELL:
		addflag = CheckSpell(rspell, _pRSplType, false);
		break;
	case RSpellType::SCROLL:
		addflag = UseScroll();
		break;
	case RSpellType::CHARGES:
		addflag = UseStaff();
		break;
	}

	if (addflag) {
		if (_pRSpell == SpellId::FIREWALL) {
			sd = GetDirection(pos(), cursm);
			sl = GetSpellLevel(_pRSpell);
			NetSendCmdLocParam3(true, CMD_SPELLXYD, cursm, WORD(_pRSpell), int(sd), sl);
		} else if (pcursmonst != -1) {
			sl = GetSpellLevel(_pRSpell);
			NetSendCmdParam3(true, CMD_SPELLID, pcursmonst, WORD(_pRSpell), sl);
		} else if (pcursplr != -1) {
			sl = GetSpellLevel(_pRSpell);
			NetSendCmdParam3(true, CMD_SPELLPID, pcursplr, WORD(_pRSpell), sl);
		} else { //145
			sl = GetSpellLevel(_pRSpell);
			NetSendCmdLocParam2(true, CMD_SPELLXY, cursm, WORD(_pRSpell), sl);
		}
		return;
	}

	if (_pRSplType == RSpellType::SPELL) {
		if (_pClass == PlayerClass::warrior) {
			PlaySFX(PS_WARR35);
		} else if (_pClass == PlayerClass::rogue) {
			PlaySFX(PS_ROGUE35);
		} else if (_pClass == PlayerClass::sorceror) {
			PlaySFX(PS_MAGE35);
		}
	}
}

int Player::GetSpellLevel(SpellId sn)
{
	int result;
	if (isMyPlr())
		result = _pISplLvlAdd + _pSplLvl[sn];
	else
		result = 1;
	if (result < 0) result = 0;
	return result;
}

void Player::SyncPlrAnim()
{
	MagicType sType;
	Dir dir = _pdir;
	switch (_pmode) {
	case PlayerMode::STAND:
		_pAnimData = _pNAnim[int(dir)];
		break;
	case PlayerMode::WALK:
	case PlayerMode::WALK2:
	case PlayerMode::WALK3:
		_pAnimData = _pWAnim[int(dir)];
		break;
	case PlayerMode::ATTACK:
		_pAnimData = _pAAnim[int(dir)];
		break;
	case PlayerMode::RATTACK:
		_pAnimData = _pAAnim[int(dir)];
		break;
	case PlayerMode::BLOCK:
		_pAnimData = _pBAnim[int(dir)];
		break;
	case PlayerMode::SPELL:
		if (isMyPlr())
			sType = spelldata[_pSpell].sType;
		else
			sType = MagicType::FIRE;
		if (sType == MagicType::FIRE)
			_pAnimData = _pFAnim[int(dir)];
		if (sType == MagicType::LIGHTNING)
			_pAnimData = _pLAnim[int(dir)];
		if (sType == MagicType::MAGIC)
			_pAnimData = _pTAnim[int(dir)];
		break;
	case PlayerMode::GOTHIT:
		_pAnimData = _pHAnim[int(dir)];
		break;
	case PlayerMode::NEWLVL:
		_pAnimData = _pNAnim[int(dir)];
		break;
	case PlayerMode::DEATH:
		_pAnimData = _pDAnim[int(dir)];
		break;
	case PlayerMode::QUIT:
		_pAnimData = _pNAnim[int(dir)];
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

	_pathtarg = pos();

	if (game.isSingleplayer() || plrlevel != lvl.currlevel) {
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
		_pathtarg = p;
		View = p;
	}
}

void Player::SyncInitPlr()
{
	SetPlrAnims();
	SyncInitPlrPos();
}

void Player::_updateBaseHp() {
	int hp = _vitality;
	if (_pClass == PlayerClass::warrior) {
		hp <<= 1;
	}
	_hp.setBaseMax(hp);
	if (isMyPlr()) gui.setRedrawHp();
}

void Player::_updateBaseMana() {
	int m = _magic;
	if (_pClass == PlayerClass::sorceror) {
		m <<= 1;
	}
	_mana.setBaseMax(m);
	if (isMyPlr()) gui.setRedrawMana();
}

void Player::_updateBaseDmgMod() {
	if (_pClass == PlayerClass::rogue) {
		_pDamageMod = _pLevel * (_strength + _dexterity) / 200;
	}
	else {
		_pDamageMod = _pLevel * _strength / 100;
	}
}

void Player::damage(int pts) {
	_hp.changeBase(-pts);
	if (isMyPlr()) gui.setRedrawHp();
}

void Player::heal(int pts) {
	damage(-pts);
}

void Player::useMana(int pts) {
	_mana.changeBase(-pts);
	if (isMyPlr()) gui.setRedrawMana();
}
void Player::restoreMana(int pts) {
	useMana(-pts);
}

void Player::CheckStats()
{
	_strength.check();
	_magic.check();
	_dexterity.check();
	_vitality.check();
}

void Player::SetPlrStr(int v)
{
	_strength.setBase(v);
	_updateBaseDmgMod();
	CalcPlrInv(true);
	if (isMyPlr()) NetSendCmdParam1(false, CMD_SETSTR, _strength.getBase()); //60
}

void Player::ModifyPlrStr(int l)
{
	SetPlrStr(_strength.getBase() + l);
}

void Player::SetPlrMag(int v)
{
	_magic.setBase(v);
	_updateBaseMana();
	CalcPlrInv(true);
	if (isMyPlr()) NetSendCmdParam1(false, CMD_SETMAG, _magic.getBase());
}

void Player::ModifyPlrMag(int l)
{
	SetPlrMag(_magic.getBase() + l);
}

void Player::SetPlrDex(int v)
{
	_dexterity.setBase(v);
	_updateBaseDmgMod();
	CalcPlrInv(true);
	if (isMyPlr()) NetSendCmdParam1(false, CMD_SETDEX, _dexterity.getBase());
}

void Player::ModifyPlrDex(int l)
{
	SetPlrDex(_dexterity.getBase() + l);
}

void Player::SetPlrVit(int v)
{
	_vitality.setBase(v);
	_updateBaseHp();
	CalcPlrInv(true);
	if (isMyPlr()) NetSendCmdParam1(false, CMD_SETVIT, _vitality.getBase());
}

void Player::ModifyPlrVit(int l)
{
	SetPlrVit(_vitality.getBase() + l);
}

void Player::InitDungMsgs()
{
	pDungMsgs = 0;
}

void PlayDungMsgs()
{
	if (lvl.currlevel == 1 && !myplr()._pLvlVisited[1] && game.isSingleplayer() && !(myplr().pDungMsgs & DMSG_CATHEDRAL)) {
		sfxdelay = 40;
		if (myplr()._pClass == PlayerClass::warrior) {
			sfxdnum = PS_WARR97;
		} else if (myplr()._pClass == PlayerClass::rogue) {
			sfxdnum = PS_ROGUE97;
		} else if (myplr()._pClass == PlayerClass::sorceror) {
			sfxdnum = PS_MAGE97;
		}
		myplr().pDungMsgs = myplr().pDungMsgs | DMSG_CATHEDRAL;
	} else if (lvl.currlevel == 5 && !myplr()._pLvlVisited[5] && game.isSingleplayer() && !(myplr().pDungMsgs & DMSG_CATACOMBS)) {
		sfxdelay = 40;
		if (myplr()._pClass == PlayerClass::warrior) {
			sfxdnum = PS_WARR96B;
		} else if (myplr()._pClass == PlayerClass::rogue) {
			sfxdnum = PS_ROGUE96;
		} else if (myplr()._pClass == PlayerClass::sorceror) {
			sfxdnum = PS_MAGE96;
		}
		myplr().pDungMsgs |= DMSG_CATACOMBS;
	} else if (lvl.currlevel == 9 && !myplr()._pLvlVisited[9] && plr.isSingleplayer() && !(myplr().pDungMsgs & DMSG_CAVES)) {
		sfxdelay = 40;
		if (myplr()._pClass == PlayerClass::warrior) {
			sfxdnum = PS_WARR98;
		} else if (myplr()._pClass == PlayerClass::rogue) {
			sfxdnum = PS_ROGUE98;
		} else if (myplr()._pClass == PlayerClass::sorceror) {
			sfxdnum = PS_MAGE98;
		}
		myplr().pDungMsgs |= DMSG_CAVES;
	} else if (lvl.currlevel == 13 && !myplr()._pLvlVisited[13] && plr.isSingleplayer() && !(myplr().pDungMsgs & DMSG_HELL)) {
		sfxdelay = 40;
		if (myplr()._pClass == PlayerClass::warrior) {
			sfxdnum = PS_WARR99;
		} else if (myplr()._pClass == PlayerClass::rogue) {
			sfxdnum = PS_ROGUE99;
		} else if (myplr()._pClass == PlayerClass::sorceror) {
			sfxdnum = PS_MAGE99;
		}
		myplr().pDungMsgs |= DMSG_HELL;
	} else if (lvl.currlevel == 16 && !myplr()._pLvlVisited[15] && plr.isSingleplayer() && !(myplr().pDungMsgs & DMSG_DIABLO)) { // BUGFIX: _pLvlVisited should check 16 or this message will never play
		sfxdelay = 40;
		if (myplr()._pClass == PlayerClass::warrior || myplr()._pClass == PlayerClass::rogue || myplr()._pClass == PlayerClass::sorceror) {
			sfxdnum = PS_DIABLVLINT;
		}
		myplr().pDungMsgs |= DMSG_DIABLO;
	} else {
		sfxdelay = 0;
	}
}

Item *Player::HasItem(int item) // takes IDI enum, e.g. ItemIndex::LAZSTAFF
{
	for (*i = 0; *i < _pNumInv; (*i)++) {
		if (InvList[*i].IDidx == item)
			return &InvList[*i];
	}
	return 0;
}

DEVILUTION_END_NAMESPACE
