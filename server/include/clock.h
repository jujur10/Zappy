/*
** EPITECH PROJECT, 2024
** clock.h
** File description:
** clock.h.
*/
#pragma once

#include <stdint.h>
#include <time.h>
#include "stdbool.h"

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
/// @return True if timeout exceed, false if not.
bool is_timeout_exceed(timespec_t PTR server_clock,
    timespec_t PTR clock_to_inspect);

/// @brief Function that verify if the blocking time is expired.
/// @param server_clock The server's actual blocking time (reference).
/// @param clock_to_inspect The blocking time to verify.
/// @return True if blocking time has expired, false if not.
bool has_blocking_time_expired(double server_clock, double clock_to_inspect);

/// @brief Function that calculate the difference between two clocks.
/// @param clock_1 The first clock.
/// @param clock_2 The second clock.
/// @param result The resulting clock containing difference.
void diff_clock(const timespec_t PTR clock_1,
    const timespec_t PTR clock_2, timespec_t PTR result);

/// @brief Function that calculate the difference between two clocks in
/// seconds.
/// @param clock_1 The first clock.
/// @param clock_2 The second clock.
double diff_seconds(const timespec_t PTR clock_1,
    const timespec_t PTR clock_2);

/// @brief Function that calculate time units from clocks diffs.
/// @param new_clock The first clock.
/// @param old_clock The second clock.
double real_time_clock_to_time_units(uint32_t frequency,
    const timespec_t PTR new_clock, const timespec_t PTR old_clock);
