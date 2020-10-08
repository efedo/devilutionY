/**
 * @file item_manager.cpp
 *
 * Item manager
 */

#include "all.h"

namespace dvl {

ItemManager::ItemManager()
{
}

ItemManager::~ItemManager()
{
}

bool ItemManager::exists(const ItemID id)
{
	return (list.count(id));
}

Item * ItemManager::get(const ItemID id)
{
	if (!list.count(id)) return 0;
	return list.at(id);
}

void ItemManager::addItem(const ItemID id, Item * item)
{
	if (!list.count(id)) app_fatal("Duplicated item id %d", id);
	list.emplace(id, Item(id));
}

void ItemManager::destroyItem(const ItemID id)
{
	if (!list.count(id)) app_fatal("Illegal item %d", id);
	list.erase(id);
}

}
