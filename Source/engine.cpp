/**
 * @file engine.cpp
 *
 * Implementation of basic engine helper functions:
 * - Sprite blitting
 * - Drawing
 * - Angle calculation
 * - RNG
 * - Memory allocation
 * - File loading
 * - Video playback
 */
#include "all.h"
#include "../3rdParty/Storm/Source/storm.h"
#include <random>

DEVILUTION_BEGIN_NAMESPACE

char gbPixelCol;  // automap pixel color 8-bit (palette entry)
bool gbRotateMap; // flip - if y < x
int orgseed;
/** Width of sprite being blitted */
int sgnWidth;
/** Current game seed */
int sglGameSeed;
static CCritSect sgMemCrit;
int SeedCount;
bool gbNotInView; // valid - if x/y are in bounds

/**
 * Specifies the increment used in the Borland C/C++ pseudo-random.
 */
const int RndInc = 1;

/**
 * Specifies the multiplier used in the Borland C/C++ pseudo-random number generator algorithm.
 */
const int RndMult = 0x015A4E35;

void CelDraw(int sx, int sy, uint8_t *pCelBuff, int nCel, int nWidth)
{
	CelDraw({ sx, sy }, pCelBuff, nCel, nWidth);
}

/**
 * @brief Blit CEL sprite to the back buffer at the given coordinates
 * @param sx Back buffer coordinate
 * @param sy Back buffer coordinate
 * @param pCelBuff Cel data
 * @param nCel CEL frame number
 * @param nWidth Width of sprite
 */
void CelDraw(V2Di s, uint8_t *pCelBuff, int nCel, int nWidth)
{
	CelBlitFrame(&gpBuffer[s.x + BUFFER_WIDTH * s.y], pCelBuff, nCel, nWidth);
}

/**
 * @brief Blit a given CEL frame to the given buffer
 * @param pBuff Target buffer
 * @param pCelBuff Cel data
 * @param nCel CEL frame number
 * @param nWidth Width of sprite
 */
void CelBlitFrame(uint8_t *pBuff, uint8_t *pCelBuff, int nCel, int nWidth)
{
	int nDataSize;
	uint8_t *pRLEBytes;

	assert(pCelBuff != NULL);
	assert(pBuff != NULL);

	pRLEBytes = CelGetFrame(pCelBuff, nCel, &nDataSize);
	CelBlitSafe(pBuff, pRLEBytes, nDataSize, nWidth);
}

/**
 * @brief Same as CelDraw but with the option to skip parts of the top and bottom of the sprite
 * @param sx Back buffer coordinate
 * @param sy Back buffer coordinate
 * @param pCelBuff Cel data
 * @param nCel CEL frame number
 * @param nWidth Width of sprite
 */
void CelClippedDraw(V2Di s, uint8_t *pCelBuff, int nCel, int nWidth)
{
	uint8_t *pRLEBytes;
	int nDataSize;

	assert(gpBuffer);
	assert(pCelBuff != NULL);

	pRLEBytes = CelGetFrameClipped(pCelBuff, nCel, &nDataSize);

	CelBlitSafe(
	    &gpBuffer[s.x + BUFFER_WIDTH * s.y],
	    pRLEBytes,
	    nDataSize,
	    nWidth);
}

/**
 * @brief Blit CEL sprite, and apply lighting, to the back buffer at the given coordinates
 * @param sx Back buffer coordinate
 * @param sy Back buffer coordinate
 * @param pCelBuff Cel data
 * @param nCel CEL frame number
 * @param nWidth Width of sprite
 */
void CelDrawLight(V2Di s, uint8_t *pCelBuff, int nCel, int nWidth, uint8_t *tbl)
{
	int nDataSize;
	uint8_t *pDecodeTo, *pRLEBytes;

	assert(gpBuffer);
	assert(pCelBuff != NULL);

	pRLEBytes = CelGetFrame(pCelBuff, nCel, &nDataSize);
	pDecodeTo = &gpBuffer[s.x + BUFFER_WIDTH * s.y];

	if (light_table_index || tbl)
		CelBlitLightSafe(pDecodeTo, pRLEBytes, nDataSize, nWidth, tbl);
	else
		CelBlitSafe(pDecodeTo, pRLEBytes, nDataSize, nWidth);
}

/**
 * @brief Same as CelDrawLight but with the option to skip parts of the top and bottom of the sprite
 * @param sx Back buffer coordinate
 * @param sy Back buffer coordinate
 * @param pCelBuff Cel data
 * @param nCel CEL frame number
 * @param nWidth Width of sprite
 */
