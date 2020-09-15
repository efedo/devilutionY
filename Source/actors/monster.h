/**
 * @file monster.h
 *
 * Interface of monster functionality, AI, actions, spawning, loading, etc.
 */
#ifndef __MONSTER_H__
#define __MONSTER_H__

DEVILUTION_BEGIN_NAMESPACE

class MonsterType {
public:
	MonsterType(int new_monst = 0)
	{
	}
	void InitMonsterTRN(BOOL special);
	void InitMonsterGFX();
	CMonster data;
};

class MonsterCatalog {
public:
	MonsterCatalog() {}
	auto &operator[](size_t n)
	{
		return types[n];
	}

	void InitLevelMonsters();
	int AddMonsterType(int type, int placeflag);
	void GetLevelMTypes();

	BYTE GraphicTable[NUMLEVELS][MAX_LVLMTYPES]; // Stores all monster graphics
private:
	MonsterType types[MAX_LVLMTYPES];
};

extern MonsterCatalog beastiary;

class MonsterInstance {
public:
	MonsterInstance() {};
	MonsterInstance(int i, int rd, int mtype, int x, int y);
	void NewMonsterAnim(AnimStruct *anim, int md);
	void ClearMVars();

	void SyncMonsterAnim();

	void ActivateSpawn(int x, int y, int dir);
	void SpawnGolum(int x, int y, int mi);
	void TalktoMonster();

	BOOL DirOK(int mdir);
	BOOL M_Ranged();
	BOOL M_Talker();
	void M_Enemy();
	int M_GetDir();
	void M_CheckEFlag();
	void M_StartStand(int md);
	void M_StartDelay(int len);
	void M_StartSpStand(int md);
	void M_StartWalk(int xvel, int yvel, int xadd, int yadd, int EndDir);
	void M_StartWalk2(int xvel, int yvel, int xoff, int yoff, int xadd, int yadd, int EndDir);
	void M_StartWalk3(int xvel, int yvel, int xoff, int yoff, int xadd, int yadd, int mapx, int mapy, int EndDir);
	void M_StartAttack();
	void M_StartRAttack(int missile_type, int dam);
	void M_StartRSpAttack(int missile_type, int dam);
	void M_StartSpAttack();
	void M_StartEat();
	void M_ClearSquares();
	void M_GetKnockback();
	void M_StartHit(int pnum, int dam);
	void M_DiabloDeath(BOOL sendmsg);
	//void M2MStartHit(int mid, int i, int dam);
	void MonstStartKill(int pnum, BOOL sendmsg);
	void M2MStartKill(int mid);
	void M_StartKill(int pnum);
	void M_SyncStartKill(int x, int y, int pnum);
	void M_StartFadein(int md, BOOL backwards);
	void M_StartFadeout(int md, BOOL backwards);
	void M_StartHeal();
	void M_ChangeLightOffset();
	BOOL M_DoStand();
	BOOL M_DoWalk();
	BOOL M_DoWalk2();
	BOOL M_DoWalk3();
	void M_TryM2MHit(int mid, int hper, int mind, int maxd);
	void M_TryH2HHit(int pnum, int Hit, int MinDam, int MaxDam);
	BOOL M_DoAttack();
	BOOL M_DoRAttack();
	int M_DoRSpAttack();
	BOOL M_DoSAttack();
	BOOL M_DoFadein();
	BOOL M_DoFadeout();
	int M_DoHeal();
	int M_DoTalk();
	void M_Teleport();
	BOOL M_DoGotHit();
	void M_UpdateLeader();
	BOOL M_DoDeath();
	BOOL M_DoSpStand();
	BOOL M_DoDelay();
	BOOL M_DoStone();
	void M_WalkDir(int md);
	void GroupUnity();
	BOOL M_CallWalk(int md);
	BOOL M_PathWalk();
	BOOL M_CallWalk2(int md);
	BOOL M_DumbWalk(int md);
	BOOL M_RoundWalk(int md, int *dir);
	void MAI_Zombie();
	void MAI_SkelSd();
	BOOL MAI_Path();
	void MAI_Snake();
	void MAI_Bat();
	void MAI_SkelBow();
	void MAI_Fat();
	void MAI_Sneak();
	void MAI_Fireman();
	void MAI_Fallen();
	void MAI_Cleaver();
	void MAI_Round(BOOL special);
	void MAI_GoatMc();
	void MAI_Ranged(int missile_type, BOOL special);
	void MAI_GoatBow();
	void MAI_Succ();
	void MAI_AcidUniq();
	void MAI_Scav();
	void MAI_Garg();
	void MAI_RoundRanged(int missile_type, BOOL checkdoors, int dam, int lessmissiles);
	void MAI_Magma();
	void MAI_Storm();
	void MAI_Acid();
	void MAI_Diablo();
	void MAI_RR2(int mistype, int dam);
	void MAI_Mega();
	void MAI_Golum();
	void MAI_SkelKing();
	void MAI_Rhino();
	void MAI_Counselor();
	void MAI_Garbud();
	void MAI_Zhar();
	void MAI_SnotSpil();
	void MAI_Lazurus();
	void MAI_Lazhelp();
	void MAI_Lachdanan();
	void MAI_Warlord();

