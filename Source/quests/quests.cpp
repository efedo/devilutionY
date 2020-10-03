#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

int qtopline;
uint8_t *pQLogCel;
QuestStruct quests[MAXQUESTS];
int qline;
int qlist[MAXQUESTS];
int numqlines;
int WaterDone;
int ReturnLvlX;
int ReturnLvlY;
DunType ReturnLvlT;
int ReturnLvl;

QuestData questlist[MAXQUESTS] = {
	// clang-format off
	// _qdlvl, _qdmultlvl, _qlvlt,          _qdtype,   _qdrnd, _qslvl, _qflags, _qdmsg,         _qlstr
	{       5,         -1, DunType::none,      Q_ROCK,  100,      SetLvl::None,       0, TEXT_INFRA5,   "The Magic Rock"           },
	{       9,         -1, DunType::none,      Q_MUSHROOM,   100,      SetLvl::None,       0, TEXT_MUSH8,    "Black Mushroom"           },
	{       4,         -1, DunType::none,      Q_GARBUD,   100,      SetLvl::None,       0, TEXT_GARBUD1,  "Gharbad The Weak"         },
	{       8,         -1, DunType::none,      Q_ZHAR,   100,      SetLvl::None,       0, TEXT_ZHAR1,    "Zhar the Mad"             },
	{      14,         -1, DunType::none,      Q_VEIL,   100,      SetLvl::None,       0, TEXT_VEIL9,    "Lachdanan"                },
	{      15,         -1, DunType::none,      Q_DIABLO,    100,      SetLvl::None,       1, TEXT_VILE3,    "Diablo"                   },
	{       2,          2, DunType::none,      Q_BUTCHER,  100,      SetLvl::None,       1, TEXT_BUTCH9,   "The Butcher"              },
	{       4,         -1, DunType::none,      Q_LTBANNER,    100,      SetLvl::None,       0, TEXT_BANNER2,  "Ogden's Sign"             },
	{       7,         -1, DunType::none,      Q_BLIND,  100,      SetLvl::None,       0, TEXT_BLINDING, "Halls of the Blind"       },
	{       5,         -1, DunType::none,      Q_BLOOD,  100,      SetLvl::None,       0, TEXT_BLOODY,   "Valor"                    },
	{      10,         -1, DunType::none,      Q_ANVIL,  100,      SetLvl::None,       0, TEXT_ANVIL5,   "Anvil of Fury"            },
	{      13,         -1, DunType::none,      Q_WARLORD, 100,      SetLvl::None,       0, TEXT_BLOODWAR, "Warlord of Blood"         },
	{       3,          3, DunType::cathedral, Q_SKELKING,   100,      SetLvl::SkelKing,       1, TEXT_KING2,    "The Curse of King Leoric" },
	{       2,         -1, DunType::caves,     Q_PWATER,     100,      SetLvl::PoisonWater,       0, TEXT_POISON3,  "Poisoned Water Supply"    },
	{       6,         -1, DunType::catacombs, Q_SCHAMB,   100,      SetLvl::BoneChamb,       0, TEXT_BONER,    "The Chamber of Bone"      },
	{      15,         15, DunType::cathedral, Q_BETRAYER,     100,      SetLvl::VileBetrayer,       1, TEXT_VILE1,    "Archbishop Lazarus"       },
	// clang-format on
};
V2Di questoff[7] = {
	{ 0, 0 },
	{ -1, 0 },
	{ 0, -1 },
	{ -1, -1 },
	{ -2, -1 },
	{ -1, -2 },
	{ -2, -2 }
};

char *questtrigstr[5] = {
	"King Leoric's Tomb",
	"The Chamber of Bone",
	"Maze",
	"A Dark Passage",
	"Unholy Altar"
};
int QuestGroup1[3] = { Q_BUTCHER, Q_LTBANNER, Q_GARBUD };
int QuestGroup2[3] = { Q_BLIND, Q_ROCK, Q_BLOOD };
int QuestGroup3[3] = { Q_MUSHROOM, Q_ZHAR, Q_ANVIL };
int QuestGroup4[2] = { Q_VEIL, Q_WARLORD };

