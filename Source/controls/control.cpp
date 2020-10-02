/**
 * @file control.cpp
 *
 * Implementation of the character and main control panels
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

uint8_t sgbNextTalkSave;
uint8_t sgbTalkSavePos;
uint8_t *pDurIcons;
uint8_t *pChrButtons;
bool drawhpflag;
bool dropGoldFlag;
bool panbtn[8];
bool chrbtn[4];
uint8_t *pMultiBtns;
uint8_t *pPanelButtons;
uint8_t *pChrPanel;
bool lvlbtndown;
char sgszTalkSave[8][80];
int dropGoldValue;
bool drawmanaflag;
bool chrbtnactive;
char sgszTalkMsg[MAX_SEND_STR_LEN];
uint8_t *pPanelText;
uint8_t *pLifeBuff;
uint8_t *pBtmBuff;
uint8_t *pTalkBtns;
bool pstrjust[4];
int pnumlines;
bool pinfoflag;
bool talkbtndown[3];
int pSpell;
uint8_t *pManaBuff;
char infoclr;
int sgbPlrTalkTbl;
uint8_t *pGBoxBuff;
uint8_t *pSBkBtnCel;
char tempstr[256];
bool whisper[MAX_PLRS];
int sbooktab;
int pSplType;
int initialDropGoldIndex;
bool talkflag;
uint8_t *pSBkIconCels;
bool chrflag;
bool drawbtnflag;
uint8_t *pSpellBkCel;
char infostr[256];
int numpanbtns;
uint8_t *pStatusPanel;
char panelstr[4][64];
bool panelflag;
uint8_t SplTransTbl[256];
int initialDropGoldValue;
uint8_t *pSpellCels;
bool panbtndown;
uint8_t *pTalkPanel;
bool spselflag;

/** Maps from font index to smaltext.cel frame number. */
const uint8_t fontframe[128] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 54, 44, 57, 58, 56, 55, 47, 40, 41, 59, 39, 50, 37, 51, 52,
	36, 27, 28, 29, 30, 31, 32, 33, 34, 35, 48, 49, 60, 38, 61, 53,
	62, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
	16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 42, 63, 43, 64, 65,
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
	16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 40, 66, 41, 67, 0
};

/**
 * Maps from smaltext.cel frame number to character width. Note, the
 * character width may be distinct from the frame width, which is 13 for every
 * smaltext.cel frame.
 */
const uint8_t fontkern[68] = {
	8, 10, 7, 9, 8, 7, 6, 8, 8, 3,
	3, 8, 6, 11, 9, 10, 6, 9, 9, 6,
	9, 11, 10, 13, 10, 11, 7, 5, 7, 7,
	8, 7, 7, 7, 7, 7, 10, 4, 5, 6,
	3, 3, 4, 3, 6, 6, 3, 3, 3, 3,
	3, 2, 7, 6, 3, 10, 10, 6, 6, 7,
	4, 4, 9, 6, 6, 12, 3, 7
};
/**
 * Line start position for info box text when displaying 1, 2, 3, 4 and 5 lines respectivly
 */
const int lineOffsets[5][5] = {
	{ 82 },
	{ 70, 94 },
	{ 64, 82, 100 },
	{ 60, 75, 89, 104 },
	{ 58, 70, 82, 94, 105 },
};

/**
 * Maps ASCII character code to font index, as used by the
 * small, medium and large sized fonts; which corresponds to smaltext.cel,
 * medtexts.cel and bigtgold.cel respectively.
 */
const uint8_t gbFontTransTbl[256] = {
	// clang-format off
	'\0', 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	' ',  '!',  '\"', '#',  '$',  '%',  '&',  '\'', '(',  ')',  '*',  '+',  ',',  '-',  '.',  '/',
	'0',  '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  ':',  ';',  '<',  '=',  '>',  '?',
	'@',  'A',  'B',  'C',  'D',  'E',  'F',  'G',  'H',  'I',  'J',  'K',  'L',  'M',  'N',  'O',
	'P',  'Q',  'R',  'S',  'T',  'U',  'V',  'W',  'X',  'Y',  'Z',  '[',  '\\', ']',  '^',  '_',
	'`',  'a',  'b',  'c',  'd',  'e',  'f',  'g',  'h',  'i',  'j',  'k',  'l',  'm',  'n',  'o',
	'p',  'q',  'r',  's',  't',  'u',  'v',  'w',  'x',  'y',  'z',  '{',  '|',  '}',  '~',  0x01,
	'C',  'u',  'e',  'a',  'a',  'a',  'a',  'c',  'e',  'e',  'e',  'i',  'i',  'i',  'A',  'A',
	'E',  'a',  'A',  'o',  'o',  'o',  'u',  'u',  'y',  'O',  'U',  'c',  'L',  'Y',  'P',  'f',
	'a',  'i',  'o',  'u',  'n',  'N',  'a',  'o',  '?',  0x01, 0x01, 0x01, 0x01, '!',  '<',  '>',
	'o',  '+',  '2',  '3',  '\'', 'u',  'P',  '.',  ',',  '1',  '0',  '>',  0x01, 0x01, 0x01, '?',
	'A',  'A',  'A',  'A',  'A',  'A',  'A',  'C',  'E',  'E',  'E',  'E',  'I',  'I',  'I',  'I',
	'D',  'N',  'O',  'O',  'O',  'O',  'O',  'X',  '0',  'U',  'U',  'U',  'U',  'Y',  'b',  'B',
	'a',  'a',  'a',  'a',  'a',  'a',  'a',  'c',  'e',  'e',  'e',  'e',  'i',  'i',  'i',  'i',
	'o',  'n',  'o',  'o',  'o',  'o',  'o',  '/',  '0',  'u',  'u',  'u',  'u',  'y',  'b',  'y',
	// clang-format on
};

/* data */

/** Maps from spell_id to spelicon.cel frame number. */
char SpellITbl[MAX_SPELLS] = {
	1,
	1,
	2,
	3,
	4,
	5,
	6,
	7,
	8,
	9,
	28,
	13,
	12,
	18,
	16,
	14,
	18,
	19,
	11,
	20,
	15,
	21,
	23,
	24,
	25,
	22,
	26,
	29,
	37,
	38,
	39,
	42,
	41,
	40,
	10,
	36,
	30,
};
/** Maps from panel_button_id to the position and dimensions of a panel button. */
int PanBtnPos[8][5] = {
	// clang-format off
	{ PANEL_LEFT +   9, PANEL_TOP +   9, 71, 19, true  }, // char button
	{ PANEL_LEFT +   9, PANEL_TOP +  35, 71, 19, false }, // quests button
	{ PANEL_LEFT +   9, PANEL_TOP +  75, 71, 19, true  }, // map button
	{ PANEL_LEFT +   9, PANEL_TOP + 101, 71, 19, false }, // menu button
	{ PANEL_LEFT + 560, PANEL_TOP +   9, 71, 19, true  }, // inv button
	{ PANEL_LEFT + 560, PANEL_TOP +  35, 71, 19, false }, // spells button
	{ PANEL_LEFT +  87, PANEL_TOP +  91, 33, 32, true  }, // chat button
	{ PANEL_LEFT + 527, PANEL_TOP +  91, 33, 32, true  }, // friendly fire button
	// clang-format on
};
/** Maps from panel_button_id to hotkey name. */
char *PanBtnHotKey[8] = { "'c'", "'q'", "Tab", "Esc", "'i'", "'b'", "Enter", NULL };
/** Maps from panel_button_id to panel button description. */
char *PanBtnStr[8] = {
	"Character Information",
	"Quests log",
	"Automap",
	"Main Menu",
	"Inventory",
	"Spell book",
	"Send Message",
	"Player Attack"
};
/** Maps from attribute_id to the rectangle on screen used for attribute increment buttons. */
RECT32 ChrBtnsRect[4] = {
	{ 137, 138, 41, 22 },
	{ 137, 166, 41, 22 },
	{ 137, 195, 41, 22 },
	{ 137, 223, 41, 22 }
};

/** Maps from spellbook page number and position to spell_id. */
int SpellPages[6][7] = {
	{ SPL_NULL, SPL_FIREBOLT, SPL_CBOLT, SPL_HBOLT, SPL_HEAL, SPL_HEALOTHER, SPL_FLAME },
	{ SPL_RESURRECT, SPL_FIREWALL, SPL_TELEKINESIS, SPL_LIGHTNING, SPL_TOWN, SPL_FLASH, SPL_STONE },
	{ SPL_RNDTELEPORT, SPL_MANASHIELD, SPL_ELEMENT, SPL_FIREBALL, SPL_WAVE, SPL_CHAIN, SPL_GUARDIAN },
	{ SPL_NOVA, SPL_GOLEM, SPL_TELEPORT, SPL_APOCA, SPL_BONESPIRIT, SPL_FLARE, SPL_ETHEREALIZE },
	{ -1, -1, -1, -1, -1, -1, -1 },
	{ -1, -1, -1, -1, -1, -1, -1 }
};

#define SPLICONLENGTH 56
#define SPLROWICONLS 10
#define SPLICONLAST 43

void DrawSpellCel(int px, int py, uint8_t *Trans, int nCel, int w)
{
	DrawSpellCel({ px, py }, Trans, nCel, w);
}

/**
 * Draw spell cell onto the back buffer.
 * @param xp Back buffer coordinate
 * @param yp Back buffer coordinate
 * @param Trans Pointer to the cel buffer.
 * @param nCel Index of the cel frame to draw. 0 based.
 * @param w Width of the frame.
 */
void DrawSpellCel(V2Di p, uint8_t *Trans, int nCel, int w)
{
	CelDrawLight(p, Trans, nCel, w, SplTransTbl);
}

