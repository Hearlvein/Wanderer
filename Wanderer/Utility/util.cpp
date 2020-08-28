#include "Utility/util.hpp"

float sign(float number)
{
    if (number < 0.f)
        return -1.f;
    return 1.f;
}

float distance(const sf::Vector2f& vec)
{
    return std::sqrt((vec.x * vec.x) + (vec.y * vec.y));
}