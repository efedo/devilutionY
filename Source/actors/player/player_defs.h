/**
 * @file xyz.abc
 *
 * Description.
 */
#ifndef __PLAYER_DEFS__
#define __PLAYER_DEFS__

#include "vectordvl.h"
#include "player_enums.h"
#include "player_attribute.h"
#include "skills/skill_enums.h"
#include "skills/spellbook.h"
#include <queue>

namespace dvl {

//////////////////////////////////////////////////
// player
//////////////////////////////////////////////////

typedef struct PlayerStruct {
	PlayerMode _pmode;
	// char walkpath = -1;
	std::queue<PathNode> wkpath;
	bool plractive;
	DestinationAction destAction;
	int destParam1;
	int destParam2;
	int destParam3;
	int destParam4;
	int plrlevel;  // Holds either standard or 'set' level
	V2Di _pos;
	V2Di _posfut;
	V2Di _posdraw;
	V2Di _pathtarg;
	V2Di _powner;
	V2Di _pold;
	V2Di _poff;
	V2Di _pvel;
	Dir _pdir;
	// int _nextdir;
	AnimWeaponId _pgfxnum;
	unsigned char *_pAnimData;
	int _pAnimDelay;
	int _pAnimCnt;
	int _pAnimLen;
	int _pAnimFrame;
	int _pAnimWidth;
	int _pAnimWidth2;
	int _peflag;
	int _plid;
	int _pvid;
	SpellBook _spellBook;
	SpellId _pSpell;
	RSpellType _pSplType;
	char _pSplFrom;
	SpellId _pTSpell;
	RSpellType _pTSplType;
	SpellId _pRSpell;
	RSpellType _pRSplType;
	SpellId _pSBkSpell;
	RSpellType _pSBkSplType;
//	char _pSplLvl[64];

	//uint64_t _pMemorySpells;
	//SpellId _pAbilitySpells; // very odd shifting goes on during assigment
	//uint64_t _pScrollSpells;

	uint8_t _pSpellFlags;
	int _pSplHotKey[4];
	char _pSplTHotKey[4];
	PlayerWeaponType _pwtype;
	bool _pBlockFlag;
	bool _pInvincible; // this appears to be inversed (i.e. 0 = invincible, 1 = mortal)
	char _pLightRad;
	bool _pLvlChanging;
	char _pName[PLR_NAME_LEN];
	PlayerClass _pClass;

	PAttribute _strength;
	PAttribute _magic;
	PAttribute _dexterity;
	PAttribute _vitality;
	PAttribute _hp;
	PAttribute _mana;

	int _pStatPts;
	int _pDamageMod;
	int _pBaseToBlk;

	char _pLevel;
	char _pMaxLvl;
	int _pExperience;
	int _pMaxExp;
	int _pNextExper;

	char _pArmorClass;
	char _pMagResist;
	char _pFireResist;
	char _pLghtResist;

	int _pGold;
	bool _pInfraFlag;
	int _pVar1;
	int _pVar2;
	int _pVar3;
	int _pVar4;
	int _pVar5;
	int _pVar6;
	int _pVar7;
	int _pVar8;
	bool _pLvlVisited[NUMLEVELS]; // Dungeon levels visited
	bool _pSetLvlVisited[NUMLEVELS];  // Set levels visited
	int _pGFXLoad;

	unsigned char *_pNAnim[8];
	unsigned char* _pNData;
	int _pNFrames;
	int _pNWidth;

	unsigned char *_pWAnim[8];
	unsigned char* _pWData;
	int _pWFrames;
	int _pWWidth;

	unsigned char *_pAAnim[8];
	unsigned char* _pAData;
	int _pAFrames;
	int _pAWidth;
	int _pAFNum;

	unsigned char *_pLAnim[8];
	unsigned char* _pLData;

	unsigned char *_pFAnim[8];
	unsigned char* _pFData;

	unsigned char *_pTAnim[8];
	unsigned char* _pTData;

	int _pSFrames;
	int _pSWidth;
	int _pSFNum;

	unsigned char *_pHAnim[8];
	unsigned char* _pHData;
	int _pHFrames;
	int _pHWidth;

	unsigned char *_pDAnim[8];
	unsigned char* _pDData;
	int _pDFrames;
	int _pDWidth;

	unsigned char *_pBAnim[8];
	unsigned char* _pBData;
	int _pBFrames;
	int _pBWidth;

	int _pIMinDam;
	int _pIMaxDam;
	int _pIAC;
	int _pIBonusDam;
	int _pIBonusToHit;
	int _pIBonusAC;
	int _pIBonusDamMod;
	uint64_t _pISpells;
	int _pIFlags;
	int _pIGetHit;
	char _pISplLvlAdd; // bonus to all spell levels?
	char _pISplCost;
	int _pISplDur;
	int _pIEnAc;
	int _pIFMinDam;
	int _pIFMaxDam;
	int _pILMinDam;
	int _pILMaxDam;
	int _pOilType;
	unsigned char pTownWarps;
	unsigned char pDungMsgs;
	unsigned char pLvlLoad;
	unsigned char pBattleNet;
	bool pManaShield;
	char bReserved[3];
	short wReserved[8];
	uint32_t pDiabloKillLevel;
	int dwReserved[7];

	void *pReserved;
} PlayerStruct;


}

#endif  // __PLAYER_DEFS__
