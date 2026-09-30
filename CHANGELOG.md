# Changelog

All notable changes to **daily-core-sdk** will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.23.0] - 2026-09-30

### Added

- Added `daily_core_string_free()` to free the strings returned by
  `daily_core_*` functions, such as `daily_core_call_client_participants()` or
  `daily_core_context_custom_audio_track_id()`. Before, there was no way to free
  them, so they leaked.

- Added support to automatically start transcription when joining a room if the
  `auto_start_transcription` meeting token property is set, using the room's
  `auto_transcription_settings`.

- `DailyStartTranscriptionProperties` now accepts `participants` and
  `instanceId`, matching the REST start-transcription schema, so
  `auto_transcription_settings` that scope transcription to specific
  participants or a named instance are honored.

- Added candidate-pair round-trip time to call metrics, so latency is
  measured consistently, including for receive-only participants.

- Added `include/daily_core_version.h`, with the SDK version:
  `DAILY_CORE_VERSION` (e.g. `"0.23.0"`), and `DAILY_CORE_VERSION_MAJOR`,
  `DAILY_CORE_VERSION_MINOR` and `DAILY_CORE_VERSION_PATCH`.

- Added a CMake package. Point `CMAKE_PREFIX_PATH` or `DailyCore_ROOT` to the
  SDK, and then use `find_package(DailyCore 0.23 REQUIRED)` and link to
  `DailyCore::DailyCore`, the shared library. It brings the headers, and you no
  longer need to list system libraries yourself.

