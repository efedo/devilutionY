/**
 * @file mpqapi.h
 *
 * Interface of functions for creating and editing MPQ files.
 */
#ifndef __MPQAPI_H__
#define __MPQAPI_H__

#include <stdint.h>

DEVILUTION_BEGIN_NAMESPACE

//#ifdef __cplusplus
//extern "C" {
//#endif

void mpqapi_remove_hash_entry(const char *pszName);
void mpqapi_alloc_block(uint32_t block_offset, uint32_t block_size);
_BLOCKENTRY *mpqapi_new_block(int *block_index);
int FetchHandle(const char *pszName);
int mpqapi_get_hash_index(short index, int hash_a, int hash_b, int locale);
void mpqapi_remove_hash_entries(bool (*fnGetName)(DWORD, char *));
bool mpqapi_write_file(const char *pszName, const uint8_t *pbData, DWORD dwLen);
_BLOCKENTRY *mpqapi_add_file(const char *pszName, _BLOCKENTRY *pBlk, int block_index);
bool mpqapi_write_file_contents(const char *pszName, const uint8_t *pbData, DWORD dwLen, _BLOCKENTRY *pBlk);
int mpqapi_find_free_block(uint32_t size, uint32_t *block_size);
void mpqapi_rename(char *pszOld, char *pszNew);
bool mpqapi_has_file(const char *pszName);
bool OpenMPQ(const char *pszArchive, DWORD dwChar);
bool mpqapi_flush_and_close(const char *pszArchive, bool bFree, DWORD dwChar);
/* rdata */

/* data */
//#ifdef __cplusplus
//}
//#endif

DEVILUTION_END_NAMESPACE

#endif /* __MPQAPI_H__ */
