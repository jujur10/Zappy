//
// Created by quentinf on 05/06/24.
//

#include "map.hpp"

#include <Matrix.hpp>
#include <flecs.h>

namespace zappy_gui::map
{

void generateMap(const flecs::iter &it)
{
    constexpr float32 spacing         = 1.05f32;
    constexpr float32 tileSize        = 1.f32;
    constexpr float32 tileHeight      = 0.f32;
    constexpr float32 verticalSpacing = std::sqrtf(3) / 2 * tileSize * spacing;

    for (int32_t y = 0; y < MAP_HEIGHT; ++y)
    {
        for (int32_t x = 0; x < MAP_WIDTH; ++x)
        {
            const float32 offsetX = y % 2 == 0 ? 0.f32 : tileSize * 0.5f32;

            auto position = raylib::Matrix::Translate(
                (static_cast<float32>(x) * tileSize + offsetX - static_cast<float32>(MAP_WIDTH) * tileSize * 0.5f32) * spacing,
                tileHeight,
                static_cast<float32>(y) * tileSize * verticalSpacing - static_cast<float32>(MAP_HEIGHT) * tileSize * 0.375f32);

            // Assign the appropriate tag based on tile position (outer or inner)
            if (0 == x || x == (MAP_WIDTH - 1) || 0 == y || y == (MAP_HEIGHT - 1))
            {
                it.world().entity().set<raylib::Matrix>(position).add<outerTile>();
            }
            else
            {
                it.world().entity().set<raylib::Matrix>(position).add<innerTile>();
            }
        }
    }
}

}
