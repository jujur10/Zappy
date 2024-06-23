/*
** EPITECH PROJECT, 2024
** arrays_resources_u32.h
** File description:
** arrays_resources_u32.h.
*/

#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "resources.h"

/// @brief Structure representing arrays of resources_t structure.
///
/// @var array The pointer on resources_t array.
/// @var len The length of the array.
typedef struct array_resources_u32_s {
    resources_t ARRAY array;
    uint32_t len;
} array_resources_u32_t;

#define ALLOC_RES_ARR_U32_STACK(x, arr_len) \
x.array = alloca(sizeof(resources_t) * (arr_len)); \
memset(x.array, 0, sizeof(resources_t) * (arr_len)); \
x.len = (arr_len)
