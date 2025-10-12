# GOAT Makefile

# Compiler
CC = gcc

# Compilation flags
CFLAGS = -Wall -Wextra -Werror -std=c99 -Iinclude -Isrc

# Coverage flags
COVFLAGS = -fprofile-arcs -ftest-coverage -O0 -g

# Source directory
SRC_DIR = src

# Build directory
BUILD_DIR = build

# Final binary
TARGET = $(BUILD_DIR)/goat

# Source files
SRCS = $(SRC_DIR)/main.c \
       $(SRC_DIR)/utils/fs.c \
       $(SRC_DIR)/utils/log.c \
       $(SRC_DIR)/utils/repo.c \
       $(SRC_DIR)/core/command.c \
       $(SRC_DIR)/commands/init/init.c \
       $(SRC_DIR)/commands/init/init_handlers.c \
       $(SRC_DIR)/commands/add/add.c \
       $(SRC_DIR)/commands/add/add_internal.c

# Object files
OBJS = $(SRCS:.c=.o)

# Default rule
all: $(BUILD_DIR) $(TARGET)

# -------------------------------
# Unit tests
# -------------------------------
TEST_SRC = test/test_init.c
TEST_OBJ = $(BUILD_DIR)/test_init.o
TEST_BIN = $(BUILD_DIR)/test_init

TEST_OBJS = $(BUILD_DIR)/utils/fs.o \
            $(BUILD_DIR)/utils/log.o \
            $(BUILD_DIR)/utils/repo.o \
            $(BUILD_DIR)/core/command.o \
            $(BUILD_DIR)/commands/init/init.o \
            $(BUILD_DIR)/commands/init/init_handlers.o \
            $(BUILD_DIR)/commands/add/add.o \
            $(BUILD_DIR)/commands/add/add_handlers.o

tests: $(TEST_BIN)

$(TEST_BIN): $(TEST_OBJ) $(TEST_OBJS)
	$(CC) $(CFLAGS) $(COVFLAGS) $^ -o $@ -lcriterion

$(TEST_OBJ): $(TEST_SRC) $(BUILD_DIR)
	$(CC) $(CFLAGS) $(COVFLAGS) -Itest -c $< -o $@

# -------------------------------
# Coverage
# -------------------------------
coverage: tests
	./$(TEST_BIN)
	lcov --directory . --capture --output-file coverage.info --ignore-errors mismatch
	genhtml coverage.info --output-directory coverage
	@echo "Open coverage/index.html in your browser to see the report"

# -------------------------------
# Build binary
# -------------------------------
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile .c files to .o (gère les sous-dossiers)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile test .c files to .o
$(BUILD_DIR)/%.o: test/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# -------------------------------
# Build directory
# -------------------------------
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# -------------------------------
# Clean
# -------------------------------
clean:
	rm -f $(OBJS) $(TARGET) $(TEST_OBJ) $(TEST_BIN) *.gcda *.gcno *.info

distclean: clean
	rm -rf $(BUILD_DIR) coverage

re: distclean all

.PHONY: all clean distclean re tests coverage