void SetSpellTrans(char t)
{
	int i;

	if (t == RSPLTYPE_SKILL) {
		for (i = 0; i < 128; i++)
			SplTransTbl[i] = i;
	}
	for (i = 128; i < 256; i++)
		SplTransTbl[i] = i;
	SplTransTbl[255] = 0;

	switch (t) {
	case RSPLTYPE_SPELL:
		SplTransTbl[PAL8_YELLOW] = PAL16_BLUE + 1;
		SplTransTbl[PAL8_YELLOW + 1] = PAL16_BLUE + 3;
		SplTransTbl[PAL8_YELLOW + 2] = PAL16_BLUE + 5;
		for (i = PAL16_BLUE; i < PAL16_BLUE + 16; i++) {
			SplTransTbl[PAL16_BEIGE - PAL16_BLUE + i] = i;
			SplTransTbl[PAL16_YELLOW - PAL16_BLUE + i] = i;
			SplTransTbl[PAL16_ORANGE - PAL16_BLUE + i] = i;
		}
		break;
	case RSPLTYPE_SCROLL:
		SplTransTbl[PAL8_YELLOW] = PAL16_BEIGE + 1;
		SplTransTbl[PAL8_YELLOW + 1] = PAL16_BEIGE + 3;
		SplTransTbl[PAL8_YELLOW + 2] = PAL16_BEIGE + 5;
		for (i = PAL16_BEIGE; i < PAL16_BEIGE + 16; i++) {
			SplTransTbl[PAL16_YELLOW - PAL16_BEIGE + i] = i;
			SplTransTbl[PAL16_ORANGE - PAL16_BEIGE + i] = i;
		}
		break;
	case RSPLTYPE_CHARGES:
		SplTransTbl[PAL8_YELLOW] = PAL16_ORANGE + 1;
		SplTransTbl[PAL8_YELLOW + 1] = PAL16_ORANGE + 3;
		SplTransTbl[PAL8_YELLOW + 2] = PAL16_ORANGE + 5;
		for (i = PAL16_ORANGE; i < PAL16_ORANGE + 16; i++) {
			SplTransTbl[PAL16_BEIGE - PAL16_ORANGE + i] = i;
			SplTransTbl[PAL16_YELLOW - PAL16_ORANGE + i] = i;
		}
		break;
	case RSPLTYPE_INVALID:
		SplTransTbl[PAL8_YELLOW] = PAL16_GRAY + 1;
		SplTransTbl[PAL8_YELLOW + 1] = PAL16_GRAY + 3;
		SplTransTbl[PAL8_YELLOW + 2] = PAL16_GRAY + 5;
		for (i = PAL16_GRAY; i < PAL16_GRAY + 15; i++) {
			SplTransTbl[PAL16_BEIGE - PAL16_GRAY + i] = i;
			SplTransTbl[PAL16_YELLOW - PAL16_GRAY + i] = i;
			SplTransTbl[PAL16_ORANGE - PAL16_GRAY + i] = i;
		}
		SplTransTbl[PAL16_BEIGE + 15] = 0;
		SplTransTbl[PAL16_YELLOW + 15] = 0;
		SplTransTbl[PAL16_ORANGE + 15] = 0;
		break;
	}
}

/**
 * Sets the spell frame to draw and its position then draws it.
 */
void DrawSpell()
{
	char st;
	int spl, tlvl;

	spl = myplr().data._pRSpell;
	st = myplr().data._pRSplType;

	// BUGFIX: Move the next line into the if statement to avoid OOB (SPL_INVALID is -1) (fixed)
	if (st == RSPLTYPE_SPELL && spl != SPL_INVALID) {
		tlvl = myplr().data._pISplLvlAdd + myplr().data._pSplLvl[spl];
		if (!CheckSpell(myplr(), spl, RSPLTYPE_SPELL, true))
			st = RSPLTYPE_INVALID;
		if (tlvl <= 0)
			st = RSPLTYPE_INVALID;
	}
	if (lvl.currlevel == 0 && st != RSPLTYPE_INVALID && !spelldata[spl].sTownSpell)
		st = RSPLTYPE_INVALID;
	if (myplr().data._pRSpell < 0)
		st = RSPLTYPE_INVALID;
	SetSpellTrans(st);
	if (spl != SPL_INVALID)
		DrawSpellCel(PANEL_X + 565, PANEL_Y + 119, pSpellCels, SpellITbl[spl], SPLICONLENGTH);
	else
		DrawSpellCel(PANEL_X + 565, PANEL_Y + 119, pSpellCels, 27, SPLICONLENGTH);
}

void DrawSpellList()
{
	int i, j, x, y, c, s, t, v, lx, ly, trans;
	unsigned __int64 mask, spl;

	pSpell = SPL_INVALID;
	infostr[0] = '\0';
	x = PANEL_X + 12 + SPLICONLENGTH * SPLROWICONLS;
	y = PANEL_Y - 17;
	ClearPanel();
	for (i = 0; i < 4; i++) {
		switch ((spell_type)i) {
		case RSPLTYPE_SKILL:
			SetSpellTrans(RSPLTYPE_SKILL);
			c = SPLICONLAST + 3;
			mask = myplr().data._pAblSpells;
			break;
		case RSPLTYPE_SPELL:
			c = SPLICONLAST + 4;
			mask = myplr().data._pMemSpells;
			break;
		case RSPLTYPE_SCROLL:
			SetSpellTrans(RSPLTYPE_SCROLL);
			c = SPLICONLAST + 1;
			mask = myplr().data._pScrlSpells;
			break;
		case RSPLTYPE_CHARGES:
			SetSpellTrans(RSPLTYPE_CHARGES);
			c = SPLICONLAST + 2;
			mask = myplr().data._pISpells;
			break;
		}
		for (spl = 1, j = 1; j < MAX_SPELLS; spl <<= 1, j++) {
			if (!(mask & spl))
				continue;
			if (i == RSPLTYPE_SPELL) {
				s = myplr().data._pISplLvlAdd + myplr().data._pSplLvl[j];
				if (s < 0)
					s = 0;
				if (s > 0)
					trans = RSPLTYPE_SPELL;
				else
					trans = RSPLTYPE_INVALID;
				SetSpellTrans(trans);
			}
			if (lvl.currlevel == 0 && !spelldata[j].sTownSpell)
				SetSpellTrans(RSPLTYPE_INVALID);
			DrawSpellCel(x, y, pSpellCels, SpellITbl[j], SPLICONLENGTH);
			lx = x - BORDER_LEFT;
			ly = y - BORDER_TOP - SPLICONLENGTH;
			if (Mouse.x >= lx && Mouse.x < lx + SPLICONLENGTH && Mouse.y >= ly && Mouse.y < ly + SPLICONLENGTH) {
				pSpell = j;
				pSplType = i;
				DrawSpellCel(x, y, pSpellCels, c, SPLICONLENGTH);
				switch (i) {
				case RSPLTYPE_SKILL:
					sprintf(infostr, "%s Skill", spelldata[pSpell].sSkillText);
					break;
				case RSPLTYPE_SPELL:
					sprintf(infostr, "%s Spell", spelldata[pSpell].sNameText);
					if (pSpell == SPL_HBOLT) {
						sprintf(tempstr, "Damages undead only");
						AddPanelString(tempstr, true);
					}
					if (s == 0)
						sprintf(tempstr, "Spell Level 0 - Unusable");
					else
						sprintf(tempstr, "Spell Level %i", s);
					AddPanelString(tempstr, true);
					break;
				case RSPLTYPE_SCROLL:
					sprintf(infostr, "Scroll of %s", spelldata[pSpell].sNameText);
					v = 0;
					for (t = 0; t < myplr().data._pNumInv; t++) {
						if (myplr().data.InvList[t]._itype != ItemType::none
						    && (myplr().data.InvList[t]._iMiscId == MiscItemId::SCROLL || myplr().data.InvList[t]._iMiscId == MiscItemId::SCROLLT)
						    && myplr().data.InvList[t]._iSpell == pSpell) {
							v++;
						}
					}
					for (t = 0; t < MAXBELTITEMS; t++) {
						if (myplr().data.SpdList[t]._itype != ItemType::none
						    && (myplr().data.SpdList[t]._iMiscId == MiscItemId::SCROLL || myplr().data.SpdList[t]._iMiscId == MiscItemId::SCROLLT)
						    && myplr().data.SpdList[t]._iSpell == pSpell) {
							v++;
						}
					}
					if (v == 1)
						strcpy(tempstr, "1 Scroll");
					else
						sprintf(tempstr, "%i Scrolls", v);
					AddPanelString(tempstr, true);
					break;
				case RSPLTYPE_CHARGES:
					sprintf(infostr, "Staff of %s", spelldata[pSpell].sNameText);
					if (myplr().data.InvBody[INVLOC_HAND_LEFT]._iCharges == 1)
						strcpy(tempstr, "1 Charge");
					else
						sprintf(tempstr, "%i Charges", myplr().data.InvBody[INVLOC_HAND_LEFT]._iCharges);
					AddPanelString(tempstr, true);
					break;
				}
				for (t = 0; t < 4; t++) {
					if (myplr().data._pSplHotKey[t] == pSpell && myplr().data._pSplTHotKey[t] == pSplType) {
						DrawSpellCel(x, y, pSpellCels, t + SPLICONLAST + 5, SPLICONLENGTH);
						sprintf(tempstr, "Spell Hot Key #F%i", t + 5);
						AddPanelString(tempstr, true);
					}
				}
			}
			x -= SPLICONLENGTH;
			if (x == PANEL_X + 12 - SPLICONLENGTH) {
				x = PANEL_X + 12 + SPLICONLENGTH * SPLROWICONLS;
				y -= SPLICONLENGTH;
			}
		}
		if (mask != 0 && x != PANEL_X + 12 + SPLICONLENGTH * SPLROWICONLS)
			x -= SPLICONLENGTH;
		if (x == PANEL_X + 12 - SPLICONLENGTH) {
			x = PANEL_X + 12 + SPLICONLENGTH * SPLROWICONLS;
			y -= SPLICONLENGTH;
		}
	}
}

void SetSpell()
{
	spselflag = false;
	if (pSpell != SPL_INVALID) {
		ClearPanel();
		myplr().data._pRSpell = pSpell;
		myplr().data._pRSplType = pSplType;
		force_redraw = 255;
	}
}

void SetSpeedSpell(int slot)
{
	int i;

	if (pSpell != SPL_INVALID) {
		for (i = 0; i < 4; ++i) {
			if (myplr().data._pSplHotKey[i] == pSpell && myplr().data._pSplTHotKey[i] == pSplType)
				myplr().data._pSplHotKey[i] = SPL_INVALID;
		}
		myplr().data._pSplHotKey[slot] = pSpell;
		myplr().data._pSplTHotKey[slot] = pSplType;
	}
}

void ToggleSpell(int slot)
{
	unsigned __int64 spells;

	if (myplr().data._pSplHotKey[slot] == -1) {
		return;
	}

	switch (myplr().data._pSplTHotKey[slot]) {
	case RSPLTYPE_SKILL:
		spells = myplr().data._pAblSpells;
		break;
	case RSPLTYPE_SPELL:
		spells = myplr().data._pMemSpells;
		break;
	case RSPLTYPE_SCROLL:
		spells = myplr().data._pScrlSpells;
		break;
	case RSPLTYPE_CHARGES:
		spells = myplr().data._pISpells;
		break;
	}

	if (spells & (__int64)1 << (myplr().data._pSplHotKey[slot] - 1)) {
		myplr().data._pRSpell = myplr().data._pSplHotKey[slot];
		myplr().data._pRSplType = myplr().data._pSplTHotKey[slot];
		force_redraw = 255;
	}
}

/**
 * @brief Print letter to the back buffer
 * @param sx Backbuffer offset
 * @param sy Backbuffer offset
 * @param nCel Number of letter in Windows-1252
 * @param col text_color color value
 */
