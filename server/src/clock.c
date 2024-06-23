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

void diff_clock(const timespec_t PTR clock_1, const timespec_t PTR clock_2,
    timespec_t PTR result)
{
    if ((clock_2->tv_nsec - clock_1->tv_nsec) < 0) {
        result->tv_sec = clock_2->tv_sec - clock_1->tv_sec - 1;
        result->tv_nsec = 1000000000 + clock_2->tv_nsec - clock_1->tv_nsec;
    } else {
        result->tv_sec = clock_2->tv_sec - clock_1->tv_sec;
        result->tv_nsec = clock_2->tv_nsec - clock_1->tv_nsec;
    }
}

double diff_seconds(const timespec_t PTR clock_1, const timespec_t PTR clock_2)
{
    timespec_t diff;

    diff_clock(clock_2, clock_1, &diff);
    return (double)diff.tv_sec + ((double)diff.tv_nsec / 1e9);
}

double real_time_clock_to_time_units(uint32_t frequency,
    const timespec_t PTR new_clock, const timespec_t PTR old_clock)
{
    double diff_as_seconds;

    diff_as_seconds = diff_seconds(new_clock, old_clock);
    return diff_as_seconds * (double)frequency;
}
