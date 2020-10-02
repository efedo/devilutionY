#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

const int snSFX[3][NUM_CLASSES] = {
	{ PS_WARR52, PS_ROGUE52, PS_MAGE52 },
	{ PS_WARR49, PS_ROGUE49, PS_MAGE49 },
	{ PS_WARR50, PS_ROGUE50, PS_MAGE50 },
};

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

Towner::Towner(int i) : mytnum(i)
{
	assert(numInstances >= 0);
	++numInstances;
}

Towner::~Towner()
{
	assert(numInstances >= 0);
	--numInstances;

	if (numInstances <= 0) {
		if (_tNData == Cow::pCowCels) {
			MemFreeDbg(Cow::pCowCels);
			_tNData = NULL;
		} else if (_tNData) {
			MemFreeDbg(_tNData);
		}
	}
}

int numInstances = 0;

void Towner::NewTownerAnim(uint8_t *pAnim, int numFrames, int Delay)
{
	_tAnimData = pAnim;
	_tAnimLen = numFrames;
	_tAnimFrame = 1;
	_tAnimCnt = 0;
	_tAnimDelay = Delay;
}

void Towner::InitTownerInfo(int w, int sel, int t, int x, int y, int ao, int tp)
{
	_tSelFlag = sel;
	_tAnimWidth = w;
	_tAnimWidth2 = (w - 64) >> 1;
	_tMsgSaid = false;
	_ttype = t;
	_t = { x, y };
	grid[x][y].setActor(*this); // i should be my monster id
	_tAnimOrder = ao;
	_tTenPer = tp;
	_tSeed = GetRndSeed();
}

void Towner::InitQstSnds()
{
	int i = mytnum;
	int j, tl;
	tl = i;
	if (npcs.boyloadflag)
		tl++;
	for (j = 0; j < MAXQUESTS; j++) {
		qsts[j]._qsttype = quests[j]._qtype;
		qsts[j]._qstmsg = ((int *)(Qtalklist + tl))[j];
		if (((int *)(Qtalklist + tl))[j] != -1)
			qsts[j]._qstmsgact = true;
		else
			qsts[j]._qstmsgact = false;
	}
}

void Smith::Init()
{
	InitTownerInfo(96, true, TownerId::SMITH, 62, 63, 0, 10);
	InitQstSnds();
	_tNData = LoadFileInMem("Towners\\Smith\\SmithN.CEL", NULL);
	for (int i = 0; i < 8; i++) {
		_tNAnim[i] = _tNData;
	}
	_tNFrames = 16;
	NewTownerAnim(_tNAnim[int(Dir::SW)], _tNFrames, 3);
	strcpy(_tName, "Griswold the Blacksmith");
}

void BarOwner::Init()
{
	InitTownerInfo(96, true, TownerId::TAVERN, 55, 62, 3, 10);
	InitQstSnds();
	_tNData = LoadFileInMem("Towners\\TwnF\\TwnFN.CEL", NULL);
	for (int i = 0; i < 8; i++) {
		_tNAnim[i] = _tNData;
	}
	_tNFrames = 16;
	NewTownerAnim(_tNAnim[int(Dir::SW)], _tNFrames, 3);
	strcpy(_tName, "Ogden the Tavern owner");
}

void DeadGuy::Init()
{
	InitTownerInfo(96, true, TownerId::DEADGUY, 24, 32, -1, 10);
	InitQstSnds();
	_tNData = LoadFileInMem("Towners\\Butch\\Deadguy.CEL", NULL);
	for (int i = 0; i < 8; i++) {
		_tNAnim[i] = _tNData;
	}
	_tNFrames = 8;
	NewTownerAnim(_tNAnim[int(Dir::N)], _tNFrames, 6);
	strcpy(_tName, "Wounded Townsman");
}

