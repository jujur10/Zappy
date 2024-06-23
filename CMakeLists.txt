cmake_minimum_required(VERSION 3.26)
project(Zappy CXX C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED YES)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

add_library(build_configs INTERFACE)

target_compile_options(build_configs INTERFACE
    $<$<CONFIG:DEBUG>:-ggdb3>
    $<$<CONFIG:RELEASE>:-Ofast>
    -Wall
    -Wextra
    -flto=auto
    $<$<CONFIG:DEBUG>:-ffat-lto-objects>
    -ffunction-sections
)

target_compile_definitions(build_configs INTERFACE
    $<$<CONFIG:DEBUG>:DEBUG>
    $<$<CONFIG:RELEASE>:NDEBUG>
    BRACKET_O={
    BRACKET_C=}
    "SEMICOLON=\;"
    PTR=*
    ARRAY=*
#    LOG=1
)

target_link_options(build_configs INTERFACE
    -flto=auto
    $<$<CONFIG:DEBUG>:-ffat-lto-objects>
    -Wl,--gc-sections
)

add_subdirectory(gui)
add_subdirectory(server)
add_subdirectory(ai)
