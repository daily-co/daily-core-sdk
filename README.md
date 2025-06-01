# Daily Core C++ SDK

`daily-core-sdk` is a C++ SDK to build native client applications for the
[Daily](https://daily.co) platform.

It supports Linux (`x86_64` and `aarch64`), macOS (`x86_64` and `aarch64`) and
Windows (`x86_64`).

## 🧪 Code examples

Take a look at the [examples](./examples) directory.

## 🧰 Requirements

On all platforms:

- cmake 3.16 or older.

On Windows you also need vcpkg:

- [vcpkg](https://learn.microsoft.com/en-us/vcpkg/get_started/overview)

## 🚀 First steps

Before building the example we need to declare a few environment variables:

```bash
DAILY_CORE_PATH=/path/to/daily-core
```

Now, make a copy of the `cmake` folder into your project.

On Windows also copy [CMakePresets.json](./CMakePresets.json) to your project.

## 🛠️ Preparing your CMakeList.txt

We need to make sure the `DAILY_CORE_PATH` environment variable is defined:

```
#
# DAILY_CORE_PATH environment variable should be defined.
#
if (NOT DEFINED ENV{DAILY_CORE_PATH})
  message(FATAL_ERROR "You must define DAILY_CORE_PATH environment variable.")
endif()

set(DAILY_CORE_PATH "$ENV{DAILY_CORE_PATH}")
```

And then you can add `find_package()` to find the library:

```
find_package(DailyCore)
```

This will expose `DAILY_CORE_INCLUDE_DIRS` and `DAILY_CORE_LIBRARIES` which you
can then use in your target application:

```
target_include_directories(daily_example
  PRIVATE
  ${DAILY_CORE_INCLUDE_DIRS}
)

target_link_libraries(daily_example
  PRIVATE
  ${DAILY_CORE_LIBRARIES}
)
```

## 🐧🍏 Linux and macOS

```bash
cmake . -G Ninja -Bbuild -DCMAKE_BUILD_TYPE=Release
ninja -C build
```

## 🪟 Windows

Initialize the command-line development environment.

```bash
"C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvarsall.bat" amd64
```

And then configure and build:

```bash
cmake . -Bbuild --preset vcpkg
cmake --build build --config Release
```

## 🔀🐧Cross-compiling (Linux aarch64)

It is possible to build the example for the `aarch64` architecture in Linux with:

```bash
cmake . -Bbuild -DCMAKE_TOOLCHAIN_FILE=aarch64-linux-toolchain.cmake -DCMAKE_BUILD_TYPE=Release
ninja -C build
```
