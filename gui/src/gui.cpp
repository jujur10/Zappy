#include "Mouse.hpp"
#include "Rectangle.hpp"
#include "raylib.h"
#include "Color.hpp"
#include <cstdint>
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "raylib_utils.hpp"
#include <flecs.h>
#include "gui_to_server_cmd_structs.hpp"
#include "gui.hpp"

namespace zappy_gui::gui {
    static bool GuiSliderProOwning(raylib::Rectangle bounds, const char *textLeft, const char *textRight,
                            float *value, const float minValue, float maxValue, const int sliderWidth, bool editMode) {
        auto state = static_cast<GuiState>(raygui::GuiGetState());
        float tempValue = *value;
        bool pressed = false;

        const auto sliderValue = ((tempValue - minValue) / (maxValue - minValue)) *
                           (bounds.width - 2 * static_cast<float>(raygui::GuiGetStyle(SLIDER, BORDER_WIDTH)));

        Rectangle slider = {
                bounds.x,
                bounds.y,
                0,
                bounds.height - static_cast<float>(2 * raygui::GuiGetStyle(SLIDER, BORDER_WIDTH) -
                                                   2 * raygui::GuiGetStyle(SLIDER, SLIDER_PADDING))
        };

        if (sliderWidth > 0)        // Slider
        {
            slider.x += sliderValue - static_cast<float>(sliderWidth) / 2;
            slider.width = static_cast<float>(sliderWidth);
        } else if (0 == sliderWidth)  // SliderBar
        {
            slider.x += static_cast<float>(raygui::GuiGetStyle(SLIDER, BORDER_WIDTH));
            slider.width = sliderValue;
        }

        // Update control
        //--------------------------------------------------------------------
        if ((state != STATE_DISABLED) && (editMode || !raygui::guiLocked)) {
            const Vector2 mousePoint = raylib::Mouse::GetPosition();

            if (raylib::Mouse::IsButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (bounds.CheckCollision(mousePoint)) {
                    pressed = true;
                }
            } else if (raylib::Mouse::IsButtonReleased(MOUSE_LEFT_BUTTON) && editMode) {
                pressed = true;
            }
            if (editMode) {
                state = STATE_PRESSED;
                tempValue = ((maxValue - minValue) *
                             (mousePoint.x - (bounds.x + static_cast<float>(sliderWidth) / 2))) /
                            (bounds.width - static_cast<float>(sliderWidth)) + minValue;

                if (sliderWidth > 0) {
                    slider.x = mousePoint.x - slider.width / 2;  // Slider
                } else if (0 == sliderWidth) {
                    slider.width = sliderValue;          // SliderBar
                }

            } else if (bounds.CheckCollision(mousePoint)) {
                state = STATE_FOCUSED;
            }

            if (tempValue > maxValue) {
                tempValue = maxValue;
            } else if (tempValue < minValue) {
                tempValue = minValue;
            }
        }


        // Bar limits check
        if (sliderWidth > 0)        // Slider
        {
            if (slider.x <= (bounds.x + static_cast<float>(raygui::GuiGetStyle(SLIDER, BORDER_WIDTH)))) {
                slider.x = bounds.x + static_cast<float>(raygui::GuiGetStyle(SLIDER, BORDER_WIDTH));
            } else if ((slider.x + slider.width) >= (bounds.x + bounds.width)) {
                slider.x = bounds.x + bounds.width - slider.width - static_cast<float>(raygui::GuiGetStyle(SLIDER, BORDER_WIDTH));
            }
        } else if (0 == sliderWidth)  // SliderBar
        {
            if (slider.width > bounds.width) {
                slider.width = bounds.width - 2 * static_cast<float>(raygui::GuiGetStyle(SLIDER, BORDER_WIDTH));
            }
        }

        //--------------------------------------------------------------------
        // Draw control
        //--------------------------------------------------------------------
        raygui::GuiDrawRectangle(bounds, raygui::GuiGetStyle(SLIDER, BORDER_WIDTH),
            Fade(raylib::Color(raygui::GuiGetStyle(SLIDER, raygui::BORDER + (state * 3))), raygui::guiAlpha),
            Fade(raylib::Color(raygui::GuiGetStyle(SLIDER,(state != STATE_DISABLED) ? BASE_COLOR_NORMAL : BASE_COLOR_DISABLED)), raygui::guiAlpha));

        // Draw slider internal bar (depends on state)
        if ((state == STATE_NORMAL) || (state == STATE_PRESSED)) {
            raygui::GuiDrawRectangle(slider, 0, BLANK,
                Fade(raylib::Color(raygui::GuiGetStyle(SLIDER, BASE_COLOR_PRESSED)), raygui::guiAlpha));
        } else if (state == STATE_FOCUSED) {
            raygui::GuiDrawRectangle(slider, 0, BLANK,
                Fade(raylib::Color(raygui::GuiGetStyle(SLIDER, TEXT_COLOR_FOCUSED)), raygui::guiAlpha));
        }

        // Draw left/right text if provided
        if (textLeft != nullptr) {
            Rectangle textBounds = {0};
            textBounds.width = static_cast<float>(raygui::GetTextWidth(textLeft));
            textBounds.height = static_cast<float>(raygui::GuiGetStyle(DEFAULT, TEXT_SIZE));
            textBounds.x = bounds.x - textBounds.width - static_cast<float>(raygui::GuiGetStyle(SLIDER, TEXT_PADDING));
            textBounds.y =
                bounds.y + bounds.height / 2 - static_cast<float>(raygui::GuiGetStyle(DEFAULT, TEXT_SIZE)) / 2;

            raygui::GuiDrawText(textLeft, textBounds, TEXT_ALIGN_RIGHT,
                Fade(raylib::Color(raygui::GuiGetStyle(SLIDER, raygui::TEXT + (state * 3))), raygui::guiAlpha));
        }

        if (textRight != nullptr) {
            Rectangle textBounds = {0};
            textBounds.width = static_cast<float>(raygui::GetTextWidth(textRight));
            textBounds.height = static_cast<float>(raygui::GuiGetStyle(DEFAULT, TEXT_SIZE));
            textBounds.x = bounds.x + bounds.width + static_cast<float>(raygui::GuiGetStyle(SLIDER, TEXT_PADDING));
            textBounds.y =
                bounds.y + bounds.height / 2 - static_cast<float>(raygui::GuiGetStyle(DEFAULT, TEXT_SIZE)) / 2;

            raygui::GuiDrawText(textRight, textBounds, TEXT_ALIGN_LEFT,
                Fade(raylib::Color(raygui::GuiGetStyle(SLIDER, raygui::TEXT + (state * 3))), raygui::guiAlpha));
        }
        //--------------------------------------------------------------------

        *value = tempValue;
        return pressed;
    }

