//HEADER_GOES_HERE
#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <string>
#include <array>
#include <vector>

DEVILUTION_BEGIN_NAMESPACE

//#ifdef __cplusplus
//extern "C" {
//#endif

class Player {
public:
	Player(int newpnum) : pnum(newpnum), inventory(*this) {}
	const int id() { return pnum; };
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
	void StartWalk(int xvel, int yvel, int xadd, int yadd, int EndDir, int sdir);
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
	BOOL PosOkPlayer(int x, int y);
	void MakePlrPath(int xx, int yy, BOOL endspace);
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
	auto &local()
	{
		return operator[](myplr);
	}
	void setLocal(int newlocal)
	{
		myplr = newlocal;
	}

private:
	std::vector<Player> list;
	int myplr;
};

extern PlayerManager plr;

int myplr();

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

//void InitPlayerGFX(int pnum);
//void InitPlrGFXMem(int pnum);
//void FreePlayerGFX(int pnum);
//void NewPlrAnim(int pnum, BYTE *Peq, int numFrames, int Delay, int width);
//void ClearPlrPVars(int pnum);
//void SetPlrAnims(int pnum);
//void ClearPlrRVars(PlayerStruct *p);
//void CreatePlayer(int pnum, char c);
//int CalcStatDiff(int pnum);
//void NextPlrLevel(int pnum);
//void AddPlrExperience(int pnum, int lvl, int exp);



//void InitPlayer(int pnum, BOOL FirstTime);
//BOOL PlrDirOK(int pnum, int dir);
//void SetPlayerOld(int pnum);
//void FixPlayerLocation(int pnum, int bDir);
//void StartStand(int pnum, int dir);
//void StartWalkStand(int pnum);
//void PM_ChangeLightOff(int pnum);
//void PM_ChangeOffset(int pnum);
//void StartWalk(int pnum, int xvel, int yvel, int xadd, int yadd, int EndDir, int sdir);
//void StartWalk2(int pnum, int xvel, int yvel, int xoff, int yoff, int xadd, int yadd, int EndDir, int sdir);
//void StartWalk3(int pnum, int xvel, int yvel, int xoff, int yoff, int xadd, int yadd, int mapx, int mapy, int EndDir, int sdir);
//void StartAttack(int pnum, int d);
//void StartRangeAttack(int pnum, int d, int cx, int cy);
//void StartPlrBlock(int pnum, int dir);
//void StartSpell(int pnum, int d, int cx, int cy);
//void FixPlrWalkTags(int pnum);
//void RemovePlrFromMap(int pnum);
//void StartPlrHit(int pnum, int dam, BOOL forcehit);
//void StartPlayerKill(int pnum, int earflag);
//void PlrDeadItem(int pnum, ItemStruct *itm, int xx, int yy);
//void DropHalfPlayersGold(int pnum);
//void SyncPlrKill(int pnum, int earflag);
//void j_StartPlayerKill(int pnum, int earflag);
//void RemovePlrMissiles(int pnum);
//void InitLevelChange(int pnum);
//void StartNewLvl(int pnum, int fom, int lvl);
//void RestartTownLvl(int pnum);
//void StartWarpLvl(int pnum, int pidx);
//BOOL PM_DoStand(int pnum);
//BOOL PM_DoNewLvl(int pnum);
//BOOL PM_DoWalk(int pnum);
//BOOL PM_DoWalk2(int pnum);
//BOOL PM_DoWalk3(int pnum);
//BOOL WeaponDur(int pnum, int durrnd);
//BOOL PlrHitMonst(int pnum, int m);
//BOOL PlrHitPlr(int pnum, char p);
//BOOL PlrHitObj(int pnum, int mx, int my);
//BOOL PM_DoAttack(int pnum);
//BOOL PM_DoRangeAttack(int pnum);
//void ShieldDur(int pnum);
//BOOL PM_DoBlock(int pnum);
//BOOL PM_DoSpell(int pnum);
//BOOL PM_DoGotHit(int pnum);
//void ArmorDur(int pnum);
//BOOL PM_DoDeath(int pnum);
//void CheckNewPath(int pnum);
//void CheckCheatStats(int pnum);
//void ClrPlrPath(int pnum);
//BOOL PosOkPlayer(int pnum, int x, int y);
//void MakePlrPath(int pnum, int xx, int yy, BOOL endspace);
//void SyncPlrAnim(int pnum);
//void SyncInitPlrPos(int pnum);
//void SyncInitPlr(int pnum);
//void SetPlayerHitPoints(int pnum, int val);
//void InitDungMsgs(int pnum);
BOOL PosOkPlayer(int pnum, int x, int y);


