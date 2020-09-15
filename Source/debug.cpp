/**
 * @file debug.cpp
 *
 * Implementation of debug functions.
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

#ifdef _DEBUG
BOOL update_seed_check = FALSE;
#endif

#define DEBUGSEEDS 4096
int seed_index;
int level_seeds[NUMLEVELS + 1];
int seed_table[DEBUGSEEDS];

BYTE *pSquareCel;
char dMonsDbg[NUMLEVELS][MAXDUNX][MAXDUNY];
char dFlagDbg[NUMLEVELS][MAXDUNX][MAXDUNY];

void LoadDebugGFX()
{
	if (visiondebug)
		pSquareCel = LoadFileInMem("Data\\Square.CEL", NULL);
}

void FreeDebugGFX()
{
	MemFreeDbg(pSquareCel);
}

void CheckDungeonClear()
{
	int i, j;

	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++) {
			if (grid[i][j].dMonster != 0)
				app_fatal("Monsters not cleared");
			if (grid[i][j].dPlayer != 0)
				app_fatal("Players not cleared");

			dMonsDbg[level.currlevel][i][j] = grid[i][j].dFlags & BFLAG_VISIBLE;
			dFlagDbg[level.currlevel][i][j] = grid[i][j].dFlags & BFLAG_POPULATED;
		}
	}
}

#ifdef _DEBUG
void GiveGoldCheat()
{
	int i, ni;

	for (i = 0; i < NUM_INV_GRID_ELEM; i++) {
		if (!myplr().data.InvGrid[i]) {
			ni = myplr().data._pNumInv++;
			SetPlrHandItem(&myplr().data.InvList[ni], IDI_GOLD);
			GetPlrHandSeed(&myplr().data.InvList[ni]);
			myplr().data.InvList[ni]._ivalue = GOLD_MAX_LIMIT;
			myplr().data.InvList[ni]._iCurs = ICURS_GOLD_LARGE;
			myplr().data._pGold += GOLD_MAX_LIMIT;
			myplr().data.InvGrid[i] = myplr().data._pNumInv;
		}
	}
}

void StoresCheat()
{
	int i;

	numpremium = 0;

	for (i = 0; i < SMITH_PREMIUM_ITEMS; i++)
		premiumitem[i]._itype = ITYPE_NONE;

	SpawnPremium(30);

	for (i = 0; i < 20; i++)
		witchitem[i]._itype = ITYPE_NONE;

	SpawnWitch(30);
}

void TakeGoldCheat()
{
	int i;
	char ig;

	for (i = 0; i < NUM_INV_GRID_ELEM; i++) {
		ig = myplr().data.InvGrid[i];
		if (ig > 0 && myplr().data.InvList[ig - 1]._itype == ITYPE_GOLD)
			myplr().inventory.RemoveInvItem(ig - 1);
	}

	for (i = 0; i < MAXBELTITEMS; i++) {
		if (myplr().data.SpdList[i]._itype == ITYPE_GOLD)
			myplr().data.SpdList[i]._itype = ITYPE_NONE;
	}

	myplr().data._pGold = 0;
}

void MaxSpellsCheat()
{
	int i;

	for (i = 1; i < MAX_SPELLS; i++) {
		if (spelldata[i].sBookLvl != -1) {
			myplr().data._pMemSpells |= (__int64)1 << (i - 1);
			myplr().data._pSplLvl[i] = 10;
		}
	}
}

void SetSpellLevelCheat(char spl, int spllvl)
{
	myplr().data._pMemSpells |= (__int64)1 << (spl - 1);
	myplr().data._pSplLvl[spl] = spllvl;
}

void SetAllSpellsCheat()
{
	SetSpellLevelCheat(SPL_FIREBOLT, 8);
	SetSpellLevelCheat(SPL_CBOLT, 11);
	SetSpellLevelCheat(SPL_HBOLT, 10);
	SetSpellLevelCheat(SPL_HEAL, 7);
	SetSpellLevelCheat(SPL_HEALOTHER, 5);
	SetSpellLevelCheat(SPL_LIGHTNING, 9);
	SetSpellLevelCheat(SPL_FIREWALL, 5);
	SetSpellLevelCheat(SPL_TELEKINESIS, 3);
	SetSpellLevelCheat(SPL_TOWN, 3);
	SetSpellLevelCheat(SPL_FLASH, 3);
	SetSpellLevelCheat(SPL_RNDTELEPORT, 2);
	SetSpellLevelCheat(SPL_MANASHIELD, 2);
	SetSpellLevelCheat(SPL_WAVE, 4);
	SetSpellLevelCheat(SPL_FIREBALL, 3);
	SetSpellLevelCheat(SPL_STONE, 1);
	SetSpellLevelCheat(SPL_CHAIN, 1);
	SetSpellLevelCheat(SPL_GUARDIAN, 4);
	SetSpellLevelCheat(SPL_ELEMENT, 3);
	SetSpellLevelCheat(SPL_NOVA, 1);
	SetSpellLevelCheat(SPL_GOLEM, 2);
	SetSpellLevelCheat(SPL_FLARE, 1);
	SetSpellLevelCheat(SPL_BONESPIRIT, 1);
}

void PrintDebugPlayer(BOOL bNextPlayer)
{
	char dstr[128];

	if (bNextPlayer)
		dbgplr = ((BYTE)dbgplr + 1) & 3;

	sprintf(dstr, "Plr %i : Active = %i", dbgplr, plr[dbgplr].data.plractive);
	NetSendCmdString(1 << myplr(), dstr);

	if (plr[dbgplr].data.plractive) {
		sprintf(dstr, "  Plr %i is %s", dbgplr, plr[dbgplr].data._pName);
		NetSendCmdString(1 << myplr(), dstr);
		sprintf(dstr, "  Lvl = %i : Change = %i", plr[dbgplr].data.plrlevel, plr[dbgplr].data._pLvlChanging);
		NetSendCmdString(1 << myplr(), dstr);
		sprintf(dstr, "  x = %i, y = %i : tx = %i, ty = %i", plr[dbgplr].data._px, plr[dbgplr].data._py, plr[dbgplr].data._ptargx, plr[dbgplr].data._ptargy);
		NetSendCmdString(1 << myplr(), dstr);
		sprintf(dstr, "  mode = %i : daction = %i : walk[0] = %i", plr[dbgplr].data._pmode, plr[dbgplr].data.destAction, plr[dbgplr].data.walkpath[0]);
		NetSendCmdString(1 << myplr(), dstr);
		sprintf(dstr, "  inv = %i : hp = %i", plr[dbgplr].data._pInvincible, plr[dbgplr].data._pHitPoints);
		NetSendCmdString(1 << myplr(), dstr);
	}
}

void PrintDebugQuest()
{
	char dstr[128];

	sprintf(dstr, "Quest %i :  Active = %i, Var1 = %i", dbgqst, quests[dbgqst]._qactive, quests[dbgqst]._qvar1);
	NetSendCmdString(1 << myplr(), dstr);

	dbgqst++;
	if (dbgqst == MAXQUESTS)
		dbgqst = 0;
}

void PrintDebugMonster(int m)
{
	BOOL bActive;
	int i;
	char dstr[128];

	sprintf(dstr, "Monster %i = %s", m, monsters[m].data.mName);
	NetSendCmdString(1 << myplr(), dstr);
	sprintf(dstr, "X = %i, Y = %i", monsters[m].data._mx, monsters[m].data._my);
	NetSendCmdString(1 << myplr(), dstr);
	sprintf(dstr, "Enemy = %i, HP = %i", monsters[m].data._menemy, monsters[m].data._mhitpoints);
	NetSendCmdString(1 << myplr(), dstr);
	sprintf(dstr, "Mode = %i, Var1 = %i", monsters[m].data._mmode, monsters[m].data._mVar1);
	NetSendCmdString(1 << myplr(), dstr);

	bActive = FALSE;

	for (i = 0; i < nummonsters; i++) {
		if (monstactive[i] == m)
			bActive = TRUE;
	}

	sprintf(dstr, "Active List = %i, Squelch = %i", bActive, monsters[m].data._msquelch);
	NetSendCmdString(1 << myplr(), dstr);
}

void GetDebugMonster()
{
	int mi1, mi2;

	mi1 = pcursmonst;
	if (mi1 == -1) {
		mi2 = grid[cursmx][cursmy].dMonster;
		if (mi2 != 0) {
			mi1 = mi2 - 1;
			if (mi2 <= 0)
				mi1 = -1 - mi2;
		} else {
			mi1 = dbgmon;
		}
	}
	PrintDebugMonster(mi1);
}

void NextDebugMonster()
{
	char dstr[128];

	dbgmon++;
	if (dbgmon == MAXMONSTERS)
		dbgmon = 0;

	sprintf(dstr, "Current debug monster = %i", dbgmon);
	NetSendCmdString(1 << myplr(), dstr);
}
#endif

DEVILUTION_END_NAMESPACE
