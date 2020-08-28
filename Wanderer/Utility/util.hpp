#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <cmath>

// Macro definitions
#define SIGN(X) ((X < 0) ? -1 : 1)

float sign(float number);
float distance(const sf::Vector2f& vec);