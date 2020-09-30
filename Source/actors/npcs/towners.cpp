#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

bool storeflag;
int sgnCowMsg;
int numtowners;
DWORD sgdwCowClicks;
/** unused 0x6AAC28 */
bool bannerflag;
bool boyloadflag;
uint8_t *pCowCels;
TownerStruct towner[NUM_TOWNERS];

const int snSFX[3][NUM_CLASSES] = {
	{ PS_WARR52, PS_ROGUE52, PS_MAGE52 },
	{ PS_WARR49, PS_ROGUE49, PS_MAGE49 },
	{ PS_WARR50, PS_ROGUE50, PS_MAGE50 },
};

/* data */

char AnimOrder[6][148] = {
	{ 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
	    14, 13, 12, 11, 10, 9, 8, 7, 6, 5,
	    5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
	    14, 13, 12, 11, 10, 9, 8, 7, 6, 5,
	    5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
	    14, 13, 12, 11, 10, 9, 8, 7, 6, 5,
	    5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
	    14, 13, 12, 11, 10, 9, 8, 7, 6, 5,
	    5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
	    14, 13, 12, 11, 10, 9, 8, 7, 6, 5,
	    5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
	    15, 5, 1, 1, 1, 1, 1, 1, 1, 1,
	    1, 1, 1, 1, 1, 1, 1, 2, 3, 4,
	    -1 },
	{ 1, 2, 3, 3, 2, 1, 20, 19, 19, 20,
	    1, 2, 3, 3, 2, 1, 20, 19, 19, 20,
	    1, 2, 3, 3, 2, 1, 20, 19, 19, 20,
	    1, 2, 3, 3, 2, 1, 20, 19, 19, 20,
	    1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
	    11, 12, 13, 14, 15, 16, 15, 14, 13, 12,
	    11, 10, 9, 8, 7, 6, 5, 4, 5, 6,
	    7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
	    15, 14, 13, 12, 11, 10, 9, 8, 7, 6,
	    5, 4, 5, 6, 7, 8, 9, 10, 11, 12,
	    13, 14, 15, 16, 17, 18, 19, 20, -1 },
	{ 1, 1, 25, 25, 24, 23, 22, 21, 20, 19,
	    18, 17, 16, 15, 16, 17, 18, 19, 20, 21,
	    22, 23, 24, 25, 25, 25, 1, 1, 1, 25,
	    1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
	    11, 12, 13, 14, 15, 14, 13, 12, 11, 10,
	    9, 8, 7, 6, 5, 4, 3, 2, 1, -1 },
	{ 1, 2, 3, 3, 2, 1, 16, 15, 14, 14,
	    16, 1, 2, 3, 3, 2, 1, 16, 15, 14,
	    14, 15, 16, 1, 2, 3, 3, 2, 1, 16,
	    15, 14, 14, 15, 16, 1, 2, 3, 3, 2,
	    1, 16, 15, 14, 14, 15, 16, 1, 2, 3,
	    3, 2, 1, 16, 15, 14, 14, 15, 16, 1,
	    2, 3, 3, 2, 1, 16, 15, 14, 14, 15,
	    16, 1, 2, 3, 3, 2, 1, 16, 15, 14,
	    14, 15, 16, 1, 2, 3, 2, 1, 16, 15,
	    14, 14, 15, 16, 1, 2, 3, 4, 5, 6,
	    7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
	    -1 },
	{ 1, 1, 1, 2, 3, 4, 5, 6, 7, 8,
	    9, 10, 11, 11, 11, 11, 12, 13, 14, 15,
	    16, 17, 18, 18, 1, 1, 1, 18, 17, 16,
	    15, 14, 13, 12, 11, 10, 11, 12, 13, 14,
	    15, 16, 17, 18, 1, 2, 3, 4, 5, 5,
	    5, 4, 3, 2, -1 },
	{ 4, 4, 4, 5, 6, 6, 6, 5, 4, 15,
	    14, 13, 13, 13, 14, 15, 4, 5, 6, 6,
	    6, 5, 4, 4, 4, 5, 6, 6, 6, 5,
	    4, 15, 14, 13, 13, 13, 14, 15, 4, 5,
	    6, 6, 6, 5, 4, 4, 4, 5, 6, 6,
	    6, 5, 4, 15, 14, 13, 13, 13, 14, 15,
	    4, 5, 6, 6, 6, 5, 4, 3, 2, 1,
	    19, 18, 19, 1, 2, 1, 19, 18, 19, 1,
	    2, 1, 2, 3, 4, 5, 6, 7, 8, 9,
	    10, 11, 12, 13, 14, 15, 15, 15, 14, 13,
	    13, 13, 13, 14, 15, 15, 15, 14, 13, 12,
	    12, 12, 11, 10, 10, 10, 9, 8, 9, 10,
	    10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
	    1, 2, 1, 19, 18, 19, 1, 2, 1, 2,
	    3, -1 }
};
int TownCowX[] = { 58, 56, 59 };
int TownCowY[] = { 16, 14, 20 };
Dir TownCowDir[] = { Dir::SW, Dir::NW, Dir::N };
int cowoffx[8] = { -1, 0, -1, -1, -1, 0, -1, -1 };
int cowoffy[8] = { -1, -1, -1, 0, -1, -1, -1, 0 };
QuestTalkData Qtalklist[] = {
	// clang-format off
	// _qinfra,      _qblkm,       _qgarb,      _qzhar,      _qveil,      _qmod,       _qbutch,      _qbol,         _qblind,      _qblood,      _qanvil,      _qwarlrd,      _qking,       _qpw,           _qbone,      _qvb
	{ TEXT_INFRA6,  TEXT_MUSH6,  -1,          -1,          TEXT_VEIL5, -1,          TEXT_BUTCH5, TEXT_BANNER6, TEXT_BLIND5, TEXT_BLOOD5, TEXT_ANVIL6, TEXT_WARLRD5, TEXT_KING7,  TEXT_POISON7,  TEXT_BONE5, TEXT_VILE9  },
	{ TEXT_INFRA3,  -1,           -1,          -1,          TEXT_VEIL3, -1,          TEXT_BUTCH3, TEXT_BANNER4, TEXT_BLIND3, TEXT_BLOOD3, TEXT_ANVIL3, TEXT_WARLRD3, TEXT_KING5,  TEXT_POISON4,  TEXT_BONE3, TEXT_VILE7  },
	{ -1,            -1,           -1,          -1,          -1,          -1,          -1,           -1,            -1,           -1,           -1,           -1,            -1,           -1,             -1,          -1           },
	{ TEXT_INFRA2,  TEXT_MUSH2,  -1,          -1,          TEXT_VEIL2, -1,          TEXT_BUTCH2, -1,            TEXT_BLIND2, TEXT_BLOOD2, TEXT_ANVIL2, TEXT_WARLRD2, TEXT_KING3,  TEXT_POISON2,  TEXT_BONE2, TEXT_VILE4  },
	{ TEXT_INFRA1,  TEXT_MUSH1,  -1,          -1,          TEXT_VEIL1, TEXT_VILE3, TEXT_BUTCH1, TEXT_BANNER1, TEXT_BLIND1, TEXT_BLOOD1, TEXT_ANVIL1, TEXT_WARLRD1, TEXT_KING1,  TEXT_POISON1,  TEXT_BONE1, TEXT_VILE2  },
	{ TEXT_INFRA8,  TEXT_MUSH7,  -1,          -1,          TEXT_VEIL6, -1,          TEXT_BUTCH6, TEXT_BANNER7, TEXT_BLIND6, TEXT_BLOOD6, TEXT_ANVIL8, TEXT_WARLRD6, TEXT_KING8,  TEXT_POISON8,  TEXT_BONE6, TEXT_VILE10 },
	{ TEXT_INFRA9,  TEXT_MUSH9,  -1,          -1,          TEXT_VEIL7, -1,          TEXT_BUTCH7, TEXT_BANNER8, TEXT_BLIND7, TEXT_BLOOD7, TEXT_ANVIL9, TEXT_WARLRD7, TEXT_KING9,  TEXT_POISON9,  TEXT_BONE7, TEXT_VILE11 },
	{ TEXT_INFRA4,  TEXT_MUSH5,  -1,          -1,          TEXT_VEIL4, -1,          TEXT_BUTCH4, TEXT_BANNER5, TEXT_BLIND4, TEXT_BLOOD4, TEXT_ANVIL4, TEXT_WARLRD4, TEXT_KING6,  TEXT_POISON6,  TEXT_BONE4, TEXT_VILE8  },
	{ TEXT_INFRA10, TEXT_MUSH13, -1,          -1,          TEXT_VEIL8, -1,          TEXT_BUTCH8, TEXT_BANNER9, TEXT_BLIND8, TEXT_BLOOD8, TEXT_ANVIL10,TEXT_WARLRD8, TEXT_KING10, TEXT_POISON10, TEXT_BONE8, TEXT_VILE12 },
	{ -1,            -1,           -1,          -1,          -1,          -1,          -1,           -1,            -1,           -1,           -1,           -1,            -1,           -1,             -1,          -1           },
	{ TEXT_KING1,   TEXT_KING1,  TEXT_KING1, TEXT_KING1, TEXT_KING1, TEXT_KING1, TEXT_KING1,  TEXT_KING1,   TEXT_KING1,  TEXT_KING1,  TEXT_KING1,  TEXT_KING1,   TEXT_KING1,  TEXT_KING1,    TEXT_KING1, TEXT_KING1  }
	// clang-format on
};
int CowPlaying = -1;

