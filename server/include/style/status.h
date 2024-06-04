/*
** EPITECH PROJECT, 2024
** status.h
** File description:
** status.h.
*/

#pragma once

// Macro used to put enum on 1 byte.
#define PACKED __attribute__ ((packed))

/// @brief Enumeration representing the returns status of a function.
///
/// @var SUCCESS The function behaves as expected.
/// @var FAILURE Function does not behave as expected.
typedef enum status_s {
    SUCCESS,
    FAILURE
} PACKED status_t;
