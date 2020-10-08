/**
 * @file gmenu.h
 *
 * In-game dialogs.
 */
#ifndef __DIALOG_H__
#define __DIALOG_H__

namespace dvl {

class GameDialog
{
   public:
	GameDialog();

	// Interface
	void DrawSTextBack();
	void PrintSString(int x, int y, bool cjustflag, char *str, char col,
	                         int val);
	void DrawSLine(int y);
	void DrawSSlider(int y1, int y2);
	void DrawSTextHelp();
	void ClearSText(int s, int e);
	void AddSLine(int y);
	void AddSTextVal(int y, int val);
	void OffsetSTextY(int y, int yo);
	void AddSText(int x, int y, bool j, char *str, char clr, bool sel);
	void DrawSText();
	void STextESC();
	void STextUp();
	void STextDown();
	void STextPrior();
	void STextNext();

	// Quest (scrolling) text
	void FreeQuestText();
	void InitQuestText();
	void InitQTextMsg(int m);
	void DrawQTextBack();
	void PrintQTextChr(int sx, int sy, uint8_t *pCelBuff, int nCel);
	void DrawQText();

	int qtextSpd;
	int scrolltexty;
	int qtexty;
	char *qtextptr;
	bool qtextflag;
	bool storeflag;
	uint8_t *pMedTextCels;
	uint8_t *pTextBoxCels;

	/** Maps from font index to medtexts.cel frame number. */
	const uint8_t mfontframe[127] = {
	    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	    0,  37, 49, 38, 0,  39, 40, 47, 42, 43, 41, 45, 52, 44, 53, 55,
	    36, 27, 28, 29, 30, 31, 32, 33, 34, 35, 51, 50, 48, 46, 49, 54,
	    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15,
	    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 42, 0,  43, 0,  0,
	    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15,
	    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 48, 0,  49, 0};
	/**
	 * Maps from medtexts.cel frame number to character width. Note, the
	 * character width may be distinct from the frame width, which is 22 for
	 * every medtexts.cel frame.
	 */

	const uint8_t mfontkern[56] = {
	    5,  15, 10, 13, 14, 10, 9,  13, 11, 5,  5,  11, 10, 16,
	    13, 16, 10, 15, 12, 10, 14, 17, 17, 22, 17, 16, 11, 5,
	    11, 11, 11, 10, 11, 11, 11, 11, 15, 5,  10, 18, 15, 8,
	    6,  6,  7,  10, 9,  6,  10, 10, 5,  5,  5,  5,  11, 12};

	/* data */

	/**
	 * Text scroll speeds. Positive numbers will delay scrolling 1 out of n
	 * frames, negative numbers will scroll 1+(-n) pixels.
	 */
	int qscroll_spd_tbl[9] = {2, 4, 6, 8, 0, -1, -2, -3, -4};
};

extern GameDialog dialog;

}

#endif // __DIALOG_H__
