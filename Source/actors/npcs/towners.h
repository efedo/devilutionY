//HEADER_GOES_HERE
#ifndef __TOWNERS_H__
#define __TOWNERS_H__

#include "npc.h"
#include "towner_defs.h"
#include "towner_enums.h"

namespace dvl {

class Player;

class Towner : public Npc, public TownerStruct
{
public:
	Towner(int);
	~Towner();
	int numInstances = 0;
	virtual void Init();
	void NewTownerAnim(uint8_t *pAnim, int numFrames,
	                           int Delay);
	void InitTownerInfo(int w, int sel, TownerId t, int x, int y,
	                            int ao, int tp);
	void InitQstSnds();
	virtual void TalkToTowner(Player &player);
	void TownerTalk(int first); // takes  enum _speech_id
	virtual void TownCtrlMsg();
	void advanceAnim();
	const int mytnum = 0; // original hard-coded towner id num
};

class Smith : public Towner
{
public:
	Smith() : Towner(0) {}
	void Init() final;
	virtual void TalkToTowner(Player &player);
};

class BarOwner : public Towner
{
public:
	BarOwner() : Towner(3) {}
	void Init() final;
	virtual void TalkToTowner(Player &player);
};

class DeadGuy : public Towner
{
public:
	DeadGuy() : Towner(2) {}
	void Init() final;
	virtual void TalkToTowner(Player &player);
	void TownCtrlMsg() final;
};

class Witch : public Towner
{
public:
	Witch() : Towner(6) {}
	void Init() final;
	virtual void TalkToTowner(Player &player);
};

class Barmaid : public Towner
{
public:
	Barmaid() : Towner(7) {}
	void Init() final;
	virtual void TalkToTowner(Player &player);
};

class Boy : public Towner
{
public:
	Boy() : Towner(8) {}
	void Init() final;
	virtual void TalkToTowner(Player &player);
};

class Healer : public Towner
{
public:
	Healer() : Towner(1) {}
	void Init() final;
	virtual void TalkToTowner(Player &player);
};

class Teller : public Towner
{
public:
	Teller() : Towner(4) {}
	void Init() final;
	virtual void TalkToTowner(Player &player);
};

class Drunk : public Towner
{
public:
	Drunk() : Towner(5) {}
	void Init() final;
	virtual void TalkToTowner(Player &player);
};

class Cow : public Towner
{
public:
	Cow() : Towner(9) { CowPlaying = -1; }
	void Init() final;
	virtual void TalkToTowner(Player &player);
	void CowSFX(Player &player);
	static uint8_t *pCowCels;
	static int CowPlaying;
	static int sgnCowMsg;
	static DWORD sgdwCowClicks;
};

class Story : public Towner // virtual towner for storytelling
{
public:
	virtual void TalkToTowner(Player &player);
};

extern QuestTalkData Qtalklist[];

}

#endif /* __TOWNERS_H__ */
