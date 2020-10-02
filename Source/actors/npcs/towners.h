//HEADER_GOES_HERE
#ifndef __TOWNERS_H__
#define __TOWNERS_H__

#include "npc.h"

DEVILUTION_BEGIN_NAMESPACE

class Towner : public Npc, public TownerStruct
{
   public:
	Towner(int);
	virtual void Init();
	void NewTownerAnim(uint8_t *pAnim, int numFrames,
	                           int Delay);
	void InitTownerInfo(int w, int sel, int t, int x, int y,
	                            int ao, int tp);
	void InitQstSnds();
	virtual void TownCtrlMsg();
	void advanceAnim();
	const int mytnum = 0; // original hard-coded towner id num
};

class Smith : public Towner
{
   public:
	Smith() : Towner(0) {}
	void Init() final;
};

class BarOwner : public Towner
{
   public:
	BarOwner() : Towner(3) {}
	void Init() final;
};

class DeadGuy : public Towner
{
   public:
	DeadGuy() : Towner(2) {}
	void Init() final;
	void TownCtrlMsg() final;
};

class Witch : public Towner
{
   public:
	Witch() : Towner(6) {}
	void Init() final;
};

class Barmaid : public Towner
{
   public:
	Barmaid() : Towner(7) {}
	void Init() final;
};

class Boy : public Towner
{
   public:
	Boy() : Towner(8) {}
	void Init() final;
};

class Healer : public Towner
{
   public:
	Healer() : Towner(1) {}
	void Init() final;
};

class Teller : public Towner
{
   public:
	Teller() : Towner(4) {}
	void Init() final;
};

class Drunk : public Towner
{
   public:
	Drunk() : Towner(5) {}
	void Init() final;
};

class Cow : public Towner
{
   public:
	Cow() : Towner(9) {}
	void Init() final;
	void CowSFX(int pnum);
};

extern Smith smith;
extern BarOwner barowner;
extern DeadGuy deadguy;
extern Witch witch;
extern Barmaid barmaid;
extern Boy boy;
extern Healer healer;
extern Teller teller;
extern Drunk drunk;
extern Cow cow[3];

void SetTownerGPtrs(uint8_t *pData, uint8_t **pAnim);
void InitCows();
void InitTowners();
void FreeTownerGFX();
void ProcessTowners();
void TownerTalk(int first, int t);
void TalkToTowner(int p, int t);

extern QuestTalkData Qtalklist[];

DEVILUTION_END_NAMESPACE

#endif /* __TOWNERS_H__ */
