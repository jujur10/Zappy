//
// Created by quentinf on 06/06/24.
//

#pragma once
#include <cstdint>

struct Color; // Forward declaration to improve compile time and avoid circular dependency

namespace raylib
{
class Matrix;  // Forward declaration to improve compile time and avoid circular dependency
class Model;   // Forward declaration to improve compile time and avoid circular dependency
}  // namespace raylib

namespace zappy_gui::utils
{
/// @brief Setup the model with the given shaders
///
/// @param model Pointer to the model to setup
/// @param vertShaderPath Path to the vertex shader
/// @param fragShaderPath Path to the fragment shader
void SetupModel(const raylib::Model *model, const char *vertShaderPath, const char *fragShaderPath);

/// @brief Instanced draw of the model
///
/// @param model Pointer to the model to draw
/// @param matrixArray Array of transform matrices for the models
/// @param count Number of matrices in the array
void DrawModelInstanced(const raylib::Model *model, const raylib::Matrix *matrixArray, int32_t count);

/// @brief Draw the crystals with the given color
///
/// @param crystalModel Pointer to the model of the crystals
/// @param matrixArray Array of transform matrices for the crystals
/// @param count Number of matrices in the array
/// @param color Color of the crystals
void DrawCrystals(const raylib::Model *crystalModel, const raylib::Matrix *matrixArray, int32_t count, Color color);

/// @brief Unload the shaders of the model
///
/// @param model Pointer to the model to unload the shaders from
void UnloadShaders(const raylib::Model *model);
}  // namespace zappy_gui::utils
