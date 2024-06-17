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
    float offsetX = (static_cast<float>(kMAP_WIDTH) * tileSize * 0.5f) * spacing;
    float offsetY = static_cast<float>(kMAP_HEIGHT) * tileSize * 0.375f;

    float x = (transformMatrix.m12 + offsetX) / (tileSize * spacing);
    float y = (transformMatrix.m14 + offsetY) / (tileSize * verticalSpacing);

    auto tileX = static_cast<uint32_t>(std::round(x));
    auto tileY = static_cast<uint32_t>(std::round(y));

    if (tileY % 2 != 0)
    {
        float offsetOddRow = x - static_cast<float>(tileX);
        if (offsetOddRow < 0.5f)
        {
            --tileX;
        }
    }

    return Vector2{static_cast<float>(tileX), static_cast<float>(tileY)};
}

bool IsTileInOddRow(const float y)
{
    auto offsetY = static_cast<float>(kMAP_HEIGHT) * tileSize * 0.375f;
    float tileY = (y + offsetY) / (tileSize * verticalSpacing);
    auto tileYRounded = static_cast<uint32_t>(std::round(tileY));

    return tileYRounded % 2;
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
