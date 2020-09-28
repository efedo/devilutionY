/**
 * @file structs.h
 *
 * Various global structures.
 */

#ifndef __STRUCTS_H__
#define __STRUCTS_H__

#include <vector>
#include <queue>
#include "enums.h"
#include "vectordvl.h"
#include "pathfinding/directions.h"
#include "pathfinding/pathfinder.h"

DEVILUTION_BEGIN_NAMESPACE

//////////////////////////////////////////////////
// control
//////////////////////////////////////////////////

typedef struct RECT32 {
	int x;
	int y;
	int w;
	int h;
} RECT32;

//////////////////////////////////////////////////
// items
//////////////////////////////////////////////////

typedef struct PLStruct {
	const char *PLName;
	int PLPower;
	int PLParam1;
	int PLParam2;
	char PLMinLvl;
	int PLIType;
	uint8_t PLGOE;
	bool PLDouble;
	bool PLOk;
	int PLMinVal;
	int PLMaxVal;
	int PLMultVal;
} PLStruct;

typedef struct UItemStruct {
	char *UIName;
	char UIItemId;
	char UIMinLvl;
	char UINumPL;
	int UIValue;
	char UIPower1;
	int UIParam1;
	int UIParam2;
	char UIPower2;
	int UIParam3;
	int UIParam4;
	char UIPower3;
	int UIParam5;
	int UIParam6;
	char UIPower4;
	int UIParam7;
	int UIParam8;
	char UIPower5;
	int UIParam9;
	int UIParam10;
	char UIPower6;
	int UIParam11;
	int UIParam12;
} UItemStruct;

typedef struct ItemDataStruct {
	int iRnd;
	char iClass;
	char iLoc;
	int iCurs;
	char itype;
	char iItemId;
	char *iName;
	char *iSName;
	char iMinMLvl;
	int iDurability;
	int iMinDam;
	int iMaxDam;
	int iMinAC;
	int iMaxAC;
	char iMinStr;
	char iMinMag;
	char iMinDex;
	// item_special_effect
	int iFlags;
	// item_misc_id
	int iMiscId;
	// spell_id
	int iSpell;
	bool iUsable;
	int iValue;
	int iMaxValue;
} ItemDataStruct;

typedef struct ItemGetRecordStruct {
	int nSeed;
	unsigned short wCI;
	int nIndex;
	unsigned int dwTimestamp;
} ItemGetRecordStruct;

typedef struct ItemStruct {
	int _iSeed;
	WORD _iCreateInfo;
	int _itype;
	V2Di _i;
	bool _iAnimFlag;
	unsigned char *_iAnimData; // PSX name -> ItemFrame
	int _iAnimLen;
	int _iAnimFrame;
	int _iAnimWidth;
	int _iAnimWidth2; // width 2?
	bool _iDelFlag;   // set when item is flagged for deletion, deprecated in 1.02
	char _iSelFlag;
	bool _iPostDraw;
	bool _iIdentified;
	char _iMagical;
	char _iName[64];
	char _iIName[64];
	char _iLoc;
	// item_class enum
	char _iClass;
	int _iCurs;
	int _ivalue;
	int _iIvalue;
	int _iMinDam;
	int _iMaxDam;
	int _iAC;
	// item_special_effect
	int _iFlags;
	// item_misc_id
	int _iMiscId;
	// spell_id
	int _iSpell;
	int _iCharges;
	int _iMaxCharges;
	int _iDurability;
	int _iMaxDur;
	int _iPLDam;
	int _iPLToHit;
	int _iPLAC;
	int _iPLStr;
	int _iPLMag;
	int _iPLDex;
	int _iPLVit;
	int _iPLFR;
	int _iPLLR;
	int _iPLMR;
	int _iPLMana;
	int _iPLHP;
	int _iPLDamMod;
	int _iPLGetHit;
	int _iPLLight;
	char _iSplLvlAdd;
	char _iRequest;
	int _iUid;
	int _iFMinDam;
	int _iFMaxDam;
	int _iLMinDam;
	int _iLMaxDam;
	int _iPLEnAc;
	char _iPrePower;
	char _iSufPower;
	int _iVAdd1;
	int _iVMult1;
	int _iVAdd2;
	int _iVMult2;
	char _iMinStr;
	unsigned char _iMinMag;
	char _iMinDex;
	bool _iStatFlag;
	int IDidx;
	int offs016C; // _oldlight or _iInvalid
} ItemStruct;

//////////////////////////////////////////////////
// player
//////////////////////////////////////////////////

