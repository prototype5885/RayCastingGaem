FROM alpine:3.22

RUN apk add --no-cache \
    mingw-w64-gcc \
    cmake \
    make \
    git \
    && rm -rf /var/cache/apk/*

WORKDIR /app

COPY CMakeLists.txt .

CMD ["sh", "-c", "cmake -S . -B build -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ -DCMAKE_BUILD_TYPE=Release && cmake --build build --config Release && echo Build complete. Find output in /app/build"]