void CelClippedDrawLight(V2Di s, uint8_t *pCelBuff, int nCel, int nWidth)
{
	int nDataSize;
	uint8_t *pRLEBytes, *pDecodeTo;

	assert(gpBuffer);
	assert(pCelBuff != NULL);

	pRLEBytes = CelGetFrameClipped(pCelBuff, nCel, &nDataSize);
	pDecodeTo = &gpBuffer[s.x + BUFFER_WIDTH * s.y];

	if (light_table_index)
		CelBlitLightSafe(pDecodeTo, pRLEBytes, nDataSize, nWidth, NULL);
	else
		CelBlitSafe(pDecodeTo, pRLEBytes, nDataSize, nWidth);
}

/**
 * @brief Blit CEL sprite, and apply lighting, to the back buffer at the given coordinates, translated to a red hue
 * @param sx Back buffer coordinate
 * @param sy Back buffer coordinate
 * @param pCelBuff Cel data
 * @param nCel CEL frame number
 * @param nWidth Width of sprite
 * @param light Light shade to use
 */
void CelDrawLightRed(V2Di s, uint8_t *pCelBuff, int nCel, int nWidth, char light)
{
	int nDataSize, w, idx;
	uint8_t *pRLEBytes, *dst, *tbl;

	assert(gpBuffer);
	assert(pCelBuff != NULL);

	pRLEBytes = CelGetFrameClipped(pCelBuff, nCel, &nDataSize);
	dst = &gpBuffer[s.x + BUFFER_WIDTH * s.y];

	idx = light4flag ? 1024 : 4096;
	if (light == 2)
		idx += 256; // gray colors
	if (light >= 4)
		idx += (light - 1) << 8;

	uint8_t width;
	uint8_t *end;

	tbl = &pLightTbl[idx];
	end = &pRLEBytes[nDataSize];

	for (; pRLEBytes != end; dst -= BUFFER_WIDTH + nWidth) {
		for (w = nWidth; w;) {
			width = *pRLEBytes++;
			if (!(width & 0x80)) {
				w -= width;
				while (width) {
					*dst = tbl[*pRLEBytes];
					pRLEBytes++;
					dst++;
					width--;
				}
			} else {
				width = -(char)width;
				dst += width;
				w -= width;
			}
		}
	}
}

/**
 * @brief Blit CEL sprite to the given buffer, checks for drawing outside the buffer
 * @param pDecodeTo The output buffer
 * @param pRLEBytes CEL pixel stream (run-length encoded)
 * @param nDataSize Size of CEL in bytes
 * @param nWidth Width of sprite
 */
void CelBlitSafe(uint8_t *pDecodeTo, uint8_t *pRLEBytes, int nDataSize, int nWidth)
{
	int i, w;
	uint8_t width;
	uint8_t *src, *dst;

	assert(pDecodeTo != NULL);
	assert(pRLEBytes != NULL);
	assert(gpBuffer);

	src = pRLEBytes;
	dst = pDecodeTo;
	w = nWidth;

	for (; src != &pRLEBytes[nDataSize]; dst -= BUFFER_WIDTH + w) {
		for (i = w; i;) {
			width = *src++;
			if (!(width & 0x80)) {
				i -= width;
				if (dst < gpBufEnd && dst > gpBufStart) {
					memcpy(dst, src, width);
				}
				src += width;
				dst += width;
			} else {
				width = -(char)width;
				dst += width;
				i -= width;
			}
		}
	}
}

/**
 * @brief Same as CelClippedDraw but checks for drawing outside the buffer
 * @param sx Back buffer coordinate
 * @param sy Back buffer coordinate
 * @param pCelBuff Cel data
 * @param nCel CEL frame number
 * @param nWidth Width of sprite
 */
void CelClippedDrawSafe(V2Di s, uint8_t *pCelBuff, int nCel, int nWidth)
{
	uint8_t *pRLEBytes;
	int nDataSize;

	assert(gpBuffer);
	assert(pCelBuff != NULL);

	pRLEBytes = CelGetFrameClipped(pCelBuff, nCel, &nDataSize);

	CelBlitSafe(
	    &gpBuffer[s.x + BUFFER_WIDTH * s.y],
	    pRLEBytes,
	    nDataSize,
	    nWidth);
}

/**
 * @brief Blit CEL sprite, and apply lighting, to the given buffer, checks for drawing outside the buffer
 * @param pDecodeTo The output buffer
 * @param pRLEBytes CEL pixel stream (run-length encoded)
 * @param nDataSize Size of CEL in bytes
 * @param nWidth Width of sprite
 * @param tbl Palette translation table
 */