void Witch::Init()
{
	InitTownerInfo(96, true, TownerId::WITCH, 80, 20, 5, 10);
	InitQstSnds();
	_tNData = LoadFileInMem("Towners\\TownWmn1\\Witch.CEL", NULL);
	for (int i = 0; i < 8; i++) {
		_tNAnim[i] = _tNData;
	}
	_tNFrames = 19;
	NewTownerAnim(_tNAnim[int(Dir::S)], _tNFrames, 6);
	strcpy(_tName, "Adria the Witch");
}

void Barmaid::Init()
{
	InitTownerInfo(96, true, TownerId::BMAID, 43, 66, -1, 10);
	InitQstSnds();
	_tNData = LoadFileInMem("Towners\\TownWmn1\\WmnN.CEL", NULL);
	for (int i = 0; i < 8; i++) {
		_tNAnim[i] = _tNData;
	}
	_tNFrames = 18;
	NewTownerAnim(_tNAnim[int(Dir::S)], _tNFrames, 6);
	strcpy(_tName, "Gillian the Barmaid");
}

void Boy::Init()
{
	npcs.boyloadflag = true;
	InitTownerInfo(96, true, TownerId::PEGBOY, 11, 53, -1, 10);
	InitQstSnds();
	_tNData = LoadFileInMem("Towners\\TownBoy\\PegKid1.CEL", NULL);
	for (int i = 0; i < 8; i++) {
		_tNAnim[i] = _tNData;
	}
	_tNFrames = 20;
	NewTownerAnim(_tNAnim[int(Dir::S)], _tNFrames, 6);
	strcpy(_tName, "Wirt the Peg-legged boy");
}

void Healer::Init()
{
	InitTownerInfo(96, true, TownerId::HEALER, 55, 79, 1, 10);
	InitQstSnds();
	_tNData = LoadFileInMem("Towners\\Healer\\Healer.CEL", NULL);
	for (int i = 0; i < 8; i++) {
		_tNAnim[i] = _tNData;
	}
	_tNFrames = 20;
	NewTownerAnim(_tNAnim[int(Dir::SE)], _tNFrames, 6);
	strcpy(_tName, "Pepin the Healer");
}

void Teller::Init()
{
	InitTownerInfo(96, true, TownerId::STORY, 62, 71, 2, 10);
	InitQstSnds();
	_tNData = LoadFileInMem("Towners\\Strytell\\Strytell.CEL", NULL);
	for (int i = 0; i < 8; i++) {
		_tNAnim[i] = _tNData;
	}
	_tNFrames = 25;
	NewTownerAnim(_tNAnim[int(Dir::S)], _tNFrames, 3);
	strcpy(_tName, "Cain the Elder");
}

void Drunk::Init()
{
	InitTownerInfo(96, true, TownerId::DRUNK, 71, 84, 4, 10);
	InitQstSnds();
	_tNData = LoadFileInMem("Towners\\Drunk\\TwnDrunk.CEL", NULL);
	for (int i = 0; i < 8; i++) {
		_tNAnim[i] = _tNData;
	}
	_tNFrames = 18;
	NewTownerAnim(_tNAnim[int(Dir::S)], _tNFrames, 3);
	strcpy(_tName, "Farnham the Drunk");
}


void Towner::TownCtrlMsg()
{
	if (_tbtcnt != 0) {
		if (!_tVar1) return;
		int dist = (_t - _tVar1->pos()).maxabs();
		if (dist >= 2)
			_tbtcnt = 0;
		if (!_tbtcnt) {
			dialog.qtextflag = false;
			stream_stop();
		}
	}
	advanceAnim();
}

void DeadGuy::TownCtrlMsg()
{
	Towner::TownCtrlMsg();
	if (!dialog.qtextflag) {
		if (quests[Q_BUTCHER]._qactive == QUEST_ACTIVE && quests[Q_BUTCHER]._qlog == 0) {
			return;
		}
		if (quests[Q_BUTCHER]._qactive != QUEST_INIT) {
			_tAnimDelay = 1000;
			_tAnimFrame = 1;
			strcpy(_tName, "Slain Townsman");
		}
	}
	if (quests[Q_BUTCHER]._qactive != QUEST_INIT)
		_tAnimCnt = 0;
}

