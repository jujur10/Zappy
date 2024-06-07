/*
** EPITECH PROJECT, 2024
** tests_spreading.c
** File description:
** tests_spreading.c.
*/
#include <unistd.h>
#include <criterion/criterion.h>

#include "arguments.h"
#include "map.h"
#include "utils/pre_generate/pre_generate.h"

Test(TEST_SPREADING, test_spreading_1)
{
    char *argv[] = {"/bin/ls", "-c", "90", "-x", "10", "-y", "10", "-p",
                    "500", "-f", "2", "-n", "toto", "titi", "tutu"};
    uint32_t argc = 15;
    argument_t args = {};
    map_t map = {};
    resources64_t result = {};

    get_arguments(argc, (const char **)argv, &args);
    init_map(&args, &map);
    pre_generate_resources_counter(&args);
    spread_resources_on_map(&map);
    for (uint64_t i = 0; i < map.height * map.width; i++)
        for (uint8_t j = 0; j < R_STRUCT_SIZE - 1; j++)
            result.arr[j] += map.tiles[i].arr[j];
    cr_assert_eq(result.attr.foods, NB_OF_FOODS);
    cr_assert_eq(result.attr.linemates, NB_OF_LINEMATES);
    cr_assert_eq(result.attr.deraumeres, NB_OF_DERAUMERES);
    cr_assert_eq(result.attr.siburs, NB_OF_SIBURS);
    cr_assert_eq(result.attr.mendianes, NB_OF_MENDIANES);
    cr_assert_eq(result.attr.phiras, NB_OF_PHIRAS);
    cr_assert_eq(result.attr.thystames, NB_OF_THYSTAMES);
    cr_assert_eq(result.attr.players, 0);
}

Test(TEST_SPREADING, test_spreading_2)
{
    char *argv[] = {"/bin/ls", "-c", "90", "-x", "10", "-y", "10", "-p",
                    "500", "-f", "2", "-n", "toto", "titi", "tutu"};
    uint32_t argc = 15;
    argument_t args = {};
    map_t map = {};
    resources64_t result = {};

    get_arguments(argc, (const char **)argv, &args);
    init_map(&args, &map);
    pre_generate_resources_counter(&args);
    map.tiles[42].attr.siburs = 600;
    map.total_resources.attr.siburs = 600;
    spread_resources_on_map(&map);
    for (uint64_t i = 0; i < map.height * map.width; i++)
        for (uint8_t j = 0; j < R_STRUCT_SIZE - 1; j++)
            result.arr[j] += map.tiles[i].arr[j];
    cr_assert_eq(result.attr.foods, NB_OF_FOODS);
    cr_assert_eq(result.attr.linemates, NB_OF_LINEMATES);
    cr_assert_eq(result.attr.deraumeres, NB_OF_DERAUMERES);
    cr_assert_eq(result.attr.siburs, 600);
    cr_assert_eq(result.attr.mendianes, NB_OF_MENDIANES);
    cr_assert_eq(result.attr.phiras, NB_OF_PHIRAS);
    cr_assert_eq(result.attr.thystames, NB_OF_THYSTAMES);
    cr_assert_eq(result.attr.players, 0);
}

Test(TEST_SPREADING, test_spreading_3)
{
    char *argv[] = {"/bin/ls", "-c", "90", "-x", "10", "-y", "10", "-p",
                    "500", "-f", "2", "-n", "toto", "titi", "tutu"};
    uint32_t argc = 15;
    argument_t args = {};
    map_t map = {};
    resources64_t result = {};

    get_arguments(argc, (const char **)argv, &args);
    init_map(&args, &map);
    pre_generate_resources_counter(&args);
    map.tiles[42].attr.siburs = 9;
    map.total_resources.attr.siburs = 9;
    spread_resources_on_map(&map);
    for (uint64_t i = 0; i < map.height * map.width; i++)
        for (uint8_t j = 0; j < R_STRUCT_SIZE - 1; j++)
            result.arr[j] += map.tiles[i].arr[j];
    cr_assert_eq(result.attr.foods, NB_OF_FOODS);
    cr_assert_eq(result.attr.linemates, NB_OF_LINEMATES);
    cr_assert_eq(result.attr.deraumeres, NB_OF_DERAUMERES);
    cr_assert_eq(result.attr.siburs, NB_OF_SIBURS);
    cr_assert_eq(result.attr.mendianes, NB_OF_MENDIANES);
    cr_assert_eq(result.attr.phiras, NB_OF_PHIRAS);
    cr_assert_eq(result.attr.thystames, NB_OF_THYSTAMES);
    cr_assert_eq(result.attr.players, 0);
}
