// HEADER_GOES_HERE
#ifndef __STORE_HEALER_H__
#define __STORE_HEALER_H__

DEVILUTION_BEGIN_NAMESPACE

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

DEVILUTION_END_NAMESPACE

#endif /* __STORE_HEALER_H__ */
