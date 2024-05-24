/*
** EPITECH PROJECT, 2024
** arguments.c
** File description:
** arguments.c.
*/
#include "arguments.h"

#include <stdlib.h>
#include <unistd.h>

/// @brief Set team names into argument structure.
///
/// @param argc The number of arguments of the program.
/// @param argument_index The current argument index.
/// @param argv The raw arguments of the program.
/// @param argument The argument structure to fill.
/// @return 0 on success, 1 on failure.
static uint8_t set_team_names(uint32_t argc, uint32_t PTR argument_index,
    const char ARRAY ARRAY argv, argument_t PTR args)
{
    uint32_t index = 1;

    args->team_names = malloc(sizeof(char *) * (argc - *argument_index));
    if (NULL == args->team_names)
        return 1;
    for (; index < (argc - *argument_index); index++) {
        if ('-' == *argv[index + *argument_index])
            break;
        args->team_names[index - 1] = argv[index + *argument_index];
    }
    args->nb_of_teams = index - 1;
    *argument_index += index - 1;
    return 0;
}

/// @brief Set uint32 argument after checking value.
///
/// @param argument_index The current argument index.
/// @param argv The raw arguments of the program.
/// @param argument The argument structure to fill.
/// @return 0 on success, 1 on failure.
static uint8_t set_uint32_argument(uint32_t PTR argument_index,
    const char ARRAY ARRAY argv, uint32_t PTR argument)
{
    uint64_t converted_arg;

    converted_arg = strtol(argv[*argument_index + 1], NULL, 10);
    if (converted_arg >= UINT32_MAX)
        return 1;
    *argument = (uint32_t)converted_arg;
    (*argument_index)++;
    return 0;
}

/// @brief Set uint16 argument after checking value.
///
/// @param argument_index The current argument index.
/// @param argv The raw arguments of the program.
/// @param argument The argument structure to fill.
/// @return 0 on success, 1 on failure.
static uint8_t set_uint16_argument(uint32_t PTR argument_index,
    const char ARRAY ARRAY argv, uint16_t PTR argument)
{
    uint64_t converted_arg;

    converted_arg = strtol(argv[*argument_index + 1], NULL, 10);
    if (converted_arg >= UINT16_MAX)
        return 1;
    *argument = (uint16_t)converted_arg;
    (*argument_index)++;
    return 0;
}

/// @brief Check flag and set argument.
///
/// @param argc The number of arguments of the program.
/// @param argument_index The current argument index.
/// @param argv The raw arguments of the program.
/// @param args The argument structure to fill.
/// @return 0 on success, 1 on failure.
static uint8_t set_argument(uint32_t argc, uint32_t PTR argument_index,
    const char ARRAY ARRAY argv, argument_t PTR args)
{
    switch (*(const uint32_t *)argv[*argument_index] & 0x00FFFFFF) {
        case ARG_PORT:
            return set_uint16_argument(argument_index, argv, &args->port);
        case ARG_X:
            return set_uint32_argument(argument_index, argv, &args->width);
        case ARG_Y:
            return set_uint32_argument(argument_index, argv, &args->height);
        case ARG_CLIENTS_NB:
            return set_uint32_argument(argument_index, argv,
            &args->clients_nb);
        case ARG_FREQ:
            return set_uint32_argument(argument_index, argv, &args->frequency);
        case ARG_TEAM_NAMES:
            return set_team_names(argc, argument_index, argv, args);
        default:
            return 1;
    }
}

uint8_t get_arguments(uint32_t argc, const char ARRAY ARRAY argv,
    argument_t PTR args)
{
    for (uint32_t i = 1; i < argc - 1; i++)
        if (1 == set_argument(argc, &i, argv, args))
            return 1;
    if (0 == args->port || 0 == args->width || 0 == args->height ||
    0 == args->clients_nb || 0 == args->nb_of_teams)
        return 1;
    if (0 == args->frequency)
        args->frequency = FREQUENCY_DEFAULT_VALUE;
    return 0;
}
