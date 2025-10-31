## 32-bit Windows Game (Build on macOS Apple Silicon)

**Goal**: Build a native 32-bit Windows `.exe` from macOS (Apple Silicon) without installing MinGW. We use `zig cc` as a cross-compiler targeting `i386-windows-gnu`.

### Prerequisites

- Install Zig (recommended via Homebrew):

```bash
brew install zig
```

- Optional (to run the EXE on macOS): install Wine

```bash
brew install --cask wine-stable
```

### Build

Use the provided script (no global toolchain setup beyond Zig required). It builds a single-file 32-bit EXE with static runtime linkage and size optimizations.

```bash
bash scripts/build-win32.sh
```

Artifacts are placed in `dist/Win32Game.exe`.

You can also invoke `zig cc` directly (same flags as the script):

```bash
zig cc -target x86-windows-gnu -Os -s \
  -ffunction-sections -fdata-sections \
  -fno-asynchronous-unwind-tables -fno-unwind-tables -fno-exceptions -fno-stack-protector \
  -DWIN32_LEAN_AND_MEAN \
  src/main.c \
  -o dist/Win32Game.exe \
  -Wl,--subsystem,windows -Wl,--gc-sections -Wl,--build-id=none \
  -static \
  -lkernel32 -luser32 -lgdi32
```

### Run (optional, via Wine on macOS)

```bash
bash scripts/run-on-mac.sh
```

### Minimal build (simplest possible EXE)

If your environment struggles with GUI loops or GDI, try the minimal target that only shows a `MessageBoxA` and exits:

```bash
bash scripts/build-win32.sh minimal
```

This produces `dist/Win32Minimal.exe` with only `kernel32` and `user32` dependencies.

### Project Layout

- `src/main.c` — Minimal Win32 game loop with double-buffered GDI rendering
- `scripts/build-win32.sh` — Cross-compile to 32-bit Windows `.exe` using Zig
  (static runtime linkage, size-optimized, single-file EXE)
- `scripts/run-on-mac.sh` — Run the produced `.exe` via Wine (optional)
- `dist/` — Build output directory (created on demand)

### Notes

- We target pure Win32 API (no external graphics libs). This avoids needing platform SDKs or SDL/raylib for cross-linking.
- The binary is PE32 and should run on 32-bit and 64-bit Windows.

## WebAssembly (browser) build

Windows EXEs will not run in WASM. To run in a browser, build the WASM target using Emscripten:

### Prerequisites

```bash
brew install emscripten
```

### Build minimal WASM (single-file HTML)

```bash
bash scripts/build-wasm.sh
```

Output: `dist/minimal.html` (contains embedded JS+WASM in a single file). Open it in a browser. If your browser blocks local file access, serve it:

```bash
cd dist && python3 -m http.server 8080
# then visit http://localhost:8080/minimal.html
```

Optional: build a standalone `.wasm` (for WASI runtimes) instead of HTML:

```bash
emcc src/wasm_minimal.c -Os -s STANDALONE_WASM=1 -o dist/minimal.wasm
```

### Web game port of main.c (arrow keys + canvas)

Build the interactive canvas version and serve it:

```bash
bash scripts/build-webgame.sh
cd dist && python3 -m http.server 8080
# then visit http://localhost:8080/webgame.html
```


