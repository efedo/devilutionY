/**
 * @file monster_instance.h
 *
 * Monster instance class
 */
#ifndef __MONSTER_INSTANCE_H__
#define __MONSTER_INSTANCE_H__

#include "../actor.h"
#include <numeric>

namespace dvl {

enum class MissileType;
enum class MonsterType;
class ActorId;

class Monster : public Actor {
public:
	Monster() : Actor(ActorType::monster){};
	Monster(Dir rd, MonsterType mtype, V2Di pos);
	void NewMonsterAnim(AnimStruct *anim, Dir md);
	void PlantInPosition(Dir dir = Dir::NONE, V2Di *pos = 0);
	void ClearMVars();
	void SyncMonsterAnim();
	void ActivateSpawn(V2Di pos, Dir dir);
	void SpawnGolum(V2Di pos, int mi);
	void TalktoMonster();
	bool DirOK(Dir mdir);

	void PlayEffect(int mode);
	bool PosOkMonst(V2Di pos);
	bool PosOkMonst2(V2Di pos);
	bool PosOkMonst3(V2Di pos);
	bool CanTalkToMonst();
	bool CheckMonsterHit(bool* ret);
	int encode_enemy();
	void decode_enemy(int enemy);

	MonsterStruct data;

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
	void M_StartRAttack(MissileType missile_type, int dam);
	void M_StartRSpAttack(MissileType missile_type, int dam);
	void M_StartSpAttack();
	void M_StartEat();
	void M_ClearSquares();
	void M_GetKnockback();


	void M_DiabloDeath(bool sendmsg);
	//void M2MStartHit(int mid, int i, int dam);



	void MonstStartKill(ActorId pnum, bool sendmsg);
	void M2MStartKill(ActorId killed);
	void startHitByPlayer(ActorId pnum, int dam);
	void startKillByPlayer(ActorId pnum);

	void M_SyncStartKill(V2Di pos, ActorId pnum);
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

	void ai_Zombie();
	void ai_SkelSd();
	bool ai_Path();
	void ai_Snake();
	void ai_Bat();
	void ai_SkelBow();
	void ai_Fat();
	void ai_Sneak();
	void ai_Fireman();
	void ai_Fallen();
	void ai_Cleaver();
	void ai_Round(bool special);
	void ai_GoatMc();
	void ai_Ranged(MissileType missile_type, bool special);
	void ai_GoatBow();
	void ai_Succ();
	void ai_AcidUniq();
	void ai_Scav();
	void ai_Garg();
	void ai_RoundRanged(MissileType missile_type, bool checkdoors, int dam,
		                        int lessmissiles);
	void ai_Magma();
	void ai_Storm();
	void ai_Acid();
	void ai_Diablo();
	void ai_RR2(MissileType mistype, int dam);
	void ai_Mega();
	void ai_Golum();
	void ai_SkelKing();
	void ai_Rhino();
	void ai_Counselor();
	void ai_Garbud();
	void ai_Zhar();
	void ai_SnotSpil();
	void ai_Lazurus();
	void ai_Lazhelp();
	void ai_Lachdanan();
	void ai_Warlord();
private:
	uint64_t _id;
};

}

#endif // __MONSTER_INSTANCE_H__
