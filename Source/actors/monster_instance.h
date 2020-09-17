/**
 * @file monster_instance.h
 *
 * Monster instance class
 */
#ifndef __MONSTER_INSTANCE_H__
#define __MONSTER_INSTANCE_H__

#include <numeric>

DEVILUTION_BEGIN_NAMESPACE

class MonsterInstance {
public:
	MonsterInstance() {};
	MonsterInstance(int i, int rd, int mtype, V2Di pos);
	void NewMonsterAnim(AnimStruct *anim, int md);

	void PlantInPosition(int dir = std::numeric_limits<int>::max(), V2Di *pos = 0);

	void ClearMVars();

	void SyncMonsterAnim();

	void ActivateSpawn(V2Di pos, int dir);
	void SpawnGolum(V2Di pos, int mi);
	void TalktoMonster();

	BOOL DirOK(int mdir);
	BOOL M_Ranged();
	BOOL M_Talker();
	void M_Enemy();
	int M_GetDir();
	void M_CheckEFlag();
	void M_StartStand(int md);
	void M_StartDelay(int len);
	void M_StartSpStand(int md);
	void M_StartWalk(V2Di vel, V2Di add, int EndDir);
	void M_StartWalk2(V2Di vel, V2Di off, V2Di add, int EndDir);
	void M_StartWalk3(V2Di vel, V2Di off, V2Di add, V2Di map, int EndDir);
	void M_StartAttack();
	void M_StartRAttack(int missile_type, int dam);
	void M_StartRSpAttack(int missile_type, int dam);
	void M_StartSpAttack();
	void M_StartEat();
	void M_ClearSquares();
	void M_GetKnockback();
	void M_StartHit(int pnum, int dam);
	void M_DiabloDeath(BOOL sendmsg);
	//void M2MStartHit(int mid, int i, int dam);
	void MonstStartKill(int pnum, BOOL sendmsg);
	void M2MStartKill(int mid);
	void M_StartKill(int pnum);
	void M_SyncStartKill(V2Di pos, int pnum);
	void M_StartFadein(int md, BOOL backwards);
	void M_StartFadeout(int md, BOOL backwards);
	void M_StartHeal();
	void M_ChangeLightOffset();
	BOOL M_DoStand();
	BOOL M_DoWalk();
	BOOL M_DoWalk2();
	BOOL M_DoWalk3();
	void M_TryM2MHit(int mid, int hper, int mind, int maxd);
	void M_TryH2HHit(int pnum, int Hit, int MinDam, int MaxDam);
	BOOL M_DoAttack();
	BOOL M_DoRAttack();
	int M_DoRSpAttack();
	BOOL M_DoSAttack();
	BOOL M_DoFadein();
	BOOL M_DoFadeout();
	int M_DoHeal();
	int M_DoTalk();
	void M_Teleport();
	BOOL M_DoGotHit();
	void M_UpdateLeader();
	BOOL M_DoDeath();
	BOOL M_DoSpStand();
	BOOL M_DoDelay();
	BOOL M_DoStone();
	void M_WalkDir(int md);
	void GroupUnity();
	BOOL M_CallWalk(int md);
	BOOL M_PathWalk();
	BOOL M_CallWalk2(int md);
	BOOL M_DumbWalk(int md);
	BOOL M_RoundWalk(int md, int *dir);
	void MAI_Zombie();
	void MAI_SkelSd();
	BOOL MAI_Path();
	void MAI_Snake();
	void MAI_Bat();
	void MAI_SkelBow();
	void MAI_Fat();
	void MAI_Sneak();
	void MAI_Fireman();
	void MAI_Fallen();
	void MAI_Cleaver();
	void MAI_Round(BOOL special);
	void MAI_GoatMc();
	void MAI_Ranged(int missile_type, BOOL special);
	void MAI_GoatBow();
	void MAI_Succ();
	void MAI_AcidUniq();
	void MAI_Scav();
	void MAI_Garg();
	void MAI_RoundRanged(int missile_type, BOOL checkdoors, int dam, int lessmissiles);
	void MAI_Magma();
	void MAI_Storm();
	void MAI_Acid();
	void MAI_Diablo();
	void MAI_RR2(int mistype, int dam);
	void MAI_Mega();
	void MAI_Golum();
	void MAI_SkelKing();
	void MAI_Rhino();
	void MAI_Counselor();
	void MAI_Garbud();
	void MAI_Zhar();
	void MAI_SnotSpil();
	void MAI_Lazurus();
	void MAI_Lazhelp();
	void MAI_Lachdanan();
	void MAI_Warlord();

	MonsterStruct data;

private:
	int i;
};

DEVILUTION_END_NAMESPACE

#endif // __MONSTER_INSTANCE_H__