void PrintChar(V2Di s, int nCel, char col)
{
	assert(gpBuffer);

	int i;
	uint8_t pix;
	uint8_t tbl[256];

	switch (col) {
	case COL_WHITE:
		CelDraw(s, pPanelText, nCel, 13);
		return;
	case COL_BLUE:
		for (i = 0; i < 256; i++) {
			pix = i;
			if (pix > PAL16_GRAY + 13)
				pix = PAL16_BLUE + 15;
			else if (pix >= PAL16_GRAY)
				pix -= PAL16_GRAY - (PAL16_BLUE + 2);
			tbl[i] = pix;
		}
		break;
	case COL_RED:
		for (i = 0; i < 256; i++) {
			pix = i;
			if (pix >= PAL16_GRAY)
				pix -= PAL16_GRAY - PAL16_RED;
			tbl[i] = pix;
		}
		break;
	default:
		for (i = 0; i < 256; i++) {
			pix = i;
			if (pix >= PAL16_GRAY) {
				if (pix >= PAL16_GRAY + 14)
					pix = PAL16_YELLOW + 15;
				else
					pix -= PAL16_GRAY - (PAL16_YELLOW + 2);
			}
			tbl[i] = pix;
		}
		break;
	}
	CelDrawLight(s, pPanelText, nCel, 13, tbl);
}

void AddPanelString(char *str, bool just)
{
	strcpy(panelstr[pnumlines], str);
	pstrjust[pnumlines] = just;

	if (pnumlines < 4)
		pnumlines++;
}

void ClearPanel()
{
	pnumlines = 0;
	pinfoflag = false;
}

void DrawPanelBox(RECT32 r, V2Di s)
{
	int nSrcOff, nDstOff;

	assert(gpBuffer);

	nSrcOff = r.x + PANEL_WIDTH * r.y;
	nDstOff = s.x + BUFFER_WIDTH * s.y;

	int hgt;
	uint8_t *src, *dst;

	src = &pBtmBuff[nSrcOff];
	dst = &gpBuffer[nDstOff];

	for (hgt = r.h; hgt; hgt--, src += PANEL_WIDTH, dst += BUFFER_WIDTH) {
		memcpy(dst, src, r.w);
	}
}

/**
 * Draws a section of the empty flask cel on top of the panel to create the illusion
 * of the flask getting empty. This function takes a cel and draws a
 * horizontal stripe of height (max-min) onto the back buffer.
 * @param pCelBuff Buffer of the empty flask cel.
 * @param min Top of the flask cel section to draw.
 * @param max Bottom of the flask cel section to draw.
 * @param sx Back buffer coordinate
 * @param sy Back buffer coordinate
 */
void SetFlaskHeight(uint8_t *pCelBuff, int min, int max, V2Di s)
{
	int nSrcOff, nDstOff, w;

	assert(gpBuffer);

	nSrcOff = 88 * min;
	nDstOff = s.x + BUFFER_WIDTH * s.y;
	w = max - min;

	uint8_t *src, *dst;

	src = &pCelBuff[nSrcOff];
	dst = &gpBuffer[nDstOff];

	for (; w; w--, src += 88, dst += BUFFER_WIDTH)
		memcpy(dst, src, 88);
}

/**
 * Draws the dome of the flask that protrudes above the panel top line.
 * It draws a rectangle of fixed width 59 and height 'h' from the source buffer
 * into the target buffer.
 * @param pCelBuff The flask cel buffer.
 * @param w Width of the cel.
 * @param nSrcOff Offset of the source buffer from where the bytes will start to be copied from.
 * @param pBuff Target buffer.
 * @param nDstOff Offset of the target buffer where the bytes will start to be copied to.
 * @param h How many lines of the source buffer that will be copied.
 */
void DrawFlask(uint8_t *pCelBuff, int w, int nSrcOff, uint8_t *pBuff, int nDstOff, int h)
{
	int wdt, hgt;
	uint8_t *src, *dst;

	src = &pCelBuff[nSrcOff];
	dst = &pBuff[nDstOff];

	for (hgt = h; hgt; hgt--, src += w - 59, dst += BUFFER_WIDTH - 59) {
		for (wdt = 59; wdt; wdt--) {
			if (*src)
				*dst = *src;
			src++;
			dst++;
		}
	}
}

/**
 * Draws the top dome of the life flask (that part that protrudes out of the control panel).
 * First it draws the empty flask cel and then draws the filled part on top if needed.
 */
void DrawLifeFlask()
{
	double p;
	int filled;

	p = (double)myplr().data._pHitPoints / (double)myplr().data._pMaxHP * 80.0;
	myplr().data._pHPPer = p;
	filled = myplr().data._pHPPer;
	if (filled > 80)
		filled = 80;
	filled = 80 - filled;
	if (filled > 11)
		filled = 11;
	filled += 2;

	DrawFlask(pLifeBuff, 88, 88 * 3 + 13, gpBuffer, SCREENXY(PANEL_LEFT + 109, PANEL_TOP - 13), filled);
	if (filled != 13)
		DrawFlask(pBtmBuff, PANEL_WIDTH, PANEL_WIDTH * (filled + 3) + 109, gpBuffer, SCREENXY(PANEL_LEFT + 109, PANEL_TOP - 13 + filled), 13 - filled);
}

/**
 * Controls the drawing of the area of the life flask within the control panel.
 * First sets the fill amount then draws the empty flask cel portion then the filled
 * flask portion.
 */
void UpdateLifeFlask()
{
	double p;
	int filled;
	p = (double)myplr().data._pHitPoints / (double)myplr().data._pMaxHP * 80.0;
	filled = p;
	myplr().data._pHPPer = filled;

	if (filled > 69)
		filled = 69;
	else if (filled < 0)
		filled = 0;
	if (filled != 69)
		SetFlaskHeight(pLifeBuff, 16, 85 - filled, { 96 + PANEL_X, PANEL_Y });
	if (filled != 0)
		DrawPanelBox({ 96, 85 - filled, 88, filled }, { 96 + PANEL_X, PANEL_Y + 69 - filled });
}

void DrawManaFlask()
{
	int filled = myplr().data._pManaPer;
	if (filled > 80)
		filled = 80;
	filled = 80 - filled;
	if (filled > 11)
		filled = 11;
	filled += 2;

	DrawFlask(pManaBuff, 88, 88 * 3 + 13, gpBuffer, SCREENXY(PANEL_LEFT + 475, PANEL_TOP - 13), filled);
	if (filled != 13)
		DrawFlask(pBtmBuff, PANEL_WIDTH, PANEL_WIDTH * (filled + 3) + 475, gpBuffer, SCREENXY(PANEL_LEFT + 475, PANEL_TOP - 13 + filled), 13 - filled);
}

void control_update_life_mana()
{
	int manaPer;
	int maxMana = myplr().data._pMaxMana;
	int mana = myplr().data._pMana;
	if (maxMana < 0)
		maxMana = 0;
	if (mana < 0)
		mana = 0;
	if (maxMana == 0)
		manaPer = 0;
	else
		manaPer = (double)mana / (double)maxMana * 80.0;
	myplr().data._pManaPer = manaPer;
	myplr().data._pHPPer = (double)myplr().data._pHitPoints / (double)myplr().data._pMaxHP * 80.0;
}

/**
 * Controls the drawing of the area of the life flask within the control panel.
 * Also for some reason draws the current right mouse button spell.
 */
void UpdateManaFlask()
{
	int filled;
	int maxMana = myplr().data._pMaxMana;
	int mana = myplr().data._pMana;
	if (maxMana < 0)
		maxMana = 0;
	if (mana < 0)
		mana = 0;

	if (maxMana == 0)
		filled = 0;
	else
		filled = (double)mana / (double)maxMana * 80.0;

	myplr().data._pManaPer = filled;

	if (filled > 69)
		filled = 69;
	if (filled != 69)
		SetFlaskHeight(pManaBuff, 16, 85 - filled, { PANEL_X + 464, PANEL_Y });
	if (filled != 0)
		DrawPanelBox({ 464, 85 - filled, 88, filled }, { PANEL_X + 464, PANEL_Y + 69 - filled });

	DrawSpell();
}

void InitControlPan()
{
	int i;

	if (plr.isSingleplayer()) {
		pBtmBuff = DiabloAllocPtr((PANEL_HEIGHT + 16) * PANEL_WIDTH);
		memset(pBtmBuff, 0, (PANEL_HEIGHT + 16) * PANEL_WIDTH);
	} else {
		pBtmBuff = DiabloAllocPtr((PANEL_HEIGHT + 16) * 2 * PANEL_WIDTH);
		memset(pBtmBuff, 0, (PANEL_HEIGHT + 16) * 2 * PANEL_WIDTH);
	}
	pManaBuff = DiabloAllocPtr(88 * 88);
	memset(pManaBuff, 0, 88 * 88);
	pLifeBuff = DiabloAllocPtr(88 * 88);
	memset(pLifeBuff, 0, 88 * 88);
	pPanelText = LoadFileInMem("CtrlPan\\SmalText.CEL", NULL);
	pChrPanel = LoadFileInMem("Data\\Char.CEL", NULL);
	pSpellCels = LoadFileInMem("CtrlPan\\SpelIcon.CEL", NULL);
	SetSpellTrans(RSPLTYPE_SKILL);
	pStatusPanel = LoadFileInMem("CtrlPan\\Panel8.CEL", NULL);
	CelBlitWidth(pBtmBuff, { 0, (PANEL_HEIGHT + 16) - 1 }, PANEL_WIDTH, pStatusPanel, 1, PANEL_WIDTH);
	MemFreeDbg(pStatusPanel);
	pStatusPanel = LoadFileInMem("CtrlPan\\P8Bulbs.CEL", NULL);
	CelBlitWidth(pLifeBuff, { 0, 87 }, 88, pStatusPanel, 1, 88);
	CelBlitWidth(pManaBuff, { 0, 87 }, 88, pStatusPanel, 2, 88);
	MemFreeDbg(pStatusPanel);
	talkflag = false;
	if (plr.isMultiplayer()) {
		pTalkPanel = LoadFileInMem("CtrlPan\\TalkPanl.CEL", NULL);
		CelBlitWidth(pBtmBuff, { 0, (PANEL_HEIGHT + 16) * 2 - 1 }, PANEL_WIDTH, pTalkPanel, 1, PANEL_WIDTH);
		MemFreeDbg(pTalkPanel);
		pMultiBtns = LoadFileInMem("CtrlPan\\P8But2.CEL", NULL);
		pTalkBtns = LoadFileInMem("CtrlPan\\TalkButt.CEL", NULL);
		sgbPlrTalkTbl = 0;
		sgszTalkMsg[0] = '\0';
		for (i = 0; i < MAX_PLRS; i++)
			whisper[i] = true;
		for (i = 0; i < sizeof(talkbtndown) / sizeof(talkbtndown[0]); i++)
			talkbtndown[i] = false;
	}
	panelflag = false;
	lvlbtndown = false;
	pPanelButtons = LoadFileInMem("CtrlPan\\Panel8bu.CEL", NULL);
	for (i = 0; i < sizeof(panbtn) / sizeof(panbtn[0]); i++)
		panbtn[i] = false;
	panbtndown = false;
	if (plr.isSingleplayer())
		numpanbtns = 6;
	else
		numpanbtns = 8;
	pChrButtons = LoadFileInMem("Data\\CharBut.CEL", NULL);
	for (i = 0; i < sizeof(chrbtn) / sizeof(chrbtn[0]); i++)
		chrbtn[i] = false;
	chrbtnactive = false;
	pDurIcons = LoadFileInMem("Items\\DurIcons.CEL", NULL);
	strcpy(infostr, "");
	ClearPanel();
	drawhpflag = true;
	drawmanaflag = true;
	chrflag = false;
	spselflag = false;
	pSpellBkCel = LoadFileInMem("Data\\SpellBk.CEL", NULL);
	pSBkBtnCel = LoadFileInMem("Data\\SpellBkB.CEL", NULL);
	pSBkIconCels = LoadFileInMem("Data\\SpellI2.CEL", NULL);
	sbooktab = 0;
	sbookflag = false;
	if (myplr().data._pClass == PC_WARRIOR) {
		SpellPages[0][0] = SPL_REPAIR;
	} else if (myplr().data._pClass == PC_ROGUE) {
		SpellPages[0][0] = SPL_DISARM;
	} else if (myplr().data._pClass == PC_SORCERER) {
		SpellPages[0][0] = SPL_RECHARGE;
	}
	pQLogCel = LoadFileInMem("Data\\Quest.CEL", NULL);
	pGBoxBuff = LoadFileInMem("CtrlPan\\Golddrop.cel", NULL);
	dropGoldFlag = false;
	dropGoldValue = 0;
	initialDropGoldValue = 0;
	initialDropGoldIndex = 0;
	PentSpn2Frame = 1;
}

