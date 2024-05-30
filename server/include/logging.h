/*
** EPITECH PROJECT, 2024
** logging.h
** File description:
** Logs
*/
#pragma once

#include <stdio.h>

#ifdef DEBUG

    #define CYAN "\033[96m"
    #define YELLOW "\033[93m"
    #define RED "\033[91m"
    #define BU_RED "\033[91;1;4m"
    #define NO_COLOR "\033[0m"

    #define EPRINTF(fmt, ...) SEMICOLON dprintf(2, fmt, __VA_ARGS__) SEMICOLON
    #define EPRINT(fmt) SEMICOLON dprintf(2, fmt) SEMICOLON

    #define LOG_FMT(f) "[" CYAN "LOG" NO_COLOR "]      " f "\n"
    /// Print a log to stderr
    #define LOG(format) EPRINT(LOG_FMT(format))
    // Format and print a log to stderr
    #define LOGF(format, ...) EPRINTF(LOG_FMT(format), __VA_ARGS__)

    #define WARN_FMT(f) "[" YELLOW "WARN" NO_COLOR "]     " f "\n"
    /// Print a warning to stderr
    #define WARN(format) EPRINT(WARN_FMT(format))
    /// Format and print a warning to stderr
    #define WARNF(format, ...) EPRINTF(WARN_FMT(format), __VA_ARGS__)

    #define ERROR_FMT(f) "[" RED "ERROR" NO_COLOR "]    " f "\n"
    /// Print an error to stderr
    #define ERROR(format) EPRINT(ERROR_FMT(format))
    /// Format and print an error to stderr
    #define ERRORF(format, ...) EPRINTF(ERROR_FMT(format), __VA_ARGS__)

    #define CRITICAL_FMT(f) "[" BU_RED "CRITICAL" NO_COLOR "] " f "\n"
    /// Print a critical error to stderr
    #define CRITICAL(format) EPRINT(CRITICAL_FMT(format))
    /// Format and print a critical error to stderr
    #define CRITICALF(format, ...) EPRINTF(CRITICAL_FMT(format), __VA_ARGS__)

#else

    #define CYAN
    #define YELLOW
    #define RED
    #define BU_RED
    #define NO_COLOR

    #define EPRINTF(fmt, ...) SEMICOLON
    #define EPRINT(fmt) SEMICOLON

    #define LOG_FMT(f) SEMICOLON
    /// Print a log to stderr
    #define LOG(format) SEMICOLON
    // Format and print a log to stderr
    #define LOGF(format, ...) SEMICOLON

    #define WARN_FMT(f) SEMICOLON
    /// Print a warning to stderr
    #define WARN(format) SEMICOLON
    /// Format and print a warning to stderr
    #define WARNF(format, ...) SEMICOLON

    #define ERROR_FMT(f) SEMICOLON
    /// Print an error to stderr
    #define ERROR(format) SEMICOLON
    /// Format and print an error to stderr
    #define ERRORF(format, ...) SEMICOLON

    #define CRITICAL_FMT(f) SEMICOLON
    /// Print a critical error to stderr
    #define CRITICAL(format) SEMICOLON
    /// Format and print a critical error to stderr
    #define CRITICALF(format, ...) SEMICOLON

#endif
