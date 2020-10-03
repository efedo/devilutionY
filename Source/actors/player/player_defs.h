/**
 * @file xyz.abc
 *
 * Description.
 */
#ifndef __TEMPLATE_H__
#define __TEMPLATE_H__

DEVILUTION_BEGIN_NAMESPACE

//////////////////////////////////////////////////
// player
//////////////////////////////////////////////////

typedef struct PlayerStruct {
	int _pmode;
	// char walkpath = -1;
	std::queue<PathNode> wkpath;
	bool plractive;
	int destAction;
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
	int _pSpell;
	char _pSplType;
	char _pSplFrom;
	int _pTSpell;
	char _pTSplType;
	int _pRSpell;
	// enum spell_type
	char _pRSplType;
	int _pSBkSpell;
	char _pSBkSplType;
	char _pSplLvl[64];
	uint64_t _pMemSpells;
	uint64_t _pAblSpells;
	uint64_t _pScrlSpells;
	uint8_t _pSpellFlags;
	int _pSplHotKey[4];
	char _pSplTHotKey[4];
	int _pwtype;
	bool _pBlockFlag;
	bool _pInvincible;
	char _pLightRad;
	bool _pLvlChanging;
	char _pName[PLR_NAME_LEN];
	// plr_class enum value.
	// TODO: this could very well be `enum plr_class _pClass`
	// since there are 3 bytes of alingment after this field.
	// it could just be that the compiler optimized away all accesses to
	// the higher bytes by using byte instructions, since all possible values
	// of plr_class fit into one byte.
	PlayerClass _pClass;
	int _pStrength;
	int _pBaseStr;
	int _pMagic;
	int _pBaseMag;
	int _pDexterity;
	int _pBaseDex;
	int _pVitality;
	int _pBaseVit;
	int _pStatPts;
	int _pDamageMod;
	int _pBaseToBlk;
	int _pHPBase;
	int _pMaxHPBase;
	int _pHitPoints;
	int _pMaxHP;
	int _pHPPer;
	int _pManaBase;
	int _pMaxManaBase;
	int _pMana;
	int _pMaxMana;
	int _pManaPer;
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
	bool _pLvlVisited[NUMLEVELS];
	bool _pSetLvlVisited[NUMLEVELS];  // only 10 used
	int _pGFXLoad;
	unsigned char *_pNAnim[8];
	int _pNFrames;
	int _pNWidth;
	unsigned char *_pWAnim[8];
	int _pWFrames;
	int _pWWidth;
	unsigned char *_pAAnim[8];
	int _pAFrames;
	int _pAWidth;
	int _pAFNum;
	unsigned char *_pLAnim[8];
	unsigned char *_pFAnim[8];
	unsigned char *_pTAnim[8];
	int _pSFrames;
	int _pSWidth;
	int _pSFNum;
	unsigned char *_pHAnim[8];
	int _pHFrames;
	int _pHWidth;
	unsigned char *_pDAnim[8];
	int _pDFrames;
	int _pDWidth;
	unsigned char *_pBAnim[8];
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
	char _pISplLvlAdd;
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
	unsigned char *_pNData;
	unsigned char *_pWData;
	unsigned char *_pAData;
	unsigned char *_pLData;
	unsigned char *_pFData;
	unsigned char *_pTData;
	unsigned char *_pHData;
	unsigned char *_pDData;
	unsigned char *_pBData;
	void *pReserved;
} PlayerStruct;


DEVILUTION_END_NAMESPACE

#endif  // __TEMPLATE_H__