void InitQuests()
{
	int i, initiatedQuests;
	DWORD z;

	if (plr.isSingleplayer()) {
		for (i = 0; i < MAXQUESTS; i++) {
			quests[i]._qactive = QUEST_NOTAVAIL;
		}
	} else {
		for (i = 0; i < MAXQUESTS; i++) {
			if (!(questlist[i]._qflags & 1)) {
				quests[i]._qactive = QUEST_NOTAVAIL;
			}
		}
	}

	questlog = false;
	PentSpn2Frame = 1;
	WaterDone = 0;
	initiatedQuests = 0;

	for (z = 0; z < MAXQUESTS; z++) {
		if (plr.isSingleplayer() || questlist[z]._qflags & 1) {
			quests[z]._qtype = questlist[z]._qdtype;
			if (plr.isMultiplayer()) {
				quests[z]._qlevel = questlist[z]._qdmultlvl;
				if (!delta_quest_inited(initiatedQuests)) {
					quests[z]._qactive = QUEST_INIT;
					quests[z]._qvar1 = 0;
					quests[z]._qlog = 0;
				}
				initiatedQuests++;
			} else {
				quests[z]._qactive = QUEST_INIT;
				quests[z]._qlevel = questlist[z]._qdlvl;
				quests[z]._qvar1 = 0;
				quests[z]._qlog = 0;
			}

			quests[z]._qslvl = questlist[z]._qslvl;
			quests[z]._qt = { 0, 0 };
			quests[z]._qidx = z;
			quests[z]._qlvltype = questlist[z]._qlvlt;
			quests[z]._qvar2 = 0;
			quests[z]._qmsg = questlist[z]._qdmsg;
		}
	}

	if (plr.isSingleplayer()) {
		SetRndSeed(glSeedTbl[15]);
		if (random_(0, 2))
			quests[Q_PWATER]._qactive = QUEST_NOTAVAIL;
		else
			quests[Q_SKELKING]._qactive = QUEST_NOTAVAIL;

		quests[QuestGroup1[random_(0, sizeof(QuestGroup1) / sizeof(int))]]._qactive = QUEST_NOTAVAIL;
		quests[QuestGroup2[random_(0, sizeof(QuestGroup2) / sizeof(int))]]._qactive = QUEST_NOTAVAIL;
		quests[QuestGroup3[random_(0, sizeof(QuestGroup3) / sizeof(int))]]._qactive = QUEST_NOTAVAIL;
		quests[QuestGroup4[random_(0, sizeof(QuestGroup4) / sizeof(int))]]._qactive = QUEST_NOTAVAIL;
	}
#ifdef _DEBUG
	if (questdebug != -1)
		quests[questdebug]._qactive = QUEST_ACTIVE;
#endif
	if (!quests[Q_SKELKING]._qactive)
		quests[Q_SKELKING]._qvar2 = 2;
	if (!quests[Q_ROCK]._qactive)
		quests[Q_ROCK]._qvar2 = 2;
	quests[Q_LTBANNER]._qvar1 = 1;
	if (plr.isMultiplayer())
		quests[Q_BETRAYER]._qvar1 = 2;
}

