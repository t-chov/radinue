# Radinue

[日本語](README-ja.md)

Radinue is a small desktop audio player for Windows and macOS, designed for
listening to radio recordings stored in a directory. It remembers the current
track and playback position so that you can resume exactly where you stopped.

> [!NOTE]
> Radinue is in the early stages of development. Playback controls are available
> in development builds, but there is no stable player release yet.

## Current status

- Select a directory and display its directly contained audio files
- Filter files through an explicit, case-insensitive extension allowlist
- Ignore subdirectories, symbolic links, state files, and unsupported files
- Sort filenames deterministically in ascending or descending order
- Play or pause, seek by 10 seconds, and move between tracks with icon buttons
- Automatically advance at the end of a track without wrapping at the playlist end
- Skip malformed or unsupported tracks during active playback, stopping safely at
  the playlist end; a failed track selected while paused remains selected
- View and change the playback position with a seek bar
- Change playback speed from 50% to 200% in exact 10% steps, with pitch correction
- Adjust volume from 0% to 200%; values above 100% use software amplification
- Atomically save the current track and position to `.radinue-state.json` in the
  playlist directory and restore them when that directory is reopened

Supported extensions are `.aac`, `.flac`, `.m4a`, `.mka`, `.mp3`, `.ogg`,
`.opus`, `.wav`, and `.wma`.

Radinue deliberately focuses on reliable local playback. It does not aim to
provide library management, streaming, podcast feeds, shuffle, repeat,
equalizers, cloud sync, telemetry, or automatic updates.

## Keyboard shortcuts

Shortcuts are active only while Radinue has focus.

| Key | Action |
| --- | --- |
| `z` | Seek backward 10 seconds |
| `x` | Seek forward 10 seconds |
| `s` | Decrease speed by 10% |
| `d` | Increase speed by 10% |
| `g` | Reset speed to 100% |

## Technology

Radinue is planned around a deliberately small native stack:

- C++20
- Qt 6 Widgets
- libmpv
- CMake

## Building

### Requirements

- CMake 3.25 or newer
- A C++20 compiler (Apple Clang on macOS or MSVC on Windows)
- Qt 6.8 or newer with the Widgets and Test components
- libmpv development headers and libraries
- Ninja for the developer presets

On macOS, the required packages can be installed with Homebrew:

```sh
brew install cmake ninja qt mpv
cmake --preset debug -DCMAKE_PREFIX_PATH="$(brew --prefix qt)"
cmake --build --preset debug
ctest --preset debug
```

Open the built application and choose a directory in the window:

```sh
open build/debug/src/radinue.app
```

A directory may also be supplied when launching a development build:

```sh
open build/debug/src/radinue.app --args "/path/to/recordings"
```

On Windows, install Visual Studio 2022 with the MSVC x64 tools and Qt 6.8.3 for
MSVC x64, then run the following commands from PowerShell. The setup script
downloads a pinned libmpv SDK and verifies its SHA-256 digest.

```powershell
cmake -DOUTPUT_DIR="$PWD/.deps/mpv" -P cmake/DownloadMpvWindows.cmake
./scripts/CreateMpvImportLibrary.ps1 -MpvRoot "$PWD/.deps/mpv"
cmake --preset ci-windows
cmake --build --preset ci-windows --parallel
ctest --preset ci-windows
```

If Qt is not on the default CMake search path, set `CMAKE_PREFIX_PATH` in a
local `CMakeUserPresets.json` or pass it on the configure command line. This
file is ignored by Git so local paths are never committed.

The intended supported platforms are:

- Windows, built natively with MSVC
- macOS, built natively with Apple Clang

GitHub Actions builds the application and runs the tests on both platforms for
every pull request and every push to `main`.

Pushing a Git tag builds release packages for Windows amd64 and macOS Apple
Silicon and publishes them to the tag's GitHub Release. The macOS package is
currently ad-hoc signed because signing and notarization credentials are not
configured in the repository.

## Contributing

Radinue prioritizes stability, predictable behavior, and a small maintenance
surface. Before proposing a change, please read [AGENTS.md](AGENTS.md) for the
project requirements, architecture, and testing expectations.

## License

Radinue is free software licensed under the
[GNU General Public License v3.0 or later](LICENSE).

See [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md) for dependency licensing
and provenance information.
