/**
 * @file enums.h
 *
 * Various global enumerators.
 */

#ifndef __ITEM_DEFS_H__
#define __ITEM_DEFS_H__

#include "item_enums.h"
#include "vectordvl.h"

namespace dvl {

//////////////////////////////////////////////////
// items
//////////////////////////////////////////////////

typedef struct TestMeStruct {

} TestMeStruct;

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
	ItemType _itype;
	V2Di _i;
	bool _iAnimFlag;
	unsigned char *_iAnimData;  // PSX name -> ItemFrame
	int _iAnimLen;
	int _iAnimFrame;
	int _iAnimWidth;
	int _iAnimWidth2;  // width 2?
	bool
	    _iDelFlag;  // set when item is flagged for deletion, deprecated in 1.02
	char _iSelFlag;
	bool _iPostDraw;
	bool _iIdentified;
	ItemQuality _iMagical;
	char _iName[64];
	char _iIName[64];
	ItemSlot _iLoc = ItemSlot::Invalid;
	// item_class enum
	ItemClass _iClass;
	ItemCursor _iCurs;
	int _ivalue;
	int _iIvalue;
	int _iMinDam;
	int _iMaxDam;
	int _iAC;
	// item_special_effect
	ItemSpecialEffectFlags _iFlags;
	// item_misc_id
	MiscItemId _iMiscId;
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
	ItemEffectType _iPrePower;
	ItemEffectType _iSufPower;
	int _iVAdd1;
	int _iVMult1;
	int _iVAdd2;
	int _iVMult2;
	char _iMinStr;
	unsigned char _iMinMag;
	char _iMinDex;
	bool _iStatFlag;
	ItemIndex IDidx;  // Index of entry in "all items list" (list of global item data)
	int offs016C;  // _oldlight or _iInvalid
} ItemStruct;

}

#endif // __ITEM_DEFS_H__

