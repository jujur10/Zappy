//
// Created by quentinf on 05/06/24.
//

#include "map.hpp"

#include <flecs.h>

#include <Matrix.hpp>

namespace zappy_gui::map
{

float32 spacing;
float32 tileSize;
float32 tileHeight;
float32 verticalSpacing;

void GenerateMap(const flecs::iter &it)
{
    spacing = 1.2f32;
    tileSize = 1.f32;
    tileHeight = 0.f32;
    verticalSpacing = std::sqrtf(3) / 2 * tileSize * spacing;

    for (int32_t y = 0; y < kMAP_HEIGHT; ++y)
    {
        for (int32_t x = 0; x < kMAP_WIDTH; ++x)
        {
            const float32 offsetX = y % 2 == 0 ? 0.f32 : tileSize * 0.5f32;  // Offset for odd rows

            auto position = raylib::Matrix::Translate(
                (static_cast<float32>(x) * tileSize + offsetX - static_cast<float32>(kMAP_WIDTH) * tileSize * 0.5f32) * spacing,
                tileHeight,
                static_cast<float32>(y) * tileSize * verticalSpacing - static_cast<float32>(kMAP_HEIGHT) * tileSize * 0.375f32);

            auto tileEntity = it.world().make_alive(y * kMAP_WIDTH + x + 1 + FLECS_HI_ID_RECORD_ID).set<raylib::Matrix>(position);

            raylib::Matrix matrixArray[7];
            matrixArray[0].m13 = 0.3f;  // y position
            matrixArray[0].m12 = resourceOffset[0].x * 1.25;
            matrixArray[0].m14 = resourceOffset[0].y * 1.25;
            matrixArray[0] = matrixArray[0] + raylib::Matrix::Scale(0.2f, 0.2f, 0.2f);
            matrixArray[0].m12 += position.m12;
            matrixArray[0].m14 += position.m14;
            for (int32_t i = 1; i < 7; ++i)
            {
                matrixArray[i].m13 = 0.17f;  // y position
                matrixArray[i].m12 = resourceOffset[i].x;
                matrixArray[i].m14 = resourceOffset[i].y;
                matrixArray[i] = matrixArray[i] + raylib::Matrix::Scale(0.15f, 0.15f, 0.15f);
                matrixArray[i].m12 += position.m12;
                matrixArray[i].m14 += position.m14;
            }

            // Create the entities for the resources as child of the current tile.
            // The resources are created with a transform matrix used to draw them, then the quantity as a uint16_t and the resource tag.
            resourceIds tileResourceIds;
            flecs::entity resource;

            for (auto i = static_cast<int>(resourceType::food); i < static_cast<int>(resourceType::total); ++i)
            {
                resource = it.world().entity().set<const raylib::Matrix>(matrixArray[i]).set<uint16_t>(0).add(static_cast<resourceType>(i)).disable();
                tileResourceIds.array[i] = resource.id();
            }

            tileEntity.set<resourceIds>(tileResourceIds);
        }
    }
}

}  // namespace zappy_gui::map
