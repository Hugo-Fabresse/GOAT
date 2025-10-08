# GOAT Makefile

# Compiler
CC = gcc

# Compilation flags
CFLAGS = -Wall -Wextra -Werror -std=c99 -Iinclude -Isrc

# Source directory
SRC_DIR = src

# Build directory
BUILD_DIR = build

# Final binary
TARGET = $(BUILD_DIR)/goat

# Source files
SRCS = $(SRC_DIR)/main.c \
       $(SRC_DIR)/utils/fs.c \
       $(SRC_DIR)/commands/init/init.c \
       $(SRC_DIR)/commands/init/init_handlers.c \
       $(SRC_DIR)/utils/log.c

# Object files
OBJS = $(SRCS:.c=.o)

# Default rule
all: $(BUILD_DIR) $(TARGET)

# Link object files into binary
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

# Compile .c files to .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Clean object files and binary
clean:
	rm -f $(OBJS) $(TARGET)

# Full clean
distclean: clean
	rm -rf $(BUILD_DIR)

# Rebuild everything
re: distclean all

# Phony targets
.PHONY: all clean distclean re