typedef struct PlayerStruct {
	int _pmode;
	//char walkpath = -1;
	std::queue<PathNode> wkpath;
	bool plractive;
	int destAction;
	int destParam1;
	int destParam2;
	int destParam3;
	int destParam4;
	int plrlevel; // Holds either standard or 'set' level 
	V2Di _pos;
	V2Di _posfut;
	V2Di _posdraw;
	V2Di _pathtarg;
	V2Di _powner;
	V2Di _pold;
	V2Di _poff;
	V2Di _pvel;
	Dir _pdir;
	//int _nextdir;
	int _pgfxnum;
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
	char _pClass;
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
	bool _pSetLvlVisited[NUMLEVELS]; // only 10 used
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
	ItemStruct InvBody[NUM_INVLOC];
	ItemStruct InvList[NUM_INV_GRID_ELEM];
	int _pNumInv;
	char InvGrid[NUM_INV_GRID_ELEM];
	ItemStruct SpdList[MAXBELTITEMS];
	ItemStruct HoldItem;
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

//////////////////////////////////////////////////
// textdat
//////////////////////////////////////////////////

typedef struct TextDataStruct {
	char *txtstr;
	int scrlltxt;
	int txtspd;
	int sfxnr;
} TextDataStruct;

//////////////////////////////////////////////////
// missiles
//////////////////////////////////////////////////

// TPDEF PTR FCN VOID MIADDPRC
// TPDEF PTR FCN VOID MIPROC

typedef struct MissileData {
	unsigned char mName;
	void (*mAddProc)(int, V2Di, V2Di, Dir, char, int, int);
	void (*mProc)(int);
	bool mDraw;
	unsigned char mType;
	unsigned char mResist;
	unsigned char mFileNum;
	int mlSFX;
	int miSFX;
} MissileData;

typedef struct MisFileData {
	unsigned char mAnimName;
	unsigned char mAnimFAmt;
	char *mName;
	int mFlags;
	unsigned char *mAnimData[16];
	unsigned char mAnimDelay[16];
	unsigned char mAnimLen[16];
	int mAnimWidth[16];
	int mAnimWidth2[16];
} MisFileData;

typedef struct ChainStruct {
	int idx;
	int _mitype;
	int _mirange;
} ChainStruct;

typedef struct MissileStruct {
	int _mitype;
	V2Di _mi;
	V2Di _mioff;
	V2Di _mivel;
	V2Di _mis;
	V2Di _mitoff;
	Dir _mimfnum; // Imf Num??? Imformation???
	int _mispllvl;
	bool _miDelFlag;
	uint8_t _miAnimType;
	int _miAnimFlags;
	unsigned char *_miAnimData;
	int _miAnimDelay;
	int _miAnimLen;
	int _miAnimWidth;
	int _miAnimWidth2;
	int _miAnimCnt;
	int _miAnimAdd;
	int _miAnimFrame;
	bool _miDrawFlag;
	bool _miLightFlag;
	bool _miPreFlag;
	int _miUniqTrans;
	int _mirange;
	int _misource;
	int _micaster;
	int _midam;
	bool _miHitFlag;
	int _midist;
	int _mlid;
	int _mirnd;
	int _miVar1;
	int _miVar2;
	int _miVar3;
	int _miVar4;
	int _miVar5;
	int _miVar6;
	int _miVar7;
	int _miVar8;
} MissileStruct;

//////////////////////////////////////////////////
// effects/sound
//////////////////////////////////////////////////

typedef struct TSnd {
	char *sound_path;
	SoundSample *DSB;
	int start_tc;
} TSnd;

typedef struct TSFX {
	unsigned char bFlags;
	char *pszName;
	TSnd *pSnd;
} TSFX;

//////////////////////////////////////////////////
// monster
//////////////////////////////////////////////////

typedef struct AnimStruct {
	uint8_t *CMem;
	uint8_t *Data[8];
	int Frames;
	int Rate;
} AnimStruct;

typedef struct MonsterData {
	int width;
	int mImage;
	char *GraphicType;
	bool has_special;
	char *sndfile;
	bool snd_special;
	bool has_trans;
	char *TransFile;
	int Frames[6];
	int Rate[6];
	char *mName;
	char mMinDLvl;
	char mMaxDLvl;
	char mLevel;
	int mMinHP;
	int mMaxHP;
	char mAi;
	int mFlags;
	unsigned char mInt;
	unsigned char mHit; // BUGFIX: Some monsters overflow this value on high difficulty
	unsigned char mAFNum;
	unsigned char mMinDamage;
	unsigned char mMaxDamage;
	unsigned char mHit2; // BUGFIX: Some monsters overflow this value on high difficulty
	unsigned char mAFNum2;
	unsigned char mMinDamage2;
	unsigned char mMaxDamage2;
	unsigned char mArmorClass;
	char mMonstClass;
	unsigned short mMagicRes;
	unsigned short mMagicRes2;
	unsigned short mTreasure;
	char mSelFlag;
	unsigned short mExp;
} MonsterData;

typedef struct CMonster {
	unsigned char mtype;
	// TODO: Add enum for place flags
	unsigned char mPlaceFlags;
	AnimStruct Anims[6];
	TSnd *Snds[4][2];
	int width;
	int width2;
	unsigned char mMinHP;
	unsigned char mMaxHP;
	bool has_special;
	unsigned char mAFNum;
	char mdeadval;
	MonsterData *MData;
	// A TRN file contains a sequence of colour transitions, represented
	// as indexes into a palette. (a 256 byte array of palette indices)
	uint8_t *trans_file;
} CMonster;

typedef struct MonsterStruct { // note: missing field _mAFNum
	int _mMTidx;
	int _mmode; /* MON_MODE */
	unsigned char _mgoal;
	int _mgoalvar1;
	int _mgoalvar2;
	int _mgoalvar3;
	int field_18;
	unsigned char _pathcount;
	V2Di _m;
	V2Di _mfut;
	V2Di _mold;
	V2Di _moff;
	V2Di _mvel;
	Dir _mdir;
	int _menemy;
	V2Di _menemypos;
	short falign_52; // probably _mAFNum (unused)
	unsigned char *_mAnimData;
	int _mAnimDelay;
	int _mAnimCnt;
	int _mAnimLen;
	int _mAnimFrame;
	bool _meflag;
	bool _mDelFlag;
	int _mVar1;
	int _mVar2;
	int _mVar3;
	int _mVar4;
	int _mVar5;
	int _mVar6;
	int _mVar7;
	int _mVar8;
	int _mmaxhp;
	int _mhitpoints;
	unsigned char _mAi;
	unsigned char _mint;
	short falign_9A;
	int _mFlags;
	uint8_t _msquelch;
	int falign_A4;
	V2Di _last;
	int _mRndSeed;
	int _mAISeed;
	int falign_B8;
	unsigned char _uniqtype;
	unsigned char _uniqtrans;
	char _udeadval;
	char mWhoHit;
	char mLevel;
	unsigned short mExp;
	unsigned char mHit;
	unsigned char mMinDamage;
	unsigned char mMaxDamage;
	unsigned char mHit2;
	unsigned char mMinDamage2;
	unsigned char mMaxDamage2;
	unsigned char mArmorClass;
	char falign_CB;
	unsigned short mMagicRes;
	int mtalkmsg;
	unsigned char leader;
	unsigned char leaderflag;
	unsigned char packsize;
	unsigned char mlid;
	char *mName;
	CMonster *MType;
	MonsterData *MData;
} MonsterStruct;

typedef struct UniqMonstStruct {
	char mtype;
	char *mName;
	char *mTrnName;
	unsigned char mlevel;
	unsigned short mmaxhp;
	unsigned char mAi;
	unsigned char mint;
	unsigned char mMinDamage;
	unsigned char mMaxDamage;
	unsigned short mMagicRes;
	unsigned short mUnqAttr;
	unsigned char mUnqVar1;
	unsigned char mUnqVar2;
	int mtalkmsg;
} UniqMonstStruct;

//////////////////////////////////////////////////
// objects
//////////////////////////////////////////////////

typedef struct ObjDataStruct {
	char oload;
	char ofindex;
	char ominlvl;
	char omaxlvl;
	DunType olvltype;
	char otheme;
	char oquest;
	int oAnimFlag;
	int oAnimDelay;
	int oAnimLen;
	int oAnimWidth;
	bool oSolidFlag;
	bool oMissFlag;
	bool oLightFlag;
	char oBreak;
	char oSelFlag;
	bool oTrapFlag;
} ObjDataStruct;

typedef struct ObjectStruct {
	int _otype;
	V2Di _o;
	int _oLight;
	int _oAnimFlag;
	unsigned char *_oAnimData;
	int _oAnimDelay;
	int _oAnimCnt;
	int _oAnimLen;
	int _oAnimFrame;
	int _oAnimWidth;
	int _oAnimWidth2;
	bool _oDelFlag;
	char _oBreak; // check
	bool _oSolidFlag;
	bool _oMissFlag;
	char _oSelFlag; // check
	bool _oPreFlag;
	bool _oTrapFlag;
	bool _oDoorFlag;
	int _olid;
	int _oRndSeed;
	int _oVar1;
	int _oVar2;
	int _oVar3;
	int _oVar4;
	int _oVar5;
	int _oVar6;
	int _oVar7;
	int _oVar8;
} ObjectStruct;

//////////////////////////////////////////////////
// portal
//////////////////////////////////////////////////

typedef struct PortalStruct {
	bool open;
	V2Di pos;
	int level;
	DunType ltype;
	bool setlvl;
} PortalStruct;

//////////////////////////////////////////////////
// msg
//////////////////////////////////////////////////

#pragma pack(push, 1)
typedef struct TCmd {
	uint8_t bCmd;
} TCmd;

typedef struct TCmdLoc {
	uint8_t bCmd;
	uint8_t x;
	uint8_t y;
} TCmdLoc;

typedef struct TCmdLocParam1 {
	uint8_t bCmd;
	uint8_t x;
	uint8_t y;
	WORD wParam1;
} TCmdLocParam1;

typedef struct TCmdLocParam2 {
	uint8_t bCmd;
	uint8_t x;
	uint8_t y;
	WORD wParam1;
	WORD wParam2;
} TCmdLocParam2;

typedef struct TCmdLocParam3 {
	uint8_t bCmd;
	uint8_t x;
	uint8_t y;
	WORD wParam1;
	WORD wParam2;
	WORD wParam3;
} TCmdLocParam3;

typedef struct TCmdParam1 {
	uint8_t bCmd;
	WORD wParam1;
} TCmdParam1;

typedef struct TCmdParam2 {
	uint8_t bCmd;
	WORD wParam1;
	WORD wParam2;
} TCmdParam2;

typedef struct TCmdParam3 {
	uint8_t bCmd;
	WORD wParam1;
	WORD wParam2;
	WORD wParam3;
} TCmdParam3;

typedef struct TCmdGolem {
	uint8_t bCmd;
	uint8_t _mx;
	uint8_t _my;
	uint8_t _mdir;
	char _menemy;
	int _mhitpoints;
	uint8_t currlevel;
} TCmdGolem;

typedef struct TCmdQuest {
	uint8_t bCmd;
	uint8_t q;
	uint8_t qstate;
	uint8_t qlog;
	uint8_t qvar1;
} TCmdQuest;

typedef struct TCmdGItem {
	uint8_t bCmd;
	uint8_t bMaster;
	uint8_t bPnum;
	uint8_t bCursitem;
	uint8_t bLevel;
	uint8_t x;
	uint8_t y;
	WORD wIndx;
	WORD wCI;
	int dwSeed;
	uint8_t bId;
	uint8_t bDur;
	uint8_t bMDur;
	uint8_t bCh;
	uint8_t bMCh;
	WORD wValue;
	uint32_t dwBuff;
	int dwTime;
} TCmdGItem;

typedef struct TCmdPItem {
	uint8_t bCmd;
	uint8_t x;
	uint8_t y;
	WORD wIndx;
	WORD wCI;
	int dwSeed;
	uint8_t bId;
	uint8_t bDur;
	uint8_t bMDur;
	uint8_t bCh;
	uint8_t bMCh;
	WORD wValue;
	uint32_t dwBuff;
} TCmdPItem;

typedef struct TCmdChItem {
	uint8_t bCmd;
	uint8_t bLoc;
	WORD wIndx;
	WORD wCI;
	int dwSeed;
	bool bId;
} TCmdChItem;

typedef struct TCmdDelItem {
	uint8_t bCmd;
	uint8_t bLoc;
} TCmdDelItem;

typedef struct TCmdDamage {
	uint8_t bCmd;
	uint8_t bPlr;
	uint32_t dwDam;
} TCmdDamage;

typedef struct TCmdPlrInfoHdr {
	uint8_t bCmd;
	WORD wOffset;
	WORD wBytes;
} TCmdPlrInfoHdr;

typedef struct TCmdString {
	uint8_t bCmd;
	char str[MAX_SEND_STR_LEN];
} TCmdString;

typedef struct TFakeCmdPlr {
	uint8_t bCmd;
	uint8_t bPlr;
} TFakeCmdPlr;

typedef struct TFakeDropPlr {
	uint8_t bCmd;
	uint8_t bPlr;
	uint32_t dwReason;
} TFakeDropPlr;

typedef struct TSyncHeader {
	uint8_t bCmd;
	uint8_t bLevel;
	WORD wLen;
	uint8_t bObjId;
	uint8_t bObjCmd;
	uint8_t bItemI;
	uint8_t bItemX;
	uint8_t bItemY;
	WORD wItemIndx;
	WORD wItemCI;
	uint32_t dwItemSeed;
	uint8_t bItemId;
	uint8_t bItemDur;
	uint8_t bItemMDur;
	uint8_t bItemCh;
	uint8_t bItemMCh;
	WORD wItemVal;
	uint32_t dwItemBuff;
	uint8_t bPInvLoc;
	WORD wPInvIndx;
	WORD wPInvCI;
	uint32_t dwPInvSeed;
	uint8_t bPInvId;
} TSyncHeader;

typedef struct TSyncMonster {
	uint8_t _mndx;
	uint8_t _mx;
	uint8_t _my;
	uint8_t _menemy;
	uint8_t _mdelta;
} TSyncMonster;

typedef struct TPktHdr {
	uint8_t px;
	uint8_t py;
	uint8_t targx;
	uint8_t targy;
	int php;
	int pmhp;
	uint8_t bstr;
	uint8_t bmag;
	uint8_t bdex;
	WORD wCheck;
	WORD wLen;
} TPktHdr;

typedef struct TPkt {
	TPktHdr hdr;
	uint8_t body[493];
} TPkt;

typedef struct DMonsterStr {
	uint8_t _mx;
	uint8_t _my;
	uint8_t _mdir;
	uint8_t _menemy;
	uint8_t _mactive;
	int _mhitpoints;
} DMonsterStr;

typedef struct DObjectStr {
	uint8_t bCmd;
} DObjectStr;

typedef struct DLevel {
	TCmdPItem item[MAXITEMS];
	DObjectStr object[MAXOBJECTS];
	DMonsterStr monster[MAXMONSTERS];
} DLevel;

typedef struct LocalLevel {
	uint8_t automapsv[DMAXX][DMAXY];
} LocalLevel;

typedef struct DPortal {
	uint8_t x;
	uint8_t y;
	uint8_t level;
	DunType ltype;
	uint8_t setlvl;
} DPortal;

typedef struct MultiQuests {
	uint8_t qstate;
	uint8_t qlog;
	uint8_t qvar1;
} MultiQuests;

typedef struct DJunk {
	DPortal portal[MAXPORTAL];
	MultiQuests quests[MAXMULTIQUESTS];
} DJunk;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct TMegaPkt {
	struct TMegaPkt *pNext;
	uint32_t dwSpaceLeft;
	uint8_t data[32000];
} TMegaPkt;
#pragma pack(pop)

typedef struct TBuffer {
	uint32_t dwNextWriteOffset;
	uint8_t bData[4096];
} TBuffer;

//////////////////////////////////////////////////
// quests
//////////////////////////////////////////////////

typedef struct QuestStruct {
	unsigned char _qlevel;
	unsigned char _qtype;
	unsigned char _qactive;
	DunType _qlvltype;
	V2Di _qt;
	//int _qtx;
	//int _qty;
	unsigned char _qslvl;
	unsigned char _qidx;
	unsigned char _qmsg;
	unsigned char _qvar1;
	unsigned char _qvar2;
	int _qlog;
} QuestStruct;

typedef struct QuestData {
	unsigned char _qdlvl;
	char _qdmultlvl;
	DunType _qlvlt;
	unsigned char _qdtype;
	unsigned char _qdrnd;
	unsigned char _qslvl;
	int _qflags; /* unsigned char */
	int _qdmsg;
	char *_qlstr;
} QuestData;

//////////////////////////////////////////////////
// gamemenu/gmenu
//////////////////////////////////////////////////

// TPDEF PTR FCN VOID TMenuFcn

typedef struct TMenuItem {
	uint32_t dwFlags;
	char *pszStr;
	void (*fnMenu)(bool); /* fix, should have one arg */
} TMenuItem;

// TPDEF PTR FCN VOID TMenuUpdateFcn

//////////////////////////////////////////////////
// spells
//////////////////////////////////////////////////

typedef struct SpellData {
	unsigned char sName;
	unsigned char sManaCost;
	unsigned char sType;
	char *sNameText;
	char *sSkillText;
	int sBookLvl;
	int sStaffLvl;
	bool sTargeted;
	bool sTownSpell;
	int sMinInt;
	unsigned char sSFX;
	unsigned char sMissiles[3];
	unsigned char sManaAdj;
	unsigned char sMinMana;
	int sStaffMin;
	int sStaffMax;
	int sBookCost;
	int sStaffCost;
} SpellData;

//////////////////////////////////////////////////
// towners
//////////////////////////////////////////////////

typedef struct TNQ {
	unsigned char _qsttype;
	unsigned char _qstmsg;
	bool _qstmsgact;
} TNQ;

typedef struct TownerStruct {
	int _tmode;
	int _ttype;
	V2Di _t;
	V2Di _toff;
	V2Di _tvel;
	int _tdir;
	unsigned char *_tAnimData;
	int _tAnimDelay;
	int _tAnimCnt;
	int _tAnimLen;
	int _tAnimFrame;
	int _tAnimFrameCnt;
	char _tAnimOrder;
	int _tAnimWidth;
	int _tAnimWidth2;
	int _tTenPer;
	int _teflag;
	int _tbtcnt;
	int _tSelFlag;
	bool _tMsgSaid;
	TNQ qsts[MAXQUESTS];
	int _tSeed;
	int _tVar1;
	int _tVar2;
	int _tVar3;
	int _tVar4;
	char _tName[PLR_NAME_LEN];
	unsigned char *_tNAnim[8];
	int _tNFrames;
	unsigned char *_tNData;
} TownerStruct;

typedef struct QuestTalkData {
	int _qinfra;
	int _qblkm;
	int _qgarb;
	int _qzhar;
	int _qveil;
	int _qmod;
	int _qbutch;
	int _qbol;
	int _qblind;
	int _qblood;
	int _qanvil;
	int _qwarlrd;
	int _qking;
	int _qpw;
	int _qbone;
	int _qvb;
} QuestTalkData;

//////////////////////////////////////////////////
// gendung
//////////////////////////////////////////////////

typedef struct ScrollStruct {
	V2Di _soff;
	V2Di _sd;
	ScrollDir _sdir;
} ScrollStruct;

typedef struct THEME_LOC {
	//V2Di pos;
	int x;
	int y;
	int ttval;
	int width;
	int height;
} THEME_LOC;

typedef struct MICROS {
	WORD mt[16];
} MICROS;

//////////////////////////////////////////////////
// drlg
//////////////////////////////////////////////////

typedef struct ShadowStruct {
	unsigned char strig;
	unsigned char s1;
	unsigned char s2;
	unsigned char s3;
	unsigned char nv1;
	unsigned char nv2;
	unsigned char nv3;
} ShadowStruct;

typedef struct HALLNODE {
	int nHallx1;
	int nHally1;
	int nHallx2;
	int nHally2;
	int nHalldir;
	struct HALLNODE *pNext;
} HALLNODE;

typedef struct ROOMNODE {
	int nRoomx1;
	int nRoomy1;
	int nRoomx2;
	int nRoomy2;
	int nRoomDest;
} ROOMNODE;

//////////////////////////////////////////////////
// themes
//////////////////////////////////////////////////

typedef struct ThemeStruct {
	char ttype; /* aligned 4 */
	int ttval;
} ThemeStruct;

//////////////////////////////////////////////////
// inv
//////////////////////////////////////////////////

typedef struct InvXY {
	int X;
	int Y;
} InvXY;

//////////////////////////////////////////////////
// lighting
//////////////////////////////////////////////////

typedef struct LightListStruct {
	V2Di _l;
	int _lradius;
	int _lid;
	int _ldel;
	int _lunflag;
	int field_18;
	V2Di _lun;
	int _lunr;
	V2Di _off;
	int _lflags;
} LightListStruct;

//////////////////////////////////////////////////
// dead
//////////////////////////////////////////////////

typedef struct DeadStruct {
	unsigned char *_deadData[8];
	int _deadFrame;
	int _deadWidth;
	int _deadWidth2;
	char _deadtrans;
} DeadStruct;

//////////////////////////////////////////////////
// diabloui
//////////////////////////////////////////////////

// TPDEF PTR FCN VOID PLAYSND

typedef struct _gamedata {
	int dwSeed;
	uint8_t bDiff;
} _gamedata;

typedef struct _uidefaultstats {
	WORD strength;
	WORD magic;
	WORD dexterity;
	WORD vitality;
} _uidefaultstats;

typedef struct _uiheroinfo {
	struct _uiheroinfo *next;
	char name[16];
	WORD level;
	uint8_t heroclass;
	uint8_t herorank;
	WORD strength;
	WORD magic;
	WORD dexterity;
	WORD vitality;
	int gold;
	int hassaved;
	bool spawned;
} _uiheroinfo;

// TPDEF PTR FCN uint8_t ENUMHEROPROC
// TPDEF PTR FCN uint8_t ENUMHEROS
// TPDEF PTR FCN uint8_t CREATEHERO
// TPDEF PTR FCN uint8_t DELETEHERO
// TPDEF PTR FCN uint8_t GETDEFHERO

// TPDEF PTR FCN INT PROGRESSFCN

//////////////////////////////////////////////////
// storm
//////////////////////////////////////////////////

// TPDEF PTR FCN uint8_t SMSGIDLEPROC
// TPDEF PTR FCN VOID SMSGHANDLER

typedef struct _SNETCAPS {
	uint32_t size;
	uint32_t flags;
	uint32_t maxmessagesize;
	uint32_t maxqueuesize;
	uint32_t maxplayers;
	uint32_t bytessec;
	uint32_t latencyms;
	uint32_t defaultturnssec;
	uint32_t defaultturnsintransit;
} _SNETCAPS;

typedef struct _SNETEVENT {
	uint32_t eventid;
	uint32_t playerid;
	void *data;
	uint32_t databytes;
} _SNETEVENT;

// TPDEF PTR FCN uint8_t SNETABORTPROC
// TPDEF PTR FCN uint8_t SNETCATEGORYPROC
// TPDEF PTR FCN uint8_t SNETCHECKAUTHPROC
// TPDEF PTR FCN uint8_t SNETCREATEPROC
// TPDEF PTR FCN uint8_t SNETDRAWDESCPROC
// TPDEF PTR FCN uint8_t SNETENUMDEVICESPROC
// TPDEF PTR FCN uint8_t SNETENUMGAMESPROC
// TPDEF PTR FCN uint8_t SNETENUMPROVIDERSPROC
// TPDEF PTR FCN VOID SNETEVENTPROC
// TPDEF PTR FCN uint8_t SNETGETARTPROC
// TPDEF PTR FCN uint8_t SNETGETDATAPROC
// TPDEF PTR FCN INT SNETMESSAGEBOXPROC
// TPDEF PTR FCN uint8_t SNETPLAYSOUNDPROC
// TPDEF PTR FCN uint8_t SNETSELECTEDPROC
// TPDEF PTR FCN uint8_t SNETSTATUSPROC

typedef struct _SNETPLAYERDATA {
	int size;
	char *playername;
	char *playerdescription;
	int reserved;
} _SNETPLAYERDATA;

typedef struct _SNETPROGRAMDATA {
	int size;
	char *programname;
	char *programdescription;
	int programid;
	int versionid;
	int reserved1;
	int maxplayers;
	_gamedata *initdata;
	int initdatabytes;
	void *reserved2;
	int optcategorybits;
	char *cdkey;
	char *registereduser;
	int spawned;
	int lcid;
} _SNETPROGRAMDATA;

typedef struct _SNETVERSIONDATA {
	int size;
	char *versionstring;
	char *executablefile;
	char *originalarchivefile;
	char *patcharchivefile;
} _SNETVERSIONDATA;

typedef struct _SNETUIDATA {
	int size;
	int uiflags;
	HWND parentwindow;
	void (*artcallback)();
	void (*authcallback)();
	void (*createcallback)();
	void (*drawdesccallback)();
	void (*selectedcallback)();
	void (*messageboxcallback)();
	void (*soundcallback)();
	void (*statuscallback)();
	void (*getdatacallback)();
	void (*categorycallback)();
	void (*categorylistcallback)();
	void (*newaccountcallback)();
	void (*profilecallback)();
	const char **profilefields;
	void (*profilebitmapcallback)();
	bool (*selectnamecallback)(
	    const struct _SNETPROGRAMDATA *,
	    const struct _SNETPLAYERDATA *,
	    const struct _SNETUIDATA *,
	    const struct _SNETVERSIONDATA *,
	    uint32_t provider, /* e.g. 'BNET', 'IPXN', 'MODM', 'SCBL' */
	    char *, uint32_t,  /* character name will be copied here */
	    char *, uint32_t,  /* character "description" will be copied here (used to advertise games) */
	    bool *          /* new character? - unsure about this */
	);
	void (*changenamecallback)();
} _SNETUIDATA;

// TPDEF PTR FCN uint8_t SNETSPIBIND
// TPDEF PTR FCN uint8_t SNETSPIQUERY

//////////////////////////////////////////////////
// pack
//////////////////////////////////////////////////

#pragma pack(push, 1)
typedef struct PkItemStruct {
	uint32_t iSeed;
	WORD iCreateInfo;
	WORD idx;
	uint8_t bId;
	uint8_t bDur;
	uint8_t bMDur;
	uint8_t bCh;
	uint8_t bMCh;
	WORD wValue;
	uint32_t dwBuff;
} PkItemStruct;

typedef struct PkPlayerStruct {
	FILETIME archiveTime;
	char destAction;
	char destParam1;
	char destParam2;
	uint8_t plrlevel;
	V2Di p;
	V2Di targ;
	char pName[PLR_NAME_LEN];
	char pClass;
	uint8_t pBaseStr;
	uint8_t pBaseMag;
	uint8_t pBaseDex;
	uint8_t pBaseVit;
	char pLevel;
	uint8_t pStatPts;
	int pExperience;
	int pGold;
	int pHPBase;
	int pMaxHPBase;
	int pManaBase;
	int pMaxManaBase;
	char pSplLvl[MAX_SPELLS];
	uint64_t pMemSpells;
	PkItemStruct InvBody[NUM_INVLOC];
	PkItemStruct InvList[NUM_INV_GRID_ELEM];
	char InvGrid[NUM_INV_GRID_ELEM];
	uint8_t _pNumInv;
	PkItemStruct SpdList[MAXBELTITEMS];
	char pTownWarps;
	char pDungMsgs;
	char pLvlLoad;
	char pBattleNet;
	bool pManaShield;
	char bReserved[3];
	short wReserved[8];
	int pDiabloKillLevel;
	int dwReserved[7];
} PkPlayerStruct;
#pragma pack(pop)

// TPDEF PTR FCN uint8_t CHECKFUNC1

// TPDEF PTR FCN uint8_t CHECKFUNC

//////////////////////////////////////////////////
// sha
//////////////////////////////////////////////////

typedef struct SHA1Context {
	uint32_t state[5];
	uint32_t count[2];
	char buffer[64];
} SHA1Context;

//////////////////////////////////////////////////
// tmsg
//////////////////////////////////////////////////

#pragma pack(push, 1)
typedef struct TMsg TMsg;

typedef struct TMsgHdr {
	TMsg *pNext;
	int dwTime;
	uint8_t bLen;
} TMsgHdr;

typedef struct TMsg {
	TMsgHdr hdr;
	// this is actually alignment padding, but the message body is appended to the struct
	// so it's convenient to use byte-alignment and name it "body"
	unsigned char body[3];
} TMsg;
#pragma pack(pop)

//////////////////////////////////////////////////
// mpqapi
//////////////////////////////////////////////////

typedef struct _FILEHEADER {
	int signature;
	int headersize;
	int filesize;
	WORD version;
	short sectorsizeid;
	int hashoffset;
	int blockoffset;
	int hashcount;
	int blockcount;
	char pad[72];
} _FILEHEADER;

typedef struct _HASHENTRY {
	uint32_t hashcheck[2];
	uint32_t lcid;
	uint32_t block;
} _HASHENTRY;

typedef struct _BLOCKENTRY {
	uint32_t offset;
	uint32_t sizealloc;
	uint32_t sizefile;
	uint32_t flags;
} _BLOCKENTRY;

// TPDEF PTR FCN uint8_t TGetNameFcn

// TPDEF PTR FCN VOID TCrypt

//////////////////////////////////////////////////
// trigs
//////////////////////////////////////////////////

typedef struct TriggerStruct {
	V2Di _t;
	//int _tx;
	//int _ty;
	int _tmsg;
	int _tlvl;
} TriggerStruct;

//////////////////////////////////////////////////
// stores
//////////////////////////////////////////////////

typedef struct STextStruct {
	int _sx;
	int _syoff;
	char _sstr[128];
	bool _sjust;
	char _sclr;
	int _sline;
	bool _ssel;
	int _sval;
} STextStruct;

//////////////////////////////////////////////////
// wave
//////////////////////////////////////////////////

typedef struct MEMFILE {
	uint32_t end;
	int32_t offset;
	uint32_t buf_len;
	uint32_t dist;
	uint32_t bytes_to_read;
	uint8_t *buf;
	HANDLE file;
} MEMFILE;

//////////////////////////////////////////////////
// plrmsg
//////////////////////////////////////////////////

typedef struct _plrmsg {
	uint32_t time;
	unsigned char player;
	char str[144];
} _plrmsg;

//////////////////////////////////////////////////
// capture
//////////////////////////////////////////////////

typedef struct _PcxHeader {
	uint8_t Manufacturer;
	uint8_t Version;
	uint8_t Encoding;
	uint8_t BitsPerPixel;
	WORD Xmin;
	WORD Ymin;
	WORD Xmax;
	WORD Ymax;
	WORD HDpi;
	WORD VDpi;
	uint8_t Colormap[48];
	uint8_t Reserved;
	uint8_t NPlanes;
	WORD BytesPerLine;
	WORD PaletteInfo;
	WORD HscreenSize;
	WORD VscreenSize;
	uint8_t Filler[54];
} PCXHEADER;

//////////////////////////////////////////////////
// encrypt
//////////////////////////////////////////////////

typedef struct TDataInfo {
	uint8_t *srcData;
	uint32_t srcOffset;
	uint8_t *destData;
	uint32_t destOffset;
	uint32_t size;
} TDataInfo;

DEVILUTION_END_NAMESPACE

#endif /* __STRUCTS_H__ */
