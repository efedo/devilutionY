//HEADER_GOES_HERE
#ifndef __STORES_H__
#define __STORES_H__

#include <vector>

DEVILUTION_BEGIN_NAMESPACE

class Store {
	std::vector<ItemStruct> items;
	void start();
	void enter();
};

extern int stextup;
extern int storenumh;
extern int stextlhold;
extern ItemStruct boyitem;
extern int stextshold;
extern uint8_t *pSTextBoxCels;
extern int premiumlevel;
extern int talker;
extern STextStruct stext[24];
extern char stextsize;
extern int stextsmax;
extern ItemStruct storehold[48];
extern int gossipstart;
extern bool stextscrl;
extern int numpremium;
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
extern int boylevel;
extern int stextdown;
extern char stextscrlubtn;
extern char stextflag;

// Smith
extern ItemStruct smithitem[SMITH_ITEMS];
extern ItemStruct premiumitem[SMITH_PREMIUM_ITEMS];

// Healer
extern ItemStruct healitem[20];

// With
extern ItemStruct witchitem[20];

void InitStores();
void PentSpn2Spin();
void SetupTownStores();
void FreeStoreMem();
void DrawSTextBack();
void PrintSString(int x, int y, bool cjustflag, char *str, char col, int val);
void DrawSLine(int y);
void DrawSSlider(int y1, int y2);
void DrawSTextHelp();
void ClearSText(int s, int e);
void AddSLine(int y);
void AddSTextVal(int y, int val);
void OffsetSTextY(int y, int yo);
void AddSText(int x, int y, bool j, char *str, char clr, bool sel);
void StoreAutoPlace();

// Smith
void S_StartSmith();
void S_SmithEnter();

bool SmithSellOk(int i);
void S_ScrollSBuy(int idx);
void PrintStoreItem(ItemStruct *x, int l, char iclr);
void S_StartSBuy();
void S_ScrollSPBuy(int idx);
bool S_StartSPBuy();
void S_ScrollSSell(int idx);
void S_StartSSell();
bool SmithRepairOk(int i);
void S_StartSRepair();
void SmithBuyItem();
void S_SBuyEnter();
void SmithBuyPItem();
void S_SPBuyEnter();
void S_SSellEnter();
void SmithRepairItem();
void S_SRepairEnter();

// Witch
void S_StartWitch();
void S_WitchEnter();

void S_ScrollWBuy(int idx);
void S_StartWBuy();
bool WitchSellOk(int i);
void S_StartWSell();

bool WitchRechargeOk(int i);
void AddStoreHoldRecharge(ItemStruct itm, int i);
void S_StartWRecharge();
void WitchBuyItem();
void S_WBuyEnter();
void S_WSellEnter();
void WitchRechargeItem();
void S_WRechargeEnter();

// Wirt
void S_StartBoy();
void S_BoyEnter();

void S_StartBBoy();
void BoyBuyItem();

// Healer
void S_StartHealer();
void S_HealerEnter();

void HealerBuyItem();
void S_HBuyEnter();
void S_ScrollHBuy(int idx);
void S_StartHBuy();

// Misc
void S_StartTavern();
void S_StartBarMaid();
void S_StartDrunk();
void S_TavernEnter();
void S_BarmaidEnter();
void S_DrunkEnter();

void AddStoreHoldRepair(ItemStruct *itm, int i);
void S_StartNoMoney();
void S_StartNoRoom();
void S_StartConfirm();
void S_StartStory();
bool IdItemOk(ItemStruct *i);
void AddStoreHoldId(ItemStruct itm, int i);
void S_StartSIdentify();
void S_StartIdShow();
void S_StartTalk();
void StartStore(char s);
void DrawSText();
void STextESC();
void STextUp();
void STextDown();
void STextPrior();
void STextNext();

void SetGoldCurs(int pnum, int i);
void SetSpdbarGoldCurs(int pnum, int i);
void TakePlrsMoney(int cost);
bool StoreGoldFit(int idx);
void PlaceStoreGold(int v);
void StoreSellItem();

void S_BBuyEnter();
void StoryIdItem();
void S_ConfirmEnter();

void S_StoryEnter();
void S_SIDEnter();
void S_TalkEnter();

void STextEnter();
void CheckStoreBtn();
void ReleaseStoreBtn();

/* rdata */

extern char *talkname[9];

DEVILUTION_END_NAMESPACE

#endif /* __STORES_H__ */
