/*
** EPITECH PROJECT, 2024
** gui_sgt_command.c
** File description:
** gui_sgt_command.c.
*/
#include "utils/itoa/fast_itoa.h"
#include "utils/pre_generate/pre_generate.h"

/// @brief Write a number to the buffer and increment the count value.
///
/// param nb The number to write into the buffer.
/// param buffer The buffer to modify.
/// param count The count value to use and modify.
static void write_nb_to_buffer(uint32_t nb, char ARRAY buffer,
    uint32_t PTR count)
{
    (*count) += fast_itoa_u32(nb, buffer + *count);
    buffer[*count] = ' ';
    (*count)++;
}

void execute_gui_sgt_command(server_t *server, uint16_t gui_idx,
    __attribute__((unused)) const gui_command_t *command)
{
    char buffer[50] = "sgt ";
    msg_t message;
    uint32_t count = 4;

    write_nb_to_buffer(server->frequency, buffer, &count);
    buffer[count - 1] = '\n';
    create_message(buffer, count, &message);
    add_msg_to_queue(&server->guis[gui_idx].queue, &message);
}
