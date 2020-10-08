// HEADER_GOES_HERE
#ifndef __SKILL_DEFS_H__
#define __SKILL_DEFS_H__

namespace dvl {

//////////////////////////////////////////////////
// spells
//////////////////////////////////////////////////

typedef struct SpellData {
	unsigned char sName;
	unsigned char sManaCost;
	MagicType sType;
	char* sNameText;
	char* sSkillText;
	int sBookLvl;
	int sStaffLvl;
	bool sTargeted;
	bool sTownSpell;
	int sMinInt;
	unsigned char sSFX;
	unsigned char sMissiles[3];
	unsigned char sManaAdj;
	unsigned char sMinMana;
	int sStaffMin;
	int sStaffMax;
	int sBookCost;
	int sStaffCost;
} SpellData;

}

#endif // __SKILL_DEFS_H__

