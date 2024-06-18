//
// Created by quentinf on 06/06/24.
//

#pragma once

#include <cstdint>

namespace flecs
{
struct world;
}

namespace zappy_gui::systems
{
/// @brief Register all systems in the ECS
///
/// @param ecs The ECS to register the systems in
void registerSystems(const flecs::world &ecs);

}  // namespace zappy_gui::systems
