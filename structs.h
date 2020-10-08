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
#include "items/item_enums.h"
#include "vectordvl.h"
#include "pathfinding/directions.h"
#include "pathfinding/pathfinder.h"

namespace dvl {

class Item;
class Player;

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
// textdat
//////////////////////////////////////////////////

typedef struct TextDataStruct {
	char *txtstr;
	int scrlltxt;
	int txtspd;
	int sfxnr;
} TextDataStruct;

//////////////////////////////////////////////////
// effects/sound
//////////////////////////////////////////////////

typedef struct TSnd {
	char *sound_path;
	SoundSample *DSB;
	int start_tc;
} TSnd;

typedef struct TSFX {
	uint8_t bFlags;
	char *pszName;
	TSnd *pSnd;
} TSFX;


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
	Cmd bCmd;
} TCmd;

typedef struct TCmdLoc {
	Cmd bCmd;
	uint8_t x;
	uint8_t y;
} TCmdLoc;

typedef struct TCmdLocParam1 {
	Cmd bCmd;
	uint8_t x;
	uint8_t y;
	WORD wParam1;
} TCmdLocParam1;

typedef struct TCmdLocParam2 {
	Cmd bCmd;
	uint8_t x;
	uint8_t y;
	WORD wParam1;
	WORD wParam2;
} TCmdLocParam2;

typedef struct TCmdLocParam3 {
	Cmd bCmd;
	uint8_t x;
	uint8_t y;
	WORD wParam1;
	WORD wParam2;
	WORD wParam3;
} TCmdLocParam3;

typedef struct TCmdParam1 {
	Cmd bCmd;
	WORD wParam1;
} TCmdParam1;

typedef struct TCmdParam2 {
	Cmd bCmd;
	WORD wParam1;
	WORD wParam2;
} TCmdParam2;

typedef struct TCmdParam3 {
	Cmd bCmd;
	WORD wParam1;
	WORD wParam2;
	WORD wParam3;
} TCmdParam3;

typedef struct TCmdGolem {
	Cmd bCmd;
	uint8_t _mx;
	uint8_t _my;
	uint8_t _mdir;
	char _menemy;
	int _mhitpoints;
	uint8_t currlevel;
} TCmdGolem;

typedef struct TCmdQuest {
	Cmd bCmd;
	uint8_t q;
	uint8_t qstate;
	uint8_t qlog;
	uint8_t qvar1;
} TCmdQuest;

typedef struct TCmdGItem {
	Cmd bCmd;
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
	Cmd bCmd;
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
	Cmd bCmd;
	uint8_t bLoc;
	WORD wIndx;
	WORD wCI;
	int dwSeed;
	bool bId;
} TCmdChItem;

typedef struct TCmdDelItem {
	Cmd bCmd;
	uint8_t bLoc;
} TCmdDelItem;

typedef struct TCmdDamage {
	Cmd bCmd;
	uint8_t bPlr;
	uint32_t dwDam;
} TCmdDamage;

typedef struct TCmdPlrInfoHdr {
	Cmd bCmd;
	WORD wOffset;
	WORD wBytes;
} TCmdPlrInfoHdr;

typedef struct TCmdString {
	Cmd bCmd;
	char str[MAX_SEND_STR_LEN];
} TCmdString;

typedef struct TFakeCmdPlr {
	Cmd bCmd;
	uint8_t bPlr;
} TFakeCmdPlr;

typedef struct TFakeDropPlr {
	Cmd bCmd;
	uint8_t bPlr;
	uint32_t dwReason;
} TFakeDropPlr;

typedef struct TSyncHeader {
	Cmd bCmd;
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
	Cmd bCmd;
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
	SetLvl _qslvl;
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
	SetLvl _qslvl;
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
	PkItemStruct InvBody[MAXINVITEMS];
	PkItemStruct InvList[MAXINVITEMS];
	char InvGrid[MAXINVITEMS];
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

}

#endif /* __STRUCTS_H__ */
