APP           := todue

CC            := gcc
CXX           := g++

C_STD         := -std=c11
CXX_STD       := -std=c++11

SRC_DIR       := src
INC_DIR       := include
BUILD_DIR     := build
OBJ_DIR       := $(BUILD_DIR)/obj
TP_DIR        := third_party

CFLAGS        := -Wall -Wextra -I$(INC_DIR) -I$(TP_DIR) -MMD -MP
CXXFLAGS      := -Wall -Wextra -I$(INC_DIR) -I$(TP_DIR) -MMD -MP

DEBUG_FLAGS   := -g -O0 -DDEBUG
RELEASE_FLAGS := -O2

# -------------------------
# Platform detection
# -------------------------
ifeq ($(OS),Windows_NT)
    PLATFORM := windows
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Darwin)
        PLATFORM := macos
    else ifeq ($(UNAME_S),Linux)
        PLATFORM := linux
    else
$(warning Platform not officially supported)
        PLATFORM := other
    endif
endif

# -------------------------
# Sanitizer
# -------------------------
ifeq ($(PLATFORM),windows)
    ASAN_FLAGS :=
else
    ASAN_FLAGS := -fsanitize=address -fno-omit-frame-pointer
endif

# -------------------------
# Sources
# -------------------------
SRC_C := \
    $(wildcard $(SRC_DIR)/*.c) \
    $(TP_DIR)/sqlite/sqlite3.c

SRC_CPP := \
    $(TP_DIR)/linenoise/linenoise.cpp \
    $(TP_DIR)/linenoise/ConvertUTF.cpp \
    $(TP_DIR)/linenoise/wcwidth.cpp \
    $(SRC_DIR)/repl_input.cpp

OBJ := \
    $(SRC_C:%.c=$(OBJ_DIR)/%.o) \
    $(SRC_CPP:%.cpp=$(OBJ_DIR)/%.o)

DEPS := $(OBJ:.o=.d)

# -------------------------
# Targets
# -------------------------
.PHONY: all debug release run clean reset

all: debug

debug: CFLAGS += $(DEBUG_FLAGS) $(ASAN_FLAGS)
debug: CXXFLAGS += $(DEBUG_FLAGS) $(ASAN_FLAGS)
debug: LDFLAGS += $(ASAN_FLAGS)
debug: $(APP)

release: CFLAGS += $(RELEASE_FLAGS)
release: CXXFLAGS += $(RELEASE_FLAGS)
release: $(APP)

$(APP): $(OBJ)
	$(CXX) $(STD_VER) $(LDFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(C_STD) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXX_STD) $(CXXFLAGS) -c $< -o $@

run:
	@if [ -x "$(APP)" ]; then \
		"./$(APP)"; \
	else \
		echo "$(APP) not found."; \
	fi

clean:
	rm -rf $(OBJ_DIR)/$(SRC_DIR)

reset:
	rm -rf $(BUILD_DIR) $(APP)*

-include $(DEPS)