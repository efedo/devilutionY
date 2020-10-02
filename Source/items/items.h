/**
 * @file items.h
 *
 * Interface of item functionality.
 */
#ifndef __ITEMS_H__
#define __ITEMS_H__


DEVILUTION_BEGIN_NAMESPACE

class Player;

class ItemTemplate {
public:
	uint8_t *anims;
};

extern ItemTemplate itemTypes[ITEMTYPES];

extern bool UniqueItemFlag[128];

extern int gnNumGetRecords; // num item records
extern ItemGetRecordStruct itemrecord[MAXITEMS];

extern bool itemhold[3][3];

extern bool uitemflag; // flags if a unique item is currently highlighted
extern ItemStruct curruitem; // current unique item (for info. display)

void InitItemGFX();
bool ItemPlace(V2Di pos);
void AddInitItems();
void InitItems();

void SetPlrHandItem(ItemStruct *h, int idata);
void GetPlrHandSeed(ItemStruct *h);
bool ItemMinStats(PlayerStruct *p, ItemStruct *x);
void SetPlrHandSeed(ItemStruct *h, int iseed);
void SetPlrHandGoldCurs(ItemStruct *h);

bool ItemSpaceOk(int i, int j);
void GetSuperItemLoc(V2Di pos, V2Di &r);
int RndPL(int param1, int param2);
int PLVal(int pv, int p1, int p2, int minv, int maxv);

int RndItem(int m);
int RndUItem(int m);
int RndAllItems();
int RndTypeItems(int itype, int imid);

void SpawnUnique(int uid, V2Di pos);
void SpawnItem(int m, V2Di pos, bool sendmsg);
void CreateItem(int uid, V2Di pos);

void CreateRndItem(V2Di pos, bool onlygood, bool sendmsg, bool delta);
void CreateTypeItem(V2Di pos, bool onlygood, int itype, int imisc, bool sendmsg, bool delta);
void SpawnQuestItem(int itemid, V2Di pos, int randarea, int selflag);
void CreateSpellBook(V2Di pos, int ispell, bool sendmsg, bool delta);
void CreateMagicArmor(V2Di pos, int imisc, int icurs, bool sendmsg, bool delta);
void CreateMagicWeapon(V2Di pos, int imisc, int icurs, bool sendmsg,
                       bool delta);

void SpawnRock();
void ItemDoppel();
void ProcessItems();
void FreeItemGFX();

void DrawUTextBack();
void PrintUString(V2Di pos, bool cjustflag, char *str, int col);
void DrawULine(int y);
void DrawUniqueInfo();

void SpawnStoreGold();
void RecalcStoreStats();
int ItemNoFlippy();

bool GetItemRecord(int nSeed, WORD wCI, int nIndex);
void NextItemRecord(int i);
void SetItemRecord(int nSeed, WORD wCI, int nIndex);
void PutItemRecord(int nSeed, WORD wCI, int nIndex);

/* data */

extern uint8_t ItemCAnimTbl[];
extern char *ItemDropNames[];
extern uint8_t ItemAnimLs[];
extern int ItemDropSnds[];
extern int ItemInvSnds[];
extern int idoppely;
extern int premiumlvladd[6];

DEVILUTION_END_NAMESPACE

#endif /* __ITEMS_H__ */
