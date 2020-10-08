//HEADER_GOES_HERE
#ifndef __SETMAPS_H__
#define __SETMAPS_H__

namespace dvl {

//#ifdef __cplusplus
//extern "C" {
//#endif

int ObjIndex(int x, int y);
void AddSKingObjs();
void AddSChamObjs();
void AddVileObjs();
void DRLG_SetMapTrans(char *sFileName);
void LoadSetMap();

/* rdata */
extern uint8_t SkelKingTrans1[8];
extern uint8_t SkelKingTrans2[8];
extern uint8_t SkelKingTrans3[20];
extern uint8_t SkelKingTrans4[28];
extern uint8_t SkelChamTrans1[20];
extern uint8_t SkelChamTrans2[8];
extern uint8_t SkelChamTrans3[36];
extern char *quest_level_names[];

//#ifdef __cplusplus
//}
//#endif

}

#endif /* __SETMAPS_H__ */
