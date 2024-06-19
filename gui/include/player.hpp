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
#define IDLE_ANIMATION_IDX 3

/// @brief The index of the run animation
#define RUN_ANIMATION_IDX 9

/// @brief The index of the walk animation
#define WALK_ANIMATION_IDX 10

/// @brief The angle in degrees for the player when facing east
#define EAST_ANGLE 90.f

/// @brief The angle in degrees for the player when facing west
#define WEST_ANGLE 270.f

/// @brief The angle in degrees for the player when facing north in a regular row
#define NORTH_REGULAR_ANGLE 150.f

/// @brief The angle in degrees for the player when facing north in an odd row
#define NORTH_ODD_ANGLE 210.f

/// @brief The angle in degrees for the player when facing south in a regular row
#define SOUTH_REGULAR_ANGLE 30.f

/// @brief The angle in degrees for the player when facing south in an odd row
#define SOUTH_ODD_ANGLE 330.f



enum class Orientation
{
    NORTH = 1,
    EAST = 2,
    SOUTH = 3,
    WEST = 4,
};

struct playerAnimations
{
    std::vector<raylib::ModelAnimation> *animations;
};

struct playerAnimationData
{
    raylib::ModelAnimation *currentAnimation;
    int32_t currentFrame;
};

struct playerTargetInfo
{
    Vector2 target;
    Vector2 normalizedDirection;
};

}  // namespace zappy_gui::player
