/**
 * @file encrypt.h
 *
 * Interface of functions for compression and decompressing MPQ data.
 */
#ifndef __ENCRYPT_H__
#define __ENCRYPT_H__

namespace dvl {

//#ifdef __cplusplus
//extern "C" {
//#endif

extern DWORD hashtable[5][256];

void Decrypt(DWORD *castBlock, DWORD size, DWORD key);
void Encrypt(DWORD *castBlock, DWORD size, DWORD key);
DWORD Hash(const char *s, int type);
void InitHash();
int PkwareCompress(uint8_t *srcData, int size);
unsigned int PkwareBufferRead(char *buf, unsigned int *size, void *param);
void PkwareBufferWrite(char *buf, unsigned int *size, void *param);
void PkwareDecompress(uint8_t *pbInBuff, int recv_size, int dwMaxBytes);

//#ifdef __cplusplus
//}
//#endif

}

#endif /* __ENCRYPT_H__ */