int GetActiveTowner(int t)
{
	int i;

	for (i = 0; i < numtowners; i++) {
		if (towner[i]._ttype == t)
			return i;
	}

	return -1;
}

void SetTownerGPtrs(uint8_t *pData, uint8_t **pAnim)
{
	int i;
	DWORD *pFrameTable;

	pFrameTable = (DWORD *)pData;

	for (i = 0; i < 8; i++) {
		pAnim[i] = CelGetFrameStart(pData, i);
	}
}

void NewTownerAnim(int tnum, uint8_t *pAnim, int numFrames, int Delay)
{
	towner[tnum]._tAnimData = pAnim;
	towner[tnum]._tAnimLen = numFrames;
	towner[tnum]._tAnimFrame = 1;
	towner[tnum]._tAnimCnt = 0;
	towner[tnum]._tAnimDelay = Delay;
}

void InitTownerInfo(int i, int w, int sel, int t, int x, int y, int ao, int tp)
{
	memset(&towner[i], 0, sizeof(TownerStruct));
	towner[i]._tSelFlag = sel;
	towner[i]._tAnimWidth = w;
	towner[i]._tAnimWidth2 = (w - 64) >> 1;
	towner[i]._tMsgSaid = false;
	towner[i]._ttype = t;
	towner[i]._t = { x, y };
	grid[x][y].setMonster(i);
	towner[i]._tAnimOrder = ao;
	towner[i]._tTenPer = tp;
	towner[i]._tSeed = GetRndSeed();
}

