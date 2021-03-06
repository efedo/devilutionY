/**
 * @file drlg_l2.cpp
 *
 * Implementation of the catacombs level generation algorithms.
 */

#include <algorithm>

#include "all.h"

namespace dvl {

int nSx1;
int nSy1;
int nSx2;
int nSy2;
int nRoomCnt;
uint8_t predungeon[DMAXX][DMAXY];
ROOMNODE RoomList[81];
HALLNODE *pHallList;

int Area_Min = 2;
int Room_Max = 10;
int Room_Min = 4;
int Dir_Xadd[5] = { 0, 0, 1, 0, -1 };
int Dir_Yadd[5] = { 0, -1, 0, 1, 0 };
ShadowStruct SPATSL2[2] = { { 6, 3, 0, 3, 48, 0, 50 }, { 9, 3, 0, 3, 48, 0, 50 } };
//short word_48489A = 0;
uint8_t BTYPESL2[161] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0, 0, 17, 18, 1, 1, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 2, 2, 2, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 3, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
uint8_t BSTYPESL2[161] = { 0, 1, 2, 3, 0, 0, 6, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 6, 6, 6, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 2, 2, 2, 0, 0, 0, 1, 1, 1, 1, 6, 2, 2, 2, 0, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 2, 2, 3, 3, 3, 3, 1, 1, 2, 2, 3, 3, 3, 3, 1, 1, 3, 3, 2, 2, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
uint8_t VARCH1[] = { 2, 4, 3, 0, 3, 1, 3, 4, 0, 7, 48, 0, 51, 39, 47, 44, 0, 0 };
uint8_t VARCH2[] = { 2, 4, 3, 0, 3, 1, 3, 4, 0, 8, 48, 0, 51, 39, 47, 44, 0, 0 };
uint8_t VARCH3[] = { 2, 4, 3, 0, 3, 1, 3, 4, 0, 6, 48, 0, 51, 39, 47, 44, 0, 0 };
uint8_t VARCH4[] = { 2, 4, 3, 0, 3, 1, 3, 4, 0, 9, 48, 0, 51, 39, 47, 44, 0, 0 };
uint8_t VARCH5[] = { 2, 4, 3, 0, 3, 1, 3, 4, 0, 14, 48, 0, 51, 39, 47, 44, 0, 0 };
uint8_t VARCH6[] = { 2, 4, 3, 0, 3, 1, 3, 4, 0, 13, 48, 0, 51, 39, 47, 44, 0, 0 };
uint8_t VARCH7[] = { 2, 4, 3, 0, 3, 1, 3, 4, 0, 16, 48, 0, 51, 39, 47, 44, 0, 0 };
uint8_t VARCH8[] = { 2, 4, 3, 0, 3, 1, 3, 4, 0, 15, 48, 0, 51, 39, 47, 44, 0, 0 };
uint8_t VARCH9[] = { 2, 4, 3, 0, 3, 8, 3, 4, 0, 7, 48, 0, 51, 42, 47, 44, 0, 0 };
uint8_t VARCH10[] = { 2, 4, 3, 0, 3, 8, 3, 4, 0, 8, 48, 0, 51, 42, 47, 44, 0, 0 };
uint8_t VARCH11[] = { 2, 4, 3, 0, 3, 8, 3, 4, 0, 6, 48, 0, 51, 42, 47, 44, 0, 0 };
uint8_t VARCH12[] = { 2, 4, 3, 0, 3, 8, 3, 4, 0, 9, 48, 0, 51, 42, 47, 44, 0, 0 };
uint8_t VARCH13[] = { 2, 4, 3, 0, 3, 8, 3, 4, 0, 14, 48, 0, 51, 42, 47, 44, 0, 0 };
uint8_t VARCH14[] = { 2, 4, 3, 0, 3, 8, 3, 4, 0, 13, 48, 0, 51, 42, 47, 44, 0, 0 };
uint8_t VARCH15[] = { 2, 4, 3, 0, 3, 8, 3, 4, 0, 16, 48, 0, 51, 42, 47, 44, 0, 0 };
uint8_t VARCH16[] = { 2, 4, 3, 0, 3, 8, 3, 4, 0, 15, 48, 0, 51, 42, 47, 44, 0, 0 };
uint8_t VARCH17[] = { 2, 3, 2, 7, 3, 4, 0, 7, 141, 39, 47, 44, 0, 0 };
uint8_t VARCH18[] = { 2, 3, 2, 7, 3, 4, 0, 8, 141, 39, 47, 44, 0, 0 };
uint8_t VARCH19[] = { 2, 3, 2, 7, 3, 4, 0, 6, 141, 39, 47, 44, 0, 0 };
uint8_t VARCH20[] = { 2, 3, 2, 7, 3, 4, 0, 9, 141, 39, 47, 44, 0, 0 };
uint8_t VARCH21[] = { 2, 3, 2, 7, 3, 4, 0, 14, 141, 39, 47, 44, 0, 0 };
uint8_t VARCH22[] = { 2, 3, 2, 7, 3, 4, 0, 13, 141, 39, 47, 44, 0, 0 };
uint8_t VARCH23[] = { 2, 3, 2, 7, 3, 4, 0, 16, 141, 39, 47, 44, 0, 0 };
uint8_t VARCH24[] = { 2, 3, 2, 7, 3, 4, 0, 15, 141, 39, 47, 44, 0, 0 };
uint8_t VARCH25[] = { 2, 4, 3, 0, 3, 4, 3, 1, 0, 7, 48, 0, 51, 39, 47, 44, 0, 0 };
uint8_t VARCH26[] = { 2, 4, 3, 0, 3, 4, 3, 1, 0, 8, 48, 0, 51, 39, 47, 44, 0, 0 };
uint8_t VARCH27[] = { 2, 4, 3, 0, 3, 4, 3, 1, 0, 6, 48, 0, 51, 39, 47, 44, 0, 0 };
uint8_t VARCH28[] = { 2, 4, 3, 0, 3, 4, 3, 1, 0, 9, 48, 0, 51, 39, 47, 44, 0, 0 };
uint8_t VARCH29[] = { 2, 4, 3, 0, 3, 4, 3, 1, 0, 14, 48, 0, 51, 39, 47, 44, 0, 0 };
uint8_t VARCH30[] = { 2, 4, 3, 0, 3, 4, 3, 1, 0, 13, 48, 0, 51, 39, 47, 44, 0, 0 };
uint8_t VARCH31[] = { 2, 4, 3, 0, 3, 4, 3, 1, 0, 16, 48, 0, 51, 39, 47, 44, 0, 0 };
uint8_t VARCH32[] = { 2, 4, 3, 0, 3, 4, 3, 1, 0, 15, 48, 0, 51, 39, 47, 44, 0, 0 };
uint8_t VARCH33[] = { 2, 4, 2, 0, 3, 8, 3, 4, 0, 7, 142, 0, 51, 42, 47, 44, 0, 0 };
uint8_t VARCH34[] = { 2, 4, 2, 0, 3, 8, 3, 4, 0, 8, 142, 0, 51, 42, 47, 44, 0, 0 };
uint8_t VARCH35[] = { 2, 4, 2, 0, 3, 8, 3, 4, 0, 6, 142, 0, 51, 42, 47, 44, 0, 0 };
uint8_t VARCH36[] = { 2, 4, 2, 0, 3, 8, 3, 4, 0, 9, 142, 0, 51, 42, 47, 44, 0, 0 };
uint8_t VARCH37[] = { 2, 4, 2, 0, 3, 8, 3, 4, 0, 14, 142, 0, 51, 42, 47, 44, 0, 0 };
uint8_t VARCH38[] = { 2, 4, 2, 0, 3, 8, 3, 4, 0, 13, 142, 0, 51, 42, 47, 44, 0, 0 };
uint8_t VARCH39[] = { 2, 4, 2, 0, 3, 8, 3, 4, 0, 16, 142, 0, 51, 42, 47, 44, 0, 0 };
uint8_t VARCH40[] = { 2, 4, 2, 0, 3, 8, 3, 4, 0, 15, 142, 0, 51, 42, 47, 44, 0, 0 };
uint8_t HARCH1[] = { 3, 2, 3, 3, 0, 2, 5, 9, 49, 46, 0, 40, 45, 0 };
uint8_t HARCH2[] = { 3, 2, 3, 3, 0, 2, 5, 6, 49, 46, 0, 40, 45, 0 };
uint8_t HARCH3[] = { 3, 2, 3, 3, 0, 2, 5, 8, 49, 46, 0, 40, 45, 0 };
uint8_t HARCH4[] = { 3, 2, 3, 3, 0, 2, 5, 7, 49, 46, 0, 40, 45, 0 };
uint8_t HARCH5[] = { 3, 2, 3, 3, 0, 2, 5, 15, 49, 46, 0, 40, 45, 0 };
uint8_t HARCH6[] = { 3, 2, 3, 3, 0, 2, 5, 16, 49, 46, 0, 40, 45, 0 };
uint8_t HARCH7[] = { 3, 2, 3, 3, 0, 2, 5, 13, 49, 46, 0, 40, 45, 0 };
uint8_t HARCH8[] = { 3, 2, 3, 3, 0, 2, 5, 14, 49, 46, 0, 40, 45, 0 };
uint8_t HARCH9[] = { 3, 2, 3, 3, 0, 8, 5, 9, 49, 46, 0, 43, 45, 0 };
uint8_t HARCH10[] = { 3, 2, 3, 3, 0, 8, 5, 6, 49, 46, 0, 43, 45, 0 };
uint8_t HARCH11[] = { 3, 2, 3, 3, 0, 8, 5, 8, 49, 46, 0, 43, 45, 0 };
uint8_t HARCH12[] = { 3, 2, 3, 3, 0, 8, 5, 7, 49, 46, 0, 43, 45, 0 };
uint8_t HARCH13[] = { 3, 2, 3, 3, 0, 8, 5, 15, 49, 46, 0, 43, 45, 0 };
uint8_t HARCH14[] = { 3, 2, 3, 3, 0, 8, 5, 16, 49, 46, 0, 43, 45, 0 };
uint8_t HARCH15[] = { 3, 2, 3, 3, 0, 8, 5, 13, 49, 46, 0, 43, 45, 0 };
uint8_t HARCH16[] = { 3, 2, 3, 3, 0, 8, 5, 14, 49, 46, 0, 43, 45, 0 };
uint8_t HARCH17[] = { 3, 2, 1, 3, 0, 8, 5, 9, 140, 46, 0, 43, 45, 0 };
uint8_t HARCH18[] = { 3, 2, 1, 3, 0, 8, 5, 6, 140, 46, 0, 43, 45, 0 };
uint8_t HARCH19[] = { 3, 2, 1, 3, 0, 8, 5, 8, 140, 46, 0, 43, 45, 0 };
uint8_t HARCH20[] = { 3, 2, 1, 3, 0, 8, 5, 7, 140, 46, 0, 43, 45, 0 };
uint8_t HARCH21[] = { 3, 2, 1, 3, 0, 8, 5, 15, 140, 46, 0, 43, 45, 0 };
uint8_t HARCH22[] = { 3, 2, 1, 3, 0, 8, 5, 16, 140, 46, 0, 43, 45, 0 };
uint8_t HARCH23[] = { 3, 2, 1, 3, 0, 8, 5, 13, 140, 46, 0, 43, 45, 0 };
uint8_t HARCH24[] = { 3, 2, 1, 3, 0, 8, 5, 14, 140, 46, 0, 43, 45, 0 };
uint8_t HARCH25[] = { 3, 2, 3, 3, 0, 5, 2, 9, 49, 46, 0, 40, 45, 0 };
uint8_t HARCH26[] = { 3, 2, 3, 3, 0, 5, 2, 6, 49, 46, 0, 40, 45, 0 };
uint8_t HARCH27[] = { 3, 2, 3, 3, 0, 5, 2, 8, 49, 46, 0, 40, 45, 0 };
uint8_t HARCH28[] = { 3, 2, 3, 3, 0, 5, 2, 7, 49, 46, 0, 40, 45, 0 };
uint8_t HARCH29[] = { 3, 2, 3, 3, 0, 5, 2, 15, 49, 46, 0, 40, 45, 0 };
uint8_t HARCH30[] = { 3, 2, 3, 3, 0, 5, 2, 16, 49, 46, 0, 40, 45, 0 };
uint8_t HARCH31[] = { 3, 2, 3, 3, 0, 5, 2, 13, 49, 46, 0, 40, 45, 0 };
uint8_t HARCH32[] = { 3, 2, 3, 3, 0, 5, 2, 14, 49, 46, 0, 40, 45, 0 };
uint8_t HARCH33[] = { 3, 2, 1, 3, 0, 9, 5, 9, 140, 46, 0, 40, 45, 0 };
uint8_t HARCH34[] = { 3, 2, 1, 3, 0, 9, 5, 6, 140, 46, 0, 40, 45, 0 };
uint8_t HARCH35[] = { 3, 2, 1, 3, 0, 9, 5, 8, 140, 46, 0, 40, 45, 0 };
uint8_t HARCH36[] = { 3, 2, 1, 3, 0, 9, 5, 7, 140, 46, 0, 40, 45, 0 };
uint8_t HARCH37[] = { 3, 2, 1, 3, 0, 9, 5, 15, 140, 46, 0, 40, 45, 0 };
uint8_t HARCH38[] = { 3, 2, 1, 3, 0, 9, 5, 16, 140, 46, 0, 40, 45, 0 };
uint8_t HARCH39[] = { 3, 2, 1, 3, 0, 9, 5, 13, 140, 46, 0, 40, 45, 0 };
uint8_t HARCH40[] = { 3, 2, 1, 3, 0, 9, 5, 14, 140, 46, 0, 40, 45, 0 };
uint8_t USTAIRS[] = { 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 72, 77, 0, 0, 76, 0, 0, 0, 0, 0, 0 };
uint8_t DSTAIRS[] = { 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 48, 71, 0, 0, 50, 78, 0, 0, 0, 0, 0 };
uint8_t WARPSTAIRS[] = { 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 158, 160, 0, 0, 159, 0, 0, 0, 0, 0, 0 };
uint8_t CRUSHCOL[] = { 3, 3, 3, 1, 3, 2, 6, 3, 3, 3, 3, 0, 0, 0, 0, 83, 0, 0, 0, 0 };
uint8_t BIG1[] = { 2, 2, 3, 3, 3, 3, 113, 0, 112, 0 };
uint8_t BIG2[] = { 2, 2, 3, 3, 3, 3, 114, 115, 0, 0 };
uint8_t BIG3[] = { 1, 2, 1, 1, 117, 116 };
uint8_t BIG4[] = { 2, 1, 2, 2, 118, 119 };
uint8_t BIG5[] = { 2, 2, 3, 3, 3, 3, 120, 122, 121, 123 };
uint8_t BIG6[] = { 1, 2, 1, 1, 125, 124 };
uint8_t BIG7[] = { 2, 1, 2, 2, 126, 127 };
uint8_t BIG8[] = { 2, 2, 3, 3, 3, 3, 128, 130, 129, 131 };
uint8_t BIG9[] = { 2, 2, 1, 3, 1, 3, 133, 135, 132, 134 };
uint8_t BIG10[] = { 2, 2, 2, 2, 3, 3, 136, 137, 3, 3 };
uint8_t RUINS1[] = { 1, 1, 1, 80 };
uint8_t RUINS2[] = { 1, 1, 1, 81 };
uint8_t RUINS3[] = { 1, 1, 1, 82 };
uint8_t RUINS4[] = { 1, 1, 2, 84 };
uint8_t RUINS5[] = { 1, 1, 2, 85 };
uint8_t RUINS6[] = { 1, 1, 2, 86 };
uint8_t RUINS7[] = { 1, 1, 8, 87 };
uint8_t PANCREAS1[] = { 5, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 108, 0, 0, 0, 0, 0, 0, 0 };
uint8_t PANCREAS2[] = { 5, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 110, 0, 0, 0, 0, 0, 0, 0 };
uint8_t CTRDOOR1[] = { 3, 3, 3, 1, 3, 0, 4, 0, 0, 9, 0, 0, 4, 0, 0, 1, 0, 0, 0, 0 };
uint8_t CTRDOOR2[] = { 3, 3, 3, 1, 3, 0, 4, 0, 0, 8, 0, 0, 4, 0, 0, 1, 0, 0, 0, 0 };
uint8_t CTRDOOR3[] = { 3, 3, 3, 1, 3, 0, 4, 0, 0, 6, 0, 0, 4, 0, 0, 1, 0, 0, 0, 0 };
uint8_t CTRDOOR4[] = { 3, 3, 3, 1, 3, 0, 4, 0, 0, 7, 0, 0, 4, 0, 0, 1, 0, 0, 0, 0 };
uint8_t CTRDOOR5[] = { 3, 3, 3, 1, 3, 0, 4, 0, 0, 15, 0, 0, 4, 0, 0, 1, 0, 0, 0, 0 };
uint8_t CTRDOOR6[] = { 3, 3, 3, 1, 3, 0, 4, 0, 0, 13, 0, 0, 4, 0, 0, 1, 0, 0, 0, 0 };
uint8_t CTRDOOR7[] = { 3, 3, 3, 1, 3, 0, 4, 0, 0, 16, 0, 0, 4, 0, 0, 1, 0, 0, 0, 0 };
uint8_t CTRDOOR8[] = { 3, 3, 3, 1, 3, 0, 4, 0, 0, 14, 0, 0, 4, 0, 0, 1, 0, 0, 0, 0 };
int Patterns[100][10] = {
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 3 },
	{ 0, 0, 0, 0, 2, 0, 0, 0, 0, 3 },
	{ 0, 7, 0, 0, 1, 0, 0, 5, 0, 2 },
	{ 0, 5, 0, 0, 1, 0, 0, 7, 0, 2 },
	{ 0, 0, 0, 7, 1, 5, 0, 0, 0, 1 },
	{ 0, 0, 0, 5, 1, 7, 0, 0, 0, 1 },
	{ 0, 1, 0, 0, 3, 0, 0, 1, 0, 4 },
	{ 0, 0, 0, 1, 3, 1, 0, 0, 0, 5 },
	{ 0, 6, 0, 6, 1, 0, 0, 0, 0, 6 },
	{ 0, 6, 0, 0, 1, 6, 0, 0, 0, 9 },
	{ 0, 0, 0, 6, 1, 0, 0, 6, 0, 7 },
	{ 0, 0, 0, 0, 1, 6, 0, 6, 0, 8 },
	{ 0, 6, 0, 6, 6, 0, 8, 6, 0, 7 },
	{ 0, 6, 8, 6, 6, 6, 0, 0, 0, 9 },
	{ 0, 6, 0, 0, 6, 6, 0, 6, 8, 8 },
	{ 6, 6, 6, 6, 6, 6, 0, 6, 0, 8 },
	{ 2, 6, 6, 6, 6, 6, 0, 6, 0, 8 },
	{ 7, 7, 7, 6, 6, 6, 0, 6, 0, 8 },
	{ 6, 6, 2, 6, 6, 6, 0, 6, 0, 8 },
	{ 6, 2, 6, 6, 6, 6, 0, 6, 0, 8 },
	{ 2, 6, 6, 6, 6, 6, 0, 6, 0, 8 },
	{ 6, 7, 7, 6, 6, 6, 0, 6, 0, 8 },
	{ 4, 4, 6, 6, 6, 6, 2, 6, 2, 8 },
	{ 2, 2, 2, 2, 6, 2, 2, 6, 2, 7 },
	{ 2, 2, 2, 2, 6, 2, 6, 6, 6, 7 },
	{ 2, 2, 6, 2, 6, 6, 2, 2, 6, 9 },
	{ 2, 6, 2, 2, 6, 2, 2, 2, 2, 6 },
	{ 2, 2, 2, 2, 6, 6, 2, 2, 2, 9 },
	{ 2, 2, 2, 6, 6, 2, 2, 2, 2, 6 },
	{ 2, 2, 0, 2, 6, 6, 2, 2, 0, 9 },
	{ 0, 0, 0, 0, 4, 0, 0, 0, 0, 12 },
	{ 0, 1, 0, 0, 1, 4, 0, 1, 0, 10 },
	{ 0, 0, 0, 1, 1, 1, 0, 4, 0, 11 },
	{ 0, 0, 0, 6, 1, 4, 0, 1, 0, 14 },
	{ 0, 6, 0, 1, 1, 0, 0, 4, 0, 16 },
	{ 0, 6, 0, 0, 1, 1, 0, 4, 0, 15 },
	{ 0, 0, 0, 0, 1, 1, 0, 1, 4, 13 },
	{ 8, 8, 8, 8, 1, 1, 0, 1, 1, 13 },
	{ 8, 8, 4, 8, 1, 1, 0, 1, 1, 10 },
	{ 0, 0, 0, 1, 1, 1, 1, 1, 1, 11 },
	{ 1, 1, 1, 1, 1, 1, 2, 2, 8, 2 },
	{ 0, 1, 0, 1, 1, 4, 1, 1, 0, 16 },
	{ 0, 0, 0, 1, 1, 1, 1, 1, 4, 11 },
	{ 1, 1, 4, 1, 1, 1, 0, 2, 2, 2 },
	{ 1, 1, 1, 1, 1, 1, 6, 2, 6, 2 },
	{ 4, 1, 1, 1, 1, 1, 6, 2, 6, 2 },
	{ 2, 2, 2, 1, 1, 1, 4, 1, 1, 11 },
	{ 4, 1, 1, 1, 1, 1, 2, 2, 2, 2 },
	{ 1, 1, 4, 1, 1, 1, 2, 2, 1, 2 },
	{ 4, 1, 1, 1, 1, 1, 1, 2, 2, 2 },
	{ 2, 2, 6, 1, 1, 1, 4, 1, 1, 11 },
	{ 4, 1, 1, 1, 1, 1, 2, 2, 6, 2 },
	{ 1, 2, 2, 1, 1, 1, 4, 1, 1, 11 },
	{ 0, 1, 1, 0, 1, 1, 0, 1, 1, 10 },
	{ 2, 1, 1, 3, 1, 1, 2, 1, 1, 14 },
	{ 1, 1, 0, 1, 1, 2, 1, 1, 0, 1 },
	{ 0, 4, 0, 1, 1, 1, 0, 1, 1, 14 },
	{ 4, 1, 0, 1, 1, 0, 1, 1, 0, 1 },
	{ 0, 1, 0, 4, 1, 1, 0, 1, 1, 15 },
	{ 1, 1, 1, 1, 1, 1, 0, 2, 2, 2 },
	{ 0, 1, 1, 2, 1, 1, 2, 1, 4, 10 },
	{ 2, 1, 1, 1, 1, 1, 0, 4, 0, 16 },
	{ 1, 1, 4, 1, 1, 2, 0, 1, 2, 1 },
	{ 2, 1, 1, 2, 1, 1, 1, 1, 4, 10 },
	{ 1, 1, 2, 1, 1, 2, 4, 1, 8, 1 },
	{ 2, 1, 4, 1, 1, 1, 4, 4, 1, 16 },
	{ 2, 1, 1, 1, 1, 1, 1, 1, 1, 16 },
	{ 1, 1, 2, 1, 1, 1, 1, 1, 1, 15 },
	{ 1, 1, 1, 1, 1, 1, 2, 1, 1, 14 },
	{ 4, 1, 1, 1, 1, 1, 2, 1, 1, 14 },
	{ 1, 1, 1, 1, 1, 1, 1, 1, 2, 8 },
	{ 0, 0, 0, 0, 255, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

LvlCatacombs::LvlCatacombs()
    : Level(DunType::catacombs)
{
	_automapFile = "Levels\\L2Data\\L2.AMP";
}

void LvlCatacombs::create(int lvldir)
{
	CreateL2Dungeon(glSeedTbl[lvl.currlevel], lvldir);
	InitL2Triggers();
	Freeupstairs();
	LoadRndLvlPal(DunType::catacombs);
};

void LvlCatacombs::loadGFX()
{
	assert(!pDungeonCels);
	pDungeonCels = LoadFileInMem("Levels\\L2Data\\L2.CEL", NULL);
	pMegaTiles = LoadFileInMem("Levels\\L2Data\\L2.TIL", NULL);
	pLevelPieces = LoadFileInMem("Levels\\L2Data\\L2.MIN", NULL);
	pSpecialCels = LoadFileInMem("Levels\\L2Data\\L2S.CEL", NULL);
}

void LvlCatacombs::LoadSetMap()
{
	switch (lvl.setlvlnum) {
	case SetLvl::BoneChamb:
		LoadPreL2Dungeon("Levels\\L2Data\\Bonecha2.DUN", 69, 39);
		LoadL2Dungeon("Levels\\L2Data\\Bonecha1.DUN", 69, 39);
		LoadPalette("Levels\\L2Data\\L2_2.pal");
		DRLG_ListTrans(sizeof(SkelChamTrans1) / 4, &SkelChamTrans1[0]);
		DRLG_AreaTrans(sizeof(SkelChamTrans2) / 4, &SkelChamTrans2[0]);
		DRLG_ListTrans(sizeof(SkelChamTrans3) / 4, &SkelChamTrans3[0]);
		AddL2Objs({ 0, 0 }, { MAXDUNX, MAXDUNY });
		AddSChamObjs();
		InitSChambTriggers();
		break;
	}
}

void LvlCatacombs::DRLG_CreateThemeRoom(int themeIndex)
{
	int xx, yy;

	for (yy = lvl.themeLoc[themeIndex].y; yy < lvl.themeLoc[themeIndex].y + lvl.themeLoc[themeIndex].height; yy++) {
		for (xx = lvl.themeLoc[themeIndex].x; xx < lvl.themeLoc[themeIndex].x + lvl.themeLoc[themeIndex].width; xx++) {
			if (yy == lvl.themeLoc[themeIndex].y
				    && xx >= lvl.themeLoc[themeIndex].x
				    && xx <= lvl.themeLoc[themeIndex].x + lvl.themeLoc[themeIndex].width
				|| yy == lvl.themeLoc[themeIndex].y + lvl.themeLoc[themeIndex].height - 1
				    && xx >= lvl.themeLoc[themeIndex].x
				    && xx <= lvl.themeLoc[themeIndex].x + lvl.themeLoc[themeIndex].width) {
				dgrid[xx][yy].dungeon = 2;
			} else if (xx == lvl.themeLoc[themeIndex].x
				    && yy >= lvl.themeLoc[themeIndex].y
				    && yy <= lvl.themeLoc[themeIndex].y + lvl.themeLoc[themeIndex].height
				|| xx == lvl.themeLoc[themeIndex].x + lvl.themeLoc[themeIndex].width - 1
				    && yy >= lvl.themeLoc[themeIndex].y
				    && yy <= lvl.themeLoc[themeIndex].y + lvl.themeLoc[themeIndex].height) {
				dgrid[xx][yy].dungeon = 1;
			} else {
				dgrid[xx][yy].dungeon = 3;
			}
		}
	}

	dgrid[lvl.themeLoc[themeIndex].x][lvl.themeLoc[themeIndex].y].dungeon = 8;
	dgrid[lvl.themeLoc[themeIndex].x + lvl.themeLoc[themeIndex].width - 1][lvl.themeLoc[themeIndex].y].dungeon = 7;
	dgrid[lvl.themeLoc[themeIndex].x][lvl.themeLoc[themeIndex].y + lvl.themeLoc[themeIndex].height - 1].dungeon = 9;
	dgrid[lvl.themeLoc[themeIndex].x + lvl.themeLoc[themeIndex].width - 1][lvl.themeLoc[themeIndex].y + lvl.themeLoc[themeIndex].height - 1].dungeon = 6;

	switch (random_(0, 2)) {
	case 0:
		dgrid[lvl.themeLoc[themeIndex].x + lvl.themeLoc[themeIndex].width - 1][lvl.themeLoc[themeIndex].y + lvl.themeLoc[themeIndex].height / 2].dungeon = 4;
		break;
	case 1:
		dgrid[lvl.themeLoc[themeIndex].x + lvl.themeLoc[themeIndex].width / 2][lvl.themeLoc[themeIndex].y + lvl.themeLoc[themeIndex].height - 1].dungeon = 5;
		break;
	}
}

static bool DRLG_L2PlaceMiniSet(uint8_t *miniset, int tmin, int tmax, int cx, int cy, bool setview, int ldir)
{
	int sx, sy, sw, sh, xx, yy, i, ii, numt, bailcnt;
	bool found;

	sw = miniset[0];
	sh = miniset[1];

	if (tmax - tmin == 0) {
		numt = 1;
	} else {
		numt = random_(0, tmax - tmin) + tmin;
	}

	for (i = 0; i < numt; i++) {
		sx = random_(0, DMAXX - sw);
		sy = random_(0, DMAXY - sh);
		found = false;
		for (bailcnt = 0; !found && bailcnt < 200; bailcnt++) {
			found = true;
			if (sx >= nSx1 && sx <= nSx2 && sy >= nSy1 && sy <= nSy2) {
				found = false;
			}
			if (cx != -1 && sx >= cx - sw && sx <= cx + 12) {
				sx = random_(0, DMAXX - sw);
				sy = random_(0, DMAXY - sh);
				found = false;
			}
			if (cy != -1 && sy >= cy - sh && sy <= cy + 12) {
				sx = random_(0, DMAXX - sw);
				sy = random_(0, DMAXY - sh);
				found = false;
			}
			ii = 2;
			for (yy = 0; yy < sh && found == true; yy++) {
				for (xx = 0; xx < sw && found == true; xx++) {
					if (miniset[ii] != 0 && dgrid[xx + sx][yy + sy].dungeon != miniset[ii]) {
						found = false;
					}
					if (dgrid[xx + sx][yy + sy].dflags != 0) {
						found = false;
					}
					ii++;
				}
			}
			if (!found) {
				sx++;
				if (sx == DMAXX - sw) {
					sx = 0;
					sy++;
					if (sy == DMAXY - sh) {
						sy = 0;
					}
				}
			}
		}
		if (bailcnt >= 200) {
			return false;
		}
		ii = sw * sh + 2;
		for (yy = 0; yy < sh; yy++) {
			for (xx = 0; xx < sw; xx++) {
				if (miniset[ii] != 0) {
					dgrid[xx + sx][yy + sy].dungeon = miniset[ii];
				}
				ii++;
			}
		}
	}

	if (setview == true) {
		View.x = 2 * sx + 21;
		View.y = 2 * sy + 22;
	}
	if (ldir == 0) {
		LvlView.x = 2 * sx + 21;
		LvlView.y = 2 * sy + 22;
	}
	if (ldir == 6) {
		LvlView.x = 2 * sx + 21;
		LvlView.y = 2 * sy + 22;
	}

	return true;
}

static void DRLG_L2PlaceRndSet(uint8_t *miniset, int rndper)
{
	int sx, sy, sw, sh, xx, yy, ii, kk;
	bool found;

	sw = miniset[0];
	sh = miniset[1];

	for (sy = 0; sy < DMAXY - sh; sy++) {
		for (sx = 0; sx < DMAXX - sw; sx++) {
			found = true;
			ii = 2;
			if (sx >= nSx1 && sx <= nSx2 && sy >= nSy1 && sy <= nSy2) {
				found = false;
			}
			for (yy = 0; yy < sh && found == true; yy++) {
				for (xx = 0; xx < sw && found == true; xx++) {
					if (miniset[ii] != 0 && dgrid[xx + sx][yy + sy].dungeon != miniset[ii]) {
						found = false;
					}
					if (dgrid[xx + sx][yy + sy].dflags != 0) {
						found = false;
					}
					ii++;
				}
			}
			kk = sw * sh + 2;
			if (found == true) {
				for (yy = std::max(sy - sh, 0); yy < std::min(sy + 2 * sh, DMAXY) && found == true; yy++) {
					for (xx = std::max(sx - sw, 0); xx < std::min(sx + 2 * sw, DMAXX); xx++) {
						// BUGFIX: yy and xx can go out of bounds (fixed)
						if (dgrid[xx][yy].dungeon == miniset[kk]) {
							found = false;
						}
					}
				}
			}
			if (found == true && random_(0, 100) < rndper) {
				for (yy = 0; yy < sh; yy++) {
					for (xx = 0; xx < sw; xx++) {
						if (miniset[kk] != 0) {
							dgrid[xx + sx][yy + sy].dungeon = miniset[kk];
						}
						kk++;
					}
				}
			}
		}
	}
}

static void DRLG_L2Subs()
{
	int x, y, i, j, k, rv;
	uint8_t c;

	for (y = 0; y < DMAXY; y++) {
		for (x = 0; x < DMAXX; x++) {
			if ((x < nSx1 || x > nSx2) && (y < nSy1 || y > nSy2) && random_(0, 4) == 0) {
				c = BTYPESL2[dgrid[x][y].dungeon];
				if (c != 0) {
					rv = random_(0, 16);
					k = -1;
					while (rv >= 0) {
						k++;
						if (k == sizeof(BTYPESL2)) {
							k = 0;
						}
						if (c == BTYPESL2[k]) {
							rv--;
						}
					}
					for (j = y - 2; j < y + 2; j++) {
						for (i = x - 2; i < x + 2; i++) {
							if (dgrid[i][j].dungeon == k) {
								j = y + 3;
								i = x + 2;
							}
						}
					}
					if (j < y + 3) {
						dgrid[x][y].dungeon = k;
					}
				}
			}
		}
	}
}

static void DRLG_L2Shadows()
{
	int x, y, i;
	bool patflag;
	uint8_t sd[2][2];

	for (y = 1; y < DMAXY; y++) {
		for (x = 1; x < DMAXX; x++) {
			sd[0][0] = BSTYPESL2[dgrid[x][y].dungeon];
			sd[1][0] = BSTYPESL2[dgrid[x - 1][y].dungeon];
			sd[0][1] = BSTYPESL2[dgrid[x][y - 1].dungeon];
			sd[1][1] = BSTYPESL2[dgrid[x - 1][y - 1].dungeon];
			for (i = 0; i < 2; i++) {
				if (SPATSL2[i].strig == sd[0][0]) {
					patflag = true;
					if (SPATSL2[i].s1 != 0 && SPATSL2[i].s1 != sd[1][1]) {
						patflag = false;
					}
					if (SPATSL2[i].s2 != 0 && SPATSL2[i].s2 != sd[0][1]) {
						patflag = false;
					}
					if (SPATSL2[i].s3 != 0 && SPATSL2[i].s3 != sd[1][0]) {
						patflag = false;
					}
					if (patflag == true) {
						if (SPATSL2[i].nv1 != 0) {
							dgrid[x - 1][y - 1].dungeon = SPATSL2[i].nv1;
						}
						if (SPATSL2[i].nv2 != 0) {
							dgrid[x][y - 1].dungeon = SPATSL2[i].nv2;
						}
						if (SPATSL2[i].nv3 != 0) {
							dgrid[x - 1][y].dungeon = SPATSL2[i].nv3;
						}
					}
				}
			}
		}
	}
}

void InitDungeon()
{
	int i, j;

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			predungeon[i][j] = 32;
			dgrid[i][j].dflags = 0;
		}
	}
}

static void DRLG_LoadL2SP()
{
	setloadflag = false;

	if (QuestStatus(QuestId::blind)) {
		pSetPiece = LoadFileInMem("Levels\\L2Data\\Blind2.DUN", NULL);
		setloadflag = true;
	} else if (QuestStatus(QuestId::blood)) {
		pSetPiece = LoadFileInMem("Levels\\L2Data\\Blood1.DUN", NULL);
		setloadflag = true;
	} else if (QuestStatus(QuestId::schamb)) {
		pSetPiece = LoadFileInMem("Levels\\L2Data\\Bonestr2.DUN", NULL);
		setloadflag = true;
	}
}

static void DRLG_FreeL2SP()
{
	MemFreeDbg(pSetPiece);
}

static void DRLG_L2SetRoom(int rx1, int ry1)
{
	int rw, rh, i, j;
	uint8_t *sp;

	rw = pSetPiece[0];
	rh = pSetPiece[2];

	lvl.setpc({ rx1, ry1, rw, rh });

	sp = &pSetPiece[4];

	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*sp != 0) {
				dgrid[i + rx1][j + ry1].dungeon = *sp;
				dgrid[i + rx1][j + ry1].dflags |= DLRG_PROTECTED;
			} else {
				dgrid[i + rx1][j + ry1].dungeon = 3;
			}
			sp += 2;
		}
	}
}

