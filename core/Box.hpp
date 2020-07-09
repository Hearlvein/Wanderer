#pragma once

// Better use typedefs Box or Boxi
template <typename T>
struct _Box
{
	_Box(T _x = 0, T _y = 0, T _w = 0, T _h = 0)
		: x(_x), y(_y), w(_w), h(_h) {}
	T x, y, w, h;
};

typedef _Box<float> Box;	// a box with floating values is seen as default
typedef _Box<int> Boxi;		// a box can also holds integers

bool boxesOverlapping(const Box& lhs, const Box& rhs);
