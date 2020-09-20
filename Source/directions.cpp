/**
 * @file directions.cpp
 *
 * Implementation of function for sending and reciving network messages.
 */

#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

Dir left(const Dir &dir)
{
	static int _left[8] = { 7, 0, 1, 2, 3, 4, 5, 6 };
	return Dir(_left[int(dir)]);
};

Dir right(const Dir &dir)
{
	static int _right[8] = { 1, 2, 3, 4, 5, 6, 7, 0 };
	return Dir(_right[int(dir)]);
}

// Rotates the specified number of steps (positive clockwise, negative counterclockwise)
Dir rotate(const Dir &dir, const int n)
{
	return Dir((int(dir) + n) & 7);
	// Same as:
	//int newv = (int(dir) + n) % 8;
	//if (newv < 0) newv += 8;
	//return Dir(newv);
}

//// Rotates the specified number of steps (positive clockwise, negative counterclockwise)
//Dir16 rotate(const Dir16 &dir, const int n)
//{
//	return Dir16((int(dir) + n) & 15);
//}

Dir opposite(const Dir &dir)
{
	static int _opposite[8] = { 4, 5, 6, 7, 0, 1, 2, 3 };
	return Dir(_opposite[int(dir)]);
}

/** Maps from direction to X/Y-offset. */
V2Di offset(const Dir &dir)
{
	static const V2Di _offset[8] = {
		{  1,  1 }, {  0,  1 }, { -1,  1 }, { -1,  0 },
		{ -1, -1 }, {  0, -1 }, {  1, -1 }, {  1,  0 },
	};
	return _offset[int(dir)];
}

DEVILUTION_END_NAMESPACE
