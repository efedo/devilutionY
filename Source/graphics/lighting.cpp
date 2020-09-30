/**
 * @file lighting.cpp
 *
 * Implementation of light and vision.
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

LightListStruct VisionList[MAXVISION];
uint8_t lightactive[MAXLIGHTS];
LightListStruct LightList[MAXLIGHTS];
int numlights;
uint8_t lightradius[16][128];
bool dovision;
int numvision;
char lightmax;
bool dolighting;
uint8_t lightblock[64][16][16];
int visionid;
uint8_t *pLightTbl;
bool lightflag;

/**
 * CrawlTable specifies X- and Y-coordinate deltas from a missile target coordinate.
 *
 * n=4
 *
 *    y
 *    ^
 *    |  1
 *    | 3#4
 *    |  2
 *    +-----> x
 *
 * n=16
 *
 *    y
 *    ^
 *    |  314
 *    | B7 8C
 *    | F # G
 *    | D9 AE
 *    |  526
 *    +-------> x
 */
char CrawlTable[2749] = {
	1,
	0, 0,
	4,
	0, 1, 0, -1, -1, 0, 1, 0,
	16,
	0, 2, 0, -2, -1, 2, 1, 2,
	-1, -2, 1, -2, -1, 1, 1, 1,
	-1, -1, 1, -1, -2, 1, 2, 1,
	-2, -1, 2, -1, -2, 0, 2, 0,
	24,
	0, 3, 0, -3, -1, 3, 1, 3,
	-1, -3, 1, -3, -2, 3, 2, 3,
	-2, -3, 2, -3, -2, 2, 2, 2,
	-2, -2, 2, -2, -3, 2, 3, 2,
	-3, -2, 3, -2, -3, 1, 3, 1,
	-3, -1, 3, -1, -3, 0, 3, 0,
	32,
	0, 4, 0, -4, -1, 4, 1, 4,
	-1, -4, 1, -4, -2, 4, 2, 4,
	-2, -4, 2, -4, -3, 4, 3, 4,
	-3, -4, 3, -4, -3, 3, 3, 3,
	-3, -3, 3, -3, -4, 3, 4, 3,
	-4, -3, 4, -3, -4, 2, 4, 2,
	-4, -2, 4, -2, -4, 1, 4, 1,
	-4, -1, 4, -1, -4, 0, 4, 0,
	40,
	0, 5, 0, -5, -1, 5, 1, 5,
	-1, -5, 1, -5, -2, 5, 2, 5,
	-2, -5, 2, -5, -3, 5, 3, 5,
	-3, -5, 3, -5, -4, 5, 4, 5,
	-4, -5, 4, -5, -4, 4, 4, 4,
	-4, -4, 4, -4, -5, 4, 5, 4,
	-5, -4, 5, -4, -5, 3, 5, 3,
	-5, -3, 5, -3, -5, 2, 5, 2,
	-5, -2, 5, -2, -5, 1, 5, 1,
	-5, -1, 5, -1, -5, 0, 5, 0,
	48,
	0, 6, 0, -6, -1, 6, 1, 6,
	-1, -6, 1, -6, -2, 6, 2, 6,
	-2, -6, 2, -6, -3, 6, 3, 6,
	-3, -6, 3, -6, -4, 6, 4, 6,
	-4, -6, 4, -6, -5, 6, 5, 6,
	-5, -6, 5, -6, -5, 5, 5, 5,
	-5, -5, 5, -5, -6, 5, 6, 5,
	-6, -5, 6, -5, -6, 4, 6, 4,
	-6, -4, 6, -4, -6, 3, 6, 3,
	-6, -3, 6, -3, -6, 2, 6, 2,
	-6, -2, 6, -2, -6, 1, 6, 1,
	-6, -1, 6, -1, -6, 0, 6, 0,
	56,
	0, 7, 0, -7, -1, 7, 1, 7,
	-1, -7, 1, -7, -2, 7, 2, 7,
	-2, -7, 2, -7, -3, 7, 3, 7,
	-3, -7, 3, -7, -4, 7, 4, 7,
	-4, -7, 4, -7, -5, 7, 5, 7,
	-5, -7, 5, -7, -6, 7, 6, 7,
	-6, -7, 6, -7, -6, 6, 6, 6,
	-6, -6, 6, -6, -7, 6, 7, 6,
	-7, -6, 7, -6, -7, 5, 7, 5,
	-7, -5, 7, -5, -7, 4, 7, 4,
	-7, -4, 7, -4, -7, 3, 7, 3,
	-7, -3, 7, -3, -7, 2, 7, 2,
	-7, -2, 7, -2, -7, 1, 7, 1,
	-7, -1, 7, -1, -7, 0, 7, 0,
	64,
	0, 8, 0, -8, -1, 8, 1, 8,
	-1, -8, 1, -8, -2, 8, 2, 8,
	-2, -8, 2, -8, -3, 8, 3, 8,
	-3, -8, 3, -8, -4, 8, 4, 8,
	-4, -8, 4, -8, -5, 8, 5, 8,
	-5, -8, 5, -8, -6, 8, 6, 8,
	-6, -8, 6, -8, -7, 8, 7, 8,
	-7, -8, 7, -8, -7, 7, 7, 7,
	-7, -7, 7, -7, -8, 7, 8, 7,
	-8, -7, 8, -7, -8, 6, 8, 6,
	-8, -6, 8, -6, -8, 5, 8, 5,
	-8, -5, 8, -5, -8, 4, 8, 4,
	-8, -4, 8, -4, -8, 3, 8, 3,
	-8, -3, 8, -3, -8, 2, 8, 2,
	-8, -2, 8, -2, -8, 1, 8, 1,
	-8, -1, 8, -1, -8, 0, 8, 0,
	72,
	0, 9, 0, -9, -1, 9, 1, 9,
	-1, -9, 1, -9, -2, 9, 2, 9,
	-2, -9, 2, -9, -3, 9, 3, 9,
	-3, -9, 3, -9, -4, 9, 4, 9,
	-4, -9, 4, -9, -5, 9, 5, 9,
	-5, -9, 5, -9, -6, 9, 6, 9,
	-6, -9, 6, -9, -7, 9, 7, 9,
	-7, -9, 7, -9, -8, 9, 8, 9,
	-8, -9, 8, -9, -8, 8, 8, 8,
	-8, -8, 8, -8, -9, 8, 9, 8,
	-9, -8, 9, -8, -9, 7, 9, 7,
	-9, -7, 9, -7, -9, 6, 9, 6,
	-9, -6, 9, -6, -9, 5, 9, 5,
	-9, -5, 9, -5, -9, 4, 9, 4,
	-9, -4, 9, -4, -9, 3, 9, 3,
	-9, -3, 9, -3, -9, 2, 9, 2,
	-9, -2, 9, -2, -9, 1, 9, 1,
	-9, -1, 9, -1, -9, 0, 9, 0,
	80,
	0, 10, 0, -10, -1, 10, 1, 10,
	-1, -10, 1, -10, -2, 10, 2, 10,
	-2, -10, 2, -10, -3, 10, 3, 10,
	-3, -10, 3, -10, -4, 10, 4, 10,
	-4, -10, 4, -10, -5, 10, 5, 10,
	-5, -10, 5, -10, -6, 10, 6, 10,
	-6, -10, 6, -10, -7, 10, 7, 10,
	-7, -10, 7, -10, -8, 10, 8, 10,
	-8, -10, 8, -10, -9, 10, 9, 10,
	-9, -10, 9, -10, -9, 9, 9, 9,
	-9, -9, 9, -9, -10, 9, 10, 9,
	-10, -9, 10, -9, -10, 8, 10, 8,
	-10, -8, 10, -8, -10, 7, 10, 7,
	-10, -7, 10, -7, -10, 6, 10, 6,
	-10, -6, 10, -6, -10, 5, 10, 5,
	-10, -5, 10, -5, -10, 4, 10, 4,
	-10, -4, 10, -4, -10, 3, 10, 3,
	-10, -3, 10, -3, -10, 2, 10, 2,
	-10, -2, 10, -2, -10, 1, 10, 1,
	-10, -1, 10, -1, -10, 0, 10, 0,
	88,
	0, 11, 0, -11, -1, 11, 1, 11,
	-1, -11, 1, -11, -2, 11, 2, 11,
	-2, -11, 2, -11, -3, 11, 3, 11,
	-3, -11, 3, -11, -4, 11, 4, 11,
	-4, -11, 4, -11, -5, 11, 5, 11,
	-5, -11, 5, -11, -6, 11, 6, 11,
	-6, -11, 6, -11, -7, 11, 7, 11,
	-7, -11, 7, -11, -8, 11, 8, 11,
	-8, -11, 8, -11, -9, 11, 9, 11,
	-9, -11, 9, -11, -10, 11, 10, 11,
	-10, -11, 10, -11, -10, 10, 10, 10,
	-10, -10, 10, -10, -11, 10, 11, 10,
	-11, -10, 11, -10, -11, 9, 11, 9,
	-11, -9, 11, -9, -11, 8, 11, 8,
	-11, -8, 11, -8, -11, 7, 11, 7,
	-11, -7, 11, -7, -11, 6, 11, 6,
	-11, -6, 11, -6, -11, 5, 11, 5,
	-11, -5, 11, -5, -11, 4, 11, 4,
	-11, -4, 11, -4, -11, 3, 11, 3,
	-11, -3, 11, -3, -11, 2, 11, 2,
	-11, -2, 11, -2, -11, 1, 11, 1,
	-11, -1, 11, -1, -11, 0, 11, 0,
	96,
	0, 12, 0, -12, -1, 12, 1, 12,
	-1, -12, 1, -12, -2, 12, 2, 12,
	-2, -12, 2, -12, -3, 12, 3, 12,
	-3, -12, 3, -12, -4, 12, 4, 12,
	-4, -12, 4, -12, -5, 12, 5, 12,
	-5, -12, 5, -12, -6, 12, 6, 12,
	-6, -12, 6, -12, -7, 12, 7, 12,
	-7, -12, 7, -12, -8, 12, 8, 12,
	-8, -12, 8, -12, -9, 12, 9, 12,
	-9, -12, 9, -12, -10, 12, 10, 12,
	-10, -12, 10, -12, -11, 12, 11, 12,
	-11, -12, 11, -12, -11, 11, 11, 11,
	-11, -11, 11, -11, -12, 11, 12, 11,
	-12, -11, 12, -11, -12, 10, 12, 10,
	-12, -10, 12, -10, -12, 9, 12, 9,
	-12, -9, 12, -9, -12, 8, 12, 8,
	-12, -8, 12, -8, -12, 7, 12, 7,
	-12, -7, 12, -7, -12, 6, 12, 6,
	-12, -6, 12, -6, -12, 5, 12, 5,
	-12, -5, 12, -5, -12, 4, 12, 4,
	-12, -4, 12, -4, -12, 3, 12, 3,
	-12, -3, 12, -3, -12, 2, 12, 2,
	-12, -2, 12, -2, -12, 1, 12, 1,
	-12, -1, 12, -1, -12, 0, 12, 0,
	104,
	0, 13, 0, -13, -1, 13, 1, 13,
	-1, -13, 1, -13, -2, 13, 2, 13,
	-2, -13, 2, -13, -3, 13, 3, 13,
	-3, -13, 3, -13, -4, 13, 4, 13,
	-4, -13, 4, -13, -5, 13, 5, 13,
	-5, -13, 5, -13, -6, 13, 6, 13,
	-6, -13, 6, -13, -7, 13, 7, 13,
	-7, -13, 7, -13, -8, 13, 8, 13,
	-8, -13, 8, -13, -9, 13, 9, 13,
	-9, -13, 9, -13, -10, 13, 10, 13,
	-10, -13, 10, -13, -11, 13, 11, 13,
	-11, -13, 11, -13, -12, 13, 12, 13,
	-12, -13, 12, -13, -12, 12, 12, 12,
	-12, -12, 12, -12, -13, 12, 13, 12,
	-13, -12, 13, -12, -13, 11, 13, 11,
	-13, -11, 13, -11, -13, 10, 13, 10,
	-13, -10, 13, -10, -13, 9, 13, 9,
	-13, -9, 13, -9, -13, 8, 13, 8,
	-13, -8, 13, -8, -13, 7, 13, 7,
	-13, -7, 13, -7, -13, 6, 13, 6,
	-13, -6, 13, -6, -13, 5, 13, 5,
	-13, -5, 13, -5, -13, 4, 13, 4,
	-13, -4, 13, -4, -13, 3, 13, 3,
	-13, -3, 13, -3, -13, 2, 13, 2,
	-13, -2, 13, -2, -13, 1, 13, 1,
	-13, -1, 13, -1, -13, 0, 13, 0,
	112,
	0, 14, 0, -14, -1, 14, 1, 14,
	-1, -14, 1, -14, -2, 14, 2, 14,
	-2, -14, 2, -14, -3, 14, 3, 14,
	-3, -14, 3, -14, -4, 14, 4, 14,
	-4, -14, 4, -14, -5, 14, 5, 14,
	-5, -14, 5, -14, -6, 14, 6, 14,
	-6, -14, 6, -14, -7, 14, 7, 14,
	-7, -14, 7, -14, -8, 14, 8, 14,
	-8, -14, 8, -14, -9, 14, 9, 14,
	-9, -14, 9, -14, -10, 14, 10, 14,
	-10, -14, 10, -14, -11, 14, 11, 14,
	-11, -14, 11, -14, -12, 14, 12, 14,
	-12, -14, 12, -14, -13, 14, 13, 14,
	-13, -14, 13, -14, -13, 13, 13, 13,
	-13, -13, 13, -13, -14, 13, 14, 13,
	-14, -13, 14, -13, -14, 12, 14, 12,
	-14, -12, 14, -12, -14, 11, 14, 11,
	-14, -11, 14, -11, -14, 10, 14, 10,
	-14, -10, 14, -10, -14, 9, 14, 9,
	-14, -9, 14, -9, -14, 8, 14, 8,
	-14, -8, 14, -8, -14, 7, 14, 7,
	-14, -7, 14, -7, -14, 6, 14, 6,
	-14, -6, 14, -6, -14, 5, 14, 5,
	-14, -5, 14, -5, -14, 4, 14, 4,
	-14, -4, 14, -4, -14, 3, 14, 3,
	-14, -3, 14, -3, -14, 2, 14, 2,
	-14, -2, 14, -2, -14, 1, 14, 1,
	-14, -1, 14, -1, -14, 0, 14, 0,
	120,
	0, 15, 0, -15, -1, 15, 1, 15,
	-1, -15, 1, -15, -2, 15, 2, 15,
	-2, -15, 2, -15, -3, 15, 3, 15,
	-3, -15, 3, -15, -4, 15, 4, 15,
	-4, -15, 4, -15, -5, 15, 5, 15,
	-5, -15, 5, -15, -6, 15, 6, 15,
	-6, -15, 6, -15, -7, 15, 7, 15,
	-7, -15, 7, -15, -8, 15, 8, 15,
	-8, -15, 8, -15, -9, 15, 9, 15,
	-9, -15, 9, -15, -10, 15, 10, 15,
	-10, -15, 10, -15, -11, 15, 11, 15,
	-11, -15, 11, -15, -12, 15, 12, 15,
	-12, -15, 12, -15, -13, 15, 13, 15,
	-13, -15, 13, -15, -14, 15, 14, 15,
	-14, -15, 14, -15, -14, 14, 14, 14,
	-14, -14, 14, -14, -15, 14, 15, 14,
	-15, -14, 15, -14, -15, 13, 15, 13,
	-15, -13, 15, -13, -15, 12, 15, 12,
	-15, -12, 15, -12, -15, 11, 15, 11,
	-15, -11, 15, -11, -15, 10, 15, 10,
	-15, -10, 15, -10, -15, 9, 15, 9,
	-15, -9, 15, -9, -15, 8, 15, 8,
	-15, -8, 15, -8, -15, 7, 15, 7,
	-15, -7, 15, -7, -15, 6, 15, 6,
	-15, -6, 15, -6, -15, 5, 15, 5,
	-15, -5, 15, -5, -15, 4, 15, 4,
	-15, -4, 15, -4, -15, 3, 15, 3,
	-15, -3, 15, -3, -15, 2, 15, 2,
	-15, -2, 15, -2, -15, 1, 15, 1,
	-15, -1, 15, -1, -15, 0, 15, 0,
	(char)128,
	0, 16, 0, -16, -1, 16, 1, 16,
	-1, -16, 1, -16, -2, 16, 2, 16,
	-2, -16, 2, -16, -3, 16, 3, 16,
	-3, -16, 3, -16, -4, 16, 4, 16,
	-4, -16, 4, -16, -5, 16, 5, 16,
	-5, -16, 5, -16, -6, 16, 6, 16,
	-6, -16, 6, -16, -7, 16, 7, 16,
	-7, -16, 7, -16, -8, 16, 8, 16,
	-8, -16, 8, -16, -9, 16, 9, 16,
	-9, -16, 9, -16, -10, 16, 10, 16,
	-10, -16, 10, -16, -11, 16, 11, 16,
	-11, -16, 11, -16, -12, 16, 12, 16,
	-12, -16, 12, -16, -13, 16, 13, 16,
	-13, -16, 13, -16, -14, 16, 14, 16,
	-14, -16, 14, -16, -15, 16, 15, 16,
	-15, -16, 15, -16, -15, 15, 15, 15,
	-15, -15, 15, -15, -16, 15, 16, 15,
	-16, -15, 16, -15, -16, 14, 16, 14,
	-16, -14, 16, -14, -16, 13, 16, 13,
	-16, -13, 16, -13, -16, 12, 16, 12,
	-16, -12, 16, -12, -16, 11, 16, 11,
	-16, -11, 16, -11, -16, 10, 16, 10,
	-16, -10, 16, -10, -16, 9, 16, 9,
	-16, -9, 16, -9, -16, 8, 16, 8,
	-16, -8, 16, -8, -16, 7, 16, 7,
	-16, -7, 16, -7, -16, 6, 16, 6,
	-16, -6, 16, -6, -16, 5, 16, 5,
	-16, -5, 16, -5, -16, 4, 16, 4,
	-16, -4, 16, -4, -16, 3, 16, 3,
	-16, -3, 16, -3, -16, 2, 16, 2,
	-16, -2, 16, -2, -16, 1, 16, 1,
	-16, -1, 16, -1, -16, 0, 16, 0,
	(char)136,
	0, 17, 0, -17, -1, 17, 1, 17,
	-1, -17, 1, -17, -2, 17, 2, 17,
	-2, -17, 2, -17, -3, 17, 3, 17,
	-3, -17, 3, -17, -4, 17, 4, 17,
	-4, -17, 4, -17, -5, 17, 5, 17,
	-5, -17, 5, -17, -6, 17, 6, 17,
	-6, -17, 6, -17, -7, 17, 7, 17,
	-7, -17, 7, -17, -8, 17, 8, 17,
	-8, -17, 8, -17, -9, 17, 9, 17,
	-9, -17, 9, -17, -10, 17, 10, 17,
	-10, -17, 10, -17, -11, 17, 11, 17,
	-11, -17, 11, -17, -12, 17, 12, 17,
	-12, -17, 12, -17, -13, 17, 13, 17,
	-13, -17, 13, -17, -14, 17, 14, 17,
	-14, -17, 14, -17, -15, 17, 15, 17,
	-15, -17, 15, -17, -16, 17, 16, 17,
	-16, -17, 16, -17, -16, 16, 16, 16,
	-16, -16, 16, -16, -17, 16, 17, 16,
	-17, -16, 17, -16, -17, 15, 17, 15,
	-17, -15, 17, -15, -17, 14, 17, 14,
	-17, -14, 17, -14, -17, 13, 17, 13,
	-17, -13, 17, -13, -17, 12, 17, 12,
	-17, -12, 17, -12, -17, 11, 17, 11,
	-17, -11, 17, -11, -17, 10, 17, 10,
	-17, -10, 17, -10, -17, 9, 17, 9,
	-17, -9, 17, -9, -17, 8, 17, 8,
	-17, -8, 17, -8, -17, 7, 17, 7,
	-17, -7, 17, -7, -17, 6, 17, 6,
	-17, -6, 17, -6, -17, 5, 17, 5,
	-17, -5, 17, -5, -17, 4, 17, 4,
	-17, -4, 17, -4, -17, 3, 17, 3,
	-17, -3, 17, -3, -17, 2, 17, 2,
	-17, -2, 17, -2, -17, 1, 17, 1,
	-17, -1, 17, -1, -17, 0, 17, 0,
	(char)144,
	0, 18, 0, -18, -1, 18, 1, 18,
	-1, -18, 1, -18, -2, 18, 2, 18,
	-2, -18, 2, -18, -3, 18, 3, 18,
	-3, -18, 3, -18, -4, 18, 4, 18,
	-4, -18, 4, -18, -5, 18, 5, 18,
	-5, -18, 5, -18, -6, 18, 6, 18,
	-6, -18, 6, -18, -7, 18, 7, 18,
	-7, -18, 7, -18, -8, 18, 8, 18,
	-8, -18, 8, -18, -9, 18, 9, 18,
	-9, -18, 9, -18, -10, 18, 10, 18,
	-10, -18, 10, -18, -11, 18, 11, 18,
	-11, -18, 11, -18, -12, 18, 12, 18,
	-12, -18, 12, -18, -13, 18, 13, 18,
	-13, -18, 13, -18, -14, 18, 14, 18,
	-14, -18, 14, -18, -15, 18, 15, 18,
	-15, -18, 15, -18, -16, 18, 16, 18,
	-16, -18, 16, -18, -17, 18, 17, 18,
	-17, -18, 17, -18, -17, 17, 17, 17,
	-17, -17, 17, -17, -18, 17, 18, 17,
	-18, -17, 18, -17, -18, 16, 18, 16,
	-18, -16, 18, -16, -18, 15, 18, 15,
	-18, -15, 18, -15, -18, 14, 18, 14,
	-18, -14, 18, -14, -18, 13, 18, 13,
	-18, -13, 18, -13, -18, 12, 18, 12,
	-18, -12, 18, -12, -18, 11, 18, 11,
	-18, -11, 18, -11, -18, 10, 18, 10,
	-18, -10, 18, -10, -18, 9, 18, 9,
	-18, -9, 18, -9, -18, 8, 18, 8,
	-18, -8, 18, -8, -18, 7, 18, 7,
	-18, -7, 18, -7, -18, 6, 18, 6,
	-18, -6, 18, -6, -18, 5, 18, 5,
	-18, -5, 18, -5, -18, 4, 18, 4,
	-18, -4, 18, -4, -18, 3, 18, 3,
	-18, -3, 18, -3, -18, 2, 18, 2,
	-18, -2, 18, -2, -18, 1, 18, 1,
	-18, -1, 18, -1, -18, 0, 18, 0
};