void CelBlitLightSafe(uint8_t *pDecodeTo, uint8_t *pRLEBytes, int nDataSize, int nWidth, uint8_t *tbl)
{
	int i, w;
	uint8_t width;
	uint8_t *src, *dst;

	assert(pDecodeTo != NULL);
	assert(pRLEBytes != NULL);
	assert(gpBuffer);

	src = pRLEBytes;
	dst = pDecodeTo;
	if (tbl == NULL)
		tbl = &pLightTbl[light_table_index * 256];
	w = nWidth;

	for (; src != &pRLEBytes[nDataSize]; dst -= BUFFER_WIDTH + w) {
		for (i = w; i;) {
			width = *src++;
			if (!(width & 0x80)) {
				i -= width;
				if (dst < gpBufEnd && dst > gpBufStart) {
					if (width & 1) {
						dst[0] = tbl[src[0]];
						src++;
						dst++;
					}
					width >>= 1;
					if (width & 1) {
						dst[0] = tbl[src[0]];
						dst[1] = tbl[src[1]];
						src += 2;
						dst += 2;
					}
					width >>= 1;
					for (; width; width--) {
						dst[0] = tbl[src[0]];
						dst[1] = tbl[src[1]];
						dst[2] = tbl[src[2]];
						dst[3] = tbl[src[3]];
						src += 4;
						dst += 4;
					}
				} else {
					src += width;
					dst += width;
				}
			} else {
				width = -(char)width;
				dst += width;
				i -= width;
			}
		}
	}
}

/**
 * @brief Same as CelBlitLightSafe, with transparancy applied
 * @param pDecodeTo The output buffer
 * @param pRLEBytes CEL pixel stream (run-length encoded)
 * @param nDataSize Size of CEL in bytes
 * @param nWidth Width of sprite
 */
void CelBlitLightTransSafe(uint8_t *pDecodeTo, uint8_t *pRLEBytes, int nDataSize, int nWidth)
{
	int w;
	bool shift;
	uint8_t *tbl;

	assert(pDecodeTo != NULL);
	assert(pRLEBytes != NULL);
	assert(gpBuffer);

	int i;
	uint8_t width;
	uint8_t *src, *dst;

	src = pRLEBytes;
	dst = pDecodeTo;
	tbl = &pLightTbl[light_table_index * 256];
	w = nWidth;
	shift = (uint8_t)(size_t)dst & 1;

	for (; src != &pRLEBytes[nDataSize]; dst -= BUFFER_WIDTH + w, shift = (shift + 1) & 1) {
		for (i = w; i;) {
			width = *src++;
			if (!(width & 0x80)) {
				i -= width;
				if (dst < gpBufEnd && dst > gpBufStart) {
					if (((uint8_t)(size_t)dst & 1) == shift) {
						if (!(width & 1)) {
							goto L_ODD;
						} else {
							src++;
							dst++;
						L_EVEN:
							width >>= 1;
							if (width & 1) {
								dst[0] = tbl[src[0]];
								src += 2;
								dst += 2;
							}
							width >>= 1;
							for (; width; width--) {
								dst[0] = tbl[src[0]];
								dst[2] = tbl[src[2]];
								src += 4;
								dst += 4;
							}
						}
					} else {
						if (!(width & 1)) {
							goto L_EVEN;
						} else {
							dst[0] = tbl[src[0]];
							src++;
							dst++;
						L_ODD:
							width >>= 1;
							if (width & 1) {
								dst[1] = tbl[src[1]];
								src += 2;
								dst += 2;
							}
							width >>= 1;
							for (; width; width--) {
								dst[1] = tbl[src[1]];
								dst[3] = tbl[src[3]];
								src += 4;
								dst += 4;
							}
						}
					}
				} else {
					src += width;
					dst += width;
				}
			} else {
				width = -(char)width;
				dst += width;
				i -= width;
			}
		}
	}
}

/**
 * @brief Same as CelBlitLightTransSafe
 * @param pBuff Target buffer
 * @param pCelBuff Cel data
 * @param nCel CEL frame number
 * @param nWidth Width of sprite
 */
void CelClippedBlitLightTrans(uint8_t *pBuff, uint8_t *pCelBuff, int nCel, int nWidth, int trans)
{
	int nDataSize;
	uint8_t *pRLEBytes;

	assert(pCelBuff != NULL);

	pRLEBytes = CelGetFrameClipped(pCelBuff, nCel, &nDataSize);

	if (trans)
		CelBlitLightTransSafe(pBuff, pRLEBytes, nDataSize, nWidth);
	else if (light_table_index)
		CelBlitLightSafe(pBuff, pRLEBytes, nDataSize, nWidth, NULL);
	else
		CelBlitSafe(pBuff, pRLEBytes, nDataSize, nWidth);
}

/**
 * @brief Same as CelDrawLightRed but checks for drawing outside the buffer
 * @param sx Back buffer coordinate
 * @param sy Back buffer coordinate
 * @param pCelBuff Cel data
 * @param nCel CEL frame number
 * @param nWidth Width of cel
 * @param light Light shade to use
 */