static void DefineRoom(int nX1, int nY1, int nX2, int nY2, bool ForceHW)
{
	int i, j;

	predungeon[nX1][nY1] = 67;
	predungeon[nX1][nY2] = 69;
	predungeon[nX2][nY1] = 66;
	predungeon[nX2][nY2] = 65;

	nRoomCnt++;
	RoomList[nRoomCnt].nRoomx1 = nX1;
	RoomList[nRoomCnt].nRoomx2 = nX2;
	RoomList[nRoomCnt].nRoomy1 = nY1;
	RoomList[nRoomCnt].nRoomy2 = nY2;

	if (ForceHW == true) {
		for (i = nX1; i < nX2; i++) {
			/// BUGFIX: Should loop j between nY1 and nY2 instead of always using nY1.
			while (i < nY2) {
				dgrid[i][nY1].dflags |= DLRG_PROTECTED;
				i++;
			}
		}
	}
	for (i = nX1 + 1; i <= nX2 - 1; i++) {
		predungeon[i][nY1] = 35;
		predungeon[i][nY2] = 35;
	}
	nY2--;
	for (j = nY1 + 1; j <= nY2; j++) {
		predungeon[nX1][j] = 35;
		predungeon[nX2][j] = 35;
		for (i = nX1 + 1; i < nX2; i++) {
			predungeon[i][j] = 46;
		}
	}
}