/** pCrawlTable maps from circle radius to the X- and Y-coordinate deltas from the center of a circle. */
char *pCrawlTable[19] = {
	CrawlTable,
	CrawlTable + 3,
	CrawlTable + 12,
	CrawlTable + 45,
	CrawlTable + 94,
	CrawlTable + 159,
	CrawlTable + 240,
	CrawlTable + 337,
	CrawlTable + 450,
	CrawlTable + 579,
	CrawlTable + 724,
	CrawlTable + 885,
	CrawlTable + 1062,
	CrawlTable + 1255,
	CrawlTable + 1464,
	CrawlTable + 1689,
	CrawlTable + 1930,
	CrawlTable + 2187,
	CrawlTable + 2460
};
/** vCrawlTable specifies the X- Y-coordinate offsets of lighting visions. */
uint8_t vCrawlTable[23][30] = {
	{ 1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0, 8, 0, 9, 0, 10, 0, 11, 0, 12, 0, 13, 0, 14, 0, 15, 0 },
	{ 1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0, 8, 1, 9, 1, 10, 1, 11, 1, 12, 1, 13, 1, 14, 1, 15, 1 },
	{ 1, 0, 2, 0, 3, 0, 4, 1, 5, 1, 6, 1, 7, 1, 8, 1, 9, 1, 10, 1, 11, 1, 12, 2, 13, 2, 14, 2, 15, 2 },
	{ 1, 0, 2, 0, 3, 1, 4, 1, 5, 1, 6, 1, 7, 1, 8, 2, 9, 2, 10, 2, 11, 2, 12, 2, 13, 3, 14, 3, 15, 3 },
	{ 1, 0, 2, 1, 3, 1, 4, 1, 5, 1, 6, 2, 7, 2, 8, 2, 9, 3, 10, 3, 11, 3, 12, 3, 13, 4, 14, 4, 0, 0 },
	{ 1, 0, 2, 1, 3, 1, 4, 1, 5, 2, 6, 2, 7, 3, 8, 3, 9, 3, 10, 4, 11, 4, 12, 4, 13, 5, 14, 5, 0, 0 },
	{ 1, 0, 2, 1, 3, 1, 4, 2, 5, 2, 6, 3, 7, 3, 8, 3, 9, 4, 10, 4, 11, 5, 12, 5, 13, 6, 14, 6, 0, 0 },
	{ 1, 1, 2, 1, 3, 2, 4, 2, 5, 3, 6, 3, 7, 4, 8, 4, 9, 5, 10, 5, 11, 6, 12, 6, 13, 7, 0, 0, 0, 0 },
	{ 1, 1, 2, 1, 3, 2, 4, 2, 5, 3, 6, 4, 7, 4, 8, 5, 9, 6, 10, 6, 11, 7, 12, 7, 12, 8, 13, 8, 0, 0 },
	{ 1, 1, 2, 2, 3, 2, 4, 3, 5, 4, 6, 5, 7, 5, 8, 6, 9, 7, 10, 7, 10, 8, 11, 8, 12, 9, 0, 0, 0, 0 },
	{ 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 5, 7, 6, 8, 7, 9, 8, 10, 9, 11, 9, 11, 10, 0, 0, 0, 0, 0, 0 },
	{ 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 9, 11, 10, 11, 0, 0, 0, 0, 0, 0 },
	{ 1, 1, 2, 2, 2, 3, 3, 4, 4, 5, 5, 6, 5, 7, 6, 8, 7, 9, 7, 10, 8, 10, 8, 11, 9, 12, 0, 0, 0, 0 },
	{ 1, 1, 1, 2, 2, 3, 2, 4, 3, 5, 4, 6, 4, 7, 5, 8, 6, 9, 6, 10, 7, 11, 7, 12, 8, 12, 8, 13, 0, 0 },
	{ 1, 1, 1, 2, 2, 3, 2, 4, 3, 5, 3, 6, 4, 7, 4, 8, 5, 9, 5, 10, 6, 11, 6, 12, 7, 13, 0, 0, 0, 0 },
	{ 0, 1, 1, 2, 1, 3, 2, 4, 2, 5, 3, 6, 3, 7, 3, 8, 4, 9, 4, 10, 5, 11, 5, 12, 6, 13, 6, 14, 0, 0 },
	{ 0, 1, 1, 2, 1, 3, 1, 4, 2, 5, 2, 6, 3, 7, 3, 8, 3, 9, 4, 10, 4, 11, 4, 12, 5, 13, 5, 14, 0, 0 },
	{ 0, 1, 1, 2, 1, 3, 1, 4, 1, 5, 2, 6, 2, 7, 2, 8, 3, 9, 3, 10, 3, 11, 3, 12, 4, 13, 4, 14, 0, 0 },
	{ 0, 1, 0, 2, 1, 3, 1, 4, 1, 5, 1, 6, 1, 7, 2, 8, 2, 9, 2, 10, 2, 11, 2, 12, 3, 13, 3, 14, 3, 15 },
	{ 0, 1, 0, 2, 0, 3, 1, 4, 1, 5, 1, 6, 1, 7, 1, 8, 1, 9, 1, 10, 1, 11, 2, 12, 2, 13, 2, 14, 2, 15 },
	{ 0, 1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 1, 8, 1, 9, 1, 10, 1, 11, 1, 12, 1, 13, 1, 14, 1, 15 },
	{ 0, 1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0, 8, 0, 9, 0, 10, 0, 11, 0, 12, 0, 13, 0, 14, 0, 15 }
};
/** unused */
uint8_t byte_49463C[18][18] = {
	{ 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 },
	{ 0, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 },
	{ 0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 },
	{ 0, 0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 },
	{ 0, 0, 1, 1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 },
	{ 0, 0, 1, 1, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 },
	{ 0, 0, 0, 1, 1, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 },
	{ 0, 0, 0, 1, 1, 1, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 },
	{ 0, 0, 0, 0, 1, 1, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3 },
	{ 0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3 },
	{ 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3 },
	{ 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3 },
	{ 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3 },
	{ 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3 },
	{ 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3 },
	{ 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3 },
	{ 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2 }
};