void Towner::advanceAnim()
{
	_tAnimCnt++;
	if (_tAnimCnt >= _tAnimDelay) {
		_tAnimCnt = 0;

		if (_tAnimOrder >= 0) {
			int ao = _tAnimOrder;
			_tAnimFrameCnt++;
			if (AnimOrder[ao][_tAnimFrameCnt] == -1)
				_tAnimFrameCnt = 0;

			_tAnimFrame = AnimOrder[ao][_tAnimFrameCnt];
		} else {
			_tAnimFrame++;
			if (_tAnimFrame > _tAnimLen)
				_tAnimFrame = 1;
		}
	}
}

void Towner::TownerTalk(int first)
{
	Cow::sgdwCowClicks = 0;
	Cow::sgnCowMsg = 0;
	dialog.storeflag = true;
	dialog.InitQTextMsg(first);
}

void Towner::TalkToTowner(Player &player)
{
	int i, dx, dy, rv1, rv2, rv3;
	ItemStruct *Item;

	int dist = (player.pos() - _t).maxabs();
	#ifdef _DEBUG
	if (!debug_mode_key_d && (dist >= 2)) { return; }
	#else
	if (dist >= 2) { return; }
	#endif

	if (dialog.qtextflag) { return; }

	_tMsgSaid = false;

	if (pcurs >= CURSOR_FIRSTITEM && !DropItemBeforeTrig()) { return; }
}

void BarOwner::TalkToTowner(Player &player)
{
	Towner::TalkToTowner(player);
	if (!player.data._pLvlVisited[0] && !_tMsgSaid) {
		_tbtcnt = 150;
		_tVar1 = &player;
		dialog.InitQTextMsg(TEXT_INTRO);
		_tMsgSaid = true;
	}
	if ((player.data._pLvlVisited[2] || player.data._pLvlVisited[4]) &&
		quests[Q_SKELKING]._qactive != QUEST_NOTAVAIL) {
		if (quests[Q_SKELKING]._qvar2 == 0 && !_tMsgSaid) {
			quests[Q_SKELKING]._qvar2 = 1;
			quests[Q_SKELKING]._qlog = true;
			if (quests[Q_SKELKING]._qactive == QUEST_INIT) {
				quests[Q_SKELKING]._qactive = QUEST_ACTIVE;
				quests[Q_SKELKING]._qvar1 = 1;
			}
			_tbtcnt = 150;
			_tVar1 = p;
			dialog.InitQTextMsg(TEXT_KING2);
			_tMsgSaid = true;
			NetSendCmdQuest(true, Q_SKELKING);
		}
		if (quests[Q_SKELKING]._qactive == QUEST_DONE &&
			quests[Q_SKELKING]._qvar2 == 1 && !_tMsgSaid) {
			quests[Q_SKELKING]._qvar2 = 2;
			quests[Q_SKELKING]._qvar1 = 2;
			_tbtcnt = 150;
			_tVar1 = p;
			dialog.InitQTextMsg(TEXT_KING4);
			_tMsgSaid = true;
			NetSendCmdQuest(true, Q_SKELKING);
		}
	}
	if (plr.isSingleplayer() && plr[p].data._pLvlVisited[3] &&
		quests[Q_LTBANNER]._qactive != QUEST_NOTAVAIL) {
		if ((quests[Q_LTBANNER]._qactive == QUEST_INIT ||
			    quests[Q_LTBANNER]._qactive == QUEST_ACTIVE) &&
			quests[Q_LTBANNER]._qvar2 == 0 && !_tMsgSaid) {
			quests[Q_LTBANNER]._qvar2 = 1;
			if (quests[Q_LTBANNER]._qactive == QUEST_INIT) {
				quests[Q_LTBANNER]._qvar1 = 1;
				quests[Q_LTBANNER]._qactive = QUEST_ACTIVE;
			}
			quests[Q_LTBANNER]._qlog = true;
			_tbtcnt = 150;
			_tVar1 = &player;
			dialog.InitQTextMsg(TEXT_BANNER2);
			_tMsgSaid = true;
		}
		if (quests[Q_LTBANNER]._qvar2 == 1 &&
			player.HasItem(ItemIndex::BANNER) != NULL) {
			if (!_tMsgSaid) {
				quests[Q_LTBANNER]._qactive = QUEST_DONE;
				quests[Q_LTBANNER]._qvar1 = 3;
				player.RemoveInvItem(i);
				CreateItem(UniqueItemType::HARCREST,
					        {_t.x, _t.y + 1});
				_tbtcnt = 150;
				_tVar1 = p;
				dialog.InitQTextMsg(TEXT_BANNER3);
				_tMsgSaid = true;
			}
		}
	}
	if (!dialog.qtextflag) {
		TownerTalk(TEXT_OGDEN1);
		if (dialog.storeflag) { StartStore(StoreTalkId::TAVERN); }
	}
}

