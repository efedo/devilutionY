#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

V2Di::V2Di()
{
}

V2Di::V2Di(int nx, int ny)
	: x(nx)
	, y(ny)
{
}

int V2Di::maxdim()
{
	return (x > y) ? x : y;
}

int V2Di::maxabs()
{
	return (::abs(x) > ::abs(y)) ? ::abs(x) : ::abs(y);
}

V2Di V2Di::abs()
{
	return V2Di(::abs(x), ::abs(y));
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

V2Di operator*(const int &lhs, const V2Di &rhs)
{
	return V2Di(rhs.x * lhs, rhs.y * lhs);
}

V2Di operator*(const V2Di &lhs, const int &rhs)
{
	return rhs * lhs;
}

V2Di operator/(const V2Di &lhs, const int &rhs)
{
	return V2Di(lhs.x / rhs, lhs.y / rhs);
}

V2Di operator>>(const V2Di &lhs, const int &rhs)
{
	return V2Di(lhs.x >> rhs, lhs.y >> rhs);
}


V2Di operator<<(const V2Di &lhs, const int &rhs)
{
	return V2Di(lhs.x << rhs, lhs.y << rhs);
}

V2Di &operator+=(V2Di &lhs, const V2Di &rhs)
{
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	return lhs;
}

V2Di &operator/=(V2Di &lhs, const int &rhs)
{
	lhs.x /= rhs;
	lhs.y /= rhs;
	return lhs;
}

V2Di &operator*=(V2Di &lhs, const int &rhs)
{
	lhs.x *= rhs;
	lhs.y *= rhs;
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




//// Dir
//
//Dir::Dir()
//{
//}
//
//Dir::Dir(const Dir &rhs)
//{
//	data = rhs.data;
//}
//
//Dir::Dir(const int &rhs)
//{
//	data = Dir(rhs);
//}
//
//Dir::operator int()
//{
//	return int(data);
//}
//
//Dir &Dir::operator=(const Dir &rhs)
//{
//	data = rhs;
//	return *this;
//}
//
//Dir &Dir::operator=(const Dir &rhs)
//{
//	data = rhs.data;
//	return *this;
//}
//
//Dir &Dir::operator=(const int &rhs)
//{
//	data = Dir(rhs);
//	return *this;
//}
//
//V2Di Dir::offset()
//{
//	throw;
//}
//
//Dir operator+(const Dir &lhs, const Dir &rhs)
//{
//	return Dir(int(lhs.data) + int(rhs.data));
//}
//
//Dir operator-(const Dir &lhs, const Dir &rhs)
//{
//	return Dir(int(lhs.data) - int(rhs.data));
//}
//
//Dir operator-(const Dir &lhs)
//{
//	return Dir(int(lhs.data));
//}
//
//Dir operator*(const Dir &lhs, const Dir &rhs)
//{
//	return Dir(int(lhs.data) * int(rhs.data));
//}
//
//
//Dir operator/(const Dir &lhs, const Dir &rhs)
//{
//	return Dir(int(lhs.data) / int(rhs.data));
//}
//
//Dir operator>>(const Dir &lhs, const Dir &rhs)
//{
//	return Dir(int(lhs.data) >> int(rhs.data));
//}
//
//Dir operator<<(const Dir &lhs, const Dir &rhs)
//{
//	return Dir(int(lhs.data) << int(rhs.data));
//}
//
//Dir &operator+=(Dir &lhs, const Dir &rhs)
//{
//	lhs.data = Dir(int(lhs.data) + int(rhs.data));
//	return lhs;
//}
//
//Dir &operator/=(Dir &lhs, const Dir &rhs)
//{
//	lhs.data = Dir(int(lhs.data) / int(rhs.data));
//	return lhs;
//}
//
//Dir &operator*=(Dir &lhs, const Dir &rhs)
//{
//	lhs.data = Dir(int(lhs.data) * int(rhs.data));
//	return lhs;
//}
//
//Dir &operator-=(Dir &lhs, const Dir &rhs)
//{
//	lhs.data = Dir(int(lhs.data) - int(rhs.data));
//	return lhs;
//}
//
//bool operator==(const Dir &lhs, const Dir &rhs)
//{
//	if (lhs.data != rhs.data)
//		return false;
//	if (lhs.data != rhs.data)
//		return false;
//	return true;
//}
//
//bool operator!=(const Dir &lhs, const Dir &rhs)
//{
//	return (!(lhs == rhs));
//}

DEVILUTION_END_NAMESPACE
