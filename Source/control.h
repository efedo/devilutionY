/**
 * @file control.h
 *
 * Interface of the character and main control panels
 */
#ifndef __CONTROL_H__
#define __CONTROL_H__

DEVILUTION_BEGIN_NAMESPACE

extern uint8_t *pDurIcons;
extern uint8_t *pChrButtons;
extern bool drawhpflag;
extern bool dropGoldFlag;
extern bool panbtn[8];
extern bool chrbtn[4];
extern uint8_t *pMultiBtns;
extern uint8_t *pPanelButtons;
extern uint8_t *pChrPanel;
extern bool lvlbtndown;
extern int dropGoldValue;
extern bool drawmanaflag;
extern bool chrbtnactive;
extern uint8_t *pPanelText;
extern uint8_t *pLifeBuff;
extern uint8_t *pBtmBuff;
extern uint8_t *pTalkBtns;
extern bool pstrjust[4];
extern int pnumlines;
extern bool pinfoflag;
extern bool talkbtndown[3];
extern int pSpell;
extern uint8_t *pManaBuff;
extern char infoclr;
extern uint8_t *pGBoxBuff;
extern uint8_t *pSBkBtnCel;
extern char tempstr[256];
extern bool whisper[MAX_PLRS];
extern int sbooktab;
extern int pSplType;
extern int initialDropGoldIndex;
extern bool talkflag;
extern uint8_t *pSBkIconCels;
extern bool sbookflag;
extern bool chrflag;
extern bool drawbtnflag;
extern uint8_t *pSpellBkCel;
extern char infostr[256];
extern int numpanbtns;
extern uint8_t *pStatusPanel;
extern char panelstr[4][64];
extern bool panelflag;
extern uint8_t SplTransTbl[256];
extern int initialDropGoldValue;
extern uint8_t *pSpellCels;
extern bool panbtndown;
extern uint8_t *pTalkPanel;
extern bool spselflag;

void DrawSpellCel(int px, int py, uint8_t *Trans, int nCel, int w);
void DrawSpellCel(V2Di p, uint8_t *Trans, int nCel, int w);
void SetSpellTrans(char t);
void DrawSpell();
void DrawSpellList();
void SetSpell();
void SetSpeedSpell(int slot);
void ToggleSpell(int slot);
void PrintChar(V2Di s, int nCel, char col);
void AddPanelString(char *str, bool just);
void ClearPanel();
void DrawPanelBox(RECT32 r, V2Di s);
void InitPanelStr();
void SetFlaskHeight(uint8_t *pCelBuff, int min, int max, V2Di s);
void DrawFlask(uint8_t *pCelBuff, int w, int nSrcOff, uint8_t *pBuff, int nDstOff, int h);
void DrawLifeFlask();
void UpdateLifeFlask();
void DrawManaFlask();
void control_update_life_mana();
void UpdateManaFlask();
void InitControlPan();
void DrawCtrlPan();
void DrawCtrlBtns();
void DoSpeedBook();
void DoPanBtn();
void control_set_button_down(int btn_id);
void control_check_btn_press();
void DoAutoMap();
void CheckPanelInfo();
void CheckBtnUp();
void FreeControlPan();
bool control_WriteStringToBuffer(uint8_t *str);
void DrawInfoBox();
void PrintInfo();
void CPrintString(int y, char *str, bool center, int lines);
void PrintGameStr(V2Di pos, const char *str, int color);
void DrawChr();
#define ADD_PlrStringXY(x, y, width, pszStr, col) MY_PlrStringXY(x, y, width, pszStr, col, 1)
void MY_PlrStringXY(int x, int y, int width, char *pszStr, char col, int base);
void MY_PlrStringXY(V2Di pos, int width, char *pszStr, char col, int base);
void CheckLvlBtn();
void ReleaseLvlBtn();
void DrawLevelUpIcon();
void CheckChrBtns();
void ReleaseChrBtns();
void DrawDurIcon();
int DrawDurIcon4Item(ItemStruct *pItem, int x, int c);
void RedBack();
char GetSBookTrans(int ii, bool townok);
void DrawSpellBook();
void PrintSBookStr(V2Di pos, bool cjustflag, char *pszStr, char col);
void CheckSBook();
char *get_pieces_str(int nGold);
void DrawGoldSplit(int amount);
void control_drop_gold(char vkey);
void control_remove_gold(int pnum, int gold_index);
void control_set_gold_curs(int pnum);
void DrawTalkPan();
char *control_print_talk_msg(char *msg, int *x, int y, int just);
bool control_check_talk_btn();
void control_release_talk_btn();
void control_reset_talk_msg();
void control_type_message();
void control_reset_talk();
bool control_talk_last_key(int vkey);
bool control_presskeys(int vkey);
void control_press_enter();
void control_up_down(int v);

/* rdata */
extern const uint8_t fontframe[128];
extern const uint8_t fontkern[68];
extern const int lineOffsets[5][5];
extern const uint8_t gbFontTransTbl[256];

/* data */

extern char SpellITbl[MAX_SPELLS];
extern int PanBtnPos[8][5];
extern char *PanBtnHotKey[8];
extern char *PanBtnStr[8];
extern RECT32 ChrBtnsRect[4];
extern int SpellPages[6][7];

DEVILUTION_END_NAMESPACE

#endif /* __CONTROL_H__ */