/** RadiusAdj maps from vCrawlTable index to lighting vision radius adjustment. */
uint8_t RadiusAdj[23] = { 0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 4, 3, 2, 2, 2, 1, 1, 1, 0, 0, 0, 0 };

void RotateRadius(V2Di &pos, V2Di &d, V2Di &l, V2Di & b)
{
	int swap;

	b = { 0, 0 };

	swap = d.x;
	d.x = 7 - d.y;
	d.y = swap;
	swap = l.x;
	l.x = 7 - l.y;
	l.y = swap;

	pos = d - l;

	if (pos.x < 0) {
		pos.x += 8;
		b.x = 1;
	}
	if (pos.y < 0) {
		pos.y += 8;
		b.y = 1;
	}
}

void DoLighting(V2Di pos, int nRadius, int Lnum)
{
	int x, y, v, mult, radius_block;
	V2Di off, dist, light, block, temp, min, max;
	if (Lnum >= 0) {
		off = LightList[Lnum]._off;
		if (off.x < 0) {
			off.x += 8;
			pos.x--;
		}
		if (off.y < 0) {
			off.y += 8;
			pos.y--;
		}
	}

	dist = off;

	if (pos.x - 15 < 0) {
		min.x = pos.x + 1;
	} else {
		min.x = 15;
	}
	if (pos.x + 15 > MAXDUNX) {
		max.x = MAXDUNX - pos.x;
	} else {
		max.x = 15;
	}
	if (pos.y - 15 < 0) {
		min.y = pos.y + 1;
	} else {
		min.y = 15;
	}
	if (pos.y + 15 > MAXDUNY) {
		max.y = MAXDUNY - pos.y;
	} else {
		max.y = 15;
	}

	if (pos.x >= 0 && pos.x < MAXDUNX && pos.y >= 0 && pos.y < MAXDUNY) {
		grid[pos.x][pos.y].dLight = 0;
	}

	mult = off.x + 8 * off.y;
	for (y = 0; y < min.y; y++) {
		for (x = 1; x < max.x; x++) {
			radius_block = lightblock[mult][y][x];
			if (radius_block < 128) {
				temp.x = pos.x + x;
				temp.y = pos.y + y;
				v = lightradius[nRadius][radius_block];
				if (temp.x >= 0 && temp.x < MAXDUNX && temp.y >= 0 && temp.y < MAXDUNY) {
					if (v < grid[temp.x][temp.y].dLight) {
						grid[temp.x][temp.y].dLight = v;
					}
				}
			}
		}
	}
	RotateRadius(off, dist, light, block);
	mult = off.x + 8 * off.y;
	for (y = 0; y < max.y; y++) {
		for (x = 1; x < max.x; x++) {
			radius_block = lightblock[mult][y + block.y][x + block.x];
			if (radius_block < 128) {
				temp.x = pos.x + y;
				temp.y = pos.y - x;
				v = lightradius[nRadius][radius_block];
				if (temp.x >= 0 && temp.x < MAXDUNX && temp.y >= 0 && temp.y < MAXDUNY) {
					if (v < grid[temp.x][temp.y].dLight) {
						grid[temp.x][temp.y].dLight = v;
					}
				}
			}
		}
	}
	RotateRadius(off, dist, light, block);
	mult = off.x + 8 * off.y;
	for (y = 0; y < max.y; y++) {
		for (x = 1; x < min.x; x++) {
			radius_block = lightblock[mult][y + block.y][x + block.x];
			if (radius_block < 128) {
				temp.x = pos.x - x;
				temp.y = pos.y - y;
				v = lightradius[nRadius][radius_block];
				if (temp.x >= 0 && temp.x < MAXDUNX && temp.y >= 0 && temp.y < MAXDUNY) {
					if (v < grid[temp.x][temp.y].dLight) {
						grid[temp.x][temp.y].dLight = v;
					}
				}
			}
		}
	}
	RotateRadius(off, dist, light, block);
	mult = off.x + 8 * off.y;
	for (y = 0; y < min.y; y++) {
		for (x = 1; x < min.x; x++) {
			radius_block = lightblock[mult][y + block.y][x + block.x];
			if (radius_block < 128) {
				temp.x = pos.x - y;
				temp.y = pos.y + x;
				v = lightradius[nRadius][radius_block];
				if (temp.x >= 0 && temp.x < MAXDUNX && temp.y >= 0 && temp.y < MAXDUNY) {
					if (v < grid[temp.x][temp.y].dLight) {
						grid[temp.x][temp.y].dLight = v;
					}
				}
			}
		}
	}
}

