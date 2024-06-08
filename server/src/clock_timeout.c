/*
** EPITECH PROJECT, 2024
** clock_timeout.c
** File description:
** clock_timeout.c.
*/
#include <time.h>
#include <stdbool.h>

#include "clock.h"

bool is_timeout_exceed(timespec_t PTR server_clock,
    timespec_t PTR clock_to_inspect)
{
    if (server_clock->tv_sec > clock_to_inspect->tv_sec ||
    (server_clock->tv_sec == clock_to_inspect->tv_sec &&
    server_clock->tv_nsec > clock_to_inspect->tv_nsec))
        return true;
    return false;
}

bool has_blocking_time_expired(timespec_t PTR server_clock,
    timespec_t PTR clock_to_inspect)
{
    if (server_clock->tv_sec > clock_to_inspect->tv_sec ||
    (server_clock->tv_sec == clock_to_inspect->tv_sec &&
    server_clock->tv_nsec >= clock_to_inspect->tv_nsec))
        return true;
    return false;
}
