/**
 * @file monster.h
 *
 * Interface of monster functionality, AI, actions, spawning, loading, etc.
 */
#ifndef __MONSTER_H__
#define __MONSTER_H__

#include "monster_class.h"

namespace dvl {

class Monster;
enum class MissileType;

extern int MissileFileFlag;
extern bool sgbSaveSoundOn;
extern int uniquetrans;

void MissToMonst(int i, V2Di pos);
bool MonstPlace(V2Di p);
bool PosOkMissile(V2Di pos);
bool CheckNoSolid(V2Di pos);
bool LineClearF(bool (*Clear)(V2Di), V2Di p1, V2Di p2);
bool LineClear(V2Di p1, V2Di p2);
bool LineClearF1(bool (*Clear)(int, V2Di), int monst, V2Di p1, V2Di p2);

void PrintUniqueHistory();
void M_FallenFear(V2Di pos);
void M2MStartHit(int mid, int i, int dam);

/* rdata */
extern const char plr2monst[9];
extern const MissileType counsmiss[4];

/* data */
extern int MWVel[24][3];
extern void (Monster:: *AiProc[])();

}

#endif /* __MONSTER_H__ */
