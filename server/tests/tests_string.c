/*
** EPITECH PROJECT, 2024
** tests_string.c
** File description:
** tests_string.c.
*/
#include <unistd.h>
#include <criterion/criterion.h>

#include "utils/string/string.h"

Test(TEST_STRING, test_string_init)
{
    string_t string;

    init_string_from_char(&string, "Toto and Tutu\n", 14);
    cr_assert_str_eq(string.ptr, "Toto and Tutu\n");
    cr_assert_eq(string.len, 14);
}

Test(TEST_STRING, test_string_clear)
{
    string_t string;

    init_string_from_char(&string, "Toto and Tutu\n", 14);
    clear_string(&string);
    cr_assert_eq(string.ptr, NULL);
    cr_assert_eq(string.len, 0);
}

Test(TEST_STRING, test_string_append)
{
    string_t string;

    init_string_from_char(&string, "Toto and Tutu", 13);
    append_to_string_from_char(&string, " and Titi\n", 10);
    cr_assert_str_eq(string.ptr, "Toto and Tutu and Titi\n");
    cr_assert_eq(string.len, 23);
}
