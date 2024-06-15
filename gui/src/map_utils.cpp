//
// Created by quentinf on 12/06/24.
//

#include "map_utils.hpp"

#include <flecs.h>

#include <Matrix.hpp>
#include <cstdint>
#include <map.hpp>

using namespace zappy_gui::map;

namespace zappy_gui::utils
{
Vector2 GetCoordsFromTileMatrix(const raylib::Matrix &transformMatrix)
{
    float32 offsetX = (static_cast<float32>(kMAP_WIDTH) * tileSize * 0.5f32) * spacing;
    float32 offsetY = static_cast<float32>(kMAP_WIDTH) * tileSize * 0.375f32;

    float32 x = (transformMatrix.m12 + offsetX) / (tileSize * spacing);
    float32 y = (transformMatrix.m14 + offsetY) / (tileSize * verticalSpacing);

    auto tileX = static_cast<uint32_t>(std::round(x));
    auto tileY = static_cast<uint32_t>(std::round(y));

    if (tileY % 2 != 0)
    {
        float32 offsetOddRow = x - static_cast<float32>(tileX);
        if (offsetOddRow < 0.5f32)
        {
            --tileX;
        }
    }

    return Vector2{static_cast<float>(tileX), static_cast<float>(tileY)};
}

uint64_t GetTileIndexFromCoords(const Vector2 &coords)
{
    return (static_cast<uint64_t>(coords.y) * static_cast<uint64_t>(kMAP_WIDTH)) + static_cast<uint64_t>(coords.x)
        + 1 + FLECS_HI_ID_RECORD_ID;
}

uint64_t GetTileIndexFromCoords(const uint16_t x, const uint16_t y)
{
    return (y * static_cast<uint64_t>(kMAP_WIDTH)) + x + 1 + FLECS_HI_ID_RECORD_ID;
}

uint64_t GetTileIndexFromCoords(const int32_t x, const int32_t y)
{
    return (static_cast<uint64_t>(y) * static_cast<uint64_t>(kMAP_WIDTH)) + static_cast<uint64_t>(x) + 1 + FLECS_HI_ID_RECORD_ID;
}

uint64_t GetTileIndexFromCoords(const float x, const float y)
{
    return (static_cast<uint64_t>(y) * static_cast<uint64_t>(kMAP_WIDTH)) + static_cast<uint64_t>(x) + 1 + FLECS_HI_ID_RECORD_ID;
}

uint64_t GetTileIndexFromCoords(const raylib::Matrix &transformMatrix)
{
    const Vector2 coords = GetCoordsFromTileMatrix(transformMatrix);
    return GetTileIndexFromCoords(coords);
}
}
