/**
 * @file drlg_l4.h
 *
 * Interface of the hell level generation algorithms.
 */
#ifndef __DRLG_L4_H__
#define __DRLG_L4_H__

DEVILUTION_BEGIN_NAMESPACE

class LvlHell : public Level {
	LvlHell();
	void create(int lvldir) override;
	void loadGFX() override;
	void CreateL4Dungeon(DWORD rseed, int entry);
	void LoadL4Dungeon(char *sFileName, int vx, int vy);
	void DRLG_L4Pass3();

	void DRLG_CreateThemeRoom(int themeIndex) override;
};

extern int diabquad1x;
extern int diabquad1y;
extern int diabquad2x;
extern int diabquad2y;
extern int diabquad3x;
extern int diabquad3y;
extern int diabquad4x;
extern int diabquad4y;
extern bool hallok[20];
extern int l4holdx;
extern int l4holdy;
extern int SP4x1;
extern int SP4y1;
extern int SP4x2;
extern int SP4y2;
extern uint8_t L4dungeon[80][80];
extern uint8_t dung[20][20];
//int dword_52A4DC;

void LoadPreL4Dungeon(char *sFileName, int vx, int vy);
void LoadL4Dungeon(char *sFileName, int vx, int vy);

void DRLG_LoadL4SP();
void DRLG_FreeL4SP();
void DRLG_L4SetSPRoom(int rx1, int ry1);
void L4SaveQuads();
void DRLG_L4SetRoom(uint8_t *pSetPiece, int rx1, int ry1);
void DRLG_LoadDiabQuads(bool preflag);
bool IsDURWall(char d);
bool IsDLLWall(char dd);
void L4FixRim();
void DRLG_L4GeneralFix();
void CreateL4Dungeon(DWORD rseed, int entry);

/* rdata */
extern const uint8_t L4ConvTbl[16];
extern const uint8_t L4USTAIRS[42];
extern const uint8_t L4TWARP[42];
extern const uint8_t L4DSTAIRS[52];
extern const uint8_t L4PENTA[52];
extern const uint8_t L4PENTA2[52];
extern const uint8_t L4BTYPES[140];

//#ifdef __cplusplus
//}
//#endif

DEVILUTION_END_NAMESPACE

#endif /* __DRLG_L4_H__ */
