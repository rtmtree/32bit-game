#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DIST_DIR="$ROOT_DIR/dist"
EXE="$DIST_DIR/Win32Minimal.exe"

if [[ ! -f "$EXE" ]]; then
  echo "Build output not found: $EXE"
  echo "Run: bash scripts/build-win32.sh"
  exit 1
fi

if ! command -v wine >/dev/null 2>&1; then
  echo "Wine is not installed. Install with: brew install --cask wine-stable"
  exit 1
fi

echo "Running via Wine..."
wine "$EXE"


