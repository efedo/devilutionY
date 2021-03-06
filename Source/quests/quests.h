//HEADER_GOES_HERE
#ifndef __QUESTS_H__
#define __QUESTS_H__

namespace dvl {

extern int qtopline;
extern uint8_t *pQLogCel;
extern QuestStruct quests[MAXQUESTS];
extern int qline;
extern int qlist[MAXQUESTS];
extern int numqlines;
extern int WaterDone;
extern int ReturnLvlX;
extern int ReturnLvlY;
extern DunType ReturnLvlT;
extern int ReturnLvl;

void InitQuests();
void CheckQuests();
bool ForceQuests();
bool QuestStatus(QuestId i);
void CheckQuestKill(int m, bool sendmsg);
void DrawButcher();
void DrawSkelKing(int q, int x, int y);
void DrawWarLord(int x, int y);
void DrawSChamber(int q, int x, int y);
void DrawLTBanner(int x, int y);
void DrawBlind(int x, int y);
void DrawBlood(int x, int y);
void DRLG_CheckQuests(int x, int y);
void SetReturnLvlPos();
void GetReturnLvlPos();
void ResyncMPQuests();
void ResyncQuests();
void PrintQLString(int x, int y, bool cjustflag, char *str, int col);
void DrawQuestLog();
void StartQuestlog();
void QuestlogUp();
void QuestlogDown();
void QuestlogEnter();
void QuestlogESC();
void SetMultiQuest(int q, int s, int l, int v1);

/* rdata */
extern QuestData questlist[MAXQUESTS];
extern V2Di questoff[7];
extern char questyoff[7];
extern char *questtrigstr[5];
extern int QuestGroup1[3];
extern int QuestGroup2[3];
extern int QuestGroup3[3];
extern int QuestGroup4[2];

}

#endif /* __QUESTS_H__ */