void DoUnLight(V2Di pos, int nRadius)
{
	V2Di min, max;
	nRadius++;
	min.y = pos.y - nRadius;
	max.y = pos.y + nRadius;
	min.x = pos.x - nRadius;
	max.x = pos.x + nRadius;

	if (min.y < 0) min.y = 0;
	if (max.y > MAXDUNY) max.y = MAXDUNY;
	if (min.x < 0) min.x = 0;
	if (max.x > MAXDUNX) max.x = MAXDUNX;

	for (int y = min.y; y < max.y; y++) {
		for (int x = min.x; x < max.x; x++) {
			if (x >= 0 && x < MAXDUNX && y >= 0 && y < MAXDUNY)
				grid[x][y].dLight = grid[x][y].dPreLight;
		}
	}
}

void DoUnVision(V2Di pos, int nRadius)
{
	int i, j, x1, y1, x2, y2;
	nRadius++;
	y1 = pos.y - nRadius;
	y2 = pos.y + nRadius;
	x1 = pos.x - nRadius;
	x2 = pos.x + nRadius;

	if (y1 < 0) y1 = 0;
	if (y2 > MAXDUNY) y2 = MAXDUNY;
	if (x1 < 0) x1 = 0;
	if (x2 > MAXDUNX) x2 = MAXDUNX;

	for (i = x1; i < x2; i++) {
		for (j = y1; j < y2; j++) {
			grid[i][j].dFlags &= ~(BFLAG_VISIBLE | BFLAG_LIT);
		}
	}
}

