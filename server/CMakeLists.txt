cmake_minimum_required(VERSION 3.26)
project(Zappy_Server C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED YES)
set(CMAKE_C_FLAGS "-Wall -Wextra")

set(EXECUTABLE_NAME zappy_server)

add_executable(zappy_server
    src/ai_handling.c
    src/arguments.c
    src/clock.c
    src/clock_timeout.c
    src/commands/command_utils.c
    src/commands/gui/gui_bct_command.c
    src/commands/gui/gui_command.c
    src/commands/gui/gui_command_parsing.c
    src/commands/gui/gui_mct_command.c
    src/commands/gui/gui_msz_command.c
    src/commands/gui/gui_none_command.c
    src/commands/gui/gui_pin_command.c
    src/commands/gui/gui_plv_command.c
    src/commands/gui/gui_ppo_command.c
    src/commands/gui/gui_responses.c
    src/commands/gui/gui_sgt_command.c
    src/commands/gui/gui_sst_command.c
    src/commands/gui/gui_tna_command.c
    src/commands/player/player_broadcast_command.c
    src/commands/player/player_command.c
    src/commands/player/player_command_parsing.c
    src/commands/player/player_connect_nbr_command.c
    src/commands/player/player_direction_command.c
    src/commands/player/player_eject_command.c
    src/commands/player/player_fork_command.c
    src/commands/player/player_forward_command.c
    src/commands/player/player_frequency_command.c
    src/commands/player/player_incantation_command.c
    src/commands/player/player_inventory_command.c
    src/commands/player/player_look_command.c
    src/commands/player/player_none_command.c
    src/commands/player/player_responses.c
    src/commands/player/player_right_left_command.c
    src/commands/player/player_set_command.c
    src/commands/player/player_take_command.c
    src/coordinates.c
    src/eggs.c
    src/end_of_the_game.c
    src/events/gui/gui_enw_event.c
    src/events/gui/gui_events.c
    src/events/gui/gui_none_event.c
    src/events/gui/gui_pbc_event.c
    src/events/gui/gui_pdi_event.c
    src/events/gui/gui_pdr_event.c
    src/events/gui/gui_pex_event.c
    src/events/gui/gui_pfk_event.c
    src/events/gui/gui_pgt_event.c
    src/events/gui/gui_pic_event.c
    src/events/gui/gui_pnw_event.c
    src/events/gui/gui_seg_event.c
    src/events/gui/gui_update_mct_event.c
    src/events/new_client/new_client_event.c
    src/events/new_client/new_client_new_slot_available_event.c
    src/events/new_client/new_client_none_event.c
    src/events/player/player_death_event.c
    src/events/player/player_end_of_game_event.c
    src/events/player/player_events.c
    src/events/player/player_events.c
    src/events/player/player_incantation_event.c
    src/events/player/player_none_event.c
    src/gui.c
    src/gui_handling.c
    src/main.c
    src/map.c
    src/map_buffer.c
    src/msg.c
    src/new_clients_handling.c
    src/player.c
    src/resource.c
    src/server.c
    src/server_events.c
    src/server_update.c
    src/signal_handler.c
    src/socket/create_socket.c
    src/swap_clients.c
    src/team.c
    src/team_management.c
    src/utils/arrays/arrays_u32.c
    src/utils/arrays/arrays_u64.c
    src/utils/arrays/arrays_virtual.c
    src/utils/itoa/fast_itoa_u32.c
    src/utils/pre_generate/gui/gui_pre_generate_mct.c
    src/utils/pre_generate/gui/gui_pre_generate_tna.c
    src/utils/pre_generate/player/pre_generate_look_indexes.c
    src/utils/pre_generate/pre_generate_buffers.c
    src/utils/pre_generate/pre_generate_resources.c
    src/utils/queue/queue.c
    src/utils/queue/queue_buffer.c
    src/utils/string/buffer.c
    src/utils/string/string.c
)

target_include_directories(zappy_server PRIVATE
    include
    tests/include
)

