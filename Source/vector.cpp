#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

V2Di::V2Di(int nx, int ny)
	: x(nx)
	, y(ny)
{
}

int V2Di::maxdim()
{
	return ::dvl::maxdim(*this);
}

V2Di operator+(const V2Di &lhs, const V2Di &rhs)
{
	return V2Di(lhs.x + rhs.y, lhs.y + rhs.y);
}

V2Di operator-(const V2Di &lhs, const V2Di &rhs)
{
	return V2Di(lhs.x - rhs.y, lhs.y - rhs.y);
}

V2Di operator-(const V2Di &lhs)
{
	return V2Di(-lhs.x, -lhs.y);
}

V2Di &operator+=(V2Di &lhs, const V2Di &rhs)
{
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	return lhs;
}

V2Di &operator-=(V2Di &lhs, const V2Di &rhs)
{
	lhs.x -= rhs.x;
	lhs.y -= rhs.y;
	return lhs;
}

bool operator==(const V2Di &lhs, const V2Di &rhs)
{
	if (lhs.x != rhs.x) return false;
	if (lhs.y != rhs.y) return false;
	return true;
}

bool operator!=(const V2Di &lhs, const V2Di &rhs)
{
	return (!(lhs == rhs));
}

V2Di abs(const V2Di &lhs) {
	return V2Di(::abs(lhs.x), ::abs(lhs.y));
}

int maxdim(const V2Di &lhs)
{
	return (lhs.x > lhs.y) ? lhs.x : lhs.y;
}

DEVILUTION_END_NAMESPACE