void DoVision(V2Di pos, int nRadius, bool doautomap, bool visible)
{
	bool nBlockerFlag;
	int nCrawlX, nCrawlY, nLineLen, nTrans;
	int j, k, v, x1adj, x2adj, y1adj, y2adj;

	if (pos.x >= 0 && pos.x <= MAXDUNX && pos.y >= 0 && pos.y <= MAXDUNY) {
		if (doautomap) {
			if (grid[pos.x][pos.y].dFlags >= 0) {
				automap.SetAutomapView(pos); // Is this an error? Should be y?
			}
			grid.at(pos).dFlags |= BFLAG_EXPLORED;
		}
		if (visible) {
			grid.at(pos).dFlags |= BFLAG_LIT;
		}
		grid.at(pos).dFlags |= BFLAG_VISIBLE;
	}

	for (v = 0; v < 4; v++) {
		for (j = 0; j < 23; j++) {
			nBlockerFlag = false;
			nLineLen = 2 * (nRadius - RadiusAdj[j]);
			for (k = 0; k < nLineLen && !nBlockerFlag; k += 2) {
				x1adj = 0;
				x2adj = 0;
				y1adj = 0;
				y2adj = 0;
				switch (v) {
				case 0:
					nCrawlX = pos.x + vCrawlTable[j][k];
					nCrawlY = pos.y + vCrawlTable[j][k + 1];
					if (vCrawlTable[j][k] > 0 && vCrawlTable[j][k + 1] > 0) {
						x1adj = -1;
						y2adj = -1;
					}
					break;
				case 1:
					nCrawlX = pos.x - vCrawlTable[j][k];
					nCrawlY = pos.y - vCrawlTable[j][k + 1];
					if (vCrawlTable[j][k] > 0 && vCrawlTable[j][k + 1] > 0) {
						y1adj = 1;
						x2adj = 1;
					}
					break;
				case 2:
					nCrawlX = pos.x + vCrawlTable[j][k];
					nCrawlY = pos.y - vCrawlTable[j][k + 1];
					if (vCrawlTable[j][k] > 0 && vCrawlTable[j][k + 1] > 0) {
						x1adj = -1;
						y2adj = 1;
					}
					break;
				case 3:
					nCrawlX = pos.x - vCrawlTable[j][k];
					nCrawlY = pos.y + vCrawlTable[j][k + 1];
					if (vCrawlTable[j][k] > 0 && vCrawlTable[j][k + 1] > 0) {
						y1adj = -1;
						x2adj = 1;
					}
					break;
				}
				if (nCrawlX >= 0 && nCrawlX < MAXDUNX && nCrawlY >= 0 && nCrawlY < MAXDUNY) {
					nBlockerFlag = grid[nCrawlX][nCrawlY].blocksLight();
					if ((x1adj + nCrawlX >= 0 && x1adj + nCrawlX < MAXDUNX && y1adj + nCrawlY >= 0 && y1adj + nCrawlY < MAXDUNY
					        && !grid[x1adj + nCrawlX][y1adj + nCrawlY].blocksLight())
					    || (x2adj + nCrawlX >= 0 && x2adj + nCrawlX < MAXDUNX && y2adj + nCrawlY >= 0 && y2adj + nCrawlY < MAXDUNY
					        && !grid[x2adj + nCrawlX][y2adj + nCrawlY].blocksLight())) {
						if (doautomap) {
							if (grid[nCrawlX][nCrawlY].dFlags >= 0) {
								automap.SetAutomapView({ nCrawlX, nCrawlY });
							}
							grid[nCrawlX][nCrawlY].dFlags |= BFLAG_EXPLORED;
						}
						if (visible) {
							grid[nCrawlX][nCrawlY].dFlags |= BFLAG_LIT;
						}
						grid[nCrawlX][nCrawlY].dFlags |= BFLAG_VISIBLE;
						if (!nBlockerFlag) {
							nTrans = grid[nCrawlX][nCrawlY].dTransVal;
							if (nTrans != 0) {
								lvl.TransList[nTrans] = TRUE;
							}
						}
					}
				}
			}
		}
	}
}

