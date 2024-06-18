//
// Created by quentinf on 10/06/24.
//

#pragma once

#include <cstdint>

namespace zappy_gui::net
{
/// @brief Enum class representing the first 3 bytes of the server commands casted to uint32_t
enum class ServerCommands: uint32_t {
    MAP_SIZE = 8024941, // msz
    MAP_TILE = 7627618, // bct
    NEW_PLAYER = 7827056, // pnw
    DEATH_OF_PLAYER = 6906992, // pdi
};
}