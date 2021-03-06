/**
 * @file missiles.h
 *
 * Interface of missile functionality.
 */
#ifndef __MISSILES_H__
#define __MISSILES_H__

namespace dvl {

extern int missileactive[MAXMISSILES];
extern int missileavail[MAXMISSILES];
extern MissileStruct missile[MAXMISSILES];
extern ChainStruct chain[MAXMISSILES];
extern int nummissiles;
extern bool ManashieldFlag;
extern bool MissilePreFlag;
extern int numchains;

void GetDamageAmt(int i, int *mind, int *maxd);
bool CheckBlock(V2Di f, V2Di t);
int FindClosest(V2Di s, int rad);
Dir GetDirection8(V2Di t1, V2Di t2);
Dir16 GetDirection16(V2Di t1, V2Di t2);
void DeleteMissile(int mi, int i);
void GetMissileVel(int i, V2Di s, V2Di d, int v);
void PutMissile(int i);
void GetMissilePos(int i);
void MoveMissilePos(int i);
bool MonsterTrapHit(int m, int mindam, int maxdam, int dist, int t, bool shift);
bool MonsterMHit(int pnum, int m, int mindam, int maxdam, int dist, int t, bool shift);
bool PlayerMHit(int pnum, int m, int dist, int mind, int maxd, int mtype, bool shift, int earflag);
bool Plr2PlrMHit(int pnum, int p, int mindam, int maxdam, int dist, int mtype, bool shift);
void CheckMissileCol(int i, int mindam, int maxdam, bool shift, V2Di m, bool nodel);
void SetMissAnim(int mi, int animtype);
void SetMissDir(int mi, Dir dir);
void LoadMissileGFX(MissileGraphic mi);
void InitMissileGFX();
void FreeMissileGFX(int mi);
void FreeMissiles();
void FreeMissiles2();
void InitMissiles();
void AddLArrow(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void AddArrow(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void GetVileMissPos(int mi, V2Di d);
void AddRndTeleport(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void AddFirebolt(int mi, V2Di s, V2Di d, Dir midir, char micaster, int id, int dam);
void AddMagmaball(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void miss_null_33(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void AddTeleport(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void AddLightball(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void AddFirewall(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void AddFireball(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void AddLightctrl(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void AddLightning(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void AddMisexp(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void AddWeapexp(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
bool CheckIfTrig(V2Di t);
void AddTown(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void AddFlash(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void AddFlash2(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void AddManashield(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void AddFiremove(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void AddGuardian(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void AddChain(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void miss_null_11(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void miss_null_12(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void miss_null_13(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void AddRhino(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void miss_null_32(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void AddFlare(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void AddAcid(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void miss_null_1D(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void AddAcidpud(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void AddStone(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void AddGolem(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void AddEtherealize(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void miss_null_1F(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void miss_null_23(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void AddBoom(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void AddHeal(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void AddHealOther(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void AddElement(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void AddIdentify(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void AddFirewallC(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void AddInfra(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void AddWave(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void AddNova(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void AddBlodboil(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void AddRepair(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void AddRecharge(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void AddDisarm(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void AddApoca(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void AddFlame(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void AddFlamec(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void AddCbolt(int mi, V2Di s, V2Di d, Dir midir, char micaster, int id, int dam);
void AddHbolt(int mi, V2Di s, V2Di d, Dir midir, char micaster, int id, int dam);
void AddResurrect(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void AddResurrectBeam(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void AddTelekinesis(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void AddBoneSpirit(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void AddRportal(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
void AddDiabApoca(int mi, V2Di s, V2Di d, Dir midir, char mienemy, int id, int dam);
int AddMissile(V2Di s, V2Di d, Dir midir, MissileType mitype, char micaster, ActorId id, int midam, int spllvl);
int Sentfire(int i, V2Di s);
void MI_Dummy(int i);
void MI_Golem(int i);
void MI_SetManashield(int i);
void MI_LArrow(int i);
void MI_Arrow(int i);
void MI_Firebolt(int i);
void MI_Lightball(int i);
void mi_null_33(int i);
void MI_Acidpud(int i);
void MI_Firewall(int i);
void MI_Fireball(int i);
void MI_Lightctrl(int i);
void MI_Lightning(int i);
void MI_Town(int i);
void MI_Flash(int i);
void MI_Flash2(int i);
void MI_Manashield(int i);
void MI_Etherealize(int i);
void MI_Firemove(int i);
void MI_Guardian(int i);
void MI_Chain(int i);
void mi_null_11(int i);
void MI_Weapexp(int i);
void MI_Misexp(int i);
void MI_Acidsplat(int i);
void MI_Teleport(int i);
void MI_Stone(int i);
void MI_Boom(int i);
void MI_Rhino(int i);
void mi_null_32(int i);
void MI_FirewallC(int i);
void MI_Infra(int i);
void MI_Apoca(int i);
void MI_Wave(int i);
void MI_Nova(int i);
void MI_Blodboil(int i);
void MI_Flame(int i);
void MI_Flamec(int i);
void MI_Cbolt(int i);
void MI_Hbolt(int i);
void MI_Element(int i);
void MI_Bonespirit(int i);
void MI_ResurrectBeam(int i);
void MI_Rportal(int i);
void ProcessMissiles();
void missiles_process_charge();
void ClearMissileSpot(int mi);

/* rdata */

}

#endif /* __MISSILES_H__ */
