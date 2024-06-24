#include "Mouse.hpp"
#include "Rectangle.hpp"
#include "raylib.h"
#include "Color.hpp"
#include <cstdint>
#include <cstring>
#define RAYGUI_IMPLEMENTATION
#include "player.hpp"

#include "flecs.h"
#include "gui.hpp"
#include "gui_to_server_cmd_structs.hpp"
#include "gui_to_server_cmd_value.hpp"
#include "map.hpp"
#include "raygui.h"
#include "raylib_utils.hpp"

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

    void timeStepSliderAction(float value)
{
        auto *sstString = new char[17];

        ::memcpy(sstString, GUI_SET_TIME_UNIT, 4); // NOLINT

        value = std::round(value);

        const auto [ptr, ec] = std::to_chars(sstString + 4, sstString + 14, static_cast<uint32_t>(value));
        if (ec != std::errc()) {
            return;
        }

        *ptr = '\n';
        *(ptr + 1) = '\0';

        net::GuiToServerQueue.try_push(sstString);
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

    static Vector2 window_position = { 10, 10 };
    static Vector2 window_size = { 206, 400 };
    static bool minimized = false;
    static bool moving = false;
    static bool resizing = false;
    static Vector2 scroll;

    void GuiWindowFloating(Vector2 *position,
        Vector2 *size,
        bool *minimized,
        bool *moving,
        bool *resizing,
        void (*draw_content)(Vector2, Vector2, flecs::world&),
        Vector2 content_size,
        Vector2 *scroll,
        const char* title,
        flecs::world &ecs)
{
        #if !defined(RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT)
            #define RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT 24
        #endif

        #if !defined(RAYGUI_WINDOW_CLOSEBUTTON_SIZE)
            #define RAYGUI_WINDOW_CLOSEBUTTON_SIZE 18
        #endif

        int close_title_size_delta_half = (RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT - RAYGUI_WINDOW_CLOSEBUTTON_SIZE) / 2;

        // window movement and resize input and collision check
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !*moving && !*resizing) {
            Vector2 mouse_position = GetMousePosition();

            Rectangle title_collision_rect = { position->x, position->y, size->x - (RAYGUI_WINDOW_CLOSEBUTTON_SIZE + close_title_size_delta_half), RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT };
            Rectangle resize_collision_rect = { position->x + size->x - 20, position->y + size->y - 20, 20, 20 };

            if(CheckCollisionPointRec(mouse_position, title_collision_rect)) {
                *moving = true;
            } else if(!*minimized && CheckCollisionPointRec(mouse_position, resize_collision_rect)) {
                *resizing = true;
            }
        }

        // window movement and resize update
        if(*moving) {
            Vector2 mouse_delta = GetMouseDelta();
            position->x += mouse_delta.x;
            position->y += mouse_delta.y;

            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                *moving = false;

                // clamp window position keep it inside the application area
                if(position->x < 0) position->x = 0;
                else if(position->x > GetScreenWidth() - size->x) position->x = GetScreenWidth() - size->x;
                if(position->y < 0) position->y = 0;
                else if(position->y > GetScreenHeight()) position->y = GetScreenHeight() - RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT;
            }

        } else if(*resizing) {
            Vector2 mouse = GetMousePosition();
            if (mouse.x > position->x)
                size->x = mouse.x - position->x;
            if (mouse.y > position->y)
                size->y = mouse.y - position->y;

            // clamp window size to an arbitrary minimum value and the window size as the maximum
            if(size->x < 100) size->x = 100;
            else if(size->x > GetScreenWidth()) size->x = GetScreenWidth();
            if(size->y < 100) size->y = 100;
            else if(size->y > GetScreenHeight()) size->y = GetScreenHeight();

            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                *resizing = false;
            }
        }

        // window and content drawing with scissor and scroll area
        if(*minimized) {
            raygui::GuiStatusBar((Rectangle){ position->x, position->y, size->x, RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT }, title);

            if (raygui::GuiButton((Rectangle){ position->x + size->x - RAYGUI_WINDOW_CLOSEBUTTON_SIZE - close_title_size_delta_half,
                                       position->y + close_title_size_delta_half,
                                       RAYGUI_WINDOW_CLOSEBUTTON_SIZE,
                                       RAYGUI_WINDOW_CLOSEBUTTON_SIZE },
                                       "#120#")) {
                *minimized = false;
            }

        } else {
            *minimized = raygui::GuiWindowBox((Rectangle) { position->x, position->y, size->x, size->y }, title);

            // scissor and draw content within a scroll panel
            if(draw_content != NULL) {
                Rectangle scissor = {0};
                raygui::GuiScrollPanel((Rectangle) { position->x, position->y + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT, size->x, size->y - RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT },
                                             NULL,
                                             (Rectangle) { position->x, position->y, content_size.x, content_size.y },
                                             scroll,
                                             &scissor);

                bool require_scissor = size->x < content_size.x || size->y < content_size.y;

                if(require_scissor) {
                    BeginScissorMode(scissor.x, scissor.y, scissor.width, scissor.height);
                }

                draw_content(*position, *scroll, ecs);

                if(require_scissor) {
                    EndScissorMode();
                }
            }

            // draw the resize button/icon
            raygui::GuiDrawIcon(71, position->x + size->x - 20, position->y + size->y - 20, 1, WHITE);
        }
    }

    static void DrawContent(Vector2 position, Vector2 scroll, flecs::world &ecs)
    {
        const char* *tabs = (const char*[]){"Ressources", "Joueurs"};
        const int *icons = (const int[]){raygui::ICON_COIN, raygui::ICON_PLAYER};
        static int activeTab = 0;
        raygui::GuiTabBar(Rectangle{position.x + scroll.x, position.y + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT + scroll.y, 100, 25}, tabs, icons, 2, &activeTab);
        char *resourceNames[] = {"Nourriture: ", "Linemate: ", "Deraumere: ", "Sibur: ", "Mendiane: ", "Phiras: ", "Thystame: "};
        int resourceValues[] = {0, 0, 0, 0, 0, 0, 0};




        if (0 == activeTab)
        {
            for (int i = 0; i < 7; i++) {
                char resourceBuffer[7] = {0};
                raygui::GuiDrawText(resourceNames[i], (Rectangle){position.x + 20 + scroll.x, position.y + 50 + i * 25 + scroll.y, 100, 25}, TEXT_ALIGN_LEFT, map::resourceColors[i]);
                std::to_chars(&resourceBuffer[0], &resourceBuffer[6], resourceValues[i]);
                raygui::GuiDrawText(resourceBuffer, (Rectangle){position.x + 50 + scroll.x, position.y + 50 + i * 25 + scroll.y, 100, 25}, TEXT_ALIGN_RIGHT, map::resourceColors[i]);
            }
        }
        else
        {
            static int listViewExScrollIndex = 0;
            static int listViewExActive = 2;
            static int listViewExFocus = -1;

            /// Get the selection position
            const auto * const selection = ecs.get<Selection>();

            /// Query the position of all players
            static flecs::filter<Vector2> pQuery = ecs.filter_builder<Vector2>().with<player::PlayerInventory>().build();

            const auto totalPlayerCount = pQuery.count();
            const uint16_t playerCount = 0;
            char listViewExList[totalPlayerCount][16] = {0};
            uint64_t playerIdx[totalPlayerCount];
            std::memset(listViewExList, 0, sizeof(char) * totalPlayerCount * 16);

            uint16_t i = 0;
            pQuery.each([&listViewExList, &i, &selection, &playerIdx](const flecs::entity e, const Vector2 &pos) {
                if (std::fabs(pos.x - selection->position->x) > 0.1 && std::fabs(pos.y - selection->position->y) > 0.1)
                {
                    return;
                }
                std::strncpy(listViewExList[i], "Joueur: ", 9);
                std::to_chars(listViewExList[i] + 8, listViewExList[i] + 15, e.id() - PLAYER_STARTING_IDX);
                playerIdx[i] = e.id();
                ++i;
            });

            raygui::GuiListViewEx((Rectangle){ position.x + 22 + scroll.x, position.y + 50 + scroll.y, 168, 100 }, reinterpret_cast<char**>(listViewExList), playerCount, &listViewExScrollIndex, &listViewExActive, &listViewExFocus);

            if (listViewExActive != -1)
            {
                raygui::GuiDrawText("Informations:", (Rectangle){position.x + 32 + scroll.x, position.y + 200 + scroll.y, 100, 35}, TEXT_ALIGN_LEFT, BLACK);

                raygui::GuiDrawText("Equipe: ", (Rectangle){position.x + 22 + scroll.x, position.y + 250 + scroll.y, 100, 25}, TEXT_ALIGN_LEFT, BLACK);

                raygui::GuiDrawText("Niveau: ", (Rectangle){position.x + 122 + scroll.x, position.y + 250 + scroll.y, 100, 25}, TEXT_ALIGN_LEFT, BLACK);

                for (uint8_t r = 0; i < 7; i++) {
                    raygui::GuiDrawText(resourceNames[r], (Rectangle){position.x + 22 + scroll.x, position.y + 270 + r * 25 + scroll.y, 100, 25}, TEXT_ALIGN_LEFT, map::resourceColors[r]);
                }

                if (totalPlayerCount == 0)
                {
                    return;
                }

                const auto player = ecs.entity(playerIdx[listViewExActive]);
                const player::PlayerInventory* inventory = nullptr;
                const uint8_t *level = nullptr;
                const uint64_t *teamId = nullptr;

                if (player)
                {
                    inventory = player.get<player::PlayerInventory>();
                    level = player.get<uint8_t>();
                    teamId = player.get<uint64_t>();
                }

                if (teamId && ecs.exists(*teamId))
                {
                    flecs::entity team = ecs.entity(*teamId);
                    const auto *teamName = team.name().c_str();
                    if (nullptr != teamName)
                    {
                        raygui::GuiDrawText(teamName, (Rectangle){position.x + 44 + scroll.x, position.y + 250 + scroll.y, 100, 25}, TEXT_ALIGN_RIGHT, BLACK);
                    }
                }

                if (nullptr != level)
                {
                    char levelBuffer[3];
                    std::to_chars(&levelBuffer[0], &levelBuffer[2], *level);
                    raygui::GuiDrawText(levelBuffer, (Rectangle){position.x + 144 + scroll.x, position.y + 250 + scroll.y, 100, 25}, TEXT_ALIGN_RIGHT, BLACK);
                }

                if (nullptr != inventory)
                {
                    for (uint8_t r = 0; i < 7; i++) {
                        char resourceBuffer[7];
                        std::to_chars(&resourceBuffer[0], &resourceBuffer[6], inventory->resources[r]);
                        raygui::GuiDrawText(resourceBuffer, (Rectangle){position.x + 44 + scroll.x, position.y + 270 + r * 25 + scroll.y, 100, 25}, TEXT_ALIGN_RIGHT, map::resourceColors[r]);
                    }
                }
            }
        }
    }

    void GuiWindowFloatingTest(flecs::world ecs) {
        GuiWindowFloating(&window_position, &window_size, &minimized, &moving, &resizing, DrawContent, (Vector2){204, 300}, &scroll, "Menu", ecs);
    }
}
