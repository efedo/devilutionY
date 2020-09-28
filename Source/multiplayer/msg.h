/**
 * @file msg.h
 *
 * Interface of function for sending and reciving network messages.
 */
#ifndef __MSG_H__
#define __MSG_H__

DEVILUTION_BEGIN_NAMESPACE

//#ifdef __cplusplus
//extern "C" {
//#endif

extern bool deltaload;
extern uint8_t gbBufferMsgs;
extern int dwRecCount;

void msg_send_drop_pkt(int pnum, int reason);
void msg_send_packet(int pnum, const void *packet, DWORD dwSize);
void msg_get_next_packet();
bool msg_wait_resync();
void msg_free_packets();
int msg_wait_for_turns();
void run_delta_info();
void msg_pre_packet();
void DeltaExportData(int pnum);
uint8_t *DeltaExportItem(uint8_t *dst, TCmdPItem *src);
uint8_t *DeltaExportObject(uint8_t *dst, DObjectStr *src);
uint8_t *DeltaExportMonster(uint8_t *dst, DMonsterStr *src);
uint8_t *DeltaExportJunk(uint8_t *dst);
int msg_comp_level(uint8_t *buffer, uint8_t *end);
void delta_init();
void delta_kill_monster(int mi, V2Di pos, uint8_t bLevel);
void delta_monster_hp(int mi, int hp, uint8_t bLevel);
void delta_sync_monster(const TSyncMonster *pSync, uint8_t bLevel);
void delta_sync_golem(TCmdGolem *pG, int pnum, uint8_t bLevel);
void delta_leave_sync(uint8_t bLevel);
bool delta_portal_inited(int i);
bool delta_quest_inited(int i);
void DeltaAddItem(int ii);
void DeltaSaveLevel();
void DeltaLoadLevel();
void NetSendCmd(bool bHiPri, uint8_t bCmd);
void NetSendCmdGolem(V2Di pos, uint8_t dir, uint8_t menemy, int hp, uint8_t cl);
void NetSendCmdLoc(bool bHiPri, uint8_t bCmd, V2Di pos);
void NetSendCmdLocParam1(bool bHiPri, uint8_t bCmd, V2Di pos, WORD wParam1);
void NetSendCmdLocParam2(bool bHiPri, uint8_t bCmd, V2Di pos, WORD wParam1, WORD wParam2);
void NetSendCmdLocParam3(bool bHiPri, uint8_t bCmd, V2Di pos, WORD wParam1, WORD wParam2, WORD wParam3);
void NetSendCmdParam1(bool bHiPri, uint8_t bCmd, WORD wParam1);
void NetSendCmdParam2(bool bHiPri, uint8_t bCmd, WORD wParam1, WORD wParam2);
void NetSendCmdParam3(bool bHiPri, uint8_t bCmd, WORD wParam1, WORD wParam2, WORD wParam3);
void NetSendCmdQuest(bool bHiPri, uint8_t q);
void NetSendCmdGItem(bool bHiPri, uint8_t bCmd, uint8_t mast, uint8_t pnum, uint8_t ii);
void NetSendCmdGItem2(bool usonly, uint8_t bCmd, uint8_t mast, uint8_t pnum, TCmdGItem *p);
bool NetSendCmdReq2(uint8_t bCmd, uint8_t mast, uint8_t pnum, TCmdGItem *p);
void NetSendCmdExtra(TCmdGItem *p);
void NetSendCmdPItem(bool bHiPri, uint8_t bCmd, uint8_t x, uint8_t y);
void NetSendCmdChItem(bool bHiPri, uint8_t bLoc);
void NetSendCmdDelItem(bool bHiPri, uint8_t bLoc);
void NetSendCmdDItem(bool bHiPri, int ii);
void NetSendCmdDamage(bool bHiPri, uint8_t bPlr, DWORD dwDam);
void NetSendCmdString(int pmask, const char *pszStr);
void delta_close_portal(int pnum);
DWORD ParseCmd(int pnum, TCmd *pCmd);
DWORD On_DLEVEL(int pnum, TCmd *pCmd);
void DeltaImportData(uint8_t cmd, DWORD recv_offset);
uint8_t *DeltaImportItem(uint8_t *src, TCmdPItem *dst);
uint8_t *DeltaImportObject(uint8_t *src, DObjectStr *dst);
uint8_t *DeltaImportMonster(uint8_t *src, DMonsterStr *dst);
void DeltaImportJunk(uint8_t *src);
DWORD On_SYNCDATA(TCmd *pCmd, int pnum);
DWORD On_WALKXY(TCmd *pCmd, int pnum);
DWORD On_ADDSTR(TCmd *pCmd, int pnum);
DWORD On_ADDMAG(TCmd *pCmd, int pnum);
DWORD On_ADDDEX(TCmd *pCmd, int pnum);
DWORD On_ADDVIT(TCmd *pCmd, int pnum);
DWORD On_SBSPELL(TCmd *pCmd, int pnum);
void msg_errorf(const char *pszFmt, ...);
DWORD On_GOTOGETITEM(TCmd *pCmd, int pnum);
DWORD On_REQUESTGITEM(TCmd *pCmd, int pnum);
bool i_own_level(int nReqLevel);
DWORD On_GETITEM(TCmd *pCmd, int pnum);
bool delta_get_item(TCmdGItem *pI, uint8_t bLevel);
DWORD On_GOTOAGETITEM(TCmd *pCmd, int pnum);
DWORD On_REQUESTAGITEM(TCmd *pCmd, int pnum);
DWORD On_AGETITEM(TCmd *pCmd, int pnum);
DWORD On_ITEMEXTRA(TCmd *pCmd, int pnum);
DWORD On_PUTITEM(TCmd *pCmd, int pnum);
void delta_put_item(TCmdPItem *pI, V2Di pos, uint8_t bLevel);
void check_update_plr(int pnum);
DWORD On_SYNCPUTITEM(TCmd *pCmd, int pnum);
DWORD On_RESPAWNITEM(TCmd *pCmd, int pnum);
DWORD On_ATTACKXY(TCmd *pCmd, int pnum);
DWORD On_SATTACKXY(TCmd *pCmd, int pnum);
DWORD On_RATTACKXY(TCmd *pCmd, int pnum);
DWORD On_SPELLXYD(TCmd *pCmd, int pnum);
DWORD On_SPELLXY(TCmd *pCmd, int pnum);
DWORD On_TSPELLXY(TCmd *pCmd, int pnum);
DWORD On_OPOBJXY(TCmd *pCmd, int pnum);
DWORD On_DISARMXY(TCmd *pCmd, int pnum);
DWORD On_OPOBJT(TCmd *pCmd, int pnum);
DWORD On_ATTACKID(TCmd *pCmd, int pnum);
DWORD On_ATTACKPID(TCmd *pCmd, int pnum);
DWORD On_RATTACKID(TCmd *pCmd, int pnum);
DWORD On_RATTACKPID(TCmd *pCmd, int pnum);
DWORD On_SPELLID(TCmd *pCmd, int pnum);
DWORD On_SPELLPID(TCmd *pCmd, int pnum);
DWORD On_TSPELLID(TCmd *pCmd, int pnum);
DWORD On_TSPELLPID(TCmd *pCmd, int pnum);
DWORD On_KNOCKBACK(TCmd *pCmd, int pnum);
DWORD On_RESURRECT(TCmd *pCmd, int pnum);
DWORD On_HEALOTHER(TCmd *pCmd, int pnum);
DWORD On_TALKXY(TCmd *pCmd, int pnum);
DWORD On_NEWLVL(TCmd *pCmd, int pnum);
DWORD On_WARP(TCmd *pCmd, int pnum);
DWORD On_MONSTDEATH(TCmd *pCmd, int pnum);
DWORD On_KILLGOLEM(TCmd *pCmd, int pnum);
DWORD On_AWAKEGOLEM(TCmd *pCmd, int pnum);
DWORD On_MONSTDAMAGE(TCmd *pCmd, int pnum);
DWORD On_PLRDEAD(TCmd *pCmd, int pnum);
DWORD On_PLRDAMAGE(TCmd *pCmd, int pnum);
DWORD On_OPENDOOR(TCmd *pCmd, int pnum);
void delta_sync_object(int oi, uint8_t bCmd, uint8_t bLevel);
DWORD On_CLOSEDOOR(TCmd *pCmd, int pnum);
DWORD On_OPERATEOBJ(TCmd *pCmd, int pnum);
DWORD On_PLROPOBJ(TCmd *pCmd, int pnum);
DWORD On_BREAKOBJ(TCmd *pCmd, int pnum);
DWORD On_CHANGEPLRITEMS(TCmd *pCmd, int pnum);
DWORD On_DELPLRITEMS(TCmd *pCmd, int pnum);
DWORD On_PLRLEVEL(TCmd *pCmd, int pnum);
DWORD On_DROPITEM(TCmd *pCmd, int pnum);
DWORD On_SEND_PLRINFO(TCmd *pCmd, int pnum);
DWORD On_ACK_PLRINFO(TCmd *pCmd, int pnum);
DWORD On_PLAYER_JOINLEVEL(TCmd *pCmd, int pnum);
DWORD On_ACTIVATEPORTAL(TCmd *pCmd, int pnum);
void delta_open_portal(int pnum, V2Di pos, uint8_t bLevel, DunType bLType, uint8_t bSetLvl);
DWORD On_DEACTIVATEPORTAL(TCmd *pCmd, int pnum);
DWORD On_RETOWN(TCmd *pCmd, int pnum);
DWORD On_SETSTR(TCmd *pCmd, int pnum);
DWORD On_SETDEX(TCmd *pCmd, int pnum);
DWORD On_SETMAG(TCmd *pCmd, int pnum);
DWORD On_SETVIT(TCmd *pCmd, int pnum);
DWORD On_STRING(TCmd *pCmd, int pnum);
DWORD On_STRING2(int pnum, TCmd *pCmd);
DWORD On_SYNCQUEST(TCmd *pCmd, int pnum);
DWORD On_ENDSHIELD(TCmd *pCmd, int pnum);
DWORD On_CHEAT_EXPERIENCE(TCmd *pCmd, int pnum);
DWORD On_CHEAT_SPELL_LEVEL(TCmd *pCmd, int pnum);
DWORD On_DEBUG(TCmd *pCmd, int pnum);
DWORD On_NOVA(TCmd *pCmd, int pnum);
DWORD On_SETSHIELD(TCmd *pCmd, int pnum);
DWORD On_REMSHIELD(TCmd *pCmd, int pnum);

//#ifdef __cplusplus
//}
//#endif

DEVILUTION_END_NAMESPACE

#endif /* __MSG_H__ */
