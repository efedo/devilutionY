//HEADER_GOES_HERE
#ifndef __OBJECTS_H__
#define __OBJECTS_H__

namespace dvl {

//#ifdef __cplusplus
//extern "C" {
//#endif

extern int trapid;
extern int trapdir;
extern uint8_t *pObjCels[40];
extern char ObjFileList[40];
extern int objectactive[MAXOBJECTS];
extern int nobjects;
extern int leverid;
extern int objectavail[MAXOBJECTS];
extern ObjectStruct object[MAXOBJECTS];
extern bool InitObjFlag;
extern int numobjfiles;

void InitObjectGFX();
void FreeObjectGFX();
bool RndLocOk(V2Di p);
bool RndLocOk(V2Di p);
void InitRndLocObj(int min, int max, int objtype);
void InitRndLocBigObj(int min, int max, int objtype);
void InitRndLocObj5x5(int min, int max, int objtype);
void ClrAllObjects();
void AddTortures();
void AddCandles();
void AddBookLever(V2Di l1, V2Di l2, V2Di p1, V2Di p2, int msg);
void InitRndBarrels();
void AddL1Objs(V2Di p1, V2Di p2);
void AddL2Objs(V2Di p1, V2Di p2);
void AddL3Objs(V2Di p1, V2Di p2);
bool WallTrapLocOk(V2Di p);
void AddL2Torches();
bool TorchLocOK(V2Di p);
void AddObjTraps();
void AddChestTraps();
void LoadMapObjects(uint8_t *pMap, V2Di start, V2Di p1, int w, int h, int leveridx);
void LoadMapObjs(uint8_t *pMap, V2Di start);
void AddDiabObjs();
void AddStoryBooks();
void AddHookedBodies(int freq);
void AddL4Goodies();
void AddLazStand();
void InitObjects();
void SetMapObjects(uint8_t *pMap, V2Di start);
void DeleteObject_(int oi, int i);
void SetupObject(int i, V2Di pos, int ot);
void SetObjMapRange(int i, V2Di p1, V2Di p2, int v);
void SetBookMsg(int i, int msg);
void AddL1Door(int i, V2Di pos, int ot);
void AddSCambBook(int i);
void AddChest(int i, int t);
void AddL2Door(int i, V2Di p, int ot);
void AddL3Door(int i, V2Di p, int ot);
void AddSarc(int i);
void AddFlameTrap(int i);
void AddFlameLvr(int i);
void AddTrap(int i, int ot);
void AddObjLight(int i, int r);
void AddBarrel(int i, int t);
void AddShrine(int i);
void AddBookcase(int i);
void AddPurifyingFountain(int i);
void AddArmorStand(int i);
void AddGoatShrine(int i);
void AddCauldron(int i);
void AddMurkyFountain(int i);
void AddTearFountain(int i);
void AddDecap(int i);
void AddVilebook(int i);
void AddMagicCircle(int i);
void AddBrnCross(int i);
void AddBookstand(int i);
void AddBloodFtn(int i);
void AddPedistal(int i);
void AddStoryBook(int i);
void AddWeaponRack(int i);
void AddTorturedBody(int i);
void GetRndObjLoc(int randarea, int *xx, int *yy);
void AddMushPatch();
void AddSlainHero();
void AddObject(int ot, int ox, int oy);
void AddObject(int ot, V2Di o);
void Obj_Light(int i, int lr);
void Obj_Circle(int i);
void Obj_StopAnim(int i);
void Obj_Door(int i);
void Obj_Sarc(int i);
void ActivateTrapLine(int ttype, int tid);
void Obj_FlameTrap(int i);
void Obj_Trap(int i);
void Obj_BCrossDamage(int i);
void ProcessObjects();
void ObjSetMicro(V2Di d, int pn);
void objects_set_door_piece(V2Di p);
void ObjSetMini(V2Di p, int v);
void ObjL1Special(V2Di p1, V2Di p2);
void ObjL2Special(V2Di p1, V2Di p2);
void DoorSet(int oi, V2Di d);
void RedoPlayerVision();
void OperateL1RDoor(int pnum, int oi, bool sendflag);
void OperateL1LDoor(int pnum, int oi, bool sendflag);
void OperateL2RDoor(int pnum, int oi, bool sendflag);
void OperateL2LDoor(int pnum, int oi, bool sendflag);
void OperateL3RDoor(int pnum, int oi, bool sendflag);
void OperateL3LDoor(int pnum, int oi, bool sendflag);
void MonstCheckDoors(int n);
void ObjChangeMap(V2Di p1, V2Di p2);
void ObjChangeMapResync(V2Di p1, V2Di p2);
void OperateL1Door(int pnum, int i, bool sendflag);
void OperateLever(int pnum, int i);
void OperateBook(int pnum, int i);
void OperateBookLever(int pnum, int i);
void OperateSChambBk(int pnum, int i);
void OperateChest(int pnum, int i, bool sendmsg);
void OperateMushPatch(int pnum, int i);
void OperateInnSignChest(int pnum, int i);
void OperateSlainHero(int pnum, int i, bool sendmsg);
void OperateTrapLvr(int i);
void OperateSarc(int pnum, int i, bool sendmsg);
void OperateL2Door(int pnum, int i, bool sendflag);
void OperateL3Door(int pnum, int i, bool sendflag);
void OperatePedistal(int pnum, int i);
void TryDisarm(int pnum, int i);
int ItemMiscIdIdx(int imiscid);
void OperateShrine(int pnum, int i, int sType);
void OperateSkelBook(int pnum, int i, bool sendmsg);
void OperateBookCase(int pnum, int i, bool sendmsg);
void OperateDecap(int pnum, int i, bool sendmsg);
void OperateArmorStand(int pnum, int i, bool sendmsg);
int FindValidShrine(int i);
void OperateGoatShrine(int pnum, int i, int sType);
void OperateCauldron(int pnum, int i, int sType);
bool OperateFountains(int pnum, int i);
void OperateWeaponRack(int pnum, int i, bool sendmsg);
void OperateStoryBook(int pnum, int i);
void OperateLazStand(int pnum, int i);
void OperateObject(int pnum, int i, bool TeleFlag);
void SyncOpL1Door(int pnum, int cmd, int i);
void SyncOpL2Door(int pnum, int cmd, int i);
void SyncOpL3Door(int pnum, int cmd, int i);
void SyncOpObject(int pnum, int cmd, int i);
void BreakCrux(int i);
void BreakBarrel(int pnum, int i, int dam, bool forcebreak, bool sendmsg);
void BreakObject(int pnum, int oi);
void SyncBreakObj(int pnum, int oi);
void SyncL1Doors(int i);
void SyncCrux(int i);
void SyncLever(int i);
void SyncQSTLever(int i);
void SyncPedistal(int i);
void SyncL2Doors(int i);
void SyncL3Doors(int i);
void SyncObjectAnim(int o);
void GetObjectStr(int i);

/* rdata */

extern int bxadd[8];
extern int byadd[8];
extern char *shrinestrs[NUM_SHRINETYPE];
extern char shrinemin[NUM_SHRINETYPE];
extern char shrinemax[NUM_SHRINETYPE];
extern uint8_t shrineavail[NUM_SHRINETYPE];
extern char *StoryBookName[9];
extern int StoryText[3][3];

//#ifdef __cplusplus
//}
//#endif

}

#endif /* __OBJECTS_H__ */