void CelDrawLightRedSafe(V2Di s, uint8_t *pCelBuff, int nCel, int nWidth, char light)
{
	int nDataSize, w, idx;
	uint8_t *pRLEBytes, *dst, *tbl;

	assert(gpBuffer);
	assert(pCelBuff != NULL);

	pRLEBytes = CelGetFrameClipped(pCelBuff, nCel, &nDataSize);
	dst = &gpBuffer[s.x + BUFFER_WIDTH * s.y];

	idx = light4flag ? 1024 : 4096;
	if (light == 2)
		idx += 256; // gray colors
	if (light >= 4)
		idx += (light - 1) << 8;

	tbl = &pLightTbl[idx];

	uint8_t width;
	uint8_t *end;

	end = &pRLEBytes[nDataSize];

	for (; pRLEBytes != end; dst -= BUFFER_WIDTH + nWidth) {
		for (w = nWidth; w;) {
			width = *pRLEBytes++;
			if (!(width & 0x80)) {
				w -= width;
				if (dst < gpBufEnd && dst > gpBufStart) {
					while (width) {
						*dst = tbl[*pRLEBytes];
						pRLEBytes++;
						dst++;
						width--;
					}
				} else {
					pRLEBytes += width;
					dst += width;
				}
			} else {
				width = -(char)width;
				dst += width;
				w -= width;
			}
		}
	}
}

/**
 * @brief Blit to a buffer at given coordinates
 * @param pBuff Target buffer
 * @param x Cordinate in pBuff buffer
 * @param y Cordinate in pBuff buffer
 * @param wdt Width of pBuff
 * @param pCelBuff Cel data
 * @param nCel CEL frame number
 * @param nWidth Width of cel
 */
void CelBlitWidth(uint8_t *pBuff, V2Di s, int wdt, uint8_t *pCelBuff, int nCel, int nWidth)
{
	uint8_t *pRLEBytes, *dst, *end;

	assert(pCelBuff != NULL);
	assert(pBuff != NULL);

	int i, nDataSize;
	uint8_t width;

	pRLEBytes = CelGetFrame(pCelBuff, nCel, &nDataSize);
	end = &pRLEBytes[nDataSize];
	dst = &pBuff[s.y * wdt + s.x];

	for (; pRLEBytes != end; dst -= wdt + nWidth) {
		for (i = nWidth; i;) {
			width = *pRLEBytes++;
			if (!(width & 0x80)) {
				i -= width;
				memcpy(dst, pRLEBytes, width);
				dst += width;
				pRLEBytes += width;
			} else {
				width = -(char)width;
				dst += width;
				i -= width;
			}
		}
	}
}

/**
 * @brief Blit a solid colder shape one pixel larger then the given sprite shape, to the back buffer at the given coordianates
 * @param col Color index from current palette
 * @param sx Back buffer coordinate
 * @param sy Back buffer coordinate
 * @param pCelBuff CEL buffer
 * @param nCel CEL frame number
 * @param nWidth Width of sprite
 */
void CelBlitOutline(char col, V2Di s, uint8_t *pCelBuff, int nCel, int nWidth)
{
	int nDataSize, w;
	uint8_t *src, *dst, *end;
	uint8_t width;

	assert(pCelBuff != NULL);
	assert(gpBuffer);

	src = CelGetFrameClipped(pCelBuff, nCel, &nDataSize);
	end = &src[nDataSize];
	dst = &gpBuffer[s.x + BUFFER_WIDTH * s.y];

	for (; src != end; dst -= BUFFER_WIDTH + nWidth) {
		for (w = nWidth; w;) {
			width = *src++;
			if (!(width & 0x80)) {
				w -= width;
				if (dst < gpBufEnd && dst > gpBufStart) {
					if (dst >= gpBufEnd - BUFFER_WIDTH) {
						while (width) {
							if (*src++) {
								dst[-BUFFER_WIDTH] = col;
								dst[-1] = col;
								dst[1] = col;
							}
							dst++;
							width--;
						}
					} else {
						while (width) {
							if (*src++) {
								dst[-BUFFER_WIDTH] = col;
								dst[-1] = col;
								dst[1] = col;
								dst[BUFFER_WIDTH] = col;
							}
							dst++;
							width--;
						}
					}
				} else {
					src += width;
					dst += width;
				}
			} else {
				width = -(char)width;
				dst += width;
				w -= width;
			}
		}
	}
}

void ENG_set_pixel(int sx, int sy, uint8_t col)
{
	ENG_set_pixel({ sx, sy }, col);
}

