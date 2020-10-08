// HEADER_GOES_HERE
#ifndef __StoreTalkId::HEALER_H__
#define __StoreTalkId::HEALER_H__

namespace dvl {

class StoreHealer : public Store
{
   public:
	virtual void S_Start();
	virtual void S_Enter();
	virtual void Spawn(int lvl);
	static int RndItem(int lvl);

	// Buy
	virtual void BuyItem();
	virtual void S_StartBuy();
	virtual void S_BuyEnter();
	virtual void S_ScrollBuy(int idx);
	virtual void SortItems();

	// Item check
	virtual bool ItemOk(int i);

	ItemStruct items[20];
};

extern StoreHealer healerStore;

}

#endif /* __StoreTalkId::HEALER_H__ */
