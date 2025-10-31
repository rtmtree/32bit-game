#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
SRC_DIR="$ROOT_DIR/src"
DIST_DIR="$ROOT_DIR/dist"

mkdir -p "$DIST_DIR"

MODE="${1:-game}"

if [[ "$MODE" == "minimal" ]]; then
  echo "Building minimal 32-bit Windows EXE with zig cc..."
  zig cc -target x86-windows-gnu -Os -s \
    -ffunction-sections -fdata-sections \
    -fno-asynchronous-unwind-tables -fno-unwind-tables -fno-exceptions -fno-stack-protector \
    -DWIN32_LEAN_AND_MEAN \
    "$SRC_DIR/minimal.c" \
    -o "$DIST_DIR/Win32Minimal.exe" \
    -Wl,--subsystem,windows -Wl,--gc-sections -Wl,--build-id=none \
    -static \
    -lkernel32 -luser32
  echo "Done: $DIST_DIR/Win32Minimal.exe"
else
  echo "Building lightweight 32-bit Windows EXE with zig cc..."
  zig cc -target x86-windows-gnu -Os -s \
    -ffunction-sections -fdata-sections \
    -fno-asynchronous-unwind-tables -fno-unwind-tables -fno-exceptions -fno-stack-protector \
    -DWIN32_LEAN_AND_MEAN \
    "$SRC_DIR/main.c" \
    -o "$DIST_DIR/Win32Game.exe" \
    -Wl,--subsystem,windows -Wl,--gc-sections -Wl,--build-id=none \
    -static \
    -lkernel32 -luser32 -lgdi32
  echo "Done: $DIST_DIR/Win32Game.exe"
fi

echo "Done: $DIST_DIR/Win32Game.exe"


