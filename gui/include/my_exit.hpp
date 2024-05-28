//
// Created by quentinf on 28/05/24.
//

#pragma once

#include <cstdint>

namespace zappy_gui
{
/**
 * @class SystemExit
 * A class that provides a static function to exit the program.
 */
class SystemExit {
public:
    /**
     * Exits the program with the specified status code.
     * @param status The exit status code.
     * @note This function does not return.
     */
    [[noreturn]] static void exit(const uint8_t status) {
        asm volatile (
            "movzbl %0, %%edi\n\t"
            "movl $60, %%eax\n\t"
            "syscall"
            :: "r" (status)
            : "%edi", "%eax"
        );
        __builtin_unreachable();
    }
};
}
