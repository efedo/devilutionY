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

extern BOOL invflag;
extern BOOL drawsbarflag;
extern const InvXY InvRect[73];

class Player;

class PlayerInventory {
public:
	PlayerInventory(Player &);
	BOOL AutoPlace(int ii, int sx, int sy, BOOL saveflag);
	BOOL SpecialAutoPlace(int ii, int sx, int sy, BOOL saveflag);
	BOOL GoldAutoPlace();
	BOOL WeaponAutoPlace();
	void CheckInvPaste(int mx, int my);
	void CheckInvSwap(BYTE bLoc, int idx, WORD wCI, int seed, BOOL bId);
	void CheckInvCut(int mx, int my);
	void inv_update_rem_item(BYTE iv);
	void RemoveInvItem(int iv);
	void RemoveSpdBarItem(int iv);
	void CheckItemStats();
	void CheckBookLevel();
	void CheckQuestItem();
	void InvGetItem(int ii);
	void AutoGetItem(int ii);
	int InvPutItem(int x, int y);
	int SyncPutItem(int x, int y, int idx, WORD icreateinfo, int iseed, int Id, int dur, int mdur, int ch, int mch, int ivalue, DWORD ibuff);
	void RemoveScroll();
	void UseStaffCharge();
	BOOL UseInvItem(int cii);
	int CalculateGold();
private:
	Player & owner;
};

void FreeInvGFX();
void InitInv();
void InvDrawSlotBack(int X, int Y, int W, int H);
void DrawInv();
void DrawInvBelt();
int SwapItem(ItemStruct *a, ItemStruct *b);
void CheckInvItem();
void CheckInvScrn();
int FindGetItem(int idx, WORD ci, int iseed);
void SyncGetItem(int x, int y, int idx, WORD ci, int iseed);
BOOL CanPut(int x, int y);
BOOL TryInvPut();
void DrawInvMsg(char *msg);
char CheckInvHLight();
BOOL UseScroll();
BOOL UseStaff();
void StartGoldDrop();
void DoTelekinesis();
BOOL DropItemBeforeTrig();

/* data */

extern int AP2x2Tbl[10];

//#ifdef __cplusplus
//}
//#endif

DEVILUTION_END_NAMESPACE

#endif /* __INV_H__ */
