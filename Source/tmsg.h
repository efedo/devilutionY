//HEADER_GOES_HERE
#ifndef __TMSG_H__
#define __TMSG_H__

namespace dvl {

int tmsg_get(uint8_t *pbMsg, DWORD dwMaxLen);
void tmsg_add(uint8_t *pbMsg, uint8_t bLen);
void tmsg_start();
void tmsg_cleanup();

}

#endif /* __TMSG_H__ */