void CheckQuests()
{
	int i;
	V2Di rport;

	if (QuestStatus(Q_BETRAYER) && plr.isMultiplayer() && quests[Q_BETRAYER]._qvar1 == 2) {
		AddObject(ObjectType::ALTBOY, 2 * lvl.getpc().x + 20, 2 * lvl.getpc().y + 22);
		quests[Q_BETRAYER]._qvar1 = 3;
		NetSendCmdQuest(true, Q_BETRAYER);
	}

	if (plr.isMultiplayer()) {
		return;
	}

	if (lvl.currlevel == quests[Q_BETRAYER]._qlevel
	    && !lvl.setlevel
	    && quests[Q_BETRAYER]._qvar1 >= 2
	    && (quests[Q_BETRAYER]._qactive == QUEST_ACTIVE || quests[Q_BETRAYER]._qactive == QUEST_DONE)
	    && (quests[Q_BETRAYER]._qvar2 == 0 || quests[Q_BETRAYER]._qvar2 == 2)) {
		quests[Q_BETRAYER]._qt = 2 * quests[Q_BETRAYER]._qt + V2Di(16, 16);
		rport = quests[Q_BETRAYER]._qt;
		AddMissile(rport, rport, Dir(0), MissileType::RPORTAL, 0, myplr(), 0, 0);
		quests[Q_BETRAYER]._qvar2 = 1;
		if (quests[Q_BETRAYER]._qactive == QUEST_ACTIVE) {
			quests[Q_BETRAYER]._qvar1 = 3;
		}
	}

	if (quests[Q_BETRAYER]._qactive == QUEST_DONE
	    && lvl.setlevel
	    && lvl.setlvlnum == SetLvl::VileBetrayer
	    && quests[Q_BETRAYER]._qvar2 == 4) {
		rport = { 35, 32 };
		AddMissile(rport, rport, Dir(0), MissileType::RPORTAL, 0, myplr(), 0, 0);
		quests[Q_BETRAYER]._qvar2 = 3;
	}

	if (lvl.setlevel) {
		if (lvl.setlvlnum == quests[Q_PWATER]._qslvl
		    && quests[Q_PWATER]._qactive != QUEST_INIT
		    && lvl.type() == quests[Q_PWATER]._qlvltype
		    && nummonsters == 4
		    && quests[Q_PWATER]._qactive != QUEST_DONE) {
			quests[Q_PWATER]._qactive = QUEST_DONE;
			PlaySfxLoc(IS_QUESTDN, myplr().pos());
			LoadPalette("Levels\\L3Data\\L3pwater.pal");
			WaterDone = 32;
		}
		if (WaterDone > 0) {
			palette_update_quest_palette(WaterDone);
			WaterDone--;
		}
	} else if (myplr().data._pmode == PlayerMode::STAND) {
		for (i = 0; i < MAXQUESTS; i++) {
			if (lvl.currlevel == quests[i]._qlevel
			    && quests[i]._qslvl != SetLvl::None
			    && quests[i]._qactive != QUEST_NOTAVAIL
			    && myplr().pos() == quests[i]._qt) {
				if (quests[i]._qlvltype != DunType::none) {
					lvl.setlvltype = quests[i]._qlvltype;
				}
				myplr().StartNewLvl(WM_DIABSETLVL, int(quests[i]._qslvl));
			}
		}
	}
}

bool ForceQuests()
{
	int i, j, ql;
	V2Di q;

	if (plr.isMultiplayer()) {
		return false;
	}

	for (i = 0; i < MAXQUESTS; i++) {

		if (i != Q_BETRAYER && lvl.currlevel == quests[i]._qlevel && quests[i]._qslvl != SetLvl::None) {
			ql = int(quests[quests[i]._qidx]._qslvl) - 1;
			q = quests[i]._qt;

			for (j = 0; j < 7; j++) {
				if (q + questoff[j] == cursm) {
					sprintf(infostr, "To %s", questtrigstr[ql]);
					cursm = q;
					return true;
				}
			}
		}
	}

	return false;
}

bool QuestStatus(int i)
{
	bool result;

	if (lvl.setlevel
	    || lvl.currlevel != quests[i]._qlevel
	    || !quests[i]._qactive
	    || (result = 1, plr.isMultiplayer()) && !(questlist[i]._qflags & 1)) {
		result = false;
	}
	return result;
}

