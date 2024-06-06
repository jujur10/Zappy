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

/// @brief Function that remove seconds and/or nanoseconds from the clock.
/// @param clock The clock to modify.
/// @param sec The number of seconds to remove from the clock.
/// @param ns The number of nanoseconds to remove from the clock.
void remove_from_clock(timespec_t PTR clock, int64_t sec, int64_t ns);

/// @brief Function that verify if the timeout has been exceed.
/// @param server_clock The server's clock (reference).
/// @param clock_to_inspect The clock to verify.
/// @return 0 if timeout exceed, 1 if not.
uint8_t is_timeout_exceed(timespec_t PTR server_clock,
    timespec_t PTR clock_to_inspect);

/// @brief Function that verify if the blocking time is expired.
/// @param server_clock The server's clock (reference).
/// @param clock_to_inspect The clock to verify.
/// @return 0 if blocking time has expired, 1 if not.
uint8_t has_blocking_time_expired(timespec_t PTR server_clock,
    timespec_t PTR clock_to_inspect);