void InitQstSnds(int i)
{
	int j, tl;
	tl = i;
	if (boyloadflag)
		tl++;
	for (j = 0; j < MAXQUESTS; j++) {
		towner[i].qsts[j]._qsttype = quests[j]._qtype;
		towner[i].qsts[j]._qstmsg = ((int *)(Qtalklist + tl))[j];
		if (((int *)(Qtalklist + tl))[j] != -1)
			towner[i].qsts[j]._qstmsgact = TRUE;
		else
			towner[i].qsts[j]._qstmsgact = false;
	}
}

void InitSmith()
{
	int i;

	InitTownerInfo(numtowners, 96, TRUE, TOWN_SMITH, 62, 63, 0, 10);
	InitQstSnds(numtowners);
	towner[numtowners]._tNData = LoadFileInMem("Towners\\Smith\\SmithN.CEL", NULL);
	for (i = 0; i < 8; i++) {
		towner[numtowners]._tNAnim[i] = towner[numtowners]._tNData;
	}
	towner[numtowners]._tNFrames = 16;
	NewTownerAnim(numtowners, towner[numtowners]._tNAnim[int(Dir::SW)], towner[numtowners]._tNFrames, 3);
	strcpy(towner[numtowners]._tName, "Griswold the Blacksmith");
	numtowners++;
}

void InitBarOwner()
{
	int i;

	bannerflag = false; // unused
	InitTownerInfo(numtowners, 96, TRUE, TOWN_TAVERN, 55, 62, 3, 10);
	InitQstSnds(numtowners);
	towner[numtowners]._tNData = LoadFileInMem("Towners\\TwnF\\TwnFN.CEL", NULL);
	for (i = 0; i < 8; i++) {
		towner[numtowners]._tNAnim[i] = towner[numtowners]._tNData;
	}
	towner[numtowners]._tNFrames = 16;
	NewTownerAnim(numtowners, towner[numtowners]._tNAnim[int(Dir::SW)], towner[numtowners]._tNFrames, 3);
	strcpy(towner[numtowners]._tName, "Ogden the Tavern owner");
	numtowners++;
}

void InitTownDead()
{
	int i;

	InitTownerInfo(numtowners, 96, TRUE, TOWN_DEADGUY, 24, 32, -1, 10);
	InitQstSnds(numtowners);
	towner[numtowners]._tNData = LoadFileInMem("Towners\\Butch\\Deadguy.CEL", NULL);
	for (i = 0; i < 8; i++) {
		towner[numtowners]._tNAnim[i] = towner[numtowners]._tNData;
	}
	towner[numtowners]._tNFrames = 8;
	NewTownerAnim(numtowners, towner[numtowners]._tNAnim[int(Dir::N)], towner[numtowners]._tNFrames, 6);
	strcpy(towner[numtowners]._tName, "Wounded Townsman");
	numtowners++;
}

void InitWitch()
{
	int i;

	InitTownerInfo(numtowners, 96, TRUE, TOWN_WITCH, 80, 20, 5, 10);
	InitQstSnds(numtowners);
	towner[numtowners]._tNData = LoadFileInMem("Towners\\TownWmn1\\Witch.CEL", NULL);
	for (i = 0; i < 8; i++) {
		towner[numtowners]._tNAnim[i] = towner[numtowners]._tNData;
	}
	towner[numtowners]._tNFrames = 19;
	NewTownerAnim(numtowners, towner[numtowners]._tNAnim[int(Dir::S)], towner[numtowners]._tNFrames, 6);
	strcpy(towner[numtowners]._tName, "Adria the Witch");
	numtowners++;
}

void InitBarmaid()
{
	int i;

	InitTownerInfo(numtowners, 96, TRUE, TOWN_BMAID, 43, 66, -1, 10);
	InitQstSnds(numtowners);
	towner[numtowners]._tNData = LoadFileInMem("Towners\\TownWmn1\\WmnN.CEL", NULL);
	for (i = 0; i < 8; i++) {
		towner[numtowners]._tNAnim[i] = towner[numtowners]._tNData;
	}
	towner[numtowners]._tNFrames = 18;
	NewTownerAnim(numtowners, towner[numtowners]._tNAnim[int(Dir::S)], towner[numtowners]._tNFrames, 6);
	strcpy(towner[numtowners]._tName, "Gillian the Barmaid");
	numtowners++;
}

void InitBoy()
{
	int i;

	boyloadflag = TRUE;
	InitTownerInfo(numtowners, 96, TRUE, TOWN_PEGBOY, 11, 53, -1, 10);
	InitQstSnds(numtowners);
	towner[numtowners]._tNData = LoadFileInMem("Towners\\TownBoy\\PegKid1.CEL", NULL);
	for (i = 0; i < 8; i++) {
		towner[numtowners]._tNAnim[i] = towner[numtowners]._tNData;
	}
	towner[numtowners]._tNFrames = 20;
	NewTownerAnim(numtowners, towner[numtowners]._tNAnim[int(Dir::S)], towner[numtowners]._tNFrames, 6);
	strcpy(towner[numtowners]._tName, "Wirt the Peg-legged boy");
	numtowners++;
}

void InitHealer()
{
	int i;

	InitTownerInfo(numtowners, 96, TRUE, TOWN_HEALER, 55, 79, 1, 10);
	InitQstSnds(numtowners);
	towner[numtowners]._tNData = LoadFileInMem("Towners\\Healer\\Healer.CEL", NULL);
	for (i = 0; i < 8; i++) {
		towner[numtowners]._tNAnim[i] = towner[numtowners]._tNData;
	}
	towner[numtowners]._tNFrames = 20;
	NewTownerAnim(numtowners, towner[numtowners]._tNAnim[int(Dir::SE)], towner[numtowners]._tNFrames, 6);
	strcpy(towner[numtowners]._tName, "Pepin the Healer");
	numtowners++;
}

