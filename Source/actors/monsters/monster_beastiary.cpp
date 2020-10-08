/**
 * @file monster_beastiary.cpp
 *
 * Monster beastiary class (manages monster types)
 */
#include "all.h"
#include "../3rdParty/Storm/Source/storm.h"
#include <vector>

namespace dvl {

Beastiary beastiary;

Beastiary::Beastiary()
{
}

void Beastiary::InitLevelMonsters()
{
	monstimgtot = 0;
	MissileFileFlag = 0;
	for (int i = 0; i < MAX_LVLMTYPES; i++) {
		types[i].data.mPlaceFlags = 0;
	}
	actors.clearAllMonsters();
	uniquetrans = 0;
}

void Beastiary::AddMonsterType(MonsterType type, int placeflag)
{
	MonsterClass& cls = getClass(type);
	if (cls.data.mtype != type) {
		cls.data.mtype = type;
		monstimgtot += cls.cdata().mImage;
		cls.InitMonsterGFX();
		cls.InitMonsterSND();
	}
	cls.data.mPlaceFlags |= placeflag;
}

void Beastiary::GetLevelMTypes()
{
	AddMonsterType(MonsterType::GOLEM, 2);
	if (lvl.currlevel == 16) {
		AddMonsterType(MonsterType::ADVOCATE, 1);
		AddMonsterType(MonsterType::RBLACK, 1);
		AddMonsterType(MonsterType::DIABLO, 2);
		return;
	}

	if (!lvl.setlevel) {
		if (QuestStatus(QuestId::butcher))	AddMonsterType(MonsterType::CLEAVER, 2);
		if (QuestStatus(QuestId::garbud))	AddMonsterType(getUnique(UniqueMonsterType::GARBUD).mtype, 4);
		if (QuestStatus(QuestId::zhar))		AddMonsterType(getUnique(UniqueMonsterType::ZHAR).mtype, 4);
		if (QuestStatus(QuestId::ltbanner)) AddMonsterType(getUnique(UniqueMonsterType::SNOTSPIL).mtype, 4);
		if (QuestStatus(QuestId::veil))		AddMonsterType(getUnique(UniqueMonsterType::LACHDAN).mtype, 4);
		if (QuestStatus(QuestId::warlord))	AddMonsterType(getUnique(UniqueMonsterType::WARLORD).mtype, 4);

		// Add extra random skeleton type if multiplayer and skelking level
		if (game.isMultiplayer() && lvl.currlevel == quests[int(QuestId::skelking)]._qlevel) {
			MonsterType skeltype = getRandomActiveSkelType(true);
			AddMonsterType(skeltype, 1);
		}

		// Add standard monsters
		const char mamask = 3; // monster availability mask
		std::vector<MonsterType> typelist;
		for (int i = 0; i < numTypes(); i++) {
			int minl = 15 * monsterdata[i].mMinDLvl / 30 + 1;
			int maxl = 15 * monsterdata[i].mMaxDLvl / 30 + 1;
			if (lvl.currlevel >= minl && lvl.currlevel <= maxl) {
				if (MonstAvailTbl[i] & mamask) {
					typelist.push_back(MonsterType(i));
				}
			}
		}
		if (monstdebug) {
			for (int i = 0; i < debugmonsttypes; i++) {
				AddMonsterType(MonsterType(DebugMonsters[i]), 1);
			}
		} else {
			// Used to check that not too much image memory used
			std::shuffle(typelist.begin(), typelist.end(), randMt());
			for (int i = 0; i < MAX_LVLMTYPES && i < typelist.size(); ++i) {
				AddMonsterType(typelist[i], 1);
			}
		}
	} else { // is set level
		if (lvl.setlvlnum == SetLvl::SkelKing) {
			AddMonsterType(MonsterType::SKING, 4);
		}
	}
}

MonsterClass& Beastiary::getClass(MonsterType type) {
	return types[int(type)];
}

const UniqMonstStruct& Beastiary::getUnique(UniqueMonsterType uniquetype) {
	return UniqMonst[int(uniquetype)];
}

MonsterType Beastiary::convertDunFileMonType(uint8_t duntype) {
	return MonstConvTbl[duntype];
}

MonsterType Beastiary::getRandomActiveSkelType() {

}

void Beastiary::FreeMonsterSnd()
{
	for (MonsterClass & mcls : types) {
		MonsterType mtype = mcls.data.mtype;
		for (int j = 0; j < 4; ++j) {
			for (int k = 0; k < 2; ++k) {
				TSnd* pSnd = mcls.data.Snds[j][k];
				if (pSnd) {
					mcls.data.Snds[j][k] = NULL;
					char* file = pSnd->sound_path;
					pSnd->sound_path = NULL;
					sound_file_cleanup(pSnd);
					mem_free_dbg(file);
				}
			}
		}
	}
}

void Beastiary::LoadDiabMonsts()
{
	uint8_t* lpSetPiece;
	lpSetPiece = LoadFileInMem("Levels\\L4Data\\diab1.DUN", NULL);
	SetMapMonsters(lpSetPiece, { 2 * diabquad1x, 2 * diabquad1y });
	mem_free_dbg(lpSetPiece);
	lpSetPiece = LoadFileInMem("Levels\\L4Data\\diab2a.DUN", NULL);
	SetMapMonsters(lpSetPiece, { 2 * diabquad2x, 2 * diabquad2y });
	mem_free_dbg(lpSetPiece);
	lpSetPiece = LoadFileInMem("Levels\\L4Data\\diab3a.DUN", NULL);
	SetMapMonsters(lpSetPiece, { 2 * diabquad3x, 2 * diabquad3y });
	mem_free_dbg(lpSetPiece);
	lpSetPiece = LoadFileInMem("Levels\\L4Data\\diab4a.DUN", NULL);
	SetMapMonsters(lpSetPiece, { 2 * diabquad4x, 2 * diabquad4y });
	mem_free_dbg(lpSetPiece);
}

void Beastiary::InitMonsters()
{
	if (game.isMultiplayer()) CheckDungeonClear();
	if (!lvl.setlevel) {
		AddMonster({ 1, 0 }, Dir(0), MonsterType::INVALID, false);
		AddMonster({ 1, 0 }, Dir(0), MonsterType::INVALID, false);
		AddMonster({ 1, 0 }, Dir(0), MonsterType::INVALID, false);
		AddMonster({ 1, 0 }, Dir(0), MonsterType::INVALID, false);
	}
	if (!lvl.setlevel && lvl.currlevel == 16) LoadDiabMonsts();
	int nt = numtrigs;
	if (lvl.currlevel == 15) nt = 1;
	for (int i = 0; i < nt; i++) {
		for (int s = -2; s < 2; s++) {
			for (int t = -2; t < 2; t++)
				DoVision({ s + trigs[i]._t.x, t + trigs[i]._t.y }, 15, false, false);
		}
	}
	actors.PlaceQuestMonsters();
	if (!lvl.setlevel) {
		int scattertypes[200];
		PlaceUniques();
		int na = 0;
		for (int s = 16; s < 96; s++)
			for (int t = 16; t < 96; t++)
				if (!grid.at({ s, t }).isSolid())
					na++;
		int numplacemonsters = na / 30;
		if (game.isMultiplayer()) numplacemonsters += numplacemonsters >> 1;
		const int nummonsters = actors.numMonsters();
		if (nummonsters + numplacemonsters > 190)
			numplacemonsters = 190 - nummonsters;
		const int totalmonsters = nummonsters + numplacemonsters;
		int numscattypes = 0;

		for (int i = 0; i < numTypes(); i++) {
			if (beastiary[i].data.mPlaceFlags & 1) {
				scattertypes[numscattypes] = i;
				numscattypes++;
			}
		}
		int mtype;
		while (nummonsters < totalmonsters) {
			mtype = scattertypes[random_(95, numscattypes)];
			if (lvl.currlevel == 1 || random_(95, 2) == 0) {
				na = 1;
			}
			else if (lvl.currlevel == 2) {
				na = random_(95, 2) + 2;
			}
			else {
				na = random_(95, 3) + 3;
			}
			actors.PlaceGroup(mtype, na, 0, 0);
		}
	}
	for (int i = 0; i < nt; i++) {
		for (int s = -2; s < 2; s++) {
			for (int t = -2; t < 2; t++)
				DoUnVision({ s + trigs[i]._t.x, t + trigs[i]._t.y }, 15);
		}
	}
}

void Beastiary::PlaceUniques()
{
	int u, mt;
	bool done;

	for (u = 0; UniqMonst[u].mtype != -1; u++) {
		if (UniqMonst[u].mlevel != lvl.currlevel)
			continue;
		done = false;
		for (mt = 0; mt < nummtypes; mt++) {
			if (done)
				break;
			done = (beastiary[mt].data.mtype == UniqMonst[u].mtype);
		}
		mt--;
		if (u == UniqueMonsterType::GARBUD && quests[QuestId::garbud]._qactive == QuestState::not_available)
			done = false;
		if (u == UniqueMonsterType::ZHAR && quests[QuestId::zhar]._qactive == QuestState::not_available)
			done = false;
		if (u == UniqueMonsterType::SNOTSPIL && quests[QuestId::ltbanner]._qactive == QuestState::not_available)
			done = false;
		if (u == UniqueMonsterType::LACHDAN && quests[QuestId::veil]._qactive == QuestState::not_available)
			done = false;
		if (u == UniqueMonsterType::WARLORD && quests[QuestId::warlord]._qactive == QuestState::not_available)
			done = false;
		if (done)
			monsters.PlaceUniqueMonster(u, mt, 8);
	}
}

void Beastiary::SetMapMonsters(uint8_t* pMap, V2Di start)
{
	WORD rw, rh;
	WORD* lm;
	int i, j;
	int mtype;

	beastiary.AddMonsterType(MonsterType::GOLEM, 2);
	AddMonster({ 1, 0 }, Dir(0), 0, false);
	AddMonster({ 1, 0 }, Dir(0), 0, false);
	AddMonster({ 1, 0 }, Dir(0), 0, false);
	AddMonster({ 1, 0 }, Dir(0), 0, false);
	if (lvl.setlevel && lvl.setlvlnum == SetLvl::VileBetrayer) {
		beastiary.AddMonsterType(UniqMonst[UniqueMonsterType::LAZURUS].mtype, 4);
		beastiary.AddMonsterType(UniqMonst[UniqueMonsterType::RED_VEX].mtype, 4);
		beastiary.AddMonsterType(UniqMonst[UniqueMonsterType::BLACKJADE].mtype, 4);
		monsters.PlaceUniqueMonster(UniqueMonsterType::LAZURUS, 0, 0);
		monsters.PlaceUniqueMonster(UniqueMonsterType::RED_VEX, 0, 0);
		monsters.PlaceUniqueMonster(UniqueMonsterType::BLACKJADE, 0, 0);
	}
	lm = (WORD*)pMap;
	rw = SDL_SwapLE16(*lm);
	lm++;
	rh = SDL_SwapLE16(*lm);
	lm += (rw * rh + 1);
	rw = rw << 1;
	rh = rh << 1;
	lm += rw * rh;

	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*lm) {
				mtype = beastiary.AddMonsterType(MonstConvTbl[SDL_SwapLE16(*lm) - 1], 2);
				monsters.PlaceMonster(nummonsters++, mtype, { i + start.x + 16, j + start.y + 16 });
			}
			lm++;
		}
	}
}