static void CreateDoorType(int nX, int nY)
{
	bool fDoneflag;

	fDoneflag = false;

	if (predungeon[nX - 1][nY] == 68) {
		fDoneflag = true;
	}
	if (predungeon[nX + 1][nY] == 68) {
		fDoneflag = true;
	}
	if (predungeon[nX][nY - 1] == 68) {
		fDoneflag = true;
	}
	if (predungeon[nX][nY + 1] == 68) {
		fDoneflag = true;
	}
	if (predungeon[nX][nY] == 66 || predungeon[nX][nY] == 67 || predungeon[nX][nY] == 65 || predungeon[nX][nY] == 69) {
		fDoneflag = true;
	}

	if (!fDoneflag) {
		predungeon[nX][nY] = 68;
	}
}

static void PlaceHallExt(int nX, int nY)
{
	if (predungeon[nX][nY] == 32) {
		predungeon[nX][nY] = 44;
	}
}

static void AddHall(int nX1, int nY1, int nX2, int nY2, int nHd)
{
	HALLNODE *p1, *p2;

	if (pHallList == NULL) {
		pHallList = (HALLNODE *)DiabloAllocPtr(sizeof(*pHallList));
		pHallList->nHallx1 = nX1;
		pHallList->nHally1 = nY1;
		pHallList->nHallx2 = nX2;
		pHallList->nHally2 = nY2;
		pHallList->nHalldir = nHd;
		pHallList->pNext = NULL;
	} else {
		p1 = (HALLNODE *)DiabloAllocPtr(sizeof(*pHallList));
		p1->nHallx1 = nX1;
		p1->nHally1 = nY1;
		p1->nHallx2 = nX2;
		p1->nHally2 = nY2;
		p1->nHalldir = nHd;
		p1->pNext = NULL;
		p2 = pHallList;
		while (p2->pNext != NULL) {
			p2 = p2->pNext;
		}
		p2->pNext = p1;
	}
}

