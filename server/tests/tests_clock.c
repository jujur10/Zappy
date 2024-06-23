/*
** EPITECH PROJECT, 2024
** test_queue.c
** File description:
** test_queue.c.
*/
#include <unistd.h>
#include <criterion/criterion.h>

#include "clock.h"

Test(TEST_CLOCK, test_clock_1)
{
    struct timespec clock_1;
    struct timespec clock_2;
    double time_units;

    clock_gettime(CLOCK_MONOTONIC, &clock_1);
    clock_2 = clock_1;
    add_to_clock(&clock_2, 1, 0);
    time_units = real_time_clock_to_time_units(1, &clock_2, &clock_1);
    cr_assert_eq(time_units, 1, "Time units = %lf\n", time_units);
}

Test(TEST_CLOCK, test_clock_2)
{
    struct timespec clock_1;
    struct timespec clock_2;
    double time_units;

    clock_gettime(CLOCK_MONOTONIC, &clock_1);
    clock_2 = clock_1;
    add_to_clock(&clock_2, 1, 0);
    time_units = real_time_clock_to_time_units(100, &clock_2, &clock_1);
    cr_assert_eq(time_units, 100, "Time units = %lf\n", time_units);
}

Test(TEST_CLOCK, test_clock_3)
{
    struct timespec clock_1;
    struct timespec clock_2;
    double time_units;

    clock_gettime(CLOCK_MONOTONIC, &clock_1);
    clock_2 = clock_1;
    add_to_clock(&clock_2, 86400, 0);
    time_units = real_time_clock_to_time_units(100, &clock_2, &clock_1);
    cr_assert_eq(time_units, 8640000, "Time units = %lf\n", time_units);
}

Test(TEST_CLOCK, test_clock_4)
{
    struct timespec clock_1;
    struct timespec clock_2;
    double time_units;

    clock_gettime(CLOCK_MONOTONIC, &clock_1);
    clock_2 = clock_1;
    add_to_clock(&clock_2, 86400, 0);
    time_units = real_time_clock_to_time_units(100, &clock_2, &clock_1);
    cr_assert_eq(time_units, 8640000, "Time units = %lf\n", time_units);

}
