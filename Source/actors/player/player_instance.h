//HEADER_GOES_HERE
#ifndef __PLAYER_MANAGER_H__
#define __PLAYER_MANAGER_H__

#include <string>
#include <array>
#include <vector>

#include "inv.h"

DEVILUTION_BEGIN_NAMESPACE

class ItemID;
class Item;

class Player : public Actor, public PlayerStruct {
public:
	// Init
	Player(ActorId);
	~Player();
	void CreatePlayer(PlayerClass c);
	void InitPlayer(bool FirstTime);

	// Misc
	bool isMyPlr();
	Item * heldItem() const;
	DWORD GetPlrGFXSize(char *szCel);

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
	void LoadPlrGFX(PlayerGraphicFileFlags gfxflag);
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
	void StartPlayerKill();
	bool PM_DoAttack();
	bool PM_DoRangeAttack();
	bool PM_DoBlock();
	bool PM_DoSpell();
	bool PM_DoGotHit();
	bool PM_DoDeath();
	bool PlrHitMonst(int m);
	bool PlrHitPlr(Player & target);
	bool PlrHitObj(V2Di m);
	void j_StartPlayerKill();
	void RemovePlrMissiles();

	void AddPlrExperience(int lvl, int exp);
	void NextPlrLevel();
	void ProcessPlayer(); // New
	void CheckEFlag(bool flag);
	void PM_ChangeLightOff();
	void PM_ChangeOffset();
	void PlrDeadItem(Item &, V2Di n);
	void DropHalfPlayersGold();
	void InitDungMsgs();
	void RestartTownLvl();
	bool PM_DoNewLvl();
	void InitLevelChange();
	void StartNewLvl(int fom, int lvl);
	void StartWarpLvl(int pidx);

	void SyncPlrKill();
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

	void damage(int);
	void heal(int);
	void useMana(int);
	void restoreMana(int);
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

	// 'Item' functions
	void CalcPlrItemVals(bool Loadgfx);
	void CalcPlrScrolls();
	void CalcPlrStaff();
	void CalcSelfItems();
	void CalcPlrItemMin();
	void CalcPlrBookVals();
	void CalcPlrInv(bool Loadgfx);
	void CreatePlrItems();
	void UseItem(int Mid, int spl);
	bool DurReduce(BodyLoc loc, int itmclass, int itmtype);
	bool WeaponDur(int durrnd);  // reduces weapon and/or shield dur
	void ShieldDur();            // reduces shield dur
	void ArmorDur();             // reduces armor dur

	// Misc
	Item *HasItem(int item);
	void ValidatePlayer();
	int GetSpellLevel(SpellId sn);
	void CheckPlrSpell();
	bool PlrDeathModeOK();
	void InitMultiView();

	// Not yet fixed
	void CreateRndUseful(V2Di pos, bool sendmsg);
	void CheckIdentify(int cii);
	void DoRepair(int cii);
	void DoRecharge(int cii);

	PlayerInventory inv;

	// Misc data

	static const char ArmourChar[4];
	static const char WepChar[10];
	static const char CharChar[];
	static V2Di plroff[9];
	static V2Di plroff2[9];
	static int ExpLvlsTbl[MAXCHARLEVEL];
	static uint8_t fix[9];

	bool deathflag;
	int deathdelay;

	uint8_t plr_gfx_flag = 0;
	uint8_t plr_gfx_bflag = 0;

	int plr_lframe_size;
	int plr_wframe_size;
	int plr_aframe_size;
	int plr_fframe_size;
	int plr_qframe_size;
	int plr_hframe_size;
	int plr_bframe_size;
	int plr_sframe_size;
	int plr_dframe_size;

private:
	void _updateBaseHp();
	void _updateBaseMana();
	void _updateBaseDmgMod();
	bool _checkDie();
};

void PlayDungMsgs();

Player & myplr();

DEVILUTION_END_NAMESPACE

#endif /* __PLAYER_MANAGER_H__ */