void DeadGuy::TalkToTowner(Player &player)
{
	Towner::TalkToTowner(player);
	if (quests[Q_BUTCHER]._qactive == QUEST_ACTIVE &&
	    quests[Q_BUTCHER]._qvar1 == 1) {
		_tbtcnt = 150;
		_tVar1 = &player;
		quests[Q_BUTCHER]._qvar1 = 1;
		if (player.data._pClass == PC_WARRIOR && !effect_is_playing(PS_WARR8)) {
			PlaySFX(PS_WARR8);
		} else if (player.data._pClass == PC_ROGUE &&
		           !effect_is_playing(PS_ROGUE8)) {
			PlaySFX(PS_ROGUE8);
		} else if (player.data._pClass == PC_SORCERER &&
		           !effect_is_playing(PS_MAGE8)) {
			PlaySFX(PS_MAGE8);
		}
		_tMsgSaid = true;
	} else if (quests[Q_BUTCHER]._qactive == QUEST_DONE &&
	           quests[Q_BUTCHER]._qvar1 == 1) {
		quests[Q_BUTCHER]._qvar1 = 1;
		_tbtcnt = 150;
		_tVar1 = &player;
		_tMsgSaid = true;
	} else if (quests[Q_BUTCHER]._qactive == QUEST_INIT ||
	           quests[Q_BUTCHER]._qactive == QUEST_ACTIVE &&
	               quests[Q_BUTCHER]._qvar1 == 0) {
		quests[Q_BUTCHER]._qactive = QUEST_ACTIVE;
		quests[Q_BUTCHER]._qlog = true;
		quests[Q_BUTCHER]._qmsg = TEXT_BUTCH9;
		quests[Q_BUTCHER]._qvar1 = 1;
		_tbtcnt = 50;
		_tVar1 = &player;
		_tVar2 = 3;
		dialog.InitQTextMsg(TEXT_BUTCH9);
		_tMsgSaid = true;
		NetSendCmdQuest(true, Q_BUTCHER);
	}
}

