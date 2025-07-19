import os
import shutil
import subprocess
import textwrap
import http.server
import socketserver
import webbrowser
import threading

# user options
ALPINE_VERSION = "3.22"
EMSCRIPTEN_VERSION = "4.0.10"

SDL2_VERSION = "2.32.8"
ASSETS_FOLDER = "assets"
# user options end

# compiler options
DEFAULT_COMPILE_ARGS = "-Wall -Wextra -s -O3 -Iinclude"
DEFAULT_LINKING_ARGS = "-lSDL2main -lSDL2 -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lpthread -flto"

WINDOWS_COMPILE_ARGS = f"-mwindows {DEFAULT_COMPILE_ARGS}"
WINDOWS_LINKING_ARGS = f"-lmingw32 {DEFAULT_LINKING_ARGS}"

LINUX_COMPILE_ARGS = f"{DEFAULT_COMPILE_ARGS}"
LINUX_LINKING_ARGS = f"{DEFAULT_LINKING_ARGS}"

EMSCRIPTEN_COMPILE_ARGS = f"{DEFAULT_COMPILE_ARGS} -sNO_DISABLE_EXCEPTION_CATCHING"
EMSCRIPTEN_LINKING_ARGS = ""
# compiler options end

DEFAULT_TEXT = "Type a number (Default = 1)"


def error(error):
    print()
    print(f"ERROR: {error}")
    print()
    input("Press any key to exit...")
    exit(1)


def make_docker_file(IMAGE, COMMAND, SDL2_VERSION, OS):
    if OS == "1":
        return textwrap.dedent(f"""\
            FROM {IMAGE}

            RUN {COMMAND}
            RUN wget -q https://github.com/libsdl-org/SDL/releases/download/release-{SDL2_VERSION}/SDL2-devel-{SDL2_VERSION}-mingw.zip 
            RUN unzip SDL2-devel-{SDL2_VERSION}-mingw.zip
            RUN cp -r SDL2-{SDL2_VERSION}/x86_64-w64-mingw32 /usr
            RUN rm -rf SDL2-devel-{SDL2_VERSION}-mingw.zip SDL2-{SDL2_VERSION}

            WORKDIR /app
        """)
    elif OS == "2":
        return textwrap.dedent(f"""\
            FROM {IMAGE}
            RUN {COMMAND}
            WORKDIR /app
        """)


def build_emscripten(files, OUTPUT, BUILD_DIR, ASSETS_FOLDER):
    if not os.path.exists(os.path.join("emscripten", "index.html")):
        error("index.html was not found in the emscripten folder")

    os.makedirs(BUILD_DIR, exist_ok=True)
    subprocess.run(["copy", os.path.join("emscripten", "index.html"), BUILD_DIR], shell=True, check=True)

    emcc_command = f'em++ {EMSCRIPTEN_COMPILE_ARGS} {files} -o {BUILD_DIR}/{OUTPUT}.js -s USE_SDL=2 --preload-file {ASSETS_FOLDER}'

    print("Compiling...")
    result = subprocess.run(
        f'docker run --rm -v "{os.getcwd()}:/app" -w /app emscripten/emsdk:{EMSCRIPTEN_VERSION} sh -c "{emcc_command}"',
        shell=True)
    if result.returncode == 0:
        print(f"Build successful into folder: {BUILD_DIR}")

        port = 8080
        print(f"Host a temporary local HTTP server on port {port} to test the game in browser?")
        print("y / n (Default = n)")
        choice = input() or "n"

        if choice == "y":
            print(f"Starting local temporary HTTP server on port {port} for 10 seconds")
            os.chdir(BUILD_DIR)
            Handler = http.server.SimpleHTTPRequestHandler
            with socketserver.TCPServer(("", port), Handler) as httpd:
                webbrowser.open("http://127.0.0.1:8080")
                threading.Timer(10, lambda: (print("Shutting down temporary server..."), httpd.shutdown())).start()
                httpd.serve_forever()

        exit(0)
    else:
        error("Build failed")


