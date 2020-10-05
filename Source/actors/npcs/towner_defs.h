// HEADER_GOES_HERE
#ifndef __TOWNER_ENUMS_H__
#define __TOWNER_ENUMS_H__

DEVILUTION_BEGIN_NAMESPACE


//////////////////////////////////////////////////
// towners
//////////////////////////////////////////////////

typedef struct TNQ {
	unsigned char _qsttype = 0;
	unsigned char _qstmsg = 0;
	bool _qstmsgact = 0;
} TNQ;

typedef struct TownerStruct {
	//int _tmode = 0;
	TownerId _ttype = TownerId::SMITH;
	V2Di _t;
	V2Di _toff;
	V2Di _tvel;
	//int _tdir = 0;
	unsigned char *_tAnimData = 0;
	int _tAnimDelay = 0;
	int _tAnimCnt = 0;
	int _tAnimLen = 0;
	int _tAnimFrame = 0;
	int _tAnimFrameCnt = 0;
	char _tAnimOrder = 0;
	int _tAnimWidth = 0;
	int _tAnimWidth2 = 0;
	int _tTenPer = 0;
	int _teflag = 0;
	int _tbtcnt = 0;
	int _tSelFlag = 0;
	bool _tMsgSaid = 0;
	TNQ qsts[MAXQUESTS];
	int _tSeed = 0;
	Player * _tVar1 = 0;
	int _tVar2 = 0;
	int _tVar3 = 0;
	int _tVar4 = 0;
	char _tName[PLR_NAME_LEN];
	unsigned char *_tNAnim[8];
	int _tNFrames = 0;
	unsigned char *_tNData = 0;
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


DEVILUTION_END_NAMESPACE

#endif /* __TOWNER_ENUMS_H__ */
