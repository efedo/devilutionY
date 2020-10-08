// HEADER_GOES_HERE
#ifndef __StoreTalkId::SMITH_H__
#define __StoreTalkId::SMITH_H__

namespace dvl {

class StoreSmith : public Store
{
   public:
	virtual void S_Start();
	virtual void S_Enter();
	virtual void Spawn(int lvl);
	static int RndItem(int lvl);
	void SpawnPremium(int lvl);
	void SpawnOnePremium(int i, int plvl);
	static int RndPremiumItem(int minlvl, int maxlvl);

	// Buy
	virtual void BuyItem();
	virtual void S_StartBuy();
	virtual void S_BuyEnter();
	virtual void S_ScrollBuy(int idx);
	virtual void SortItems();
	void BuyPremiumItem();
	bool S_StartPremiumBuy();
	void S_PremiumBuyEnter();
	void S_ScrollPremiumBuy(int idx);

	// Item check
	virtual bool ItemOk(int i);
	bool PremiumItemOk(int i);

	// Sell
	void S_StartSell();
	void S_SellEnter();
	bool SellOk(int i);
	void S_ScrollSell(int idx);

	// Repair
	void RepairItem();
	void S_RepairEnter();
	bool RepairOk(int i);
	void S_StartRepair();

	ItemStruct smithitem[SMITH_ITEMS];
	ItemStruct premiumitem[SMITH_PREMIUM_ITEMS];
	int premiumlevel;
	int numpremium;
};

extern StoreSmith smithStore;

}

#endif /* __StoreTalkId::SMITH_H__ */
