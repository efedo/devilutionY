/**
 * @file help.cpp
 *
 * Implementation of the in-game help text.
 */
#include "all.h"

namespace dvl {

const char HelpDialog::gszHelpText[] = {
	"$Keyboard Shortcuts:|"
	"F1:    Open Help Screen|"
	"Esc:   Display Main Menu|"
	"Tab:   Display Auto-map|"
	"Space: Hide all info screens|"
	"S: Open Speedbook|"
	"B: Open Spellbook|"
	"I: Open Inventory screen|"
	"C: Open Character screen|"
	"Q: Open Quest log|"
	"F: Reduce screen brightness|"
	"G: Increase screen brightness|"
	"Z: Zoom Game Screen|"
	"+ / -: Zoom Automap|"
	"1 - 8: Use Belt item|"
	"F5, F6, F7, F8:     Set hot key for skill or spell|"
	"Shift + Left Click: Attack without moving|"
	"|"
	"$Movement:|"
	"If you hold the mouse button down while moving, the character "
	"will continue to move in that direction.|"
	"|"
	"$Combat:|"
	"Holding down the shift key and then left-clicking allows the "
	"character to attack without moving.|"
	"|"
	"$Auto-map:|"
	"To access the auto-map, click the 'MAP' button on the "
	"Information Bar or press 'TAB' on the keyboard. Zooming in and "
	"out of the map is done with the + and - keys. Scrolling the map "
	"uses the arrow keys.|"
	"|"
	"$Picking up Objects:|"
	"Useable items that are small in size, such as potions or scrolls, "
	"are automatically placed in your 'belt' located at the top of "
	"the Interface bar . When an item is placed in the belt, a small "
	"number appears in that box. Items may be used by either pressing "
	"the corresponding number or right-clicking on the item.|"
	"|"
	"$Gold|"
	"You can select a specific amount of gold to drop by right "
	"clicking on a pile of gold in your inventory.|"
	"|"
	"$Skills & Spells:|"
	"You can access your list of skills and spells by left-clicking on "
	"the 'SPELLS' button in the interface bar. Memorized spells and "
	"those available through staffs are listed here. Left-clicking on "
	"the spell you wish to cast will ready the spell. A readied spell "
	"may be cast by simply right-clicking in the play area.|"
	"|"
	"$Using the Speedbook for Spells|"
	"Left-clicking on the 'readied spell' button will open the 'Speedbook' "
	"which allows you to select a skill or spell for immediate use.  "
	"To use a readied skill or spell, simply right-click in the main play "
	"area.|"
	"|"
	"$Setting Spell Hotkeys|"
	"You can assign up to four Hot Keys for skills, spells or scrolls.  "
	"Start by opening the 'speedbook' as described in the section above. "
	"Press the F5, F6, F7 or F8 keys after highlighting the spell you "
	"wish to assign.|"
	"|"
	"$Spell Books|"
	"Reading more than one book increases your knowledge of that "
	"spell, allowing you to cast the spell more effectively.|"
	"&"
};

void HelpDialog::InitHelp()
{
	helpflag = false;
	dword_634494 = 0;
	displayinghelp[0] = 0;
}

void HelpDialog::DrawHelp()
{
	int i, c, w;
	char col;
	const char *s;

	dialog.DrawSTextHelp();
	dialog.DrawQTextBack();
	dialog.PrintSString(0, 2, true, "Diablo Help", COL_GOLD, 0);
	dialog.DrawSLine(5);

	s = &gszHelpText[0];

	for (i = 0; i < help_select_line; i++) {
		c = 0;
		w = 0;
		while (*s == '\0') {
			s++;
		}
		if (*s == '$') {
			s++;
		}
		if (*s == '&') {
			continue;
		}
		while (*s != '|' && w < 577) {
			while (*s == '\0') {
				s++;
			}
			tempstr[c] = *s;
			w += fontkern[fontframe[gbFontTransTbl[(uint8_t)tempstr[c]]]] + 1;
			c++;
			s++;
		}
		if (w >= 577) {
			c--;
			while (tempstr[c] != ' ') {
				s--;
				c--;
			}
		}
		if (*s == '|') {
			s++;
		}
	}
	for (i = 7; i < 22; i++) {
		c = 0;
		w = 0;
		while (*s == '\0') {
			s++;
		}
		if (*s == '$') {
			s++;
			col = COL_RED;
		} else {
			col = COL_WHITE;
		}
		if (*s == '&') {
			HelpTop = help_select_line;
			continue;
		}
		while (*s != '|' && w < 577) {
			while (*s == '\0') {
				s++;
			}
			tempstr[c] = *s;
			w += fontkern[fontframe[gbFontTransTbl[(uint8_t)tempstr[c]]]] + 1;
			c++;
			s++;
		}
		if (w >= 577) {
			c--;
			while (tempstr[c] != ' ') {
				s--;
				c--;
			}
		}
		if (c != 0) {
			tempstr[c] = '\0';
			DrawHelpLine(0, i, tempstr, col);
		}
		if (*s == '|') {
			s++;
		}
	}

	dialog.PrintSString(0, 23, true, "Press ESC to end or the arrow keys to scroll.", COL_GOLD, 0);
}

void HelpDialog::DrawHelpLine(int always_0, int help_line_nr, char *text,
                              char color)
{
	uint8_t c;

	int width = 0;
	V2Di s = { always_0 + 96 + PANEL_LEFT, help_line_nr * 12 + 204 };
	while (*text) {
		c = gbFontTransTbl[(uint8_t)*text];
		text++;
		c = fontframe[c];
		width += fontkern[c] + 1;
		if (c) {
			if (width <= 577)
				PrintChar(s, c, color);
		}
		s.x += fontkern[c] + 1;
	}
}

void HelpDialog::DisplayHelp()
{
	help_select_line = 0;
	helpflag = true;
	HelpTop = 5000;
}

void HelpDialog::HelpScrollUp()
{
	if (help_select_line > 0)
		help_select_line--;
}

void HelpDialog::HelpScrollDown()
{
	if (help_select_line < HelpTop)
		help_select_line++;
}

}
