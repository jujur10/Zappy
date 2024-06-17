//
// Created by quentinf on 05/06/24.
//

#include "map.hpp"

#include <random>

#include "Matrix.hpp"
#include "flecs.h"

namespace zappy_gui::map
{

float32 spacing;
float32 tileSize;
float32 tileHeight;
float32 verticalSpacing;

void GenerateMap(const flecs::iter &it)
{
    // Define the subtract_with_carry_engine with 24 bits of state, 10 bits of shift, and 24 bits of output with a random device
    std::subtract_with_carry_engine<std::uint_fast32_t, 24, 10, 24> engine(std::random_device{}());

    // Define a discrete distribution with weights for each tile type
    std::discrete_distribution<uint_fast32_t> distribution({3, 2, 1});
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

            auto tileEntity = it.world().make_alive((y * kMAP_WIDTH) + x + 1 + FLECS_HI_ID_RECORD_ID).set<raylib::Matrix>(position);

            raylib::Matrix matrices[7];
            matrices[0].m13 = 0.3f;  // y position
            matrices[0].m12 = resourceOffset[0].x * 1.25f;
            matrices[0].m14 = resourceOffset[0].y * 1.25f;
            matrices[0] = matrices[0] + raylib::Matrix::Scale(0.2f, 0.2f, 0.2f);
            matrices[0].m12 += position.m12;
            matrices[0].m14 += position.m14;
            for (int32_t i = 1; i < 7; ++i)
            {
                matrices[i].m13 = 0.17f;  // y position
                matrices[i].m12 = resourceOffset[i].x;
                matrices[i].m14 = resourceOffset[i].y;
                matrices[i] = matrices[i] + raylib::Matrix::Scale(0.15f, 0.15f, 0.15f);
                matrices[i].m12 += position.m12;
                matrices[i].m14 += position.m14;
            }

            // Create the entities for the resources as child of the current tile.
            // The resources are created with a transform matrix used to draw them, then the quantity as a uint16_t and the resource tag.
            resourceIds tileResourceIds;
            flecs::entity resource;

            for (auto i = static_cast<int>(resourceType::food); i < static_cast<int>(resourceType::total); ++i)
            {
                resource =
                    it.world().entity().set<const raylib::Matrix>(matrices[i]).set<uint16_t>(0).add(static_cast<resourceType>(i)).disable();
                tileResourceIds.array[i] = resource.id();
            }

            tileEntity.set<resourceIds>(tileResourceIds);

            // Randomly assign a tile type to the tile to have different models
            switch (distribution(engine))
            {
                case 0:
                    tileEntity.add<tileType1>();
                break;
                case 1:
                    tileEntity.add<tileType2>();
                break;
                case 2:
                    tileEntity.add<tileType3>();
                break;
                default:
                    tileEntity.add<tileType1>();
                break;
            }
        }
    }
}
}  // namespace zappy_gui::map
