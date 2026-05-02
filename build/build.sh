#!/bin/bash

ROOT=".."
BUILD_FILE="$ROOT/build_log.txt"
VERSION=1

# Read sub version
if [ -f "$BUILD_FILE" ]; then
    SUB_VERSION=$(cat "$BUILD_FILE")
else
    SUB_VERSION=0
fi

# Create directories
mkdir -p "$ROOT/object"
mkdir -p "$ROOT/release"

# Compile
g++ -c "$ROOT/main.cpp" -I"$ROOT/include" -o "$ROOT/object/main.o"

# Link
OUTPUT="$ROOT/release/weChat_v${VERSION}.${SUB_VERSION}.exe"
g++ "$ROOT/object/main.o" -o "$OUTPUT"

echo "Build created: $OUTPUT"

# Increment build number
NEXT_VERSION=$((SUB_VERSION + 1))
echo $NEXT_VERSION > "$BUILD_FILE"