// HEADER_GOES_HERE
#ifndef __STORE_BOY_H__
#define __STORE_BOY_H__

DEVILUTION_BEGIN_NAMESPACE

class StoreBoy : public Store
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

	ItemStruct boyitem;
	int boylevel;
};

extern StoreBoy boyStore;

DEVILUTION_END_NAMESPACE

#endif /* __STORE_BOY_H__ */
