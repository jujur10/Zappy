/*
** EPITECH PROJECT, 2024
** fast_itoa.h
** File description:
** fast_itoa.h.
*/

#pragma once

#include <stdint.h>

/// @brief Fast implementation of itoa for uint32_t.\n
/// This implementation consist to have 2 digits by division instead of 1.
/// @param value The uint16_t value to convert to ascii.
/// @param buffer The array to write the ascii representing the value.
/// @return The number of characters wrote.
uint64_t fast_itoa_u32(uint32_t value, char ARRAY buffer);
