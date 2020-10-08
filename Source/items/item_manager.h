/**
 * @file item_manager.h
 *
 * Item manager
 */
#ifndef __ITEM_MANAGER_H__
#define __ITEM_MANAGER_H__

#include <unordered_map>

namespace dvl {

class ItemManager {
public:
	ItemManager();
	~ItemManager();
	bool exists(const ItemID id);
	Item * get(const ItemID id);
private:
	friend class Item;
	void ItemManager::addItem(const ItemID id, Item *item);
	void ItemManager::destroyItem(const ItemID id);
	std::unordered_map<ItemID, Item *> list;
};

extern ItemManager items;

}

#endif /* __ITEM_MANAGER_H__ */