void FreeLightTable()
{
	MemFreeDbg(pLightTbl);
}

void InitLightTable()
{
	assert(!pLightTbl);
	pLightTbl = DiabloAllocPtr(LIGHTSIZE);
}

void MakeLightTable()
{
	int i, j, k, l, lights, shade, l1, l2, cnt, rem, div;
	double fs, fa;
	uint8_t col, max;
	uint8_t *tbl, *trn;
	uint8_t blood[16];

	tbl = pLightTbl;
	shade = 0;

	if (light4flag) {
		lights = 3;
	} else {
		lights = 15;
	}

	for (i = 0; i < lights; i++) {
		*tbl++ = 0;
		for (j = 0; j < 8; j++) {
			col = 16 * j + shade;
			max = 16 * j + 15;
			for (k = 0; k < 16; k++) {
				if (k != 0 || j != 0) {
					*tbl++ = col;
				}
				if (col < max) {
					col++;
				} else {
					max = 0;
					col = 0;
				}
			}
		}
		for (j = 16; j < 20; j++) {
			col = 8 * j + (shade >> 1);
			max = 8 * j + 7;
			for (k = 0; k < 8; k++) {
				*tbl++ = col;
				if (col < max) {
					col++;
				} else {
					max = 0;
					col = 0;
				}
			}
		}
		for (j = 10; j < 16; j++) {
			col = 16 * j + shade;
			max = 16 * j + 15;
			for (k = 0; k < 16; k++) {
				*tbl++ = col;
				if (col < max) {
					col++;
				} else {
					max = 0;
					col = 0;
				}
				if (col == 255) {
					max = 0;
					col = 0;
				}
			}
		}
		if (light4flag) {
			shade += 5;
		} else {
			shade++;
		}
	}

	for (i = 0; i < 256; i++) {
		*tbl++ = 0;
	}

	if (lvl.type() == DunType::hell) {
		tbl = pLightTbl;
		for (i = 0; i < lights; i++) {
			l1 = lights - i;
			l2 = l1;
			div = lights / l1;
			rem = lights % l1;
			cnt = 0;
			blood[0] = 0;
			col = 1;
			for (j = 1; j < 16; j++) {
				blood[j] = col;
				l2 += rem;
				if (l2 > l1 && j < 15) {
					j++;
					blood[j] = col;
					l2 -= l1;
				}
				cnt++;
				if (cnt == div) {
					col++;
					cnt = 0;
				}
			}
			*tbl++ = 0;
			for (j = 1; j <= 15; j++) {
				*tbl++ = blood[j];
			}
			for (j = 15; j > 0; j--) {
				*tbl++ = blood[j];
			}
			*tbl++ = 1;
			tbl += 224;
		}
		*tbl++ = 0;
		for (j = 0; j < 31; j++) {
			*tbl++ = 1;
		}
		tbl += 224;
	}

	trn = LoadFileInMem("PlrGFX\\Infra.TRN", NULL);
	for (i = 0; i < 256; i++) {
		*tbl++ = trn[i];
	}
	mem_free_dbg(trn);

	trn = LoadFileInMem("PlrGFX\\Stone.TRN", NULL);
	for (i = 0; i < 256; i++) {
		*tbl++ = trn[i];
	}
	mem_free_dbg(trn);

	for (i = 0; i < 8; i++) {
		for (col = 226; col < 239; col++) {
			if (i != 0 || col != 226) {
				*tbl++ = col;
			} else {
				*tbl++ = 0;
			}
		}
		*tbl++ = 0;
		*tbl++ = 0;
		*tbl++ = 0;
	}
	for (i = 0; i < 4; i++) {
		col = 224;
		for (j = 224; j < 239; j += 2) {
			*tbl++ = col;
			col += 2;
		}
	}
	for (i = 0; i < 6; i++) {
		for (col = 224; col < 239; col++) {
			*tbl++ = col;
		}
		*tbl++ = 0;
	}

	for (k = 0; k < 16; k++) {
		for (l = 0; l < 128; l++) {
			if (l > (k + 1) * 8) {
				lightradius[k][l] = 15;
			} else {
				lightradius[k][l] = l * 15.0 / ((k + 1) * 8.0) + 0.5;
			}
		}
	}

	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			for (k = 0; k < 16; k++) {
				for (l = 0; l < 16; l++) {
					fs = (uint8_t)sqrt((double)(8 * l - j) * (8 * l - j) + (8 * k - i) * (8 * k - i));
					if (fs < 0.0) {
						fa = -0.5;
					} else {
						fa = 0.5;
					}
					lightblock[i * 8 + j][k][l] = fs + fa;
				}
			}
		}
	}
}