void DrawCtrlPan()
{
	DrawPanelBox({ 0, sgbPlrTalkTbl + 16, PANEL_WIDTH, PANEL_HEIGHT }, { PANEL_X, PANEL_Y });
	DrawInfoBox();
}

/**
 * Draws the control panel buttons in their current state. If the button is in the default
 * state draw it from the panel cel(extract its sub-rect). Else draw it from the buttons cel.
 */
void DrawCtrlBtns()
{
	int i;

	for (i = 0; i < 6; i++) {
		if (!panbtn[i])
			DrawPanelBox({ PanBtnPos[i][0] - PANEL_LEFT, PanBtnPos[i][1] - (PANEL_TOP - 16), 71, 20 }, { PanBtnPos[i][0] + SCREEN_X, PanBtnPos[i][1] + SCREEN_Y });
		else
			CelDraw(PanBtnPos[i][0] + SCREEN_X, PanBtnPos[i][1] + SCREEN_Y + 18, pPanelButtons, i + 1, 71);
	}
	if (numpanbtns == 8) {
		CelDraw(87 + PANEL_X, 122 + PANEL_Y, pMultiBtns, panbtn[6] + 1, 33);
		if (FriendlyMode)
			CelDraw(527 + PANEL_X, 122 + PANEL_Y, pMultiBtns, panbtn[7] + 3, 33);
		else
			CelDraw(527 + PANEL_X, 122 + PANEL_Y, pMultiBtns, panbtn[7] + 5, 33);
	}
}

/**
 * Draws the "Speed Book": the rows of known spells for quick-setting a spell that
 * show up when you click the spell slot at the control panel.
 */
void DoSpeedBook()
{
	unsigned __int64 spells, spell;
	int xo, yo, X, Y, i, j;

	spselflag = true;
	xo = PANEL_X + 12 + SPLICONLENGTH * 10;
	yo = PANEL_Y - 17;
	X = xo - (BORDER_LEFT - SPLICONLENGTH / 2);
	Y = yo - (BORDER_TOP + SPLICONLENGTH / 2);
	if (myplr().data._pRSpell != SPL_INVALID) {
		for (i = 0; i < 4; i++) {
			switch (i) {
			case RSPLTYPE_SKILL:
				spells = myplr().data._pAblSpells;
				break;
			case RSPLTYPE_SPELL:
				spells = myplr().data._pMemSpells;
				break;
			case RSPLTYPE_SCROLL:
				spells = myplr().data._pScrlSpells;
				break;
			case RSPLTYPE_CHARGES:
				spells = myplr().data._pISpells;
				break;
			}
			spell = (__int64)1;
			for (j = 1; j < MAX_SPELLS; j++) {
				if (spell & spells) {
					if (j == myplr().data._pRSpell && i == myplr().data._pRSplType) {
						X = xo - (BORDER_LEFT - SPLICONLENGTH / 2);
						Y = yo - (BORDER_TOP + SPLICONLENGTH / 2);
					}
					xo -= SPLICONLENGTH;
					if (xo == PANEL_X + 12 - SPLICONLENGTH) {
						xo = PANEL_X + 12 + SPLICONLENGTH * SPLROWICONLS;
						yo -= SPLICONLENGTH;
					}
				}
				spell <<= (__int64)1;
			}
			if (spells && xo != PANEL_X + 12 + SPLICONLENGTH * SPLROWICONLS)
				xo -= SPLICONLENGTH;
			if (xo == PANEL_X + 12 - SPLICONLENGTH) {
				xo = PANEL_X + 12 + SPLICONLENGTH * SPLROWICONLS;
				yo -= SPLICONLENGTH;
			}
		}
	}

	SetCursorPos(X, Y);
}

/**
 * Checks if the mouse cursor is within any of the panel buttons and flag it if so.
 */
void DoPanBtn()
{
	int i;

	for (i = 0; i < numpanbtns; i++) {
		if (Mouse.x >= PanBtnPos[i][0] && Mouse.x <= PanBtnPos[i][0] + PanBtnPos[i][2]) {
			if (Mouse.y >= PanBtnPos[i][1] && Mouse.y <= PanBtnPos[i][1] + PanBtnPos[i][3]) {
				panbtn[i] = true;
				drawbtnflag = true;
				panbtndown = true;
			}
		}
	}
	if (!spselflag && Mouse.x >= 565 + PANEL_LEFT && Mouse.x < 621 + PANEL_LEFT && Mouse.y >= 64 + PANEL_TOP && Mouse.y < 120 + PANEL_TOP) {
		DoSpeedBook();
		gamemenu_off();
	}
}

void control_set_button_down(int btn_id)
{
	panbtn[btn_id] = true;
	drawbtnflag = true;
	panbtndown = true;
}

void control_check_btn_press()
{
	if (Mouse.x >= PanBtnPos[3][0]
	    && Mouse.x <= PanBtnPos[3][0] + PanBtnPos[3][2]
	    && Mouse.y >= PanBtnPos[3][1]
	    && Mouse.y <= PanBtnPos[3][1] + PanBtnPos[3][3]) {
		control_set_button_down(3);
	}
	if (Mouse.x >= PanBtnPos[6][0]
	    && Mouse.x <= PanBtnPos[6][0] + PanBtnPos[6][2]
	    && Mouse.y >= PanBtnPos[6][1]
	    && Mouse.y <= PanBtnPos[6][1] + PanBtnPos[6][3]) {
		control_set_button_down(6);
	}
}

void DoAutoMap()
{
	if (lvl.currlevel != 0 || plr.isMultiplayer()) {
		if (!automap.enabled())
			automap.start();
		else
			automap.disable();
	} else {
		InitDiabloMsg(EMSG_NO_AUTOMAP_IN_TOWN);
	}
}

/**
 * Checks the mouse cursor position within the control panel and sets information
 * strings if needed.
 */
void CheckPanelInfo()
{
	int i, c, v, s, xend, yend;

	panelflag = false;
	ClearPanel();
	for (i = 0; i < numpanbtns; i++) {
		xend = PanBtnPos[i][0] + PanBtnPos[i][2];
		yend = PanBtnPos[i][1] + PanBtnPos[i][3];
		if (Mouse.x >= PanBtnPos[i][0] && Mouse.x <= xend && Mouse.y >= PanBtnPos[i][1] && Mouse.y <= yend) {
			if (i != 7) {
				strcpy(infostr, PanBtnStr[i]);
			} else {
				if (FriendlyMode)
					strcpy(infostr, "Player friendly");
				else
					strcpy(infostr, "Player attack");
			}
			if (PanBtnHotKey[i] != NULL) {
				sprintf(tempstr, "Hotkey : %s", PanBtnHotKey[i]);
				AddPanelString(tempstr, true);
			}
			infoclr = COL_WHITE;
			panelflag = true;
			pinfoflag = true;
		}
	}
	if (!spselflag && Mouse.x >= 565 + PANEL_LEFT && Mouse.x < 621 + PANEL_LEFT && Mouse.y >= 64 + PANEL_TOP && Mouse.y < 120 + PANEL_TOP) {
		strcpy(infostr, "Select current spell button");
		infoclr = COL_WHITE;
		panelflag = true;
		pinfoflag = true;
		strcpy(tempstr, "Hotkey : 's'");
		AddPanelString(tempstr, true);
		v = myplr().data._pRSpell;
		if (v != SPL_INVALID) {
			switch (myplr().data._pRSplType) {
			case RSPLTYPE_SKILL:
				sprintf(tempstr, "%s Skill", spelldata[v].sSkillText);
				AddPanelString(tempstr, true);
				break;
			case RSPLTYPE_SPELL:
				sprintf(tempstr, "%s Spell", spelldata[v].sNameText);
				AddPanelString(tempstr, true);
				c = myplr().data._pISplLvlAdd + myplr().data._pSplLvl[v];
				if (c < 0)
					c = 0;
				if (c == 0)
					sprintf(tempstr, "Spell Level 0 - Unusable");
				else
					sprintf(tempstr, "Spell Level %i", c);
				AddPanelString(tempstr, true);
				break;
			case RSPLTYPE_SCROLL:
				sprintf(tempstr, "Scroll of %s", spelldata[v].sNameText);
				AddPanelString(tempstr, true);
				s = 0;
				for (i = 0; i < myplr().data._pNumInv; i++) {
					if (myplr().data.InvList[i]._itype != ItemType::none
					    && (myplr().data.InvList[i]._iMiscId == MiscItemId::SCROLL || myplr().data.InvList[i]._iMiscId == MiscItemId::SCROLLT)
					    && myplr().data.InvList[i]._iSpell == v) {
						s++;
					}
				}
				for (i = 0; i < MAXBELTITEMS; i++) {
					if (myplr().data.SpdList[i]._itype != ItemType::none
					    && (myplr().data.SpdList[i]._iMiscId == MiscItemId::SCROLL || myplr().data.SpdList[i]._iMiscId == MiscItemId::SCROLLT)
					    && myplr().data.SpdList[i]._iSpell == v) {
						s++;
					}
				}
				if (s == 1)
					strcpy(tempstr, "1 Scroll");
				else
					sprintf(tempstr, "%i Scrolls", s);
				AddPanelString(tempstr, true);
				break;
			case RSPLTYPE_CHARGES:
				sprintf(tempstr, "Staff of %s", spelldata[v].sNameText);
				AddPanelString(tempstr, true);
				if (myplr().data.InvBody[INVLOC_HAND_LEFT]._iCharges == 1)
					strcpy(tempstr, "1 Charge");
				else
					sprintf(tempstr, "%i Charges", myplr().data.InvBody[INVLOC_HAND_LEFT]._iCharges);
				AddPanelString(tempstr, true);
				break;
			}
		}
	}
	if (Mouse.x > 190 + PANEL_LEFT && Mouse.x < 437 + PANEL_LEFT && Mouse.y > 4 + PANEL_TOP && Mouse.y < 33 + PANEL_TOP)
		pcursinvitem = CheckInvHLight();
}

