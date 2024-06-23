//
// Created by quentinf on 05/06/24.
//

#pragma once

// #include <cstdint>
#include <cstdint>
#include <stdfloat>

#include "Model.hpp"

// pre-declaration to prevent circular dependencies
namespace flecs
{
struct iter;
}

namespace zappy_gui::map
{

using float32 = std::float32_t;

/// @var represent the map width
extern int32_t kMAP_WIDTH;

/// @var represent the map height
extern int32_t kMAP_HEIGHT;

/// @var represent the additional spacing between each tile
extern float spacing;

/// @var represent the tile size
extern float tileSize;

/// @var represent the tile width
extern float tileHeight;

/// @var represent the tile vertical spacing (offset because of the hexagon shape)
extern float verticalSpacing;

/// @var represent the coordinates offset based on the tile origin point for each resource
constexpr Vector2 resourceOffset[] = {{0 * 0.4, -1 * 0.4},
                                       {-0.78 * 0.4, -0.62 * 0.4},
                                       {-0.97 * 0.4, 0.22 * 0.4},
                                       {-0.43 * 0.4, 0.9 * 0.4},
                                       {0.43 * 0.4, 0.9 * 0.4},
                                       {0.97 * 0.4, 0.22 * 0.4},
                                       {0.78 * 0.4, -0.62 * 0.4}};

/// @var represent the colors of each resource obtained by adding the model color with another one
constexpr Color resourceColors[] = {
    {80, 80, 80, 255},     // linemate: DARKGRAY
    {0, 228, 48, 255},     // deraumere: GREEN
    {102, 191, 255, 255},  // sibur: SKYBLUE
    {211, 176, 131, 255},  // mendiane: BEIGE
    {255, 109, 194, 255},  // phiras: PINK
    {230, 41, 55, 255}     // thystame: RED
};

/// @brief Store the 3d model for the skybox
struct skybox
{
    raylib::Model *model;
};

/// @brief Store the 3d models for tiles
struct tileModels
{
    raylib::Model *sand;
    raylib::Model *sandRock;
    raylib::Model *sandCactus;
};

/// @brief Tag component for the tile type used to apply different models
struct tileType1
{

};

/// @brief Tag component for the tile type used to apply different models
struct tileType2
{

};

/// @brief Tag component for the tile type used to apply different models
struct tileType3
{

};

/// @brief Store the 3d models for resource
struct resourceModels
{
    raylib::Model *food;
    raylib::Model *crystal;
};

/// @brief Enum class for representing the different types of resources
enum class resourceType
{
    food,
    linemate,
    deraumere,
    sibur,
    mendiane,
    phiras,
    thystame,
    total
};

/// @brief resource ID component
/// This component is used to store each resource ID in a tile
union resourceIds
{
    struct attributes
    {
        uint64_t food;
        uint64_t linemate;
        uint64_t deraumere;
        uint64_t sibur;
        uint64_t mendiane;
        uint64_t phiras;
        uint64_t thystame;
    } attributes;

    uint64_t array[static_cast<int>(resourceType::total)];
};

/// @brief Generate the map by computing and creating every tile entity
/// The map is generated by creating a tiles entities and linking resource entities to each tile
/// @param it The ECS query iterator
void GenerateMap(const flecs::iter &it);

}  // namespace zappy_gui::map
