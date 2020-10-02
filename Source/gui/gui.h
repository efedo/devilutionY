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
	bool invflag = false;  // Inventory is open flag???
	bool sbookflag;
};

extern GameGui gui;

DEVILUTION_END_NAMESPACE

#endif  // __GUI_H__
