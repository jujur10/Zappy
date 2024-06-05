//
// Created by quentinf on 05/06/24.
//

#pragma once

#include "Model.hpp"

#include <stdfloat>

// pre-declaration to prevent circular dependencies
namespace flecs
{
struct iter;
}


namespace zappy_gui::map
{

constexpr int32_t MAP_WIDTH  = 1'024;
constexpr int32_t MAP_HEIGHT = 1'024;

using float32 = std::float32_t;

struct tileModels
{
    raylib::Model innerModel;
    raylib::Model outerModel;
};

struct innerTile // Tag
{
};

struct outerTile // Tag
{
};

void generateMap(const flecs::iter &it);

}
