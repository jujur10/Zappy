//
// Created by quentinf on 06/06/24.
//

#pragma once
#include <cstdint>

namespace raylib
{
class Matrix;  // Forward declaration to avoid circular dependency
class Model;   // Forward declaration to avoid circular dependency
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

/// @brief Unload the shaders of the model
///
/// @param model Pointer to the model to unload the shaders from
void UnloadShaders(const raylib::Model *model);
}  // namespace zappy_gui::utils
