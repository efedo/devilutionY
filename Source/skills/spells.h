//HEADER_GOES_HERE
#ifndef __SPELLS_H__
#define __SPELLS_H__

DEVILUTION_BEGIN_NAMESPACE

//#ifdef __cplusplus
//extern "C" {
//#endif

int GetManaAmount(int id, int sn);
void UseMana(int id, int sn);
bool CheckSpell(int id, int sn, char st, bool manaonly);
void CastSpell(int id, int spl, V2Di s, V2Di d, int caster, int spllvl);
void DoResurrect(int pnum, int rid);
void DoHealOther(int pnum, int rid);

//#ifdef __cplusplus
//}
//#endif

DEVILUTION_END_NAMESPACE

#endif /* __SPELLS_H__ */
