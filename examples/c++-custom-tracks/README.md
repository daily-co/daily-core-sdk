# Custom tracks example

This example has two apps:

- `daily_sender` sends a sine wave in a custom audio track named `cxx-wave`.
- `daily_receiver` receives `cxx-wave` from the first participant who joins,
  and sends it back in a custom audio track named `cxx-wave-mirror`.

They show how to:

- Join a room with a user name.
- Handle events, such as participants joining and leaving.
- Send and receive custom tracks.
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

Run the sender first, then the receiver in another terminal:

```bash
./build/daily_sender -m ROOM_URL
./build/daily_receiver -m ROOM_URL
```

On Windows, the apps are in `build\Release\`.

Both apps take the same options:

| Option | Description                                                   |
|--------|---------------------------------------------------------------|
| `-m`   | The room URL.                                                 |
| `-t`   | A meeting token, if the room needs one.                       |
| `-n`   | The user name to join with (default: `Sender` or `Receiver`). |

Then open `index.html` in your browser, join the same room, and pick the custom
track you want to hear. Join from the browser last: the receiver mirrors the
first participant who joins, which has to be the sender.
