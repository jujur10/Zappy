cmake_minimum_required(VERSION 3.26)
project(Zappy_AI)

set(EXECUTABLE_NAME zappy_ai)

find_program(GO_COMPILER go)
if(NOT GO_COMPILER)
    message(FATAL_ERROR "Go compiler not found")
endif()

add_custom_target(zappy_ai ALL
    COMMAND ${GO_COMPILER} build -buildvcs=false
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_custom_command(TARGET zappy_ai
    POST_BUILD COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/${EXECUTABLE_NAME} ../
    POST_BUILD COMMAND mv ${CMAKE_CURRENT_SOURCE_DIR}/${EXECUTABLE_NAME} ../../
)

