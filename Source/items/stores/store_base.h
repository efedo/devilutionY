// HEADER_GOES_HERE
#ifndef __StoreTalkId::BASE_H__
#define __StoreTalkId::BASE_H__

namespace dvl {

class Store
{
   public:

	// GUI

	// Overridden functions
	virtual void S_Start();
	virtual void S_Enter();
	virtual void Spawn(int lvl);
	//virtual int RndItem(int lvl);

	// Buy
	virtual void BuyItem();
	virtual void S_StartBuy();
	virtual void S_BuyEnter();
	virtual void S_ScrollBuy(int idx);
	virtual void SortItems();

	// Item check
	virtual bool ItemOk(int i);

	std::vector<ItemStruct> items;
};

void S_StartSIdentify();
void S_StartIdShow();
void S_StartTalk();
void S_StartNoMoney();
void S_StartNoRoom();
void S_StartConfirm();
void S_StartStory();
void S_ConfirmEnter();
void S_StoryEnter();
void S_SIDEnter();
void S_TalkEnter();
void STextEnter();




}

#endif /* __StoreTalkId::BASE_H__ */
