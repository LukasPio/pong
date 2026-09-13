# Pong

[Português (Brasil)](README.md) | [English](README.en.md)

A single-player Pong game written in C with SDL2. Face the computer in fullscreen: return the ball with your paddle and get it past your opponent. The ball speeds up every two paddle hits. Missing a ball ends the match; you can immediately start another one.

## Download and play

| Platform | Download | Launch |
| --- | --- | --- |
| Windows 11, 64-bit (Intel/AMD) | [Pong — Windows](https://github.com/LukasPio/pong/releases/tag/v1.0.0-windows) | Download `pong-windows-x86_64.exe` and double-click it. |
| Linux, 64-bit (Intel/AMD) | [Pong — Linux](https://github.com/LukasPio/pong/releases/tag/v1.0.0-linux) | Download `pong-linux-x86_64`, mark it executable, and run it. |

Each download is a single executable with SDL2, SDL2_ttf, FreeType, the font, and license notices embedded. You do not need to install these libraries, copy DLLs, download the `font` directory, or compile the project. The game works offline and launches from any directory.

On Linux:

```sh
chmod +x pong-linux-x86_64
./pong-linux-x86_64
```

The Linux release is built on Ubuntu 22.04 and requires glibc 2.35 or newer and an X11 desktop or Wayland with XWayland. Your system provides the graphics libraries and display drivers. Distributions based on musl, such as Alpine, are not compatible with this binary. The Windows executable only uses Windows system DLLs and does not require the Visual C++ Redistributable.

On the release page, select the executable under **Assets**. The automatic **Source code** downloads are for development. `SHA256SUMS.txt` lets you verify the download; `THIRD-PARTY-NOTICES.txt` contains a copy of the licenses already embedded in the executable.

## Controls

| Key | Action |
| --- | --- |
| `W` or `↑` | Move your paddle up |
| `S` or `↓` | Move your paddle down |
| `R` | Restart after winning or losing |
| `Esc` or `-` | Quit |

The game opens directly in fullscreen, preserving the aspect ratio of its 1280 × 720 playfield. You control the left paddle; the computer controls the right paddle. In-game messages are in English.

## Build from source

Requirements: CMake 3.22+, a C compiler, and internet access during the first configuration. CMake downloads pinned dependency versions and verifies their SHA-256 hashes. The redistributable DejaVu Sans Bold font is included in the repository; builds do not use the original Arial font.

On Ubuntu/Debian:

```sh
sudo apt-get update
sudo apt-get install build-essential cmake ninja-build pkg-config \
  libx11-dev libxext-dev libxcursor-dev libxi-dev libxfixes-dev \
  libxrandr-dev libxss-dev libgl1-mesa-dev
cmake -S . -B build/linux -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build/linux --parallel
./build/linux/pong
```

To cross-compile the Windows `.exe` from Linux, also install `gcc-mingw-w64-x86-64`:

```sh
cmake -S . -B build/windows -G Ninja -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_TOOLCHAIN_FILE=cmake/mingw-w64.cmake
cmake --build build/windows --parallel
```

Output: `build/windows/pong.exe`. The `make`, `make test`, and `make windows` shortcuts are also available.

## Validation and releases

```sh
ctest --test-dir build/linux --output-on-failure
```

The `--smoke-test` checks initialization, the embedded font, both result screens, movement, restart, and quit. The [Release](.github/workflows/release.yml) workflow also runs the Linux binary outside the project on a virtual display, audits dependencies for both binaries, and executes the `.exe` on a Windows runner. This does not replace manual testing on every distribution or on a Windows 11 PC.

To publish a new version, update the version in `CMakeLists.txt` and the README download links, push the changes, then select **Actions → Release → Run workflow** and enter a version such as `v1.0.1`. After testing, the workflow publishes two releases, tagged `v1.0.1-linux` and `v1.0.1-windows`, pointing to the same commit. Existing tags are not overwritten.

## License

The project code uses the [MIT license](LICENSE). SDL2 and SDL2_ttf use the zlib license; FreeType uses the FreeType Project License; DejaVu uses the [Bitstream Vera license, with DejaVu changes in the public domain](font/LICENSE-DejaVu.txt). Portions of this software are copyright © The FreeType Project (www.freetype.org). All rights reserved.

Full notices are embedded in the executable and included as `THIRD-PARTY-NOTICES.txt` in releases. On Linux, `./pong-linux-x86_64 --licenses` prints them.
