# Example

This example shows how to use the Daily Core C++ SDK. It joins a room and
mirrors the audio of the first remote participant to join back to the room.

These are the main showcased features:

- Join a room
- Set client user name
- Event handling (participant joins, participant leaves, ...)
- Receive audio from the room
- Send audio to the room
- Update subscriptions and publishing

## Building

The example finds the SDK it's in. To build it with another copy of the SDK,
add `-DDailyCore_ROOT=/path/to/daily-core-sdk` when configuring.

### Linux and macOS

```bash
cmake . -G Ninja -Bbuild -DCMAKE_BUILD_TYPE=Release
ninja -C build
```

### Windows

```bash
cmake . -Bbuild
cmake --build build --config Release
```

### Cross-compiling (Linux aarch64)

Use the example from the `linux-arm64` SDK, or point `DailyCore_ROOT` to it,
and build with:

```bash
cmake . -G Ninja -Bbuild -DCMAKE_TOOLCHAIN_FILE=aarch64-linux-toolchain.cmake -DCMAKE_BUILD_TYPE=Release
ninja -C build
```

## Usage

After building the example you should be able to run it:


```bash
./build/daily_example

| Argument | Description                                                               |
|----------|---------------------------------------------------------------------------|
| -m       | The Daily meeting URL                                                     |
| -t       | Daily meeting token if required by the meeting                            |
| -n       | The name this client should be connected to the meeting. (default: Guest) |
```

For example:

```bash
./build/daily_example -m ROOM_URL
```

Now, join the room URL from your browser (this will load Daily Prebuilt). You
should be able to hear yourself when you speak because of the mirroring.