    bool GuiSlider(const Rectangle bounds, const char *const textLeft, const char *const textRight, float *const value,
        const float minValue, const float maxValue, const bool editMode) {
        return GuiSliderProOwning(bounds, textLeft, textRight, value, minValue, maxValue,
            raygui::GuiGetStyle(SLIDER, SLIDER_WIDTH), editMode);
    }

    void timeStepSliderAction(const float value) {
        std::string sstString = "sst ";
        sstString += std::to_string(static_cast<int>(value));
        sstString += "\n";
        net::GuiToServerQueue.try_push(sstString.data());
    }

    void createGuiEntities(const flecs::world &ecs, const uint16_t screenWidth, const uint16_t screenHeight) {
        const float menuWidth = 350.f;
        const float arrowRectSize = 40.f;
        const auto menuExpandArrowRect = raylib::Rectangle(static_cast<float>(screenWidth) - arrowRectSize,
                                                           10, arrowRectSize, arrowRectSize);
        const auto menuRetractArrowRect = raylib::Rectangle(static_cast<float>(screenWidth) - menuWidth - arrowRectSize,
                                                            10, arrowRectSize, arrowRectSize);
        const auto menuRect = raylib::Rectangle(static_cast<float>(screenWidth) - menuWidth, 0, menuWidth, screenHeight);
        ecs.entity().set<raylib::Rectangle>(menuExpandArrowRect).add(utils::MenuLabels::MenuExpandArrow);
        ecs.entity().set<raylib::Rectangle>(menuRetractArrowRect).add(utils::MenuLabels::MenuRetractArrow);
        ecs.entity().set<raylib::Rectangle>(menuRect).add(utils::MenuLabels::Menu);
        ecs.entity().set<Slider>(Slider{
            .bounds = Rectangle{static_cast<float>(screenWidth) - menuWidth + 80, 15, menuWidth - 100, 32},
            .minValue = 1.f, .maxValue = 200.f, .value = 1.f, .formatTextLeft = "TimeStep %.0f",
            .formatTextRight = nullptr, .editMode = false, .releaseAction = timeStepSliderAction});
        disableMenuSystems(ecs);
    }

    void disableMenuSystems(const flecs::world &ecs) {
        ecs.lookup("drawMenu").disable();
        ecs.lookup("drawMenuRetractArrow").disable();
        ecs.lookup("drawMenuSliders").disable();
    }

    void destroyGuiEntities(const flecs::world &ecs) {
        ecs.query_builder<raylib::Rectangle>().with(zappy_gui::utils::MenuLabels::MenuExpandArrow).build().destruct();
        ecs.query_builder<raylib::Rectangle>().with(zappy_gui::utils::MenuLabels::MenuRetractArrow).build().destruct();
        ecs.query_builder<raylib::Rectangle>().with(zappy_gui::utils::MenuLabels::Menu).build().destruct();
        ecs.query_builder<Slider>().build().destruct();
    }
}
