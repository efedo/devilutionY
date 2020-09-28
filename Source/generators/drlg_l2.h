/**
 * @file drlg_l2.h
 *
 * Interface of the catacombs level generation algorithms.
 */
#ifndef __DRLG_L2_H__
#define __DRLG_L2_H__

DEVILUTION_BEGIN_NAMESPACE

class LvlCatacombs : public Level {
	LvlCatacombs();
	void create(int lvldir) override;
	void loadGFX() override;
	void CreateL2Dungeon(DWORD rseed, int entry);
	void LoadL2Dungeon(char *sFileName, int vx, int vy);
	void DRLG_L2Pass3();

	void DRLG_CreateThemeRoom(int themeIndex) override;
};

extern int nSx1;
extern int nSy1;
extern int nSx2;
extern int nSy2;
extern int nRoomCnt;
extern uint8_t predungeon[DMAXX][DMAXY];
extern ROOMNODE RoomList[81];
extern HALLNODE *pHallList;

void InitDungeon();
void L2LockoutFix();
void L2DoorFix();
void LoadL2Dungeon(char *sFileName, int vx, int vy);
void LoadPreL2Dungeon(char *sFileName, int vx, int vy);
void CreateL2Dungeon(DWORD rseed, int entry);

/* rdata */
extern int Area_Min;
extern int Room_Max;
extern int Room_Min;
extern int Dir_Xadd[5];
extern int Dir_Yadd[5];
extern ShadowStruct SPATSL2[2];
//short word_48489A;
extern uint8_t BTYPESL2[161];
extern uint8_t BSTYPESL2[161];
extern uint8_t VARCH1[];
extern uint8_t VARCH2[];
extern uint8_t VARCH3[];
extern uint8_t VARCH4[];
extern uint8_t VARCH5[];
extern uint8_t VARCH6[];
extern uint8_t VARCH7[];
extern uint8_t VARCH8[];
extern uint8_t VARCH9[];
extern uint8_t VARCH10[];
extern uint8_t VARCH11[];
extern uint8_t VARCH12[];
extern uint8_t VARCH13[];
extern uint8_t VARCH14[];
extern uint8_t VARCH15[];
extern uint8_t VARCH16[];
extern uint8_t VARCH17[];
extern uint8_t VARCH18[];
extern uint8_t VARCH19[];
extern uint8_t VARCH20[];
extern uint8_t VARCH21[];
extern uint8_t VARCH22[];
extern uint8_t VARCH23[];
extern uint8_t VARCH24[];
extern uint8_t VARCH25[];
extern uint8_t VARCH26[];
extern uint8_t VARCH27[];
extern uint8_t VARCH28[];
extern uint8_t VARCH29[];
extern uint8_t VARCH30[];
extern uint8_t VARCH31[];
extern uint8_t VARCH32[];
extern uint8_t VARCH33[];
extern uint8_t VARCH34[];
extern uint8_t VARCH35[];
extern uint8_t VARCH36[];
extern uint8_t VARCH37[];
extern uint8_t VARCH38[];
extern uint8_t VARCH39[];
extern uint8_t VARCH40[];
extern uint8_t HARCH1[];
extern uint8_t HARCH2[];
extern uint8_t HARCH3[];
extern uint8_t HARCH4[];
extern uint8_t HARCH5[];
extern uint8_t HARCH6[];
extern uint8_t HARCH7[];
extern uint8_t HARCH8[];
extern uint8_t HARCH9[];
extern uint8_t HARCH10[];
extern uint8_t HARCH11[];
extern uint8_t HARCH12[];
extern uint8_t HARCH13[];
extern uint8_t HARCH14[];
extern uint8_t HARCH15[];
extern uint8_t HARCH16[];
extern uint8_t HARCH17[];
extern uint8_t HARCH18[];
extern uint8_t HARCH19[];
extern uint8_t HARCH20[];
extern uint8_t HARCH21[];
extern uint8_t HARCH22[];
extern uint8_t HARCH23[];
extern uint8_t HARCH24[];
extern uint8_t HARCH25[];
extern uint8_t HARCH26[];
extern uint8_t HARCH27[];
extern uint8_t HARCH28[];
extern uint8_t HARCH29[];
extern uint8_t HARCH30[];
extern uint8_t HARCH31[];
extern uint8_t HARCH32[];
extern uint8_t HARCH33[];
extern uint8_t HARCH34[];
extern uint8_t HARCH35[];
extern uint8_t HARCH36[];
extern uint8_t HARCH37[];
extern uint8_t HARCH38[];
extern uint8_t HARCH39[];
extern uint8_t HARCH40[];
extern uint8_t USTAIRS[];
extern uint8_t DSTAIRS[];
extern uint8_t WARPSTAIRS[];
extern uint8_t CRUSHCOL[];
extern uint8_t BIG1[];
extern uint8_t BIG2[];
extern uint8_t BIG3[];
extern uint8_t BIG4[];
extern uint8_t BIG5[];
extern uint8_t BIG6[];
extern uint8_t BIG7[];
extern uint8_t BIG8[];
extern uint8_t BIG9[];
extern uint8_t BIG10[];
extern uint8_t RUINS1[];
extern uint8_t RUINS2[];
extern uint8_t RUINS3[];
extern uint8_t RUINS4[];
extern uint8_t RUINS5[];
extern uint8_t RUINS6[];
extern uint8_t RUINS7[];
extern uint8_t PANCREAS1[];
extern uint8_t PANCREAS2[];
extern uint8_t CTRDOOR1[];
extern uint8_t CTRDOOR2[];
extern uint8_t CTRDOOR3[];
extern uint8_t CTRDOOR4[];
extern uint8_t CTRDOOR5[];
extern uint8_t CTRDOOR6[];
extern uint8_t CTRDOOR7[];
extern uint8_t CTRDOOR8[];
extern int Patterns[100][10];

//#ifdef __cplusplus
//}
//#endif

DEVILUTION_END_NAMESPACE

#endif /* __DRLG_L2_H__ */
