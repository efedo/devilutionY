//HEADER_GOES_HERE
#ifndef __SPELL_BOOK_H__
#define __SPELL_BOOK_H__

#include "skill_enums.h"
#include "spelldat.h"
#include <cstdint>
#include "../defs.h"

namespace dvl {

struct SpellBookEntry {
	uint8_t ability_level = 0;
	uint8_t memory_level = 0;
	uint8_t scroll_level = 0;
private:
	friend class SpellBook;
	SpellId id = SpellId::INVALID;
};

class SpellBook {
public:
	SpellBook() { for (int i = 0; i < MAX_SPELLS; ++i) _spells[i].id = SpellId(i); }
	SpellBookEntry& entry(SpellId spell_id);
	MagicType getMagicType(SpellId spell_id) { return spelldata[int(spell_id)].sType; }
	auto getSfx(SpellId spell_id) { return spelldata[int(spell_id)].sSFX; };
private:
	SpellBookEntry _spells[MAX_SPELLS];
};

}

#endif /* __SPELL_BOOK_H__ */
