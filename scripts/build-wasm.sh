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

echo "Building minimal WebAssembly (single-file HTML) with Emscripten..."
emcc "$SRC_DIR/wasm_minimal.c" -Os \
  --pre-js "$SRC_DIR/pre.js" \
  -s MINIMAL_RUNTIME=1 -s SINGLE_FILE=1 -s NO_EXIT_RUNTIME=0 \
  -s ENVIRONMENT=web \
  -o "$DIST_DIR/minimal.html"

echo "Done: $DIST_DIR/minimal.html"


