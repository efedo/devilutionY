// HEADER_GOES_HERE
#ifndef __StoreTalkId::WITCH_H__
#define __StoreTalkId::WITCH_H__

namespace dvl {

class StoreWitch : public Store
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

	// Sell
	void S_StartSell();
	void S_SellEnter();
	bool SellOk(int i);

	// Recharge
	void RechargeItem();
	void S_RechargeEnter();
	bool RechargeOk(int i);
	void AddStoreHoldRecharge(ItemStruct itm, int i);
	void S_StartRecharge();

	ItemStruct witchitem[20];
};

extern StoreWitch witchStore;

void WitchBookLevel(int ii);

}

#endif /* __StoreTalkId::WITCH_H__ */
