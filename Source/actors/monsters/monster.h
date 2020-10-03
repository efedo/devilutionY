/**
 * @file monster.h
 *
 * Interface of monster functionality, AI, actions, spawning, loading, etc.
 */
#ifndef __MONSTER_H__
#define __MONSTER_H__

#include "monster_type.h"

DEVILUTION_BEGIN_NAMESPACE

class Monster;

extern int monstkills[MAXMONSTERS];
//extern int monstactive[MAXMONSTERS]; // list of active monsters
extern int MissileFileFlag;
extern int nummonsters;
extern bool sgbSaveSoundOn;
extern int totalmonsters;
extern int monstimgtot;
extern int uniquetrans;
extern int nummtypes;

void MissToMonst(int i, V2Di pos);
bool MonstPlace(V2Di p);
void LoadDiabMonsts();
void InitMonsters();
void PlaceUniques();
void SetMapMonsters(uint8_t *pMap, V2Di start);
int AddMonster(V2Di pos, Dir dir, int mtype, bool InMap);

void DeleteMonsterList();
void ProcessMonsters();
void FreeMonsters();
bool PosOkMissile(V2Di pos);
bool CheckNoSolid(V2Di pos);
bool LineClearF(bool (*Clear)(V2Di), V2Di p1, V2Di p2);
bool LineClear(V2Di p1, V2Di p2);
bool LineClearF1(bool (*Clear)(int, V2Di), int monst, V2Di p1, V2Di p2);
void M_FallenFear(V2Di pos);
void PrintMonstHistory(int mt);
void PrintUniqueHistory();
bool PosOkMonst(int i, V2Di pos);
bool PosOkMonst2(int i, V2Di pos);
bool PosOkMonst3(int i, V2Di pos);
void M2MStartHit(int mid, int i, int dam);

bool IsSkel(int mt);
bool IsGoat(int mt);
bool CanTalkToMonst(int m);
bool CheckMonsterHit(int m, bool *ret);


int M_SpawnSkel(V2Di pos, Dir dir);
bool SpawnSkeleton(int ii, V2Di pos);
int PreSpawnSkeleton();

int encode_enemy(int m);
void decode_enemy(int m, int enemy);
void DoEnding();
void PrepDoEnding();

/* rdata */

extern const char plr2monst[9];
extern const uint8_t counsmiss[4];

/* data */

extern int MWVel[24][3];
extern char animletter[7];

extern void (Monster:: *AiProc[])();

DEVILUTION_END_NAMESPACE

#endif /* __MONSTER_H__ */
