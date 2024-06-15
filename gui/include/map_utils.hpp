//
// Created by quentinf on 12/06/24.
//

#pragma once

#include <cstdint>

#include "Matrix.hpp"

namespace zappy_gui::utils
{
/// @brief Get the integer coordinates (x, y) from a tile matrix
/// The tile matrix is a 4x4 matrix where the x(m12) and z(m14) components are the coordinates of the tile
/// x, z correspond to x, y in the 2D space
/// @param transformMatrix A tile transformation matrix to extract the coordinates from
/// @return The coordinates of the tile in the 2D space
Vector2 GetCoordsFromTileMatrix(const raylib::Matrix &transformMatrix);

/// @brief Get the tile index from the coordinates
/// @param coords The coordinates of the tile in the 2D space (x, y)
/// @return The index of the corresponding tile entity in the world
uint64_t GetTileIndexFromCoords(const Vector2 &coords);

/// @brief Get the tile index from the coordinates
/// @param x The x coordinate of the tile in the 2D space
/// @param y The y coordinate of the tile in the 2D space equivalent to z in the 3D space
/// @return The index of the corresponding tile entity in the world
uint64_t GetTileIndexFromCoords(uint16_t x, uint16_t y);

/// @brief Get the tile index from the coordinates
/// @param x The x coordinate of the tile in the 2D space
/// @param y The y coordinate of the tile in the 2D space equivalent to z in the 3D space
/// @return The index of the corresponding tile entity in the world
uint64_t GetTileIndexFromCoords(int32_t x, int32_t y);

/// @brief Get the tile index from the coordinates
/// @param x The x coordinate of the tile in the 2D space
/// @param y The y coordinate of the tile in the 2D space equivalent to z in the 3D space
/// @return The index of the corresponding tile entity in the world
uint64_t GetTileIndexFromCoords(float x, float y);

/// @brief Get the tile index from the a tile matrix
/// The tile matrix is a 4x4 matrix where the x(m12) and z(m14) components are the coordinates of the tile
/// x, z correspond to x, y in the 2D space
/// @param transformMatrix A tile transformation matrix to compute the index from
uint64_t GetTileIndexFromCoords(const raylib::Matrix &transformMatrix);
}  // namespace zappy_gui::utils
