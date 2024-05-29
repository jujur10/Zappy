//
// Created by quentinf on 28/05/24.
//

#pragma once

#include <cstdint>
#include <unistd.h>

namespace zappy_gui::SystemExit
{
/**
 * Exits the program with the specified status code.
 * @param status The exit status code.
 * @note This function does not return.
 */
[[noreturn]] static void exit(const uint8_t status) {
    ::_exit(status);
    __builtin_unreachable();
}
}
