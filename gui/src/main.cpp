#include <cstring>
#include <thread>

#include "Camera3D.hpp"
#include "Window.hpp"
#include "flecs.h"
#include "gui_to_server_cmd_structs.hpp"
#include "map.hpp"
#include "my_exit.hpp"
#include "my_write.hpp"
#include "networking.hpp"
#include "server_to_gui_cmd_structs.hpp"
#include "server_to_gui_cmd_value.hpp"
#include "sockets.hpp"
#include "string_utils.hpp"
#include "systems.hpp"

namespace zappy_gui
{

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
constexpr const char *const help = "USAGE: ./zappy_gui -p port -h machine\n";
constexpr int32_t screenWidth = 1'280;
constexpr int32_t screenHeight = 720;
constexpr uint32_t serverToGuiQueueCapacity = 4096;  // 4096 is the minimum size for the queues
constexpr uint32_t GuiToServerQueueCapacity = 4096;

namespace map
{
int32_t kMAP_WIDTH;
int32_t kMAP_HEIGHT;
}  // namespace map

net::STGQueue net::ServerToGuiQueue{serverToGuiQueueCapacity};
net::GTSQueue net::GuiToServerQueue{GuiToServerQueueCapacity};

/**
 * @brief Performs the handshake process with the server.
 *
 * This function handles the handshake process with the server. It exchanges
 * necessary information and validates the received data to establish a
 * connection and retrieve the map dimensions.
 *
 * The function follows these steps:
 * 1. Reads the welcome message from the server and verifies it.
 * 2. Sends the team name to the server.
 * 3. Receives the client number from the server and validates it.
 * 4. Receives the map dimensions (width and height) from the server and
 * validates them.
 * 5. Sets the `map::MAP_WIDTH` and `map::MAP_HEIGHT` variables with the
 * received map dimensions.
 *
 * If any errors occur during the handshake process, such as receiving invalid
 * data or failing to convert the data, the function writes an appropriate error
 * message to the error writer and exits the program with a status code of 1.
 *
 * @param serverSocket The socket used for communication with the server.
 */
void Handshake(const Socket &serverSocket)
{
    const FileWriter errWriter(2);
    std::vector<char> responseBuffer;
    std::string errorMsg;

    // Lambda to exit with an error message
    auto exitWithError = [&errWriter](const std::string &msg)
    {
        errWriter.writeNoReturn(msg.c_str(), msg.length());
        SystemExit::exit(1);
    };

    // Lambda to read a line from the server and check for errors
    auto readLineAndCheck = [&serverSocket, &responseBuffer, &errorMsg, &exitWithError]
    {
        std::string line = serverSocket.ReadLineTimeout(responseBuffer, 1'000000000, errorMsg);
        if (line.empty())
        {
            exitWithError(errorMsg);
        }
        return line;
    };

    std::string responseLine = readLineAndCheck();
    if (responseLine != "WELCOME" || !responseBuffer.empty())
    {
        exitWithError(
            "Failed handshake with server, did not receive welcome or received "
            "too much data");
    }

    if (serverSocket.Write(static_cast<const char *>("GRAPHIC\n"), 8) == -1)
    {
        exitWithError("Failed handshake with server at writing team name");
    }

    uint16_t clientNumber = 0;
    responseLine = readLineAndCheck();

    // Check if the response is a command or a client number
    const uint32_t command = *std::bit_cast<const uint32_t *>(responseLine.c_str()) & 0x00'FF'FF'FFu;
    if (command == static_cast<uint32_t>(net::ServerCommands::MAP_SIZE))
    {
        // Epitech test server doesn't follow the full AI protocol
        // They send no client number and no map size
        // Instead they send the commands: msz, sgt
        // After that they send bct * tile number every 200ms
        const std::string args = responseLine.substr(4);

        const size_t mapWidthIdx = args.find(' ');

        int32_t mapWidth = 0;
        int32_t mapHeight = 0;
        if (!string_utils::convertFromString(static_cast<std::string_view>(args).substr(0, mapWidthIdx), mapWidth) ||
            !string_utils::convertFromString(static_cast<std::string_view>(args).substr(mapWidthIdx + 1), mapHeight))
        {
            exitWithError("Failed to convert map dimensions from string");
        }
        if (mapWidth <= 0 || mapHeight <= 0)
        {
            exitWithError("Failed handshake with server, invalid map size");
        }

        map::kMAP_WIDTH = mapWidth;
        map::kMAP_HEIGHT = mapHeight;
        return;
    }
    if (!string_utils::convertFromString(responseLine, clientNumber))
    {
        exitWithError("Failed to convert client number from string");
    }
    if (0 == clientNumber)
    {
        exitWithError("Failed handshake with server, no slot left");
    }

    responseLine = readLineAndCheck();
    const size_t mapWidthIdx = responseLine.find(' ');
    if (mapWidthIdx == std::string::npos)
    {
        exitWithError(
            "Failed handshake with server, missing space between map_width and "
            "map_height");
    }

    int32_t mapWidth = 0;
    int32_t mapHeight = 0;
    if (!string_utils::convertFromString(static_cast<std::string_view>(responseLine).substr(0, mapWidthIdx), mapWidth) ||
        !string_utils::convertFromString(static_cast<std::string_view>(responseLine).substr(mapWidthIdx + 1), mapHeight))
    {
        exitWithError("Failed to convert map dimensions from string");
    }
    if (mapWidth <= 0 || mapHeight <= 0)
    {
        exitWithError("Failed handshake with server, invalid map size");
    }

    map::kMAP_WIDTH = mapWidth;
    map::kMAP_HEIGHT = mapHeight;
}

}  // namespace zappy_gui

//----------------------------------------------------------------------------------
// Main Enry Point
//----------------------------------------------------------------------------------
int32_t main(const int32_t argc, char *argv[])
{
    // Initialization
    //--------------------------------------------------------------------------------------
    if (2 == argc && 0 == std::strcmp(argv[1], "-help"))
    {
        const zappy_gui::FileWriter writer(2);
        writer.writeNoReturn(zappy_gui::help, sizeof(zappy_gui::help));
        return 0;
    }
    if (5 != argc)
    {
        zappy_gui::SystemExit::exit(1);
    }
    const zappy_gui::Socket serverSocket = zappy_gui::ConnectToServer(argv);

    Handshake(serverSocket);

    flecs::world ecs;
    ecs.set_entity_range(4'269'420, 0);  // Allow flecs to only genereate entity ids starting from 4'269'420

    ecs.import <flecs::monitor>();
    ecs.import <flecs::metrics>();
    ecs.import <flecs::alerts>();
    // Start REST API with default parameters
    ecs.set<flecs::Rest>({});

    ::SetConfigFlags(FLAG_MSAA_4X_HINT);
    raylib::Window window(zappy_gui::screenWidth, zappy_gui::screenHeight, "raylib-cpp - basic window");
    window.SetTargetFPS(60);

    raylib::Camera3D camera({10.0f, 10.0f, 10.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, 45.f, CAMERA_PERSPECTIVE);
    ecs.set<raylib::Camera3D>(camera);

    auto grassMod = raylib::Model("gui/resources/assets/grass2.glb");
    ecs.set<zappy_gui::map::tileModels>({&grassMod});

    auto food = raylib::Model("gui/resources/assets/food.glb");
    auto crystal = raylib::Model("gui/resources/assets/crystal1_optimized.glb");

    ecs.set<zappy_gui::map::resourceModels>({&food, &crystal});

    zappy_gui::systems::registerSystems(ecs);

    ecs.progress();  // Progress throught OnStart pipeline

    std::jthread networkThread(zappy_gui::net::NetworkTreadLoop, serverSocket);
    //--------------------------------------------------------------------------------------
    // Main game loop

    while (!window.ShouldClose() && ecs.progress())  // Detect window close button or ESC key
    {
    }

    networkThread.request_stop();

    // TODO: unload all shaders, do a function ?
    const auto *tileModels = ecs.get_mut<zappy_gui::map::tileModels>();
    ::UnloadShader(tileModels->grass->GetMaterials()[0].shader);

    const auto *ressourceModels = ecs.get_mut<zappy_gui::map::resourceModels>();
    ::UnloadShader(ressourceModels->crystal->GetMaterials()[0].shader);
}
