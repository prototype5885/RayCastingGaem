#!/bin/bash

set -e

OUTPUT_NAME="emscripten"
BUILD_DIR="build_emscripten"

ERROR_TEXT="Press any key to exit..."

if [ -f "emscripten/index.html" ]; then
    echo "Copying index.html to ${BUILD_DIR}/..."
    mkdir -p "${BUILD_DIR}"
    cp "emscripten/index.html" "${BUILD_DIR}/"
else
    echo "ERROR: index.html was not found in the emscripten folder"
    read -p ${ERROR_TEXT}
    exit 1
fi

FILES=""
for f in "src"/*.cpp; do
    if [ -f "$f" ]; then
        FILES+=" /app/src/$(basename "$f")"
    fi
done

EMCC_COMMAND="em++ -O3 ${FILES} -o ${BUILD_DIR}/${OUTPUT_NAME}.js -s USE_SDL=2 --preload-file textures"

echo "Running Emscripten compilation using Docker..."
docker run --rm -v "$(pwd):/app" -w /app emscripten/emsdk bash -c "${EMCC_COMMAND}"

if [ $? -eq 0 ]; then
    echo "Build successful into folder: $BUILD_DIR"
else
    echo "Build failed."
fi
