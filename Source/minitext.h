/**
 * @file minitext.h
 *
 * Interface of scrolling dialog text.
 */
#ifndef __MINITEXT_H__
#define __MINITEXT_H__

DEVILUTION_BEGIN_NAMESPACE

//#ifdef __cplusplus
//extern "C" {
//#endif

extern int qtexty;
extern char *qtextptr;
extern bool qtextflag;
extern uint8_t *pMedTextCels;
extern uint8_t *pTextBoxCels;

void FreeQuestText();
void InitQuestText();
void InitQTextMsg(int m);
void DrawQTextBack();
void PrintQTextChr(int sx, int sy, uint8_t *pCelBuff, int nCel);
void DrawQText();

/* rdata */

extern const uint8_t mfontframe[127];
extern const uint8_t mfontkern[56];

/* data */

extern int qscroll_spd_tbl[9];

//#ifdef __cplusplus
//}
//#endif

DEVILUTION_END_NAMESPACE

#endif /* __MINITEXT_H__ */