void Smith::TalkToTowner(Player &player)
{
	Towner::TalkToTowner(player);
	if (plr.isSingleplayer()) {
		if (player.data._pLvlVisited[4] &&
		    quests[Q_ROCK]._qactive != QUEST_NOTAVAIL) {
			if (quests[Q_ROCK]._qvar2 == 0) {
				quests[Q_ROCK]._qvar2 = 1;
				quests[Q_ROCK]._qlog = true;
				if (quests[Q_ROCK]._qactive == QUEST_INIT) {
					quests[Q_ROCK]._qactive = QUEST_ACTIVE;
					quests[Q_ROCK]._qvar1 = 1;
				}
				_tbtcnt = 150;
				_tVar1 = &player;
				dialog.InitQTextMsg(TEXT_INFRA5);
				_tMsgSaid = true;
			}
			if (quests[Q_ROCK]._qvar2 == 1 &&
			    player.HasItem(ItemIndex::ROCK) != NULL) {
				if (!_tMsgSaid) {
					quests[Q_ROCK]._qactive = QUEST_DONE;
					quests[Q_ROCK]._qvar2 = 2;
					quests[Q_ROCK]._qvar1 = 2;
					player.RemoveInvItem(i);
					CreateItem(UniqueItemType::INFRARING,
					           {_t.x, _t.y + 1});
					_tbtcnt = 150;
					_tVar1 = &player;
					dialog.InitQTextMsg(TEXT_INFRA7);
					_tMsgSaid = true;
				}
			}
		}
		if (plr[p].data._pLvlVisited[9] &&
		    quests[Q_ANVIL]._qactive != QUEST_NOTAVAIL) {
			if ((quests[Q_ANVIL]._qactive == QUEST_INIT ||
			     quests[Q_ANVIL]._qactive == QUEST_ACTIVE) &&
			    quests[Q_ANVIL]._qvar2 == 0 && !_tMsgSaid) {
				if (quests[Q_ROCK]._qvar2 == 2 ||
				    quests[Q_ROCK]._qactive == QUEST_ACTIVE &&
				        quests[Q_ROCK]._qvar2 == 1) {
					quests[Q_ANVIL]._qvar2 = 1;
					quests[Q_ANVIL]._qlog = true;
					if (quests[Q_ANVIL]._qactive == QUEST_INIT) {
						quests[Q_ANVIL]._qactive = QUEST_ACTIVE;
						quests[Q_ANVIL]._qvar1 = 1;
					}
					_tbtcnt = 150;
					_tVar1 = &player;
					dialog.InitQTextMsg(TEXT_ANVIL5);
					_tMsgSaid = true;
				}
			}
			if (quests[Q_ANVIL]._qvar2 == 1 &&
			    player.HasItem(ItemIndex::ANVIL) != NULL) {
				if (!_tMsgSaid) {
					quests[Q_ANVIL]._qactive = QUEST_DONE;
					quests[Q_ANVIL]._qvar2 = 2;
					quests[Q_ANVIL]._qvar1 = 2;
					player.RemoveInvItem(i);
					CreateItem(UniqueItemType::GRISWOLD,
					           {_t.x, _t.y + 1});
					_tbtcnt = 150;
					_tVar1 = p;
					dialog.InitQTextMsg(TEXT_ANVIL7);
					_tMsgSaid = true;
				}
			}
		}
	}
	if (!dialog.qtextflag) {
		TownerTalk(TEXT_GRISWOLD1);
		if (dialog.storeflag) { StartStore(StoreTalkId::SMITH); }
	}
}

