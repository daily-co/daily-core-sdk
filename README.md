# Daily Core C++ SDK

`daily-core-sdk` is a C++ SDK to build native client applications for the
[Daily](https://daily.co) platform.

It supports Linux (`x86_64` and `aarch64`), macOS (`x86_64` and `aarch64`) and
Windows (`x86_64`).

## 🧰 Requirements

- A C or C++ compiler: GCC, Clang, Apple Clang or MSVC.
- CMake 3.16 or newer, if you build with CMake.
- On Linux, glibc 2.28 or newer.

## 📦 What's in the SDK

Download the SDK for your platform from the
[releases](https://github.com/daily-co/daily-core-sdk/releases) and unpack it.
It has:

- `include/daily_core.h`: the C API.
- `include/daily_core_version.h`: the SDK version, e.g. `DAILY_CORE_VERSION`
  (`"0.23.0"`).
- The shared library: `lib/libdaily_core.so` on Linux, `lib/libdaily_core.dylib`
  on macOS, and `bin/daily_core.dll` on Windows, which you link with
  `lib/daily_core.dll.lib`.
- The static library: `lib/libdaily_core.a` (`lib/Release/daily_core.lib` and
  `lib/Debug/daily_cored.lib` on Windows).
- `cmake/`: the CMake package.
- `shared-library/`: builds the shared library from the static one.
- `examples/`: example apps.

## ⚖️ Shared or static library?

Use the shared library, unless you need your app to be a single binary. It only
exports the C API and includes everything else it needs, so:

- You don't link anything else, like system libraries or frameworks.
- It works with any compiler and C++ standard library, e.g. Unreal Engine's
  libc++ on Linux, and from C.
- On Windows, your app can use any runtime library and Debug settings.

You ship it with your app (see below).

The static library is linked into your app, but you also link the system
libraries it needs, and link your app as C++. On Windows, your app must also
be compiled with `_ITERATOR_DEBUG_LEVEL=0`.

## 🛠️ Using the SDK with CMake

Find the package and link to `DailyCore::DailyCore`, the shared library:

```cmake
find_package(DailyCore 0.23 REQUIRED)
target_link_libraries(my_app PRIVATE DailyCore::DailyCore)
```

Then point `CMAKE_PREFIX_PATH` (or `DailyCore_ROOT`) to the SDK when you
configure your project:

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/daily-core-sdk
```

To link the static library instead, use `DailyCore::DailyCoreStatic`. It brings
the system libraries it needs on each platform and, with MSVC,
`_ITERATOR_DEBUG_LEVEL=0`.

The version in `find_package()` is the oldest SDK you need. Newer SDKs with the
same major version are accepted too. You can also check the version in your
code:

```c
#include <daily_core_version.h>

printf("Daily Core %s\n", DAILY_CORE_VERSION);
```

### Shipping the shared library

- **Linux and macOS:** CMake adds the SDK's `lib/` to your app's rpath, so it
  runs from your build folder. When you install or package your app, ship the
  library with it and set the rpath, e.g. to `$ORIGIN/../lib` on Linux or
  `@executable_path/../Frameworks` on macOS.
- **Windows:** put `daily_core.dll` next to your app's `.exe`. For example,
  copy it there after building:

  ```cmake
  add_custom_command(TARGET my_app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      $<TARGET_FILE:DailyCore::DailyCore> $<TARGET_FILE_DIR:my_app>
  )
  ```

### Migrating from `FindDailyCore.cmake`

If your project copied `cmake/FindDailyCore.cmake` and sets `DAILY_CORE_PATH`,
it keeps working, with the static library. To use the package instead:

1. Remove your copy of `FindDailyCore.cmake` and the `DAILY_CORE_PATH` check.
2. Link to `DailyCore::DailyCore` (or `DailyCore::DailyCoreStatic`) instead of
   using `DAILY_CORE_INCLUDE_DIRS` and `DAILY_CORE_LIBRARIES`.
3. Remove the system libraries, frameworks and `_ITERATOR_DEBUG_LEVEL` you
   added for Daily Core.

## 🔧 Other build systems

Add `include/` to your include path, and link the shared library:
`libdaily_core.so` or `libdaily_core.dylib` with an rpath to find it, or
`daily_core.dll.lib` on Windows. For example, on Linux:

```bash
gcc main.c -I/path/to/daily-core-sdk/include \
  -L/path/to/daily-core-sdk/lib -ldaily_core \
  -Wl,-rpath,/path/to/daily-core-sdk/lib
```

To link the static library, also link these system libraries:

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

## 🏗️ Building the shared library

The SDK comes with the shared library. To build it from the static library
yourself, e.g. from your own build of Daily Core:

```bash
cmake -S shared-library -B build/shared-library -DCMAKE_BUILD_TYPE=Release
cmake --build build/shared-library --config Release
cmake --install build/shared-library --config Release --prefix .
```

It uses the static library in `lib/`. Set `DAILY_CORE_STATIC_LIBRARY` to use
another one.

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
