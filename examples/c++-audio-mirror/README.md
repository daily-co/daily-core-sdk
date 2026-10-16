# Audio mirror example

This example joins a room and sends the audio of the first participant who
joins back to the room, so they hear themselves.

It shows how to:

- Join a room with a user name.
- Handle events, such as participants joining and leaving.
- Receive and send audio.
- Update subscriptions and publishing.

## Building

The example uses the SDK it's in. To use another copy of the SDK, add
`-DDailyCore_ROOT=/path/to/daily-core-sdk` when you configure.

On Linux and macOS:

```bash
cmake . -G Ninja -Bbuild -DCMAKE_BUILD_TYPE=Release
ninja -C build
```

On Windows:

```bash
cmake . -Bbuild
cmake --build build --config Release
```

To cross-compile for Linux aarch64, use the `linux-arm64` SDK and:

```bash
cmake . -G Ninja -Bbuild -DCMAKE_TOOLCHAIN_FILE=aarch64-linux-toolchain.cmake -DCMAKE_BUILD_TYPE=Release
ninja -C build
```

## Running

```bash
./build/daily_example -m ROOM_URL
```

On Windows, the app is `build\Release\daily_example.exe`.

| Option | Description                                  |
|--------|----------------------------------------------|
| `-m`   | The room URL.                                |
| `-t`   | A meeting token, if the room needs one.      |
| `-n`   | The user name to join with (default: Guest). |

Then join the same room from your browser. You should hear yourself when you
speak.