void InitTeller()
{
	int i;

	InitTownerInfo(numtowners, 96, TRUE, TOWN_STORY, 62, 71, 2, 10);
	InitQstSnds(numtowners);
	towner[numtowners]._tNData = LoadFileInMem("Towners\\Strytell\\Strytell.CEL", NULL);
	for (i = 0; i < 8; i++) {
		towner[numtowners]._tNAnim[i] = towner[numtowners]._tNData;
	}
	towner[numtowners]._tNFrames = 25;
	NewTownerAnim(numtowners, towner[numtowners]._tNAnim[int(Dir::S)], towner[numtowners]._tNFrames, 3);
	strcpy(towner[numtowners]._tName, "Cain the Elder");
	numtowners++;
}

void InitDrunk()
{
	int i;

	InitTownerInfo(numtowners, 96, TRUE, TOWN_DRUNK, 71, 84, 4, 10);
	InitQstSnds(numtowners);
	towner[numtowners]._tNData = LoadFileInMem("Towners\\Drunk\\TwnDrunk.CEL", NULL);
	for (i = 0; i < 8; i++) {
		towner[numtowners]._tNAnim[i] = towner[numtowners]._tNData;
	}
	towner[numtowners]._tNFrames = 18;
	NewTownerAnim(numtowners, towner[numtowners]._tNAnim[int(Dir::S)], towner[numtowners]._tNFrames, 3);
	strcpy(towner[numtowners]._tName, "Farnham the Drunk");
	numtowners++;
}

void InitCows()
{
	Dir dir;
	int i;
	int x, y, xo, yo;

	//if ( pCowCels )
	// assertion_failed(__LINE__, __FILE__, "! pCowCels");
	pCowCels = LoadFileInMem("Towners\\Animals\\Cow.CEL", NULL);
	for (i = 0; i < 3; i++) {
		x = TownCowX[i];
		y = TownCowY[i];
		dir = TownCowDir[i];
		InitTownerInfo(numtowners, 128, false, TOWN_COW, x, y, -1, 10);
		towner[numtowners]._tNData = pCowCels;
		SetTownerGPtrs(towner[numtowners]._tNData, towner[numtowners]._tNAnim);
		towner[numtowners]._tNFrames = 12;
		NewTownerAnim(numtowners, towner[numtowners]._tNAnim[int(dir)], towner[numtowners]._tNFrames, 3);
		towner[numtowners]._tAnimFrame = random_(0, 11) + 1;
		towner[numtowners]._tSelFlag = TRUE;
		strcpy(towner[numtowners]._tName, "Cow");

		xo = x + cowoffx[int(dir)];
		yo = y + cowoffy[int(dir)];
		if (!grid[x][yo].isMonster())
			grid[x][yo].setMonster(numtowners);
		if (!grid[x][yo].isMonster())
			grid[xo][y].setMonster(numtowners);
		if (!grid[x][yo].isMonster())
			grid[xo][yo].setMonster(numtowners);

		numtowners++;
	}
}

void InitTowners()
{
	numtowners = 0;
	boyloadflag = false;
	InitSmith();
	InitHealer();
	if (quests[Q_BUTCHER]._qactive != QUEST_NOTAVAIL && quests[Q_BUTCHER]._qactive != QUEST_DONE)
		InitTownDead();
	InitBarOwner();
	InitTeller();
	InitDrunk();
	InitWitch();
	InitBarmaid();
	InitBoy();
	InitCows();
}

void FreeTownerGFX()
{
	for (int i = 0; i < NUM_TOWNERS; i++) {
		if (towner[i]._tNData == pCowCels) {
			towner[i]._tNData = NULL;
		} else if (towner[i]._tNData) {
			MemFreeDbg(towner[i]._tNData);
		}
	}
	MemFreeDbg(pCowCels);
}

void TownCtrlMsg(int i)
{
	if (towner[i]._tbtcnt != 0) {
		int p = towner[i]._tVar1;
		int dist = (towner[i]._t - plr[p].pos()).maxabs();
		if (dist >= 2)
			towner[i]._tbtcnt = 0;
		if (!towner[i]._tbtcnt) {
			qtextflag = false;
			stream_stop();
		}
	}
}

void TownBlackSmith()
{
	int i = GetActiveTowner(TOWN_SMITH);
	TownCtrlMsg(i);
}

void TownBarOwner()
{
	int i = GetActiveTowner(TOWN_TAVERN);
	TownCtrlMsg(i);
}

void TownDead()
{
	int tidx = GetActiveTowner(TOWN_DEADGUY);
	TownCtrlMsg(tidx);
	if (!qtextflag) {
		if (quests[Q_BUTCHER]._qactive == QUEST_ACTIVE && quests[Q_BUTCHER]._qlog == 0) {
			return;
		}
		if (quests[Q_BUTCHER]._qactive != QUEST_INIT) {
			towner[tidx]._tAnimDelay = 1000;
			towner[tidx]._tAnimFrame = 1;
			strcpy(towner[tidx]._tName, "Slain Townsman");
		}
	}
	if (quests[Q_BUTCHER]._qactive != QUEST_INIT)
		towner[tidx]._tAnimCnt = 0;
}

void TownHealer()
{
	int i = GetActiveTowner(TOWN_HEALER);
	TownCtrlMsg(i);
}

void TownStory()
{
	int i = GetActiveTowner(TOWN_STORY);
	TownCtrlMsg(i);
}

