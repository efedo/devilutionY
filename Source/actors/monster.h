/**
 * @file monster.h
 *
 * Interface of monster functionality, AI, actions, spawning, loading, etc.
 */
#ifndef __MONSTER_H__
#define __MONSTER_H__

#include "monster_type.h"

DEVILUTION_BEGIN_NAMESPACE

class MonsterInstance;

extern int monstkills[MAXMONSTERS];
extern int monstactive[MAXMONSTERS];
extern int MissileFileFlag;
extern int nummonsters;
extern BOOLEAN sgbSaveSoundOn;
extern int totalmonsters;
extern int monstimgtot;
extern int uniquetrans;
extern int nummtypes;

void MissToMonst(int i, V2Di pos);
BOOL MonstPlace(V2Di p);
void LoadDiabMonsts();
void InitMonsters();
void PlaceUniques();
void SetMapMonsters(BYTE *pMap, V2Di start);
int AddMonster(V2Di pos, Dir dir, int mtype, BOOL InMap);

void DeleteMonsterList();
void ProcessMonsters();
void FreeMonsters();
BOOL PosOkMissile(V2Di pos);
BOOL CheckNoSolid(V2Di pos);
BOOL LineClearF(BOOL (*Clear)(V2Di), V2Di p1, V2Di p2);
BOOL LineClear(V2Di p1, V2Di p2);
BOOL LineClearF1(BOOL (*Clear)(int, V2Di), int monst, V2Di p1, V2Di p2);
void M_FallenFear(V2Di pos);
void PrintMonstHistory(int mt);
void PrintUniqueHistory();
BOOL PosOkMonst(int i, V2Di pos);
BOOL PosOkMonst2(int i, V2Di pos);
BOOL PosOkMonst3(int i, V2Di pos);
void M2MStartHit(int mid, int i, int dam);

BOOL IsSkel(int mt);
BOOL IsGoat(int mt);
BOOL CanTalkToMonst(int m);
BOOL CheckMonsterHit(int m, BOOL *ret);


int M_SpawnSkel(V2Di pos, Dir dir);
BOOL SpawnSkeleton(int ii, V2Di pos);
int PreSpawnSkeleton();

int encode_enemy(int m);
void decode_enemy(int m, int enemy);
void DoEnding();
void PrepDoEnding();

/* rdata */

extern const char plr2monst[9];
extern const BYTE counsmiss[4];

/* data */

extern int MWVel[24][3];
extern char animletter[7];

extern void (MonsterInstance:: *AiProc[])();

DEVILUTION_END_NAMESPACE

#endif /* __MONSTER_H__ */
