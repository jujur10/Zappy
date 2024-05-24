/*
** EPITECH PROJECT, 2024
** tests_seminar.cpp
** File description:
** Tests.
*/
#include <unistd.h>
#include <criterion/criterion.h>

#include "arguments.h"

Test(TEST_PARSING, test_parsing_01)
{
    char *argv[] = {"/bin/ls", "-c", "90", "-x", "40", "-y", "50", "-p",
    "500", "-f", "2", "-n", "toto", "titi", "tutu"};
    uint32_t argc = 15;
    argument_t args = {};
    uint8_t return_status;

    return_status = get_arguments(argc, (const char **)argv, &args);
    cr_assert_eq(return_status, 0, "Parsing");
    cr_assert_eq(args.port, 500, "Port");
    cr_assert_eq(args.width, 40, "Width");
    cr_assert_eq(args.height, 50, "Height");
    cr_assert_eq(args.clients_nb, 90, "Clients nb value");
    cr_assert_eq(args.frequency, 2, "Frequency");
    cr_assert_eq(args.nb_of_teams, 3, "Number of teams");
    cr_assert_str_eq(args.team_names[0], "toto", "Toto team name");
    cr_assert_str_eq(args.team_names[1], "titi", "Titi team name");
    cr_assert_str_eq(args.team_names[2], "tutu", "Tutu team name");
    free(args.team_names);
}

Test(TEST_PARSING, test_parsing_02)
{
    char *argv[] = {"/bin/ls", "-c", "-10", "-x", "40", "-y", "50", "-p",
    "500", "-f", "2", "-n", "toto", "titi", "tutu"};
    uint32_t argc = 15;
    argument_t args = {};
    uint8_t return_status;

    return_status = get_arguments(argc, (const char **)argv, &args);
    cr_assert_eq(return_status, 1, "Bad parsing");
    free(args.team_names);
}

Test(TEST_PARSING, test_parsing_03)
{
    char *argv[] = {"/bin/ls", "-c", "90", "-x", "40", "-y", "50", "-p",
                    "500", "-n", "toto", "titi", "tutu"};
    uint32_t argc = 13;
    argument_t args = {};
    uint8_t return_status;

    return_status = get_arguments(argc, (const char **)argv, &args);
    cr_assert_eq(return_status, 0, "Parsing");
    cr_assert_eq(args.port, 500, "Port");
    cr_assert_eq(args.width, 40, "Width");
    cr_assert_eq(args.height, 50, "Height");
    cr_assert_eq(args.clients_nb, 90, "Clients nb value");
    cr_assert_eq(args.frequency, FREQUENCY_DEFAULT_VALUE, "Frequency");
    cr_assert_eq(args.nb_of_teams, 3, "Number of teams");
    cr_assert_str_eq(args.team_names[0], "toto", "Toto team name");
    cr_assert_str_eq(args.team_names[1], "titi", "Titi team name");
    cr_assert_str_eq(args.team_names[2], "tutu", "Tutu team name");
    free(args.team_names);
}