void TownDrunk()
{
	int i;

	i = GetActiveTowner(TOWN_DRUNK);
	TownCtrlMsg(i);
}

void TownBoy()
{
	int i;

	i = GetActiveTowner(TOWN_PEGBOY);
	TownCtrlMsg(i);
}

void TownWitch()
{
	int i;

	i = GetActiveTowner(TOWN_WITCH);
	TownCtrlMsg(i);
}

void TownBarMaid()
{
	int i;

	i = GetActiveTowner(TOWN_BMAID);
	TownCtrlMsg(i);
}

void TownCow()
{
	int i;

	i = GetActiveTowner(TOWN_COW);
	TownCtrlMsg(i);
}

void ProcessTowners()
{
	int i, ao;

	for (i = 0; i < NUM_TOWNERS; i++) {
		switch (towner[i]._ttype) {
		case TOWN_SMITH:
			TownBlackSmith();
			break;
		case TOWN_HEALER:
			TownHealer();
			break;
		case TOWN_DEADGUY:
			TownDead();
			break;
		case TOWN_TAVERN:
			TownBarOwner();
			break;
		case TOWN_STORY:
			TownStory();
			break;
		case TOWN_DRUNK:
			TownDrunk();
			break;
		case TOWN_WITCH:
			TownWitch();
			break;
		case TOWN_BMAID:
			TownBarMaid();
			break;
		case TOWN_PEGBOY:
			TownBoy();
			break;
		case TOWN_COW:
			TownCow();
			break;
		}

		towner[i]._tAnimCnt++;
		if (towner[i]._tAnimCnt >= towner[i]._tAnimDelay) {
			towner[i]._tAnimCnt = 0;

			if (towner[i]._tAnimOrder >= 0) {
				ao = towner[i]._tAnimOrder;
				towner[i]._tAnimFrameCnt++;
				if (AnimOrder[ao][towner[i]._tAnimFrameCnt] == -1)
					towner[i]._tAnimFrameCnt = 0;

				towner[i]._tAnimFrame = AnimOrder[ao][towner[i]._tAnimFrameCnt];
			} else {
				towner[i]._tAnimFrame++;
				if (towner[i]._tAnimFrame > towner[i]._tAnimLen)
					towner[i]._tAnimFrame = 1;
			}
		}
	}
}

ItemStruct *PlrHasItem(int pnum, int item, int *i)
{
	for (*i = 0; *i < plr[pnum].data._pNumInv; (*i)++) {
		if (plr[pnum].data.InvList[*i].IDidx == item)
			return &plr[pnum].data.InvList[*i];
	}

	return NULL;
}

void TownerTalk(int first, int t)
{
	sgdwCowClicks = 0;
	sgnCowMsg = 0;
	storeflag = TRUE;
	InitQTextMsg(first);
}

