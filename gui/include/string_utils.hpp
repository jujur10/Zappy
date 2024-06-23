//
// Created by quentinf on 08/06/24.
//

#pragma once

#include <charconv>
#include <string_view>

namespace zappy_gui::string_utils
{
/**
 * @brief Converts a string to a specified type.
 *
 * This function attempts to convert a string to a value of the specified type using the `std::from_chars` function.
 * It returns a boolean value indicating the success or failure of the conversion.
 *
 * @tparam T The target type to convert the string to.
 * @param str The string to be converted, represented as a `std::string_view`.
 * @param value A reference to the variable where the converted value will be stored.
 * @return `true` if the conversion is successful, `false` otherwise.
 */
template <typename T>
bool convertFromString(const std::string_view &str, T &value)
{
    auto result = std::from_chars(str.data(), str.data() + str.size(), value);
    return result.ec == std::errc();
}
}  // namespace zappy_gui::string_utils