/**
 * Check if the mouse is within a control panel button that's flagged.
 * Takes apropiate action if so.
 */
void CheckBtnUp()
{
	int i;
	bool gamemenuOff;

	gamemenuOff = true;
	drawbtnflag = true;
	panbtndown = false;

	for (i = 0; i < 8; i++) {
		if (!panbtn[i]) {
			continue;
		}

		panbtn[i] = false;

		if (Mouse.x < PanBtnPos[i][0]
		    || Mouse.x > PanBtnPos[i][0] + PanBtnPos[i][2]
		    || Mouse.y < PanBtnPos[i][1]
		    || Mouse.y > PanBtnPos[i][1] + PanBtnPos[i][3]) {
			continue;
		}

		switch (i) {
		case PANBTN_CHARINFO:
			questlog = false;
			chrflag = !chrflag;
			break;
		case PANBTN_QLOG:
			chrflag = false;
			if (!questlog)
				StartQuestlog();
			else
				questlog = false;
			break;
		case PANBTN_AUTOMAP:
			DoAutoMap();
			break;
		case PANBTN_MAINMENU:
			qtextflag = false;
			gamemenu_handle_previous();
			gamemenuOff = false;
			break;
		case PANBTN_INVENTORY:
			sbookflag = false;
			invflag = !invflag;
			if (dropGoldFlag) {
				dropGoldFlag = false;
				dropGoldValue = 0;
			}
			break;
		case PANBTN_SPELLBOOK:
			invflag = false;
			if (dropGoldFlag) {
				dropGoldFlag = false;
				dropGoldValue = 0;
			}
			sbookflag = !sbookflag;
			break;
		case PANBTN_SENDMSG:
			if (talkflag)
				control_reset_talk();
			else
				control_type_message();
			break;
		case PANBTN_FRIENDLY:
			FriendlyMode = !FriendlyMode;
			break;
		}
	}

	if (gamemenuOff)
		gamemenu_off();
}

void FreeControlPan()
{
	MemFreeDbg(pBtmBuff);
	MemFreeDbg(pManaBuff);
	MemFreeDbg(pLifeBuff);
	MemFreeDbg(pPanelText);
	MemFreeDbg(pChrPanel);
	MemFreeDbg(pSpellCels);
	MemFreeDbg(pPanelButtons);
	MemFreeDbg(pMultiBtns);
	MemFreeDbg(pTalkBtns);
	MemFreeDbg(pChrButtons);
	MemFreeDbg(pDurIcons);
	MemFreeDbg(pQLogCel);
	MemFreeDbg(pSpellBkCel);
	MemFreeDbg(pSBkBtnCel);
	MemFreeDbg(pSBkIconCels);
	MemFreeDbg(pGBoxBuff);
}

bool control_WriteStringToBuffer(uint8_t *str)
{
	int k;
	uint8_t ichar;

	k = 0;
	while (*str) {
		ichar = gbFontTransTbl[*str];
		str++;
		k += fontkern[fontframe[ichar]];
		if (k >= 125)
			return false;
	}

	return true;
}

/**
 * Sets a string to be drawn in the info box and then draws it.
 */
void DrawInfoBox()
{
	int nGold;

	DrawPanelBox({ 177, 62, 288, 60 }, { PANEL_X + 177, PANEL_Y + 46 });
	if (!panelflag && !trigflag && pcursinvitem == -1 && !spselflag) {
		infostr[0] = '\0';
		infoclr = COL_WHITE;
		ClearPanel();
	}
	if (spselflag || trigflag) {
		infoclr = COL_WHITE;
	} else if (pcurs >= CURSOR_FIRSTITEM) {
		if (myplr().data.HoldItem._itype == ItemType::gold) {
			nGold = myplr().data.HoldItem._ivalue;
			sprintf(infostr, "%i gold %s", nGold, get_pieces_str(nGold));
		} else if (!myplr().data.HoldItem._iStatFlag) {
			ClearPanel();
			AddPanelString("Requirements not met", true);
			pinfoflag = true;
		} else {
			if (myplr().data.HoldItem._iIdentified)
				strcpy(infostr, myplr().data.HoldItem._iIName);
			else
				strcpy(infostr, myplr().data.HoldItem._iName);
			if (myplr().data.HoldItem._iMagical == ItemQuality::magic)
				infoclr = COL_BLUE;
			if (myplr().data.HoldItem._iMagical == ItemQuality::unique)
				infoclr = COL_GOLD;
		}
	} else {
		if (pcursitem != -1)
			GetItemStr(pcursitem);
		else if (pcursobj != -1)
			GetObjectStr(pcursobj);
		if (pcursmonst != -1) {
			if (lvl.type() != DunType::town) {
				infoclr = COL_WHITE;
				strcpy(infostr, monsters[pcursmonst].data.mName);
				ClearPanel();
				if (monsters[pcursmonst].data._uniqtype != 0) {
					infoclr = COL_GOLD;
					PrintUniqueHistory();
				} else {
					PrintMonstHistory(monsters[pcursmonst].data.MType->mtype);
				}
			} else if (pcursitem == -1) {
				strcpy(infostr, towner[pcursmonst]._tName);
			}
		}
		if (pcursplr != -1) {
			infoclr = COL_GOLD;
			strcpy(infostr, plr[pcursplr].data._pName);
			ClearPanel();
			sprintf(tempstr, "Level : %i", plr[pcursplr].data._pLevel);
			AddPanelString(tempstr, true);
			sprintf(tempstr, "Hit Points %i of %i", plr[pcursplr].data._pHitPoints >> 6, plr[pcursplr].data._pMaxHP >> 6);
			AddPanelString(tempstr, true);
		}
	}
	if (infostr[0] != '\0' || pnumlines != 0)
		PrintInfo();
}

void PrintInfo()
{
	int yo, lo, i;

	if (!talkflag) {
		yo = 0;
		lo = 1;
		if (infostr[0] != '\0') {
			CPrintString(0, infostr, true, pnumlines);
			yo = 1;
			lo = 0;
		}

		for (i = 0; i < pnumlines; i++) {
			CPrintString(i + yo, panelstr[i], pstrjust[i], pnumlines - lo);
		}
	}
}

void CPrintString(int y, char *str, bool center, int lines)
{
	uint8_t c;
	char *tmp;
	int lineOffset, strWidth;
	V2Di s;

	lineOffset = 0;
	s.x = 177 + PANEL_X;
	s.y = lineOffsets[lines][y] + PANEL_Y;
	if (center == true) {
		strWidth = 0;
		tmp = str;
		while (*tmp) {
			c = gbFontTransTbl[(uint8_t)*tmp++];
			strWidth += fontkern[fontframe[c]] + 2;
		}
		if (strWidth < 288)
			lineOffset = (288 - strWidth) >> 1;
		s.x += lineOffset;
	}
	while (*str) {
		c = gbFontTransTbl[(uint8_t)*str++];
		c = fontframe[c];
		lineOffset += fontkern[c] + 2;
		if (c) {
			if (lineOffset < 288) {
				PrintChar(s, c, infoclr);
			}
		}
		s.x += fontkern[c] + 2;
	}
}

void PrintGameStr(V2Di pos, const char *str, int color)
{
	uint8_t c;
	V2Di s;
	s.x = pos.x + SCREEN_X;
	s.y = pos.y + SCREEN_Y;
	while (*str) {
		c = gbFontTransTbl[(uint8_t)*str++];
		c = fontframe[c];
		if (c)
			PrintChar(s, c, color);
		s.x += fontkern[c] + 1;
	}
}