/**
 * @brief Set the value of a single pixel in the back buffer, checks bounds
 * @param sx Back buffer coordinate
 * @param sy Back buffer coordinate
 * @param col Color index from current palette
 */
void ENG_set_pixel(V2Di s, uint8_t col)
{
	uint8_t *dst;

	assert(gpBuffer);

	if (s.y < 0 || s.y >= SCREEN_HEIGHT + SCREEN_Y || s.x < SCREEN_X || s.x >= SCREEN_WIDTH + SCREEN_X)
		return;

	dst = &gpBuffer[s.x + BUFFER_WIDTH * s.y];

	if (dst < gpBufEnd && dst > gpBufStart)
		*dst = col;
}

/**
 * @brief Set the value of a single pixel in the back buffer to that of gbPixelCol, checks bounds
 * @param sx Back buffer coordinate
 * @param sy Back buffer coordinate
 */
void engine_draw_pixel(V2Di s)
{
	uint8_t *dst;

	assert(gpBuffer);

	if (gbRotateMap) {
		if (gbNotInView && (s.x < 0 || s.x >= SCREEN_HEIGHT + SCREEN_Y || s.y < SCREEN_X || s.y >= SCREEN_WIDTH + SCREEN_X))
			return;
		dst = &gpBuffer[s.y + BUFFER_WIDTH * s.x];
	} else {
		if (gbNotInView && (s.y < 0 || s.y >= SCREEN_HEIGHT + SCREEN_Y || s.x < SCREEN_X || s.x >= SCREEN_WIDTH + SCREEN_X))
			return;
		dst = &gpBuffer[s.x + BUFFER_WIDTH * s.y];
	}

	if (dst < gpBufEnd && dst > gpBufStart)
		*dst = gbPixelCol;
}

void DrawLine(int x0, int y0, int x1, int y1, uint8_t col)
{
	DrawLine({ x0, y0 }, { x1, y1 }, col);
}


/**
 * @brief Draw a line on the back buffer
 * @param x0 Back buffer coordinate
 * @param y0 Back buffer coordinate
 * @param x1 Back buffer coordinate
 * @param y1 Back buffer coordinate
 * @param col Color index from current palette
 */
void DrawLine(V2Di p0, V2Di p1, uint8_t col)
{
	int i, steps;
	float ix, iy, sx, sy;
	V2Di d = p1 - p0;
	steps = d.abs().maxdim();
	ix = d.x / (float)steps;
	iy = d.y / (float)steps;
	sx = p0.x;
	sy = p0.y;

	for (i = 0; i <= steps; i++, sx += ix, sy += iy) {
		ENG_set_pixel(V2Di(sx, sy), col);
	}
}

/**
 * @brief Calculate the best fit direction between two points
 * @param x1 Tile coordinate
 * @param y1 Tile coordinate
 * @param x2 Tile coordinate
 * @param y2 Tile coordinate
 * @return A value from the direction enum
 */
Dir GetDirection(V2Di t1, V2Di t2)
{
	int mx, my;
	int ny;
	Dir md;

	mx = t2.x - t1.x;
	my = t2.y - t1.y;

	if (mx >= 0) {
		if (my >= 0) {
			md = Dir::S;
			if (2 * mx < my)
				md = Dir::SW;
		} else {
			my = -my;
			md = Dir::E;
			if (2 * mx < my)
				md = Dir::NE;
		}
		if (2 * my < mx)
			return Dir::SE;
	} else {
		if (my >= 0) {
			ny = -mx;
			md = Dir::W;
			if (2 * ny < my)
				md = Dir::SW;
		} else {
			ny = -mx;
			my = -my;
			md = Dir::N;
			if (2 * ny < my)
				md = Dir::NE;
		}
		if (2 * my < ny)
			return Dir::NW;
	}

	return md;
}

/**
 * @brief Set the RNG seed
 * @param s RNG seed
 */
void SetRndSeed(int s)
{
	SeedCount = 0;
	sglGameSeed = s;
	orgseed = s;
}

/**
 * @brief Get the current RNG seed
 * @return RNG seed
 */
int GetRndSeed()
{
	SeedCount++;
	sglGameSeed = static_cast<unsigned int>(RndMult) * sglGameSeed + RndInc;
	return abs(sglGameSeed);
}

/**
 * @brief Main RNG function
 * @param idx Unused
 * @param v The upper limit for the return value
 * @return A random number from 0 to (v-1)
 */
int random_(uint8_t idx, int v)
{
	if (v <= 0)
		return 0;
	if (v < 0xFFFF)
		return (GetRndSeed() >> 16) % v;
	return GetRndSeed() % v;
}

