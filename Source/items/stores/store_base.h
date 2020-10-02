// HEADER_GOES_HERE
#ifndef __STORE_BASE_H__
#define __STORE_BASE_H__

DEVILUTION_BEGIN_NAMESPACE

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




DEVILUTION_END_NAMESPACE

#endif /* __STORE_BASE_H__ */
