APP          := todue

CC           := gcc
CXX          := g++

SRCDIR       := src
INCDIR       := include
BUILDDIR     := build
OBJDIR       := $(BUILDDIR)/obj
TPDIR        := third_party

CFLAGS       := -std=c11 -Wall -Wextra -I$(INCDIR) -I$(TPDIR) -MMD -MP
CXXFLAGS     := -std=c++11 -Wall -Wextra -I$(INCDIR) -I$(TPDIR) -MMD -MP

DEBUGFLAGS   := -g -O0 -DDEBUG
RELEASEFLAGS := -O2

# -------------------------
# Platform detection
# -------------------------
UNAME := $(shell uname -s)

ifneq (,$(filter MINGW% MSYS% CYGWIN%,$(UNAME)))
    PLATFORM := windows
	PREFIX   ?= $(LOCALAPPDATA)/Programs
    BIN      := $(APP).exe
    BINDIR   ?= $(PREFIX)/$(APP)
else
    BIN := $(APP)
	ifeq ($(UNAME),Darwin)
		PLATFORM := darwin
		PREFIX   ?= /usr/local
	else ifeq ($(UNAME),Linux)
		PLATFORM := linux
		PREFIX   ?= $(HOME)/.local
	else
$(warning Platform not recognized or officially supported)
		PLATFORM := unknown
		PREFIX   ?= .
	endif
	BINDIR  ?= $(PREFIX)/bin
endif

# -------------------------
# Sanitizer
# -------------------------
ifeq ($(PLATFORM),windows)
    ASANFLAGS :=
else
    ASANFLAGS := -fsanitize=address -fno-omit-frame-pointer
endif

# -------------------------
# Sources
# -------------------------
SRC_C := \
    $(wildcard $(SRCDIR)/*.c) \
    $(TPDIR)/sqlite/sqlite3.c

SRC_CPP := \
    $(TPDIR)/linenoise/linenoise.cpp \
    $(TPDIR)/linenoise/ConvertUTF.cpp \
    $(TPDIR)/linenoise/wcwidth.cpp \
    $(SRCDIR)/repl_input.cpp

OBJ := \
    $(SRC_C:%.c=$(OBJDIR)/%.o) \
    $(SRC_CPP:%.cpp=$(OBJDIR)/%.o)

DEPS := $(OBJ:.o=.d)

# -------------------------
# Targets
# -------------------------
.PHONY: all debug release clean reset install uninstall run

all: debug

debug: CFLAGS   += $(DEBUGFLAGS) $(ASANFLAGS)
debug: CXXFLAGS += $(DEBUGFLAGS) $(ASANFLAGS)
debug: LDFLAGS  += $(ASANFLAGS)
debug: $(BIN)
	@echo "Debug build complete: $(BIN)"

release: CFLAGS   += $(RELEASEFLAGS)
release: CXXFLAGS += $(RELEASEFLAGS)
release: $(BIN)
	@echo "Release build complete: $(BIN)"

$(BIN): $(OBJ)
	$(CXX) $(LDFLAGS) $^ -o $@

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR)/$(SRCDIR)

reset:
	rm -rf $(BUILDDIR) $(BIN)

install:
	install -d $(DESTDIR)$(BINDIR)
	install -m 755 $(BIN) $(DESTDIR)$(BINDIR)/$(BIN)

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/$(BIN)

run:
	@if [ -x "$(BIN)" ]; then \
		"./$(BIN)"; \
	else \
		echo "$(BIN) not found."; \
	fi

-include $(DEPS)