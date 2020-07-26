#include "Tile.hpp"
#include "Constants.hpp"

const Tile Tile::s_Void   = { { 0.f, 0.f             }, Void };
const Tile Tile::s_Block1 = { { TILE_SIZE,     0.f   }, Solid };
const Tile Tile::s_Block2 = { { TILE_SIZE * 2.f, 0.f }, Solid };
const Tile Tile::s_Ladder = { { TILE_SIZE * 3.f, 0.f }, Ladder };
