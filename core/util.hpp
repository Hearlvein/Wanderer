#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <string>

// Macro definitions
#define SIGN(X) ((X < 0) ? -1 : 1)

float sign(float number);
void out(const char* message);
float distance(const sf::Vector2f& vec);