void Witch::TalkToTowner(Player &player)
{
	Towner::TalkToTowner(player);
	if (quests[Q_MUSHROOM]._qactive == QUEST_INIT &&
	    player.HasItem(ItemIndex::FUNGALTM) != NULL) {
		player.RemoveInvItem(i);
		quests[Q_MUSHROOM]._qactive = QUEST_ACTIVE;
		quests[Q_MUSHROOM]._qlog = true;
		quests[Q_MUSHROOM]._qvar1 = QS_TOMEGIVEN;
		_tbtcnt = 150;
		_tVar1 = &player;
		dialog.InitQTextMsg(TEXT_MUSH8);
		_tMsgSaid = true;
	} else if (quests[Q_MUSHROOM]._qactive == QUEST_ACTIVE) {
		if (quests[Q_MUSHROOM]._qvar1 >= QS_TOMEGIVEN &&
		    quests[Q_MUSHROOM]._qvar1 <= QS_MUSHPICKED) {
			if (player.HasItem(ItemIndex::MUSHROOM) != NULL) {
				player.RemoveInvItem(i);
				quests[Q_MUSHROOM]._qvar1 = 5;
				Qtalklist[TownerId::HEALER]._qblkm = TEXT_MUSH3;
				Qtalklist[TownerId::WITCH]._qblkm = -1;
				_tbtcnt = 150;
				_tVar1 = &player;
				quests[Q_MUSHROOM]._qmsg = TEXT_MUSH10;
				dialog.InitQTextMsg(TEXT_MUSH10);
				_tMsgSaid = true;
			} else if (quests[Q_MUSHROOM]._qmsg != TEXT_MUSH9) {
				_tbtcnt = 150;
				_tVar1 = &player;
				quests[Q_MUSHROOM]._qmsg = TEXT_MUSH9;
				dialog.InitQTextMsg(TEXT_MUSH9);
				_tMsgSaid = true;
			}
		} else {
			Item * item = player.GetItem(ItemIndex::SPECELIX);
			if (item) {
				_tbtcnt = 150;
				_tVar1 = &player;
				dialog.InitQTextMsg(TEXT_MUSH12);
				quests[Q_MUSHROOM]._qactive = QUEST_DONE;
				_tMsgSaid = true;
				AllItemsList[item->IDidx].iUsable = true;
			} else if (player.HasItem(ItemIndex::BRAIN) != NULL &&
			           quests[Q_MUSHROOM]._qvar2 != TEXT_MUSH11) {
				_tbtcnt = 150;
				_tVar1 = &player;
				quests[Q_MUSHROOM]._qvar2 = TEXT_MUSH11;
				dialog.InitQTextMsg(TEXT_MUSH11);
				_tMsgSaid = true;
			}
		}
	}
	if (!dialog.qtextflag) {
		TownerTalk(TEXT_ADRIA1);
		if (dialog.storeflag) { StartStore(StoreTalkId::WITCH); }
	}
}

void Barmaid::TalkToTowner(Player &player)
{
	Towner::TalkToTowner(player);
	if (!dialog.qtextflag) {
		TownerTalk(TEXT_GILLIAN1);
		if (dialog.storeflag) { StartStore(StoreTalkId::BARMAID); }
	}
}

void Drunk::TalkToTowner(Player &player)
{
	Towner::TalkToTowner(player);
	if (!dialog.qtextflag) {
		TownerTalk(TEXT_FARNHAM1);
		if (dialog.storeflag) {
			StartStore(StoreTalkId::DRUNK);
		}
	}
}

void Healer::TalkToTowner(Player &player)
{
	Towner::TalkToTowner(player);
	if (plr.isSingleplayer()) {
		if (player.data._pLvlVisited[1] && !_tMsgSaid) {
			if (quests[Q_PWATER]._qactive == QUEST_INIT) {
				quests[Q_PWATER]._qactive = QUEST_ACTIVE;
				quests[Q_PWATER]._qlog = true;
				quests[Q_PWATER]._qmsg = TEXT_POISON3;
				quests[Q_PWATER]._qvar1 = 1;
				_tbtcnt = 150;
				_tVar1 = &player;
				dialog.InitQTextMsg(TEXT_POISON3);
				_tMsgSaid = true;
			} else if (quests[Q_PWATER]._qactive == QUEST_DONE &&
			           quests[Q_PWATER]._qvar1 != 2) {
				quests[Q_PWATER]._qvar1 = 2;
				_tbtcnt = 150;
				_tVar1 = &player;
				dialog.InitQTextMsg(TEXT_POISON5);
				CreateItem(UniqueItemType::TRING, {_t.x, _t.y + 1});
				_tMsgSaid = true;
			}
		}
		if (quests[Q_MUSHROOM]._qactive == QUEST_ACTIVE &&
		    quests[Q_MUSHROOM]._qmsg == TEXT_MUSH10 &&
		    player.HasItem(ItemIndex::BRAIN) != NULL) {
			player.RemoveInvItem(i);
			SpawnQuestItem(ItemIndex::SPECELIX, {_t.x, _t.y + 1},
			               0, 0);
			dialog.InitQTextMsg(TEXT_MUSH4);
			quests[Q_MUSHROOM]._qvar1 = QS_BRAINGIVEN;
			Qtalklist[TownerId::HEALER]._qblkm = -1;
		}
	}
	if (!dialog.qtextflag) {
		TownerTalk(TEXT_PEPIN1);
		if (dialog.storeflag) { StartStore(StoreTalkId::HEALER); }
	}
}