/**
 * Draws a random room rectangle, and then subdivides the rest of the passed in rectangle into 4 and recurses.
 * @param nX1 Lower X boundary of the area to draw into.
 * @param nY1 Lower Y boundary of the area to draw into.
 * @param nX2 Upper X boundary of the area to draw into.
 * @param nY2 Upper Y boundary of the area to draw into.
 * @param nRDest The room number of the parent room this call was invoked for. Zero for empty
 * @param nHDir The direction of the hall from nRDest to this room.
 * @param ForceHW If set, nH and nW are used for room size instead of random values.
 * @param nH Height of the room, if ForceHW is set.
 * @param nW Width of the room, if ForceHW is set.
 */
static void CreateRoom(int nX1, int nY1, int nX2, int nY2, int nRDest, int nHDir, bool ForceHW, int nH, int nW)
{
	int nAw, nAh, nRw, nRh, nRx1, nRy1, nRx2, nRy2, nHw, nHh, nHx1, nHy1, nHx2, nHy2, nRid;

	if (nRoomCnt >= 80) {
		return;
	}

	nAw = nX2 - nX1;
	nAh = nY2 - nY1;
	if (nAw < Area_Min || nAh < Area_Min) {
		return;
	}

	if (nAw > Room_Max) {
		nRw = random_(0, Room_Max - Room_Min) + Room_Min;
	} else if (nAw > Room_Min) {
		nRw = random_(0, nAw - Room_Min) + Room_Min;
	} else {
		nRw = nAw;
	}
	if (nAh > Room_Max) {
		nRh = random_(0, Room_Max - Room_Min) + Room_Min;
	} else if (nAh > Room_Min) {
		nRh = random_(0, nAh - Room_Min) + Room_Min;
	} else {
		nRh = nAh;
	}

	if (ForceHW == true) {
		nRw = nW;
		nRh = nH;
	}

	nRx1 = random_(0, nX2 - nX1) + nX1;
	nRy1 = random_(0, nY2 - nY1) + nY1;
	nRx2 = nRw + nRx1;
	nRy2 = nRh + nRy1;
	if (nRx2 > nX2) {
		nRx2 = nX2;
		nRx1 = nX2 - nRw;
	}
	if (nRy2 > nY2) {
		nRy2 = nY2;
		nRy1 = nY2 - nRh;
	}

	if (nRx1 >= 38) {
		nRx1 = 38;
	}
	if (nRy1 >= 38) {
		nRy1 = 38;
	}
	if (nRx1 <= 1) {
		nRx1 = 1;
	}
	if (nRy1 <= 1) {
		nRy1 = 1;
	}
	if (nRx2 >= 38) {
		nRx2 = 38;
	}
	if (nRy2 >= 38) {
		nRy2 = 38;
	}
	if (nRx2 <= 1) {
		nRx2 = 1;
	}
	if (nRy2 <= 1) {
		nRy2 = 1;
	}
	DefineRoom(nRx1, nRy1, nRx2, nRy2, ForceHW);

	if (ForceHW == true) {
		nSx1 = nRx1 + 2;
		nSy1 = nRy1 + 2;
		nSx2 = nRx2;
		nSy2 = nRy2;
	}

	nRid = nRoomCnt;
	RoomList[nRid].nRoomDest = nRDest;

	if (nRDest != 0) {
		if (nHDir == 1) {
			nHx1 = random_(0, nRx2 - nRx1 - 2) + nRx1 + 1;
			nHy1 = nRy1;
			nHw = RoomList[nRDest].nRoomx2 - RoomList[nRDest].nRoomx1 - 2;
			nHx2 = random_(0, nHw) + RoomList[nRDest].nRoomx1 + 1;
			nHy2 = RoomList[nRDest].nRoomy2;
		}
		if (nHDir == 3) {
			nHx1 = random_(0, nRx2 - nRx1 - 2) + nRx1 + 1;
			nHy1 = nRy2;
			nHw = RoomList[nRDest].nRoomx2 - RoomList[nRDest].nRoomx1 - 2;
			nHx2 = random_(0, nHw) + RoomList[nRDest].nRoomx1 + 1;
			nHy2 = RoomList[nRDest].nRoomy1;
		}
		if (nHDir == 2) {
			nHx1 = nRx2;
			nHy1 = random_(0, nRy2 - nRy1 - 2) + nRy1 + 1;
			nHx2 = RoomList[nRDest].nRoomx1;
			nHh = RoomList[nRDest].nRoomy2 - RoomList[nRDest].nRoomy1 - 2;
			nHy2 = random_(0, nHh) + RoomList[nRDest].nRoomy1 + 1;
		}
		if (nHDir == 4) {
			nHx1 = nRx1;
			nHy1 = random_(0, nRy2 - nRy1 - 2) + nRy1 + 1;
			nHx2 = RoomList[nRDest].nRoomx2;
			nHh = RoomList[nRDest].nRoomy2 - RoomList[nRDest].nRoomy1 - 2;
			nHy2 = random_(0, nHh) + RoomList[nRDest].nRoomy1 + 1;
		}
		AddHall(nHx1, nHy1, nHx2, nHy2, nHDir);
	}

	if (nRh > nRw) {
		CreateRoom(nX1 + 2, nY1 + 2, nRx1 - 2, nRy2 - 2, nRid, 2, 0, 0, 0);
		CreateRoom(nRx2 + 2, nRy1 + 2, nX2 - 2, nY2 - 2, nRid, 4, 0, 0, 0);
		CreateRoom(nX1 + 2, nRy2 + 2, nRx2 - 2, nY2 - 2, nRid, 1, 0, 0, 0);
		CreateRoom(nRx1 + 2, nY1 + 2, nX2 - 2, nRy1 - 2, nRid, 3, 0, 0, 0);
	} else {
		CreateRoom(nX1 + 2, nY1 + 2, nRx2 - 2, nRy1 - 2, nRid, 3, 0, 0, 0);
		CreateRoom(nRx1 + 2, nRy2 + 2, nX2 - 2, nY2 - 2, nRid, 1, 0, 0, 0);
		CreateRoom(nX1 + 2, nRy1 + 2, nRx1 - 2, nY2 - 2, nRid, 2, 0, 0, 0);
		CreateRoom(nRx2 + 2, nY1 + 2, nX2 - 2, nRy2 - 2, nRid, 4, 0, 0, 0);
	}
}

