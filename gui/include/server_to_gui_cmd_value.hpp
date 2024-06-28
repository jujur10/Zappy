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
    PLAYER_POSITION = 7303280,  // ppo
    SET_TIME_UNIT = 7628659, // sgt
    TIME_UNIT_CHANGED = 7631731, // sst
    START_INCANTATION = 6515056, // pic
    END_INCANTATION = 6646128, // pie
    PLAYER_INVENTORY = 7235952, // pin
    PLAYER_PICKUP = 7628656, // pgt
    PLAYER_DROP = 7496816, // pdr
    TEAM_NAME = 6385268, // tna
    PLAYER_LEVEL = 7761008, // plv
    PLAYER_LAY_EGG = 7038576, // pfk
    EGG_LAID = 7827045, // enw
    CONNECTION_ON_EGG = 7299685, // ebo
    EGG_DEATH = 6906981, // edi
    BROADCAST = 6513264, // pbc
    EXPULSION = 7890288, // pex
    GAME_END = 6776179, // seg
};
}