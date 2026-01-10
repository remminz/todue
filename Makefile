APP           := todue
CC            := clang
STD_VER       := -std=c11

SRC_DIR       := src
INC_DIR       := include
BUILD_DIR     := build
OBJ_DIR       := $(BUILD_DIR)/obj
TP_DIR        := third_party

CFLAGS        := -Wall -Wextra
CFLAGS        += -I$(INC_DIR) -I$(TP_DIR)
CFLAGS        += -MMD -MP
LDFLAGS       := -fsanitize=address -fno-omit-frame-pointer
LIBS          := -lreadline
DEBUG_FLAGS   := -g -O0 -DDEBUG -fsanitize=address -fno-omit-frame-pointer
RELEASE_FLAGS := -O2

SRC           := $(wildcard $(SRC_DIR)/*.c)
SRC           += $(TP_DIR)/sqlite/sqlite3.c
OBJ           := $(SRC:%.c=$(OBJ_DIR)/%.o)
DEPS          := $(OBJ:.o=.d)

.PHONY: all debug release run clean reset

all: debug

debug: CFLAGS += $(DEBUG_FLAGS)
debug: $(APP)
	@echo "Debug build complete: $(APP)"

release: CFLAGS += $(RELEASE_FLAGS)
release: $(APP)
	@echo "Release build complete: $(APP)"

$(APP): $(OBJ)
	$(CC) $(STD_VER) $(LDFLAGS) $(LIBS) $^ -o $@

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(STD_VER) $(CFLAGS) -c $< -o $@

run:
	@if [ -x "$(APP)" ]; then \
		"./$(APP)"; \
	else \
		echo "$(APP) not found. Nothing to run."; \
	fi

clean:
	rm -rf $(OBJ_DIR)/$(SRC_DIR)

reset:
	rm -rf $(BUILD_DIR) $(APP)

-include $(DEPS)