static void GetHall(int *nX1, int *nY1, int *nX2, int *nY2, int *nHd)
{
	HALLNODE *p1;

	p1 = pHallList->pNext;
	*nX1 = pHallList->nHallx1;
	*nY1 = pHallList->nHally1;
	*nX2 = pHallList->nHallx2;
	*nY2 = pHallList->nHally2;
	*nHd = pHallList->nHalldir;
	MemFreeDbg(pHallList);
	pHallList = p1;
}

static void ConnectHall(int nX1, int nY1, int nX2, int nY2, int nHd)
{
	int nCurrd, nDx, nDy, nRp, nOrigX1, nOrigY1, fMinusFlag, fPlusFlag;
	bool fDoneflag, fInroom;

	fDoneflag = false;
	fMinusFlag = random_(0, 100);
	fPlusFlag = random_(0, 100);
	nOrigX1 = nX1;
	nOrigY1 = nY1;
	CreateDoorType(nX1, nY1);
	CreateDoorType(nX2, nY2);
	nDx = abs(nX2 - nX1); /* unused */
	nDy = abs(nY2 - nY1); /* unused */
	nCurrd = nHd;
	nX2 -= Dir_Xadd[nCurrd];
	nY2 -= Dir_Yadd[nCurrd];
	predungeon[nX2][nY2] = 44;
	fInroom = false;

	while (!fDoneflag) {
		if (nX1 >= 38 && nCurrd == 2) {
			nCurrd = 4;
		}
		if (nY1 >= 38 && nCurrd == 3) {
			nCurrd = 1;
		}
		if (nX1 <= 1 && nCurrd == 4) {
			nCurrd = 2;
		}
		if (nY1 <= 1 && nCurrd == 1) {
			nCurrd = 3;
		}
		if (predungeon[nX1][nY1] == 67 && (nCurrd == 1 || nCurrd == 4)) {
			nCurrd = 2;
		}
		if (predungeon[nX1][nY1] == 66 && (nCurrd == 1 || nCurrd == 2)) {
			nCurrd = 3;
		}
		if (predungeon[nX1][nY1] == 69 && (nCurrd == 4 || nCurrd == 3)) {
			nCurrd = 1;
		}
		if (predungeon[nX1][nY1] == 65 && (nCurrd == 2 || nCurrd == 3)) {
			nCurrd = 4;
		}
		nX1 += Dir_Xadd[nCurrd];
		nY1 += Dir_Yadd[nCurrd];
		if (predungeon[nX1][nY1] == 32) {
			if (fInroom) {
				CreateDoorType(nX1 - Dir_Xadd[nCurrd], nY1 - Dir_Yadd[nCurrd]);
			} else {
				if (fMinusFlag < 50) {
					if (nCurrd != 1 && nCurrd != 3) {
						PlaceHallExt(nX1, nY1 - 1);
					} else {
						PlaceHallExt(nX1 - 1, nY1);
					}
				}
				if (fPlusFlag < 50) {
					if (nCurrd != 1 && nCurrd != 3) {
						PlaceHallExt(nX1, nY1 + 1);
					} else {
						PlaceHallExt(nX1 + 1, nY1);
					}
				}
			}
			predungeon[nX1][nY1] = 44;
			fInroom = false;
		} else {
			if (!fInroom && predungeon[nX1][nY1] == 35) {
				CreateDoorType(nX1, nY1);
			}
			if (predungeon[nX1][nY1] != 44) {
				fInroom = true;
			}
		}
		nDx = abs(nX2 - nX1);
		nDy = abs(nY2 - nY1);
		if (nDx > nDy) {
			nRp = 2 * nDx;
			if (nRp > 30) {
				nRp = 30;
			}
			if (random_(0, 100) < nRp) {
				if (nX2 <= nX1 || nX1 >= DMAXX) {
					nCurrd = 4;
				} else {
					nCurrd = 2;
				}
			}
		} else {
			nRp = 5 * nDy;
			if (nRp > 80) {
				nRp = 80;
			}
			if (random_(0, 100) < nRp) {
				if (nY2 <= nY1 || nY1 >= DMAXY) {
					nCurrd = 1;
				} else {
					nCurrd = 3;
				}
			}
		}
		if (nDy < 10 && nX1 == nX2 && (nCurrd == 2 || nCurrd == 4)) {
			if (nY2 <= nY1 || nY1 >= DMAXY) {
				nCurrd = 1;
			} else {
				nCurrd = 3;
			}
		}
		if (nDx < 10 && nY1 == nY2 && (nCurrd == 1 || nCurrd == 3)) {
			if (nX2 <= nX1 || nX1 >= DMAXX) {
				nCurrd = 4;
			} else {
				nCurrd = 2;
			}
		}
		if (nDy == 1 && nDx > 1 && (nCurrd == 1 || nCurrd == 3)) {
			if (nX2 <= nX1 || nX1 >= DMAXX) {
				nCurrd = 4;
			} else {
				nCurrd = 2;
			}
		}
		if (nDx == 1 && nDy > 1 && (nCurrd == 2 || nCurrd == 4)) {
			if (nY2 <= nY1 || nX1 >= DMAXX) {
				nCurrd = 1;
			} else {
				nCurrd = 3;
			}
		}
		if (nDx == 0 && predungeon[nX1][nY1] != 32 && (nCurrd == 2 || nCurrd == 4)) {
			if (nX2 <= nOrigX1 || nX1 >= DMAXX) {
				nCurrd = 1;
			} else {
				nCurrd = 3;
			}
		}
		if (nDy == 0 && predungeon[nX1][nY1] != 32 && (nCurrd == 1 || nCurrd == 3)) {
			if (nY2 <= nOrigY1 || nY1 >= DMAXY) {
				nCurrd = 4;
			} else {
				nCurrd = 2;
			}
		}
		if (nX1 == nX2 && nY1 == nY2) {
			fDoneflag = true;
		}
	}
}

static void DoPatternCheck(int i, int j)
{
	int k, l, x, y, nOk;

	for (k = 0; Patterns[k][4] != 255; k++) {
		x = i - 1;
		y = j - 1;
		nOk = 254;
		for (l = 0; l < 9 && nOk == 254; l++) {
			nOk = 255;
			if (l == 3 || l == 6) {
				y++;
				x = i - 1;
			}
			if (x >= 0 && x < DMAXX && y >= 0 && y < DMAXY) {
				switch (Patterns[k][l]) {
				case 0:
					nOk = 254;
					break;
				case 1:
					if (predungeon[x][y] == 35) {
						nOk = 254;
					}
					break;
				case 2:
					if (predungeon[x][y] == 46) {
						nOk = 254;
					}
					break;
				case 4:
					if (predungeon[x][y] == 32) {
						nOk = 254;
					}
					break;
				case 3:
					if (predungeon[x][y] == 68) {
						nOk = 254;
					}
					break;
				case 5:
					if (predungeon[x][y] == 68 || predungeon[x][y] == 46) {
						nOk = 254;
					}
					break;
				case 6:
					if (predungeon[x][y] == 68 || predungeon[x][y] == 35) {
						nOk = 254;
					}
					break;
				case 7:
					if (predungeon[x][y] == 32 || predungeon[x][y] == 46) {
						nOk = 254;
					}
					break;
				case 8:
					if (predungeon[x][y] == 68 || predungeon[x][y] == 35 || predungeon[x][y] == 46) {
						nOk = 254;
					}
					break;
				}
			} else {
				nOk = 254;
			}
			x++;
		}
		if (nOk == 254) {
			dgrid[i][j].dungeon = Patterns[k][9];
		}
	}
}

static void L2TileFix()
{
	int i, j;

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dgrid[i][j].dungeon == 1 && dgrid[i][j + 1].dungeon == 3) {
				dgrid[i][j + 1].dungeon = 1;
			}
			if (dgrid[i][j].dungeon == 3 && dgrid[i][j + 1].dungeon == 1) {
				dgrid[i][j + 1].dungeon = 3;
			}
			if (dgrid[i][j].dungeon == 3 && dgrid[i + 1][j].dungeon == 7) {
				dgrid[i + 1][j].dungeon = 3;
			}
			if (dgrid[i][j].dungeon == 2 && dgrid[i + 1][j].dungeon == 3) {
				dgrid[i + 1][j].dungeon = 2;
			}
			if (dgrid[i][j].dungeon == 11 && dgrid[i + 1][j].dungeon == 14) {
				dgrid[i + 1][j].dungeon = 16;
			}
		}
	}
}

static bool DL2_Cont(bool x1f, bool y1f, bool x2f, bool y2f)
{
	if (x1f && x2f && y1f && y2f) {
		return false;
	}
	if (x1f && x2f && (y1f || y2f)) {
		return true;
	}
	if (y1f && y2f && (x1f || x2f)) {
		return true;
	}

	return false;
}

static int DL2_NumNoChar()
{
	int t, ii, jj;

	t = 0;
	for (jj = 0; jj < DMAXY; jj++) {
		for (ii = 0; ii < DMAXX; ii++) {
			if (predungeon[ii][jj] == 32) {
				t++;
			}
		}
	}

	return t;
}

static void DL2_DrawRoom(int x1, int y1, int x2, int y2)
{
	int ii, jj;

	for (jj = y1; jj <= y2; jj++) {
		for (ii = x1; ii <= x2; ii++) {
			predungeon[ii][jj] = 46;
		}
	}
	for (jj = y1; jj <= y2; jj++) {
		predungeon[x1][jj] = 35;
		predungeon[x2][jj] = 35;
	}
	for (ii = x1; ii <= x2; ii++) {
		predungeon[ii][y1] = 35;
		predungeon[ii][y2] = 35;
	}
}

static void DL2_KnockWalls(int x1, int y1, int x2, int y2)
{
	int ii, jj;

	for (ii = x1 + 1; ii < x2; ii++) {
		if (predungeon[ii][y1 - 1] == 46 && predungeon[ii][y1 + 1] == 46) {
			predungeon[ii][y1] = 46;
		}
		if (predungeon[ii][y2 - 1] == 46 && predungeon[ii][y2 + 1] == 46) {
			predungeon[ii][y2] = 46;
		}
		if (predungeon[ii][y1 - 1] == 68) {
			predungeon[ii][y1 - 1] = 46;
		}
		if (predungeon[ii][y2 + 1] == 68) {
			predungeon[ii][y2 + 1] = 46;
		}
	}
	for (jj = y1 + 1; jj < y2; jj++) {
		if (predungeon[x1 - 1][jj] == 46 && predungeon[x1 + 1][jj] == 46) {
			predungeon[x1][jj] = 46;
		}
		if (predungeon[x2 - 1][jj] == 46 && predungeon[x2 + 1][jj] == 46) {
			predungeon[x2][jj] = 46;
		}
		if (predungeon[x1 - 1][jj] == 68) {
			predungeon[x1 - 1][jj] = 46;
		}
		if (predungeon[x2 + 1][jj] == 68) {
			predungeon[x2 + 1][jj] = 46;
		}
	}
}

