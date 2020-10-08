//HEADER_GOES_HERE
#ifndef __PACK_H__
#define __PACK_H__

namespace dvl {

//#ifdef __cplusplus
//extern "C" {
//#endif

void PackPlayer(PkPlayerStruct *pPack, int pnum, bool manashield);
void VerifyGoldSeeds(PlayerStruct *pPlayer);
void UnPackPlayer(PkPlayerStruct *pPack, int pnum, bool killok);

/* rdata */
//#ifdef __cplusplus
//}
//#endif

}

#endif /* __PACK_H__ */