// Modern 64-bit number generator
uint64_t rand64()
{
	static std::random_device rnddevice;
	static std::mt19937 rndgenerator(rnddevice());
	static std::uniform_int_distribution<uint64_t> rnddistribution(0, UINT64_MAX);
	return rnddistribution(rndgenerator);
}

/**
 * @brief Multithreaded safe malloc
 * @param dwBytes Byte size to allocate
 */
uint8_t *DiabloAllocPtr(DWORD dwBytes)
{
	uint8_t *buf;

	sgMemCrit.Enter();
	buf = (uint8_t *)SMemAlloc(dwBytes, __FILE__, __LINE__, 0);
	sgMemCrit.Leave();

	if (buf == NULL) {
		char *text = "System memory exhausted.\n"
		             "Make sure you have at least 64MB of free system memory before running the game";
		ERR_DLG("Out of Memory Error", text);
	}

	return buf;
}

/**
 * @brief Multithreaded safe memfree
 * @param p Memory pointer to free
 */
void mem_free_dbg(void *p)
{
	if (p) {
		sgMemCrit.Enter();
		SMemFree(p, __FILE__, __LINE__, 0);
		sgMemCrit.Leave();
	}
}

/**
 * @brief Load a file in to a buffer
 * @param pszName Path of file
 * @param pdwFileLen Will be set to file size if non-NULL
 * @return Buffer with content of file
 */
uint8_t *LoadFileInMem(const char *pszName, DWORD *pdwFileLen)
{
	HANDLE file;
	uint8_t *buf;
	int fileLen;

	WOpenFile(pszName, &file, false);
	fileLen = WGetFileSize(file, NULL, pszName);

	if (pdwFileLen)
		*pdwFileLen = fileLen;

	if (!fileLen)
		app_fatal("Zero length SFILE:\n%s", pszName);

	buf = (uint8_t *)DiabloAllocPtr(fileLen);

	WReadFile(file, buf, fileLen, pszName);
	WCloseFile(file);

	return buf;
}

/**
 * @brief Load a file in to the given buffer
 * @param pszName Path of file
 * @param p Target buffer
 * @return Size of file
 */
DWORD LoadFileWithMem(const char *pszName, uint8_t *p)
{
	DWORD dwFileLen;
	HANDLE hsFile;

	assert(pszName);
	if (p == NULL) {
		app_fatal("LoadFileWithMem(NULL):\n%s", pszName);
	}

	WOpenFile(pszName, &hsFile, false);

	dwFileLen = WGetFileSize(hsFile, NULL, pszName);
	if (dwFileLen == 0) {
		app_fatal("Zero length SFILE:\n%s", pszName);
	}

	WReadFile(hsFile, p, dwFileLen, pszName);
	WCloseFile(hsFile);

	return dwFileLen;
}

/**
 * @brief Apply the color swaps to a CL2 sprite
 * @param p CL2 buffer
 * @param ttbl Palette translation table
 * @param nCel Frame number in CL2 file
 */
void Cl2ApplyTrans(uint8_t *p, uint8_t *ttbl, int nCel)
{
	int i, nDataSize;
	char width;
	uint8_t *dst;

	assert(p != NULL);
	assert(ttbl != NULL);

	for (i = 1; i <= nCel; i++) {
		dst = CelGetFrame(p, i, &nDataSize) + 10;
		nDataSize -= 10;
		while (nDataSize) {
			width = *dst++;
			nDataSize--;
			assert(nDataSize >= 0);
			if (width < 0) {
				width = -width;
				if (width > 65) {
					nDataSize--;
					assert(nDataSize >= 0);
					*dst = ttbl[*dst];
					dst++;
				} else {
					nDataSize -= width;
					assert(nDataSize >= 0);
					while (width) {
						*dst = ttbl[*dst];
						dst++;
						width--;
					}
				}
			}
		}
	}
}

/**
 * @brief Blit CL2 sprite, to the back buffer at the given coordianates
 * @param sx Back buffer coordinate
 * @param sy Back buffer coordinate
 * @param pCelBuff CL2 buffer
 * @param nCel CL2 frame number
 * @param nWidth Width of sprite
 */
void Cl2Draw(V2Di s, uint8_t *pCelBuff, int nCel, int nWidth)
{
	uint8_t *pRLEBytes;
	int nDataSize;

	assert(gpBuffer != NULL);
	assert(pCelBuff != NULL);
	assert(nCel > 0);

	pRLEBytes = CelGetFrameClipped(pCelBuff, nCel, &nDataSize);

	Cl2BlitSafe(
	    &gpBuffer[s.x + BUFFER_WIDTH * s.y],
	    pRLEBytes,
	    nDataSize,
	    nWidth);
}