static bool DL2_FillVoids()
{
	int ii, jj, xx, yy, x1, x2, y1, y2;
	bool xf1, xf2, yf1, yf2;
	int to;

	to = 0;
	while (DL2_NumNoChar() > 700 && to < 100) {
		xx = random_(0, 38) + 1;
		yy = random_(0, 38) + 1;
		if (predungeon[xx][yy] != 35) {
			continue;
		}
		xf1 = xf2 = yf1 = yf2 = false;
		if (predungeon[xx - 1][yy] == 32 && predungeon[xx + 1][yy] == 46) {
			if (predungeon[xx + 1][yy - 1] == 46
			    && predungeon[xx + 1][yy + 1] == 46
			    && predungeon[xx - 1][yy - 1] == 32
			    && predungeon[xx - 1][yy + 1] == 32) {
				xf1 = yf1 = yf2 = true;
			}
		} else if (predungeon[xx + 1][yy] == 32 && predungeon[xx - 1][yy] == 46) {
			if (predungeon[xx - 1][yy - 1] == 46
			    && predungeon[xx - 1][yy + 1] == 46
			    && predungeon[xx + 1][yy - 1] == 32
			    && predungeon[xx + 1][yy + 1] == 32) {
				xf2 = yf1 = yf2 = true;
			}
		} else if (predungeon[xx][yy - 1] == 32 && predungeon[xx][yy + 1] == 46) {
			if (predungeon[xx - 1][yy + 1] == 46
			    && predungeon[xx + 1][yy + 1] == 46
			    && predungeon[xx - 1][yy - 1] == 32
			    && predungeon[xx + 1][yy - 1] == 32) {
				yf1 = xf1 = xf2 = true;
			}
		} else if (predungeon[xx][yy + 1] == 32 && predungeon[xx][yy - 1] == 46) {
			if (predungeon[xx - 1][yy - 1] == 46
			    && predungeon[xx + 1][yy - 1] == 46
			    && predungeon[xx - 1][yy + 1] == 32
			    && predungeon[xx + 1][yy + 1] == 32) {
				yf2 = xf1 = xf2 = true;
			}
		}
		if (DL2_Cont(xf1, yf1, xf2, yf2)) {
			if (xf1) {
				x1 = xx - 1;
			} else {
				x1 = xx;
			}
			if (xf2) {
				x2 = xx + 1;
			} else {
				x2 = xx;
			}
			if (yf1) {
				y1 = yy - 1;
			} else {
				y1 = yy;
			}
			if (yf2) {
				y2 = yy + 1;
			} else {
				y2 = yy;
			}
			if (!xf1) {
				while (yf1 || yf2) {
					if (y1 == 0) {
						yf1 = false;
					}
					if (y2 == DMAXY - 1) {
						yf2 = false;
					}
					if (y2 - y1 >= 14) {
						yf1 = false;
						yf2 = false;
					}
					if (yf1) {
						y1--;
					}
					if (yf2) {
						y2++;
					}
					if (predungeon[x2][y1] != 32) {
						yf1 = false;
					}
					if (predungeon[x2][y2] != 32) {
						yf2 = false;
					}
				}
				y1 += 2;
				y2 -= 2;
				if (y2 - y1 > 5) {
					while (xf2) {
						if (x2 == 39) {
							xf2 = false;
						}
						if (x2 - x1 >= 12) {
							xf2 = false;
						}
						for (jj = y1; jj <= y2; jj++) {
							if (predungeon[x2][jj] != 32) {
								xf2 = false;
							}
						}
						if (xf2) {
							x2++;
						}
					}
					x2 -= 2;
					if (x2 - x1 > 5) {
						DL2_DrawRoom(x1, y1, x2, y2);
						DL2_KnockWalls(x1, y1, x2, y2);
					}
				}
			} else if (!xf2) {
				while (yf1 || yf2) {
					if (y1 == 0) {
						yf1 = false;
					}
					if (y2 == DMAXY - 1) {
						yf2 = false;
					}
					if (y2 - y1 >= 14) {
						yf1 = false;
						yf2 = false;
					}
					if (yf1) {
						y1--;
					}
					if (yf2) {
						y2++;
					}
					if (predungeon[x1][y1] != 32) {
						yf1 = false;
					}
					if (predungeon[x1][y2] != 32) {
						yf2 = false;
					}
				}
				y1 += 2;
				y2 -= 2;
				if (y2 - y1 > 5) {
					while (xf1) {
						if (x1 == 0) {
							xf1 = false;
						}
						if (x2 - x1 >= 12) {
							xf1 = false;
						}
						for (jj = y1; jj <= y2; jj++) {
							if (predungeon[x1][jj] != 32) {
								xf1 = false;
							}
						}
						if (xf1) {
							x1--;
						}
					}
					x1 += 2;
					if (x2 - x1 > 5) {
						DL2_DrawRoom(x1, y1, x2, y2);
						DL2_KnockWalls(x1, y1, x2, y2);
					}
				}
			} else if (!yf1) {
				while (xf1 || xf2) {
					if (x1 == 0) {
						xf1 = false;
					}
					if (x2 == DMAXX - 1) {
						xf2 = false;
					}
					if (x2 - x1 >= 14) {
						xf1 = false;
						xf2 = false;
					}
					if (xf1) {
						x1--;
					}
					if (xf2) {
						x2++;
					}
					if (predungeon[x1][y2] != 32) {
						xf1 = false;
					}
					if (predungeon[x2][y2] != 32) {
						xf2 = false;
					}
				}
				x1 += 2;
				x2 -= 2;
				if (x2 - x1 > 5) {
					while (yf2) {
						if (y2 == DMAXY - 1) {
							yf2 = false;
						}
						if (y2 - y1 >= 12) {
							yf2 = false;
						}
						for (ii = x1; ii <= x2; ii++) {
							if (predungeon[ii][y2] != 32) {
								yf2 = false;
							}
						}
						if (yf2) {
							y2++;
						}
					}
					y2 -= 2;
					if (y2 - y1 > 5) {
						DL2_DrawRoom(x1, y1, x2, y2);
						DL2_KnockWalls(x1, y1, x2, y2);
					}
				}
			} else if (!yf2) {
				while (xf1 || xf2) {
					if (x1 == 0) {
						xf1 = false;
					}
					if (x2 == DMAXX - 1) {
						xf2 = false;
					}
					if (x2 - x1 >= 14) {
						xf1 = false;
						xf2 = false;
					}
					if (xf1) {
						x1--;
					}
					if (xf2) {
						x2++;
					}
					if (predungeon[x1][y1] != 32) {
						xf1 = false;
					}
					if (predungeon[x2][y1] != 32) {
						xf2 = false;
					}
				}
				x1 += 2;
				x2 -= 2;
				if (x2 - x1 > 5) {
					while (yf1) {
						if (y1 == 0) {
							yf1 = false;
						}
						if (y2 - y1 >= 12) {
							yf1 = false;
						}
						for (ii = x1; ii <= x2; ii++) {
							if (predungeon[ii][y1] != 32) {
								yf1 = false;
							}
						}
						if (yf1) {
							y1--;
						}
					}
					y1 += 2;
					if (y2 - y1 > 5) {
						DL2_DrawRoom(x1, y1, x2, y2);
						DL2_KnockWalls(x1, y1, x2, y2);
					}
				}
			}
		}
		to++;
	}

	return DL2_NumNoChar() <= 700;
}

static bool CreateDungeon()
{
	int i, j, nHx1, nHy1, nHx2, nHy2, nHd, ForceH, ForceW;
	bool ForceHW;

	ForceW = 0;
	ForceH = 0;
	ForceHW = false;

	switch (lvl.currlevel) {
	case 5:
		if (quests[QuestId::blood]._qactive) {
			ForceHW = true;
			ForceH = 20;
			ForceW = 14;
		}
		break;
	case 6:
		if (quests[QuestId::schamb]._qactive) {
			ForceHW = true;
			ForceW = 10;
			ForceH = 10;
		}
		break;
	case 7:
		if (quests[QuestId::blind]._qactive) {
			ForceHW = true;
			ForceW = 15;
			ForceH = 15;
		}
		break;
	case 8:
		break;
	}

	CreateRoom(2, 2, DMAXX - 1, DMAXY - 1, 0, 0, ForceHW, ForceH, ForceW);

	while (pHallList != NULL) {
		GetHall(&nHx1, &nHy1, &nHx2, &nHy2, &nHd);
		ConnectHall(nHx1, nHy1, nHx2, nHy2, nHd);
	}

	for (j = 0; j < DMAXY; j++) {     /// BUGFIX: change '<=' to '<' (fixed)
		for (i = 0; i < DMAXX; i++) { /// BUGFIX: change '<=' to '<' (fixed)
			if (predungeon[i][j] == 67) {
				predungeon[i][j] = 35;
			}
			if (predungeon[i][j] == 66) {
				predungeon[i][j] = 35;
			}
			if (predungeon[i][j] == 69) {
				predungeon[i][j] = 35;
			}
			if (predungeon[i][j] == 65) {
				predungeon[i][j] = 35;
			}
			if (predungeon[i][j] == 44) {
				predungeon[i][j] = 46;
				if (predungeon[i - 1][j - 1] == 32) {
					predungeon[i - 1][j - 1] = 35;
				}
				if (predungeon[i - 1][j] == 32) {
					predungeon[i - 1][j] = 35;
				}
				if (predungeon[i - 1][1 + j] == 32) {
					predungeon[i - 1][1 + j] = 35;
				}
				if (predungeon[i + 1][j - 1] == 32) {
					predungeon[i + 1][j - 1] = 35;
				}
				if (predungeon[i + 1][j] == 32) {
					predungeon[i + 1][j] = 35;
				}
				if (predungeon[i + 1][1 + j] == 32) {
					predungeon[i + 1][1 + j] = 35;
				}
				if (predungeon[i][j - 1] == 32) {
					predungeon[i][j - 1] = 35;
				}
				if (predungeon[i][j + 1] == 32) {
					predungeon[i][j + 1] = 35;
				}
			}
		}
	}

	if (!DL2_FillVoids()) {
		return false;
	}

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			DoPatternCheck(i, j);
		}
	}

	return true;
}

void LvlCatacombs::DRLG_L2Pass3()
{
	int i, j, xx, yy;
	long v1, v2, v3, v4, lv;
	WORD *MegaTiles;

	lv = 12 - 1;

	MegaTiles = (WORD *)&pMegaTiles[lv * 8];
	v1 = SDL_SwapLE16(*(MegaTiles + 0)); // +1;
	v2 = SDL_SwapLE16(*(MegaTiles + 1)); // +1;
	v3 = SDL_SwapLE16(*(MegaTiles + 2)); // +1;
	v4 = SDL_SwapLE16(*(MegaTiles + 3)); // +1;

	for (j = 0; j < MAXDUNY; j += 2) {
		for (i = 0; i < MAXDUNX; i += 2) {
			grid[i][j].setPiece(v1);
			grid[i + 1][j].setPiece(v2);
			grid[i][j + 1].setPiece(v3);
			grid[i + 1][j + 1].setPiece(v4);
		}
	}

	yy = 16;
	for (j = 0; j < DMAXY; j++) {
		xx = 16;
		for (i = 0; i < DMAXX; i++) {
			lv = dgrid[i][j].dungeon - 1;
			MegaTiles = (WORD *)&pMegaTiles[lv * 8];
			v1 = SDL_SwapLE16(*(MegaTiles + 0)); // +1;
			v2 = SDL_SwapLE16(*(MegaTiles + 1)); // +1;
			v3 = SDL_SwapLE16(*(MegaTiles + 2)); // +1;
			v4 = SDL_SwapLE16(*(MegaTiles + 3)); // +1;
			grid[xx][yy].setPiece(v1);
			grid[xx + 1][yy].setPiece(v2);
			grid[xx][yy + 1].setPiece(v3);
			grid[xx + 1][yy + 1].setPiece(v4);
			xx += 2;
		}
		yy += 2;
	}
}

