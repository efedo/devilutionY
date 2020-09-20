//HEADER_GOES_HERE
#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <string>
#include <array>
#include <vector>

DEVILUTION_BEGIN_NAMESPACE

class Player {
public:
	Player(int newpnum);
	const int id() const;
	operator int() const;
	void ChangePlayerPos(const V2Di newpos)
	{
		grid.movePlayer(id(), data._pos, newpos);
		data._pos = newpos;
	}
	const V2Di &pos() const
	{
		return data._pos;
	}

	bool isLocal();
	void LoadPlrGFX(player_graphic gfxflag);
	void InitPlayerGFX();
	void InitPlrGFXMem();
	void FreePlayerGFX();
	void NewPlrAnim(BYTE *Peq, int numFrames, int Delay, int width);
	void ClearPlrPVars();
	void SetPlrAnims();
	void ClearPlrRVars();
	void CreatePlayer(char c);
	int CalcStatDiff();
	void NextPlrLevel();
	void AddPlrExperience(int lvl, int exp);
	void ValidatePlayer(); // New
	void ProcessPlayer(); // New
	void InitPlayer(BOOL FirstTime);
	void CheckEFlag(BOOL flag);
	BOOL PlrDirOK(Dir dir);
	void SetPlayerOld();
	void FixPlayerLocation(Dir bDir);
	void StartStand(Dir dir);
	void StartWalkStand();
	void PM_ChangeLightOff();
	void PM_ChangeOffset();
	void StartWalk(Dir);
	void StartWalk(V2Di vel, V2Di add, Dir EndDir, ScrollDir sdir);
	void StartWalk2(V2Di vel, V2Di off, V2Di add, Dir EndDir, ScrollDir sdir);
	void StartWalk3(V2Di vel, V2Di off, V2Di add, V2Di map, Dir EndDir, ScrollDir sdir);
	void StartAttack(Dir d);
	void StartRangeAttack(Dir d, V2Di c);
	void StartPlrBlock(Dir dir);
	void StartSpell(Dir d, V2Di c);
	void FixPlrWalkTags();
	void RemovePlrFromMap();
	void StartPlrHit(int dam, BOOL forcehit);
	void StartPlayerKill(int earflag);
	void PlrDeadItem(ItemStruct *itm, V2Di n);
	void DropHalfPlayersGold();
	void SyncPlrKill(int earflag);
	void j_StartPlayerKill(int earflag);
	void RemovePlrMissiles();
	void InitLevelChange();
	void StartNewLvl(int fom, int lvl);
	void RestartTownLvl();
	void StartWarpLvl(int pidx);
	BOOL PM_DoStand();
	BOOL PM_DoNewLvl();
	BOOL PM_DoWalk();
	BOOL PM_DoWalk2();
	BOOL PM_DoWalk3();
	BOOL WeaponDur(int durrnd);
	BOOL PlrHitMonst(int m);
	BOOL PlrHitPlr(char p);
	BOOL PlrHitObj(V2Di m);
	BOOL PM_DoAttack();
	BOOL PM_DoRangeAttack();
	void ShieldDur();
	BOOL PM_DoBlock();
	BOOL PM_DoSpell();
	BOOL PM_DoGotHit();
	void ArmorDur();
	BOOL PM_DoDeath();
	void CheckNewPath();
	void ClrPlrPath();
	BOOL PosOkPlayer(V2Di pos);
	void MakePlrPath(V2Di pos, BOOL endspace);
	void SyncPlrAnim();
	void SyncInitPlrPos();
	void SyncInitPlr();
	void InitDungMsgs();

	void CheckStats();
	void CheckCheatStats();
	void ModifyPlrStr(int l);
	void ModifyPlrMag(int l);
	void ModifyPlrDex(int l);
	void ModifyPlrVit(int l);
	void SetPlrStr(int v);
	void SetPlrMag(int v);
	void SetPlrDex(int v);
	void SetPlrVit(int v);
	void SetPlayerHitPoints(int val);

	PlayerStruct data;
	PlayerInventory inventory;
private:
	int pnum;
};

Player & myplr();

class PlayerManager {
public:
	PlayerManager()
	{
		list.push_back(Player(0));
		list.push_back(Player(1));
		list.push_back(Player(2));
		list.push_back(Player(3));
	}

	auto &operator[](size_t p)
	{
		if ((DWORD)p >= MAX_PLRS) {
			app_fatal("Illegal player %d", p);
		}
		return list[p];
	}

	void setLocal(int newlocal)
	{
		myplr = newlocal;
	}
	bool isValidPlayer(int number)
	{
		if (number < 0) return false;
		if (number >= list.size()) return false;
		return true;
	}
private:
	friend Player & myplr();

	auto &_local()
	{
		return operator[](myplr);
	}

	std::vector<Player> list;
	int myplr;
};

extern PlayerManager plr;

BOOL PosOkPlayer(int pnum, V2Di pos);
BOOL PlrDeathModeOK(int p);

extern int plr_lframe_size;
extern int plr_wframe_size;
extern BYTE plr_gfx_flag;
extern int plr_aframe_size;
extern int plr_fframe_size;
extern int plr_qframe_size;
extern BOOL deathflag;
extern int plr_hframe_size;
extern int plr_bframe_size;
extern BYTE plr_gfx_bflag;
extern int plr_sframe_size;
extern int deathdelay;
extern int plr_dframe_size;

void SetPlayerGPtrs(BYTE *pData, BYTE **pAnim);
DWORD GetPlrGFXSize(char *szCel);
void AddPlrMonstExper(int lvl, int exp, char pmask);
void InitMultiView();
BOOL SolidLoc(V2Di pos);
void PlrClrTrans(V2Di pos);
void PlrDoTrans(V2Di pos);
void RespawnDeadItem(ItemStruct *itm, V2Di pos);
void ValidatePlayer();
void ProcessPlayers();
void CheckPlrSpell();
void PlayDungMsgs();

/* rdata */

extern const char ArmourChar[4];
extern const char WepChar[10];
extern const char CharChar[];

/* data */

extern V2Di plroff[9];
extern V2Di plroff2[9];

extern int ExpLvlsTbl[MAXCHARLEVEL];
extern BYTE fix[9];

DEVILUTION_END_NAMESPACE

#endif /* __PLAYER_H__ */
