cmake_minimum_required(VERSION 3.26)
project(Zappy CXX C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED YES)
set(CMAKE_C_FLAGS "-Wall -Wextra")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")

add_library(build_configs INTERFACE)

target_compile_options(build_configs INTERFACE
    $<$<CONFIG:DEBUG>:-ggdb3>
    $<$<CONFIG:DEBUG>:-Og>
    $<$<CONFIG:RELEASE>:-Ofast>
    -flto=auto
)

target_compile_definitions(build_configs INTERFACE
    $<$<CONFIG:DEBUG>:DEBUG>
    $<$<CONFIG:RELEASE>:NDEBUG>
    BRACKET_O={
    BRACKET_C=}
    "SEMICOLON=\;"
    PTR=*
    ARRAY=*
)

target_link_options(build_configs INTERFACE
    -flto=auto
)

add_subdirectory(gui)
add_subdirectory(server)
add_subdirectory(ai)
