/*
** EPITECH PROJECT, 2024
** clock.c
** File description:
** clock.c.
*/
#include <stdint.h>
#include <time.h>

#include "clock.h"

void add_to_clock(timespec_t PTR clock, int64_t sec, int64_t ns)
{
    clock->tv_sec += sec;
    clock->tv_nsec += ns;
    if (clock->tv_nsec >= 1000000000) {
        clock->tv_sec += 1;
        clock->tv_nsec -= 1000000000;
    }
}

void remove_from_clock(timespec_t PTR clock, int64_t sec, int64_t ns)
{
    if (clock->tv_nsec < ns) {
        clock->tv_sec -= 1;
        clock->tv_nsec += 1000000000L;
    }
    clock->tv_nsec -= ns;
    clock->tv_sec -= sec;
    if (clock->tv_sec < 0) {
        clock->tv_sec = 0;
        clock->tv_nsec = 0;
    }
}

uint8_t is_timeout_exceed(timespec_t PTR server_clock,
    timespec_t PTR clock_to_inspect)
{
    if (server_clock->tv_sec > clock_to_inspect->tv_sec ||
    (server_clock->tv_sec == clock_to_inspect->tv_sec &&
    server_clock->tv_nsec > clock_to_inspect->tv_nsec))
        return 0;
    return 1;
}

uint8_t has_blocking_time_expired(timespec_t PTR server_clock,
    timespec_t PTR clock_to_inspect)
{
    if (server_clock->tv_sec > clock_to_inspect->tv_sec ||
    (server_clock->tv_sec == clock_to_inspect->tv_sec &&
    server_clock->tv_nsec >= clock_to_inspect->tv_nsec))
        return 0;
    return 1;
}
