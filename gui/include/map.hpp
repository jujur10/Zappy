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

constexpr int32_t MAP_WIDTH  = 128;
constexpr int32_t MAP_HEIGHT = 128;

using float32 = std::float32_t;

/// @brief Tile models
///
/// @note The inner model is the same as the full model but only the top face
struct tileModels
{
    raylib::Model *innerModel;
    raylib::Model *outerModel;
};

/// @brief Tag used to identify the inner tiles in the ECS
struct innerTile
{
};

/// @brief Tag used to identify the outer tiles in the ECS
struct outerTile
{
};

/// @brief Generate the map by computing and creating every tile entity
///
/// @param it The ECS query iterator
void generateMap(const flecs::iter &it);

}
