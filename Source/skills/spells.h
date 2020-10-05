//HEADER_GOES_HERE
#ifndef __SPELLS_H__
#define __SPELLS_H__

DEVILUTION_BEGIN_NAMESPACE

int GetManaAmount(int id, int sn);
void UseMana(int id, int sn);
bool CheckSpell(SpellId id, RSpellType sn, char st, bool manaonly);
void CastSpell(int id, int spl, V2Di s, V2Di d, int caster, int spllvl);
void DoResurrect(int pnum, int rid);
void DoHealOther(int pnum, int rid);

DEVILUTION_END_NAMESPACE

#endif /* __SPELLS_H__ */
