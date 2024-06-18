/*
** EPITECH PROJECT, 2024
** gui_events.h
** File description:
** gui_events.h.
*/

#pragma once

/// @brief Enumeration representing the possible gui events to handle.
///
/// @var GUI_EVENT_NONE Representing empty event.
typedef enum {
    GUI_EVENT_NONE,
    GUI_EVENT_KILL,
    GUI_NB_EVENT
} PACKED gui_event_t;
