# Example

This example shows how to use the Daily Core C++ SDK to send and receive custom
tracks. There are a couple of applications, `daily_sender` and `daily_receiver`.

The `daily_sender` application only sends a custom track named "cxx-wave" with a
basic since wave.

The `daily_receiver` applications receives the custom track "cxx-wave" and
creates a mirror to another custom track named "cxx-wave-mirror".

These are the main showcased features:

- Join a room
- Set client user name
- Event handling (participant joins, participant leaves, ...)
- Update subscriptions and publishing
- Send a custom track
- Receive a custom track from a participant

## Prerequisites

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

After building the example you should be able to run both `daily_sender` and `daily_receiver`:


```bash
./build/daily_sender

| Argument | Description                                                                |
|----------|----------------------------------------------------------------------------|
| -m       | The Daily meeting URL                                                      |
| -t       | Daily meeting token if required by the meeting                             |
| -n       | The name this client should be connected to the meeting. (default: Sender) |

./build/daily_receiver

| Argument | Description                                                                  |
|----------|------------------------------------------------------------------------------|
| -m       | The Daily meeting URL                                                        |
| -t       | Daily meeting token if required by the meeting                               |
| -n       | The name this client should be connected to the meeting. (default: Receiver) |
```

For example:

```bash
./build/daily_sender -m ROOM_URL
```

and then

```bash
./build/daily_receiver -m ROOM_URL
```

In order to verify that custom tracks are being sent correctly, you can open the
provided `index.html` into your favorite browser and join the Daily room URL you
have used before. You should be able to select the custom audio track you want
to hear.

NOTE: Run `daily_sender` first, then `daily_receiver` and, lastly, join with the
browser application. Make sure the web application is not joined when you run
`daily_receiver`, because `daily_receiver` reads the custom track from the first
participant that joins.
