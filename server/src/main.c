/*
** EPITECH PROJECT, 2024
** main.c
** File description:
** Zappy server
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "arguments.h"

static uint8_t print_help(void)
{
    write(1, "USAGE: ./zappy_server -p port -x width -y height -n name1 "
    "name2 ... -c clientsNb -f freq\n", 89);
    return 0;
}

int main(int argc, char *argv[])
{
    argument_t args = {};

    if (argc < MIN_NB_OF_ARGS) {
        if (2 == argc && 0 == memcmp(argv[1], "-help", 6))
            return print_help();
        return 84;
    }
    if (1 == get_arguments(argc, (const char **)argv, &args))
        return 84;
    free(args.team_names);
    return 0;
}
