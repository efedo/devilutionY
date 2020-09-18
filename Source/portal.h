//HEADER_GOES_HERE
#ifndef __PORTAL_H__
#define __PORTAL_H__

DEVILUTION_BEGIN_NAMESPACE

//#ifdef __cplusplus
//extern "C" {
//#endif

extern PortalStruct portal[MAXPORTAL];
extern int portalindex;

void InitPortals();
void SetPortalStats(int i, BOOL o, V2Di p, int lvl, int lvltype);
void AddWarpMissile(int i, V2Di p);
void SyncPortals();
void AddInTownPortal(int i);
void ActivatePortal(int i, V2Di p, int lvl, int lvltype, BOOL sp);
void DeactivatePortal(int i);
BOOL PortalOnLevel(int i);
void RemovePortalMissile(int id);
void SetCurrentPortal(int p);
void GetPortalLevel();
void GetPortalLvlPos();
BOOL PosOkPortal(int lvl, V2Di p);

/* rdata */
extern int WarpDropX[MAXPORTAL];
extern int WarpDropY[MAXPORTAL];

//#ifdef __cplusplus
//}
//#endif

DEVILUTION_END_NAMESPACE

#endif /* __PORTAL_H__ */
