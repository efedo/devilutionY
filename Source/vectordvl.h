//HEADER_GOES_HERE
#ifndef __DVLVECTOR_H__
#define __DVLVECTOR_H__

DEVILUTION_BEGIN_NAMESPACE

// Lean 2D int vector
class V2Di {
public:
	V2Di(int nx = 0, int ny = 0);
	int x;
	int y;
	int maxdim();
};

V2Di operator+(const V2Di &lhs, const V2Di &rhs);
V2Di operator-(const V2Di &lhs, const V2Di &rhs);
V2Di operator-(const V2Di &lhs);
V2Di& operator+=(V2Di &lhs, const V2Di &rhs);
V2Di& operator-=(V2Di &lhs, const V2Di &rhs);
bool operator==(const V2Di &lhs, const V2Di &rhs);
bool operator!=(const V2Di &lhs, const V2Di &rhs);
V2Di abs(const V2Di &lhs);
int maxdim(const V2Di &lhs);

DEVILUTION_END_NAMESPACE

#endif /* __DVLVECTOR_H__ */
