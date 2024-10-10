# Example

Before building the example we need to declare a few environment variables:

```bash
DAILY_CORE_PATH=/path/to/daily-core
```

## Linux and macOS

```bash
cmake . -G Ninja -Bbuild -DCMAKE_BUILD_TYPE=Release
ninja -C build
```

## Windows

Initialize the command-line development environment.

```bash
"C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvarsall.bat" amd64
```

And then configure and build:

```bash
cmake . -Bbuild --preset vcpkg
cmake --build build --config Release
```

## Cross-compiling (Linux aarch64)

It is possible to build the example for the `aarch64` architecture in Linux with:

```bash
cmake . -Bbuild -DCMAKE_TOOLCHAIN_FILE=aarch64-linux-toolchain.cmake -DCMAKE_BUILD_TYPE=Release
ninja -C build
```

## Usage

After building the example you should be able to run it:


```bash
./build/daily_example
```

| Argument | Description                                                               |
|----------|---------------------------------------------------------------------------|
| -m       | The Daily meeting URL                                                     |
| -t       | Daily meeting token if required by the meeting                            |
| -n       | The name this client should be connected to the meeting. (default: Guest) |
