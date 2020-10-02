// HEADER_GOES_HERE
#ifndef __MONSTER_DEFS_H__
#define __MONSTER_DEFS_H__

DEVILUTION_BEGIN_NAMESPACE

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
	unsigned char
	    mHit;  // BUGFIX: Some monsters overflow this value on high difficulty
	unsigned char mAFNum;
	unsigned char mMinDamage;
	unsigned char mMaxDamage;
	unsigned char
	    mHit2;  // BUGFIX: Some monsters overflow this value on high difficulty
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

typedef struct MonsterStruct {  // note: missing field _mAFNum
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
	short falign_52;  // probably _mAFNum (unused)
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


DEVILUTION_END_NAMESPACE

#endif /* __MONSTER_DEFS_H__ */