target_link_libraries(zappy_server PRIVATE
    build_configs
    m
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
        src/ai_handling.c
        src/arguments.c
        src/clock.c
        src/clock_timeout.c
        src/commands/command_utils.c
        src/commands/gui/gui_bct_command.c
        src/commands/gui/gui_command.c
        src/commands/gui/gui_command_parsing.c
        src/commands/gui/gui_msz_command.c
        src/commands/gui/gui_none_command.c
        src/commands/gui/gui_pin_command.c
        src/commands/gui/gui_plv_command.c
        src/commands/gui/gui_ppo_command.c
        src/commands/gui/gui_responses.c
        src/commands/gui/gui_sgt_command.c
        src/commands/gui/gui_sst_command.c
        src/commands/player/player_broadcast_command.c
        src/commands/player/player_broadcast_command.c
        src/commands/player/player_command.c
        src/commands/player/player_command.c
        src/commands/player/player_command_parsing.c
        src/commands/player/player_command_parsing.c
        src/commands/player/player_connect_nbr_command.c
        src/commands/player/player_direction_command.c
        src/commands/player/player_direction_command.c
        src/commands/player/player_eject_command.c
        src/commands/player/player_fork_command.c
        src/commands/player/player_forward_command.c
        src/commands/player/player_frequency_command.c
        src/commands/player/player_frequency_command.c
        src/commands/player/player_incantation_command.c
        src/commands/player/player_incantation_command.c
        src/commands/player/player_inventory_command.c
        src/commands/player/player_inventory_command.c
        src/commands/player/player_look_command.c
        src/commands/player/player_look_command.c
        src/commands/player/player_none_command.c
        src/commands/player/player_responses.c
        src/commands/player/player_responses.c
        src/commands/player/player_right_left_command.c
        src/commands/player/player_set_command.c
        src/commands/player/player_take_command.c
        src/coordinates.c
        src/eggs.c
        src/events/gui/gui_enw_event.c
        src/events/gui/gui_events.c
        src/events/gui/gui_none_event.c
        src/events/gui/gui_pbc_event.c
        src/events/gui/gui_pdi_event.c
        src/events/gui/gui_pdr_event.c
        src/events/gui/gui_pex_event.c
        src/events/gui/gui_pfk_event.c
        src/events/gui/gui_pgt_event.c
        src/events/gui/gui_pic_event.c
        src/events/gui/gui_pnw_event.c
        src/events/gui/gui_seg_event.c
        src/events/gui/gui_update_mct_event.c
        src/events/new_client/new_client_event.c
        src/events/new_client/new_client_new_slot_available_event.c
        src/events/new_client/new_client_none_event.c
        src/events/player/player_death_event.c
        src/events/player/player_end_of_game_event.c
        src/events/player/player_events.c
        src/events/player/player_events.c
        src/events/player/player_incantation_event.c
        src/events/player/player_none_event.c
        src/gui.c
        src/gui_handling.c
        src/map.c
        src/map_buffer.c
        src/msg.c
        src/new_clients_handling.c
        src/player.c
        src/player.c
        src/resource.c
        src/swap_clients.c
        src/team.c
        src/team_management.c
        src/utils/arrays/arrays_u32.c
        src/utils/arrays/arrays_u64.c
        src/utils/arrays/arrays_virtual.c
        src/utils/itoa/fast_itoa_u32.c
        src/utils/pre_generate/gui/gui_pre_generate_mct.c
        src/utils/pre_generate/gui/gui_pre_generate_tna.c
        src/utils/pre_generate/player/pre_generate_look_indexes.c
        src/utils/pre_generate/pre_generate_buffers.c
        src/utils/pre_generate/pre_generate_resources.c
        src/utils/queue/queue.c
        src/utils/queue/queue_buffer.c
        src/utils/string/buffer.c
        src/utils/string/string.c
        tests/commands/gui/tests_gui_bct_command.c
        tests/commands/gui/tests_gui_msz_command.c
        tests/commands/player/tests_player_broadcast_command.c
        tests/commands/player/tests_player_direction_command.c
        tests/commands/player/tests_player_frequency_command.c
        tests/commands/player/tests_player_inventory_command.c
        tests/commands/player/tests_player_look_command.c
        tests/tests_clock.c
        tests/tests_gui_command_handling.c
        tests/tests_itoa.c
        tests/tests_parsing.c
        tests/tests_player_command_handling.c
        tests/tests_queue.c
        tests/tests_spreading.c
        tests/tests_string.c
    )

    target_include_directories(test_zappy_server PRIVATE
        include
        tests/include
    )

    target_link_libraries(test_zappy_server PRIVATE
        criterion
        gcov
        m
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
