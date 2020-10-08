/**
 * @file lighting.h
 *
 * Interface of light and vision.
 */
#ifndef __LIGHTING_H__
#define __LIGHTING_H__

namespace dvl {

extern LightListStruct VisionList[MAXVISION];
extern uint8_t lightactive[MAXLIGHTS];
extern LightListStruct LightList[MAXLIGHTS];
extern int numlights;
extern uint8_t lightradius[16][128];
extern bool dovision;
extern int numvision;
extern char lightmax;
extern bool dolighting;
extern uint8_t lightblock[64][16][16];
extern int visionid;
extern uint8_t *pLightTbl;
extern bool lightflag;

void RotateRadius(V2Di &pos, V2Di &d, V2Di &l, V2Di &b);
void DoLighting(V2Di pos, int nRadius, int Lnum);
void DoUnLight(V2Di pos, int nRadius);
void DoUnVision(V2Di pos, int nRadius);
void DoVision(V2Di pos, int nRadius, bool doautomap, bool visible);
void FreeLightTable();
void InitLightTable();
void MakeLightTable();
#ifdef _DEBUG
void ToggleLighting();
#endif
void InitLightMax();
void InitLighting();
int AddLight(V2Di pos, int r);
void AddUnLight(int i);
void ChangeLightRadius(int i, int r);
void ChangeLightXY(int i, V2Di pos);
void ChangeLightOff(int i, V2Di pos);
void ChangeLight(int i, V2Di pos, int r);
void ProcessLightList();
void SavePreLighting();
void InitVision();
int AddVision(V2Di pos, int r, bool mine);
void ChangeVisionRadius(int id, int r);
void ChangeVisionXY(int id, V2Di pos);
void ProcessVisionList();
void lighting_color_cycling();

/* rdata */

extern char CrawlTable[2749];
extern char *pCrawlTable[19];
extern uint8_t vCrawlTable[23][30];
extern uint8_t byte_49463C[18][18];
extern uint8_t RadiusAdj[23];

}

#endif /* __LIGHTING_H__ */
