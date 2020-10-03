/**
 * @file monstdat.h
 *
 * Interface of all monster data.
 */
#ifndef __MONSTDAT_H__
#define __MONSTDAT_H__

#include "monster_defs.h"
#include "monster_enums.h"

DEVILUTION_BEGIN_NAMESPACE

extern MonsterData monsterdata[];
extern uint8_t MonstConvTbl[];
extern uint8_t MonstAvailTbl[];
extern UniqMonstStruct UniqMonst[];

//#ifdef __cplusplus
//}
//#endif

DEVILUTION_END_NAMESPACE

#endif /* __MONSTDAT_H__ */