static void DRLG_L2FTVR(int i, int j, int x, int y, int d)
{
	if (grid[x][y].dTransVal != 0 || dgrid[i][j].dungeon != 3) {
		if (d == 1) {
			grid[x][y].dTransVal = lvl.TransVal;
			grid[x][y + 1].dTransVal = lvl.TransVal;
		}
		if (d == 2) {
			grid[x + 1][y].dTransVal = lvl.TransVal;
			grid[x + 1][y + 1].dTransVal = lvl.TransVal;
		}
		if (d == 3) {
			grid[x][y].dTransVal = lvl.TransVal;
			grid[x + 1][y].dTransVal = lvl.TransVal;
		}
		if (d == 4) {
			grid[x][y + 1].dTransVal = lvl.TransVal;
			grid[x + 1][y + 1].dTransVal = lvl.TransVal;
		}
		if (d == 5) {
			grid[x + 1][y + 1].dTransVal = lvl.TransVal;
		}
		if (d == 6) {
			grid[x][y + 1].dTransVal = lvl.TransVal;
		}
		if (d == 7) {
			grid[x + 1][y].dTransVal = lvl.TransVal;
		}
		if (d == 8) {
			grid[x][y].dTransVal = lvl.TransVal;
		}
	} else {
		grid[x][y].dTransVal = lvl.TransVal;
		grid[x + 1][y].dTransVal = lvl.TransVal;
		grid[x][y + 1].dTransVal = lvl.TransVal;
		grid[x + 1][y + 1].dTransVal = lvl.TransVal;
		DRLG_L2FTVR(i + 1, j, x + 2, y, 1);
		DRLG_L2FTVR(i - 1, j, x - 2, y, 2);
		DRLG_L2FTVR(i, j + 1, x, y + 2, 3);
		DRLG_L2FTVR(i, j - 1, x, y - 2, 4);
		DRLG_L2FTVR(i - 1, j - 1, x - 2, y - 2, 5);
		DRLG_L2FTVR(i + 1, j - 1, x + 2, y - 2, 6);
		DRLG_L2FTVR(i - 1, j + 1, x - 2, y + 2, 7);
		DRLG_L2FTVR(i + 1, j + 1, x + 2, y + 2, 8);
	}
}

static void DRLG_L2FloodTVal()
{
	int i, j, xx, yy;

	yy = 16;
	for (j = 0; j < DMAXY; j++) {
		xx = 16;
		for (i = 0; i < DMAXX; i++) {
			if (dgrid[i][j].dungeon == 3 && grid[xx][yy].dTransVal == 0) {
				DRLG_L2FTVR(i, j, xx, yy, 0);
				lvl.TransVal++;
			}
			xx += 2;
		}
		yy += 2;
	}
}

static void DRLG_L2TransFix()
{
	int i, j, xx, yy;

	yy = 16;
	for (j = 0; j < DMAXY; j++) {
		xx = 16;
		for (i = 0; i < DMAXX; i++) {
			if (dgrid[i][j].dungeon == 14 && dgrid[i][j - 1].dungeon == 10) {
				grid[xx + 1][yy].dTransVal = grid[xx][yy].dTransVal;
				grid[xx + 1][yy + 1].dTransVal = grid[xx][yy].dTransVal;
			}
			if (dgrid[i][j].dungeon == 15 && dgrid[i + 1][j].dungeon == 11) {
				grid[xx][yy + 1].dTransVal = grid[xx][yy].dTransVal;
				grid[xx + 1][yy + 1].dTransVal = grid[xx][yy].dTransVal;
			}
			if (dgrid[i][j].dungeon == 10) {
				grid[xx + 1][yy].dTransVal = grid[xx][yy].dTransVal;
				grid[xx + 1][yy + 1].dTransVal = grid[xx][yy].dTransVal;
			}
			if (dgrid[i][j].dungeon == 11) {
				grid[xx][yy + 1].dTransVal = grid[xx][yy].dTransVal;
				grid[xx + 1][yy + 1].dTransVal = grid[xx][yy].dTransVal;
			}
			if (dgrid[i][j].dungeon == 16) {
				grid[xx + 1][yy].dTransVal = grid[xx][yy].dTransVal;
				grid[xx][yy + 1].dTransVal = grid[xx][yy].dTransVal;
				grid[xx + 1][yy + 1].dTransVal = grid[xx][yy].dTransVal;
			}
			xx += 2;
		}
		yy += 2;
	}
}

static void L2DirtFix()
{
	int i, j;

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dgrid[i][j].dungeon == 13 && dgrid[i + 1][j].dungeon != 11) {
				dgrid[i][j].dungeon = 146;
			}
			if (dgrid[i][j].dungeon == 11 && dgrid[i + 1][j].dungeon != 11) {
				dgrid[i][j].dungeon = 144;
			}
			if (dgrid[i][j].dungeon == 15 && dgrid[i + 1][j].dungeon != 11) {
				dgrid[i][j].dungeon = 148;
			}
			if (dgrid[i][j].dungeon == 10 && dgrid[i][j + 1].dungeon != 10) {
				dgrid[i][j].dungeon = 143;
			}
			if (dgrid[i][j].dungeon == 13 && dgrid[i][j + 1].dungeon != 10) {
				dgrid[i][j].dungeon = 146;
			}
			if (dgrid[i][j].dungeon == 14 && dgrid[i][j + 1].dungeon != 15) {
				dgrid[i][j].dungeon = 147;
			}
		}
	}
}

void L2LockoutFix()
{
	int i, j;
	bool doorok;

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dgrid[i][j].dungeon == 4 && dgrid[i - 1][j].dungeon != 3) {
				dgrid[i][j].dungeon = 1;
			}
			if (dgrid[i][j].dungeon == 5 && dgrid[i][j - 1].dungeon != 3) {
				dgrid[i][j].dungeon = 2;
			}
		}
	}
	for (j = 1; j < DMAXY - 1; j++) {
		for (i = 1; i < DMAXX - 1; i++) {
			if (dgrid[i][j].dflags & DLRG_PROTECTED) {
				continue;
			}
			if ((dgrid[i][j].dungeon == 2 || dgrid[i][j].dungeon == 5) && dgrid[i][j - 1].dungeon == 3 && dgrid[i][j + 1].dungeon == 3) {
				doorok = false;
				while (1) {
					if (dgrid[i][j].dungeon != 2 && dgrid[i][j].dungeon != 5) {
						break;
					}
					if (dgrid[i][j - 1].dungeon != 3 || dgrid[i][j + 1].dungeon != 3) {
						break;
					}
					if (dgrid[i][j].dungeon == 5) {
						doorok = true;
					}
					i++;
				}
				if (!doorok && !(dgrid[i - 1][j].dflags & DLRG_PROTECTED)) {
					dgrid[i - 1][j].dungeon = 5;
				}
			}
		}
	}
	for (j = 1; j < DMAXX - 1; j++) { /* check: might be flipped */
		for (i = 1; i < DMAXY - 1; i++) {
			if (dgrid[j][i].dflags & DLRG_PROTECTED) {
				continue;
			}
			if ((dgrid[j][i].dungeon == 1 || dgrid[j][i].dungeon == 4) && dgrid[j - 1][i].dungeon == 3 && dgrid[j + 1][i].dungeon == 3) {
				doorok = false;
				while (1) {
					if (dgrid[j][i].dungeon != 1 && dgrid[j][i].dungeon != 4) {
						break;
					}
					if (dgrid[j - 1][i].dungeon != 3 || dgrid[j + 1][i].dungeon != 3) {
						break;
					}
					if (dgrid[j][i].dungeon == 4) {
						doorok = true;
					}
					i++;
				}
				if (!doorok && !(dgrid[j][i - 1].dflags & DLRG_PROTECTED)) {
					dgrid[j][i - 1].dungeon = 4;
				}
			}
		}
	}
}

void L2DoorFix()
{
	int i, j;

	for (j = 1; j < DMAXY; j++) {
		for (i = 1; i < DMAXX; i++) {
			if (dgrid[i][j].dungeon == 4 && dgrid[i][j - 1].dungeon == 3) {
				dgrid[i][j].dungeon = 7;
			}
			if (dgrid[i][j].dungeon == 5 && dgrid[i - 1][j].dungeon == 3) {
				dgrid[i][j].dungeon = 9;
			}
		}
	}
}

