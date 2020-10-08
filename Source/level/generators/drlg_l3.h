/**
 * @file drlg_l3.h
 *
 * Interface of the caves level generation algorithms.
 */
#ifndef __DRLG_L3_H__
#define __DRLG_L3_H__

namespace dvl {

class LvlCaves : public Level {
	LvlCaves();
	void create(int lvldir) override;
	void loadGFX() override;
	void LoadSetMap() override;
	void CreateL3Dungeon(DWORD rseed, int entry);
	void LoadL3Dungeon(char *sFileName, int vx, int vy);
	void DRLG_L3Pass3();

	void DRLG_CreateThemeRoom(int themeIndex) override;
};


extern bool lavapool;
extern int abyssx;
extern int lockoutcnt;
extern bool lockout[DMAXX][DMAXY];

void AddFenceDoors();
void FenceDoorFix();
bool DRLG_L3Anvil();
void FixL3Warp();
void FixL3HallofHeroes();
void DRLG_L3LockRec(int x, int y);
bool DRLG_L3Lockout();
void LoadPreL3Dungeon(char *sFileName, int vx, int vy);

/* rdata */
extern const uint8_t L3ConvTbl[16];
extern const uint8_t L3UP[20];
extern const uint8_t L3DOWN[20];
extern const uint8_t L3HOLDWARP[20];
extern const uint8_t L3TITE1[34];
extern const uint8_t L3TITE2[34];
extern const uint8_t L3TITE3[34];
extern const uint8_t L3TITE6[42];
extern const uint8_t L3TITE7[42];
extern const uint8_t L3TITE8[20];
extern const uint8_t L3TITE9[20];
extern const uint8_t L3TITE10[20];
extern const uint8_t L3TITE11[20];
extern const uint8_t L3TITE12[6];
extern const uint8_t L3TITE13[6];
extern const uint8_t L3CREV1[6];
extern const uint8_t L3CREV2[6];
extern const uint8_t L3CREV3[6];
extern const uint8_t L3CREV4[6];
extern const uint8_t L3CREV5[6];
extern const uint8_t L3CREV6[6];
extern const uint8_t L3CREV7[6];
extern const uint8_t L3CREV8[6];
extern const uint8_t L3CREV9[6];
extern const uint8_t L3CREV10[6];
extern const uint8_t L3CREV11[6];
extern const uint8_t L3ISLE1[14];
extern const uint8_t L3ISLE2[14];
extern const uint8_t L3ISLE3[14];
extern const uint8_t L3ISLE4[14];
extern const uint8_t L3ISLE5[10];
extern const uint8_t L3XTRA1[4];
extern const uint8_t L3XTRA2[4];
extern const uint8_t L3XTRA3[4];
extern const uint8_t L3XTRA4[4];
extern const uint8_t L3XTRA5[4];
extern const uint8_t L3ANVIL[244];

//#ifdef __cplusplus
//}
//#endif

}

#endif /* __DRLG_L3_H__ */