- Added a shared library: `lib/libdaily_core.so` on Linux,
  `lib/libdaily_core.dylib` on macOS, and `bin/daily_core.dll` (with
  `lib/daily_core.dll.lib`) on Windows. It only exports the C API and includes
  everything else it needs, so apps link nothing else, can use any compiler and
  C++ standard library (e.g. Unreal Engine's libc++ on Linux), and on Windows
  any runtime library and Debug settings. Its dependencies (libwebrtc,
  BoringSSL, ...) stay inside it, so they no longer clash with the ones apps
  use, e.g. their own OpenSSL. On Windows it includes the C runtime (`/MT`), so
  it doesn't need the Visual C++ Redistributable.

### Changed

- Upgraded libwebrtc to `branch-heads/8010` (M153). Its vendored media and
  crypto dependencies (ffmpeg, libvpx and BoringSSL among them) are
  substantially newer.

- On Apple platforms, H.265 calls interoperate with current browsers again.
  H.265 support now comes from libwebrtc itself, matching the H.265 RTP
  behaviour current browsers expect; the implementation we previously
  back-ported predated it.

- **Breaking:** the minimum supported macOS version is now 13.0 (previously
  10.15). It follows the deployment target libwebrtc 8010 is built against.

- `cmake/FindDailyCore.cmake` now uses the CMake package. It still sets
  `DAILY_CORE_INCLUDE_DIRS` and `DAILY_CORE_LIBRARIES`, which is now the shared
  library, and `DAILY_CORE_PATH` is no longer required.

- The examples use the CMake package and find the SDK they're in, so they no
  longer need `DAILY_CORE_PATH` or their own copy of `FindDailyCore.cmake`.

### Removed

- **Breaking:** removed the static library (`lib/libdaily_core.a`, and
  `lib/Release/daily_core.lib` and `lib/Debug/daily_cored.lib` on Windows).
  Link the shared library instead (see the README). The static library exposed
  all the symbols of its dependencies, which clashed with apps that use their
  own (e.g. OpenSSL), and required apps to link the system libraries it needed.

- Removed `CMakePresets.json`, and the ones in the examples. They set up vcpkg,
  which isn't needed, and required Visual Studio 2019. On Windows, configure
  with `cmake -S . -B build` instead.

### Fixed

- Fixed `daily_core_context_custom_audio_track_id()` and
  `daily_core_context_custom_video_track_id()` returning a string that had
  already been freed, so the track id could not be read. They also released a
  reference to the track that belonged to the caller, so calling them twice
  freed the track while it was still in use.

- Fixed a use-after-free when a call client was destroyed without `leave()`
  ever being called: the mediasoup transports were never torn down, so the peer
  connection and microphone outlived the client and went on to use WebRTC
  threads that the context had already deleted. The transports are now torn
  down explicitly on destroy, and the transport listeners no longer hold a
  reference that kept the transport alive.

- Fixed a deadlock in the logging layer that could freeze the whole process
  when a call client was being destroyed while a log line arrived for a call
  that had already gone away.

- Fixed a native memory leak that grew with the length of a call: the
  transport, producer and consumer stats, ids and app data read from
  mediasoupclient were copied but never freed. Call metrics read them every
  2 seconds, leaking about 0.7 MB per minute with one producer and one
  consumer, and more with more tracks.

- Fixed an issue where `videoBitrate` and `audioBitrate` in a meeting token's
  `start_cloud_recording_opts` were ignored, causing recordings started from
  the token to use the default bitrates.

## [0.22.0] - 2026-08-19

### Fixed

- Fixed an issue where SIP transfer server error messages were not handled
  properly.

- Fixed multiple memory, thread and file descriptor leaks when creating and
  releasing call clients'.

## [0.21.0] - 2026-08-03

### Added

- Added automatic fallback to `dailywebrtc.com` and `dailywebrtc.net` when
  `daily.co` authoritative nameservers are unreachable, improving connection
  resilience.

- Added support for `screenAudio` input. It is now possible to send a custom
  screen-share audio track (alongside or independently of `screenVideo`) with:

```
{
  "inputs": {
      "screenAudio": {
          "isEnabled": True,
          "settings": {
             "customTrack": {
                 "id": "CUSTOM_TRACK_ID"
             }
          }
      }
  }
}
```

- Added support for `screenVideo` custom tracks. It is now possible to send a
  custom screen share track with:

```
{
  "inputs": {
      "screenVideo": {
          "isEnabled": True,
          "settings": {
             "customTrack": {
                 "id": "CUSTOM_TRACK_ID"
             }
          }
      }
  }
}
```

- Added debug logs to HTTP requests.

- Added support for `dtmf-event`.

- Added `provider` field to dialout properties. Currently only supports `"daily"`.

- Added `extension` and `waitBeforeExtensionDialSec` fields to dialout properties.

- Added optional `method` field to send DTMF properties. Supported values are
  `"sip-info"`, `"telephone-event"`, and `"auto"`.

- Added optional `digitDurationMs` field to send DTMF properties.

- It is now possible to create custom video tracks with
  `daily_core_context_create_custom_video_track()`. The custom video tracks need
  a custom video source which can be created with
  `daily_core_context_create_custom_video_source()`. The track can then be added
  to the call client with `daily_core_call_client_add_custom_video_track()`. You
  can also update an existing custom track with a new video source with
  `daily_core_call_client_update_custom_video_track()` or remove and existing
  custom track with `daily_core_call_client_remove_custom_video_track`. Write
  frames with `daily_core_context_custom_video_source_write_frame()`.

- Added a `trackType` field to the `transcription-message` event to indicate
  which track the transcription originated from.

### Changed

- Adaptive Bitrate (ABR) is now enabled by default for camera tracks. It can
  still be disabled by setting `allow_adaptive_layers` to `False` in the camera
  `send_settings`.

- Increased start transcription and start dialout timeouts to 20 seconds.

- `daily_core_call_client_start_dialout()` now returns two parameters when
  completing: session ID and error. Previously, it only returned an error.

- `daily_core_call_client_start_recording()` now returns two parameters when
  completing: stream ID and error. Previously, it only return an error.

### Fixed

- Fixed a use-after-free that could cause a segmentation fault in the audio
  playout thread (e.g. `daily-speaker-p`) if a `DailyRawCallClient` was released
  while still in a call (i.e. without leaving first) and participant audio
  renderers were registered.

- Fixed a segmentation fault that could occur when a `DailyRawCallClient` was
  released while still in a call, especially right before the application
  exited: WebRTC internals are no longer torn down at process exit while SDK
  threads may still be running.

  Note that the recommended way to terminate a call is still to
  `daily_core_call_client_leave()` (and wait for its completion) before calling
  `release()`.

- Fixed an issue where a room configured with `enable_recording` set to
  `cloud-audio-only` was ignored when starting a recording, causing the
  recording to also capture video.

- Fixed an issue where `daily_core_call_client_start_recording()` null
  properties were serialized to `null`.

- Fixed a segmentation fault that could occur after virtual microphone or
  speaker inputs or publishing updates.

- Fixed panics in signalling reconnect paths when room lookup returns no worker.

- Fixed support for cloud-audio-only recording type.

- Avoid unnecessary signalling reconnection attempts when
  `daily_core_call_client_leave()` is called during a network failure.

- Fixed an issue that could cause a crash when leaving a room if a video or
  audio renderer was active.

- Fixed an issue where start recording, start transcription and start dialout
  server error messages were ignored.

### Performance

- Added per-phase connection timing metrics for WebSocket signalling (DNS lookup,
  TCP connect, TLS handshake, and WebSocket upgrade), complementing the existing
  HTTP connection timings.

- Replaced `ureq` HTTP client with `hyper` + `rustls` (ring backend). Includes
  connection timeouts, TLS configuration reuse, and per-request timing metrics
  for DNS lookup, TCP connect, and TLS handshake.

## [0.20.0] - 2025-10-17

### Added

- Added `callerId` field to `daily_core_call_client_sip_call_transfer()`
  properties.

- Added `daily_core_context_create_custom_audio_source_with_silence(sample_rate,
  channels)` to create a custom audio source with a predefined sample rate and
  number of channels that sends silence audio when no audio frames are being
  written to the audio source.

- Added `daily_core_set_log_level(log_level)`. The available log levels are:
  `DailyLogLevel_Off`, `DailyLogLevel_Error`, `DailyLogLevel_Warn`,
  `DailyLogLevel_Info`, `DailyLogLevel_Debug`, `DailyLogLevel_Trace`.

### Fixed

- Fixed an issue that could cause a segmentation fault when switching or
  stopping the network while using custom audio sources.

- Fixed an issue where custom audio tracks won't send silence from the very
  beginning. This is necessary to avoid issues with browsers that expect RTP
  packets from the start.

- Fixed an issue where the application could keep attempting to send messages
  after the signaling channel was closed preventing the application to finish.

- Fixed a race condition that would cause audio to never be processed when
  received.

### Other

- Link examples with `crypt32` and `ncrypt` libraries.

## [0.19.3] - 2025-06-17

### Changed

- ⚠️ Added argument `ignore_audio_level` to
  `daily_core_call_client_add_custom_audio_track()` and
  `daily_core_call_client_update_custom_audio_track()` to be able to ignore
  custom tracks' audio level.

## [0.19.2] - 2025-06-09

### Fixed

- Fixed an issue where an audio source could cause a segmentation fault on
  application exit if audio was still being written.

## [0.19.1] - 2025-05-28

### Fixed

- Fixed an audio renderer issue that would cause garbled audio if remote audio
  was stereo.

- Handle meeting session ID changes which occur once the session has started.


## [0.19.1] - 2025-05-28

### Fixed

- Fixed an audio renderer issue that would cause garbled audio if remote audio
  was stereo.

- Handle meeting session ID changes which occur once the session has started.


## [0.19.0] - 2025-05-23

### Added

- It is now possible to create an audio track with
  `daily_core_context_create_custom_audio_track()` and assign it as your track
  microphone.

- Added support for `daily_core_call_client_start_dialout()` new fields:
  `displayName`, `userId`, `video`, `codecs`, and `permissions`.

### Changed

- ⚠️ `daily_core_call_client_set_participant_audio_renderer()` has a new argument
  to specify the desired sample rate.

- System certificates are now loaded on macOS, Linux and Windows platforms
  instead of the embedded Mozilla's root certificates.

### Fixed

- Fixed an issue that would cause a blocking virtual microphone to not send any
  audio in some cases.

## [0.18.1] - 2025-05-02

### Fixed

- Fixed an issue that would cause virtual microphones to not call the completion
  callbacks in some situations.

- Retrying multiple times to open the signalling channel during the join.

- Alternate websocket URI format to avoid connection issues potentially caused by ISPs.

## [0.18.0] - 2025-05-02

### Added

- It is now possible to create custom audio tracks with
  `daily_core_context_create_custom_audio_track()`. The custom audio tracks need
  a custom audio source which can be created with
  `daily_core_context_create_custom_audio_source()`. The track can then be added
  to the call client with `daily_core_call_client_add_custom_audio_track()`. You
  can also update an existing custom track with a new audio source with
  `daily_core_call_client_update_custom_audio_track()` or remove and existing
  custom track with `daily_core_call_client_remove_custom_audio_track`.

- Support the new `canReceive` permission, which involves:
  - Showing the proper track state (i.e. `"off"`, with `"receivePermission"`
    among the `"offReasons"`) when `canReceive` permission is lacking for a
    remote track.
  - Resuming receiving remote tracks when previously-revoked `canReceive`
    permissions have been restored.

### Changed

- Error messages "no subscription for consumer" and "Failed to close consumer"
  are now logged as debug messages since they can be caused by a remote
  participant simply disconnecting which is a valid and common case.

## [0.14.2] - 2025-01-14

### Added

- Initial x86_64 macos support.

## [0.11.0] - 2024-10-09

### Added

- Initial Windows support.

## [0.10.1] - 2024-08-27

### Added

- Initial release.