void CheckQuestKill(int m, bool sendmsg)
{
	int i, j;

	if (monsters[m].data.MType->mtype == MonsterType::SKING) {
		quests[Q_SKELKING]._qactive = QUEST_DONE;
		sfxdelay = 30;
		if (myplr().data._pClass == PlayerClass::warrior) {
			sfxdnum = PS_WARR82;
		} else if (myplr().data._pClass == PlayerClass::rogue) {
			sfxdnum = PS_ROGUE82;
		} else if (myplr().data._pClass == PlayerClass::sorceror) {
			sfxdnum = PS_MAGE82;
		}
		if (sendmsg)
			NetSendCmdQuest(true, Q_SKELKING);

	} else if (monsters[m].data.MType->mtype == MonsterType::CLEAVER) {
		quests[Q_BUTCHER]._qactive = QUEST_DONE;
		sfxdelay = 30;
		if (myplr().data._pClass == PlayerClass::warrior) {
			sfxdnum = PS_WARR80;
		} else if (myplr().data._pClass == PlayerClass::rogue) {
			sfxdnum = PS_ROGUE80;
		} else if (myplr().data._pClass == PlayerClass::sorceror) {
			sfxdnum = PS_MAGE80;
		}
		if (sendmsg)
			NetSendCmdQuest(true, Q_BUTCHER);
	} else if (monsters[m].data.mName == UniqMonst[UniqueMonsterType::GARBUD].mName) { //"Gharbad the Weak"
		quests[Q_GARBUD]._qactive = QUEST_DONE;
		sfxdelay = 30;
		if (myplr().data._pClass == PlayerClass::warrior) {
			sfxdnum = PS_WARR61;
		} else if (myplr().data._pClass == PlayerClass::rogue) {
			sfxdnum = PS_ROGUE61;
		} else if (myplr().data._pClass == PlayerClass::sorceror) {
			sfxdnum = PS_MAGE61;
		}
	} else if (monsters[m].data.mName == UniqMonst[UniqueMonsterType::ZHAR].mName) { //"Zhar the Mad"
		quests[Q_ZHAR]._qactive = QUEST_DONE;
		sfxdelay = 30;
		if (myplr().data._pClass == PlayerClass::warrior) {
			sfxdnum = PS_WARR62;
		} else if (myplr().data._pClass == PlayerClass::rogue) {
			sfxdnum = PS_ROGUE62;
		} else if (myplr().data._pClass == PlayerClass::sorceror) {
			sfxdnum = PS_MAGE62;
		}
	} else if (monsters[m].data.mName == UniqMonst[UniqueMonsterType::LAZURUS].mName && plr.isMultiplayer()) { //"Arch-Bishop Lazarus"
		quests[Q_BETRAYER]._qactive = QUEST_DONE;
		quests[Q_BETRAYER]._qvar1 = 7;
		sfxdelay = 30;
		quests[Q_DIABLO]._qactive = QUEST_ACTIVE;

		for (j = 0; j < MAXDUNY; j++) {
			for (i = 0; i < MAXDUNX; i++) {
				if (grid[i][j].getPiece() == 370) {
					trigs[numtrigs]._t.x = i;
					trigs[numtrigs]._t.y = j;
					trigs[numtrigs]._tmsg = WM_DIABNEXTLVL;
					numtrigs++;
				}
			}
		}
		if (myplr().data._pClass == PlayerClass::warrior) {
			sfxdnum = PS_WARR83;
		} else if (myplr().data._pClass == PlayerClass::rogue) {
			sfxdnum = PS_ROGUE83;
		} else if (myplr().data._pClass == PlayerClass::sorceror) {
			sfxdnum = PS_MAGE83;
		}
		if (sendmsg) {
			NetSendCmdQuest(true, Q_BETRAYER);
			NetSendCmdQuest(true, Q_DIABLO);
		}
	} else if (monsters[m].data.mName == UniqMonst[UniqueMonsterType::LAZURUS].mName && plr.isSingleplayer()) { //"Arch-Bishop Lazarus"
		quests[Q_BETRAYER]._qactive = QUEST_DONE;
		sfxdelay = 30;
		InitVPTriggers();
		quests[Q_BETRAYER]._qvar1 = 7;
		quests[Q_BETRAYER]._qvar2 = 4;
		quests[Q_DIABLO]._qactive = QUEST_ACTIVE;
		AddMissile({ 35, 32 }, { 35, 32 }, Dir(0), MissileType::RPORTAL, 0, myplr(), 0, 0);
		if (myplr().data._pClass == PlayerClass::warrior) {
			sfxdnum = PS_WARR83;
		} else if (myplr().data._pClass == PlayerClass::rogue) {
			sfxdnum = PS_ROGUE83;
		} else if (myplr().data._pClass == PlayerClass::sorceror) {
			sfxdnum = PS_MAGE83;
		}
	} else if (monsters[m].data.mName == UniqMonst[UniqueMonsterType::WARLORD].mName) { //"Warlord of Blood"
		quests[Q_WARLORD]._qactive = QUEST_DONE;
		sfxdelay = 30;
		if (myplr().data._pClass == PlayerClass::warrior) {
			sfxdnum = PS_WARR94;
		} else if (myplr().data._pClass == PlayerClass::rogue) {
			sfxdnum = PS_ROGUE94;
		} else if (myplr().data._pClass == PlayerClass::sorceror) {
			sfxdnum = PS_MAGE94;
		}
	}
}

