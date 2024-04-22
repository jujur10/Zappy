##
## EPITECH PROJECT, 2024
## Makefile
## File description:
## Makefile
##

EXECUTABLE_NAME := template

CMAKE_GEN_FLAGS :=
CMAKE_BUILD_FLAGS :=

BUILD_DIR := build
DEBUG_DIR := debug

all: build

build:
	@cmake -B $(BUILD_DIR) . $(CMAKE_GEN_FLAGS) -DCMAKE_BUILD_TYPE=Release
	@cmake --build $(BUILD_DIR) --config Release $(CMAKE_BUILD_FLAGS)

debug:
	@cmake -B $(DEBUG_DIR) . $(CMAKE_GEN_FLAGS) -DCMAKE_BUILD_TYPE=Debug
	@cmake --build $(DEBUG_DIR) --config Debug $(CMAKE_BUILD_FLAGS)

clean:
	@rm -rf *.o .idea/
	@find . -type f,d \( -name "*~" -o -name "\#*\#" \) -delete
	@find . -type f,d -name "vgcore*" -delete
	@find . -type f,d -name "*.gcno" -delete
	@find . -type f,d -name "*.gcda" -delete
	@find . -type f,d -name "*.gcov" -delete
	@rm -rf $(BUILD_DIR) $(DEBUG_DIR)

fclean: clean
	@find . -name $(EXECUTABLE_NAME) -delete
	@find . -name test_bin -delete

tests_run: fclean
	@gcc -o test_bin $(TESTS_SRC) $(SRCS) --coverage -lcriterion $(CDEBUGFLAGS)
	@./test_bin

.PHONY: all clean fclean re tests_run debug