/**
 * @brief Blit CL2 sprite to the given buffer
 * @param pDecodeTo The output buffer
 * @param pRLEBytes CL2 pixel stream (run-length encoded)
 * @param nDataSize Size of CL2 in bytes
 * @param nWidth Width of sprite
 */
void Cl2BlitSafe(uint8_t *pDecodeTo, uint8_t *pRLEBytes, int nDataSize, int nWidth)
{
	int w;
	char width;
	uint8_t fill;
	uint8_t *src, *dst;

	src = pRLEBytes;
	dst = pDecodeTo;
	w = nWidth;

	while (nDataSize) {
		width = *src++;
		nDataSize--;
		if (width < 0) {
			width = -width;
			if (width > 65) {
				width -= 65;
				nDataSize--;
				fill = *src++;
				if (dst < gpBufEnd && dst > gpBufStart) {
					w -= width;
					while (width) {
						*dst = fill;
						dst++;
						width--;
					}
					if (!w) {
						w = nWidth;
						dst -= BUFFER_WIDTH + w;
					}
					continue;
				}
			} else {
				nDataSize -= width;
				if (dst < gpBufEnd && dst > gpBufStart) {
					w -= width;
					while (width) {
						*dst = *src;
						src++;
						dst++;
						width--;
					}
					if (!w) {
						w = nWidth;
						dst -= BUFFER_WIDTH + w;
					}
					continue;
				} else {
					src += width;
				}
			}
		}
		while (width) {
			if (width > w) {
				dst += w;
				width -= w;
				w = 0;
			} else {
				dst += width;
				w -= width;
				width = 0;
			}
			if (!w) {
				w = nWidth;
				dst -= BUFFER_WIDTH + w;
			}
		}
	}
}

/**
 * @brief Blit a solid colder shape one pixel larger then the given sprite shape, to the back buffer at the given coordianates
 * @param col Color index from current palette
 * @param sx Back buffer coordinate
 * @param sy Back buffer coordinate
 * @param pCelBuff CL2 buffer
 * @param nCel CL2 frame number
 * @param nWidth Width of sprite
 */
void Cl2DrawOutline(char col, V2Di s, uint8_t *pCelBuff, int nCel, int nWidth)
{
	int nDataSize;
	uint8_t *pRLEBytes;

	assert(gpBuffer != NULL);
	assert(pCelBuff != NULL);
	assert(nCel > 0);

	pRLEBytes = CelGetFrameClipped(pCelBuff, nCel, &nDataSize);

	gpBufEnd -= BUFFER_WIDTH;
	Cl2BlitOutlineSafe(
	    &gpBuffer[s.x + BUFFER_WIDTH * s.y],
	    pRLEBytes,
	    nDataSize,
	    nWidth,
	    col);
	gpBufEnd += BUFFER_WIDTH;
}

/**
 * @brief Blit a solid colder shape one pixel larger then the given sprite shape, to the given buffer
 * @param pDecodeTo The output buffer
 * @param pRLEBytes CL2 pixel stream (run-length encoded)
 * @param nDataSize Size of CL2 in bytes
 * @param nWidth Width of sprite
 * @param col Color index from current palette
 */
void Cl2BlitOutlineSafe(uint8_t *pDecodeTo, uint8_t *pRLEBytes, int nDataSize, int nWidth, char col)
{
	int w;
	char width;
	uint8_t *src, *dst;

	src = pRLEBytes;
	dst = pDecodeTo;
	w = nWidth;

	while (nDataSize) {
		width = *src++;
		nDataSize--;
		if (width < 0) {
			width = -width;
			if (width > 65) {
				width -= 65;
				nDataSize--;
				if (*src++ && dst < gpBufEnd && dst > gpBufStart) {
					w -= width;
					dst[-1] = col;
					dst[width] = col;
					while (width) {
						dst[-BUFFER_WIDTH] = col;
						dst[BUFFER_WIDTH] = col;
						dst++;
						width--;
					}
					if (!w) {
						w = nWidth;
						dst -= BUFFER_WIDTH + w;
					}
					continue;
				}
			} else {
				nDataSize -= width;
				if (dst < gpBufEnd && dst > gpBufStart) {
					w -= width;
					while (width) {
						if (*src++) {
							dst[-1] = col;
							dst[1] = col;
							dst[-BUFFER_WIDTH] = col;
							dst[BUFFER_WIDTH] = col;
						}
						dst++;
						width--;
					}
					if (!w) {
						w = nWidth;
						dst -= BUFFER_WIDTH + w;
					}
					continue;
				} else {
					src += width;
				}
			}
		}
		while (width) {
			if (width > w) {
				dst += w;
				width -= w;
				w = 0;
			} else {
				dst += width;
				w -= width;
				width = 0;
			}
			if (!w) {
				w = nWidth;
				dst -= BUFFER_WIDTH + w;
			}
		}
	}
}

