//
// Created by quentinf on 05/06/24.
//

#pragma once

// #include <cstdint>
#include <stdfloat>

#include "Model.hpp"

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

extern float32 spacing;
extern float32 tileSize;
extern float32 tileHeight;
extern float32 verticalSpacing;

constexpr Vector2 ressourceOffset[] = {{0 * 0.4, -1 * 0.4},
                                 {-0.78 * 0.4, -0.62 * 0.4},
                                 {-0.97 * 0.4, 0.22 * 0.4},
                                 {-0.43 * 0.4, 0.9 * 0.4},
                                 {0.43 * 0.4, 0.9 * 0.4},
                                 {0.97 * 0.4, 0.22 * 0.4},
                                 {0.78 * 0.4, -0.62 * 0.4}};

/// @brief Tile models
///
/// @note The inner model is the same as the full model but only the top face
struct tileModels
{
    raylib::Model *grassModel;
};

struct ressourceModels
{
    raylib::Model *crystal;
};

enum class ressourceType
{
    food,
    linemate,
    deraumere,
    sibur,
    mendiane,
    phiras,
    thystame
};

/// @brief Tag used to identify the food on the map
struct food
{
};

/// @brief Tag used to identify linemate on the map
struct linemate // GREEN
{
};

/// @brief Tag used to identify deraumere on the map
struct deraumere // RED
{
};

/// @brief Tag used to identify sibur on the map
struct sibur // BLUE
{
};

/// @brief Tag used to identify mendiane on the map
struct mendiane // ORANGE
{
};

/// @brief Tag used to identify phiras on the map
struct phiras // YELLOW
{
};

/// @brief Tag used to identify thystame on the map
struct thystame // PURPLE
{
};

/// @brief Generate the map by computing and creating every tile entity
///
/// @param it The ECS query iterator
void GenerateMap(const flecs::iter &it);

}  // namespace zappy_gui::map
