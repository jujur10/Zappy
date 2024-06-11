//
// Created by quentinf on 06/06/24.
//

#include "raylib_utils.hpp"

#include "raylib.h"

#include <Matrix.hpp>
#include <Model.hpp>

namespace zappy_gui::utils
{

void SetupModel(const raylib::Model *const model, const char *const vertShaderPath, const char *const fragShaderPath)
{
    Shader shader                        = ::LoadShader(vertShaderPath, fragShaderPath);
    shader.locs[SHADER_LOC_MATRIX_MODEL] = ::GetShaderLocationAttrib(shader, "instanceTransform");
    shader.locs[SHADER_LOC_MATRIX_MVP]   = ::GetShaderLocation(shader, "mvp");
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

} // namespace zappy_gui::utils
