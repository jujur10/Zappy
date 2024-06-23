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
[[nodiscard]]
Vector2 GetCoordsFromTileMatrix(const raylib::Matrix &transformMatrix);

/// @brief Get the integer coordinates (x, y) from a vector
/// Get the tile x, y coordinates/indices from a vector where the vector coordinates are the one used to draw
/// @param positionVector The vector containing the "drawing" coordinates
Vector2 GetCoordsFromVector(const Vector2 &positionVector);

/// @brief Get the integer coordinates (x, y) from a vector
/// Get the tile x, y coordinates/indices from a vector where the vector coordinates are the one used to draw
/// @param positionVector The vector containing the "drawing" coordinates
Vector2 GetCoordsFromVector(const Vector3 &positionVector);

/// @brief Get the tile index from the coordinates
/// @param coords The coordinates of the tile in the 2D space (x, y)
/// @return The index of the corresponding tile entity in the world
[[nodiscard]]
uint64_t GetTileIndexFromCoords(const Vector2 &coords);

/// @brief Get the tile index from the coordinates
/// @param x The x coordinate of the tile in the 2D space
/// @param y The y coordinate of the tile in the 2D space equivalent to z in the 3D space
/// @return The index of the corresponding tile entity in the world
[[nodiscard]]
uint64_t GetTileIndexFromCoords(uint16_t x, uint16_t y);

/// @brief Get the tile index from the coordinates
/// @param x The x coordinate of the tile in the 2D space
/// @param y The y coordinate of the tile in the 2D space equivalent to z in the 3D space
/// @return The index of the corresponding tile entity in the world
[[nodiscard]]
uint64_t GetTileIndexFromCoords(int32_t x, int32_t y);

/// @brief Get the tile index from the coordinates
/// @param x The x coordinate of the tile in the 2D space
/// @param y The y coordinate of the tile in the 2D space equivalent to z in the 3D space
/// @return The index of the corresponding tile entity in the world
[[nodiscard]]
uint64_t GetTileIndexFromCoords(float x, float y);

/// @brief Get the tile index from the a tile matrix
/// The tile matrix is a 4x4 matrix where the x(m12) and z(m14) components are the coordinates of the tile
/// x, z correspond to x, y in the 2D space
/// @param transformMatrix A tile transformation matrix to compute the index from
[[nodiscard]]
uint64_t GetTileIndexFromCoords(const raylib::Matrix &transformMatrix);

/// @brief Check if the tile is in an offset row with the drawing y coordinate of the tile
/// The y coordinate is the one gound in the tile transformation matrix
/// @param y The y coordinate corresponding to the z coordinate in the 3D space
bool IsTileInOddRow(float y);
}  // namespace zappy_gui::utils
