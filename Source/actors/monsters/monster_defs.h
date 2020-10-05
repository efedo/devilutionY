// HEADER_GOES_HERE
#ifndef __MONSTER_DEFS_H__
#define __MONSTER_DEFS_H__

#include "monster_enums.h"

DEVILUTION_BEGIN_NAMESPACE

//////////////////////////////////////////////////
// monster
//////////////////////////////////////////////////

struct AnimStruct {
	uint8_t *CMem;
	uint8_t *Data[8];
	int Frames;
	int Rate;
};

struct MonsterData {
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
	MonstAi mAi;
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
};

struct CMonster {
	MonsterType mtype;
	// TODO: Add enum for place flags
	unsigned char mPlaceFlags;
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
	AnimStruct &getAnim(MonstAnim anm) { return Anims[int(anm)]; }
   private:
	AnimStruct Anims[6];
};

struct MonsterStruct {  // note: missing field _mAFNum
	int _mMTidx;
	MonsterMode _mmode = MonsterMode::STAND; /* MON_MODE */
	MonsterGoal _mgoal = MonsterGoal::None;
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
	Dir _mdir = Dir(random_(89, 8));
	int _menemy = random_(89, gbActivePlayers);
	V2Di _menemypos = plr[Monst->_menemy].futpos();
	short falign_52;  // probably _mAFNum (unused)
	unsigned char *_mAnimData = 0;
	int _mAnimDelay = 0;
	int _mAnimCnt = 0;
	int _mAnimLen = 0;
	int _mAnimFrame = 0;
	bool _meflag;
	bool _mDelFlag = false;
	int _mVar1 = 0;
	int _mVar2 = 0;
	int _mVar3 = 0;
	int _mVar4 = 0;
	int _mVar5 = 0;
	int _mVar6 = 0;
	int _mVar7 = 0;
	int _mVar8 = 0;
	int _mmaxhp;
	int _mhitpoints;
	MonstAi _mAi;
	unsigned char _mint;
	short falign_9A;
	MonsterFlags _mFlags = 0;
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
	char *mName = "Invalid Monster";
	CMonster *MType;
	MonsterData *MData;
};

struct UniqMonstStruct {
	MonsterType mtype;
	char *mName;
	char *mTrnName;
	unsigned char mlevel;
	unsigned short mmaxhp;
	MonstAi mAi;
	unsigned char mint;
	unsigned char mMinDamage;
	unsigned char mMaxDamage;
	unsigned short mMagicRes;
	unsigned short mUnqAttr;
	unsigned char mUnqVar1;
	unsigned char mUnqVar2;
	int mtalkmsg;
};


DEVILUTION_END_NAMESPACE

#endif /* __MONSTER_DEFS_H__ */
