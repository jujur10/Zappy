//
// Created by quentinf on 17/06/24.
//

#pragma once

#include "ModelAnimation.hpp"

namespace zappy_gui::player
{

/// @brief The starting index used in flecs for the player entities
#define PLAYER_STARTING_IDX 1'100'000

/// @brief The index of the idle animation
#define IDLE_ANIMATION_IDX 12

enum class Orientation : uint8_t
{
    NORTH = 0,
    EAST = 1,
    SOUTH = 2,
    WEST = 3,
};

struct playerAnimations
{
    std::vector<raylib::ModelAnimation> *animations;
};

struct playerAnimationData
{
    raylib::ModelAnimation *currentAnimation;
    uint8_t currentFrame;
};
}  // namespace zappy_gui::player
