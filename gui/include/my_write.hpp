//
// Created by quentinf on 28/05/24.
//

#pragma once

#include <cstdint>

namespace zappy_gui
{
/**
 * @class FileWriter
 * A class that provides functions to write data to a file descriptor.
 */
class FileWriter {
public:
    /**
     * Constructs a FileWriter object with the specified file descriptor.
     * @param fd The file descriptor to write to.
     */
    explicit FileWriter(const uint16_t fd) : fileDescriptor(fd) {}

    /**
     * Writes data to the file descriptor.
     * @param buffer The buffer containing the data to write.
     * @param size The size of the data to write.
     * @return The number of bytes written, or -1 if an error occurred.
     */
    int64_t write(const char * const buffer, const uint64_t size) const {
        int64_t bytesWritten;
        asm volatile (
            "syscall"
            : "=a" (bytesWritten)
            : "a" (1), "D" (fileDescriptor), "S" (buffer), "d" (size)
            : "rcx", "r11", "memory"
        );
        return bytesWritten;
    }

    /**
     * Writes data to the file descriptor (variant without return value).
     * @param buffer The buffer containing the data to write.
     * @param size The size of the data to write.
     */
    void write2(const char * const buffer, const uint64_t size) const {
        asm volatile (
            "syscall"
            :: "a" (1), "D" (fileDescriptor), "S" (buffer), "d" (size)
            : "rcx", "r11", "memory"
        );
    }
    private:
        uint16_t fileDescriptor; /**< The file descriptor to write to. */
};
}
