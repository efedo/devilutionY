/**
 * @file items.h
 *
 * Interface of item functionality.
 */
#ifndef __ITEMS_H__
#define __ITEMS_H__

DEVILUTION_BEGIN_NAMESPACE

class Player;

class Item {
public:
	Item(Player *);
	Player &owner();
private:
	Player *_owner = 0;
};

extern int itemactive[MAXITEMS];
extern int itemavail[MAXITEMS];
extern ItemGetRecordStruct itemrecord[MAXITEMS];

extern BOOL uitemflag;

extern ItemStruct curruitem;
extern ItemStruct item[MAXITEMS + 1];

extern BOOL itemhold[3][3];
extern BYTE *itemanims[ITEMTYPES];
extern BOOL UniqueItemFlag[128];
extern int numitems;
extern int gnNumGetRecords;

void InitItemGFX();
BOOL ItemPlace(V2Di pos);
void AddInitItems();
void InitItems();
void CalcPlrItemVals(int p, BOOL Loadgfx);
void CalcPlrScrolls(int p);
void CalcPlrStaff(int p);
void CalcSelfItems(int pnum);
void CalcPlrItemMin(int pnum);
BOOL ItemMinStats(PlayerStruct *p, ItemStruct *x);
void CalcPlrBookVals(int p);
void CalcPlrInv(int p, BOOL Loadgfx);
void SetPlrHandItem(ItemStruct *h, int idata);
void GetPlrHandSeed(ItemStruct *h);
void GetGoldSeed(int pnum, ItemStruct *h);
void SetPlrHandSeed(ItemStruct *h, int iseed);
void SetPlrHandGoldCurs(ItemStruct *h);
void CreatePlrItems(int p);
BOOL ItemSpaceOk(int i, int j);
BOOL GetItemSpace(V2Di pos, char inum);
void GetSuperItemSpace(V2Di pos, char inum);
void GetSuperItemLoc(V2Di pos, V2Di &r);
void CalcItemValue(int i);
void GetBookSpell(int i, int lvl);
void GetStaffPower(int i, int lvl, int bs, BOOL onlygood);
void GetStaffSpell(int i, int lvl, BOOL onlygood);
void GetItemAttrs(int i, int idata, int lvl);
int RndPL(int param1, int param2);
int PLVal(int pv, int p1, int p2, int minv, int maxv);
void SaveItemPower(int i, int power, int param1, int param2, int minval, int maxval, int multval);
void GetItemPower(int i, int minlvl, int maxlvl, int flgs, BOOL onlygood);
void GetItemBonus(int i, int idata, int minlvl, int maxlvl, BOOL onlygood);
void SetupItem(int i);
int RndItem(int m);
int RndUItem(int m);
int RndAllItems();
int RndTypeItems(int itype, int imid);
int CheckUnique(int i, int lvl, int uper, BOOL recreate);
void GetUniqueItem(int i, int uid);
void SpawnUnique(int uid, V2Di pos);
void ItemRndDur(int ii);
void SetupAllItems(int ii, int idx, int iseed, int lvl, int uper, int onlygood, BOOL recreate, BOOL pregen);
void SpawnItem(int m, V2Di pos, BOOL sendmsg);
void CreateItem(int uid, V2Di pos);
void CreateRndItem(V2Di pos, BOOL onlygood, BOOL sendmsg, BOOL delta);
void SetupAllUseful(int ii, int iseed, int lvl);
void CreateRndUseful(int pnum, V2Di pos, BOOL sendmsg);
void CreateTypeItem(V2Di pos, BOOL onlygood, int itype, int imisc, BOOL sendmsg, BOOL delta);
void RecreateItem(int ii, int idx, WORD icreateinfo, int iseed, int ivalue);
void RecreateEar(int ii, WORD ic, int iseed, int Id, int dur, int mdur, int ch, int mch, int ivalue, int ibuff);
void SpawnQuestItem(int itemid, V2Di pos, int randarea, int selflag);
void SpawnRock();
void RespawnItem(int i, BOOL FlipFlag);
void DeleteItem(int ii, int i);
void ItemDoppel();
void ProcessItems();
void FreeItemGFX();
void GetItemFrm(int i);
void GetItemStr(int i);
void CheckIdentify(int pnum, int cii);
void DoRepair(int pnum, int cii);
void RepairItem(ItemStruct *i, int lvl);
void DoRecharge(int pnum, int cii);
void RechargeItem(ItemStruct *i, int r);
void PrintItemOil(char IDidx);
void PrintItemPower(char plidx, ItemStruct *x);
void DrawUTextBack();
void PrintUString(V2Di pos, BOOL cjustflag, char *str, int col);
void DrawULine(int y);
void DrawUniqueInfo();
void PrintItemMisc(ItemStruct *x);
void PrintItemDetails(ItemStruct *x);
void PrintItemDur(ItemStruct *x);
void UseItem(int p, int Mid, int spl);
BOOL StoreStatOk(ItemStruct *h);
BOOL SmithItemOk(int i);
int RndSmithItem(int lvl);
void BubbleSwapItem(ItemStruct *a, ItemStruct *b);
void SortSmith();
void SpawnSmith(int lvl);
BOOL PremiumItemOk(int i);
int RndPremiumItem(int minlvl, int maxlvl);
void SpawnOnePremium(int i, int plvl);
void SpawnPremium(int lvl);
BOOL WitchItemOk(int i);
int RndWitchItem(int lvl);
void SortWitch();
void WitchBookLevel(int ii);
void SpawnWitch(int lvl);
int RndBoyItem(int lvl);
void SpawnBoy(int lvl);
BOOL HealerItemOk(int i);
int RndHealerItem(int lvl);
void SortHealer();
void SpawnHealer(int lvl);
void SpawnStoreGold();
void RecreateSmithItem(int ii, int idx, int lvl, int iseed);
void RecreatePremiumItem(int ii, int idx, int plvl, int iseed);
void RecreateBoyItem(int ii, int idx, int lvl, int iseed);
void RecreateWitchItem(int ii, int idx, int lvl, int iseed);
void RecreateHealerItem(int ii, int idx, int lvl, int iseed);
void RecreateTownItem(int ii, int idx, WORD icreateinfo, int iseed, int ivalue);
void RecalcStoreStats();
int ItemNoFlippy();
void CreateSpellBook(V2Di pos, int ispell, BOOL sendmsg, BOOL delta);
void CreateMagicArmor(V2Di pos, int imisc, int icurs, BOOL sendmsg, BOOL delta);
void CreateMagicWeapon(V2Di pos, int imisc, int icurs, BOOL sendmsg, BOOL delta);
BOOL GetItemRecord(int nSeed, WORD wCI, int nIndex);
void NextItemRecord(int i);
void SetItemRecord(int nSeed, WORD wCI, int nIndex);
void PutItemRecord(int nSeed, WORD wCI, int nIndex);

/* data */

extern BYTE ItemCAnimTbl[];
extern char *ItemDropNames[];
extern BYTE ItemAnimLs[];
extern int ItemDropSnds[];
extern int ItemInvSnds[];
extern int idoppely;
extern int premiumlvladd[6];

DEVILUTION_END_NAMESPACE

#endif /* __ITEMS_H__ */
