#pragma once

struct Box
{
	Box(float _x = 0, float _y = 0, float _w = 0, float _h = 0);
	float x, y, w, h;
};

bool boxesOverlapping(Box& lhs, Box& rhs);
