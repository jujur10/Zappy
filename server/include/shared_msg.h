/*
** EPITECH PROJECT, 2024
** shared_msg.h
** File description:
** shared_msg.h.
*/

#pragma once

#include <stdint.h>

/// @brief Structure representing shared message.
///
/// @var counter The message counter (if 0, release memory).
/// @var len The message len.
/// @var ptr The pointer on message data.
typedef struct shared_msg_s {
    uint32_t counter;
    uint32_t len;
    const char ARRAY ptr;
} shared_msg_t;
