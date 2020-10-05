/**
 * @file monster_instance.h
 *
 * Monster instance class
 */
#ifndef __MONSTER_INSTANCE_H__
#define __MONSTER_INSTANCE_H__

#include "../actor.h"
#include <numeric>

DEVILUTION_BEGIN_NAMESPACE

class ActorId;

class Monster : public Actor, public MonsterStruct {
public:
	Monster() : Actor(ActorType::monster){};
	Monster(Dir rd, int mtype, V2Di pos);
	void NewMonsterAnim(AnimStruct *anim, Dir md);
	void PlantInPosition(Dir dir = Dir::NONE, V2Di *pos = 0);
	void ClearMVars();
	void SyncMonsterAnim();
	void ActivateSpawn(V2Di pos, Dir dir);
	void SpawnGolum(V2Di pos, int mi);
	void TalktoMonster();
	bool DirOK(Dir mdir);
	bool M_Ranged();
	bool M_Talker();
	void M_Enemy();
	Dir M_GetDir();
	void GetTargetLoc(V2Di &pos, int &dist, Dir &dir);
	void M_CheckEFlag();
	void M_StartStand(Dir md);
	void M_StartDelay(int len);
	void M_StartSpStand(Dir md);
	void M_StartWalk(V2Di vel, V2Di add, Dir EndDir);
	void M_StartWalk2(V2Di vel, V2Di off, V2Di add, Dir EndDir);
	void M_StartWalk3(V2Di vel, V2Di off, V2Di add, V2Di map, Dir EndDir);
	void M_StartAttack();
	void M_StartRAttack(int missile_type, int dam);
	void M_StartRSpAttack(int missile_type, int dam);
	void M_StartSpAttack();
	void M_StartEat();
	void M_ClearSquares();
	void M_GetKnockback();
	void M_StartHit(int pnum, int dam);
	void M_DiabloDeath(bool sendmsg);
	//void M2MStartHit(int mid, int i, int dam);
	void MonstStartKill(int pnum, bool sendmsg);
	void M2MStartKill(int mid);
	void M_StartKill(int pnum);
	void M_SyncStartKill(V2Di pos, int pnum);
	void M_StartFadein(Dir md, bool backwards);
	void M_StartFadeout(Dir md, bool backwards);
	void M_StartHeal();
	void M_ChangeLightOffset();
	bool M_DoStand();
	bool M_DoWalk();
	bool M_DoWalk2();
	bool M_DoWalk3();
	void M_TryM2MHit(int mid, int hper, int mind, int maxd);
	void M_TryH2HHit(ActorId pnum, int Hit, int MinDam, int MaxDam);
	bool M_DoAttack();
	bool M_DoRAttack();
	int M_DoRSpAttack();
	bool M_DoSAttack();
	bool M_DoFadein();
	bool M_DoFadeout();
	int M_DoHeal();
	int M_DoTalk();
	void M_Teleport();
	bool M_DoGotHit();
	void M_UpdateLeader();
	bool M_DoDeath();
	bool M_DoSpStand();
	bool M_DoDelay();
	bool M_DoStone();
	void M_WalkDir(Dir md);
	void GroupUnity();
	bool M_CallWalk(Dir md);
	bool M_PathWalk();
	bool M_CallWalk2(Dir md);
	bool M_DumbWalk(Dir md);
	bool M_RoundWalk(Dir md, int *dir);

	struct MAI {
		void Zombie();
		void SkelSd();
		bool Path();
		void Snake();
		void Bat();
		void SkelBow();
		void Fat();
		void Sneak();
		void Fireman();
		void Fallen();
		void Cleaver();
		void Round(bool special);
		void GoatMc();
		void Ranged(int missile_type, bool special);
		void GoatBow();
		void Succ();
		void AcidUniq();
		void Scav();
		void Garg();
		void RoundRanged(int missile_type, bool checkdoors, int dam,
		                           int lessmissiles);
		void Magma();
		void Storm();
		void Acid();
		void Diablo();
		void RR2(int mistype, int dam);
		void Mega();
		void Golum();
		void SkelKing();
		void Rhino();
		void Counselor();
		void Garbud();
		void Zhar();
		void SnotSpil();
		void Lazurus();
		void Lazhelp();
		void Lachdanan();
		void Warlord();
	} ai;  // namespace Ai
private:
	uint64_t _id;
};

DEVILUTION_END_NAMESPACE

#endif // __MONSTER_INSTANCE_H__
