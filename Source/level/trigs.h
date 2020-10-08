//HEADER_GOES_HERE
#ifndef __TRIGS_H__
#define __TRIGS_H__

namespace dvl {

//#ifdef __cplusplus
//extern "C" {
//#endif

extern bool townwarps[3];
extern bool trigflag;
extern int numtrigs;
extern TriggerStruct trigs[MAXTRIGGERS];
extern int TWarpFrom;

void InitNoTriggers();
void InitTownTriggers();
void InitL1Triggers();
void InitL2Triggers();
void InitL3Triggers();
void InitL4Triggers();
void InitSKingTriggers();
void InitSChambTriggers();
void InitPWaterTriggers();
void InitVPTriggers();
bool ForceTownTrig();
bool ForceL1Trig();
bool ForceL2Trig();
bool ForceL3Trig();
bool ForceL4Trig();
void Freeupstairs();
bool ForceSKingTrig();
bool ForceSChambTrig();
bool ForcePWaterTrig();
void CheckTrigForce();
void CheckTriggers();

/* rdata */

extern int TownDownList[11];
extern int TownWarp1List[13];
extern int L1UpList[12];
extern int L1DownList[10];
extern int L2UpList[3];
extern int L2DownList[5];
extern int L2TWarpUpList[3];
extern int L3UpList[15];
extern int L3DownList[9];
extern int L3TWarpUpList[14];
extern int L4UpList[4];
extern int L4DownList[6];
extern int L4TWarpUpList[4];
extern int L4PentaList[33];

//#ifdef __cplusplus
//}
//#endif

}

#endif /* __TRIGS_H__ */
