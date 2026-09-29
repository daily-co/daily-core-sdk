# Daily Core C++ SDK

`daily-core-sdk` is a C++ SDK to build native client applications for the
[Daily](https://daily.co) platform.

It supports Linux (`x86_64` and `aarch64`), macOS (`x86_64` and `aarch64`) and
Windows (`x86_64`).

## 🧰 Requirements

- A C++ compiler: GCC, Clang, Apple Clang or MSVC. The library includes C++
  code, so link your app as C++.
- CMake 3.16 or newer, if you build with CMake.
- On Linux, glibc 2.28 or newer.

## 📦 What's in the SDK

Download the SDK for your platform from the
[releases](https://github.com/daily-co/daily-core-sdk/releases) and unpack it.
It has:

- `include/daily_core.h`: the C API.
- `include/daily_core_version.h`: the SDK version, e.g. `DAILY_CORE_VERSION`
  (`"0.22.0"`).
- `lib/`: the static library, `libdaily_core.a` (`Release/daily_core.lib` and
  `Debug/daily_cored.lib` on Windows).
- `cmake/`: the CMake package.
- `examples/`: example apps.

## 🛠️ Using the SDK with CMake

Find the package and link to `DailyCore::DailyCore`:

```cmake
find_package(DailyCore 0.22 REQUIRED)
target_link_libraries(my_app PRIVATE DailyCore::DailyCore)
```

Then point `CMAKE_PREFIX_PATH` (or `DailyCore_ROOT`) to the SDK when you
configure your project:

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/daily-core-sdk
```

`DailyCore::DailyCore` brings the headers, the library and the system
libraries it needs on each platform. With MSVC, it also sets
`_ITERATOR_DEBUG_LEVEL=0`, like the library is built with.

The version in `find_package()` is the oldest SDK you need. Newer SDKs with the
same major version are accepted too. You can also check the version in your
code:

```c
#include <daily_core_version.h>

printf("Daily Core %s\n", DAILY_CORE_VERSION);
```

### Projects using `FindDailyCore.cmake`

Projects that copied `cmake/FindDailyCore.cmake` and set `DAILY_CORE_PATH`
keep working. To switch to the package:

1. Remove your copy of `FindDailyCore.cmake` and the `DAILY_CORE_PATH` check.
2. Link to `DailyCore::DailyCore` instead of using `DAILY_CORE_INCLUDE_DIRS`
   and `DAILY_CORE_LIBRARIES`.
3. Remove the system libraries and frameworks you added for Daily Core.

## 🔧 Other build systems

Add `include/` to your include path, and link the library with these system
libraries:

- **Linux:** `-lpthread -ldl -lm`.
- **macOS:** `-ObjC` and the `AppKit`, `AudioToolbox`, `AVFoundation`,
  `CoreAudio`, `CoreGraphics`, `CoreMedia`, `CoreVideo`, `Foundation`,
  `IOSurface`, `Metal`, `MetalKit`, `OpenGL`, `QuartzCore`, `ScreenCaptureKit`,
  `Security` and `VideoToolbox` frameworks.
- **Windows:** `bcrypt`, `crypt32`, `d3d11`, `dmoguids`, `dwmapi`, `dxgi`,
  `gdi32`, `iphlpapi`, `msdmo`, `ncrypt`, `ntdll`, `ole32`, `secur32`,
  `shcore`, `strmiids`, `userenv`, `winmm`, `wmcodecdspuuid` and `ws2_32`.
  Also compile with `_ITERATOR_DEBUG_LEVEL=0`.

For example, on Linux:

```bash
g++ -std=c++17 main.cpp -I/path/to/daily-core-sdk/include \
  /path/to/daily-core-sdk/lib/libdaily_core.a -lpthread -ldl -lm
```

## 🧪 Examples

- [c++](./examples/c++): joins a room and sends the audio of the first
  participant who joins back to the room.
- [c++-custom-tracks](./examples/c++-custom-tracks): sends and receives custom
  audio tracks.

The examples find the SDK they're in, so you can build them right away:

```bash
cd examples/c++
cmake . -G Ninja -Bbuild -DCMAKE_BUILD_TYPE=Release
ninja -C build
```

See each example's README for Windows, cross-compiling and how to run it.