/**
 * @brief Blit CL2 sprite, and apply a given lighting, to the back buffer at the given coordianates
 * @param sx Back buffer coordinate
 * @param sy Back buffer coordinate
 * @param pCelBuff CL2 buffer
 * @param nCel CL2 frame number
 * @param nWidth Width of sprite
 * @param light Light shade to use
 */
void Cl2DrawLightTbl(V2Di s, uint8_t *pCelBuff, int nCel, int nWidth, char light)
{
	int nDataSize, idx;
	uint8_t *pRLEBytes, *pDecodeTo;

	assert(gpBuffer != NULL);
	assert(pCelBuff != NULL);
	assert(nCel > 0);

	pRLEBytes = CelGetFrameClipped(pCelBuff, nCel, &nDataSize);
	pDecodeTo = &gpBuffer[s.x + BUFFER_WIDTH * s.y];

	idx = light4flag ? 1024 : 4096;
	if (light == 2)
		idx += 256; // gray colors
	if (light >= 4)
		idx += (light - 1) << 8;

	Cl2BlitLightSafe(
	    pDecodeTo,
	    pRLEBytes,
	    nDataSize,
	    nWidth,
	    &pLightTbl[idx]);
}

/**
 * @brief Blit CL2 sprite, and apply lighting, to the given buffer
 * @param pDecodeTo The output buffer
 * @param pRLEBytes CL2 pixel stream (run-length encoded)
 * @param nDataSize Size of CL2 in bytes
 * @param nWidth With of CL2 sprite
 * @param pTable Light color table
 */
void Cl2BlitLightSafe(uint8_t *pDecodeTo, uint8_t *pRLEBytes, int nDataSize, int nWidth, uint8_t *pTable)
{
	int w;
	char width;
	uint8_t fill;
	uint8_t *src, *dst;

	src = pRLEBytes;
	dst = pDecodeTo;
	w = nWidth;
	sgnWidth = nWidth;

	while (nDataSize) {
		width = *src++;
		nDataSize--;
		if (width < 0) {
			width = -width;
			if (width > 65) {
				width -= 65;
				nDataSize--;
				fill = pTable[*src++];
				if (dst < gpBufEnd && dst > gpBufStart) {
					w -= width;
					while (width) {
						*dst = fill;
						dst++;
						width--;
					}
					if (!w) {
						w = sgnWidth;
						dst -= BUFFER_WIDTH + w;
					}
					continue;
				}
			} else {
				nDataSize -= width;
				if (dst < gpBufEnd && dst > gpBufStart) {
					w -= width;
					while (width) {
						*dst = pTable[*src];
						src++;
						dst++;
						width--;
					}
					if (!w) {
						w = sgnWidth;
						dst -= BUFFER_WIDTH + w;
					}
					continue;
				} else {
					src += width;
				}
			}
		}
		while (width) {
			if (width > w) {
				dst += w;
				width -= w;
				w = 0;
			} else {
				dst += width;
				w -= width;
				width = 0;
			}
			if (!w) {
				w = sgnWidth;
				dst -= BUFFER_WIDTH + w;
			}
		}
	}
}

/**
 * @brief Blit CL2 sprite, and apply lighting, to the back buffer at the given coordinates
 * @param sx Back buffer coordinate
 * @param sy Back buffer coordinate
 * @param pCelBuff CL2 buffer
 * @param nCel CL2 frame number
 * @param nWidth Width of sprite
 */
void Cl2DrawLight(V2Di s, uint8_t *pCelBuff, int nCel, int nWidth)
{
	int nDataSize;
	uint8_t *pRLEBytes, *pDecodeTo;

	assert(gpBuffer != NULL);
	assert(pCelBuff != NULL);
	assert(nCel > 0);

	pRLEBytes = CelGetFrameClipped(pCelBuff, nCel, &nDataSize);
	pDecodeTo = &gpBuffer[s.x + BUFFER_WIDTH * s.y];

	if (light_table_index)
		Cl2BlitLightSafe(pDecodeTo, pRLEBytes, nDataSize, nWidth, &pLightTbl[light_table_index * 256]);
	else
		Cl2BlitSafe(pDecodeTo, pRLEBytes, nDataSize, nWidth);
}

/**
 * @brief Fade to black and play a video
 * @param pszMovie file path of movie
 */
void PlayInGameMovie(char *pszMovie)
{
	PaletteFadeOut(8);
	play_movie(pszMovie, false);
	ClearScreenBuffer();
	force_redraw = 255;
	scrollrt_draw_game_screen(true);
	PaletteFadeIn(8);
	force_redraw = 255;
}

DEVILUTION_END_NAMESPACE
