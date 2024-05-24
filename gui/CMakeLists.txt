cmake_minimum_required(VERSION 3.26)
project(Zappy_GUI CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(EXECUTABLE_NAME zappy_gui)

if (NOT DEFINED BUILD_PACKAGE)
    set(CMAKE_C_FLAGS "-Ofast -march=native -flto=auto" CACHE STRING "" FORCE)
endif ()

set(RAYLIB_VERSION 5.0)
find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
    include(FetchContent)
    FetchContent_Declare(
        raylib
        DOWNLOAD_EXTRACT_TIMESTAMP OFF
        URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    )
    FetchContent_GetProperties(raylib)
    if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
        set(FETCHCONTENT_QUIET NO)
        FetchContent_Populate(raylib)
        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
        add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
    endif ()
endif ()

# raylib-cpp
find_package(raylib_cpp QUIET)
if (NOT raylib_cpp_FOUND)
    if (NOT DEFINED RAYLIB_CPP_VERSION)
        set(RAYLIB_CPP_VERSION v5.0.1)
    endif ()
    include(FetchContent)
    FetchContent_Declare(
        raylib_cpp
        GIT_REPOSITORY https://github.com/RobLoach/raylib-cpp.git
        GIT_TAG ${RAYLIB_CPP_VERSION}
    )
    FetchContent_MakeAvailable(raylib_cpp)
endif ()


set(FLECS_VERSION v3.2.11)
find_package(flecs QUIET) # QUIET or REQUIRED
if (NOT flecs_FOUND) # If there's none, fetch and build flecs
    include(FetchContent)
    FetchContent_Declare(
        flecs
        DOWNLOAD_EXTRACT_TIMESTAMP OFF
        URL https://github.com/SanderMertens/flecs/archive/refs/tags/${FLECS_VERSION}.tar.gz
    )
    FetchContent_GetProperties(flecs)
    if (NOT flecs_POPULATED) # Have we downloaded flecs yet?
        set(FETCHCONTENT_QUIET NO)
        FetchContent_Populate(flecs)
        set(FLECS_SHARED OFF CACHE BOOL "" FORCE) # don't build flecs in shared
        add_subdirectory(${flecs_SOURCE_DIR} ${flecs_BINARY_DIR})
    endif ()
endif ()


add_executable(zappy_gui
    src/main.cpp
)

target_include_directories(zappy_gui PRIVATE
    include
)

target_link_libraries(zappy_gui PRIVATE
    build_configs
    raylib
    raylib_cpp
    flecs::flecs_static
)

set_target_properties(zappy_gui PROPERTIES
    OUTPUT_NAME ${EXECUTABLE_NAME}
)

add_custom_command(TARGET zappy_gui
    POST_BUILD COMMAND cp ${EXECUTABLE_NAME} ../
    POST_BUILD COMMAND cp ${EXECUTABLE_NAME} ../../
)

if (DEFINED BUILD_PACKAGE)
    target_compile_options(zappy_gui PRIVATE
        -static
        -march=x86-64
    )
    target_link_options(zappy_gui PRIVATE
        -static
    )
else ()
    target_compile_options(zappy_gui PRIVATE
        -march=native
    )
endif ()
