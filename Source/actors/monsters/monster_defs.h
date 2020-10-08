// HEADER_GOES_HERE
#ifndef __MONSTER_DEFS_H__
#define __MONSTER_DEFS_H__

#include "monster_enums.h"
#include "objects/object_enums.h"
#include "actors/actor.h"
#include <set>

namespace dvl {

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
	MonstClassId mMonstClass;
	unsigned short mMagicRes;
	unsigned short mMagicRes2;
	unsigned short mTreasure;
	char mSelFlag;
	unsigned short mExp;
};

struct CMonster {
	MonsterType mtype = MonsterType::INVALID;
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
	const MonsterData *MData;
	// A TRN file contains a sequence of colour transitions, represented
	// as indexes into a palette. (a 256 byte array of palette indices)
	uint8_t *trans_file;
	AnimStruct &getAnim(MonstAnim anm) { return Anims[int(anm)]; }
	uint8_t * getAnim(MonstAnim anm, Dir dir) { return Anims[int(anm)].Data[int(dir)]; }
private:
	AnimStruct Anims[6];
};

struct MonsterStruct {  // note: missing field _mAFNum
	MonsterType _mMTidx;
	MonsterMode _mmode = MonsterMode::STAND; /* MON_MODE */
	MonsterGoal _mgoal = MonsterGoal::NORMAL;
	int _mgoalvar1 = 0;
	int _mgoalvar2 = 0;
	int _mgoalvar3 = 0;
	int field_18 = 0;
	unsigned char _pathcount = 0;
	V2Di _m;
	V2Di _mfut;
	V2Di _mold;
	V2Di _moff;
	V2Di _mvel;
	Dir _mdir = Dir::S;// Dir(random_(89, 8));
	ActorId _menemy = ActorId::getInvalid();//random_(89, gbActivePlayers);
	V2Di _menemypos;// = plr[Monst->_menemy].futpos();
	short falign_52;  // probably _mAFNum (unused)
	unsigned char *_mAnimData = 0;
	int _mAnimDelay = 0;
	int _mAnimCnt = 0;
	int _mAnimLen = 0;
	int _mAnimFrame = 0;
	bool _meflag;
	bool _mDelFlag = false;
	MonsterMode _mVar1_mmode = MonsterMode::STAND;
	MissileType _mVar1_mistype = MissileType::ARROW;
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
	uint8_t _msquelch = 0;
	int falign_A4;
	V2Di _last;
	int _mRndSeed;
	int _mAISeed;
	int falign_B8;
	UniqueMonsterType _uniqtype = UniqueMonsterType::INVALID; // will be UniqueMonsterType, delaying fix due to dual use of 0 entry for none and garbud
	unsigned char _uniqtrans;
	char _udeadval;
	std::set<ActorId> mWhoHit; // who hit the monster last
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
	int mtalkmsg = 0;
	ActorId leader = ActorId::getInvalid();
	unsigned char leaderflag = 0;
	unsigned char packsize;
	unsigned char mlid;
	std::string mName = "Invalid Monster";
	CMonster *MType;
	const MonsterData *MData;
};

struct UniqMonstStruct {
	MonsterType mtype;
	std::string mName;
	std::string mTrnName;
	unsigned char mlevel;
	unsigned short mmaxhp;
	MonstAi mAi;
	unsigned char mint;
	unsigned char mMinDamage;
	unsigned char mMaxDamage;
	MonsterResistFlags mMagicRes;
	unsigned short mUnqAttr;
	unsigned char mUnqVar1;
	unsigned char mUnqVar2;
	int mtalkmsg;
};


}

#endif /* __MONSTER_DEFS_H__ */