void DrawButcher()
{
	int x, y;

	x = 2 * lvl.getpc().x + 16;
	y = 2 * lvl.getpc().y + 16;
	DRLG_RectTrans(x + 3, y + 3, x + 10, y + 10);
}

void DrawSkelKing(int q, int x, int y)
{
	quests[q]._qt.x = 2 * x + 28;
	quests[q]._qt.y = 2 * y + 23;
}

void DrawWarLord(int x, int y)
{
	int rw, rh;
	int i, j;
	uint8_t *sp, *setp;
	int v;

	setp = LoadFileInMem("Levels\\L4Data\\Warlord2.DUN", NULL);
	rw = *setp;
	sp = setp + 2;
	rh = *sp;
	sp += 2;
	lvl.setpc({ x, y, rw, rh });

	for (j = y; j < y + rh; j++) {
		for (i = x; i < x + rw; i++) {
			if (*sp != 0) {
				v = *sp;
			} else {
				v = 6;
			}
			dgrid[i][j].dungeon = v;
			sp += 2;
		}
	}
	mem_free_dbg(setp);
}

void DrawSChamber(int q, int x, int y)
{
	int i, j;
	int rw, rh;
	int xx, yy;
	uint8_t *sp, *setp;
	int v;

	setp = LoadFileInMem("Levels\\L2Data\\Bonestr1.DUN", NULL);
	rw = *setp;
	sp = setp + 2;
	rh = *sp;
	sp += 2;
	lvl.setpc({ x, y, rw, rh });

	for (j = y; j < y + rh; j++) {
		for (i = x; i < x + rw; i++) {
			if (*sp != 0) {
				v = *sp;
			} else {
				v = 3;
			}
			dgrid[i][j].dungeon = v;
			sp += 2;
		}
	}
	xx = 2 * x + 22;
	yy = 2 * y + 23;
	quests[q]._qt.x = xx;
	quests[q]._qt.y = yy;
	mem_free_dbg(setp);
}

void DrawLTBanner(int x, int y)
{
	int rw, rh;
	int i, j;
	uint8_t *sp, *setp;

	setp = LoadFileInMem("Levels\\L1Data\\Banner1.DUN", NULL);
	rw = *setp;
	sp = setp + 2;
	rh = *sp;
	sp += 2;
	lvl.setpc({ x, y, rw, rh });

	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*sp != 0) {
				dgrid[x + i][y + j].pdungeon = *sp;
			}
			sp += 2;
		}
	}
	mem_free_dbg(setp);
}

void DrawBlind(int x, int y)
{
	int rw, rh;
	int i, j;
	uint8_t *sp, *setp;

	setp = LoadFileInMem("Levels\\L2Data\\Blind1.DUN", NULL);
	rw = *setp;
	sp = setp + 2;
	rh = *sp;
	sp += 2;
	lvl.setpc({ x, y, rw, rh });

	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*sp != 0) {
				dgrid[x + i][y + j].pdungeon = *sp;
			}
			sp += 2;
		}
	}
	mem_free_dbg(setp);
}

void DrawBlood(int x, int y)
{
	int rw, rh;
	int i, j;
	uint8_t *sp, *setp;

	setp = LoadFileInMem("Levels\\L2Data\\Blood2.DUN", NULL);
	rw = *setp;
	sp = setp + 2;
	rh = *sp;
	sp += 2;
	lvl.setpc({x, y, rw, rh});

	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*sp != 0) {
				dgrid[x + i][y + j].dungeon = *sp;
			}
			sp += 2;
		}
	}
	mem_free_dbg(setp);
}

void DRLG_CheckQuests(int x, int y)
{
	int i;

	for (i = 0; i < MAXQUESTS; i++) {
		if (QuestStatus(i)) {
			switch (quests[i]._qtype) {
			case Q_BUTCHER:
				DrawButcher();
				break;
			case Q_LTBANNER:
				DrawLTBanner(x, y);
				break;
			case Q_BLIND:
				DrawBlind(x, y);
				break;
			case Q_BLOOD:
				DrawBlood(x, y);
				break;
			case Q_WARLORD:
				DrawWarLord(x, y);
				break;
			case Q_SKELKING:
				DrawSkelKing(i, x, y);
				break;
			case Q_SCHAMB:
				DrawSChamber(i, x, y);
				break;
			}
		}
	}
}