void TalkToTowner(int p, int t)
{
	int i, dx, dy, rv1, rv2, rv3;
	ItemStruct *Item;

	rv1 = random_(6, 3); /* unused */
	rv2 = random_(6, 4); /* unused */
	rv3 = random_(6, 5); /* unused */
	int dist = (plr[p].pos() - towner[t]._t).maxabs();
	#ifdef _DEBUG
	if (!debug_mode_key_d && (dist >= 2)) {
		return;
	}
	#else
	if (dist >= 2) {
		return;
	}
	#endif

	if (qtextflag) {
		return;
	}

	towner[t]._tMsgSaid = false;

	if (pcurs >= CURSOR_FIRSTITEM && !DropItemBeforeTrig()) {
		return;
	}

	if (t == GetActiveTowner(TOWN_TAVERN)) {
		if (!plr[p].data._pLvlVisited[0] && !towner[t]._tMsgSaid) {
			towner[t]._tbtcnt = 150;
			towner[t]._tVar1 = p;
			InitQTextMsg(TEXT_INTRO);
			towner[t]._tMsgSaid = TRUE;
		}
		if ((plr[p].data._pLvlVisited[2] || plr[p].data._pLvlVisited[4]) && quests[Q_SKELKING]._qactive != QUEST_NOTAVAIL) {
			if (quests[Q_SKELKING]._qvar2 == 0 && !towner[t]._tMsgSaid) {
				quests[Q_SKELKING]._qvar2 = 1;
				quests[Q_SKELKING]._qlog = TRUE;
				if (quests[Q_SKELKING]._qactive == QUEST_INIT) {
					quests[Q_SKELKING]._qactive = QUEST_ACTIVE;
					quests[Q_SKELKING]._qvar1 = 1;
				}
				towner[t]._tbtcnt = 150;
				towner[t]._tVar1 = p;
				InitQTextMsg(TEXT_KING2);
				towner[t]._tMsgSaid = TRUE;
				NetSendCmdQuest(TRUE, Q_SKELKING);
			}
			if (quests[Q_SKELKING]._qactive == QUEST_DONE && quests[Q_SKELKING]._qvar2 == 1 && !towner[t]._tMsgSaid) {
				quests[Q_SKELKING]._qvar2 = 2;
				quests[Q_SKELKING]._qvar1 = 2;
				towner[t]._tbtcnt = 150;
				towner[t]._tVar1 = p;
				InitQTextMsg(TEXT_KING4);
				towner[t]._tMsgSaid = TRUE;
				NetSendCmdQuest(TRUE, Q_SKELKING);
			}
		}
		if (plr.isSingleplayer() && plr[p].data._pLvlVisited[3] && quests[Q_LTBANNER]._qactive != QUEST_NOTAVAIL) {
			if ((quests[Q_LTBANNER]._qactive == QUEST_INIT || quests[Q_LTBANNER]._qactive == QUEST_ACTIVE) && quests[Q_LTBANNER]._qvar2 == 0 && !towner[t]._tMsgSaid) {
				quests[Q_LTBANNER]._qvar2 = 1;
				if (quests[Q_LTBANNER]._qactive == QUEST_INIT) {
					quests[Q_LTBANNER]._qvar1 = 1;
					quests[Q_LTBANNER]._qactive = QUEST_ACTIVE;
				}
				quests[Q_LTBANNER]._qlog = TRUE;
				towner[t]._tbtcnt = 150;
				towner[t]._tVar1 = p;
				InitQTextMsg(TEXT_BANNER2);
				towner[t]._tMsgSaid = TRUE;
			}
			if (quests[Q_LTBANNER]._qvar2 == 1 && PlrHasItem(p, IDI_BANNER, &i) != NULL) {
				if (!towner[t]._tMsgSaid) {
					quests[Q_LTBANNER]._qactive = QUEST_DONE;
					quests[Q_LTBANNER]._qvar1 = 3;
					plr[p].inventory.RemoveInvItem(i);
					CreateItem(UITEM_HARCREST, { towner[t]._t.x, towner[t]._t.y + 1 });
					towner[t]._tbtcnt = 150;
					towner[t]._tVar1 = p;
					InitQTextMsg(TEXT_BANNER3);
					towner[t]._tMsgSaid = TRUE;
				}
			}
		}
		if (!qtextflag) {
			TownerTalk(TEXT_OGDEN1, t);
			if (storeflag) {
				StartStore(STORE_TAVERN);
			}
		}
	} else if (t == GetActiveTowner(TOWN_DEADGUY)) {
		if (quests[Q_BUTCHER]._qactive == QUEST_ACTIVE && quests[Q_BUTCHER]._qvar1 == 1) {
			towner[t]._tbtcnt = 150;
			towner[t]._tVar1 = p;
			quests[Q_BUTCHER]._qvar1 = 1;
			if (plr[p].data._pClass == PC_WARRIOR && !effect_is_playing(PS_WARR8)) {
				PlaySFX(PS_WARR8);
			} else if (plr[p].data._pClass == PC_ROGUE && !effect_is_playing(PS_ROGUE8)) {
				PlaySFX(PS_ROGUE8);
			} else if (plr[p].data._pClass == PC_SORCERER && !effect_is_playing(PS_MAGE8)) {
				PlaySFX(PS_MAGE8);
			}
			towner[t]._tMsgSaid = TRUE;
		} else if (quests[Q_BUTCHER]._qactive == QUEST_DONE && quests[Q_BUTCHER]._qvar1 == 1) {
			quests[Q_BUTCHER]._qvar1 = 1;
			towner[t]._tbtcnt = 150;
			towner[t]._tVar1 = p;
			towner[t]._tMsgSaid = TRUE;
		} else if (quests[Q_BUTCHER]._qactive == QUEST_INIT || quests[Q_BUTCHER]._qactive == QUEST_ACTIVE && quests[Q_BUTCHER]._qvar1 == 0) {
			quests[Q_BUTCHER]._qactive = QUEST_ACTIVE;
			quests[Q_BUTCHER]._qlog = TRUE;
			quests[Q_BUTCHER]._qmsg = TEXT_BUTCH9;
			quests[Q_BUTCHER]._qvar1 = 1;
			towner[t]._tbtcnt = 50;
			towner[t]._tVar1 = p;
			towner[t]._tVar2 = 3;
			InitQTextMsg(TEXT_BUTCH9);
			towner[t]._tMsgSaid = TRUE;
			NetSendCmdQuest(TRUE, Q_BUTCHER);
		}
	} else if (t == GetActiveTowner(TOWN_SMITH)) {
		if (plr.isSingleplayer()) {
			if (plr[p].data._pLvlVisited[4] && quests[Q_ROCK]._qactive != QUEST_NOTAVAIL) {
				if (quests[Q_ROCK]._qvar2 == 0) {
					quests[Q_ROCK]._qvar2 = 1;
					quests[Q_ROCK]._qlog = TRUE;
					if (quests[Q_ROCK]._qactive == QUEST_INIT) {
						quests[Q_ROCK]._qactive = QUEST_ACTIVE;
						quests[Q_ROCK]._qvar1 = 1;
					}
					towner[t]._tbtcnt = 150;
					towner[t]._tVar1 = p;
					InitQTextMsg(TEXT_INFRA5);
					towner[t]._tMsgSaid = TRUE;
				}
				if (quests[Q_ROCK]._qvar2 == 1 && PlrHasItem(p, IDI_ROCK, &i) != NULL) {
					if (!towner[t]._tMsgSaid) {
						quests[Q_ROCK]._qactive = QUEST_DONE;
						quests[Q_ROCK]._qvar2 = 2;
						quests[Q_ROCK]._qvar1 = 2;
						plr[p].inventory.RemoveInvItem(i);
						CreateItem(UITEM_INFRARING, { towner[t]._t.x, towner[t]._t.y + 1 });
						towner[t]._tbtcnt = 150;
						towner[t]._tVar1 = p;
						InitQTextMsg(TEXT_INFRA7);
						towner[t]._tMsgSaid = TRUE;
					}
				}
			}
			if (plr[p].data._pLvlVisited[9] && quests[Q_ANVIL]._qactive != QUEST_NOTAVAIL) {
				if ((quests[Q_ANVIL]._qactive == QUEST_INIT || quests[Q_ANVIL]._qactive == QUEST_ACTIVE) && quests[Q_ANVIL]._qvar2 == 0 && !towner[t]._tMsgSaid) {
					if (quests[Q_ROCK]._qvar2 == 2 || quests[Q_ROCK]._qactive == QUEST_ACTIVE && quests[Q_ROCK]._qvar2 == 1) {
						quests[Q_ANVIL]._qvar2 = 1;
						quests[Q_ANVIL]._qlog = TRUE;
						if (quests[Q_ANVIL]._qactive == QUEST_INIT) {
							quests[Q_ANVIL]._qactive = QUEST_ACTIVE;
							quests[Q_ANVIL]._qvar1 = 1;
						}
						towner[t]._tbtcnt = 150;
						towner[t]._tVar1 = p;
						InitQTextMsg(TEXT_ANVIL5);
						towner[t]._tMsgSaid = TRUE;
					}
				}
				if (quests[Q_ANVIL]._qvar2 == 1 && PlrHasItem(p, IDI_ANVIL, &i) != NULL) {
					if (!towner[t]._tMsgSaid) {
						quests[Q_ANVIL]._qactive = QUEST_DONE;
						quests[Q_ANVIL]._qvar2 = 2;
						quests[Q_ANVIL]._qvar1 = 2;
						plr[p].inventory.RemoveInvItem(i);
						CreateItem(UITEM_GRISWOLD, { towner[t]._t.x, towner[t]._t.y + 1 });
						towner[t]._tbtcnt = 150;
						towner[t]._tVar1 = p;
						InitQTextMsg(TEXT_ANVIL7);
						towner[t]._tMsgSaid = TRUE;
					}
				}
			}
		}
		if (!qtextflag) {
			TownerTalk(TEXT_GRISWOLD1, t);
			if (storeflag) {
				StartStore(STORE_SMITH);
			}
		}
	} else if (t == GetActiveTowner(TOWN_WITCH)) {
		if (quests[Q_MUSHROOM]._qactive == QUEST_INIT && PlrHasItem(p, IDI_FUNGALTM, &i) != NULL) {
			plr[p].inventory.RemoveInvItem(i);
			quests[Q_MUSHROOM]._qactive = QUEST_ACTIVE;
			quests[Q_MUSHROOM]._qlog = TRUE;
			quests[Q_MUSHROOM]._qvar1 = QS_TOMEGIVEN;
			towner[t]._tbtcnt = 150;
			towner[t]._tVar1 = p;
			InitQTextMsg(TEXT_MUSH8);
			towner[t]._tMsgSaid = TRUE;
		} else if (quests[Q_MUSHROOM]._qactive == QUEST_ACTIVE) {
			if (quests[Q_MUSHROOM]._qvar1 >= QS_TOMEGIVEN && quests[Q_MUSHROOM]._qvar1 <= QS_MUSHPICKED) {
				if (PlrHasItem(p, IDI_MUSHROOM, &i) != NULL) {
					plr[p].inventory.RemoveInvItem(i);
					quests[Q_MUSHROOM]._qvar1 = 5;
					Qtalklist[TOWN_HEALER]._qblkm = TEXT_MUSH3;
					Qtalklist[TOWN_WITCH]._qblkm = -1;
					towner[t]._tbtcnt = 150;
					towner[t]._tVar1 = p;
					quests[Q_MUSHROOM]._qmsg = TEXT_MUSH10;
					InitQTextMsg(TEXT_MUSH10);
					towner[t]._tMsgSaid = TRUE;
				} else if (quests[Q_MUSHROOM]._qmsg != TEXT_MUSH9) {
					towner[t]._tbtcnt = 150;
					towner[t]._tVar1 = p;
					quests[Q_MUSHROOM]._qmsg = TEXT_MUSH9;
					InitQTextMsg(TEXT_MUSH9);
					towner[t]._tMsgSaid = TRUE;
				}
			} else {
				Item = PlrHasItem(p, IDI_SPECELIX, &i);
				if (Item != NULL) {
					towner[t]._tbtcnt = 150;
					towner[t]._tVar1 = p;
					InitQTextMsg(TEXT_MUSH12);
					quests[Q_MUSHROOM]._qactive = QUEST_DONE;
					towner[t]._tMsgSaid = TRUE;
					AllItemsList[Item->IDidx].iUsable = TRUE;
				} else if (PlrHasItem(p, IDI_BRAIN, &i) != NULL && quests[Q_MUSHROOM]._qvar2 != TEXT_MUSH11) {
					towner[t]._tbtcnt = 150;
					towner[t]._tVar1 = p;
					quests[Q_MUSHROOM]._qvar2 = TEXT_MUSH11;
					InitQTextMsg(TEXT_MUSH11);
					towner[t]._tMsgSaid = TRUE;
				}
			}
		}
		if (!qtextflag) {
			TownerTalk(TEXT_ADRIA1, t);
			if (storeflag) {
				StartStore(STORE_WITCH);
			}
		}
	} else if (t == GetActiveTowner(TOWN_BMAID)) {
		if (!qtextflag) {
			TownerTalk(TEXT_GILLIAN1, t);
			if (storeflag) {
				StartStore(STORE_BARMAID);
			}
		}
	} else if (t == GetActiveTowner(TOWN_DRUNK)) {
		if (!qtextflag) {
			TownerTalk(TEXT_FARNHAM1, t);
			if (storeflag) {
				StartStore(STORE_DRUNK);
			}
		}
	} else if (t == GetActiveTowner(TOWN_HEALER)) {
		if (plr.isSingleplayer()) {
			if (plr[p].data._pLvlVisited[1] && !towner[t]._tMsgSaid) {
				if (quests[Q_PWATER]._qactive == QUEST_INIT) {
					quests[Q_PWATER]._qactive = QUEST_ACTIVE;
					quests[Q_PWATER]._qlog = TRUE;
					quests[Q_PWATER]._qmsg = TEXT_POISON3;
					quests[Q_PWATER]._qvar1 = 1;
					towner[t]._tbtcnt = 150;
					towner[t]._tVar1 = p;
					InitQTextMsg(TEXT_POISON3);
					towner[t]._tMsgSaid = TRUE;
				} else if (quests[Q_PWATER]._qactive == QUEST_DONE && quests[Q_PWATER]._qvar1 != 2) {
					quests[Q_PWATER]._qvar1 = 2;
					towner[t]._tbtcnt = 150;
					towner[t]._tVar1 = p;
					InitQTextMsg(TEXT_POISON5);
					CreateItem(UITEM_TRING, { towner[t]._t.x, towner[t]._t.y + 1 });
					towner[t]._tMsgSaid = TRUE;
				}
			}
			if (quests[Q_MUSHROOM]._qactive == QUEST_ACTIVE && quests[Q_MUSHROOM]._qmsg == TEXT_MUSH10 && PlrHasItem(p, IDI_BRAIN, &i) != NULL) {
				plr[p].inventory.RemoveInvItem(i);
				SpawnQuestItem(IDI_SPECELIX, { towner[t]._t.x, towner[t]._t.y + 1 }, 0, 0);
				InitQTextMsg(TEXT_MUSH4);
				quests[Q_MUSHROOM]._qvar1 = QS_BRAINGIVEN;
				Qtalklist[TOWN_HEALER]._qblkm = -1;
			}
		}
		if (!qtextflag) {
			TownerTalk(TEXT_PEPIN1, t);
			if (storeflag) {
				StartStore(STORE_HEALER);
			}
		}
	} else if (t == GetActiveTowner(TOWN_PEGBOY)) {
		if (!qtextflag) {
			TownerTalk(TEXT_WIRT1, t);
			if (storeflag) {
				StartStore(STORE_BOY);
			}
		}
	} else if (t == GetActiveTowner(TOWN_STORY)) {
		if (plr.isSingleplayer()) {
			if (quests[Q_BETRAYER]._qactive == QUEST_INIT && PlrHasItem(p, IDI_LAZSTAFF, &i) != NULL) {
				plr[p].inventory.RemoveInvItem(i);
				quests[Q_BETRAYER]._qvar1 = 2;
				towner[t]._tbtcnt = 150;
				towner[t]._tVar1 = p;
				InitQTextMsg(TEXT_VILE1);
				towner[t]._tMsgSaid = TRUE;
				quests[Q_BETRAYER]._qactive = QUEST_ACTIVE;
				quests[Q_BETRAYER]._qlog = TRUE;
			} else if (quests[Q_BETRAYER]._qactive == QUEST_DONE && quests[Q_BETRAYER]._qvar1 == 7) {
				quests[Q_BETRAYER]._qvar1 = 8;
				towner[t]._tbtcnt = 150;
				towner[t]._tVar1 = p;
				InitQTextMsg(TEXT_VILE3);
				towner[t]._tMsgSaid = TRUE;
				quests[Q_DIABLO]._qlog = TRUE;
			}
		}
		if (plr.isMultiplayer()) {
			if (quests[Q_BETRAYER]._qactive == QUEST_ACTIVE && !quests[Q_BETRAYER]._qlog) {
				towner[t]._tbtcnt = 150;
				towner[t]._tVar1 = p;
				InitQTextMsg(TEXT_VILE1);
				towner[t]._tMsgSaid = TRUE;
				quests[Q_BETRAYER]._qlog = TRUE;
				NetSendCmdQuest(TRUE, Q_BETRAYER);
			} else if (quests[Q_BETRAYER]._qactive == QUEST_DONE && quests[Q_BETRAYER]._qvar1 == 7) {
				quests[Q_BETRAYER]._qvar1 = 8;
				towner[t]._tbtcnt = 150;
				towner[t]._tVar1 = p;
				InitQTextMsg(TEXT_VILE3);
				towner[t]._tMsgSaid = TRUE;
				NetSendCmdQuest(TRUE, Q_BETRAYER);
				quests[Q_DIABLO]._qlog = TRUE;
				NetSendCmdQuest(TRUE, Q_DIABLO);
			}
		}
		if (!qtextflag) {
			TownerTalk(TEXT_STORY1, t);
			if (storeflag) {
				StartStore(STORE_STORY);
			}
		}
	} else if (towner[t]._ttype == TOWN_COW) {
		if (!qtextflag)
			CowSFX(p);
	}
}

void CowSFX(int pnum)
{
	if (CowPlaying == -1 || !effect_is_playing(CowPlaying)) {
		sgdwCowClicks++;
		if (sgdwCowClicks >= 8) {
			PlaySfxLoc(TSFX_COW1, { plr[pnum].pos().x, plr[pnum].pos().y + 5 });
			sgdwCowClicks = 4;
			CowPlaying = snSFX[sgnCowMsg][plr[pnum].data._pClass]; /* snSFX is local */
			sgnCowMsg++;
			if (sgnCowMsg >= 3)
				sgnCowMsg = 0;
		} else {
			CowPlaying = sgdwCowClicks == 4 ? TSFX_COW2 : TSFX_COW1;
		}
		PlaySfxLoc(CowPlaying, plr[pnum].pos());
	}
}

DEVILUTION_END_NAMESPACE
