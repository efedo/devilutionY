//HEADER_GOES_HERE
#ifndef __SPELLS_H__
#define __SPELLS_H__

namespace dvl {

int GetManaAmount(int id, int sn);
void UseMana(int id, int sn);
bool CheckSpell(SpellId id, RSpellType sn, char st, bool manaonly);
void CastSpell(ActorId id, SpellId spl, V2Di s, V2Di d, int caster, int spllvl);
void DoResurrect(int pnum, int rid);
void DoHealOther(int pnum, int rid);

}

#endif /* __SPELLS_H__ */
