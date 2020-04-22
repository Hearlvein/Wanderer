#include "util.hpp"

float sign(float number)
{
    if (number < 0.f)
        return -1.f;
    return 1.f;
}

void out(const char* message)
{
	std::cout << message << std::endl;
}
