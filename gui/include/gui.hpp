#pragma once

#include "Rectangle.hpp"

namespace zappy_gui::gui {

    /// Draw a slider
    /// @param bounds The bounds of the slider
    /// @param textLeft The text on the left of the slider
    /// @param textRight The text on the left of the slider
    /// @param value A pointer to the value of the slider
    /// @param minValue The minimum value of the slider
    /// @param maxValue The maximum value of the slider
    /// @param editMode Can the value be changed
    /// @return True if the slider was pressed
    bool GuiSlider(const Rectangle bounds, const char *const textLeft, const char *const textRight, float *const value,
                   const float minValue, const float maxValue, const bool editMode);

}