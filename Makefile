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
       $(SRC_DIR)/core/goat.c \
       $(SRC_DIR)/core/hash.c \
       $(SRC_DIR)/core/index.c \
       $(SRC_DIR)/core/blob.c \
       $(SRC_DIR)/utils/fs.c \
       $(SRC_DIR)/utils/log.c \
       $(SRC_DIR)/utils/repo.c \
       $(SRC_DIR)/utils/path.c \
       $(SRC_DIR)/utils/std_ext.c \
       $(SRC_DIR)/utils/timestamp.c \
       $(SRC_DIR)/core/command.c \
       $(SRC_DIR)/commands/init/init.c \
       $(SRC_DIR)/commands/init/init_handlers.c \
       $(SRC_DIR)/commands/add/add.c \
       $(SRC_DIR)/commands/add/add_all.c \
       $(SRC_DIR)/commands/add/add_update.c \
       $(SRC_DIR)/commands/add/add_internal.c \
       $(SRC_DIR)/commands/add/add_handlers.c \
       $(SRC_DIR)/commands/commit/commit.c \
       $(SRC_DIR)/commands/commit/commit_internal.c \
       $(SRC_DIR)/commands/commit/commit_handlers.c \
       $(SRC_DIR)/commands/status/status.c \
       $(SRC_DIR)/commands/status/status_internal.c

# Object files
OBJS = $(SRCS:.c=.o)

# Default rule
all: $(BUILD_DIR) $(TARGET)

# -------------------------------
# Unit tests
# -------------------------------
TEST_SRCS = test/test_init.c test/test_add.c
TEST_OBJS_TARGETS = $(BUILD_DIR)/test_init.o $(BUILD_DIR)/test_add.o
TEST_BINS = $(BUILD_DIR)/test_init $(BUILD_DIR)/test_add

TEST_OBJS = $(BUILD_DIR)/utils/fs.o \
            $(BUILD_DIR)/utils/log.o \
            $(BUILD_DIR)/utils/repo.o \
            $(BUILD_DIR)/utils/timestamp.o \
            $(BUILD_DIR)/utils/path.o \
            $(BUILD_DIR)/utils/std_ext.o \
            $(BUILD_DIR)/core/command.o \
            $(BUILD_DIR)/core/hash.o \
            $(BUILD_DIR)/core/index.o \
            $(BUILD_DIR)/core/blob.o \
            $(BUILD_DIR)/commands/init/init.o \
            $(BUILD_DIR)/commands/init/init_handlers.o \
            $(BUILD_DIR)/commands/add/add.o \
            $(BUILD_DIR)/commands/add/add_all.o \
            $(BUILD_DIR)/commands/add/add_update.o \
            $(BUILD_DIR)/commands/add/add_internal.o \
            $(BUILD_DIR)/commands/add/add_handlers.o \
            $(BUILD_DIR)/commands/commit/commit.o \
            $(BUILD_DIR)/commands/commit/commit_internal.o \
            $(BUILD_DIR)/commands/commit/commit_handlers.o \
            $(BUILD_DIR)/commands/status/status.o \
            $(BUILD_DIR)/commands/status/status_internal.o

# Test-specific objects with coverage flags
TEST_COV_OBJS = $(BUILD_DIR)/test_utils/fs.o \
                $(BUILD_DIR)/test_utils/log.o \
                $(BUILD_DIR)/test_utils/repo.o \
                $(BUILD_DIR)/test_utils/timestamp.o \
                $(BUILD_DIR)/test_utils/path.o \
                $(BUILD_DIR)/test_utils/std_ext.o \
                $(BUILD_DIR)/test_core/command.o \
                $(BUILD_DIR)/test_core/hash.o \
                $(BUILD_DIR)/test_core/index.o \
                $(BUILD_DIR)/test_core/blob.o \
                $(BUILD_DIR)/test_commands/init/init.o \
                $(BUILD_DIR)/test_commands/init/init_handlers.o \
                $(BUILD_DIR)/test_commands/add/add.o \
                $(BUILD_DIR)/test_commands/add/add_all.o \
                $(BUILD_DIR)/test_commands/add/add_update.o \
                $(BUILD_DIR)/test_commands/add/add_internal.o \
                $(BUILD_DIR)/test_commands/add/add_handlers.o \
                $(BUILD_DIR)/test_commands/commit/commit.o \
                $(BUILD_DIR)/test_commands/commit/commit_internal.o \
                $(BUILD_DIR)/test_commands/commit/commit_handlers.o \
                $(BUILD_DIR)/test_commands/status/status.o \
		$(BUILD_DIR)/test_commands/status/status_internal.o

tests: $(TEST_BINS)

$(BUILD_DIR)/test_init: $(BUILD_DIR)/test_init.o $(TEST_COV_OBJS)
	$(CC) $(CFLAGS) $(COVFLAGS) $^ -o $@ -lcriterion -lssl -lcrypto

$(BUILD_DIR)/test_add: $(BUILD_DIR)/test_add.o $(TEST_COV_OBJS)
	$(CC) $(CFLAGS) $(COVFLAGS) $^ -o $@ -lcriterion -lssl -lcrypto

$(BUILD_DIR)/test_init.o: test/test_init.c $(BUILD_DIR)
	$(CC) $(CFLAGS) $(COVFLAGS) -Itest -c $< -o $@

$(BUILD_DIR)/test_add.o: test/test_add.c $(BUILD_DIR)
	$(CC) $(CFLAGS) $(COVFLAGS) -Itest -c $< -o $@

# -------------------------------
# Coverage
# -------------------------------
coverage: tests
	-./$(BUILD_DIR)/test_init
	-./$(BUILD_DIR)/test_add
	lcov --directory . --capture --output-file coverage.info --ignore-errors mismatch
	lcov --remove coverage.info '*/test/*' --output-file coverage.info
	lcov --extract coverage.info '*/src/*' --output-file coverage.info
	genhtml coverage.info --output-directory coverage
	@echo "Open coverage/index.html in your browser to see the report"

# -------------------------------
# Build binary
# -------------------------------
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lssl -lcrypto

# Compile .c files to .o (gère les sous-dossiers)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile test .c files to .o
$(BUILD_DIR)/%.o: test/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile source files with coverage flags for testing
$(BUILD_DIR)/test_%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(COVFLAGS) -c $< -o $@

# -------------------------------
# Build directory
# -------------------------------
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# -------------------------------
# Clean
# -------------------------------
clean:
	rm -f $(OBJS) $(TARGET) $(TEST_OBJS_TARGETS) $(TEST_BINS) *.gcda *.gcno *.info

distclean: clean
	rm -rf $(BUILD_DIR) coverage

re: distclean all

.PHONY: all clean distclean re tests coverage
