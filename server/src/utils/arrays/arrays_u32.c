/*
** EPITECH PROJECT, 2024
** arrays_u32.c
** File description:
** arrays_u32.c.
*/
#include <stdint.h>
#include <stdbool.h>

void arrays_sub_u32(const uint32_t ARRAY arr_1, const uint32_t ARRAY arr_2,
    uint32_t ARRAY arr_out, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++)
        arr_out[i] = arr_1[i] - arr_2[i];
}

void arrays_sub_u32_to_int64(const uint32_t ARRAY arr_1,
    const uint32_t ARRAY arr_2, int64_t ARRAY arr_out, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++)
        arr_out[i] = arr_1[i] - arr_2[i];
}

void arrays_add_u32(const uint32_t ARRAY arr_1, const uint32_t ARRAY arr_2,
    uint32_t ARRAY arr_out, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++)
        arr_out[i] = arr_1[i] + arr_2[i];
}

void arrays_mul_u32(const uint32_t ARRAY arr_1, const uint32_t ARRAY arr_2,
    uint32_t ARRAY arr_out, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++)
        arr_out[i] = arr_1[i] * arr_2[i];
}

bool arrays_test_u32(const uint32_t ARRAY arr_1, bool (*fct)(uint32_t element),
    uint32_t len)
{
    for (uint32_t i = 0; i < len; i++)
        if (false == fct(arr_1[i]))
            return false;
    return true;
}
