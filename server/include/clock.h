/*
** EPITECH PROJECT, 2024
** clock.h
** File description:
** clock.h.
*/
#pragma once

#include <stdint.h>
#include <time.h>

// Coding style.
typedef struct timespec timespec_t;

/// @brief Function that add seconds and/or nanoseconds to the clock.
/// @param clock The clock to modify.
/// @param sec The number of seconds to add to the clock.
/// @param ns The number of nanoseconds to add to the clock.
void add_to_clock(timespec_t PTR clock, int64_t sec, int64_t ns);
