//HEADER_GOES_HERE
#ifndef __TMSG_H__
#define __TMSG_H__

DEVILUTION_BEGIN_NAMESPACE

int tmsg_get(uint8_t *pbMsg, DWORD dwMaxLen);
void tmsg_add(uint8_t *pbMsg, uint8_t bLen);
void tmsg_start();
void tmsg_cleanup();

DEVILUTION_END_NAMESPACE

#endif /* __TMSG_H__ */
