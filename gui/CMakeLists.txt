cmake_minimum_required(VERSION 3.26)
project(Zappy_GUI CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(EXECUTABLE_NAME zappy_gui)

if (NOT DEFINED BUILD_PACKAGE)
    set(CMAKE_C_FLAGS "-Ofast -march=native -flto=auto" CACHE STRING "" FORCE)
endif ()



#######################
###### Add Flecs ######
#######################
add_library(flecs STATIC
    resources/flecs/flecs.c
)
target_include_directories(flecs INTERFACE
    resources/flecs
)
set_target_properties(flecs PROPERTIES C_STANDARD 99)



#######################
###### Add Raylib #####
#######################
set(BUILD_EXAMPLES OFF)
set(PLATFORM Desktop)
set(OPENGL_VERSION 3.3)
add_subdirectory(resources/raylib/ resources/raylib/src)


#######################
#### Add Raylib-cpp ###
#######################
add_subdirectory(resources/raylib_cpp/include)



#######################
###### Add Queue ######
#######################
add_library(atomic_queue INTERFACE)
target_include_directories(atomic_queue INTERFACE
    resources/atomic_queue
)

#######################
###### Zappy GUI ######
#######################
add_executable(zappy_gui
    src/sockets.cpp
    src/main.cpp
    src/argv_parser.cpp
    src/systems.cpp
    src/map.cpp
    src/raylib_utils.cpp
    src/network_main_loop.cpp
    include/commands.hpp
)

target_include_directories(zappy_gui PRIVATE
    include
)

target_link_libraries(zappy_gui PRIVATE
    build_configs
    raylib
    raylib_cpp
    flecs
    atomic_queue
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
        -march=x86-64
    )
else ()
    target_compile_options(zappy_gui PRIVATE
        -march=native
    )
endif ()

if (DEFINED RUN_TESTS)
    add_executable(test_zappy_gui
        tests/tests_socket.cpp
        src/sockets.cpp
    )

    target_include_directories(test_zappy_gui PRIVATE
        include
    )

    target_link_libraries(test_zappy_gui PRIVATE
        criterion
        gcov
        raylib
        raylib_cpp
        flecs
        atomic_queue
    )

    target_compile_options(test_zappy_gui PRIVATE
        --coverage
    )

    add_custom_command(TARGET test_zappy_gui
        POST_BUILD COMMAND cp test_zappy_gui ../
        POST_BUILD COMMAND cp test_zappy_gui ../../
    )
endif ()
