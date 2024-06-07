cmake_minimum_required(VERSION 3.26)
project(Zappy_Server C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED YES)
set(CMAKE_C_FLAGS "-Wall -Wextra")

set(EXECUTABLE_NAME zappy_server)

add_executable(zappy_server
    src/main.c
    src/server.c
    src/arguments.c
    src/socket/create_socket.c
    src/team.c
    src/map.c
    src/signal_handler.c
    src/clock.c
    src/server_events.c
    src/new_clients_handling.c
    src/utils/itoa/fast_itoa_u32.c
    src/utils/pre_generate/pre_generate_buffers.c
    src/queue/queue_utils_msg.c
    src/msg.c
    src/gui_handling.c
    src/swap_clients.c
    src/ai_handling.c
    src/utils/pre_generate/pre_generate_ressources.c
    src/utils/arrays/arrays_u32.c
    src/utils/arrays/arrays_u64.c
)

target_include_directories(zappy_server PRIVATE
    include
)

target_link_libraries(zappy_server PRIVATE
    build_configs
)

set_target_properties(zappy_server PROPERTIES
    OUTPUT_NAME ${EXECUTABLE_NAME}
)

add_custom_command(TARGET zappy_server
    POST_BUILD COMMAND cp ${EXECUTABLE_NAME} ../
    POST_BUILD COMMAND cp ${EXECUTABLE_NAME} ../../
)

if (DEFINED BUILD_PACKAGE)
    target_compile_options(zappy_server PRIVATE
            -static
            -march=x86-64
    )
    target_link_options(zappy_server PRIVATE
            -static
    )
else ()
    target_compile_options(zappy_server PRIVATE
            -march=native
    )
endif ()

if (DEFINED RUN_TESTS)
    add_executable(test_zappy_server
        tests/tests_parsing.c
        tests/tests_itoa.c
        tests/tests_queue.c
        src/arguments.c
        src/utils/itoa/fast_itoa_u32.c
        src/queue/queue_utils_msg.c
        src/msg.c
        tests/tests_spreading.c
        src/map.c
        src/utils/arrays/arrays_u64.c
        src/utils/arrays/arrays_u32.c
        src/utils/pre_generate/pre_generate_ressources.c
    )

    target_include_directories(test_zappy_server PRIVATE
        include
    )

    target_link_libraries(test_zappy_server PRIVATE
        criterion
        gcov
    )

    target_compile_options(test_zappy_server PRIVATE
        --coverage
    )

    target_compile_definitions(test_zappy_server PRIVATE
        PTR=*
        ARRAY=*
        "SEMICOLON=\;"
    )

    add_custom_command(TARGET test_zappy_server
        POST_BUILD COMMAND cp test_zappy_server ../
        POST_BUILD COMMAND cp test_zappy_server ../../
    )
endif ()