void DrawChr()
{
	char col;
	char chrstr[64];
	int pc, mindam, maxdam;

	CelDraw(SCREEN_X, 351 + SCREEN_Y, pChrPanel, 1, SPANEL_WIDTH);
	ADD_PlrStringXY(20, 32, 151, myplr().data._pName, COL_WHITE);

	if (myplr().data._pClass == PC_WARRIOR) {
		ADD_PlrStringXY(168, 32, 299, "Warrior", COL_WHITE);
	} else if (myplr().data._pClass == PC_ROGUE) {
		ADD_PlrStringXY(168, 32, 299, "Rogue", COL_WHITE);
	} else if (myplr().data._pClass == PC_SORCERER) {
		ADD_PlrStringXY(168, 32, 299, "Sorceror", COL_WHITE);
	}

	sprintf(chrstr, "%i", myplr().data._pLevel);
	ADD_PlrStringXY(66, 69, 109, chrstr, COL_WHITE);

	sprintf(chrstr, "%li", myplr().data._pExperience);
	ADD_PlrStringXY(216, 69, 300, chrstr, COL_WHITE);

	if (myplr().data._pLevel == MAXCHARLEVEL - 1) {
		strcpy(chrstr, "None");
		col = COL_GOLD;
	} else {
		sprintf(chrstr, "%li", myplr().data._pNextExper);
		col = COL_WHITE;
	}
	ADD_PlrStringXY(216, 97, 300, chrstr, col);

	sprintf(chrstr, "%i", myplr().data._pGold);
	ADD_PlrStringXY(216, 146, 300, chrstr, COL_WHITE);

	col = COL_WHITE;
	if (myplr().data._pIBonusAC > 0)
		col = COL_BLUE;
	if (myplr().data._pIBonusAC < 0)
		col = COL_RED;
	sprintf(chrstr, "%i", myplr().data._pIBonusAC + myplr().data._pIAC + myplr().data._pDexterity / 5);
	ADD_PlrStringXY(258, 183, 301, chrstr, col);

	col = COL_WHITE;
	if (myplr().data._pIBonusToHit > 0)
		col = COL_BLUE;
	if (myplr().data._pIBonusToHit < 0)
		col = COL_RED;
	sprintf(chrstr, "%i%%", (myplr().data._pDexterity >> 1) + myplr().data._pIBonusToHit + 50);
	ADD_PlrStringXY(258, 211, 301, chrstr, col);

	col = COL_WHITE;
	if (myplr().data._pIBonusDam > 0)
		col = COL_BLUE;
	if (myplr().data._pIBonusDam < 0)
		col = COL_RED;
	mindam = myplr().data._pIMinDam;
	mindam += myplr().data._pIBonusDam * mindam / 100;
	mindam += myplr().data._pIBonusDamMod;
	if (myplr().data.InvBody[INVLOC_HAND_LEFT]._itype == ItemType::bow) {
		if (myplr().data._pClass == PC_ROGUE)
			mindam += myplr().data._pDamageMod;
		else
			mindam += myplr().data._pDamageMod >> 1;
	} else {
		mindam += myplr().data._pDamageMod;
	}
	maxdam = myplr().data._pIMaxDam;
	maxdam += myplr().data._pIBonusDam * maxdam / 100;
	maxdam += myplr().data._pIBonusDamMod;
	if (myplr().data.InvBody[INVLOC_HAND_LEFT]._itype == ItemType::bow) {
		if (myplr().data._pClass == PC_ROGUE)
			maxdam += myplr().data._pDamageMod;
		else
			maxdam += myplr().data._pDamageMod >> 1;
	} else {
		maxdam += myplr().data._pDamageMod;
	}
	sprintf(chrstr, "%i-%i", mindam, maxdam);
	if (mindam >= 100 || maxdam >= 100)
		MY_PlrStringXY(254, 239, 305, chrstr, col, -1);
	else
		MY_PlrStringXY(258, 239, 301, chrstr, col, 0);

	col = myplr().data._pMagResist == 0 ? COL_WHITE : COL_BLUE;
	if (myplr().data._pMagResist < MAXRESIST) {
		sprintf(chrstr, "%i%%", myplr().data._pMagResist);
	} else {
		col = COL_GOLD;
		sprintf(chrstr, "MAX");
	}
	ADD_PlrStringXY(257, 276, 300, chrstr, col);

	col = myplr().data._pFireResist == 0 ? COL_WHITE : COL_BLUE;
	if (myplr().data._pFireResist < MAXRESIST) {
		sprintf(chrstr, "%i%%", myplr().data._pFireResist);
	} else {
		col = COL_GOLD;
		sprintf(chrstr, "MAX");
	}
	ADD_PlrStringXY(257, 304, 300, chrstr, col);

	col = myplr().data._pLghtResist == 0 ? COL_WHITE : COL_BLUE;
	if (myplr().data._pLghtResist < MAXRESIST) {
		sprintf(chrstr, "%i%%", myplr().data._pLghtResist);
	} else {
		col = COL_GOLD;
		sprintf(chrstr, "MAX");
	}
	ADD_PlrStringXY(257, 332, 300, chrstr, col);

	col = COL_WHITE;
	sprintf(chrstr, "%i", myplr().data._pBaseStr);
	if (classes[myplr().data._pClass].MaxStats[ATTRIB_STR] == myplr().data._pBaseStr)
		col = COL_GOLD;
	ADD_PlrStringXY(95, 155, 126, chrstr, col);

	col = COL_WHITE;
	sprintf(chrstr, "%i", myplr().data._pBaseMag);
	if (classes[myplr().data._pClass].MaxStats[ATTRIB_MAG] == myplr().data._pBaseMag)
		col = COL_GOLD;
	ADD_PlrStringXY(95, 183, 126, chrstr, col);

	col = COL_WHITE;
	sprintf(chrstr, "%i", myplr().data._pBaseDex);
	if (classes[myplr().data._pClass].MaxStats[ATTRIB_DEX] == myplr().data._pBaseDex)
		col = COL_GOLD;
	ADD_PlrStringXY(95, 211, 126, chrstr, col);

	col = COL_WHITE;
	sprintf(chrstr, "%i", myplr().data._pBaseVit);
	if (classes[myplr().data._pClass].MaxStats[ATTRIB_VIT] == myplr().data._pBaseVit)
		col = COL_GOLD;
	ADD_PlrStringXY(95, 239, 126, chrstr, col);

	col = COL_WHITE;
	if (myplr().data._pStrength > myplr().data._pBaseStr)
		col = COL_BLUE;
	if (myplr().data._pStrength < myplr().data._pBaseStr)
		col = COL_RED;
	sprintf(chrstr, "%i", myplr().data._pStrength);
	ADD_PlrStringXY(143, 155, 173, chrstr, col);

	col = COL_WHITE;
	if (myplr().data._pMagic > myplr().data._pBaseMag)
		col = COL_BLUE;
	if (myplr().data._pMagic < myplr().data._pBaseMag)
		col = COL_RED;
	sprintf(chrstr, "%i", myplr().data._pMagic);
	ADD_PlrStringXY(143, 183, 173, chrstr, col);

	col = COL_WHITE;
	if (myplr().data._pDexterity > myplr().data._pBaseDex)
		col = COL_BLUE;
	if (myplr().data._pDexterity < myplr().data._pBaseDex)
		col = COL_RED;
	sprintf(chrstr, "%i", myplr().data._pDexterity);
	ADD_PlrStringXY(143, 211, 173, chrstr, col);

	col = COL_WHITE;
	if (myplr().data._pVitality > myplr().data._pBaseVit)
		col = COL_BLUE;
	if (myplr().data._pVitality < myplr().data._pBaseVit)
		col = COL_RED;
	sprintf(chrstr, "%i", myplr().data._pVitality);
	ADD_PlrStringXY(143, 239, 173, chrstr, col);

	if (myplr().data._pStatPts > 0) {
		if (myplr().CalcStatDiff() < myplr().data._pStatPts) {
			myplr().data._pStatPts = myplr().CalcStatDiff();
		}
	}
	if (myplr().data._pStatPts > 0) {
		sprintf(chrstr, "%i", myplr().data._pStatPts);
		ADD_PlrStringXY(95, 266, 126, chrstr, COL_RED);
		pc = myplr().data._pClass;
		if (myplr().data._pBaseStr < classes[pc].MaxStats[ATTRIB_STR])
			CelDraw(137 + SCREEN_X, 159 + SCREEN_Y, pChrButtons, chrbtn[ATTRIB_STR] + 2, 41);
		if (myplr().data._pBaseMag < classes[pc].MaxStats[ATTRIB_MAG])
			CelDraw(137 + SCREEN_X, 187 + SCREEN_Y, pChrButtons, chrbtn[ATTRIB_MAG] + 4, 41);
		if (myplr().data._pBaseDex < classes[pc].MaxStats[ATTRIB_DEX])
			CelDraw(137 + SCREEN_X, 216 + SCREEN_Y, pChrButtons, chrbtn[ATTRIB_DEX] + 6, 41);
		if (myplr().data._pBaseVit < classes[pc].MaxStats[ATTRIB_VIT])
			CelDraw(137 + SCREEN_X, 244 + SCREEN_Y, pChrButtons, chrbtn[ATTRIB_VIT] + 8, 41);
	}

	col = myplr().data._pMaxHP <= myplr().data._pMaxHPBase ? COL_WHITE : COL_BLUE;
	sprintf(chrstr, "%i", myplr().data._pMaxHP >> 6);
	ADD_PlrStringXY(95, 304, 126, chrstr, col);
	if (myplr().data._pHitPoints != myplr().data._pMaxHP)
		col = COL_RED;
	sprintf(chrstr, "%i", myplr().data._pHitPoints >> 6);
	ADD_PlrStringXY(143, 304, 174, chrstr, col);

	col = myplr().data._pMaxMana <= myplr().data._pMaxManaBase ? COL_WHITE : COL_BLUE;
	sprintf(chrstr, "%i", myplr().data._pMaxMana >> 6);
	ADD_PlrStringXY(95, 332, 126, chrstr, col);
	if (myplr().data._pMana != myplr().data._pMaxMana)
		col = COL_RED;
	sprintf(chrstr, "%i", myplr().data._pMana >> 6);
	ADD_PlrStringXY(143, 332, 174, chrstr, col);
}

void MY_PlrStringXY(int x, int y, int endX, char *pszStr, char col, int base)
{
	MY_PlrStringXY({ x, y }, endX, pszStr, col, base);
}


/**
 * @brief Render text string to back buffer
 * @param x Screen coordinate
 * @param y Screen coordinate
 * @param endX End of line in screen coordinate
 * @param pszStr String to print, in Windows-1252 encoding
 * @param col text_color color value
 * @param base Letter spacing
 */
void MY_PlrStringXY(V2Di pos, int endX, char *pszStr, char col, int base)
{
	uint8_t c;
	char *tmp;
	V2Di s;
	int screen_x, line, widthOffset;

	s.x = pos.x + SCREEN_X;
	s.y = pos.y + SCREEN_Y;
	widthOffset = endX - pos.x + 1;
	line = 0;
	screen_x = 0;
	tmp = pszStr;
	while (*tmp) {
		c = gbFontTransTbl[(uint8_t)*tmp++];
		screen_x += fontkern[fontframe[c]] + base;
	}
	if (screen_x < widthOffset)
		line = (widthOffset - screen_x) >> 1;
	s.x += line;
	while (*pszStr) {
		c = gbFontTransTbl[(uint8_t)*pszStr++];
		c = fontframe[c];
		line += fontkern[c] + base;
		if (c) {
			if (line < widthOffset)
				PrintChar(s, c, col);
		}
		s.x += fontkern[c] + base;
	}
}

void CheckLvlBtn()
{
	if (!lvlbtndown && Mouse.x >= 40 + PANEL_LEFT && Mouse.x <= 81 + PANEL_LEFT && Mouse.y >= -39 + PANEL_TOP && Mouse.y <= -17 + PANEL_TOP)
		lvlbtndown = true;
}

void ReleaseLvlBtn()
{
	if (Mouse.x >= 40 + PANEL_LEFT && Mouse.x <= 81 + PANEL_LEFT && Mouse.y >= -39 + PANEL_TOP && Mouse.y <= -17 + PANEL_TOP)
		chrflag = true;
	lvlbtndown = false;
}

void DrawLevelUpIcon()
{
	int nCel;

	if (stextflag == StoreTalkId::NONE) {
		nCel = lvlbtndown ? 3 : 2;
		ADD_PlrStringXY(PANEL_LEFT + 0, PANEL_TOP - 49, PANEL_LEFT + 120, "Level Up", COL_WHITE);
		CelDraw(40 + PANEL_X, -17 + PANEL_Y, pChrButtons, nCel, 41);
	}
}

void CheckChrBtns()
{
	int pc, i;

	if (!chrbtnactive && myplr().data._pStatPts) {
		pc = myplr().data._pClass;
		for (i = 0; i < 4; i++) {
			switch (i) {
			case ATTRIB_STR:
				if (myplr().data._pBaseStr >= classes[pc].MaxStats[ATTRIB_STR])
					continue;
				break;
			case ATTRIB_MAG:
				if (myplr().data._pBaseMag >= classes[pc].MaxStats[ATTRIB_MAG])
					continue;
				break;
			case ATTRIB_DEX:
				if (myplr().data._pBaseDex >= classes[pc].MaxStats[ATTRIB_DEX])
					continue;
				break;
			case ATTRIB_VIT:
				if (myplr().data._pBaseVit >= classes[pc].MaxStats[ATTRIB_VIT])
					continue;
				break;
			default:
				continue;
			}
			if (Mouse.x >= ChrBtnsRect[i].x
			    && Mouse.x <= ChrBtnsRect[i].x + ChrBtnsRect[i].w
			    && Mouse.y >= ChrBtnsRect[i].y
			    && Mouse.y <= ChrBtnsRect[i].y + ChrBtnsRect[i].h) {
				chrbtn[i] = true;
				chrbtnactive = true;
			}
		}
	}
}

