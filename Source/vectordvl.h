//HEADER_GOES_HERE
#ifndef __DVLVECTOR_H__
#define __DVLVECTOR_H__

DEVILUTION_BEGIN_NAMESPACE

// Lean 2D int vector
class V2Di {
public:
	V2Di();
	V2Di(int nx, int ny);
	int x = 0;
	int y = 0;
	V2Di abs() const;
	int maxdim() const;
	int maxabs() const;
	friend V2Di operator+(const V2Di &lhs, const V2Di &rhs);
	friend V2Di operator-(const V2Di &lhs, const V2Di &rhs);
	friend V2Di operator-(const V2Di &lhs);
	friend V2Di operator*(const int &lhs, const V2Di &rhs);
	friend V2Di operator*(const V2Di &lhs, const int &rhs);
	friend V2Di operator/(const V2Di &lhs, const int &rhs);
	friend V2Di operator>>(const V2Di &lhs, const int &rhs);
	friend V2Di operator<<(const V2Di &lhs, const int &rhs);
	friend V2Di &operator+=(V2Di &lhs, const V2Di &rhs);
	friend V2Di &operator-=(V2Di &lhs, const V2Di &rhs);
	friend V2Di &operator*=(V2Di &lhs, const int &rhs);
	friend V2Di &operator/=(V2Di &lhs, const int &rhs);
	friend bool operator==(const V2Di &lhs, const V2Di &rhs);
	friend bool operator!=(const V2Di &lhs, const V2Di &rhs);
};


// Trouble with this approach is that it is a PITA to implement break,
// and impossible to implement return without additional boilerplate code in calling function,
// defeating the goal of simplification
template <typename T>
void foreach2D(V2Di start, V2Di stop, T &&lambda)
{
	V2Di p = start;
	for (; p.y < stop.y; p.y++) {
		for (; p.x < stop.x; p.x++) {
			lambda(p);
		}
	}
}
// foreach2D({0,0}, {MAXDUNX, MAXDUNY}, lambda);


//class Dir {
//public:
//	Dir();
//	Dir(const typename Dir &rhs) { data = rhs; }
//	Dir(const Dir &rhs);
//	Dir(const int &rhs);
//	operator int();
//	Dir &operator=(const Dir &rhs);
//	Dir &operator=(const Dir &rhs);
//	Dir &operator=(const int &rhs);
//	V2Di offset();
//	friend Dir operator+(const Dir &lhs, const Dir &rhs);
//	friend Dir operator-(const Dir &lhs, const Dir &rhs);
//	friend Dir operator-(const Dir &lhs);
//	friend Dir operator*(const Dir &lhs, const Dir &rhs);
//	friend Dir operator/(const Dir &lhs, const Dir &rhs);
//	friend Dir operator>>(const Dir &lhs, const Dir &rhs);
//	friend Dir operator<<(const Dir &lhs, const Dir &rhs);
//	friend Dir &operator+=(Dir &lhs, const Dir &rhs);
//	friend Dir &operator-=(Dir &lhs, const Dir &rhs);
//	friend Dir &operator*=(Dir &lhs, const Dir &rhs);
//	friend Dir &operator/=(Dir &lhs, const Dir &rhs);
//	friend bool operator==(const Dir &lhs, const Dir &rhs);
//	friend bool operator!=(const Dir &lhs, const Dir &rhs);
//private:
//	Dir data = Dir::NONE;
//};

DEVILUTION_END_NAMESPACE

#endif /* __DVLVECTOR_H__ */
