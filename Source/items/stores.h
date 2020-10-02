//HEADER_GOES_HERE
#ifndef __STORES_H__
#define __STORES_H__

#include <vector>

DEVILUTION_BEGIN_NAMESPACE

extern int stextup;
extern int storenumh;
extern int stextlhold;
extern int stextshold;
extern uint8_t *pSTextBoxCels;
extern int talker;

extern STextStruct stext[24];


extern char stextsize;
extern int stextsmax;
extern ItemStruct storehold[48];
extern int gossipstart;
extern bool stextscrl;
extern ItemStruct golditem;
extern char storehidx[48];
extern uint8_t *pSTextSlidCels;
extern int stextvhold;
extern int stextsel;
extern char stextscrldbtn;
extern int gossipend;
extern uint8_t *pSPentSpn2Cels;
extern uint8_t PentSpn2Frame;
extern int stextsval;
extern int stextdown;
extern char stextscrlubtn;
extern char stextflag;

void InitStores();
void PentSpn2Spin();
void SetupTownStores();
void FreeStoreMem();

void StartStore(char s);

void StoreAutoPlace();
void AddStoreHoldRepair(ItemStruct *itm, int i);

bool IdItemOk(ItemStruct *i);
void AddStoreHoldId(ItemStruct itm, int i);

void CheckStoreBtn();
void ReleaseStoreBtn();


void SetGoldCurs(int pnum, int i);
void SetSpdbarGoldCurs(int pnum, int i);
void TakePlrsMoney(int cost);
bool StoreGoldFit(int idx);
void PlaceStoreGold(int v);
void StoreSellItem();
void PrintStoreItem(ItemStruct *x, int l, char iclr);
void StoryIdItem();

/* rdata */

extern char *talkname[9];

DEVILUTION_END_NAMESPACE

#endif /* __STORES_H__ */
