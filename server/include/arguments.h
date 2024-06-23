/*
** EPITECH PROJECT, 2024
** arguments.h
** File description:
** arguments.h.
*/

#pragma once

#include <stdint.h>

// 10 Arguments + execution path.
#define MIN_NB_OF_ARGS (10 + 1)

// Definition of the default frequency value.
#define FREQUENCY_DEFAULT_VALUE 100

/// @brief Structure representing arguments.
///
/// @var port The port of the server.
/// @var width The width of the world.
/// @var height The height of the world.
/// @var clients_nb The number of authorized clients per team (-c argument).
/// @var frequency The reciprocal of time unit for execution of actions.
/// @var nb_of_teams The number of teams (correlated to team_names).
/// @var team_names The array of pointer on arguments representing names on
/// argv.
typedef struct argument_s {
    uint16_t port;
    uint32_t width;
    uint32_t height;
    uint16_t clients_nb;
    uint32_t frequency;
    uint32_t nb_of_teams;
    const char ARRAY ARRAY team_names;
} argument_t;

/// @brief Enumeration representing flag arguments as uint32_t.
///
/// @var ARG_PORT The -p flag -> argument_t.port
/// @var ARG_X The -x flag -> argument_t.width
/// @var ARG_Y The -y flag -> argument_t.height
/// @var ARG_TEAM_NAMES The -n flag -> argument_t.team_names
/// @var ARG_CLIENTS_NB The -c flag -> argument_t.clients_nb
/// @var ARG_FREQ The -f flag -> argument_t.frequency
enum Arguments {
    ARG_PORT = 28717,
    ARG_X = 30765,
    ARG_Y = 31021,
    ARG_TEAM_NAMES = 28205,
    ARG_CLIENTS_NB = 25389,
    ARG_FREQ = 26157,
};

// Function prototype.

/// @brief Get arguments and put them into argument_t structure.
///
/// @param argc The number of args.
/// @param argv The raw arguments.
/// @param args The argument structure to fill.
/// @return 0 on success, 1 on failure.
uint8_t get_arguments(uint32_t argc, const char ARRAY ARRAY argv,
    argument_t PTR args);
