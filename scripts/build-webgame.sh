#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
SRC_DIR="$ROOT_DIR/src"
DIST_DIR="$ROOT_DIR/dist"

mkdir -p "$DIST_DIR"

if ! command -v emcc >/dev/null 2>&1; then
  echo "Emscripten (emcc) not found. Install via: brew install emscripten"
  exit 1
fi

echo "Building web game (single-file HTML) with Emscripten..."
emcc "$SRC_DIR/web_main.c" -Os \
  --pre-js "$SRC_DIR/pre.js" \
  -s MINIMAL_RUNTIME=1 -s SINGLE_FILE=1 -s NO_EXIT_RUNTIME=1 \
  -s ENVIRONMENT=web \
  -o "$DIST_DIR/webgame.html"

echo "Done: $DIST_DIR/webgame.html"


