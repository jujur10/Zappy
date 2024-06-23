/*
** EPITECH PROJECT, 2024
** arrays_u64.c
** File description:
** arrays_u64.c.
*/
#include <stdint.h>
#include <stdbool.h>

void arrays_sub_u64(const uint64_t ARRAY arr_1, const uint64_t ARRAY arr_2,
    uint64_t ARRAY arr_out, uint64_t len)
{
    for (uint64_t i = 0; i < len; i++)
        arr_out[i] = arr_1[i] - arr_2[i];
}

void arrays_sub_u64_to_int64(const uint64_t ARRAY arr_1,
    const uint64_t ARRAY arr_2, int64_t ARRAY arr_out, uint64_t len)
{
    for (uint64_t i = 0; i < len; i++)
        arr_out[i] = arr_1[i] - arr_2[i];
}

void arrays_add_u64(const uint64_t ARRAY arr_1, const uint64_t ARRAY arr_2,
    uint64_t ARRAY arr_out, uint64_t len)
{
    for (uint64_t i = 0; i < len; i++)
        arr_out[i] = arr_1[i] + arr_2[i];
}

void arrays_mul_u64(const uint64_t ARRAY arr_1, const uint64_t ARRAY arr_2,
    uint64_t ARRAY arr_out, uint64_t len)
{
    for (uint64_t i = 0; i < len; i++)
        arr_out[i] = arr_1[i] * arr_2[i];
}

bool arrays_test_u64(const uint64_t ARRAY arr_1, bool (*fct)(uint64_t element),
    uint64_t len)
{
    for (uint64_t i = 0; i < len; i++)
        if (false == fct(arr_1[i]))
            return false;
    return true;
}
