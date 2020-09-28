//HEADER_GOES_HERE
#ifndef __PLAYER_INSTANCE_H__
#define __PLAYER_INSTANCE_H__

#include <string>
#include <array>
#include <vector>

DEVILUTION_BEGIN_NAMESPACE

template <class T>
class Stat {
	T get();
	void set(T);
	void setMin(T);
	void setMax(T);
private:
	T val;
	T min;
	T max;
};

class Player : public Actor {
public:
	// Init
	Player(int newpnum);
	~Player();
	void CreatePlayer(char c);
	void InitPlayer(bool FirstTime);

	// Misc
	const int id() const;
	operator int() const;
	bool isMyPlr();

	// Position control (and grid synchronization)
	bool PosOkPlayer(V2Di pos);
	bool PlrDirOK(Dir dir);
	V2Di pos() const;
	V2Di futpos() const;
	V2Di drawpos() const;
	void changePos(const V2Di newpos); // Changes current position
	void changeFutPos(const V2Di newpos); // Changes future position
	void changeDrawPos(const V2Di newpos); // Changes future position
	void _changePosOffMap(const V2Di newpos); // Unsafe, used to assign pos when map not fully set up
	void _changeFutPosOffMap(const V2Di newpos); // Unsafe, used to assign future pos when map not fully set up
	void advancePos(); // Makes current position = future position
	void addToMap(); // Adds the player to the map (upon level change/game entry)
	void RemoveFromMap();
	void SetPlayerOld();
	void PlantInPlace(Dir bDir);

	// Graphics storage/setup
	void LoadPlrGFX(player_graphic gfxflag);
	void InitPlayerGFX();
	void InitPlrGFXMem();
	void FreePlayerGFX();
	void SetPlrAnims();

	// Animation
	void NewPlrAnim(uint8_t *Peq, int numFrames, int Delay, int width); // Plays an animation
	void ClearPlrPVars();
	void ClearPlrRVars();

	// Combat
	void StartAttack(Dir d);
	void StartRangeAttack(Dir d, V2Di c);
	void StartPlrBlock(Dir dir);
	void StartSpell(Dir d, V2Di c);
	void StartPlrHit(int dam, bool forcehit);
	void StartPlayerKill(int earflag);
	bool PM_DoAttack();
	bool PM_DoRangeAttack();
	bool PM_DoBlock();
	bool PM_DoSpell();
	bool PM_DoGotHit();
	bool PM_DoDeath();
	bool PlrHitMonst(int m);
	bool PlrHitPlr(char p);
	bool PlrHitObj(V2Di m);
	void j_StartPlayerKill(int earflag);
	void RemovePlrMissiles();

	void AddPlrExperience(int lvl, int exp);
	void NextPlrLevel();
	void ValidatePlayer(); // New
	void ProcessPlayer(); // New
	void CheckEFlag(bool flag);
	void PM_ChangeLightOff();
	void PM_ChangeOffset();
	void PlrDeadItem(ItemStruct *itm, V2Di n);
	void DropHalfPlayersGold();
	void InitDungMsgs();
	void RestartTownLvl();
	bool PM_DoNewLvl();
	void InitLevelChange();
	void StartNewLvl(int fom, int lvl);
	void StartWarpLvl(int pidx);
	bool WeaponDur(int durrnd);
	void ShieldDur();
	void ArmorDur();

	void SyncPlrKill(int earflag);
	void SyncPlrAnim();
	void SyncInitPlrPos();
	void SyncInitPlr();

	// Path functions
	void MakePlrPath(V2Di pos, bool endspace);
	void CheckNewPath();
	void ClrPlrPath();

	// Walk functions
	void StartWalk(Dir);
	void StartWalk(V2Di vel, V2Di add, Dir EndDir, ScrollDir sdir);
	void StartWalk2(V2Di vel, V2Di off, V2Di add, Dir EndDir, ScrollDir sdir);
	void StartWalk3(V2Di vel, V2Di off, V2Di add, V2Di map, Dir EndDir, ScrollDir sdir);
	void FixPlrWalkTags();
	void StartStand(Dir dir);
	void StartWalkStand();
	bool PM_DoStand();
	bool PM_DoWalk();
	bool PM_DoWalk2();
	bool PM_DoWalk3();

	// Stats functions
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
	int CalcStatDiff();

	PlayerStruct data;
	PlayerInventory inventory;
private:
	int pnum;
};

Player & myplr();

bool PosOkPlayer(int pnum, V2Di pos);
bool PlrDeathModeOK(int p);

extern int plr_lframe_size;
extern int plr_wframe_size;
extern uint8_t plr_gfx_flag;
extern int plr_aframe_size;
extern int plr_fframe_size;
extern int plr_qframe_size;
extern bool deathflag;
extern int plr_hframe_size;
extern int plr_bframe_size;
extern uint8_t plr_gfx_bflag;
extern int plr_sframe_size;
extern int deathdelay;
extern int plr_dframe_size;

void SetPlayerGPtrs(uint8_t *pData, uint8_t **pAnim);
DWORD GetPlrGFXSize(char *szCel);
void AddPlrMonstExper(int lvl, int exp, char pmask);
void InitMultiView();
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
extern uint8_t fix[9];

DEVILUTION_END_NAMESPACE

#endif /* __PLAYER_INSTANCE_H__ */
