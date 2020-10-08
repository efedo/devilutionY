//HEADER_GOES_HERE
#ifndef __PLRMSG_H__
#define __PLRMSG_H__

namespace dvl {

//#ifdef __cplusplus
//extern "C" {
//#endif

extern _plrmsg plr_msgs[PMSG_COUNT];

void plrmsg_delay(bool delay);
char *ErrorPlrMsg(const char *pszMsg);
size_t EventPlrMsg(const char *pszFmt, ...);
void SendPlrMsg(int pnum, const char *pszStr);
void ClearPlrMsg();
void InitPlrMsg();
void DrawPlrMsg();
void PrintPlrMsg(DWORD x, DWORD y, DWORD width, const char *str, uint8_t col);

/* rdata */

extern const char text_color_from_player_num[MAX_PLRS + 1];

//#ifdef __cplusplus
//}
//#endif

}

#endif /* __PLRMSG_H__ */
