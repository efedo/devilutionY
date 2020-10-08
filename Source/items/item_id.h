/**
 * @file item_id.h
 *
 * Item id
 */

#ifndef __ITEM_ID_H__
#define __ITEM_ID_H__

namespace dvl {

class ItemID {
public:
	ItemID(uint64_t newid);
	ItemID(const ItemID &rhs);
	ItemID &operator=(const ItemID &rhs);
	friend bool operator==(const ItemID &lhs, const ItemID &rhs);
	friend bool operator>(const ItemID &lhs, const ItemID &rhs);
	friend bool operator<(const ItemID &lhs, const ItemID &rhs);
private:
	uint64_t id = 0;
};

}

#endif /* __ITEM_ID_H__ */
