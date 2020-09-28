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
void SetPortalStats(int i, bool o, V2Di p, int lvl, DunType lvltype);
void AddWarpMissile(int i, V2Di p);
void SyncPortals();
void AddInTownPortal(int i);
void ActivatePortal(int i, V2Di p, int lvl, DunType lvltype, bool sp);
void DeactivatePortal(int i);
bool PortalOnLevel(int i);
void RemovePortalMissile(int id);
void SetCurrentPortal(int p);
void GetPortalLevel();
void GetPortalLvlPos();
bool PosOkPortal(int lvl, V2Di p);

/* rdata */
extern int WarpDropX[MAXPORTAL];
extern int WarpDropY[MAXPORTAL];

//#ifdef __cplusplus
//}
//#endif

DEVILUTION_END_NAMESPACE

#endif /* __PORTAL_H__ */
