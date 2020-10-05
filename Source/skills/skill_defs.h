// HEADER_GOES_HERE
#ifndef __SKILL_DEFS_H__
#define __SKILL_DEFS_H__

DEVILUTION_BEGIN_NAMESPACE

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

DEVILUTION_END_NAMESPACE

#endif // __SKILL_DEFS_H__

