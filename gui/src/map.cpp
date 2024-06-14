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

            // printf("position x: %d, y: %d\n", x, y);

            it.world().make_alive(y * kMAP_WIDTH + x + 1 + FLECS_HI_ID_RECORD_ID).set<raylib::Matrix>(position).scope([&]
            {
                // printf("index: %d, tile_id: %ld\n", y * kMAP_WIDTH + x + 1 + FLECS_HI_ID_RECORD_ID, tile.raw_id());
                raylib::Matrix matrixArray[7];
                for (int32_t i = 0; i < 7; ++i)
                {
                    matrixArray[i].m13 = 0.17f;  // y position
                    matrixArray[i].m12 = ressourceOffset[i].x;
                    matrixArray[i].m14 = ressourceOffset[i].y;
                    matrixArray[i] = matrixArray[i] + raylib::Matrix::Scale(0.15f, 0.15f, 0.15f);
                    matrixArray[i].m12 += position.m12;
                    matrixArray[i].m14 += position.m14;
                }

                // ressourceIDs ids;

                // Create the entities for the resources as child of the current tile.
                // The resources are created with a transform matrix used to draw them, then the quantity as a uint16_t and the ressource tag.
                it.world().entity().set<const raylib::Matrix>(matrixArray[0]).set<uint16_t>(0).add<food>().disable();

                it.world().entity().set<const raylib::Matrix>(matrixArray[1]).set<uint16_t>(0).add<linemate>().disable();

                it.world().entity().set<const raylib::Matrix>(matrixArray[2]).set<uint16_t>(0).add<deraumere>().disable();

                it.world().entity().set<const raylib::Matrix>(matrixArray[3]).set<uint16_t>(0).add<sibur>().disable();

                it.world().entity().set<const raylib::Matrix>(matrixArray[4]).set<uint16_t>(0).add<mendiane>().disable();

                it.world().entity().set<const raylib::Matrix>(matrixArray[5]).set<uint16_t>(0).add<phiras>().disable();

                it.world().entity().set<const raylib::Matrix>(matrixArray[6]).set<uint16_t>(0).add<thystame>().disable();
           });
        }
    }
}

}  // namespace zappy_gui::map
