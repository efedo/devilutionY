/**
 * @file dead.h
 *
 * Interface of functions for placing dead monsters.
 */
#ifndef __DEAD_H__
#define __DEAD_H__

namespace dvl {

//#ifdef __cplusplus
//extern "C" {
//#endif

extern int spurtndx;
extern DeadStruct dead[MAXDEAD];
extern int stonendx;

void InitDead();
void AddDead(V2Di pos, char dv, Dir ddir);
void SetDead();

//#ifdef __cplusplus
//}
//#endif

}

#endif /* __DEAD_H__ */
