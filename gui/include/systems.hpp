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

/// @brief Create the GUI clickable entities
///
/// @param ecs The ECS to register the entities in
/// @param screenWidth The width of the screen
void createGuiEntities(const flecs::world &ecs, uint16_t screenWidth);

}  // namespace zappy_gui::systems