#ifdef _DEBUG
void ToggleLighting()
{
	int i;

	lightflag ^= TRUE;

	if (lightflag != 0) {
		grid.clearLight();
		//memset(dLight, 0, sizeof(dLight));
	} else {
		//memcpy(dLight, dPreLight, sizeof(dLight));
		grid.prelightToLight();
		for (i = 0; i < MAX_PLRS; i++) {
			if (plr[i].data.plractive && plr[i].data.plrlevel == lvl.currlevel) {
				DoLighting(plr[i].pos(), plr[i].data._pLightRad, -1);
			}
		}
	}
}
#endif

void InitLightMax()
{
	if (light4flag) {
		lightmax = 3;
	} else {
		lightmax = 15;
	}
}

void InitLighting()
{
	int i;

	numlights = 0;
	dolighting = false;
	lightflag = false;

	for (i = 0; i < MAXLIGHTS; i++) {
		lightactive[i] = i;
	}
}

int AddLight(V2Di pos, int r)
{
	if (lightflag != 0) return -1;
	int lid = -1;

	if (numlights < MAXLIGHTS) {
		lid = lightactive[numlights++];
		LightList[lid]._l = pos;
		LightList[lid]._lradius = r;
		LightList[lid]._off = { 0, 0 };
		LightList[lid]._ldel = false;
		LightList[lid]._lunflag = false;
		dolighting = TRUE;
	}
	return lid;
}

