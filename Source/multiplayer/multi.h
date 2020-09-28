/**
 * @file multi.h
 *
 * Interface of functions for keeping multiplaye games in sync.
 */
#ifndef __MULTI_H__
#define __MULTI_H__

DEVILUTION_BEGIN_NAMESPACE

//#ifdef __cplusplus
//extern "C" {
//#endif

extern bool gbSomebodyWonGameKludge;
extern char szPlayerDescript[128];
extern WORD sgwPackPlrOffsetTbl[MAX_PLRS];
extern PkPlayerStruct netplr[MAX_PLRS];
extern bool gbShouldValidatePackage;
extern uint8_t gbActivePlayers;
extern bool gbGameDestroyed;
extern bool gbSelectProvider;
//extern uint8_t gbMaxPlayers;
extern char szPlayerName[128];
extern uint8_t gbDeltaSender;
extern int player_state[MAX_PLRS];

void multi_msg_add(uint8_t *pbMsg, uint8_t bLen);
void NetSendLoPri(uint8_t *pbMsg, uint8_t bLen);
void multi_copy_packet(TBuffer *pBuf, void *packet, uint8_t size);
void multi_send_packet(void *packet, uint8_t dwSize);
void NetRecvPlrData(TPkt *pkt);
void NetSendHiPri(uint8_t *pbMsg, uint8_t bLen);
uint8_t *multi_recv_packet(TBuffer *pBuf, uint8_t *body, int *size);
void multi_send_msg_packet(int pmask, uint8_t *src, uint8_t len);
void multi_msg_countdown();
void multi_parse_turn(int pnum, int turn);
void multi_handle_turn_upper_bit(int pnum);
void multi_player_left(int pnum, int reason);
void multi_clear_left_tbl();
void multi_player_left_msg(int pnum, int left);
void multi_net_ping();
int multi_handle_delta();
int multi_check_pkt_valid(TBuffer *pBuf);
void multi_mon_seeds();
void multi_begin_timeout();
void multi_check_drop_player();
void multi_process_network_packets();
void multi_handle_all_packets(int pnum, uint8_t *pData, int nSize);
void multi_process_tmsgs();
void multi_send_zero_packet(int pnum, uint8_t bCmd, uint8_t *pbSrc, DWORD dwLen);
void NetClose();
void multi_event_handler(bool add);
void multi_handle_events(_SNETEVENT *pEvt);
bool NetInit(bool bSinglePlayer, bool *pfExitProgram);
void buffer_init(TBuffer *pBuf);
void multi_send_pinfo(int pnum, char cmd);
int InitLevelType(int l);
void SetupLocalCoords();
bool multi_init_single(_SNETPROGRAMDATA *client_info, _SNETPLAYERDATA *user_info, _SNETUIDATA *ui_info);
bool multi_init_multi(_SNETPROGRAMDATA *client_info, _SNETPLAYERDATA *user_info, _SNETUIDATA *ui_info, bool *pfExitProgram);
bool multi_upgrade(bool *pfExitProgram);
void recv_plrinfo(int pnum, TCmdPlrInfoHdr *p, bool recv);

/* rdata */

extern const int event_types[3];

//#ifdef __cplusplus
//}
//#endif

DEVILUTION_END_NAMESPACE

#endif /* __MULTI_H__ */
