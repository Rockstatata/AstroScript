# ============================================================================
# AstroScript Compiler Build System
# ----------------------------------------------------------------------------
# This Makefile formalizes the same pipeline implemented by:
# - scripts/build_compiler.sh
# - scripts/build_compiler.ps1
#
# Build stages:
# 1) Generate lexer with Flex
# 2) Generate parser with Bison
# 3) Compile compiler binary with g++ (C++17)
# 4) Sync platform binary into web/src/server/compiler
# ============================================================================

# Detect OS for platform-specific commands
ifeq ($(OS),Windows_NT)
    RM := del /F /Q
    CP := copy_binary.bat
    MKDIR := mkdir
    CHMOD := @rem
else
    RM := rm -f
    CP := cp
    MKDIR := mkdir -p
    CHMOD := chmod +x
endif

# Toolchain
FLEX ?= flex
BISON ?= bison
CXX ?= g++
OBJDUMP ?= objdump

# Some Windows GNU Make distributions set a cross-compiler as built-in CXX.
# Prefer local MinGW g++ unless the user explicitly overrides CXX.
ifeq ($(origin CXX), default)
	CXX := g++
endif

# Paths
COMPILER_DIR := backend/compiler
BUILD_DIR := $(COMPILER_DIR)/build
SERVER_COMPILER_DIR := web/src/server/compiler

# Grammar sources
LEXER_SRC := $(COMPILER_DIR)/lexer/lexer.l
PARSER_SRC := $(COMPILER_DIR)/parser/parser.y

# Generated files
LEXER_OUT := $(BUILD_DIR)/lex.yy.c
PARSER_OUT := $(BUILD_DIR)/parser.tab.c
PARSER_HDR := $(BUILD_DIR)/parser.tab.h

# Compiler sources
CPP_SRCS := \
	$(COMPILER_DIR)/semantic/symbol_table.cpp \
	$(COMPILER_DIR)/ir/tac.cpp \
	$(COMPILER_DIR)/main.cpp

# Binaries
LINUX_BINARY := $(BUILD_DIR)/astroscript
WINDOWS_BINARY := $(BUILD_DIR)/astroscript.exe
SYNCED_LINUX_BINARY := $(SERVER_COMPILER_DIR)/astroscript-linux
SYNCED_WINDOWS_BINARY := $(SERVER_COMPILER_DIR)/astroscript-windows.exe

CXX_STD := -std=c++17
INCLUDES := -I$(COMPILER_DIR) -I$(BUILD_DIR)

.NOTPARALLEL:

.PHONY: help all linux windows check-glibc sync-linux sync-windows clean clean-generated

help:
	@echo "AstroScript Makefile targets:"
	@echo "  make linux            Build Linux binary and sync to web server path"
	@echo "  make windows          Build Windows binary and sync to web server path"
	@echo "  make all              Build both Linux and Windows variants"
	@echo "  make clean            Remove generated lexer/parser and binaries"

all: linux windows

$(BUILD_DIR):
	@$(MKDIR) $(BUILD_DIR)

$(SERVER_COMPILER_DIR):
	@$(MKDIR) $(SERVER_COMPILER_DIR)

$(LEXER_OUT): $(LEXER_SRC) | $(BUILD_DIR)
	@echo "==> Running Flex (lexer)..."
	$(FLEX) -o$(LEXER_OUT) $(LEXER_SRC)

$(PARSER_OUT) $(PARSER_HDR): $(PARSER_SRC) | $(BUILD_DIR)
	@echo "==> Running Bison (parser)..."
	$(BISON) -d -o$(PARSER_OUT) $(PARSER_SRC)

$(LINUX_BINARY): $(LEXER_OUT) $(PARSER_OUT) $(PARSER_HDR) $(CPP_SRCS) | $(SERVER_COMPILER_DIR)
	@echo "==> Compiling AstroScript compiler (Linux target)..."
	$(CXX) $(LEXER_OUT) $(PARSER_OUT) $(CPP_SRCS) $(INCLUDES) $(CXX_STD) -o $(LINUX_BINARY)

check-glibc: $(LINUX_BINARY)
	@if command -v $(OBJDUMP) >/dev/null 2>&1; then \
		if $(OBJDUMP) -T $(LINUX_BINARY) | grep -qE '\(GLIBC_2\.(3[8-9]|[4-9][0-9])\)'; then \
			echo "ERROR: Linux binary requires GLIBC >= 2.38 and is not Vercel-compatible."; \
			echo "Rebuild on an older Linux baseline or update toolchain settings before deployment."; \
			exit 1; \
		fi; \
	fi

sync-linux: $(LINUX_BINARY) | $(SERVER_COMPILER_DIR)
	@$(CP) $(LINUX_BINARY) $(SYNCED_LINUX_BINARY)
	@$(CHMOD) $(SYNCED_LINUX_BINARY)

linux: $(LINUX_BINARY) check-glibc sync-linux
	@echo "==> Build complete: $(LINUX_BINARY)"
	@echo "==> Synced Linux binary to: $(SYNCED_LINUX_BINARY)"

$(WINDOWS_BINARY): $(LEXER_OUT) $(PARSER_OUT) $(PARSER_HDR) $(CPP_SRCS) | $(SERVER_COMPILER_DIR)
	@echo "==> Compiling AstroScript compiler (Windows target)..."
	$(CXX) $(LEXER_OUT) $(PARSER_OUT) $(CPP_SRCS) $(INCLUDES) $(CXX_STD) -o $(WINDOWS_BINARY)

sync-windows: $(WINDOWS_BINARY) | $(SERVER_COMPILER_DIR)
	@$(CP) $(WINDOWS_BINARY) $(SYNCED_WINDOWS_BINARY)

windows: $(WINDOWS_BINARY) sync-windows
	@echo "==> Build complete: $(WINDOWS_BINARY)"
	@echo "==> Synced Windows binary to: $(SYNCED_WINDOWS_BINARY)"

clean-generated:
	@$(RM) $(LEXER_OUT) $(PARSER_OUT) $(PARSER_HDR)

clean: clean-generated
	@$(RM) $(LINUX_BINARY) $(WINDOWS_BINARY) $(SYNCED_LINUX_BINARY) $(SYNCED_WINDOWS_BINARY)
	@echo "==> Cleaned generated files and binaries"