void AddUnLight(int i)
{
	if (lightflag || i == -1) {
		return;
	}

	LightList[i]._ldel = TRUE;
	dolighting = TRUE;
}

void ChangeLightRadius(int i, int r)
{
	if (lightflag || i == -1) return;
	LightList[i]._lunflag = TRUE;
	LightList[i]._lun = LightList[i]._l;
	LightList[i]._lunr = LightList[i]._lradius;
	LightList[i]._lradius = r;
	dolighting = TRUE;
}

void ChangeLightXY(int i, V2Di pos)
{
	if (lightflag || i == -1) return;
	LightList[i]._lunflag = TRUE;
	LightList[i]._lun = LightList[i]._l;
	LightList[i]._lunr = LightList[i]._lradius;
	LightList[i]._l = pos;
	dolighting = TRUE;
}

void ChangeLightOff(int i, V2Di pos)
{
	if (lightflag || i == -1) return;
	LightList[i]._lunflag = TRUE;
	LightList[i]._lun = LightList[i]._l;
	LightList[i]._lunr = LightList[i]._lradius;
	LightList[i]._off = pos;
	dolighting = TRUE;
}

void ChangeLight(int i, V2Di pos, int r)
{
	if (lightflag || i == -1) return;
	LightList[i]._lunflag = TRUE;
	LightList[i]._lun = LightList[i]._l;
	LightList[i]._lunr = LightList[i]._lradius;
	LightList[i]._l = pos;
	LightList[i]._lradius = r;
	dolighting = TRUE;
}

void ProcessLightList()
{
	int i, j;
	uint8_t temp;

	if (lightflag != 0) {
		return;
	}

	if (dolighting) {
		for (i = 0; i < numlights; i++) {
			j = lightactive[i];
			if (LightList[j]._ldel) {
				DoUnLight(LightList[j]._l, LightList[j]._lradius);
			}
			if (LightList[j]._lunflag) {
				DoUnLight(LightList[j]._lun, LightList[j]._lunr);
				LightList[j]._lunflag = false;
			}
		}
		for (i = 0; i < numlights; i++) {
			j = lightactive[i];
			if (!LightList[j]._ldel) {
				DoLighting(LightList[j]._l, LightList[j]._lradius, j);
			}
		}
		i = 0;
		while (i < numlights) {
			if (LightList[lightactive[i]]._ldel) {
				numlights--;
				temp = lightactive[numlights];
				lightactive[numlights] = lightactive[i];
				lightactive[i] = temp;
			} else {
				i++;
			}
		}
	}

	dolighting = false;
}

void SavePreLighting()
{
	grid.lightToPrelight();
	//memcpy(dPreLight, dLight, sizeof(dPreLight));
}

void InitVision()
{
	int i;

	numvision = 0;
	dovision = false;
	visionid = 1;

	for (i = 0; i < lvl.TransVal; i++) {
		lvl.TransList[i] = false;
	}
}

int AddVision(V2Di pos, int r, bool mine)
{
	int vid; // BUGFIX: if numvision >= MAXVISION behavior is undefined
	if (numvision < MAXVISION) {
		VisionList[numvision]._l = pos;
		VisionList[numvision]._lradius = r;
		vid = visionid++;
		VisionList[numvision]._lid = vid;
		VisionList[numvision]._ldel = 0;
		VisionList[numvision]._lunflag = 0;
		VisionList[numvision]._lflags = mine != 0;
		numvision++;
		dovision = TRUE;
	}

	return vid;
}

void ChangeVisionRadius(int id, int r)
{
	int i;

	for (i = 0; i < numvision; i++) {
		if (VisionList[i]._lid == id) {
			VisionList[i]._lunflag = TRUE;
			VisionList[i]._lun = VisionList[i]._l;
			VisionList[i]._lunr = VisionList[i]._lradius;
			VisionList[i]._lradius = r;
			dovision = TRUE;
		}
	}
}

void ChangeVisionXY(int id, V2Di pos)
{
	for (int i = 0; i < numvision; i++) {
		if (VisionList[i]._lid == id) {
			VisionList[i]._lunflag = TRUE;
			VisionList[i]._lun = VisionList[i]._l;
			VisionList[i]._lunr = VisionList[i]._lradius;
			VisionList[i]._l = pos;
			dovision = TRUE;
		}
	}
}

void ProcessVisionList()
{
	int i;
	bool delflag;

	if (dovision) {
		for (i = 0; i < numvision; i++) {
			if (VisionList[i]._ldel) {
				DoUnVision(VisionList[i]._l, VisionList[i]._lradius);
			}
			if (VisionList[i]._lunflag) {
				DoUnVision(VisionList[i]._lun, VisionList[i]._lunr);
				VisionList[i]._lunflag = false;
			}
		}
		for (i = 0; i < lvl.TransVal; i++) {
			lvl.TransList[i] = false;
		}
		for (i = 0; i < numvision; i++) {
			if (!VisionList[i]._ldel) {
				DoVision(
				    VisionList[i]._l,
				    VisionList[i]._lradius,
				    VisionList[i]._lflags & 1,
				    VisionList[i]._lflags & 1);
			}
		}
		do {
			delflag = false;
			for (i = 0; i < numvision; i++) {
				if (VisionList[i]._ldel) {
					numvision--;
					if (numvision > 0 && i != numvision) {
						VisionList[i] = VisionList[numvision];
					}
					delflag = TRUE;
				}
			}
		} while (delflag);
	}

	dovision = false;
}

void lighting_color_cycling()
{
	int i, j, l;
	uint8_t col;
	uint8_t *tbl;

	l = light4flag ? 4 : 16;

	if (lvl.type() != DunType::hell) {
		return;
	}

	tbl = pLightTbl;

	for (j = 0; j < l; j++) {
		tbl++;
		col = *tbl;
		for (i = 0; i < 30; i++) {
			tbl[0] = tbl[1];
			tbl++;
		}
		*tbl = col;
		tbl += 225;
	}
}

DEVILUTION_END_NAMESPACE
