/*
** EPITECH PROJECT, 2024
** shared_msg.h
** File description:
** shared_msg.h.
*/

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "style/status.h"
#include "utils/string/buffer.h"

/// @brief Structure representing message.
///
/// @var len The message len.
/// @var ptr The pointer on message data.
typedef struct msg_s {
    uint32_t len;
    char ARRAY ptr;
    bool to_free;
} msg_t;

/// @brief Function used to create a message.
///
/// @param msg_content The message content to put into the message.
/// @param len The message length.
/// @param message The pointer on the structure to fill.
/// @return Success or failure if the allocation failed.
status_t create_message(const char ARRAY msg_content, uint32_t len,
    msg_t PTR message);

/// @brief Function used to create a message without allocation.
///
/// @param ptr The pointer on the buffer to send.
/// @param len The data length.
/// @param message The pointer on the structure to fill.
void create_message_from_ptr(char ARRAY ptr, uint32_t len,
    msg_t PTR message);

/// @brief Function used to create a message from an existing buffer.
///
/// @param buffer The pointer on the buffer to send.
/// @param message The pointer on the structure to fill.
void create_message_from_buffer(const buffer_t PTR buffer, msg_t PTR message);

/// @brief Function used to destroy a message.
///
/// @param message The pointer on the message to free.
void destroy_message(msg_t PTR message);