void InitMultiView();

BOOL SolidLoc(int x, int y);
void PlrClrTrans(int x, int y);
void PlrDoTrans(int x, int y);
void RespawnDeadItem(ItemStruct *itm, int x, int y);
BOOL PlrDeathModeOK(int p);
void ValidatePlayer();
void ProcessPlayers();
void CheckPlrSpell();
//void CheckStats(int p);
//void ModifyPlrStr(int p, int l);
//void ModifyPlrMag(int p, int l);
//void ModifyPlrDex(int p, int l);
//void ModifyPlrVit(int p, int l);
//void SetPlrStr(int p, int v);
//void SetPlrMag(int p, int v);
//void SetPlrDex(int p, int v);
//void SetPlrVit(int p, int v);
void PlayDungMsgs();

/* rdata */

extern const char ArmourChar[4];
extern const char WepChar[10];
extern const char CharChar[];

/* data */

extern int plrxoff[9];
extern int plryoff[9];
extern int plrxoff2[9];
extern int plryoff2[9];


// These are all character templates

// need to rename current abs before adding std libraries

class CharType {
public:
	std::array<char, 11> PlrGFXAnimLens;
	int PWVel[3] = { 2048, 1024, 512 };
	int AnimLenFromClass = 8; // Total number of frames in walk animation.
	int strength = 0; // starting strength
	int magic = 0;
	int dexterity = 0;
	int vitality = 0;
	int block = 0;
	std::array<int, 4> MaxStats = {0, 0, 0, 0};
	std::string name = "unnamed";
};

class CharacterTypes {
public:
	const int numclasses = 3;
	CharacterTypes()
	{
		init();
	}
	void init() {
		list[0].name = "Warrior";
		list[0].PlrGFXAnimLens = { 10, 16, 8, 2, 20, 20, 6, 20, 8, 9, 14 };
		list[0].strength = 30;
		list[0].magic = 10;
		list[0].dexterity = 20;
		list[0].vitality = 25;
		list[0].block = 30;
		list[0].MaxStats = { 250, 50, 60, 100 };

		list[1].name = "Rogue";
		list[1].PlrGFXAnimLens = { 8, 18, 8, 4, 20, 16, 7, 20, 8, 10, 12 };
		list[1].strength = 20;
		list[1].magic = 15;
		list[1].dexterity = 30;
		list[1].vitality = 20;
		list[1].block = 20;
		list[1].MaxStats = { 55, 70, 250, 80 };

		list[2].name = "Sorceror";
		list[2].PlrGFXAnimLens = { 8, 16, 8, 6, 20, 12, 8, 20, 8, 12, 8 };
		list[2].strength = 15;
		list[2].magic = 35;
		list[2].dexterity = 15;
		list[2].vitality = 20;
		list[2].block = 10;		
		list[2].MaxStats = { 45, 250, 85, 80 };
	}

	auto &operator[](size_t n)
	{
		return list[n];
	}
private:
	CharType list[3];
};

extern CharacterTypes classes;

extern int ExpLvlsTbl[MAXCHARLEVEL];
extern BYTE fix[9];

//#ifdef __cplusplus
//}
//#endif

DEVILUTION_END_NAMESPACE

#endif /* __PLAYER_H__ */