void SetReturnLvlPos()
{
	switch (lvl.setlvlnum) {
	case SetLvl::SkelKing:
		ReturnLvlX = quests[Q_SKELKING]._qt.x + 1;
		ReturnLvlY = quests[Q_SKELKING]._qt.y;
		ReturnLvl = quests[Q_SKELKING]._qlevel;
		ReturnLvlT = DunType::cathedral;
		break;
	case SetLvl::BoneChamb:
		ReturnLvlX = quests[Q_SCHAMB]._qt.x + 1;
		ReturnLvlY = quests[Q_SCHAMB]._qt.y;
		ReturnLvl = quests[Q_SCHAMB]._qlevel;
		ReturnLvlT = DunType::catacombs;
		break;
	case SetLvl::PoisonWater:
		ReturnLvlX = quests[Q_PWATER]._qt.x;
		ReturnLvlY = quests[Q_PWATER]._qt.y + 1;
		ReturnLvl = quests[Q_PWATER]._qlevel;
		ReturnLvlT = DunType::cathedral;
		break;
	case SetLvl::VileBetrayer:
		ReturnLvlX = quests[Q_BETRAYER]._qt.x + 1;
		ReturnLvlY = quests[Q_BETRAYER]._qt.y - 1;
		ReturnLvl = quests[Q_BETRAYER]._qlevel;
		ReturnLvlT = DunType::hell;
		break;
	}
}

void GetReturnLvlPos()
{
	if (quests[Q_BETRAYER]._qactive == QUEST_DONE)
		quests[Q_BETRAYER]._qvar2 = 2;
	View.x = ReturnLvlX;
	View.y = ReturnLvlY;
	lvl.currlevel = ReturnLvl;
	lvl.setType(ReturnLvlT);
}

void ResyncMPQuests()
{
	if (quests[Q_SKELKING]._qactive == QUEST_INIT
	    && lvl.currlevel >= quests[Q_SKELKING]._qlevel - 1
	    && lvl.currlevel <= quests[Q_SKELKING]._qlevel + 1) {
		quests[Q_SKELKING]._qactive = QUEST_ACTIVE;
		NetSendCmdQuest(true, Q_SKELKING);
	}
	if (quests[Q_BUTCHER]._qactive == QUEST_INIT
	    && lvl.currlevel >= quests[Q_BUTCHER]._qlevel - 1
	    && lvl.currlevel <= quests[Q_BUTCHER]._qlevel + 1) {
		quests[Q_BUTCHER]._qactive = QUEST_ACTIVE;
		NetSendCmdQuest(true, Q_BUTCHER);
	}
	if (quests[Q_BETRAYER]._qactive == QUEST_INIT && lvl.currlevel == quests[Q_BETRAYER]._qlevel - 1) {
		quests[Q_BETRAYER]._qactive = QUEST_ACTIVE;
		NetSendCmdQuest(true, Q_BETRAYER);
	}
	if (QuestStatus(Q_BETRAYER))
		AddObject(ObjectType::ALTBOY, 2 * lvl.getpc().x + 20, 2 * lvl.getpc().y + 22);
}

