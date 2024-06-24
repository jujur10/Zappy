#pragma once

#include <Model.hpp>
#include <Vector3.hpp>

#include "Rectangle.hpp"
#include "RenderTexture.hpp"
#include "Shader.hpp"

namespace zappy_gui::gui
{

using SliderReleaseAction = void (*)(float value);

/// @brief Tag to represent if the mouse is in the UI
struct InUI
{
};

/// @brief Represent
struct Selection
{
    raylib::Shader *sobolShader;
    raylib::RenderTexture *renderTexture;
    raylib::Vector3 *position;
};

struct Slider
{
    Rectangle bounds;
    float minValue;
    float maxValue;
    float value;
    const char *formatTextLeft;
    const char *formatTextRight;
    bool editMode;
    SliderReleaseAction releaseAction;
};

/// Draw a slider
/// @param bounds The bounds of the slider
/// @param textLeft The text on the left of the slider
/// @param textRight The text on the left of the slider
/// @param value A pointer to the value of the slider
/// @param minValue The minimum value of the slider
/// @param maxValue The maximum value of the slider
/// @param editMode Can the value be changed
/// @return True if the slider was pressed
bool GuiSlider(Rectangle bounds, const char *textLeft, const char *textRight, float *value, float minValue, float maxValue, bool editMode);

/// @brief Create the GUI clickable entities
///
/// @param ecs The ECS to register the entities in
/// @param screenWidth The width of the screen
void createGuiEntities(const flecs::world &ecs, uint16_t screenWidth, uint16_t screenHeight);

/// Release action for the time step slider
/// @param value the value of the slider
void timeStepSliderAction(float value);

/// Disable the menu systems that should not be displayed at startup
/// @param ecs The ECS
void disableMenuSystems(const flecs::world &ecs);

void GuiWindowFloatingTest(flecs::world ecs);
}  // namespace zappy_gui::gui