static void DRLG_L2(int entry)
{
	int i, j;
	bool doneflag;

	doneflag = false;
	while (!doneflag) {
		nRoomCnt = 0;
		InitDungeon();
		DRLG_InitTrans();
		if (!CreateDungeon()) {
			continue;
		}
		L2TileFix();
		if (setloadflag) {
			DRLG_L2SetRoom(nSx1, nSy1);
		}
		DRLG_L2FloodTVal();
		DRLG_L2TransFix();
		if (entry == 0) {
			doneflag = DRLG_L2PlaceMiniSet(USTAIRS, 1, 1, -1, -1, 1, 0);
			if (doneflag) {
				doneflag = DRLG_L2PlaceMiniSet(DSTAIRS, 1, 1, -1, -1, 0, 1);
				if (doneflag && lvl.currlevel == 5) {
					doneflag = DRLG_L2PlaceMiniSet(WARPSTAIRS, 1, 1, -1, -1, 0, 6);
				}
			}
			View.y -= 2;
		} else if (entry == 1) {
			doneflag = DRLG_L2PlaceMiniSet(USTAIRS, 1, 1, -1, -1, 0, 0);
			if (doneflag) {
				doneflag = DRLG_L2PlaceMiniSet(DSTAIRS, 1, 1, -1, -1, 1, 1);
				if (doneflag && lvl.currlevel == 5) {
					doneflag = DRLG_L2PlaceMiniSet(WARPSTAIRS, 1, 1, -1, -1, 0, 6);
				}
			}
			View.x--;
		} else {
			doneflag = DRLG_L2PlaceMiniSet(USTAIRS, 1, 1, -1, -1, 0, 0);
			if (doneflag) {
				doneflag = DRLG_L2PlaceMiniSet(DSTAIRS, 1, 1, -1, -1, 0, 1);
				if (doneflag && lvl.currlevel == 5) {
					doneflag = DRLG_L2PlaceMiniSet(WARPSTAIRS, 1, 1, -1, -1, 1, 6);
				}
			}
			View.y -= 2;
		}
	}

	L2LockoutFix();
	L2DoorFix();
	L2DirtFix();

	DRLG_PlaceThemeRooms(6, 10, 3, 0, 0);
	DRLG_L2PlaceRndSet(CTRDOOR1, 100);
	DRLG_L2PlaceRndSet(CTRDOOR2, 100);
	DRLG_L2PlaceRndSet(CTRDOOR3, 100);
	DRLG_L2PlaceRndSet(CTRDOOR4, 100);
	DRLG_L2PlaceRndSet(CTRDOOR5, 100);
	DRLG_L2PlaceRndSet(CTRDOOR6, 100);
	DRLG_L2PlaceRndSet(CTRDOOR7, 100);
	DRLG_L2PlaceRndSet(CTRDOOR8, 100);
	DRLG_L2PlaceRndSet(VARCH33, 100);
	DRLG_L2PlaceRndSet(VARCH34, 100);
	DRLG_L2PlaceRndSet(VARCH35, 100);
	DRLG_L2PlaceRndSet(VARCH36, 100);
	DRLG_L2PlaceRndSet(VARCH37, 100);
	DRLG_L2PlaceRndSet(VARCH38, 100);
	DRLG_L2PlaceRndSet(VARCH39, 100);
	DRLG_L2PlaceRndSet(VARCH40, 100);
	DRLG_L2PlaceRndSet(VARCH1, 100);
	DRLG_L2PlaceRndSet(VARCH2, 100);
	DRLG_L2PlaceRndSet(VARCH3, 100);
	DRLG_L2PlaceRndSet(VARCH4, 100);
	DRLG_L2PlaceRndSet(VARCH5, 100);
	DRLG_L2PlaceRndSet(VARCH6, 100);
	DRLG_L2PlaceRndSet(VARCH7, 100);
	DRLG_L2PlaceRndSet(VARCH8, 100);
	DRLG_L2PlaceRndSet(VARCH9, 100);
	DRLG_L2PlaceRndSet(VARCH10, 100);
	DRLG_L2PlaceRndSet(VARCH11, 100);
	DRLG_L2PlaceRndSet(VARCH12, 100);
	DRLG_L2PlaceRndSet(VARCH13, 100);
	DRLG_L2PlaceRndSet(VARCH14, 100);
	DRLG_L2PlaceRndSet(VARCH15, 100);
	DRLG_L2PlaceRndSet(VARCH16, 100);
	DRLG_L2PlaceRndSet(VARCH17, 100);
	DRLG_L2PlaceRndSet(VARCH18, 100);
	DRLG_L2PlaceRndSet(VARCH19, 100);
	DRLG_L2PlaceRndSet(VARCH20, 100);
	DRLG_L2PlaceRndSet(VARCH21, 100);
	DRLG_L2PlaceRndSet(VARCH22, 100);
	DRLG_L2PlaceRndSet(VARCH23, 100);
	DRLG_L2PlaceRndSet(VARCH24, 100);
	DRLG_L2PlaceRndSet(VARCH25, 100);
	DRLG_L2PlaceRndSet(VARCH26, 100);
	DRLG_L2PlaceRndSet(VARCH27, 100);
	DRLG_L2PlaceRndSet(VARCH28, 100);
	DRLG_L2PlaceRndSet(VARCH29, 100);
	DRLG_L2PlaceRndSet(VARCH30, 100);
	DRLG_L2PlaceRndSet(VARCH31, 100);
	DRLG_L2PlaceRndSet(VARCH32, 100);
	DRLG_L2PlaceRndSet(HARCH1, 100);
	DRLG_L2PlaceRndSet(HARCH2, 100);
	DRLG_L2PlaceRndSet(HARCH3, 100);
	DRLG_L2PlaceRndSet(HARCH4, 100);
	DRLG_L2PlaceRndSet(HARCH5, 100);
	DRLG_L2PlaceRndSet(HARCH6, 100);
	DRLG_L2PlaceRndSet(HARCH7, 100);
	DRLG_L2PlaceRndSet(HARCH8, 100);
	DRLG_L2PlaceRndSet(HARCH9, 100);
	DRLG_L2PlaceRndSet(HARCH10, 100);
	DRLG_L2PlaceRndSet(HARCH11, 100);
	DRLG_L2PlaceRndSet(HARCH12, 100);
	DRLG_L2PlaceRndSet(HARCH13, 100);
	DRLG_L2PlaceRndSet(HARCH14, 100);
	DRLG_L2PlaceRndSet(HARCH15, 100);
	DRLG_L2PlaceRndSet(HARCH16, 100);
	DRLG_L2PlaceRndSet(HARCH17, 100);
	DRLG_L2PlaceRndSet(HARCH18, 100);
	DRLG_L2PlaceRndSet(HARCH19, 100);
	DRLG_L2PlaceRndSet(HARCH20, 100);
	DRLG_L2PlaceRndSet(HARCH21, 100);
	DRLG_L2PlaceRndSet(HARCH22, 100);
	DRLG_L2PlaceRndSet(HARCH23, 100);
	DRLG_L2PlaceRndSet(HARCH24, 100);
	DRLG_L2PlaceRndSet(HARCH25, 100);
	DRLG_L2PlaceRndSet(HARCH26, 100);
	DRLG_L2PlaceRndSet(HARCH27, 100);
	DRLG_L2PlaceRndSet(HARCH28, 100);
	DRLG_L2PlaceRndSet(HARCH29, 100);
	DRLG_L2PlaceRndSet(HARCH30, 100);
	DRLG_L2PlaceRndSet(HARCH31, 100);
	DRLG_L2PlaceRndSet(HARCH32, 100);
	DRLG_L2PlaceRndSet(HARCH33, 100);
	DRLG_L2PlaceRndSet(HARCH34, 100);
	DRLG_L2PlaceRndSet(HARCH35, 100);
	DRLG_L2PlaceRndSet(HARCH36, 100);
	DRLG_L2PlaceRndSet(HARCH37, 100);
	DRLG_L2PlaceRndSet(HARCH38, 100);
	DRLG_L2PlaceRndSet(HARCH39, 100);
	DRLG_L2PlaceRndSet(HARCH40, 100);
	DRLG_L2PlaceRndSet(CRUSHCOL, 99);
	DRLG_L2PlaceRndSet(RUINS1, 10);
	DRLG_L2PlaceRndSet(RUINS2, 10);
	DRLG_L2PlaceRndSet(RUINS3, 10);
	DRLG_L2PlaceRndSet(RUINS4, 10);
	DRLG_L2PlaceRndSet(RUINS5, 10);
	DRLG_L2PlaceRndSet(RUINS6, 10);
	DRLG_L2PlaceRndSet(RUINS7, 50);
	DRLG_L2PlaceRndSet(PANCREAS1, 1);
	DRLG_L2PlaceRndSet(PANCREAS2, 1);
	DRLG_L2PlaceRndSet(BIG1, 3);
	DRLG_L2PlaceRndSet(BIG2, 3);
	DRLG_L2PlaceRndSet(BIG3, 3);
	DRLG_L2PlaceRndSet(BIG4, 3);
	DRLG_L2PlaceRndSet(BIG5, 3);
	DRLG_L2PlaceRndSet(BIG6, 20);
	DRLG_L2PlaceRndSet(BIG7, 20);
	DRLG_L2PlaceRndSet(BIG8, 3);
	DRLG_L2PlaceRndSet(BIG9, 20);
	DRLG_L2PlaceRndSet(BIG10, 20);
	DRLG_L2Subs();
	DRLG_L2Shadows();

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			dgrid[i][j].pdungeon = dgrid[i][j].dungeon;
		}
	}

	DRLG_Init_Globals();
	DRLG_CheckQuests(nSx1, nSy1);
}

static void DRLG_InitL2Vals()
{
	int i, j, pc;

	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++) {
			if (grid[i][j].getPiece() == 541) {
				pc = 5;
			} else if (grid[i][j].getPiece() == 178) {
				pc = 5;
			} else if (grid[i][j].getPiece() == 551) {
				pc = 5;
			} else if (grid[i][j].getPiece() == 542) {
				pc = 6;
			} else if (grid[i][j].getPiece() == 553) {
				pc = 6;
			} else if (grid[i][j].getPiece() == 13) {
				pc = 5;
			} else if (grid[i][j].getPiece() == 17) {
				pc = 6;
			} else {
				continue;
			}
			grid[i][j].dSpecial = pc;
		}
	}
	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++) {
			if (grid[i][j].getPiece() == 132) {
				grid[i][j + 1].dSpecial = 2;
				grid[i][j + 2].dSpecial = 1;
			} else if (grid[i][j].getPiece() == 135 || grid[i][j].getPiece() == 139) {
				grid[i + 1][j].dSpecial = 3;
				grid[i + 2][j].dSpecial = 4;
			}
		}
	}
}

void LvlCatacombs::LoadL2Dungeon(char *sFileName, int vx, int vy)
{
	int i, j, rw, rh, pc;
	uint8_t *pLevelMap, *lm;

	InitDungeon();
	DRLG_InitTrans();
	pLevelMap = LoadFileInMem(sFileName, NULL);

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			dgrid[i][j].dungeon = 12;
			dgrid[i][j].dflags = 0;
		}
	}

	lm = pLevelMap;
	rw = *lm;
	lm += 2;
	rh = *lm;
	lm += 2;

	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*lm != 0) {
				dgrid[i][j].dungeon = *lm;
				dgrid[i][j].dflags |= DLRG_PROTECTED;
			} else {
				dgrid[i][j].dungeon = 3;
			}
			lm += 2;
		}
	}
	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dgrid[i][j].dungeon == 0) {
				dgrid[i][j].dungeon = 12;
			}
		}
	}

	DRLG_L2Pass3();
	DRLG_Init_Globals();

	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++) {
			pc = 0;
			if (grid[i][j].getPiece() == 541) {
				pc = 5;
			}
			if (grid[i][j].getPiece() == 178) {
				pc = 5;
			}
			if (grid[i][j].getPiece() == 551) {
				pc = 5;
			}
			if (grid[i][j].getPiece() == 542) {
				pc = 6;
			}
			if (grid[i][j].getPiece() == 553) {
				pc = 6;
			}
			if (grid[i][j].getPiece() == 13) {
				pc = 5;
			}
			if (grid[i][j].getPiece() == 17) {
				pc = 6;
			}
			grid[i][j].dSpecial = pc;
		}
	}
	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++) {
			if (grid[i][j].getPiece() == 132) {
				grid[i][j + 1].dSpecial = 2;
				grid[i][j + 2].dSpecial = 1;
			} else if (grid[i][j].getPiece() == 135 || grid[i][j].getPiece() == 139) {
				grid[i + 1][j].dSpecial = 3;
				grid[i + 2][j].dSpecial = 4;
			}
		}
	}

	View.x = vx;
	View.y = vy;
	SetMapMonsters(pLevelMap, { 0, 0 });
	SetMapObjects(pLevelMap, { 0, 0 });
	mem_free_dbg(pLevelMap);
}

void LoadPreL2Dungeon(char *sFileName, int vx, int vy)
{
	int i, j, rw, rh;
	uint8_t *pLevelMap, *lm;

	InitDungeon();
	DRLG_InitTrans();
	pLevelMap = LoadFileInMem(sFileName, NULL);

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			dgrid[i][j].dungeon = 12;
			dgrid[i][j].dflags = 0;
		}
	}

	lm = pLevelMap;
	rw = *lm;
	lm += 2;
	rh = *lm;
	lm += 2;

	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*lm != 0) {
				dgrid[i][j].dungeon = *lm;
				dgrid[i][j].dflags |= DLRG_PROTECTED;
			} else {
				dgrid[i][j].dungeon = 3;
			}
			lm += 2;
		}
	}
	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dgrid[i][j].dungeon == 0) {
				dgrid[i][j].dungeon = 12;
			}
		}
	}
	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			dgrid[i][j].pdungeon = dgrid[i][j].dungeon;
		}
	}

	mem_free_dbg(pLevelMap);
}

void LvlCatacombs::CreateL2Dungeon(DWORD rseed, int entry)
{
	if (game.isSingleplayer()) {
		if (lvl.currlevel == 7 && !quests[QuestId::blind]._qactive) {
			lvl.currlevel = 6;
			CreateL2Dungeon(glSeedTbl[6], 4);
			lvl.currlevel = 7;
		}
		if (lvl.currlevel == 8) {
			if (!quests[QuestId::blind]._qactive) {
				lvl.currlevel = 6;
				CreateL2Dungeon(glSeedTbl[6], 4);
				lvl.currlevel = 8;
			} else {
				lvl.currlevel = 7;
				CreateL2Dungeon(glSeedTbl[7], 4);
				lvl.currlevel = 8;
			}
		}
	}

	SetRndSeed(rseed);

	lvl.dmin = { 16, 16 };
	lvl.dmax = { 96, 96 };

	DRLG_InitTrans();
	DRLG_InitSetPC();
	DRLG_LoadL2SP();
	DRLG_L2(entry);
	DRLG_L2Pass3();
	DRLG_FreeL2SP();
	DRLG_InitL2Vals();
	DRLG_SetPC();
}

}
