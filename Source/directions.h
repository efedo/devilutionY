/**
 * @file directions.h
 *
 * Direction classes and enums
 */

#ifndef __DIRECTIONS_H__
#define __DIRECTIONS_H__

DEVILUTION_BEGIN_NAMESPACE

enum class Dir {
	NONE = -1,
	S = 0,    //0x0,
	SW = 1,   //0x1,
	W = 2,    //0x2,
	NW = 3,   //0x3,
	N = 4,    //0x4,
	NE = 5,   //0x5,
	E = 6,    //0x6,
	SE = 7,   //0x7,
	OMNI = 8, //0x8,
};

Dir left(const Dir &);     // left (counterclockwise?) of given direction
Dir right(const Dir &);    // right (clockwise?) of given direction
Dir opposite(const Dir &); // opposite of given direction
V2Di offset(const Dir &);  // x, y offset of a given direction
Dir rotate(const Dir &dir, const int n); // Rotates the specified number of steps (positive clockwise, negative counterclockwise)

// Will convert to enum, eventually
struct Dir16 {
	Dir16() {};
	explicit Dir16(const int &ndata)
	    : data(ndata) {};
	explicit Dir16(const Dir &ndata)
	    : data(int(ndata)) {};
	explicit operator int() const
	{
		return data;
	};
	Dir to8()
	{
		if (data > 7)
			throw;
		else
			return Dir(data);
	}

private:
	int data = 0;
};

enum class ScrollDir {
	NONE = 0x0,
	N = 0x1,
	NE = 0x2,
	E = 0x3,
	SE = 0x4,
	S = 0x5,
	SW = 0x6,
	W = 0x7,
	NW = 0x8,
};

DEVILUTION_END_NAMESPACE

#endif /*__DIRECTIONS_H__*/
