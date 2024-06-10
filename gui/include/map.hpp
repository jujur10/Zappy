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

extern int32_t kMAP_WIDTH;
extern int32_t kMAP_HEIGHT;

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
void GenerateMap(const flecs::iter &it);

} // namespace zappy_gui::map
