/*
** EPITECH PROJECT, 2024
** arrays_u64.h
** File description:
** arrays_u64.h.
*/

#pragma once

#include <stdint.h>
#include <stdbool.h>

/// @brief Function that subtracts arr_2 from arr_1 and puts the result
/// in array_out.\n
/// All the arrays must have the same length.
///
/// @param arr_1 The first array.
/// @param arr_2 The array to subtract from first array.
/// @param arr_out The array containing the result of the operation.
/// @param len The len of the array.
void arrays_sub_u64(const uint64_t ARRAY arr_1, const uint64_t ARRAY arr_2,
    uint64_t ARRAY arr_out, uint64_t len);

/// @brief Function that subtracts arr_2 from arr_1 and puts the result
/// in array_out (for negative values).\n
/// All the arrays must have the same length.
///
/// @param arr_1 The first array.
/// @param arr_2 The array to subtract from first array.
/// @param arr_out The array containing the result of the operation.
/// @param len The len of the array.
void arrays_sub_u64_to_int64(const uint64_t ARRAY arr_1,
    const uint64_t ARRAY arr_2, int64_t ARRAY arr_out, uint64_t len);

/// @brief Function which add elements from arr_1 and arr_2 and put it
/// into arr_out.
/// All the arrays must have the same length.
///
/// @param arr_1 The first array.
/// @param arr_2 The second array.
/// @param arr_out The resulting array.
/// @param len The len of the array.
void arrays_add_u64(const uint64_t ARRAY arr_1, const uint64_t ARRAY arr_2,
    uint64_t ARRAY arr_out, uint64_t len);

/// @brief Function which put the result multiplication of arr_1 and arr_2 and
/// put it into arr_out.
/// All the arrays must have the same length.
///
/// @param arr_1 The first array.
/// @param arr_2 The second array.
/// @param arr_out The resulting array.
/// @param len The len of the array.
void arrays_mul_u64(const uint64_t ARRAY arr_1, const uint64_t ARRAY arr_2,
    uint64_t ARRAY arr_out, uint64_t len);

/// @brief Function which test all the element of an array and returns false
/// if fct returns false.
/// All the arrays must have the same length.
///
/// @param arr_1 The first array.
/// @param fct The function to apply on the array.
/// @param len The len of the array.
bool arrays_test_u64(const uint64_t ARRAY arr_1, bool (*fct)(uint64_t element),
    uint64_t len);
