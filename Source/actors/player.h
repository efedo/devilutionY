//HEADER_GOES_HERE
#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <string>
#include <array>
#include <vector>

DEVILUTION_BEGIN_NAMESPACE

class Player {
public:
	Player(int newpnum)
		: pnum(newpnum), inventory(*this)
	{
		memset(&data, 0, sizeof(data)); // 0 out data struct
	}
	const int id() const
	{
		return pnum;
	};

	operator int() const
	{
		return id();
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
	BOOL PlrDirOK(int dir);
	void SetPlayerOld();
	void FixPlayerLocation(int bDir);
	void StartStand(int dir);
	void StartWalkStand();
	void PM_ChangeLightOff();
	void PM_ChangeOffset();
	void StartWalk(V2Di vel, V2Di add, int EndDir, int sdir);
	void StartWalk2(int xvel, int yvel, int xoff, int yoff, int xadd, int yadd, int EndDir, int sdir);
	void StartWalk3(int xvel, int yvel, int xoff, int yoff, int xadd, int yadd, int mapx, int mapy, int EndDir, int sdir);
	void StartAttack(int d);
	void StartRangeAttack(int d, int cx, int cy);
	void StartPlrBlock(int dir);
	void StartSpell(int d, int cx, int cy);
	void FixPlrWalkTags();
	void RemovePlrFromMap();
	void StartPlrHit(int dam, BOOL forcehit);
	void StartPlayerKill(int earflag);
	void PlrDeadItem(ItemStruct *itm, int xx, int yy);
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
	BOOL PlrHitObj(int mx, int my);
	BOOL PM_DoAttack();
	BOOL PM_DoRangeAttack();
	void ShieldDur();
	BOOL PM_DoBlock();
	BOOL PM_DoSpell();
	BOOL PM_DoGotHit();
	void ArmorDur();
	BOOL PM_DoDeath();
	void CheckNewPath();
	void CheckCheatStats();
	void ClrPlrPath();
	BOOL PosOkPlayer(V2Di pos);
	void MakePlrPath(V2Di pos, BOOL endspace);
	void SyncPlrAnim();
	void SyncInitPlrPos();
	void SyncInitPlr();
	void SetPlayerHitPoints(int val);
	void InitDungMsgs();
	void CheckStats();
	void ModifyPlrStr(int l);
	void ModifyPlrMag(int l);
	void ModifyPlrDex(int l);
	void ModifyPlrVit(int l);
	void SetPlrStr(int v);
	void SetPlrMag(int v);
	void SetPlrDex(int v);
	void SetPlrVit(int v);
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

extern int plrxoff[9]; // stores x and y offsets for players independently for multiplayer for some reason
						// 9 slots even though only 4 players max (assuming higher slots are not used?)
extern int plryoff[9];
extern int plrxoff2[9];
extern int plryoff2[9];


extern int ExpLvlsTbl[MAXCHARLEVEL];
extern BYTE fix[9];

DEVILUTION_END_NAMESPACE

#endif /* __PLAYER_H__ */
