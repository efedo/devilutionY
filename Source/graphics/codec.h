/**
 * @file codec.h
 *
 * Interface of save game encryption algorithm.
 */
#ifndef __CODEC_H__
#define __CODEC_H__

namespace dvl {

//#ifdef __cplusplus
//extern "C" {
//#endif

int codec_decode(uint8_t *pbSrcDst, DWORD size, char *pszPassword);
void codec_init_key(int unused, char *pszPassword);
DWORD codec_get_encoded_len(DWORD dwSrcBytes);
void codec_encode(uint8_t *pbSrcDst, DWORD size, int size_64, char *pszPassword);

//#ifdef __cplusplus
//}
//#endif

}

#endif /* __CODEC_H__ */
