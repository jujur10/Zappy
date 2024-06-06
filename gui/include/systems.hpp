//
// Created by quentinf on 06/06/24.
//

#pragma once

namespace flecs
{
struct world;
}


namespace zappy_gui::systems
{

void registerSystems(const flecs::world &ecs);

}
