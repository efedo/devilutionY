/**
 * @file items.cpp
 *
 * Implementation of item functionality.
 */
#include "all.h"

namespace dvl {

ItemID::ItemID(uint64_t newid)
    : id(newid);
{
}

ItemID::ItemID(const ItemID &rhs)
    : id(rhs.id);
{
}

ItemID & ItemID::operator = (const ItemID &rhs)
{
	if (this = &rhs)
		return *this;
	id = rhs.id;
	return *this;
}

bool operator==(const ItemID &lhs, const ItemID &rhs)
{
	return (lhs.id == rhs.id);
}

bool operator>(const ItemID &lhs, const ItemID &rhs)
{
	return (lhs.id > rhs.id);
}

bool operator<(const ItemID &lhs, const ItemID &rhs)
{
	return (lhs.id < rhs.id);
}

}