void ResyncQuests()
{
	int i, tren, x, y;

	if (lvl.setlevel && lvl.setlvlnum == quests[Q_PWATER]._qslvl && quests[Q_PWATER]._qactive != QUEST_INIT && lvl.type() == quests[Q_PWATER]._qlvltype) {

		if (quests[Q_PWATER]._qactive == QUEST_DONE)
			LoadPalette("Levels\\L3Data\\L3pwater.pal");
		else
			LoadPalette("Levels\\L3Data\\L3pfoul.pal");

		for (i = 0; i <= 32; i++)
			palette_update_quest_palette(i);
	}

	if (QuestStatus(Q_LTBANNER)) {
		if (quests[Q_LTBANNER]._qvar1 == 1)
			ObjChangeMapResync(
			    { lvl.getpc().w + lvl.getpc().x - 2,
			        lvl.getpc().h + lvl.getpc().y - 2 },
			    { lvl.getpc().w + lvl.getpc().x + 1,
			        lvl.getpc().h + lvl.getpc().y + 1 });
		if (quests[Q_LTBANNER]._qvar1 == 2) {
			ObjChangeMapResync(
			    { lvl.getpc().w + lvl.getpc().x - 2,
			        lvl.getpc().h + lvl.getpc().y - 2 },
			    { lvl.getpc().w + lvl.getpc().x + 1,
			        lvl.getpc().h + lvl.getpc().y + 1 });
			ObjChangeMapResync({ lvl.getpc().x, lvl.getpc().y }, { (lvl.getpc().w >> 1) + lvl.getpc().x + 2, (lvl.getpc().h >> 1) + lvl.getpc().y - 2 });
			for (i = 0; i < nobjects; i++)
				SyncObjectAnim(objectactive[i]);
			tren = lvl.TransVal;
			lvl.TransVal = 9;
			DRLG_MRectTrans(lvl.getpc().x, lvl.getpc().y, (lvl.getpc().w >> 1) + lvl.getpc().x + 4, lvl.getpc().y + (lvl.getpc().h >> 1));
			lvl.TransVal = tren;
		}
		if (quests[Q_LTBANNER]._qvar1 == 3) {
			x = lvl.getpc().x;
			y = lvl.getpc().y;
			ObjChangeMapResync({ x, y }, { x + lvl.getpc().w + 1, y + lvl.getpc().h + 1 });
			for (i = 0; i < nobjects; i++)
				SyncObjectAnim(objectactive[i]);
			tren = lvl.TransVal;
			lvl.TransVal = 9;
			DRLG_MRectTrans(lvl.getpc().x, lvl.getpc().y, (lvl.getpc().w >> 1) + lvl.getpc().x + 4, lvl.getpc().y + (lvl.getpc().h >> 1));
			lvl.TransVal = tren;
		}
	}
	if (lvl.currlevel == quests[Q_MUSHROOM]._qlevel) {
		if (quests[Q_MUSHROOM]._qactive == QUEST_INIT && !quests[Q_MUSHROOM]._qvar1) {
			SpawnQuestItem(ItemIndex::FUNGALTM, { 0, 0 }, 5, 1);
			quests[Q_MUSHROOM]._qvar1 = QS_TOMESPAWNED;
		} else {
			if (quests[Q_MUSHROOM]._qactive == QUEST_ACTIVE) {
				if (quests[Q_MUSHROOM]._qvar1 >= QS_MUSHGIVEN) {
					Qtalklist[TownerId::WITCH]._qblkm = -1;
					Qtalklist[TownerId::HEALER]._qblkm = TEXT_MUSH3;
				} else if (quests[Q_MUSHROOM]._qvar1 >= QS_BRAINGIVEN) {
					Qtalklist[TownerId::HEALER]._qblkm = -1;
				}
			}
		}
	}
	if (lvl.currlevel == quests[Q_VEIL]._qlevel + 1 && quests[Q_VEIL]._qactive == QUEST_ACTIVE && !quests[Q_VEIL]._qvar1) {
		quests[Q_VEIL]._qvar1 = 1;
		SpawnQuestItem(ItemIndex::GLDNELIX, { 0, 0 }, 5, 1);
	}
	if (lvl.setlevel && lvl.setlvlnum == SetLvl::VileBetrayer) {
		if (quests[Q_BETRAYER]._qvar1 >= 4)
			ObjChangeMapResync({ 1, 11 }, { 20, 18 });
		if (quests[Q_BETRAYER]._qvar1 >= 6)
			ObjChangeMapResync({ 1, 18 }, { 20, 24 });
		if (quests[Q_BETRAYER]._qvar1 >= 7)
			InitVPTriggers();
		for (i = 0; i < nobjects; i++)
			SyncObjectAnim(objectactive[i]);
	}
	if (lvl.currlevel == quests[Q_BETRAYER]._qlevel
	    && !lvl.setlevel
	    && (quests[Q_BETRAYER]._qvar2 == 1 || quests[Q_BETRAYER]._qvar2 >= 3)
	    && (quests[Q_BETRAYER]._qactive == QUEST_ACTIVE || quests[Q_BETRAYER]._qactive == QUEST_DONE)) {
		quests[Q_BETRAYER]._qvar2 = 2;
	}
}

