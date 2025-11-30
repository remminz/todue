CC           := gcc
CFLAGS 		  = -std=c11 -Wall -Wextra -MMD -MP -Iinclude
DEBUG_FLAGS   = -g -O0
RELEASE_FLAGS = -O2
LIB           = -lsqlite3

SRC_DIR := src
OBJ_DIR := build
BIN_DIR := bin
DB_DIR  := db

TARGET := $(BIN_DIR)/todue

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

all: debug

dirs:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR) $(DB_DIR)

debug: CFLAGS += $(DEBUG_FLAGS)
debug: dirs $(TARGET)
	@echo "Debug build complete: $(TARGET)"

release: CFLAGS += $(RELEASE_FLAGS)
release: dirs $(TARGET)
	@echo "Release build complete: $(TARGET)"

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LIB) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | dirs
	$(CC) $(CFLAGS) -c $< -o $@

-include $(DEPS)

run:
	@if [ -x "$(TARGET)" ]; then \
		"$(TARGET)"; \
	else \
		echo "$(TARGET) not found. Nothing to run."; \
	fi

clean:
	rm -rf $(OBJ_DIR)/* $(BIN_DIR)/*

reset:
	rm -rf $(OBJ_DIR) $(DB_DIR) $(BIN_DIR)

.PHONY: all dirs debug release run clean reset