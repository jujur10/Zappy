#include <cstdint>
#include <cstring>

#include "Color.hpp"
#include "Mouse.hpp"
#include "Rectangle.hpp"
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include <cfloat>

#include "flecs.h"
#include "gui.hpp"
#include "gui_to_server_cmd_structs.hpp"
#include "gui_to_server_cmd_value.hpp"
#include "map.hpp"
#include "player.hpp"
#include "raygui.h"
#include "raylib_utils.hpp"
#include "time_unit.hpp"

namespace zappy_gui::gui
{

// Constants
constexpr int MAX_RESOURCES = 7;
constexpr int MAX_PLAYERS = 100;  // Adjust as needed

// Enums
enum TileTab
{
    RESOURCES,
    PLAYERS
};

enum WorldTab
{
    ALL_PLAYERS,
    COMMUNICATION,
    SETTINGS
};

// Structs
struct ResourceInfo
{
    const char *name;
    uint16_t value;
    Color color;
};

static raylib::Vector2 tileMenuPosition = {0, 10};
static raylib::Vector2 tileMenuSize = {300, 500};
static bool tileMenuMinimized = true;
static bool tileMenuMoving = false;
static bool tileMenuResizing = false;
static raylib::Vector2 tileMenuScrolling;

static raylib::Vector2 worldMenuPosition = {780, 10};
static raylib::Vector2 worldMenuSize = {500, 600};
static bool worldMenuMinimized = true;
static bool worldMenuMoving = false;
static bool worldMenuResizing = false;
static raylib::Vector2 worldMenuScrolling;

static bool GuiSliderProOwning(raylib::Rectangle bounds,
                               const char *textLeft,
                               const char *textRight,
                               const char *textUp,
                               float *value,
                               const float minValue,
                               float maxValue,
                               const int sliderWidth,
                               bool editMode)
{
    auto state = static_cast<GuiState>(raygui::GuiGetState());
    float tempValue = *value;
    bool pressed = false;

    const auto sliderValue = ((tempValue - minValue) / (maxValue - minValue)) *
                             (bounds.width - 2 * static_cast<float>(raygui::GuiGetStyle(SLIDER, BORDER_WIDTH)));

    raylib::Rectangle slider = {bounds.x,
                                bounds.y,
                                0,
                                bounds.height - static_cast<float>(2 * raygui::GuiGetStyle(SLIDER, BORDER_WIDTH) -
                                                                   2 * raygui::GuiGetStyle(SLIDER, SLIDER_PADDING))};

    if (sliderWidth > 0)  // Slider
    {
        slider.x += sliderValue - static_cast<float>(sliderWidth) / 2;
        slider.width = static_cast<float>(sliderWidth);
    }
    else if (0 == sliderWidth)  // SliderBar
    {
        slider.x += static_cast<float>(raygui::GuiGetStyle(SLIDER, BORDER_WIDTH));
        slider.width = sliderValue;
    }

    // Update control
    //--------------------------------------------------------------------
    if ((state != STATE_DISABLED) && (editMode || !raygui::guiLocked))
    {
        const raylib::Vector2 mousePoint = raylib::Mouse::GetPosition();

        if (raylib::Mouse::IsButtonPressed(MOUSE_LEFT_BUTTON))
        {
            if (bounds.CheckCollision(mousePoint))
            {
                pressed = true;
            }
        }
        else if (raylib::Mouse::IsButtonReleased(MOUSE_LEFT_BUTTON) && editMode)
        {
            pressed = true;
        }
        if (editMode)
        {
            state = STATE_PRESSED;
            tempValue = ((maxValue - minValue) * (mousePoint.x - (bounds.x + static_cast<float>(sliderWidth) / 2))) /
                            (bounds.width - static_cast<float>(sliderWidth)) +
                        minValue;

            if (sliderWidth > 0)
            {
                slider.x = mousePoint.x - slider.width / 2;  // Slider
            }
            else if (0 == sliderWidth)
            {
                slider.width = sliderValue;  // SliderBar
            }
        }
        else if (bounds.CheckCollision(mousePoint))
        {
            state = STATE_FOCUSED;
        }

        if (tempValue > maxValue)
        {
            tempValue = maxValue;
        }
        else if (tempValue < minValue)
        {
            tempValue = minValue;
        }
    }

    // Bar limits check
    if (sliderWidth > 0)  // Slider
    {
        if (slider.x <= (bounds.x + static_cast<float>(raygui::GuiGetStyle(SLIDER, BORDER_WIDTH))))
        {
            slider.x = bounds.x + static_cast<float>(raygui::GuiGetStyle(SLIDER, BORDER_WIDTH));
        }
        else if ((slider.x + slider.width) >= (bounds.x + bounds.width))
        {
            slider.x = bounds.x + bounds.width - slider.width - static_cast<float>(raygui::GuiGetStyle(SLIDER, BORDER_WIDTH));
        }
    }
    else if (0 == sliderWidth)  // SliderBar
    {
        if (slider.width > bounds.width)
        {
            slider.width = bounds.width - 2 * static_cast<float>(raygui::GuiGetStyle(SLIDER, BORDER_WIDTH));
        }
    }

    //--------------------------------------------------------------------
    // Draw control
    //--------------------------------------------------------------------
    raygui::GuiDrawRectangle(
        bounds,
        raygui::GuiGetStyle(SLIDER, BORDER_WIDTH),
        Fade(raylib::Color(raygui::GuiGetStyle(SLIDER, raygui::BORDER + (state * 3))), raygui::guiAlpha),
        Fade(raylib::Color(raygui::GuiGetStyle(SLIDER, (state != STATE_DISABLED) ? BASE_COLOR_NORMAL : BASE_COLOR_DISABLED)),
             raygui::guiAlpha));

    // Draw slider internal bar (depends on state)
    if ((state == STATE_NORMAL) || (state == STATE_PRESSED))
    {
        raygui::GuiDrawRectangle(slider, 0, BLANK, Fade(raylib::Color(raygui::GuiGetStyle(SLIDER, BASE_COLOR_PRESSED)), raygui::guiAlpha));
    }
    else if (state == STATE_FOCUSED)
    {
        raygui::GuiDrawRectangle(slider, 0, BLANK, Fade(raylib::Color(raygui::GuiGetStyle(SLIDER, TEXT_COLOR_FOCUSED)), raygui::guiAlpha));
    }

    // Draw left/right text if provided
    if (textLeft != nullptr)
    {
        raylib::Rectangle textBounds = {0};
        textBounds.width = static_cast<float>(raygui::GetTextWidth(textLeft));
        textBounds.height = static_cast<float>(raygui::GuiGetStyle(DEFAULT, TEXT_SIZE));
        textBounds.x = bounds.x - textBounds.width - static_cast<float>(raygui::GuiGetStyle(SLIDER, TEXT_PADDING));
        textBounds.y = bounds.y + bounds.height / 2 - static_cast<float>(raygui::GuiGetStyle(DEFAULT, TEXT_SIZE)) / 2;

        raygui::GuiDrawText(textLeft,
                            textBounds,
                            TEXT_ALIGN_RIGHT,
                            Fade(raylib::Color(raygui::GuiGetStyle(SLIDER, raygui::TEXT + (state * 3))), raygui::guiAlpha));
    }

    if (textRight != nullptr)
    {
        raylib::Rectangle textBounds = {0};
        textBounds.width = static_cast<float>(raygui::GetTextWidth(textRight));
        textBounds.height = static_cast<float>(raygui::GuiGetStyle(DEFAULT, TEXT_SIZE));
        textBounds.x = bounds.x + bounds.width + static_cast<float>(raygui::GuiGetStyle(SLIDER, TEXT_PADDING));
        textBounds.y = bounds.y + bounds.height / 2 - static_cast<float>(raygui::GuiGetStyle(DEFAULT, TEXT_SIZE)) / 2;

        raygui::GuiDrawText(textRight,
                            textBounds,
                            TEXT_ALIGN_CENTER,
                            Fade(raylib::Color(raygui::GuiGetStyle(SLIDER, raygui::TEXT + (state * 3))), raygui::guiAlpha));
    }

    if (textUp != nullptr)
    {
        Rectangle textBounds;
        textBounds.width = static_cast<float>(raygui::GetTextWidth(textUp));
        textBounds.height = static_cast<float>(raygui::GuiGetStyle(DEFAULT, TEXT_SIZE));
        textBounds.x = bounds.x + bounds.width / 2 - textBounds.width / 2;
        textBounds.y = bounds.y - textBounds.height - static_cast<float>(raygui::GuiGetStyle(TEXTBOX, TEXT_PADDING));

        raygui::GuiDrawText(textUp,
                            textBounds,
                            TEXT_ALIGN_CENTER,
                            Fade(raylib::Color(raygui::GuiGetStyle(SLIDER, raygui::TEXT + (state * 3))), raygui::guiAlpha));
    }
    //--------------------------------------------------------------------

    *value = tempValue;
    return pressed;
}

bool GuiSlider(const raylib::Rectangle bounds,
               const char *const textLeft,
               const char *const textRight,
               const char *const textUp,
               float *const value,
               const float minValue,
               const float maxValue,
               const bool editMode)
{
    return GuiSliderProOwning(
        bounds, textLeft, textRight, textUp, value, minValue, maxValue, raygui::GuiGetStyle(SLIDER, SLIDER_WIDTH), editMode);
}

void timeStepSliderAction(float value)
{
    auto *sstString = new char[17];

    std::memcpy(sstString, GUI_SET_TIME_UNIT, 4);  // NOLINT

    value = std::round(value);

    const auto [ptr, ec] = std::to_chars(sstString + 4, sstString + 14, static_cast<uint32_t>(value));
    if (ec != std::errc())
    {
        return;
    }

    *ptr = '\n';
    *(ptr + 1) = '\0';

    net::GuiToServerQueue.try_push(sstString);
}

void createGuiEntities(const flecs::world &ecs, const uint16_t screenWidth, const uint16_t screenHeight)
{
    const float menuWidth = 350.f;
    const float arrowRectSize = 40.f;
    const auto menuExpandArrowRect = raylib::Rectangle(static_cast<float>(screenWidth) - arrowRectSize, 10, arrowRectSize, arrowRectSize);
    const auto menuRetractArrowRect =
        raylib::Rectangle(static_cast<float>(screenWidth) - menuWidth - arrowRectSize, 10, arrowRectSize, arrowRectSize);
    const auto menuRect = raylib::Rectangle(static_cast<float>(screenWidth) - menuWidth, 0, menuWidth, screenHeight);
    ecs.entity().set<raylib::Rectangle>(menuExpandArrowRect).add(utils::MenuLabels::MenuExpandArrow);
    ecs.entity().set<raylib::Rectangle>(menuRetractArrowRect).add(utils::MenuLabels::MenuRetractArrow);
    ecs.entity().set<raylib::Rectangle>(menuRect).add(utils::MenuLabels::Menu);
    ecs.entity().set<Slider>(
        Slider{.bounds = raylib::Rectangle{worldMenuPosition.x + 45, worldMenuPosition.y + 50, worldMenuSize.x - 90, 50},
               .minValue = 1.f,
               .maxValue = 200.f,
               .value = 1.f,
               .formatTextLeft = "%.0f",
               .formatTextRight = nullptr,
               .formatTextUp = "Periode de temps",
               .editMode = false,
               .releaseAction = timeStepSliderAction});
}

template <typename DrawContentFunc>  // equivalent to void (*draw_content)(raylib::Vector2, raylib::Vector2, flecs::world &)
void GuiWindowFloating(raylib::Vector2 *const position,
                       raylib::Vector2 *const size,
                       bool *const minimize,
                       bool *const move,
                       bool *const resize,
                       DrawContentFunc draw_content,
                       raylib::Vector2 content_size,
                       raylib::Vector2 *scroll,
                       const char *title,
                       flecs::world &ecs)
{
#if !defined(RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT)
#define RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT 24
#endif

#if !defined(RAYGUI_WINDOW_CLOSEBUTTON_SIZE)
#define RAYGUI_WINDOW_CLOSEBUTTON_SIZE 18
#endif

    constexpr uint8_t closeTitleSizeDeltaHalf = (RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT - RAYGUI_WINDOW_CLOSEBUTTON_SIZE) / 2;

    // window movement and resize input and collision check
    if (raylib::Mouse::IsButtonPressed(MOUSE_LEFT_BUTTON) && !*move && !*resize)
    {
        const raylib::Vector2 mousePosition = raylib::Mouse::GetPosition();

        const raylib::Rectangle titleCollisionRect = {position->x,
                                                      position->y,
                                                      size->x - (RAYGUI_WINDOW_CLOSEBUTTON_SIZE + closeTitleSizeDeltaHalf),
                                                      RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT};
        const raylib::Rectangle resizeCollisionRect = {position->x + size->x - 20, position->y + size->y - 20, 20, 20};

        if (titleCollisionRect.CheckCollision(mousePosition))
        {
            *move = true;
            if (!ecs.has<InUI>())
            {
                ecs.add<InUI>();
            }
            ecs.entity<Hover>().disable();
        }
        else if (!*minimize && resizeCollisionRect.CheckCollision(mousePosition))
        {
            *resize = true;
            if (!ecs.has<InUI>())
            {
                ecs.add<InUI>();
            }
            ecs.entity<Hover>().disable();
        }
    }

    // window movement and resize update
    if (*move)
    {
        const raylib::Vector2 mouseDelta = raylib::Mouse::GetDelta();
        position->x += mouseDelta.x;
        position->y += mouseDelta.y;

        if (raylib::Mouse::IsButtonReleased(MOUSE_LEFT_BUTTON))
        {
            *move = false;

            ecs.remove<InUI>();
            ecs.entity<Hover>().enable();

            // clamp window position keep it inside the application area
            if (position->x < 0)
            {
                position->x = 0;
            }
            else if (position->x > static_cast<float>(::GetScreenWidth()) - size->x)
            {
                position->x = static_cast<float>(::GetScreenWidth()) - size->x;
            }
            if (position->y < 0)
            {
                position->y = 0;
            }
            else if (position->y > static_cast<float>(::GetScreenHeight()))
            {
                position->y = static_cast<float>(::GetScreenHeight() - RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT);
            }
        }
    }
    else if (*resize)
    {
        const raylib::Vector2 mouse = raylib::Mouse::GetPosition();
        if (mouse.x > position->x)
        {
            size->x = mouse.x - position->x;
        }
        if (mouse.y > position->y)
        {
            size->y = mouse.y - position->y;
        }

        // clamp window size to an arbitrary minimum value and the window size as the maximum
        if (size->x < 200)
        {
            size->x = 200;
        }
        else if (size->x > static_cast<float>(::GetScreenWidth()))
        {
            size->x = static_cast<float>(::GetScreenWidth());
        }
        if (size->y < 350)
        {
            size->y = 350;
        }
        else if (size->y > static_cast<float>(::GetScreenHeight()))
        {
            size->y = static_cast<float>(::GetScreenHeight());
        }

        if (raylib::Mouse::IsButtonReleased(MOUSE_LEFT_BUTTON))
        {
            *resize = false;
            ecs.remove<InUI>();
            ecs.entity<Hover>().enable();
        }
    }

    // window and content drawing with scissor and scroll area
    if (*minimize)
    {
        raygui::GuiStatusBar(raylib::Rectangle{position->x, position->y, size->x, RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT}, title);

        if (raygui::GuiButton(raylib::Rectangle{position->x + size->x - RAYGUI_WINDOW_CLOSEBUTTON_SIZE - closeTitleSizeDeltaHalf,
                                                position->y + closeTitleSizeDeltaHalf,
                                                RAYGUI_WINDOW_CLOSEBUTTON_SIZE,
                                                RAYGUI_WINDOW_CLOSEBUTTON_SIZE},
                              "#120#"))
        {
            *minimize = false;
        }
    }
    else
    {
        *minimize = raygui::GuiWindowBox((Rectangle){position->x, position->y, size->x, size->y}, title);

        // scissor and draw content within a scroll panel
        const raylib::Rectangle scissor = {0};

        const bool requireScissor = size->x < content_size.x || size->y < content_size.y;

        if (requireScissor)
        {
            ::BeginScissorMode(scissor.x, scissor.y, scissor.width, scissor.height);
        }

        draw_content(*position, *scroll, ecs);

        if (requireScissor)
        {
            ::EndScissorMode();
        }

        // draw the resize button/icon
        raygui::GuiDrawIcon(71, position->x + size->x - 20, position->y + size->y - 20, 1, WHITE);
    }
}

void DrawTileMenuTabs(const raylib::Vector2 position, const raylib::Vector2 scroll, int *const activeTab)
{
    const char *tabs[] = {"Ressources", "Joueurs"};
    const int icons[] = {raygui::ICON_COIN, raygui::ICON_PLAYER};
    const raylib::Rectangle tabRect = {
        position.x + scroll.x, position.y + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT + scroll.y, tileMenuSize.x / 2, 25};
    raygui::GuiTabBar(tabRect, tabs, icons, 2, activeTab);
}

void DrawTileMenuResourceList(const raylib::Vector2 position,
                              const raylib::Vector2 scroll,
                              const ResourceInfo *const resources,
                              const raylib::Vector2 size)
{
    for (uint8_t r = 0; r < MAX_RESOURCES; ++r)
    {
        const raylib::Rectangle nameRect = {position.x + 20 + scroll.x, position.y + tileMenuSize.y - 400 + r * 40 + scroll.y, 100, 25};
        raygui::GuiDrawText(resources[r].name, nameRect, TEXT_ALIGN_LEFT, resources[r].color);

        char valueBuffer[7] = {0};
        std::to_chars(&valueBuffer[0], &valueBuffer[6], resources[r].value);

        const raylib::Rectangle valueRect = {
            position.x + 50 + scroll.x, position.y + tileMenuSize.y - 400 + r * 40 + scroll.y, size.x - 100, 25};
        raygui::GuiDrawText(valueBuffer, valueRect, TEXT_ALIGN_RIGHT, resources[r].color);
    }
}

void DrawTileMenuResources(const raylib::Vector2 position,
                           const raylib::Vector2 scroll,
                           const flecs::world &ecs,
                           const raylib::Vector2 size)
{
    const auto *selection = ecs.get<Selection>();
    if (!selection)
        return;

    const auto selectionEntity = ecs.entity(selection->entityId);
    if (!selectionEntity)
        return;

    const auto *currentResourceIds = selectionEntity.get<map::resourceIds>();
    if (!currentResourceIds)
        return;

    ResourceInfo resources[MAX_RESOURCES] = {{"Nourriture: ", 0, BLACK},
                                             {"Linemate: ", 0, map::resourceColors[0]},
                                             {"Deraumere: ", 0, map::resourceColors[1]},
                                             {"Sibur: ", 0, map::resourceColors[2]},
                                             {"Mendiane: ", 0, map::resourceColors[3]},
                                             {"Phiras: ", 0, map::resourceColors[4]},
                                             {"Thystame: ", 0, map::resourceColors[5]}};

    for (uint8_t i = 0; i < MAX_RESOURCES; ++i)
    {
        const auto resourceEntity = ecs.entity(currentResourceIds->array[i]);
        if (resourceEntity && resourceEntity.enabled())
        {
            resources[i].value = resourceEntity.ensure<uint16_t>();
        }
    }

    DrawTileMenuResourceList(position, scroll, resources, size);
}

void DrawMenuPlayerInfo(const raylib::Vector2 position,
                        const raylib::Vector2 scroll,
                        const flecs::entity player,
                        const raylib::Vector2 &size)
{
    if (!player)
    {
        return;
    }
    BeginScissorMode(position.x, position.y + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT, size.x, size.y - RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT);

    if (!player.is_alive())
    {
        return;
    }
    const player::PlayerInventory *inventory = player.get<player::PlayerInventory>();
    const uint8_t *level = player.get<uint8_t>();
    const uint64_t *teamId = player.get<uint64_t>();

    raygui::GuiDrawText("Informations:",
                        raylib::Rectangle{position.x + 22 + scroll.x, position.y + size.y - 280 + scroll.y, size.x - 44, 25},
                        TEXT_ALIGN_CENTER,
                        BLACK);

    // Draw team information
    raygui::GuiDrawText(
        "Equipe: ", raylib::Rectangle{position.x + 22 + scroll.x, position.y + size.y - 230 + scroll.y, 100, 25}, TEXT_ALIGN_LEFT, BLACK);
    if (teamId && player.world().exists(*teamId))
    {
        const flecs::entity team = player.world().entity(*teamId);
        const char *const teamName = team.name().c_str();
        if (teamName)
        {
            raygui::GuiDrawText(teamName,
                                raylib::Rectangle{position.x + 22 + scroll.x, position.y + size.y - 230 + scroll.y, size.x - 44, 25},
                                TEXT_ALIGN_RIGHT,
                                BLACK);
        }
    }

    // Draw level information
    raygui::GuiDrawText(
        "Niveau: ", raylib::Rectangle{position.x + 22 + scroll.x, position.y + size.y - 205 + scroll.y, 100, 25}, TEXT_ALIGN_LEFT, BLACK);
    if (level)
    {
        char levelBuffer[3] = {0};
        std::to_chars(&levelBuffer[0], &levelBuffer[2], *level);
        raygui::GuiDrawText(levelBuffer,
                            raylib::Rectangle{position.x + 22 + scroll.x, position.y + size.y - 205 + scroll.y, size.x - 44, 25},
                            TEXT_ALIGN_RIGHT,
                            BLACK);
    }

    // Draw inventory information
    if (inventory)
    {
        constexpr const char *resourceNames[MAX_RESOURCES] = {
            "Nourriture: ", "Linemate: ", "Deraumere: ", "Sibur: ", "Mendiane: ", "Phiras: ", "Thystame: "};
        constexpr Color resourceColors[MAX_RESOURCES] = {BLACK,
                                                         map::resourceColors[0],
                                                         map::resourceColors[1],
                                                         map::resourceColors[2],
                                                         map::resourceColors[3],
                                                         map::resourceColors[4],
                                                         map::resourceColors[5]};

        for (uint8_t r = 0; r < MAX_RESOURCES; ++r)
        {
            raygui::GuiDrawText(resourceNames[r],
                                raylib::Rectangle{position.x + 22 + scroll.x, position.y + size.y - 180 + r * 25 + scroll.y, 100, 25},
                                TEXT_ALIGN_LEFT,
                                resourceColors[r]);
            char resourceBuffer[7] = {0};
            std::to_chars(&resourceBuffer[0], &resourceBuffer[6], inventory->resources[r]);
            raygui::GuiDrawText(
                resourceBuffer,
                raylib::Rectangle{position.x + 22 + scroll.x, position.y + size.y - 180 + r * 25 + scroll.y, size.x - 44, 25},
                TEXT_ALIGN_RIGHT,
                resourceColors[r]);
        }
    }

    EndScissorMode();
}

void DrawTileMenuPlayerList(const raylib::Vector2 position, const raylib::Vector2 scroll, const flecs::world &ecs)
{
    static int listViewExScrollIndex = 0;
    static int listViewExActive = -1;
    static int listViewExFocus = -1;

    const auto *selection = ecs.get<Selection>();
    if (!selection)
        return;

    const auto selectionEntity = ecs.entity(selection->entityId);
    if (!selectionEntity)
        return;

    const auto *selectionMatrix = selectionEntity.get<raylib::Matrix>();
    if (!selectionMatrix)
        return;

    const raylib::Vector3 selectionPosition = {selectionMatrix->m12, selectionMatrix->m13, selectionMatrix->m14};

    static const flecs::filter<Vector2> pQuery = ecs.filter_builder<Vector2>().with<player::PlayerInventory>().build();

    const auto totalPlayerCount = pQuery.count();
    char listViewExList[totalPlayerCount][16] = {0};
    uint64_t playerIdx[totalPlayerCount];
    uint16_t playerCount = 0;

    pQuery.each(
        [&](const flecs::entity e, const raylib::Vector2 &pos)
        {
            if (std::fabs(pos.x - selectionPosition.x) > 0.2 || std::fabs(pos.y - selectionPosition.z) > 0.2)
                return;

            std::strncpy(listViewExList[playerCount], "Joueur: ", 9);
            std::to_chars(listViewExList[playerCount] + 8, listViewExList[playerCount] + 15, e.id() - PLAYER_STARTING_IDX);
            playerIdx[playerCount] = e.id();
            ++playerCount;
        });

    char *listViewExListPtrs[totalPlayerCount];
    for (uint16_t i = 0; i < totalPlayerCount; ++i)
    {
        listViewExListPtrs[i] = listViewExList[i];
    }

    const raylib::Rectangle listRect = {position.x + 22 + scroll.x, position.y + 60 + scroll.y, tileMenuSize.x - 44, tileMenuSize.y - 350};
    raygui::GuiListViewEx(listRect, listViewExListPtrs, playerCount, &listViewExScrollIndex, &listViewExActive, &listViewExFocus);

    if (listViewExActive != -1 && playerCount > 0)
    {
        const auto player = ecs.entity(playerIdx[listViewExActive]);
        if (player)
        {
            DrawMenuPlayerInfo(position, scroll, player, tileMenuSize);
        }
    }
}

void DrawTileMenuContent(const raylib::Vector2 position, const raylib::Vector2 scroll, flecs::world &ecs)
{
    static int activeTab = 0;
    DrawTileMenuTabs(position, scroll, &activeTab);

    if (activeTab == RESOURCES)
    {
        DrawTileMenuResources(position, scroll, ecs, tileMenuSize);
    }
    else
    {
        DrawTileMenuPlayerList(position, scroll, ecs);
    }
}

void TileFloatingWindowMenu(flecs::world ecs)
{
    GuiWindowFloating(&tileMenuPosition,
                      &tileMenuSize,
                      &tileMenuMinimized,
                      &tileMenuMoving,
                      &tileMenuResizing,
                      DrawTileMenuContent,
                      raylib::Vector2{tileMenuSize.x, tileMenuSize.y},
                      &tileMenuScrolling,
                      "Tuile",
                      ecs);
}

void DrawWorldMenuPlayerList(const raylib::Vector2 position, const raylib::Vector2 scroll, const flecs::world &ecs)
{
    static int listViewExScrollIndex = 0;
    static int listViewExActive = -1;
    static int listViewExFocus = -1;

    static const flecs::filter pQuery = ecs.filter_builder().with<player::PlayerInventory>().build();

    const auto totalPlayerCount = pQuery.count();
    char listViewExList[totalPlayerCount][16] = {0};
    uint64_t playerIdx[totalPlayerCount];
    uint16_t playerCount = 0;

    pQuery.each(
        [&](const flecs::entity e)
        {
            std::strncpy(listViewExList[playerCount], "Joueur: ", 9);
            std::to_chars(listViewExList[playerCount] + 8, listViewExList[playerCount] + 15, e.id() - PLAYER_STARTING_IDX);
            playerIdx[playerCount] = e.id();
            ++playerCount;
        });

    char *listViewExListPtrs[totalPlayerCount];
    for (uint16_t i = 0; i < totalPlayerCount; ++i)
    {
        listViewExListPtrs[i] = listViewExList[i];
    }

    const raylib::Rectangle listRect = {
        position.x + 22 + scroll.x, position.y + 60 + scroll.y, worldMenuSize.x - 44, worldMenuSize.y - 350};
    raygui::GuiListViewEx(listRect, listViewExListPtrs, playerCount, &listViewExScrollIndex, &listViewExActive, &listViewExFocus);

    if (listViewExActive != -1 && playerCount > 0)
    {
        const auto player = ecs.entity(playerIdx[listViewExActive]);
        if (player)
        {
            DrawMenuPlayerInfo(position, scroll, player, worldMenuSize);
        }
    }
}

void DrawWorldMenuTabs(const raylib::Vector2 position, const raylib::Vector2 scroll, int *const activeTab)
{
    const char *tabs[3] = {"Joueurs", "Communication", "Parametres"};
    const int icons[3] = {raygui::ICON_PLAYER, raygui::ICON_TEXT_POPUP, raygui::ICON_GEAR_BIG};
    const raylib::Rectangle tabRect = {
        position.x + scroll.x, position.y + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT + scroll.y, worldMenuSize.x / 3, 25};
    raygui::GuiTabBar(tabRect, tabs, icons, 3, activeTab);
}

void DrawWorldMenuCommunication(const raylib::Vector2 position, const raylib::Vector2 scroll, const flecs::world &ecs)
{
    const Rectangle chatBox = {position.x + 11 + scroll.x, position.y + 60 + scroll.y, worldMenuSize.x - 11, worldMenuSize.y - 75};

    const auto *const chat = ecs.get<ChatHistory>();
    if (!chat)
    {
        return;
    }

    ::BeginScissorMode(chatBox.x, chatBox.y, chatBox.width, chatBox.height);
    for (uint32_t i = 0; i < chat->messageCount; ++i)
    {
        raygui::GuiDrawText(chat->messages[i], raylib::Rectangle{chatBox.x + 5, chatBox.y + 5 + i * 20, 1000, 25}, TEXT_ALIGN_LEFT, BLACK);
    }
    ::EndScissorMode();
}

void DrawWorldSettings(const raylib::Vector2 position, [[maybe_unused]] const raylib::Vector2 scroll, const flecs::world &ecs)
{
    static const flecs::filter sliderQuery = ecs.filter<Slider>();

    sliderQuery.each(
        [&](const flecs::entity &e, Slider &slider)
        {
            slider.bounds.x = position.x + 45;
            slider.bounds.y = position.y + 100;
            slider.bounds.width = worldMenuSize.x - 90;
            slider.bounds.height = 30;
            const char *textLeft = nullptr;
            const char *textRight = nullptr;
            const char *textUp = nullptr;
            if (slider.formatTextLeft != nullptr)
            {
                textLeft = ::TextFormat(slider.formatTextLeft, slider.value);
            }
            if (slider.formatTextRight != nullptr)
            {
                textRight = ::TextFormat(slider.formatTextRight, slider.value);
            }
            if (slider.formatTextUp != nullptr)
            {
                textUp = ::TextFormat(slider.formatTextUp, slider.value);
            }

            const auto *const frequency = e.world().get<TimeUnit>();
            if (frequency != nullptr && std::fabs(slider.value - static_cast<float>(frequency->frequency)) > FLT_EPSILON)
            {
                slider.value = static_cast<float>(frequency->frequency);
            }

            if (GuiSlider(slider.bounds, textLeft, textRight, textUp, &slider.value, slider.minValue, slider.maxValue, slider.editMode))
            {
                if (slider.editMode)
                {
                    slider.releaseAction(slider.value);
                }
                slider.editMode = !slider.editMode;
            }
        });
}

void DrawWorldMenuContent(const raylib::Vector2 position, const raylib::Vector2 scroll, const flecs::world &ecs)
{
    static int activeTab = 0;
    DrawWorldMenuTabs(position, scroll, &activeTab);

    if (activeTab == ALL_PLAYERS)
    {
        DrawWorldMenuPlayerList(position, scroll, ecs);
    }
    else if (activeTab == COMMUNICATION)
    {
        DrawWorldMenuCommunication(position, scroll, ecs);
    }
    else
    {
        DrawWorldSettings(position, scroll, ecs);
    }
}

void WorldFloatingWindowMenu(flecs::world ecs)
{
    raygui::GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
    GuiWindowFloating(&worldMenuPosition,
                      &worldMenuSize,
                      &worldMenuMinimized,
                      &worldMenuMoving,
                      &worldMenuResizing,
                      DrawWorldMenuContent,
                      raylib::Vector2{worldMenuSize.x - 27, worldMenuSize.y - 20},
                      &worldMenuScrolling,
                      "Monde",
                      ecs);
}
}  // namespace zappy_gui::gui
