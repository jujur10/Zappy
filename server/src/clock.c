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
