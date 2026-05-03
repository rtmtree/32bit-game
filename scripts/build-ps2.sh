#!/bin/bash
set -e

# PS2 Build Script - Local macOS Build
# Attempts to build PS2SDK locally on macOS

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
DIST_DIR="$PROJECT_ROOT/dist"
PS2DEV_DIR="$PROJECT_ROOT/ps2dev"

mkdir -p "$DIST_DIR"

echo "=== PS2 Local Build ==="
echo ""

# Check if PS2SDK is already available
if [ -d "$PS2DEV_DIR/ps2sdk" ]; then
    echo "✅ Found existing PS2SDK installation"
    source "$PS2DEV_DIR/ps2dev.sh"
    BUILD_PS2=1
else
    echo "PS2SDK not found. Attempting local installation..."
    echo ""
    echo "This will take 30+ minutes and requires significant disk space (~2GB)"
    echo ""
    read -p "Continue with local PS2SDK installation? (y/N): " -n 1 -r
    echo
    
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        BUILD_PS2SDK=1
    else
        echo "Installation cancelled."
        echo ""
        echo "Alternative options:"
        echo "1. Use GitHub Actions (recommended): https://github.com/rtmtree/32bit-game/actions"
        echo "2. Use Linux VM or remote server"
        echo "3. Manual setup instructions in README.md"
        exit 0
    fi
fi

# Install PS2SDK if needed
if [ "$BUILD_PS2SDK" = "1" ]; then
    echo "📦 Installing PS2SDK locally..."
    
    # Install required dependencies via Homebrew
    echo "📚 Installing dependencies..."
    brew install gmp mpfr texinfo autoconf automake libmpc || echo "Some dependencies may already be installed"
    
    # Clone PS2SDK
    echo "📥 Cloning PS2SDK..."
    if [ ! -d "$PS2DEV_DIR" ]; then
        mkdir -p "$PS2DEV_DIR"
        cd "$PS2DEV_DIR"
        git clone https://github.com/ps2dev/ps2toolchain.git .
    else
        cd "$PS2DEV_DIR"
    fi
    
    # Skip local PS2SDK build - it's not reliable on macOS Apple Silicon
    echo "❌ Skipping local PS2SDK build"
    echo ""
    echo "PS2SDK builds are not reliable on macOS Apple Silicon due to:"
    echo "- Library compatibility issues (GMP, MPFR, MPC)"
    echo "- Cross-compilation toolchain problems"
    echo "- Missing system dependencies"
    echo ""
    echo "🔄 Working alternatives:"
    echo ""
    echo "✅ Option 1: GitHub Actions (RECOMMENDED)"
    echo "   - Already set up and working"
    echo "   - Automatic builds on Linux runners"
    echo "   - Download artifacts from: https://github.com/rtmtree/32bit-game/actions"
    echo ""
    echo "🐧 Option 2: Linux VM or remote server"
    echo "   - Install Ubuntu/Debian"
    echo "   - Follow: https://github.com/ps2dev/ps2sdk"
    echo ""
    echo "💻 Option 3: Download pre-built PS2SDK"
    echo "   - From a Linux machine with working PS2SDK"
    echo "   - Copy to macOS and set paths manually"
    echo ""
    echo "The GitHub Actions workflow is the most reliable option"
    echo "and will automatically build your PS2 game when you push changes."
    exit 1
fi

# Build PS2 game if PS2SDK is available
if [ "$BUILD_PS2" = "1" ]; then
    echo ""
    echo "🎮 Building PS2 game..."
    
    cd "$PROJECT_ROOT"
    mkdir -p dist
    
    # Build the PS2 ELF
    mips64r5900el-ps2-elf-gcc -O2 -Wall \
        -I$PS2SDK/ee/include \
        -I$PS2SDK/common/include \
        -L$PS2SDK/ee/lib \
        src/ps2_main.c \
        -o dist/ps2_game.elf \
        -lgs -ldma -lpad -lc -lgcc
    
    if [ $? -eq 0 ]; then
        echo "✅ PS2 ELF built successfully"
        
        # Create binary
        mips64r5900el-ps2-elf-objcopy dist/ps2_game.elf -O binary dist/ps2_game.bin
        echo "✅ PS2 binary created"
        
        echo ""
        echo "📦 Build complete!"
        echo "Output files:"
        echo "- dist/ps2_game.elf ($(stat -f%z dist/ps2_game.elf) bytes)"
        echo "- dist/ps2_game.bin ($(stat -f%z dist/ps2_game.bin) bytes)"
        echo ""
        echo "To run on PS2:"
        echo "1. Copy ps2_game.elf to your PS2 via USB, network, or other method"
        echo "2. Use a homebrew loader like uLaunchELF to run the ELF"
        echo "3. Requires a modded PS2 or PS2 with FreeMCBoot installed"
    else
        echo "❌ PS2 build failed"
        exit 1
    fi
else
    echo "❌ PS2SDK not available for building"
    exit 1
fi

