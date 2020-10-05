/**
 * @file gui.h
 *
 * Main game GUI class
 */
#ifndef __GUI_H__
#define __GUI_H__

DEVILUTION_BEGIN_NAMESPACE

// Rename to just Gui if no collisions
class GameGui
{
public:
	void setRedrawHp() { panels.redrawhpflag = true; }
	void setRedrawMana() { panels.redrawmanaflag = true; }

	class {
	public:
		[[nodiscard]] bool inventoryUp() const noexcept { return invflag; };
		[[nodiscard]] bool spellBookUp() const noexcept { return sbookflag; };
		[[nodiscard]] bool characterUp() const noexcept { return chrflag; };
		[[nodiscard]] bool questLogUp() const noexcept { return questlog; };

	private:
		friend class GameGui;
		bool invflag = false;  // Inventory is open flag???
		bool sbookflag; // is book showing?
		bool chrflag; // is character showing
		bool questlog; // is quest log showing?
		bool redrawhpflag = false;
		bool redrawmanaflag = false;

	} panels;

private:

};

extern GameGui gui;

DEVILUTION_END_NAMESPACE

#endif  // __GUI_H__