void PrintQLString(int x, int y, bool cjustflag, char *str, int col)
{
	int len, width, i, k, sx, sy;
	uint8_t c;

	sx = x + 96;
	sy = y * 12 + 204;
	len = strlen(str);
	k = 0;
	if (cjustflag) {
		width = 0;
		for (i = 0; i < len; i++)
			width += fontkern[fontframe[gbFontTransTbl[(uint8_t)str[i]]]] + 1;
		if (width < 257)
			k = (257 - width) >> 1;
		sx += k;
	}
	if (qline == y) {
		CelDraw(cjustflag ? x + k + 76 : x + 76, sy + 1, pSPentSpn2Cels, PentSpn2Frame, 12);
	}
	for (i = 0; i < len; i++) {
		c = fontframe[gbFontTransTbl[(uint8_t)str[i]]];
		k += fontkern[c] + 1;
		if (c && k <= 257) {
			PrintChar({ sx, sy }, c, col);
		}
		sx += fontkern[c] + 1;
	}
	if (qline == y) {
		CelDraw(cjustflag ? x + k + 100 : 340 - x, sy + 1, pSPentSpn2Cels, PentSpn2Frame, 12);
	}
}

void DrawQuestLog()
{
	int y, i;

	PrintQLString(0, 2, true, "Quest Log", 3);
	CelDraw(SCREEN_X, SCREEN_Y + 351, pQLogCel, 1, SPANEL_WIDTH);
	y = qtopline;
	for (i = 0; i < numqlines; i++) {
		PrintQLString(0, y, true, questlist[qlist[i]]._qlstr, 0);
		y += 2;
	}
	PrintQLString(0, 22, true, "Close Quest Log", 0);
	PentSpn2Spin();
}

void StartQuestlog()
{
	DWORD i;

	numqlines = 0;
	for (i = 0; i < MAXQUESTS; i++) {
		if (quests[i]._qactive == QUEST_ACTIVE && quests[i]._qlog) {
			qlist[numqlines] = i;
			numqlines++;
		}
	}
	if (numqlines > 5) {
		qtopline = 5 - (numqlines >> 1);
	} else {
		qtopline = 8;
	}
	qline = 22;
	if (numqlines != 0)
		qline = qtopline;
	questlog = true;
	PentSpn2Frame = 1;
}

void QuestlogUp()
{
	if (numqlines) {
		if (qline == qtopline) {
			qline = 22;
		} else if (qline == 22) {
			qline = qtopline + 2 * numqlines - 2;
		} else {
			qline -= 2;
		}
		PlaySFX(IS_TITLEMOV);
	}
}

void QuestlogDown()
{
	if (numqlines) {
		if (qline == 22) {
			qline = qtopline;
		} else if (qline == qtopline + 2 * numqlines - 2) {
			qline = 22;
		} else {
			qline += 2;
		}
		PlaySFX(IS_TITLEMOV);
	}
}

void QuestlogEnter()
{
	PlaySFX(IS_TITLSLCT);
	if (numqlines && qline != 22)
		InitQTextMsg(quests[qlist[(qline - qtopline) >> 1]]._qmsg);
	questlog = false;
}

void QuestlogESC()
{
	int y, i;

	y = (Mouse.y - 32) / 12;
	if (numqlines) {
		for (i = 0; i < numqlines; i++) {
			if (y == qtopline + 2 * i) {
				qline = y;
				QuestlogEnter();
			}
		}
	}
	if (y == 22) {
		qline = 22;
		QuestlogEnter();
	}
}

void SetMultiQuest(int q, int s, int l, int v1)
{
	if (quests[q]._qactive != QUEST_DONE) {
		if (s > quests[q]._qactive)
			quests[q]._qactive = s;
		quests[q]._qlog |= l;
		if (v1 > quests[q]._qvar1)
			quests[q]._qvar1 = v1;
	}
}

DEVILUTION_END_NAMESPACE
