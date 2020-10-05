// HEADER_GOES_HERE
#ifndef __MISSILE_DEFS_H__
#define __MISSILE_DEFS_H__

DEVILUTION_BEGIN_NAMESPACE


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
	MissileType _mitype;
	V2Di _mi;
	V2Di _mioff;
	V2Di _mivel;
	V2Di _mis;
	V2Di _mitoff;
	Dir _mimfnum;  // Imf Num??? Imformation???
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
	ActorId _misource;
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


DEVILUTION_END_NAMESPACE

#endif /* __MISSILE_DEFS_H__ */
