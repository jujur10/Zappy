cmake_minimum_required(VERSION 3.26)
project(Zappy_GUI CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")

set(EXECUTABLE_NAME zappy_gui)

add_executable(zappy_gui
    src/main.cpp
)

target_include_directories(zappy_gui PRIVATE
    include
)

target_link_libraries(zappy_gui PRIVATE
    build_configs
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
