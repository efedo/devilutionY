/**
 * @file inv.h
 *
 * Interface of player inventory.
 */
#ifndef __INV_H__
#define __INV_H__

DEVILUTION_BEGIN_NAMESPACE

//#ifdef __cplusplus
//extern "C" {
//#endif

extern bool invflag;
extern bool drawsbarflag;
extern const InvXY InvRect[73];

class Player;

class PlayerInventory {
public:
	PlayerInventory(Player &);
	bool AutoPlace(int ii, V2Di pos, bool saveflag);
	bool SpecialAutoPlace(int ii, V2Di pos, bool saveflag);
	bool GoldAutoPlace();
	bool WeaponAutoPlace();
	void CheckInvPaste(V2Di pos);
	void CheckInvSwap(uint8_t bLoc, int idx, WORD wCI, int seed, bool bId);
	void CheckInvCut(V2Di pos);
	void inv_update_rem_item(uint8_t iv);
	void RemoveInvItem(int iv);
	void RemoveSpdBarItem(int iv);
	void CheckItemStats();
	void CheckBookLevel();
	void CheckQuestItem();
	void InvGetItem(int ii);
	void AutoGetItem(int ii);
	int InvPutItem(V2Di pos);
	int SyncPutItem(V2Di pos, int idx, WORD icreateinfo, int iseed, int Id, int dur, int mdur, int ch, int mch, int ivalue, DWORD ibuff);
	int _PrepPutItem(V2Di &pos); // EF Added
	void RemoveScroll();
	void UseStaffCharge();
	bool UseInvItem(int cii);
	int CalculateGold();
private:
	Player & owner;
};

void FreeInvGFX();
void InitInv();
void InvDrawSlotBack(RECT32 r);
void DrawInv();
void DrawInvBelt();
int SwapItem(ItemStruct *a, ItemStruct *b);
void CheckInvItem();
void CheckInvScrn();
int FindGetItem(int idx, WORD ci, int iseed);
void SyncGetItem(V2Di pos, int idx, WORD ci, int iseed);
bool CanPut(V2Di pos);
bool TryInvPut();
void DrawInvMsg(char *msg);
char CheckInvHLight();
bool UseScroll();
bool UseStaff();
void StartGoldDrop();
void DoTelekinesis();
bool DropItemBeforeTrig();

/* data */

extern int AP2x2Tbl[10];

//#ifdef __cplusplus
//}
//#endif

DEVILUTION_END_NAMESPACE

#endif /* __INV_H__ */
