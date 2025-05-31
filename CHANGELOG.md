# Changelog

All notable changes to **daily-core-sdk** will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

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