def main():
    files = " ".join([
        os.path.join("/app", root, file).replace("\\", "/")
        for root, _, files in os.walk("src/")
        for file in files if file.endswith(".cpp")
    ])

    print("Which target you want to build for?")
    print("1. Windows, 2. Linux, 3. Browser")
    print(DEFAULT_TEXT)
    chosen_os = input() or "1"

    if chosen_os == "1":
        print("Building for windows...")
        OUTPUT = "game.exe"
        BUILD_DIR = "build_windows"

        COMPILER = "x86_64-w64-mingw32-g++"
        COMPILE_ARGS = WINDOWS_COMPILE_ARGS
        LINKING_ARGS = WINDOWS_LINKING_ARGS
    elif chosen_os == "2":
        print("Building for linux...")
        OUTPUT = "game"
        BUILD_DIR = "build_linux"

        COMPILER = "g++"
        COMPILE_ARGS = LINUX_COMPILE_ARGS
        LINKING_ARGS = LINUX_LINKING_ARGS
    elif chosen_os == "3":
        OUTPUT = "emscripten"
        BUILD_DIR = "build_emscripten"
        build_emscripten(files, OUTPUT, BUILD_DIR, ASSETS_FOLDER)
    else:
        error("Incorrect target selection")

    print("What you want your Docker image to be?")
    print("1. Alpine, 2. Arch")
    print(DEFAULT_TEXT)
    chosen_docker = input() or "1"

    if chosen_docker == "1":
        IMAGE = f"alpine:{ALPINE_VERSION}"

        COMMAND = "apk update && apk upgrade && apk add --no-cache wget unzip "
        if chosen_os == "1":
            COMMAND += "mingw-w64-gcc"
            DOCKER_NAME = "alpine_builder_windows"
        elif chosen_os == "2":
            COMMAND += "build-base sdl2-dev"
            DOCKER_NAME = "alpine_builder_linux"

    elif chosen_docker == "2":
        IMAGE = "archlinux:latest"

        COMMAND = "pacman-key --init && pacman -Syu --noconfirm && pacman -S --noconfirm wget unzip "
        if chosen_os == "1":
            COMMAND += "mingw-w64-gcc"
            DOCKER_NAME = "arch_builder_windows"
        elif chosen_os == "2":
            COMMAND += "gcc sdl2-compat"
            DOCKER_NAME = "arch_builder_linux"
        COMMAND += " && pacman -Scc"

    # elif chosen_docker == "3":
    #     IMAGE = "debian:stable"

    #     COMMAND = "apt update && apt install -y wget unzip "
    #     if chosen_os == "1":
    #         COMMAND += "g++-mingw-w64-x86-64"
    #         DOCKER_NAME = "debian_builder_windows"
    #     elif chosen_os == "2":
    #         COMMAND += "build-essential libsdl2-dev"
    #         DOCKER_NAME = "debian_builder_linux"

    else:
        error("Incorrect Docker image selection")

    print(f"Creating '{BUILD_DIR}' folder...")
    os.makedirs(BUILD_DIR, exist_ok=True)

    print(f"Copying assets folder '{ASSETS_FOLDER}'...")
    try:
        dest_assets_path = os.path.join(BUILD_DIR, ASSETS_FOLDER)
        if os.path.exists(ASSETS_FOLDER):
            if os.path.exists(dest_assets_path):
                shutil.rmtree(dest_assets_path)  # remove previous
            shutil.copytree(ASSETS_FOLDER, dest_assets_path)
        else:
            print(f"'{ASSETS_FOLDER}' folder was not found, ignoring...")
    except Exception as e:
        error(e)

    # make a custom docker image for compilations
    print("Building Docker image...")
    dockerfile = make_docker_file(IMAGE, COMMAND, SDL2_VERSION, chosen_os)
    docker_build_command = f"docker build -t {DOCKER_NAME} -"
    try:
        build_result = subprocess.run(docker_build_command, input=dockerfile.encode('utf-8'), check=False)
        if build_result.returncode != 0:
            error("Docker build failed")
    except FileNotFoundError:
        error("Docker not installed")
    except Exception as e:
        error(e)

    docker_run_command = f"{COMPILER} {COMPILE_ARGS} {files} -o {BUILD_DIR}/{OUTPUT} {LINKING_ARGS}"

    print("Compiling...")
    if chosen_os == "1":
        docker_run_command += f" && cp /usr/x86_64-w64-mingw32/bin/SDL2.dll {BUILD_DIR}"

    docker_run_full_command = [
        "docker", "run",
        "--rm",
        "-v", f"{os.getcwd()}:/app",
        "-w", "/app",
        DOCKER_NAME,
        "sh", "-c", docker_run_command
    ]
    try:
        compile_result = subprocess.run(docker_run_full_command, check=False)
        if compile_result.returncode == 0:
            print(f"Build successful into folder '{BUILD_DIR}'")
        else:
            error("Build failed.")
    except FileNotFoundError:
        error("Docker not installed")
    except Exception as e:
        error(e)

    if chosen_os == "1" and os.name == "nt" or chosen_os == "2" and os.name == "posix":
        print("Starting the game...")
        subprocess.run(os.path.join(BUILD_DIR, OUTPUT))


if __name__ == "__main__":
    main()
