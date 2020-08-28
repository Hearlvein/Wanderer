#include "Utility/Box.hpp"

bool boxesOverlapping(const Box& lhs, const Box& rhs)
{
	return 	(lhs.x < rhs.x + rhs.w) &&
		(lhs.x + lhs.w > rhs.x) &&
		(lhs.y < rhs.y + rhs.h) &&
		(lhs.y + lhs.h > rhs.y);
}