void ReleaseChrBtns()
{
	int i;

	chrbtnactive = false;
	for (i = 0; i < 4; ++i) {
		if (chrbtn[i]) {
			chrbtn[i] = false;
			if (Mouse.x >= ChrBtnsRect[i].x
			    && Mouse.x <= ChrBtnsRect[i].x + ChrBtnsRect[i].w
			    && Mouse.y >= ChrBtnsRect[i].y
			    && Mouse.y <= ChrBtnsRect[i].y + ChrBtnsRect[i].h) {
				switch (i) {
				case 0:
					NetSendCmdParam1(true, CMD_ADDSTR, 1);
					myplr().data._pStatPts--;
					break;
				case 1:
					NetSendCmdParam1(true, CMD_ADDMAG, 1);
					myplr().data._pStatPts--;
					break;
				case 2:
					NetSendCmdParam1(true, CMD_ADDDEX, 1);
					myplr().data._pStatPts--;
					break;
				case 3:
					NetSendCmdParam1(true, CMD_ADDVIT, 1);
					myplr().data._pStatPts--;
					break;
				}
			}
		}
	}
}

void DrawDurIcon()
{
	PlayerStruct *p;
	int x1, x2, x3, x4;

	if (!chrflag && !questlog || !invflag && !sbookflag) {
		x1 = 272 + RIGHT_PANEL_X;
		if (invflag || sbookflag)
			x1 = 272 + PANEL_X;
		p = &myplr().data;
		x2 = DrawDurIcon4Item(&p->InvBody[INVLOC_HEAD], x1, 4);
		x3 = DrawDurIcon4Item(&p->InvBody[INVLOC_CHEST], x2, 3);
		x4 = DrawDurIcon4Item(&p->InvBody[INVLOC_HAND_LEFT], x3, 0);
		DrawDurIcon4Item(&p->InvBody[INVLOC_HAND_RIGHT], x4, 0);
	}
}

int DrawDurIcon4Item(ItemStruct *pItem, int x, int c)
{
	if (pItem->_itype == ItemType::none)
		return x;
	if (pItem->_iDurability > 5)
		return x;
	if (c == 0) {
		if (pItem->_iClass == ItemClass::weapon) {
			switch (pItem->_itype) {
			case ItemType::sword:
				c = 2;
				break;
			case ItemType::axe:
				c = 6;
				break;
			case ItemType::bow:
				c = 7;
				break;
			case ItemType::mace:
				c = 5;
				break;
			case ItemType::staff:
				c = 8;
				break;
			}
		} else {
			c = 1;
		}
	}
	if (pItem->_iDurability > 2)
		c += 8;
	CelDraw(x, -17 + PANEL_Y, pDurIcons, c, 32);
	return x - 40;
}

void RedBack()
{
	int idx;

	idx = light4flag ? 1536 : 4608;

	assert(gpBuffer);

	int w, h;
	uint8_t *dst, *tbl;

	if (lvl.type() != DunType::hell) {
		dst = &gpBuffer[SCREENXY(0, 0)];
		tbl = &pLightTbl[idx];
		for (h = VIEWPORT_HEIGHT; h; h--, dst += BUFFER_WIDTH - SCREEN_WIDTH) {
			for (w = SCREEN_WIDTH; w; w--) {
				*dst = tbl[*dst];
				dst++;
			}
		}
	} else {
		dst = &gpBuffer[SCREENXY(0, 0)];
		tbl = &pLightTbl[idx];
		for (h = VIEWPORT_HEIGHT; h; h--, dst += BUFFER_WIDTH - SCREEN_WIDTH) {
			for (w = SCREEN_WIDTH; w; w--) {
				if (*dst >= 32)
					*dst = tbl[*dst];
				dst++;
			}
		}
	}
}

char GetSBookTrans(int ii, bool townok)
{
	char st;

	st = RSPLTYPE_SPELL;
	if (myplr().data._pISpells & (__int64)1 << (ii - 1)) {
		st = RSPLTYPE_CHARGES;
	}
	if (myplr().data._pAblSpells & (__int64)1 << (ii - 1)) { /// BUGFIX: missing (__int64) (fixed)
		st = RSPLTYPE_SKILL;
	}
	if (st == RSPLTYPE_SPELL) {
		if (!CheckSpell(myplr(), ii, RSPLTYPE_SPELL, true)) {
			st = RSPLTYPE_INVALID;
		}
		if ((char)(myplr().data._pSplLvl[ii] + myplr().data._pISplLvlAdd) <= 0) {
			st = RSPLTYPE_INVALID;
		}
	}
	if (townok && lvl.currlevel == 0 && st != RSPLTYPE_INVALID && !spelldata[ii].sTownSpell) {
		st = RSPLTYPE_INVALID;
	}

	return st;
}

void DrawSpellBook()
{
	int i, sn, mana, lvl, yp, min, max;
	char st;
	unsigned __int64 spl;

	CelDraw(RIGHT_PANEL_X, 351 + SCREEN_Y, pSpellBkCel, 1, SPANEL_WIDTH);
	CelDraw(RIGHT_PANEL_X + 76 * sbooktab + 7, 348 + SCREEN_Y, pSBkBtnCel, sbooktab + 1, 76);

	spl = myplr().data._pMemSpells | myplr().data._pISpells | myplr().data._pAblSpells;

	yp = 215;
	for (i = 1; i < 8; i++) {
		sn = SpellPages[sbooktab][i - 1];
		if (sn != -1 && spl & (__int64)1 << (sn - 1)) {
			st = GetSBookTrans(sn, true);
			SetSpellTrans(st);
			DrawSpellCel(RIGHT_PANEL + 75, yp, pSBkIconCels, SpellITbl[sn], 37);
			if (sn == myplr().data._pRSpell && st == myplr().data._pRSplType) {
				SetSpellTrans(RSPLTYPE_SKILL);
				DrawSpellCel(RIGHT_PANEL + 75, yp, pSBkIconCels, SPLICONLAST, 37);
			}
			PrintSBookStr({ 10, yp - 23 }, false, spelldata[sn].sNameText, COL_WHITE);
			switch (GetSBookTrans(sn, false)) {
			case RSPLTYPE_SKILL:
				strcpy(tempstr, "Skill");
				break;
			case RSPLTYPE_CHARGES:
				sprintf(tempstr, "Staff (%i charges)", myplr().data.InvBody[INVLOC_HAND_LEFT]._iCharges);
				break;
			default:
				mana = GetManaAmount(myplr(), sn) >> 6;
				GetDamageAmt(sn, &min, &max);
				if (min != -1) {
					sprintf(tempstr, "Mana: %i  Dam: %i - %i", mana, min, max);
				} else {
					sprintf(tempstr, "Mana: %i   Dam: n/a", mana);
				}
				if (sn == SPL_BONESPIRIT) {
					sprintf(tempstr, "Mana: %i  Dam: 1/3 tgt hp", mana);
				}
				PrintSBookStr({ 10, yp - 1 }, false, tempstr, COL_WHITE);
				lvl = myplr().data._pSplLvl[sn] + myplr().data._pISplLvlAdd;
				if (lvl < 0) {
					lvl = 0;
				}
				if (lvl == 0) {
					sprintf(tempstr, "Spell Level 0 - Unusable");
				} else {
					sprintf(tempstr, "Spell Level %i", lvl);
				}
				break;
			}
			PrintSBookStr({ 10, yp - 12 }, false, tempstr, COL_WHITE);
		}
		yp += 43;
	}
}

void PrintSBookStr(V2Di pos, bool cjustflag, char *pszStr, char col)
{
	uint8_t c;
	char *tmp;
	int screen_x, line, sx;

	sx = pos.x + RIGHT_PANEL + 120;
	line = 0;
	if (cjustflag) {
		screen_x = 0;
		tmp = pszStr;
		while (*tmp) {
			c = gbFontTransTbl[(uint8_t)*tmp++];
			screen_x += fontkern[fontframe[c]] + 1;
		}
		if (screen_x < 222)
			line = (222 - screen_x) >> 1;
		sx += line;
	}
	while (*pszStr) {
		c = gbFontTransTbl[(uint8_t)*pszStr++];
		c = fontframe[c];
		line += fontkern[c] + 1;
		if (c) {
			if (line <= 222)
				PrintChar({ sx, pos.y }, c, col);
		}
		sx += fontkern[c] + 1;
	}
}

void CheckSBook()
{
	int sn;
	char st;
	unsigned __int64 spl;

	if (Mouse.x >= RIGHT_PANEL + 11 && Mouse.x < RIGHT_PANEL + 48 && Mouse.y >= 18 && Mouse.y < 314) {
		spl = myplr().data._pMemSpells | myplr().data._pISpells | myplr().data._pAblSpells;
		sn = SpellPages[sbooktab][(Mouse.y - 18) / 43];
		if (sn != -1 && spl & (__int64)1 << (sn - 1)) {
			st = RSPLTYPE_SPELL;
			if (myplr().data._pISpells & (__int64)1 << (sn - 1)) {
				st = RSPLTYPE_CHARGES;
			}
			if (myplr().data._pAblSpells & (__int64)1 << (sn - 1)) {
				st = RSPLTYPE_SKILL;
			}
			myplr().data._pRSpell = sn;
			myplr().data._pRSplType = st;
			force_redraw = 255;
		}
	}
	if (Mouse.x >= RIGHT_PANEL + 7 && Mouse.x < RIGHT_PANEL + 311 && Mouse.y >= SPANEL_WIDTH && Mouse.y < 349) { /// BUGFIX: change `< 313` to `< 311` (fixed)
		sbooktab = (Mouse.x - (RIGHT_PANEL + 7)) / 76;
	}
}

char *get_pieces_str(int nGold)
{
	char *result;

	result = "piece";
	if (nGold != 1)
		result = "pieces";
	return result;
}

void DrawGoldSplit(int amount)
{
	int screen_x, i;

	screen_x = 0;
	CelDraw(351 + SCREEN_X, 178 + SCREEN_Y, pGBoxBuff, 1, 261);
	sprintf(tempstr, "You have %u gold", initialDropGoldValue);
	ADD_PlrStringXY(366, 87, 600, tempstr, COL_GOLD);
	sprintf(tempstr, "%s.  How many do", get_pieces_str(initialDropGoldValue));
	ADD_PlrStringXY(366, 103, 600, tempstr, COL_GOLD);
	ADD_PlrStringXY(366, 121, 600, "you want to remove?", COL_GOLD);
	if (amount > 0) {
		sprintf(tempstr, "%u", amount);
		PrintGameStr({ 388, 140 }, tempstr, COL_WHITE);
		for (i = 0; i < tempstr[i]; i++) {
			screen_x += fontkern[fontframe[gbFontTransTbl[(uint8_t)tempstr[i]]]] + 1;
		}
		screen_x += 452;
	} else {
		screen_x = 450;
	}
	CelDraw(screen_x, 140 + SCREEN_Y, pSPentSpn2Cels, PentSpn2Frame, 12);
	PentSpn2Spin();
}

