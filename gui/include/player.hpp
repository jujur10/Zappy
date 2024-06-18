//
// Created by quentinf on 17/06/24.
//

#pragma once

#include "ModelAnimation.hpp"

namespace zappy_gui::player
{

/// @var The starting index used in flecs for the player entities
#define PLAYER_STARTING_IDX 1'100'000

/// @var The index of the idle animation
#define IDLE_ANIMATION_IDX 3

/// @var The index of the run animation
#define RUN_ANIMATION_IDX 9

/// @var The index of the walk animation
#define WALK_ANIMATION_IDX 10

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
