#pragma once

#include <Model.hpp>
#include <Vector3.hpp>
#include <cstdint>
#include <memory>

#include "RenderTexture.hpp"
#include "Shader.hpp"

namespace raylib
{
class Rectangle;
}
namespace flecs
{
struct world;
}

namespace zappy_gui::gui
{

using SliderReleaseAction = void (*)(float value);

/// @brief Tag to represent if the mouse is in the UI
struct InUI
{
};

/// @brief Represent hovering effect
struct HoverEffect
{
    raylib::Shader *sobolShader;
    std::unique_ptr<raylib::RenderTexture> renderTexture;
};

/// @brief Represent hovered entity
struct Hover
{
    raylib::Model *model;
    raylib::Vector3 *position;
};

/// @brief Represent the selected player or tile
struct Selection
{
    uint64_t entityId;
};

struct Slider
{
    Rectangle bounds;
    float minValue;
    float maxValue;
    float value;
    const char *formatTextLeft;
    const char *formatTextRight;
    const char *formatTextUp;
    bool editMode;
    SliderReleaseAction releaseAction;
};

#define MAX_MESSAGES 40
#define MAX_MESSAGE_LENGTH 128

struct ChatHistory {
    char messages[MAX_MESSAGES][MAX_MESSAGE_LENGTH];
    uint32_t messageCount = 0;
};

/// Draw a slider
/// @param bounds The bounds of the slider
/// @param textLeft The text on the left of the slider
/// @param textRight The text on the left of the slider
/// @param textUp The text on the top of the slider
/// @param value A pointer to the value of the slider
/// @param minValue The minimum value of the slider
/// @param maxValue The maximum value of the slider
/// @param editMode Can the value be changed
/// @return True if the slider was pressed
bool GuiSlider(raylib::Rectangle bounds, const char *textLeft, const char *textRight, const char *textUp,
    float *value, float minValue, float maxValue, bool editMode);

/// @brief Create the GUI clickable entities
///
/// @param ecs The ECS to register the entities in
/// @param screenWidth The width of the screen
/// @param screenHeight The height of the screen
void createGuiEntities(const flecs::world &ecs, uint16_t screenWidth, uint16_t screenHeight);

/// Release action for the time step slider
/// @param value the value of the slider
void timeStepSliderAction(float value);

void TileFloatingWindowMenu(flecs::world ecs);

void WorldFloatingWindowMenu(flecs::world ecs);
}  // namespace zappy_gui::gui