void Boy::TalkToTowner(Player &player)
{
	Towner::TalkToTowner(player);
	if (!dialog.qtextflag) {
		TownerTalk(TEXT_WIRT1);
		if (dialog.storeflag) {
			StartStore(StoreTalkId::BOY);
		}
	}
}

void Cow::TalkToTowner(Player &player)
{
	Towner::TalkToTowner(player);
	if (!dialog.qtextflag) CowSFX(player);
}

void Story::TalkToTowner(Player &player)
{
	Towner::TalkToTowner(player);
	if (plr.isSingleplayer()) {
		if (quests[Q_BETRAYER]._qactive == QUEST_INIT &&
		    player.HasItem(ItemIndex::LAZSTAFF) != NULL) {
			player.RemoveInvItem(i);
			quests[Q_BETRAYER]._qvar1 = 2;
			_tbtcnt = 150;
			_tVar1 = &player;
			dialog.InitQTextMsg(TEXT_VILE1);
			_tMsgSaid = true;
			quests[Q_BETRAYER]._qactive = QUEST_ACTIVE;
			quests[Q_BETRAYER]._qlog = true;
		} else if (quests[Q_BETRAYER]._qactive == QUEST_DONE &&
		           quests[Q_BETRAYER]._qvar1 == 7) {
			quests[Q_BETRAYER]._qvar1 = 8;
			_tbtcnt = 150;
			_tVar1 = &player;
			dialog.InitQTextMsg(TEXT_VILE3);
			_tMsgSaid = true;
			quests[Q_DIABLO]._qlog = true;
		}
	}
	if (plr.isMultiplayer()) {
		if (quests[Q_BETRAYER]._qactive == QUEST_ACTIVE &&
		    !quests[Q_BETRAYER]._qlog) {
			_tbtcnt = 150;
			_tVar1 = &player;
			dialog.InitQTextMsg(TEXT_VILE1);
			_tMsgSaid = true;
			quests[Q_BETRAYER]._qlog = true;
			NetSendCmdQuest(true, Q_BETRAYER);
		} else if (quests[Q_BETRAYER]._qactive == QUEST_DONE &&
		           quests[Q_BETRAYER]._qvar1 == 7) {
			quests[Q_BETRAYER]._qvar1 = 8;
			_tbtcnt = 150;
			_tVar1 = &player;
			dialog.InitQTextMsg(TEXT_VILE3);
			_tMsgSaid = true;
			NetSendCmdQuest(true, Q_BETRAYER);
			quests[Q_DIABLO]._qlog = true;
			NetSendCmdQuest(true, Q_DIABLO);
		}
	}
	if (!dialog.qtextflag) {
		TownerTalk(TEXT_STORY1);
		if (dialog.storeflag) { StartStore(StoreTalkId::STORY); }
	}
}

void Cow::CowSFX(Player &player)
{
	if (CowPlaying == -1 || !effect_is_playing(CowPlaying)) {
		sgdwCowClicks++;
		if (sgdwCowClicks >= 8) {
			PlaySfxLoc(TSFX_COW1, { player.pos().x, player.pos().y + 5 });
			sgdwCowClicks = 4;
			CowPlaying = snSFX[sgnCowMsg][player.data._pClass]; /* snSFX is local */
			sgnCowMsg++;
			if (sgnCowMsg >= 3) sgnCowMsg = 0;
		} else {
			CowPlaying = sgdwCowClicks == 4 ? TSFX_COW2 : TSFX_COW1;
		}
		PlaySfxLoc(CowPlaying, player.pos());
	}
}

DEVILUTION_END_NAMESPACE
