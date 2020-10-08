// HEADER_GOES_HERE
#ifndef __StoreTalkId::BOY_H__
#define __StoreTalkId::BOY_H__

namespace dvl {

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

}

#endif /* __StoreTalkId::BOY_H__ */
