##
## EPITECH PROJECT, 2024
## Makefile
## File description:
## Makefile
##

VERSION := 1.0.0

URL = "https://gitlab.gottagofast.fr/api/v4/projects/32/packages/generic/zappy"

GUI_NAME := zappy_gui
SERVER_NAME := zappy_server
AI_NAME := zappy_ai

CMAKE_GEN_FLAGS :=
CMAKE_BUILD_FLAGS :=

BUILD_DIR := build
DEBUG_DIR := debug

all: dl

dl_zappy_ai:
	@curl "$(URL)/1.0.0/zappy_ai" --output zappy_ai
	@chmod +x zappy_ai

dl_zappy_gui:
	@curl "$(URL)/1.0.0/zappy_gui" --output zappy_gui
	@chmod +x zappy_gui

dl_zappy_server:
	@curl "$(URL)/1.0.0/zappy_server" --output zappy_server
	@chmod +x zappy_server

dl:	dl_zappy_ai	dl_zappy_gui	dl_zappy_server


dl_zappy_ai_dev:
	@curl "$(URL)/0.1.0/zappy_ai" --output zappy_ai
	@chmod +x zappy_ai

dl_zappy_gui_dev:
	@curl "$(URL)/0.1.0/zappy_gui" --output zappy_gui
	@chmod +x zappy_gui

dl_zappy_server_dev:
	@curl "$(URL)/0.1.0/zappy_server" --output zappy_server
	@chmod +x zappy_server

dl-dev:	dl_zappy_ai_dev	dl_zappy_gui_dev	dl_zappy_server_dev

generate_build_files:
	@cmake -B $(BUILD_DIR) . $(CMAKE_GEN_FLAGS) -DCMAKE_BUILD_TYPE=Release

build_zappy_ai:	generate_build_files
	@cmake --build $(BUILD_DIR) --config Release $(CMAKE_BUILD_FLAGS) \
--target zappy_ai

build_zappy_gui:	generate_build_files
	@cmake --build $(BUILD_DIR) --config Release $(CMAKE_BUILD_FLAGS) \
--target zappy_gui

build_zappy_server:	generate_build_files
	@cmake --build $(BUILD_DIR) --config Release $(CMAKE_BUILD_FLAGS) \
--target zappy_server

build:	build_zappy_ai	build_zappy_gui	build_zappy_server

debug:
	@cmake -B $(DEBUG_DIR) . $(CMAKE_GEN_FLAGS) -DCMAKE_BUILD_TYPE=Debug
	@cmake --build $(DEBUG_DIR) --config Debug $(CMAKE_BUILD_FLAGS)
	@cp $(DEBUG_DIR)/zappy_* .

clean:
	@rm -rf *.o .idea/
	@find . -type f,d \( -name "*~" -o -name "\#*\#" \) -delete
	@find . -type f,d -name "vgcore*" -delete
	@find . -type f,d -name "*.gcno" -delete
	@find . -type f,d -name "*.gcda" -delete
	@find . -type f,d -name "*.gcov" -delete
	@rm -rf $(BUILD_DIR) $(DEBUG_DIR)

clean_binaries:
	@find . -name $(GUI_NAME) -delete
	@find . -name $(AI_NAME) -delete
	@find . -name $(SERVER_NAME) -delete
	@find . -name test_bin -delete

fclean: clean	clean_binaries

tests_run: test_server test_ai

test_ai:
	@cd ai/ && go test $(shell cd ai/ && find . -type d) -v -cover && cd ../

test_server:
	cmake -B build_test . $(CMAKE_GEN_FLAGS) -DCMAKE_BUILD_TYPE=Release \
	-DRUN_TESTS=1
	cmake --build build_test --config Release $(DCMAKE_BUILD_FLAGS) \
	--target test_zappy_server && ./test_zappy_server

.PHONY: all clean fclean re tests_run debug clean_binaries
.PHONY: build zappy_ai zappy_server zappy_gui
.PHONY: dl dl_zappy_ai dl_zappy_gui dl_zappy_server
.PHONY: dl-dev dl_zappy_ai_dev dl_zappy_gui_dev dl_zappy_server_dev
