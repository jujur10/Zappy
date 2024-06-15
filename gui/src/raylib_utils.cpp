//
// Created by quentinf on 06/06/24.
//

#include "raylib_utils.hpp"

#include "Matrix.hpp"
#include "Model.hpp"
#include "raylib.h"

namespace zappy_gui::utils
{

void SetupModel(const raylib::Model *const model, const char *const vertShaderPath, const char *const fragShaderPath)
{
    const Shader shader = ::LoadShader(vertShaderPath, fragShaderPath);
    shader.locs[SHADER_LOC_MATRIX_MODEL] = ::GetShaderLocationAttrib(shader, "instanceTransform");
    shader.locs[SHADER_LOC_MATRIX_MVP] = ::GetShaderLocation(shader, "mvp");
    for (int32_t i = 0; i < model->materialCount; ++i)
    {
        model->materials[i].shader = shader;
    }
}

void DrawModelInstanced(const raylib::Model *const model, const raylib::Matrix *const matrixArray, const int32_t count)
{
    for (int32_t i = 0; i < model->meshCount; ++i)
    {
        ::DrawMeshInstanced(model->meshes[i], model->materials[model->meshMaterial[i]], matrixArray, count);
    }
}

void DrawCrystals(const raylib::Model *const crystalModel, const raylib::Matrix *const matrixArray, const int32_t count, const Color color)
{
    const auto originalColor = crystalModel->materials[1].maps[MATERIAL_MAP_DIFFUSE].color;

    crystalModel->materials[1].maps[MATERIAL_MAP_DIFFUSE].color = color;

    for (int32_t i = 0; i < crystalModel->meshCount; ++i)
    {
        ::DrawMeshInstanced(crystalModel->meshes[i], crystalModel->materials[crystalModel->meshMaterial[i]], matrixArray, count);
    }

    crystalModel->materials[1].maps[MATERIAL_MAP_DIFFUSE].color = originalColor;
}

void UnloadShaders(const raylib::Model *const model)
{
    ::UnloadShader(model->materials[0].shader);
}
}  // namespace zappy_gui::utils
