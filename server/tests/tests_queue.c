/*
** EPITECH PROJECT, 2024
** test_queue.c
** File description:
** test_queue.c.
*/
#include <unistd.h>
#include <criterion/criterion.h>
#include <criterion/new/assert.h>

#include "msg.h"
#include "queue/msg_queue.h"

Test(TEST_QUEUE, test_queue_1)
{
    msg_t message;
    msg_queue_head_t queue;

    TAILQ_INIT(&queue);
    create_message("Toto tutu", 10, &message);
    add_msg_to_queue(&queue, &message);
    pop_msg(&queue, &message);
    cr_assert_eq(message.len, 10);
    cr_assert_str_eq(message.ptr, "Toto tutu");
}

Test(TEST_QUEUE, test_queue_2)
{
    msg_t message;
    msg_queue_head_t queue;

    TAILQ_INIT(&queue);
    create_message("Toto", 5, &message);
    add_msg_to_queue(&queue, &message);
    create_message("Tutu", 5, &message);
    add_msg_to_queue(&queue, &message);
    pop_msg(&queue, &message);
    cr_assert_eq(message.len, 5);
    cr_assert_str_eq(message.ptr, "Toto");
    free(message.ptr);
    pop_msg(&queue, &message);
    cr_assert_eq(message.len, 5);
    cr_assert_str_eq(message.ptr, "Tutu");
    free(message.ptr);
}

