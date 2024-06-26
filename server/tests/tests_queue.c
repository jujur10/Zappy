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
#include "utils/queue/queue.h"

Test(TEST_QUEUE, test_queue_1)
{
    msg_t message = {};
    queue_t queue;

    queue_new(&queue);
    create_message("Toto tutu", 10, &message);
    queue_push(&queue, &message);
    queue_pop(&queue, &message);
    cr_assert_eq(message.len, 10);
    cr_assert_str_eq(message.ptr, "Toto tutu");
}

Test(TEST_QUEUE, test_queue_2)
{
    msg_t message = {};
    queue_t queue;

    queue_new(&queue);
    create_message("Toto", 5, &message);
    queue_push(&queue, &message);
    create_message("Tutu", 5, &message);
    queue_push(&queue, &message);
    queue_pop(&queue, &message);
    cr_assert_eq(message.len, 5);
    cr_assert_str_eq(message.ptr, "Toto");
    free(message.ptr);
    queue_pop(&queue, &message);
    cr_assert_eq(message.len, 5);
    cr_assert_str_eq(message.ptr, "Tutu");
    free(message.ptr);
}

