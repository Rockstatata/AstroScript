#!/usr/bin/env bash
set -euo pipefail

# AstroScript Compiler Build Script
# Generates lexer/parser and compiles the compiler binary

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
COMPILER_DIR="$ROOT_DIR/backend/compiler"
BUILD_DIR="$COMPILER_DIR/build"
SERVER_COMPILER_DIR="$ROOT_DIR/web/src/server/compiler"

mkdir -p "$BUILD_DIR"
mkdir -p "$SERVER_COMPILER_DIR"

echo "==> Running Flex (lexer)..."
flex -o "$BUILD_DIR/lex.yy.c" "$COMPILER_DIR/lexer/lexer.l"

echo "==> Running Bison (parser)..."
bison -d -o "$BUILD_DIR/parser.tab.c" "$COMPILER_DIR/parser/parser.y"

echo "==> Compiling AstroScript compiler..."
g++ \
    "$BUILD_DIR/lex.yy.c" \
    "$BUILD_DIR/parser.tab.c" \
    "$COMPILER_DIR/semantic/symbol_table.cpp" \
    "$COMPILER_DIR/ir/tac.cpp" \
    "$COMPILER_DIR/main.cpp" \
    -I"$COMPILER_DIR" \
    -I"$BUILD_DIR" \
    -std=c++17 \
    -o "$BUILD_DIR/astroscript"

if command -v objdump >/dev/null 2>&1; then
    if objdump -T "$BUILD_DIR/astroscript" | grep -qE '\(GLIBC_2\.(3[8-9]|[4-9][0-9])\)'; then
        echo "ERROR: Linux binary requires GLIBC >= 2.38 and is not Vercel-compatible."
        echo "Rebuild on an older Linux baseline or update toolchain settings before deployment."
        exit 1
    fi
fi

cp "$BUILD_DIR/astroscript" "$SERVER_COMPILER_DIR/astroscript-linux"
chmod +x "$SERVER_COMPILER_DIR/astroscript-linux"

echo "==> Build complete: $BUILD_DIR/astroscript"
echo "==> Synced Linux binary to: $SERVER_COMPILER_DIR/astroscript-linux"
