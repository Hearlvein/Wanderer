#include "Box.hpp"

Box::Box(float _x, float _y, float _w, float _h)
	: x(_x), y(_y), w(_w), h(_h)
{

}

bool boxesOverlapping(Box& lhs, Box& rhs)
{
	return 	(lhs.x < rhs.x + rhs.w) &&
			(lhs.x + lhs.w > rhs.x) &&
			(lhs.y < rhs.y + rhs.h) &&
			(lhs.y + lhs.h > rhs.y);
}