void control_drop_gold(char vkey)
{
	char input[6];

	if (myplr().data._pHitPoints >> 6 <= 0) {
		dropGoldFlag = false;
		dropGoldValue = 0;
		return;
	}

	memset(input, 0, sizeof(input));
	snprintf(input, sizeof(input), "%d", dropGoldValue);
	if (vkey == DVL_VK_RETURN) {
		if (dropGoldValue > 0)
			control_remove_gold(myplr(), initialDropGoldIndex);
		dropGoldFlag = false;
	} else if (vkey == DVL_VK_ESCAPE) {
		dropGoldFlag = false;
		dropGoldValue = 0;
	} else if (vkey == DVL_VK_BACK) {
		input[strlen(input) - 1] = '\0';
		dropGoldValue = atoi(input);
	} else if (vkey - '0' >= 0 && vkey - '0' <= 9) {
		if (dropGoldValue != 0 || atoi(input) <= initialDropGoldValue) {
			input[strlen(input)] = vkey;
			if (atoi(input) > initialDropGoldValue)
				return;
			if (strlen(input) > strlen(input))
				return;
		} else {
			input[0] = vkey;
		}
		dropGoldValue = atoi(input);
	}
}

void control_remove_gold(int pnum, int gold_index)
{
	int gi;

	if (gold_index <= INVITEM_INV_LAST) {
		gi = gold_index - INVITEM_INV_FIRST;
		plr[pnum].data.InvList[gi]._ivalue -= dropGoldValue;
		if (plr[pnum].data.InvList[gi]._ivalue > 0)
			SetGoldCurs(pnum, gi);
		else
			plr[pnum].inventory.RemoveInvItem(gi);
	} else {
		gi = gold_index - INVITEM_BELT_FIRST;
		plr[pnum].data.SpdList[gi]._ivalue -= dropGoldValue;
		if (plr[pnum].data.SpdList[gi]._ivalue > 0)
			SetSpdbarGoldCurs(pnum, gi);
		else
			plr[pnum].inventory.removeBeltItem(gi);
	}
	SetPlrHandItem(&plr[pnum].data.HoldItem, ItemIndex::GOLD);
	GetGoldSeed(pnum, &plr[pnum].data.HoldItem);
	plr[pnum].data.HoldItem._ivalue = dropGoldValue;
	plr[pnum].data.HoldItem._iStatFlag = true;
	control_set_gold_curs(pnum);
	plr[pnum].data._pGold = plr[pnum].inventory.CalculateGold();
	dropGoldValue = 0;
}

void control_set_gold_curs(int pnum)
{
	if (plr[pnum].data.HoldItem._ivalue >= GOLD_MEDIUM_LIMIT)
		plr[pnum].data.HoldItem._iCurs = ItemCursor::GOLD_LARGE;
	else if (plr[pnum].data.HoldItem._ivalue <= GOLD_SMALL_LIMIT)
		plr[pnum].data.HoldItem._iCurs = ItemCursor::GOLD_SMALL;
	else
		plr[pnum].data.HoldItem._iCurs = ItemCursor::GOLD_MEDIUM;

	NewCursor(plr[pnum].data.HoldItem._iCurs + CURSOR_FIRSTITEM);
}

void DrawTalkPan()
{
	int i, off, talk_btn, color, nCel, x;
	char *msg;

	if (!talkflag)
		return;

	DrawPanelBox({ 175, sgbPlrTalkTbl + 20, 294, 5 }, { PANEL_X + 175, PANEL_Y + 4 });
	off = 0;
	for (i = 293; i > 283; off++, i--) {
		DrawPanelBox({ (off >> 1) + 175, sgbPlrTalkTbl + off + 25, i, 1 }, { (off >> 1) + PANEL_X + 175, off + PANEL_Y + 9 });
	}
	DrawPanelBox({ 185, sgbPlrTalkTbl + 35, 274, 30 }, { PANEL_X + 185, PANEL_Y + 19 });
	DrawPanelBox({ 180, sgbPlrTalkTbl + 65, 284, 5 }, { PANEL_X + 180, PANEL_Y + 49 });
	for (i = 0; i < 10; i++) {
		DrawPanelBox({ 180, sgbPlrTalkTbl + i + 70, i + 284, 1 }, { PANEL_X + 180, i + PANEL_Y + 54 });
	}
	DrawPanelBox({ 170, sgbPlrTalkTbl + 80, 310, 55 }, { PANEL_X + 170, PANEL_Y + 64 });
	msg = sgszTalkMsg;
	for (i = 0; i < 39; i += 13) {
		x = 0 + PANEL_LEFT;
		msg = control_print_talk_msg(msg, &x, i, 0);
		if (!msg)
			break;
	}
	if (msg)
		*msg = '\0';
	CelDraw(x, i + 22 + PANEL_Y, pSPentSpn2Cels, PentSpn2Frame, 12);
	PentSpn2Spin();
	talk_btn = 0;
	for (i = 0; i < 4; i++) {
		if (i == myplr())
			continue;
		if (whisper[i]) {
			color = COL_GOLD;
			if (talkbtndown[talk_btn]) {
				if (talk_btn != 0)
					nCel = 4;
				else
					nCel = 3;
				CelDraw(172 + PANEL_X, 84 + 18 * talk_btn + PANEL_Y, pTalkBtns, nCel, 61);
			}
		} else {
			color = COL_RED;
			if (talk_btn != 0)
				nCel = 2;
			else
				nCel = 1;
			if (talkbtndown[talk_btn])
				nCel += 4;
			CelDraw(172 + PANEL_X, 84 + 18 * talk_btn + PANEL_Y, pTalkBtns, nCel, 61);
		}
		if (plr[i].data.plractive) {
			x = 46 + PANEL_LEFT;
			control_print_talk_msg(plr[i].data._pName, &x, 60 + talk_btn * 18, color);
		}

		talk_btn++;
	}
}

char *control_print_talk_msg(char *msg, int *x, int y, int color)
{
	uint8_t c;
	int width;

	*x += 264;
	width = *x;
	while (*msg) {

		c = fontframe[gbFontTransTbl[(uint8_t)*msg]];
		width += fontkern[c] + 1;
		if (width > 514 + PANEL_LEFT)
			return msg;
		msg++;
		if (c != 0) {
			PrintChar({ *x, y + 22 + PANEL_Y }, c, color);
		}
		*x += fontkern[c] + 1;
	}
	return NULL;
}

bool control_check_talk_btn()
{
	int i;

	if (!talkflag)
		return false;

	if (Mouse.x < 172 + PANEL_LEFT)
		return false;
	if (Mouse.y < 69 + PANEL_TOP)
		return false;
	if (Mouse.x > 233 + PANEL_LEFT)
		return false;
	if (Mouse.y > 123 + PANEL_TOP)
		return false;

	for (i = 0; i < sizeof(talkbtndown) / sizeof(talkbtndown[0]); i++) {
		talkbtndown[i] = false;
	}

	talkbtndown[(Mouse.y - (69 + PANEL_TOP)) / 18] = true;

	return true;
}

void control_release_talk_btn()
{
	int i, p, off;

	if (talkflag) {
		for (i = 0; i < sizeof(talkbtndown) / sizeof(talkbtndown[0]); i++)
			talkbtndown[i] = false;
		if (Mouse.x >= 172 + PANEL_LEFT && Mouse.y >= 421 + PANEL_LEFT && Mouse.x <= -119 + PANEL_TOP && Mouse.y <= 123 + PANEL_TOP) {
			off = (Mouse.y - (69 + PANEL_TOP)) / 18;

			for (p = 0; p < MAX_PLRS && off != -1; p++) {
				if (p != myplr())
					off--;
			}
			if (p <= MAX_PLRS)
				whisper[p - 1] = !whisper[p - 1];
		}
	}
}

void control_reset_talk_msg(char *msg)
{
	int i, pmask;
	pmask = 0;

	for (i = 0; i < MAX_PLRS; i++) {
		if (whisper[i])
			pmask |= 1 << i;
	}
	NetSendCmdString(pmask, sgszTalkMsg);
}

void control_type_message()
{
	int i;

	if (plr.isSingleplayer()) {
		return;
	}

	talkflag = true;
	sgszTalkMsg[0] = '\0';
	PentSpn2Frame = 1;
	for (i = 0; i < 3; i++) {
		talkbtndown[i] = false;
	}
	sgbPlrTalkTbl = PANEL_HEIGHT + 16;
	force_redraw = 255;
	sgbTalkSavePos = sgbNextTalkSave;
}

void control_reset_talk()
{
	talkflag = false;
	sgbPlrTalkTbl = 0;
	force_redraw = 255;
}

bool control_talk_last_key(int vkey)
{
	int result;

	if (plr.isSingleplayer())
		return false;

	if (!talkflag)
		return false;

	if ((DWORD)vkey < DVL_VK_SPACE)
		return false;

	result = strlen(sgszTalkMsg);
	if (result < 78) {
		sgszTalkMsg[result] = vkey;
		sgszTalkMsg[result + 1] = '\0';
	}
	return true;
}

bool control_presskeys(int vkey)
{
	int len;
	bool ret;

	if (plr.isMultiplayer()) {
		if (!talkflag) {
			ret = false;
		} else {
			if (vkey == DVL_VK_SPACE) {
			} else if (vkey == DVL_VK_ESCAPE) {
				control_reset_talk();
			} else if (vkey == DVL_VK_RETURN) {
				control_press_enter();
			} else if (vkey == DVL_VK_BACK) {
				len = strlen(sgszTalkMsg);
				if (len > 0)
					sgszTalkMsg[len - 1] = '\0';
			} else if (vkey == DVL_VK_DOWN) {
				control_up_down(1);
			} else if (vkey == DVL_VK_UP) {
				control_up_down(-1);
			} else {
				return false;
			}
			ret = true;
		}
	} else {
		ret = false;
	}
	return ret;
}

void control_press_enter()
{
	int i;
	uint8_t talk_save;

	if (sgszTalkMsg[0] != 0) {
		control_reset_talk_msg(sgszTalkMsg);
		for (i = 0; i < 8; i++) {
			if (!strcmp(sgszTalkSave[i], sgszTalkMsg))
				break;
		}
		if (i >= 8) {
			strcpy(sgszTalkSave[sgbNextTalkSave], sgszTalkMsg);
			sgbNextTalkSave++;
			sgbNextTalkSave &= 7;
		} else {
			talk_save = sgbNextTalkSave - 1;
			talk_save &= 7;
			if (i != talk_save) {
				strcpy(sgszTalkSave[i], sgszTalkSave[talk_save]);
				strcpy(sgszTalkSave[talk_save], sgszTalkMsg);
			}
		}
		sgszTalkMsg[0] = '\0';
		sgbTalkSavePos = sgbNextTalkSave;
	}
	control_reset_talk();
}

void control_up_down(int v)
{
	int i;

	for (i = 0; i < 8; i++) {
		sgbTalkSavePos = (v + sgbTalkSavePos) & 7;
		if (sgszTalkSave[sgbTalkSavePos][0]) {
			strcpy(sgszTalkMsg, sgszTalkSave[sgbTalkSavePos]);
			return;
		}
	}
}

DEVILUTION_END_NAMESPACE
