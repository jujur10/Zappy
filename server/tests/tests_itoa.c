/*
** EPITECH PROJECT, 2024
** tests_seminar.cpp
** File description:
** Tests.
*/
#include <unistd.h>
#include <criterion/criterion.h>
#include <stdint.h>

#include "utils/itoa/fast_itoa.h"

Test(TEST_ITOA, test_itoa_max_u32)
{
    char buffer[20];
    uint32_t max = UINT32_MAX;
    uint64_t ret = fast_itoa_u32(max, buffer);

    buffer[ret] = 0;
    cr_assert_str_eq(buffer, "4294967295");
}

Test(TEST_ITOA, test_itoa_min_u32)
{
    char buffer[20];
    uint32_t min = 0;
    uint64_t ret = fast_itoa_u32(min, buffer);

    buffer[ret] = 0;
    cr_assert_str_eq(buffer, "0");
}

Test(TEST_ITOA, test_itoa_42_u32)
{
    char buffer[20];
    uint32_t value_42 = 42;
    uint64_t ret = fast_itoa_u32(value_42, buffer);

    buffer[ret] = 0;
    cr_assert_str_eq(buffer, "42");
}
