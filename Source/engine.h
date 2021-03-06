/**
 * @file engine.h
 *
 *  of basic engine helper functions:
 * - Sprite blitting
 * - Drawing
 * - Angle calculation
 * - RNG
 * - Memory allocation
 * - File loading
 * - Video playback
 */
#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <random>
#include <string>

namespace dvl {

/** automap pixel color 8-bit (palette entry) */
extern char gbPixelCol;
/** flip - if y < x */
extern bool gbRotateMap;
/** Seed value before the most recent call to SetRndSeed() */
extern int orgseed;
/** Track number of calls to GetRndSeed() since last call to SetRndSeed() */
extern int SeedCount;
/** valid - if x/y are in bounds */
extern bool gbNotInView;

inline uint8_t *CelGetFrameStart(uint8_t *pCelBuff, int nCel)
{
	DWORD *pFrameTable;

	pFrameTable = (DWORD *)pCelBuff;

	return pCelBuff + SwapLE32(pFrameTable[nCel]);
}

#define LOAD_LE32(b) (((DWORD)(b)[3] << 24) | ((DWORD)(b)[2] << 16) | ((DWORD)(b)[1] << 8) | (DWORD)(b)[0])
inline uint8_t *CelGetFrame(uint8_t *pCelBuff, int nCel, int *nDataSize)
{
	DWORD nCellStart;

	nCellStart = LOAD_LE32(&pCelBuff[nCel * 4]);
	*nDataSize = LOAD_LE32(&pCelBuff[(nCel + 1) * 4]) - nCellStart;
	return pCelBuff + nCellStart;
}

inline uint8_t *CelGetFrameClipped(uint8_t *pCelBuff, int nCel, int *nDataSize)
{
	DWORD nDataStart;
	uint8_t *pRLEBytes = CelGetFrame(pCelBuff, nCel, nDataSize);

	nDataStart = pRLEBytes[1] << 8 | pRLEBytes[0];
	*nDataSize -= nDataStart;

	return pRLEBytes + nDataStart;
}

void CelDraw(int sx, int sy, uint8_t *pCelBuff, int nCel, int nWidth);
void CelDraw(V2Di s, uint8_t *pCelBuff, int nCel, int nWidth);
void CelBlitFrame(uint8_t *pBuff, uint8_t *pCelBuff, int nCel, int nWidth);
void CelClippedDraw(V2Di s, uint8_t *pCelBuff, int nCel, int nWidth);
void CelDrawLight(V2Di s, uint8_t *pCelBuff, int nCel, int nWidth, uint8_t *tbl);
void CelClippedDrawLight(V2Di s, uint8_t *pCelBuff, int nCel, int nWidth);
void CelClippedBlitLightTrans(uint8_t *pBuff, uint8_t *pCelBuff, int nCel, int nWidth, int trans);
void CelDrawLightRed(V2Di s, uint8_t *pCelBuff, int nCel, int nWidth, char light);
void CelBlitSafe(uint8_t *pDecodeTo, uint8_t *pRLEBytes, int nDataSize, int nWidth);
void CelClippedDrawSafe(V2Di s, uint8_t *pCelBuff, int nCel, int nWidth);
void CelBlitLightSafe(uint8_t *pDecodeTo, uint8_t *pRLEBytes, int nDataSize, int nWidth, uint8_t *tbl);
void CelBlitLightTransSafe(uint8_t *pDecodeTo, uint8_t *pRLEBytes, int nDataSize, int nWidth);
void CelDrawLightRedSafe(V2Di s, uint8_t *pCelBuff, int nCel, int nWidth, char light);
void CelBlitWidth(uint8_t *pBuff, V2Di s, int wdt, uint8_t *pCelBuff, int nCel, int nWidth);
void CelBlitOutline(char col, V2Di s, uint8_t *pCelBuff, int nCel, int nWidth);
void ENG_set_pixel(int sx, int sy, uint8_t col);
void ENG_set_pixel(V2Di s, uint8_t col);
void engine_draw_pixel(V2Di s);
void DrawLine(int x0, int y0, int x1, int y1, uint8_t col);
void DrawLine(V2Di p0, V2Di p1, uint8_t col);
Dir GetDirection(V2Di t1, V2Di t2);
void SetRndSeed(int s);
int GetRndSeed();
std::mt19937& randMt();
int random_(uint8_t idx, int v);
uint64_t rand64();
void engine_debug_trap(bool show_cursor);
uint8_t *DiabloAllocPtr(DWORD dwBytes);
void mem_free_dbg(void *p);
uint8_t *LoadFileInMem(const std::string pszName, DWORD *pdwFileLen);
DWORD LoadFileWithMem(const std::string pszName, uint8_t *p);
void Cl2ApplyTrans(uint8_t *p, uint8_t *ttbl, int nCel);
void Cl2Draw(V2Di s, uint8_t *pCelBuff, int nCel, int nWidth);
void Cl2DrawOutline(char col, V2Di s, uint8_t *pCelBuff, int nCel, int nWidth);
void Cl2DrawLightTbl(V2Di s, uint8_t *pCelBuff, int nCel, int nWidth, char light);
void Cl2DrawLight(V2Di s, uint8_t *pCelBuff, int nCel, int nWidth);
void Cl2BlitSafe(uint8_t *pDecodeTo, uint8_t *pRLEBytes, int nDataSize, int nWidth);
void Cl2BlitOutlineSafe(uint8_t *pDecodeTo, uint8_t *pRLEBytes, int nDataSize, int nWidth, char col);
void Cl2BlitLightSafe(uint8_t *pDecodeTo, uint8_t *pRLEBytes, int nDataSize, int nWidth, uint8_t *pTable);
void PlayInGameMovie(char *pszMovie);

/* rdata */

extern const int RndInc;
extern const int RndMult;

//#ifdef __cplusplus
//}
//#endif

}

#endif /* __ENGINE_H__ */