	MonsterStruct data;
private:
	int i;
};

extern int monstkills[MAXMONSTERS];
extern int monstactive[MAXMONSTERS];

class MonsterManager {
public:
	MonsterManager() {}
	MonsterInstance &operator[](size_t n);
	void PlaceMonster(int i, int mtype, int x, int y);
	void PlaceUniqueMonster(int uniqindex, int miniontype, int unpackfilesize);
	void PlaceQuestMonsters();
	void PlaceGroup(int mtype, int num, int leaderf, int leader);
	void DeleteMonster(int i);
	void ClrAllMonsters();
private:
	MonsterInstance list[MAXMONSTERS];
};

extern MonsterManager monsters;

extern int MissileFileFlag;
extern int nummonsters;
extern BOOLEAN sgbSaveSoundOn;
extern int totalmonsters;
extern int monstimgtot;
extern int uniquetrans;
extern int nummtypes;

void MissToMonst(int i, int x, int y);
BOOL MonstPlace(int xp, int yp);
void LoadDiabMonsts();
void InitMonsters();
void PlaceUniques();
void SetMapMonsters(BYTE *pMap, int startx, int starty);
int AddMonster(int x, int y, int dir, int mtype, BOOL InMap);

void DeleteMonsterList();
void ProcessMonsters();
void FreeMonsters();
BOOL PosOkMissile(int x, int y);
BOOL CheckNoSolid(int x, int y);
BOOL LineClearF(BOOL (*Clear)(int, int), int x1, int y1, int x2, int y2);
BOOL LineClear(int x1, int y1, int x2, int y2);
BOOL LineClearF1(BOOL (*Clear)(int, int, int), int monst, int x1, int y1, int x2, int y2);
void M_FallenFear(int x, int y);
void PrintMonstHistory(int mt);
void PrintUniqueHistory();
BOOL PosOkMonst(int i, int x, int y);
BOOL PosOkMonst2(int i, int x, int y);
BOOL PosOkMonst3(int i, int x, int y);
void M2MStartHit(int mid, int i, int dam);

BOOL IsSkel(int mt);
BOOL IsGoat(int mt);
BOOL CanTalkToMonst(int m);
BOOL CheckMonsterHit(int m, BOOL *ret);


int M_SpawnSkel(int x, int y, int dir);
BOOL SpawnSkeleton(int ii, int x, int y);
int PreSpawnSkeleton();

int encode_enemy(int m);
void decode_enemy(int m, int enemy);
void DoEnding();
void PrepDoEnding();

/* rdata */

extern const char plr2monst[9];
extern const BYTE counsmiss[4];

/* data */

extern int MWVel[24][3];
extern char animletter[7];
extern int left[8];
extern int right[8];
extern int opposite[8];
extern int offset_x[8];
extern int offset_y[8];

extern void (dvl::MonsterInstance:: *AiProc[])();

DEVILUTION_END_NAMESPACE

#endif /* __MONSTER_H__ */
