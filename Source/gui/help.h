/**
 * @file help.h
 *
 * Interface of the in-game help text.
 */
#ifndef __HELP_H__
#define __HELP_H__

DEVILUTION_BEGIN_NAMESPACE

// Should inherit from game dialog eventually
class HelpDialog
{
	int help_select_line;
	int dword_634494;
	bool helpflag;
	int displayinghelp[22];
	int HelpTop;

	void InitHelp();
	void DrawHelp();
	void DrawHelpLine(int always_0, int help_line_nr, char *text, char color);
	void DisplayHelp();
	void HelpScrollUp();
	void HelpScrollDown();

	static const char gszHelpText[];
};

extern HelpDialog help;

DEVILUTION_END_NAMESPACE

#endif /* __HELP_H__ */
