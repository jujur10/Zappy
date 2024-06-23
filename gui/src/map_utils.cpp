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
    const float offsetY = static_cast<float>(kMAP_HEIGHT) * tileSize * 0.375f;
    const float y = (transformMatrix.m14 + offsetY) / (tileSize * verticalSpacing);
    const auto tileY = static_cast<uint32_t>(std::round(y));

    const float offsetX = 0 == tileY % 2 ? 0.f : tileSize * 0.5f;
    const float intermediateX = (transformMatrix.m12 / spacing) + (static_cast<float>(kMAP_WIDTH) * tileSize * 0.5f) - offsetX;
    const float x = (intermediateX / tileSize);
    const auto tileX = static_cast<uint32_t>(std::round(x));

    return Vector2{static_cast<float>(tileX), static_cast<float>(tileY)};
}

Vector2 GetCoordsFromVector(const Vector2 &positionVector)
{
    const float offsetY = static_cast<float>(kMAP_HEIGHT) * tileSize * 0.375f;
    const float y = (positionVector.y + offsetY) / (tileSize * verticalSpacing);
    const auto tileY = static_cast<uint32_t>(std::round(y));

    const float offsetX = 0 == tileY % 2 ? 0.f : tileSize * 0.5f;
    const float intermediateX = (positionVector.x / spacing) + (static_cast<float>(kMAP_WIDTH) * tileSize * 0.5f) - offsetX;
    const float x = (intermediateX / tileSize);
    const auto tileX = static_cast<uint32_t>(std::round(x));

    return Vector2{static_cast<float>(tileX), static_cast<float>(tileY)};
}

Vector2 GetCoordsFromVector(const Vector3 &positionVector)
{
    const float offsetY = static_cast<float>(kMAP_HEIGHT) * tileSize * 0.375f;
    const float y = (positionVector.z + offsetY) / (tileSize * verticalSpacing);
    const auto tileY = static_cast<uint32_t>(std::round(y));

    const float offsetX = 0 == tileY % 2 ? 0.f : tileSize * 0.5f;
    const float intermediateX = (positionVector.x / spacing) + (static_cast<float>(kMAP_WIDTH) * tileSize * 0.5f) - offsetX;
    const float x = (intermediateX / tileSize);
    const auto tileX = static_cast<uint32_t>(std::round(x));

    return Vector2{static_cast<float>(tileX), static_cast<float>(tileY)};
}

bool IsTileInOddRow(const float y)
{
    const auto offsetY = static_cast<float>(kMAP_HEIGHT) * tileSize * 0.375f;
    const float tileY = (y + offsetY) / (tileSize * verticalSpacing);
    const auto tileYRounded = static_cast<uint32_t>(std::round(tileY));

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