void Beastiary::FreeMonsters()
{
	for (int i = 0; i < nummtypes; i++) {
		int mtype = beastiary[i].data.mtype;
		for (int j = 0; j < 6; j++) {
			if (animletter[j] != 's' || monsterdata[mtype].has_special) {
				MemFreeDbg(beastiary[i].data.Anims[j].CMem);
			}
		}
	}
	FreeMissiles2();
}

// Restricted to specified active levels?
MonsterType Beastiary::getRandomActiveSkelType(bool lvl_restrict) {
	const char mamask = 3; // monster availability mask
	std::vector<MonsterType> skeltypes;
	for (int i = int(MonsterType::WSKELAX); i <= int(MonsterType::XSKELSD); i++) {
		if (getClass(MonsterType(i)).IsSkel()) {
			int minl = 15 * monsterdata[i].mMinDLvl / 30 + 1;
			int maxl = 15 * monsterdata[i].mMaxDLvl / 30 + 1;
			if (lvl.currlevel >= minl && lvl.currlevel <= maxl) {
				if (MonstAvailTbl[i] & mamask) {
					skeltypes.push_back(MonsterType(i));
				}
			}
		}
	}
	if (skeltypes.empty()) return MonsterType::INVALID;
	return skeltypes[random_(88, (skeltypes.size() - 1))];
}

}
