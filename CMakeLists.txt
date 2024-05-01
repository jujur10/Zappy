cmake_minimum_required(VERSION 3.26)
project(CMake_Template CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_C_FLAGS "-Wall -Wextra")

set(EXECUTABLE_NAME template)

add_library(build_configs INTERFACE)

target_compile_options(build_configs INTERFACE
    $<$<CONFIG:DEBUG>:-ggdb3>
    $<$<CONFIG:DEBUG>:-Og>
    $<$<CONFIG:RELEASE>:-Ofast>
    -march=native
    -flto=auto
)

target_compile_definitions(build_configs INTERFACE
    $<$<CONFIG:DEBUG>:DEBUG>
    $<$<CONFIG:RELEASE>:NDEBUG>
    BRACKET_O={
    BRACKET_C=}
    "SEMICOLON=\;"
)

target_link_options(build_configs INTERFACE
    -flto=auto
)


add_executable(template
    src/main.cpp
)

target_include_directories(template PRIVATE
    include
)

target_link_libraries(template PRIVATE
    build_configs
)

set_target_properties(template PROPERTIES
    OUTPUT_NAME ${EXECUTABLE_NAME}
)

add_custom_command(TARGET template
    POST_BUILD COMMAND cp ${EXECUTABLE_NAME} ../
)
