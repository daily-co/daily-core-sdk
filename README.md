# Daily Core

`daily-core` is a preliminary SDK to build native applications for the
[Daily](https://daily.co) platform with a simple static library and a C header
file.

It supports Linux (`x86_64` and `aarch64`) and macOS (`aarch64`).

An example which uses the CMake build system is also included as part of the
SDK.

# Example

Before building the example we need to declare a few environment variables:

```bash
export DAILY_CORE_PATH=/path/to/daily-core
```

After that, it should be as simple as running:

```bash
cmake . -Bbuild
```

You can also specify if you want `Debug` (default) or `Release`:

```bash
cmake . -Bbuild -DCMAKE_BUILD_TYPE=Release
```

then, just compile:

```bash
make -C build
```

## Cross-compiling (Linux aarch64)

It is possible to build the example for the `aarch64` architecture in Linux with:

```bash
cmake . -Bbuild -DCMAKE_TOOLCHAIN_FILE=aarch64-linux-toolchain.cmake
```

or in release mode:

```bash
cmake . -Bbuild -DCMAKE_TOOLCHAIN_FILE=aarch64-linux-toolchain.cmake -DCMAKE_BUILD_TYPE=Release
```

And finally, as before:

```bash
make -C build
```

## Usage

After building the example you should be able to run it:


```bash
./build/daily-example
```

| Argument | Description                                                               |
|----------|---------------------------------------------------------------------------|
| -m       | The Daily meeting URL                                                     |
| -t       | Daily meeting token if required by the meeting                            |
| -n       | The name this client should be connected to the meeting. (default: Guest) |
