// HEADER_GOES_HERE
#ifndef __OBJECT_DEFS_H__
#define __OBJECT_DEFS_H__

namespace dvl {


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
	ObjectType _otype;
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
	char _oBreak;  // check
	bool _oSolidFlag;
	bool _oMissFlag;
	char _oSelFlag;  // check
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


}

#endif /* __OBJECT_DEFS_H__ */
